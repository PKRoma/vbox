/* $Id$ */
/** @file
 * DevPCNet - AMD PCnet-PCI II / PCnet-FAST III (Am79C970A / Am79C973) Ethernet Controller Emulation.
 *
 * This software was written to be compatible with the specifications:
 *      AMD Am79C970A PCnet-PCI II Ethernet Controller Data-Sheet
 *      AMD Publication# 19436  Rev:E  Amendment/0  Issue Date: June 2000
 * and
 *      todo
 */

/*
 * Copyright (C) 2006-2008 Sun Microsystems, Inc.
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 USA or visit http://www.sun.com if you need
 * additional information or have any questions.
 * --------------------------------------------------------------------
 *
 * This code is based on:
 *
 * AMD PC-Net II (Am79C970A) emulation
 *
 * Copyright (c) 2004 Antony T Curtis
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define LOG_GROUP LOG_GROUP_DEV_PCNET
#include <VBox/pdmdev.h>
#include <VBox/pgm.h>
#include <VBox/vm.h> /* for VM_IS_EMT */
#include <VBox/DevPCNet.h>
#include <iprt/asm.h>
#include <iprt/assert.h>
#include <iprt/critsect.h>
#include <iprt/string.h>
#include <iprt/time.h>
#include <iprt/net.h>
#ifdef IN_RING3
# include <iprt/mem.h>
# include <iprt/semaphore.h>
#endif

#include "../Builtins.h"

/* Enable this to catch writes to the ring descriptors instead of using excessive polling */
/* #define PCNET_NO_POLLING */

/* Enable to handle frequent io reads in the guest context (recommended) */
#define PCNET_GC_ENABLED

/* Experimental: queue TX packets */
//#define PCNET_QUEUE_SEND_PACKETS

#if defined(LOG_ENABLED)
#define PCNET_DEBUG_IO
#define PCNET_DEBUG_BCR
#define PCNET_DEBUG_CSR
#define PCNET_DEBUG_RMD
#define PCNET_DEBUG_TMD
#define PCNET_DEBUG_MATCH
#define PCNET_DEBUG_MII
#endif

#define PCNET_IOPORT_SIZE               0x20
#define PCNET_PNPMMIO_SIZE              0x20

#define PCNET_SAVEDSTATE_VERSION        10

#define BCR_MAX_RAP                     50
#define MII_MAX_REG                     32
#define CSR_MAX_REG                     128

/* Maximum number of times we report a link down to the guest (failure to send frame) */
#define PCNET_MAX_LINKDOWN_REPORTED     3

/* Maximum frame size we handle */
#define MAX_FRAME                       1536


typedef struct PCNetState_st PCNetState;

struct PCNetState_st
{
    PCIDEVICE                           PciDev;
#ifndef PCNET_NO_POLLING
    /** Poll timer - R3. */
    PTMTIMERR3                          pTimerPollR3;
    /** Poll timer - R0. */
    PTMTIMERR0                          pTimerPollR0;
    /** Poll timer - RC. */
    PTMTIMERRC                          pTimerPollRC;
#endif

#if HC_ARCH_BITS == 64
    uint32_t                            Alignment1;
#endif

    /** Software Interrupt timer - R3. */
    PTMTIMERR3                          pTimerSoftIntR3;
    /** Software Interrupt timer - R0. */
    PTMTIMERR0                          pTimerSoftIntR0;
    /** Software Interrupt timer - RC. */
    PTMTIMERRC                          pTimerSoftIntRC;

    /** Register Address Pointer */
    uint32_t                            u32RAP;
    /** Internal interrupt service */
    int32_t                             iISR;
    /** ??? */
    uint32_t                            u32Lnkst;
    /** Address of the RX descriptor table (ring). Loaded at init. */
    RTGCPHYS32                          GCRDRA;
    /** Address of the TX descriptor table (ring). Loaded at init. */
    RTGCPHYS32                          GCTDRA;
    uint8_t                             aPROM[16];
    uint16_t                            aCSR[CSR_MAX_REG];
    uint16_t                            aBCR[BCR_MAX_RAP];
    uint16_t                            aMII[MII_MAX_REG];
    uint16_t                            u16CSR0LastSeenByGuest;
    uint16_t                            Alignment2[HC_ARCH_BITS == 32 ? 2 : 4];
    /** Last time we polled the queues */
    uint64_t                            u64LastPoll;

    /** Size of current send frame */
    uint32_t                            cbSendFrame;
#if HC_ARCH_BITS == 64
    uint32_t                            Alignment3;
#endif
    /** Buffer address of current send frame */
    R3PTRTYPE(uint8_t *)                pvSendFrame;
    /** The xmit buffer. */
    uint8_t                             abSendBuf[4096];
    /** The recv buffer. */
    uint8_t                             abRecvBuf[4096];

    /** Pending send packet counter. */
    uint32_t                            cPendingSends;

    /** Size of a RX/TX descriptor (8 or 16 bytes according to SWSTYLE */
    int                                 iLog2DescSize;
    /** Bits 16..23 in 16-bit mode */
    RTGCPHYS32                          GCUpperPhys;

    /** Transmit signaller - RC. */
    RCPTRTYPE(PPDMQUEUE)                pXmitQueueRC;
    /** Transmit signaller - R3. */
    R3PTRTYPE(PPDMQUEUE)                pXmitQueueR3;
    /** Transmit signaller - R0. */
    R0PTRTYPE(PPDMQUEUE)                pXmitQueueR0;

    /** Receive signaller - R3. */
    R3PTRTYPE(PPDMQUEUE)                pCanRxQueueR3;
    /** Receive signaller - R0. */
    R0PTRTYPE(PPDMQUEUE)                pCanRxQueueR0;
    /** Receive signaller - RC. */
    RCPTRTYPE(PPDMQUEUE)                pCanRxQueueRC;
    /** Pointer to the device instance - RC. */
    PPDMDEVINSRC                        pDevInsRC;
    /** Pointer to the device instance - R3. */
    PPDMDEVINSR3                        pDevInsR3;
    /** Pointer to the device instance - R0. */
    PPDMDEVINSR0                        pDevInsR0;
    /** Restore timer.
     *  This is used to disconnect and reconnect the link after a restore. */
    PTMTIMERR3                          pTimerRestore;
    /** Pointer to the connector of the attached network driver. */
    R3PTRTYPE(PPDMINETWORKCONNECTOR)    pDrv;
    /** Pointer to the attached network driver. */
    R3PTRTYPE(PPDMIBASE)                pDrvBase;
    /** The base interface. */
    PDMIBASE                            IBase;
    /** The network port interface. */
    PDMINETWORKPORT                     INetworkPort;
    /** The network config port interface. */
    PDMINETWORKCONFIG                   INetworkConfig;
    /** Base address of the MMIO region. */
    RTGCPHYS32                          MMIOBase;
    /** Base port of the I/O space region. */
    RTIOPORT                            IOPortBase;
    /** If set the link is currently up. */
    bool                                fLinkUp;
    /** If set the link is temporarily down because of a saved state load. */
    bool                                fLinkTempDown;

    /** Number of times we've reported the link down. */
    RTUINT                              cLinkDownReported;
    /** The configured MAC address. */
    RTMAC                               MacConfigured;
    /** Alignment padding. */
    uint8_t                             Alignment4[HC_ARCH_BITS == 64 ? 6 : 2];

    /** The LED. */
    PDMLED                              Led;
    /** The LED ports. */
    PDMILEDPORTS                        ILeds;
    /** Partner of ILeds. */
    R3PTRTYPE(PPDMILEDCONNECTORS)       pLedsConnector;

    /** Async send thread */
    RTSEMEVENT                          hSendEventSem;
    /** The Async send thread. */
    PPDMTHREAD                          pSendThread;

    /** Access critical section. */
    PDMCRITSECT                         CritSect;
    /** Event semaphore for blocking on receive. */
    RTSEMEVENT                          hEventOutOfRxSpace;
    /** We are waiting/about to start waiting for more receive buffers. */
    bool volatile                       fMaybeOutOfSpace;
    /** True if we signal the guest that RX packets are missing. */
    bool                                fSignalRxMiss;
    uint8_t                             Alignment5[HC_ARCH_BITS == 64 ? 6 : 2];

#ifdef PCNET_NO_POLLING
    RTGCPHYS32                          TDRAPhysOld;
    uint32_t                            cbTDRAOld;

    RTGCPHYS32                          RDRAPhysOld;
    uint32_t                            cbRDRAOld;

    DECLRCCALLBACKMEMBER(int, pfnEMInterpretInstructionRC, (PVM pVM, PCPUMCTXCORE pRegFrame, RTGCPTR pvFault, uint32_t *pcbSize));
    DECLR0CALLBACKMEMBER(int, pfnEMInterpretInstructionR0, (PVM pVM, PCPUMCTXCORE pRegFrame, RTGCPTR pvFault, uint32_t *pcbSize));
#endif

    /** The shared memory used for the private interface - R3. */
    R3PTRTYPE(PPCNETGUESTSHAREDMEMORY)  pSharedMMIOR3;
    /** The shared memory used for the private interface - R0. */
    R0PTRTYPE(PPCNETGUESTSHAREDMEMORY)  pSharedMMIOR0;
    /** The shared memory used for the private interface - RC. */
    RCPTRTYPE(PPCNETGUESTSHAREDMEMORY)  pSharedMMIORC;

    /** Error counter for bad receive descriptors. */
    uint32_t                            uCntBadRMD;

    /** True if host and guest admitted to use the private interface. */
    bool                                fPrivIfEnabled;
    bool                                fGCEnabled;
    bool                                fR0Enabled;
    bool                                fAm79C973;
    uint32_t                            u32LinkSpeed;

#ifdef PCNET_QUEUE_SEND_PACKETS
# define PCNET_MAX_XMIT_SLOTS           128
# define PCNET_MAX_XMIT_SLOTS_MASK      (PCNET_MAX_XMIT_SLOTS - 1)

    uint32_t                            iXmitRingBufProd;
    uint32_t                            iXmitRingBufCons;
    /** @todo XXX currently atomic operations on this variable are overkill */
    volatile int32_t                    cXmitRingBufPending;
    uint16_t                            cbXmitRingBuffer[PCNET_MAX_XMIT_SLOTS];
    R3PTRTYPE(uint8_t *)                apXmitRingBuffer[PCNET_MAX_XMIT_SLOTS];
#endif

    STAMCOUNTER                         StatReceiveBytes;
    STAMCOUNTER                         StatTransmitBytes;
#ifdef VBOX_WITH_STATISTICS
    STAMPROFILEADV                      StatMMIOReadGC;
    STAMPROFILEADV                      StatMMIOReadHC;
    STAMPROFILEADV                      StatMMIOWriteGC;
    STAMPROFILEADV                      StatMMIOWriteHC;
    STAMPROFILEADV                      StatAPROMRead;
    STAMPROFILEADV                      StatAPROMWrite;
    STAMPROFILEADV                      StatIOReadGC;
    STAMPROFILEADV                      StatIOReadHC;
    STAMPROFILEADV                      StatIOWriteGC;
    STAMPROFILEADV                      StatIOWriteHC;
    STAMPROFILEADV                      StatTimer;
    STAMPROFILEADV                      StatReceive;
    STAMPROFILEADV                      StatTransmit;
    STAMPROFILEADV                      StatTransmitSend;
    STAMPROFILEADV                      StatTdtePollGC;
    STAMPROFILEADV                      StatTdtePollHC;
    STAMPROFILEADV                      StatTmdStoreGC;
    STAMPROFILEADV                      StatTmdStoreHC;
    STAMPROFILEADV                      StatRdtePollGC;
    STAMPROFILEADV                      StatRdtePollHC;
    STAMPROFILE                         StatRxOverflow;
    STAMCOUNTER                         StatRxOverflowWakeup;
    STAMCOUNTER                         aStatXmitFlush[16];
    STAMCOUNTER                         aStatXmitChainCounts[16];
    STAMCOUNTER                         StatXmitSkipCurrent;
    STAMPROFILEADV                      StatInterrupt;
    STAMPROFILEADV                      StatPollTimer;
    STAMCOUNTER                         StatMIIReads;
# ifdef PCNET_NO_POLLING
    STAMCOUNTER                         StatRCVRingWrite;
    STAMCOUNTER                         StatTXRingWrite;
    STAMCOUNTER                         StatRingWriteHC;
    STAMCOUNTER                         StatRingWriteR0;
    STAMCOUNTER                         StatRingWriteGC;

    STAMCOUNTER                         StatRingWriteFailedHC;
    STAMCOUNTER                         StatRingWriteFailedR0;
    STAMCOUNTER                         StatRingWriteFailedGC;

    STAMCOUNTER                         StatRingWriteOutsideHC;
    STAMCOUNTER                         StatRingWriteOutsideR0;
    STAMCOUNTER                         StatRingWriteOutsideGC;
# endif
#endif /* VBOX_WITH_STATISTICS */
};
AssertCompileMemberAlignment(PCNetState, StatReceiveBytes, 8);

#define PCNETSTATE_2_DEVINS(pPCNet)            ((pPCNet)->CTX_SUFF(pDevIns))
#define PCIDEV_2_PCNETSTATE(pPciDev)           ((PCNetState *)(pPciDev))
#define PCNET_INST_NR                          (PCNETSTATE_2_DEVINS(pThis)->iInstance)

/* BUS CONFIGURATION REGISTERS */
#define BCR_MSRDA       0
#define BCR_MSWRA       1
#define BCR_MC          2
#define BCR_RESERVED3   3
#define BCR_LNKST       4
#define BCR_LED1        5
#define BCR_LED2        6
#define BCR_LED3        7
#define BCR_RESERVED8   8
#define BCR_FDC         9
/* 10 - 15 = reserved */
#define BCR_IOBASEL     16  /* Reserved */
#define BCR_IOBASEU     16  /* Reserved */
#define BCR_BSBC        18
#define BCR_EECAS       19
#define BCR_SWS         20
#define BCR_INTCON      21  /* Reserved */
#define BCR_PLAT        22
#define BCR_PCISVID     23
#define BCR_PCISID      24
#define BCR_SRAMSIZ     25
#define BCR_SRAMB       26
#define BCR_SRAMIC      27
#define BCR_EBADDRL     28
#define BCR_EBADDRU     29
#define BCR_EBD         30
#define BCR_STVAL       31
#define BCR_MIICAS      32
#define BCR_MIIADDR     33
#define BCR_MIIMDR      34
#define BCR_PCIVID      35
#define BCR_PMC_A       36
#define BCR_DATA0       37
#define BCR_DATA1       38
#define BCR_DATA2       39
#define BCR_DATA3       40
#define BCR_DATA4       41
#define BCR_DATA5       42
#define BCR_DATA6       43
#define BCR_DATA7       44
#define BCR_PMR1        45
#define BCR_PMR2        46
#define BCR_PMR3        47

#define BCR_DWIO(S)      !!((S)->aBCR[BCR_BSBC] & 0x0080)
#define BCR_SSIZE32(S)   !!((S)->aBCR[BCR_SWS ] & 0x0100)
#define BCR_SWSTYLE(S)     ((S)->aBCR[BCR_SWS ] & 0x00FF)

#define CSR_INIT(S)      !!((S)->aCSR[0] & 0x0001)  /**< Init assertion */
#define CSR_STRT(S)      !!((S)->aCSR[0] & 0x0002)  /**< Start assertion */
#define CSR_STOP(S)      !!((S)->aCSR[0] & 0x0004)  /**< Stop assertion */
#define CSR_TDMD(S)      !!((S)->aCSR[0] & 0x0008)  /**< Transmit demand. (perform xmit poll now (readable, settable, not clearable) */
#define CSR_TXON(S)      !!((S)->aCSR[0] & 0x0010)  /**< Transmit on (readonly) */
#define CSR_RXON(S)      !!((S)->aCSR[0] & 0x0020)  /**< Receive On */
#define CSR_INEA(S)      !!((S)->aCSR[0] & 0x0040)  /**< Interrupt Enable */
#define CSR_LAPPEN(S)    !!((S)->aCSR[3] & 0x0020)  /**< Look Ahead Packet Processing Enable */
#define CSR_DXSUFLO(S)   !!((S)->aCSR[3] & 0x0040)  /**< Disable Transmit Stop on Underflow error */
#define CSR_ASTRP_RCV(S) !!((S)->aCSR[4] & 0x0400)  /**< Auto Strip Receive */
#define CSR_DPOLL(S)     !!((S)->aCSR[4] & 0x1000)  /**< Disable Transmit Polling */
#define CSR_SPND(S)      !!((S)->aCSR[5] & 0x0001)  /**< Suspend */
#define CSR_LTINTEN(S)   !!((S)->aCSR[5] & 0x4000)  /**< Last Transmit Interrupt Enable */
#define CSR_TOKINTD(S)   !!((S)->aCSR[5] & 0x8000)  /**< Transmit OK Interrupt Disable */

#define CSR_STINT        !!((S)->aCSR[7] & 0x0800)  /**< Software Timer Interrupt */
#define CSR_STINTE       !!((S)->aCSR[7] & 0x0400)  /**< Software Timer Interrupt Enable */

#define CSR_DRX(S)       !!((S)->aCSR[15] & 0x0001) /**< Disable Receiver */
#define CSR_DTX(S)       !!((S)->aCSR[15] & 0x0002) /**< Disable Transmit */
#define CSR_LOOP(S)      !!((S)->aCSR[15] & 0x0004) /**< Loopback Enable */
#define CSR_DRCVPA(S)    !!((S)->aCSR[15] & 0x2000) /**< Disable Receive Physical Address */
#define CSR_DRCVBC(S)    !!((S)->aCSR[15] & 0x4000) /**< Disable Receive Broadcast */
#define CSR_PROM(S)      !!((S)->aCSR[15] & 0x8000) /**< Promiscuous Mode */

#if !defined(RT_ARCH_X86) && !defined(RT_ARCH_AMD64)
#error fix macros (and more in this file) for big-endian machines
#endif

#define CSR_IADR(S)  (*(uint32_t*)((S)->aCSR +  1)) /**< Initialization Block Address */
#define CSR_CRBA(S)  (*(uint32_t*)((S)->aCSR + 18)) /**< Current Receive Buffer Address */
#define CSR_CXBA(S)  (*(uint32_t*)((S)->aCSR + 20)) /**< Current Transmit Buffer Address */
#define CSR_NRBA(S)  (*(uint32_t*)((S)->aCSR + 22)) /**< Next Receive Buffer Address */
#define CSR_BADR(S)  (*(uint32_t*)((S)->aCSR + 24)) /**< Base Address of Receive Ring */
#define CSR_NRDA(S)  (*(uint32_t*)((S)->aCSR + 26)) /**< Next Receive Descriptor Address */
#define CSR_CRDA(S)  (*(uint32_t*)((S)->aCSR + 28)) /**< Current Receive Descriptor Address */
#define CSR_BADX(S)  (*(uint32_t*)((S)->aCSR + 30)) /**< Base Address of Transmit Descriptor */
#define CSR_NXDA(S)  (*(uint32_t*)((S)->aCSR + 32)) /**< Next Transmit Descriptor Address */
#define CSR_CXDA(S)  (*(uint32_t*)((S)->aCSR + 34)) /**< Current Transmit Descriptor Address */
#define CSR_NNRD(S)  (*(uint32_t*)((S)->aCSR + 36)) /**< Next Next Receive Descriptor Address */
#define CSR_NNXD(S)  (*(uint32_t*)((S)->aCSR + 38)) /**< Next Next Transmit Descriptor Address */
#define CSR_CRBC(S)  ((S)->aCSR[40])                /**< Current Receive Byte Count */
#define CSR_CRST(S)  ((S)->aCSR[41])                /**< Current Receive Status */
#define CSR_CXBC(S)  ((S)->aCSR[42])                /**< Current Transmit Byte Count */
#define CSR_CXST(S)  ((S)->aCSR[43])                /**< Current transmit status */
#define CSR_NRBC(S)  ((S)->aCSR[44])                /**< Next Receive Byte Count */
#define CSR_NRST(S)  ((S)->aCSR[45])                /**< Next Receive Status */
#define CSR_POLL(S)  ((S)->aCSR[46])                /**< Transmit Poll Time Counter */
#define CSR_PINT(S)  ((S)->aCSR[47])                /**< Transmit Polling Interval */
#define CSR_PXDA(S)  (*(uint32_t*)((S)->aCSR + 60)) /**< Previous Transmit Descriptor Address*/
#define CSR_PXBC(S)  ((S)->aCSR[62])                /**< Previous Transmit Byte Count */
#define CSR_PXST(S)  ((S)->aCSR[63])                /**< Previous Transmit Status */
#define CSR_NXBA(S)  (*(uint32_t*)((S)->aCSR + 64)) /**< Next Transmit Buffer Address */
#define CSR_NXBC(S)  ((S)->aCSR[66])                /**< Next Transmit Byte Count */
#define CSR_NXST(S)  ((S)->aCSR[67])                /**< Next Transmit Status */
#define CSR_RCVRC(S) ((S)->aCSR[72])                /**< Receive Descriptor Ring Counter */
#define CSR_XMTRC(S) ((S)->aCSR[74])                /**< Transmit Descriptor Ring Counter */
#define CSR_RCVRL(S) ((S)->aCSR[76])                /**< Receive Descriptor Ring Length */
#define CSR_XMTRL(S) ((S)->aCSR[78])                /**< Transmit Descriptor Ring Length */
#define CSR_MISSC(S) ((S)->aCSR[112])               /**< Missed Frame Count */

#define PHYSADDR(S,A) ((A) | (S)->GCUpperPhys)

/* Version for the PCnet/FAST III 79C973 card */
#define CSR_VERSION_LOW_79C973  0x5003  /* the lower two bits must be 11b for AMD */
#define CSR_VERSION_LOW_79C970A 0x1003  /* the lower two bits must be 11b for AMD */
#define CSR_VERSION_HIGH        0x0262

/** @todo All structs: big endian? */

struct INITBLK16
{
    uint16_t mode;      /**< copied into csr15 */
    uint16_t padr1;     /**< MAC  0..15 */
    uint16_t padr2;     /**< MAC 16..32 */
    uint16_t padr3;     /**< MAC 33..47 */
    uint16_t ladrf1;    /**< logical address filter  0..15 */
    uint16_t ladrf2;    /**< logical address filter 16..31 */
    uint16_t ladrf3;    /**< logical address filter 32..47 */
    uint16_t ladrf4;    /**< logical address filter 48..63 */
    uint32_t rdra:24;   /**< address of receive descriptor ring */
    uint32_t res1:5;    /**< reserved */
    uint32_t rlen:3;    /**< number of receive descriptor ring entries */
    uint32_t tdra:24;   /**< address of transmit descriptor ring */
    uint32_t res2:5;    /**< reserved */
    uint32_t tlen:3;    /**< number of transmit descriptor ring entries */
};
AssertCompileSize(INITBLK16, 24);

/** bird:  I've changed the type for the bitfields. They should only be 16-bit all together.
 *  frank: I've changed the bitfiled types to uint32_t to prevent compiler warnings. */
struct INITBLK32
{
    uint16_t mode;      /**< copied into csr15 */
    uint16_t res1:4;    /**< reserved */
    uint16_t rlen:4;    /**< number of receive descriptor ring entries */
    uint16_t res2:4;    /**< reserved */
    uint16_t tlen:4;    /**< number of transmit descriptor ring entries */
    uint16_t padr1;     /**< MAC  0..15 */
    uint16_t padr2;     /**< MAC 16..31 */
    uint16_t padr3;     /**< MAC 32..47 */
    uint16_t res3;      /**< reserved */
    uint16_t ladrf1;    /**< logical address filter  0..15 */
    uint16_t ladrf2;    /**< logical address filter 16..31 */
    uint16_t ladrf3;    /**< logibal address filter 32..47 */
    uint16_t ladrf4;    /**< logical address filter 48..63 */
    uint32_t rdra;      /**< address of receive descriptor ring */
    uint32_t tdra;      /**< address of transmit descriptor ring */
};
AssertCompileSize(INITBLK32, 28);

/** Transmit Message Descriptor */
typedef struct TMD
{
    struct
    {
        uint32_t tbadr;         /**< transmit buffer address */
    } tmd0;
    struct
    {
        uint32_t bcnt:12;       /**< buffer byte count (two's complement) */
        uint32_t ones:4;        /**< must be 1111b */
        uint32_t res:7;         /**< reserved */
        uint32_t bpe:1;         /**< bus parity error */
        uint32_t enp:1;         /**< end of packet */
        uint32_t stp:1;         /**< start of packet */
        uint32_t def:1;         /**< deferred */
        uint32_t one:1;         /**< exactly one retry was needed to transmit a frame */
        uint32_t ltint:1;       /**< suppress interrupts after successful transmission */
        uint32_t nofcs:1;       /**< when set, the state of DXMTFCS is ignored and
                                     transmitter FCS generation is activated. */
        uint32_t err:1;         /**< error occurred */
        uint32_t own:1;         /**< 0=owned by guest driver, 1=owned by controller */
    } tmd1;
    struct
    {
        uint32_t trc:4;         /**< transmit retry count */
        uint32_t res:12;        /**< reserved */
        uint32_t tdr:10;        /**< ??? */
        uint32_t rtry:1;        /**< retry error */
        uint32_t lcar:1;        /**< loss of carrier */
        uint32_t lcol:1;        /**< late collision */
        uint32_t exdef:1;       /**< excessive deferral */
        uint32_t uflo:1;        /**< underflow error */
        uint32_t buff:1;        /**< out of buffers (ENP not found) */
    } tmd2;
    struct
    {
        uint32_t res;           /**< reserved for user defined space */
    } tmd3;
} TMD;
AssertCompileSize(TMD, 16);

/** Receive Message Descriptor */
typedef struct RMD
{
    struct
    {
        uint32_t rbadr;         /**< receive buffer address */
    } rmd0;
    struct
    {
        uint32_t bcnt:12;       /**< buffer byte count (two's complement) */
        uint32_t ones:4;        /**< must be 1111b */
        uint32_t res:4;         /**< reserved */
        uint32_t bam:1;         /**< broadcast address match */
        uint32_t lafm:1;        /**< logical filter address match */
        uint32_t pam:1;         /**< physcial address match */
        uint32_t bpe:1;         /**< bus parity error */
        uint32_t enp:1;         /**< end of packet */
        uint32_t stp:1;         /**< start of packet */
        uint32_t buff:1;        /**< buffer error */
        uint32_t crc:1;         /**< crc error on incoming frame */
        uint32_t oflo:1;        /**< overflow error (lost all or part of incoming frame) */
        uint32_t fram:1;        /**< frame error */
        uint32_t err:1;         /**< error occurred */
        uint32_t own:1;         /**< 0=owned by guest driver, 1=owned by controller */
    } rmd1;
    struct
    {
        uint32_t mcnt:12;       /**< message byte count */
        uint32_t zeros:4;       /**< 0000b */
        uint32_t rpc:8;         /**< receive frame tag */
        uint32_t rcc:8;         /**< receive frame tag + reserved */
    } rmd2;
    struct
    {
        uint32_t res;           /**< reserved for user defined space */
    } rmd3;
} RMD;
AssertCompileSize(RMD, 16);


#ifndef VBOX_DEVICE_STRUCT_TESTCASE
/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#define PRINT_TMD(T) Log2((    \
        "TMD0 : TBADR=%#010x\n" \
        "TMD1 : OWN=%d, ERR=%d, FCS=%d, LTI=%d, "       \
        "ONE=%d, DEF=%d, STP=%d, ENP=%d,\n"             \
        "       BPE=%d, BCNT=%d\n"                      \
        "TMD2 : BUF=%d, UFL=%d, EXD=%d, LCO=%d, "       \
        "LCA=%d, RTR=%d,\n"                             \
        "       TDR=%d, TRC=%d\n",                      \
        (T)->tmd0.tbadr,                                \
        (T)->tmd1.own, (T)->tmd1.err, (T)->tmd1.nofcs,  \
        (T)->tmd1.ltint, (T)->tmd1.one, (T)->tmd1.def,  \
        (T)->tmd1.stp, (T)->tmd1.enp, (T)->tmd1.bpe,    \
        4096-(T)->tmd1.bcnt,                            \
        (T)->tmd2.buff, (T)->tmd2.uflo, (T)->tmd2.exdef,\
        (T)->tmd2.lcol, (T)->tmd2.lcar, (T)->tmd2.rtry, \
        (T)->tmd2.tdr, (T)->tmd2.trc))

#define PRINT_RMD(R) Log2((    \
        "RMD0 : RBADR=%#010x\n" \
        "RMD1 : OWN=%d, ERR=%d, FRAM=%d, OFLO=%d, "     \
        "CRC=%d, BUFF=%d, STP=%d, ENP=%d,\n       "     \
        "BPE=%d, PAM=%d, LAFM=%d, BAM=%d, ONES=%d, BCNT=%d\n" \
        "RMD2 : RCC=%d, RPC=%d, MCNT=%d, ZEROS=%d\n",   \
        (R)->rmd0.rbadr,                                \
        (R)->rmd1.own, (R)->rmd1.err, (R)->rmd1.fram,   \
        (R)->rmd1.oflo, (R)->rmd1.crc, (R)->rmd1.buff,  \
        (R)->rmd1.stp, (R)->rmd1.enp, (R)->rmd1.bpe,    \
        (R)->rmd1.pam, (R)->rmd1.lafm, (R)->rmd1.bam,   \
        (R)->rmd1.ones, 4096-(R)->rmd1.bcnt,            \
        (R)->rmd2.rcc, (R)->rmd2.rpc, (R)->rmd2.mcnt,   \
        (R)->rmd2.zeros))

#if defined(PCNET_QUEUE_SEND_PACKETS) && defined(IN_RING3)
static int pcnetSyncTransmit(PCNetState *pThis);
#endif
static void pcnetPollTimerStart(PCNetState *pThis);

/**
 * Load transmit message descriptor
 * Make sure we read the own flag first.
 *
 * @param pThis         adapter private data
 * @param addr          physical address of the descriptor
 * @param fRetIfNotOwn  return immediately after reading the own flag if we don't own the descriptor
 * @return              true if we own the descriptor, false otherwise
 */
