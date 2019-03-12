/** @file
  This driver implements EFI_PCI_HOT_PLUG_INIT_PROTOCOL, providing the PCI bus
  driver with resource padding information, for PCIe hotplug purposes.

  Copyright (C) 2016, Red Hat, Inc.

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, WITHOUT
  WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include <IndustryStandard/Acpi10.h>
#include <IndustryStandard/QemuPciBridgeCapabilities.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PciLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/PciHotPlugInit.h>
#include <Protocol/PciRootBridgeIo.h>

//
// The protocol interface this driver produces.
//
// Refer to 12.6 "PCI Hot Plug PCI Initialization Protocol" in the Platform
// Init 1.4a Spec, Volume 5.
//
STATIC EFI_PCI_HOT_PLUG_INIT_PROTOCOL mPciHotPlugInit;


//
// Resource padding template for the GetResourcePadding() protocol member
// function.
//
// Refer to Table 8 "ACPI 2.0 & 3.0 QWORD Address Space Descriptor Usage" in
// the Platform Init 1.4a Spec, Volume 5.
//
// This structure is interpreted by the ApplyResourcePadding() function in the
// edk2 PCI Bus UEFI_DRIVER.
//
// We can request padding for at most four resource types, each of which is
// optional, independently of the others:
// (a) bus numbers,
// (b) IO space,
// (c) non-prefetchable MMIO space (32-bit only),
// (d) prefetchable MMIO space (either 32-bit or 64-bit, never both).
//
#pragma pack (1)
typedef struct {
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR Padding[4];
  EFI_ACPI_END_TAG_DESCRIPTOR       EndDesc;
} RESOURCE_PADDING;
#pragma pack ()


/**
  Initialize a RESOURCE_PADDING object.

  @param[out] ResourcePadding  The caller-allocated RESOURCE_PADDING object to
                               initialize.
**/
STATIC
VOID
InitializeResourcePadding (
  OUT RESOURCE_PADDING *ResourcePadding
  )
{
  UINTN Index;

  ZeroMem (ResourcePadding, sizeof *ResourcePadding);

  //
  // Fill in the Padding fields that don't vary across resource types.
  //
  for (Index = 0; Index < ARRAY_SIZE (ResourcePadding->Padding); ++Index) {
    EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptor;

    Descriptor       = ResourcePadding->Padding + Index;
    Descriptor->Desc = ACPI_ADDRESS_SPACE_DESCRIPTOR;
    Descriptor->Len  = (UINT16)(
                         sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) -
                         OFFSET_OF (
                           EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR,
                           ResType
                           )
                         );
  }

  //
  // Fill in the End Tag.
  //
  ResourcePadding->EndDesc.Desc = ACPI_END_TAG_DESCRIPTOR;
}


/**
  Set up a descriptor entry for reserving IO space.

  @param[in,out] Descriptor  The descriptor to configure. The caller shall have
                             initialized Descriptor earlier, with
                             InitializeResourcePadding().

  @param[in] SizeExponent    The size and natural alignment of the reservation
                             are determined by raising two to this power.
**/
STATIC
VOID
SetIoPadding (
  IN OUT EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptor,
  IN     UINTN                             SizeExponent
  )
{
  Descriptor->ResType = ACPI_ADDRESS_SPACE_TYPE_IO;
  Descriptor->AddrLen = LShiftU64 (1, SizeExponent);
  Descriptor->AddrRangeMax = Descriptor->AddrLen - 1;
}


