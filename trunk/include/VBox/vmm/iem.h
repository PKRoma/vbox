/** @file
 * IEM - Interpreted Execution Manager.
 */

/*
 * Copyright (C) 2011-2017 Oracle Corporation
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

#ifndef ___VBox_vmm_iem_h
#define ___VBox_vmm_iem_h

#include <VBox/types.h>
#include <VBox/vmm/trpm.h>
#include <iprt/assert.h>


RT_C_DECLS_BEGIN

/** @defgroup grp_iem       The Interpreted Execution Manager API.
 * @ingroup grp_vmm
 * @{
 */

/** @name IEMXCPTRAISEINFO_XXX - Extra info. on a recursive exception situation.
 *
 * This is primarily used by HM for working around a PGM limitation (see
 * @bugref{6607}) and special NMI/IRET handling. In the future, this may be
 * used for diagnostics.
 *
 * @{
 */
typedef uint32_t IEMXCPTRAISEINFO;
/** Pointer to a IEMXCPTINFO type. */
typedef IEMXCPTRAISEINFO *PIEMXCPTRAISEINFO;
/** No addition info. available. */
#define IEMXCPTRAISEINFO_NONE                    RT_BIT_32(0)
/** Delivery of a \#AC caused another \#AC. */
#define IEMXCPTRAISEINFO_AC_AC                   RT_BIT_32(1)
/** Delivery of a \#PF caused another \#PF. */
#define IEMXCPTRAISEINFO_PF_PF                   RT_BIT_32(2)
/** Delivery of a \#PF caused some contributory exception. */
#define IEMXCPTRAISEINFO_PF_CONTRIBUTORY_XCPT    RT_BIT_32(3)
/** Delivery of an external interrupt caused an exception. */
#define IEMXCPTRAISEINFO_EXT_INT_XCPT            RT_BIT_32(4)
/** Delivery of an external interrupt caused an \#PF. */
#define IEMXCPTRAISEINFO_EXT_INT_PF              RT_BIT_32(5)
/** Delivery of a software interrupt caused an exception. */
#define IEMXCPTRAISEINFO_SOFT_INT_XCPT           RT_BIT_32(6)
/** Delivery of an NMI caused an exception. */
#define IEMXCPTRAISEINFO_NMI_XCPT                RT_BIT_32(7)
/** Delivery of an NMI caused a \#PF. */
#define IEMXCPTRAISEINFO_NMI_PF                  RT_BIT_32(8)
/** Can re-execute the instruction at CS:RIP. */
#define IEMXCPTRAISEINFO_CAN_REEXEC_INSTR        RT_BIT_32(9)
/** @} */


/** @name IEMXCPTRAISE_XXX - Ways to handle a recursive exception condition.
 * @{ */
typedef enum IEMXCPTRAISE
{
    /** Raise the current (second) exception. */
    IEMXCPTRAISE_CURRENT_XCPT = 0,
    /** Re-raise the previous (first) event (for HM, unused by IEM). */
    IEMXCPTRAISE_PREV_EVENT,
    /** Re-execute instruction at CS:RIP (for HM, unused by IEM). */
    IEMXCPTRAISE_REEXEC_INSTR,
    /** Raise a \#DF exception. */
    IEMXCPTRAISE_DOUBLE_FAULT,
    /** Raise a triple fault. */
    IEMXCPTRAISE_TRIPLE_FAULT,
    /** Cause a CPU hang. */
    IEMXCPTRAISE_CPU_HANG,
    /** Invalid sequence of events. */
    IEMXCPTRAISE_INVALID = 0x7fffffff
} IEMXCPTRAISE;
/** Pointer to a IEMXCPTRAISE type. */
typedef IEMXCPTRAISE *PIEMXCPTRAISE;
/** @} */


/** @name Operand or addressing mode.
 * @{ */
typedef uint8_t IEMMODE;
#define IEMMODE_16BIT 0
#define IEMMODE_32BIT 1
#define IEMMODE_64BIT 2
/** @} */


