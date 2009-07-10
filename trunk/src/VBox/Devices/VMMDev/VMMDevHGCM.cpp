/* $Id$ */
/** @file
 * VMMDev - HGCM - Host-Guest Communication Manager Device.
 */

/*
 * Copyright (C) 2006-2007 Sun Microsystems, Inc.
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


#define LOG_GROUP LOG_GROUP_DEV_VMM
#include <iprt/alloc.h>
#include <iprt/asm.h>
#include <iprt/assert.h>
#include <iprt/param.h>
#include <iprt/string.h>

#include <VBox/err.h>
#include <VBox/hgcmsvc.h>

#include <VBox/log.h>

#include "VMMDevHGCM.h"

typedef enum _VBOXHGCMCMDTYPE
{
    VBOXHGCMCMDTYPE_LOADSTATE = 0,
    VBOXHGCMCMDTYPE_CONNECT,
    VBOXHGCMCMDTYPE_DISCONNECT,
    VBOXHGCMCMDTYPE_CALL,
    VBOXHGCMCMDTYPE_SizeHack = 0x7fffffff
} VBOXHGCMCMDTYPE;

/* Information about a linear ptr parameter. */
typedef struct _VBOXHGCMLINPTR
{
    /* Index of the parameter. */
    uint32_t iParm;

    /* Offset in the first physical page of the region. */
    uint32_t offFirstPage;

    /* How many pages. */
    uint32_t cPages;

    /* Pointer to array of the GC physical addresses for these pages.
     * It is assumed that the physical address of the locked resident
     * guest page does not change.
     */
    RTGCPHYS *paPages;

} VBOXHGCMLINPTR;

struct VBOXHGCMCMD
{
    /* Active commands, list is protected by critsectHGCMCmdList. */
    struct VBOXHGCMCMD *pNext;
    struct VBOXHGCMCMD *pPrev;

    /* Size of memory buffer for this command structure, including trailing paHostParms.
     * This field simplifies loading of saved state.
     */
    uint32_t cbCmd;

    /* The type of the command. */
    VBOXHGCMCMDTYPE enmCmdType;

    /* Whether the command was cancelled by the guest. */
    bool fCancelled;

    /* GC physical address of the guest request. */
    RTGCPHYS        GCPhys;

    /* Request packet size */
    uint32_t        cbSize;

    /* Pointer to converted host parameters in case of a Call request.
     * Parameters follow this structure in the same memory block.
     */
    VBOXHGCMSVCPARM *paHostParms;

    /* Linear pointer parameters information. */
    int cLinPtrs;

    /* How many pages for all linptrs of this command.
     * Only valid if cLinPtrs > 0. This field simplifies loading of saved state.
     */
    int cLinPtrPages;

    /* Pointer to descriptions of linear pointers.  */
    VBOXHGCMLINPTR *paLinPtrs;
};

static int vmmdevHGCMCmdListLock (VMMDevState *pVMMDevState)
{
    int rc = RTCritSectEnter (&pVMMDevState->critsectHGCMCmdList);
    AssertRC (rc);
    return rc;
}

static void vmmdevHGCMCmdListUnlock (VMMDevState *pVMMDevState)
{
    int rc = RTCritSectLeave (&pVMMDevState->critsectHGCMCmdList);
    AssertRC (rc);
}

static int vmmdevHGCMAddCommand (VMMDevState *pVMMDevState, PVBOXHGCMCMD pCmd, RTGCPHYS GCPhys, uint32_t cbSize, VBOXHGCMCMDTYPE enmCmdType)
{
    /* PPDMDEVINS pDevIns = pVMMDevState->pDevIns; */

    int rc = vmmdevHGCMCmdListLock (pVMMDevState);

    if (RT_SUCCESS (rc))
    {
        LogFlowFunc(("%p type %d\n", pCmd, enmCmdType));

        /* Insert at the head of the list. The vmmdevHGCMLoadStateDone depends on this. */
        pCmd->pNext = pVMMDevState->pHGCMCmdList;
        pCmd->pPrev = NULL;

        if (pVMMDevState->pHGCMCmdList)
        {
            pVMMDevState->pHGCMCmdList->pPrev = pCmd;
        }

        pVMMDevState->pHGCMCmdList = pCmd;

        if (enmCmdType != VBOXHGCMCMDTYPE_LOADSTATE)
        {
            /* Loaded commands already have the right type. */
            pCmd->enmCmdType = enmCmdType;
        }
        pCmd->GCPhys = GCPhys;
        pCmd->cbSize = cbSize;

        /* Automatically enable HGCM events, if there are HGCM commands. */
        if (   enmCmdType == VBOXHGCMCMDTYPE_CONNECT
            || enmCmdType == VBOXHGCMCMDTYPE_DISCONNECT
            || enmCmdType == VBOXHGCMCMDTYPE_CALL)
        {
            Log(("vmmdevHGCMAddCommand: u32HGCMEnabled = %d\n", pVMMDevState->u32HGCMEnabled));
            if (ASMAtomicCmpXchgU32(&pVMMDevState->u32HGCMEnabled, 1, 0))
            {
                 VMMDevCtlSetGuestFilterMask (pVMMDevState, VMMDEV_EVENT_HGCM, 0);
            }
        }

        vmmdevHGCMCmdListUnlock (pVMMDevState);
    }

    return rc;
}

static int vmmdevHGCMRemoveCommand (VMMDevState *pVMMDevState, PVBOXHGCMCMD pCmd)
{
    /* PPDMDEVINS pDevIns = pVMMDevState->pDevIns; */

    int rc = vmmdevHGCMCmdListLock (pVMMDevState);

    if (RT_SUCCESS (rc))
    {
        LogFlowFunc(("%p\n", pCmd));

        if (pCmd->pNext)
        {
            pCmd->pNext->pPrev = pCmd->pPrev;
        }
        else
        {
           /* Tail, do nothing. */
        }

        if (pCmd->pPrev)
        {
            pCmd->pPrev->pNext = pCmd->pNext;
        }
        else
        {
            pVMMDevState->pHGCMCmdList = pCmd->pNext;
        }

        vmmdevHGCMCmdListUnlock (pVMMDevState);
    }

    return rc;
}


/**
 * Find a HGCM command by its physical address.
 *
 * The caller is responsible for taking the command list lock before calling
 * this function.
 *
 * @returns Pointer to the command on success, NULL otherwise.
 * @param   pThis           The VMMDev instance data.
 * @param   GCPhys          The physical address of the command we're looking
 *                          for.
 */
DECLINLINE(PVBOXHGCMCMD) vmmdevHGCMFindCommandLocked (VMMDevState *pThis, RTGCPHYS GCPhys)
{
    for (PVBOXHGCMCMD pCmd = pThis->pHGCMCmdList;
         pCmd;
         pCmd = pCmd->pNext)
    {
         if (pCmd->GCPhys == GCPhys)
             return pCmd;
    }
    return NULL;
}

static int vmmdevHGCMSaveLinPtr (PPDMDEVINS pDevIns,
                                 uint32_t iParm,
                                 RTGCPTR GCPtr,
                                 uint32_t u32Size,
                                 uint32_t iLinPtr,
                                 VBOXHGCMLINPTR *paLinPtrs,
                                 RTGCPHYS **ppPages)
{
    int rc = VINF_SUCCESS;

    AssertRelease (u32Size > 0);

    VBOXHGCMLINPTR *pLinPtr = &paLinPtrs[iLinPtr];

    /* Take the offset into the current page also into account! */
    u32Size += GCPtr & PAGE_OFFSET_MASK;

    uint32_t cPages = (u32Size + PAGE_SIZE - 1) / PAGE_SIZE;

    Log(("vmmdevHGCMSaveLinPtr: parm %d: %RGv %d = %d pages\n", iParm, GCPtr, u32Size, cPages));

    pLinPtr->iParm          = iParm;
    pLinPtr->offFirstPage   = GCPtr & PAGE_OFFSET_MASK;
    pLinPtr->cPages         = cPages;
    pLinPtr->paPages        = *ppPages;

    *ppPages += cPages;

    uint32_t iPage = 0;

    GCPtr &= PAGE_BASE_GC_MASK;

    /* Gonvert the guest linear pointers of pages to HC addresses. */
    while (iPage < cPages)
    {
        /* convert */
        RTGCPHYS GCPhys;

        rc = PDMDevHlpPhysGCPtr2GCPhys(pDevIns, GCPtr, &GCPhys);

        Log(("vmmdevHGCMSaveLinPtr: Page %d: %RGv -> %RGp. %Rrc\n", iPage, GCPtr, GCPhys, rc));

        if (RT_FAILURE (rc))
        {
            break;
        }

        /* store */
        pLinPtr->paPages[iPage++] = GCPhys;

        /* next */
        GCPtr += PAGE_SIZE;
    }

    AssertRelease (iPage == cPages);

    return rc;
}

static int vmmdevHGCMWriteLinPtr (PPDMDEVINS pDevIns,
                                  uint32_t iParm,
                                  void *pvHost,
                                  uint32_t u32Size,
                                  uint32_t iLinPtr,
                                  VBOXHGCMLINPTR *paLinPtrs)
{
    int rc = VINF_SUCCESS;

    VBOXHGCMLINPTR *pLinPtr = &paLinPtrs[iLinPtr];

    AssertRelease (u32Size > 0 && iParm == (uint32_t)pLinPtr->iParm);

    RTGCPHYS GCPhysDst = pLinPtr->paPages[0] + pLinPtr->offFirstPage;
    uint8_t *pu8Src    = (uint8_t *)pvHost;

    Log(("vmmdevHGCMWriteLinPtr: parm %d: size %d, cPages = %d\n", iParm, u32Size, pLinPtr->cPages));

    uint32_t iPage = 0;

    while (iPage < pLinPtr->cPages)
    {
        /* copy */
        uint32_t cbWrite = iPage == 0?
                               PAGE_SIZE - pLinPtr->offFirstPage:
                               PAGE_SIZE;

        Log(("vmmdevHGCMWriteLinPtr: page %d: dst %RGp, src %p, cbWrite %d\n", iPage, GCPhysDst, pu8Src, cbWrite));

        iPage++;

        if (cbWrite >= u32Size)
        {
            PDMDevHlpPhysWrite(pDevIns, GCPhysDst, pu8Src, u32Size);
            u32Size = 0;
            break;
        }

        PDMDevHlpPhysWrite(pDevIns, GCPhysDst, pu8Src, cbWrite);

        /* next */
        u32Size    -= cbWrite;
        pu8Src     += cbWrite;

        GCPhysDst   = pLinPtr->paPages[iPage];
    }

    AssertRelease (iPage == pLinPtr->cPages);
    Assert(u32Size == 0);

    return rc;
}

static int vmmdevHGCMPageListRead(PPDMDEVINSR3 pDevIns, void *pvDst, uint32_t cbDst, const HGCMPageListInfo *pPageListInfo)
{
    int rc = VINF_SUCCESS;

    uint8_t *pu8Dst = (uint8_t *)pvDst;
    uint32_t offPage = pPageListInfo->offFirstPage;
    size_t cbRemaining = (size_t)cbDst;

    uint32_t iPage;
    for (iPage = 0; iPage < pPageListInfo->cPages; iPage++)
    {
        if (cbRemaining == 0)
        {
            break;
        }

        size_t cbChunk = PAGE_SIZE - offPage;

        if (cbChunk > cbRemaining)
        {
            cbChunk = cbRemaining;
        }

        rc = PDMDevHlpPhysRead(pDevIns,
                               pPageListInfo->aPages[iPage] + offPage,
                               pu8Dst, cbChunk);

        AssertRCBreak(rc);

        offPage = 0; /* A next page is read from 0 offset. */
        cbRemaining -= cbChunk;
        pu8Dst += cbChunk;
    }

    return rc;
}