DECLINLINE(bool) pcnetTmdLoad(PCNetState *pThis, TMD *tmd, RTGCPHYS32 addr, bool fRetIfNotOwn)
{
    PPDMDEVINS pDevIns = PCNETSTATE_2_DEVINS(pThis);
    uint8_t    ownbyte;

    if (pThis->fPrivIfEnabled)
    {
        /* RX/TX descriptors shared between host and guest => direct copy */
        uint8_t *pv = (uint8_t*)pThis->CTX_SUFF(pSharedMMIO)
                    + (addr - pThis->GCTDRA)
                    + pThis->CTX_SUFF(pSharedMMIO)->V.V1.offTxDescriptors;
        if (!(pv[7] & 0x80) && fRetIfNotOwn)
            return false;
        memcpy(tmd, pv, 16);
        return true;
    }
    else if (RT_UNLIKELY(BCR_SWSTYLE(pThis) == 0))
    {
        uint16_t xda[4];

        PDMDevHlpPhysRead(pDevIns, addr+3, &ownbyte, 1);
        if (!(ownbyte & 0x80) && fRetIfNotOwn)
            return false;
        PDMDevHlpPhysRead(pDevIns, addr, (void*)&xda[0], sizeof(xda));
        ((uint32_t *)tmd)[0] = (uint32_t)xda[0] | ((uint32_t)(xda[1] & 0x00ff) << 16);
        ((uint32_t *)tmd)[1] = (uint32_t)xda[2] | ((uint32_t)(xda[1] & 0xff00) << 16);
        ((uint32_t *)tmd)[2] = (uint32_t)xda[3] << 16;
        ((uint32_t *)tmd)[3] = 0;
    }
    else if (RT_LIKELY(BCR_SWSTYLE(pThis) != 3))
    {
        PDMDevHlpPhysRead(pDevIns, addr+7, &ownbyte, 1);
        if (!(ownbyte & 0x80) && fRetIfNotOwn)
            return false;
        PDMDevHlpPhysRead(pDevIns, addr, (void*)tmd, 16);
    }
    else
    {
        uint32_t xda[4];
        PDMDevHlpPhysRead(pDevIns, addr+7, &ownbyte, 1);
        if (!(ownbyte & 0x80) && fRetIfNotOwn)
            return false;
        PDMDevHlpPhysRead(pDevIns, addr, (void*)&xda[0], sizeof(xda));
        ((uint32_t *)tmd)[0] = xda[2];
        ((uint32_t *)tmd)[1] = xda[1];
        ((uint32_t *)tmd)[2] = xda[0];
        ((uint32_t *)tmd)[3] = xda[3];
    }
    /* Double check the own bit; guest drivers might be buggy and lock prefixes in the recompiler are ignored by other threads. */
#ifdef DEBUG
    if (tmd->tmd1.own == 1 && !(ownbyte & 0x80))
        Log(("pcnetTmdLoad: own bit flipped while reading!!\n"));
#endif
    if (!(ownbyte & 0x80))
        tmd->tmd1.own = 0;

    return !!tmd->tmd1.own;
}

/**
 * Store transmit message descriptor and hand it over to the host (the VM guest).
 * Make sure that all data are transmitted before we clear the own flag.
 */
DECLINLINE(void) pcnetTmdStorePassHost(PCNetState *pThis, TMD *tmd, RTGCPHYS32 addr)
{
    STAM_PROFILE_ADV_START(&pThis->CTXSUFF(StatTmdStore), a);
    PPDMDEVINS pDevIns = PCNETSTATE_2_DEVINS(pThis);
    if (pThis->fPrivIfEnabled)
    {
        /* RX/TX descriptors shared between host and guest => direct copy */
        uint8_t *pv = (uint8_t*)pThis->CTX_SUFF(pSharedMMIO)
                    + (addr - pThis->GCTDRA)
                    + pThis->CTX_SUFF(pSharedMMIO)->V.V1.offTxDescriptors;
        memcpy(pv, tmd, 16);
        pv[7] &= ~0x80;
    }
    else if (RT_UNLIKELY(BCR_SWSTYLE(pThis) == 0))
    {
        uint16_t xda[4];
        xda[0] =   ((uint32_t *)tmd)[0]        & 0xffff;
        xda[1] = ((((uint32_t *)tmd)[0] >> 16) &   0xff) | ((((uint32_t *)tmd)[1]>>16) & 0xff00);
        xda[2] =   ((uint32_t *)tmd)[1]        & 0xffff;
        xda[3] =   ((uint32_t *)tmd)[2] >> 16;
        xda[1] |=  0x8000;
        PDMDevHlpPhysWrite(pDevIns, addr, (void*)&xda[0], sizeof(xda));
        xda[1] &= ~0x8000;
        PDMDevHlpPhysWrite(pDevIns, addr+3, (uint8_t*)xda + 3, 1);
    }
    else if (RT_LIKELY(BCR_SWSTYLE(pThis) != 3))
    {
        ((uint32_t*)tmd)[1] |=  0x80000000;
        PDMDevHlpPhysWrite(pDevIns, addr, (void*)tmd, 16);
        ((uint32_t*)tmd)[1] &= ~0x80000000;
        PDMDevHlpPhysWrite(pDevIns, addr+7, (uint8_t*)tmd + 7, 1);
    }
    else
    {
        uint32_t xda[4];
        xda[0] = ((uint32_t *)tmd)[2];
        xda[1] = ((uint32_t *)tmd)[1];
        xda[2] = ((uint32_t *)tmd)[0];
        xda[3] = ((uint32_t *)tmd)[3];
        xda[1] |=  0x80000000;
        PDMDevHlpPhysWrite(pDevIns, addr, (void*)&xda[0], sizeof(xda));
        xda[1] &= ~0x80000000;
        PDMDevHlpPhysWrite(pDevIns, addr+7, (uint8_t*)xda + 7, 1);
    }
    STAM_PROFILE_ADV_STOP(&pThis->CTXSUFF(StatTmdStore), a);
}

/**
 * Load receive message descriptor
 * Make sure we read the own flag first.
 *
 * @param pThis         adapter private data
 * @param addr          physical address of the descriptor
 * @param fRetIfNotOwn  return immediately after reading the own flag if we don't own the descriptor
 * @return              true if we own the descriptor, false otherwise
 */
DECLINLINE(int) pcnetRmdLoad(PCNetState *pThis, RMD *rmd, RTGCPHYS32 addr, bool fRetIfNotOwn)
{
    PPDMDEVINS pDevIns = PCNETSTATE_2_DEVINS(pThis);
    uint8_t    ownbyte;

    if (pThis->fPrivIfEnabled)
    {
        /* RX/TX descriptors shared between host and guest => direct copy */
        uint8_t *pb = (uint8_t*)pThis->CTX_SUFF(pSharedMMIO)
                    + (addr - pThis->GCRDRA)
                    + pThis->CTX_SUFF(pSharedMMIO)->V.V1.offRxDescriptors;
        if (!(pb[7] & 0x80) && fRetIfNotOwn)
            return false;
        memcpy(rmd, pb, 16);
        return true;
    }
    else if (RT_UNLIKELY(BCR_SWSTYLE(pThis) == 0))
    {
        uint16_t rda[4];
        PDMDevHlpPhysRead(pDevIns, addr+3, &ownbyte, 1);
        if (!(ownbyte & 0x80) && fRetIfNotOwn)
            return false;
        PDMDevHlpPhysRead(pDevIns, addr, (void*)&rda[0], sizeof(rda));
        ((uint32_t *)rmd)[0] = (uint32_t)rda[0] | ((rda[1] & 0x00ff) << 16);
        ((uint32_t *)rmd)[1] = (uint32_t)rda[2] | ((rda[1] & 0xff00) << 16);
        ((uint32_t *)rmd)[2] = (uint32_t)rda[3];
        ((uint32_t *)rmd)[3] = 0;
    }
    else if (RT_LIKELY(BCR_SWSTYLE(pThis) != 3))
    {
        PDMDevHlpPhysRead(pDevIns, addr+7, &ownbyte, 1);
        if (!(ownbyte & 0x80) && fRetIfNotOwn)
            return false;
        PDMDevHlpPhysRead(pDevIns, addr, (void*)rmd, 16);
    }
    else
    {
        uint32_t rda[4];
        PDMDevHlpPhysRead(pDevIns, addr+7, &ownbyte, 1);
        if (!(ownbyte & 0x80) && fRetIfNotOwn)
            return false;
        PDMDevHlpPhysRead(pDevIns, addr, (void*)&rda[0], sizeof(rda));
        ((uint32_t *)rmd)[0] = rda[2];
        ((uint32_t *)rmd)[1] = rda[1];
        ((uint32_t *)rmd)[2] = rda[0];
        ((uint32_t *)rmd)[3] = rda[3];
    }
    /* Double check the own bit; guest drivers might be buggy and lock prefixes in the recompiler are ignored by other threads. */
#ifdef DEBUG
    if (rmd->rmd1.own == 1 && !(ownbyte & 0x80))
        Log(("pcnetRmdLoad: own bit flipped while reading!!\n"));
#endif
    if (!(ownbyte & 0x80))
        rmd->rmd1.own = 0;

    return !!rmd->rmd1.own;
}

#ifdef IN_RING3

/**
 * Store receive message descriptor and hand it over to the host (the VM guest).
 * Make sure that all data are transmitted before we clear the own flag.
 */
DECLINLINE(void) pcnetRmdStorePassHost(PCNetState *pThis, RMD *rmd, RTGCPHYS32 addr)
{
    PPDMDEVINS pDevIns = PCNETSTATE_2_DEVINS(pThis);
    if (pThis->fPrivIfEnabled)
    {
        /* RX/TX descriptors shared between host and guest => direct copy */
        uint8_t *pv = (uint8_t*)pThis->CTX_SUFF(pSharedMMIO)
                    + (addr - pThis->GCRDRA)
                    + pThis->CTX_SUFF(pSharedMMIO)->V.V1.offRxDescriptors;
        memcpy(pv, rmd, 16);
        pv[7] &= ~0x80;
    }
    else if (RT_UNLIKELY(BCR_SWSTYLE(pThis) == 0))
    {
        uint16_t rda[4];
        rda[0] =   ((uint32_t *)rmd)[0]      & 0xffff;
        rda[1] = ((((uint32_t *)rmd)[0]>>16) &   0xff) | ((((uint32_t *)rmd)[1]>>16) & 0xff00);
        rda[2] =   ((uint32_t *)rmd)[1]      & 0xffff;
        rda[3] =   ((uint32_t *)rmd)[2]      & 0xffff;
        rda[1] |=  0x8000;
        PDMDevHlpPhysWrite(pDevIns, addr, (void*)&rda[0], sizeof(rda));
        rda[1] &= ~0x8000;
        PDMDevHlpPhysWrite(pDevIns, addr+3, (uint8_t*)rda + 3, 1);
    }
    else if (RT_LIKELY(BCR_SWSTYLE(pThis) != 3))
    {
        ((uint32_t*)rmd)[1] |=  0x80000000;
        PDMDevHlpPhysWrite(pDevIns, addr, (void*)rmd, 16);
        ((uint32_t*)rmd)[1] &= ~0x80000000;
        PDMDevHlpPhysWrite(pDevIns, addr+7, (uint8_t*)rmd + 7, 1);
    }
    else
    {
        uint32_t rda[4];
        rda[0] = ((uint32_t *)rmd)[2];
        rda[1] = ((uint32_t *)rmd)[1];
        rda[2] = ((uint32_t *)rmd)[0];
        rda[3] = ((uint32_t *)rmd)[3];
        rda[1] |=  0x80000000;
        PDMDevHlpPhysWrite(pDevIns, addr, (void*)&rda[0], sizeof(rda));
        rda[1] &= ~0x80000000;
        PDMDevHlpPhysWrite(pDevIns, addr+7, (uint8_t*)rda + 7, 1);
    }
}

/**
 * Read+Write a TX/RX descriptor to prevent PDMDevHlpPhysWrite() allocating
 * pages later when we shouldn't schedule to EMT. Temporarily hack.
 */
static void pcnetDescTouch(PCNetState *pThis, RTGCPHYS32 addr)
{
    PPDMDEVINS pDevIns = PCNETSTATE_2_DEVINS(pThis);

    if (!pThis->fPrivIfEnabled)
    {
        uint8_t aBuf[16];
        size_t cbDesc;
        if (RT_UNLIKELY(BCR_SWSTYLE(pThis) == 0))
            cbDesc = 8;
        else
            cbDesc = 16;
        PDMDevHlpPhysRead(pDevIns, addr, aBuf, cbDesc);
        PDMDevHlpPhysWrite(pDevIns, addr, aBuf, cbDesc);
    }
}

#endif /* IN_RING3 */

/** Checks if it's a bad (as in invalid) RMD.*/
#define IS_RMD_BAD(rmd)      ((rmd).rmd1.ones != 15 || (rmd).rmd2.zeros != 0)

/** The network card is the owner of the RDTE/TDTE, actually it is this driver */
#define CARD_IS_OWNER(desc)   (((desc) & 0x8000))

/** The host is the owner of the RDTE/TDTE -- actually the VM guest. */
#define HOST_IS_OWNER(desc)  (!((desc) & 0x8000))

#ifndef ETHER_IS_MULTICAST /* Net/Open BSD macro it seems */
#define ETHER_IS_MULTICAST(a) ((*(uint8_t *)(a)) & 1)
#endif

#define ETHER_ADDR_LEN ETH_ALEN
#define ETH_ALEN 6
#pragma pack(1)
struct ether_header /** @todo Use RTNETETHERHDR */
{
    uint8_t  ether_dhost[ETH_ALEN]; /**< destination ethernet address */
    uint8_t  ether_shost[ETH_ALEN]; /**< source ethernet address */
    uint16_t ether_type;            /**< packet type ID field */
};
#pragma pack()

#define PRINT_PKTHDR(BUF) do {                                        \
    struct ether_header *hdr = (struct ether_header *)(BUF);          \
    Log(("#%d packet dhost=%02x:%02x:%02x:%02x:%02x:%02x, "          \
         "shost=%02x:%02x:%02x:%02x:%02x:%02x, "                      \
         "type=%#06x (bcast=%d)\n", PCNET_INST_NR,                    \
         hdr->ether_dhost[0],hdr->ether_dhost[1],hdr->ether_dhost[2], \
         hdr->ether_dhost[3],hdr->ether_dhost[4],hdr->ether_dhost[5], \
         hdr->ether_shost[0],hdr->ether_shost[1],hdr->ether_shost[2], \
         hdr->ether_shost[3],hdr->ether_shost[4],hdr->ether_shost[5], \
         htons(hdr->ether_type),                                      \
         !!ETHER_IS_MULTICAST(hdr->ether_dhost)));                    \
} while (0)


#ifdef IN_RING3

/**
 * Initialize the shared memory for the private guest interface.
 *
 * @note Changing this layout will break SSM for guests using the private guest interface!
 */
static void pcnetInitSharedMemory(PCNetState *pThis)
{
    /* Clear the entire block for pcnetReset usage. */
    memset(pThis->pSharedMMIOR3, 0, PCNET_GUEST_SHARED_MEMORY_SIZE);

    pThis->pSharedMMIOR3->u32Version = PCNET_GUEST_INTERFACE_VERSION;
    uint32_t off = 2048; /* Leave some space for more fields within the header */

    /*
     * The Descriptor arrays.
     */
    pThis->pSharedMMIOR3->V.V1.offTxDescriptors = off;
    off = RT_ALIGN(off + PCNET_GUEST_TX_DESCRIPTOR_SIZE * PCNET_GUEST_MAX_TX_DESCRIPTORS, 32);

    pThis->pSharedMMIOR3->V.V1.offRxDescriptors = off;
    off = RT_ALIGN(off + PCNET_GUEST_RX_DESCRIPTOR_SIZE * PCNET_GUEST_MAX_RX_DESCRIPTORS, 32);

    /* Make sure all the descriptors are mapped into HMA space (and later ring-0). The 8192
       bytes limit is hardcoded in the PDMDevHlpMMHyperMapMMIO2 call down in pcnetConstruct. */
    AssertRelease(off <= 8192);

    /*
     * The buffer arrays.
     */
#if 0
    /* Don't allocate TX buffers since Windows guests cannot use it */
    pThis->pSharedMMIOR3->V.V1.offTxBuffers = off;
    off = RT_ALIGN(off + PCNET_GUEST_NIC_BUFFER_SIZE * PCNET_GUEST_MAX_TX_DESCRIPTORS, 32);
#endif

    pThis->pSharedMMIOR3->V.V1.offRxBuffers = off;
    pThis->pSharedMMIOR3->fFlags = PCNET_GUEST_FLAGS_ADMIT_HOST;
    off = RT_ALIGN(off + PCNET_GUEST_NIC_BUFFER_SIZE * PCNET_GUEST_MAX_RX_DESCRIPTORS, 32);
    AssertRelease(off <= PCNET_GUEST_SHARED_MEMORY_SIZE);

    /* Update the header with the final size. */
    pThis->pSharedMMIOR3->cbUsed = off;
}

#define MULTICAST_FILTER_LEN 8

DECLINLINE(uint32_t) lnc_mchash(const uint8_t *ether_addr)
{
#define LNC_POLYNOMIAL          0xEDB88320UL
    uint32_t crc = 0xFFFFFFFF;
    int idx, bit;
    uint8_t data;

    for (idx = 0; idx < ETHER_ADDR_LEN; idx++)
    {
        for (data = *ether_addr++, bit = 0; bit < MULTICAST_FILTER_LEN; bit++)
        {
            crc = (crc >> 1) ^ (((crc ^ data) & 1) ? LNC_POLYNOMIAL : 0);
            data >>= 1;
        }
    }
    return crc;
#undef LNC_POLYNOMIAL
}

#define CRC(crc, ch)  (crc = (crc >> 8) ^ crctab[(crc ^ (ch)) & 0xff])

/* generated using the AUTODIN II polynomial
 *   x^32 + x^26 + x^23 + x^22 + x^16 +
 *   x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x^1 + 1
 */
static const uint32_t crctab[256] =
{
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
    0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
    0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
    0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
    0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
    0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
    0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
    0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
    0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
    0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
    0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
    0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
    0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
    0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
    0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
    0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
    0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
    0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
    0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
    0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
    0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};

DECLINLINE(int) padr_match(PCNetState *pThis, const uint8_t *buf, size_t size)
{
    struct ether_header *hdr = (struct ether_header *)buf;
    int     result;
#if (defined(RT_ARCH_X86) || defined(RT_ARCH_AMD64)) && !defined(PCNET_DEBUG_MATCH)
    result = !CSR_DRCVPA(pThis) && !memcmp(hdr->ether_dhost, pThis->aCSR + 12, 6);
#else
    uint8_t padr[6];
    padr[0] = pThis->aCSR[12] & 0xff;
    padr[1] = pThis->aCSR[12] >> 8;
    padr[2] = pThis->aCSR[13] & 0xff;
    padr[3] = pThis->aCSR[13] >> 8;
    padr[4] = pThis->aCSR[14] & 0xff;
    padr[5] = pThis->aCSR[14] >> 8;
    result = !CSR_DRCVPA(pThis) && !memcmp(hdr->ether_dhost, padr, 6);
#endif

#ifdef PCNET_DEBUG_MATCH
    Log(("#%d packet dhost=%02x:%02x:%02x:%02x:%02x:%02x, "
         "padr=%02x:%02x:%02x:%02x:%02x:%02x => %d\n", PCNET_INST_NR,
         hdr->ether_dhost[0],hdr->ether_dhost[1],hdr->ether_dhost[2],
         hdr->ether_dhost[3],hdr->ether_dhost[4],hdr->ether_dhost[5],
         padr[0],padr[1],padr[2],padr[3],padr[4],padr[5], result));
#endif
    return result;
}

DECLINLINE(int) padr_bcast(PCNetState *pThis, const uint8_t *buf, size_t size)
{
    static uint8_t aBCAST[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    struct ether_header *hdr = (struct ether_header *)buf;
    int result = !CSR_DRCVBC(pThis) && !memcmp(hdr->ether_dhost, aBCAST, 6);
#ifdef PCNET_DEBUG_MATCH
    Log(("#%d padr_bcast result=%d\n", PCNET_INST_NR, result));
#endif
   return result;
}

static int ladr_match(PCNetState *pThis, const uint8_t *buf, size_t size)
{
    struct ether_header *hdr = (struct ether_header *)buf;
    if (RT_UNLIKELY(hdr->ether_dhost[0] & 0x01) && ((uint64_t *)&pThis->aCSR[8])[0] != 0LL)
    {
        int index;
#if defined(RT_ARCH_X86) || defined(RT_ARCH_AMD64)
        index = lnc_mchash(hdr->ether_dhost) >> 26;
        return ((uint8_t*)(pThis->aCSR + 8))[index >> 3] & (1 << (index & 7));
#else
        uint8_t ladr[8];
        ladr[0] = pThis->aCSR[8] & 0xff;
        ladr[1] = pThis->aCSR[8] >> 8;
        ladr[2] = pThis->aCSR[9] & 0xff;
        ladr[3] = pThis->aCSR[9] >> 8;
        ladr[4] = pThis->aCSR[10] & 0xff;
        ladr[5] = pThis->aCSR[10] >> 8;
        ladr[6] = pThis->aCSR[11] & 0xff;
        ladr[7] = pThis->aCSR[11] >> 8;
        index = lnc_mchash(hdr->ether_dhost) >> 26;
        return (ladr[index >> 3] & (1 << (index & 7)));
#endif
    }
    return 0;
}

#endif /* IN_RING3 */

/**
 * Get the receive descriptor ring address with a given index.
 */
DECLINLINE(RTGCPHYS32) pcnetRdraAddr(PCNetState *pThis, int idx)
{
    return pThis->GCRDRA + ((CSR_RCVRL(pThis) - idx) << pThis->iLog2DescSize);
}

/**
 * Get the transmit descriptor ring address with a given index.
 */
DECLINLINE(RTGCPHYS32) pcnetTdraAddr(PCNetState *pThis, int idx)
{
    return pThis->GCTDRA + ((CSR_XMTRL(pThis) - idx) << pThis->iLog2DescSize);
}

RT_C_DECLS_BEGIN
PDMBOTHCBDECL(int) pcnetIOPortRead(PPDMDEVINS pDevIns, void *pvUser,
                                   RTIOPORT Port, uint32_t *pu32, unsigned cb);
PDMBOTHCBDECL(int) pcnetIOPortWrite(PPDMDEVINS pDevIns, void *pvUser,
                                    RTIOPORT Port, uint32_t u32, unsigned cb);
PDMBOTHCBDECL(int) pcnetIOPortAPromWrite(PPDMDEVINS pDevIns, void *pvUser,
                                         RTIOPORT Port, uint32_t u32, unsigned cb);
PDMBOTHCBDECL(int) pcnetIOPortAPromRead(PPDMDEVINS pDevIns, void *pvUser,
                                        RTIOPORT Port, uint32_t *pu32, unsigned cb);
PDMBOTHCBDECL(int) pcnetMMIORead(PPDMDEVINS pDevIns, void *pvUser,
                                 RTGCPHYS GCPhysAddr, void *pv, unsigned cb);
PDMBOTHCBDECL(int) pcnetMMIOWrite(PPDMDEVINS pDevIns, void *pvUser,
                                  RTGCPHYS GCPhysAddr, void *pv, unsigned cb);
#ifndef IN_RING3
DECLEXPORT(int) pcnetHandleRingWrite(PVM pVM, RTGCUINT uErrorCode, PCPUMCTXCORE pRegFrame,
                                     RTGCPTR pvFault, RTGCPHYS GCPhysFault, void *pvUser);
#endif
RT_C_DECLS_END

#undef htonl
#define htonl(x)    ASMByteSwapU32(x)
#undef htons
#define htons(x)    ( (((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8) )

static void     pcnetPollRxTx(PCNetState *pThis);
static void     pcnetPollTimer(PCNetState *pThis);
static void     pcnetUpdateIrq(PCNetState *pThis);
static uint32_t pcnetBCRReadU16(PCNetState *pThis, uint32_t u32RAP);
static int      pcnetBCRWriteU16(PCNetState *pThis, uint32_t u32RAP, uint32_t val);


#ifdef PCNET_NO_POLLING
# ifndef IN_RING3

/**
 * #PF Virtual Handler callback for Guest write access to the ring descriptor page(pThis)
 *
 * @return  VBox status code (appropriate for trap handling and GC return).
 * @param   pVM         VM Handle.
 * @param   uErrorCode  CPU Error code.
 * @param   pRegFrame   Trap register frame.
 * @param   pvFault     The fault address (cr2).
 * @param   GCPhysFault The GC physical address corresponding to pvFault.
 * @param   pvUser      User argument.
 */
DECLEXPORT(int) pcnetHandleRingWrite(PVM pVM, RTGCUINT uErrorCode, PCPUMCTXCORE pRegFrame,
                                     RTGCPTR pvFault, RTGCPHYS GCPhysFault, void *pvUser)
{
    PCNetState *pThis   = (PCNetState *)pvUser;

    Log(("#%d pcnetHandleRingWriteGC: write to %#010x\n", PCNET_INST_NR, GCPhysFault));

    uint32_t cb;
    int rc = CTXALLSUFF(pThis->pfnEMInterpretInstruction)(pVM, pRegFrame, pvFault, &cb);
    if (RT_SUCCESS(rc) && cb)
    {
        if (    (GCPhysFault >= pThis->GCTDRA && GCPhysFault + cb < pcnetTdraAddr(pThis, 0))
#ifdef PCNET_MONITOR_RECEIVE_RING
            ||  (GCPhysFault >= pThis->GCRDRA && GCPhysFault + cb < pcnetRdraAddr(pThis, 0))
#endif
           )
        {
            uint32_t offsetTDRA = (GCPhysFault - pThis->GCTDRA);

            int rc = PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);
            if (RT_SUCCESS(rc))
            {
                STAM_COUNTER_INC(&CTXALLSUFF(pThis->StatRingWrite)); ;

                /* Check if we can do something now */
                pcnetPollRxTx(pThis);
                pcnetUpdateIrq(pThis);

                PDMCritSectLeave(&pThis->CritSect);
                return VINF_SUCCESS;
            }
        }
        else
        {
            STAM_COUNTER_INC(&CTXALLSUFF(pThis->StatRingWriteOutside)); ;
            return VINF_SUCCESS;    /* outside of the ring range */
        }
    }
    STAM_COUNTER_INC(&CTXALLSUFF(pThis->StatRingWriteFailed)); ;
    return VINF_IOM_HC_MMIO_WRITE; /* handle in ring3 */
}

# else /* IN_RING3 */

/**
 * #PF Handler callback for physical access handler ranges (MMIO among others) in HC.
 *
 * The handler can not raise any faults, it's mainly for monitoring write access
 * to certain pages.
 *
 * @returns VINF_SUCCESS if the handler have carried out the operation.
 * @returns VINF_PGM_HANDLER_DO_DEFAULT if the caller should carry out the access operation.
 * @param   pVM             VM Handle.
 * @param   GCPhys          The physical address the guest is writing to.
 * @param   pvPhys          The HC mapping of that address.
 * @param   pvBuf           What the guest is reading/writing.
 * @param   cbBuf           How much it's reading/writing.
 * @param   enmAccessType   The access type.
 * @param   pvUser          User argument.
 */
static DECLCALLBACK(int) pcnetHandleRingWrite(PVM pVM, RTGCPHYS GCPhys, void *pvPhys, void *pvBuf,
                                              size_t cbBuf, PGMACCESSTYPE enmAccessType, void *pvUser)
{
    PPDMDEVINS  pDevIns = (PPDMDEVINS)pvUser;
    PCNetState *pThis   = PDMINS_2_DATA(pDevIns, PCNetState *);

    Log(("#%d pcnetHandleRingWrite: write to %#010x\n", PCNET_INST_NR, GCPhys));
#ifdef VBOX_WITH_STATISTICS
    STAM_COUNTER_INC(&CTXSUFF(pThis->StatRingWrite));
    if (GCPhys >= pThis->GCRDRA && GCPhys < pcnetRdraAddr(pThis, 0))
        STAM_COUNTER_INC(&pThis->StatRCVRingWrite);
    else if (GCPhys >= pThis->GCTDRA && GCPhys < pcnetTdraAddr(pThis, 0))
        STAM_COUNTER_INC(&pThis->StatTXRingWrite);
#endif
    /* Perform the actual write */
    memcpy((char *)pvPhys, pvBuf, cbBuf);

    /* Writes done by our code don't require polling of course */
    if (PDMCritSectIsOwner(&pThis->CritSect) == false)
    {
        if (    (GCPhys >= pThis->GCTDRA && GCPhys + cbBuf < pcnetTdraAddr(pThis, 0))
#ifdef PCNET_MONITOR_RECEIVE_RING
            ||  (GCPhys >= pThis->GCRDRA && GCPhys + cbBuf < pcnetRdraAddr(pThis, 0))
#endif
           )
        {
            int rc = PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);
            AssertReleaseRC(rc);
            /* Check if we can do something now */
            pcnetPollRxTx(pThis);
            pcnetUpdateIrq(pThis);
            PDMCritSectLeave(&pThis->CritSect);
        }
    }
    return VINF_SUCCESS;
}
# endif /* !IN_RING3 */
#endif /* PCNET_NO_POLLING */

static void pcnetSoftReset(PCNetState *pThis)
{
    Log(("#%d pcnetSoftReset:\n", PCNET_INST_NR));

    pThis->u32Lnkst = 0x40;
    pThis->GCRDRA   = 0;
    pThis->GCTDRA   = 0;
    pThis->u32RAP   = 0;

    pThis->aCSR[0]   = 0x0004;
    pThis->aCSR[3]   = 0x0000;
    pThis->aCSR[4]   = 0x0115;
    pThis->aCSR[5]   = 0x0000;
    pThis->aCSR[6]   = 0x0000;
    pThis->aCSR[8]   = 0;
    pThis->aCSR[9]   = 0;
    pThis->aCSR[10]  = 0;
    pThis->aCSR[11]  = 0;
    pThis->aCSR[12]  = RT_LE2H_U16(((uint16_t *)&pThis->aPROM[0])[0]);
    pThis->aCSR[13]  = RT_LE2H_U16(((uint16_t *)&pThis->aPROM[0])[1]);
    pThis->aCSR[14]  = RT_LE2H_U16(((uint16_t *)&pThis->aPROM[0])[2]);
    pThis->aCSR[15] &= 0x21c4;
    CSR_RCVRC(pThis) = 1;
    CSR_XMTRC(pThis) = 1;
    CSR_RCVRL(pThis) = 1;
    CSR_XMTRL(pThis) = 1;
    pThis->aCSR[80]  = 0x1410;
    pThis->aCSR[88]  = pThis->fAm79C973 ? CSR_VERSION_LOW_79C973 : CSR_VERSION_LOW_79C970A;
    pThis->aCSR[89]  = CSR_VERSION_HIGH;
    pThis->aCSR[94]  = 0x0000;
    pThis->aCSR[100] = 0x0200;
    pThis->aCSR[103] = 0x0105;
    pThis->aCSR[103] = 0x0105;
    CSR_MISSC(pThis) = 0;
    pThis->aCSR[114] = 0x0000;
    pThis->aCSR[122] = 0x0000;
    pThis->aCSR[124] = 0x0000;
}

