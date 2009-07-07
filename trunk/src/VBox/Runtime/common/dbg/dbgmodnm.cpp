/* $Id$ */
/** @file
 * IPRT - Debug Map Reader For NM Like Mapfiles.
 */

/*
 * Copyright (C) 2009 Sun Microsystems, Inc.
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


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <iprt/dbg.h>
#include "internal/iprt.h"

#include <iprt/err.h>
#include <iprt/ctype.h>
#include <iprt/mem.h>
#include <iprt/stream.h>
#include <iprt/string.h>
#include "internal/dbgmod.h"


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/**
 * Instance data.
 */
typedef struct RTDBGMODNM
{
    /** The debug container containing doing the real work. */
    RTDBGMOD                    hCnt;
} RTDBGMODNM;
/** Pointer to instance data NM map reader. */
typedef RTDBGMODNM *PRTDBGMODNM;



/** @copydoc RTDBGMODVTDBG::pfnLineByAddr */
static DECLCALLBACK(int) rtDbgModNm_LineByAddr(PRTDBGMODINT pMod, RTDBGSEGIDX iSeg, RTUINTPTR off,
                                               PRTINTPTR poffDisp, PRTDBGLINE pLineInfo)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    return RTDbgModLineByAddr(pThis->hCnt, iSeg, off, poffDisp, pLineInfo);
}


/** @copydoc RTDBGMODVTDBG::pfnLineByOrdinal */
static DECLCALLBACK(int) rtDbgModNm_LineByOrdinal(PRTDBGMODINT pMod, uint32_t iOrdinal, PRTDBGLINE pLineInfo)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    return RTDbgModLineByOrdinal(pThis->hCnt, iOrdinal, pLineInfo);
}


/** @copydoc RTDBGMODVTDBG::pfnLineCount */
static DECLCALLBACK(uint32_t) rtDbgModNm_LineCount(PRTDBGMODINT pMod)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    return RTDbgModLineCount(pThis->hCnt);
}


/** @copydoc RTDBGMODVTDBG::pfnLineAdd */
static DECLCALLBACK(int) rtDbgModNm_LineAdd(PRTDBGMODINT pMod, const char *pszFile, size_t cchFile, uint32_t uLineNo,
                                            uint32_t iSeg, RTUINTPTR off, uint32_t *piOrdinal)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    return RTDbgModLineAdd(pThis->hCnt, pszFile, uLineNo, iSeg, off, piOrdinal);
}


/** @copydoc RTDBGMODVTDBG::pfnSymbolByAddr */
static DECLCALLBACK(int) rtDbgModNm_SymbolByAddr(PRTDBGMODINT pMod, RTDBGSEGIDX iSeg, RTUINTPTR off,
                                                 PRTINTPTR poffDisp, PRTDBGSYMBOL pSymInfo)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    return RTDbgModSymbolByAddr(pThis->hCnt, iSeg, off, poffDisp, pSymInfo);
}


/** @copydoc RTDBGMODVTDBG::pfnSymbolByName */
static DECLCALLBACK(int) rtDbgModNm_SymbolByName(PRTDBGMODINT pMod, const char *pszSymbol, size_t cchSymbol, PRTDBGSYMBOL pSymInfo)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    return RTDbgModSymbolByName(pThis->hCnt, pszSymbol, pSymInfo);
}


/** @copydoc RTDBGMODVTDBG::pfnSymbolByOrdinal */
static DECLCALLBACK(int) rtDbgModNm_SymbolByOrdinal(PRTDBGMODINT pMod, uint32_t iOrdinal, PRTDBGSYMBOL pSymInfo)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    return RTDbgModSymbolByOrdinal(pThis->hCnt, iOrdinal, pSymInfo);
}


/** @copydoc RTDBGMODVTDBG::pfnSymbolCount */
static DECLCALLBACK(uint32_t) rtDbgModNm_SymbolCount(PRTDBGMODINT pMod)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    return RTDbgModSymbolCount(pThis->hCnt);
}


