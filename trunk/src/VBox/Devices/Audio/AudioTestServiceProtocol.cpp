/* $Id$ */
/** @file
 * AudioTestService - Audio test execution server, Protocol helpers.
 */

/*
 * Copyright (C) 2021 Oracle Corporation
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
 */


/*********************************************************************************************************************************
*   Header Files                                                                                                                 *
*********************************************************************************************************************************/
#define LOG_GROUP RTLOGGROUP_DEFAULT
#include <iprt/asm.h>
#include <iprt/cdefs.h>

#include "AudioTestServiceProtocol.h"



/**
 * Converts a ATS packet header from host to network byte order.
 *
 * @returns nothing.
 * @param   pPktHdr          The packet header to convert.
 */
DECLINLINE(void) atsProtocolPktHdrH2N(PATSPKTHDR pPktHdr)
{
    pPktHdr->cb     = RT_H2N_U32(pPktHdr->cb);
    pPktHdr->uCrc32 = RT_H2N_U32(pPktHdr->uCrc32);
}


#if 0 /* Unused */
/**
 * Converts a ATS packet header from network to host byte order.
 *
 * @returns nothing.
 * @param   pPktHdr          The packet header to convert.
 */
DECLINLINE(void) atsProtocolPktHdrN2H(PATSPKTHDR pPktHdr)
{
    pPktHdr->cb     = RT_N2H_U32(pPktHdr->cb);
    pPktHdr->uCrc32 = RT_N2H_U32(pPktHdr->uCrc32);
}


/**
 * Converts a ATS status header from host to network byte order.
 *
 * @returns nothing.
 * @param   pPktHdr          The packet header to convert.
 */
DECLINLINE(void) atsProtocolStsHdrH2N(PATSPKTSTS pPktHdr)
{
    atsProtocolPktHdrH2N(&pPktHdr->Hdr);
    pPktHdr->rcReq     = RT_H2N_U32(pPktHdr->rcReq);
    pPktHdr->cchStsMsg = RT_H2N_U32(pPktHdr->cchStsMsg);
}


/**
 * Converts a ATS status header from network to host byte order.
 *
 * @returns nothing.
 * @param   pPktHdr          The packet header to convert.
 */
DECLINLINE(void) atsProtocolStsHdrN2H(PATSPKTSTS pPktHdr)
{
    atsProtocolPktHdrN2H(&pPktHdr->Hdr);
    pPktHdr->rcReq     = RT_N2H_U32(pPktHdr->rcReq);
    pPktHdr->cchStsMsg = RT_N2H_U32(pPktHdr->cchStsMsg);
}
#endif


DECLHIDDEN(void) atsProtocolReqH2N(PATSPKTHDR pPktHdr)
{
    atsProtocolPktHdrH2N(pPktHdr);
}


DECLHIDDEN(void) atsProtocolReqN2H(PATSPKTHDR pPktHdr)
{
    RT_NOREF1(pPktHdr);
}


DECLHIDDEN(void) atsProtocolRepH2N(PATSPKTSTS pPktHdr)
{
    RT_NOREF1(pPktHdr);
}


DECLHIDDEN(void) atsProtocolRepN2H(PATSPKTSTS pPktHdr)
{
    RT_NOREF1(pPktHdr);
}
