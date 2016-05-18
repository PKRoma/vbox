/* $Id$ */
/** @file
 * UsbTestServ - Remote USB test configuration and execution server, Config file API.
 */

/*
 * Copyright (C) 2016 Oracle Corporation
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

#include <iprt/stream.h>
#include <iprt/process.h>
#include <iprt/string.h>
#include <iprt/mem.h>
#include <iprt/ctype.h>
#include <iprt/message.h>

#include "UsbTestServiceCfg.h"


/*********************************************************************************************************************************
*   Constants And Macros, Structures and Typedefs                                                                                *
*********************************************************************************************************************************/

/**
 * Token type.
 */
typedef enum CFGTOKENTYPE
{
    /** Invalid token type. */
    CFGTOKENTYPE_INVALID = 0,
    /** Identifier. */
    CFGTOKENTYPE_ID,
    /** Comma. */
    CFGTOKENTYPE_COMMA,
    /** Equal sign. */
    CFGTOKENTYPE_EQUAL,
    /** Open curly brackets. */
    CFGTOKENTYPE_CURLY_OPEN,
    /** Closing curly brackets. */
    CFGTOKENTYPE_CURLY_CLOSING,
    /** End of file. */
    CFGTOKENTYPE_EOF,
    /** 32bit hack. */
    CFGTOKENTYPE_32BIT_HACK = 0x7fffffff
} CFGTOKENTYPE;
/** Pointer to a token type. */
typedef CFGTOKENTYPE *PCFGTOKENTYPE;
/** Pointer to a const token type. */
typedef const CFGTOKENTYPE *PCCFGTOKENTYPE;

/**
 * A token.
 */
typedef struct CFGTOKEN
{
    /** Type of the token. */
    CFGTOKENTYPE    enmType;
    /** Line number of the token. */
    unsigned        iLine;
    /** Starting character of the token in the stream. */
    unsigned        cchStart;
    /** Type dependen token data. */
    union
    {
        /** Data for the ID type. */
        struct
        {
            /** Size of the id in characters, excluding the \0 terminator. */
            size_t  cchToken;
            /** Token data, variable size (given by cchToken member). */
            char    achToken[1];
        } Id;
    } u;
} CFGTOKEN;
/** Pointer to a token. */
typedef CFGTOKEN *PCFGTOKEN;
/** Pointer to a const token. */
typedef const CFGTOKEN *PCCFGTOKEN;

/**
 * Tokenizer instance data for the config data.
 */
typedef struct CFGTOKENIZER
{
    /** Config file handle. */
    PRTSTREAM hStrmConfig;
    /** String buffer for the current line we are operating in. */
    char      *pszLine;
    /** Size of the string buffer. */
    size_t     cbLine;
    /** Current position in the line. */
    char      *pszLineCurr;
    /** Current line in the config file. */
    unsigned   iLine;
    /** Current character of the line. */
    unsigned   cchCurr;
    /** Flag whether the end of the config stream is reached. */
    bool       fEof;
    /** Pointer to the next token in the stream (used to peek). */
    PCFGTOKEN  pTokenNext;
} CFGTOKENIZER, *PCFGTOKENIZER;


/*********************************************************************************************************************************
*   Internal Functions                                                                                                           *
*********************************************************************************************************************************/

/**
 * Free a config token.
 *
 * @returns nothing.
 * @param   pCfgTokenizer    The config tokenizer.
 * @param   pToken           The token to free.
 */
static void utsConfigTokenFree(PCFGTOKENIZER pCfgTokenizer, PCFGTOKEN pToken)
{
    NOREF(pCfgTokenizer);
    RTMemFree(pToken);
}

/**
 * Reads the next line from the config stream.
 *
 * @returns VBox status code.
 * @param   pCfgTokenizer    The config tokenizer.
 */
static int utsConfigTokenizerReadNextLine(PCFGTOKENIZER pCfgTokenizer)
{
    int rc = VINF_SUCCESS;

    if (pCfgTokenizer->fEof)
        return VERR_EOF;

    do
    {
        rc = RTStrmGetLine(pCfgTokenizer->hStrmConfig, pCfgTokenizer->pszLine,
                           pCfgTokenizer->cbLine);
        if (rc == VERR_BUFFER_OVERFLOW)
        {
            char *pszTmp;

            pCfgTokenizer->cbLine += 128;
            pszTmp = (char *)RTMemRealloc(pCfgTokenizer->pszLine, pCfgTokenizer->cbLine);
            if (pszTmp)
                pCfgTokenizer->pszLine = pszTmp;
            else
                rc = VERR_NO_MEMORY;
        }
    } while (rc == VERR_BUFFER_OVERFLOW);

    if (   RT_SUCCESS(rc)
        || rc == VERR_EOF)
    {
        pCfgTokenizer->iLine++;
        pCfgTokenizer->cchCurr = 1;
        pCfgTokenizer->pszLineCurr = pCfgTokenizer->pszLine;
        if (rc == VERR_EOF)
            pCfgTokenizer->fEof = true;
    }

    return rc;
}