/** @name IEM_XCPT_FLAGS_XXX - flags for iemRaiseXcptOrInt.
 * @{ */
/** CPU exception. */
#define IEM_XCPT_FLAGS_T_CPU_XCPT       RT_BIT_32(0)
/** External interrupt (from PIC, APIC, whatever). */
#define IEM_XCPT_FLAGS_T_EXT_INT        RT_BIT_32(1)
/** Software interrupt (int or into, not bound).
 * Returns to the following instruction */
#define IEM_XCPT_FLAGS_T_SOFT_INT       RT_BIT_32(2)
/** Takes an error code. */
#define IEM_XCPT_FLAGS_ERR              RT_BIT_32(3)
/** Takes a CR2. */
#define IEM_XCPT_FLAGS_CR2              RT_BIT_32(4)
/** Generated by the breakpoint instruction. */
#define IEM_XCPT_FLAGS_BP_INSTR         RT_BIT_32(5)
/** Generated by a DRx instruction breakpoint and RF should be cleared. */
#define IEM_XCPT_FLAGS_DRx_INSTR_BP     RT_BIT_32(6)
/** Generated by the icebp instruction. */
#define IEM_XCPT_FLAGS_ICEBP_INSTR      RT_BIT_32(7)
/** Generated by the overflow instruction. */
#define IEM_XCPT_FLAGS_OF_INSTR         RT_BIT_32(8)
/** @}  */


/** @name IEMTARGETCPU_XXX - IEM target CPU specification.
 *
 * This is a gross simpliciation of CPUMMICROARCH for dealing with really old
 * CPUs which didn't have much in the way of hinting at supported instructions
 * and features.  This slowly changes with the introduction of CPUID with the
 * Intel Pentium.
 *
 * @{
 */
/** The dynamic target CPU mode is for getting thru the BIOS and then use
 * the debugger or modifying instruction behaviour (e.g. HLT) to switch to a
 * different target CPU. */
#define IEMTARGETCPU_DYNAMIC    UINT32_C(0)
/** Intel 8086/8088.  */
#define IEMTARGETCPU_8086       UINT32_C(1)
/** NEC V20/V30.
 * @remarks must be between 8086 and 80186. */
#define IEMTARGETCPU_V20        UINT32_C(2)
/** Intel 80186/80188.  */
#define IEMTARGETCPU_186        UINT32_C(3)
/** Intel 80286.  */
#define IEMTARGETCPU_286        UINT32_C(4)
/** Intel 80386.  */
#define IEMTARGETCPU_386        UINT32_C(5)
/** Intel 80486.  */
#define IEMTARGETCPU_486        UINT32_C(6)
/** Intel Pentium .  */
#define IEMTARGETCPU_PENTIUM    UINT32_C(7)
/** Intel PentiumPro.  */
#define IEMTARGETCPU_PPRO       UINT32_C(8)
/** A reasonably current CPU, probably newer than the pentium pro when it comes
 * to the feature set and behaviour.  Generally the CPUID info and CPU vendor
 * dicates the behaviour here. */
#define IEMTARGETCPU_CURRENT    UINT32_C(9)
/** @} */


/** @name IEM status codes.
 *
 * Not quite sure how this will play out in the end, just aliasing safe status
 * codes for now.
 *
 * @{ */
#define VINF_IEM_RAISED_XCPT    VINF_EM_RESCHEDULE
/** @} */


/** The CPUMCTX_EXTRN_XXX mask required to be cleared when interpreting anything.
 * IEM will ASSUME the caller of IEM APIs has ensured these are already present. */