/**
 * Check if we have to send an interrupt to the guest. An interrupt can occur on
 * - csr0 (written quite often)
 * - csr4 (only written by pcnetSoftReset(), pcnetStop() or by the guest driver)
 * - csr5 (only written by pcnetSoftReset(), pcnetStop or by the driver guest)
 */
static void pcnetUpdateIrq(PCNetState *pThis)
{
    register int      iISR = 0;
    register uint16_t csr0 = pThis->aCSR[0];

    csr0 &= ~0x0080; /* clear INTR */

    STAM_PROFILE_ADV_START(&pThis->StatInterrupt, a);

    /* Linux guests set csr4=0x0915
     * W2k   guests set csr3=0x4940 (disable BABL, MERR, IDON, DXSUFLO */

#if 1
    if (    ( (csr0               & ~pThis->aCSR[3]) & 0x5f00)
         || (((pThis->aCSR[4]>>1) & ~pThis->aCSR[4]) & 0x0115)
         || (((pThis->aCSR[5]>>1) &  pThis->aCSR[5]) & 0x0048))
#else
    if (  ( !(pThis->aCSR[3] & 0x4000) && !!(csr0           & 0x4000)) /* BABL */
        ||( !(pThis->aCSR[3] & 0x1000) && !!(csr0           & 0x1000)) /* MISS */
        ||( !(pThis->aCSR[3] & 0x0100) && !!(csr0           & 0x0100)) /* IDON */
        ||( !(pThis->aCSR[3] & 0x0200) && !!(csr0           & 0x0200)) /* TINT */
        ||( !(pThis->aCSR[3] & 0x0400) && !!(csr0           & 0x0400)) /* RINT */
        ||( !(pThis->aCSR[3] & 0x0800) && !!(csr0           & 0x0800)) /* MERR */
        ||( !(pThis->aCSR[4] & 0x0001) && !!(pThis->aCSR[4] & 0x0002)) /* JAB */
        ||( !(pThis->aCSR[4] & 0x0004) && !!(pThis->aCSR[4] & 0x0008)) /* TXSTRT */
        ||( !(pThis->aCSR[4] & 0x0010) && !!(pThis->aCSR[4] & 0x0020)) /* RCVO */
        ||( !(pThis->aCSR[4] & 0x0100) && !!(pThis->aCSR[4] & 0x0200)) /* MFCO */
        ||(!!(pThis->aCSR[5] & 0x0040) && !!(pThis->aCSR[5] & 0x0080)) /* EXDINT */
        ||(!!(pThis->aCSR[5] & 0x0008) && !!(pThis->aCSR[5] & 0x0010)) /* MPINT */)
#endif
    {
        iISR = !!(csr0 & 0x0040); /* CSR_INEA */
        csr0 |= 0x0080; /* set INTR */
    }

#ifdef VBOX
    if (pThis->aCSR[4] & 0x0080) /* UINTCMD */
    {
        pThis->aCSR[4] &= ~0x0080; /* clear UINTCMD */
        pThis->aCSR[4] |=  0x0040; /* set UINT */
        Log(("#%d user int\n", PCNET_INST_NR));
    }
    if (pThis->aCSR[4] & csr0 & 0x0040 /* CSR_INEA */)
    {
        csr0 |=  0x0080; /* set INTR */
        iISR = 1;
    }
#else /* !VBOX */
    if (!!(pThis->aCSR[4] & 0x0080) && CSR_INEA(pThis)) /* UINTCMD */
    {
        pThis->aCSR[4] &= ~0x0080;
        pThis->aCSR[4] |=  0x0040; /* set UINT */
        csr0           |=  0x0080; /* set INTR */
        iISR = 1;
        Log(("#%d user int\n", PCNET_INST_NR));
    }
#endif /* !VBOX */

#if 1
    if (((pThis->aCSR[5]>>1) & pThis->aCSR[5]) & 0x0500)
#else
    if (   (!!(pThis->aCSR[5] & 0x0400) && !!(pThis->aCSR[5] & 0x0800)) /* SINT */
         ||(!!(pThis->aCSR[5] & 0x0100) && !!(pThis->aCSR[5] & 0x0200)) /* SLPINT */)
#endif
    {
        iISR = 1;
        csr0 |= 0x0080; /* INTR */
    }

    if ((pThis->aCSR[7] & 0x0C00) == 0x0C00) /* STINT + STINTE */
        iISR = 1;

    pThis->aCSR[0] = csr0;

    Log2(("#%d set irq iISR=%d\n", PCNET_INST_NR, iISR));

    /* normal path is to _not_ change the IRQ status */
    if (RT_UNLIKELY(iISR != pThis->iISR))
    {
        Log(("#%d INTA=%d\n", PCNET_INST_NR, iISR));
        PDMDevHlpPCISetIrqNoWait(PCNETSTATE_2_DEVINS(pThis), 0, iISR);
        pThis->iISR = iISR;
    }
    STAM_PROFILE_ADV_STOP(&pThis->StatInterrupt, a);
}

/**
 * Enable/disable the private guest interface.
 */
static void pcnetEnablePrivateIf(PCNetState *pThis)
{
    bool fPrivIfEnabled =       pThis->pSharedMMIOR3
                          && !!(pThis->CTX_SUFF(pSharedMMIO)->fFlags & PCNET_GUEST_FLAGS_ADMIT_GUEST);
    if (fPrivIfEnabled != pThis->fPrivIfEnabled)
    {
        pThis->fPrivIfEnabled = fPrivIfEnabled;
        LogRel(("PCNet#%d: %s private interface\n", PCNET_INST_NR, fPrivIfEnabled ? "Enabling" : "Disabling"));
    }
}

#ifdef IN_RING3
#ifdef PCNET_NO_POLLING
static void pcnetUpdateRingHandlers(PCNetState *pThis)
{
    PPDMDEVINS pDevIns = PCNETSTATE_2_DEVINS(pThis);
    int rc;

    Log(("pcnetUpdateRingHandlers TD %RX32 size %#x -> %RX32 ?size? %#x\n", pThis->TDRAPhysOld, pThis->cbTDRAOld, pThis->GCTDRA, pcnetTdraAddr(pThis, 0)));
    Log(("pcnetUpdateRingHandlers RX %RX32 size %#x -> %RX32 ?size? %#x\n", pThis->RDRAPhysOld, pThis->cbRDRAOld, pThis->GCRDRA, pcnetRdraAddr(pThis, 0)));

    /** @todo unregister order not correct! */

#ifdef PCNET_MONITOR_RECEIVE_RING
    if (pThis->GCRDRA != pThis->RDRAPhysOld || CSR_RCVRL(pThis) != pThis->cbRDRAOld)
    {
        if (pThis->RDRAPhysOld != 0)
            PGMHandlerPhysicalDeregister(PDMDevHlpGetVM(pDevIns),
                                        pThis->RDRAPhysOld & ~PAGE_OFFSET_MASK);

        rc = PGMR3HandlerPhysicalRegister(PDMDevHlpGetVM(pDevIns),
                                          PGMPHYSHANDLERTYPE_PHYSICAL_WRITE,
                                          pThis->GCRDRA & ~PAGE_OFFSET_MASK,
                                          RT_ALIGN(pcnetRdraAddr(pThis, 0), PAGE_SIZE) - 1,
                                          pcnetHandleRingWrite, pDevIns,
                                          g_DevicePCNet.szR0Mod, "pcnetHandleRingWrite",
                                          pThis->pDevInsHC->pvInstanceDataHC,
                                          g_DevicePCNet.szRCMod, "pcnetHandleRingWrite",
                                          pThis->pDevInsHC->pvInstanceDataRC,
                                          "PCNet receive ring write access handler");
        AssertRC(rc);

        pThis->RDRAPhysOld = pThis->GCRDRA;
        pThis->cbRDRAOld   = pcnetRdraAddr(pThis, 0);
    }
#endif /* PCNET_MONITOR_RECEIVE_RING */

#ifdef PCNET_MONITOR_RECEIVE_RING
    /* 3 possibilities:
     * 1) TDRA on different physical page as RDRA
     * 2) TDRA completely on same physical page as RDRA
     * 3) TDRA & RDRA overlap partly with different physical pages
     */
    RTGCPHYS32 RDRAPageStart = pThis->GCRDRA & ~PAGE_OFFSET_MASK;
    RTGCPHYS32 RDRAPageEnd   = (pcnetRdraAddr(pThis, 0) - 1) & ~PAGE_OFFSET_MASK;
    RTGCPHYS32 TDRAPageStart = pThis->GCTDRA & ~PAGE_OFFSET_MASK;
    RTGCPHYS32 TDRAPageEnd   = (pcnetTdraAddr(pThis, 0) - 1) & ~PAGE_OFFSET_MASK;

    if (    RDRAPageStart > TDRAPageEnd
        ||  TDRAPageStart > RDRAPageEnd)
    {
#endif /* PCNET_MONITOR_RECEIVE_RING */
        /* 1) */
        if (pThis->GCTDRA != pThis->TDRAPhysOld || CSR_XMTRL(pThis) != pThis->cbTDRAOld)
        {
            if (pThis->TDRAPhysOld != 0)
                PGMHandlerPhysicalDeregister(PDMDevHlpGetVM(pDevIns),
                                             pThis->TDRAPhysOld & ~PAGE_OFFSET_MASK);

            rc = PGMR3HandlerPhysicalRegister(PDMDevHlpGetVM(pDevIns),
                                              PGMPHYSHANDLERTYPE_PHYSICAL_WRITE,
                                              pThis->GCTDRA & ~PAGE_OFFSET_MASK,
                                              RT_ALIGN(pcnetTdraAddr(pThis, 0), PAGE_SIZE) - 1,
                                              pcnetHandleRingWrite, pDevIns,
                                              g_DevicePCNet.szR0Mod, "pcnetHandleRingWrite",
                                              pThis->pDevInsHC->pvInstanceDataHC,
                                              g_DevicePCNet.szRCMod, "pcnetHandleRingWrite",
                                              pThis->pDevInsHC->pvInstanceDataRC,
                                              "PCNet transmit ring write access handler");
            AssertRC(rc);

            pThis->TDRAPhysOld = pThis->GCTDRA;
            pThis->cbTDRAOld   = pcnetTdraAddr(pThis, 0);
        }
#ifdef PCNET_MONITOR_RECEIVE_RING
    }
    else
    if (    RDRAPageStart != TDRAPageStart
        &&  (   TDRAPageStart == RDRAPageEnd
             || TDRAPageEnd   == RDRAPageStart
            )
        )
    {
        /* 3) */
        AssertFailed();
    }
    /* else 2) */
#endif
}
#endif /* PCNET_NO_POLLING */

static void pcnetInit(PCNetState *pThis)
{
    PPDMDEVINS pDevIns = PCNETSTATE_2_DEVINS(pThis);
    Log(("#%d pcnetInit: init_addr=%#010x\n", PCNET_INST_NR, PHYSADDR(pThis, CSR_IADR(pThis))));

    /** @todo Documentation says that RCVRL and XMTRL are stored as two's complement!
     *        Software is allowed to write these registers directly. */
#define PCNET_INIT() do { \
        PDMDevHlpPhysRead(pDevIns, PHYSADDR(pThis, CSR_IADR(pThis)),         \
                          (uint8_t *)&initblk, sizeof(initblk));             \
        pThis->aCSR[15]  = RT_LE2H_U16(initblk.mode);                        \
        CSR_RCVRL(pThis) = (initblk.rlen < 9) ? (1 << initblk.rlen) : 512;   \
        CSR_XMTRL(pThis) = (initblk.tlen < 9) ? (1 << initblk.tlen) : 512;   \
        pThis->aCSR[ 6]  = (initblk.tlen << 12) | (initblk.rlen << 8);       \
        pThis->aCSR[ 8]  = RT_LE2H_U16(initblk.ladrf1);                      \
        pThis->aCSR[ 9]  = RT_LE2H_U16(initblk.ladrf2);                      \
        pThis->aCSR[10]  = RT_LE2H_U16(initblk.ladrf3);                      \
        pThis->aCSR[11]  = RT_LE2H_U16(initblk.ladrf4);                      \
        pThis->aCSR[12]  = RT_LE2H_U16(initblk.padr1);                       \
        pThis->aCSR[13]  = RT_LE2H_U16(initblk.padr2);                       \
        pThis->aCSR[14]  = RT_LE2H_U16(initblk.padr3);                       \
        pThis->GCRDRA    = PHYSADDR(pThis, initblk.rdra);                    \
        pThis->GCTDRA    = PHYSADDR(pThis, initblk.tdra);                    \
} while (0)

    pcnetEnablePrivateIf(pThis);

    if (BCR_SSIZE32(pThis))
    {
        struct INITBLK32 initblk;
        pThis->GCUpperPhys = 0;
        PCNET_INIT();
        Log(("#%d initblk.rlen=%#04x, initblk.tlen=%#04x\n",
             PCNET_INST_NR, initblk.rlen, initblk.tlen));
    }
    else
    {
        struct INITBLK16 initblk;
        pThis->GCUpperPhys = (0xff00 & (uint32_t)pThis->aCSR[2]) << 16;
        PCNET_INIT();
        Log(("#%d initblk.rlen=%#04x, initblk.tlen=%#04x\n",
             PCNET_INST_NR, initblk.rlen, initblk.tlen));
    }

#undef PCNET_INIT

    size_t cbRxBuffers = 0;
    for (int i = CSR_RCVRL(pThis); i >= 1; i--)
    {
        RMD rmd;
        RTGCPHYS32 rdaddr = PHYSADDR(pThis, pcnetRdraAddr(pThis, i));

        pcnetDescTouch(pThis, rdaddr);
        /* At this time it is not guaranteed that the buffers are already initialized. */
        if (pcnetRmdLoad(pThis, &rmd, rdaddr, false))
        {
            uint32_t cbBuf = 4096U-rmd.rmd1.bcnt;
            cbRxBuffers += cbBuf;
        }
    }

    for (int i = CSR_XMTRL(pThis); i >= 1; i--)
    {
        RTGCPHYS32 tdaddr = PHYSADDR(pThis, pcnetTdraAddr(pThis, i));

        pcnetDescTouch(pThis, tdaddr);
    }

    /*
     * Heuristics: The Solaris pcn driver allocates too few RX buffers (128 buffers of a
     * size of 128 bytes are 16KB in summary) leading to frequent RX buffer overflows. In
     * that case we don't signal RX overflows through the CSR0_MISS flag as the driver
     * re-initializes the device on every miss. Other guests use at least 32 buffers of
     * usually 1536 bytes and should therefore not run into condition. If they are still
     * short in RX buffers we notify this condition.
     */
    pThis->fSignalRxMiss = (cbRxBuffers == 0 || cbRxBuffers >= 32*_1K);

    if (pThis->pDrv)
        pThis->pDrv->pfnSetPromiscuousMode(pThis->pDrv, CSR_PROM(pThis));

    CSR_RCVRC(pThis) = CSR_RCVRL(pThis);
    CSR_XMTRC(pThis) = CSR_XMTRL(pThis);

#ifdef PCNET_NO_POLLING
    pcnetUpdateRingHandlers(pThis);
#endif

    /* Reset cached RX and TX states */
    CSR_CRST(pThis) = CSR_CRBC(pThis) = CSR_NRST(pThis) = CSR_NRBC(pThis) = 0;
    CSR_CXST(pThis) = CSR_CXBC(pThis) = CSR_NXST(pThis) = CSR_NXBC(pThis) = 0;

    LogRel(("PCNet#%d: Init: ss32=%d GCRDRA=%#010x[%d] GCTDRA=%#010x[%d]%s\n",
            PCNET_INST_NR, BCR_SSIZE32(pThis),
            pThis->GCRDRA, CSR_RCVRL(pThis), pThis->GCTDRA, CSR_XMTRL(pThis),
            !pThis->fSignalRxMiss ? " (CSR0_MISS disabled)" : ""));

    pThis->aCSR[0] |=  0x0101;       /* Initialization done */
    pThis->aCSR[0] &= ~0x0004;       /* clear STOP bit */
}
#endif /* IN_RING3 */

/**
 * Start RX/TX operation.
 */
static void pcnetStart(PCNetState *pThis)
{
    Log(("#%d pcnetStart:\n", PCNET_INST_NR));
    if (!CSR_DTX(pThis))
        pThis->aCSR[0] |= 0x0010;    /* set TXON */
    if (!CSR_DRX(pThis))
        pThis->aCSR[0] |= 0x0020;    /* set RXON */
    pcnetEnablePrivateIf(pThis);
    pThis->aCSR[0] &= ~0x0004;       /* clear STOP bit */
    pThis->aCSR[0] |=  0x0002;       /* STRT */
    pcnetPollTimerStart(pThis);      /* start timer if it was stopped */
}

/**
 * Stop RX/TX operation.
 */
static void pcnetStop(PCNetState *pThis)
{
    Log(("#%d pcnetStop:\n", PCNET_INST_NR));
    pThis->aCSR[0] &= ~0x7feb;
    pThis->aCSR[0] |=  0x0014;
    pThis->aCSR[4] &= ~0x02c2;
    pThis->aCSR[5] &= ~0x0011;
    pcnetEnablePrivateIf(pThis);
    pcnetPollTimer(pThis);
}

#ifdef IN_RING3
static DECLCALLBACK(void) pcnetWakeupReceive(PPDMDEVINS pDevIns)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    STAM_COUNTER_INC(&pThis->StatRxOverflowWakeup);
    if (pThis->hEventOutOfRxSpace != NIL_RTSEMEVENT)
        RTSemEventSignal(pThis->hEventOutOfRxSpace);
}

static DECLCALLBACK(bool) pcnetCanRxQueueConsumer(PPDMDEVINS pDevIns, PPDMQUEUEITEMCORE pItem)
{
    pcnetWakeupReceive(pDevIns);
    return true;
}
#endif /* IN_RING3 */


/**
 * Poll Receive Descriptor Table Entry and cache the results in the appropriate registers.
 * Note: Once a descriptor belongs to the network card (this driver), it cannot be changed
 * by the host (the guest driver) anymore. Well, it could but the results are undefined by
 * definition.
 * @param  fSkipCurrent       if true, don't scan the current RDTE.
 */
static void pcnetRdtePoll(PCNetState *pThis, bool fSkipCurrent=false)
{
    STAM_PROFILE_ADV_START(&pThis->CTXSUFF(StatRdtePoll), a);
    /* assume lack of a next receive descriptor */
    CSR_NRST(pThis) = 0;

    if (RT_LIKELY(pThis->GCRDRA))
    {
        /*
         * The current receive message descriptor.
         */
        RMD        rmd;
        int        i = CSR_RCVRC(pThis);
        RTGCPHYS32 addr;

        if (i < 1)
            i = CSR_RCVRL(pThis);

        if (!fSkipCurrent)
        {
            addr = pcnetRdraAddr(pThis, i);
            CSR_CRDA(pThis) = CSR_CRBA(pThis) = 0;
            CSR_CRBC(pThis) = CSR_CRST(pThis) = 0;
            if (!pcnetRmdLoad(pThis, &rmd, PHYSADDR(pThis, addr), true))
            {
                STAM_PROFILE_ADV_STOP(&pThis->CTXSUFF(StatRdtePoll), a);
                return;
            }
            if (RT_LIKELY(!IS_RMD_BAD(rmd)))
            {
                CSR_CRDA(pThis) = addr;                        /* Receive Descriptor Address */
                CSR_CRBA(pThis) = rmd.rmd0.rbadr;              /* Receive Buffer Address */
                CSR_CRBC(pThis) = rmd.rmd1.bcnt;               /* Receive Byte Count */
                CSR_CRST(pThis) = ((uint32_t *)&rmd)[1] >> 16; /* Receive Status */
                if (pThis->fMaybeOutOfSpace)
                {
#ifdef IN_RING3
                    pcnetWakeupReceive(PCNETSTATE_2_DEVINS(pThis));
#else
                    PPDMQUEUEITEMCORE pItem = PDMQueueAlloc(pThis->CTX_SUFF(pCanRxQueue));
                    if (pItem)
                        PDMQueueInsert(pThis->CTX_SUFF(pCanRxQueue), pItem);
#endif
                }
            }
            else
            {
                STAM_PROFILE_ADV_STOP(&pThis->CTXSUFF(StatRdtePoll), a);
                /* This is not problematic since we don't own the descriptor
                 * We actually do own it, otherwise pcnetRmdLoad would have returned false.
                 * Don't flood the release log with errors.
                 */
                if (++pThis->uCntBadRMD < 50)
                    LogRel(("PCNet#%d: BAD RMD ENTRIES AT %#010x (i=%d)\n",
                            PCNET_INST_NR, addr, i));
                return;
            }
        }

        /*
         * The next descriptor.
         */
        if (--i < 1)
            i = CSR_RCVRL(pThis);
        addr = pcnetRdraAddr(pThis, i);
        CSR_NRDA(pThis) = CSR_NRBA(pThis) = 0;
        CSR_NRBC(pThis) = 0;
        if (!pcnetRmdLoad(pThis, &rmd, PHYSADDR(pThis, addr), true))
        {
            STAM_PROFILE_ADV_STOP(&pThis->CTXSUFF(StatRdtePoll), a);
            return;
        }
        if (RT_LIKELY(!IS_RMD_BAD(rmd)))
        {
            CSR_NRDA(pThis) = addr;                         /* Receive Descriptor Address */
            CSR_NRBA(pThis) = rmd.rmd0.rbadr;               /* Receive Buffer Address */
            CSR_NRBC(pThis) = rmd.rmd1.bcnt;                /* Receive Byte Count */
            CSR_NRST(pThis) = ((uint32_t *)&rmd)[1] >> 16;  /* Receive Status */
        }
        else
        {
            STAM_PROFILE_ADV_STOP(&pThis->CTXSUFF(StatRdtePoll), a);
            /* This is not problematic since we don't own the descriptor
             * We actually do own it, otherwise pcnetRmdLoad would have returned false.
             * Don't flood the release log with errors.
             */
            if (++pThis->uCntBadRMD < 50)
                LogRel(("PCNet#%d: BAD RMD ENTRIES + AT %#010x (i=%d)\n",
                        PCNET_INST_NR, addr, i));
            return;
        }

        /**
         * @todo NNRD
         */
    }
    else
    {
        CSR_CRDA(pThis) = CSR_CRBA(pThis) = CSR_NRDA(pThis) = CSR_NRBA(pThis) = 0;
        CSR_CRBC(pThis) = CSR_NRBC(pThis) = CSR_CRST(pThis) = 0;
    }
    STAM_PROFILE_ADV_STOP(&pThis->CTXSUFF(StatRdtePoll), a);
}

/**
 * Poll Transmit Descriptor Table Entry
 * @return true if transmit descriptors available
 */
static int pcnetTdtePoll(PCNetState *pThis, TMD *tmd)
{
    STAM_PROFILE_ADV_START(&pThis->CTXSUFF(StatTdtePoll), a);
    if (RT_LIKELY(pThis->GCTDRA))
    {
        RTGCPHYS32 cxda = pcnetTdraAddr(pThis, CSR_XMTRC(pThis));

        if (!pcnetTmdLoad(pThis, tmd, PHYSADDR(pThis, cxda), true))
        {
            STAM_PROFILE_ADV_STOP(&pThis->CTXSUFF(StatTdtePoll), a);
            return 0;
        }

        if (RT_UNLIKELY(tmd->tmd1.ones != 15))
        {
            STAM_PROFILE_ADV_STOP(&pThis->CTXSUFF(StatTdtePoll), a);
            LogRel(("PCNet#%d: BAD TMD XDA=%#010x\n",
                    PCNET_INST_NR, PHYSADDR(pThis, cxda)));
            return 0;
        }

        /* previous xmit descriptor */
        CSR_PXDA(pThis) = CSR_CXDA(pThis);
        CSR_PXBC(pThis) = CSR_CXBC(pThis);
        CSR_PXST(pThis) = CSR_CXST(pThis);

        /* set current trasmit decriptor. */
        CSR_CXDA(pThis) = cxda;
        CSR_CXBC(pThis) = tmd->tmd1.bcnt;
        CSR_CXST(pThis) = ((uint32_t *)tmd)[1] >> 16;
        STAM_PROFILE_ADV_STOP(&pThis->CTXSUFF(StatTdtePoll), a);
        return CARD_IS_OWNER(CSR_CXST(pThis));
    }
    else
    {
        /** @todo consistency with previous receive descriptor */
        CSR_CXDA(pThis) = 0;
        CSR_CXBC(pThis) = CSR_CXST(pThis) = 0;
        STAM_PROFILE_ADV_STOP(&pThis->CTXSUFF(StatTdtePoll), a);
        return 0;
    }
}


#ifdef IN_RING3

/**
 * Write data into guest receive buffers.
 */
static void pcnetReceiveNoSync(PCNetState *pThis, const uint8_t *buf, size_t cbToRecv)
{
    PPDMDEVINS pDevIns = PCNETSTATE_2_DEVINS(pThis);
    int is_padr = 0, is_bcast = 0, is_ladr = 0;
    unsigned iRxDesc;
    int cbPacket;

    if (RT_UNLIKELY(CSR_DRX(pThis) || CSR_STOP(pThis) || CSR_SPND(pThis) || !cbToRecv))
        return;

    /*
     * Drop packets if the VM is not running yet/anymore.
     */
    VMSTATE enmVMState = PDMDevHlpVMState(pDevIns);
    if (    enmVMState != VMSTATE_RUNNING
        &&  enmVMState != VMSTATE_RUNNING_LS)
        return;

    Log(("#%d pcnetReceiveNoSync: size=%d\n", PCNET_INST_NR, cbToRecv));

    /*
     * Perform address matching.
     */
    if (   CSR_PROM(pThis)
        || (is_padr  = padr_match(pThis, buf, cbToRecv))
        || (is_bcast = padr_bcast(pThis, buf, cbToRecv))
        || (is_ladr  = ladr_match(pThis, buf, cbToRecv)))
    {
        if (HOST_IS_OWNER(CSR_CRST(pThis)))
            pcnetRdtePoll(pThis);
        if (RT_UNLIKELY(HOST_IS_OWNER(CSR_CRST(pThis))))
        {
            /* Not owned by controller. This should not be possible as
             * we already called pcnetCanReceive(). */
            LogRel(("PCNet#%d: no buffer: RCVRC=%d\n",
                    PCNET_INST_NR, CSR_RCVRC(pThis)));
            /* Dump the status of all RX descriptors */
            const unsigned  cb = 1 << pThis->iLog2DescSize;
            RTGCPHYS32      GCPhys = pThis->GCRDRA;
            iRxDesc = CSR_RCVRL(pThis);
            while (iRxDesc-- > 0)
            {
                RMD rmd;
                pcnetRmdLoad(pThis, &rmd, PHYSADDR(pThis, GCPhys), false);
                LogRel(("  %#010x\n", rmd.rmd1));
                GCPhys += cb;
            }
            pThis->aCSR[0] |= 0x1000; /* Set MISS flag */
            CSR_MISSC(pThis)++;
        }
        else
        {
            uint8_t   *src = &pThis->abRecvBuf[8];
            RTGCPHYS32 crda = CSR_CRDA(pThis);
            RTGCPHYS32 next_crda;
            RMD        rmd, next_rmd;

            memcpy(src, buf, cbToRecv);
            if (!CSR_ASTRP_RCV(pThis))
            {
                uint32_t fcs = ~0;
                uint8_t *p = src;

                while (cbToRecv < 60)
                    src[cbToRecv++] = 0;
                while (p != &src[cbToRecv])
                    CRC(fcs, *p++);
                ((uint32_t *)&src[cbToRecv])[0] = htonl(fcs);
                /* FCS at end of packet */
            }
            cbToRecv += 4;
            cbPacket = (int)cbToRecv;                           Assert((size_t)cbPacket == cbToRecv);

#ifdef PCNET_DEBUG_MATCH
            PRINT_PKTHDR(buf);
#endif

            pcnetRmdLoad(pThis, &rmd, PHYSADDR(pThis, crda), false);
            /*if (!CSR_LAPPEN(pThis))*/
                rmd.rmd1.stp = 1;

            size_t cbBuf = RT_MIN(4096 - (size_t)rmd.rmd1.bcnt, cbToRecv);
            RTGCPHYS32 rbadr = PHYSADDR(pThis, rmd.rmd0.rbadr);

            /* save the old value to check if it was changed as long as we didn't
             * hold the critical section */
            iRxDesc = CSR_RCVRC(pThis);

            /* We have to leave the critical section here or we risk deadlocking
             * with EMT when the write is to an unallocated page or has an access
             * handler associated with it.
             *
             * This shouldn't be a problem because:
             *  - any modification to the RX descriptor by the driver is
             *    forbidden as long as it is owned by the device
             *  - we don't cache any register state beyond this point
             */
            PDMCritSectLeave(&pThis->CritSect);
            PDMDevHlpPhysWrite(pDevIns, rbadr, src, cbBuf);
            int rc = PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);
            AssertReleaseRC(rc);

            /* RX disabled in the meantime? If so, abort RX. */
            if (RT_UNLIKELY(CSR_DRX(pThis) || CSR_STOP(pThis) || CSR_SPND(pThis)))
                return;

            /* Was the register modified in the meantime? If so, don't touch the
             * register but still update the RX descriptor. */
            if (RT_LIKELY(iRxDesc == CSR_RCVRC(pThis)))
            {
                if (iRxDesc-- < 2)
                    iRxDesc = CSR_RCVRL(pThis);
                CSR_RCVRC(pThis) = iRxDesc;
            }
            else
                iRxDesc = CSR_RCVRC(pThis);

            src      += cbBuf;
            cbToRecv -= cbBuf;

            while (cbToRecv > 0)
            {
                /* Read the entire next descriptor as we're likely to need it. */
                next_crda = pcnetRdraAddr(pThis, iRxDesc);

                /* Check next descriptor's own bit. If we don't own it, we have
                 * to quit and write error status into the last descriptor we own.
                 */
                if (!pcnetRmdLoad(pThis, &next_rmd, PHYSADDR(pThis, next_crda), true))
                    break;

                /* Write back current descriptor, clear the own bit. */
                pcnetRmdStorePassHost(pThis, &rmd, PHYSADDR(pThis, crda));

                /* Switch to the next descriptor */
                crda = next_crda;
                rmd  = next_rmd;

                cbBuf = RT_MIN(4096 - (size_t)rmd.rmd1.bcnt, cbToRecv);
                RTGCPHYS32 rbadr = PHYSADDR(pThis, rmd.rmd0.rbadr);

                /* We have to leave the critical section here or we risk deadlocking
                 * with EMT when the write is to an unallocated page or has an access
                 * handler associated with it. See above for additional comments. */
                PDMCritSectLeave(&pThis->CritSect);
                PDMDevHlpPhysWrite(pDevIns, rbadr, src, cbBuf);
                rc = PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);
                AssertReleaseRC(rc);

                /* RX disabled in the meantime? If so, abort RX. */
                if (RT_UNLIKELY(CSR_DRX(pThis) || CSR_STOP(pThis) || CSR_SPND(pThis)))
                    return;

                /* Was the register modified in the meantime? If so, don't touch the
                 * register but still update the RX descriptor. */
                if (RT_LIKELY(iRxDesc == CSR_RCVRC(pThis)))
                {
                    if (iRxDesc-- < 2)
                        iRxDesc = CSR_RCVRL(pThis);
                    CSR_RCVRC(pThis) = iRxDesc;
                }
                else
                    iRxDesc = CSR_RCVRC(pThis);

                src      += cbBuf;
                cbToRecv -= cbBuf;
            }

            if (RT_LIKELY(cbToRecv == 0))
            {
                rmd.rmd1.enp  = 1;
                rmd.rmd1.pam  = !CSR_PROM(pThis) && is_padr;
                rmd.rmd1.lafm = !CSR_PROM(pThis) && is_ladr;
                rmd.rmd1.bam  = !CSR_PROM(pThis) && is_bcast;
                rmd.rmd2.mcnt = cbPacket;

                STAM_REL_COUNTER_ADD(&pThis->StatReceiveBytes, cbPacket);
            }
            else
            {
                Log(("#%d: Overflow by %ubytes\n", PCNET_INST_NR, cbToRecv));
                rmd.rmd1.oflo = 1;
                rmd.rmd1.buff = 1;
                rmd.rmd1.err  = 1;
            }

            /* write back, clear the own bit */
            pcnetRmdStorePassHost(pThis, &rmd, PHYSADDR(pThis, crda));

            pThis->aCSR[0] |= 0x0400;

            Log(("#%d RCVRC=%d CRDA=%#010x\n", PCNET_INST_NR,
                 CSR_RCVRC(pThis), PHYSADDR(pThis, CSR_CRDA(pThis))));
#ifdef PCNET_DEBUG_RMD
            PRINT_RMD(&rmd);
#endif

            /* guest driver is owner: force repoll of current and next RDTEs */
            CSR_CRST(pThis) = 0;
        }
    }

    /* see description of TXDPOLL:
     * ``transmit polling will take place following receive activities'' */
    pcnetPollRxTx(pThis);
    pcnetUpdateIrq(pThis);
}