/**
 * Get the next token from the config stream and create a token structure.
 *
 * @returns VBox status code.
 * @param   pCfgTokenizer    The config tokenizer data.
 * @param   pCfgTokenUse     Allocated token structure to use or NULL to allocate
 *                           a new one. It will bee freed if an error is encountered.
 * @param   ppCfgToken       Where to store the pointer to the next token on success.
 */
static int utsConfigTokenizerCreateToken(PCFGTOKENIZER pCfgTokenizer,
                                               PCFGTOKEN pCfgTokenUse, PCFGTOKEN *ppCfgToken)
{
    const char *pszToken = NULL;
    size_t cchToken = 1;
    size_t cchAdvance = 0;
    CFGTOKENTYPE enmType = CFGTOKENTYPE_INVALID;
    int rc = VINF_SUCCESS;

    for (;;)
    {
        pszToken = pCfgTokenizer->pszLineCurr;

        /* Skip all spaces. */
        while (RT_C_IS_BLANK(*pszToken))
        {
            pszToken++;
            cchAdvance++;
        }

        /* Check if we have to read a new line. */
        if (   *pszToken == '\0'
            || *pszToken == '#')
        {
            rc = utsConfigTokenizerReadNextLine(pCfgTokenizer);
            if (rc == VERR_EOF)
            {
                enmType = CFGTOKENTYPE_EOF;
                rc = VINF_SUCCESS;
                break;
            }
            else if (RT_FAILURE(rc))
                break;
            /* start from the beginning. */
            cchAdvance = 0;
        }
        else if (*pszToken == '=')
        {
            enmType = CFGTOKENTYPE_EQUAL;
            break;
        }
        else if (*pszToken == ',')
        {
            enmType = CFGTOKENTYPE_COMMA;
            break;
        }
        else if (*pszToken == '{')
        {
            enmType = CFGTOKENTYPE_CURLY_OPEN;
            break;
        }
        else if (*pszToken == '}')
        {
            enmType = CFGTOKENTYPE_CURLY_CLOSING;
            break;
        }
        else
        {
            const char *pszTmp = pszToken;
            cchToken = 0;
            enmType = CFGTOKENTYPE_ID;

            /* Get the complete token. */
            while (   RT_C_IS_ALNUM(*pszTmp)
                   || *pszTmp == '_'
                   || *pszTmp == '.')
            {
                pszTmp++;
                cchToken++;
            }
            break;
        }
    }

    Assert(RT_FAILURE(rc) || enmType != CFGTOKENTYPE_INVALID);

    if (RT_SUCCESS(rc))
    {
        /* Free the given token if it is an ID or the current one is an ID token. */
        if (   pCfgTokenUse
            && (   pCfgTokenUse->enmType == CFGTOKENTYPE_ID
                || enmType == CFGTOKENTYPE_ID))
        {
            utsConfigTokenFree(pCfgTokenizer, pCfgTokenUse);
            pCfgTokenUse = NULL;
        }

        if (!pCfgTokenUse)
        {
            size_t cbToken = sizeof(CFGTOKEN);
            if (enmType == CFGTOKENTYPE_ID)
                cbToken += (cchToken + 1) * sizeof(char);

            pCfgTokenUse = (PCFGTOKEN)RTMemAllocZ(cbToken);
            if (!pCfgTokenUse)
                rc = VERR_NO_MEMORY;
        }

        if (RT_SUCCESS(rc))
        {
            /* Copy token data. */
            pCfgTokenUse->enmType  = enmType;
            pCfgTokenUse->cchStart = pCfgTokenizer->cchCurr;
            pCfgTokenUse->iLine    = pCfgTokenizer->iLine;
            if (enmType == CFGTOKENTYPE_ID)
            {
                pCfgTokenUse->u.Id.cchToken = cchToken;
                memcpy(pCfgTokenUse->u.Id.achToken, pszToken, cchToken);
            }
        }
        else if (pCfgTokenUse)
            utsConfigTokenFree(pCfgTokenizer, pCfgTokenUse);

        if (RT_SUCCESS(rc))
        {
            /* Set new position in config stream. */
            pCfgTokenizer->pszLineCurr += cchToken + cchAdvance;
            pCfgTokenizer->cchCurr     += cchToken + cchAdvance;
            *ppCfgToken                 = pCfgTokenUse;
        }
    }

    return rc;
}

