/** @file
 * VBoxGuest - VirtualBox Guest Additions Driver Interface. (ADD,DEV)
 *
 * @remarks This is in the process of being split up and usage cleaned up.
 */

/*
 * Copyright (C) 2006-2009 Sun Microsystems, Inc.
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * The contents of this file may alternatively be used under the terms
 * of the Common Development and Distribution License Version 1.0
 * (CDDL) only, as it comes in the "COPYING.CDDL" file of the
 * VirtualBox OSE distribution, in which case the provisions of the
 * CDDL are applicable instead of those of the GPL.
 *
 * You may elect to license modified versions of this file under the
 * terms and conditions of either the GPL or the CDDL or both.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 USA or visit http://www.sun.com if you need
 * additional information or have any questions.
 */

#ifndef ___VBox_VBoxGuest_h
#define ___VBox_VBoxGuest_h

#include <VBox/cdefs.h>
#include <VBox/types.h>
#include <iprt/assert.h>
#include <VBox/VMMDev.h> /* Temporarily. */


/** @defgroup grp_vboxguest     VirtualBox Guest Additions Driver Interface
 * @{
 */

/** @todo it would be nice if we could have two define without paths. */

/** @def VBOXGUEST_DEVICE_NAME
 * The support device name. */
/** @def VBOXGUEST_USER_DEVICE_NAME
 * The support device name of the user accessible device node. */

#if defined(RT_OS_OS2)
# define VBOXGUEST_DEVICE_NAME          "\\Dev\\VBoxGst$"

#elif defined(RT_OS_WINDOWS)
# define VBOXGUEST_DEVICE_NAME          "\\\\.\\VBoxGuest"

/** The support service name. */
# define VBOXGUEST_SERVICE_NAME         "VBoxGuest"
/** Global name for Win2k+ */
# define VBOXGUEST_DEVICE_NAME_GLOBAL   "\\\\.\\Global\\VBoxGuest"
/** Win32 driver name */
# define VBOXGUEST_DEVICE_NAME_NT       L"\\Device\\VBoxGuest"
/** Device name. */
# define VBOXGUEST_DEVICE_NAME_DOS      L"\\DosDevices\\VBoxGuest"

#elif defined(RT_OS_LINUX) && !defined(VBOX_WITH_COMMON_VBOXGUEST_ON_LINUX)
# define VBOXGUEST_DEVICE_NAME          "/dev/vboxadd"
# define VBOXGUEST_USER_DEVICE_NAME     "/dev/vboxuser"

#else /* PORTME */
# define VBOXGUEST_DEVICE_NAME          "/dev/vboxguest"
# if defined(RT_OS_LINUX)
#  define VBOXGUEST_USER_DEVICE_NAME    "/dev/vboxuser"
# endif
#endif

#ifndef VBOXGUEST_USER_DEVICE_NAME
# define VBOXGUEST_USER_DEVICE_NAME     VBOXGUEST_DEVICE_NAME
#endif


#if !defined(IN_RC) && !defined(IN_RING0_AGNOSTIC) && !defined(IPRT_NO_CRT)
/** @name VBoxGuest IOCTL codes and structures.
 *
 * The range 0..15 is for basic driver communication.
 * The range 16..31 is for HGCM communcation.
 * The range 32..47 is reserved for future use.
 * The range 48..63 is for OS specific communcation.
 * The 7th bit is reserved for future hacks.
 * The 8th bit is reserved for distinguishing between 32-bit and 64-bit
 * processes in future 64-bit guest additions.
 *
 * @remarks When creating new IOCtl interfaces keep in mind that not all OSes supports
 *          reporting back the output size. (This got messed up a little bit in VBoxDrv.)
 *
 *          The request size is also a little bit tricky as it's passed as part of the
 *          request code on unix. The size field is 14 bits on Linux, 12 bits on *BSD,
 *          13 bits Darwin, and 8-bits on Solaris. All the BSDs and Darwin kernels
 *          will make use of the size field, while Linux and Solaris will not. We're of
 *          course using the size to validate and/or map/lock the request, so it has
 *          to be valid.
 *
 *          For Solaris we will have to do something special though, 255 isn't
 *          sufficent for all we need. A 4KB restriction (BSD) is probably not
 *          too problematic (yet) as a general one.
 *
 *          More info can be found in SUPDRVIOC.h and related sources.
 *
 * @remarks If adding interfaces that only has input or only has output, some new macros
 *          needs to be created so the most efficient IOCtl data buffering method can be
 *          used.
 * @{
 */