/**
 * Checks if the link is up.
 * @returns true if the link is up.
 * @returns false if the link is down.
 */
DECLINLINE(bool) pcnetIsLinkUp(PCNetState *pThis)
{
    return pThis->pDrv && !pThis->fLinkTempDown && pThis->fLinkUp;
}


/**
 * Transmit queue consumer
 * This is just a very simple way of delaying sending to R3.
 *
 * @returns Success indicator.
 *          If false the item will not be removed and the flushing will stop.
 * @param   pDevIns     The device instance.
 * @param   pItem       The item to consume. Upon return this item will be freed.
 */
static DECLCALLBACK(bool) pcnetXmitQueueConsumer(PPDMDEVINS pDevIns, PPDMQUEUEITEMCORE pItem)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    NOREF(pItem);

    /* Clear counter .*/
    ASMAtomicAndU32(&pThis->cPendingSends, 0);
#ifdef PCNET_QUEUE_SEND_PACKETS
    int rc = PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);
    AssertReleaseRC(rc);
    pcnetSyncTransmit(pThis);
    PDMCritSectLeave(&pThis->CritSect);
#else
    int rc = RTSemEventSignal(pThis->hSendEventSem);
    AssertRC(rc);
#endif
    return true;
}


/**
 * Scraps the top frame.
 * This is done as a precaution against mess left over by on
 */
DECLINLINE(void) pcnetXmitScrapFrame(PCNetState *pThis)
{
    pThis->pvSendFrame = NULL;
    pThis->cbSendFrame = 0;
}


/**
 * Reads the first part of a frame
 */
DECLINLINE(void) pcnetXmitRead1st(PCNetState *pThis, RTGCPHYS32 GCPhysFrame, const unsigned cbFrame)
{
    Assert(PDMCritSectIsOwner(&pThis->CritSect));
    Assert(cbFrame < sizeof(pThis->abSendBuf));

#ifdef PCNET_QUEUE_SEND_PACKETS
    AssertRelease(pThis->cXmitRingBufPending < PCNET_MAX_XMIT_SLOTS-1);
    pThis->pvSendFrame = pThis->apXmitRingBuffer[pThis->iXmitRingBufProd];
#else
    pThis->pvSendFrame = pThis->abSendBuf;
#endif
    PDMDevHlpPhysRead(pThis->CTX_SUFF(pDevIns), GCPhysFrame, pThis->pvSendFrame, cbFrame);
    pThis->cbSendFrame = cbFrame;
}


/**
 * Reads more into the current frame.
 */
DECLINLINE(void) pcnetXmitReadMore(PCNetState *pThis, RTGCPHYS32 GCPhysFrame, const unsigned cbFrame)
{
    Assert(pThis->cbSendFrame + cbFrame <= MAX_FRAME);
    PDMDevHlpPhysRead(pThis->CTX_SUFF(pDevIns), GCPhysFrame, pThis->pvSendFrame + pThis->cbSendFrame, cbFrame);
    pThis->cbSendFrame += cbFrame;
}


/**
 * Completes the current frame.
 * If we've reached the maxium number of frames, they will be flushed.
 */
DECLINLINE(int) pcnetXmitCompleteFrame(PCNetState *pThis)
{
#ifdef PCNET_QUEUE_SEND_PACKETS
    Assert(PDMCritSectIsOwner(&pThis->CritSect));
    AssertRelease(pThis->cXmitRingBufPending < PCNET_MAX_XMIT_SLOTS-1);
    Assert(!pThis->cbXmitRingBuffer[pThis->iXmitRingBufProd]);

    pThis->cbXmitRingBuffer[pThis->iXmitRingBufProd] = (uint16_t)pThis->cbSendFrame;
    pThis->iXmitRingBufProd = (pThis->iXmitRingBufProd+1) & PCNET_MAX_XMIT_SLOTS_MASK;
    ASMAtomicIncS32(&pThis->cXmitRingBufPending);

    int rc = RTSemEventSignal(pThis->hSendEventSem);
    AssertRC(rc);

    return VINF_SUCCESS;
#else
    /* Don't hold the critical section while transmitting data. */
    /** @note also avoids deadlocks with NAT as it can call us right back. */
    PDMCritSectLeave(&pThis->CritSect);

    STAM_PROFILE_ADV_START(&pThis->StatTransmitSend, a);
    if (pThis->cbSendFrame > 70) /* unqualified guess */
        pThis->Led.Asserted.s.fWriting = pThis->Led.Actual.s.fWriting = 1;

    pThis->pDrv->pfnSend(pThis->pDrv, pThis->pvSendFrame, pThis->cbSendFrame);
    STAM_REL_COUNTER_ADD(&pThis->StatTransmitBytes, pThis->cbSendFrame);
    pThis->Led.Actual.s.fWriting = 0;
    STAM_PROFILE_ADV_STOP(&pThis->StatTransmitSend, a);

    return PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);
#endif
}


/**
 * Fails a TMD with a link down error.
 */
static void pcnetXmitFailTMDLinkDown(PCNetState *pThis, TMD *pTmd)
{
    /* make carrier error - hope this is correct. */
    pThis->cLinkDownReported++;
    pTmd->tmd2.lcar = pTmd->tmd1.err = 1;
    pThis->aCSR[0] |= RT_BIT(15) | RT_BIT(13); /* ERR | CERR */
    pThis->Led.Asserted.s.fError = pThis->Led.Actual.s.fError = 1;
    Log(("#%d pcnetTransmit: Signaling send error. swstyle=%#x\n",
         PCNET_INST_NR, pThis->aBCR[BCR_SWS]));
}

/**
 * Fails a TMD with a generic error.
 */
static void pcnetXmitFailTMDGeneric(PCNetState *pThis, TMD *pTmd)
{
    /* make carrier error - hope this is correct. */
    pTmd->tmd2.lcar = pTmd->tmd1.err = 1;
    pThis->aCSR[0] |= RT_BIT(15) | RT_BIT(13); /* ERR | CERR */
    pThis->Led.Asserted.s.fError = pThis->Led.Actual.s.fError = 1;
    Log(("#%d pcnetTransmit: Signaling send error. swstyle=%#x\n",
         PCNET_INST_NR, pThis->aBCR[BCR_SWS]));
}


/**
 * Transmit a loopback frame.
 */
DECLINLINE(void) pcnetXmitLoopbackFrame(PCNetState *pThis)
{
    pThis->Led.Asserted.s.fReading = pThis->Led.Actual.s.fReading = 1;
    if (HOST_IS_OWNER(CSR_CRST(pThis)))
        pcnetRdtePoll(pThis);

    Assert(pThis->pvSendFrame);
    pcnetReceiveNoSync(pThis, (const uint8_t *)pThis->pvSendFrame, pThis->cbSendFrame);
    pcnetXmitScrapFrame(pThis);
    pThis->Led.Actual.s.fReading = 0;
}

/**
 * Flushes queued frames.
 */
DECLINLINE(void) pcnetXmitFlushFrames(PCNetState *pThis)
{
    pcnetXmitQueueConsumer(pThis->CTX_SUFF(pDevIns), NULL);
}

#endif /* IN_RING3 */



/**
 * Try to transmit frames
 */
static void pcnetTransmit(PCNetState *pThis)
{
    if (RT_UNLIKELY(!CSR_TXON(pThis)))
    {
        pThis->aCSR[0] &= ~0x0008; /* Clear TDMD */
        return;
    }

    /*
     * Check the current transmit descriptors.
     */
    TMD tmd;
    if (!pcnetTdtePoll(pThis, &tmd))
        return;

    /*
     * Clear TDMD.
     */
    pThis->aCSR[0] &= ~0x0008;

    /*
     * If we're in Ring-3 we should flush the queue now, in GC/R0 we'll queue a flush job.
     */
#ifdef IN_RING3
    pcnetXmitFlushFrames(pThis);
#else
# if 1
    PPDMQUEUEITEMCORE pItem = PDMQueueAlloc(pThis->CTX_SUFF(pXmitQueue));
    if (RT_UNLIKELY(pItem))
        PDMQueueInsert(pThis->CTX_SUFF(pXmitQueue), pItem);
# else
    if (ASMAtomicIncU32(&pThis->cPendingSends) < 16)
    {
        PPDMQUEUEITEMCORE pItem = PDMQueueAlloc(pThis->CTX_SUFF(pXmitQueue));
        if (RT_UNLIKELY(pItem))
            PDMQueueInsert(pThis->CTX_SUFF(pXmitQueue), pItem);
    }
    else
        PDMQueueFlush(pThis->CTX_SUFF(pXmitQueue));
# endif
#endif
}

#ifdef IN_RING3

/**
 * Try to transmit frames
 */
#ifdef PCNET_QUEUE_SEND_PACKETS
static int pcnetAsyncTransmit(PCNetState *pThis)
{
    Assert(PDMCritSectIsOwner(&pThis->CritSect));
    size_t cb;

    while ((pThis->cXmitRingBufPending > 0))
    {
        cb = pThis->cbXmitRingBuffer[pThis->iXmitRingBufCons];

        /* Don't hold the critical section while transmitting data. */
        /** @note also avoids deadlocks with NAT as it can call us right back. */
        PDMCritSectLeave(&pThis->CritSect);

        STAM_PROFILE_ADV_START(&pThis->StatTransmitSend, a);
        if (cb > 70) /* unqualified guess */
            pThis->Led.Asserted.s.fWriting = pThis->Led.Actual.s.fWriting = 1;

        pThis->pDrv->pfnSend(pThis->pDrv, pThis->apXmitRingBuffer[pThis->iXmitRingBufCons], cb);
        STAM_REL_COUNTER_ADD(&pThis->StatTransmitBytes, cb);
        pThis->Led.Actual.s.fWriting = 0;
        STAM_PROFILE_ADV_STOP(&pThis->StatTransmitSend, a);

        int rc = PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);
        AssertReleaseRC(rc);

        pThis->cbXmitRingBuffer[pThis->iXmitRingBufCons] = 0;
        pThis->iXmitRingBufCons = (pThis->iXmitRingBufCons+1) & PCNET_MAX_XMIT_SLOTS_MASK;
        ASMAtomicDecS32(&pThis->cXmitRingBufPending);
    }
    return VINF_SUCCESS;
}

static int pcnetSyncTransmit(PCNetState *pThis)
#else
static int pcnetAsyncTransmit(PCNetState *pThis)
#endif
{
    unsigned cFlushIrq = 0;

    Assert(PDMCritSectIsOwner(&pThis->CritSect));

    if (RT_UNLIKELY(!CSR_TXON(pThis)))
    {
        pThis->aCSR[0] &= ~0x0008; /* Clear TDMD */
        return VINF_SUCCESS;
    }

    /*
     * Iterate the transmit descriptors.
     */
    STAM_PROFILE_ADV_START(&pThis->StatTransmit, a);
    do
    {
#ifdef VBOX_WITH_STATISTICS
        unsigned cBuffers = 1;
#endif
        TMD tmd;
        if (!pcnetTdtePoll(pThis, &tmd))
            break;

        /* Don't continue sending packets when the link is down. */
        if (RT_UNLIKELY(   !pcnetIsLinkUp(pThis)
                        &&  pThis->cLinkDownReported > PCNET_MAX_LINKDOWN_REPORTED)
            )
            break;

#ifdef PCNET_DEBUG_TMD
        Log2(("#%d TMDLOAD %#010x\n", PCNET_INST_NR, PHYSADDR(pThis, CSR_CXDA(pThis))));
        PRINT_TMD(&tmd);
#endif
        pcnetXmitScrapFrame(pThis);

        /*
         * The typical case - a complete packet.
         */
        if (tmd.tmd1.stp && tmd.tmd1.enp)
        {
            const unsigned cb = 4096 - tmd.tmd1.bcnt;
            Log(("#%d pcnetTransmit: stp&enp: cb=%d xmtrc=%#x\n", PCNET_INST_NR, cb, CSR_XMTRC(pThis)));

            if (RT_LIKELY(pcnetIsLinkUp(pThis) || CSR_LOOP(pThis)))
            {
                /* From the manual: ``A zero length buffer is acceptable as
                 * long as it is not the last buffer in a chain (STP = 0 and
                 * ENP = 1).'' That means that the first buffer might have a
                 * zero length if it is not the last one in the chain. */
                if (RT_LIKELY(cb <= MAX_FRAME))
                {
                    pcnetXmitRead1st(pThis, PHYSADDR(pThis, tmd.tmd0.tbadr), cb);
                    if (CSR_LOOP(pThis))
                        pcnetXmitLoopbackFrame(pThis);
                    else
                    {
                        int rc = pcnetXmitCompleteFrame(pThis);
                        AssertRCReturn(rc, rc);
                    }
                }
                else if (cb == 4096)
                {
                    /* The Windows NT4 pcnet driver sometimes marks the first
                     * unused descriptor as owned by us. Ignore that (by
                     * passing it back). Do not update the ring counter in this
                     * case (otherwise that driver becomes even more confused,
                     * which causes transmit to stall for about 10 seconds).
                     * This is just a workaround, not a final solution. */
                    /* r=frank: IMHO this is the correct implementation. The
                     * manual says: ``If the OWN bit is set and the buffer
                     * length is 0, the OWN bit will be cleared. In the C-LANCE
                     * the buffer length of 0 is interpreted as a 4096-byte
                     * buffer.'' */
                    LogRel(("PCNet#%d: pcnetAsyncTransmit: illegal 4kb frame -> ignoring\n", PCNET_INST_NR));
                    pcnetTmdStorePassHost(pThis, &tmd, PHYSADDR(pThis, CSR_CXDA(pThis)));
                    break;
                }
                else
                {
                    /* Signal error, as this violates the Ethernet specs. */
                    /** @todo check if the correct error is generated. */
                    LogRel(("PCNet#%d: pcnetAsyncTransmit: illegal 4kb frame -> signalling error\n", PCNET_INST_NR));

                    pcnetXmitFailTMDGeneric(pThis, &tmd);
                }
            }
            else
                pcnetXmitFailTMDLinkDown(pThis, &tmd);

            /* Write back the TMD and pass it to the host (clear own bit). */
            pcnetTmdStorePassHost(pThis, &tmd, PHYSADDR(pThis, CSR_CXDA(pThis)));

            /* advance the ring counter register */
            if (CSR_XMTRC(pThis) < 2)
                CSR_XMTRC(pThis) = CSR_XMTRL(pThis);
            else
                CSR_XMTRC(pThis)--;
        }
        else if (tmd.tmd1.stp)
        {
            /*
             * Read TMDs until end-of-packet or tdte poll fails (underflow).
             */
            bool fDropFrame = false;
            unsigned cb = 4096 - tmd.tmd1.bcnt;
            pcnetXmitRead1st(pThis, PHYSADDR(pThis, tmd.tmd0.tbadr), cb);
            for (;;)
            {
                /*
                 * Advance the ring counter register and check the next tmd.
                 */
#ifdef LOG_ENABLED
                const uint32_t iStart = CSR_XMTRC(pThis);
#endif
                const uint32_t GCPhysPrevTmd = PHYSADDR(pThis, CSR_CXDA(pThis));
                if (CSR_XMTRC(pThis) < 2)
                    CSR_XMTRC(pThis) = CSR_XMTRL(pThis);
                else
                    CSR_XMTRC(pThis)--;

                TMD dummy;
                if (!pcnetTdtePoll(pThis, &dummy))
                {
                    /*
                     * Underflow!
                     */
                    tmd.tmd2.buff = tmd.tmd2.uflo = tmd.tmd1.err = 1;
                    pThis->aCSR[0] |= 0x0200;        /* set TINT */
                    if (!CSR_DXSUFLO(pThis))         /* stop on xmit underflow */
                        pThis->aCSR[0] &= ~0x0010;   /* clear TXON */
                    pcnetTmdStorePassHost(pThis, &tmd, GCPhysPrevTmd);
                    AssertMsgFailed(("pcnetTransmit: Underflow!!!\n"));
                    break;
                }

                /* release & save the previous tmd, pass it to the host */
                pcnetTmdStorePassHost(pThis, &tmd, GCPhysPrevTmd);

                /*
                 * The next tdm.
                 */
#ifdef VBOX_WITH_STATISTICS
                cBuffers++;
#endif
                pcnetTmdLoad(pThis, &tmd, PHYSADDR(pThis, CSR_CXDA(pThis)), false);
                cb = 4096 - tmd.tmd1.bcnt;
                if (    pThis->cbSendFrame + cb < MAX_FRAME
                    &&  !fDropFrame)
                    pcnetXmitReadMore(pThis, PHYSADDR(pThis, tmd.tmd0.tbadr), cb);
                else
                {
                    AssertMsg(fDropFrame, ("pcnetTransmit: Frame is too big!!! %d bytes\n",
                                pThis->cbSendFrame + cb));
                    fDropFrame = true;
                }
                if (tmd.tmd1.enp)
                {
                    Log(("#%d pcnetTransmit: stp: cb=%d xmtrc=%#x-%#x\n", PCNET_INST_NR,
                                pThis->cbSendFrame, iStart, CSR_XMTRC(pThis)));
                    if (pcnetIsLinkUp(pThis) && !fDropFrame)
                    {
                        int rc = pcnetXmitCompleteFrame(pThis);
                        AssertRCReturn(rc, rc);
                    }
                    else if (CSR_LOOP(pThis) && !fDropFrame)
                        pcnetXmitLoopbackFrame(pThis);
                    else
                    {
                        if (!fDropFrame)
                            pcnetXmitFailTMDLinkDown(pThis, &tmd);
                        pcnetXmitScrapFrame(pThis);
                    }

                    /* Write back the TMD, pass it to the host */
                    pcnetTmdStorePassHost(pThis, &tmd, PHYSADDR(pThis, CSR_CXDA(pThis)));

                    /* advance the ring counter register */
                    if (CSR_XMTRC(pThis) < 2)
                        CSR_XMTRC(pThis) = CSR_XMTRL(pThis);
                    else
                        CSR_XMTRC(pThis)--;
                    break;
                }
            }
        }
        else
        {
            /*
             * We underflowed in a previous transfer, or the driver is giving us shit.
             * Simply stop the transmitting for now.
             */
            /** @todo according to the specs we're supposed to clear the own bit and move on to the next one. */
            Log(("#%d pcnetTransmit: guest is giving us shit!\n", PCNET_INST_NR));
            break;
        }
        /* Update TDMD, TXSTRT and TINT. */
        pThis->aCSR[0] &= ~0x0008;       /* clear TDMD */

        pThis->aCSR[4] |=  0x0008;       /* set TXSTRT */
        if (    !CSR_TOKINTD(pThis)      /* Transmit OK Interrupt Disable, no infl. on errors. */
            ||  (CSR_LTINTEN(pThis) && tmd.tmd1.ltint)
            ||  tmd.tmd1.err)
        {
            cFlushIrq++;
        }

        /** @todo should we continue after an error (tmd.tmd1.err) or not? */

        STAM_COUNTER_INC(&pThis->aStatXmitChainCounts[RT_MIN(cBuffers,
                                                      RT_ELEMENTS(pThis->aStatXmitChainCounts)) - 1]);
    } while (CSR_TXON(pThis));          /* transfer on */

    if (cFlushIrq)
    {
        STAM_COUNTER_INC(&pThis->aStatXmitFlush[RT_MIN(cFlushIrq, RT_ELEMENTS(pThis->aStatXmitFlush)) - 1]);
        pThis->aCSR[0] |= 0x0200;    /* set TINT */
        pcnetUpdateIrq(pThis);
    }

    STAM_PROFILE_ADV_STOP(&pThis->StatTransmit, a);

    return VINF_SUCCESS;
}


/**
 * Async I/O thread for delayed sending of packets.
 *
 * @returns VBox status code. Returning failure will naturally terminate the thread.
 * @param   pDevIns     The pcnet device instance.
 * @param   pThread     The thread.
 */
static DECLCALLBACK(int) pcnetAsyncSendThread(PPDMDEVINS pDevIns, PPDMTHREAD pThread)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);

    /*
     * We can enter this function in two states, initializing or resuming.
     *
     * The idea about the initializing bit is that we can do per-thread
     * initialization while the creator thread can still pick up errors.
     * At present, there is nothing to init, or at least nothing that
     * need initing in the thread.
     */
    if (pThread->enmState == PDMTHREADSTATE_INITIALIZING)
        return VINF_SUCCESS;

    /*
     * Stay in the run-loop until we're supposed to leave the
     * running state. If something really bad happens, we'll
     * quit the loop while in the running state and return
     * an error status to PDM and let it terminate the thread.
     */
    while (pThread->enmState == PDMTHREADSTATE_RUNNING)
    {
        /*
         * Block until we've got something to send or is supposed
         * to leave the running state.
         */
        int rc = RTSemEventWait(pThis->hSendEventSem, RT_INDEFINITE_WAIT);
        AssertRCReturn(rc, rc);
        if (RT_UNLIKELY(pThread->enmState != PDMTHREADSTATE_RUNNING))
            break;

        /*
         * Perform async send. Mind that we might be requested to
         * suspended while waiting for the critical section.
         */
        rc = PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);
        AssertReleaseRCReturn(rc, rc);

        if (pThread->enmState == PDMTHREADSTATE_RUNNING)
        {
            rc = pcnetAsyncTransmit(pThis);
            AssertReleaseRC(rc);
        }

        PDMCritSectLeave(&pThis->CritSect);
    }

    /* The thread is being suspended or terminated. */
    return VINF_SUCCESS;
}


/**
 * Unblock the send thread so it can respond to a state change.
 *
 * @returns VBox status code.
 * @param   pDevIns     The pcnet device instance.
 * @param   pThread     The send thread.
 */
static DECLCALLBACK(int) pcnetAsyncSendThreadWakeUp(PPDMDEVINS pDevIns, PPDMTHREAD pThread)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    return RTSemEventSignal(pThis->hSendEventSem);
}

#endif /* IN_RING3 */

/**
 * Poll for changes in RX and TX descriptor rings.
 */
static void pcnetPollRxTx(PCNetState *pThis)
{
    if (CSR_RXON(pThis))
    {
        /*
         * The second case is important for pcnetWaitReceiveAvail(): If CSR_CRST(pThis) was
         * true but pcnetCanReceive() returned false for some other reason we need to check
         * _now_ if we have to wakeup pcnetWaitReceiveAvail().
         */
        if (   HOST_IS_OWNER(CSR_CRST(pThis))  /* only poll RDTEs if none available or ... */
            || pThis->fMaybeOutOfSpace)        /* ... for waking up pcnetWaitReceiveAvail() */
            pcnetRdtePoll(pThis);
    }

    if (CSR_TDMD(pThis) || (CSR_TXON(pThis) && !CSR_DPOLL(pThis)))
        pcnetTransmit(pThis);
}


/**
 * Start the poller timer.
 * Poll timer interval is fixed to 500Hz. Don't stop it.
 * @thread EMT, TAP.
 */
static void pcnetPollTimerStart(PCNetState *pThis)
{
    TMTimerSetMillies(pThis->CTX_SUFF(pTimerPoll), 2);
}


/**
 * Update the poller timer.
 * @thread EMT.
 */
static void pcnetPollTimer(PCNetState *pThis)
{
    STAM_PROFILE_ADV_START(&pThis->StatPollTimer, a);

#ifdef LOG_ENABLED
    TMD dummy;
    if (CSR_STOP(pThis) || CSR_SPND(pThis))
        Log2(("#%d pcnetPollTimer time=%#010llx CSR_STOP=%d CSR_SPND=%d\n",
             PCNET_INST_NR, RTTimeMilliTS(), CSR_STOP(pThis), CSR_SPND(pThis)));
    else
        Log2(("#%d pcnetPollTimer time=%#010llx TDMD=%d TXON=%d POLL=%d TDTE=%d TDRA=%#x\n",
             PCNET_INST_NR, RTTimeMilliTS(), CSR_TDMD(pThis), CSR_TXON(pThis),
             !CSR_DPOLL(pThis), pcnetTdtePoll(pThis, &dummy), pThis->GCTDRA));
    Log2(("#%d pcnetPollTimer: CSR_CXDA=%#x CSR_XMTRL=%d CSR_XMTRC=%d\n",
          PCNET_INST_NR, CSR_CXDA(pThis), CSR_XMTRL(pThis), CSR_XMTRC(pThis)));
#endif
#ifdef PCNET_DEBUG_TMD
    if (CSR_CXDA(pThis))
    {
        TMD tmd;
        pcnetTmdLoad(pThis, &tmd, PHYSADDR(pThis, CSR_CXDA(pThis)), false);
        Log2(("#%d pcnetPollTimer: TMDLOAD %#010x\n", PCNET_INST_NR, PHYSADDR(pThis, CSR_CXDA(pThis))));
        PRINT_TMD(&tmd);
    }
#endif
    if (CSR_TDMD(pThis))
        pcnetTransmit(pThis);

    pcnetUpdateIrq(pThis);

    /* If the receive thread is waiting for new descriptors, poll TX/RX even if polling
     * disabled. We wouldn't need to poll for new TX descriptors in that case but it will
     * not hurt as waiting for RX descriptors should happen very seldom */
    if (RT_LIKELY(   !CSR_STOP(pThis)
                  && !CSR_SPND(pThis)
                  && (   !CSR_DPOLL(pThis)
                      || pThis->fMaybeOutOfSpace)))
    {
        /* We ensure that we poll at least every 2ms (500Hz) but not more often than
         * 5000 times per second. This way we completely prevent the overhead from
         * heavy reprogramming the timer which turned out to be very CPU-intensive.
         * The drawback is that csr46 and csr47 are not updated properly anymore
         * but so far I have not seen any guest depending on these values. The 2ms
         * interval is the default polling interval of the PCNet card (65536/33MHz). */
#ifdef PCNET_NO_POLLING
        pcnetPollRxTx(pThis);
#else
        uint64_t u64Now = TMTimerGet(pThis->CTX_SUFF(pTimerPoll));
        if (RT_UNLIKELY(u64Now - pThis->u64LastPoll > 200000))
        {
            pThis->u64LastPoll = u64Now;
            pcnetPollRxTx(pThis);
        }
        if (!TMTimerIsActive(pThis->CTX_SUFF(pTimerPoll)))
            pcnetPollTimerStart(pThis);
#endif
    }
    STAM_PROFILE_ADV_STOP(&pThis->StatPollTimer, a);
}