static int vmmdevHGCMPageListWrite(PPDMDEVINSR3 pDevIns, const HGCMPageListInfo *pPageListInfo, const void *pvSrc, uint32_t cbSrc)
{
    int rc = VINF_SUCCESS;

    uint8_t *pu8Src = (uint8_t *)pvSrc;
    uint32_t offPage = pPageListInfo->offFirstPage;
    size_t cbRemaining = (size_t)cbSrc;

    uint32_t iPage;
    for (iPage = 0; iPage < pPageListInfo->cPages; iPage++)
    {
        if (cbRemaining == 0)
        {
            break;
        }

        size_t cbChunk = PAGE_SIZE - offPage;

        if (cbChunk > cbRemaining)
        {
            cbChunk = cbRemaining;
        }

        rc = PDMDevHlpPhysWrite(pDevIns,
                                pPageListInfo->aPages[iPage] + offPage,
                                pu8Src, cbChunk);

        AssertRCBreak(rc);

        offPage = 0; /* A next page is read from 0 offset. */
        cbRemaining -= cbChunk;
        pu8Src += cbChunk;
    }

    return rc;
}

static void logRelSavedCmdSizeMismatch (const char *pszFunction, uint32_t cbExpected, uint32_t cbCmdSize)
{
    LogRel(("Warning: VMMDev %s command length %d (expected %d)\n",
            pszFunction, cbCmdSize, cbExpected));
}

int vmmdevHGCMConnect (VMMDevState *pVMMDevState, VMMDevHGCMConnect *pHGCMConnect, RTGCPHYS GCPhys)
{
    int rc = VINF_SUCCESS;

    uint32_t cbCmdSize = sizeof (struct VBOXHGCMCMD) + pHGCMConnect->header.header.size;

    PVBOXHGCMCMD pCmd = (PVBOXHGCMCMD)RTMemAllocZ (cbCmdSize);

    if (pCmd)
    {
        VMMDevHGCMConnect *pHGCMConnectCopy = (VMMDevHGCMConnect *)(pCmd+1);

        vmmdevHGCMAddCommand (pVMMDevState, pCmd, GCPhys, pHGCMConnect->header.header.size, VBOXHGCMCMDTYPE_CONNECT);

        memcpy(pHGCMConnectCopy, pHGCMConnect, pHGCMConnect->header.header.size);

        pCmd->cbCmd       = cbCmdSize;
        pCmd->paHostParms = NULL;
        pCmd->cLinPtrs = 0;
        pCmd->paLinPtrs = NULL;

        /* Only allow the guest to use existing services! */
        Assert(pHGCMConnect->loc.type == VMMDevHGCMLoc_LocalHost_Existing);
        pHGCMConnect->loc.type = VMMDevHGCMLoc_LocalHost_Existing;

        rc = pVMMDevState->pHGCMDrv->pfnConnect (pVMMDevState->pHGCMDrv, pCmd, &pHGCMConnectCopy->loc, &pHGCMConnectCopy->u32ClientID);
    }
    else
    {
        rc = VERR_NO_MEMORY;
    }

    return rc;
}

static int vmmdevHGCMConnectSaved (VMMDevState *pVMMDevState, VMMDevHGCMConnect *pHGCMConnect, bool *pfHGCMCalled, VBOXHGCMCMD *pSavedCmd)
{
    int rc = VINF_SUCCESS;

    uint32_t cbCmdSize = sizeof (struct VBOXHGCMCMD) + pHGCMConnect->header.header.size;

    if (pSavedCmd->cbCmd < cbCmdSize)
    {
        logRelSavedCmdSizeMismatch ("HGCMConnect", pSavedCmd->cbCmd, cbCmdSize);
        return VERR_INVALID_PARAMETER;
    }

    VMMDevHGCMConnect *pHGCMConnectCopy = (VMMDevHGCMConnect *)(pSavedCmd+1);

    memcpy(pHGCMConnectCopy, pHGCMConnect, pHGCMConnect->header.header.size);

    /* Only allow the guest to use existing services! */
    Assert(pHGCMConnect->loc.type == VMMDevHGCMLoc_LocalHost_Existing);
    pHGCMConnect->loc.type = VMMDevHGCMLoc_LocalHost_Existing;

    rc = pVMMDevState->pHGCMDrv->pfnConnect (pVMMDevState->pHGCMDrv, pSavedCmd, &pHGCMConnectCopy->loc, &pHGCMConnectCopy->u32ClientID);
    if (RT_SUCCESS (rc))
    {
        *pfHGCMCalled = true;
    }

    return rc;
}

int vmmdevHGCMDisconnect (VMMDevState *pVMMDevState, VMMDevHGCMDisconnect *pHGCMDisconnect, RTGCPHYS GCPhys)
{
    int rc = VINF_SUCCESS;

    uint32_t cbCmdSize = sizeof (struct VBOXHGCMCMD);

    PVBOXHGCMCMD pCmd = (PVBOXHGCMCMD)RTMemAllocZ (cbCmdSize);

    if (pCmd)
    {
        vmmdevHGCMAddCommand (pVMMDevState, pCmd, GCPhys, pHGCMDisconnect->header.header.size, VBOXHGCMCMDTYPE_DISCONNECT);

        pCmd->cbCmd       = cbCmdSize;
        pCmd->paHostParms = NULL;
        pCmd->cLinPtrs = 0;
        pCmd->paLinPtrs = NULL;

        rc = pVMMDevState->pHGCMDrv->pfnDisconnect (pVMMDevState->pHGCMDrv, pCmd, pHGCMDisconnect->u32ClientID);
    }
    else
    {
        rc = VERR_NO_MEMORY;
    }

    return rc;
}

static int vmmdevHGCMDisconnectSaved (VMMDevState *pVMMDevState, VMMDevHGCMDisconnect *pHGCMDisconnect, bool *pfHGCMCalled, VBOXHGCMCMD *pSavedCmd)
{
    int rc = VINF_SUCCESS;

    uint32_t cbCmdSize = sizeof (struct VBOXHGCMCMD);

    if (pSavedCmd->cbCmd < cbCmdSize)
    {
        logRelSavedCmdSizeMismatch ("HGCMConnect", pSavedCmd->cbCmd, cbCmdSize);
        return VERR_INVALID_PARAMETER;
    }

    rc = pVMMDevState->pHGCMDrv->pfnDisconnect (pVMMDevState->pHGCMDrv, pSavedCmd, pHGCMDisconnect->u32ClientID);
    if (RT_SUCCESS (rc))
    {
        *pfHGCMCalled = true;
    }

    return rc;
}