#define IEM_CPUMCTX_EXTRN_MUST_MASK    (  CPUMCTX_EXTRN_GPRS_MASK \
                                        | CPUMCTX_EXTRN_RIP \
                                        | CPUMCTX_EXTRN_RFLAGS \
                                        | CPUMCTX_EXTRN_SS \
                                        | CPUMCTX_EXTRN_CS \
                                        | CPUMCTX_EXTRN_CR0 \
                                        | CPUMCTX_EXTRN_CR3 \
                                        | CPUMCTX_EXTRN_CR4 \
                                        | CPUMCTX_EXTRN_APIC_TPR \
                                        | CPUMCTX_EXTRN_EFER \
                                        | CPUMCTX_EXTRN_DR7 )
/** The CPUMCTX_EXTRN_XXX mask needed when injecting an exception/interrupt.
 * IEM will import missing bits, callers are encouraged to make these registers
 * available prior to injection calls if fetching state anyway.  */
#define IEM_CPUMCTX_EXTRN_XCPT_MASK    (  IEM_CPUMCTX_EXTRN_MUST_MASK \
                                        | CPUMCTX_EXTRN_CR2 \
                                        | CPUMCTX_EXTRN_SREG_MASK \
                                        | CPUMCTX_EXTRN_TABLE_MASK )


VMMDECL(VBOXSTRICTRC)       IEMExecOne(PVMCPU pVCpu);
VMMDECL(VBOXSTRICTRC)       IEMExecOneEx(PVMCPU pVCpu, PCPUMCTXCORE pCtxCore, uint32_t *pcbWritten);
VMMDECL(VBOXSTRICTRC)       IEMExecOneWithPrefetchedByPC(PVMCPU pVCpu, PCPUMCTXCORE pCtxCore, uint64_t OpcodeBytesPC,
                                                         const void *pvOpcodeBytes, size_t cbOpcodeBytes);
VMMDECL(VBOXSTRICTRC)       IEMExecOneBypassEx(PVMCPU pVCpu, PCPUMCTXCORE pCtxCore, uint32_t *pcbWritten);
VMMDECL(VBOXSTRICTRC)       IEMExecOneBypassWithPrefetchedByPC(PVMCPU pVCpu, PCPUMCTXCORE pCtxCore, uint64_t OpcodeBytesPC,
                                                               const void *pvOpcodeBytes, size_t cbOpcodeBytes);
VMMDECL(VBOXSTRICTRC)       IEMExecOneBypassWithPrefetchedByPCWritten(PVMCPU pVCpu, PCPUMCTXCORE pCtxCore, uint64_t OpcodeBytesPC,
                                                                      const void *pvOpcodeBytes, size_t cbOpcodeBytes,
                                                                      uint32_t *pcbWritten);
VMMDECL(VBOXSTRICTRC)       IEMExecLots(PVMCPU pVCpu, uint32_t *pcInstructions);
/** Statistics returned by IEMExecForExits. */
typedef struct IEMEXECFOREXITSTATS
{
    uint32_t cInstructions;
    uint32_t cExits;
    uint32_t cMaxExitDistance;
    uint32_t cReserved;
} IEMEXECFOREXITSTATS;
/** Pointer to statistics returned by IEMExecForExits. */
typedef IEMEXECFOREXITSTATS *PIEMEXECFOREXITSTATS;
VMMDECL(VBOXSTRICTRC)       IEMExecForExits(PVMCPU pVCpu, uint32_t fWillExit, uint32_t cMinInstructions, uint32_t cMaxInstructions,
                                            uint32_t cMaxInstructionsWithoutExits, PIEMEXECFOREXITSTATS pStats);
VMMDECL(VBOXSTRICTRC)       IEMInjectTrpmEvent(PVMCPU pVCpu);
VMM_INT_DECL(VBOXSTRICTRC)  IEMInjectTrap(PVMCPU pVCpu, uint8_t u8TrapNo, TRPMEVENT enmType, uint16_t uErrCode, RTGCPTR uCr2,
                                          uint8_t cbInstr);

VMM_INT_DECL(int)           IEMBreakpointSet(PVM pVM, RTGCPTR GCPtrBp);
VMM_INT_DECL(int)           IEMBreakpointClear(PVM pVM, RTGCPTR GCPtrBp);