static int pcnetCSRWriteU16(PCNetState *pThis, uint32_t u32RAP, uint32_t val)
{
    int      rc  = VINF_SUCCESS;
#ifdef PCNET_DEBUG_CSR
    Log(("#%d pcnetCSRWriteU16: rap=%d val=%#06x\n", PCNET_INST_NR, u32RAP, val));
#endif
    switch (u32RAP)
    {
        case 0:
            {
                uint16_t csr0 = pThis->aCSR[0];
                /* Clear any interrupt flags.
                 * Don't clear an interrupt flag which was not seen by the guest yet. */
                csr0 &= ~(val  &  0x7f00  & pThis->u16CSR0LastSeenByGuest);
                csr0  =  (csr0 & ~0x0040) | (val  & 0x0048);
                val   =  (val  &  0x007f) | (csr0 & 0x7f00);

                /* Iff STOP, STRT and INIT are set, clear STRT and INIT */
                if ((val & 7) == 7)
                    val &= ~3;

                Log(("#%d CSR0: old=%#06x new=%#06x\n", PCNET_INST_NR, pThis->aCSR[0], csr0));

#ifndef IN_RING3
                if (!(csr0 & 0x0001/*init*/) && (val & 1))
                {
                    Log(("#%d pcnetCSRWriteU16: pcnetInit requested => HC\n", PCNET_INST_NR));
                    return VINF_IOM_HC_IOPORT_WRITE;
                }
#endif
                pThis->aCSR[0] = csr0;

                if (!CSR_STOP(pThis) && (val & 4))
                    pcnetStop(pThis);

#ifdef IN_RING3
                if (!CSR_INIT(pThis) && (val & 1))
                    pcnetInit(pThis);
#endif

                if (!CSR_STRT(pThis) && (val & 2))
                    pcnetStart(pThis);

                if (CSR_TDMD(pThis))
                    pcnetTransmit(pThis);

                return rc;
            }
        case 1:  /* IADRL */
        case 2:  /* IADRH */
        case 8:  /* LADRF  0..15 */
        case 9:  /* LADRF 16..31 */
        case 10: /* LADRF 32..47 */
        case 11: /* LADRF 48..63 */
        case 12: /* PADR   0..15 */
        case 13: /* PADR  16..31 */
        case 14: /* PADR  32..47 */
        case 18: /* CRBAL */
        case 19: /* CRBAU */
        case 20: /* CXBAL */
        case 21: /* CXBAU */
        case 22: /* NRBAL */
        case 23: /* NRBAU */
        case 26: /* NRDAL */
        case 27: /* NRDAU */
        case 28: /* CRDAL */
        case 29: /* CRDAU */
        case 32: /* NXDAL */
        case 33: /* NXDAU */
        case 34: /* CXDAL */
        case 35: /* CXDAU */
        case 36: /* NNRDL */
        case 37: /* NNRDU */
        case 38: /* NNXDL */
        case 39: /* NNXDU */
        case 40: /* CRBCL */
        case 41: /* CRBCU */
        case 42: /* CXBCL */
        case 43: /* CXBCU */
        case 44: /* NRBCL */
        case 45: /* NRBCU */
        case 46: /* POLL */
        case 47: /* POLLINT */
        case 72: /* RCVRC */
        case 74: /* XMTRC */
        case 112: /* MISSC */
            if (CSR_STOP(pThis) || CSR_SPND(pThis))
                break;
        case 3: /* Interrupt Mask and Deferral Control */
            break;
        case 4: /* Test and Features Control */
            pThis->aCSR[4] &= ~(val & 0x026a);
            val &= ~0x026a;
            val |= pThis->aCSR[4] & 0x026a;
            break;
        case 5: /* Extended Control and Interrupt 1 */
            pThis->aCSR[5] &= ~(val & 0x0a90);
            val &= ~0x0a90;
            val |= pThis->aCSR[5] & 0x0a90;
            break;
        case 7: /* Extended Control and Interrupt 2 */
            {
                uint16_t csr7 = pThis->aCSR[7];
                csr7 &=        ~0x0400 ;
                csr7 &= ~(val & 0x0800);
                csr7 |=  (val & 0x0400);
                pThis->aCSR[7] = csr7;
                return rc;
            }
        case 15: /* Mode */
            if ((pThis->aCSR[15] & 0x8000) != (val & 0x8000) && pThis->pDrv)
            {
                Log(("#%d: promiscuous mode changed to %d\n", PCNET_INST_NR, !!(val & 0x8000)));
#ifndef IN_RING3
                return VINF_IOM_HC_IOPORT_WRITE;
#else
                /* check for promiscuous mode change */
                if (pThis->pDrv)
                    pThis->pDrv->pfnSetPromiscuousMode(pThis->pDrv, !!(val & 0x8000));
#endif
            }
            break;
        case 16: /* IADRL */
            return pcnetCSRWriteU16(pThis, 1, val);
        case 17: /* IADRH */
            return pcnetCSRWriteU16(pThis, 2, val);

        /*
         * 24 and 25 are the Base Address of Receive Descriptor.
         * We combine and mirror these in GCRDRA.
         */
        case 24: /* BADRL */
        case 25: /* BADRU */
            if (!CSR_STOP(pThis) && !CSR_SPND(pThis))
            {
                Log(("#%d: WRITE CSR%d, %#06x !!\n", PCNET_INST_NR, u32RAP, val));
                return rc;
            }
            if (u32RAP == 24)
                pThis->GCRDRA = (pThis->GCRDRA & 0xffff0000) | (val & 0x0000ffff);
            else
                pThis->GCRDRA = (pThis->GCRDRA & 0x0000ffff) | ((val & 0x0000ffff) << 16);
            Log(("#%d: WRITE CSR%d, %#06x => GCRDRA=%08x (alt init)\n", PCNET_INST_NR, u32RAP, val, pThis->GCRDRA));
            break;

        /*
         * 30 & 31 are the Base Address of Transmit Descriptor.
         * We combine and mirrorthese in GCTDRA.
         */
        case 30: /* BADXL */
        case 31: /* BADXU */
            if (!CSR_STOP(pThis) && !CSR_SPND(pThis))
            {
                Log(("#%d: WRITE CSR%d, %#06x !!\n", PCNET_INST_NR, u32RAP, val));
                return rc;
            }
            if (u32RAP == 30)
                pThis->GCTDRA = (pThis->GCTDRA & 0xffff0000) | (val & 0x0000ffff);
            else
                pThis->GCTDRA = (pThis->GCTDRA & 0x0000ffff) | ((val & 0x0000ffff) << 16);
            Log(("#%d: WRITE CSR%d, %#06x => GCTDRA=%08x (alt init)\n", PCNET_INST_NR, u32RAP, val, pThis->GCTDRA));
            break;

        case 58: /* Software Style */
            rc = pcnetBCRWriteU16(pThis, BCR_SWS, val);
            break;

        /*
         * Registers 76 and 78 aren't stored correctly (see todos), but I'm don't dare
         * try fix that right now. So, as a quick hack for 'alt init' I'll just correct them here.
         */
        case 76: /* RCVRL */ /** @todo call pcnetUpdateRingHandlers */
                             /** @todo receive ring length is stored in two's complement! */
        case 78: /* XMTRL */ /** @todo call pcnetUpdateRingHandlers */
                             /** @todo transmit ring length is stored in two's complement! */
            if (!CSR_STOP(pThis) && !CSR_SPND(pThis))
            {
                Log(("#%d: WRITE CSR%d, %#06x !!\n", PCNET_INST_NR, u32RAP, val));
                return rc;
            }
            Log(("#%d: WRITE CSR%d, %#06x (hacked %#06x) (alt init)\n", PCNET_INST_NR,
                 u32RAP, val, 1 + ~(uint16_t)val));
            val = 1 + ~(uint16_t)val;

            /*
             * HACK ALERT! Set the counter registers too.
             */
            pThis->aCSR[u32RAP - 4] = val;
            break;

        default:
            return rc;
    }
    pThis->aCSR[u32RAP] = val;
    return rc;
}

/**
 * Encode a 32-bit link speed into a custom 16-bit floating-point value
 */
static uint32_t pcnetLinkSpd(uint32_t speed)
{
    unsigned    exp = 0;

    while (speed & 0xFFFFE000)
    {
        speed /= 10;
        ++exp;
    }
    return (exp << 13) | speed;
}

static uint32_t pcnetCSRReadU16(PCNetState *pThis, uint32_t u32RAP)
{
    uint32_t val;
    switch (u32RAP)
    {
        case 0:
            pcnetUpdateIrq(pThis);
            val = pThis->aCSR[0];
            val |= (val & 0x7800) ? 0x8000 : 0;
            pThis->u16CSR0LastSeenByGuest = val;
            break;
        case 16:
            return pcnetCSRReadU16(pThis, 1);
        case 17:
            return pcnetCSRReadU16(pThis, 2);
        case 58:
            return pcnetBCRReadU16(pThis, BCR_SWS);
        case 68:    /* Custom register to pass link speed to driver */
            return pcnetLinkSpd(pThis->u32LinkSpeed);
        case 88:
            val = pThis->aCSR[89];
            val <<= 16;
            val |= pThis->aCSR[88];
            break;
        default:
            val = pThis->aCSR[u32RAP];
    }
#ifdef PCNET_DEBUG_CSR
    Log(("#%d pcnetCSRReadU16: rap=%d val=%#06x\n", PCNET_INST_NR, u32RAP, val));
#endif
    return val;
}

static int pcnetBCRWriteU16(PCNetState *pThis, uint32_t u32RAP, uint32_t val)
{
    int rc = VINF_SUCCESS;
    u32RAP &= 0x7f;
#ifdef PCNET_DEBUG_BCR
    Log2(("#%d pcnetBCRWriteU16: rap=%d val=%#06x\n", PCNET_INST_NR, u32RAP, val));
#endif
    switch (u32RAP)
    {
        case BCR_SWS:
            if (!(CSR_STOP(pThis) || CSR_SPND(pThis)))
                return rc;
            val &= ~0x0300;
            switch (val & 0x00ff)
            {
                default:
                    Log(("#%d Bad SWSTYLE=%#04x\n", PCNET_INST_NR, val & 0xff));
                    // fall through
                case 0:
                    val |= 0x0200; /* 16 bit */
                    pThis->iLog2DescSize = 3;
                    pThis->GCUpperPhys   = (0xff00 & (uint32_t)pThis->aCSR[2]) << 16;
                    break;
                case 1:
                    val |= 0x0100; /* 32 bit */
                    pThis->iLog2DescSize = 4;
                    pThis->GCUpperPhys   = 0;
                    break;
                case 2:
                case 3:
                    val |= 0x0300; /* 32 bit */
                    pThis->iLog2DescSize = 4;
                    pThis->GCUpperPhys   = 0;
                    break;
            }
            Log(("#%d BCR_SWS=%#06x\n", PCNET_INST_NR, val));
            pThis->aCSR[58] = val;
            /* fall through */
        case BCR_LNKST:
        case BCR_LED1:
        case BCR_LED2:
        case BCR_LED3:
        case BCR_MC:
        case BCR_FDC:
        case BCR_BSBC:
        case BCR_EECAS:
        case BCR_PLAT:
        case BCR_MIICAS:
        case BCR_MIIADDR:
            pThis->aBCR[u32RAP] = val;
            break;

        case BCR_STVAL:
            val &= 0xffff;
            pThis->aBCR[BCR_STVAL] = val;
            if (pThis->fAm79C973)
                TMTimerSetNano(pThis->CTX_SUFF(pTimerSoftInt), 12800U * val);
            break;

        case BCR_MIIMDR:
            pThis->aMII[pThis->aBCR[BCR_MIIADDR] & 0x1f] = val;
#ifdef PCNET_DEBUG_MII
            Log(("#%d pcnet: mii write %d <- %#x\n", PCNET_INST_NR, pThis->aBCR[BCR_MIIADDR] & 0x1f, val));
#endif
            break;

        default:
            break;
    }
    return rc;
}

static uint32_t pcnetMIIReadU16(PCNetState *pThis, uint32_t miiaddr)
{
    uint32_t val;
    bool autoneg, duplex, fast;
    STAM_COUNTER_INC(&pThis->StatMIIReads);

    autoneg = (pThis->aBCR[BCR_MIICAS] & 0x20) != 0;
    duplex  = (pThis->aBCR[BCR_MIICAS] & 0x10) != 0;
    fast    = (pThis->aBCR[BCR_MIICAS] & 0x08) != 0;

    switch (miiaddr)
    {
        case 0:
            /* MII basic mode control register. */
            val = 0;
            if (autoneg)
                val |= 0x1000;  /* Enable auto negotiation. */
            if (fast)
                val |= 0x2000;  /* 100 Mbps */
            if (duplex) /* Full duplex forced */
                val |= 0x0100;  /* Full duplex */
            break;

        case 1:
            /* MII basic mode status register. */
            val = 0x7800    /* Can do 100mbps FD/HD and 10mbps FD/HD. */
                | 0x0040    /* Mgmt frame preamble not required. */
                | 0x0020    /* Auto-negotiation complete. */
                | 0x0008    /* Able to do auto-negotiation. */
                | 0x0004    /* Link up. */
                | 0x0001;   /* Extended Capability, i.e. registers 4+ valid. */
            if (!pThis->fLinkUp || pThis->fLinkTempDown) {
                val &= ~(0x0020 | 0x0004);
                pThis->cLinkDownReported++;
            }
            if (!autoneg) {
                /* Auto-negotiation disabled. */
                val &= ~(0x0020 | 0x0008);
                if (duplex)
                    /* Full duplex forced. */
                    val &= ~0x2800;
                else
                    /* Half duplex forced. */
                    val &= ~0x5000;

                if (fast)
                    /* 100 Mbps forced */
                    val &= ~0x1800;
                else
                    /* 10 Mbps forced */
                    val &= ~0x6000;
            }
            break;

        case 2:
            /* PHY identifier 1. */
            val = 0x22;     /* Am79C874 PHY */
            break;

        case 3:
            /* PHY identifier 2. */
            val = 0x561b;   /* Am79C874 PHY */
            break;

        case 4:
            /* Advertisement control register. */
            val =   0x01e0  /* Try 100mbps FD/HD and 10mbps FD/HD. */
#if 0
                // Advertising flow control is a) not the default, and b) confuses
                // the link speed detection routine in Windows PCnet driver
                  | 0x0400  /* Try flow control. */
#endif
                  | 0x0001; /* CSMA selector. */
            break;

        case 5:
            /* Link partner ability register. */
            if (pThis->fLinkUp && !pThis->fLinkTempDown)
                val =   0x8000  /* Next page bit. */
                      | 0x4000  /* Link partner acked us. */
                      | 0x0400  /* Can do flow control. */
                      | 0x01e0  /* Can do 100mbps FD/HD and 10mbps FD/HD. */
                      | 0x0001; /* Use CSMA selector. */
            else
            {
                val = 0;
                pThis->cLinkDownReported++;
            }
            break;

        case 6:
            /* Auto negotiation expansion register. */
            if (pThis->fLinkUp && !pThis->fLinkTempDown)
                val =   0x0008  /* Link partner supports npage. */
                      | 0x0004  /* Enable npage words. */
                      | 0x0001; /* Can do N-way auto-negotiation. */
            else
            {
                val = 0;
                pThis->cLinkDownReported++;
            }
            break;

        default:
            val = 0;
            break;
    }

#ifdef PCNET_DEBUG_MII
    Log(("#%d pcnet: mii read %d -> %#x\n", PCNET_INST_NR, miiaddr, val));
#endif
    return val;
}

static uint32_t pcnetBCRReadU16(PCNetState *pThis, uint32_t u32RAP)
{
    uint32_t val;
    u32RAP &= 0x7f;
    switch (u32RAP)
    {
        case BCR_LNKST:
        case BCR_LED1:
        case BCR_LED2:
        case BCR_LED3:
            val = pThis->aBCR[u32RAP] & ~0x8000;
            /* Clear LNKSTE if we're not connected or if we've just loaded a VM state. */
            if (!pThis->pDrv || pThis->fLinkTempDown || !pThis->fLinkUp)
            {
                if (u32RAP == 4)
                    pThis->cLinkDownReported++;
                val &= ~0x40;
            }
            val |= (val & 0x017f & pThis->u32Lnkst) ? 0x8000 : 0;
            break;

        case BCR_MIIMDR:
            if (pThis->fAm79C973 && (pThis->aBCR[BCR_MIIADDR] >> 5 & 0x1f) == 0)
            {
                uint32_t miiaddr = pThis->aBCR[BCR_MIIADDR] & 0x1f;
                val = pcnetMIIReadU16(pThis, miiaddr);
            }
            else
                val = 0xffff;
            break;

        default:
            val = u32RAP < BCR_MAX_RAP ? pThis->aBCR[u32RAP] : 0;
            break;
    }
#ifdef PCNET_DEBUG_BCR
    Log2(("#%d pcnetBCRReadU16: rap=%d val=%#06x\n", PCNET_INST_NR, u32RAP, val));
#endif
    return val;
}

#ifdef IN_RING3 /* move down */
static void pcnetHardReset(PCNetState *pThis)
{
    int      i;
    uint16_t checksum;

    /* Initialize the PROM */
    Assert(sizeof(pThis->MacConfigured) == 6);
    memcpy(pThis->aPROM, &pThis->MacConfigured, sizeof(pThis->MacConfigured));
    pThis->aPROM[ 8] = 0x00;
    pThis->aPROM[ 9] = 0x11;
    pThis->aPROM[12] = pThis->aPROM[13] = 0x00;
    pThis->aPROM[14] = pThis->aPROM[15] = 0x57;

    for (i = 0, checksum = 0; i < 16; i++)
        checksum += pThis->aPROM[i];
    *(uint16_t *)&pThis->aPROM[12] = RT_H2LE_U16(checksum);

    pThis->aBCR[BCR_MSRDA] = 0x0005;
    pThis->aBCR[BCR_MSWRA] = 0x0005;
    pThis->aBCR[BCR_MC   ] = 0x0002;
    pThis->aBCR[BCR_LNKST] = 0x00c0;
    pThis->aBCR[BCR_LED1 ] = 0x0084;
    pThis->aBCR[BCR_LED2 ] = 0x0088;
    pThis->aBCR[BCR_LED3 ] = 0x0090;
    pThis->aBCR[BCR_FDC  ] = 0x0000;
    pThis->aBCR[BCR_BSBC ] = 0x9001;
    pThis->aBCR[BCR_EECAS] = 0x0002;
    pThis->aBCR[BCR_STVAL] = 0xffff;
    pThis->aCSR[58       ] = /* CSR58 is an alias for BCR20 */
    pThis->aBCR[BCR_SWS  ] = 0x0200;
    pThis->iLog2DescSize   = 3;
    pThis->aBCR[BCR_PLAT ] = 0xff06;
    pThis->aBCR[BCR_MIIADDR ] = 0;  /* Internal PHY on Am79C973 would be (0x1e << 5) */
    pThis->aBCR[BCR_PCIVID] = PCIDevGetVendorId(&pThis->PciDev);
    pThis->aBCR[BCR_PCISID] = PCIDevGetSubSystemId(&pThis->PciDev);
    pThis->aBCR[BCR_PCISVID] = PCIDevGetSubSystemVendorId(&pThis->PciDev);

    /* Reset the error counter. */
    pThis->uCntBadRMD      = 0;

    pcnetSoftReset(pThis);
}
#endif /* IN_RING3 */

static void pcnetAPROMWriteU8(PCNetState *pThis, uint32_t addr, uint32_t val)
{
    addr &= 0x0f;
    val  &= 0xff;
    Log(("#%d pcnetAPROMWriteU8: addr=%#010x val=%#04x\n", PCNET_INST_NR, addr, val));
    /* Check APROMWE bit to enable write access */
    if (pcnetBCRReadU16(pThis, 2) & 0x80)
        pThis->aPROM[addr] = val;
}

static uint32_t pcnetAPROMReadU8(PCNetState *pThis, uint32_t addr)
{
    uint32_t val = pThis->aPROM[addr &= 0x0f];
    Log(("#%d pcnetAPROMReadU8: addr=%#010x val=%#04x\n", PCNET_INST_NR, addr, val));
    return val;
}

static int pcnetIoportWriteU8(PCNetState *pThis, uint32_t addr, uint32_t val)
{
    int rc = VINF_SUCCESS;

#ifdef PCNET_DEBUG_IO
    Log2(("#%d pcnetIoportWriteU8: addr=%#010x val=%#06x\n", PCNET_INST_NR,
         addr, val));
#endif
    if (RT_LIKELY(!BCR_DWIO(pThis)))
    {
        switch (addr & 0x0f)
        {
            case 0x04: /* RESET */
                break;
        }
    }
    else
        Log(("#%d pcnetIoportWriteU8: addr=%#010x val=%#06x BCR_DWIO !!\n", PCNET_INST_NR, addr, val));

    return rc;
}

static uint32_t pcnetIoportReadU8(PCNetState *pThis, uint32_t addr, int *pRC)
{
    uint32_t val = ~0U;

    *pRC = VINF_SUCCESS;

    if (RT_LIKELY(!BCR_DWIO(pThis)))
    {
        switch (addr & 0x0f)
        {
            case 0x04: /* RESET */
                pcnetSoftReset(pThis);
                val = 0;
                break;
        }
    }
    else
        Log(("#%d pcnetIoportReadU8: addr=%#010x val=%#06x BCR_DWIO !!\n", PCNET_INST_NR, addr, val & 0xff));

    pcnetUpdateIrq(pThis);

#ifdef PCNET_DEBUG_IO
    Log2(("#%d pcnetIoportReadU8: addr=%#010x val=%#06x\n", PCNET_INST_NR, addr, val & 0xff));
#endif
    return val;
}

static int pcnetIoportWriteU16(PCNetState *pThis, uint32_t addr, uint32_t val)
{
    int rc = VINF_SUCCESS;

#ifdef PCNET_DEBUG_IO
    Log2(("#%d pcnetIoportWriteU16: addr=%#010x val=%#06x\n", PCNET_INST_NR,
         addr, val));
#endif
    if (RT_LIKELY(!BCR_DWIO(pThis)))
    {
        switch (addr & 0x0f)
        {
            case 0x00: /* RDP */
                pcnetPollTimer(pThis);
                rc = pcnetCSRWriteU16(pThis, pThis->u32RAP, val);
                pcnetUpdateIrq(pThis);
                break;
            case 0x02: /* RAP */
                pThis->u32RAP = val & 0x7f;
                break;
            case 0x06: /* BDP */
                rc = pcnetBCRWriteU16(pThis, pThis->u32RAP, val);
                break;
        }
    }
    else
        Log(("#%d pcnetIoportWriteU16: addr=%#010x val=%#06x BCR_DWIO !!\n", PCNET_INST_NR, addr, val));

    return rc;
}

static uint32_t pcnetIoportReadU16(PCNetState *pThis, uint32_t addr, int *pRC)
{
    uint32_t val = ~0U;

    *pRC = VINF_SUCCESS;

    if (RT_LIKELY(!BCR_DWIO(pThis)))
    {
        switch (addr & 0x0f)
        {
            case 0x00: /* RDP */
                /** @note if we're not polling, then the guest will tell us when to poll by setting TDMD in CSR0 */
                /** Polling is then useless here and possibly expensive. */
                if (!CSR_DPOLL(pThis))
                    pcnetPollTimer(pThis);

                val = pcnetCSRReadU16(pThis, pThis->u32RAP);
                if (pThis->u32RAP == 0)  // pcnetUpdateIrq() already called by pcnetCSRReadU16()
                    goto skip_update_irq;
                break;
            case 0x02: /* RAP */
                val = pThis->u32RAP;
                goto skip_update_irq;
            case 0x04: /* RESET */
                pcnetSoftReset(pThis);
                val = 0;
                break;
            case 0x06: /* BDP */
                val = pcnetBCRReadU16(pThis, pThis->u32RAP);
                break;
        }
    }
    else
        Log(("#%d pcnetIoportReadU16: addr=%#010x val=%#06x BCR_DWIO !!\n", PCNET_INST_NR, addr, val & 0xffff));

    pcnetUpdateIrq(pThis);

skip_update_irq:
#ifdef PCNET_DEBUG_IO
    Log2(("#%d pcnetIoportReadU16: addr=%#010x val=%#06x\n", PCNET_INST_NR, addr, val & 0xffff));
#endif
    return val;
}

static int pcnetIoportWriteU32(PCNetState *pThis, uint32_t addr, uint32_t val)
{
    int rc = VINF_SUCCESS;

#ifdef PCNET_DEBUG_IO
    Log2(("#%d pcnetIoportWriteU32: addr=%#010x val=%#010x\n", PCNET_INST_NR,
         addr, val));
#endif
    if (RT_LIKELY(BCR_DWIO(pThis)))
    {
        switch (addr & 0x0f)
        {
            case 0x00: /* RDP */
                pcnetPollTimer(pThis);
                rc = pcnetCSRWriteU16(pThis, pThis->u32RAP, val & 0xffff);
                pcnetUpdateIrq(pThis);
                break;
            case 0x04: /* RAP */
                pThis->u32RAP = val & 0x7f;
                break;
            case 0x0c: /* BDP */
                rc = pcnetBCRWriteU16(pThis, pThis->u32RAP, val & 0xffff);
                break;
        }
    }
    else if ((addr & 0x0f) == 0)
    {
        /* switch device to dword I/O mode */
        pcnetBCRWriteU16(pThis, BCR_BSBC, pcnetBCRReadU16(pThis, BCR_BSBC) | 0x0080);
#ifdef PCNET_DEBUG_IO
        Log2(("device switched into dword i/o mode\n"));
#endif
    }
    else
        Log(("#%d pcnetIoportWriteU32: addr=%#010x val=%#010x !BCR_DWIO !!\n", PCNET_INST_NR, addr, val));

    return rc;
}

static uint32_t pcnetIoportReadU32(PCNetState *pThis, uint32_t addr, int *pRC)
{
    uint32_t val = ~0U;

    *pRC = VINF_SUCCESS;

    if (RT_LIKELY(BCR_DWIO(pThis)))
    {
        switch (addr & 0x0f)
        {
            case 0x00: /* RDP */
                /** @note if we're not polling, then the guest will tell us when to poll by setting TDMD in CSR0 */
                /** Polling is then useless here and possibly expensive. */
                if (!CSR_DPOLL(pThis))
                    pcnetPollTimer(pThis);

                val = pcnetCSRReadU16(pThis, pThis->u32RAP);
                if (pThis->u32RAP == 0)  // pcnetUpdateIrq() already called by pcnetCSRReadU16()
                    goto skip_update_irq;
                break;
            case 0x04: /* RAP */
                val = pThis->u32RAP;
                goto skip_update_irq;
            case 0x08: /* RESET */
                pcnetSoftReset(pThis);
                val = 0;
                break;
            case 0x0c: /* BDP */
                val = pcnetBCRReadU16(pThis, pThis->u32RAP);
                break;
        }
    }
    else
        Log(("#%d pcnetIoportReadU32: addr=%#010x val=%#010x !BCR_DWIO !!\n", PCNET_INST_NR, addr, val));
    pcnetUpdateIrq(pThis);

skip_update_irq:
#ifdef PCNET_DEBUG_IO
    Log2(("#%d pcnetIoportReadU32: addr=%#010x val=%#010x\n", PCNET_INST_NR, addr, val));
#endif
    return val;
}

static void pcnetMMIOWriteU8(PCNetState *pThis, RTGCPHYS addr, uint32_t val)
{
#ifdef PCNET_DEBUG_IO
    Log2(("#%d pcnetMMIOWriteU8: addr=%#010x val=%#04x\n", PCNET_INST_NR, addr, val));
#endif
    if (!(addr & 0x10))
        pcnetAPROMWriteU8(pThis, addr, val);
}

static uint32_t pcnetMMIOReadU8(PCNetState *pThis, RTGCPHYS addr)
{
    uint32_t val = ~0U;
    if (!(addr & 0x10))
        val = pcnetAPROMReadU8(pThis, addr);
#ifdef PCNET_DEBUG_IO
    Log2(("#%d pcnetMMIOReadU8: addr=%#010x val=%#04x\n", PCNET_INST_NR, addr, val & 0xff));
#endif
    return val;
}

static void pcnetMMIOWriteU16(PCNetState *pThis, RTGCPHYS addr, uint32_t val)
{
#ifdef PCNET_DEBUG_IO
    Log2(("#%d pcnetMMIOWriteU16: addr=%#010x val=%#06x\n", PCNET_INST_NR, addr, val));
#endif
    if (addr & 0x10)
        pcnetIoportWriteU16(pThis, addr & 0x0f, val);
    else
    {
        pcnetAPROMWriteU8(pThis, addr,   val     );
        pcnetAPROMWriteU8(pThis, addr+1, val >> 8);
    }
}

static uint32_t pcnetMMIOReadU16(PCNetState *pThis, RTGCPHYS addr)
{
    uint32_t val = ~0U;
    int      rc;

    if (addr & 0x10)
        val = pcnetIoportReadU16(pThis, addr & 0x0f, &rc);
    else
    {
        val = pcnetAPROMReadU8(pThis, addr+1);
        val <<= 8;
        val |= pcnetAPROMReadU8(pThis, addr);
    }
#ifdef PCNET_DEBUG_IO
    Log2(("#%d pcnetMMIOReadU16: addr=%#010x val = %#06x\n", PCNET_INST_NR, addr, val & 0xffff));
#endif
    return val;
}

static void pcnetMMIOWriteU32(PCNetState *pThis, RTGCPHYS addr, uint32_t val)
{
#ifdef PCNET_DEBUG_IO
    Log2(("#%d pcnetMMIOWriteU32: addr=%#010x val=%#010x\n", PCNET_INST_NR, addr, val));
#endif
    if (addr & 0x10)
        pcnetIoportWriteU32(pThis, addr & 0x0f, val);
    else
    {
        pcnetAPROMWriteU8(pThis, addr,   val      );
        pcnetAPROMWriteU8(pThis, addr+1, val >>  8);
        pcnetAPROMWriteU8(pThis, addr+2, val >> 16);
        pcnetAPROMWriteU8(pThis, addr+3, val >> 24);
    }
}