/** @copydoc RTDBGMODVTDBG::pfnSymbolAdd */
static DECLCALLBACK(int) rtDbgModNm_SymbolAdd(PRTDBGMODINT pMod, const char *pszSymbol, size_t cchSymbol,
                                              RTDBGSEGIDX iSeg, RTUINTPTR off, RTUINTPTR cb, uint32_t fFlags,
                                              uint32_t *piOrdinal)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    return RTDbgModSymbolAdd(pThis->hCnt, pszSymbol, iSeg, off, cb, fFlags, piOrdinal);
}


/** @copydoc RTDBGMODVTDBG::pfnSegmentByIndex */
static DECLCALLBACK(int) rtDbgModNm_SegmentByIndex(PRTDBGMODINT pMod, RTDBGSEGIDX iSeg, PRTDBGSEGMENT pSegInfo)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    return RTDbgModSegmentByIndex(pThis->hCnt, iSeg, pSegInfo);
}


/** @copydoc RTDBGMODVTDBG::pfnSegmentCount */
static DECLCALLBACK(RTDBGSEGIDX) rtDbgModNm_SegmentCount(PRTDBGMODINT pMod)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    return RTDbgModSegmentCount(pThis->hCnt);
}


/** @copydoc RTDBGMODVTDBG::pfnSegmentAdd */
static DECLCALLBACK(int) rtDbgModNm_SegmentAdd(PRTDBGMODINT pMod, RTUINTPTR uRva, RTUINTPTR cb, const char *pszName, size_t cchName,
                                               uint32_t fFlags, PRTDBGSEGIDX piSeg)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    return RTDbgModSegmentAdd(pThis->hCnt, uRva, cb, pszName, fFlags, piSeg);
}


/** @copydoc RTDBGMODVTDBG::pfnRvaToSegOff */
static DECLCALLBACK(RTUINTPTR) rtDbgModNm_ImageSize(PRTDBGMODINT pMod)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    return RTDbgModImageSize(pThis->hCnt);
}


/** @copydoc RTDBGMODVTDBG::pfnRvaToSegOff */
static DECLCALLBACK(RTDBGSEGIDX) rtDbgModNm_RvaToSegOff(PRTDBGMODINT pMod, RTUINTPTR uRva, PRTUINTPTR poffSeg)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    return RTDbgModRvaToSegOff(pThis->hCnt, uRva, poffSeg);
}


/** @copydoc RTDBGMODVTDBG::pfnClose */
static DECLCALLBACK(int) rtDbgModNm_Close(PRTDBGMODINT pMod)
{
    PRTDBGMODNM pThis = (PRTDBGMODNM)pMod->pvDbgPriv;
    RTDbgModRelease(pThis->hCnt);
    pThis->hCnt = NIL_RTDBGMOD;
    RTMemFree(pThis);
    return VINF_SUCCESS;
}


/**
 * Scans a NM-like map file.
 *
 * This implements both passes to avoid code duplication.
 *
 * @returns IPRT status code.
 * @param   pThis               The instance data.
 * @param   pStrm               The stream.
 * @param   fAddSymbols         false in the first pass, true in the second.
 */