VMM_INT_DECL(void)          IEMTlbInvalidateAll(PVMCPU pVCpu, bool fVmm);
VMM_INT_DECL(void)          IEMTlbInvalidatePage(PVMCPU pVCpu, RTGCPTR GCPtr);
VMM_INT_DECL(void)          IEMTlbInvalidateAllPhysical(PVMCPU pVCpu);
VMM_INT_DECL(bool)          IEMGetCurrentXcpt(PVMCPU pVCpu, uint8_t *puVector, uint32_t *pfFlags, uint32_t *puErr,
                                              uint64_t *puCr2);
VMM_INT_DECL(IEMXCPTRAISE)  IEMEvaluateRecursiveXcpt(PVMCPU pVCpu, uint32_t fPrevFlags, uint8_t uPrevVector, uint32_t fCurFlags,
                                                     uint8_t uCurVector, PIEMXCPTRAISEINFO pXcptRaiseInfo);

/** @name Given Instruction Interpreters
 * @{ */
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecStringIoWrite(PVMCPU pVCpu, uint8_t cbValue, IEMMODE enmAddrMode,
                                                 bool fRepPrefix, uint8_t cbInstr, uint8_t iEffSeg, bool fIoChecked);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecStringIoRead(PVMCPU pVCpu, uint8_t cbValue, IEMMODE enmAddrMode,
                                                bool fRepPrefix, uint8_t cbInstr, bool fIoChecked);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedOut(PVMCPU pVCpu, uint8_t cbInstr, uint16_t u16Port, uint8_t cbReg);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedIn(PVMCPU pVCpu, uint8_t cbInstr, uint16_t u16Port, uint8_t cbReg);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedMovCRxWrite(PVMCPU pVCpu, uint8_t cbInstr, uint8_t iCrReg, uint8_t iGReg);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedMovCRxRead(PVMCPU pVCpu, uint8_t cbInstr, uint8_t iGReg, uint8_t iCrReg);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedClts(PVMCPU pVCpu, uint8_t cbInstr);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedLmsw(PVMCPU pVCpu, uint8_t cbInstr, uint16_t uValue);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedXsetbv(PVMCPU pVCpu, uint8_t cbInstr);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedInvlpg(PVMCPU pVCpu,  uint8_t cbInstr, RTGCPTR GCPtrPage);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedInvpcid(PVMCPU pVCpu, uint8_t cbInstr, uint8_t uType, RTGCPTR GCPtrInvpcidDesc);
#ifdef VBOX_WITH_NESTED_HWVIRT_SVM
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedClgi(PVMCPU pVCpu, uint8_t cbInstr);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedStgi(PVMCPU pVCpu, uint8_t cbInstr);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedVmload(PVMCPU pVCpu, uint8_t cbInstr);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedVmsave(PVMCPU pVCpu, uint8_t cbInstr);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedInvlpga(PVMCPU pVCpu, uint8_t cbInstr);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecDecodedVmrun(PVMCPU pVCpu, uint8_t cbInstr);
VMM_INT_DECL(VBOXSTRICTRC)  IEMExecSvmVmexit(PVMCPU pVCpu, uint64_t uExitCode, uint64_t uExitInfo1, uint64_t uExitInfo2);
#endif
/** @}  */


/** @defgroup grp_iem_r3     The IEM Host Context Ring-3 API.
 * @{
 */
VMMR3DECL(int)      IEMR3Init(PVM pVM);
VMMR3DECL(int)      IEMR3Term(PVM pVM);
VMMR3DECL(void)     IEMR3Relocate(PVM pVM);
VMMR3_INT_DECL(VBOXSTRICTRC) IEMR3ProcessForceFlag(PVM pVM, PVMCPU pVCpu, VBOXSTRICTRC rcStrict);
/** @} */

/** @} */

RT_C_DECLS_END

#endif