static uint32_t pcnetMMIOReadU32(PCNetState *pThis, RTGCPHYS addr)
{
    uint32_t val;
    int      rc;

    if (addr & 0x10)
        val = pcnetIoportReadU32(pThis, addr & 0x0f, &rc);
    else
    {
        val  = pcnetAPROMReadU8(pThis, addr+3);
        val <<= 8;
        val |= pcnetAPROMReadU8(pThis, addr+2);
        val <<= 8;
        val |= pcnetAPROMReadU8(pThis, addr+1);
        val <<= 8;
        val |= pcnetAPROMReadU8(pThis, addr  );
    }
#ifdef PCNET_DEBUG_IO
    Log2(("#%d pcnetMMIOReadU32: addr=%#010x val=%#010x\n", PCNET_INST_NR, addr, val));
#endif
    return val;
}


/**
 * Port I/O Handler for IN operations.
 *
 * @returns VBox status code.
 *
 * @param   pDevIns     The device instance.
 * @param   pvUser      User argument.
 * @param   Port        Port number used for the IN operation.
 * @param   pu32        Where to store the result.
 * @param   cb          Number of bytes read.
 */
PDMBOTHCBDECL(int) pcnetIOPortAPromRead(PPDMDEVINS pDevIns, void *pvUser,
                                        RTIOPORT Port, uint32_t *pu32, unsigned cb)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    int        rc;

    STAM_PROFILE_ADV_START(&pThis->StatAPROMRead, a);
    rc = PDMCritSectEnter(&pThis->CritSect, VINF_IOM_HC_IOPORT_WRITE);
    if (rc == VINF_SUCCESS)
    {

        /* FreeBSD is accessing in dwords. */
        if (cb == 1)
            *pu32 = pcnetAPROMReadU8(pThis, Port);
        else if (cb == 2 && !BCR_DWIO(pThis))
            *pu32 = pcnetAPROMReadU8(pThis, Port)
                  | (pcnetAPROMReadU8(pThis, Port + 1) << 8);
        else if (cb == 4 && BCR_DWIO(pThis))
            *pu32 = pcnetAPROMReadU8(pThis, Port)
                  | (pcnetAPROMReadU8(pThis, Port + 1) << 8)
                  | (pcnetAPROMReadU8(pThis, Port + 2) << 16)
                  | (pcnetAPROMReadU8(pThis, Port + 3) << 24);
        else
        {
            Log(("#%d pcnetIOPortAPromRead: Port=%RTiop cb=%d BCR_DWIO !!\n", PCNET_INST_NR, Port, cb));
            rc = VERR_IOM_IOPORT_UNUSED;
        }
        PDMCritSectLeave(&pThis->CritSect);
    }
    STAM_PROFILE_ADV_STOP(&pThis->StatAPROMRead, a);
    LogFlow(("#%d pcnetIOPortAPromRead: Port=%RTiop *pu32=%#RX32 cb=%d rc=%Rrc\n", PCNET_INST_NR, Port, *pu32, cb, rc));
    return rc;
}


/**
 * Port I/O Handler for OUT operations.
 *
 * @returns VBox status code.
 *
 * @param   pDevIns     The device instance.
 * @param   pvUser      User argument.
 * @param   Port        Port number used for the IN operation.
 * @param   u32         The value to output.
 * @param   cb          The value size in bytes.
 */
PDMBOTHCBDECL(int) pcnetIOPortAPromWrite(PPDMDEVINS pDevIns, void *pvUser,
                                         RTIOPORT Port, uint32_t u32, unsigned cb)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    int        rc;

    if (cb == 1)
    {
        STAM_PROFILE_ADV_START(&pThis->StatAPROMWrite, a);
        rc = PDMCritSectEnter(&pThis->CritSect, VINF_IOM_HC_IOPORT_WRITE);
        if (RT_LIKELY(rc == VINF_SUCCESS))
        {
            pcnetAPROMWriteU8(pThis, Port, u32);
            PDMCritSectLeave(&pThis->CritSect);
        }
        STAM_PROFILE_ADV_STOP(&pThis->StatAPROMWrite, a);
    }
    else
    {
        AssertMsgFailed(("Port=%#x cb=%d u32=%#x\n", Port, cb, u32));
        rc = VINF_SUCCESS;
    }
    LogFlow(("#%d pcnetIOPortAPromWrite: Port=%RTiop u32=%#RX32 cb=%d rc=%Rrc\n", PCNET_INST_NR, Port, u32, cb, rc));
#ifdef LOG_ENABLED
    if (rc == VINF_IOM_HC_IOPORT_WRITE)
        LogFlow(("#%d => HC\n", PCNET_INST_NR));
#endif
    return rc;
}


/**
 * Port I/O Handler for IN operations.
 *
 * @returns VBox status code.
 *
 * @param   pDevIns     The device instance.
 * @param   pvUser      User argument.
 * @param   Port        Port number used for the IN operation.
 * @param   pu32        Where to store the result.
 * @param   cb          Number of bytes read.
 */
PDMBOTHCBDECL(int) pcnetIOPortRead(PPDMDEVINS pDevIns, void *pvUser,
                                   RTIOPORT Port, uint32_t *pu32, unsigned cb)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    int         rc    = VINF_SUCCESS;

    STAM_PROFILE_ADV_START(&pThis->CTXSUFF(StatIORead), a);
    rc = PDMCritSectEnter(&pThis->CritSect, VINF_IOM_HC_IOPORT_READ);
    if (RT_LIKELY(rc == VINF_SUCCESS))
    {
        switch (cb)
        {
            case 1: *pu32 = pcnetIoportReadU8(pThis, Port, &rc); break;
            case 2: *pu32 = pcnetIoportReadU16(pThis, Port, &rc); break;
            case 4: *pu32 = pcnetIoportReadU32(pThis, Port, &rc); break;
            default:
                rc = PDMDeviceDBGFStop(pThis->CTX_SUFF(pDevIns), RT_SRC_POS,
                                       "pcnetIOPortRead: unsupported op size: offset=%#10x cb=%u\n",
                                       Port, cb);
        }
        PDMCritSectLeave(&pThis->CritSect);
    }
    STAM_PROFILE_ADV_STOP(&pThis->CTXSUFF(StatIORead), a);
    Log2(("#%d pcnetIOPortRead: Port=%RTiop *pu32=%#RX32 cb=%d rc=%Rrc\n", PCNET_INST_NR, Port, *pu32, cb, rc));
#ifdef LOG_ENABLED
    if (rc == VINF_IOM_HC_IOPORT_READ)
        LogFlow(("#%d pcnetIOPortRead/critsect failed in GC => HC\n", PCNET_INST_NR));
#endif
    return rc;
}


/**
 * Port I/O Handler for OUT operations.
 *
 * @returns VBox status code.
 *
 * @param   pDevIns     The device instance.
 * @param   pvUser      User argument.
 * @param   Port        Port number used for the IN operation.
 * @param   u32         The value to output.
 * @param   cb          The value size in bytes.
 */
PDMBOTHCBDECL(int) pcnetIOPortWrite(PPDMDEVINS pDevIns, void *pvUser,
                                    RTIOPORT Port, uint32_t u32, unsigned cb)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    int         rc    = VINF_SUCCESS;

    STAM_PROFILE_ADV_START(&pThis->CTXSUFF(StatIOWrite), a);
    rc = PDMCritSectEnter(&pThis->CritSect, VINF_IOM_HC_IOPORT_WRITE);
    if (RT_LIKELY(rc == VINF_SUCCESS))
    {
        switch (cb)
        {
            case 1: rc = pcnetIoportWriteU8(pThis, Port, u32); break;
            case 2: rc = pcnetIoportWriteU16(pThis, Port, u32); break;
            case 4: rc = pcnetIoportWriteU32(pThis, Port, u32); break;
            default:
                rc = PDMDeviceDBGFStop(pThis->CTX_SUFF(pDevIns), RT_SRC_POS,
                                       "pcnetIOPortWrite: unsupported op size: offset=%#10x cb=%u\n",
                                       Port, cb);
        }
        PDMCritSectLeave(&pThis->CritSect);
    }
    STAM_PROFILE_ADV_STOP(&pThis->CTXSUFF(StatIOWrite), a);
    Log2(("#%d pcnetIOPortWrite: Port=%RTiop u32=%#RX32 cb=%d rc=%Rrc\n", PCNET_INST_NR, Port, u32, cb, rc));
#ifdef LOG_ENABLED
    if (rc == VINF_IOM_HC_IOPORT_WRITE)
        LogFlow(("#%d pcnetIOPortWrite/critsect failed in GC => HC\n", PCNET_INST_NR));
#endif
    return rc;
}


/**
 * Memory mapped I/O Handler for read operations.
 *
 * @returns VBox status code.
 *
 * @param   pDevIns     The device instance.
 * @param   pvUser      User argument.
 * @param   GCPhysAddr  Physical address (in GC) where the read starts.
 * @param   pv          Where to store the result.
 * @param   cb          Number of bytes read.
 */
PDMBOTHCBDECL(int) pcnetMMIORead(PPDMDEVINS pDevIns, void *pvUser,
                                 RTGCPHYS GCPhysAddr, void *pv, unsigned cb)
{
    PCNetState *pThis = (PCNetState *)pvUser;
    int         rc    = VINF_SUCCESS;

    /*
     * We have to check the range, because we're page aligning the MMIO stuff presently.
     */
    if (GCPhysAddr - pThis->MMIOBase < PCNET_PNPMMIO_SIZE)
    {
        STAM_PROFILE_ADV_START(&pThis->CTXSUFF(StatMMIORead), a);
        rc = PDMCritSectEnter(&pThis->CritSect, VINF_IOM_HC_MMIO_READ);
        if (RT_LIKELY(rc == VINF_SUCCESS))
        {
            switch (cb)
            {
                case 1:  *(uint8_t  *)pv = pcnetMMIOReadU8 (pThis, GCPhysAddr); break;
                case 2:  *(uint16_t *)pv = pcnetMMIOReadU16(pThis, GCPhysAddr); break;
                case 4:  *(uint32_t *)pv = pcnetMMIOReadU32(pThis, GCPhysAddr); break;
                default:
                    rc = PDMDeviceDBGFStop(pThis->CTX_SUFF(pDevIns), RT_SRC_POS,
                                           "pcnetMMIORead: unsupported op size: address=%RGp cb=%u\n",
                                           GCPhysAddr, cb);
            }
            PDMCritSectLeave(&pThis->CritSect);
        }
        STAM_PROFILE_ADV_STOP(&pThis->CTXSUFF(StatMMIORead), a);
    }
    else
        memset(pv, 0, cb);

    LogFlow(("#%d pcnetMMIORead: pvUser=%p:{%.*Rhxs} cb=%d GCPhysAddr=%RGp rc=%Rrc\n",
             PCNET_INST_NR, pv, cb, pv, cb, GCPhysAddr, rc));
#ifdef LOG_ENABLED
    if (rc == VINF_IOM_HC_MMIO_READ)
        LogFlow(("#%d => HC\n", PCNET_INST_NR));
#endif
    return rc;
}


/**
 * Port I/O Handler for write operations.
 *
 * @returns VBox status code.
 *
 * @param   pDevIns     The device instance.
 * @param   pvUser      User argument.
 * @param   GCPhysAddr  Physical address (in GC) where the read starts.
 * @param   pv          Where to fetch the result.
 * @param   cb          Number of bytes to write.
 */
PDMBOTHCBDECL(int) pcnetMMIOWrite(PPDMDEVINS pDevIns, void *pvUser,
                                  RTGCPHYS GCPhysAddr, void *pv, unsigned cb)
{
    PCNetState *pThis = (PCNetState *)pvUser;
    int         rc    = VINF_SUCCESS;

    /*
     * We have to check the range, because we're page aligning the MMIO stuff presently.
     */
    if (GCPhysAddr - pThis->MMIOBase < PCNET_PNPMMIO_SIZE)
    {
        STAM_PROFILE_ADV_START(&pThis->CTXSUFF(StatMMIOWrite), a);
        rc = PDMCritSectEnter(&pThis->CritSect, VINF_IOM_HC_MMIO_WRITE);
        if (RT_LIKELY(rc == VINF_SUCCESS))
        {
            switch (cb)
            {
                case 1:  pcnetMMIOWriteU8 (pThis, GCPhysAddr, *(uint8_t  *)pv); break;
                case 2:  pcnetMMIOWriteU16(pThis, GCPhysAddr, *(uint16_t *)pv); break;
                case 4:  pcnetMMIOWriteU32(pThis, GCPhysAddr, *(uint32_t *)pv); break;
                default:
                    rc = PDMDeviceDBGFStop(pThis->CTX_SUFF(pDevIns), RT_SRC_POS,
                                           "pcnetMMIOWrite: unsupported op size: address=%RGp cb=%u\n",
                                           GCPhysAddr, cb);
            }
            PDMCritSectLeave(&pThis->CritSect);
        }
        // else rc == VINF_IOM_HC_MMIO_WRITE => handle in ring3

        STAM_PROFILE_ADV_STOP(&pThis->CTXSUFF(StatMMIOWrite), a);
    }
    LogFlow(("#%d pcnetMMIOWrite: pvUser=%p:{%.*Rhxs} cb=%d GCPhysAddr=%RGp rc=%Rrc\n",
             PCNET_INST_NR, pv, cb, pv, cb, GCPhysAddr, rc));
#ifdef LOG_ENABLED
    if (rc == VINF_IOM_HC_MMIO_WRITE)
        LogFlow(("#%d => HC\n", PCNET_INST_NR));
#endif
    return rc;
}


#ifdef IN_RING3
/**
 * Device timer callback function.
 *
 * @param   pDevIns         Device instance of the device which registered the timer.
 * @param   pTimer          The timer handle.
 * @thread  EMT
 */
static DECLCALLBACK(void) pcnetTimer(PPDMDEVINS pDevIns, PTMTIMER pTimer, void *pvUser)
{
    PCNetState *pThis = (PCNetState *)pvUser;
    STAM_PROFILE_ADV_START(&pThis->StatTimer, a);
    pcnetPollTimer(pThis);
    STAM_PROFILE_ADV_STOP(&pThis->StatTimer, a);
}


/**
 * Software interrupt timer callback function.
 *
 * @param   pDevIns         Device instance of the device which registered the timer.
 * @param   pTimer          The timer handle.
 * @thread  EMT
 */
static DECLCALLBACK(void) pcnetTimerSoftInt(PPDMDEVINS pDevIns, PTMTIMER pTimer, void *pvUser)
{
    PCNetState *pThis = (PCNetState *)pvUser;

/** @todo why aren't we taking any critsect here?!? */
    pThis->aCSR[7] |= 0x0800; /* STINT */
    pcnetUpdateIrq(pThis);
    TMTimerSetNano(pThis->CTX_SUFF(pTimerSoftInt), 12800U * (pThis->aBCR[BCR_STVAL] & 0xffff));
}


/**
 * Restore timer callback.
 *
 * This is only called when've restored a saved state and temporarily
 * disconnected the network link to inform the guest that network connections
 * should be considered lost.
 *
 * @param   pDevIns         Device instance of the device which registered the timer.
 * @param   pTimer          The timer handle.
 */
static DECLCALLBACK(void) pcnetTimerRestore(PPDMDEVINS pDevIns, PTMTIMER pTimer, void *pvUser)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    int         rc = PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);
    AssertReleaseRC(rc);

    rc = VERR_GENERAL_FAILURE;
    if (pThis->cLinkDownReported <= PCNET_MAX_LINKDOWN_REPORTED)
        rc = TMTimerSetMillies(pThis->pTimerRestore, 1500);
    if (RT_FAILURE(rc))
    {
        pThis->fLinkTempDown = false;
        if (pThis->fLinkUp)
        {
            LogRel(("PCNet#%d: The link is back up again after the restore.\n",
                    pDevIns->iInstance));
            Log(("#%d pcnetTimerRestore: Clearing ERR and CERR after load. cLinkDownReported=%d\n",
                 pDevIns->iInstance, pThis->cLinkDownReported));
            pThis->aCSR[0] &= ~(RT_BIT(15) | RT_BIT(13)); /* ERR | CERR - probably not 100% correct either... */
            pThis->Led.Actual.s.fError = 0;
        }
    }
    else
        Log(("#%d pcnetTimerRestore: cLinkDownReported=%d, wait another 1500ms...\n",
             pDevIns->iInstance, pThis->cLinkDownReported));

    PDMCritSectLeave(&pThis->CritSect);
}

/**
 * Callback function for mapping an PCI I/O region.
 *
 * @return VBox status code.
 * @param   pPciDev         Pointer to PCI device. Use pPciDev->pDevIns to get the device instance.
 * @param   iRegion         The region number.
 * @param   GCPhysAddress   Physical address of the region. If iType is PCI_ADDRESS_SPACE_IO, this is an
 *                          I/O port, else it's a physical address.
 *                          This address is *NOT* relative to pci_mem_base like earlier!
 * @param   cb              Region size.
 * @param   enmType         One of the PCI_ADDRESS_SPACE_* values.
 */
static DECLCALLBACK(int) pcnetIOPortMap(PPCIDEVICE pPciDev, /*unsigned*/ int iRegion,
                                        RTGCPHYS GCPhysAddress, uint32_t cb, PCIADDRESSSPACE enmType)
{
    int         rc;
    PPDMDEVINS  pDevIns = pPciDev->pDevIns;
    RTIOPORT    Port    = (RTIOPORT)GCPhysAddress;
    PCNetState *pThis   = PCIDEV_2_PCNETSTATE(pPciDev);

    Assert(enmType == PCI_ADDRESS_SPACE_IO);
    Assert(cb >= 0x20);

    rc = PDMDevHlpIOPortRegister(pDevIns, Port, 0x10, 0, pcnetIOPortAPromWrite,
                                 pcnetIOPortAPromRead, NULL, NULL, "PCNet ARPOM");
    if (RT_FAILURE(rc))
        return rc;
    rc = PDMDevHlpIOPortRegister(pDevIns, Port + 0x10, 0x10, 0, pcnetIOPortWrite,
                                 pcnetIOPortRead, NULL, NULL, "PCNet");
    if (RT_FAILURE(rc))
        return rc;

    if (pThis->fGCEnabled)
    {
        rc = PDMDevHlpIOPortRegisterGC(pDevIns, Port, 0x10, 0, "pcnetIOPortAPromWrite",
                                       "pcnetIOPortAPromRead", NULL, NULL, "PCNet aprom");
        if (RT_FAILURE(rc))
            return rc;
        rc = PDMDevHlpIOPortRegisterGC(pDevIns, Port + 0x10, 0x10, 0, "pcnetIOPortWrite",
                                       "pcnetIOPortRead", NULL, NULL, "PCNet");
        if (RT_FAILURE(rc))
            return rc;
    }
    if (pThis->fR0Enabled)
    {
        rc = PDMDevHlpIOPortRegisterR0(pDevIns, Port, 0x10, 0, "pcnetIOPortAPromWrite",
                                       "pcnetIOPortAPromRead", NULL, NULL, "PCNet aprom");
        if (RT_FAILURE(rc))
            return rc;
        rc = PDMDevHlpIOPortRegisterR0(pDevIns, Port + 0x10, 0x10, 0, "pcnetIOPortWrite",
                                       "pcnetIOPortRead", NULL, NULL, "PCNet");
        if (RT_FAILURE(rc))
            return rc;
    }

    pThis->IOPortBase = Port;
    return VINF_SUCCESS;
}


/**
 * Callback function for mapping the MMIO region.
 *
 * @return VBox status code.
 * @param   pPciDev         Pointer to PCI device. Use pPciDev->pDevIns to get the device instance.
 * @param   iRegion         The region number.
 * @param   GCPhysAddress   Physical address of the region. If iType is PCI_ADDRESS_SPACE_IO, this is an
 *                          I/O port, else it's a physical address.
 *                          This address is *NOT* relative to pci_mem_base like earlier!
 * @param   cb              Region size.
 * @param   enmType         One of the PCI_ADDRESS_SPACE_* values.
 */
static DECLCALLBACK(int) pcnetMMIOMap(PPCIDEVICE pPciDev, /*unsigned*/ int iRegion,
                                      RTGCPHYS GCPhysAddress, uint32_t cb, PCIADDRESSSPACE enmType)
{
    PCNetState *pThis = PCIDEV_2_PCNETSTATE(pPciDev);
    int         rc;

    Assert(enmType == PCI_ADDRESS_SPACE_MEM);
    Assert(cb >= PCNET_PNPMMIO_SIZE);

    /* We use the assigned size here, because we currently only support page aligned MMIO ranges. */
    rc = PDMDevHlpMMIORegister(pPciDev->pDevIns, GCPhysAddress, cb, pThis,
                               pcnetMMIOWrite, pcnetMMIORead, NULL, "PCNet");
    if (RT_FAILURE(rc))
        return rc;
    pThis->MMIOBase = GCPhysAddress;
    return rc;
}


/**
 * Callback function for mapping the MMIO region.
 *
 * @return VBox status code.
 * @param   pPciDev         Pointer to PCI device. Use pPciDev->pDevIns to get the device instance.
 * @param   iRegion         The region number.
 * @param   GCPhysAddress   Physical address of the region. If iType is PCI_ADDRESS_SPACE_IO, this is an
 *                          I/O port, else it's a physical address.
 *                          This address is *NOT* relative to pci_mem_base like earlier!
 * @param   cb              Region size.
 * @param   enmType         One of the PCI_ADDRESS_SPACE_* values.
 */
static DECLCALLBACK(int) pcnetMMIOSharedMap(PPCIDEVICE pPciDev, /*unsigned*/ int iRegion,
                                            RTGCPHYS GCPhysAddress, uint32_t cb, PCIADDRESSSPACE enmType)
{
    if (GCPhysAddress != NIL_RTGCPHYS)
        return PDMDevHlpMMIO2Map(pPciDev->pDevIns, iRegion, GCPhysAddress);

    /* nothing to clean up */
    return VINF_SUCCESS;
}


/**
 * PCNET status info callback.
 *
 * @param   pDevIns     The device instance.
 * @param   pHlp        The output helpers.
 * @param   pszArgs     The arguments.
 */
static DECLCALLBACK(void) pcnetInfo(PPDMDEVINS pDevIns, PCDBGFINFOHLP pHlp, const char *pszArgs)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    bool        fRcvRing = false;
    bool        fXmtRing = false;

    /*
     * Parse args.
     */
    if (pszArgs)
    {
        fRcvRing = strstr(pszArgs, "verbose") || strstr(pszArgs, "rcv");
        fXmtRing = strstr(pszArgs, "verbose") || strstr(pszArgs, "xmt");
    }

    /*
     * Show info.
     */
    pHlp->pfnPrintf(pHlp,
                    "pcnet #%d: port=%RTiop mmio=%RX32 mac-cfg=%RTmac %s\n",
                    pDevIns->iInstance,
                    pThis->IOPortBase, pThis->MMIOBase, &pThis->MacConfigured,
                    pThis->fAm79C973 ? "Am79C973" : "Am79C970A", pThis->fGCEnabled ? " GC" : "", pThis->fR0Enabled ? " R0" : "");

    PDMCritSectEnter(&pThis->CritSect, VERR_INTERNAL_ERROR); /* Take it here so we know why we're hanging... */

    pHlp->pfnPrintf(pHlp,
                    "CSR0=%#06x:\n",
                    pThis->aCSR[0]);

    pHlp->pfnPrintf(pHlp,
                    "CSR1=%#06x:\n",
                    pThis->aCSR[1]);

    pHlp->pfnPrintf(pHlp,
                    "CSR2=%#06x:\n",
                    pThis->aCSR[2]);

    pHlp->pfnPrintf(pHlp,
                    "CSR3=%#06x: BSWP=%d EMBA=%d DXMT2PD=%d LAPPEN=%d DXSUFLO=%d IDONM=%d TINTM=%d RINTM=%d MERRM=%d MISSM=%d BABLM=%d\n",
                    pThis->aCSR[3],
                    !!(pThis->aCSR[3] & RT_BIT(2)), !!(pThis->aCSR[3] & RT_BIT(3)), !!(pThis->aCSR[3] & RT_BIT(4)), CSR_LAPPEN(pThis),
                    CSR_DXSUFLO(pThis), !!(pThis->aCSR[3] & RT_BIT(8)), !!(pThis->aCSR[3] & RT_BIT(9)), !!(pThis->aCSR[3] & RT_BIT(10)),
                    !!(pThis->aCSR[3] & RT_BIT(11)), !!(pThis->aCSR[3] & RT_BIT(12)), !!(pThis->aCSR[3] & RT_BIT(14)));

    pHlp->pfnPrintf(pHlp,
                    "CSR4=%#06x: JABM=%d JAB=%d TXSTRM=%d TXSTRT=%d RCVCOOM=%d RCVCCO=%d UINT=%d UINTCMD=%d\n"
                    "              MFCOM=%d MFCO=%d ASTRP_RCV=%d APAD_XMT=%d DPOLL=%d TIMER=%d EMAPLUS=%d EN124=%d\n",
                    pThis->aCSR[4],
                    !!(pThis->aCSR[4] & RT_BIT( 0)), !!(pThis->aCSR[4] & RT_BIT( 1)), !!(pThis->aCSR[4] & RT_BIT( 2)), !!(pThis->aCSR[4] & RT_BIT( 3)),
                    !!(pThis->aCSR[4] & RT_BIT( 4)), !!(pThis->aCSR[4] & RT_BIT( 5)), !!(pThis->aCSR[4] & RT_BIT( 6)), !!(pThis->aCSR[4] & RT_BIT( 7)),
                    !!(pThis->aCSR[4] & RT_BIT( 8)), !!(pThis->aCSR[4] & RT_BIT( 9)), !!(pThis->aCSR[4] & RT_BIT(10)), !!(pThis->aCSR[4] & RT_BIT(11)),
                    !!(pThis->aCSR[4] & RT_BIT(12)), !!(pThis->aCSR[4] & RT_BIT(13)), !!(pThis->aCSR[4] & RT_BIT(14)), !!(pThis->aCSR[4] & RT_BIT(15)));

    pHlp->pfnPrintf(pHlp,
                    "CSR5=%#06x:\n",
                    pThis->aCSR[5]);

    pHlp->pfnPrintf(pHlp,
                    "CSR6=%#06x: RLEN=%#x* TLEN=%#x* [* encoded]\n",
                    pThis->aCSR[6],
                    (pThis->aCSR[6] >> 8) & 0xf, (pThis->aCSR[6] >> 12) & 0xf);

    pHlp->pfnPrintf(pHlp,
                    "CSR8..11=%#06x,%#06x,%#06x,%#06x: LADRF=%#018llx\n",
                    pThis->aCSR[8], pThis->aCSR[9], pThis->aCSR[10], pThis->aCSR[11],
                      (uint64_t)(pThis->aCSR[ 8] & 0xffff)
                    | (uint64_t)(pThis->aCSR[ 9] & 0xffff) << 16
                    | (uint64_t)(pThis->aCSR[10] & 0xffff) << 32
                    | (uint64_t)(pThis->aCSR[11] & 0xffff) << 48);

    pHlp->pfnPrintf(pHlp,
                    "CSR12..14=%#06x,%#06x,%#06x: PADR=%02x:%02x:%02x:%02x:%02x:%02x (Current MAC Address)\n",
                    pThis->aCSR[12], pThis->aCSR[13], pThis->aCSR[14],
                     pThis->aCSR[12]       & 0xff,
                    (pThis->aCSR[12] >> 8) & 0xff,
                     pThis->aCSR[13]       & 0xff,
                    (pThis->aCSR[13] >> 8) & 0xff,
                     pThis->aCSR[14]       & 0xff,
                    (pThis->aCSR[14] >> 8) & 0xff);

    pHlp->pfnPrintf(pHlp,
                    "CSR15=%#06x: DXR=%d DTX=%d LOOP=%d DXMTFCS=%d FCOLL=%d DRTY=%d INTL=%d PORTSEL=%d LTR=%d\n"
                    "              MENDECL=%d DAPC=%d DLNKTST=%d DRCVPV=%d DRCVBC=%d PROM=%d\n",
                    pThis->aCSR[15],
                    !!(pThis->aCSR[15] & RT_BIT( 0)), !!(pThis->aCSR[15] & RT_BIT( 1)), !!(pThis->aCSR[15] & RT_BIT( 2)), !!(pThis->aCSR[15] & RT_BIT( 3)),
                    !!(pThis->aCSR[15] & RT_BIT( 4)), !!(pThis->aCSR[15] & RT_BIT( 5)), !!(pThis->aCSR[15] & RT_BIT( 6)),   (pThis->aCSR[15] >> 7) & 3,
                                                   !!(pThis->aCSR[15] & RT_BIT( 9)), !!(pThis->aCSR[15] & RT_BIT(10)), !!(pThis->aCSR[15] & RT_BIT(11)),
                    !!(pThis->aCSR[15] & RT_BIT(12)), !!(pThis->aCSR[15] & RT_BIT(13)), !!(pThis->aCSR[15] & RT_BIT(14)), !!(pThis->aCSR[15] & RT_BIT(15)));

    pHlp->pfnPrintf(pHlp,
                    "CSR46=%#06x: POLL=%#06x (Poll Time Counter)\n",
                    pThis->aCSR[46], pThis->aCSR[46] & 0xffff);

    pHlp->pfnPrintf(pHlp,
                    "CSR47=%#06x: POLLINT=%#06x (Poll Time Interval)\n",
                    pThis->aCSR[47], pThis->aCSR[47] & 0xffff);

    pHlp->pfnPrintf(pHlp,
                    "CSR58=%#06x: SWSTYLE=%d %s SSIZE32=%d CSRPCNET=%d APERRENT=%d\n",
                    pThis->aCSR[58],
                    pThis->aCSR[58] & 0x7f,
                    (pThis->aCSR[58] & 0x7f) == 0 ? "C-LANCE / PCnet-ISA"
                    : (pThis->aCSR[58] & 0x7f) == 1 ? "ILACC"
                    : (pThis->aCSR[58] & 0x7f) == 2 ? "PCNet-PCI II"
                    : (pThis->aCSR[58] & 0x7f) == 3 ? "PCNet-PCI II controller"
                    : "!!reserved!!",
                    !!(pThis->aCSR[58] & RT_BIT(8)), !!(pThis->aCSR[58] & RT_BIT(9)), !!(pThis->aCSR[58] & RT_BIT(10)));

    pHlp->pfnPrintf(pHlp,
                    "CSR112=%04RX32: MFC=%04x (Missed receive Frame Count)\n",
                    pThis->aCSR[112], pThis->aCSR[112] & 0xffff);

    pHlp->pfnPrintf(pHlp,
                    "CSR122=%04RX32: RCVALGN=%04x (Receive Frame Align)\n",
                    pThis->aCSR[122], !!(pThis->aCSR[122] & RT_BIT(0)));

    pHlp->pfnPrintf(pHlp,
                    "CSR124=%04RX32: RPA=%04x (Runt Packet Accept)\n",
                    pThis->aCSR[122], !!(pThis->aCSR[122] & RT_BIT(3)));


    /*
     * Dump the receive ring.
     */
    pHlp->pfnPrintf(pHlp,
                    "RCVRL=%04x RCVRC=%04x  GCRDRA=%RX32 \n"
                    "CRDA=%08RX32 CRBA=%08RX32 CRBC=%03x CRST=%04x\n"
                    "NRDA=%08RX32 NRBA=%08RX32 NRBC=%03x NRST=%04x\n"
                    "NNRDA=%08RX32\n"
                    ,
                    CSR_RCVRL(pThis), CSR_RCVRC(pThis), pThis->GCRDRA,
                    CSR_CRDA(pThis), CSR_CRBA(pThis), CSR_CRBC(pThis), CSR_CRST(pThis),
                    CSR_NRDA(pThis), CSR_NRBA(pThis), CSR_NRBC(pThis), CSR_NRST(pThis),
                    CSR_NNRD(pThis));
    if (fRcvRing)
    {
        const unsigned  cb = 1 << pThis->iLog2DescSize;
        RTGCPHYS32      GCPhys = pThis->GCRDRA;
        unsigned        i = CSR_RCVRL(pThis);
        while (i-- > 0)
        {
            RMD rmd;
            pcnetRmdLoad(pThis, &rmd, PHYSADDR(pThis, GCPhys), false);
            pHlp->pfnPrintf(pHlp,
                            "%04x %RX32:%c%c RBADR=%08RX32 BCNT=%03x MCNT=%03x "
                            "OWN=%d ERR=%d FRAM=%d OFLO=%d CRC=%d BUFF=%d STP=%d ENP=%d BPE=%d "
                            "PAM=%d LAFM=%d BAM=%d RCC=%02x RPC=%02x ONES=%#x ZEROS=%d\n",
                            i, GCPhys, i + 1 == CSR_RCVRC(pThis) ? '*' : ' ', GCPhys == CSR_CRDA(pThis) ? '*' : ' ',
                            rmd.rmd0.rbadr, 4096 - rmd.rmd1.bcnt, rmd.rmd2.mcnt,
                            rmd.rmd1.own, rmd.rmd1.err, rmd.rmd1.fram, rmd.rmd1.oflo, rmd.rmd1.crc, rmd.rmd1.buff,
                            rmd.rmd1.stp, rmd.rmd1.enp, rmd.rmd1.bpe,
                            rmd.rmd1.pam, rmd.rmd1.lafm, rmd.rmd1.bam, rmd.rmd2.rcc, rmd.rmd2.rpc,
                            rmd.rmd1.ones, rmd.rmd2.zeros);

            GCPhys += cb;
        }
    }

    /*
     * Dump the transmit ring.
     */
    pHlp->pfnPrintf(pHlp,
                    "XMTRL=%04x XMTRC=%04x  GCTDRA=%08RX32 BADX=%08RX32\n"
                    "PXDA=%08RX32               PXBC=%03x PXST=%04x\n"
                    "CXDA=%08RX32 CXBA=%08RX32 CXBC=%03x CXST=%04x\n"
                    "NXDA=%08RX32 NXBA=%08RX32 NXBC=%03x NXST=%04x\n"
                    "NNXDA=%08RX32\n"
                    ,
                    CSR_XMTRL(pThis), CSR_XMTRC(pThis),
                    pThis->GCTDRA, CSR_BADX(pThis),
                    CSR_PXDA(pThis),                  CSR_PXBC(pThis), CSR_PXST(pThis),
                    CSR_CXDA(pThis), CSR_CXBA(pThis), CSR_CXBC(pThis), CSR_CXST(pThis),
                    CSR_NXDA(pThis), CSR_NXBA(pThis), CSR_NXBC(pThis), CSR_NXST(pThis),
                    CSR_NNXD(pThis));
    if (fXmtRing)
    {
        const unsigned  cb = 1 << pThis->iLog2DescSize;
        RTGCPHYS32      GCPhys = pThis->GCTDRA;
        unsigned        i = CSR_XMTRL(pThis);
        while (i-- > 0)
        {
            TMD tmd;
            pcnetTmdLoad(pThis, &tmd, PHYSADDR(pThis, GCPhys), false);
            pHlp->pfnPrintf(pHlp,
                            "%04x %RX32:%c%c TBADR=%08RX32 BCNT=%03x OWN=%d "
                            "ERR=%d NOFCS=%d LTINT=%d ONE=%d DEF=%d STP=%d ENP=%d BPE=%d "
                            "BUFF=%d UFLO=%d EXDEF=%d LCOL=%d LCAR=%d RTRY=%d TDR=%03x TRC=%#x ONES=%#x\n"
                            ,
                            i, GCPhys, i + 1 == CSR_XMTRC(pThis) ? '*' : ' ', GCPhys == CSR_CXDA(pThis) ? '*' : ' ',
                            tmd.tmd0.tbadr, 4096 - tmd.tmd1.bcnt,
                            tmd.tmd2.tdr,
                            tmd.tmd2.trc,
                            tmd.tmd1.own,
                            tmd.tmd1.err,
                            tmd.tmd1.nofcs,
                            tmd.tmd1.ltint,
                            tmd.tmd1.one,
                            tmd.tmd1.def,
                            tmd.tmd1.stp,
                            tmd.tmd1.enp,
                            tmd.tmd1.bpe,
                            tmd.tmd2.buff,
                            tmd.tmd2.uflo,
                            tmd.tmd2.exdef,
                            tmd.tmd2.lcol,
                            tmd.tmd2.lcar,
                            tmd.tmd2.rtry,
                            tmd.tmd2.tdr,
                            tmd.tmd2.trc,
                            tmd.tmd1.ones);

            GCPhys += cb;
        }
    }

    PDMCritSectLeave(&pThis->CritSect);
}


