/* $Id$ */
/** @file
 * DrvNetSniffer - Network sniffer filter driver.
 */

/*
 * Copyright (C) 2006-2010 Sun Microsystems, Inc.
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
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define LOG_GROUP LOG_GROUP_DRV_NAT
#include <VBox/pdmdrv.h>
#include <VBox/pdmnetifs.h>

#include <VBox/log.h>
#include <iprt/assert.h>
#include <iprt/critsect.h>
#include <iprt/file.h>
#include <iprt/process.h>
#include <iprt/string.h>
#include <iprt/time.h>
#include <iprt/uuid.h>
#include <VBox/param.h>

#include "Pcap.h"
#include "Builtins.h"


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/**
 * Block driver instance data.
 *
 * @implements  PDMINETWORKUP
 * @implements  PDMINETWORKDOWN
 * @implements  PDMINETWORKCONFIG
 */
typedef struct DRVNETSNIFFER
{
    /** The network interface. */
    PDMINETWORKUP           INetworkUp;
    /** The network interface. */
    PDMINETWORKDOWN         INetworkDown;
    /** The network config interface.
     * @todo this is a main interface and shouldn't be here...  */
    PDMINETWORKCONFIG       INetworkConfig;
    /** The port we're attached to. */
    PPDMINETWORKDOWN        pIAboveNet;
    /** The config port interface we're attached to. */
    PPDMINETWORKCONFIG      pIAboveConfig;
    /** The connector that's attached to us. */
    PPDMINETWORKUP          pIBelowNet;
    /** The filename. */
    char                    szFilename[RTPATH_MAX];
    /** The filehandle. */
    RTFILE                  File;
    /** The lock serializing the file access. */
    RTCRITSECT              Lock;
    /** The NanoTS delta we pass to the pcap writers. */
    uint64_t                StartNanoTS;
    /** Pointer to the driver instance. */
    PPDMDRVINS              pDrvIns;

} DRVNETSNIFFER, *PDRVNETSNIFFER;



/**
 * @interface_method_impl{PDMINETWORKUP,pfnAllocBuf}
 */
static DECLCALLBACK(int) drvNetSnifferUp_AllocBuf(PPDMINETWORKUP pInterface, size_t cbMin, PPPDMSCATTERGATHER ppSgBuf)
{
    PDRVNETSNIFFER pThis = RT_FROM_MEMBER(pInterface, DRVNETSNIFFER, INetworkUp);
    if (RT_UNLIKELY(!pThis->pIBelowNet))
        return VERR_NET_DOWN;
    return pThis->pIBelowNet->pfnAllocBuf(pThis->pIBelowNet, cbMin, ppSgBuf);
}


/**
 * @interface_method_impl{PDMINETWORKUP,pfnFreeBuf}
 */
static DECLCALLBACK(int) drvNetSnifferUp_FreeBuf(PPDMINETWORKUP pInterface, PPDMSCATTERGATHER pSgBuf)
{
    PDRVNETSNIFFER pThis = RT_FROM_MEMBER(pInterface, DRVNETSNIFFER, INetworkUp);
    if (RT_UNLIKELY(!pThis->pIBelowNet))
        return VERR_NET_DOWN;
    return pThis->pIBelowNet->pfnFreeBuf(pThis->pIBelowNet, pSgBuf);
}


/**
 * @interface_method_impl{PDMINETWORKUP,pfnSendBuf}
 */
static DECLCALLBACK(int) drvNetSnifferUp_SendBuf(PPDMINETWORKUP pInterface, PPDMSCATTERGATHER pSgBuf, bool fOnWorkerThread)
{
    PDRVNETSNIFFER pThis = RT_FROM_MEMBER(pInterface, DRVNETSNIFFER, INetworkUp);
    if (RT_UNLIKELY(!pThis->pIBelowNet))
        return VERR_NET_DOWN;

    /* output to sniffer */
    RTCritSectEnter(&pThis->Lock);
    PcapFileFrame(pThis->File, pThis->StartNanoTS,
                  pSgBuf->aSegs[0].pvSeg,
                  pSgBuf->cbUsed,
                  RT_MIN(pSgBuf->cbUsed, pSgBuf->aSegs[0].cbSeg));
    RTCritSectLeave(&pThis->Lock);

    return pThis->pIBelowNet->pfnSendBuf(pThis->pIBelowNet, pSgBuf, fOnWorkerThread);
}


/**
 * @interface_method_impl{PDMINETWORKUP,pfnSendDeprecated}
 */
