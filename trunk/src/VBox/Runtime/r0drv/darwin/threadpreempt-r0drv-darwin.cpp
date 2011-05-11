/* $Id$ */
/** @file
 * IPRT - Thread Preemption, Ring-0 Driver, Darwin.
 */

/*
 * Copyright (C) 2009 Oracle Corporation
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


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "the-darwin-kernel.h"
#include "internal/iprt.h"
#include <iprt/thread.h>

#if defined(RT_ARCH_AMD64) || defined(RT_ARCH_X86)
# include <iprt/asm-amd64-x86.h>
#endif
#include <iprt/assert.h>
#include <iprt/err.h>
#include <iprt/mp.h>


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct RTDARWINPREEMPTHACK
{
    /** The spinlock we exploit for disabling preemption. */
    lck_spin_t         *pSpinLock;
    /** The preemption count for this CPU, to guard against nested calls. */
    uint32_t            cRecursion;
} RTDARWINPREEMPTHACK;
typedef RTDARWINPREEMPTHACK *PRTDARWINPREEMPTHACK;


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static RTDARWINPREEMPTHACK  g_aPreemptHacks[16]; /* see MAX_CPUS in i386/mp.h */


/**
 * Allocates the per-cpu spin locks used to disable preemption.
 *
 * Called by rtR0InitNative.
 */
int rtThreadPreemptDarwinInit(void)
{
    Assert(g_pDarwinLockGroup);

    for (size_t i = 0; i < RT_ELEMENTS(g_aPreemptHacks); i++)
    {
        g_aPreemptHacks[i].pSpinLock = lck_spin_alloc_init(g_pDarwinLockGroup, LCK_ATTR_NULL);
        if (!g_aPreemptHacks[i].pSpinLock)
            return VERR_NO_MEMORY; /* (The caller will invoke rtThreadPreemptDarwinTerm) */
    }
    return VINF_SUCCESS;
}


/**
 * Frees the per-cpu spin locks used to disable preemption.
 *
 * Called by rtR0TermNative.
 */
void rtThreadPreemptDarwinTerm(void)
{
    for (size_t i = 0; i < RT_ELEMENTS(g_aPreemptHacks); i++)
        if (g_aPreemptHacks[i].pSpinLock)
        {
            lck_spin_free(g_aPreemptHacks[i].pSpinLock, g_pDarwinLockGroup);
            g_aPreemptHacks[i].pSpinLock = NULL;
        }
}


RTDECL(bool) RTThreadPreemptIsEnabled(RTTHREAD hThread)
{
    Assert(hThread == NIL_RTTHREAD);
    return preemption_enabled();
}


RTDECL(bool) RTThreadPreemptIsPending(RTTHREAD hThread)
{
    Assert(hThread == NIL_RTTHREAD);

    /* HACK ALERT! This ASSUMES that the cpu_pending_ast member of cpu_data_t doesn't move. */
    uint32_t ast_pending;
#if defined(RT_ARCH_X86) || defined(RT_ARCH_AMD64)
    __asm__ volatile("movl %%gs:%P1,%0\n\t"
                     : "=r" (ast_pending)
                     : "i"  (7*sizeof(void*) + (7 + (ARCH_BITS == 64)) *sizeof(int)) );
#else
# error fixme.
    cpu_data_t *pCpu = current_cpu_datap(void);
    AssertCompileMemberOffset(cpu_data_t, cpu_pending_ast, 7*sizeof(void*) + 7*sizeof(int));
    cpu_pending_ast = pCpu->cpu_pending_ast;
#endif
    AssertMsg(!(ast_pending & UINT32_C(0xfffff000)),("%#x\n", ast_pending));
    return (ast_pending & (AST_PREEMPT | AST_URGENT)) != 0;
}


RTDECL(bool) RTThreadPreemptIsPendingTrusty(void)
{
    /* yes, we think that RTThreadPreemptIsPending is reliable... */
    return true;
}


RTDECL(bool) RTThreadPreemptIsPossible(void)
{
    /* yes, kernel preemption is possible. */
    return true;
}


RTDECL(void) RTThreadPreemptDisable(PRTTHREADPREEMPTSTATE pState)
{
    AssertPtr(pState);
    Assert(pState->u32Reserved == 0);
    pState->u32Reserved = 42;

    /*
     * Disable to prevent preemption while we grab the per-cpu spin lock.
     * Note! Only take the lock on the first call or we end up spinning for ever.
     */
    RTCCUINTREG fSavedFlags = ASMIntDisableFlags();
    RTCPUID     idCpu       = RTMpCpuId();
    if (RT_UNLIKELY(idCpu < RT_ELEMENTS(g_aPreemptHacks)))
    {
        Assert(g_aPreemptHacks[idCpu].cRecursion < UINT32_MAX / 2);
        if (++g_aPreemptHacks[idCpu].cRecursion == 1)
        {
            lck_spin_t *pSpinLock = g_aPreemptHacks[idCpu].pSpinLock;
            if (pSpinLock)
                lck_spin_lock(pSpinLock);
            else
                AssertFailed();
        }
    }
    ASMSetFlags(fSavedFlags);
    Assert(!RTThreadPreemptIsEnabled(NIL_RTTHREAD));
    RT_ASSERT_PREEMPT_CPUID_DISABLE(pState);
}


RTDECL(void) RTThreadPreemptRestore(PRTTHREADPREEMPTSTATE pState)
{
    AssertPtr(pState);
    Assert(pState->u32Reserved == 42);
    pState->u32Reserved = 0;
    RT_ASSERT_PREEMPT_CPUID_RESTORE(pState);

    RTCPUID idCpu = RTMpCpuId();
    if (RT_UNLIKELY(idCpu < RT_ELEMENTS(g_aPreemptHacks)))
    {
        Assert(g_aPreemptHacks[idCpu].cRecursion > 0);
        if (--g_aPreemptHacks[idCpu].cRecursion == 0)
        {
            lck_spin_t *pSpinLock = g_aPreemptHacks[idCpu].pSpinLock;
            if (pSpinLock)
                lck_spin_unlock(pSpinLock);
            else
                AssertFailed();
        }
    }
}


RTDECL(bool) RTThreadIsInInterrupt(RTTHREAD hThread)
{
    Assert(hThread == NIL_RTTHREAD); NOREF(hThread);
    /** @todo Darwin: Implement RTThreadIsInInterrupt. Required for guest
     *        additions! */
    return !ASMIntAreEnabled();
}