int vmmdevHGCMCall (VMMDevState *pVMMDevState, VMMDevHGCMCall *pHGCMCall, uint32_t cbHGCMCall, RTGCPHYS GCPhys, bool f64Bits)
{
    int rc = VINF_SUCCESS;

    Log(("vmmdevHGCMCall: client id = %d, function = %d, %s bit\n", pHGCMCall->u32ClientID, pHGCMCall->u32Function, f64Bits? "64": "32"));

    /* Compute size and allocate memory block to hold:
     *    struct VBOXHGCMCMD
     *    VBOXHGCMSVCPARM[cParms]
     *    memory buffers for pointer parameters.
     */

    uint32_t cParms = pHGCMCall->cParms;

    Log(("vmmdevHGCMCall: cParms = %d\n", cParms));

    /*
     * Compute size of required memory buffer.
     */

    uint32_t cbCmdSize = sizeof (struct VBOXHGCMCMD) + cParms * sizeof (VBOXHGCMSVCPARM);

    uint32_t i;

    uint32_t cLinPtrs = 0;
    uint32_t cLinPtrPages  = 0;

    if (f64Bits)
    {
#ifdef VBOX_WITH_64_BITS_GUESTS
        HGCMFunctionParameter64 *pGuestParm = VMMDEV_HGCM_CALL_PARMS64(pHGCMCall);
#else
        HGCMFunctionParameter *pGuestParm = VMMDEV_HGCM_CALL_PARMS(pHGCMCall);
        AssertFailed (); /* This code should not be called in this case */
#endif /* VBOX_WITH_64_BITS_GUESTS */

        /* Look for pointer parameters, which require a host buffer. */
        for (i = 0; i < cParms && RT_SUCCESS(rc); i++, pGuestParm++)
        {
            switch (pGuestParm->type)
            {
                case VMMDevHGCMParmType_LinAddr_In:  /* In (read) */
                case VMMDevHGCMParmType_LinAddr_Out: /* Out (write) */
                case VMMDevHGCMParmType_LinAddr:     /* In & Out */
                {
                    if (pGuestParm->u.Pointer.size > 0)
                    {
                        /* Only pointers with some actual data are counted. */
                        cbCmdSize += pGuestParm->u.Pointer.size;

                        cLinPtrs++;
                        /* Take the offset into the current page also into account! */
                        cLinPtrPages += ((pGuestParm->u.Pointer.u.linearAddr & PAGE_OFFSET_MASK)
                                          + pGuestParm->u.Pointer.size + PAGE_SIZE - 1) / PAGE_SIZE;
                    }

                    Log(("vmmdevHGCMCall: linptr size = %d\n", pGuestParm->u.Pointer.size));
                } break;

                case VMMDevHGCMParmType_PageList:
                {
                    cbCmdSize += pGuestParm->u.PageList.size;
                    Log(("vmmdevHGCMCall: pagelist size = %d\n", pGuestParm->u.PageList.size));
                } break;

                case VMMDevHGCMParmType_32bit:
                case VMMDevHGCMParmType_64bit:
                {
                } break;

                default:
                case VMMDevHGCMParmType_PhysAddr:
                {
                    AssertMsgFailed(("vmmdevHGCMCall: invalid parameter type %x\n", pGuestParm->type));
                    rc = VERR_INVALID_PARAMETER;
                    break;
                }
            }
        }
    }
    else
    {
#ifdef VBOX_WITH_64_BITS_GUESTS
        HGCMFunctionParameter32 *pGuestParm = VMMDEV_HGCM_CALL_PARMS32(pHGCMCall);
#else
        HGCMFunctionParameter *pGuestParm = VMMDEV_HGCM_CALL_PARMS(pHGCMCall);
#endif /* VBOX_WITH_64_BITS_GUESTS */

        /* Look for pointer parameters, which require a host buffer. */
        for (i = 0; i < cParms && RT_SUCCESS(rc); i++, pGuestParm++)
        {
            switch (pGuestParm->type)
            {
                case VMMDevHGCMParmType_LinAddr_In:  /* In (read) */
                case VMMDevHGCMParmType_LinAddr_Out: /* Out (write) */
                case VMMDevHGCMParmType_LinAddr:     /* In & Out */
                {
                    if (pGuestParm->u.Pointer.size > 0)
                    {
                        /* Only pointers with some actual data are counted. */
                        cbCmdSize += pGuestParm->u.Pointer.size;

                        cLinPtrs++;
                        /* Take the offset into the current page also into account! */
                        cLinPtrPages += ((pGuestParm->u.Pointer.u.linearAddr & PAGE_OFFSET_MASK)
                                          + pGuestParm->u.Pointer.size + PAGE_SIZE - 1) / PAGE_SIZE;
                    }

                    Log(("vmmdevHGCMCall: linptr size = %d\n", pGuestParm->u.Pointer.size));
                } break;

                case VMMDevHGCMParmType_PageList:
                {
                    cbCmdSize += pGuestParm->u.PageList.size;
                    Log(("vmmdevHGCMCall: pagelist size = %d\n", pGuestParm->u.PageList.size));
                } break;

                case VMMDevHGCMParmType_32bit:
                case VMMDevHGCMParmType_64bit:
                {
                } break;

                default:
                {
                    AssertMsgFailed(("vmmdevHGCMCall: invalid parameter type %x\n", pGuestParm->type));
                    rc = VERR_INVALID_PARAMETER;
                    break;
                }
            }
        }
    }

    if (RT_FAILURE (rc))
    {
        return rc;
    }

    PVBOXHGCMCMD pCmd = (PVBOXHGCMCMD)RTMemAlloc (cbCmdSize);

    if (pCmd == NULL)
    {
        return VERR_NO_MEMORY;
    }

    memset (pCmd, 0, sizeof (*pCmd));

    pCmd->cbCmd       = cbCmdSize;
    pCmd->paHostParms = NULL;
    pCmd->cLinPtrs    = cLinPtrs;
    pCmd->cLinPtrPages = cLinPtrPages;

    if (cLinPtrs > 0)
    {
        pCmd->paLinPtrs = (VBOXHGCMLINPTR *)RTMemAlloc (  sizeof (VBOXHGCMLINPTR) * cLinPtrs
                                                          + sizeof (RTGCPHYS) * cLinPtrPages);

        if (pCmd->paLinPtrs == NULL)
        {
            RTMemFree (pCmd);
            return VERR_NO_MEMORY;
        }
    }
    else
    {
        pCmd->paLinPtrs = NULL;
    }

    /* Process parameters, changing them to host context pointers for easy
     * processing by connector. Guest must insure that the pointed data is actually
     * in the guest RAM and remains locked there for entire request processing.
     */

    if (cParms != 0)
    {
        /* Compute addresses of host parms array and first memory buffer. */
        VBOXHGCMSVCPARM *pHostParm = (VBOXHGCMSVCPARM *)((char *)pCmd + sizeof (struct VBOXHGCMCMD));

        uint8_t *pcBuf = (uint8_t *)pHostParm + cParms * sizeof (VBOXHGCMSVCPARM);

        pCmd->paHostParms = pHostParm;

        uint32_t iLinPtr = 0;
        RTGCPHYS *pPages  = (RTGCPHYS *)((uint8_t *)pCmd->paLinPtrs + sizeof (VBOXHGCMLINPTR) *cLinPtrs);

        if (f64Bits)
        {
#ifdef VBOX_WITH_64_BITS_GUESTS
            HGCMFunctionParameter64 *pGuestParm = VMMDEV_HGCM_CALL_PARMS64(pHGCMCall);
#else
            HGCMFunctionParameter *pGuestParm = VMMDEV_HGCM_CALL_PARMS(pHGCMCall);
            AssertFailed (); /* This code should not be called in this case */
#endif /* VBOX_WITH_64_BITS_GUESTS */


            for (i = 0; i < cParms && RT_SUCCESS(rc); i++, pGuestParm++, pHostParm++)
            {
                switch (pGuestParm->type)
                {
                     case VMMDevHGCMParmType_32bit:
                     {
                         uint32_t u32 = pGuestParm->u.value32;

                         pHostParm->type = VBOX_HGCM_SVC_PARM_32BIT;
                         pHostParm->u.uint32 = u32;

                         Log(("vmmdevHGCMCall: uint32 guest parameter %u\n", u32));
                         break;
                     }

                     case VMMDevHGCMParmType_64bit:
                     {
                         uint64_t u64 = pGuestParm->u.value64;

                         pHostParm->type = VBOX_HGCM_SVC_PARM_64BIT;
                         pHostParm->u.uint64 = u64;

                         Log(("vmmdevHGCMCall: uint64 guest parameter %llu\n", u64));
                         break;
                     }

                     case VMMDevHGCMParmType_LinAddr_In:  /* In (read) */
                     case VMMDevHGCMParmType_LinAddr_Out: /* Out (write) */
                     case VMMDevHGCMParmType_LinAddr:     /* In & Out */
                     {
                         uint32_t size = pGuestParm->u.Pointer.size;
                         RTGCPTR linearAddr = pGuestParm->u.Pointer.u.linearAddr;

                         pHostParm->type = VBOX_HGCM_SVC_PARM_PTR;
                         pHostParm->u.pointer.size = size;

                         /* Copy guest data to an allocated buffer, so
                          * services can use the data.
                          */

                         if (size == 0)
                         {
                             pHostParm->u.pointer.addr = NULL;
                         }
                         else
                         {
                             /* Don't overdo it */
                             if (pGuestParm->type != VMMDevHGCMParmType_LinAddr_Out)
                                 rc = PDMDevHlpPhysReadGCVirt(pVMMDevState->pDevIns, pcBuf, linearAddr, size);
                             else
                                 rc = VINF_SUCCESS;

                             if (RT_SUCCESS(rc))
                             {
                                 pHostParm->u.pointer.addr = pcBuf;
                                 pcBuf += size;

                                 /* Remember the guest physical pages that belong to the virtual address region.
                                  * Do it for all linear pointers because load state will require In pointer info too.
                                  */
                                 rc = vmmdevHGCMSaveLinPtr (pVMMDevState->pDevIns, i, linearAddr, size, iLinPtr, pCmd->paLinPtrs, &pPages);

                                 iLinPtr++;
                             }
                         }

                         Log(("vmmdevHGCMCall: LinAddr guest parameter %RGv, rc = %Rrc\n", linearAddr, rc));
                         break;
                     }

                     case VMMDevHGCMParmType_PageList:
                     {
                         uint32_t size = pGuestParm->u.PageList.size;

                         /* Check that the page list info is within the request. */
                         if (   cbHGCMCall < sizeof (HGCMPageListInfo)
                             || pGuestParm->u.PageList.offset > cbHGCMCall - sizeof (HGCMPageListInfo))
                         {
                             rc = VERR_INVALID_PARAMETER;
                             break;
                         }

                         /* At least the structure is within. */
                         HGCMPageListInfo *pPageListInfo = (HGCMPageListInfo *)((uint8_t *)pHGCMCall + pGuestParm->u.PageList.offset);

                         uint32_t cbPageListInfo = sizeof (HGCMPageListInfo) + (pPageListInfo->cPages - 1) * sizeof (pPageListInfo->aPages[0]);

                         if (   pPageListInfo->cPages == 0
                             || cbHGCMCall < pGuestParm->u.PageList.offset + cbPageListInfo)
                         {
                             rc = VERR_INVALID_PARAMETER;
                             break;
                         }

                         pHostParm->type = VBOX_HGCM_SVC_PARM_PTR;
                         pHostParm->u.pointer.size = size;

                         /* Copy guest data to an allocated buffer, so
                          * services can use the data.
                          */

                         if (size == 0)
                         {
                             pHostParm->u.pointer.addr = NULL;
                         }
                         else
                         {
                             if (pPageListInfo->flags & VBOX_HGCM_F_PARM_DIRECTION_TO_HOST)
                             {
                                 /* Copy pages to the pcBuf[size]. */
                                 rc = vmmdevHGCMPageListRead(pVMMDevState->pDevIns, pcBuf, size, pPageListInfo);
                             }
                             else
                                 rc = VINF_SUCCESS;

                             if (RT_SUCCESS(rc))
                             {
                                 pHostParm->u.pointer.addr = pcBuf;
                                 pcBuf += size;
                             }
                         }

                         Log(("vmmdevHGCMCall: PageList guest parameter rc = %Rrc\n", rc));
                         break;
                     }

                    /* just to shut up gcc */
                    default:
                        AssertFailed();
                        break;
                }
            }
        }
        else
        {
#ifdef VBOX_WITH_64_BITS_GUESTS
            HGCMFunctionParameter32 *pGuestParm = VMMDEV_HGCM_CALL_PARMS32(pHGCMCall);
#else
            HGCMFunctionParameter *pGuestParm = VMMDEV_HGCM_CALL_PARMS(pHGCMCall);
#endif /* VBOX_WITH_64_BITS_GUESTS */

            for (i = 0; i < cParms && RT_SUCCESS(rc); i++, pGuestParm++, pHostParm++)
            {
                switch (pGuestParm->type)
                {
                     case VMMDevHGCMParmType_32bit:
                     {
                         uint32_t u32 = pGuestParm->u.value32;

                         pHostParm->type = VBOX_HGCM_SVC_PARM_32BIT;
                         pHostParm->u.uint32 = u32;

                         Log(("vmmdevHGCMCall: uint32 guest parameter %u\n", u32));
                         break;
                     }

                     case VMMDevHGCMParmType_64bit:
                     {
                         uint64_t u64 = pGuestParm->u.value64;

                         pHostParm->type = VBOX_HGCM_SVC_PARM_64BIT;
                         pHostParm->u.uint64 = u64;

                         Log(("vmmdevHGCMCall: uint64 guest parameter %llu\n", u64));
                         break;
                     }

                     case VMMDevHGCMParmType_LinAddr_In:  /* In (read) */
                     case VMMDevHGCMParmType_LinAddr_Out: /* Out (write) */
                     case VMMDevHGCMParmType_LinAddr:     /* In & Out */
                     {
                         uint32_t size = pGuestParm->u.Pointer.size;
                         RTGCPTR linearAddr = pGuestParm->u.Pointer.u.linearAddr;

                         pHostParm->type = VBOX_HGCM_SVC_PARM_PTR;
                         pHostParm->u.pointer.size = size;

                         /* Copy guest data to an allocated buffer, so
                          * services can use the data.
                          */

                         if (size == 0)
                         {
                             pHostParm->u.pointer.addr = NULL;
                         }
                         else
                         {
                             /* Don't overdo it */
                             if (pGuestParm->type != VMMDevHGCMParmType_LinAddr_Out)
                                 rc = PDMDevHlpPhysReadGCVirt(pVMMDevState->pDevIns, pcBuf, linearAddr, size);
                             else
                                 rc = VINF_SUCCESS;

                             if (RT_SUCCESS(rc))
                             {
                                 pHostParm->u.pointer.addr = pcBuf;
                                 pcBuf += size;

                                 /* Remember the guest physical pages that belong to the virtual address region.
                                  * Do it for all linear pointers because load state will require In pointer info too.
                                  */
                                 rc = vmmdevHGCMSaveLinPtr (pVMMDevState->pDevIns, i, linearAddr, size, iLinPtr, pCmd->paLinPtrs, &pPages);

                                 iLinPtr++;
                             }
                         }

                         Log(("vmmdevHGCMCall: LinAddr guest parameter %RGv, rc = %Rrc\n", linearAddr, rc));
                         break;
                     }

                     case VMMDevHGCMParmType_PageList:
                     {
                         uint32_t size = pGuestParm->u.PageList.size;

                         /* Check that the page list info is within the request. */
                         if (   cbHGCMCall < sizeof (HGCMPageListInfo)
                             || pGuestParm->u.PageList.offset > cbHGCMCall - sizeof (HGCMPageListInfo))
                         {
                             rc = VERR_INVALID_PARAMETER;
                             break;
                         }

                         /* At least the structure is within. */
                         HGCMPageListInfo *pPageListInfo = (HGCMPageListInfo *)((uint8_t *)pHGCMCall + pGuestParm->u.PageList.offset);

                         uint32_t cbPageListInfo = sizeof (HGCMPageListInfo) + (pPageListInfo->cPages - 1) * sizeof (pPageListInfo->aPages[0]);

                         if (   pPageListInfo->cPages == 0
                             || cbHGCMCall < pGuestParm->u.PageList.offset + cbPageListInfo)
                         {
                             rc = VERR_INVALID_PARAMETER;
                             break;
                         }

                         pHostParm->type = VBOX_HGCM_SVC_PARM_PTR;
                         pHostParm->u.pointer.size = size;

                         /* Copy guest data to an allocated buffer, so
                          * services can use the data.
                          */

                         if (size == 0)
                         {
                             pHostParm->u.pointer.addr = NULL;
                         }
                         else
                         {
                             if (pPageListInfo->flags & VBOX_HGCM_F_PARM_DIRECTION_TO_HOST)
                             {
                                 /* Copy pages to the pcBuf[size]. */
                                 rc = vmmdevHGCMPageListRead(pVMMDevState->pDevIns, pcBuf, size, pPageListInfo);
                             }
                             else
                                 rc = VINF_SUCCESS;

                             if (RT_SUCCESS(rc))
                             {
                                 pHostParm->u.pointer.addr = pcBuf;
                                 pcBuf += size;
                             }
                         }

                         Log(("vmmdevHGCMCall: PageList guest parameter rc = %Rrc\n", rc));
                         break;
                     }

                    /* just to shut up gcc */
                    default:
                        AssertFailed();
                        break;
                }
            }
        }
    }

    if (RT_SUCCESS (rc))
    {
        vmmdevHGCMAddCommand (pVMMDevState, pCmd, GCPhys, pHGCMCall->header.header.size, VBOXHGCMCMDTYPE_CALL);

        /* Pass the function call to HGCM connector for actual processing */
        rc = pVMMDevState->pHGCMDrv->pfnCall (pVMMDevState->pHGCMDrv, pCmd, pHGCMCall->u32ClientID,
                                              pHGCMCall->u32Function, cParms, pCmd->paHostParms);
    }
    else
    {
        if (pCmd->paLinPtrs)
        {
            RTMemFree (pCmd->paLinPtrs);
        }

        RTMemFree (pCmd);
    }

    return rc;
}