/**
 * Destroys the given config tokenizer.
 *
 * @returns nothing.
 * @param   pCfgTokenizer    The config tokenizer to destroy.
 */
static void utsConfigTokenizerDestroy(PCFGTOKENIZER pCfgTokenizer)
{
    if (pCfgTokenizer->pszLine)
        RTMemFree(pCfgTokenizer->pszLine);
    if (pCfgTokenizer->hStrmConfig)
        RTStrmClose(pCfgTokenizer->hStrmConfig);
    if (pCfgTokenizer->pTokenNext)
        RTMemFree(pCfgTokenizer->pTokenNext);
    RTMemFree(pCfgTokenizer);
}

/**
 * Creates the config tokenizer from the given filename.
 *
 * @returns VBox status code.
 * @param   pszFilename    Config filename.
 * @param   ppCfgTokenizer Where to store the pointer to the config tokenizer on
 *                         success.
 */
static int utsConfigTokenizerCreate(const char *pszFilename, PCFGTOKENIZER *ppCfgTokenizer)
{
    int rc = VINF_SUCCESS;
    PCFGTOKENIZER pCfgTokenizer = (PCFGTOKENIZER)RTMemAllocZ(sizeof(CFGTOKENIZER));

    if (pCfgTokenizer)
    {
        pCfgTokenizer->iLine = 0;
        pCfgTokenizer->cbLine = 128;
        pCfgTokenizer->pszLine = (char *)RTMemAllocZ(pCfgTokenizer->cbLine);
        if (pCfgTokenizer->pszLine)
        {
            rc = RTStrmOpen(pszFilename, "r", &pCfgTokenizer->hStrmConfig);
            if (RT_SUCCESS(rc))
            {
                rc = utsConfigTokenizerReadNextLine(pCfgTokenizer);
                if (RT_SUCCESS(rc))
                    rc = utsConfigTokenizerCreateToken(pCfgTokenizer, NULL,
                                                             &pCfgTokenizer->pTokenNext);
            }
        }
        else
            rc = VERR_NO_MEMORY;
    }
    else
        rc = VERR_NO_MEMORY;

    if (RT_SUCCESS(rc))
        *ppCfgTokenizer = pCfgTokenizer;
    else if (   RT_FAILURE(rc)
             && pCfgTokenizer)
        utsConfigTokenizerDestroy(pCfgTokenizer);

    return rc;
}

/**
 * Return the next token from the config stream.
 *
 * @returns VBox status code.
 * @param   pCfgTokenizer   The config tokenizer.
 * @param   ppCfgToken      Where to store the next token.
 */
static int utsConfigTokenizerGetNextToken(PCFGTOKENIZER pCfgTokenizer,
                                                PCFGTOKEN *ppCfgToken)
{
    *ppCfgToken = pCfgTokenizer->pTokenNext;
    return utsConfigTokenizerCreateToken(pCfgTokenizer, NULL, &pCfgTokenizer->pTokenNext);
}

/**
 * Returns a stringified version of the token type.
 *
 * @returns Stringified version of the token type.
 * @param   enmType         Token type.
 */
static const char *utsConfigTokenTypeToStr(CFGTOKENTYPE enmType)
{
    switch (enmType)
    {
        case CFGTOKENTYPE_COMMA:
            return ",";
        case CFGTOKENTYPE_EQUAL:
            return "=";
        case CFGTOKENTYPE_CURLY_OPEN:
            return "{";
        case CFGTOKENTYPE_CURLY_CLOSING:
            return "}";
        case CFGTOKENTYPE_EOF:
            return "<EOF>";
        case CFGTOKENTYPE_ID:
            return "<Identifier>";
        default:
            AssertFailed();
            return "<Invalid>";
    }

    AssertFailed();
    return NULL;
}

/**
 * Returns a stringified version of the token.
 *
 * @returns Stringified version of the token type.
 * @param   pToken         Token.
 */