/**
  Set up a descriptor entry for reserving MMIO space.

  @param[in,out] Descriptor    The descriptor to configure. The caller shall
                               have initialized Descriptor earlier, with
                               InitializeResourcePadding().

  @param[in] Prefetchable      TRUE if the descriptor should reserve
                               prefetchable MMIO space. Pass FALSE for
                               reserving non-prefetchable MMIO space.

  @param[in] ThirtyTwoBitOnly  TRUE if the reservation should be limited to
                               32-bit address space. FALSE if the reservation
                               can be satisfied from 64-bit address space.
                               ThirtyTwoBitOnly is ignored if Prefetchable is
                               FALSE; in that case ThirtyTwoBitOnly is always
                               considered TRUE.

  @param[in] SizeExponent      The size and natural alignment of the
                               reservation are determined by raising two to
                               this power.
**/
STATIC
VOID
SetMmioPadding (
  IN OUT EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptor,
  IN     BOOLEAN                           Prefetchable,
  IN     BOOLEAN                           ThirtyTwoBitOnly,
  IN     UINTN                             SizeExponent
  )
{
  Descriptor->ResType = ACPI_ADDRESS_SPACE_TYPE_MEM;
  if (Prefetchable) {
    Descriptor->SpecificFlag =
      EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_CACHEABLE_PREFETCHABLE;
    Descriptor->AddrSpaceGranularity = ThirtyTwoBitOnly ? 32 : 64;
  } else {
    Descriptor->SpecificFlag =
      EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_NON_CACHEABLE;
    Descriptor->AddrSpaceGranularity = 32;
  }
  Descriptor->AddrLen = LShiftU64 (1, SizeExponent);
  Descriptor->AddrRangeMax = Descriptor->AddrLen - 1;
}


/**
  Round up a positive 32-bit value to the next whole power of two, and return
  the bit position of the highest bit set in the result. Equivalent to
  ceil(log2(x)).

  @param[in] Operand  The 32-bit operand to evaluate.

  @retval -1     Operand is zero.

  @retval -1     Operand is positive, not a whole power of two, and rounding it
                 up to the next power of two does not fit into 32 bits.

  @retval 0..31  Otherwise, return ceil(log2(Value)).
**/
STATIC
INTN
HighBitSetRoundUp32 (
  IN UINT32 Operand
  )
{
  INTN HighBit;

  HighBit = HighBitSet32 (Operand);
  if (HighBit == -1) {
    //
    // Operand is zero.
    //
    return HighBit;
  }
  if ((Operand & (Operand - 1)) != 0) {
    //
    // Operand is not a whole power of two.
    //
    ++HighBit;
  }
  return (HighBit < 32) ? HighBit : -1;
}


/**
  Round up a positive 64-bit value to the next whole power of two, and return
  the bit position of the highest bit set in the result. Equivalent to
  ceil(log2(x)).

  @param[in] Operand  The 64-bit operand to evaluate.

  @retval -1     Operand is zero.

  @retval -1     Operand is positive, not a whole power of two, and rounding it
                 up to the next power of two does not fit into 64 bits.

  @retval 0..63  Otherwise, return ceil(log2(Value)).
**/
STATIC
INTN
HighBitSetRoundUp64 (
  IN UINT64 Operand
  )
{
  INTN HighBit;

  HighBit = HighBitSet64 (Operand);
  if (HighBit == -1) {
    //
    // Operand is zero.
    //
    return HighBit;
  }
  if ((Operand & (Operand - 1)) != 0) {
    //
    // Operand is not a whole power of two.
    //
    ++HighBit;
  }
  return (HighBit < 64) ? HighBit : -1;
}


/**
  Read a slice from conventional PCI config space at the given offset, then
  advance the offset.

  @param[in] PciAddress  The address of the PCI Device -- Bus, Device, Function
                         -- in UEFI (not PciLib) encoding.

  @param[in,out] Offset  On input, the offset in conventional PCI config space
                         to start reading from. On output, the offset of the
                         first byte that was not read.

  @param[in] Size        The number of bytes to read.

  @param[out] Buffer     On output, the bytes read from PCI config space are
                         stored in this object.
**/
STATIC
VOID
ReadConfigSpace (
  IN     CONST EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS *PciAddress,
  IN OUT UINT8                                             *Offset,
  IN     UINT8                                             Size,
  OUT    VOID                                              *Buffer
  )
{
  PciReadBuffer (
    PCI_LIB_ADDRESS (
      PciAddress->Bus,
      PciAddress->Device,
      PciAddress->Function,
      *Offset
      ),
    Size,
    Buffer
    );
  *Offset += Size;
}