static void logRelLoadStatePointerIndexMismatch (uint32_t iParm, uint32_t iSavedParm, int iLinPtr, int cLinPtrs)
{
   LogRel(("Warning: VMMDev load state: a pointer parameter index mismatch %d (expected %d) (%d/%d)\n",
           (int)iParm, (int)iSavedParm, iLinPtr, cLinPtrs));
}

static void logRelLoadStateBufferSizeMismatch (uint32_t size, uint32_t iPage, uint32_t cPages)
{
    LogRel(("Warning: VMMDev load state: buffer size mismatch: size %d, page %d/%d\n",
            (int)size, (int)iPage, (int)cPages));
}


static int vmmdevHGCMCallSaved (VMMDevState *pVMMDevState, VMMDevHGCMCall *pHGCMCall, uint32_t cbHGCMCall, bool f64Bits, bool *pfHGCMCalled, VBOXHGCMCMD *pSavedCmd)
{
    int rc = VINF_SUCCESS;

    Log(("vmmdevHGCMCallSaved: client id = %d, function = %d, %s bit\n", pHGCMCall->u32ClientID, pHGCMCall->u32Function, f64Bits? "64": "32"));

    /* Compute size and allocate memory block to hold:
     *    struct VBOXHGCMCMD
     *    VBOXHGCMSVCPARM[cParms]
     *    memory buffers for pointer parameters.
     */

    uint32_t cParms = pHGCMCall->cParms;

    Log(("vmmdevHGCMCall: cParms = %d\n", cParms));

    /*
     * Compute size of required memory buffer.
     */

    pSavedCmd->paHostParms = NULL;

    /* Process parameters, changing them to host context pointers for easy
     * processing by connector. Guest must insure that the pointed data is actually
     * in the guest RAM and remains locked there for entire request processing.
     */

    if (cParms != 0)
    {
        /* Compute addresses of host parms array and first memory buffer. */
        VBOXHGCMSVCPARM *pHostParm = (VBOXHGCMSVCPARM *)((uint8_t *)pSavedCmd + sizeof (struct VBOXHGCMCMD));

        uint8_t *pu8Buf = (uint8_t *)pHostParm + cParms * sizeof (VBOXHGCMSVCPARM);

        pSavedCmd->paHostParms = pHostParm;

        uint32_t iParm;
        int iLinPtr = 0;

        if (f64Bits)
        {
#ifdef VBOX_WITH_64_BITS_GUESTS
            HGCMFunctionParameter64 *pGuestParm = VMMDEV_HGCM_CALL_PARMS64(pHGCMCall);
#else
            HGCMFunctionParameter *pGuestParm = VMMDEV_HGCM_CALL_PARMS(pHGCMCall);
            AssertFailed (); /* This code should not be called in this case */
#endif /* VBOX_WITH_64_BITS_GUESTS */

            for (iParm = 0; iParm < cParms && RT_SUCCESS(rc); iParm++, pGuestParm++, pHostParm++)
            {
                switch (pGuestParm->type)
                {
                     case VMMDevHGCMParmType_32bit:
                     {
                         uint32_t u32 = pGuestParm->u.value32;

                         pHostParm->type = VBOX_HGCM_SVC_PARM_32BIT;
                         pHostParm->u.uint32 = u32;

                         Log(("vmmdevHGCMCall: uint32 guest parameter %u\n", u32));
                         break;
                     }

                     case VMMDevHGCMParmType_64bit:
                     {
                         uint64_t u64 = pGuestParm->u.value64;

                         pHostParm->type = VBOX_HGCM_SVC_PARM_64BIT;
                         pHostParm->u.uint64 = u64;

                         Log(("vmmdevHGCMCall: uint64 guest parameter %llu\n", u64));
                         break;
                     }

                     case VMMDevHGCMParmType_LinAddr_In:  /* In (read) */
                     case VMMDevHGCMParmType_LinAddr_Out: /* Out (write) */
                     case VMMDevHGCMParmType_LinAddr:     /* In & Out */
                     {
                         uint32_t size = pGuestParm->u.Pointer.size;

                         pHostParm->type = VBOX_HGCM_SVC_PARM_PTR;
                         pHostParm->u.pointer.size = size;

                         /* Copy guest data to an allocated buffer, so
                          * services can use the data.
                          */

                         if (size == 0)
                         {
                             pHostParm->u.pointer.addr = NULL;
                         }
                         else
                         {
                             /* The saved command already have the page list in pCmd->paLinPtrs.
                              * Read data from guest pages.
                              */
                             /* Don't overdo it */
                             if (pGuestParm->type != VMMDevHGCMParmType_LinAddr_Out)
                             {
                                 if (   iLinPtr >= pSavedCmd->cLinPtrs
                                     || pSavedCmd->paLinPtrs[iLinPtr].iParm != iParm)
                                 {
                                     logRelLoadStatePointerIndexMismatch (iParm, pSavedCmd->paLinPtrs[iLinPtr].iParm, iLinPtr, pSavedCmd->cLinPtrs);
                                     rc = VERR_INVALID_PARAMETER;
                                 }
                                 else
                                 {
                                     VBOXHGCMLINPTR *pLinPtr = &pSavedCmd->paLinPtrs[iLinPtr];

                                     uint32_t iPage;
                                     uint32_t offPage = pLinPtr->offFirstPage;
                                     size_t cbRemaining = size;
                                     uint8_t *pu8Dst = pu8Buf;
                                     for (iPage = 0; iPage < pLinPtr->cPages; iPage++)
                                     {
                                         if (cbRemaining == 0)
                                         {
                                             logRelLoadStateBufferSizeMismatch (size, iPage, pLinPtr->cPages);
                                             break;
                                         }

                                         size_t cbChunk = PAGE_SIZE - offPage;

                                         if (cbChunk > cbRemaining)
                                         {
                                             cbChunk = cbRemaining;
                                         }

                                         rc = PDMDevHlpPhysRead(pVMMDevState->pDevIns,
                                                                pLinPtr->paPages[iPage] + offPage,
                                                                pu8Dst, cbChunk);

                                         AssertRCBreak(rc);

                                         offPage = 0; /* A next page is read from 0 offset. */
                                         cbRemaining -= cbChunk;
                                         pu8Dst += cbChunk;
                                     }
                                 }
                             }
                             else
                                 rc = VINF_SUCCESS;

                             if (RT_SUCCESS(rc))
                             {
                                 pHostParm->u.pointer.addr = pu8Buf;
                                 pu8Buf += size;

                                 iLinPtr++;
                             }
                         }

                         Log(("vmmdevHGCMCall: LinAddr guest parameter %RGv, rc = %Rrc\n",
                              pGuestParm->u.Pointer.u.linearAddr, rc));
                         break;
                     }

                     case VMMDevHGCMParmType_PageList:
                     {
                         uint32_t size = pGuestParm->u.PageList.size;

                         /* Check that the page list info is within the request. */
                         if (   cbHGCMCall < sizeof (HGCMPageListInfo)
                             || pGuestParm->u.PageList.offset > cbHGCMCall - sizeof (HGCMPageListInfo))
                         {
                             rc = VERR_INVALID_PARAMETER;
                             break;
                         }

                         /* At least the structure is within. */
                         HGCMPageListInfo *pPageListInfo = (HGCMPageListInfo *)((uint8_t *)pHGCMCall + pGuestParm->u.PageList.offset);

                         uint32_t cbPageListInfo = sizeof (HGCMPageListInfo) + (pPageListInfo->cPages - 1) * sizeof (pPageListInfo->aPages[0]);

                         if (   pPageListInfo->cPages == 0
                             || cbHGCMCall < pGuestParm->u.PageList.offset + cbPageListInfo)
                         {
                             rc = VERR_INVALID_PARAMETER;
                             break;
                         }

                         pHostParm->type = VBOX_HGCM_SVC_PARM_PTR;
                         pHostParm->u.pointer.size = size;

                         /* Copy guest data to an allocated buffer, so
                          * services can use the data.
                          */

                         if (size == 0)
                         {
                             pHostParm->u.pointer.addr = NULL;
                         }
                         else
                         {
                             if (pPageListInfo->flags & VBOX_HGCM_F_PARM_DIRECTION_TO_HOST)
                             {
                                 /* Copy pages to the pcBuf[size]. */
                                 rc = vmmdevHGCMPageListRead(pVMMDevState->pDevIns, pu8Buf, size, pPageListInfo);
                             }
                             else
                                 rc = VINF_SUCCESS;

                             if (RT_SUCCESS(rc))
                             {
                                 pHostParm->u.pointer.addr = pu8Buf;
                                 pu8Buf += size;
                             }
                         }

                         Log(("vmmdevHGCMCall: PageList guest parameter rc = %Rrc\n", rc));
                         break;
                     }

                    /* just to shut up gcc */
                    default:
                        AssertFailed();
                        break;
                }
            }
        }
        else
        {
#ifdef VBOX_WITH_64_BITS_GUESTS
            HGCMFunctionParameter32 *pGuestParm = VMMDEV_HGCM_CALL_PARMS32(pHGCMCall);
#else
            HGCMFunctionParameter *pGuestParm = VMMDEV_HGCM_CALL_PARMS(pHGCMCall);
#endif /* VBOX_WITH_64_BITS_GUESTS */

            for (iParm = 0; iParm < cParms && RT_SUCCESS(rc); iParm++, pGuestParm++, pHostParm++)
            {
                switch (pGuestParm->type)
                {
                     case VMMDevHGCMParmType_32bit:
                     {
                         uint32_t u32 = pGuestParm->u.value32;

                         pHostParm->type = VBOX_HGCM_SVC_PARM_32BIT;
                         pHostParm->u.uint32 = u32;

                         Log(("vmmdevHGCMCall: uint32 guest parameter %u\n", u32));
                         break;
                     }

                     case VMMDevHGCMParmType_64bit:
                     {
                         uint64_t u64 = pGuestParm->u.value64;

                         pHostParm->type = VBOX_HGCM_SVC_PARM_64BIT;
                         pHostParm->u.uint64 = u64;

                         Log(("vmmdevHGCMCall: uint64 guest parameter %llu\n", u64));
                         break;
                     }

                     case VMMDevHGCMParmType_LinAddr_In:  /* In (read) */
                     case VMMDevHGCMParmType_LinAddr_Out: /* Out (write) */
                     case VMMDevHGCMParmType_LinAddr:     /* In & Out */
                     {
                         uint32_t size = pGuestParm->u.Pointer.size;

                         pHostParm->type = VBOX_HGCM_SVC_PARM_PTR;
                         pHostParm->u.pointer.size = size;

                         /* Copy guest data to an allocated buffer, so
                          * services can use the data.
                          */

                         if (size == 0)
                         {
                             pHostParm->u.pointer.addr = NULL;
                         }
                         else
                         {
                             /* The saved command already have the page list in pCmd->paLinPtrs.
                              * Read data from guest pages.
                              */
                             /* Don't overdo it */
                             if (pGuestParm->type != VMMDevHGCMParmType_LinAddr_Out)
                             {
                                 if (   iLinPtr >= pSavedCmd->cLinPtrs
                                     || pSavedCmd->paLinPtrs[iLinPtr].iParm != iParm)
                                 {
                                     logRelLoadStatePointerIndexMismatch (iParm, pSavedCmd->paLinPtrs[iLinPtr].iParm, iLinPtr, pSavedCmd->cLinPtrs);
                                     rc = VERR_INVALID_PARAMETER;
                                 }
                                 else
                                 {
                                     VBOXHGCMLINPTR *pLinPtr = &pSavedCmd->paLinPtrs[iLinPtr];

                                     uint32_t iPage;
                                     uint32_t offPage = pLinPtr->offFirstPage;
                                     size_t cbRemaining = size;
                                     uint8_t *pu8Dst = pu8Buf;
                                     for (iPage = 0; iPage < pLinPtr->cPages; iPage++)
                                     {
                                         if (cbRemaining == 0)
                                         {
                                             logRelLoadStateBufferSizeMismatch (size, iPage, pLinPtr->cPages);
                                             break;
                                         }

                                         size_t cbChunk = PAGE_SIZE - offPage;

                                         if (cbChunk > cbRemaining)
                                         {
                                             cbChunk = cbRemaining;
                                         }

                                         rc = PDMDevHlpPhysRead(pVMMDevState->pDevIns,
                                                                pLinPtr->paPages[iPage] + offPage,
                                                                pu8Dst, cbChunk);

                                         AssertRCBreak(rc);

                                         offPage = 0; /* A next page is read from 0 offset. */
                                         cbRemaining -= cbChunk;
                                         pu8Dst += cbChunk;
                                     }
                                 }
                             }
                             else
                                 rc = VINF_SUCCESS;

                             if (RT_SUCCESS(rc))
                             {
                                 pHostParm->u.pointer.addr = pu8Buf;
                                 pu8Buf += size;

                                 iLinPtr++;
                             }
                         }

                         Log(("vmmdevHGCMCall: LinAddr guest parameter %RGv, rc = %Rrc\n",
                              pGuestParm->u.Pointer.u.linearAddr, rc));
                         break;
                     }

                     case VMMDevHGCMParmType_PageList:
                     {
                         uint32_t size = pGuestParm->u.PageList.size;

                         /* Check that the page list info is within the request. */
                         if (   cbHGCMCall < sizeof (HGCMPageListInfo)
                             || pGuestParm->u.PageList.offset > cbHGCMCall - sizeof (HGCMPageListInfo))
                         {
                             rc = VERR_INVALID_PARAMETER;
                             break;
                         }

                         /* At least the structure is within. */
                         HGCMPageListInfo *pPageListInfo = (HGCMPageListInfo *)((uint8_t *)pHGCMCall + pGuestParm->u.PageList.offset);

                         uint32_t cbPageListInfo = sizeof (HGCMPageListInfo) + (pPageListInfo->cPages - 1) * sizeof (pPageListInfo->aPages[0]);

                         if (   pPageListInfo->cPages == 0
                             || cbHGCMCall < pGuestParm->u.PageList.offset + cbPageListInfo)
                         {
                             rc = VERR_INVALID_PARAMETER;
                             break;
                         }

                         pHostParm->type = VBOX_HGCM_SVC_PARM_PTR;
                         pHostParm->u.pointer.size = size;

                         /* Copy guest data to an allocated buffer, so
                          * services can use the data.
                          */

                         if (size == 0)
                         {
                             pHostParm->u.pointer.addr = NULL;
                         }
                         else
                         {
                             if (pPageListInfo->flags & VBOX_HGCM_F_PARM_DIRECTION_TO_HOST)
                             {
                                 /* Copy pages to the pcBuf[size]. */
                                 rc = vmmdevHGCMPageListRead(pVMMDevState->pDevIns, pu8Buf, size, pPageListInfo);
                             }
                             else
                                 rc = VINF_SUCCESS;

                             if (RT_SUCCESS(rc))
                             {
                                 pHostParm->u.pointer.addr = pu8Buf;
                                 pu8Buf += size;
                             }
                         }

                         Log(("vmmdevHGCMCall: PageList guest parameter rc = %Rrc\n", rc));
                         break;
                     }

                    /* just to shut up gcc */
                    default:
                        AssertFailed();
                        break;
                }
            }
        }
    }

    if (RT_SUCCESS (rc))
    {
        /* Pass the function call to HGCM connector for actual processing */
        rc = pVMMDevState->pHGCMDrv->pfnCall (pVMMDevState->pHGCMDrv, pSavedCmd, pHGCMCall->u32ClientID, pHGCMCall->u32Function, cParms, pSavedCmd->paHostParms);
        if (RT_SUCCESS (rc))
        {
            *pfHGCMCalled = true;
        }
    }

    return rc;
}