static DECLCALLBACK(int) drvNetSnifferUp_SendDeprecated(PPDMINETWORKUP pInterface, const void *pvBuf, size_t cb)
{
    PDRVNETSNIFFER pThis = RT_FROM_MEMBER(pInterface, DRVNETSNIFFER, INetworkUp);

    /* output to sniffer */
    RTCritSectEnter(&pThis->Lock);
    PcapFileFrame(pThis->File, pThis->StartNanoTS, pvBuf, cb, cb);
    RTCritSectLeave(&pThis->Lock);

    /* pass down */
    if (RT_LIKELY(pThis->pIBelowNet))
    {
        int rc = pThis->pIBelowNet->pfnSendDeprecated(pThis->pIBelowNet, pvBuf, cb);
#if 0
        RTCritSectEnter(&pThis->Lock);
        u64TS = RTTimeProgramNanoTS();
        Hdr.ts_sec = (uint32_t)(u64TS / 1000000000);
        Hdr.ts_usec = (uint32_t)((u64TS / 1000) % 1000000);
        Hdr.incl_len = 0;
        RTFileWrite(pThis->File, &Hdr, sizeof(Hdr), NULL);
        RTCritSectLeave(&pThis->Lock);
#endif
        return rc;
    }
    return VINF_SUCCESS;
}


/**
 * @interface_method_impl{PDMINETWORKUP,pfnSetPromiscuousMode}
 */
static DECLCALLBACK(void) drvNetSnifferUp_SetPromiscuousMode(PPDMINETWORKUP pInterface, bool fPromiscuous)
{
    LogFlow(("drvNetSnifferUp_SetPromiscuousMode: fPromiscuous=%d\n", fPromiscuous));
    PDRVNETSNIFFER pThis = RT_FROM_MEMBER(pInterface, DRVNETSNIFFER, INetworkUp);
    if (pThis->pIBelowNet)
        pThis->pIBelowNet->pfnSetPromiscuousMode(pThis->pIBelowNet, fPromiscuous);
}


/**
 * @interface_method_impl{PDMINETWORKUP,pfnNotifyLinkChanged}
 */
static DECLCALLBACK(void) drvNetSnifferUp_NotifyLinkChanged(PPDMINETWORKUP pInterface, PDMNETWORKLINKSTATE enmLinkState)
{
    LogFlow(("drvNetSnifferUp_NotifyLinkChanged: enmLinkState=%d\n", enmLinkState));
    PDRVNETSNIFFER pThis = RT_FROM_MEMBER(pInterface, DRVNETSNIFFER, INetworkUp);
    if (pThis->pIBelowNet)
        pThis->pIBelowNet->pfnNotifyLinkChanged(pThis->pIBelowNet, enmLinkState);
}


/**
 * @copydoc PDMINETWORKDOWN::pfnWaitReceiveAvail
 */
static DECLCALLBACK(int) drvNetSnifferDown_WaitReceiveAvail(PPDMINETWORKDOWN pInterface, RTMSINTERVAL cMillies)
{
    PDRVNETSNIFFER pThis = RT_FROM_MEMBER(pInterface, DRVNETSNIFFER, INetworkDown);
    return pThis->pIAboveNet->pfnWaitReceiveAvail(pThis->pIAboveNet, cMillies);
}


/**
 * @copydoc PDMINETWORKDOWN::pfnReceive
 */
static DECLCALLBACK(int) drvNetSnifferDown_Receive(PPDMINETWORKDOWN pInterface, const void *pvBuf, size_t cb)
{
    PDRVNETSNIFFER pThis = RT_FROM_MEMBER(pInterface, DRVNETSNIFFER, INetworkDown);

    /* output to sniffer */
    RTCritSectEnter(&pThis->Lock);
    PcapFileFrame(pThis->File, pThis->StartNanoTS, pvBuf, cb, cb);
    RTCritSectLeave(&pThis->Lock);

    /* pass up */
    int rc = pThis->pIAboveNet->pfnReceive(pThis->pIAboveNet, pvBuf, cb);
#if 0
    RTCritSectEnter(&pThis->Lock);
    u64TS = RTTimeProgramNanoTS();
    Hdr.ts_sec = (uint32_t)(u64TS / 1000000000);
    Hdr.ts_usec = (uint32_t)((u64TS / 1000) % 1000000);
    Hdr.incl_len = 0;
    RTFileWrite(pThis->File, &Hdr, sizeof(Hdr), NULL);
    RTCritSectLeave(&pThis->Lock);
#endif
    return rc;
}


/**
 * @copydoc PDMINETWORKDOWN::pfnNotifyBufAvailable
 */
