; $Id$
;; @file
; VMM - R0 SetJmp / LongJmp routines for AMD64.
;

;
; Copyright (C) 2006-2009 Sun Microsystems, Inc.
;
; This file is part of VirtualBox Open Source Edition (OSE), as
; available from http://www.virtualbox.org. This file is free software;
; you can redistribute it and/or modify it under the terms of the GNU
; General Public License (GPL) as published by the Free Software
; Foundation, in version 2 as it comes in the "COPYING" file of the
; VirtualBox OSE distribution. VirtualBox OSE is distributed in the
; hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;
; Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
; Clara, CA 95054 USA or visit http://www.sun.com if you need
; additional information or have any questions.
;

;*******************************************************************************
;* Header Files                                                                *
;*******************************************************************************
%include "VBox/asmdefs.mac"
%include "../VMMInternal.mac"
%include "iprt/err.mac"
%include "VBox/param.mac"


;*******************************************************************************
;*  Defined Constants And Macros                                               *
;*******************************************************************************
%define RESUME_MAGIC    07eadf00dh
%define STACK_PADDING   0eeeeeeeeeeeeeeeeh


; For vmmR0LoggerWrapper. (The other architecture(s) use(s) C99 variadict macros.)
extern NAME(RTLogLogger)


BEGINCODE


;;
; The setjmp variant used for calling Ring-3.
;
; This differs from the normal setjmp in that it will resume VMMRZCallRing3 if we're
; in the middle of a ring-3 call. Another differences is the function pointer and
; argument. This has to do with resuming code and the stack frame of the caller.
;
; @returns  VINF_SUCCESS on success or whatever is passed to vmmR0CallRing3LongJmp.
; @param    pJmpBuf msc:rcx gcc:rdi x86:[esp+0x04]     Our jmp_buf.
; @param    pfn     msc:rdx gcc:rsi x86:[esp+0x08]     The function to be called when not resuming.
; @param    pvUser1 msc:r8  gcc:rdx x86:[esp+0x0c]     The argument of that function.
; @param    pvUser2 msc:r9  gcc:rcx x86:[esp+0x10]     The argument of that function.
;
BEGINPROC vmmR0CallRing3SetJmp
GLOBALNAME vmmR0CallRing3SetJmpEx
    ;
    ; Save the registers.
    ;
    push    rbp
    mov     rbp, rsp
 %ifdef ASM_CALL64_MSC
    sub     rsp, 30h
    mov     r11, rdx                    ; pfn
    mov     rdx, rcx                    ; pJmpBuf;
 %else
    sub     rsp, 10h
    mov     r8, rdx                     ; pvUser1 (save it like MSC)
    mov     r9, rcx                     ; pvUser2 (save it like MSC)
    mov     r11, rsi                    ; pfn
    mov     rdx, rdi                    ; pJmpBuf
 %endif
    mov     [xDX + VMMR0JMPBUF.rbx], rbx
 %ifdef ASM_CALL64_MSC
    mov     [xDX + VMMR0JMPBUF.rsi], rsi
    mov     [xDX + VMMR0JMPBUF.rdi], rdi
 %endif
    mov     r10, [rbp]
    mov     [xDX + VMMR0JMPBUF.rbp], r10
    mov     [xDX + VMMR0JMPBUF.r12], r12
    mov     [xDX + VMMR0JMPBUF.r13], r13
    mov     [xDX + VMMR0JMPBUF.r14], r14
    mov     [xDX + VMMR0JMPBUF.r15], r15
    mov     xAX, [rbp + 8]
    mov     [xDX + VMMR0JMPBUF.rip], xAX
    lea     r10, [rbp + 10h]            ; (used in resume)
    mov     [xDX + VMMR0JMPBUF.rsp], r10

    ;
    ; If we're not in a ring-3 call, call pfn and return.
    ;
    test    byte [xDX + VMMR0JMPBUF.fInRing3Call], 1
    jnz     .resume

 %ifdef VMM_R0_SWITCH_STACK
    mov     r15, [xDX + VMMR0JMPBUF.pvSavedStack]
    test    r15, r15
    jz      .entry_error
  %ifdef VBOX_STRICT
    cmp     dword [r15], 0h
    jne     .entry_error
    mov     rdi, r15
    mov     rcx, VMM_STACK_SIZE / 8
    mov     rax, qword 0eeeeeeeffeeeeeeeh
    repne stosq
    mov     [rdi - 10h], rbx
  %endif
    lea     r15, [r15 + VMM_STACK_SIZE - 40h]
    mov     rsp, r15                    ; Switch stack!
 %endif ; VMM_R0_SWITCH_STACK

    mov     r12, rdx                    ; Save pJmpBuf.
 %ifdef ASM_CALL64_MSC
    mov     rcx, r8                     ; pvUser -> arg0
    mov     rdx, r9
 %else
    mov     rdi, r8                     ; pvUser -> arg0
    mov     rsi, r9
 %endif
    call    r11
    mov     rdx, r12                    ; Restore pJmpBuf

 %ifdef VMM_R0_SWITCH_STACK
  %ifdef VBOX_STRICT
    mov     r15, [xDX + VMMR0JMPBUF.pvSavedStack]
    mov     dword [r15], 0h             ; Reset the marker
  %endif
 %endif

    ;
    ; Return like in the long jump but clear eip, no short cuts here.
    ;