/**
 * VMMDevReq_HGCMCancel worker.
 *
 * @thread EMT
 */
int vmmdevHGCMCancel (VMMDevState *pVMMDevState, VMMDevHGCMCancel *pHGCMCancel, RTGCPHYS GCPhys)
{
    NOREF(pHGCMCancel);
    int rc = vmmdevHGCMCancel2(pVMMDevState, GCPhys);
    return rc == VERR_NOT_FOUND ? VERR_INVALID_PARAMETER : rc;
}

/**
 * VMMDevReq_HGCMCancel2 worker.
 *
 * @retval  VINF_SUCCESS on success.
 * @retval  VERR_NOT_FOUND if the request was not found.
 * @retval  VERR_INVALID_PARAMETER if the request address is invalid.
 *
 * @param   pThis       The VMMDev instance data.
 * @param   GCPhys      The address of the request that should be cancelled.
 *
 * @thread EMT
 */
int vmmdevHGCMCancel2 (VMMDevState *pThis, RTGCPHYS GCPhys)
{
    if (    GCPhys == 0
        ||  GCPhys == NIL_RTGCPHYS
        ||  GCPhys == NIL_RTGCPHYS32)
    {
        Log(("vmmdevHGCMCancel2: GCPhys=%#x\n", GCPhys));
        return VERR_INVALID_PARAMETER;
    }

    /*
     * Locate the command and cancel it while under the protection of
     * the lock. hgcmCompletedWorker makes assumptions about this.
     */
    int rc = vmmdevHGCMCmdListLock (pThis);
    AssertRCReturn(rc, rc);

    PVBOXHGCMCMD pCmd = vmmdevHGCMFindCommandLocked (pThis, GCPhys);
    if (pCmd)
    {
        pCmd->fCancelled = true;
        Log(("vmmdevHGCMCancel2: Cancelled pCmd=%p / GCPhys=%#x\n", pCmd, GCPhys));
    }
    else
        rc = VERR_NOT_FOUND;

    vmmdevHGCMCmdListUnlock (pThis);
    return rc;
}

static int vmmdevHGCMCmdVerify (PVBOXHGCMCMD pCmd, VMMDevHGCMRequestHeader *pHeader)
{
    switch (pCmd->enmCmdType)
    {
        case VBOXHGCMCMDTYPE_CONNECT:
            if (   pHeader->header.requestType == VMMDevReq_HGCMConnect
                || pHeader->header.requestType == VMMDevReq_HGCMCancel) return VINF_SUCCESS;
            break;

        case VBOXHGCMCMDTYPE_DISCONNECT:
            if (   pHeader->header.requestType == VMMDevReq_HGCMDisconnect
                || pHeader->header.requestType == VMMDevReq_HGCMCancel) return VINF_SUCCESS;
            break;

        case VBOXHGCMCMDTYPE_CALL:
#ifdef VBOX_WITH_64_BITS_GUESTS
            if (   pHeader->header.requestType == VMMDevReq_HGCMCall32
                || pHeader->header.requestType == VMMDevReq_HGCMCall64
                || pHeader->header.requestType == VMMDevReq_HGCMCancel) return VINF_SUCCESS;
#else
            if (   pHeader->header.requestType == VMMDevReq_HGCMCall
                || pHeader->header.requestType == VMMDevReq_HGCMCancel) return VINF_SUCCESS;
#endif /* VBOX_WITH_64_BITS_GUESTS */

            break;

        default:
            AssertFailed ();
    }

    LogRel(("VMMDEV: Invalid HGCM command: pCmd->enmCmdType = 0x%08X, pHeader->header.requestType = 0x%08X\n",
          pCmd->enmCmdType, pHeader->header.requestType));
    return VERR_INVALID_PARAMETER;
}

#define PDMIHGCMPORT_2_VMMDEVSTATE(pInterface) ( (VMMDevState *) ((uintptr_t)pInterface - RT_OFFSETOF(VMMDevState, HGCMPort)) )