/**
  Convenience wrapper macro for ReadConfigSpace().

  Given the following conditions:

  - HeaderField is the first field in the structure pointed-to by Struct,

  - Struct->HeaderField has been populated from the conventional PCI config
    space of the PCI device identified by PciAddress,

  - *Offset points one past HeaderField in the conventional PCI config space of
    the PCI device identified by PciAddress,

  populate the rest of *Struct from conventional PCI config space, starting at
  *Offset. Finally, increment *Offset so that it point one past *Struct.

  @param[in] PciAddress  The address of the PCI Device -- Bus, Device, Function
                         -- in UEFI (not PciLib) encoding. Type: pointer to
                         CONST EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS.

  @param[in,out] Offset  On input, the offset in conventional PCI config space
                         to start reading from; one past Struct->HeaderField.
                         On output, the offset of the first byte that was not
                         read; one past *Struct. Type: pointer to UINT8.

  @param[out] Struct     The structure to complete. Type: pointer to structure
                         object.

  @param[in] HeaderField The name of the first field in *Struct, after which
                         *Struct should be populated. Type: structure member
                         identifier.
**/
#define COMPLETE_CONFIG_SPACE_STRUCT(PciAddress, Offset, Struct, HeaderField) \
          ReadConfigSpace (                                                   \
            (PciAddress),                                                     \
            (Offset),                                                         \
            (UINT8)(sizeof *(Struct) - sizeof ((Struct)->HeaderField)),       \
            &((Struct)->HeaderField) + 1                                      \
            )