static int rtDbgModNmScanFile(PRTDBGMODNM pThis, PRTSTREAM pStrm, bool fAddSymbols)
{
    /*
     * Try parse the stream.
     */
    RTUINTPTR   SegZeroRva = fAddSymbols ? RTDbgModSegmentRva(pThis->hCnt, 0/*iSeg*/) : 0;
    char        szSym[RTDBG_SYMBOL_NAME_LENGTH] = "";
    size_t      cchMod  = 0;
    size_t      offSym  = 0;
    unsigned    cchAddr = 0;
    uint64_t    u64Low  = UINT64_MAX;
    uint64_t    u64High = 0;
    char        szLine[512];
    int         rc;
    while (RT_SUCCESS(rc = RTStrmGetLine(pStrm, szLine, sizeof(szLine))))
    {
        char    chType;
        if (RT_C_IS_XDIGIT(szLine[0]))
        {
            /*
             * This is really what C was made for, string parsing.
             */
            /* The the symbol value (address). */
            uint64_t u64Addr;
            char    *psz;
            rc = RTStrToUInt64Ex(szLine, &psz, 16, &u64Addr);
            if (rc != VWRN_TRAILING_CHARS)
                return VERR_DBG_NOT_NM_MAP_FILE;

            /* Check the address width. */
            if (cchAddr == 0)
                cchAddr = psz == &szLine[8] ? 8 : 16;
            if (psz != &szLine[cchAddr])
                return VERR_DBG_NOT_NM_MAP_FILE;

            /* Get the type and check for single space before symbol. */
            chType = szLine[cchAddr + 1];
            if (    RT_C_IS_BLANK(chType)
                ||  !RT_C_IS_BLANK(szLine[cchAddr + 2])
                ||  RT_C_IS_BLANK(szLine[cchAddr + 3]))
                return VERR_DBG_NOT_NM_MAP_FILE;

            /* Find the end of the symbol name. */
            char *pszName    = &szLine[cchAddr + 3];
            char *pszNameEnd = pszName;
            char ch;
            while ((ch = *pszNameEnd) != '\0' && !RT_C_IS_SPACE(ch))
                pszNameEnd++;

            /* Any module name (linux /proc/kallsyms) following in brackets? */
            char *pszModName    = pszNameEnd;
            char *pszModNameEnd = pszModName;
            if (*pszModName)
            {
                *pszModName++ = '\0';
                pszModNameEnd = pszModName = RTStrStripL(pszModName);
                if (*pszModName != '\0')
                {
                    if (*pszModName != '[')
                        return VERR_DBG_NOT_LINUX_KALLSYMS;
                    pszModNameEnd = ++pszModName;
                    while ((ch = *pszModNameEnd) != '\0' && ch != ']')
                        pszModNameEnd++;
                    if (ch != ']')
                        return VERR_DBG_NOT_LINUX_KALLSYMS;
                    char *pszEnd = pszModNameEnd + 1;
                    if ((size_t)(pszModNameEnd - pszModName) >= 128) /* lazy bird */
                        return VERR_DBG_NOT_LINUX_KALLSYMS;
                    *pszModNameEnd = '\0';
                    if (*pszEnd)
                        pszEnd = RTStrStripL(pszEnd);
                    if (*pszEnd)
                        return VERR_DBG_NOT_LINUX_KALLSYMS;
                }
            }

            /*
             * Did the module change? Then update the symbol prefix.
             */
            if (    cchMod != (size_t)(pszModNameEnd - pszModName)
                ||  memcmp(pszModName, szSym, cchMod))
            {
                cchMod = pszModNameEnd - pszModName;
                if (cchMod == 0)
                    offSym = 0;
                else
                {
                    memcpy(szSym, pszModName, cchMod);
                    szSym[cchMod] = '.';
                    offSym = cchMod + 1;
                }
                szSym[offSym] = '\0';
            }

            /*
             * Validate the type and add the symbol if it's a type we care for.
             */
            uint32_t    fFlags  = 0;
            RTDBGSEGIDX iSegSym = 0;
            switch (chType)
            {
                /* absolute */
                case 'a':
                case '?': /* /proc/kallsyms */
                    iSegSym = RTDBGSEGIDX_ABS;
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_LOCAL;
                    break;
                case 'A':
                    iSegSym = RTDBGSEGIDX_ABS;
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_PUBLIC;
                    break;

                case 'b':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_LOCAL;
                    break;
                case 'B':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_PUBLIC;
                    break;

                case 'c':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_LOCAL | RTDBG_SYM_FLAGS_COMMON;
                    break;
                case 'C':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_PUBLIC | RTDBG_SYM_FLAGS_COMMON;
                    break;

                case 'd':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_LOCAL;
                    break;
                case 'D':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_PUBLIC;
                    break;

                case 'g':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_LOCAL;
                    break;
                case 'G':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_PUBLIC;
                    break;

                case 'i':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_LOCAL;
                    break;
                case 'I':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_PUBLIC;
                    break;

                case 'r':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_LOCAL | RTDBG_SYM_FLAGS_CONST;
                    break;
                case 'R':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_PUBLIC | RTDBG_SYM_FLAGS_CONST;
                    break;

                case 's':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_LOCAL;
                    break;
                case 'S':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_PUBLIC;
                    break;

                case 't':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_LOCAL | RTDBG_SYM_FLAGS_TEXT;
                    break;
                case 'T':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_PUBLIC | RTDBG_SYM_FLAGS_TEXT;
                    break;

                case 'w':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_WEAK | RTDBG_SYM_FLAGS_LOCAL; //???
                    break;
                case 'W':
                    /// @todo fFlags |= RTDBG_SYM_FLAGS_WEAK | RTDBG_SYM_FLAGS_PUBLIC;
                    break;

                case 'N': /* debug */
                case 'n':
                case '-': /* stabs */
                case 'u': /* undefined (/proc/kallsyms) */
                case 'U':
                case 'v': /* weakext */
                case 'V':
                    iSegSym = NIL_RTDBGSEGIDX;
                    break;

                default:
                    return VERR_DBG_NOT_NM_MAP_FILE;
            }

            if (iSegSym != NIL_RTDBGSEGIDX)
            {
                if (fAddSymbols)
                {
                    size_t cchName = pszNameEnd - pszName;
                    if (cchName >= sizeof(szSym) - offSym)
                        cchName = sizeof(szSym) - offSym - 1;
                    memcpy(&szSym[offSym], pszName, cchName + 1);
                    if (iSegSym == 0)
                        rc = RTDbgModSymbolAdd(pThis->hCnt, szSym, iSegSym, u64Addr - SegZeroRva, 0/*cb*/, fFlags, NULL);
                    else
                        rc = RTDbgModSymbolAdd(pThis->hCnt, szSym, iSegSym, u64Addr, 0/*cb*/, fFlags, NULL);
                    if (    RT_FAILURE(rc)
                        &&  rc != VERR_DBG_DUPLICATE_SYMBOL
                        &&  rc != VERR_DBG_ADDRESS_CONFLICT) /* (don't be too strict) */
                        return rc;
                }

                /* Track segment span. */
                if (iSegSym == 0)
                {
                    if (u64Low > u64Addr)
                        u64Low = u64Addr;
                    if (u64High < u64Addr)
                        u64High = u64Addr;
                }
            }
        }
        else
        {
            /*
             * This is either a blank line or a symbol without an address.
             */
            RTStrStripR(szLine);
            if (szLine[0])
            {
                size_t cch = strlen(szLine);
                if (cchAddr == 0)
                    cchAddr = cch < 16+3 || szLine[8+1] != ' ' ? 8 : 16;
                if (cch < cchAddr+3+1)
                    return VERR_DBG_NOT_NM_MAP_FILE;
                chType = szLine[cchAddr + 1];
                if (    chType != 'U'
                    &&  chType != 'w')
                    return VERR_DBG_NOT_NM_MAP_FILE;
                char *pszType = RTStrStripL(szLine);
                if (pszType != &szLine[cchAddr + 1])
                    return VERR_DBG_NOT_NM_MAP_FILE;
                if (!RT_C_IS_BLANK(szLine[cchAddr + 2]))
                    return VERR_DBG_NOT_NM_MAP_FILE;
            }
            /* else: blank - ignored */
        }
    }

    /*
     * The final segment.
     */
    if (rc == VERR_EOF)
    {
        if (fAddSymbols)
            rc = VINF_SUCCESS;
        else
        {
            if (    u64Low  != UINT64_MAX
                 || u64High != 0)
                rc = RTDbgModSegmentAdd(pThis->hCnt, u64Low, u64High - u64Low + 1, "main", 0, NULL);
            else /* No sensible symbols... throw an error instead? */
                rc = RTDbgModSegmentAdd(pThis->hCnt, 0, 0, "main", 0, NULL);
        }
    }

    return rc;
}


