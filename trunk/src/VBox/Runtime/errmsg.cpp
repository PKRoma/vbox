/* $Id$ */
/** @file
 * InnoTek Portable Runtime - Status code messages.
 */

/*
 * Copyright (C) 2006 InnoTek Systemberatung GmbH
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation,
 * in version 2 as it comes in the "COPYING" file of the VirtualBox OSE
 * distribution. VirtualBox OSE is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * If you received this file as part of a commercial VirtualBox
 * distribution, then only the terms of your commercial VirtualBox
 * license agreement apply instead of the previous paragraph.
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <iprt/err.h>
#include <iprt/asm.h>
#include <iprt/string.h>
#include <iprt/err.h>
#include <VBox/err.h>


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/** Array of messages.
 * The data is generated by a sed script.
 */
static const RTSTATUSMSG  g_aStatusMsgs[] =
{
#include "errmsgdata.h"
    { NULL, NULL, NULL, 0 }
};


/** Temporary buffers to format unknown messages in.
 * @{
 */
static char                 g_aszUnknownStr[4][64];
static RTSTATUSMSG          g_aUnknownMsgs[4] =
{
    { &g_aszUnknownStr[0][0], &g_aszUnknownStr[0][0], &g_aszUnknownStr[0][0], 0 },
    { &g_aszUnknownStr[1][0], &g_aszUnknownStr[1][0], &g_aszUnknownStr[1][0], 0 },
    { &g_aszUnknownStr[2][0], &g_aszUnknownStr[2][0], &g_aszUnknownStr[2][0], 0 },
    { &g_aszUnknownStr[3][0], &g_aszUnknownStr[3][0], &g_aszUnknownStr[3][0], 0 }
};
/** Last used index in g_aUnknownMsgs. */
static volatile uint32_t    g_iUnknownMsgs;
/** @} */


/**
 * Get the message corresponding to a given status code.
 *
 * @returns Pointer to read-only message description.
 * @param   rc      The status code.
 */
RTDECL(PCRTSTATUSMSG) RTErrGet(int rc)
{
    unsigned iFound = ~0;
    unsigned i;
    for (i = 0; i < ELEMENTS(g_aStatusMsgs); i++)
    {
        if (g_aStatusMsgs[i].iCode == rc)
        {
            /*
             * Found a match.
             * Since this isn't a unique key, we must check that it's not
             * one of those start/end #defines before we return.
             */
            if (    !strstr(g_aStatusMsgs[i].pszDefine, "FIRST")
                &&  !strstr(g_aStatusMsgs[i].pszDefine, "LAST"))
                return &g_aStatusMsgs[i];
            iFound = i;
        }
    }
    if (iFound != ~0U)
        return &g_aStatusMsgs[iFound];

    /*
     * Need to use the temporary stuff.
     */
    int iMsg = ASMAtomicXchgU32(&g_iUnknownMsgs, (g_iUnknownMsgs + 1) % ELEMENTS(g_aUnknownMsgs));
    RTStrPrintf(&g_aszUnknownStr[iMsg][0], sizeof(g_aszUnknownStr[iMsg]), "Unknown Status %d\n", rc);
    return &g_aUnknownMsgs[iMsg];
}