/**
 * Takes down the link temporarily if it's current status is up.
 *
 * This is used during restore and when replumbing the network link.
 *
 * The temporary link outage is supposed to indicate to the OS that all network
 * connections have been lost and that it for instance is appropriate to
 * renegotiate any DHCP lease.
 *
 * @param  pThis        The PCNet instance data.
 */
static void pcnetTempLinkDown(PCNetState *pThis)
{
    if (pThis->fLinkUp)
    {
        pThis->fLinkTempDown = true;
        pThis->cLinkDownReported = 0;
        pThis->aCSR[0] |= RT_BIT(15) | RT_BIT(13); /* ERR | CERR (this is probably wrong) */
        pThis->Led.Asserted.s.fError = pThis->Led.Actual.s.fError = 1;
        int rc = TMTimerSetMillies(pThis->pTimerRestore, 5000);
        AssertRC(rc);
    }
}


/**
 * Saves the configuration.
 *
 * @param   pThis       The PCNet instance data.
 * @param   pSSM        The saved state handle.
 */
static void pcnetSaveConfig(PCNetState *pThis, PSSMHANDLE pSSM)
{
    SSMR3PutMem(pSSM, &pThis->MacConfigured, sizeof(pThis->MacConfigured));
    SSMR3PutBool(pSSM, pThis->fAm79C973);                   /* >= If version 0.8 */
    SSMR3PutU32(pSSM, pThis->u32LinkSpeed);
}


/**
 * Live Save, pass 0.
 *
 * @returns VBox status code.
 * @param   pDevIns     The device instance.
 * @param   pSSM        The saved state handle.
 * @param   uPass       The pass number.
 */
static DECLCALLBACK(int) pcnetLiveExec(PPDMDEVINS pDevIns, PSSMHANDLE pSSM, uint32_t uPass)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    pcnetSaveConfig(pThis, pSSM);
    return VINF_SUCCESS;
}


/**
 * Serializes the receive thread, it may be working inside the critsect.
 *
 * @returns VBox status code.
 * @param   pDevIns     The device instance.
 * @param   pSSM        The saved state handle.
 */
static DECLCALLBACK(int) pcnetSavePrep(PPDMDEVINS pDevIns, PSSMHANDLE pSSM)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);

    int rc = PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);
    AssertRC(rc);
    PDMCritSectLeave(&pThis->CritSect);

    return VINF_SUCCESS;
}


/**
 * Saves a state of the PC-Net II device.
 *
 * @returns VBox status code.
 * @param   pDevIns     The device instance.
 * @param   pSSM        The saved state handle.
 */
static DECLCALLBACK(int) pcnetSaveExec(PPDMDEVINS pDevIns, PSSMHANDLE pSSM)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);

    SSMR3PutBool(pSSM, pThis->fLinkUp);
    SSMR3PutU32(pSSM, pThis->u32RAP);
    SSMR3PutS32(pSSM, pThis->iISR);
    SSMR3PutU32(pSSM, pThis->u32Lnkst);
    SSMR3PutBool(pSSM, pThis->fPrivIfEnabled);              /* >= If version 0.9 */
    SSMR3PutBool(pSSM, pThis->fSignalRxMiss);               /* >= If version 0.10 */
    SSMR3PutGCPhys32(pSSM, pThis->GCRDRA);
    SSMR3PutGCPhys32(pSSM, pThis->GCTDRA);
    SSMR3PutMem(pSSM, pThis->aPROM, sizeof(pThis->aPROM));
    SSMR3PutMem(pSSM, pThis->aCSR, sizeof(pThis->aCSR));
    SSMR3PutMem(pSSM, pThis->aBCR, sizeof(pThis->aBCR));
    SSMR3PutMem(pSSM, pThis->aMII, sizeof(pThis->aMII));
    SSMR3PutU16(pSSM, pThis->u16CSR0LastSeenByGuest);
    SSMR3PutU64(pSSM, pThis->u64LastPoll);
    pcnetSaveConfig(pThis, pSSM);

    int rc = VINF_SUCCESS;
#ifndef PCNET_NO_POLLING
    rc = TMR3TimerSave(pThis->CTX_SUFF(pTimerPoll), pSSM);
    if (RT_FAILURE(rc))
        return rc;
#endif
    if (pThis->fAm79C973)
        rc = TMR3TimerSave(pThis->CTX_SUFF(pTimerSoftInt), pSSM);
    return rc;
}


/**
 * Serializes the receive thread, it may be working inside the critsect.
 *
 * @returns VBox status code.
 * @param   pDevIns     The device instance.
 * @param   pSSM        The saved state handle.
 */
static DECLCALLBACK(int) pcnetLoadPrep(PPDMDEVINS pDevIns, PSSMHANDLE pSSM)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);

    int rc = PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);
    AssertRC(rc);
    PDMCritSectLeave(&pThis->CritSect);

    return VINF_SUCCESS;
}


/**
 * Loads a saved PC-Net II device state.
 *
 * @returns VBox status code.
 * @param   pDevIns     The device instance.
 * @param   pSSM  The handle to the saved state.
 * @param   uVersion  The data unit version number.
 * @param   uPass       The data pass.
 */
static DECLCALLBACK(int) pcnetLoadExec(PPDMDEVINS pDevIns, PSSMHANDLE pSSM, uint32_t uVersion, uint32_t uPass)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);

    if (   SSM_VERSION_MAJOR_CHANGED(uVersion, PCNET_SAVEDSTATE_VERSION)
        || SSM_VERSION_MINOR(uVersion) < 7)
        return VERR_SSM_UNSUPPORTED_DATA_UNIT_VERSION;

    if (uPass == SSM_PASS_FINAL)
    {
        /* restore data */
        SSMR3GetBool(pSSM, &pThis->fLinkUp);
        SSMR3GetU32(pSSM, &pThis->u32RAP);
        SSMR3GetS32(pSSM, &pThis->iISR);
        SSMR3GetU32(pSSM, &pThis->u32Lnkst);
        if (   SSM_VERSION_MAJOR(uVersion) >  0
            || SSM_VERSION_MINOR(uVersion) >= 9)
        {
            SSMR3GetBool(pSSM, &pThis->fPrivIfEnabled);
            if (pThis->fPrivIfEnabled)
                LogRel(("PCNet#%d: Enabling private interface\n", PCNET_INST_NR));
        }
        if (   SSM_VERSION_MAJOR(uVersion) >  0
            || SSM_VERSION_MINOR(uVersion) >= 10)
        {
            SSMR3GetBool(pSSM, &pThis->fSignalRxMiss);
        }
        SSMR3GetGCPhys32(pSSM, &pThis->GCRDRA);
        SSMR3GetGCPhys32(pSSM, &pThis->GCTDRA);
        SSMR3GetMem(pSSM, &pThis->aPROM, sizeof(pThis->aPROM));
        SSMR3GetMem(pSSM, &pThis->aCSR, sizeof(pThis->aCSR));
        SSMR3GetMem(pSSM, &pThis->aBCR, sizeof(pThis->aBCR));
        SSMR3GetMem(pSSM, &pThis->aMII, sizeof(pThis->aMII));
        SSMR3GetU16(pSSM, &pThis->u16CSR0LastSeenByGuest);
        SSMR3GetU64(pSSM, &pThis->u64LastPoll);
    }

    /* check config */
    RTMAC       Mac;
    int rc = SSMR3GetMem(pSSM, &Mac, sizeof(Mac));
    AssertRCReturn(rc, rc);
    if (    memcmp(&Mac, &pThis->MacConfigured, sizeof(Mac))
        && (uPass == 0 || !PDMDevHlpVMTeleportedAndNotFullyResumedYet(pDevIns)) )
        LogRel(("PCNet#%u: The mac address differs: config=%RTmac saved=%RTmac\n", PCNET_INST_NR, &pThis->MacConfigured, &Mac));

    bool        fAm79C973;
    rc = SSMR3GetBool(pSSM, &fAm79C973);
    AssertRCReturn(rc, rc);
    if (pThis->fAm79C973 != fAm79C973)
    {
        LogRel(("PCNet#%u: The fAm79C973 flag differs: config=%RTbool saved=%RTbool\n", PCNET_INST_NR, pThis->fAm79C973, fAm79C973));
        return VERR_SSM_LOAD_CONFIG_MISMATCH;
    }

    uint32_t    u32LinkSpeed;
    rc = SSMR3GetU32(pSSM, &u32LinkSpeed);
    AssertRCReturn(rc, rc);
    if (    pThis->u32LinkSpeed != u32LinkSpeed
        && (uPass == 0 || !PDMDevHlpVMTeleportedAndNotFullyResumedYet(pDevIns)) )
        LogRel(("PCNet#%u: The mac link speed differs: config=%u saved=%u\n", PCNET_INST_NR, pThis->u32LinkSpeed, u32LinkSpeed));

    if (uPass == SSM_PASS_FINAL)
    {
        /* restore timers and stuff */
#ifndef PCNET_NO_POLLING
        TMR3TimerLoad(pThis->CTX_SUFF(pTimerPoll), pSSM);
#endif
        if (pThis->fAm79C973)
        {
            if (   SSM_VERSION_MAJOR(uVersion) >  0
                || SSM_VERSION_MINOR(uVersion) >= 8)
                TMR3TimerLoad(pThis->CTX_SUFF(pTimerSoftInt), pSSM);
        }

        pThis->iLog2DescSize = BCR_SWSTYLE(pThis)
                             ? 4
                             : 3;
        pThis->GCUpperPhys   = BCR_SSIZE32(pThis)
                             ? 0
                             : (0xff00 & (uint32_t)pThis->aCSR[2]) << 16;

        /* update promiscuous mode. */
        if (pThis->pDrv)
            pThis->pDrv->pfnSetPromiscuousMode(pThis->pDrv, CSR_PROM(pThis));

#ifdef PCNET_NO_POLLING
        /* Enable physical monitoring again (!) */
        pcnetUpdateRingHandlers(pThis);
#endif
        /* Indicate link down to the guest OS that all network connections have
           been lost, unless we've been teleported here. */
        if (!PDMDevHlpVMTeleportedAndNotFullyResumedYet(pDevIns))
            pcnetTempLinkDown(pThis);
    }

    return VINF_SUCCESS;
}


/**
 * Queries an interface to the driver.
 *
 * @returns Pointer to interface.
 * @returns NULL if the interface was not supported by the driver.
 * @param   pInterface          Pointer to this interface structure.
 * @param   enmInterface        The requested interface identification.
 * @thread  Any thread.
 */
static DECLCALLBACK(void *) pcnetQueryInterface(struct PDMIBASE *pInterface, PDMINTERFACE enmInterface)
{
    PCNetState *pThis = (PCNetState *)((uintptr_t)pInterface - RT_OFFSETOF(PCNetState, IBase));
    Assert(&pThis->IBase == pInterface);
    switch (enmInterface)
    {
        case PDMINTERFACE_BASE:
            return &pThis->IBase;
        case PDMINTERFACE_NETWORK_PORT:
            return &pThis->INetworkPort;
        case PDMINTERFACE_NETWORK_CONFIG:
            return &pThis->INetworkConfig;
        case PDMINTERFACE_LED_PORTS:
            return &pThis->ILeds;
        default:
            return NULL;
    }
}

/** Converts a pointer to PCNetState::INetworkPort to a PCNetState pointer. */
#define INETWORKPORT_2_DATA(pInterface)  ( (PCNetState *)((uintptr_t)pInterface - RT_OFFSETOF(PCNetState, INetworkPort)) )


/**
 * Check if the device/driver can receive data now.
 * This must be called before the pfnRecieve() method is called.
 *
 * @returns VBox status code.
 * @param   pInterface      Pointer to the interface structure containing the called function pointer.
 */
static int pcnetCanReceive(PCNetState *pThis)
{
    int rc = PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);
    AssertReleaseRC(rc);

    rc = VERR_NET_NO_BUFFER_SPACE;

    if (RT_LIKELY(!CSR_DRX(pThis) && !CSR_STOP(pThis) && !CSR_SPND(pThis)))
    {
        if (HOST_IS_OWNER(CSR_CRST(pThis)) && pThis->GCRDRA)
            pcnetRdtePoll(pThis);

        if (RT_UNLIKELY(HOST_IS_OWNER(CSR_CRST(pThis))))
        {
            /** @todo Notify the guest _now_. Will potentially increase the interrupt load */
            if (pThis->fSignalRxMiss)
                pThis->aCSR[0] |= 0x1000; /* Set MISS flag */
        }
        else
            rc = VINF_SUCCESS;
    }

    PDMCritSectLeave(&pThis->CritSect);
    return rc;
}


/**
 *
 */
static DECLCALLBACK(int) pcnetWaitReceiveAvail(PPDMINETWORKPORT pInterface, unsigned cMillies)
{
    PCNetState *pThis = INETWORKPORT_2_DATA(pInterface);

    int rc = pcnetCanReceive(pThis);
    if (RT_SUCCESS(rc))
        return VINF_SUCCESS;
    if (RT_UNLIKELY(cMillies == 0))
        return VERR_NET_NO_BUFFER_SPACE;

    rc = VERR_INTERRUPTED;
    ASMAtomicXchgBool(&pThis->fMaybeOutOfSpace, true);
    STAM_PROFILE_START(&pThis->StatRxOverflow, a);
    VMSTATE enmVMState;
    while (RT_LIKELY(   (enmVMState = PDMDevHlpVMState(pThis->CTX_SUFF(pDevIns))) == VMSTATE_RUNNING
                     || enmVMState == VMSTATE_RUNNING_LS))
    {
        int rc2 = pcnetCanReceive(pThis);
        if (RT_SUCCESS(rc2))
        {
            rc = VINF_SUCCESS;
            break;
        }
        LogFlow(("pcnetWaitReceiveAvail: waiting cMillies=%u...\n", cMillies));
        /* Start the poll timer once which will remain active as long fMaybeOutOfSpace
         * is true -- even if (transmit) polling is disabled (CSR_DPOLL). */
        rc2 = PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);
        AssertReleaseRC(rc2);
        pcnetPollTimerStart(pThis);
        PDMCritSectLeave(&pThis->CritSect);
        RTSemEventWait(pThis->hEventOutOfRxSpace, cMillies);
    }
    STAM_PROFILE_STOP(&pThis->StatRxOverflow, a);
    ASMAtomicXchgBool(&pThis->fMaybeOutOfSpace, false);

    return rc;
}


/**
 * Receive data from the network.
 *
 * @returns VBox status code.
 * @param   pInterface      Pointer to the interface structure containing the called function pointer.
 * @param   pvBuf           The available data.
 * @param   cb              Number of bytes available in the buffer.
 * @thread  EMT
 */
static DECLCALLBACK(int) pcnetReceive(PPDMINETWORKPORT pInterface, const void *pvBuf, size_t cb)
{
    PCNetState *pThis = INETWORKPORT_2_DATA(pInterface);
    int         rc;

    STAM_PROFILE_ADV_START(&pThis->StatReceive, a);
    rc = PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);
    AssertReleaseRC(rc);

    /*
     * Check for the max ethernet frame size, taking the IEEE 802.1Q (VLAN) tag into
     * account. Note that we are *not* expecting the CRC Checksum.
     * Ethernet frames consists of a 14-byte header [+ 4-byte vlan tag] + a 1500-byte body.
     */
    if (RT_LIKELY(   cb <= 1514
                  || (   cb <= 1518
                      && ((PCRTNETETHERHDR)pvBuf)->EtherType == RT_H2BE_U16_C(RTNET_ETHERTYPE_VLAN))))
    {
        if (cb > 70) /* unqualified guess */
            pThis->Led.Asserted.s.fReading = pThis->Led.Actual.s.fReading = 1;
        pcnetReceiveNoSync(pThis, (const uint8_t *)pvBuf, cb);
        pThis->Led.Actual.s.fReading = 0;
    }
#ifdef LOG_ENABLED
    else
    {
        static bool s_fFirstBigFrameLoss = true;
        unsigned cbMaxFrame = ((PCRTNETETHERHDR)pvBuf)->EtherType == RT_H2BE_U16_C(RTNET_ETHERTYPE_VLAN)
                            ? 1518 : 1514;
        if (s_fFirstBigFrameLoss)
        {
            s_fFirstBigFrameLoss = false;
            Log(("PCNet#%d: Received giant frame %zu, max %u. (Further giants will be reported at level5.)\n",
                 PCNET_INST_NR, cb, cbMaxFrame));
        }
        else
            Log5(("PCNet#%d: Received giant frame %zu bytes, max %u.\n",
                  PCNET_INST_NR, cb, cbMaxFrame));
    }
#endif /* LOG_ENABLED */

    PDMCritSectLeave(&pThis->CritSect);
    STAM_PROFILE_ADV_STOP(&pThis->StatReceive, a);

    return VINF_SUCCESS;
}

/** Converts a pointer to PCNetState::INetworkConfig to a PCNetState pointer. */
#define INETWORKCONFIG_2_DATA(pInterface)  ( (PCNetState *)((uintptr_t)pInterface - RT_OFFSETOF(PCNetState, INetworkConfig)) )


/**
 * Gets the current Media Access Control (MAC) address.
 *
 * @returns VBox status code.
 * @param   pInterface      Pointer to the interface structure containing the called function pointer.
 * @param   pMac            Where to store the MAC address.
 * @thread  EMT
 */
static DECLCALLBACK(int) pcnetGetMac(PPDMINETWORKCONFIG pInterface, PRTMAC pMac)
{
    PCNetState *pThis = INETWORKCONFIG_2_DATA(pInterface);
    memcpy(pMac, pThis->aPROM, sizeof(*pMac));
    return VINF_SUCCESS;
}


/**
 * Gets the new link state.
 *
 * @returns The current link state.
 * @param   pInterface      Pointer to the interface structure containing the called function pointer.
 * @thread  EMT
 */
static DECLCALLBACK(PDMNETWORKLINKSTATE) pcnetGetLinkState(PPDMINETWORKCONFIG pInterface)
{
    PCNetState *pThis = INETWORKCONFIG_2_DATA(pInterface);
    if (pThis->fLinkUp && !pThis->fLinkTempDown)
        return PDMNETWORKLINKSTATE_UP;
    if (!pThis->fLinkUp)
        return PDMNETWORKLINKSTATE_DOWN;
    if (pThis->fLinkTempDown)
        return PDMNETWORKLINKSTATE_DOWN_RESUME;
    AssertMsgFailed(("Invalid link state!\n"));
    return PDMNETWORKLINKSTATE_INVALID;
}


/**
 * Sets the new link state.
 *
 * @returns VBox status code.
 * @param   pInterface      Pointer to the interface structure containing the called function pointer.
 * @param   enmState        The new link state
 * @thread  EMT
 */
static DECLCALLBACK(int) pcnetSetLinkState(PPDMINETWORKCONFIG pInterface, PDMNETWORKLINKSTATE enmState)
{
    PCNetState *pThis = INETWORKCONFIG_2_DATA(pInterface);
    bool fLinkUp;
    if (    enmState != PDMNETWORKLINKSTATE_DOWN
        &&  enmState != PDMNETWORKLINKSTATE_UP)
    {
        AssertMsgFailed(("Invalid parameter enmState=%d\n", enmState));
        return VERR_INVALID_PARAMETER;
    }

    /* has the state changed? */
    fLinkUp = enmState == PDMNETWORKLINKSTATE_UP;
    if (pThis->fLinkUp != fLinkUp)
    {
        pThis->fLinkUp = fLinkUp;
        if (fLinkUp)
        {
            /* connect  with a delay of 5 seconds */
            pThis->fLinkTempDown = true;
            pThis->cLinkDownReported = 0;
            pThis->aCSR[0] |= RT_BIT(15) | RT_BIT(13); /* ERR | CERR (this is probably wrong) */
            pThis->Led.Asserted.s.fError = pThis->Led.Actual.s.fError = 1;
            int rc = TMTimerSetMillies(pThis->pTimerRestore, 5000);
            AssertRC(rc);
        }
        else
        {
            /* disconnect */
            pThis->cLinkDownReported = 0;
            pThis->aCSR[0] |= RT_BIT(15) | RT_BIT(13); /* ERR | CERR (this is probably wrong) */
            pThis->Led.Asserted.s.fError = pThis->Led.Actual.s.fError = 1;
        }
        Assert(!PDMCritSectIsOwner(&pThis->CritSect));
        if (pThis->pDrv)
            pThis->pDrv->pfnNotifyLinkChanged(pThis->pDrv, enmState);
    }
    return VINF_SUCCESS;
}


/**
 * Gets the pointer to the status LED of a unit.
 *
 * @returns VBox status code.
 * @param   pInterface      Pointer to the interface structure containing the called function pointer.
 * @param   iLUN            The unit which status LED we desire.
 * @param   ppLed           Where to store the LED pointer.
 */
static DECLCALLBACK(int) pcnetQueryStatusLed(PPDMILEDPORTS pInterface, unsigned iLUN, PPDMLED *ppLed)
{
    PCNetState *pThis = (PCNetState *)( (uintptr_t)pInterface - RT_OFFSETOF(PCNetState, ILeds) );
    if (iLUN == 0)
    {
        *ppLed = &pThis->Led;
        return VINF_SUCCESS;
    }
    return VERR_PDM_LUN_NOT_FOUND;
}


/**
 * @copydoc FNPDMDEVPOWEROFF
 */
static DECLCALLBACK(void) pcnetPowerOff(PPDMDEVINS pDevIns)
{
    /* Poke thread waiting for buffer space. */
    pcnetWakeupReceive(pDevIns);
}

#ifdef VBOX_DYNAMIC_NET_ATTACH

/**
 * Detach notification.
 *
 * One port on the network card has been disconnected from the network.
 *
 * @param   pDevIns     The device instance.
 * @param   iLUN        The logical unit which is being detached.
 * @param   fFlags      Flags, combination of the PDMDEVATT_FLAGS_* \#defines.
 */
static DECLCALLBACK(void) pcnetDetach(PPDMDEVINS pDevIns, unsigned iLUN, uint32_t fFlags)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    Log(("#%d pcnetDetach:\n", PCNET_INST_NR));

    AssertLogRelReturnVoid(iLUN == 0);

    PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);

    /** @todo: r=pritesh still need to check if i missed
     * to clean something in this function
     */

    /*
     * Zero some important members.
     */
    pThis->pDrvBase = NULL;
    pThis->pDrv = NULL;

    PDMCritSectLeave(&pThis->CritSect);
}


/**
 * Attach the Network attachment.
 *
 * One port on the network card has been connected to a network.
 *
 * @returns VBox status code.
 * @param   pDevIns     The device instance.
 * @param   iLUN        The logical unit which is being attached.
 * @param   fFlags      Flags, combination of the PDMDEVATT_FLAGS_* \#defines.
 *
 * @remarks This code path is not used during construction.
 */
static DECLCALLBACK(int) pcnetAttach(PPDMDEVINS pDevIns, unsigned iLUN, uint32_t fFlags)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    LogFlow(("#%d pcnetAttach:\n", PCNET_INST_NR));

    AssertLogRelReturn(iLUN == 0, VERR_PDM_NO_SUCH_LUN);

    PDMCritSectEnter(&pThis->CritSect, VERR_SEM_BUSY);

    /*
     * Attach the driver.
     */
    int rc = PDMDevHlpDriverAttach(pDevIns, 0, &pThis->IBase, &pThis->pDrvBase, "Network Port");
    if (RT_SUCCESS(rc))
    {
        if (rc == VINF_NAT_DNS)
        {
#ifdef RT_OS_LINUX
            PDMDevHlpVMSetRuntimeError(pDevIns, 0 /*fFlags*/, "NoDNSforNAT",
                                       N_("A Domain Name Server (DNS) for NAT networking could not be determined. Please check your /etc/resolv.conf for <tt>nameserver</tt> entries. Either add one manually (<i>man resolv.conf</i>) or ensure that your host is correctly connected to an ISP. If you ignore this warning the guest will not be able to perform nameserver lookups and it will probably observe delays if trying so"));
#else
            PDMDevHlpVMSetRuntimeError(pDevIns, 0 /*fFlags*/, "NoDNSforNAT",
                                       N_("A Domain Name Server (DNS) for NAT networking could not be determined. Ensure that your host is correctly connected to an ISP. If you ignore this warning the guest will not be able to perform nameserver lookups and it will probably observe delays if trying so"));
#endif
        }
        pThis->pDrv = (PPDMINETWORKCONNECTOR)pThis->pDrvBase->pfnQueryInterface(pThis->pDrvBase, PDMINTERFACE_NETWORK_CONNECTOR);
        if (!pThis->pDrv)
        {
            AssertMsgFailed(("Failed to obtain the PDMINTERFACE_NETWORK_CONNECTOR interface!\n"));
            rc = VERR_PDM_MISSING_INTERFACE_BELOW;
        }
    }
    else if (rc == VERR_PDM_NO_ATTACHED_DRIVER)
        Log(("#%d No attached driver!\n", PCNET_INST_NR));


    /*
     * Temporary set the link down if it was up so that the guest
     * will know that we have change the configuration of the
     * network card
     */
    if (RT_SUCCESS(rc))
        pcnetTempLinkDown(pThis);

    PDMCritSectLeave(&pThis->CritSect);
    return rc;

}