/** @copydoc RTDBGMODVTDBG::pfnTryOpen */
static DECLCALLBACK(int) rtDbgModNm_TryOpen(PRTDBGMODINT pMod)
{
    /*
     * Try open the file and create an instance.
     */
    PRTSTREAM pStrm;
    int rc = RTStrmOpen(pMod->pszDbgFile, "r", &pStrm);
    if (RT_SUCCESS(rc))
    {
        PRTDBGMODNM pThis = (PRTDBGMODNM)RTMemAlloc(sizeof(*pThis));
        if (pThis)
        {
            rc = RTDbgModCreate(&pThis->hCnt, pMod->pszName, 0 /*cbSeg*/, 0 /*fFlags*/);
            if (RT_SUCCESS(rc))
            {
                /*
                 * Scan the file twice, first to figure the segment
                 * sizes, then to add the symbol.
                 */
                rc = rtDbgModNmScanFile(pThis, pStrm, false /*fAddSymbols*/);
                if (RT_SUCCESS(rc))
                    rc = RTStrmRewind(pStrm);
                if (RT_SUCCESS(rc))
                    rc = rtDbgModNmScanFile(pThis, pStrm, true /*fAddSymbols*/);
                if (RT_SUCCESS(rc))
                {
                    RTStrmClose(pStrm);
                    pMod->pvDbgPriv = pThis;
                    return rc;
                }
            }
            RTDbgModRelease(pThis->hCnt);
        }
        else
            rc = VERR_NO_MEMORY;
        RTStrmClose(pStrm);
    }
    return rc;
}