DECLCALLBACK(void) hgcmCompletedWorker (PPDMIHGCMPORT pInterface, int32_t result, PVBOXHGCMCMD pCmd)
{
    VMMDevState *pVMMDevState = PDMIHGCMPORT_2_VMMDEVSTATE(pInterface);

    int rc = VINF_SUCCESS;

    if (result == VINF_HGCM_SAVE_STATE)
    {
        /* If the completion routine was called because HGCM saves its state,
         * then currently nothing to be done here. The pCmd stays in the list
         * and will be saved later when the VMMDev state will be saved.
         *
         * It it assumed that VMMDev saves state after the HGCM services,
         * and, therefore, VBOXHGCMCMD structures are not removed by
         * vmmdevHGCMSaveState from the list, while HGCM uses them.
         */
        LogFlowFunc(("VINF_HGCM_SAVE_STATE for command %p\n", pCmd));
        return;
    }

    /*
     * The cancellation protocol requires us to remove the command here
     * and then check the flag. Cancelled commands must not be written
     * back to guest memory.
     */
    vmmdevHGCMRemoveCommand (pVMMDevState, pCmd);

    if (pCmd->fCancelled)
    {
        LogFlowFunc(("A cancelled command %p\n", pCmd));
    }
    else
    {
        /* Preallocated block for requests which have up to 8 parameters (most of requests). */
#ifdef VBOX_WITH_64_BITS_GUESTS
        uint8_t au8Prealloc[sizeof (VMMDevHGCMCall) + 8 * sizeof (HGCMFunctionParameter64)];
#else
        uint8_t au8Prealloc[sizeof (VMMDevHGCMCall) + 8 * sizeof (HGCMFunctionParameter)];
#endif /* VBOX_WITH_64_BITS_GUESTS */

        VMMDevHGCMRequestHeader *pHeader;

        if (pCmd->cbSize <= sizeof (au8Prealloc))
        {
            pHeader = (VMMDevHGCMRequestHeader *)&au8Prealloc[0];
        }
        else
        {
            pHeader = (VMMDevHGCMRequestHeader *)RTMemAlloc (pCmd->cbSize);
            if (pHeader == NULL)
            {
                LogRel(("VMMDev: Failed to allocate %u bytes for HGCM request completion!!!\n", pCmd->cbSize));

                /* Free it. The command have to be excluded from list of active commands anyway. */
                RTMemFree (pCmd);
                return;
            }
        }

        /*
         * Enter and leave the critical section here so we make sure
         * vmmdevRequestHandler has completed before we read & write
         * the request. (This isn't 100% optimal, but it solves the
         * 3.0 blocker.)
         */
        /** @todo s/pVMMDevState/pThis/g */
        /** @todo It would be faster if this interface would use MMIO2 memory and we
         *        didn't have to mess around with PDMDevHlpPhysRead/Write. We're
         *        reading the header 3 times now and writing the request back twice. */

        PDMCritSectEnter(&pVMMDevState->CritSect, VERR_SEM_BUSY);
        PDMCritSectLeave(&pVMMDevState->CritSect);

        PDMDevHlpPhysRead(pVMMDevState->pDevIns, pCmd->GCPhys, pHeader, pCmd->cbSize);

        /* Setup return codes. */
        pHeader->result = result;

        /* Verify the request type. */
        rc = vmmdevHGCMCmdVerify (pCmd, pHeader);

        if (RT_SUCCESS (rc))
        {
            /* Update parameters and data buffers. */

            switch (pHeader->header.requestType)
            {
#ifdef VBOX_WITH_64_BITS_GUESTS
            case VMMDevReq_HGCMCall64:
            {
                VMMDevHGCMCall *pHGCMCall = (VMMDevHGCMCall *)pHeader;

                uint32_t cParms = pHGCMCall->cParms;

                VBOXHGCMSVCPARM *pHostParm = pCmd->paHostParms;

                uint32_t i;
                uint32_t iLinPtr = 0;

                HGCMFunctionParameter64 *pGuestParm = VMMDEV_HGCM_CALL_PARMS64(pHGCMCall);

                for (i = 0; i < cParms; i++, pGuestParm++, pHostParm++)
                {
                    switch (pGuestParm->type)
                    {
                        case VMMDevHGCMParmType_32bit:
                        {
                            pGuestParm->u.value32 = pHostParm->u.uint32;
                        } break;

                        case VMMDevHGCMParmType_64bit:
                        {
                            pGuestParm->u.value64 = pHostParm->u.uint64;
                        } break;

                        case VMMDevHGCMParmType_LinAddr_In:  /* In (read) */
                        case VMMDevHGCMParmType_LinAddr_Out: /* Out (write) */
                        case VMMDevHGCMParmType_LinAddr:     /* In & Out */
                        {
                            /* Copy buffer back to guest memory. */
                            uint32_t size = pGuestParm->u.Pointer.size;

                            if (size > 0)
                            {
                                if (pGuestParm->type != VMMDevHGCMParmType_LinAddr_In)
                                {
                                    /* Use the saved page list to write data back to the guest RAM. */
                                    rc = vmmdevHGCMWriteLinPtr (pVMMDevState->pDevIns, i, pHostParm->u.pointer.addr,
                                                                size, iLinPtr, pCmd->paLinPtrs);
                                    AssertReleaseRC(rc);
                                }

                                /* All linptrs with size > 0 were saved. Advance the index to the next linptr. */
                                iLinPtr++;
                            }
                        } break;

                        case VMMDevHGCMParmType_PageList:
                        {
                            uint32_t cbHGCMCall = pCmd->cbSize; /* Size of the request. */

                            uint32_t size = pGuestParm->u.PageList.size;

                            /* Check that the page list info is within the request. */
                            if (   cbHGCMCall < sizeof (HGCMPageListInfo)
                                || pGuestParm->u.PageList.offset > cbHGCMCall - sizeof (HGCMPageListInfo))
                            {
                                rc = VERR_INVALID_PARAMETER;
                                break;
                            }

                            /* At least the structure is within. */
                            HGCMPageListInfo *pPageListInfo = (HGCMPageListInfo *)((uint8_t *)pHGCMCall + pGuestParm->u.PageList.offset);

                            uint32_t cbPageListInfo = sizeof (HGCMPageListInfo) + (pPageListInfo->cPages - 1) * sizeof (pPageListInfo->aPages[0]);

                            if (   pPageListInfo->cPages == 0
                                || cbHGCMCall < pGuestParm->u.PageList.offset + cbPageListInfo)
                            {
                                rc = VERR_INVALID_PARAMETER;
                                break;
                            }

                            if (size > 0)
                            {
                                if (pPageListInfo->flags & VBOX_HGCM_F_PARM_DIRECTION_FROM_HOST)
                                {
                                    /* Copy pHostParm->u.pointer.addr[pHostParm->u.pointer.size] to pages. */
                                    rc = vmmdevHGCMPageListWrite(pVMMDevState->pDevIns, pPageListInfo, pHostParm->u.pointer.addr, size);
                                }
                                else
                                    rc = VINF_SUCCESS;
                            }

                            Log(("vmmdevHGCMCall: PageList guest parameter rc = %Rrc\n", rc));
                        } break;

                        default:
                        {
                            /* This indicates that the guest request memory was corrupted. */
                            AssertReleaseMsgFailed(("hgcmCompleted: invalid parameter type %08X\n", pGuestParm->type));
                        }
                    }
                }
                break;
            }

            case VMMDevReq_HGCMCall32:
            {
                VMMDevHGCMCall *pHGCMCall = (VMMDevHGCMCall *)pHeader;

                uint32_t cParms = pHGCMCall->cParms;

                VBOXHGCMSVCPARM *pHostParm = pCmd->paHostParms;

                uint32_t i;
                uint32_t iLinPtr = 0;

                HGCMFunctionParameter32 *pGuestParm = VMMDEV_HGCM_CALL_PARMS32(pHGCMCall);

                for (i = 0; i < cParms; i++, pGuestParm++, pHostParm++)
                {
                    switch (pGuestParm->type)
                    {
                        case VMMDevHGCMParmType_32bit:
                        {
                            pGuestParm->u.value32 = pHostParm->u.uint32;
                        } break;

                        case VMMDevHGCMParmType_64bit:
                        {
                            pGuestParm->u.value64 = pHostParm->u.uint64;
                        } break;

                        case VMMDevHGCMParmType_LinAddr_In:  /* In (read) */
                        case VMMDevHGCMParmType_LinAddr_Out: /* Out (write) */
                        case VMMDevHGCMParmType_LinAddr:     /* In & Out */
                        {
                            /* Copy buffer back to guest memory. */
                            uint32_t size = pGuestParm->u.Pointer.size;

                            if (size > 0)
                            {
                                if (pGuestParm->type != VMMDevHGCMParmType_LinAddr_In)
                                {
                                    /* Use the saved page list to write data back to the guest RAM. */
                                    rc = vmmdevHGCMWriteLinPtr (pVMMDevState->pDevIns, i, pHostParm->u.pointer.addr, size, iLinPtr, pCmd->paLinPtrs);
                                    AssertReleaseRC(rc);
                                }

                                /* All linptrs with size > 0 were saved. Advance the index to the next linptr. */
                                iLinPtr++;
                            }
                        } break;

                        case VMMDevHGCMParmType_PageList:
                        {
                            uint32_t cbHGCMCall = pCmd->cbSize; /* Size of the request. */

                            uint32_t size = pGuestParm->u.PageList.size;

                            /* Check that the page list info is within the request. */
                            if (   cbHGCMCall < sizeof (HGCMPageListInfo)
                                || pGuestParm->u.PageList.offset > cbHGCMCall - sizeof (HGCMPageListInfo))
                            {
                                rc = VERR_INVALID_PARAMETER;
                                break;
                            }

                            /* At least the structure is within. */
                            HGCMPageListInfo *pPageListInfo = (HGCMPageListInfo *)((uint8_t *)pHGCMCall + pGuestParm->u.PageList.offset);

                            uint32_t cbPageListInfo = sizeof (HGCMPageListInfo) + (pPageListInfo->cPages - 1) * sizeof (pPageListInfo->aPages[0]);

                            if (   pPageListInfo->cPages == 0
                                || cbHGCMCall < pGuestParm->u.PageList.offset + cbPageListInfo)
                            {
                                rc = VERR_INVALID_PARAMETER;
                                break;
                            }

                            if (size > 0)
                            {
                                if (pPageListInfo->flags & VBOX_HGCM_F_PARM_DIRECTION_FROM_HOST)
                                {
                                    /* Copy pHostParm->u.pointer.addr[pHostParm->u.pointer.size] to pages. */
                                    rc = vmmdevHGCMPageListWrite(pVMMDevState->pDevIns, pPageListInfo, pHostParm->u.pointer.addr, size);
                                }
                                else
                                    rc = VINF_SUCCESS;
                            }

                            Log(("vmmdevHGCMCall: PageList guest parameter rc = %Rrc\n", rc));
                        } break;

                        default:
                        {
                            /* This indicates that the guest request memory was corrupted. */
                            AssertReleaseMsgFailed(("hgcmCompleted: invalid parameter type %08X\n", pGuestParm->type));
                        }
                    }
                }
                break;
            }
#else
            case VMMDevReq_HGCMCall:
            {
                VMMDevHGCMCall *pHGCMCall = (VMMDevHGCMCall *)pHeader;

                uint32_t cParms = pHGCMCall->cParms;

                VBOXHGCMSVCPARM *pHostParm = pCmd->paHostParms;

                uint32_t i;
                uint32_t iLinPtr = 0;

                HGCMFunctionParameter *pGuestParm = VMMDEV_HGCM_CALL_PARMS(pHGCMCall);

                for (i = 0; i < cParms; i++, pGuestParm++, pHostParm++)
                {
                    switch (pGuestParm->type)
                    {
                        case VMMDevHGCMParmType_32bit:
                        {
                            pGuestParm->u.value32 = pHostParm->u.uint32;
                        } break;

                        case VMMDevHGCMParmType_64bit:
                        {
                            pGuestParm->u.value64 = pHostParm->u.uint64;
                        } break;

                        case VMMDevHGCMParmType_LinAddr_In:  /* In (read) */
                        case VMMDevHGCMParmType_LinAddr_Out: /* Out (write) */
                        case VMMDevHGCMParmType_LinAddr:     /* In & Out */
                        {
                            /* Copy buffer back to guest memory. */
                            uint32_t size = pGuestParm->u.Pointer.size;

                            if (size > 0)
                            {
                                if (pGuestParm->type != VMMDevHGCMParmType_LinAddr_In)
                                {
                                    /* Use the saved page list to write data back to the guest RAM. */
                                    rc = vmmdevHGCMWriteLinPtr (pVMMDevState->pDevIns, i, pHostParm->u.pointer.addr, size, iLinPtr, pCmd->paLinPtrs);
                                    AssertReleaseRC(rc);
                                }

                                /* All linptrs with size > 0 were saved. Advance the index to the next linptr. */
                                iLinPtr++;
                            }
                        } break;

                        case VMMDevHGCMParmType_PageList:
                        {
                            uint32_t cbHGCMCall = pCmd->cbSize; /* Size of the request. */

                            uint32_t size = pGuestParm->u.PageList.size;

                            /* Check that the page list info is within the request. */
                            if (   cbHGCMCall < sizeof (HGCMPageListInfo)
                                || pGuestParm->u.PageList.offset > cbHGCMCall - sizeof (HGCMPageListInfo))
                            {
                                rc = VERR_INVALID_PARAMETER;
                                break;
                            }

                            /* At least the structure is within. */
                            HGCMPageListInfo *pPageListInfo = (HGCMPageListInfo *)((uint8_t *)pHGCMCall + pGuestParm->u.PageList.offset);

                            uint32_t cbPageListInfo = sizeof (HGCMPageListInfo) + (pPageListInfo->cPages - 1) * sizeof (pPageListInfo->aPages[0]);

                            if (   pPageListInfo->cPages == 0
                                || cbHGCMCall < pGuestParm->u.PageList.offset + cbPageListInfo)
                            {
                                rc = VERR_INVALID_PARAMETER;
                                break;
                            }

                            if (size > 0)
                            {
                                if (pPageListInfo->flags & VBOX_HGCM_F_PARM_DIRECTION_FROM_HOST)
                                {
                                    /* Copy pHostParm->u.pointer.addr[pHostParm->u.pointer.size] to pages. */
                                    rc = vmmdevHGCMPageListWrite(pVMMDevState->pDevIns, pPageListInfo, pHostParm->u.pointer.addr, size);
                                }
                                else
                                    rc = VINF_SUCCESS;
                            }

                            Log(("vmmdevHGCMCall: PageList guest parameter rc = %Rrc\n", rc));
                        } break;

                        default:
                        {
                            /* This indicates that the guest request memory was corrupted. */
                            AssertReleaseMsgFailed(("hgcmCompleted: invalid parameter type %08X\n", pGuestParm->type));
                        }
                    }
                }
                break;
            }
#endif /* VBOX_WITH_64_BITS_GUESTS */
            case VMMDevReq_HGCMConnect:
            {
                VMMDevHGCMConnect *pHGCMConnectCopy = (VMMDevHGCMConnect *)(pCmd+1);

                /* save the client id in the guest request packet */
                VMMDevHGCMConnect *pHGCMConnect = (VMMDevHGCMConnect *)pHeader;
                pHGCMConnect->u32ClientID = pHGCMConnectCopy->u32ClientID;
                break;
            }

            default:
                /* make gcc happy */
                break;
            }
        }
        else
        {
            /* Command type is wrong. Return error to the guest. */
            pHeader->header.rc = rc;
        }

        /* Mark request as processed. */
        pHeader->fu32Flags |= VBOX_HGCM_REQ_DONE;

        /* Write back the request */
        PDMDevHlpPhysWrite(pVMMDevState->pDevIns, pCmd->GCPhys, pHeader, pCmd->cbSize);

        /* Now, when the command was removed from the internal list, notify the guest. */
        VMMDevNotifyGuest (pVMMDevState, VMMDEV_EVENT_HGCM);

        if ((uint8_t *)pHeader != &au8Prealloc[0])
        {
            /* Only if it was allocated from heap. */
            RTMemFree (pHeader);
        }
    }

    /* Deallocate the command memory. */
    if (pCmd->paLinPtrs)
    {
        RTMemFree (pCmd->paLinPtrs);
    }

    RTMemFree (pCmd);

    return;
}