#endif /* VBOX_DYNAMIC_NET_ATTACH */

/**
 * @copydoc FNPDMDEVSUSPEND
 */
static DECLCALLBACK(void) pcnetSuspend(PPDMDEVINS pDevIns)
{
    /* Poke thread waiting for buffer space. */
    pcnetWakeupReceive(pDevIns);
}


/**
 * @copydoc FNPDMDEVRESET
 */
static DECLCALLBACK(void) pcnetReset(PPDMDEVINS pDevIns)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    if (pThis->fLinkTempDown)
    {
        pThis->cLinkDownReported = 0x10000;
        TMTimerStop(pThis->pTimerRestore);
        pcnetTimerRestore(pDevIns, pThis->pTimerRestore, pThis);
    }
    if (pThis->pSharedMMIOR3)
        pcnetInitSharedMemory(pThis);

    /** @todo How to flush the queues? */
    pcnetHardReset(pThis);
}


/**
 * @copydoc FNPDMDEVRELOCATE
 */
static DECLCALLBACK(void) pcnetRelocate(PPDMDEVINS pDevIns, RTGCINTPTR offDelta)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    pThis->pDevInsRC     = PDMDEVINS_2_RCPTR(pDevIns);
    pThis->pXmitQueueRC  = PDMQueueRCPtr(pThis->pXmitQueueR3);
    pThis->pCanRxQueueRC = PDMQueueRCPtr(pThis->pCanRxQueueR3);
    if (pThis->pSharedMMIOR3)
        pThis->pSharedMMIORC += offDelta;
#ifdef PCNET_NO_POLLING
    pThis->pfnEMInterpretInstructionRC += offDelta;
#else
    pThis->pTimerPollRC  = TMTimerRCPtr(pThis->pTimerPollR3);
#endif
    if (pThis->fAm79C973)
        pThis->pTimerSoftIntRC = TMTimerRCPtr(pThis->pTimerSoftIntR3);
}


/**
 * Destruct a device instance.
 *
 * Most VM resources are freed by the VM. This callback is provided so that any non-VM
 * resources can be freed correctly.
 *
 * @returns VBox status.
 * @param   pDevIns     The device instance data.
 */
static DECLCALLBACK(int) pcnetDestruct(PPDMDEVINS pDevIns)
{
    PCNetState *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);

    if (PDMCritSectIsInitialized(&pThis->CritSect))
    {
        /*
         * At this point the send thread is suspended and will not enter
         * this module again. So, no coordination is needed here and PDM
         * will take care of terminating and cleaning up the thread.
         */
        RTSemEventDestroy(pThis->hSendEventSem);
        pThis->hSendEventSem = NIL_RTSEMEVENT;
        RTSemEventSignal(pThis->hEventOutOfRxSpace);
        RTSemEventDestroy(pThis->hEventOutOfRxSpace);
        pThis->hEventOutOfRxSpace = NIL_RTSEMEVENT;
        PDMR3CritSectDelete(&pThis->CritSect);
    }
#ifdef PCNET_QUEUE_SEND_PACKETS
    if (pThis->apXmitRingBuffer)
        RTMemFree(pThis->apXmitRingBuffer[0]);
#endif
    return VINF_SUCCESS;
}


/**
 * Construct a device instance for a VM.
 *
 * @returns VBox status.
 * @param   pDevIns     The device instance data.
 *                      If the registration structure is needed, pDevIns->pDevReg points to it.
 * @param   iInstance   Instance number. Use this to figure out which registers and such to use.
 *                      The device number is also found in pDevIns->iInstance, but since it's
 *                      likely to be freqently used PDM passes it as parameter.
 * @param   pCfgHandle  Configuration node handle for the device. Use this to obtain the configuration
 *                      of the device instance. It's also found in pDevIns->pCfgHandle, but like
 *                      iInstance it's expected to be used a bit in this function.
 */
static DECLCALLBACK(int) pcnetConstruct(PPDMDEVINS pDevIns, int iInstance, PCFGMNODE pCfgHandle)
{
    PCNetState     *pThis = PDMINS_2_DATA(pDevIns, PCNetState *);
    PPDMIBASE       pBase;
    char            szTmp[128];
    int             rc;

    /* up to eight instances are supported */
    Assert((iInstance >= 0) && (iInstance < 8));

    Assert(RT_ELEMENTS(pThis->aBCR) == BCR_MAX_RAP);
    Assert(RT_ELEMENTS(pThis->aMII) == MII_MAX_REG);
    Assert(sizeof(pThis->abSendBuf) == RT_ALIGN_Z(sizeof(pThis->abSendBuf), 16));

    /*
     * Init what's required to make the destructor safe.
     */
    pThis->hEventOutOfRxSpace = NIL_RTSEMEVENT;
    pThis->hSendEventSem = NIL_RTSEMEVENT;

    /*
     * Validate configuration.
     */
    if (!CFGMR3AreValuesValid(pCfgHandle, "MAC\0" "CableConnected\0" "Am79C973\0" "LineSpeed\0" "GCEnabled\0" "R0Enabled\0" "PrivIfEnabled\0"))
        return PDMDEV_SET_ERROR(pDevIns, VERR_PDM_DEVINS_UNKNOWN_CFG_VALUES,
                                N_("Invalid configuration for pcnet device"));

    /*
     * Read the configuration.
     */
    rc = CFGMR3QueryBytes(pCfgHandle, "MAC", &pThis->MacConfigured, sizeof(pThis->MacConfigured));
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to get the \"MAC\" value"));
    rc = CFGMR3QueryBoolDef(pCfgHandle, "CableConnected", &pThis->fLinkUp, true);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to get the \"CableConnected\" value"));

    rc = CFGMR3QueryBoolDef(pCfgHandle, "Am79C973", &pThis->fAm79C973, false);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to get the \"Am79C973\" value"));

    rc = CFGMR3QueryU32Def(pCfgHandle, "LineSpeed", &pThis->u32LinkSpeed, 1000000); /* 1GBit/s (in kbps units)*/
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to get the \"LineSpeed\" value"));

#ifdef PCNET_GC_ENABLED
    rc = CFGMR3QueryBoolDef(pCfgHandle, "GCEnabled", &pThis->fGCEnabled, true);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to get the \"GCEnabled\" value"));

    rc = CFGMR3QueryBoolDef(pCfgHandle, "R0Enabled", &pThis->fR0Enabled, true);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to get the \"R0Enabled\" value"));

#else /* !PCNET_GC_ENABLED */
    pThis->fGCEnabled = false;
    pThis->fR0Enabled = false;
#endif /* !PCNET_GC_ENABLED */


    /*
     * Initialize data (most of it anyway).
     */
    pThis->pDevInsR3                        = pDevIns;
    pThis->pDevInsR0                        = PDMDEVINS_2_R0PTR(pDevIns);
    pThis->pDevInsRC                        = PDMDEVINS_2_RCPTR(pDevIns);
    pThis->Led.u32Magic                     = PDMLED_MAGIC;
    /* IBase */
    pThis->IBase.pfnQueryInterface          = pcnetQueryInterface;
    /* INeworkPort */
    pThis->INetworkPort.pfnWaitReceiveAvail = pcnetWaitReceiveAvail;
    pThis->INetworkPort.pfnReceive          = pcnetReceive;
    /* INetworkConfig */
    pThis->INetworkConfig.pfnGetMac         = pcnetGetMac;
    pThis->INetworkConfig.pfnGetLinkState   = pcnetGetLinkState;
    pThis->INetworkConfig.pfnSetLinkState   = pcnetSetLinkState;
    /* ILeds */
    pThis->ILeds.pfnQueryStatusLed          = pcnetQueryStatusLed;

    /* PCI Device */
    PCIDevSetVendorId(&pThis->PciDev, 0x1022);
    PCIDevSetDeviceId(&pThis->PciDev, 0x2000);
    pThis->PciDev.config[0x04] = 0x07; /* command */
    pThis->PciDev.config[0x05] = 0x00;
    pThis->PciDev.config[0x06] = 0x80; /* status */
    pThis->PciDev.config[0x07] = 0x02;
    pThis->PciDev.config[0x08] = pThis->fAm79C973 ? 0x40 : 0x10; /* revision */
    pThis->PciDev.config[0x09] = 0x00;
    pThis->PciDev.config[0x0a] = 0x00; /* ethernet network controller */
    pThis->PciDev.config[0x0b] = 0x02;
    pThis->PciDev.config[0x0e] = 0x00; /* header_type */

    pThis->PciDev.config[0x10] = 0x01; /* IO Base */
    pThis->PciDev.config[0x11] = 0x00;
    pThis->PciDev.config[0x12] = 0x00;
    pThis->PciDev.config[0x13] = 0x00;
    pThis->PciDev.config[0x14] = 0x00; /* MMIO Base */
    pThis->PciDev.config[0x15] = 0x00;
    pThis->PciDev.config[0x16] = 0x00;
    pThis->PciDev.config[0x17] = 0x00;

    /* subsystem and subvendor IDs */
    pThis->PciDev.config[0x2c] = 0x22; /* subsystem vendor id */
    pThis->PciDev.config[0x2d] = 0x10;
    pThis->PciDev.config[0x2e] = 0x00; /* subsystem id */
    pThis->PciDev.config[0x2f] = 0x20;
    pThis->PciDev.config[0x3d] = 1;    /* interrupt pin 0 */
    pThis->PciDev.config[0x3e] = 0x06;
    pThis->PciDev.config[0x3f] = 0xff;

    /*
     * Register the PCI device, its I/O regions, the timer and the saved state item.
     */
    rc = PDMDevHlpPCIRegister(pDevIns, &pThis->PciDev);
    if (RT_FAILURE(rc))
        return rc;
    rc = PDMDevHlpPCIIORegionRegister(pDevIns, 0, PCNET_IOPORT_SIZE,
                                      PCI_ADDRESS_SPACE_IO, pcnetIOPortMap);
    if (RT_FAILURE(rc))
        return rc;
    rc = PDMDevHlpPCIIORegionRegister(pDevIns, 1, PCNET_PNPMMIO_SIZE,
                                      PCI_ADDRESS_SPACE_MEM, pcnetMMIOMap);
    if (RT_FAILURE(rc))
        return rc;

    bool fPrivIfEnabled;
    rc = CFGMR3QueryBool(pCfgHandle, "PrivIfEnabled", &fPrivIfEnabled);
    if (rc == VERR_CFGM_VALUE_NOT_FOUND)
        fPrivIfEnabled = true;
    else if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to get the \"PrivIfEnabled\" value"));

    if (fPrivIfEnabled)
    {
        /*
         * Initialize shared memory between host and guest for descriptors and RX buffers. Most guests
         * should not care if there is an additional PCI ressource but just in case we made this configurable.
         */
        rc = PDMDevHlpMMIO2Register(pDevIns, 2, PCNET_GUEST_SHARED_MEMORY_SIZE, 0, (void **)&pThis->pSharedMMIOR3, "PCNetShMem");
        if (RT_FAILURE(rc))
            return PDMDevHlpVMSetError(pDevIns, rc, RT_SRC_POS,
                                       N_("Failed to allocate %u bytes of memory for the PCNet device"), PCNET_GUEST_SHARED_MEMORY_SIZE);
        rc = PDMDevHlpMMHyperMapMMIO2(pDevIns, 2, 0, 8192, "PCNetShMem", &pThis->pSharedMMIORC);
        if (RT_FAILURE(rc))
            return PDMDevHlpVMSetError(pDevIns, rc, RT_SRC_POS,
                                       N_("Failed to map 8192 bytes of memory for the PCNet device into the hyper memory"));
        pThis->pSharedMMIOR0 = (uintptr_t)pThis->pSharedMMIOR3; /** @todo #1865: Map MMIO2 into ring-0. */

        pcnetInitSharedMemory(pThis);
        rc = PDMDevHlpPCIIORegionRegister(pDevIns, 2, PCNET_GUEST_SHARED_MEMORY_SIZE,
                                          PCI_ADDRESS_SPACE_MEM, pcnetMMIOSharedMap);
        if (RT_FAILURE(rc))
            return rc;
    }

    /*
     * Initialize critical section.
     * This must be done before register the critsect with the timer code, and also before
     * attaching drivers or anything else that may call us back.
     */
    char szName[24];
    RTStrPrintf(szName, sizeof(szName), "PCNet#%d", iInstance);
    rc = PDMDevHlpCritSectInit(pDevIns, &pThis->CritSect, szName);
    if (RT_FAILURE(rc))
        return rc;

    rc = RTSemEventCreate(&pThis->hEventOutOfRxSpace);
    AssertRC(rc);

#ifdef PCNET_NO_POLLING
    /*
     * Resolve the R0 and RC handlers.
     */
    rc = PDMR3LdrGetSymbolR0Lazy(PDMDevHlpGetVM(pDevIns), NULL, "EMInterpretInstruction", &pThis->pfnEMInterpretInstructionR0);
    if (RT_SUCCESS(rc))
        rc = PDMR3LdrGetSymbolRCLazy(PDMDevHlpGetVM(pDevIns), NULL, "EMInterpretInstruction", (RTGCPTR *)&pThis->pfnEMInterpretInstructionRC);
    AssertLogRelMsgRCReturn(rc, ("PDMR3LdrGetSymbolRCLazy(EMInterpretInstruction) -> %Rrc\n", rc), rc);
#else
    rc = PDMDevHlpTMTimerCreate(pDevIns, TMCLOCK_VIRTUAL, pcnetTimer, pThis,
                                TMTIMER_FLAGS_NO_CRIT_SECT, "PCNet Poll Timer", &pThis->pTimerPollR3);
    if (RT_FAILURE(rc))
        return rc;
    pThis->pTimerPollR0 = TMTimerR0Ptr(pThis->pTimerPollR3);
    pThis->pTimerPollRC = TMTimerRCPtr(pThis->pTimerPollR3);
    TMR3TimerSetCritSect(pThis->pTimerPollR3, &pThis->CritSect);
#endif
    if (pThis->fAm79C973)
    {
        /* Software Interrupt timer */
        rc = PDMDevHlpTMTimerCreate(pDevIns, TMCLOCK_VIRTUAL, pcnetTimerSoftInt, pThis, /** @todo r=bird: the locking here looks bogus now with SMP... */
                                    TMTIMER_FLAGS_DEFAULT_CRIT_SECT, "PCNet SoftInt Timer", &pThis->pTimerSoftIntR3);
        if (RT_FAILURE(rc))
            return rc;
        pThis->pTimerSoftIntR0 = TMTimerR0Ptr(pThis->pTimerSoftIntR3);
        pThis->pTimerSoftIntRC = TMTimerRCPtr(pThis->pTimerSoftIntR3);
    }
    rc = PDMDevHlpTMTimerCreate(pDevIns, TMCLOCK_VIRTUAL, pcnetTimerRestore, pThis,
                                TMTIMER_FLAGS_NO_CRIT_SECT, "PCNet Restore Timer", &pThis->pTimerRestore);
    if (RT_FAILURE(rc))
        return rc;

    rc = PDMDevHlpSSMRegisterEx(pDevIns, PCNET_SAVEDSTATE_VERSION, sizeof(*pThis), NULL,
                                NULL,          pcnetLiveExec, NULL,
                                pcnetSavePrep, pcnetSaveExec, NULL,
                                pcnetLoadPrep, pcnetLoadExec, NULL);
    if (RT_FAILURE(rc))
        return rc;

    /*
     * Create the transmit queue.
     */
    rc = PDMDevHlpPDMQueueCreate(pDevIns, sizeof(PDMQUEUEITEMCORE), 1, 0,
                                 pcnetXmitQueueConsumer, true, "PCNet-Xmit", &pThis->pXmitQueueR3);
    if (RT_FAILURE(rc))
        return rc;
    pThis->pXmitQueueR0 = PDMQueueR0Ptr(pThis->pXmitQueueR3);
    pThis->pXmitQueueRC = PDMQueueRCPtr(pThis->pXmitQueueR3);

    /*
     * Create the RX notifer signaller.
     */
    rc = PDMDevHlpPDMQueueCreate(pDevIns, sizeof(PDMQUEUEITEMCORE), 1, 0,
                                 pcnetCanRxQueueConsumer, true, "PCNet-Rcv", &pThis->pCanRxQueueR3);
    if (RT_FAILURE(rc))
        return rc;
    pThis->pCanRxQueueR0 = PDMQueueR0Ptr(pThis->pCanRxQueueR3);
    pThis->pCanRxQueueRC = PDMQueueRCPtr(pThis->pCanRxQueueR3);

    /*
     * Register the info item.
     */
    RTStrPrintf(szTmp, sizeof(szTmp), "pcnet%d", pDevIns->iInstance);
    PDMDevHlpDBGFInfoRegister(pDevIns, szTmp, "PCNET info.", pcnetInfo);

    /*
     * Attach status driver (optional).
     */
    rc = PDMDevHlpDriverAttach(pDevIns, PDM_STATUS_LUN, &pThis->IBase, &pBase, "Status Port");
    if (RT_SUCCESS(rc))
        pThis->pLedsConnector = (PPDMILEDCONNECTORS)
            pBase->pfnQueryInterface(pBase, PDMINTERFACE_LED_CONNECTORS);
    else if (rc != VERR_PDM_NO_ATTACHED_DRIVER)
    {
        AssertMsgFailed(("Failed to attach to status driver. rc=%Rrc\n", rc));
        return rc;
    }

    /*
     * Attach driver.
     */
    rc = PDMDevHlpDriverAttach(pDevIns, 0, &pThis->IBase, &pThis->pDrvBase, "Network Port");
    if (RT_SUCCESS(rc))
    {
        if (rc == VINF_NAT_DNS)
        {
#ifdef RT_OS_LINUX
            PDMDevHlpVMSetRuntimeError(pDevIns, 0 /*fFlags*/, "NoDNSforNAT",
                                       N_("A Domain Name Server (DNS) for NAT networking could not be determined. Please check your /etc/resolv.conf for <tt>nameserver</tt> entries. Either add one manually (<i>man resolv.conf</i>) or ensure that your host is correctly connected to an ISP. If you ignore this warning the guest will not be able to perform nameserver lookups and it will probably observe delays if trying so"));
#else
            PDMDevHlpVMSetRuntimeError(pDevIns, 0 /*fFlags*/, "NoDNSforNAT",
                                       N_("A Domain Name Server (DNS) for NAT networking could not be determined. Ensure that your host is correctly connected to an ISP. If you ignore this warning the guest will not be able to perform nameserver lookups and it will probably observe delays if trying so"));
#endif
        }
        pThis->pDrv = (PPDMINETWORKCONNECTOR)
            pThis->pDrvBase->pfnQueryInterface(pThis->pDrvBase, PDMINTERFACE_NETWORK_CONNECTOR);
        if (!pThis->pDrv)
        {
            AssertMsgFailed(("Failed to obtain the PDMINTERFACE_NETWORK_CONNECTOR interface!\n"));
            return VERR_PDM_MISSING_INTERFACE_BELOW;
        }
    }
    else if (rc == VERR_PDM_NO_ATTACHED_DRIVER)
        Log(("No attached driver!\n"));
    else
        return rc;

    /*
     * Reset the device state. (Do after attaching.)
     */
    pcnetHardReset(pThis);

    /* Create send queue for the async send thread. */
    rc = RTSemEventCreate(&pThis->hSendEventSem);
    AssertRC(rc);

    /* Create asynchronous thread */
    rc = PDMDevHlpPDMThreadCreate(pDevIns, &pThis->pSendThread, pThis, pcnetAsyncSendThread, pcnetAsyncSendThreadWakeUp, 0, RTTHREADTYPE_IO, "PCNET_TX");
    AssertRCReturn(rc, rc);

#ifdef PCNET_QUEUE_SEND_PACKETS
    pThis->apXmitRingBuffer[0] = (uint8_t *)RTMemAlloc(PCNET_MAX_XMIT_SLOTS * MAX_FRAME);
    for (unsigned i = 1; i < PCNET_MAX_XMIT_SLOTS; i++)
        pThis->apXmitRingBuffer[i] = pThis->apXmitRingBuffer[0] + i*MAX_FRAME;
#endif

#ifdef VBOX_WITH_STATISTICS
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatMMIOReadGC,         STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling MMIO reads in GC",         "/Devices/PCNet%d/MMIO/ReadGC", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatMMIOReadHC,         STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling MMIO reads in HC",         "/Devices/PCNet%d/MMIO/ReadHC", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatMMIOWriteGC,        STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling MMIO writes in GC",        "/Devices/PCNet%d/MMIO/WriteGC", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatMMIOWriteHC,        STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling MMIO writes in HC",        "/Devices/PCNet%d/MMIO/WriteHC", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatAPROMRead,          STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling APROM reads",              "/Devices/PCNet%d/IO/APROMRead", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatAPROMWrite,         STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling APROM writes",             "/Devices/PCNet%d/IO/APROMWrite", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatIOReadGC,           STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling IO reads in GC",           "/Devices/PCNet%d/IO/ReadGC", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatIOReadHC,           STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling IO reads in HC",           "/Devices/PCNet%d/IO/ReadHC", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatIOWriteGC,          STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling IO writes in GC",          "/Devices/PCNet%d/IO/WriteGC", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatIOWriteHC,          STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling IO writes in HC",          "/Devices/PCNet%d/IO/WriteHC", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatTimer,              STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling Timer",                    "/Devices/PCNet%d/Timer", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatReceive,            STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling receive",                  "/Devices/PCNet%d/Receive", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatRxOverflow,         STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_OCCURENCE, "Profiling RX overflows",        "/Devices/PCNet%d/RxOverflow", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatRxOverflowWakeup,   STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_OCCURENCE, "Nr of RX overflow wakeups",     "/Devices/PCNet%d/RxOverflowWakeup", iInstance);
#endif
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatReceiveBytes,       STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_BYTES,          "Amount of data received",            "/Devices/PCNet%d/ReceiveBytes", iInstance);
#ifdef VBOX_WITH_STATISTICS
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatTransmit,           STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling transmits in HC",          "/Devices/PCNet%d/Transmit/Total", iInstance);
#endif
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatTransmitBytes,      STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_BYTES,          "Amount of data transmitted",         "/Devices/PCNet%d/TransmitBytes", iInstance);
#ifdef VBOX_WITH_STATISTICS
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatTransmitSend,       STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling PCNet send transmit in HC","/Devices/PCNet%d/Transmit/Send", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatTdtePollGC,         STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling PCNet TdtePoll in GC",     "/Devices/PCNet%d/TdtePollGC", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatTdtePollHC,         STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling PCNet TdtePoll in HC",     "/Devices/PCNet%d/TdtePollHC", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatRdtePollGC,         STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling PCNet RdtePoll in GC",     "/Devices/PCNet%d/RdtePollGC", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatRdtePollHC,         STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling PCNet RdtePoll in HC",     "/Devices/PCNet%d/RdtePollHC", iInstance);

    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatTmdStoreGC,         STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling PCNet TmdStore in GC",     "/Devices/PCNet%d/TmdStoreGC", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatTmdStoreHC,         STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling PCNet TmdStore in HC",     "/Devices/PCNet%d/TmdStoreHC", iInstance);

    unsigned i;
    for (i = 0; i < RT_ELEMENTS(pThis->aStatXmitFlush) - 1; i++)
        PDMDevHlpSTAMRegisterF(pDevIns, &pThis->aStatXmitFlush[i],  STAMTYPE_COUNTER, STAMVISIBILITY_USED, STAMUNIT_OCCURENCES,       "",                                   "/Devices/PCNet%d/XmitFlushIrq/%d", iInstance, i + 1);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->aStatXmitFlush[i],      STAMTYPE_COUNTER, STAMVISIBILITY_USED, STAMUNIT_OCCURENCES,       "",                                   "/Devices/PCNet%d/XmitFlushIrq/%d+", iInstance, i + 1);

    for (i = 0; i < RT_ELEMENTS(pThis->aStatXmitChainCounts) - 1; i++)
        PDMDevHlpSTAMRegisterF(pDevIns, &pThis->aStatXmitChainCounts[i], STAMTYPE_COUNTER, STAMVISIBILITY_USED, STAMUNIT_OCCURENCES,  "",                                   "/Devices/PCNet%d/XmitChainCounts/%d", iInstance, i + 1);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->aStatXmitChainCounts[i], STAMTYPE_COUNTER, STAMVISIBILITY_USED, STAMUNIT_OCCURENCES,      "",                                   "/Devices/PCNet%d/XmitChainCounts/%d+", iInstance, i + 1);

    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatXmitSkipCurrent,    STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_OCCURENCES,     "",                                   "/Devices/PCNet%d/Xmit/Skipped", iInstance, i + 1);

    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatInterrupt,          STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling interrupt checks",         "/Devices/PCNet%d/UpdateIRQ", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatPollTimer,          STAMTYPE_PROFILE, STAMVISIBILITY_ALWAYS, STAMUNIT_TICKS_PER_CALL, "Profiling poll timer",               "/Devices/PCNet%d/PollTimer", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatMIIReads,           STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_OCCURENCES,     "Number of MII reads",                "/Devices/PCNet%d/MIIReads", iInstance);
# ifdef PCNET_NO_POLLING
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatRCVRingWrite,       STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_OCCURENCES,     "Nr of receive ring writes",          "/Devices/PCNet%d/Ring/RCVWrites", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatTXRingWrite,        STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_OCCURENCES,     "Nr of transmit ring writes",         "/Devices/PCNet%d/Ring/TXWrites", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatRingWriteHC,        STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_OCCURENCES,     "Nr of monitored ring page writes",   "/Devices/PCNet%d/Ring/HC/Writes", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatRingWriteR0,        STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_OCCURENCES,     "Nr of monitored ring page writes",   "/Devices/PCNet%d/Ring/R0/Writes", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatRingWriteGC,        STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_OCCURENCES,     "Nr of monitored ring page writes",   "/Devices/PCNet%d/Ring/GC/Writes", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatRingWriteFailedHC,  STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_OCCURENCES,     "Nr of failed ring page writes",      "/Devices/PCNet%d/Ring/HC/Failed", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatRingWriteFailedR0,  STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_OCCURENCES,     "Nr of failed ring page writes",      "/Devices/PCNet%d/Ring/R0/Failed", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatRingWriteFailedGC,  STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_OCCURENCES,     "Nr of failed ring page writes",      "/Devices/PCNet%d/Ring/GC/Failed", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatRingWriteOutsideHC, STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_OCCURENCES,     "Nr of monitored writes outside ring","/Devices/PCNet%d/Ring/HC/Outside", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatRingWriteOutsideR0, STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_OCCURENCES,     "Nr of monitored writes outside ring","/Devices/PCNet%d/Ring/R0/Outside", iInstance);
    PDMDevHlpSTAMRegisterF(pDevIns, &pThis->StatRingWriteOutsideGC, STAMTYPE_COUNTER, STAMVISIBILITY_ALWAYS, STAMUNIT_OCCURENCES,     "Nr of monitored writes outside ring","/Devices/PCNet%d/Ring/GC/Outside", iInstance);
# endif /* PCNET_NO_POLLING */
#endif

    return VINF_SUCCESS;
}


/**
 * The device registration structure.
 */
const PDMDEVREG g_DevicePCNet =
{
    /* u32Version */
    PDM_DEVREG_VERSION,
    /* szDeviceName */
    "pcnet",
    /* szRCMod */
#ifdef PCNET_GC_ENABLED
    "VBoxDDGC.gc",
    "VBoxDDR0.r0",
#else
    "",
    "",
#endif
    /* pszDescription */
    "AMD PC-Net II Ethernet controller.\n",
    /* fFlags */
#ifdef PCNET_GC_ENABLED
    PDM_DEVREG_FLAGS_DEFAULT_BITS | PDM_DEVREG_FLAGS_RC | PDM_DEVREG_FLAGS_R0,
#else
    PDM_DEVREG_FLAGS_DEFAULT_BITS,
#endif
    /* fClass */
    PDM_DEVREG_CLASS_NETWORK,
    /* cMaxInstances */
    8,
    /* cbInstance */
    sizeof(PCNetState),
    /* pfnConstruct */
    pcnetConstruct,
    /* pfnDestruct */
    pcnetDestruct,
    /* pfnRelocate */
    pcnetRelocate,
    /* pfnIOCtl */
    NULL,
    /* pfnPowerOn */
    NULL,
    /* pfnReset */
    pcnetReset,
    /* pfnSuspend */
    pcnetSuspend,
    /* pfnResume */
    NULL,
#ifdef VBOX_DYNAMIC_NET_ATTACH
    /* pfnAttach */
    pcnetAttach,
    /* pfnDetach */
    pcnetDetach,
#else /* !VBOX_DYNAMIC_NET_ATTACH */
    /* pfnAttach */
    NULL,
    /* pfnDetach */
    NULL,
#endif /* !VBOX_DYNAMIC_NET_ATTACH */
    /* pfnQueryInterface. */
    NULL,
    /* pfnInitComplete. */
    NULL,
    /* pfnPowerOff. */
    pcnetPowerOff,
    /* pfnSoftReset */
    NULL,
    /* u32VersionEnd */
    PDM_DEVREG_VERSION
};

#endif /* IN_RING3 */
#endif /* !VBOX_DEVICE_STRUCT_TESTCASE */