static const char *utsConfigTokenToString(PCFGTOKEN pToken)
{
    if (pToken->enmType == CFGTOKENTYPE_ID)
        return pToken->u.Id.achToken;
    else
        return utsConfigTokenTypeToStr(pToken->enmType);
}

/**
 * Returns the length of the token in characters (without zero terminator).
 *
 * @returns Token length.
 * @param   pToken          Token.
 */
static size_t utsConfigTokenGetLength(PCFGTOKEN pToken)
{
    switch (pToken->enmType)
    {
        case CFGTOKENTYPE_COMMA:
        case CFGTOKENTYPE_EQUAL:
        case CFGTOKENTYPE_CURLY_OPEN:
        case CFGTOKENTYPE_CURLY_CLOSING:
            return 1;
        case CFGTOKENTYPE_EOF:
            return 0;
        case CFGTOKENTYPE_ID:
            return strlen(pToken->u.Id.achToken);
        default:
            AssertFailed();
            return 0;
    }

    AssertFailed();
    return 0;
}

/**
 * Log unexpected token error.
 *
 * @returns nothing.
 * @param   pToken          The token which caused the error.
 * @param   pszExpected     String of the token which was expected.
 * @param   ppErrInfo       Where to store the detailed error info.
 */
static void utsConfigTokenizerMsgUnexpectedToken(PCFGTOKEN pToken, const char *pszExpected,
                                                 PRTERRINFO *ppErrInfo)
{
    if (ppErrInfo)
    {
        PRTERRINFO pErrInfo = RTErrInfoAlloc(256);
        if (RT_LIKELY(pErrInfo))
        {
            RTErrInfoSetF(pErrInfo, VERR_INVALID_STATE, "Unexpected token '%s' at %d:%d.%d, expected '%s'",
                          utsConfigTokenToString(pToken),
                          pToken->iLine, pToken->cchStart,
                          pToken->cchStart + utsConfigTokenGetLength(pToken) - 1, pszExpected);
            *ppErrInfo = pErrInfo;
        }
    }
}

/**
 * Verfies a token and consumes it.
 *
 * @returns VBox status code.
 * @param   pCfgTokenizer    The config tokenizer.
 * @param   pszTokenCheck    The token to check for.
 * @param   ppErrInfo        Where to store the detailed error info.
 */
static int utsConfigTokenizerCheckAndConsume(PCFGTOKENIZER pCfgTokenizer, CFGTOKENTYPE enmType,
                                             PRTERRINFO *ppErrInfo)
{
    int rc = VINF_SUCCESS;
    PCFGTOKEN pCfgToken = NULL;

    rc = utsConfigTokenizerGetNextToken(pCfgTokenizer, &pCfgToken);
    if (RT_SUCCESS(rc))
    {
        if (pCfgToken->enmType != enmType)
        {
            utsConfigTokenizerMsgUnexpectedToken(pCfgToken, utsConfigTokenTypeToStr(enmType), ppErrInfo);
            rc = VERR_INVALID_PARAMETER;
        }

        utsConfigTokenFree(pCfgTokenizer, pCfgToken);
    }
    return rc;
}

/**
 * Consumes the next token in the stream.
 *
 * @returns VBox status code.
 * @param   pCfgTokenizer    Tokenizer instance data.
 */
static int utsConfigTokenizerConsume(PCFGTOKENIZER pCfgTokenizer)
{
    int rc = VINF_SUCCESS;
    PCFGTOKEN pCfgToken = NULL;

    rc = utsConfigTokenizerGetNextToken(pCfgTokenizer, &pCfgToken);
    if (RT_SUCCESS(rc))
        utsConfigTokenFree(pCfgTokenizer, pCfgToken);

    return rc;
}

/**
 * Returns the start of the next token without consuming it.
 *
 * @returns The next token without consuming it.
 * @param   pCfgTokenizer    Tokenizer instance data.
 */
DECLINLINE(PCFGTOKEN) utsConfigTokenizerPeek(PCFGTOKENIZER pCfgTokenizer)
{
    return pCfgTokenizer->pTokenNext;
}

/**
 * Check whether the next token is equal to the given one.
 *
 * @returns true if the next token in the stream is equal to the given one
 *          false otherwise.
 * @param   pszToken    The token to check for.
 */
DECLINLINE(bool) utsConfigTokenizerPeekIsEqual(PCFGTOKENIZER pCfgTokenizer, CFGTOKENTYPE enmType)
{
    PCFGTOKEN pToken = utsConfigTokenizerPeek(pCfgTokenizer);
    return pToken->enmType == enmType;
}