DECLCALLBACK(void) hgcmCompleted (PPDMIHGCMPORT pInterface, int32_t result, PVBOXHGCMCMD pCmd)
{
    VMMDevState *pVMMDevState = PDMIHGCMPORT_2_VMMDEVSTATE(pInterface);

/** @todo no longer necessary to forward to EMT, but it might be more
 *        efficient...? */
    /* Not safe to execute asynchroneously; forward to EMT */
    int rc = VMR3ReqCallEx(PDMDevHlpGetVM(pVMMDevState->pDevIns), VMCPUID_ANY, NULL, 0, VMREQFLAGS_NO_WAIT | VMREQFLAGS_VOID,
                           (PFNRT)hgcmCompletedWorker, 3, pInterface, result, pCmd);
    AssertRC(rc);
}

/* @thread EMT */
int vmmdevHGCMSaveState(VMMDevState *pVMMDevState, PSSMHANDLE pSSM)
{
    /* Save information about pending requests.
     * Only GCPtrs are of interest.
     */
    int rc = VINF_SUCCESS;

    LogFlowFunc(("\n"));

    /* Compute how many commands are pending. */
    uint32_t cCmds = 0;

    PVBOXHGCMCMD pIter = pVMMDevState->pHGCMCmdList;

    while (pIter)
    {
        LogFlowFunc (("pIter %p\n", pIter));
        cCmds++;
        pIter = pIter->pNext;
    }

    LogFlowFunc(("cCmds = %d\n", cCmds));

    /* Save number of commands. */
    rc = SSMR3PutU32(pSSM, cCmds);
    AssertRCReturn(rc, rc);

    if (cCmds > 0)
    {
        pIter = pVMMDevState->pHGCMCmdList;

        while (pIter)
        {
            PVBOXHGCMCMD pNext = pIter->pNext;

            LogFlowFunc (("Saving %RGp, size %d\n", pIter->GCPhys, pIter->cbSize));

            /* GC physical address of the guest request. */
            rc = SSMR3PutGCPhys(pSSM, pIter->GCPhys);
            AssertRCReturn(rc, rc);

            /* Request packet size */
            rc = SSMR3PutU32(pSSM, pIter->cbSize);
            AssertRCReturn(rc, rc);

            /*
             * Version 9+: save complete information about commands.
             */

            /* Size of entire command. */
            rc = SSMR3PutU32(pSSM, pIter->cbCmd);
            AssertRCReturn(rc, rc);

            /* The type of the command. */
            rc = SSMR3PutU32(pSSM, (uint32_t)pIter->enmCmdType);
            AssertRCReturn(rc, rc);

            /* Whether the command was cancelled by the guest. */
            rc = SSMR3PutBool(pSSM, pIter->fCancelled);
            AssertRCReturn(rc, rc);

            /* Linear pointer parameters information. How many pointers. Always 0 if not a call command. */
            rc = SSMR3PutU32(pSSM, (uint32_t)pIter->cLinPtrs);
            AssertRCReturn(rc, rc);

            if (pIter->cLinPtrs > 0)
            {
                /* How many pages for all linptrs in this command. */
                rc = SSMR3PutU32(pSSM, (uint32_t)pIter->cLinPtrPages);
                AssertRCReturn(rc, rc);
            }

            int i;
            for (i = 0; i < pIter->cLinPtrs; i++)
            {
                /* Pointer to descriptions of linear pointers.  */
                VBOXHGCMLINPTR *pLinPtr = &pIter->paLinPtrs[i];

                /* Index of the parameter. */
                rc = SSMR3PutU32(pSSM, (uint32_t)pLinPtr->iParm);
                AssertRCReturn(rc, rc);

                /* Offset in the first physical page of the region. */
                rc = SSMR3PutU32(pSSM, pLinPtr->offFirstPage);
                AssertRCReturn(rc, rc);

                /* How many pages. */
                rc = SSMR3PutU32(pSSM, pLinPtr->cPages);
                AssertRCReturn(rc, rc);

                uint32_t iPage;
                for (iPage = 0; iPage < pLinPtr->cPages; iPage++)
                {
                    /* Array of the GC physical addresses for these pages.
                     * It is assumed that the physical address of the locked resident
                     * guest page does not change.
                     */
                    rc = SSMR3PutGCPhys(pSSM, pLinPtr->paPages[iPage]);
                    AssertRCReturn(rc, rc);
                }
            }

            /* A reserved field, will allow to extend saved data for a command. */
            rc = SSMR3PutU32(pSSM, 0);
            AssertRCReturn(rc, rc);

            vmmdevHGCMRemoveCommand (pVMMDevState, pIter);

            pIter = pNext;
        }
    }

    /* A reserved field, will allow to extend saved data for VMMDevHGCM. */
    rc = SSMR3PutU32(pSSM, 0);
    AssertRCReturn(rc, rc);

    return rc;
}

/* @thread EMT */
int vmmdevHGCMLoadState(VMMDevState *pVMMDevState, PSSMHANDLE pSSM, uint32_t u32Version)
{
    int rc = VINF_SUCCESS;

    LogFlowFunc(("\n"));

    /* Read how many commands were pending. */
    uint32_t cCmds = 0;
    rc = SSMR3GetU32(pSSM, &cCmds);
    AssertRCReturn(rc, rc);

    LogFlowFunc(("cCmds = %d\n", cCmds));

    if (   SSM_VERSION_MAJOR(u32Version) ==  0
        && SSM_VERSION_MINOR(u32Version) < 9)
    {
        /* Only the guest physical address is saved. */
        while (cCmds--)
        {
            RTGCPHYS GCPhys;
            uint32_t cbSize;

            rc = SSMR3GetGCPhys(pSSM, &GCPhys);
            AssertRCReturn(rc, rc);

            rc = SSMR3GetU32(pSSM, &cbSize);
            AssertRCReturn(rc, rc);

            LogFlowFunc (("Restoring %RGp size %x bytes\n", GCPhys, cbSize));

            PVBOXHGCMCMD pCmd = (PVBOXHGCMCMD)RTMemAllocZ (sizeof (struct VBOXHGCMCMD));
            AssertReturn(pCmd, VERR_NO_MEMORY);

            vmmdevHGCMAddCommand (pVMMDevState, pCmd, GCPhys, cbSize, VBOXHGCMCMDTYPE_LOADSTATE);
        }
    }
    else
    {
        /*
         * Version 9+: Load complete information about commands.
         */
        uint32_t u32;
        bool f;

        while (cCmds--)
        {
            RTGCPHYS GCPhys;
            uint32_t cbSize;

            /* GC physical address of the guest request. */
            rc = SSMR3GetGCPhys(pSSM, &GCPhys);
            AssertRCReturn(rc, rc);

            /* The request packet size */
            rc = SSMR3GetU32(pSSM, &cbSize);
            AssertRCReturn(rc, rc);

            LogFlowFunc (("Restoring %RGp size %x bytes\n", GCPhys, cbSize));

            /* Size of entire command. */
            rc = SSMR3GetU32(pSSM, &u32);
            AssertRCReturn(rc, rc);

            PVBOXHGCMCMD pCmd = (PVBOXHGCMCMD)RTMemAllocZ (u32);
            AssertReturn(pCmd, VERR_NO_MEMORY);
            pCmd->cbCmd = u32;

            /* The type of the command. */
            rc = SSMR3GetU32(pSSM, &u32);
            AssertRCReturn(rc, rc);
            pCmd->enmCmdType = (VBOXHGCMCMDTYPE)u32;

            /* Whether the command was cancelled by the guest. */
            rc = SSMR3GetBool(pSSM, &f);
            AssertRCReturn(rc, rc);
            pCmd->fCancelled = f;

            /* Linear pointer parameters information. How many pointers. Always 0 if not a call command. */
            rc = SSMR3GetU32(pSSM, &u32);
            AssertRCReturn(rc, rc);
            pCmd->cLinPtrs = u32;

            if (pCmd->cLinPtrs > 0)
            {
                /* How many pages for all linptrs in this command. */
                rc = SSMR3GetU32(pSSM, &u32);
                AssertRCReturn(rc, rc);
                pCmd->cLinPtrPages = u32;

                pCmd->paLinPtrs = (VBOXHGCMLINPTR *)RTMemAllocZ (  sizeof (VBOXHGCMLINPTR) * pCmd->cLinPtrs
                                                                 + sizeof (RTGCPHYS) * pCmd->cLinPtrPages);
                AssertReturn(pCmd->paLinPtrs, VERR_NO_MEMORY);

                RTGCPHYS *pPages = (RTGCPHYS *)((uint8_t *)pCmd->paLinPtrs + sizeof (VBOXHGCMLINPTR) * pCmd->cLinPtrs);
                int cPages = 0;

                int i;
                for (i = 0; i < pCmd->cLinPtrs; i++)
                {
                    /* Pointer to descriptions of linear pointers. */
                    VBOXHGCMLINPTR *pLinPtr = &pCmd->paLinPtrs[i];

                    pLinPtr->paPages = pPages;

                    /* Index of the parameter. */
                    rc = SSMR3GetU32(pSSM, &u32);
                    AssertRCReturn(rc, rc);
                    pLinPtr->iParm = u32;

                    /* Offset in the first physical page of the region. */
                    rc = SSMR3GetU32(pSSM, &u32);
                    AssertRCReturn(rc, rc);
                    pLinPtr->offFirstPage = u32;

                    /* How many pages. */
                    rc = SSMR3GetU32(pSSM, &u32);
                    AssertRCReturn(rc, rc);
                    pLinPtr->cPages = u32;

                    uint32_t iPage;
                    for (iPage = 0; iPage < pLinPtr->cPages; iPage++)
                    {
                        /* Array of the GC physical addresses for these pages.
                         * It is assumed that the physical address of the locked resident
                         * guest page does not change.
                         */
                        RTGCPHYS GCPhysPage;
                        rc = SSMR3GetGCPhys(pSSM, &GCPhysPage);
                        AssertRCReturn(rc, rc);

                        /* Verify that the number of loaded pages is valid. */
                        cPages++;
                        if (cPages > pCmd->cLinPtrPages)
                        {
                            LogRel(("VMMDevHGCM load state failure: cPages %d, expected %d, ptr %d/%d\n",
                                    cPages, pCmd->cLinPtrPages, i, pCmd->cLinPtrs));
                            return VERR_SSM_UNEXPECTED_DATA;
                        }

                        *pPages++ = GCPhysPage;
                    }
                }
            }

            /* A reserved field, will allow to extend saved data for a command. */
            rc = SSMR3GetU32(pSSM, &u32);
            AssertRCReturn(rc, rc);

            vmmdevHGCMAddCommand (pVMMDevState, pCmd, GCPhys, cbSize, VBOXHGCMCMDTYPE_LOADSTATE);
        }

        /* A reserved field, will allow to extend saved data for VMMDevHGCM. */
        rc = SSMR3GetU32(pSSM, &u32);
        AssertRCReturn(rc, rc);
    }

    return rc;
}