static DECLCALLBACK(void) drvNetSnifferDown_NotifyBufAvailable(PPDMINETWORKDOWN pInterface)
{
    PDRVNETSNIFFER pThis = RT_FROM_MEMBER(pInterface, DRVNETSNIFFER, INetworkDown);
    pThis->pIAboveNet->pfnNotifyBufAvailable(pThis->pIAboveNet);
}


/**
 * Gets the current Media Access Control (MAC) address.
 *
 * @returns VBox status code.
 * @param   pInterface      Pointer to the interface structure containing the called function pointer.
 * @param   pMac            Where to store the MAC address.
 * @thread  EMT
 */
static DECLCALLBACK(int) drvNetSnifferDownCfg_GetMac(PPDMINETWORKCONFIG pInterface, PRTMAC pMac)
{
    PDRVNETSNIFFER pThis = RT_FROM_MEMBER(pInterface, DRVNETSNIFFER, INetworkConfig);
    return pThis->pIAboveConfig->pfnGetMac(pThis->pIAboveConfig, pMac);
}

/**
 * Gets the new link state.
 *
 * @returns The current link state.
 * @param   pInterface      Pointer to the interface structure containing the called function pointer.
 * @thread  EMT
 */
static DECLCALLBACK(PDMNETWORKLINKSTATE) drvNetSnifferDownCfg_GetLinkState(PPDMINETWORKCONFIG pInterface)
{
    PDRVNETSNIFFER pThis = RT_FROM_MEMBER(pInterface, DRVNETSNIFFER, INetworkConfig);
    return pThis->pIAboveConfig->pfnGetLinkState(pThis->pIAboveConfig);
}

/**
 * Sets the new link state.
 *
 * @returns VBox status code.
 * @param   pInterface      Pointer to the interface structure containing the called function pointer.
 * @param   enmState        The new link state
 * @thread  EMT
 */
static DECLCALLBACK(int) drvNetSnifferDownCfg_SetLinkState(PPDMINETWORKCONFIG pInterface, PDMNETWORKLINKSTATE enmState)
{
    PDRVNETSNIFFER pThis = RT_FROM_MEMBER(pInterface, DRVNETSNIFFER, INetworkConfig);
    return pThis->pIAboveConfig->pfnSetLinkState(pThis->pIAboveConfig, enmState);
}


/**
 * @interface_method_impl{PDMIBASE,pfnQueryInterface}
 */
static DECLCALLBACK(void *) drvNetSnifferQueryInterface(PPDMIBASE pInterface, const char *pszIID)
{
    PPDMDRVINS      pDrvIns = PDMIBASE_2_PDMDRV(pInterface);
    PDRVNETSNIFFER  pThis   = PDMINS_2_DATA(pDrvIns, PDRVNETSNIFFER);
    PDMIBASE_RETURN_INTERFACE(pszIID, PDMIBASE, &pDrvIns->IBase);
    PDMIBASE_RETURN_INTERFACE(pszIID, PDMINETWORKUP, &pThis->INetworkUp);
    PDMIBASE_RETURN_INTERFACE(pszIID, PDMINETWORKDOWN, &pThis->INetworkDown);
    PDMIBASE_RETURN_INTERFACE(pszIID, PDMINETWORKCONFIG, &pThis->INetworkConfig);
    return NULL;
}


/**
 * Detach a driver instance.
 *
 * @param   pDrvIns     The driver instance.
 * @param   fFlags      Flags, combination of the PDM_TACH_FLAGS_* \#defines.
 */
static DECLCALLBACK(void) drvNetSnifferDetach(PPDMDRVINS pDrvIns, uint32_t fFlags)
{
    PDRVNETSNIFFER pThis = PDMINS_2_DATA(pDrvIns, PDRVNETSNIFFER);

    LogFlow(("drvNetSnifferDetach: pDrvIns: %p, fFlags: %u\n", pDrvIns, fFlags));

    pThis->pIBelowNet = NULL;
}


/**
 * Attach a driver instance.
 *
 * @returns VBox status code.
 * @param   pDrvIns     The driver instance.
 * @param   fFlags      Flags, combination of the PDM_TACH_FLAGS_* \#defines.
 */