/**
 * Parse a key value node and returns the AST.
 *
 * @returns VBox status code.
 * @param   pCfgTokenizer    The tokenizer for the config stream.
 * @param   pszKey           The key for the pair.
 * @param   ppCfgAst         Where to store the resulting AST on success.
 * @param   ppErrInfo        Where to store the detailed error info.
 */
static int utsConfigParseValue(PCFGTOKENIZER pCfgTokenizer, const char *pszKey,
                               PCFGAST *ppCfgAst, PRTERRINFO *ppErrInfo)
{
    int rc = VINF_SUCCESS;
    PCFGTOKEN pToken = NULL;

    rc = utsConfigTokenizerGetNextToken(pCfgTokenizer, &pToken);
    if (   RT_SUCCESS(rc)
        && pToken->enmType == CFGTOKENTYPE_ID)
    {
        PCFGAST pCfgAst = NULL;

        pCfgAst = (PCFGAST)RTMemAllocZ(RT_OFFSETOF(CFGAST, u.KeyValue.aszValue[pToken->u.Id.cchToken + 1]));
        if (!pCfgAst)
            return VERR_NO_MEMORY;

        pCfgAst->enmType = CFGASTNODETYPE_KEYVALUE;
        pCfgAst->pszKey  = RTStrDup(pszKey);
        if (!pCfgAst->pszKey)
        {
            RTMemFree(pCfgAst);
            return VERR_NO_MEMORY;
        }

        memcpy(pCfgAst->u.KeyValue.aszValue, pToken->u.Id.achToken, pToken->u.Id.cchToken);
        pCfgAst->u.KeyValue.cchValue = pToken->u.Id.cchToken;
        *ppCfgAst = pCfgAst;
    }
    else
    {
        utsConfigTokenizerMsgUnexpectedToken(pToken, "non reserved token", ppErrInfo);
        rc = VERR_INVALID_PARAMETER;
    }

    return rc;
}

/**
 * Parses a compound node constructing the AST and returning it on success.
 *
 * @returns VBox status code.
 * @param   pCfgTokenizer    The tokenizer for the config stream.
 * @param   pszScopeId       The scope ID of the compound node.
 * @param   ppCfgAst         Where to store the resulting AST on success.
 */
static int utsConfigParseCompoundNode(PCFGTOKENIZER pCfgTokenizer, const char *pszScopeId,
                                      PCFGAST *ppCfgAst, PRTERRINFO *ppErrInfo)
{
    int rc = VINF_SUCCESS;
    unsigned cAstNodesMax = 10;
    unsigned idxAstNodeCur = 0;
    PCFGAST pCfgAst = NULL;

    pCfgAst = (PCFGAST)RTMemAllocZ(RT_OFFSETOF(CFGAST, u.Compound.apAstNodes[cAstNodesMax]));
    if (!pCfgAst)
        return VERR_NO_MEMORY;

    pCfgAst->enmType = CFGASTNODETYPE_COMPOUND;
    pCfgAst->u.Compound.cAstNodes = 0;
    pCfgAst->pszKey  = RTStrDup(pszScopeId);
    if (!pCfgAst->pszKey)
    {
        RTMemFree(pCfgAst);
        return VERR_NO_MEMORY;
    }

    do
    {
        PCFGTOKEN pToken = NULL;
        PCFGAST pAstNode = NULL;

        if (   utsConfigTokenizerPeekIsEqual(pCfgTokenizer, CFGTOKENTYPE_CURLY_CLOSING)
            || utsConfigTokenizerPeekIsEqual(pCfgTokenizer, CFGTOKENTYPE_EOF))
            break;

        rc = utsConfigTokenizerGetNextToken(pCfgTokenizer, &pToken);
        if (   RT_SUCCESS(rc)
            && pToken->enmType == CFGTOKENTYPE_ID)
        {
            /* Next must be a = token in all cases at this place. */
            rc = utsConfigTokenizerCheckAndConsume(pCfgTokenizer, CFGTOKENTYPE_EQUAL, ppErrInfo);
            if (RT_SUCCESS(rc))
            {
                /* Check whether this is a compound node. */
                if (utsConfigTokenizerPeekIsEqual(pCfgTokenizer, CFGTOKENTYPE_CURLY_OPEN))
                {
                    rc = utsConfigTokenizerConsume(pCfgTokenizer);
                    if (RT_SUCCESS(rc))
                        rc = utsConfigParseCompoundNode(pCfgTokenizer, pToken->u.Id.achToken,
                                                        &pAstNode, ppErrInfo);

                    if (RT_SUCCESS(rc))
                        rc = utsConfigTokenizerCheckAndConsume(pCfgTokenizer, CFGTOKENTYPE_CURLY_CLOSING, ppErrInfo);
                }
                else
                    rc = utsConfigParseValue(pCfgTokenizer, pToken->u.Id.achToken,
                                             &pAstNode, ppErrInfo);
            }
        }
        else if (RT_SUCCESS(rc))
        {
            utsConfigTokenizerMsgUnexpectedToken(pToken, "non reserved token", ppErrInfo);
            rc = VERR_INVALID_PARAMETER;
        }

        /* Add to the current compound node. */
        if (RT_SUCCESS(rc))
        {
            if (pCfgAst->u.Compound.cAstNodes >= cAstNodesMax)
            {
                cAstNodesMax += 10;

                PCFGAST pCfgAstNew = (PCFGAST)RTMemRealloc(pCfgAst, RT_OFFSETOF(CFGAST, u.Compound.apAstNodes[cAstNodesMax]));
                if (!pCfgAstNew)
                    rc = VERR_NO_MEMORY;
                else
                    pCfgAst = pCfgAstNew;
            }

            if (RT_SUCCESS(rc))
            {
                pCfgAst->u.Compound.apAstNodes[pCfgAst->u.Compound.cAstNodes] = pAstNode;
                pCfgAst->u.Compound.cAstNodes++;
            }
        }

        utsConfigTokenFree(pCfgTokenizer, pToken);

    } while (RT_SUCCESS(rc));

    if (RT_SUCCESS(rc))
        *ppCfgAst = pCfgAst;
    else
        utsConfigAstDestroy(pCfgAst);

    return rc;
}