/**
  Look up the QEMU-specific Resource Reservation capability in the conventional
  config space of a Hotplug Controller (that is, PCI Bridge).

  This function performs as few config space reads as possible.

  @param[in] HpcPciAddress     The address of the PCI Bridge -- Bus, Device,
                               Function -- in UEFI (not PciLib) encoding.

  @param[out] ReservationHint  The caller-allocated capability structure to
                               populate from the PCI Bridge's config space.

  @retval EFI_SUCCESS    The capability has been found, ReservationHint has
                         been populated.

  @retval EFI_NOT_FOUND  The capability is missing. The contents of
                         ReservationHint are now indeterminate.
**/
STATIC
EFI_STATUS
QueryReservationHint (
  IN  CONST EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS *HpcPciAddress,
  OUT QEMU_PCI_BRIDGE_CAPABILITY_RESOURCE_RESERVATION   *ReservationHint
)
{
  UINT16 PciVendorId;
  UINT16 PciStatus;
  UINT8  PciCapPtr;
  UINT8  Offset;

  //
  // Check the vendor identifier.
  //
  PciVendorId = PciRead16 (
                  PCI_LIB_ADDRESS (
                    HpcPciAddress->Bus,
                    HpcPciAddress->Device,
                    HpcPciAddress->Function,
                    PCI_VENDOR_ID_OFFSET
                    )
                  );
  if (PciVendorId != QEMU_PCI_BRIDGE_VENDOR_ID_REDHAT) {
    return EFI_NOT_FOUND;
  }

  //
  // Check the Capabilities List bit in the PCI Status Register.
  //
  PciStatus = PciRead16 (
                PCI_LIB_ADDRESS (
                  HpcPciAddress->Bus,
                  HpcPciAddress->Device,
                  HpcPciAddress->Function,
                  PCI_PRIMARY_STATUS_OFFSET
                  )
                );
  if ((PciStatus & EFI_PCI_STATUS_CAPABILITY) == 0) {
    return EFI_NOT_FOUND;
  }

  //
  // Fetch the start of the Capabilities List.
  //
  PciCapPtr = PciRead8 (
                PCI_LIB_ADDRESS (
                  HpcPciAddress->Bus,
                  HpcPciAddress->Device,
                  HpcPciAddress->Function,
                  PCI_CAPBILITY_POINTER_OFFSET
                  )
                );

  //
  // Scan the Capabilities List until we find the terminator element, or the
  // Resource Reservation capability.
  //
  for (Offset = PciCapPtr & 0xFC;
       Offset > 0;
       Offset = ReservationHint->BridgeHdr.VendorHdr.Hdr.NextItemPtr & 0xFC) {
    BOOLEAN EnoughRoom;

    //
    // Check if the Resource Reservation capability would fit into config space
    // at this offset.
    //
    EnoughRoom = (BOOLEAN)(
                   Offset <= PCI_MAX_CONFIG_OFFSET - sizeof *ReservationHint
                   );

    //
    // Read the standard capability header so we can check the capability ID
    // (if necessary) and advance to the next capability.
    //
    ReadConfigSpace (
      HpcPciAddress,
      &Offset,
      (UINT8)sizeof ReservationHint->BridgeHdr.VendorHdr.Hdr,
      &ReservationHint->BridgeHdr.VendorHdr.Hdr
      );
    if (!EnoughRoom ||
        (ReservationHint->BridgeHdr.VendorHdr.Hdr.CapabilityID !=
         EFI_PCI_CAPABILITY_ID_VENDOR)) {
      continue;
    }

    //
    // Read the rest of the vendor capability header so we can check the
    // capability length.
    //
    COMPLETE_CONFIG_SPACE_STRUCT (
      HpcPciAddress,
      &Offset,
      &ReservationHint->BridgeHdr.VendorHdr,
      Hdr
      );
    if (ReservationHint->BridgeHdr.VendorHdr.Length !=
        sizeof *ReservationHint) {
      continue;
    }

    //
    // Read the rest of the QEMU bridge capability header so we can check the
    // capability type.
    //
    COMPLETE_CONFIG_SPACE_STRUCT (
      HpcPciAddress,
      &Offset,
      &ReservationHint->BridgeHdr,
      VendorHdr
      );
    if (ReservationHint->BridgeHdr.Type !=
        QEMU_PCI_BRIDGE_CAPABILITY_TYPE_RESOURCE_RESERVATION) {
      continue;
    }

    //
    // Read the body of the reservation hint.
    //
    COMPLETE_CONFIG_SPACE_STRUCT (
      HpcPciAddress,
      &Offset,
      ReservationHint,
      BridgeHdr
      );
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}


/**
  Returns a list of root Hot Plug Controllers (HPCs) that require
  initialization during the boot process.

  This procedure returns a list of root HPCs. The PCI bus driver must
  initialize  these controllers during the boot process. The PCI bus driver may
  or may not be  able to detect these HPCs. If the platform includes a
  PCI-to-CardBus bridge, it  can be included in this list if it requires
  initialization.  The HpcList must be  self consistent. An HPC cannot control
  any of its parent buses. Only one HPC can  control a PCI bus. Because this
  list includes only root HPCs, no HPC in the list  can be a child of another
  HPC. This policy must be enforced by the  EFI_PCI_HOT_PLUG_INIT_PROTOCOL.
  The PCI bus driver may not check for such  invalid conditions.  The callee
  allocates the buffer HpcList

  @param[in]  This       Pointer to the EFI_PCI_HOT_PLUG_INIT_PROTOCOL
                         instance.
  @param[out] HpcCount   The number of root HPCs that were returned.
  @param[out] HpcList    The list of root HPCs. HpcCount defines the number of
                         elements in this list.

  @retval EFI_SUCCESS             HpcList was returned.
  @retval EFI_OUT_OF_RESOURCES    HpcList was not returned due to insufficient
                                  resources.
  @retval EFI_INVALID_PARAMETER   HpcCount is NULL or HpcList is NULL.
**/
STATIC
EFI_STATUS
EFIAPI
GetRootHpcList (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL *This,
  OUT UINTN                          *HpcCount,
  OUT EFI_HPC_LOCATION               **HpcList
  )
{
  if (HpcCount == NULL || HpcList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // There are no top-level (i.e., un-enumerable) hot-plug controllers in QEMU
  // that would require special initialization.
  //
  *HpcCount = 0;
  *HpcList = NULL;
  return EFI_SUCCESS;
}


/**
  Initializes one root Hot Plug Controller (HPC). This process may causes
  initialization of its subordinate buses.

  This function initializes the specified HPC. At the end of initialization,
  the hot-plug slots or sockets (controlled by this HPC) are powered and are
  connected to the bus. All the necessary registers in the HPC are set up. For
  a Standard (PCI) Hot Plug Controller (SHPC), the registers that must be set
  up are defined in the PCI Standard Hot Plug Controller and Subsystem
  Specification.

  @param[in]  This            Pointer to the EFI_PCI_HOT_PLUG_INIT_PROTOCOL
                              instance.
  @param[in]  HpcDevicePath   The device path to the HPC that is being
                              initialized.
  @param[in]  HpcPciAddress   The address of the HPC function on the PCI bus.
  @param[in]  Event           The event that should be signaled when the HPC
                              initialization is complete.  Set to NULL if the
                              caller wants to wait until the entire
                              initialization  process is complete.
  @param[out] HpcState        The state of the HPC hardware. The state is
                              EFI_HPC_STATE_INITIALIZED or
                              EFI_HPC_STATE_ENABLED.

  @retval EFI_SUCCESS             If Event is NULL, the specific HPC was
                                  successfully initialized. If Event is not
                                  NULL, Event will be  signaled at a later time
                                  when initialization is complete.
  @retval EFI_UNSUPPORTED         This instance of
                                  EFI_PCI_HOT_PLUG_INIT_PROTOCOL does not
                                  support the specified HPC.
  @retval EFI_OUT_OF_RESOURCES    Initialization failed due to insufficient
                                  resources.
  @retval EFI_INVALID_PARAMETER   HpcState is NULL.
**/
STATIC
EFI_STATUS
EFIAPI
InitializeRootHpc (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL *This,
  IN  EFI_DEVICE_PATH_PROTOCOL       *HpcDevicePath,
  IN  UINT64                         HpcPciAddress,
  IN  EFI_EVENT                      Event, OPTIONAL
  OUT EFI_HPC_STATE                  *HpcState
  )
{
  //
  // This function should never be called, due to the information returned by
  // GetRootHpcList().
  //
  ASSERT (FALSE);

  if (HpcState == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_UNSUPPORTED;
}


/**
  Returns the resource padding that is required by the PCI bus that is
  controlled by the specified Hot Plug Controller (HPC).

  This function returns the resource padding that is required by the PCI bus
  that is controlled by the specified HPC. This member function is called for
  all the  root HPCs and nonroot HPCs that are detected by the PCI bus
  enumerator. This  function will be called before PCI resource allocation is
  completed. This function  must be called after all the root HPCs, with the
  possible exception of a  PCI-to-CardBus bridge, have completed
  initialization.

  @param[in]  This            Pointer to the EFI_PCI_HOT_PLUG_INIT_PROTOCOL
                              instance.
  @param[in]  HpcDevicePath   The device path to the HPC.
  @param[in]  HpcPciAddress   The address of the HPC function on the PCI bus.
  @param[in]  HpcState        The state of the HPC hardware.
  @param[out] Padding         The amount of resource padding that is required
                              by the PCI bus under the control of the specified
                              HPC.
  @param[out] Attributes      Describes how padding is accounted for. The
                              padding is returned in the form of ACPI 2.0
                              resource descriptors.

  @retval EFI_SUCCESS             The resource padding was successfully
                                  returned.
  @retval EFI_UNSUPPORTED         This instance of the
                                  EFI_PCI_HOT_PLUG_INIT_PROTOCOL does not
                                  support the specified HPC.
  @retval EFI_NOT_READY           This function was called before HPC
                                  initialization is complete.
  @retval EFI_INVALID_PARAMETER   HpcState or Padding or Attributes is NULL.
  @retval EFI_OUT_OF_RESOURCES    ACPI 2.0 resource descriptors for Padding
                                  cannot be allocated due to insufficient
                                  resources.
**/
STATIC
EFI_STATUS
EFIAPI
GetResourcePadding (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL *This,
  IN  EFI_DEVICE_PATH_PROTOCOL       *HpcDevicePath,
  IN  UINT64                         HpcPciAddress,
  OUT EFI_HPC_STATE                  *HpcState,
  OUT VOID                           **Padding,
  OUT EFI_HPC_PADDING_ATTRIBUTES     *Attributes
  )
{
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS     *Address;
  BOOLEAN                                         DefaultIo;
  BOOLEAN                                         DefaultMmio;
  RESOURCE_PADDING                                ReservationRequest;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR               *FirstResource;
  EFI_STATUS                                      ReservationHintStatus;
  QEMU_PCI_BRIDGE_CAPABILITY_RESOURCE_RESERVATION ReservationHint;

  Address = (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS *)&HpcPciAddress;

  DEBUG_CODE (
    CHAR16                                      *DevicePathString;

    DevicePathString = ConvertDevicePathToText (HpcDevicePath, FALSE, FALSE);

    DEBUG ((EFI_D_VERBOSE, "%a: Address=%02x:%02x.%x DevicePath=%s\n",
      __FUNCTION__, Address->Bus, Address->Device, Address->Function,
      (DevicePathString == NULL) ? L"<unavailable>" : DevicePathString));

    if (DevicePathString != NULL) {
      FreePool (DevicePathString);
    }
    );

  if (HpcState == NULL || Padding == NULL || Attributes == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DefaultIo = TRUE;
  DefaultMmio = TRUE;

  //
  // Init ReservationRequest, and point FirstResource one past the last
  // descriptor entry. We're going to build the entries backwards from
  // ReservationRequest.EndDesc.
  //
  InitializeResourcePadding (&ReservationRequest);
  FirstResource = ReservationRequest.Padding +
                  ARRAY_SIZE (ReservationRequest.Padding);

  //
  // Try to get the QEMU-specific Resource Reservation capability.
  //
  ReservationHintStatus = QueryReservationHint (Address, &ReservationHint);
  if (!EFI_ERROR (ReservationHintStatus)) {
    INTN HighBit;

    DEBUG ((
      DEBUG_VERBOSE,
      "%a: BusNumbers=0x%x Io=0x%Lx NonPrefetchable32BitMmio=0x%x\n"
      "%a: Prefetchable32BitMmio=0x%x Prefetchable64BitMmio=0x%Lx\n",
      __FUNCTION__,
      ReservationHint.BusNumbers,
      ReservationHint.Io,
      ReservationHint.NonPrefetchable32BitMmio,
      __FUNCTION__,
      ReservationHint.Prefetchable32BitMmio,
      ReservationHint.Prefetchable64BitMmio
      ));

    //
    // (a) Reserve bus numbers.
    //
    switch (ReservationHint.BusNumbers) {
    case 0:
      //
      // No reservation needed.
      //
      break;
    case MAX_UINT32:
      //
      // Firmware default (unspecified). Treat it as "no reservation needed".
      //
      break;
    default:
      //
      // Request the specified amount.
      //
      --FirstResource;
      FirstResource->ResType = ACPI_ADDRESS_SPACE_TYPE_BUS;
      FirstResource->AddrLen = ReservationHint.BusNumbers;
      break;
    }

    //
    // (b) Reserve IO space.
    //
    switch (ReservationHint.Io) {
    case 0:
      //
      // No reservation needed, disable our built-in.
      //
      DefaultIo = FALSE;
      break;
    case MAX_UINT64:
      //
      // Firmware default (unspecified). Stick with our built-in.
      //
      break;
    default:
      //
      // Round the specified amount up to the next power of two. If rounding is
      // successful, reserve the rounded value. Fall back to the default
      // otherwise.
      //
      HighBit = HighBitSetRoundUp64 (ReservationHint.Io);
      if (HighBit != -1) {
        SetIoPadding (--FirstResource, (UINTN)HighBit);
        DefaultIo = FALSE;
      }
      break;
    }

    //
    // (c) Reserve non-prefetchable MMIO space (32-bit only).
    //
    switch (ReservationHint.NonPrefetchable32BitMmio) {
    case 0:
      //
      // No reservation needed, disable our built-in.
      //
      DefaultMmio = FALSE;
      break;
    case MAX_UINT32:
      //
      // Firmware default (unspecified). Stick with our built-in.
      //
      break;
    default:
      //
      // Round the specified amount up to the next power of two. If rounding is
      // successful, reserve the rounded value. Fall back to the default
      // otherwise.
      //
      HighBit = HighBitSetRoundUp32 (ReservationHint.NonPrefetchable32BitMmio);
      if (HighBit != -1) {
        SetMmioPadding (--FirstResource, FALSE, TRUE, (UINTN)HighBit);
        DefaultMmio = FALSE;
      }
      break;
    }

    //
    // (d) Reserve prefetchable MMIO space (either 32-bit or 64-bit, never
    // both).
    //
    // For either space, we treat 0 as "no reservation needed", and the maximum
    // value as "firmware default". The latter is unspecified, and we interpret
    // it as the former.
    //
    // Otherwise, round the specified amount up to the next power of two. If
    // rounding is successful, reserve the rounded value. Do not reserve
    // prefetchable MMIO space otherwise.
    //
    if (ReservationHint.Prefetchable32BitMmio > 0 &&
        ReservationHint.Prefetchable32BitMmio < MAX_UINT32) {
      HighBit = HighBitSetRoundUp32 (ReservationHint.Prefetchable32BitMmio);
      if (HighBit != -1) {
        SetMmioPadding (--FirstResource, TRUE, TRUE, (UINTN)HighBit);
      }
    } else if (ReservationHint.Prefetchable64BitMmio > 0 &&
               ReservationHint.Prefetchable64BitMmio < MAX_UINT64) {
      HighBit = HighBitSetRoundUp64 (ReservationHint.Prefetchable64BitMmio);
      if (HighBit != -1) {
        SetMmioPadding (--FirstResource, TRUE, FALSE, (UINTN)HighBit);
      }
    }
  }

  if (DefaultIo) {
    //
    // Request defaults.
    //
    SetIoPadding (--FirstResource, (UINTN)HighBitSetRoundUp64 (512));
  }

  if (DefaultMmio) {
    //
    // Request defaults.
    //
    SetMmioPadding (
      --FirstResource,
      FALSE,
      TRUE,
      (UINTN)HighBitSetRoundUp32 (SIZE_2MB)
      );
  }

  //
  // Output a copy of ReservationRequest from the lowest-address populated
  // entry until the end of the structure (including
  // ReservationRequest.EndDesc). If no reservations are necessary, we'll only
  // output the End Tag.
  //
  *Padding = AllocateCopyPool (
               (UINT8 *)(&ReservationRequest + 1) - (UINT8 *)FirstResource,
               FirstResource
               );
  if (*Padding == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Resource padding is required.
  //
  *HpcState = EFI_HPC_STATE_INITIALIZED | EFI_HPC_STATE_ENABLED;

  //
  // The padding should be applied at PCI bus level, and considered by upstream
  // bridges, recursively.
  //
  *Attributes = EfiPaddingPciBus;
  return EFI_SUCCESS;
}


/**
  Entry point for this driver.

  @param[in] ImageHandle  Image handle of this driver.
  @param[in] SystemTable  Pointer to SystemTable.

  @retval EFI_SUCESS       Driver has loaded successfully.
  @return                  Error codes from lower level functions.

**/
EFI_STATUS
EFIAPI
DriverInitialize (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;

  mPciHotPlugInit.GetRootHpcList = GetRootHpcList;
  mPciHotPlugInit.InitializeRootHpc = InitializeRootHpc;
  mPciHotPlugInit.GetResourcePadding = GetResourcePadding;
  Status = gBS->InstallMultipleProtocolInterfaces (&ImageHandle,
                  &gEfiPciHotPlugInitProtocolGuid, &mPciHotPlugInit, NULL);
  return Status;
}