static DECLCALLBACK(int) drvNetSnifferAttach(PPDMDRVINS pDrvIns, uint32_t fFlags)
{
    PDRVNETSNIFFER pThis = PDMINS_2_DATA(pDrvIns, PDRVNETSNIFFER);

    LogFlow(("drvNetSnifferAttach: pDrvIns: %p, fFlags: %u\n", pDrvIns, fFlags));

    /*
     * Query the network connector interface.
     */
    PPDMIBASE   pBaseDown;
    int rc = PDMDrvHlpAttach(pDrvIns, fFlags, &pBaseDown);
    if (rc == VERR_PDM_NO_ATTACHED_DRIVER)
        pThis->pIBelowNet = NULL;
    else if (RT_SUCCESS(rc))
    {
        pThis->pIBelowNet = PDMIBASE_QUERY_INTERFACE(pBaseDown, PDMINETWORKUP);
        if (!pThis->pIBelowNet)
        {
            AssertMsgFailed(("Configuration error: the driver below didn't export the network connector interface!\n"));
            return VERR_PDM_MISSING_INTERFACE_BELOW;
        }
    }
    else
    {
        AssertMsgFailed(("Failed to attach to driver below! rc=%Rrc\n", rc));
        return rc;
    }

    return VINF_SUCCESS;
}


/**
 * Destruct a driver instance.
 *
 * Most VM resources are freed by the VM. This callback is provided so that any non-VM
 * resources can be freed correctly.
 *
 * @param   pDrvIns     The driver instance data.
 */
static DECLCALLBACK(void) drvNetSnifferDestruct(PPDMDRVINS pDrvIns)
{
    PDRVNETSNIFFER pThis = PDMINS_2_DATA(pDrvIns, PDRVNETSNIFFER);
    PDMDRV_CHECK_VERSIONS_RETURN_VOID(pDrvIns);

    if (RTCritSectIsInitialized(&pThis->Lock))
        RTCritSectDelete(&pThis->Lock);

    if (pThis->File != NIL_RTFILE)
    {
        RTFileClose(pThis->File);
        pThis->File = NIL_RTFILE;
    }
}


/**
 * Construct a NAT network transport driver instance.
 *
 * @copydoc FNPDMDRVCONSTRUCT
 */