/* @thread EMT */
int vmmdevHGCMLoadStateDone(VMMDevState *pVMMDevState, PSSMHANDLE pSSM)
{
    LogFlowFunc(("\n"));

    /* Reissue pending requests. */
    PPDMDEVINS pDevIns = pVMMDevState->pDevIns;

    int rc = vmmdevHGCMCmdListLock (pVMMDevState);

    if (RT_SUCCESS (rc))
    {
        /* Start from the current list head and commands loaded from saved state.
         * New commands will be inserted at the list head, so they will not be seen by
         * this loop.
         */
        PVBOXHGCMCMD pIter = pVMMDevState->pHGCMCmdList;

        while (pIter)
        {
            /* This will remove the command from the list if resubmitting fails. */
            bool fHGCMCalled = false;

            LogFlowFunc (("pIter %p\n", pIter));

            PVBOXHGCMCMD pNext = pIter->pNext;

            VMMDevHGCMRequestHeader *requestHeader = (VMMDevHGCMRequestHeader *)RTMemAllocZ (pIter->cbSize);
            Assert(requestHeader);
            if (requestHeader == NULL)
                return VERR_NO_MEMORY;

            PDMDevHlpPhysRead(pDevIns, pIter->GCPhys, requestHeader, pIter->cbSize);

            /* the structure size must be greater or equal to the header size */
            if (requestHeader->header.size < sizeof(VMMDevHGCMRequestHeader))
            {
                Log(("VMMDev request header size too small! size = %d\n", requestHeader->header.size));
            }
            else
            {
                /* check the version of the header structure */
                if (requestHeader->header.version != VMMDEV_REQUEST_HEADER_VERSION)
                {
                    Log(("VMMDev: guest header version (0x%08X) differs from ours (0x%08X)\n", requestHeader->header.version, VMMDEV_REQUEST_HEADER_VERSION));
                }
                else
                {
                    Log(("VMMDev request issued: %d, command type %d\n", requestHeader->header.requestType, pIter->enmCmdType));

                    /* Use the saved command type. Even if the guest has changed the memory already,
                     * HGCM should see the same command as it was before saving state.
                     */
                    switch (pIter->enmCmdType)
                    {
                        case VBOXHGCMCMDTYPE_CONNECT:
                        {
                            if (requestHeader->header.size < sizeof(VMMDevHGCMConnect))
                            {
                                AssertMsgFailed(("VMMDevReq_HGCMConnect structure has invalid size!\n"));
                                requestHeader->header.rc = VERR_INVALID_PARAMETER;
                            }
                            else if (!pVMMDevState->pHGCMDrv)
                            {
                                Log(("VMMDevReq_HGCMConnect HGCM Connector is NULL!\n"));
                                requestHeader->header.rc = VERR_NOT_SUPPORTED;
                            }
                            else
                            {
                                VMMDevHGCMConnect *pHGCMConnect = (VMMDevHGCMConnect *)requestHeader;

                                Log(("VMMDevReq_HGCMConnect\n"));

                                requestHeader->header.rc = vmmdevHGCMConnectSaved (pVMMDevState, pHGCMConnect, &fHGCMCalled, pIter);
                            }
                            break;
                        }

                        case VBOXHGCMCMDTYPE_DISCONNECT:
                        {
                            if (requestHeader->header.size < sizeof(VMMDevHGCMDisconnect))
                            {
                                AssertMsgFailed(("VMMDevReq_HGCMDisconnect structure has invalid size!\n"));
                                requestHeader->header.rc = VERR_INVALID_PARAMETER;
                            }
                            else if (!pVMMDevState->pHGCMDrv)
                            {
                                Log(("VMMDevReq_HGCMDisconnect HGCM Connector is NULL!\n"));
                                requestHeader->header.rc = VERR_NOT_SUPPORTED;
                            }
                            else
                            {
                                VMMDevHGCMDisconnect *pHGCMDisconnect = (VMMDevHGCMDisconnect *)requestHeader;

                                Log(("VMMDevReq_VMMDevHGCMDisconnect\n"));
                                requestHeader->header.rc = vmmdevHGCMDisconnectSaved (pVMMDevState, pHGCMDisconnect, &fHGCMCalled, pIter);
                            }
                            break;
                        }

                        case VBOXHGCMCMDTYPE_CALL:
                        {
                            if (requestHeader->header.size < sizeof(VMMDevHGCMCall))
                            {
                                AssertMsgFailed(("VMMDevReq_HGCMCall structure has invalid size!\n"));
                                requestHeader->header.rc = VERR_INVALID_PARAMETER;
                            }
                            else if (!pVMMDevState->pHGCMDrv)
                            {
                                Log(("VMMDevReq_HGCMCall HGCM Connector is NULL!\n"));
                                requestHeader->header.rc = VERR_NOT_SUPPORTED;
                            }
                            else
                            {
                                VMMDevHGCMCall *pHGCMCall = (VMMDevHGCMCall *)requestHeader;

                                Log(("VMMDevReq_HGCMCall: sizeof (VMMDevHGCMRequest) = %04X\n", sizeof (VMMDevHGCMCall)));

                                Log(("%.*Rhxd\n", requestHeader->header.size, requestHeader));

#ifdef VBOX_WITH_64_BITS_GUESTS
                                bool f64Bits = (requestHeader->header.requestType == VMMDevReq_HGCMCall64);
#else
                                bool f64Bits = false;
#endif /* VBOX_WITH_64_BITS_GUESTS */
                                requestHeader->header.rc = vmmdevHGCMCallSaved (pVMMDevState, pHGCMCall, requestHeader->header.size, f64Bits, &fHGCMCalled, pIter);
                            }
                            break;
                        }
                        case VBOXHGCMCMDTYPE_LOADSTATE:
                        {
                            /* Old saved state. */
                            switch (requestHeader->header.requestType)
                            {
                                case VMMDevReq_HGCMConnect:
                                {
                                    if (requestHeader->header.size < sizeof(VMMDevHGCMConnect))
                                    {
                                        AssertMsgFailed(("VMMDevReq_HGCMConnect structure has invalid size!\n"));
                                        requestHeader->header.rc = VERR_INVALID_PARAMETER;
                                    }
                                    else if (!pVMMDevState->pHGCMDrv)
                                    {
                                        Log(("VMMDevReq_HGCMConnect HGCM Connector is NULL!\n"));
                                        requestHeader->header.rc = VERR_NOT_SUPPORTED;
                                    }
                                    else
                                    {
                                        VMMDevHGCMConnect *pHGCMConnect = (VMMDevHGCMConnect *)requestHeader;

                                        Log(("VMMDevReq_HGCMConnect\n"));

                                        requestHeader->header.rc = vmmdevHGCMConnect (pVMMDevState, pHGCMConnect, pIter->GCPhys);
                                    }
                                    break;
                                }

                                case VMMDevReq_HGCMDisconnect:
                                {
                                    if (requestHeader->header.size < sizeof(VMMDevHGCMDisconnect))
                                    {
                                        AssertMsgFailed(("VMMDevReq_HGCMDisconnect structure has invalid size!\n"));
                                        requestHeader->header.rc = VERR_INVALID_PARAMETER;
                                    }
                                    else if (!pVMMDevState->pHGCMDrv)
                                    {
                                        Log(("VMMDevReq_HGCMDisconnect HGCM Connector is NULL!\n"));
                                        requestHeader->header.rc = VERR_NOT_SUPPORTED;
                                    }
                                    else
                                    {
                                        VMMDevHGCMDisconnect *pHGCMDisconnect = (VMMDevHGCMDisconnect *)requestHeader;

                                        Log(("VMMDevReq_VMMDevHGCMDisconnect\n"));
                                        requestHeader->header.rc = vmmdevHGCMDisconnect (pVMMDevState, pHGCMDisconnect, pIter->GCPhys);
                                    }
                                    break;
                                }

#ifdef VBOX_WITH_64_BITS_GUESTS
                                case VMMDevReq_HGCMCall64:
                                case VMMDevReq_HGCMCall32:
#else
                                case VMMDevReq_HGCMCall:
#endif /* VBOX_WITH_64_BITS_GUESTS */
                                {
                                    if (requestHeader->header.size < sizeof(VMMDevHGCMCall))
                                    {
                                        AssertMsgFailed(("VMMDevReq_HGCMCall structure has invalid size!\n"));
                                        requestHeader->header.rc = VERR_INVALID_PARAMETER;
                                    }
                                    else if (!pVMMDevState->pHGCMDrv)
                                    {
                                        Log(("VMMDevReq_HGCMCall HGCM Connector is NULL!\n"));
                                        requestHeader->header.rc = VERR_NOT_SUPPORTED;
                                    }
                                    else
                                    {
                                        VMMDevHGCMCall *pHGCMCall = (VMMDevHGCMCall *)requestHeader;

                                        Log(("VMMDevReq_HGCMCall: sizeof (VMMDevHGCMRequest) = %04X\n", sizeof (VMMDevHGCMCall)));

                                        Log(("%.*Rhxd\n", requestHeader->header.size, requestHeader));

#ifdef VBOX_WITH_64_BITS_GUESTS
                                        bool f64Bits = (requestHeader->header.requestType == VMMDevReq_HGCMCall64);
#else
                                        bool f64Bits = false;
#endif /* VBOX_WITH_64_BITS_GUESTS */
                                        requestHeader->header.rc = vmmdevHGCMCall (pVMMDevState, pHGCMCall, requestHeader->header.size, pIter->GCPhys, f64Bits);
                                    }
                                    break;
                                }
                                default:
                                    AssertMsgFailed(("Unknown request type %x during LoadState\n", requestHeader->header.requestType));
                                    LogRel(("VMMDEV: Ignoring unknown request type %x during LoadState\n", requestHeader->header.requestType));
                            }
                        } break;

                        default:
                            AssertMsgFailed(("Unknown request type %x during LoadState\n", requestHeader->header.requestType));
                            LogRel(("VMMDEV: Ignoring unknown request type %x during LoadState\n", requestHeader->header.requestType));
                    }
                }
            }

            if (pIter->enmCmdType == VBOXHGCMCMDTYPE_LOADSTATE)
            {
                /* Old saved state. Remove the LOADSTATE command. */

                /* Write back the request */
                PDMDevHlpPhysWrite(pDevIns, pIter->GCPhys, requestHeader, pIter->cbSize);
                RTMemFree(requestHeader);
                requestHeader = NULL;

                vmmdevHGCMRemoveCommand (pVMMDevState, pIter);

                if (pIter->paLinPtrs != NULL)
                {
                     RTMemFree(pIter->paLinPtrs);
                }

                RTMemFree(pIter);
            }
            else
            {
                if (!fHGCMCalled)
                {
                   /* HGCM was not called. Return the error to the guest. Guest may try to repeat the call. */
                   requestHeader->header.rc = VERR_TRY_AGAIN;
                   requestHeader->fu32Flags |= VBOX_HGCM_REQ_DONE;
                }

                /* Write back the request */
                PDMDevHlpPhysWrite(pDevIns, pIter->GCPhys, requestHeader, pIter->cbSize);
                RTMemFree(requestHeader);
                requestHeader = NULL;

                if (!fHGCMCalled)
                {
                   /* HGCM was not called. Deallocate the current command and then notify guest. */
                   vmmdevHGCMRemoveCommand (pVMMDevState, pIter);

                   if (pIter->paLinPtrs != NULL)
                   {
                        RTMemFree(pIter->paLinPtrs);
                   }

                   RTMemFree(pIter);

                   VMMDevNotifyGuest (pVMMDevState, VMMDEV_EVENT_HGCM);
                }
            }

            pIter = pNext;
        }

        vmmdevHGCMCmdListUnlock (pVMMDevState);
    }

    return rc;
}