#ifdef RT_ARCH_AMD64
# define VBOXGUEST_IOCTL_FLAG     128
#elif defined(RT_ARCH_X86)
# define VBOXGUEST_IOCTL_FLAG     0
#else
# error "dunno which arch this is!"
#endif

/** Ring-3 request wrapper for big requests.
 *
 * This is necessary because the ioctl number scheme on many Unixy OSes (esp. Solaris)
 * only allows a relatively small size to be encoded into the request. So, for big
 * request this generic form is used instead. */
typedef struct VBGLBIGREQ
{
    /** Magic value (VBGLBIGREQ_MAGIC). */
    uint32_t    u32Magic;
    /** The size of the data buffer. */
    uint32_t    cbData;
    /** The user address of the data buffer. */
    RTR3PTR     pvDataR3;
#if HC_ARCH_BITS == 32
    uint32_t    u32Padding;
#endif
/** @todo r=bird: We need a 'rc' field for passing VBox status codes. Reused
 *        some input field as rc on output. */
} VBGLBIGREQ;
/** Pointer to a request wrapper for solaris guests. */
typedef VBGLBIGREQ *PVBGLBIGREQ;
/** Pointer to a const request wrapper for solaris guests. */
typedef const VBGLBIGREQ *PCVBGLBIGREQ;

/** The VBGLBIGREQ::u32Magic value (Ryuu Murakami). */
#define VBGLBIGREQ_MAGIC                            0x19520219