DECLHIDDEN(int) utsParseConfig(const char *pszFilename, PCFGAST *ppCfgAst, PRTERRINFO *ppErrInfo)
{
    PCFGTOKENIZER pCfgTokenizer = NULL;
    int rc = VINF_SUCCESS;
    PCFGAST pCfgAst = NULL;

    AssertPtrReturn(pszFilename, VERR_INVALID_POINTER);
    AssertPtrReturn(ppCfgAst, VERR_INVALID_POINTER);

    rc = utsConfigTokenizerCreate(pszFilename, &pCfgTokenizer);
    if (RT_SUCCESS(rc))
    {
        rc = utsConfigParseCompoundNode(pCfgTokenizer, "", &pCfgAst, ppErrInfo);
        if (RT_SUCCESS(rc))
            rc = utsConfigTokenizerCheckAndConsume(pCfgTokenizer, CFGTOKENTYPE_EOF, ppErrInfo);
    }

    if (pCfgTokenizer)
        utsConfigTokenizerDestroy(pCfgTokenizer);

    if (RT_SUCCESS(rc))
        *ppCfgAst = pCfgAst;

    return rc;
}

DECLHIDDEN(void) utsConfigAstDestroy(PCFGAST pCfgAst)
{
    AssertPtrReturnVoid(pCfgAst);

    switch (pCfgAst->enmType)
    {
        case CFGASTNODETYPE_KEYVALUE:
        {
            RTMemFree(pCfgAst);
            break;
        }
        case CFGASTNODETYPE_COMPOUND:
        {
            for (unsigned i = 0; i < pCfgAst->u.Compound.cAstNodes; i++)
                utsConfigAstDestroy(pCfgAst->u.Compound.apAstNodes[i]);
            RTMemFree(pCfgAst);
            break;
        }
        case CFGASTNODETYPE_LIST:
        default:
            AssertMsgFailed(("Invalid AST node type %d\n", pCfgAst->enmType));
    }
}

DECLHIDDEN(PCFGAST) utsConfigAstGetByName(PCFGAST pCfgAst, const char *pszName)
{
    if (!pCfgAst)
        return NULL;

    AssertReturn(pCfgAst->enmType == CFGASTNODETYPE_COMPOUND, NULL);

    for (unsigned i = 0; i < pCfgAst->u.Compound.cAstNodes; i++)
    {
        PCFGAST pNode = pCfgAst->u.Compound.apAstNodes[i];

        if (!RTStrCmp(pNode->pszKey, pszName))
            return pNode;
    }

    return NULL;
}