.proper_return:
    mov     rbx, [xDX + VMMR0JMPBUF.rbx]
 %ifdef ASM_CALL64_MSC
    mov     rsi, [xDX + VMMR0JMPBUF.rsi]
    mov     rdi, [xDX + VMMR0JMPBUF.rdi]
 %endif
    mov     r12, [xDX + VMMR0JMPBUF.r12]
    mov     r13, [xDX + VMMR0JMPBUF.r13]
    mov     r14, [xDX + VMMR0JMPBUF.r14]
    mov     r15, [xDX + VMMR0JMPBUF.r15]
    mov     rbp, [xDX + VMMR0JMPBUF.rbp]
    mov     xCX, [xDX + VMMR0JMPBUF.rip]
    and     qword [xDX + VMMR0JMPBUF.rip], byte 0 ; used for valid check.
    mov     rsp, [xDX + VMMR0JMPBUF.rsp]
    jmp     xCX

.entry_error:
    mov     eax, VERR_INTERNAL_ERROR_2
    jmp     .proper_return

.stack_overflow:
    mov     eax, VERR_INTERNAL_ERROR_5
    jmp     .proper_return

    ;
    ; Aborting resume.
    ;
.bad:
    and     qword [xDX + VMMR0JMPBUF.rip], byte 0 ; used for valid check.
    mov     rbx, [xDX + VMMR0JMPBUF.rbx]
 %ifdef ASM_CALL64_MSC
    mov     rsi, [xDX + VMMR0JMPBUF.rsi]
    mov     rdi, [xDX + VMMR0JMPBUF.rdi]
 %endif
    mov     r12, [xDX + VMMR0JMPBUF.r12]
    mov     r13, [xDX + VMMR0JMPBUF.r13]
    mov     r14, [xDX + VMMR0JMPBUF.r14]
    mov     r15, [xDX + VMMR0JMPBUF.r15]
    mov     eax, VERR_INTERNAL_ERROR_3    ; todo better return code!
    leave
    ret

    ;
    ; Resume VMMRZCallRing3 the call.
    ;
.resume:
    ; Sanity checks.
 %ifdef VMM_R0_SWITCH_STACK
    ;; @todo amd64/switch/resume sanity.
 %else  ; !VMM_R0_SWITCH_STACK
    cmp     r10, [xDX + VMMR0JMPBUF.SpCheck]
    jne     .bad

    mov     ecx, [xDX + VMMR0JMPBUF.cbSavedStack]
    cmp     rcx, VMM_STACK_SIZE
    ja      .bad
    test    rcx, 3
    jnz     .bad
    mov     rdi, [xDX + VMMR0JMPBUF.rsp]
    sub     rdi, [xDX + VMMR0JMPBUF.SpResume]
    cmp     rcx, rdi
    jne     .bad
 %endif

%ifdef VMM_R0_SWITCH_STACK
    ; Switch stack.
    mov     rsp, [xDX + VMMR0JMPBUF.SpResume]
%else
    ; Restore the stack.
    mov     ecx, [xDX + VMMR0JMPBUF.cbSavedStack]
    shr     ecx, 3
    mov     rsi, [xDX + VMMR0JMPBUF.pvSavedStack]
    mov     rdi, [xDX + VMMR0JMPBUF.SpResume]
    mov     rsp, rdi
    rep movsq
%endif ; !VMM_R0_SWITCH_STACK
    mov     byte [xDX + VMMR0JMPBUF.fInRing3Call], 0

    ;
    ; Continue where we left off.
    ;
%ifdef VBOX_STRICT
    pop     rax                         ; magic
    cmp     rax, RESUME_MAGIC
    je      .magic_ok
    mov     ecx, 0123h
    mov     [ecx], edx
.magic_ok:
%endif
    popf
    pop     rbx
 %ifdef ASM_CALL64_MSC
    pop     rsi
    pop     rdi
 %endif
    pop     r12
    pop     r13
    pop     r14
    pop     r15
    pop     rbp
    xor     eax, eax                    ; VINF_SUCCESS
    ret