#if defined(RT_OS_WINDOWS)
/* @todo Remove IOCTL_CODE later! Integrate it in VBOXGUEST_IOCTL_CODE below. */
/** @todo r=bird: IOCTL_CODE is supposedly defined in some header included by Windows.h or ntddk.h, which is why it wasn't in the #if 0 earlier. See HostDrivers/Support/SUPDrvIOC.h... */
# define IOCTL_CODE(DeviceType, Function, Method, Access, DataSize_ignored) \
  ( ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
# define VBOXGUEST_IOCTL_CODE_(Function, Size)      IOCTL_CODE(FILE_DEVICE_UNKNOWN, 2048 + (Function), METHOD_BUFFERED, FILE_WRITE_ACCESS, 0)
# define VBOXGUEST_IOCTL_STRIP_SIZE_(Code)          (Code)

#elif defined(RT_OS_OS2)
  /* No automatic buffering, size not encoded. */
# define VBOXGUEST_IOCTL_CATEGORY                   0xc2
# define VBOXGUEST_IOCTL_CODE_(Function, Size)      ((unsigned char)(Function))
# define VBOXGUEST_IOCTL_CATEGORY_FAST              0xc3 /**< Also defined in VBoxGuestA-os2.asm. */
# define VBOXGUEST_IOCTL_CODE_FAST_(Function)       ((unsigned char)(Function))
# define VBOXGUEST_IOCTL_STRIP_SIZE(Code)           (Code)

#elif defined(RT_OS_SOLARIS)
  /* No automatic buffering, size limited to 255 bytes => use VBGLBIGREQ for everything. */
# include <sys/ioccom.h>
# define VBOXGUEST_IOCTL_CODE_(Function, Size)      _IOWRN('V', (Function), sizeof(VBGLBIGREQ))
# define VBOXGUEST_IOCTL_CODE_FAST_(Function)       _IO(  'V', (Function))
# define VBOXGUEST_IOCTL_STRIP_SIZE(Code)           (Code)

#elif defined(RT_OS_LINUX)
  /* No automatic buffering, size limited to 16KB. */
# include <linux/ioctl.h>
# define VBOXGUEST_IOCTL_CODE_(Function, Size)      _IOC(_IOC_READ|_IOC_WRITE, 'V', (Function), (Size))
# define VBOXGUEST_IOCTL_CODE_FAST_(Function)       _IO(  'V', (Function))
# define VBOXGUEST_IOCTL_STRIP_SIZE(Code)           VBOXGUEST_IOCTL_CODE_(_IOC_NR((Code)), 0)

#elif defined(RT_OS_FREEBSD) /** @todo r=bird: Please do it like SUPDRVIOC to keep it as similar as possible. */
# include <sys/ioccom.h>

# define VBOXGUEST_IOCTL_CODE_(Function, Size)      _IOWR('V', (Function), VBGLBIGREQ)
# define VBOXGUEST_IOCTL_CODE_FAST_(Function)       _IO(  'V', (Function))
# define VBOXGUEST_IOCTL_STRIP_SIZE(Code)           IOCBASECMD(Code)

#else
/* PORTME */
#endif

#define VBOXGUEST_IOCTL_CODE(Function, Size)        VBOXGUEST_IOCTL_CODE_((Function) | VBOXGUEST_IOCTL_FLAG, Size)
#define VBOXGUEST_IOCTL_CODE_FAST(Function)         VBOXGUEST_IOCTL_CODE_FAST_((Function) | VBOXGUEST_IOCTL_FLAG)

/* Define 32 bit codes to support 32 bit applications requests in the 64 bit guest driver. */
#ifdef RT_ARCH_AMD64
# define VBOXGUEST_IOCTL_CODE_32(Function, Size)    VBOXGUEST_IOCTL_CODE_(Function, Size)
# define VBOXGUEST_IOCTL_CODE_FAST_32(Function)     VBOXGUEST_IOCTL_CODE_FAST_(Function)
#endif /* RT_ARCH_AMD64 */



/** IOCTL to VBoxGuest to query the VMMDev IO port region start.
 * @remarks Ring-0 only. */
#define VBOXGUEST_IOCTL_GETVMMDEVPORT               VBOXGUEST_IOCTL_CODE(1, sizeof(VBoxGuestPortInfo))

#pragma pack(4)
typedef struct _VBoxGuestPortInfo
{
    uint32_t portAddress;
    VMMDevMemory *pVMMDevMemory;
} VBoxGuestPortInfo;


/** IOCTL to VBoxGuest to wait for a VMMDev host notification */
#define VBOXGUEST_IOCTL_WAITEVENT                   VBOXGUEST_IOCTL_CODE_(2, sizeof(VBoxGuestWaitEventInfo))

/** @name Result codes for VBoxGuestWaitEventInfo::u32Result
 * @{
 */
/** Successful completion, an event occured. */
#define VBOXGUEST_WAITEVENT_OK          (0)
/** Successful completion, timed out. */
#define VBOXGUEST_WAITEVENT_TIMEOUT     (1)
/** Wait was interrupted. */
#define VBOXGUEST_WAITEVENT_INTERRUPTED (2)
/** An error occured while processing the request. */
#define VBOXGUEST_WAITEVENT_ERROR       (3)
/** @} */

/** Input and output buffers layout of the IOCTL_VBOXGUEST_WAITEVENT */
typedef struct VBoxGuestWaitEventInfo
{
    /** timeout in milliseconds */
    uint32_t u32TimeoutIn;
    /** events to wait for */
    uint32_t u32EventMaskIn;
    /** result code */
    uint32_t u32Result;
    /** events occured */
    uint32_t u32EventFlagsOut;
} VBoxGuestWaitEventInfo;
AssertCompileSize(VBoxGuestWaitEventInfo, 16);


/** IOCTL to VBoxGuest to interrupt (cancel) any pending WAITEVENTs and return.
 * Handled inside the guest additions and not seen by the host at all.
 * @see VBOXGUEST_IOCTL_WAITEVENT */
#define VBOXGUEST_IOCTL_CANCEL_ALL_WAITEVENTS       VBOXGUEST_IOCTL_CODE_(5, 0)



/** IOCTL to VBoxGuest to perform a VMM request
 * @remark  The data buffer for this IOCtl has an variable size, keep this in mind
 *          on systems where this matters. */
#define VBOXGUEST_IOCTL_VMMREQUEST(Size)            VBOXGUEST_IOCTL_CODE_(3, (Size))


/** IOCTL to VBoxGuest to control event filter mask. */
#define VBOXGUEST_IOCTL_CTL_FILTER_MASK             VBOXGUEST_IOCTL_CODE_(4, sizeof(VBoxGuestFilterMaskInfo))

/** Input and output buffer layout of the IOCTL_VBOXGUEST_CTL_FILTER_MASK. */
typedef struct VBoxGuestFilterMaskInfo
{
    uint32_t u32OrMask;
    uint32_t u32NotMask;
} VBoxGuestFilterMaskInfo;
AssertCompileSize(VBoxGuestFilterMaskInfo, 8);
#pragma pack()


/** IOCTL to VBoxGuest to check memory ballooning. */
#define VBOXGUEST_IOCTL_CTL_CHECK_BALLOON_MASK      VBOXGUEST_IOCTL_CODE_(7, 100)


/** IOCTL to VBoxGuest to perform backdoor logging.
 * The argument is a string buffer of the specified size. */
#define VBOXGUEST_IOCTL_LOG(Size)                   VBOXGUEST_IOCTL_CODE_(6, (Size))


#ifdef VBOX_WITH_HGCM
/** IOCTL to VBoxGuest to connect to a HGCM service. */
# define VBOXGUEST_IOCTL_HGCM_CONNECT               VBOXGUEST_IOCTL_CODE(16, sizeof(VBoxGuestHGCMConnectInfo))

# pragma pack(1) /* explicit packing for good measure. */
typedef struct VBoxGuestHGCMConnectInfo
{
    int32_t result;           /**< OUT */
    HGCMServiceLocation Loc;  /**< IN */
    uint32_t u32ClientID;     /**< OUT */
} VBoxGuestHGCMConnectInfo;
AssertCompileSize(VBoxGuestHGCMConnectInfo, 4+4+128+4);
# pragma pack()


/** IOCTL to VBoxGuest to disconnect from a HGCM service. */
# define VBOXGUEST_IOCTL_HGCM_DISCONNECT          VBOXGUEST_IOCTL_CODE(17, sizeof(VBoxGuestHGCMDisconnectInfo))
typedef struct VBoxGuestHGCMDisconnectInfo
{
    int32_t result;           /**< OUT */
    uint32_t u32ClientID;     /**< IN */
} VBoxGuestHGCMDisconnectInfo;
AssertCompileSize(VBoxGuestHGCMDisconnectInfo, 8);


/** IOCTL to VBoxGuest to make a call to a HGCM service. */
# define VBOXGUEST_IOCTL_HGCM_CALL(Size)          VBOXGUEST_IOCTL_CODE(18, (Size))
typedef struct VBoxGuestHGCMCallInfo
{
    int32_t result;           /**< OUT Host HGCM return code.*/
    uint32_t u32ClientID;     /**< IN  The id of the caller. */
    uint32_t u32Function;     /**< IN  Function number. */
    uint32_t cParms;          /**< IN  How many parms. */
    /* Parameters follow in form HGCMFunctionParameter aParms[cParms] */
} VBoxGuestHGCMCallInfo;
AssertCompileSize(VBoxGuestHGCMCallInfo, 16);


/** IOCTL to VBoxGuest to make a timed call to a HGCM service. */
# define VBOXGUEST_IOCTL_HGCM_CALL_TIMED(Size)    VBOXGUEST_IOCTL_CODE(20, (Size))
# pragma pack(1) /* explicit packing for good measure. */
typedef struct VBoxGuestHGCMCallInfoTimed
{
    uint32_t u32Timeout;         /**< IN  How long to wait for completion before cancelling the call. */
    uint32_t fInterruptible;     /**< IN  Is this request interruptible? */
    VBoxGuestHGCMCallInfo info;  /**< IN/OUT The rest of the call information.  Placed after the timeout
                                  * so that the parameters follow as they would for a normal call. */
    /* Parameters follow in form HGCMFunctionParameter aParms[cParms] */
} VBoxGuestHGCMCallInfoTimed;
AssertCompileSize(VBoxGuestHGCMCallInfoTimed, 8+16);
# pragma pack()

# ifdef RT_ARCH_AMD64
/** @name IOCTL numbers that 32-bit clients, like the Windows OpenGL guest
 *        driver, will use when taking to a 64-bit driver.
 * @remarks These are only used by the driver implementation! */
#  define VBOXGUEST_IOCTL_HGCM_CONNECT_32           VBOXGUEST_IOCTL_CODE_32(16, sizeof(VBoxGuestHGCMConnectInfo))
#  define VBOXGUEST_IOCTL_HGCM_DISCONNECT_32        VBOXGUEST_IOCTL_CODE_32(17, sizeof(VBoxGuestHGCMDisconnectInfo))
#  define VBOXGUEST_IOCTL_HGCM_CALL_32(Size)        VBOXGUEST_IOCTL_CODE_32(18, (Size))
#  define VBOXGUEST_IOCTL_HGCM_CALL_TIMED_32(Size)  VBOXGUEST_IOCTL_CODE_32(20, (Size))
/** @} */
# endif /* RT_ARCH_AMD64 */

/** Get the pointer to the first HGCM parameter.  */
# define VBOXGUEST_HGCM_CALL_PARMS(a)             ( (HGCMFunctionParameter   *)((uint8_t *)(a) + sizeof(VBoxGuestHGCMCallInfo)) )
/** Get the pointer to the first HGCM parameter in a 32-bit request.  */
# define VBOXGUEST_HGCM_CALL_PARMS32(a)           ( (HGCMFunctionParameter32 *)((uint8_t *)(a) + sizeof(VBoxGuestHGCMCallInfo)) )

/** IOCTL to VBoxGuest to make a connect to the clipboard service.
 * @todo Seems this is no longer is use. Try remove it. */
# define VBOXGUEST_IOCTL_CLIPBOARD_CONNECT        VBOXGUEST_IOCTL_CODE_(19, sizeof(uint32_t))

#endif /* VBOX_WITH_HGCM */


#ifdef RT_OS_OS2

/**
 * The data buffer layout for the IDC entry point (AttachDD).
 *
 * @remark  This is defined in multiple 16-bit headers / sources.
 *          Some places it's called VBGOS2IDC to short things a bit.
 */
typedef struct VBOXGUESTOS2IDCCONNECT
{
    /** VMMDEV_VERSION. */
    uint32_t u32Version;
    /** Opaque session handle. */
    uint32_t u32Session;

    /**
     * The 32-bit service entry point.
     *
     * @returns VBox status code.
     * @param   u32Session          The above session handle.
     * @param   iFunction           The requested function.
     * @param   pvData              The input/output data buffer. The caller ensures that this
     *                              cannot be swapped out, or that it's acceptable to take a
     *                              page in fault in the current context. If the request doesn't
     *                              take input or produces output, apssing NULL is okay.
     * @param   cbData              The size of the data buffer.
     * @param   pcbDataReturned     Where to store the amount of data that's returned.
     *                              This can be NULL if pvData is NULL.
     */
    DECLCALLBACKMEMBER(int, pfnServiceEP)(uint32_t u32Session, unsigned iFunction, void *pvData, size_t cbData, size_t *pcbDataReturned);

    /** The 16-bit service entry point for C code (cdecl).
     *
     * It's the same as the 32-bit entry point, but the types has
     * changed to 16-bit equivalents.
     *
     * @code
     * int far cdecl
     * VBoxGuestOs2IDCService16(uint32_t u32Session, uint16_t iFunction,
     *                          void far *fpvData, uint16_t cbData, uint16_t far *pcbDataReturned);
     * @endcode
     */
    RTFAR16 fpfnServiceEP;

    /** The 16-bit service entry point for Assembly code (register).
     *
     * This is just a wrapper around fpfnServiceEP to simplify calls
     * from 16-bit assembly code.
     *
     * @returns (e)ax: VBox status code; cx: The amount of data returned.
     *
     * @param   u32Session          eax   - The above session handle.
     * @param   iFunction           dl    - The requested function.
     * @param   pvData              es:bx - The input/output data buffer.
     * @param   cbData              cx    - The size of the data buffer.
     */
    RTFAR16 fpfnServiceAsmEP;
} VBOXGUESTOS2IDCCONNECT;
/** Pointer to VBOXGUESTOS2IDCCONNECT buffer. */
typedef VBOXGUESTOS2IDCCONNECT *PVBOXGUESTOS2IDCCONNECT;

/** OS/2 specific: IDC client disconnect request.
 *
 * This takes no input and it doesn't return anything. Obviously this
 * is only recognized if it arrives thru the IDC service EP.
 */
# define VBOXGUEST_IOCTL_OS2_IDC_DISCONNECT     VBOXGUEST_IOCTL_CODE(48, sizeof(uint32_t))

#endif /* RT_OS_OS2 */

/** @} */
#endif /* !defined(IN_RC) && !defined(IN_RING0_AGNOSTIC) && !defined(IPRT_NO_CRT) */


#ifdef IN_RING3
# include <VBox/VBoxGuestLib.h> /** @todo eliminate this. */
#endif /* IN_RING3 */

#endif