static DECLCALLBACK(int) drvNetSnifferConstruct(PPDMDRVINS pDrvIns, PCFGMNODE pCfg, uint32_t fFlags)
{
    PDRVNETSNIFFER pThis = PDMINS_2_DATA(pDrvIns, PDRVNETSNIFFER);
    LogFlow(("drvNetSnifferConstruct:\n"));
    PDMDRV_CHECK_VERSIONS_RETURN(pDrvIns);

    /*
     * Validate the config.
     */
    if (!CFGMR3AreValuesValid(pCfg, "File\0"))
        return VERR_PDM_DRVINS_UNKNOWN_CFG_VALUES;

    if (CFGMR3GetFirstChild(pCfg))
        LogRel(("NetSniffer: Found child config entries -- are you trying to redirect ports?\n"));

    /*
     * Init the static parts.
     */
    pThis->pDrvIns                                  = pDrvIns;
    pThis->File                                     = NIL_RTFILE;
    /* The pcap file *must* start at time offset 0,0. */
    pThis->StartNanoTS                              = RTTimeNanoTS() - RTTimeProgramNanoTS();
    /* IBase */
    pDrvIns->IBase.pfnQueryInterface                = drvNetSnifferQueryInterface;
    /* INetworkUp */
    pThis->INetworkUp.pfnAllocBuf                   = drvNetSnifferUp_AllocBuf;
    pThis->INetworkUp.pfnFreeBuf                    = drvNetSnifferUp_FreeBuf;
    pThis->INetworkUp.pfnSendBuf                    = drvNetSnifferUp_SendBuf;
    pThis->INetworkUp.pfnSendDeprecated             = drvNetSnifferUp_SendDeprecated;
    pThis->INetworkUp.pfnSetPromiscuousMode         = drvNetSnifferUp_SetPromiscuousMode;
    pThis->INetworkUp.pfnNotifyLinkChanged          = drvNetSnifferUp_NotifyLinkChanged;
    /* INetworkDown */
    pThis->INetworkDown.pfnWaitReceiveAvail         = drvNetSnifferDown_WaitReceiveAvail;
    pThis->INetworkDown.pfnReceive                  = drvNetSnifferDown_Receive;
    pThis->INetworkDown.pfnNotifyBufAvailable       = drvNetSnifferDown_NotifyBufAvailable;
    /* INetworkConfig */
    pThis->INetworkConfig.pfnGetMac                 = drvNetSnifferDownCfg_GetMac;
    pThis->INetworkConfig.pfnGetLinkState           = drvNetSnifferDownCfg_GetLinkState;
    pThis->INetworkConfig.pfnSetLinkState           = drvNetSnifferDownCfg_SetLinkState;

    /*
     * Get the filename.
     */
    int rc = CFGMR3QueryString(pCfg, "File", pThis->szFilename, sizeof(pThis->szFilename));
    if (rc == VERR_CFGM_VALUE_NOT_FOUND)
    {
        if (pDrvIns->iInstance > 0)
            RTStrPrintf(pThis->szFilename, sizeof(pThis->szFilename), "./VBox-%x-%u.pcap", RTProcSelf(), pDrvIns->iInstance);
        else
            RTStrPrintf(pThis->szFilename, sizeof(pThis->szFilename), "./VBox-%x.pcap", RTProcSelf());
    }

    else if (RT_FAILURE(rc))
    {
        AssertMsgFailed(("Failed to query \"File\", rc=%Rrc.\n", rc));
        return rc;
    }

    /*
     * Query the network port interface.
     */
    pThis->pIAboveNet = PDMIBASE_QUERY_INTERFACE(pDrvIns->pUpBase, PDMINETWORKDOWN);
    if (!pThis->pIAboveNet)
    {
        AssertMsgFailed(("Configuration error: the above device/driver didn't export the network port interface!\n"));
        return VERR_PDM_MISSING_INTERFACE_ABOVE;
    }

    /*
     * Query the network config interface.
     */
    pThis->pIAboveConfig = PDMIBASE_QUERY_INTERFACE(pDrvIns->pUpBase, PDMINETWORKCONFIG);
    if (!pThis->pIAboveConfig)
    {
        AssertMsgFailed(("Configuration error: the above device/driver didn't export the network config interface!\n"));
        return VERR_PDM_MISSING_INTERFACE_ABOVE;
    }

    /*
     * Query the network connector interface.
     */
    PPDMIBASE   pBaseDown;
    rc = PDMDrvHlpAttach(pDrvIns, fFlags, &pBaseDown);
    if (rc == VERR_PDM_NO_ATTACHED_DRIVER)
        pThis->pIBelowNet = NULL;
    else if (RT_SUCCESS(rc))
    {
        pThis->pIBelowNet = PDMIBASE_QUERY_INTERFACE(pBaseDown, PDMINETWORKUP);
        if (!pThis->pIBelowNet)
        {
            AssertMsgFailed(("Configuration error: the driver below didn't export the network connector interface!\n"));
            return VERR_PDM_MISSING_INTERFACE_BELOW;
        }
    }
    else
    {
        AssertMsgFailed(("Failed to attach to driver below! rc=%Rrc\n", rc));
        return rc;
    }

    /*
     * Create the lock.
     */
    rc = RTCritSectInit(&pThis->Lock);
    if (RT_FAILURE(rc))
        return rc;

    /*
     * Open output file / pipe.
     */
    rc = RTFileOpen(&pThis->File, pThis->szFilename,
                    RTFILE_O_WRITE | RTFILE_O_CREATE_REPLACE | RTFILE_O_DENY_WRITE);
    if (RT_FAILURE(rc))
        return PDMDrvHlpVMSetError(pDrvIns, rc, RT_SRC_POS,
                                   N_("Netsniffer cannot open '%s' for writing. The directory must exist and it must be writable for the current user"), pThis->szFilename);

    /*
     * Write pcap header.
     * Some time is done since capturing pThis->StartNanoTS so capture the current time again.
     */
    PcapFileHdr(pThis->File, RTTimeNanoTS());

    return VINF_SUCCESS;
}



/**
 * Network sniffer filter driver registration record.
 */
const PDMDRVREG g_DrvNetSniffer =
{
    /* u32Version */
    PDM_DRVREG_VERSION,
    /* szName */
    "NetSniffer",
    /* szRCMod */
    "",
    /* szR0Mod */
    "",
    /* pszDescription */
    "Network Sniffer Filter Driver",
    /* fFlags */
    PDM_DRVREG_FLAGS_HOST_BITS_DEFAULT,
    /* fClass. */
    PDM_DRVREG_CLASS_NETWORK,
    /* cMaxInstances */
    UINT32_MAX,
    /* cbInstance */
    sizeof(DRVNETSNIFFER),
    /* pfnConstruct */
    drvNetSnifferConstruct,
    /* pfnDestruct */
    drvNetSnifferDestruct,
    /* pfnRelocate */
    NULL,
    /* pfnIOCtl */
    NULL,
    /* pfnPowerOn */
    NULL,
    /* pfnReset */
    NULL,
    /* pfnSuspend */
    NULL,
    /* pfnResume */
    NULL,
    /* pfnAttach */
    drvNetSnifferAttach,
    /* pfnDetach */
    drvNetSnifferDetach,
    /* pfnPowerOff */
    NULL,
    /* pfnSoftReset */
    NULL,
    /* u32EndVersion */
    PDM_DRVREG_VERSION
};