/** Virtual function table for the NM-like map file reader. */
DECLHIDDEN(RTDBGMODVTDBG const) g_rtDbgModVtDbgNm =
{
    /*.u32Magic = */            RTDBGMODVTDBG_MAGIC,
    /*.fSupports = */           RT_DBGTYPE_MAP,
    /*.pszName = */             "nm",
    /*.pfnTryOpen = */          rtDbgModNm_TryOpen,
    /*.pfnClose = */            rtDbgModNm_Close,

    /*.pfnRvaToSegOff = */      rtDbgModNm_RvaToSegOff,
    /*.pfnImageSize = */        rtDbgModNm_ImageSize,

    /*.pfnSegmentAdd = */       rtDbgModNm_SegmentAdd,
    /*.pfnSegmentCount = */     rtDbgModNm_SegmentCount,
    /*.pfnSegmentByIndex = */   rtDbgModNm_SegmentByIndex,

    /*.pfnSymbolAdd = */        rtDbgModNm_SymbolAdd,
    /*.pfnSymbolCount = */      rtDbgModNm_SymbolCount,
    /*.pfnSymbolByOrdinal = */  rtDbgModNm_SymbolByOrdinal,
    /*.pfnSymbolByName = */     rtDbgModNm_SymbolByName,
    /*.pfnSymbolByAddr = */     rtDbgModNm_SymbolByAddr,

    /*.pfnLineAdd = */          rtDbgModNm_LineAdd,
    /*.pfnLineCount = */        rtDbgModNm_LineCount,
    /*.pfnLineByOrdinal = */    rtDbgModNm_LineByOrdinal,
    /*.pfnLineByAddr = */       rtDbgModNm_LineByAddr,

    /*.u32EndMagic = */         RTDBGMODVTDBG_MAGIC
};