ENDPROC vmmR0CallRing3SetJmp


;;
; Worker for VMMRZCallRing3.
; This will save the stack and registers.
;
; @param    pJmpBuf msc:rcx gcc:rdi x86:[ebp+8]     Pointer to the jump buffer.
; @param    rc      msc:rdx gcc:rsi x86:[ebp+c]     The return code.
;
BEGINPROC vmmR0CallRing3LongJmp
    ;
    ; Save the registers on the stack.
    ;
    push    rbp
    mov     rbp, rsp
    push    r15
    push    r14
    push    r13
    push    r12
 %ifdef ASM_CALL64_MSC
    push    rdi
    push    rsi
 %endif
    push    rbx
    pushf
%ifdef VBOX_STRICT
    push    RESUME_MAGIC
%endif

    ;
    ; Normalize the parameters.
    ;
 %ifdef ASM_CALL64_MSC
    mov     eax, edx                    ; rc
    mov     rdx, rcx                    ; pJmpBuf
 %else
    mov     rdx, rdi                    ; pJmpBuf
    mov     eax, esi                    ; rc
 %endif

    ;
    ; Is the jump buffer armed?
    ;
    cmp     qword [xDX + VMMR0JMPBUF.rip], byte 0
    je      .nok

    ;
    ; Sanity checks.
    ;
    mov     rdi, [xDX + VMMR0JMPBUF.pvSavedStack]
    test    rdi, rdi                    ; darwin may set this to 0.
    jz      .nok
    mov     [xDX + VMMR0JMPBUF.SpResume], rsp
 %ifndef VMM_R0_SWITCH_STACK
    mov     rsi, rsp
    mov     rcx, [xDX + VMMR0JMPBUF.rsp]
    sub     rcx, rsi

    ; two sanity checks on the size.
    cmp     rcx, VMM_STACK_SIZE         ; check max size.
    jnbe    .nok

    ;
    ; Copy the stack
    ;
    test    ecx, 7                      ; check alignment
    jnz     .nok
    mov     [xDX + VMMR0JMPBUF.cbSavedStack], ecx
    shr     ecx, 3
    rep movsq

 %endif ; !VMM_R0_SWITCH_STACK

    ; Save RSP & RBP to enable stack dumps
    mov     rcx, rbp
    mov     [xDX + VMMR0JMPBUF.SavedEbp], rcx
    sub     rcx, 8
    mov     [xDX + VMMR0JMPBUF.SavedEsp], rcx

    ; store the last pieces of info.
    mov     rcx, [xDX + VMMR0JMPBUF.rsp]
    mov     [xDX + VMMR0JMPBUF.SpCheck], rcx
    mov     byte [xDX + VMMR0JMPBUF.fInRing3Call], 1

    ;
    ; Do the long jump.
    ;
    mov     rbx, [xDX + VMMR0JMPBUF.rbx]
 %ifdef ASM_CALL64_MSC
    mov     rsi, [xDX + VMMR0JMPBUF.rsi]
    mov     rdi, [xDX + VMMR0JMPBUF.rdi]
 %endif
    mov     r12, [xDX + VMMR0JMPBUF.r12]
    mov     r13, [xDX + VMMR0JMPBUF.r13]
    mov     r14, [xDX + VMMR0JMPBUF.r14]
    mov     r15, [xDX + VMMR0JMPBUF.r15]
    mov     rbp, [xDX + VMMR0JMPBUF.rbp]
    mov     rcx, [xDX + VMMR0JMPBUF.rip]
    mov     rsp, [xDX + VMMR0JMPBUF.rsp]
    jmp     rcx

    ;
    ; Failure
    ;
.nok:
%ifdef VBOX_STRICT
    pop     rax                         ; magic
    cmp     rax, RESUME_MAGIC
    je      .magic_ok
    mov     ecx, 0123h
    mov     [rcx], edx
.magic_ok:
%endif
    mov     eax, VERR_INTERNAL_ERROR_4
    popf
    pop     rbx
 %ifdef ASM_CALL64_MSC
    pop     rsi
    pop     rdi
 %endif
    pop     r12
    pop     r13
    pop     r14
    pop     r15
    leave
    ret
ENDPROC vmmR0CallRing3LongJmp


;;
; Internal R0 logger worker: Logger wrapper.
;
; @cproto VMMR0DECL(void) vmmR0LoggerWrapper(const char *pszFormat, ...)
;
EXPORTEDNAME vmmR0LoggerWrapper
    int3
    int3
    int3
    ret
ENDPROC vmmR0LoggerWrapper

