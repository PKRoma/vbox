; $Id$ 
;; @file
; Auto Generated source file. Do not edit.
;

;
; Source file: vgarom.asm
;
;  ============================================================================================
;  
;   Copyright (C) 2001,2002 the LGPL VGABios developers Team
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
;  
;  ============================================================================================
;  
;   This VGA Bios is specific to the plex86/bochs Emulated VGA card.
;   You can NOT drive any physical vga card with it.
;  
;  ============================================================================================
;  

;
; Source file: vberom.asm
;
;  ============================================================================================
;  
;   Copyright (C) 2002 Jeroen Janssen
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
;  
;  ============================================================================================
;  
;   This VBE is part of the VGA Bios specific to the plex86/bochs Emulated VGA card.
;   You can NOT drive any physical vga card with it.
;  
;  ============================================================================================
;  
;   This VBE Bios is based on information taken from :
;    - VESA BIOS EXTENSION (VBE) Core Functions Standard Version 3.0 located at www.vesa.org
;  
;  ============================================================================================

;
; Source file: vgabios.c
;
;  // ============================================================================================
;  
;  vgabios.c
;  
;  // ============================================================================================
;  //
;  //  Copyright (C) 2001,2002 the LGPL VGABios developers Team
;  //
;  //  This library is free software; you can redistribute it and/or
;  //  modify it under the terms of the GNU Lesser General Public
;  //  License as published by the Free Software Foundation; either
;  //  version 2 of the License, or (at your option) any later version.
;  //
;  //  This library is distributed in the hope that it will be useful,
;  //  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  //  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;  //  Lesser General Public License for more details.
;  //
;  //  You should have received a copy of the GNU Lesser General Public
;  //  License along with this library; if not, write to the Free Software
;  //  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
;  //
;  // ============================================================================================
;  //
;  //  This VGA Bios is specific to the plex86/bochs Emulated VGA card.
;  //  You can NOT drive any physical vga card with it.
;  //
;  // ============================================================================================
;  //
;  //  This file contains code ripped from :
;  //   - rombios.c of plex86
;  //
;  //  This VGA Bios contains fonts from :
;  //   - fntcol16.zip (c) by Joseph Gil avalable at :
;  //      ftp://ftp.simtel.net/pub/simtelnet/msdos/screen/fntcol16.zip
;  //     These fonts are public domain
;  //
;  //  This VGA Bios is based on information taken from :
;  //   - Kevin Lawton's vga card emulation for bochs/plex86
;  //   - Ralf Brown's interrupts list available at http://www.cs.cmu.edu/afs/cs/user/ralf/pub/WWW/files.html
;  //   - Finn Thogersons' VGADOC4b available at http://home.worldonline.dk/~finth/
;  //   - Michael Abrash's Graphics Programming Black Book
;  //   - Francois Gervais' book "programmation des cartes graphiques cga-ega-vga" edited by sybex
;  //   - DOSEMU 1.0.1 source code for several tables values and formulas
;  //
;  // Thanks for patches, comments and ideas to :
;  //   - techt@pikeonline.net
;  //
;  // ============================================================================================

;
; Source file: vbe.c
;
;  // ============================================================================================
;  //
;  //  Copyright (C) 2002 Jeroen Janssen
;  //
;  //  This library is free software; you can redistribute it and/or
;  //  modify it under the terms of the GNU Lesser General Public
;  //  License as published by the Free Software Foundation; either
;  //  version 2 of the License, or (at your option) any later version.
;  //
;  //  This library is distributed in the hope that it will be useful,
;  //  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  //  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;  //  Lesser General Public License for more details.
;  //
;  //  You should have received a copy of the GNU Lesser General Public
;  //  License along with this library; if not, write to the Free Software
;  //  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
;  //
;  // ============================================================================================
;  //
;  //  This VBE is part of the VGA Bios specific to the plex86/bochs Emulated VGA card.
;  //  You can NOT drive any physical vga card with it.
;  //
;  // ============================================================================================
;  //
;  //  This VBE Bios is based on information taken from :
;  //   - VESA BIOS EXTENSION (VBE) Core Functions Standard Version 3.0 located at www.vesa.org
;  //
;  // ============================================================================================

;
; Oracle LGPL Disclaimer: For the avoidance of doubt, except that if any license choice
; other than GPL or LGPL is available it will apply instead, Oracle elects to use only
; the Lesser General Public License version 2.1 (LGPLv2) at this time for any software where
; a choice of LGPL license versions is made available with the language indicating
; that LGPLv2 or any later version may be used, or where a choice of which version
; of the LGPL is applied is otherwise unspecified.
;





section VGAROM progbits vstart=0x0 align=1 ; size=0x992 class=CODE group=AUTO
    db  055h, 0aah, 040h, 0e9h, 062h, 00ah, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 049h, 042h
    db  04dh, 000h
vgabios_int10_handler:                       ; 0xc0022 LB 0x54e
    pushfw                                    ; 9c
    cmp ah, 00fh                              ; 80 fc 0f
    jne short 0002eh                          ; 75 06
    call 00183h                               ; e8 58 01
    jmp near 000f3h                           ; e9 c5 00
    cmp ah, 01ah                              ; 80 fc 1a
    jne short 00039h                          ; 75 06
    call 00538h                               ; e8 02 05
    jmp near 000f3h                           ; e9 ba 00
    cmp ah, 00bh                              ; 80 fc 0b
    jne short 00044h                          ; 75 06
    call 000f5h                               ; e8 b4 00
    jmp near 000f3h                           ; e9 af 00
    cmp ax, 01103h                            ; 3d 03 11
    jne short 0004fh                          ; 75 06
    call 0042fh                               ; e8 e3 03
    jmp near 000f3h                           ; e9 a4 00
    cmp ah, 012h                              ; 80 fc 12
    jne short 00092h                          ; 75 3e
    cmp bl, 010h                              ; 80 fb 10
    jne short 0005fh                          ; 75 06
    call 0043ch                               ; e8 e0 03
    jmp near 000f3h                           ; e9 94 00
    cmp bl, 030h                              ; 80 fb 30
    jne short 0006ah                          ; 75 06
    call 0045fh                               ; e8 f8 03
    jmp near 000f3h                           ; e9 89 00
    cmp bl, 031h                              ; 80 fb 31
    jne short 00074h                          ; 75 05
    call 004b2h                               ; e8 40 04
    jmp short 000f3h                          ; eb 7f
    cmp bl, 032h                              ; 80 fb 32
    jne short 0007eh                          ; 75 05
    call 004d4h                               ; e8 58 04
    jmp short 000f3h                          ; eb 75
    cmp bl, 033h                              ; 80 fb 33
    jne short 00088h                          ; 75 05
    call 004f2h                               ; e8 6c 04
    jmp short 000f3h                          ; eb 6b
    cmp bl, 034h                              ; 80 fb 34
    jne short 000e5h                          ; 75 58
    call 00516h                               ; e8 86 04
    jmp short 000f3h                          ; eb 61
    cmp ax, 0101bh                            ; 3d 1b 10
    je short 000e5h                           ; 74 4e
    cmp ah, 010h                              ; 80 fc 10
    jne short 000a1h                          ; 75 05
    call 001aah                               ; e8 0b 01
    jmp short 000f3h                          ; eb 52
    cmp ah, 04fh                              ; 80 fc 4f
    jne short 000e5h                          ; 75 3f
    cmp AL, strict byte 003h                  ; 3c 03
    jne short 000afh                          ; 75 05
    call 0080dh                               ; e8 60 07
    jmp short 000f3h                          ; eb 44
    cmp AL, strict byte 005h                  ; 3c 05
    jne short 000b8h                          ; 75 05
    call 00832h                               ; e8 7c 07
    jmp short 000f3h                          ; eb 3b
    cmp AL, strict byte 006h                  ; 3c 06
    jne short 000c1h                          ; 75 05
    call 0085fh                               ; e8 a0 07
    jmp short 000f3h                          ; eb 32
    cmp AL, strict byte 007h                  ; 3c 07
    jne short 000cah                          ; 75 05
    call 008ach                               ; e8 e4 07
    jmp short 000f3h                          ; eb 29
    cmp AL, strict byte 008h                  ; 3c 08
    jne short 000d3h                          ; 75 05
    call 008e0h                               ; e8 0f 08
    jmp short 000f3h                          ; eb 20
    cmp AL, strict byte 009h                  ; 3c 09
    jne short 000dch                          ; 75 05
    call 00917h                               ; e8 3d 08
    jmp short 000f3h                          ; eb 17
    cmp AL, strict byte 00ah                  ; 3c 0a
    jne short 000e5h                          ; 75 05
    call 0097bh                               ; e8 98 08
    jmp short 000f3h                          ; eb 0e
    push ES                                   ; 06
    push DS                                   ; 1e
    pushaw                                    ; 60
    mov bx, 0c000h                            ; bb 00 c0
    mov ds, bx                                ; 8e db
    call 03007h                               ; e8 17 2f
    popaw                                     ; 61
    pop DS                                    ; 1f
    pop ES                                    ; 07
    popfw                                     ; 9d
    iret                                      ; cf
    cmp bh, 000h                              ; 80 ff 00
    je short 00100h                           ; 74 06
    cmp bh, 001h                              ; 80 ff 01
    je short 00151h                           ; 74 52
    retn                                      ; c3
    push ax                                   ; 50
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    push DS                                   ; 1e
    mov dx, strict word 00040h                ; ba 40 00
    mov ds, dx                                ; 8e da
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    cmp byte [word 00049h], 003h              ; 80 3e 49 00 03
    jbe short 00144h                          ; 76 2f
    mov dx, 003c0h                            ; ba c0 03
    mov AL, strict byte 000h                  ; b0 00
    out DX, AL                                ; ee
    db  08ah, 0c3h
    ; mov al, bl                                ; 8a c3
    and AL, strict byte 00fh                  ; 24 0f
    test AL, strict byte 008h                 ; a8 08
    je short 00125h                           ; 74 02
    add AL, strict byte 008h                  ; 04 08
    out DX, AL                                ; ee
    mov CL, strict byte 001h                  ; b1 01
    and bl, 010h                              ; 80 e3 10
    mov dx, 003c0h                            ; ba c0 03
    db  08ah, 0c1h
    ; mov al, cl                                ; 8a c1
    out DX, AL                                ; ee
    mov dx, 003c1h                            ; ba c1 03
    in AL, DX                                 ; ec
    and AL, strict byte 0efh                  ; 24 ef
    db  00ah, 0c3h
    ; or al, bl                                 ; 0a c3
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    db  0feh, 0c1h
    ; inc cl                                    ; fe c1
    cmp cl, 004h                              ; 80 f9 04
    jne short 0012bh                          ; 75 e7
    mov AL, strict byte 020h                  ; b0 20
    out DX, AL                                ; ee
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    pop DS                                    ; 1f
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop ax                                    ; 58
    retn                                      ; c3
    push ax                                   ; 50
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov CL, strict byte 001h                  ; b1 01
    and bl, 001h                              ; 80 e3 01
    mov dx, 003c0h                            ; ba c0 03
    db  08ah, 0c1h
    ; mov al, cl                                ; 8a c1
    out DX, AL                                ; ee
    mov dx, 003c1h                            ; ba c1 03
    in AL, DX                                 ; ec
    and AL, strict byte 0feh                  ; 24 fe
    db  00ah, 0c3h
    ; or al, bl                                 ; 0a c3
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    db  0feh, 0c1h
    ; inc cl                                    ; fe c1
    cmp cl, 004h                              ; 80 f9 04
    jne short 0015eh                          ; 75 e7
    mov AL, strict byte 020h                  ; b0 20
    out DX, AL                                ; ee
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop ax                                    ; 58
    retn                                      ; c3
    push DS                                   ; 1e
    mov ax, strict word 00040h                ; b8 40 00
    mov ds, ax                                ; 8e d8
    push bx                                   ; 53
    mov bx, strict word 00062h                ; bb 62 00
    mov al, byte [bx]                         ; 8a 07
    pop bx                                    ; 5b
    db  08ah, 0f8h
    ; mov bh, al                                ; 8a f8
    push bx                                   ; 53
    mov bx, 00087h                            ; bb 87 00
    mov ah, byte [bx]                         ; 8a 27
    and ah, 080h                              ; 80 e4 80
    mov bx, strict word 00049h                ; bb 49 00
    mov al, byte [bx]                         ; 8a 07
    db  00ah, 0c4h
    ; or al, ah                                 ; 0a c4
    mov bx, strict word 0004ah                ; bb 4a 00
    mov ah, byte [bx]                         ; 8a 27
    pop bx                                    ; 5b
    pop DS                                    ; 1f
    retn                                      ; c3
    cmp AL, strict byte 000h                  ; 3c 00
    jne short 001b0h                          ; 75 02
    jmp short 00211h                          ; eb 61
    cmp AL, strict byte 001h                  ; 3c 01
    jne short 001b6h                          ; 75 02
    jmp short 0022fh                          ; eb 79
    cmp AL, strict byte 002h                  ; 3c 02
    jne short 001bch                          ; 75 02
    jmp short 00237h                          ; eb 7b
    cmp AL, strict byte 003h                  ; 3c 03
    jne short 001c3h                          ; 75 03
    jmp near 00268h                           ; e9 a5 00
    cmp AL, strict byte 007h                  ; 3c 07
    jne short 001cah                          ; 75 03
    jmp near 00292h                           ; e9 c8 00
    cmp AL, strict byte 008h                  ; 3c 08
    jne short 001d1h                          ; 75 03
    jmp near 002bah                           ; e9 e9 00
    cmp AL, strict byte 009h                  ; 3c 09
    jne short 001d8h                          ; 75 03
    jmp near 002c8h                           ; e9 f0 00
    cmp AL, strict byte 010h                  ; 3c 10
    jne short 001dfh                          ; 75 03
    jmp near 0030dh                           ; e9 2e 01
    cmp AL, strict byte 012h                  ; 3c 12
    jne short 001e6h                          ; 75 03
    jmp near 00326h                           ; e9 40 01
    cmp AL, strict byte 013h                  ; 3c 13
    jne short 001edh                          ; 75 03
    jmp near 0034eh                           ; e9 61 01
    cmp AL, strict byte 015h                  ; 3c 15
    jne short 001f4h                          ; 75 03
    jmp near 00395h                           ; e9 a1 01
    cmp AL, strict byte 017h                  ; 3c 17
    jne short 001fbh                          ; 75 03
    jmp near 003b0h                           ; e9 b5 01
    cmp AL, strict byte 018h                  ; 3c 18
    jne short 00202h                          ; 75 03
    jmp near 003d8h                           ; e9 d6 01
    cmp AL, strict byte 019h                  ; 3c 19
    jne short 00209h                          ; 75 03
    jmp near 003e3h                           ; e9 da 01
    cmp AL, strict byte 01ah                  ; 3c 1a
    jne short 00210h                          ; 75 03
    jmp near 003eeh                           ; e9 de 01
    retn                                      ; c3
    cmp bl, 014h                              ; 80 fb 14
    jnbe short 0022eh                         ; 77 18
    push ax                                   ; 50
    push dx                                   ; 52
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov dx, 003c0h                            ; ba c0 03
    db  08ah, 0c3h
    ; mov al, bl                                ; 8a c3
    out DX, AL                                ; ee
    db  08ah, 0c7h
    ; mov al, bh                                ; 8a c7
    out DX, AL                                ; ee
    mov AL, strict byte 020h                  ; b0 20
    out DX, AL                                ; ee
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    pop dx                                    ; 5a
    pop ax                                    ; 58
    retn                                      ; c3
    push bx                                   ; 53
    mov BL, strict byte 011h                  ; b3 11
    call 00211h                               ; e8 dc ff
    pop bx                                    ; 5b
    retn                                      ; c3
    push ax                                   ; 50
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    db  08bh, 0dah
    ; mov bx, dx                                ; 8b da
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov CL, strict byte 000h                  ; b1 00
    mov dx, 003c0h                            ; ba c0 03
    db  08ah, 0c1h
    ; mov al, cl                                ; 8a c1
    out DX, AL                                ; ee
    mov al, byte [es:bx]                      ; 26 8a 07
    out DX, AL                                ; ee
    inc bx                                    ; 43
    db  0feh, 0c1h
    ; inc cl                                    ; fe c1
    cmp cl, 010h                              ; 80 f9 10
    jne short 00246h                          ; 75 f1
    mov AL, strict byte 011h                  ; b0 11
    out DX, AL                                ; ee
    mov al, byte [es:bx]                      ; 26 8a 07
    out DX, AL                                ; ee
    mov AL, strict byte 020h                  ; b0 20
    out DX, AL                                ; ee
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop ax                                    ; 58
    retn                                      ; c3
    push ax                                   ; 50
    push bx                                   ; 53
    push dx                                   ; 52
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov dx, 003c0h                            ; ba c0 03
    mov AL, strict byte 010h                  ; b0 10
    out DX, AL                                ; ee
    mov dx, 003c1h                            ; ba c1 03
    in AL, DX                                 ; ec
    and AL, strict byte 0f7h                  ; 24 f7
    and bl, 001h                              ; 80 e3 01
    sal bl, 003h                              ; c0 e3 03
    db  00ah, 0c3h
    ; or al, bl                                 ; 0a c3
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    mov AL, strict byte 020h                  ; b0 20
    out DX, AL                                ; ee
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop ax                                    ; 58
    retn                                      ; c3
    cmp bl, 014h                              ; 80 fb 14
    jnbe short 002b9h                         ; 77 22
    push ax                                   ; 50
    push dx                                   ; 52
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov dx, 003c0h                            ; ba c0 03
    db  08ah, 0c3h
    ; mov al, bl                                ; 8a c3
    out DX, AL                                ; ee
    mov dx, 003c1h                            ; ba c1 03
    in AL, DX                                 ; ec
    db  08ah, 0f8h
    ; mov bh, al                                ; 8a f8
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov dx, 003c0h                            ; ba c0 03
    mov AL, strict byte 020h                  ; b0 20
    out DX, AL                                ; ee
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    pop dx                                    ; 5a
    pop ax                                    ; 58
    retn                                      ; c3
    push ax                                   ; 50
    push bx                                   ; 53
    mov BL, strict byte 011h                  ; b3 11
    call 00292h                               ; e8 d1 ff
    db  08ah, 0c7h
    ; mov al, bh                                ; 8a c7
    pop bx                                    ; 5b
    db  08ah, 0f8h
    ; mov bh, al                                ; 8a f8
    pop ax                                    ; 58
    retn                                      ; c3
    push ax                                   ; 50
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    db  08bh, 0dah
    ; mov bx, dx                                ; 8b da
    mov CL, strict byte 000h                  ; b1 00
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov dx, 003c0h                            ; ba c0 03
    db  08ah, 0c1h
    ; mov al, cl                                ; 8a c1
    out DX, AL                                ; ee
    mov dx, 003c1h                            ; ba c1 03
    in AL, DX                                 ; ec
    mov byte [es:bx], al                      ; 26 88 07
    inc bx                                    ; 43
    db  0feh, 0c1h
    ; inc cl                                    ; fe c1
    cmp cl, 010h                              ; 80 f9 10
    jne short 002d0h                          ; 75 e7
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov dx, 003c0h                            ; ba c0 03
    mov AL, strict byte 011h                  ; b0 11
    out DX, AL                                ; ee
    mov dx, 003c1h                            ; ba c1 03
    in AL, DX                                 ; ec
    mov byte [es:bx], al                      ; 26 88 07
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov dx, 003c0h                            ; ba c0 03
    mov AL, strict byte 020h                  ; b0 20
    out DX, AL                                ; ee
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop ax                                    ; 58
    retn                                      ; c3
    push ax                                   ; 50
    push dx                                   ; 52
    mov dx, 003c8h                            ; ba c8 03
    db  08ah, 0c3h
    ; mov al, bl                                ; 8a c3
    out DX, AL                                ; ee
    mov dx, 003c9h                            ; ba c9 03
    pop ax                                    ; 58
    push ax                                   ; 50
    db  08ah, 0c4h
    ; mov al, ah                                ; 8a c4
    out DX, AL                                ; ee
    db  08ah, 0c5h
    ; mov al, ch                                ; 8a c5
    out DX, AL                                ; ee
    db  08ah, 0c1h
    ; mov al, cl                                ; 8a c1
    out DX, AL                                ; ee
    pop dx                                    ; 5a
    pop ax                                    ; 58
    retn                                      ; c3
    push ax                                   ; 50
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    mov dx, 003c8h                            ; ba c8 03
    db  08ah, 0c3h
    ; mov al, bl                                ; 8a c3
    out DX, AL                                ; ee
    pop dx                                    ; 5a
    push dx                                   ; 52
    db  08bh, 0dah
    ; mov bx, dx                                ; 8b da
    mov dx, 003c9h                            ; ba c9 03
    mov al, byte [es:bx]                      ; 26 8a 07
    out DX, AL                                ; ee
    inc bx                                    ; 43
    mov al, byte [es:bx]                      ; 26 8a 07
    out DX, AL                                ; ee
    inc bx                                    ; 43
    mov al, byte [es:bx]                      ; 26 8a 07
    out DX, AL                                ; ee
    inc bx                                    ; 43
    dec cx                                    ; 49
    jne short 00337h                          ; 75 ee
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop ax                                    ; 58
    retn                                      ; c3
    push ax                                   ; 50
    push bx                                   ; 53
    push dx                                   ; 52
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov dx, 003c0h                            ; ba c0 03
    mov AL, strict byte 010h                  ; b0 10
    out DX, AL                                ; ee
    mov dx, 003c1h                            ; ba c1 03
    in AL, DX                                 ; ec
    and bl, 001h                              ; 80 e3 01
    jne short 00371h                          ; 75 0d
    and AL, strict byte 07fh                  ; 24 7f
    sal bh, 007h                              ; c0 e7 07
    db  00ah, 0c7h
    ; or al, bh                                 ; 0a c7
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    jmp short 0038ah                          ; eb 19
    push ax                                   ; 50
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov dx, 003c0h                            ; ba c0 03
    mov AL, strict byte 014h                  ; b0 14
    out DX, AL                                ; ee
    pop ax                                    ; 58
    and AL, strict byte 080h                  ; 24 80
    jne short 00384h                          ; 75 03
    sal bh, 002h                              ; c0 e7 02
    and bh, 00fh                              ; 80 e7 0f
    db  08ah, 0c7h
    ; mov al, bh                                ; 8a c7
    out DX, AL                                ; ee
    mov AL, strict byte 020h                  ; b0 20
    out DX, AL                                ; ee
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop ax                                    ; 58
    retn                                      ; c3
    push ax                                   ; 50
    push dx                                   ; 52
    mov dx, 003c7h                            ; ba c7 03
    db  08ah, 0c3h
    ; mov al, bl                                ; 8a c3
    out DX, AL                                ; ee
    pop ax                                    ; 58
    db  08ah, 0e0h
    ; mov ah, al                                ; 8a e0
    mov dx, 003c9h                            ; ba c9 03
    in AL, DX                                 ; ec
    xchg al, ah                               ; 86 e0
    push ax                                   ; 50
    in AL, DX                                 ; ec
    db  08ah, 0e8h
    ; mov ch, al                                ; 8a e8
    in AL, DX                                 ; ec
    db  08ah, 0c8h
    ; mov cl, al                                ; 8a c8
    pop dx                                    ; 5a
    pop ax                                    ; 58
    retn                                      ; c3
    push ax                                   ; 50
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    mov dx, 003c7h                            ; ba c7 03
    db  08ah, 0c3h
    ; mov al, bl                                ; 8a c3
    out DX, AL                                ; ee
    pop dx                                    ; 5a
    push dx                                   ; 52
    db  08bh, 0dah
    ; mov bx, dx                                ; 8b da
    mov dx, 003c9h                            ; ba c9 03
    in AL, DX                                 ; ec
    mov byte [es:bx], al                      ; 26 88 07
    inc bx                                    ; 43
    in AL, DX                                 ; ec
    mov byte [es:bx], al                      ; 26 88 07
    inc bx                                    ; 43
    in AL, DX                                 ; ec
    mov byte [es:bx], al                      ; 26 88 07
    inc bx                                    ; 43
    dec cx                                    ; 49
    jne short 003c1h                          ; 75 ee
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop ax                                    ; 58
    retn                                      ; c3
    push ax                                   ; 50
    push dx                                   ; 52
    mov dx, 003c6h                            ; ba c6 03
    db  08ah, 0c3h
    ; mov al, bl                                ; 8a c3
    out DX, AL                                ; ee
    pop dx                                    ; 5a
    pop ax                                    ; 58
    retn                                      ; c3
    push ax                                   ; 50
    push dx                                   ; 52
    mov dx, 003c6h                            ; ba c6 03
    in AL, DX                                 ; ec
    db  08ah, 0d8h
    ; mov bl, al                                ; 8a d8
    pop dx                                    ; 5a
    pop ax                                    ; 58
    retn                                      ; c3
    push ax                                   ; 50
    push dx                                   ; 52
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov dx, 003c0h                            ; ba c0 03
    mov AL, strict byte 010h                  ; b0 10
    out DX, AL                                ; ee
    mov dx, 003c1h                            ; ba c1 03
    in AL, DX                                 ; ec
    db  08ah, 0d8h
    ; mov bl, al                                ; 8a d8
    shr bl, 007h                              ; c0 eb 07
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov dx, 003c0h                            ; ba c0 03
    mov AL, strict byte 014h                  ; b0 14
    out DX, AL                                ; ee
    mov dx, 003c1h                            ; ba c1 03
    in AL, DX                                 ; ec
    db  08ah, 0f8h
    ; mov bh, al                                ; 8a f8
    and bh, 00fh                              ; 80 e7 0f
    test bl, 001h                             ; f6 c3 01
    jne short 0041eh                          ; 75 03
    shr bh, 002h                              ; c0 ef 02
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov dx, 003c0h                            ; ba c0 03
    mov AL, strict byte 020h                  ; b0 20
    out DX, AL                                ; ee
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    pop dx                                    ; 5a
    pop ax                                    ; 58
    retn                                      ; c3
    push ax                                   ; 50
    push dx                                   ; 52
    mov dx, 003c4h                            ; ba c4 03
    db  08ah, 0e3h
    ; mov ah, bl                                ; 8a e3
    mov AL, strict byte 003h                  ; b0 03
    out DX, ax                                ; ef
    pop dx                                    ; 5a
    pop ax                                    ; 58
    retn                                      ; c3
    push DS                                   ; 1e
    push ax                                   ; 50
    mov ax, strict word 00040h                ; b8 40 00
    mov ds, ax                                ; 8e d8
    db  032h, 0edh
    ; xor ch, ch                                ; 32 ed
    mov bx, 00088h                            ; bb 88 00
    mov cl, byte [bx]                         ; 8a 0f
    and cl, 00fh                              ; 80 e1 0f
    mov bx, strict word 00063h                ; bb 63 00
    mov ax, word [bx]                         ; 8b 07
    mov bx, strict word 00003h                ; bb 03 00
    cmp ax, 003b4h                            ; 3d b4 03
    jne short 0045ch                          ; 75 02
    mov BH, strict byte 001h                  ; b7 01
    pop ax                                    ; 58
    pop DS                                    ; 1f
    retn                                      ; c3
    push DS                                   ; 1e
    push bx                                   ; 53
    push dx                                   ; 52
    db  08ah, 0d0h
    ; mov dl, al                                ; 8a d0
    mov ax, strict word 00040h                ; b8 40 00
    mov ds, ax                                ; 8e d8
    mov bx, 00089h                            ; bb 89 00
    mov al, byte [bx]                         ; 8a 07
    mov bx, 00088h                            ; bb 88 00
    mov ah, byte [bx]                         ; 8a 27
    cmp dl, 001h                              ; 80 fa 01
    je short 0048dh                           ; 74 15
    jc short 00497h                           ; 72 1d
    cmp dl, 002h                              ; 80 fa 02
    je short 00481h                           ; 74 02
    jmp short 004abh                          ; eb 2a
    and AL, strict byte 07fh                  ; 24 7f
    or AL, strict byte 010h                   ; 0c 10
    and ah, 0f0h                              ; 80 e4 f0
    or ah, 009h                               ; 80 cc 09
    jne short 004a1h                          ; 75 14
    and AL, strict byte 06fh                  ; 24 6f
    and ah, 0f0h                              ; 80 e4 f0
    or ah, 009h                               ; 80 cc 09
    jne short 004a1h                          ; 75 0a
    and AL, strict byte 0efh                  ; 24 ef
    or AL, strict byte 080h                   ; 0c 80
    and ah, 0f0h                              ; 80 e4 f0
    or ah, 008h                               ; 80 cc 08
    mov bx, 00089h                            ; bb 89 00
    mov byte [bx], al                         ; 88 07
    mov bx, 00088h                            ; bb 88 00
    mov byte [bx], ah                         ; 88 27
    mov ax, 01212h                            ; b8 12 12
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop DS                                    ; 1f
    retn                                      ; c3
    push DS                                   ; 1e
    push bx                                   ; 53
    push dx                                   ; 52
    db  08ah, 0d0h
    ; mov dl, al                                ; 8a d0
    and dl, 001h                              ; 80 e2 01
    sal dl, 003h                              ; c0 e2 03
    mov ax, strict word 00040h                ; b8 40 00
    mov ds, ax                                ; 8e d8
    mov bx, 00089h                            ; bb 89 00
    mov al, byte [bx]                         ; 8a 07
    and AL, strict byte 0f7h                  ; 24 f7
    db  00ah, 0c2h
    ; or al, dl                                 ; 0a c2
    mov byte [bx], al                         ; 88 07
    mov ax, 01212h                            ; b8 12 12
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop DS                                    ; 1f
    retn                                      ; c3
    push bx                                   ; 53
    push dx                                   ; 52
    db  08ah, 0d8h
    ; mov bl, al                                ; 8a d8
    and bl, 001h                              ; 80 e3 01
    xor bl, 001h                              ; 80 f3 01
    sal bl, 1                                 ; d0 e3
    mov dx, 003cch                            ; ba cc 03
    in AL, DX                                 ; ec
    and AL, strict byte 0fdh                  ; 24 fd
    db  00ah, 0c3h
    ; or al, bl                                 ; 0a c3
    mov dx, 003c2h                            ; ba c2 03
    out DX, AL                                ; ee
    mov ax, 01212h                            ; b8 12 12
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    retn                                      ; c3
    push DS                                   ; 1e
    push bx                                   ; 53
    push dx                                   ; 52
    db  08ah, 0d0h
    ; mov dl, al                                ; 8a d0
    and dl, 001h                              ; 80 e2 01
    xor dl, 001h                              ; 80 f2 01
    sal dl, 1                                 ; d0 e2
    mov ax, strict word 00040h                ; b8 40 00
    mov ds, ax                                ; 8e d8
    mov bx, 00089h                            ; bb 89 00
    mov al, byte [bx]                         ; 8a 07
    and AL, strict byte 0fdh                  ; 24 fd
    db  00ah, 0c2h
    ; or al, dl                                 ; 0a c2
    mov byte [bx], al                         ; 88 07
    mov ax, 01212h                            ; b8 12 12
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop DS                                    ; 1f
    retn                                      ; c3
    push DS                                   ; 1e
    push bx                                   ; 53
    push dx                                   ; 52
    db  08ah, 0d0h
    ; mov dl, al                                ; 8a d0
    and dl, 001h                              ; 80 e2 01
    xor dl, 001h                              ; 80 f2 01
    mov ax, strict word 00040h                ; b8 40 00
    mov ds, ax                                ; 8e d8
    mov bx, 00089h                            ; bb 89 00
    mov al, byte [bx]                         ; 8a 07
    and AL, strict byte 0feh                  ; 24 fe
    db  00ah, 0c2h
    ; or al, dl                                 ; 0a c2
    mov byte [bx], al                         ; 88 07
    mov ax, 01212h                            ; b8 12 12
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop DS                                    ; 1f
    retn                                      ; c3
    cmp AL, strict byte 000h                  ; 3c 00
    je short 00541h                           ; 74 05
    cmp AL, strict byte 001h                  ; 3c 01
    je short 00556h                           ; 74 16
    retn                                      ; c3
    push DS                                   ; 1e
    push ax                                   ; 50
    mov ax, strict word 00040h                ; b8 40 00
    mov ds, ax                                ; 8e d8
    mov bx, 0008ah                            ; bb 8a 00
    mov al, byte [bx]                         ; 8a 07
    db  08ah, 0d8h
    ; mov bl, al                                ; 8a d8
    db  032h, 0ffh
    ; xor bh, bh                                ; 32 ff
    pop ax                                    ; 58
    db  08ah, 0c4h
    ; mov al, ah                                ; 8a c4
    pop DS                                    ; 1f
    retn                                      ; c3
    push DS                                   ; 1e
    push ax                                   ; 50
    push bx                                   ; 53
    mov ax, strict word 00040h                ; b8 40 00
    mov ds, ax                                ; 8e d8
    db  08bh, 0c3h
    ; mov ax, bx                                ; 8b c3
    mov bx, 0008ah                            ; bb 8a 00
    mov byte [bx], al                         ; 88 07
    pop bx                                    ; 5b
    pop ax                                    ; 58
    db  08ah, 0c4h
    ; mov al, ah                                ; 8a c4
    pop DS                                    ; 1f
    retn                                      ; c3
    times 0x5 db 0
do_out_dx_ax:                                ; 0xc0570 LB 0x7
    xchg ah, al                               ; 86 c4
    out DX, AL                                ; ee
    xchg ah, al                               ; 86 c4
    out DX, AL                                ; ee
    retn                                      ; c3
do_in_ax_dx:                                 ; 0xc0577 LB 0x40
    in AL, DX                                 ; ec
    xchg ah, al                               ; 86 c4
    in AL, DX                                 ; ec
    retn                                      ; c3
    push ax                                   ; 50
    push dx                                   ; 52
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    test AL, strict byte 008h                 ; a8 08
    je short 00581h                           ; 74 fb
    pop dx                                    ; 5a
    pop ax                                    ; 58
    retn                                      ; c3
    push ax                                   ; 50
    push dx                                   ; 52
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    test AL, strict byte 008h                 ; a8 08
    jne short 0058eh                          ; 75 fb
    pop dx                                    ; 5a
    pop ax                                    ; 58
    retn                                      ; c3
    push dx                                   ; 52
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00003h                ; b8 03 00
    call 00570h                               ; e8 d0 ff
    mov dx, 001cfh                            ; ba cf 01
    call 00577h                               ; e8 d1 ff
    cmp AL, strict byte 004h                  ; 3c 04
    jbe short 005b5h                          ; 76 0b
    db  08ah, 0e0h
    ; mov ah, al                                ; 8a e0
    shr ah, 003h                              ; c0 ec 03
    test AL, strict byte 007h                 ; a8 07
    je short 005b5h                           ; 74 02
    db  0feh, 0c4h
    ; inc ah                                    ; fe c4
    pop dx                                    ; 5a
    retn                                      ; c3
_dispi_get_max_bpp:                          ; 0xc05b7 LB 0x26
    push dx                                   ; 52
    push bx                                   ; 53
    call 005f1h                               ; e8 35 00
    db  08bh, 0d8h
    ; mov bx, ax                                ; 8b d8
    or ax, strict byte 00002h                 ; 83 c8 02
    call 005ddh                               ; e8 19 00
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00003h                ; b8 03 00
    call 00570h                               ; e8 a3 ff
    mov dx, 001cfh                            ; ba cf 01
    call 00577h                               ; e8 a4 ff
    push ax                                   ; 50
    db  08bh, 0c3h
    ; mov ax, bx                                ; 8b c3
    call 005ddh                               ; e8 04 00
    pop ax                                    ; 58
    pop bx                                    ; 5b
    pop dx                                    ; 5a
    retn                                      ; c3
dispi_set_enable_:                           ; 0xc05dd LB 0x26
    push dx                                   ; 52
    push ax                                   ; 50
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00004h                ; b8 04 00
    call 00570h                               ; e8 88 ff
    pop ax                                    ; 58
    mov dx, 001cfh                            ; ba cf 01
    call 00570h                               ; e8 81 ff
    pop dx                                    ; 5a
    retn                                      ; c3
    push dx                                   ; 52
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00004h                ; b8 04 00
    call 00570h                               ; e8 75 ff
    mov dx, 001cfh                            ; ba cf 01
    call 00577h                               ; e8 76 ff
    pop dx                                    ; 5a
    retn                                      ; c3
dispi_set_bank_:                             ; 0xc0603 LB 0x26
    push dx                                   ; 52
    push ax                                   ; 50
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00005h                ; b8 05 00
    call 00570h                               ; e8 62 ff
    pop ax                                    ; 58
    mov dx, 001cfh                            ; ba cf 01
    call 00570h                               ; e8 5b ff
    pop dx                                    ; 5a
    retn                                      ; c3
    push dx                                   ; 52
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00005h                ; b8 05 00
    call 00570h                               ; e8 4f ff
    mov dx, 001cfh                            ; ba cf 01
    call 00577h                               ; e8 50 ff
    pop dx                                    ; 5a
    retn                                      ; c3
_dispi_set_bank_farcall:                     ; 0xc0629 LB 0xe4
    cmp bx, 00100h                            ; 81 fb 00 01
    je short 00653h                           ; 74 24
    db  00bh, 0dbh
    ; or bx, bx                                 ; 0b db
    jne short 00665h                          ; 75 32
    db  08bh, 0c2h
    ; mov ax, dx                                ; 8b c2
    push dx                                   ; 52
    push ax                                   ; 50
    mov ax, strict word 00005h                ; b8 05 00
    mov dx, 001ceh                            ; ba ce 01
    call 00570h                               ; e8 30 ff
    pop ax                                    ; 58
    mov dx, 001cfh                            ; ba cf 01
    call 00570h                               ; e8 29 ff
    call 00577h                               ; e8 2d ff
    pop dx                                    ; 5a
    db  03bh, 0d0h
    ; cmp dx, ax                                ; 3b d0
    jne short 00665h                          ; 75 16
    mov ax, strict word 0004fh                ; b8 4f 00
    retf                                      ; cb
    mov ax, strict word 00005h                ; b8 05 00
    mov dx, 001ceh                            ; ba ce 01
    call 00570h                               ; e8 14 ff
    mov dx, 001cfh                            ; ba cf 01
    call 00577h                               ; e8 15 ff
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    retf                                      ; cb
    mov ax, 0014fh                            ; b8 4f 01
    retf                                      ; cb
    push dx                                   ; 52
    push ax                                   ; 50
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00008h                ; b8 08 00
    call 00570h                               ; e8 fc fe
    pop ax                                    ; 58
    mov dx, 001cfh                            ; ba cf 01
    call 00570h                               ; e8 f5 fe
    pop dx                                    ; 5a
    retn                                      ; c3
    push dx                                   ; 52
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00008h                ; b8 08 00
    call 00570h                               ; e8 e9 fe
    mov dx, 001cfh                            ; ba cf 01
    call 00577h                               ; e8 ea fe
    pop dx                                    ; 5a
    retn                                      ; c3
    push dx                                   ; 52
    push ax                                   ; 50
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00009h                ; b8 09 00
    call 00570h                               ; e8 d6 fe
    pop ax                                    ; 58
    mov dx, 001cfh                            ; ba cf 01
    call 00570h                               ; e8 cf fe
    pop dx                                    ; 5a
    retn                                      ; c3
    push dx                                   ; 52
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00009h                ; b8 09 00
    call 00570h                               ; e8 c3 fe
    mov dx, 001cfh                            ; ba cf 01
    call 00577h                               ; e8 c4 fe
    pop dx                                    ; 5a
    retn                                      ; c3
    push ax                                   ; 50
    push bx                                   ; 53
    push dx                                   ; 52
    db  08bh, 0d8h
    ; mov bx, ax                                ; 8b d8
    call 00596h                               ; e8 d9 fe
    cmp AL, strict byte 004h                  ; 3c 04
    jnbe short 006c3h                         ; 77 02
    shr bx, 1                                 ; d1 eb
    shr bx, 003h                              ; c1 eb 03
    mov dx, 003d4h                            ; ba d4 03
    db  08ah, 0e3h
    ; mov ah, bl                                ; 8a e3
    mov AL, strict byte 013h                  ; b0 13
    out DX, ax                                ; ef
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop ax                                    ; 58
    retn                                      ; c3
    call 006b5h                               ; e8 e0 ff
    push dx                                   ; 52
    push ax                                   ; 50
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00006h                ; b8 06 00
    call 00570h                               ; e8 90 fe
    pop ax                                    ; 58
    mov dx, 001cfh                            ; ba cf 01
    call 00570h                               ; e8 89 fe
    pop dx                                    ; 5a
    retn                                      ; c3
    push dx                                   ; 52
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00006h                ; b8 06 00
    call 00570h                               ; e8 7d fe
    mov dx, 001cfh                            ; ba cf 01
    call 00577h                               ; e8 7e fe
    pop dx                                    ; 5a
    retn                                      ; c3
    push dx                                   ; 52
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00007h                ; b8 07 00
    call 00570h                               ; e8 6b fe
    mov dx, 001cfh                            ; ba cf 01
    call 00577h                               ; e8 6c fe
    pop dx                                    ; 5a
    retn                                      ; c3
_vga_compat_setup:                           ; 0xc070d LB 0xed
    push ax                                   ; 50
    push dx                                   ; 52
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00001h                ; b8 01 00
    call 00570h                               ; e8 58 fe
    mov dx, 001cfh                            ; ba cf 01
    call 00577h                               ; e8 59 fe
    push ax                                   ; 50
    mov dx, 003d4h                            ; ba d4 03
    mov ax, strict word 00011h                ; b8 11 00
    out DX, ax                                ; ef
    pop ax                                    ; 58
    push ax                                   ; 50
    shr ax, 003h                              ; c1 e8 03
    dec ax                                    ; 48
    db  08ah, 0e0h
    ; mov ah, al                                ; 8a e0
    mov AL, strict byte 001h                  ; b0 01
    out DX, ax                                ; ef
    pop ax                                    ; 58
    call 006b5h                               ; e8 80 ff
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00002h                ; b8 02 00
    call 00570h                               ; e8 32 fe
    mov dx, 001cfh                            ; ba cf 01
    call 00577h                               ; e8 33 fe
    dec ax                                    ; 48
    push ax                                   ; 50
    mov dx, 003d4h                            ; ba d4 03
    db  08ah, 0e0h
    ; mov ah, al                                ; 8a e0
    mov AL, strict byte 012h                  ; b0 12
    out DX, ax                                ; ef
    pop ax                                    ; 58
    mov AL, strict byte 007h                  ; b0 07
    out DX, AL                                ; ee
    inc dx                                    ; 42
    in AL, DX                                 ; ec
    and AL, strict byte 0bdh                  ; 24 bd
    test ah, 001h                             ; f6 c4 01
    je short 0075dh                           ; 74 02
    or AL, strict byte 002h                   ; 0c 02
    test ah, 002h                             ; f6 c4 02
    je short 00764h                           ; 74 02
    or AL, strict byte 040h                   ; 0c 40
    out DX, AL                                ; ee
    mov dx, 003d4h                            ; ba d4 03
    mov ax, strict word 00009h                ; b8 09 00
    out DX, AL                                ; ee
    mov dx, 003d5h                            ; ba d5 03
    in AL, DX                                 ; ec
    and AL, strict byte 060h                  ; 24 60
    out DX, AL                                ; ee
    mov dx, 003d4h                            ; ba d4 03
    mov AL, strict byte 017h                  ; b0 17
    out DX, AL                                ; ee
    mov dx, 003d5h                            ; ba d5 03
    in AL, DX                                 ; ec
    or AL, strict byte 003h                   ; 0c 03
    out DX, AL                                ; ee
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov dx, 003c0h                            ; ba c0 03
    mov AL, strict byte 010h                  ; b0 10
    out DX, AL                                ; ee
    mov dx, 003c1h                            ; ba c1 03
    in AL, DX                                 ; ec
    or AL, strict byte 001h                   ; 0c 01
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    mov AL, strict byte 020h                  ; b0 20
    out DX, AL                                ; ee
    mov dx, 003ceh                            ; ba ce 03
    mov ax, 00506h                            ; b8 06 05
    out DX, ax                                ; ef
    mov dx, 003c4h                            ; ba c4 03
    mov ax, 00f02h                            ; b8 02 0f
    out DX, ax                                ; ef
    mov dx, 001ceh                            ; ba ce 01
    mov ax, strict word 00003h                ; b8 03 00
    call 00570h                               ; e8 c2 fd
    mov dx, 001cfh                            ; ba cf 01
    call 00577h                               ; e8 c3 fd
    cmp AL, strict byte 008h                  ; 3c 08
    jc short 007f8h                           ; 72 40
    mov dx, 003d4h                            ; ba d4 03
    mov AL, strict byte 014h                  ; b0 14
    out DX, AL                                ; ee
    mov dx, 003d5h                            ; ba d5 03
    in AL, DX                                 ; ec
    or AL, strict byte 040h                   ; 0c 40
    out DX, AL                                ; ee
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    mov dx, 003c0h                            ; ba c0 03
    mov AL, strict byte 010h                  ; b0 10
    out DX, AL                                ; ee
    mov dx, 003c1h                            ; ba c1 03
    in AL, DX                                 ; ec
    or AL, strict byte 040h                   ; 0c 40
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    mov AL, strict byte 020h                  ; b0 20
    out DX, AL                                ; ee
    mov dx, 003c4h                            ; ba c4 03
    mov AL, strict byte 004h                  ; b0 04
    out DX, AL                                ; ee
    mov dx, 003c5h                            ; ba c5 03
    in AL, DX                                 ; ec
    or AL, strict byte 008h                   ; 0c 08
    out DX, AL                                ; ee
    mov dx, 003ceh                            ; ba ce 03
    mov AL, strict byte 005h                  ; b0 05
    out DX, AL                                ; ee
    mov dx, 003cfh                            ; ba cf 03
    in AL, DX                                 ; ec
    and AL, strict byte 09fh                  ; 24 9f
    or AL, strict byte 040h                   ; 0c 40
    out DX, AL                                ; ee
    pop dx                                    ; 5a
    pop ax                                    ; 58
_vbe_has_vbe_display:                        ; 0xc07fa LB 0x13
    push DS                                   ; 1e
    push bx                                   ; 53
    mov ax, strict word 00040h                ; b8 40 00
    mov ds, ax                                ; 8e d8
    mov bx, 000b9h                            ; bb b9 00
    mov al, byte [bx]                         ; 8a 07
    and AL, strict byte 001h                  ; 24 01
    db  032h, 0e4h
    ; xor ah, ah                                ; 32 e4
    pop bx                                    ; 5b
    pop DS                                    ; 1f
    retn                                      ; c3
vbe_biosfn_return_current_mode:              ; 0xc080d LB 0x25
    push DS                                   ; 1e
    mov ax, strict word 00040h                ; b8 40 00
    mov ds, ax                                ; 8e d8
    call 005f1h                               ; e8 db fd
    and ax, strict byte 00001h                ; 83 e0 01
    je short 00824h                           ; 74 09
    mov bx, 000bah                            ; bb ba 00
    mov ax, word [bx]                         ; 8b 07
    db  08bh, 0d8h
    ; mov bx, ax                                ; 8b d8
    jne short 0082dh                          ; 75 09
    mov bx, strict word 00049h                ; bb 49 00
    mov al, byte [bx]                         ; 8a 07
    db  08ah, 0d8h
    ; mov bl, al                                ; 8a d8
    db  032h, 0ffh
    ; xor bh, bh                                ; 32 ff
    mov ax, strict word 0004fh                ; b8 4f 00
    pop DS                                    ; 1f
    retn                                      ; c3
vbe_biosfn_display_window_control:           ; 0xc0832 LB 0x2d
    cmp bl, 000h                              ; 80 fb 00
    jne short 0085bh                          ; 75 24
    cmp bh, 001h                              ; 80 ff 01
    je short 00852h                           ; 74 16
    jc short 00842h                           ; 72 04
    mov ax, 00100h                            ; b8 00 01
    retn                                      ; c3
    db  08bh, 0c2h
    ; mov ax, dx                                ; 8b c2
    call 00603h                               ; e8 bc fd
    call 00617h                               ; e8 cd fd
    db  03bh, 0c2h
    ; cmp ax, dx                                ; 3b c2
    jne short 0085bh                          ; 75 0d
    mov ax, strict word 0004fh                ; b8 4f 00
    retn                                      ; c3
    call 00617h                               ; e8 c2 fd
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    mov ax, strict word 0004fh                ; b8 4f 00
    retn                                      ; c3
    mov ax, 0014fh                            ; b8 4f 01
    retn                                      ; c3
vbe_biosfn_set_get_logical_scan_line_length: ; 0xc085f LB 0x4d
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    cmp bl, 001h                              ; 80 fb 01
    je short 0088ah                           ; 74 24
    cmp bl, 002h                              ; 80 fb 02
    je short 00871h                           ; 74 06
    jc short 00887h                           ; 72 1a
    mov ax, 00100h                            ; b8 00 01
    retn                                      ; c3
    push ax                                   ; 50
    call 00596h                               ; e8 21 fd
    db  032h, 0ffh
    ; xor bh, bh                                ; 32 ff
    db  08ah, 0dch
    ; mov bl, ah                                ; 8a dc
    db  00ah, 0dbh
    ; or bl, bl                                 ; 0a db
    jne short 00882h                          ; 75 05
    sal ax, 003h                              ; c1 e0 03
    mov BL, strict byte 001h                  ; b3 01
    db  033h, 0d2h
    ; xor dx, dx                                ; 33 d2
    pop ax                                    ; 58
    div bx                                    ; f7 f3
    call 006d2h                               ; e8 48 fe
    call 00596h                               ; e8 09 fd
    db  032h, 0ffh
    ; xor bh, bh                                ; 32 ff
    db  08ah, 0dch
    ; mov bl, ah                                ; 8a dc
    call 006e9h                               ; e8 55 fe
    db  08bh, 0c8h
    ; mov cx, ax                                ; 8b c8
    db  00ah, 0dbh
    ; or bl, bl                                 ; 0a db
    jne short 0089fh                          ; 75 05
    shr ax, 003h                              ; c1 e8 03
    mov BL, strict byte 001h                  ; b3 01
    mul bx                                    ; f7 e3
    db  08bh, 0d8h
    ; mov bx, ax                                ; 8b d8
    call 006fbh                               ; e8 55 fe
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    mov ax, strict word 0004fh                ; b8 4f 00
    retn                                      ; c3
vbe_biosfn_set_get_display_start:            ; 0xc08ac LB 0x34
    cmp bl, 080h                              ; 80 fb 80
    je short 008bch                           ; 74 0b
    cmp bl, 001h                              ; 80 fb 01
    je short 008d0h                           ; 74 1a
    jc short 008c2h                           ; 72 0a
    mov ax, 00100h                            ; b8 00 01
    retn                                      ; c3
    call 00589h                               ; e8 ca fc
    call 0057ch                               ; e8 ba fc
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    call 00669h                               ; e8 a2 fd
    db  08bh, 0c2h
    ; mov ax, dx                                ; 8b c2
    call 0068fh                               ; e8 c3 fd
    mov ax, strict word 0004fh                ; b8 4f 00
    retn                                      ; c3
    call 0067dh                               ; e8 aa fd
    db  08bh, 0c8h
    ; mov cx, ax                                ; 8b c8
    call 006a3h                               ; e8 cb fd
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    db  032h, 0ffh
    ; xor bh, bh                                ; 32 ff
    mov ax, strict word 0004fh                ; b8 4f 00
    retn                                      ; c3
vbe_biosfn_set_get_dac_palette_format:       ; 0xc08e0 LB 0x37
    cmp bl, 001h                              ; 80 fb 01
    je short 00903h                           ; 74 1e
    jc short 008ebh                           ; 72 04
    mov ax, 00100h                            ; b8 00 01
    retn                                      ; c3
    call 005f1h                               ; e8 03 fd
    cmp bh, 006h                              ; 80 ff 06
    je short 008fdh                           ; 74 0a
    cmp bh, 008h                              ; 80 ff 08
    jne short 00913h                          ; 75 1b
    or ax, strict byte 00020h                 ; 83 c8 20
    jne short 00900h                          ; 75 03
    and ax, strict byte 0ffdfh                ; 83 e0 df
    call 005ddh                               ; e8 da fc
    mov BH, strict byte 006h                  ; b7 06
    call 005f1h                               ; e8 e9 fc
    and ax, strict byte 00020h                ; 83 e0 20
    je short 0090fh                           ; 74 02
    mov BH, strict byte 008h                  ; b7 08
    mov ax, strict word 0004fh                ; b8 4f 00
    retn                                      ; c3
    mov ax, 0014fh                            ; b8 4f 01
    retn                                      ; c3
vbe_biosfn_set_get_palette_data:             ; 0xc0917 LB 0x64
    test bl, bl                               ; 84 db
    je short 0092ah                           ; 74 0f
    cmp bl, 001h                              ; 80 fb 01
    je short 00952h                           ; 74 32
    cmp bl, 003h                              ; 80 fb 03
    jbe short 00977h                          ; 76 52
    cmp bl, 080h                              ; 80 fb 80
    jne short 00973h                          ; 75 49
    pushad                                    ; 66 60
    push DS                                   ; 1e
    push ES                                   ; 06
    pop DS                                    ; 1f
    db  08ah, 0c2h
    ; mov al, dl                                ; 8a c2
    mov dx, 003c8h                            ; ba c8 03
    out DX, AL                                ; ee
    inc dx                                    ; 42
    db  08bh, 0f7h
    ; mov si, di                                ; 8b f7
    lodsd                                     ; 66 ad
    ror eax, 010h                             ; 66 c1 c8 10
    out DX, AL                                ; ee
    rol eax, 008h                             ; 66 c1 c0 08
    out DX, AL                                ; ee
    rol eax, 008h                             ; 66 c1 c0 08
    out DX, AL                                ; ee
    loop 00938h                               ; e2 ed
    pop DS                                    ; 1f
    popad                                     ; 66 61
    mov ax, strict word 0004fh                ; b8 4f 00
    retn                                      ; c3
    pushad                                    ; 66 60
    db  08ah, 0c2h
    ; mov al, dl                                ; 8a c2
    mov dx, 003c7h                            ; ba c7 03
    out DX, AL                                ; ee
    add dl, 002h                              ; 80 c2 02
    db  066h, 033h, 0c0h
    ; xor eax, eax                              ; 66 33 c0
    in AL, DX                                 ; ec
    sal eax, 008h                             ; 66 c1 e0 08
    in AL, DX                                 ; ec
    sal eax, 008h                             ; 66 c1 e0 08
    in AL, DX                                 ; ec
    stosd                                     ; 66 ab
    loop 0095dh                               ; e2 ee
    popad                                     ; 66 61
    jmp short 0094eh                          ; eb db
    mov ax, 0014fh                            ; b8 4f 01
    retn                                      ; c3
    mov ax, 0024fh                            ; b8 4f 02
    retn                                      ; c3
vbe_biosfn_return_protected_mode_interface: ; 0xc097b LB 0x17
    test bl, bl                               ; 84 db
    jne short 0098eh                          ; 75 0f
    mov di, 0c000h                            ; bf 00 c0
    mov es, di                                ; 8e c7
    mov di, 04400h                            ; bf 00 44
    mov cx, 00115h                            ; b9 15 01
    mov ax, strict word 0004fh                ; b8 4f 00
    retn                                      ; c3
    mov ax, 0014fh                            ; b8 4f 01
    retn                                      ; c3

  ; Padding 0x6e bytes at 0xc0992
  times 110 db 0

section _TEXT progbits vstart=0xa00 align=1 ; size=0x2f57 class=CODE group=AUTO
set_int_vector_:                             ; 0xc0a00 LB 0x1a
    push bx                                   ; 53
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    movzx bx, al                              ; 0f b6 d8
    sal bx, 002h                              ; c1 e3 02
    xor ax, ax                                ; 31 c0
    mov es, ax                                ; 8e c0
    mov word [es:bx], dx                      ; 26 89 17
    mov word [es:bx+002h], 0c000h             ; 26 c7 47 02 00 c0
    pop bp                                    ; 5d
    pop bx                                    ; 5b
    retn                                      ; c3
init_vga_card_:                              ; 0xc0a1a LB 0x1c
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push dx                                   ; 52
    mov AL, strict byte 0c3h                  ; b0 c3
    mov dx, 003c2h                            ; ba c2 03
    out DX, AL                                ; ee
    mov AL, strict byte 004h                  ; b0 04
    mov dx, 003c4h                            ; ba c4 03
    out DX, AL                                ; ee
    mov AL, strict byte 002h                  ; b0 02
    mov dx, 003c5h                            ; ba c5 03
    out DX, AL                                ; ee
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop dx                                    ; 5a
    pop bp                                    ; 5d
    retn                                      ; c3
init_bios_area_:                             ; 0xc0a36 LB 0x32
    push bx                                   ; 53
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    xor bx, bx                                ; 31 db
    mov ax, strict word 00040h                ; b8 40 00
    mov es, ax                                ; 8e c0
    mov al, byte [es:bx+010h]                 ; 26 8a 47 10
    and AL, strict byte 0cfh                  ; 24 cf
    or AL, strict byte 020h                   ; 0c 20
    mov byte [es:bx+010h], al                 ; 26 88 47 10
    mov byte [es:bx+00085h], 010h             ; 26 c6 87 85 00 10
    mov word [es:bx+00087h], 0f960h           ; 26 c7 87 87 00 60 f9
    mov byte [es:bx+00089h], 051h             ; 26 c6 87 89 00 51
    mov byte [es:bx+065h], 009h               ; 26 c6 47 65 09
    pop bp                                    ; 5d
    pop bx                                    ; 5b
    retn                                      ; c3
_vgabios_init_func:                          ; 0xc0a68 LB 0x20
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    call 00a1ah                               ; e8 ac ff
    call 00a36h                               ; e8 c5 ff
    call 03482h                               ; e8 0e 2a
    mov dx, strict word 00022h                ; ba 22 00
    mov ax, strict word 00010h                ; b8 10 00
    call 00a00h                               ; e8 83 ff
    mov ax, strict word 00003h                ; b8 03 00
    db  032h, 0e4h
    ; xor ah, ah                                ; 32 e4
    int 010h                                  ; cd 10
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retf                                      ; cb
vga_get_cursor_pos_:                         ; 0xc0a88 LB 0x43
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push cx                                   ; 51
    push si                                   ; 56
    mov cl, al                                ; 88 c1
    mov si, dx                                ; 89 d6
    cmp AL, strict byte 007h                  ; 3c 07
    jbe short 00aa3h                          ; 76 0e
    push SS                                   ; 16
    pop ES                                    ; 07
    mov word [es:si], strict word 00000h      ; 26 c7 04 00 00
    mov word [es:bx], strict word 00000h      ; 26 c7 07 00 00
    jmp short 00ac4h                          ; eb 21
    mov dx, strict word 00060h                ; ba 60 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 ae 24
    push SS                                   ; 16
    pop ES                                    ; 07
    mov word [es:si], ax                      ; 26 89 04
    movzx dx, cl                              ; 0f b6 d1
    add dx, dx                                ; 01 d2
    add dx, strict byte 00050h                ; 83 c2 50
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 9b 24
    push SS                                   ; 16
    pop ES                                    ; 07
    mov word [es:bx], ax                      ; 26 89 07
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bp                                    ; 5d
    retn                                      ; c3
vga_read_char_attr_:                         ; 0xc0acb LB 0xa8
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00008h                ; 83 ec 08
    mov cl, al                                ; 88 c1
    mov si, dx                                ; 89 d6
    mov dx, strict word 00049h                ; ba 49 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 5c 24
    xor ah, ah                                ; 30 e4
    call 02f17h                               ; e8 30 24
    mov ch, al                                ; 88 c5
    cmp AL, strict byte 0ffh                  ; 3c ff
    je short 00b5ah                           ; 74 6d
    movzx ax, cl                              ; 0f b6 c1
    lea bx, [bp-010h]                         ; 8d 5e f0
    lea dx, [bp-00eh]                         ; 8d 56 f2
    call 00a88h                               ; e8 8f ff
    mov al, byte [bp-010h]                    ; 8a 46 f0
    mov byte [bp-00ah], al                    ; 88 46 f6
    mov ax, word [bp-010h]                    ; 8b 46 f0
    xor al, al                                ; 30 c0
    shr ax, 008h                              ; c1 e8 08
    mov word [bp-00ch], ax                    ; 89 46 f4
    mov dx, 00084h                            ; ba 84 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 2b 24
    movzx di, al                              ; 0f b6 f8
    inc di                                    ; 47
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 3a 24
    movzx bx, ch                              ; 0f b6 dd
    sal bx, 003h                              ; c1 e3 03
    cmp byte [bx+0462fh], 000h                ; 80 bf 2f 46 00
    jne short 00b5ah                          ; 75 2d
    mov dx, ax                                ; 89 c2
    imul dx, di                               ; 0f af d7
    add dx, dx                                ; 01 d2
    or dl, 0ffh                               ; 80 ca ff
    xor ch, ch                                ; 30 ed
    inc dx                                    ; 42
    imul cx, dx                               ; 0f af ca
    movzx dx, byte [bp-00ch]                  ; 0f b6 56 f4
    imul dx, ax                               ; 0f af d0
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    add ax, dx                                ; 01 d0
    add ax, ax                                ; 01 c0
    mov dx, cx                                ; 89 ca
    add dx, ax                                ; 01 c2
    mov ax, word [bx+04632h]                  ; 8b 87 32 46
    call 02f5ah                               ; e8 03 24
    mov word [ss:si], ax                      ; 36 89 04
    lea sp, [bp-008h]                         ; 8d 66 f8
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
    mov cs, [bp+di]                           ; 8e 0b
    int 00bh                                  ; cd 0b
    ror byte [bp+di], CL                      ; d2 0b
    fimul dword [bp+di]                       ; da 0b
    db  0dfh
    db  00bh, 0e4h
    ; or sp, sp                                 ; 0b e4
    db  00bh, 0e9h
    ; or bp, cx                                 ; 0b e9
    db  00bh, 0eeh
    ; or bp, si                                 ; 0b ee
    db  00bh
vga_get_font_info_:                          ; 0xc0b73 LB 0x82
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    mov si, dx                                ; 89 d6
    mov word [bp-006h], bx                    ; 89 5e fa
    mov bx, cx                                ; 89 cb
    cmp ax, strict word 00007h                ; 3d 07 00
    jnbe short 00bc4h                         ; 77 3f
    mov di, ax                                ; 89 c7
    add di, ax                                ; 01 c7
    jmp word [cs:di+00b63h]                   ; 2e ff a5 63 0b
    mov dx, strict word 0007ch                ; ba 7c 00
    xor ax, ax                                ; 31 c0
    call 02f76h                               ; e8 e0 23
    push SS                                   ; 16
    pop ES                                    ; 07
    mov di, word [bp-006h]                    ; 8b 7e fa
    mov word [es:di], ax                      ; 26 89 05
    mov word [es:si], dx                      ; 26 89 14
    mov dx, 00085h                            ; ba 85 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 94 23
    xor ah, ah                                ; 30 e4
    push SS                                   ; 16
    pop ES                                    ; 07
    mov word [es:bx], ax                      ; 26 89 07
    mov dx, 00084h                            ; ba 84 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 84 23
    xor ah, ah                                ; 30 e4
    push SS                                   ; 16
    pop ES                                    ; 07
    mov bx, word [bp+004h]                    ; 8b 5e 04
    mov word [es:bx], ax                      ; 26 89 07
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00002h                               ; c2 02 00
    mov dx, 0010ch                            ; ba 0c 01
    jmp short 00b91h                          ; eb bf
    mov ax, 05bech                            ; b8 ec 5b
    mov dx, 0c000h                            ; ba 00 c0
    jmp short 00b96h                          ; eb bc
    mov ax, 053ech                            ; b8 ec 53
    jmp short 00bd5h                          ; eb f6
    mov ax, 057ech                            ; b8 ec 57
    jmp short 00bd5h                          ; eb f1
    mov ax, 079ech                            ; b8 ec 79
    jmp short 00bd5h                          ; eb ec
    mov ax, 069ech                            ; b8 ec 69
    jmp short 00bd5h                          ; eb e7
    mov ax, 07b19h                            ; b8 19 7b
    jmp short 00bd5h                          ; eb e2
    jmp short 00bc4h                          ; eb cf
vga_read_pixel_:                             ; 0xc0bf5 LB 0x139
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00006h                ; 83 ec 06
    mov si, dx                                ; 89 d6
    mov word [bp-00ah], bx                    ; 89 5e f6
    mov di, cx                                ; 89 cf
    mov dx, strict word 00049h                ; ba 49 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 31 23
    xor ah, ah                                ; 30 e4
    call 02f17h                               ; e8 05 23
    mov cl, al                                ; 88 c1
    cmp AL, strict byte 0ffh                  ; 3c ff
    je near 00d27h                            ; 0f 84 0d 01
    movzx bx, al                              ; 0f b6 d8
    sal bx, 003h                              ; c1 e3 03
    cmp byte [bx+0462fh], 000h                ; 80 bf 2f 46 00
    je near 00d27h                            ; 0f 84 fe 00
    mov bl, byte [bx+04630h]                  ; 8a 9f 30 46
    cmp bl, 003h                              ; 80 fb 03
    jc short 00c43h                           ; 72 11
    jbe short 00c4bh                          ; 76 17
    cmp bl, 005h                              ; 80 fb 05
    je near 00d04h                            ; 0f 84 c9 00
    cmp bl, 004h                              ; 80 fb 04
    je short 00c4bh                           ; 74 0b
    jmp near 00d22h                           ; e9 df 00
    cmp bl, 002h                              ; 80 fb 02
    je short 00ca3h                           ; 74 5b
    jmp near 00d22h                           ; e9 d7 00
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 06 23
    imul ax, word [bp-00ah]                   ; 0f af 46 f6
    mov bx, si                                ; 89 f3
    shr bx, 003h                              ; c1 eb 03
    add bx, ax                                ; 01 c3
    mov cx, si                                ; 89 f1
    and cx, strict byte 00007h                ; 83 e1 07
    mov ax, 00080h                            ; b8 80 00
    sar ax, CL                                ; d3 f8
    mov byte [bp-008h], al                    ; 88 46 f8
    mov byte [bp-006h], ch                    ; 88 6e fa
    jmp short 00c79h                          ; eb 08
    cmp byte [bp-006h], 004h                  ; 80 7e fa 04
    jnc near 00d24h                           ; 0f 83 ab 00
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    sal ax, 008h                              ; c1 e0 08
    or AL, strict byte 004h                   ; 0c 04
    mov dx, 003ceh                            ; ba ce 03
    out DX, ax                                ; ef
    mov dx, bx                                ; 89 da
    mov ax, 0a000h                            ; b8 00 a0
    call 02f3eh                               ; e8 b0 22
    and al, byte [bp-008h]                    ; 22 46 f8
    test al, al                               ; 84 c0
    jbe short 00c9eh                          ; 76 09
    mov cl, byte [bp-006h]                    ; 8a 4e fa
    mov AL, strict byte 001h                  ; b0 01
    sal al, CL                                ; d2 e0
    or ch, al                                 ; 08 c5
    inc byte [bp-006h]                        ; fe 46 fa
    jmp short 00c71h                          ; eb ce
    mov ax, word [bp-00ah]                    ; 8b 46 f6
    shr ax, 1                                 ; d1 e8
    imul ax, ax, strict byte 00050h           ; 6b c0 50
    mov bx, si                                ; 89 f3
    shr bx, 002h                              ; c1 eb 02
    add bx, ax                                ; 01 c3
    test byte [bp-00ah], 001h                 ; f6 46 f6 01
    je short 00cbbh                           ; 74 03
    add bh, 020h                              ; 80 c7 20
    mov dx, bx                                ; 89 da
    mov ax, 0b800h                            ; b8 00 b8
    call 02f3eh                               ; e8 7b 22
    movzx bx, cl                              ; 0f b6 d9
    sal bx, 003h                              ; c1 e3 03
    cmp byte [bx+04631h], 002h                ; 80 bf 31 46 02
    jne short 00cebh                          ; 75 1b
    mov cx, si                                ; 89 f1
    xor ch, ch                                ; 30 ed
    and cl, 003h                              ; 80 e1 03
    mov bx, strict word 00003h                ; bb 03 00
    sub bx, cx                                ; 29 cb
    mov cx, bx                                ; 89 d9
    add cx, bx                                ; 01 d9
    xor ah, ah                                ; 30 e4
    sar ax, CL                                ; d3 f8
    mov ch, al                                ; 88 c5
    and ch, 003h                              ; 80 e5 03
    jmp short 00d24h                          ; eb 39
    mov cx, si                                ; 89 f1
    xor ch, ch                                ; 30 ed
    and cl, 007h                              ; 80 e1 07
    mov bx, strict word 00007h                ; bb 07 00
    sub bx, cx                                ; 29 cb
    mov cx, bx                                ; 89 d9
    xor ah, ah                                ; 30 e4
    sar ax, CL                                ; d3 f8
    mov ch, al                                ; 88 c5
    and ch, 001h                              ; 80 e5 01
    jmp short 00d24h                          ; eb 20
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 4d 22
    sal ax, 003h                              ; c1 e0 03
    imul ax, word [bp-00ah]                   ; 0f af 46 f6
    mov dx, si                                ; 89 f2
    add dx, ax                                ; 01 c2
    mov ax, 0a000h                            ; b8 00 a0
    call 02f3eh                               ; e8 20 22
    mov ch, al                                ; 88 c5
    jmp short 00d24h                          ; eb 02
    xor ch, ch                                ; 30 ed
    mov byte [ss:di], ch                      ; 36 88 2d
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_perform_gray_scale_summing_:          ; 0xc0d2e LB 0x8c
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov bx, ax                                ; 89 c3
    mov di, dx                                ; 89 d7
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    xor al, al                                ; 30 c0
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    xor si, si                                ; 31 f6
    cmp si, di                                ; 39 fe
    jnc short 00d9fh                          ; 73 52
    mov al, bl                                ; 88 d8
    mov dx, 003c7h                            ; ba c7 03
    out DX, AL                                ; ee
    mov dx, 003c9h                            ; ba c9 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov cx, ax                                ; 89 c1
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov word [bp-00ch], ax                    ; 89 46 f4
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    xor ch, ch                                ; 30 ed
    imul cx, cx, strict byte 0004dh           ; 6b c9 4d
    mov word [bp-00ah], cx                    ; 89 4e f6
    movzx cx, byte [bp-00ch]                  ; 0f b6 4e f4
    imul cx, cx, 00097h                       ; 69 c9 97 00
    add cx, word [bp-00ah]                    ; 03 4e f6
    xor ah, ah                                ; 30 e4
    imul ax, ax, strict byte 0001ch           ; 6b c0 1c
    add cx, ax                                ; 01 c1
    add cx, 00080h                            ; 81 c1 80 00
    sar cx, 008h                              ; c1 f9 08
    cmp cx, strict byte 0003fh                ; 83 f9 3f
    jbe short 00d8dh                          ; 76 03
    mov cx, strict word 0003fh                ; b9 3f 00
    mov al, bl                                ; 88 d8
    mov dx, 003c8h                            ; ba c8 03
    out DX, AL                                ; ee
    mov al, cl                                ; 88 c8
    mov dx, 003c9h                            ; ba c9 03
    out DX, AL                                ; ee
    out DX, AL                                ; ee
    out DX, AL                                ; ee
    inc bx                                    ; 43
    inc si                                    ; 46
    jmp short 00d49h                          ; eb aa
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov AL, strict byte 020h                  ; b0 20
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    lea sp, [bp-008h]                         ; 8d 66 f8
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_set_cursor_shape_:                    ; 0xc0dba LB 0xa4
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    mov ch, al                                ; 88 c5
    mov cl, dl                                ; 88 d1
    and ch, 03fh                              ; 80 e5 3f
    and cl, 01fh                              ; 80 e1 1f
    movzx di, ch                              ; 0f b6 fd
    mov bx, di                                ; 89 fb
    sal bx, 008h                              ; c1 e3 08
    movzx si, cl                              ; 0f b6 f1
    add bx, si                                ; 01 f3
    mov dx, strict word 00060h                ; ba 60 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 87 21
    mov dx, 00089h                            ; ba 89 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 54 21
    mov bl, al                                ; 88 c3
    mov dx, 00085h                            ; ba 85 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 65 21
    mov dx, ax                                ; 89 c2
    test bl, 001h                             ; f6 c3 01
    je short 00e33h                           ; 74 37
    cmp ax, strict word 00008h                ; 3d 08 00
    jbe short 00e33h                          ; 76 32
    cmp cl, 008h                              ; 80 f9 08
    jnc short 00e33h                          ; 73 2d
    cmp ch, 020h                              ; 80 fd 20
    jnc short 00e33h                          ; 73 28
    inc di                                    ; 47
    cmp si, di                                ; 39 fe
    je short 00e19h                           ; 74 09
    imul ax, di                               ; 0f af c7
    shr ax, 003h                              ; c1 e8 03
    dec ax                                    ; 48
    jmp short 00e24h                          ; eb 0b
    lea si, [di+001h]                         ; 8d 75 01
    imul ax, si                               ; 0f af c6
    shr ax, 003h                              ; c1 e8 03
    dec ax                                    ; 48
    dec ax                                    ; 48
    mov ch, al                                ; 88 c5
    movzx ax, cl                              ; 0f b6 c1
    inc ax                                    ; 40
    imul ax, dx                               ; 0f af c2
    shr ax, 003h                              ; c1 e8 03
    dec ax                                    ; 48
    mov cl, al                                ; 88 c1
    mov dx, strict word 00063h                ; ba 63 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 1e 21
    mov bx, ax                                ; 89 c3
    mov AL, strict byte 00ah                  ; b0 0a
    mov dx, bx                                ; 89 da
    out DX, AL                                ; ee
    lea si, [bx+001h]                         ; 8d 77 01
    mov al, ch                                ; 88 e8
    mov dx, si                                ; 89 f2
    out DX, AL                                ; ee
    mov AL, strict byte 00bh                  ; b0 0b
    mov dx, bx                                ; 89 da
    out DX, AL                                ; ee
    mov al, cl                                ; 88 c8
    mov dx, si                                ; 89 f2
    out DX, AL                                ; ee
    lea sp, [bp-008h]                         ; 8d 66 f8
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_set_cursor_pos_:                      ; 0xc0e5e LB 0xa2
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push si                                   ; 56
    push ax                                   ; 50
    push ax                                   ; 50
    mov byte [bp-008h], al                    ; 88 46 f8
    mov cx, dx                                ; 89 d1
    cmp AL, strict byte 007h                  ; 3c 07
    jnbe near 00ef8h                          ; 0f 87 87 00
    movzx dx, al                              ; 0f b6 d0
    add dx, dx                                ; 01 d2
    add dx, strict byte 00050h                ; 83 c2 50
    mov bx, cx                                ; 89 cb
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 e7 20
    mov dx, strict word 00062h                ; ba 62 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 b4 20
    cmp al, byte [bp-008h]                    ; 3a 46 f8
    jne short 00ef8h                          ; 75 69
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 c2 20
    mov bx, ax                                ; 89 c3
    mov dx, 00084h                            ; ba 84 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 9b 20
    xor ah, ah                                ; 30 e4
    mov dx, ax                                ; 89 c2
    inc dx                                    ; 42
    mov al, cl                                ; 88 c8
    xor cl, cl                                ; 30 c9
    shr cx, 008h                              ; c1 e9 08
    mov byte [bp-00ah], cl                    ; 88 4e f6
    imul dx, bx                               ; 0f af d3
    or dl, 0ffh                               ; 80 ca ff
    movzx cx, byte [bp-008h]                  ; 0f b6 4e f8
    inc dx                                    ; 42
    imul dx, cx                               ; 0f af d1
    mov si, ax                                ; 89 c6
    add si, dx                                ; 01 d6
    movzx dx, byte [bp-00ah]                  ; 0f b6 56 f6
    imul bx, dx                               ; 0f af da
    add si, bx                                ; 01 de
    mov dx, strict word 00063h                ; ba 63 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 84 20
    mov bx, ax                                ; 89 c3
    mov AL, strict byte 00eh                  ; b0 0e
    mov dx, bx                                ; 89 da
    out DX, AL                                ; ee
    mov ax, si                                ; 89 f0
    xor al, al                                ; 30 c0
    shr ax, 008h                              ; c1 e8 08
    lea cx, [bx+001h]                         ; 8d 4f 01
    mov dx, cx                                ; 89 ca
    out DX, AL                                ; ee
    mov AL, strict byte 00fh                  ; b0 0f
    mov dx, bx                                ; 89 da
    out DX, AL                                ; ee
    and si, 000ffh                            ; 81 e6 ff 00
    mov ax, si                                ; 89 f0
    mov dx, cx                                ; 89 ca
    out DX, AL                                ; ee
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_set_active_page_:                     ; 0xc0f00 LB 0xdc
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov cl, al                                ; 88 c1
    cmp AL, strict byte 007h                  ; 3c 07
    jnbe near 00fd2h                          ; 0f 87 c0 00
    mov dx, strict word 00049h                ; ba 49 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 23 20
    xor ah, ah                                ; 30 e4
    call 02f17h                               ; e8 f7 1f
    mov ch, al                                ; 88 c5
    cmp AL, strict byte 0ffh                  ; 3c ff
    je near 00fd2h                            ; 0f 84 aa 00
    movzx ax, cl                              ; 0f b6 c1
    lea bx, [bp-00eh]                         ; 8d 5e f2
    lea dx, [bp-00ch]                         ; 8d 56 f4
    call 00a88h                               ; e8 54 fb
    movzx bx, ch                              ; 0f b6 dd
    mov si, bx                                ; 89 de
    sal si, 003h                              ; c1 e6 03
    cmp byte [si+0462fh], 000h                ; 80 bc 2f 46 00
    jne short 00f83h                          ; 75 40
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 0e 20
    mov bx, ax                                ; 89 c3
    mov dx, 00084h                            ; ba 84 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 e7 1f
    xor ah, ah                                ; 30 e4
    inc ax                                    ; 40
    mov si, bx                                ; 89 de
    imul si, ax                               ; 0f af f0
    mov ax, si                                ; 89 f0
    add ax, si                                ; 01 f0
    or AL, strict byte 0ffh                   ; 0c ff
    movzx di, cl                              ; 0f b6 f9
    mov bx, ax                                ; 89 c3
    inc bx                                    ; 43
    imul bx, di                               ; 0f af df
    mov dx, strict word 0004eh                ; ba 4e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 f1 1f
    or si, 000ffh                             ; 81 ce ff 00
    lea bx, [si+001h]                         ; 8d 5c 01
    imul bx, di                               ; 0f af df
    jmp short 00f95h                          ; eb 12
    movzx bx, byte [bx+046aeh]                ; 0f b6 9f ae 46
    sal bx, 006h                              ; c1 e3 06
    movzx ax, cl                              ; 0f b6 c1
    mov bx, word [bx+046c5h]                  ; 8b 9f c5 46
    imul bx, ax                               ; 0f af d8
    mov dx, strict word 00063h                ; ba 63 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 bc 1f
    mov si, ax                                ; 89 c6
    mov AL, strict byte 00ch                  ; b0 0c
    mov dx, si                                ; 89 f2
    out DX, AL                                ; ee
    mov ax, bx                                ; 89 d8
    xor al, bl                                ; 30 d8
    shr ax, 008h                              ; c1 e8 08
    lea di, [si+001h]                         ; 8d 7c 01
    mov dx, di                                ; 89 fa
    out DX, AL                                ; ee
    mov AL, strict byte 00dh                  ; b0 0d
    mov dx, si                                ; 89 f2
    out DX, AL                                ; ee
    mov al, bl                                ; 88 d8
    mov dx, di                                ; 89 fa
    out DX, AL                                ; ee
    movzx si, cl                              ; 0f b6 f1
    mov bx, si                                ; 89 f3
    mov dx, strict word 00062h                ; ba 62 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 82 1f
    mov dx, word [bp-00eh]                    ; 8b 56 f2
    mov ax, si                                ; 89 f0
    call 00e5eh                               ; e8 8c fe
    lea sp, [bp-00ah]                         ; 8d 66 f6
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_set_video_mode_:                      ; 0xc0fdc LB 0x391
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00010h                ; 83 ec 10
    mov byte [bp-00ch], al                    ; 88 46 f4
    and AL, strict byte 080h                  ; 24 80
    mov byte [bp-010h], al                    ; 88 46 f0
    call 007fah                               ; e8 08 f8
    test ax, ax                               ; 85 c0
    je short 01002h                           ; 74 0c
    mov AL, strict byte 007h                  ; b0 07
    mov dx, 003c4h                            ; ba c4 03
    out DX, AL                                ; ee
    xor al, al                                ; 30 c0
    mov dx, 003c5h                            ; ba c5 03
    out DX, AL                                ; ee
    and byte [bp-00ch], 07fh                  ; 80 66 f4 7f
    cmp byte [bp-00ch], 007h                  ; 80 7e f4 07
    jne short 01010h                          ; 75 04
    mov byte [bp-00ch], 000h                  ; c6 46 f4 00
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    call 02f17h                               ; e8 00 1f
    mov byte [bp-012h], al                    ; 88 46 ee
    cmp AL, strict byte 0ffh                  ; 3c ff
    je near 01363h                            ; 0f 84 43 03
    movzx si, al                              ; 0f b6 f0
    mov al, byte [si+046aeh]                  ; 8a 84 ae 46
    mov byte [bp-00eh], al                    ; 88 46 f2
    movzx bx, al                              ; 0f b6 d8
    sal bx, 006h                              ; c1 e3 06
    movzx ax, byte [bx+046c2h]                ; 0f b6 87 c2 46
    mov word [bp-018h], ax                    ; 89 46 e8
    movzx ax, byte [bx+046c3h]                ; 0f b6 87 c3 46
    mov word [bp-016h], ax                    ; 89 46 ea
    movzx ax, byte [bx+046c4h]                ; 0f b6 87 c4 46
    mov word [bp-014h], ax                    ; 89 46 ec
    mov dx, 00087h                            ; ba 87 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 ed 1e
    mov dx, 00088h                            ; ba 88 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 e4 1e
    mov dx, 00089h                            ; ba 89 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 db 1e
    mov ah, al                                ; 88 c4
    test AL, strict byte 008h                 ; a8 08
    jne near 010f5h                           ; 0f 85 8a 00
    mov bx, si                                ; 89 f3
    sal bx, 003h                              ; c1 e3 03
    mov al, byte [bx+04634h]                  ; 8a 87 34 46
    mov dx, 003c6h                            ; ba c6 03
    out DX, AL                                ; ee
    xor al, al                                ; 30 c0
    mov dx, 003c8h                            ; ba c8 03
    out DX, AL                                ; ee
    mov bl, byte [bx+04635h]                  ; 8a 9f 35 46
    cmp bl, 001h                              ; 80 fb 01
    jc short 01095h                           ; 72 0e
    jbe short 0109eh                          ; 76 15
    cmp bl, 003h                              ; 80 fb 03
    je short 010a8h                           ; 74 1a
    cmp bl, 002h                              ; 80 fb 02
    je short 010a3h                           ; 74 10
    jmp short 010abh                          ; eb 16
    test bl, bl                               ; 84 db
    jne short 010abh                          ; 75 12
    mov di, 04e42h                            ; bf 42 4e
    jmp short 010abh                          ; eb 0d
    mov di, 04f02h                            ; bf 02 4f
    jmp short 010abh                          ; eb 08
    mov di, 04fc2h                            ; bf c2 4f
    jmp short 010abh                          ; eb 03
    mov di, 05082h                            ; bf 82 50
    xor bx, bx                                ; 31 db
    jmp short 010beh                          ; eb 0f
    xor al, al                                ; 30 c0
    mov dx, 003c9h                            ; ba c9 03
    out DX, AL                                ; ee
    out DX, AL                                ; ee
    out DX, AL                                ; ee
    inc bx                                    ; 43
    cmp bx, 00100h                            ; 81 fb 00 01
    jnc short 010e8h                          ; 73 2a
    movzx si, byte [bp-012h]                  ; 0f b6 76 ee
    sal si, 003h                              ; c1 e6 03
    movzx si, byte [si+04635h]                ; 0f b6 b4 35 46
    movzx dx, byte [si+046beh]                ; 0f b6 94 be 46
    cmp bx, dx                                ; 39 d3
    jnbe short 010afh                         ; 77 dc
    imul si, bx, strict byte 00003h           ; 6b f3 03
    add si, di                                ; 01 fe
    mov al, byte [si]                         ; 8a 04
    mov dx, 003c9h                            ; ba c9 03
    out DX, AL                                ; ee
    mov al, byte [si+001h]                    ; 8a 44 01
    out DX, AL                                ; ee
    mov al, byte [si+002h]                    ; 8a 44 02
    out DX, AL                                ; ee
    jmp short 010b7h                          ; eb cf
    test ah, 002h                             ; f6 c4 02
    je short 010f5h                           ; 74 08
    mov dx, 00100h                            ; ba 00 01
    xor ax, ax                                ; 31 c0
    call 00d2eh                               ; e8 39 fc
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    xor bx, bx                                ; 31 db
    jmp short 01104h                          ; eb 05
    cmp bx, strict byte 00013h                ; 83 fb 13
    jnbe short 0111bh                         ; 77 17
    mov al, bl                                ; 88 d8
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    movzx si, byte [bp-00eh]                  ; 0f b6 76 f2
    sal si, 006h                              ; c1 e6 06
    add si, bx                                ; 01 de
    mov al, byte [si+046e5h]                  ; 8a 84 e5 46
    out DX, AL                                ; ee
    inc bx                                    ; 43
    jmp short 010ffh                          ; eb e4
    mov AL, strict byte 014h                  ; b0 14
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    xor al, al                                ; 30 c0
    out DX, AL                                ; ee
    mov dx, 003c4h                            ; ba c4 03
    out DX, AL                                ; ee
    mov AL, strict byte 003h                  ; b0 03
    mov dx, 003c5h                            ; ba c5 03
    out DX, AL                                ; ee
    mov bx, strict word 00001h                ; bb 01 00
    jmp short 01138h                          ; eb 05
    cmp bx, strict byte 00004h                ; 83 fb 04
    jnbe short 01152h                         ; 77 1a
    mov al, bl                                ; 88 d8
    mov dx, 003c4h                            ; ba c4 03
    out DX, AL                                ; ee
    movzx si, byte [bp-00eh]                  ; 0f b6 76 f2
    sal si, 006h                              ; c1 e6 06
    add si, bx                                ; 01 de
    mov al, byte [si+046c6h]                  ; 8a 84 c6 46
    mov dx, 003c5h                            ; ba c5 03
    out DX, AL                                ; ee
    inc bx                                    ; 43
    jmp short 01133h                          ; eb e1
    xor bx, bx                                ; 31 db
    jmp short 0115bh                          ; eb 05
    cmp bx, strict byte 00008h                ; 83 fb 08
    jnbe short 01175h                         ; 77 1a
    mov al, bl                                ; 88 d8
    mov dx, 003ceh                            ; ba ce 03
    out DX, AL                                ; ee
    movzx si, byte [bp-00eh]                  ; 0f b6 76 f2
    sal si, 006h                              ; c1 e6 06
    add si, bx                                ; 01 de
    mov al, byte [si+046f9h]                  ; 8a 84 f9 46
    mov dx, 003cfh                            ; ba cf 03
    out DX, AL                                ; ee
    inc bx                                    ; 43
    jmp short 01156h                          ; eb e1
    movzx bx, byte [bp-012h]                  ; 0f b6 5e ee
    sal bx, 003h                              ; c1 e3 03
    cmp byte [bx+04630h], 001h                ; 80 bf 30 46 01
    jne short 01188h                          ; 75 05
    mov dx, 003b4h                            ; ba b4 03
    jmp short 0118bh                          ; eb 03
    mov dx, 003d4h                            ; ba d4 03
    mov si, dx                                ; 89 d6
    mov ax, strict word 00011h                ; b8 11 00
    out DX, ax                                ; ef
    xor bx, bx                                ; 31 db
    jmp short 0119ah                          ; eb 05
    cmp bx, strict byte 00018h                ; 83 fb 18
    jnbe short 011b5h                         ; 77 1b
    mov al, bl                                ; 88 d8
    mov dx, si                                ; 89 f2
    out DX, AL                                ; ee
    movzx cx, byte [bp-00eh]                  ; 0f b6 4e f2
    sal cx, 006h                              ; c1 e1 06
    mov di, cx                                ; 89 cf
    add di, bx                                ; 01 df
    lea dx, [si+001h]                         ; 8d 54 01
    mov al, byte [di+046cch]                  ; 8a 85 cc 46
    out DX, AL                                ; ee
    inc bx                                    ; 43
    jmp short 01195h                          ; eb e0
    mov bx, cx                                ; 89 cb
    mov al, byte [bx+046cbh]                  ; 8a 87 cb 46
    mov dx, 003c2h                            ; ba c2 03
    out DX, AL                                ; ee
    mov AL, strict byte 020h                  ; b0 20
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp byte [bp-010h], 000h                  ; 80 7e f0 00
    jne short 01230h                          ; 75 5f
    movzx bx, byte [bp-012h]                  ; 0f b6 5e ee
    sal bx, 003h                              ; c1 e3 03
    cmp byte [bx+0462fh], 000h                ; 80 bf 2f 46 00
    jne short 011f2h                          ; 75 13
    mov es, [bx+04632h]                       ; 8e 87 32 46
    mov cx, 04000h                            ; b9 00 40
    mov ax, 00720h                            ; b8 20 07
    xor di, di                                ; 31 ff
    cld                                       ; fc
    jcxz 011f0h                               ; e3 02
    rep stosw                                 ; f3 ab
    jmp short 01230h                          ; eb 3e
    cmp byte [bp-00ch], 00dh                  ; 80 7e f4 0d
    jnc short 0120ah                          ; 73 12
    mov es, [bx+04632h]                       ; 8e 87 32 46
    mov cx, 04000h                            ; b9 00 40
    xor ax, ax                                ; 31 c0
    xor di, di                                ; 31 ff
    cld                                       ; fc
    jcxz 01208h                               ; e3 02
    rep stosw                                 ; f3 ab
    jmp short 01230h                          ; eb 26
    mov AL, strict byte 002h                  ; b0 02
    mov dx, 003c4h                            ; ba c4 03
    out DX, AL                                ; ee
    mov dx, 003c5h                            ; ba c5 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov word [bp-01ah], ax                    ; 89 46 e6
    mov AL, strict byte 00fh                  ; b0 0f
    out DX, AL                                ; ee
    mov es, [bx+04632h]                       ; 8e 87 32 46
    mov cx, 08000h                            ; b9 00 80
    xor ax, ax                                ; 31 c0
    xor di, di                                ; 31 ff
    cld                                       ; fc
    jcxz 0122ch                               ; e3 02
    rep stosw                                 ; f3 ab
    mov al, byte [bp-01ah]                    ; 8a 46 e6
    out DX, AL                                ; ee
    movzx bx, byte [bp-00ch]                  ; 0f b6 5e f4
    mov dx, strict word 00049h                ; ba 49 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 0f 1d
    mov bx, word [bp-018h]                    ; 8b 5e e8
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 1f 1d
    movzx bx, byte [bp-00eh]                  ; 0f b6 5e f2
    sal bx, 006h                              ; c1 e3 06
    mov bx, word [bx+046c5h]                  ; 8b 9f c5 46
    mov dx, strict word 0004ch                ; ba 4c 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 0b 1d
    mov bx, si                                ; 89 f3
    mov dx, strict word 00063h                ; ba 63 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 00 1d
    movzx bx, byte [bp-016h]                  ; 0f b6 5e ea
    mov dx, 00084h                            ; ba 84 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 d7 1c
    mov bx, word [bp-014h]                    ; 8b 5e ec
    mov dx, 00085h                            ; ba 85 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 e7 1c
    mov al, byte [bp-010h]                    ; 8a 46 f0
    or AL, strict byte 060h                   ; 0c 60
    movzx bx, al                              ; 0f b6 d8
    mov dx, 00087h                            ; ba 87 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 ba 1c
    mov bx, 000f9h                            ; bb f9 00
    mov dx, 00088h                            ; ba 88 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 ae 1c
    mov dx, 00089h                            ; ba 89 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 97 1c
    and AL, strict byte 07fh                  ; 24 7f
    movzx bx, al                              ; 0f b6 d8
    mov dx, 00089h                            ; ba 89 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 97 1c
    mov bx, strict word 00008h                ; bb 08 00
    mov dx, 0008ah                            ; ba 8a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 8b 1c
    mov cx, ds                                ; 8c d9
    mov bx, 053d0h                            ; bb d0 53
    mov dx, 000a8h                            ; ba a8 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f88h                               ; e8 b9 1c
    xor bx, bx                                ; 31 db
    mov dx, strict word 00065h                ; ba 65 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 72 1c
    xor bx, bx                                ; 31 db
    mov dx, strict word 00066h                ; ba 66 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 67 1c
    movzx bx, byte [bp-012h]                  ; 0f b6 5e ee
    sal bx, 003h                              ; c1 e3 03
    cmp byte [bx+0462fh], 000h                ; 80 bf 2f 46 00
    jne short 012fch                          ; 75 09
    mov dx, strict word 00007h                ; ba 07 00
    mov ax, strict word 00006h                ; b8 06 00
    call 00dbah                               ; e8 be fa
    xor bx, bx                                ; 31 db
    jmp short 01305h                          ; eb 05
    cmp bx, strict byte 00008h                ; 83 fb 08
    jnc short 01310h                          ; 73 0b
    movzx ax, bl                              ; 0f b6 c3
    xor dx, dx                                ; 31 d2
    call 00e5eh                               ; e8 51 fb
    inc bx                                    ; 43
    jmp short 01300h                          ; eb f0
    xor ax, ax                                ; 31 c0
    call 00f00h                               ; e8 eb fb
    movzx bx, byte [bp-012h]                  ; 0f b6 5e ee
    sal bx, 003h                              ; c1 e3 03
    cmp byte [bx+0462fh], 000h                ; 80 bf 2f 46 00
    jne short 01333h                          ; 75 10
    xor bl, bl                                ; 30 db
    mov AL, strict byte 004h                  ; b0 04
    mov AH, strict byte 011h                  ; b4 11
    int 010h                                  ; cd 10
    xor bl, bl                                ; 30 db
    mov AL, strict byte 003h                  ; b0 03
    mov AH, strict byte 011h                  ; b4 11
    int 010h                                  ; cd 10
    mov dx, 057ech                            ; ba ec 57
    mov ax, strict word 0001fh                ; b8 1f 00
    call 00a00h                               ; e8 c4 f6
    mov ax, word [bp-014h]                    ; 8b 46 ec
    cmp ax, strict word 00010h                ; 3d 10 00
    je short 0135eh                           ; 74 1a
    cmp ax, strict word 0000eh                ; 3d 0e 00
    je short 01359h                           ; 74 10
    cmp ax, strict word 00008h                ; 3d 08 00
    jne short 01363h                          ; 75 15
    mov dx, 053ech                            ; ba ec 53
    mov ax, strict word 00043h                ; b8 43 00
    call 00a00h                               ; e8 a9 f6
    jmp short 01363h                          ; eb 0a
    mov dx, 05bech                            ; ba ec 5b
    jmp short 01351h                          ; eb f3
    mov dx, 069ech                            ; ba ec 69
    jmp short 01351h                          ; eb ee
    lea sp, [bp-00ah]                         ; 8d 66 f6
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
vgamem_copy_pl4_:                            ; 0xc136d LB 0x76
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov bh, cl                                ; 88 cf
    movzx di, dl                              ; 0f b6 fa
    movzx cx, byte [bp+006h]                  ; 0f b6 4e 06
    imul di, cx                               ; 0f af f9
    movzx si, byte [bp+004h]                  ; 0f b6 76 04
    imul di, si                               ; 0f af fe
    xor ah, ah                                ; 30 e4
    add di, ax                                ; 01 c7
    mov word [bp-008h], di                    ; 89 7e f8
    movzx di, bl                              ; 0f b6 fb
    imul cx, di                               ; 0f af cf
    imul cx, si                               ; 0f af ce
    add cx, ax                                ; 01 c1
    mov word [bp-006h], cx                    ; 89 4e fa
    mov ax, 00105h                            ; b8 05 01
    mov dx, 003ceh                            ; ba ce 03
    out DX, ax                                ; ef
    xor bl, bl                                ; 30 db
    cmp bl, byte [bp+006h]                    ; 3a 5e 06
    jnc short 013d3h                          ; 73 29
    movzx cx, bh                              ; 0f b6 cf
    movzx si, bl                              ; 0f b6 f3
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    imul ax, si                               ; 0f af c6
    mov si, word [bp-008h]                    ; 8b 76 f8
    add si, ax                                ; 01 c6
    mov di, word [bp-006h]                    ; 8b 7e fa
    add di, ax                                ; 01 c7
    mov dx, 0a000h                            ; ba 00 a0
    mov es, dx                                ; 8e c2
    cld                                       ; fc
    jcxz 013cfh                               ; e3 06
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsb                                 ; f3 a4
    pop DS                                    ; 1f
    db  0feh, 0c3h
    ; inc bl                                    ; fe c3
    jmp short 013a5h                          ; eb d2
    mov ax, strict word 00005h                ; b8 05 00
    mov dx, 003ceh                            ; ba ce 03
    out DX, ax                                ; ef
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00004h                               ; c2 04 00
vgamem_fill_pl4_:                            ; 0xc13e3 LB 0x61
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov byte [bp-004h], bl                    ; 88 5e fc
    mov bh, cl                                ; 88 cf
    movzx cx, dl                              ; 0f b6 ca
    movzx dx, byte [bp+004h]                  ; 0f b6 56 04
    imul cx, dx                               ; 0f af ca
    movzx dx, bh                              ; 0f b6 d7
    imul dx, cx                               ; 0f af d1
    xor ah, ah                                ; 30 e4
    add dx, ax                                ; 01 c2
    mov word [bp-006h], dx                    ; 89 56 fa
    mov ax, 00205h                            ; b8 05 02
    mov dx, 003ceh                            ; ba ce 03
    out DX, ax                                ; ef
    xor bl, bl                                ; 30 db
    cmp bl, byte [bp+004h]                    ; 3a 5e 04
    jnc short 01435h                          ; 73 22
    movzx cx, byte [bp-004h]                  ; 0f b6 4e fc
    movzx ax, byte [bp+006h]                  ; 0f b6 46 06
    movzx dx, bl                              ; 0f b6 d3
    movzx di, bh                              ; 0f b6 ff
    imul di, dx                               ; 0f af fa
    add di, word [bp-006h]                    ; 03 7e fa
    mov dx, 0a000h                            ; ba 00 a0
    mov es, dx                                ; 8e c2
    cld                                       ; fc
    jcxz 01431h                               ; e3 02
    rep stosb                                 ; f3 aa
    db  0feh, 0c3h
    ; inc bl                                    ; fe c3
    jmp short 0140eh                          ; eb d9
    mov ax, strict word 00005h                ; b8 05 00
    mov dx, 003ceh                            ; ba ce 03
    out DX, ax                                ; ef
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop di                                    ; 5f
    pop bp                                    ; 5d
    retn 00004h                               ; c2 04 00
vgamem_copy_cga_:                            ; 0xc1444 LB 0xa4
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov bh, cl                                ; 88 cf
    movzx di, dl                              ; 0f b6 fa
    movzx cx, byte [bp+006h]                  ; 0f b6 4e 06
    imul di, cx                               ; 0f af f9
    movzx si, byte [bp+004h]                  ; 0f b6 76 04
    imul di, si                               ; 0f af fe
    sar di, 1                                 ; d1 ff
    xor ah, ah                                ; 30 e4
    add di, ax                                ; 01 c7
    mov word [bp-006h], di                    ; 89 7e fa
    movzx di, bl                              ; 0f b6 fb
    imul cx, di                               ; 0f af cf
    imul si, cx                               ; 0f af f1
    sar si, 1                                 ; d1 fe
    add si, ax                                ; 01 c6
    mov word [bp-008h], si                    ; 89 76 f8
    xor bl, bl                                ; 30 db
    cmp bl, byte [bp+006h]                    ; 3a 5e 06
    jnc short 014dfh                          ; 73 61
    test bl, 001h                             ; f6 c3 01
    je short 014b4h                           ; 74 31
    movzx cx, bh                              ; 0f b6 cf
    movzx si, bl                              ; 0f b6 f3
    sar si, 1                                 ; d1 fe
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    imul ax, si                               ; 0f af c6
    mov si, word [bp-006h]                    ; 8b 76 fa
    add si, 02000h                            ; 81 c6 00 20
    add si, ax                                ; 01 c6
    mov di, word [bp-008h]                    ; 8b 7e f8
    add di, 02000h                            ; 81 c7 00 20
    add di, ax                                ; 01 c7
    mov dx, 0b800h                            ; ba 00 b8
    mov es, dx                                ; 8e c2
    cld                                       ; fc
    jcxz 014b2h                               ; e3 06
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsb                                 ; f3 a4
    pop DS                                    ; 1f
    jmp short 014dbh                          ; eb 27
    movzx cx, bh                              ; 0f b6 cf
    movzx ax, bl                              ; 0f b6 c3
    sar ax, 1                                 ; d1 f8
    movzx si, byte [bp+004h]                  ; 0f b6 76 04
    imul ax, si                               ; 0f af c6
    mov si, word [bp-006h]                    ; 8b 76 fa
    add si, ax                                ; 01 c6
    mov di, word [bp-008h]                    ; 8b 7e f8
    add di, ax                                ; 01 c7
    mov dx, 0b800h                            ; ba 00 b8
    mov es, dx                                ; 8e c2
    cld                                       ; fc
    jcxz 014dbh                               ; e3 06
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsb                                 ; f3 a4
    pop DS                                    ; 1f
    db  0feh, 0c3h
    ; inc bl                                    ; fe c3
    jmp short 01479h                          ; eb 9a
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00004h                               ; c2 04 00
vgamem_fill_cga_:                            ; 0xc14e8 LB 0x8a
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov byte [bp-006h], bl                    ; 88 5e fa
    mov bh, cl                                ; 88 cf
    movzx cx, dl                              ; 0f b6 ca
    movzx dx, byte [bp+004h]                  ; 0f b6 56 04
    imul dx, cx                               ; 0f af d1
    movzx cx, bh                              ; 0f b6 cf
    imul dx, cx                               ; 0f af d1
    sar dx, 1                                 ; d1 fa
    movzx si, al                              ; 0f b6 f0
    add si, dx                                ; 01 d6
    xor bl, bl                                ; 30 db
    cmp bl, byte [bp+004h]                    ; 3a 5e 04
    jnc short 01569h                          ; 73 57
    test bl, 001h                             ; f6 c3 01
    je short 01546h                           ; 74 2f
    movzx cx, byte [bp-006h]                  ; 0f b6 4e fa
    movzx ax, byte [bp+006h]                  ; 0f b6 46 06
    movzx dx, bl                              ; 0f b6 d3
    sar dx, 1                                 ; d1 fa
    mov word [bp-008h], dx                    ; 89 56 f8
    movzx dx, bh                              ; 0f b6 d7
    mov di, word [bp-008h]                    ; 8b 7e f8
    imul di, dx                               ; 0f af fa
    mov word [bp-008h], di                    ; 89 7e f8
    lea di, [si+02000h]                       ; 8d bc 00 20
    add di, word [bp-008h]                    ; 03 7e f8
    mov dx, 0b800h                            ; ba 00 b8
    mov es, dx                                ; 8e c2
    cld                                       ; fc
    jcxz 01544h                               ; e3 02
    rep stosb                                 ; f3 aa
    jmp short 01565h                          ; eb 1f
    movzx cx, byte [bp-006h]                  ; 0f b6 4e fa
    movzx ax, byte [bp+006h]                  ; 0f b6 46 06
    movzx di, bl                              ; 0f b6 fb
    sar di, 1                                 ; d1 ff
    movzx dx, bh                              ; 0f b6 d7
    imul di, dx                               ; 0f af fa
    add di, si                                ; 01 f7
    mov dx, 0b800h                            ; ba 00 b8
    mov es, dx                                ; 8e c2
    cld                                       ; fc
    jcxz 01565h                               ; e3 02
    rep stosb                                 ; f3 aa
    db  0feh, 0c3h
    ; inc bl                                    ; fe c3
    jmp short 0150dh                          ; eb a4
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00004h                               ; c2 04 00
biosfn_scroll_:                              ; 0xc1572 LB 0x506
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00018h                ; 83 ec 18
    mov byte [bp-010h], al                    ; 88 46 f0
    mov byte [bp-00ch], dl                    ; 88 56 f4
    mov byte [bp-008h], bl                    ; 88 5e f8
    mov byte [bp-006h], cl                    ; 88 4e fa
    cmp bl, byte [bp+004h]                    ; 3a 5e 04
    jnbe near 01a6fh                          ; 0f 87 e2 04
    cmp cl, byte [bp+006h]                    ; 3a 4e 06
    jnbe near 01a6fh                          ; 0f 87 db 04
    mov dx, strict word 00049h                ; ba 49 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 a1 19
    xor ah, ah                                ; 30 e4
    call 02f17h                               ; e8 75 19
    mov byte [bp-00eh], al                    ; 88 46 f2
    cmp AL, strict byte 0ffh                  ; 3c ff
    je near 01a6fh                            ; 0f 84 c4 04
    mov dx, 00084h                            ; ba 84 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 8a 19
    movzx cx, al                              ; 0f b6 c8
    inc cx                                    ; 41
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 99 19
    mov word [bp-016h], ax                    ; 89 46 ea
    cmp byte [bp+008h], 0ffh                  ; 80 7e 08 ff
    jne short 015d6h                          ; 75 0c
    mov dx, strict word 00062h                ; ba 62 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 6b 19
    mov byte [bp+008h], al                    ; 88 46 08
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    cmp ax, cx                                ; 39 c8
    jc short 015e5h                           ; 72 07
    mov al, cl                                ; 88 c8
    db  0feh, 0c8h
    ; dec al                                    ; fe c8
    mov byte [bp+004h], al                    ; 88 46 04
    movzx ax, byte [bp+006h]                  ; 0f b6 46 06
    cmp ax, word [bp-016h]                    ; 3b 46 ea
    jc short 015f6h                           ; 72 08
    mov al, byte [bp-016h]                    ; 8a 46 ea
    db  0feh, 0c8h
    ; dec al                                    ; fe c8
    mov byte [bp+006h], al                    ; 88 46 06
    movzx ax, byte [bp-010h]                  ; 0f b6 46 f0
    cmp ax, cx                                ; 39 c8
    jbe short 01602h                          ; 76 04
    mov byte [bp-010h], 000h                  ; c6 46 f0 00
    mov al, byte [bp+006h]                    ; 8a 46 06
    sub al, byte [bp-006h]                    ; 2a 46 fa
    db  0feh, 0c0h
    ; inc al                                    ; fe c0
    mov byte [bp-012h], al                    ; 88 46 ee
    movzx si, byte [bp-00eh]                  ; 0f b6 76 f2
    mov di, si                                ; 89 f7
    sal di, 003h                              ; c1 e7 03
    mov ax, word [bp-016h]                    ; 8b 46 ea
    dec ax                                    ; 48
    mov word [bp-018h], ax                    ; 89 46 e8
    mov ax, cx                                ; 89 c8
    dec ax                                    ; 48
    mov word [bp-01ah], ax                    ; 89 46 e6
    mov ax, word [bp-016h]                    ; 8b 46 ea
    imul ax, cx                               ; 0f af c1
    cmp byte [di+0462fh], 000h                ; 80 bd 2f 46 00
    jne near 017d1h                           ; 0f 85 9f 01
    mov dx, ax                                ; 89 c2
    add dx, ax                                ; 01 c2
    or dl, 0ffh                               ; 80 ca ff
    movzx bx, byte [bp+008h]                  ; 0f b6 5e 08
    inc dx                                    ; 42
    imul bx, dx                               ; 0f af da
    cmp byte [bp-010h], 000h                  ; 80 7e f0 00
    jne short 01681h                          ; 75 3a
    cmp byte [bp-008h], 000h                  ; 80 7e f8 00
    jne short 01681h                          ; 75 34
    cmp byte [bp-006h], 000h                  ; 80 7e fa 00
    jne short 01681h                          ; 75 2e
    movzx dx, byte [bp+004h]                  ; 0f b6 56 04
    cmp dx, word [bp-01ah]                    ; 3b 56 e6
    jne short 01681h                          ; 75 25
    movzx dx, byte [bp+006h]                  ; 0f b6 56 06
    cmp dx, word [bp-018h]                    ; 3b 56 e8
    jne short 01681h                          ; 75 1c
    movzx dx, byte [bp-00ch]                  ; 0f b6 56 f4
    sal dx, 008h                              ; c1 e2 08
    add dx, strict byte 00020h                ; 83 c2 20
    mov es, [di+04632h]                       ; 8e 85 32 46
    mov cx, ax                                ; 89 c1
    mov ax, dx                                ; 89 d0
    mov di, bx                                ; 89 df
    cld                                       ; fc
    jcxz 0167eh                               ; e3 02
    rep stosw                                 ; f3 ab
    jmp near 01a6fh                           ; e9 ee 03
    cmp byte [bp+00ah], 001h                  ; 80 7e 0a 01
    jne near 01726h                           ; 0f 85 9d 00
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    mov word [bp-014h], ax                    ; 89 46 ec
    movzx dx, byte [bp+004h]                  ; 0f b6 56 04
    cmp dx, word [bp-014h]                    ; 3b 56 ec
    jc near 01a6fh                            ; 0f 82 d4 03
    movzx ax, byte [bp-010h]                  ; 0f b6 46 f0
    add ax, word [bp-014h]                    ; 03 46 ec
    cmp ax, dx                                ; 39 d0
    jnbe short 016ach                         ; 77 06
    cmp byte [bp-010h], 000h                  ; 80 7e f0 00
    jne short 016dfh                          ; 75 33
    movzx cx, byte [bp-012h]                  ; 0f b6 4e ee
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    sal ax, 008h                              ; c1 e0 08
    add ax, strict word 00020h                ; 05 20 00
    mov si, word [bp-014h]                    ; 8b 76 ec
    imul si, word [bp-016h]                   ; 0f af 76 ea
    movzx dx, byte [bp-006h]                  ; 0f b6 56 fa
    add dx, si                                ; 01 f2
    add dx, dx                                ; 01 d2
    mov di, bx                                ; 89 df
    add di, dx                                ; 01 d7
    movzx si, byte [bp-00eh]                  ; 0f b6 76 f2
    sal si, 003h                              ; c1 e6 03
    mov es, [si+04632h]                       ; 8e 84 32 46
    cld                                       ; fc
    jcxz 016ddh                               ; e3 02
    rep stosw                                 ; f3 ab
    jmp short 01720h                          ; eb 41
    movzx dx, byte [bp-012h]                  ; 0f b6 56 ee
    mov word [bp-01ch], dx                    ; 89 56 e4
    mov dx, ax                                ; 89 c2
    imul dx, word [bp-016h]                   ; 0f af 56 ea
    movzx cx, byte [bp-006h]                  ; 0f b6 4e fa
    add dx, cx                                ; 01 ca
    add dx, dx                                ; 01 d2
    movzx si, byte [bp-00eh]                  ; 0f b6 76 f2
    sal si, 003h                              ; c1 e6 03
    mov ax, word [si+04632h]                  ; 8b 84 32 46
    mov si, word [bp-014h]                    ; 8b 76 ec
    imul si, word [bp-016h]                   ; 0f af 76 ea
    add cx, si                                ; 01 f1
    add cx, cx                                ; 01 c9
    mov di, bx                                ; 89 df
    add di, cx                                ; 01 cf
    mov cx, word [bp-01ch]                    ; 8b 4e e4
    mov si, dx                                ; 89 d6
    mov dx, ax                                ; 89 c2
    mov es, ax                                ; 8e c0
    cld                                       ; fc
    jcxz 01720h                               ; e3 06
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsw                                 ; f3 a5
    pop DS                                    ; 1f
    inc word [bp-014h]                        ; ff 46 ec
    jmp near 01690h                           ; e9 6a ff
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    mov word [bp-014h], ax                    ; 89 46 ec
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    cmp ax, word [bp-014h]                    ; 3b 46 ec
    jnbe near 01a6fh                          ; 0f 87 37 03
    movzx dx, byte [bp-008h]                  ; 0f b6 56 f8
    movzx ax, byte [bp-010h]                  ; 0f b6 46 f0
    add ax, dx                                ; 01 d0
    cmp ax, word [bp-014h]                    ; 3b 46 ec
    jnbe short 0174dh                         ; 77 06
    cmp byte [bp-010h], 000h                  ; 80 7e f0 00
    jne short 01780h                          ; 75 33
    movzx cx, byte [bp-012h]                  ; 0f b6 4e ee
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    sal ax, 008h                              ; c1 e0 08
    add ax, strict word 00020h                ; 05 20 00
    mov si, word [bp-014h]                    ; 8b 76 ec
    imul si, word [bp-016h]                   ; 0f af 76 ea
    movzx dx, byte [bp-006h]                  ; 0f b6 56 fa
    add dx, si                                ; 01 f2
    add dx, dx                                ; 01 d2
    mov di, bx                                ; 89 df
    add di, dx                                ; 01 d7
    movzx si, byte [bp-00eh]                  ; 0f b6 76 f2
    sal si, 003h                              ; c1 e6 03
    mov es, [si+04632h]                       ; 8e 84 32 46
    cld                                       ; fc
    jcxz 0177eh                               ; e3 02
    rep stosw                                 ; f3 ab
    jmp short 017c0h                          ; eb 40
    movzx cx, byte [bp-012h]                  ; 0f b6 4e ee
    movzx ax, byte [bp-010h]                  ; 0f b6 46 f0
    mov dx, word [bp-014h]                    ; 8b 56 ec
    sub dx, ax                                ; 29 c2
    imul dx, word [bp-016h]                   ; 0f af 56 ea
    movzx di, byte [bp-006h]                  ; 0f b6 7e fa
    add dx, di                                ; 01 fa
    add dx, dx                                ; 01 d2
    movzx si, byte [bp-00eh]                  ; 0f b6 76 f2
    sal si, 003h                              ; c1 e6 03
    mov ax, word [si+04632h]                  ; 8b 84 32 46
    mov si, word [bp-014h]                    ; 8b 76 ec
    imul si, word [bp-016h]                   ; 0f af 76 ea
    add di, si                                ; 01 f7
    add di, di                                ; 01 ff
    add di, bx                                ; 01 df
    mov si, dx                                ; 89 d6
    mov dx, ax                                ; 89 c2
    mov es, ax                                ; 8e c0
    cld                                       ; fc
    jcxz 017c0h                               ; e3 06
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsw                                 ; f3 a5
    pop DS                                    ; 1f
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    cmp ax, word [bp-014h]                    ; 3b 46 ec
    jc near 01a6fh                            ; 0f 82 a4 02
    dec word [bp-014h]                        ; ff 4e ec
    jmp near 0172dh                           ; e9 5c ff
    movzx bx, byte [si+046aeh]                ; 0f b6 9c ae 46
    sal bx, 006h                              ; c1 e3 06
    mov dl, byte [bx+046c4h]                  ; 8a 97 c4 46
    mov byte [bp-00ah], dl                    ; 88 56 f6
    mov bl, byte [di+04630h]                  ; 8a 9d 30 46
    cmp bl, 004h                              ; 80 fb 04
    je short 017f8h                           ; 74 0f
    cmp bl, 003h                              ; 80 fb 03
    je short 017f8h                           ; 74 0a
    cmp bl, 002h                              ; 80 fb 02
    je near 01937h                            ; 0f 84 42 01
    jmp near 01a6fh                           ; e9 77 02
    cmp byte [bp-010h], 000h                  ; 80 7e f0 00
    jne short 01850h                          ; 75 52
    cmp byte [bp-008h], 000h                  ; 80 7e f8 00
    jne short 01850h                          ; 75 4c
    cmp byte [bp-006h], 000h                  ; 80 7e fa 00
    jne short 01850h                          ; 75 46
    movzx dx, byte [bp+004h]                  ; 0f b6 56 04
    mov ax, cx                                ; 89 c8
    dec ax                                    ; 48
    cmp dx, ax                                ; 39 c2
    jne short 01850h                          ; 75 3b
    movzx dx, byte [bp+006h]                  ; 0f b6 56 06
    mov ax, word [bp-016h]                    ; 8b 46 ea
    dec ax                                    ; 48
    cmp dx, ax                                ; 39 c2
    jne short 01850h                          ; 75 2f
    mov ax, 00205h                            ; b8 05 02
    mov dx, 003ceh                            ; ba ce 03
    out DX, ax                                ; ef
    imul cx, word [bp-016h]                   ; 0f af 4e ea
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    imul cx, ax                               ; 0f af c8
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    movzx bx, byte [bp-00eh]                  ; 0f b6 5e f2
    sal bx, 003h                              ; c1 e3 03
    mov es, [bx+04632h]                       ; 8e 87 32 46
    xor di, di                                ; 31 ff
    cld                                       ; fc
    jcxz 01849h                               ; e3 02
    rep stosb                                 ; f3 aa
    mov ax, strict word 00005h                ; b8 05 00
    out DX, ax                                ; ef
    jmp near 01a6fh                           ; e9 1f 02
    cmp byte [bp+00ah], 001h                  ; 80 7e 0a 01
    jne short 018bfh                          ; 75 69
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    mov word [bp-014h], ax                    ; 89 46 ec
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    cmp ax, word [bp-014h]                    ; 3b 46 ec
    jc near 01a6fh                            ; 0f 82 07 02
    movzx dx, byte [bp-010h]                  ; 0f b6 56 f0
    add dx, word [bp-014h]                    ; 03 56 ec
    cmp dx, ax                                ; 39 c2
    jnbe short 01879h                         ; 77 06
    cmp byte [bp-010h], 000h                  ; 80 7e f0 00
    jne short 01898h                          ; 75 1f
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    push ax                                   ; 50
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    push ax                                   ; 50
    movzx cx, byte [bp-016h]                  ; 0f b6 4e ea
    movzx bx, byte [bp-012h]                  ; 0f b6 5e ee
    movzx dx, byte [bp-014h]                  ; 0f b6 56 ec
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    call 013e3h                               ; e8 4d fb
    jmp short 018bah                          ; eb 22
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    push ax                                   ; 50
    movzx ax, byte [bp-016h]                  ; 0f b6 46 ea
    push ax                                   ; 50
    movzx cx, byte [bp-012h]                  ; 0f b6 4e ee
    movzx bx, byte [bp-014h]                  ; 0f b6 5e ec
    mov al, byte [bp-014h]                    ; 8a 46 ec
    add al, byte [bp-010h]                    ; 02 46 f0
    movzx dx, al                              ; 0f b6 d0
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    call 0136dh                               ; e8 b3 fa
    inc word [bp-014h]                        ; ff 46 ec
    jmp short 0185dh                          ; eb 9e
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    mov word [bp-014h], ax                    ; 89 46 ec
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    cmp ax, word [bp-014h]                    ; 3b 46 ec
    jnbe near 01a6fh                          ; 0f 87 9e 01
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    movzx dx, byte [bp-010h]                  ; 0f b6 56 f0
    add ax, dx                                ; 01 d0
    cmp ax, word [bp-014h]                    ; 3b 46 ec
    jnbe short 018e6h                         ; 77 06
    cmp byte [bp-010h], 000h                  ; 80 7e f0 00
    jne short 01905h                          ; 75 1f
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    push ax                                   ; 50
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    push ax                                   ; 50
    movzx cx, byte [bp-016h]                  ; 0f b6 4e ea
    movzx bx, byte [bp-012h]                  ; 0f b6 5e ee
    movzx dx, byte [bp-014h]                  ; 0f b6 56 ec
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    call 013e3h                               ; e8 e0 fa
    jmp short 01927h                          ; eb 22
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    push ax                                   ; 50
    movzx ax, byte [bp-016h]                  ; 0f b6 46 ea
    push ax                                   ; 50
    movzx cx, byte [bp-012h]                  ; 0f b6 4e ee
    mov al, byte [bp-014h]                    ; 8a 46 ec
    sub al, byte [bp-010h]                    ; 2a 46 f0
    movzx bx, al                              ; 0f b6 d8
    movzx dx, byte [bp-014h]                  ; 0f b6 56 ec
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    call 0136dh                               ; e8 46 fa
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    cmp ax, word [bp-014h]                    ; 3b 46 ec
    jc near 01a6fh                            ; 0f 82 3d 01
    dec word [bp-014h]                        ; ff 4e ec
    jmp short 018c6h                          ; eb 8f
    mov dl, byte [di+04631h]                  ; 8a 95 31 46
    cmp byte [bp-010h], 000h                  ; 80 7e f0 00
    jne short 0197eh                          ; 75 3d
    cmp byte [bp-008h], 000h                  ; 80 7e f8 00
    jne short 0197eh                          ; 75 37
    cmp byte [bp-006h], 000h                  ; 80 7e fa 00
    jne short 0197eh                          ; 75 31
    movzx bx, byte [bp+004h]                  ; 0f b6 5e 04
    cmp bx, word [bp-01ah]                    ; 3b 5e e6
    jne short 0197eh                          ; 75 28
    movzx bx, byte [bp+006h]                  ; 0f b6 5e 06
    cmp bx, word [bp-018h]                    ; 3b 5e e8
    jne short 0197eh                          ; 75 1f
    movzx bx, byte [bp-00ah]                  ; 0f b6 5e f6
    imul ax, bx                               ; 0f af c3
    movzx cx, dl                              ; 0f b6 ca
    imul cx, ax                               ; 0f af c8
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    mov es, [di+04632h]                       ; 8e 85 32 46
    xor di, di                                ; 31 ff
    cld                                       ; fc
    jcxz 0197bh                               ; e3 02
    rep stosb                                 ; f3 aa
    jmp near 01a6fh                           ; e9 f1 00
    cmp dl, 002h                              ; 80 fa 02
    jne short 0198ch                          ; 75 09
    sal byte [bp-006h], 1                     ; d0 66 fa
    sal byte [bp-012h], 1                     ; d0 66 ee
    sal word [bp-016h], 1                     ; d1 66 ea
    cmp byte [bp+00ah], 001h                  ; 80 7e 0a 01
    jne short 019fbh                          ; 75 69
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    mov word [bp-014h], ax                    ; 89 46 ec
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    cmp ax, word [bp-014h]                    ; 3b 46 ec
    jc near 01a6fh                            ; 0f 82 cb 00
    movzx dx, byte [bp-010h]                  ; 0f b6 56 f0
    add dx, word [bp-014h]                    ; 03 56 ec
    cmp dx, ax                                ; 39 c2
    jnbe short 019b5h                         ; 77 06
    cmp byte [bp-010h], 000h                  ; 80 7e f0 00
    jne short 019d4h                          ; 75 1f
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    push ax                                   ; 50
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    push ax                                   ; 50
    movzx cx, byte [bp-016h]                  ; 0f b6 4e ea
    movzx bx, byte [bp-012h]                  ; 0f b6 5e ee
    movzx dx, byte [bp-014h]                  ; 0f b6 56 ec
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    call 014e8h                               ; e8 16 fb
    jmp short 019f6h                          ; eb 22
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    push ax                                   ; 50
    movzx ax, byte [bp-016h]                  ; 0f b6 46 ea
    push ax                                   ; 50
    movzx cx, byte [bp-012h]                  ; 0f b6 4e ee
    movzx bx, byte [bp-014h]                  ; 0f b6 5e ec
    mov al, byte [bp-014h]                    ; 8a 46 ec
    add al, byte [bp-010h]                    ; 02 46 f0
    movzx dx, al                              ; 0f b6 d0
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    call 01444h                               ; e8 4e fa
    inc word [bp-014h]                        ; ff 46 ec
    jmp short 01999h                          ; eb 9e
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    mov word [bp-014h], ax                    ; 89 46 ec
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    cmp ax, word [bp-014h]                    ; 3b 46 ec
    jnbe short 01a6fh                         ; 77 64
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    movzx dx, byte [bp-010h]                  ; 0f b6 56 f0
    add ax, dx                                ; 01 d0
    cmp ax, word [bp-014h]                    ; 3b 46 ec
    jnbe short 01a20h                         ; 77 06
    cmp byte [bp-010h], 000h                  ; 80 7e f0 00
    jne short 01a3fh                          ; 75 1f
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    push ax                                   ; 50
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    push ax                                   ; 50
    movzx cx, byte [bp-016h]                  ; 0f b6 4e ea
    movzx bx, byte [bp-012h]                  ; 0f b6 5e ee
    movzx dx, byte [bp-014h]                  ; 0f b6 56 ec
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    call 014e8h                               ; e8 ab fa
    jmp short 01a61h                          ; eb 22
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    push ax                                   ; 50
    movzx ax, byte [bp-016h]                  ; 0f b6 46 ea
    push ax                                   ; 50
    movzx cx, byte [bp-012h]                  ; 0f b6 4e ee
    mov al, byte [bp-014h]                    ; 8a 46 ec
    sub al, byte [bp-010h]                    ; 2a 46 f0
    movzx bx, al                              ; 0f b6 d8
    movzx dx, byte [bp-014h]                  ; 0f b6 56 ec
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    call 01444h                               ; e8 e3 f9
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    cmp ax, word [bp-014h]                    ; 3b 46 ec
    jc short 01a6fh                           ; 72 05
    dec word [bp-014h]                        ; ff 4e ec
    jmp short 01a02h                          ; eb 93
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00008h                               ; c2 08 00
write_gfx_char_pl4_:                         ; 0xc1a78 LB 0xeb
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0000ah                ; 83 ec 0a
    mov byte [bp-006h], dl                    ; 88 56 fa
    mov ah, bl                                ; 88 dc
    cmp byte [bp+006h], 010h                  ; 80 7e 06 10
    je short 01a96h                           ; 74 0b
    cmp byte [bp+006h], 00eh                  ; 80 7e 06 0e
    jne short 01a9bh                          ; 75 0a
    mov di, 05bech                            ; bf ec 5b
    jmp short 01a9eh                          ; eb 08
    mov di, 069ech                            ; bf ec 69
    jmp short 01a9eh                          ; eb 03
    mov di, 053ech                            ; bf ec 53
    movzx si, cl                              ; 0f b6 f1
    movzx bx, byte [bp+006h]                  ; 0f b6 5e 06
    imul si, bx                               ; 0f af f3
    movzx cx, byte [bp+004h]                  ; 0f b6 4e 04
    imul cx, si                               ; 0f af ce
    movzx si, ah                              ; 0f b6 f4
    add si, cx                                ; 01 ce
    mov word [bp-00eh], si                    ; 89 76 f2
    xor ah, ah                                ; 30 e4
    imul ax, bx                               ; 0f af c3
    mov word [bp-00ah], ax                    ; 89 46 f6
    mov ax, 00f02h                            ; b8 02 0f
    mov dx, 003c4h                            ; ba c4 03
    out DX, ax                                ; ef
    mov ax, 00205h                            ; b8 05 02
    mov dx, 003ceh                            ; ba ce 03
    out DX, ax                                ; ef
    test byte [bp-006h], 080h                 ; f6 46 fa 80
    je short 01ad9h                           ; 74 06
    mov ax, 01803h                            ; b8 03 18
    out DX, ax                                ; ef
    jmp short 01addh                          ; eb 04
    mov ax, strict word 00003h                ; b8 03 00
    out DX, ax                                ; ef
    xor ch, ch                                ; 30 ed
    cmp ch, byte [bp+006h]                    ; 3a 6e 06
    jnc short 01b4bh                          ; 73 67
    movzx si, ch                              ; 0f b6 f5
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    imul si, ax                               ; 0f af f0
    add si, word [bp-00eh]                    ; 03 76 f2
    mov byte [bp-008h], 000h                  ; c6 46 f8 00
    jmp short 01b0ah                          ; eb 13
    xor bx, bx                                ; 31 db
    mov dx, si                                ; 89 f2
    mov ax, 0a000h                            ; b8 00 a0
    call 02f4ch                               ; e8 4b 14
    inc byte [bp-008h]                        ; fe 46 f8
    cmp byte [bp-008h], 008h                  ; 80 7e f8 08
    jnc short 01b47h                          ; 73 3d
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    mov cl, al                                ; 88 c1
    mov ax, 00080h                            ; b8 80 00
    sar ax, CL                                ; d3 f8
    xor ah, ah                                ; 30 e4
    mov word [bp-00ch], ax                    ; 89 46 f4
    sal ax, 008h                              ; c1 e0 08
    or AL, strict byte 008h                   ; 0c 08
    mov dx, 003ceh                            ; ba ce 03
    out DX, ax                                ; ef
    mov dx, si                                ; 89 f2
    mov ax, 0a000h                            ; b8 00 a0
    call 02f3eh                               ; e8 13 14
    movzx ax, ch                              ; 0f b6 c5
    add ax, word [bp-00ah]                    ; 03 46 f6
    mov bx, di                                ; 89 fb
    add bx, ax                                ; 01 c3
    movzx ax, byte [bx]                       ; 0f b6 07
    test word [bp-00ch], ax                   ; 85 46 f4
    je short 01af7h                           ; 74 ba
    mov al, byte [bp-006h]                    ; 8a 46 fa
    and AL, strict byte 00fh                  ; 24 0f
    movzx bx, al                              ; 0f b6 d8
    jmp short 01af9h                          ; eb b2
    db  0feh, 0c5h
    ; inc ch                                    ; fe c5
    jmp short 01adfh                          ; eb 94
    mov ax, 0ff08h                            ; b8 08 ff
    mov dx, 003ceh                            ; ba ce 03
    out DX, ax                                ; ef
    mov ax, strict word 00005h                ; b8 05 00
    out DX, ax                                ; ef
    mov ax, strict word 00003h                ; b8 03 00
    out DX, ax                                ; ef
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00004h                               ; c2 04 00
write_gfx_char_cga_:                         ; 0xc1b63 LB 0x11e
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00008h                ; 83 ec 08
    mov byte [bp-008h], dl                    ; 88 56 f8
    mov si, 053ech                            ; be ec 53
    xor bh, bh                                ; 30 ff
    movzx di, byte [bp+006h]                  ; 0f b6 7e 06
    imul di, bx                               ; 0f af fb
    movzx bx, cl                              ; 0f b6 d9
    imul bx, bx, 00140h                       ; 69 db 40 01
    add di, bx                                ; 01 df
    mov word [bp-00ch], di                    ; 89 7e f4
    movzx di, al                              ; 0f b6 f8
    sal di, 003h                              ; c1 e7 03
    mov byte [bp-006h], 000h                  ; c6 46 fa 00
    jmp near 01be3h                           ; e9 50 00
    xor al, al                                ; 30 c0
    xor ah, ah                                ; 30 e4
    jmp short 01ba4h                          ; eb 0b
    or al, bl                                 ; 08 d8
    shr ch, 1                                 ; d0 ed
    db  0feh, 0c4h
    ; inc ah                                    ; fe c4
    cmp ah, 008h                              ; 80 fc 08
    jnc short 01bcch                          ; 73 28
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    add bx, di                                ; 01 fb
    add bx, si                                ; 01 f3
    movzx bx, byte [bx]                       ; 0f b6 1f
    movzx dx, ch                              ; 0f b6 d5
    test bx, dx                               ; 85 d3
    je short 01b9bh                           ; 74 e5
    mov CL, strict byte 007h                  ; b1 07
    sub cl, ah                                ; 28 e1
    mov bl, byte [bp-008h]                    ; 8a 5e f8
    and bl, 001h                              ; 80 e3 01
    sal bl, CL                                ; d2 e3
    test byte [bp-008h], 080h                 ; f6 46 f8 80
    je short 01b99h                           ; 74 d1
    xor al, bl                                ; 30 d8
    jmp short 01b9bh                          ; eb cf
    movzx bx, al                              ; 0f b6 d8
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    mov ax, 0b800h                            ; b8 00 b8
    call 02f4ch                               ; e8 74 13
    inc byte [bp-006h]                        ; fe 46 fa
    cmp byte [bp-006h], 008h                  ; 80 7e fa 08
    jnc near 01c78h                           ; 0f 83 95 00
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    sar ax, 1                                 ; d1 f8
    imul ax, ax, strict byte 00050h           ; 6b c0 50
    mov bx, word [bp-00ch]                    ; 8b 5e f4
    add bx, ax                                ; 01 c3
    mov word [bp-00ah], bx                    ; 89 5e f6
    test byte [bp-006h], 001h                 ; f6 46 fa 01
    je short 01bfeh                           ; 74 04
    add byte [bp-009h], 020h                  ; 80 46 f7 20
    mov CH, strict byte 080h                  ; b5 80
    cmp byte [bp+006h], 001h                  ; 80 7e 06 01
    jne short 01c17h                          ; 75 11
    test byte [bp-008h], ch                   ; 84 6e f8
    je short 01b93h                           ; 74 88
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    mov ax, 0b800h                            ; b8 00 b8
    call 02f3eh                               ; e8 2a 13
    jmp near 01b95h                           ; e9 7e ff
    test ch, ch                               ; 84 ed
    jbe short 01bd8h                          ; 76 bd
    test byte [bp-008h], 080h                 ; f6 46 f8 80
    je short 01c2ch                           ; 74 0b
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    mov ax, 0b800h                            ; b8 00 b8
    call 02f3eh                               ; e8 14 13
    jmp short 01c2eh                          ; eb 02
    xor al, al                                ; 30 c0
    xor ah, ah                                ; 30 e4
    jmp short 01c3dh                          ; eb 0b
    or al, bl                                 ; 08 d8
    shr ch, 1                                 ; d0 ed
    db  0feh, 0c4h
    ; inc ah                                    ; fe c4
    cmp ah, 004h                              ; 80 fc 04
    jnc short 01c67h                          ; 73 2a
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    add bx, di                                ; 01 fb
    add bx, si                                ; 01 f3
    movzx dx, byte [bx]                       ; 0f b6 17
    movzx bx, ch                              ; 0f b6 dd
    test bx, dx                               ; 85 d3
    je short 01c34h                           ; 74 e5
    mov CL, strict byte 003h                  ; b1 03
    sub cl, ah                                ; 28 e1
    mov bl, byte [bp-008h]                    ; 8a 5e f8
    and bl, 003h                              ; 80 e3 03
    add cl, cl                                ; 00 c9
    sal bl, CL                                ; d2 e3
    test byte [bp-008h], 080h                 ; f6 46 f8 80
    je short 01c32h                           ; 74 cf
    xor al, bl                                ; 30 d8
    jmp short 01c34h                          ; eb cd
    movzx bx, al                              ; 0f b6 d8
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    mov ax, 0b800h                            ; b8 00 b8
    call 02f4ch                               ; e8 d9 12
    inc word [bp-00ah]                        ; ff 46 f6
    jmp short 01c17h                          ; eb 9f
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00004h                               ; c2 04 00
write_gfx_char_lin_:                         ; 0xc1c81 LB 0x91
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00008h                ; 83 ec 08
    mov byte [bp-006h], dl                    ; 88 56 fa
    mov di, 053ech                            ; bf ec 53
    movzx dx, cl                              ; 0f b6 d1
    movzx cx, byte [bp+004h]                  ; 0f b6 4e 04
    imul cx, dx                               ; 0f af ca
    sal cx, 006h                              ; c1 e1 06
    movzx dx, bl                              ; 0f b6 d3
    sal dx, 003h                              ; c1 e2 03
    add dx, cx                                ; 01 ca
    mov word [bp-00ch], dx                    ; 89 56 f4
    movzx si, al                              ; 0f b6 f0
    sal si, 003h                              ; c1 e6 03
    xor cl, cl                                ; 30 c9
    jmp short 01cech                          ; eb 3b
    cmp ch, 008h                              ; 80 fd 08
    jnc short 01ce5h                          ; 73 2f
    xor al, al                                ; 30 c0
    movzx dx, cl                              ; 0f b6 d1
    add dx, si                                ; 01 f2
    mov bx, di                                ; 89 fb
    add bx, dx                                ; 01 d3
    movzx dx, byte [bx]                       ; 0f b6 17
    movzx bx, byte [bp-008h]                  ; 0f b6 5e f8
    test dx, bx                               ; 85 da
    je short 01ccfh                           ; 74 03
    mov al, byte [bp-006h]                    ; 8a 46 fa
    movzx bx, al                              ; 0f b6 d8
    movzx dx, ch                              ; 0f b6 d5
    add dx, word [bp-00ah]                    ; 03 56 f6
    mov ax, 0a000h                            ; b8 00 a0
    call 02f4ch                               ; e8 6e 12
    shr byte [bp-008h], 1                     ; d0 6e f8
    db  0feh, 0c5h
    ; inc ch                                    ; fe c5
    jmp short 01cb1h                          ; eb cc
    db  0feh, 0c1h
    ; inc cl                                    ; fe c1
    cmp cl, 008h                              ; 80 f9 08
    jnc short 01d09h                          ; 73 1d
    movzx bx, cl                              ; 0f b6 d9
    movzx dx, byte [bp+004h]                  ; 0f b6 56 04
    imul dx, bx                               ; 0f af d3
    sal dx, 003h                              ; c1 e2 03
    mov bx, word [bp-00ch]                    ; 8b 5e f4
    add bx, dx                                ; 01 d3
    mov word [bp-00ah], bx                    ; 89 5e f6
    mov byte [bp-008h], 080h                  ; c6 46 f8 80
    xor ch, ch                                ; 30 ed
    jmp short 01cb6h                          ; eb ad
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00002h                               ; c2 02 00
biosfn_write_char_attr_:                     ; 0xc1d12 LB 0x168
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00018h                ; 83 ec 18
    mov byte [bp-00eh], al                    ; 88 46 f2
    mov byte [bp-010h], dl                    ; 88 56 f0
    mov byte [bp-012h], bl                    ; 88 5e ee
    mov si, cx                                ; 89 ce
    mov dx, strict word 00049h                ; ba 49 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 10 12
    xor ah, ah                                ; 30 e4
    call 02f17h                               ; e8 e4 11
    mov cl, al                                ; 88 c1
    mov byte [bp-006h], al                    ; 88 46 fa
    cmp AL, strict byte 0ffh                  ; 3c ff
    je near 01e73h                            ; 0f 84 35 01
    movzx ax, byte [bp-010h]                  ; 0f b6 46 f0
    lea bx, [bp-01ch]                         ; 8d 5e e4
    lea dx, [bp-01ah]                         ; 8d 56 e6
    call 00a88h                               ; e8 3d ed
    mov al, byte [bp-01ch]                    ; 8a 46 e4
    mov byte [bp-00ch], al                    ; 88 46 f4
    mov ax, word [bp-01ch]                    ; 8b 46 e4
    xor al, al                                ; 30 c0
    shr ax, 008h                              ; c1 e8 08
    mov byte [bp-00ah], al                    ; 88 46 f6
    mov dx, 00084h                            ; ba 84 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 d9 11
    xor ah, ah                                ; 30 e4
    inc ax                                    ; 40
    mov word [bp-018h], ax                    ; 89 46 e8
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 e6 11
    mov word [bp-016h], ax                    ; 89 46 ea
    movzx bx, cl                              ; 0f b6 d9
    mov di, bx                                ; 89 df
    sal di, 003h                              ; c1 e7 03
    cmp byte [di+0462fh], 000h                ; 80 bd 2f 46 00
    jne short 01dcdh                          ; 75 47
    mov bx, word [bp-018h]                    ; 8b 5e e8
    imul bx, ax                               ; 0f af d8
    add bx, bx                                ; 01 db
    or bl, 0ffh                               ; 80 cb ff
    movzx dx, byte [bp-010h]                  ; 0f b6 56 f0
    inc bx                                    ; 43
    imul dx, bx                               ; 0f af d3
    movzx bx, byte [bp-00ah]                  ; 0f b6 5e f6
    imul ax, bx                               ; 0f af c3
    movzx bx, byte [bp-00ch]                  ; 0f b6 5e f4
    add ax, bx                                ; 01 d8
    add ax, ax                                ; 01 c0
    add dx, ax                                ; 01 c2
    movzx ax, byte [bp-012h]                  ; 0f b6 46 ee
    sal ax, 008h                              ; c1 e0 08
    movzx bx, byte [bp-00eh]                  ; 0f b6 5e f2
    add ax, bx                                ; 01 d8
    mov word [bp-01ah], ax                    ; 89 46 e6
    mov ax, word [bp-01ah]                    ; 8b 46 e6
    mov es, [di+04632h]                       ; 8e 85 32 46
    mov cx, si                                ; 89 f1
    mov di, dx                                ; 89 d7
    cld                                       ; fc
    jcxz 01dcah                               ; e3 02
    rep stosw                                 ; f3 ab
    jmp near 01e73h                           ; e9 a6 00
    movzx bx, byte [bx+046aeh]                ; 0f b6 9f ae 46
    sal bx, 006h                              ; c1 e3 06
    mov al, byte [bx+046c4h]                  ; 8a 87 c4 46
    mov byte [bp-008h], al                    ; 88 46 f8
    mov al, byte [di+04631h]                  ; 8a 85 31 46
    mov byte [bp-014h], al                    ; 88 46 ec
    dec si                                    ; 4e
    cmp si, strict byte 0ffffh                ; 83 fe ff
    je near 01e73h                            ; 0f 84 88 00
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    cmp ax, word [bp-016h]                    ; 3b 46 ea
    jnc near 01e73h                           ; 0f 83 7d 00
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    sal bx, 003h                              ; c1 e3 03
    mov al, byte [bx+04630h]                  ; 8a 87 30 46
    cmp AL, strict byte 003h                  ; 3c 03
    jc short 01e11h                           ; 72 0c
    jbe short 01e17h                          ; 76 10
    cmp AL, strict byte 005h                  ; 3c 05
    je short 01e55h                           ; 74 4a
    cmp AL, strict byte 004h                  ; 3c 04
    je short 01e17h                           ; 74 08
    jmp short 01e6dh                          ; eb 5c
    cmp AL, strict byte 002h                  ; 3c 02
    je short 01e36h                           ; 74 21
    jmp short 01e6dh                          ; eb 56
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    push ax                                   ; 50
    movzx ax, byte [bp-016h]                  ; 0f b6 46 ea
    push ax                                   ; 50
    movzx cx, byte [bp-00ah]                  ; 0f b6 4e f6
    movzx bx, byte [bp-00ch]                  ; 0f b6 5e f4
    movzx dx, byte [bp-012h]                  ; 0f b6 56 ee
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    call 01a78h                               ; e8 44 fc
    jmp short 01e6dh                          ; eb 37
    movzx ax, byte [bp-014h]                  ; 0f b6 46 ec
    push ax                                   ; 50
    movzx ax, byte [bp-016h]                  ; 0f b6 46 ea
    push ax                                   ; 50
    movzx cx, byte [bp-00ah]                  ; 0f b6 4e f6
    movzx bx, byte [bp-00ch]                  ; 0f b6 5e f4
    movzx dx, byte [bp-012h]                  ; 0f b6 56 ee
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    call 01b63h                               ; e8 10 fd
    jmp short 01e6dh                          ; eb 18
    movzx ax, byte [bp-016h]                  ; 0f b6 46 ea
    push ax                                   ; 50
    movzx cx, byte [bp-00ah]                  ; 0f b6 4e f6
    movzx bx, byte [bp-00ch]                  ; 0f b6 5e f4
    movzx dx, byte [bp-012h]                  ; 0f b6 56 ee
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    call 01c81h                               ; e8 14 fe
    inc byte [bp-00ch]                        ; fe 46 f4
    jmp near 01de3h                           ; e9 70 ff
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_write_char_only_:                     ; 0xc1e7a LB 0x16f
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00018h                ; 83 ec 18
    mov byte [bp-012h], al                    ; 88 46 ee
    mov byte [bp-006h], dl                    ; 88 56 fa
    mov byte [bp-014h], bl                    ; 88 5e ec
    mov si, cx                                ; 89 ce
    mov dx, strict word 00049h                ; ba 49 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 a8 10
    xor ah, ah                                ; 30 e4
    call 02f17h                               ; e8 7c 10
    mov cl, al                                ; 88 c1
    mov byte [bp-00eh], al                    ; 88 46 f2
    cmp AL, strict byte 0ffh                  ; 3c ff
    je near 01fe2h                            ; 0f 84 3c 01
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    lea bx, [bp-01ch]                         ; 8d 5e e4
    lea dx, [bp-01ah]                         ; 8d 56 e6
    call 00a88h                               ; e8 d5 eb
    mov al, byte [bp-01ch]                    ; 8a 46 e4
    mov byte [bp-00ch], al                    ; 88 46 f4
    mov ax, word [bp-01ch]                    ; 8b 46 e4
    xor al, al                                ; 30 c0
    shr ax, 008h                              ; c1 e8 08
    mov byte [bp-010h], al                    ; 88 46 f0
    mov dx, 00084h                            ; ba 84 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 71 10
    xor ah, ah                                ; 30 e4
    inc ax                                    ; 40
    mov word [bp-018h], ax                    ; 89 46 e8
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 7e 10
    mov word [bp-016h], ax                    ; 89 46 ea
    movzx di, cl                              ; 0f b6 f9
    mov bx, di                                ; 89 fb
    sal bx, 003h                              ; c1 e3 03
    cmp byte [bx+0462fh], 000h                ; 80 bf 2f 46 00
    jne short 01f38h                          ; 75 4a
    mov dx, word [bp-018h]                    ; 8b 56 e8
    imul dx, ax                               ; 0f af d0
    add dx, dx                                ; 01 d2
    or dl, 0ffh                               ; 80 ca ff
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    inc dx                                    ; 42
    imul bx, dx                               ; 0f af da
    movzx dx, byte [bp-010h]                  ; 0f b6 56 f0
    mov cx, ax                                ; 89 c1
    imul cx, dx                               ; 0f af ca
    movzx dx, byte [bp-00ch]                  ; 0f b6 56 f4
    add cx, dx                                ; 01 d1
    add cx, cx                                ; 01 c9
    add cx, bx                                ; 01 d9
    dec si                                    ; 4e
    cmp si, strict byte 0ffffh                ; 83 fe ff
    je near 01fe2h                            ; 0f 84 c6 00
    movzx ax, byte [bp-012h]                  ; 0f b6 46 ee
    movzx bx, byte [bp-00eh]                  ; 0f b6 5e f2
    sal bx, 003h                              ; c1 e3 03
    mov di, word [bx+04632h]                  ; 8b bf 32 46
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    mov ax, di                                ; 89 f8
    call 02f4ch                               ; e8 18 10
    inc cx                                    ; 41
    inc cx                                    ; 41
    jmp short 01f14h                          ; eb dc
    movzx di, byte [di+046aeh]                ; 0f b6 bd ae 46
    sal di, 006h                              ; c1 e7 06
    mov al, byte [di+046c4h]                  ; 8a 85 c4 46
    mov byte [bp-00ah], al                    ; 88 46 f6
    mov al, byte [bx+04631h]                  ; 8a 87 31 46
    mov byte [bp-008h], al                    ; 88 46 f8
    dec si                                    ; 4e
    cmp si, strict byte 0ffffh                ; 83 fe ff
    je near 01fe2h                            ; 0f 84 8c 00
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    cmp ax, word [bp-016h]                    ; 3b 46 ea
    jnc near 01fe2h                           ; 0f 83 81 00
    movzx bx, byte [bp-00eh]                  ; 0f b6 5e f2
    sal bx, 003h                              ; c1 e3 03
    mov bl, byte [bx+04630h]                  ; 8a 9f 30 46
    cmp bl, 003h                              ; 80 fb 03
    jc short 01f7fh                           ; 72 0e
    jbe short 01f86h                          ; 76 13
    cmp bl, 005h                              ; 80 fb 05
    je short 01fc4h                           ; 74 4c
    cmp bl, 004h                              ; 80 fb 04
    je short 01f86h                           ; 74 09
    jmp short 01fdch                          ; eb 5d
    cmp bl, 002h                              ; 80 fb 02
    je short 01fa5h                           ; 74 21
    jmp short 01fdch                          ; eb 56
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    push ax                                   ; 50
    movzx ax, byte [bp-016h]                  ; 0f b6 46 ea
    push ax                                   ; 50
    movzx cx, byte [bp-010h]                  ; 0f b6 4e f0
    movzx bx, byte [bp-00ch]                  ; 0f b6 5e f4
    movzx dx, byte [bp-014h]                  ; 0f b6 56 ec
    movzx ax, byte [bp-012h]                  ; 0f b6 46 ee
    call 01a78h                               ; e8 d5 fa
    jmp short 01fdch                          ; eb 37
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    push ax                                   ; 50
    movzx ax, byte [bp-016h]                  ; 0f b6 46 ea
    push ax                                   ; 50
    movzx cx, byte [bp-010h]                  ; 0f b6 4e f0
    movzx bx, byte [bp-00ch]                  ; 0f b6 5e f4
    movzx dx, byte [bp-014h]                  ; 0f b6 56 ec
    movzx ax, byte [bp-012h]                  ; 0f b6 46 ee
    call 01b63h                               ; e8 a1 fb
    jmp short 01fdch                          ; eb 18
    movzx ax, byte [bp-016h]                  ; 0f b6 46 ea
    push ax                                   ; 50
    movzx cx, byte [bp-010h]                  ; 0f b6 4e f0
    movzx bx, byte [bp-00ch]                  ; 0f b6 5e f4
    movzx dx, byte [bp-014h]                  ; 0f b6 56 ec
    movzx ax, byte [bp-012h]                  ; 0f b6 46 ee
    call 01c81h                               ; e8 a5 fc
    inc byte [bp-00ch]                        ; fe 46 f4
    jmp near 01f4eh                           ; e9 6c ff
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_write_pixel_:                         ; 0xc1fe9 LB 0x16a
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    sub sp, strict byte 00008h                ; 83 ec 08
    mov byte [bp-006h], dl                    ; 88 56 fa
    mov word [bp-00ah], bx                    ; 89 5e f6
    mov dx, strict word 00049h                ; ba 49 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 3f 0f
    xor ah, ah                                ; 30 e4
    call 02f17h                               ; e8 13 0f
    mov byte [bp-004h], al                    ; 88 46 fc
    cmp AL, strict byte 0ffh                  ; 3c ff
    je near 0212bh                            ; 0f 84 1e 01
    movzx bx, al                              ; 0f b6 d8
    sal bx, 003h                              ; c1 e3 03
    cmp byte [bx+0462fh], 000h                ; 80 bf 2f 46 00
    je near 0212bh                            ; 0f 84 0f 01
    mov al, byte [bx+04630h]                  ; 8a 87 30 46
    cmp AL, strict byte 003h                  ; 3c 03
    jc short 02033h                           ; 72 0f
    jbe short 0203ah                          ; 76 14
    cmp AL, strict byte 005h                  ; 3c 05
    je near 02131h                            ; 0f 84 05 01
    cmp AL, strict byte 004h                  ; 3c 04
    je short 0203ah                           ; 74 0a
    jmp near 0212bh                           ; e9 f8 00
    cmp AL, strict byte 002h                  ; 3c 02
    je short 0209fh                           ; 74 68
    jmp near 0212bh                           ; e9 f1 00
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 17 0f
    imul ax, cx                               ; 0f af c1
    mov bx, word [bp-00ah]                    ; 8b 5e f6
    shr bx, 003h                              ; c1 eb 03
    add bx, ax                                ; 01 c3
    mov word [bp-008h], bx                    ; 89 5e f8
    mov cx, word [bp-00ah]                    ; 8b 4e f6
    and cl, 007h                              ; 80 e1 07
    mov ax, 00080h                            ; b8 80 00
    sar ax, CL                                ; d3 f8
    xor ah, ah                                ; 30 e4
    sal ax, 008h                              ; c1 e0 08
    or AL, strict byte 008h                   ; 0c 08
    mov dx, 003ceh                            ; ba ce 03
    out DX, ax                                ; ef
    mov ax, 00205h                            ; b8 05 02
    out DX, ax                                ; ef
    mov dx, bx                                ; 89 da
    mov ax, 0a000h                            ; b8 00 a0
    call 02f3eh                               ; e8 cb 0e
    test byte [bp-006h], 080h                 ; f6 46 fa 80
    je short 02080h                           ; 74 07
    mov ax, 01803h                            ; b8 03 18
    mov dx, 003ceh                            ; ba ce 03
    out DX, ax                                ; ef
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    mov dx, word [bp-008h]                    ; 8b 56 f8
    mov ax, 0a000h                            ; b8 00 a0
    call 02f4ch                               ; e8 bf 0e
    mov ax, 0ff08h                            ; b8 08 ff
    mov dx, 003ceh                            ; ba ce 03
    out DX, ax                                ; ef
    mov ax, strict word 00005h                ; b8 05 00
    out DX, ax                                ; ef
    mov ax, strict word 00003h                ; b8 03 00
    out DX, ax                                ; ef
    jmp near 0212bh                           ; e9 8c 00
    mov ax, cx                                ; 89 c8
    shr ax, 1                                 ; d1 e8
    imul ax, ax, strict byte 00050h           ; 6b c0 50
    cmp byte [bx+04631h], 002h                ; 80 bf 31 46 02
    jne short 020b5h                          ; 75 08
    mov bx, word [bp-00ah]                    ; 8b 5e f6
    shr bx, 002h                              ; c1 eb 02
    jmp short 020bbh                          ; eb 06
    mov bx, word [bp-00ah]                    ; 8b 5e f6
    shr bx, 003h                              ; c1 eb 03
    add bx, ax                                ; 01 c3
    mov word [bp-008h], bx                    ; 89 5e f8
    test cl, 001h                             ; f6 c1 01
    je short 020c9h                           ; 74 04
    add byte [bp-007h], 020h                  ; 80 46 f9 20
    mov dx, word [bp-008h]                    ; 8b 56 f8
    mov ax, 0b800h                            ; b8 00 b8
    call 02f3eh                               ; e8 6c 0e
    mov bl, al                                ; 88 c3
    movzx si, byte [bp-004h]                  ; 0f b6 76 fc
    sal si, 003h                              ; c1 e6 03
    cmp byte [si+04631h], 002h                ; 80 bc 31 46 02
    jne short 020fbh                          ; 75 19
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    and AL, strict byte 003h                  ; 24 03
    mov AH, strict byte 003h                  ; b4 03
    sub ah, al                                ; 28 c4
    mov cl, ah                                ; 88 e1
    add cl, ah                                ; 00 e1
    mov bh, byte [bp-006h]                    ; 8a 7e fa
    and bh, 003h                              ; 80 e7 03
    sal bh, CL                                ; d2 e7
    mov AL, strict byte 003h                  ; b0 03
    jmp short 0210eh                          ; eb 13
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    and AL, strict byte 007h                  ; 24 07
    mov CL, strict byte 007h                  ; b1 07
    sub cl, al                                ; 28 c1
    mov bh, byte [bp-006h]                    ; 8a 7e fa
    and bh, 001h                              ; 80 e7 01
    sal bh, CL                                ; d2 e7
    mov AL, strict byte 001h                  ; b0 01
    sal al, CL                                ; d2 e0
    test byte [bp-006h], 080h                 ; f6 46 fa 80
    je short 0211ah                           ; 74 04
    xor bl, bh                                ; 30 fb
    jmp short 02120h                          ; eb 06
    not al                                    ; f6 d0
    and bl, al                                ; 20 c3
    or bl, bh                                 ; 08 fb
    xor bh, bh                                ; 30 ff
    mov dx, word [bp-008h]                    ; 8b 56 f8
    mov ax, 0b800h                            ; b8 00 b8
    call 02f4ch                               ; e8 21 0e
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 20 0e
    sal ax, 003h                              ; c1 e0 03
    imul cx, ax                               ; 0f af c8
    mov ax, word [bp-00ah]                    ; 8b 46 f6
    add ax, cx                                ; 01 c8
    mov word [bp-008h], ax                    ; 89 46 f8
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    mov dx, ax                                ; 89 c2
    mov ax, 0a000h                            ; b8 00 a0
    jmp short 02128h                          ; eb d5
biosfn_write_teletype_:                      ; 0xc2153 LB 0x241
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    sub sp, strict byte 00016h                ; 83 ec 16
    mov byte [bp-00ch], al                    ; 88 46 f4
    mov byte [bp-006h], dl                    ; 88 56 fa
    mov byte [bp-004h], bl                    ; 88 5e fc
    mov byte [bp-00eh], cl                    ; 88 4e f2
    cmp dl, 0ffh                              ; 80 fa ff
    jne short 02177h                          ; 75 0c
    mov dx, strict word 00062h                ; ba 62 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 ca 0d
    mov byte [bp-006h], al                    ; 88 46 fa
    mov dx, strict word 00049h                ; ba 49 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 be 0d
    xor ah, ah                                ; 30 e4
    call 02f17h                               ; e8 92 0d
    mov byte [bp-010h], al                    ; 88 46 f0
    cmp AL, strict byte 0ffh                  ; 3c ff
    je near 0238eh                            ; 0f 84 00 02
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    lea bx, [bp-018h]                         ; 8d 5e e8
    lea dx, [bp-016h]                         ; 8d 56 ea
    call 00a88h                               ; e8 ed e8
    mov al, byte [bp-018h]                    ; 8a 46 e8
    mov byte [bp-008h], al                    ; 88 46 f8
    mov ax, word [bp-018h]                    ; 8b 46 e8
    xor al, al                                ; 30 c0
    shr ax, 008h                              ; c1 e8 08
    mov byte [bp-00ah], al                    ; 88 46 f6
    mov dx, 00084h                            ; ba 84 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 89 0d
    xor ah, ah                                ; 30 e4
    inc ax                                    ; 40
    mov word [bp-014h], ax                    ; 89 46 ec
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 96 0d
    mov word [bp-012h], ax                    ; 89 46 ee
    mov al, byte [bp-00ch]                    ; 8a 46 f4
    cmp AL, strict byte 008h                  ; 3c 08
    jc short 021dch                           ; 72 0e
    jbe short 021e4h                          ; 76 14
    cmp AL, strict byte 00dh                  ; 3c 0d
    je short 021f2h                           ; 74 1e
    cmp AL, strict byte 00ah                  ; 3c 0a
    je near 022e4h                            ; 0f 84 0a 01
    jmp short 021f9h                          ; eb 1d
    cmp AL, strict byte 007h                  ; 3c 07
    je near 022e7h                            ; 0f 84 05 01
    jmp short 021f9h                          ; eb 15
    cmp byte [bp-008h], 000h                  ; 80 7e f8 00
    jbe near 022e7h                           ; 0f 86 fb 00
    dec byte [bp-008h]                        ; fe 4e f8
    jmp near 022e7h                           ; e9 f5 00
    mov byte [bp-008h], 000h                  ; c6 46 f8 00
    jmp near 022e7h                           ; e9 ee 00
    movzx bx, byte [bp-010h]                  ; 0f b6 5e f0
    mov si, bx                                ; 89 de
    sal si, 003h                              ; c1 e6 03
    cmp byte [si+0462fh], 000h                ; 80 bc 2f 46 00
    jne short 02256h                          ; 75 4d
    mov ax, word [bp-012h]                    ; 8b 46 ee
    imul ax, word [bp-014h]                   ; 0f af 46 ec
    add ax, ax                                ; 01 c0
    or AL, strict byte 0ffh                   ; 0c ff
    movzx dx, byte [bp-006h]                  ; 0f b6 56 fa
    mov cx, ax                                ; 89 c1
    inc cx                                    ; 41
    imul cx, dx                               ; 0f af ca
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    imul ax, word [bp-012h]                   ; 0f af 46 ee
    movzx dx, byte [bp-008h]                  ; 0f b6 56 f8
    add ax, dx                                ; 01 d0
    add ax, ax                                ; 01 c0
    add cx, ax                                ; 01 c1
    movzx bx, byte [bp-00ch]                  ; 0f b6 5e f4
    mov ax, word [si+04632h]                  ; 8b 84 32 46
    mov dx, cx                                ; 89 ca
    call 02f4ch                               ; e8 0f 0d
    cmp byte [bp-00eh], 003h                  ; 80 7e f2 03
    jne near 022d4h                           ; 0f 85 8f 00
    movzx bx, byte [bp-004h]                  ; 0f b6 5e fc
    mov dx, cx                                ; 89 ca
    inc dx                                    ; 42
    mov ax, word [si+04632h]                  ; 8b 84 32 46
    call 02f4ch                               ; e8 f9 0c
    jmp near 022d4h                           ; e9 7e 00
    movzx bx, byte [bx+046aeh]                ; 0f b6 9f ae 46
    sal bx, 006h                              ; c1 e3 06
    mov ah, byte [bx+046c4h]                  ; 8a a7 c4 46
    mov dl, byte [si+04631h]                  ; 8a 94 31 46
    mov al, byte [si+04630h]                  ; 8a 84 30 46
    cmp AL, strict byte 003h                  ; 3c 03
    jc short 0227ah                           ; 72 0c
    jbe short 02280h                          ; 76 10
    cmp AL, strict byte 005h                  ; 3c 05
    je short 022bch                           ; 74 48
    cmp AL, strict byte 004h                  ; 3c 04
    je short 02280h                           ; 74 08
    jmp short 022d4h                          ; eb 5a
    cmp AL, strict byte 002h                  ; 3c 02
    je short 0229eh                           ; 74 20
    jmp short 022d4h                          ; eb 54
    movzx ax, ah                              ; 0f b6 c4
    push ax                                   ; 50
    movzx ax, byte [bp-012h]                  ; 0f b6 46 ee
    push ax                                   ; 50
    movzx cx, byte [bp-00ah]                  ; 0f b6 4e f6
    movzx bx, byte [bp-008h]                  ; 0f b6 5e f8
    movzx dx, byte [bp-004h]                  ; 0f b6 56 fc
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    call 01a78h                               ; e8 dc f7
    jmp short 022d4h                          ; eb 36
    movzx ax, dl                              ; 0f b6 c2
    push ax                                   ; 50
    movzx ax, byte [bp-012h]                  ; 0f b6 46 ee
    push ax                                   ; 50
    movzx cx, byte [bp-00ah]                  ; 0f b6 4e f6
    movzx bx, byte [bp-008h]                  ; 0f b6 5e f8
    movzx dx, byte [bp-004h]                  ; 0f b6 56 fc
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    call 01b63h                               ; e8 a9 f8
    jmp short 022d4h                          ; eb 18
    movzx ax, byte [bp-012h]                  ; 0f b6 46 ee
    push ax                                   ; 50
    movzx cx, byte [bp-00ah]                  ; 0f b6 4e f6
    movzx bx, byte [bp-008h]                  ; 0f b6 5e f8
    movzx dx, byte [bp-004h]                  ; 0f b6 56 fc
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    call 01c81h                               ; e8 ad f9
    inc byte [bp-008h]                        ; fe 46 f8
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    cmp ax, word [bp-012h]                    ; 3b 46 ee
    jne short 022e7h                          ; 75 07
    mov byte [bp-008h], 000h                  ; c6 46 f8 00
    inc byte [bp-00ah]                        ; fe 46 f6
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    cmp ax, word [bp-014h]                    ; 3b 46 ec
    jne near 02372h                           ; 0f 85 80 00
    movzx si, byte [bp-010h]                  ; 0f b6 76 f0
    sal si, 003h                              ; c1 e6 03
    mov bh, byte [bp-014h]                    ; 8a 7e ec
    db  0feh, 0cfh
    ; dec bh                                    ; fe cf
    mov bl, byte [bp-012h]                    ; 8a 5e ee
    db  0feh, 0cbh
    ; dec bl                                    ; fe cb
    cmp byte [si+0462fh], 000h                ; 80 bc 2f 46 00
    jne short 02354h                          ; 75 4a
    mov ax, word [bp-012h]                    ; 8b 46 ee
    imul ax, word [bp-014h]                   ; 0f af 46 ec
    add ax, ax                                ; 01 c0
    or AL, strict byte 0ffh                   ; 0c ff
    movzx dx, byte [bp-006h]                  ; 0f b6 56 fa
    mov cx, ax                                ; 89 c1
    inc cx                                    ; 41
    imul cx, dx                               ; 0f af ca
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    dec ax                                    ; 48
    imul ax, word [bp-012h]                   ; 0f af 46 ee
    movzx dx, byte [bp-008h]                  ; 0f b6 56 f8
    add ax, dx                                ; 01 d0
    add ax, ax                                ; 01 c0
    mov dx, cx                                ; 89 ca
    add dx, ax                                ; 01 c2
    inc dx                                    ; 42
    mov ax, word [si+04632h]                  ; 8b 84 32 46
    call 02f3eh                               ; e8 02 0c
    push strict byte 00001h                   ; 6a 01
    movzx dx, byte [bp-006h]                  ; 0f b6 56 fa
    push dx                                   ; 52
    movzx dx, bl                              ; 0f b6 d3
    push dx                                   ; 52
    movzx dx, bh                              ; 0f b6 d7
    push dx                                   ; 52
    movzx dx, al                              ; 0f b6 d0
    xor cx, cx                                ; 31 c9
    xor bx, bx                                ; 31 db
    jmp short 02369h                          ; eb 15
    push strict byte 00001h                   ; 6a 01
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    push ax                                   ; 50
    movzx ax, bl                              ; 0f b6 c3
    push ax                                   ; 50
    movzx ax, bh                              ; 0f b6 c7
    push ax                                   ; 50
    xor cx, cx                                ; 31 c9
    xor bx, bx                                ; 31 db
    xor dx, dx                                ; 31 d2
    mov ax, strict word 00001h                ; b8 01 00
    call 01572h                               ; e8 03 f2
    dec byte [bp-00ah]                        ; fe 4e f6
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    mov word [bp-018h], ax                    ; 89 46 e8
    sal word [bp-018h], 008h                  ; c1 66 e8 08
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    add word [bp-018h], ax                    ; 01 46 e8
    mov dx, word [bp-018h]                    ; 8b 56 e8
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    call 00e5eh                               ; e8 d0 ea
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
get_font_access_:                            ; 0xc2394 LB 0x2c
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push dx                                   ; 52
    mov ax, 00100h                            ; b8 00 01
    mov dx, 003c4h                            ; ba c4 03
    out DX, ax                                ; ef
    mov ax, 00402h                            ; b8 02 04
    out DX, ax                                ; ef
    mov ax, 00704h                            ; b8 04 07
    out DX, ax                                ; ef
    mov ax, 00300h                            ; b8 00 03
    out DX, ax                                ; ef
    mov ax, 00204h                            ; b8 04 02
    mov dx, 003ceh                            ; ba ce 03
    out DX, ax                                ; ef
    mov ax, strict word 00005h                ; b8 05 00
    out DX, ax                                ; ef
    mov ax, 00406h                            ; b8 06 04
    out DX, ax                                ; ef
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop dx                                    ; 5a
    pop bp                                    ; 5d
    retn                                      ; c3
release_font_access_:                        ; 0xc23c0 LB 0x3c
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push dx                                   ; 52
    mov ax, 00100h                            ; b8 00 01
    mov dx, 003c4h                            ; ba c4 03
    out DX, ax                                ; ef
    mov ax, 00302h                            ; b8 02 03
    out DX, ax                                ; ef
    mov ax, 00304h                            ; b8 04 03
    out DX, ax                                ; ef
    mov ax, 00300h                            ; b8 00 03
    out DX, ax                                ; ef
    mov dx, 003cch                            ; ba cc 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and ax, strict word 00001h                ; 25 01 00
    sal ax, 002h                              ; c1 e0 02
    or AL, strict byte 00ah                   ; 0c 0a
    sal ax, 008h                              ; c1 e0 08
    or AL, strict byte 006h                   ; 0c 06
    mov dx, 003ceh                            ; ba ce 03
    out DX, ax                                ; ef
    mov ax, strict word 00004h                ; b8 04 00
    out DX, ax                                ; ef
    mov ax, 01005h                            ; b8 05 10
    out DX, ax                                ; ef
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop dx                                    ; 5a
    pop bp                                    ; 5d
    retn                                      ; c3
set_scan_lines_:                             ; 0xc23fc LB 0xbf
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    push si                                   ; 56
    push di                                   ; 57
    mov bl, al                                ; 88 c3
    mov dx, strict word 00063h                ; ba 63 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 4b 0b
    mov dx, ax                                ; 89 c2
    mov si, ax                                ; 89 c6
    mov AL, strict byte 009h                  ; b0 09
    out DX, AL                                ; ee
    inc dx                                    ; 42
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov ah, al                                ; 88 c4
    and ah, 0e0h                              ; 80 e4 e0
    mov al, bl                                ; 88 d8
    db  0feh, 0c8h
    ; dec al                                    ; fe c8
    or al, ah                                 ; 08 e0
    out DX, AL                                ; ee
    cmp bl, 008h                              ; 80 fb 08
    jne short 02433h                          ; 75 08
    mov dx, strict word 00007h                ; ba 07 00
    mov ax, strict word 00006h                ; b8 06 00
    jmp short 02440h                          ; eb 0d
    mov al, bl                                ; 88 d8
    sub AL, strict byte 003h                  ; 2c 03
    movzx dx, al                              ; 0f b6 d0
    mov al, bl                                ; 88 d8
    sub AL, strict byte 004h                  ; 2c 04
    xor ah, ah                                ; 30 e4
    call 00dbah                               ; e8 77 e9
    movzx di, bl                              ; 0f b6 fb
    mov bx, di                                ; 89 fb
    mov dx, 00085h                            ; ba 85 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 17 0b
    mov AL, strict byte 012h                  ; b0 12
    mov dx, si                                ; 89 f2
    out DX, AL                                ; ee
    lea cx, [si+001h]                         ; 8d 4c 01
    mov dx, cx                                ; 89 ca
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov bx, ax                                ; 89 c3
    mov AL, strict byte 007h                  ; b0 07
    mov dx, si                                ; 89 f2
    out DX, AL                                ; ee
    mov dx, cx                                ; 89 ca
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov ah, al                                ; 88 c4
    and ah, 002h                              ; 80 e4 02
    movzx dx, ah                              ; 0f b6 d4
    sal dx, 007h                              ; c1 e2 07
    and AL, strict byte 040h                  ; 24 40
    xor ah, ah                                ; 30 e4
    sal ax, 003h                              ; c1 e0 03
    add ax, dx                                ; 01 d0
    inc ax                                    ; 40
    add ax, bx                                ; 01 d8
    xor dx, dx                                ; 31 d2
    div di                                    ; f7 f7
    mov cx, ax                                ; 89 c1
    db  0feh, 0c8h
    ; dec al                                    ; fe c8
    movzx bx, al                              ; 0f b6 d8
    mov dx, 00084h                            ; ba 84 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 b7 0a
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 bc 0a
    movzx dx, cl                              ; 0f b6 d1
    mov bx, ax                                ; 89 c3
    imul bx, dx                               ; 0f af da
    add bx, bx                                ; 01 db
    mov dx, strict word 0004ch                ; ba 4c 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 b7 0a
    lea sp, [bp-00ah]                         ; 8d 66 f6
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_load_text_user_pat_:                  ; 0xc24bb LB 0x7d
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0000ah                ; 83 ec 0a
    mov byte [bp-006h], al                    ; 88 46 fa
    mov word [bp-00ch], dx                    ; 89 56 f4
    mov word [bp-008h], bx                    ; 89 5e f8
    mov word [bp-00ah], cx                    ; 89 4e f6
    call 02394h                               ; e8 c2 fe
    mov al, byte [bp+006h]                    ; 8a 46 06
    and AL, strict byte 003h                  ; 24 03
    xor ah, ah                                ; 30 e4
    mov bx, ax                                ; 89 c3
    sal bx, 00eh                              ; c1 e3 0e
    mov al, byte [bp+006h]                    ; 8a 46 06
    and AL, strict byte 004h                  ; 24 04
    xor ah, ah                                ; 30 e4
    sal ax, 00bh                              ; c1 e0 0b
    add bx, ax                                ; 01 c3
    mov word [bp-00eh], bx                    ; 89 5e f2
    xor bx, bx                                ; 31 db
    cmp bx, word [bp-00ah]                    ; 3b 5e f6
    jnc short 0251fh                          ; 73 2b
    movzx cx, byte [bp+008h]                  ; 0f b6 4e 08
    mov si, bx                                ; 89 de
    imul si, cx                               ; 0f af f1
    add si, word [bp-008h]                    ; 03 76 f8
    mov di, word [bp+004h]                    ; 8b 7e 04
    add di, bx                                ; 01 df
    sal di, 005h                              ; c1 e7 05
    add di, word [bp-00eh]                    ; 03 7e f2
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    mov ax, 0a000h                            ; b8 00 a0
    mov es, ax                                ; 8e c0
    cld                                       ; fc
    jcxz 0251ch                               ; e3 06
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsb                                 ; f3 a4
    pop DS                                    ; 1f
    inc bx                                    ; 43
    jmp short 024efh                          ; eb d0
    call 023c0h                               ; e8 9e fe
    cmp byte [bp-006h], 010h                  ; 80 7e fa 10
    jc short 0252fh                           ; 72 07
    movzx ax, byte [bp+008h]                  ; 0f b6 46 08
    call 023fch                               ; e8 cd fe
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00006h                               ; c2 06 00
biosfn_load_text_8_14_pat_:                  ; 0xc2538 LB 0x70
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov byte [bp-00ah], al                    ; 88 46 f6
    call 02394h                               ; e8 4d fe
    mov al, dl                                ; 88 d0
    and AL, strict byte 003h                  ; 24 03
    xor ah, ah                                ; 30 e4
    mov bx, ax                                ; 89 c3
    sal bx, 00eh                              ; c1 e3 0e
    mov al, dl                                ; 88 d0
    and AL, strict byte 004h                  ; 24 04
    xor ah, ah                                ; 30 e4
    sal ax, 00bh                              ; c1 e0 0b
    add bx, ax                                ; 01 c3
    mov word [bp-00ch], bx                    ; 89 5e f4
    xor bx, bx                                ; 31 db
    jmp short 0256ah                          ; eb 06
    cmp bx, 00100h                            ; 81 fb 00 01
    jnc short 02590h                          ; 73 26
    imul si, bx, strict byte 0000eh           ; 6b f3 0e
    mov di, bx                                ; 89 df
    sal di, 005h                              ; c1 e7 05
    add di, word [bp-00ch]                    ; 03 7e f4
    add si, 05bech                            ; 81 c6 ec 5b
    mov cx, strict word 0000eh                ; b9 0e 00
    mov dx, 0c000h                            ; ba 00 c0
    mov ax, 0a000h                            ; b8 00 a0
    mov es, ax                                ; 8e c0
    cld                                       ; fc
    jcxz 0258dh                               ; e3 06
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsb                                 ; f3 a4
    pop DS                                    ; 1f
    inc bx                                    ; 43
    jmp short 02564h                          ; eb d4
    call 023c0h                               ; e8 2d fe
    cmp byte [bp-00ah], 010h                  ; 80 7e f6 10
    jc short 0259fh                           ; 72 06
    mov ax, strict word 0000eh                ; b8 0e 00
    call 023fch                               ; e8 5d fe
    lea sp, [bp-008h]                         ; 8d 66 f8
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_load_text_8_8_pat_:                   ; 0xc25a8 LB 0x72
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov byte [bp-00ah], al                    ; 88 46 f6
    call 02394h                               ; e8 dd fd
    mov al, dl                                ; 88 d0
    and AL, strict byte 003h                  ; 24 03
    xor ah, ah                                ; 30 e4
    mov bx, ax                                ; 89 c3
    sal bx, 00eh                              ; c1 e3 0e
    mov al, dl                                ; 88 d0
    and AL, strict byte 004h                  ; 24 04
    xor ah, ah                                ; 30 e4
    sal ax, 00bh                              ; c1 e0 0b
    add bx, ax                                ; 01 c3
    mov word [bp-00ch], bx                    ; 89 5e f4
    xor bx, bx                                ; 31 db
    jmp short 025dah                          ; eb 06
    cmp bx, 00100h                            ; 81 fb 00 01
    jnc short 02602h                          ; 73 28
    mov si, bx                                ; 89 de
    sal si, 003h                              ; c1 e6 03
    mov di, bx                                ; 89 df
    sal di, 005h                              ; c1 e7 05
    add di, word [bp-00ch]                    ; 03 7e f4
    add si, 053ech                            ; 81 c6 ec 53
    mov cx, strict word 00008h                ; b9 08 00
    mov dx, 0c000h                            ; ba 00 c0
    mov ax, 0a000h                            ; b8 00 a0
    mov es, ax                                ; 8e c0
    cld                                       ; fc
    jcxz 025ffh                               ; e3 06
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsb                                 ; f3 a4
    pop DS                                    ; 1f
    inc bx                                    ; 43
    jmp short 025d4h                          ; eb d2
    call 023c0h                               ; e8 bb fd
    cmp byte [bp-00ah], 010h                  ; 80 7e f6 10
    jc short 02611h                           ; 72 06
    mov ax, strict word 00008h                ; b8 08 00
    call 023fch                               ; e8 eb fd
    lea sp, [bp-008h]                         ; 8d 66 f8
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_load_text_8_16_pat_:                  ; 0xc261a LB 0x72
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov byte [bp-00ah], al                    ; 88 46 f6
    call 02394h                               ; e8 6b fd
    mov al, dl                                ; 88 d0
    and AL, strict byte 003h                  ; 24 03
    xor ah, ah                                ; 30 e4
    mov bx, ax                                ; 89 c3
    sal bx, 00eh                              ; c1 e3 0e
    mov al, dl                                ; 88 d0
    and AL, strict byte 004h                  ; 24 04
    xor ah, ah                                ; 30 e4
    sal ax, 00bh                              ; c1 e0 0b
    add bx, ax                                ; 01 c3
    mov word [bp-00ch], bx                    ; 89 5e f4
    xor bx, bx                                ; 31 db
    jmp short 0264ch                          ; eb 06
    cmp bx, 00100h                            ; 81 fb 00 01
    jnc short 02674h                          ; 73 28
    mov si, bx                                ; 89 de
    sal si, 004h                              ; c1 e6 04
    mov di, bx                                ; 89 df
    sal di, 005h                              ; c1 e7 05
    add di, word [bp-00ch]                    ; 03 7e f4
    add si, 069ech                            ; 81 c6 ec 69
    mov cx, strict word 00010h                ; b9 10 00
    mov dx, 0c000h                            ; ba 00 c0
    mov ax, 0a000h                            ; b8 00 a0
    mov es, ax                                ; 8e c0
    cld                                       ; fc
    jcxz 02671h                               ; e3 06
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsb                                 ; f3 a4
    pop DS                                    ; 1f
    inc bx                                    ; 43
    jmp short 02646h                          ; eb d2
    call 023c0h                               ; e8 49 fd
    cmp byte [bp-00ah], 010h                  ; 80 7e f6 10
    jc short 02683h                           ; 72 06
    mov ax, strict word 00010h                ; b8 10 00
    call 023fch                               ; e8 79 fd
    lea sp, [bp-008h]                         ; 8d 66 f8
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_load_gfx_8_8_chars_:                  ; 0xc268c LB 0x5
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_load_gfx_user_chars_:                 ; 0xc2691 LB 0x7
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    pop bp                                    ; 5d
    retn 00002h                               ; c2 02 00
biosfn_load_gfx_8_14_chars_:                 ; 0xc2698 LB 0x5
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_load_gfx_8_8_dd_chars_:               ; 0xc269d LB 0x5
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_load_gfx_8_16_chars_:                 ; 0xc26a2 LB 0x5
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_alternate_prtsc_:                     ; 0xc26a7 LB 0x5
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_switch_video_interface_:              ; 0xc26ac LB 0x5
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_enable_video_refresh_control_:        ; 0xc26b1 LB 0x5
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_write_string_:                        ; 0xc26b6 LB 0x9c
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0000ah                ; 83 ec 0a
    mov byte [bp-00ah], al                    ; 88 46 f6
    mov byte [bp-008h], dl                    ; 88 56 f8
    mov byte [bp-006h], bl                    ; 88 5e fa
    mov si, cx                                ; 89 ce
    mov di, word [bp+00ah]                    ; 8b 7e 0a
    movzx ax, dl                              ; 0f b6 c2
    lea bx, [bp-00eh]                         ; 8d 5e f2
    lea dx, [bp-00ch]                         ; 8d 56 f4
    call 00a88h                               ; e8 b0 e3
    cmp byte [bp+004h], 0ffh                  ; 80 7e 04 ff
    jne short 026efh                          ; 75 11
    mov al, byte [bp-00eh]                    ; 8a 46 f2
    mov byte [bp+006h], al                    ; 88 46 06
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    xor al, al                                ; 30 c0
    shr ax, 008h                              ; c1 e8 08
    mov byte [bp+004h], al                    ; 88 46 04
    movzx dx, byte [bp+004h]                  ; 0f b6 56 04
    sal dx, 008h                              ; c1 e2 08
    movzx ax, byte [bp+006h]                  ; 0f b6 46 06
    add dx, ax                                ; 01 c2
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    call 00e5eh                               ; e8 5b e7
    dec si                                    ; 4e
    cmp si, strict byte 0ffffh                ; 83 fe ff
    je short 02739h                           ; 74 30
    mov dx, di                                ; 89 fa
    inc di                                    ; 47
    mov ax, word [bp+008h]                    ; 8b 46 08
    call 02f3eh                               ; e8 2c 08
    mov cl, al                                ; 88 c1
    test byte [bp-00ah], 002h                 ; f6 46 f6 02
    je short 02726h                           ; 74 0c
    mov dx, di                                ; 89 fa
    inc di                                    ; 47
    mov ax, word [bp+008h]                    ; 8b 46 08
    call 02f3eh                               ; e8 1b 08
    mov byte [bp-006h], al                    ; 88 46 fa
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    movzx dx, byte [bp-008h]                  ; 0f b6 56 f8
    movzx ax, cl                              ; 0f b6 c1
    mov cx, strict word 00003h                ; b9 03 00
    call 02153h                               ; e8 1c fa
    jmp short 02703h                          ; eb ca
    test byte [bp-00ah], 001h                 ; f6 46 f6 01
    jne short 02749h                          ; 75 0a
    mov dx, word [bp-00eh]                    ; 8b 56 f2
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    call 00e5eh                               ; e8 15 e7
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00008h                               ; c2 08 00
biosfn_read_state_info_:                     ; 0xc2752 LB 0x101
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    push dx                                   ; 52
    push bx                                   ; 53
    mov cx, ds                                ; 8c d9
    mov bx, 05382h                            ; bb 82 53
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    mov ax, word [bp-008h]                    ; 8b 46 f8
    call 02f88h                               ; e8 20 08
    mov di, word [bp-00ah]                    ; 8b 7e f6
    add di, strict byte 00004h                ; 83 c7 04
    mov cx, strict word 0001eh                ; b9 1e 00
    mov si, strict word 00049h                ; be 49 00
    mov dx, strict word 00040h                ; ba 40 00
    mov es, [bp-008h]                         ; 8e 46 f8
    cld                                       ; fc
    jcxz 02783h                               ; e3 06
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsb                                 ; f3 a4
    pop DS                                    ; 1f
    mov di, word [bp-00ah]                    ; 8b 7e f6
    add di, strict byte 00022h                ; 83 c7 22
    mov cx, strict word 00003h                ; b9 03 00
    mov si, 00084h                            ; be 84 00
    mov dx, strict word 00040h                ; ba 40 00
    mov es, [bp-008h]                         ; 8e 46 f8
    cld                                       ; fc
    jcxz 0279eh                               ; e3 06
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsb                                 ; f3 a4
    pop DS                                    ; 1f
    mov dx, 0008ah                            ; ba 8a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 97 07
    movzx bx, al                              ; 0f b6 d8
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    add dx, strict byte 00025h                ; 83 c2 25
    mov ax, word [bp-008h]                    ; 8b 46 f8
    call 02f4ch                               ; e8 96 07
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    add dx, strict byte 00026h                ; 83 c2 26
    xor bx, bx                                ; 31 db
    mov ax, word [bp-008h]                    ; 8b 46 f8
    call 02f4ch                               ; e8 88 07
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    add dx, strict byte 00027h                ; 83 c2 27
    mov bx, strict word 00010h                ; bb 10 00
    mov ax, word [bp-008h]                    ; 8b 46 f8
    call 02f4ch                               ; e8 79 07
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    add dx, strict byte 00028h                ; 83 c2 28
    xor bx, bx                                ; 31 db
    mov ax, word [bp-008h]                    ; 8b 46 f8
    call 02f4ch                               ; e8 6b 07
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    add dx, strict byte 00029h                ; 83 c2 29
    mov bx, strict word 00008h                ; bb 08 00
    mov ax, word [bp-008h]                    ; 8b 46 f8
    call 02f4ch                               ; e8 5c 07
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    add dx, strict byte 0002ah                ; 83 c2 2a
    mov bx, strict word 00002h                ; bb 02 00
    mov ax, word [bp-008h]                    ; 8b 46 f8
    call 02f4ch                               ; e8 4d 07
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    add dx, strict byte 0002bh                ; 83 c2 2b
    xor bx, bx                                ; 31 db
    mov ax, word [bp-008h]                    ; 8b 46 f8
    call 02f4ch                               ; e8 3f 07
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    add dx, strict byte 0002ch                ; 83 c2 2c
    xor bx, bx                                ; 31 db
    mov ax, word [bp-008h]                    ; 8b 46 f8
    call 02f4ch                               ; e8 31 07
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    add dx, strict byte 00031h                ; 83 c2 31
    mov bx, strict word 00003h                ; bb 03 00
    mov ax, word [bp-008h]                    ; 8b 46 f8
    call 02f4ch                               ; e8 22 07
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    add dx, strict byte 00032h                ; 83 c2 32
    xor bx, bx                                ; 31 db
    mov ax, word [bp-008h]                    ; 8b 46 f8
    call 02f4ch                               ; e8 14 07
    mov di, word [bp-00ah]                    ; 8b 7e f6
    add di, strict byte 00033h                ; 83 c7 33
    mov cx, strict word 0000dh                ; b9 0d 00
    xor ax, ax                                ; 31 c0
    mov es, [bp-008h]                         ; 8e 46 f8
    cld                                       ; fc
    jcxz 0284bh                               ; e3 02
    rep stosb                                 ; f3 aa
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_read_video_state_size2_:              ; 0xc2853 LB 0x23
    push dx                                   ; 52
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov dx, ax                                ; 89 c2
    xor ax, ax                                ; 31 c0
    test dl, 001h                             ; f6 c2 01
    je short 02863h                           ; 74 03
    mov ax, strict word 00046h                ; b8 46 00
    test dl, 002h                             ; f6 c2 02
    je short 0286bh                           ; 74 03
    add ax, strict word 0002ah                ; 05 2a 00
    test dl, 004h                             ; f6 c2 04
    je short 02873h                           ; 74 03
    add ax, 00304h                            ; 05 04 03
    pop bp                                    ; 5d
    pop dx                                    ; 5a
    retn                                      ; c3
vga_get_video_state_size_:                   ; 0xc2876 LB 0x12
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    mov bx, dx                                ; 89 d3
    call 02853h                               ; e8 d4 ff
    mov word [ss:bx], ax                      ; 36 89 07
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_save_video_state_:                    ; 0xc2888 LB 0x369
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    push ax                                   ; 50
    mov si, dx                                ; 89 d6
    mov cx, bx                                ; 89 d9
    mov dx, strict word 00063h                ; ba 63 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 bc 06
    mov di, ax                                ; 89 c7
    test byte [bp-00ch], 001h                 ; f6 46 f4 01
    je near 02a0bh                            ; 0f 84 63 01
    mov dx, 003c4h                            ; ba c4 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 94 06
    inc cx                                    ; 41
    mov dx, di                                ; 89 fa
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 84 06
    inc cx                                    ; 41
    mov dx, 003ceh                            ; ba ce 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 73 06
    inc cx                                    ; 41
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov dx, 003c0h                            ; ba c0 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov word [bp-008h], ax                    ; 89 46 f8
    movzx bx, byte [bp-008h]                  ; 0f b6 5e f8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 58 06
    inc cx                                    ; 41
    mov dx, 003cah                            ; ba ca 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 47 06
    mov ax, strict word 00001h                ; b8 01 00
    mov word [bp-00ah], ax                    ; 89 46 f6
    add cx, ax                                ; 01 c1
    jmp short 02915h                          ; eb 06
    cmp word [bp-00ah], strict byte 00004h    ; 83 7e f6 04
    jnbe short 02932h                         ; 77 1d
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    mov dx, 003c4h                            ; ba c4 03
    out DX, AL                                ; ee
    mov dx, 003c5h                            ; ba c5 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 20 06
    inc cx                                    ; 41
    inc word [bp-00ah]                        ; ff 46 f6
    jmp short 0290fh                          ; eb dd
    xor al, al                                ; 30 c0
    mov dx, 003c4h                            ; ba c4 03
    out DX, AL                                ; ee
    mov dx, 003c5h                            ; ba c5 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 04 06
    mov word [bp-00ah], strict word 00000h    ; c7 46 f6 00 00
    inc cx                                    ; 41
    jmp short 02956h                          ; eb 06
    cmp word [bp-00ah], strict byte 00018h    ; 83 7e f6 18
    jnbe short 02972h                         ; 77 1c
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    mov dx, di                                ; 89 fa
    out DX, AL                                ; ee
    lea dx, [di+001h]                         ; 8d 55 01
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 e0 05
    inc cx                                    ; 41
    inc word [bp-00ah]                        ; ff 46 f6
    jmp short 02950h                          ; eb de
    mov word [bp-00ah], strict word 00000h    ; c7 46 f6 00 00
    jmp short 0297fh                          ; eb 06
    cmp word [bp-00ah], strict byte 00013h    ; 83 7e f6 13
    jnbe short 029a8h                         ; 77 29
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov ax, word [bp-008h]                    ; 8b 46 f8
    and ax, strict word 00020h                ; 25 20 00
    or ax, word [bp-00ah]                     ; 0b 46 f6
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    mov dx, 003c1h                            ; ba c1 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 aa 05
    inc cx                                    ; 41
    inc word [bp-00ah]                        ; ff 46 f6
    jmp short 02979h                          ; eb d1
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov word [bp-00ah], strict word 00000h    ; c7 46 f6 00 00
    jmp short 029bbh                          ; eb 06
    cmp word [bp-00ah], strict byte 00008h    ; 83 7e f6 08
    jnbe short 029d8h                         ; 77 1d
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    mov dx, 003ceh                            ; ba ce 03
    out DX, AL                                ; ee
    mov dx, 003cfh                            ; ba cf 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 7a 05
    inc cx                                    ; 41
    inc word [bp-00ah]                        ; ff 46 f6
    jmp short 029b5h                          ; eb dd
    mov bx, di                                ; 89 fb
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 87 05
    inc cx                                    ; 41
    inc cx                                    ; 41
    xor bx, bx                                ; 31 db
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 60 05
    inc cx                                    ; 41
    xor bx, bx                                ; 31 db
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 56 05
    inc cx                                    ; 41
    xor bx, bx                                ; 31 db
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 4c 05
    inc cx                                    ; 41
    xor bx, bx                                ; 31 db
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 42 05
    inc cx                                    ; 41
    test byte [bp-00ch], 002h                 ; f6 46 f4 02
    je near 02b7ah                            ; 0f 84 67 01
    mov dx, strict word 00049h                ; ba 49 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 22 05
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 26 05
    inc cx                                    ; 41
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 2a 05
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 2f 05
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, strict word 0004ch                ; ba 4c 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 16 05
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 1b 05
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, strict word 00063h                ; ba 63 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 02 05
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 07 05
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, 00084h                            ; ba 84 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 d2 04
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 d6 04
    inc cx                                    ; 41
    mov dx, 00085h                            ; ba 85 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 da 04
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 df 04
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, 00087h                            ; ba 87 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 aa 04
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 ae 04
    inc cx                                    ; 41
    mov dx, 00088h                            ; ba 88 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 96 04
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 9a 04
    inc cx                                    ; 41
    mov dx, 00089h                            ; ba 89 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 82 04
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 86 04
    inc cx                                    ; 41
    mov dx, strict word 00060h                ; ba 60 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 8a 04
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 8f 04
    mov word [bp-00ah], strict word 00000h    ; c7 46 f6 00 00
    inc cx                                    ; 41
    inc cx                                    ; 41
    jmp short 02ae8h                          ; eb 06
    cmp word [bp-00ah], strict byte 00008h    ; 83 7e f6 08
    jnc short 02b06h                          ; 73 1e
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    add dx, dx                                ; 01 d2
    add dx, strict byte 00050h                ; 83 c2 50
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 64 04
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 69 04
    inc cx                                    ; 41
    inc cx                                    ; 41
    inc word [bp-00ah]                        ; ff 46 f6
    jmp short 02ae2h                          ; eb dc
    mov dx, strict word 0004eh                ; ba 4e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f5ah                               ; e8 4b 04
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 50 04
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, strict word 00062h                ; ba 62 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f3eh                               ; e8 1b 04
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 1f 04
    inc cx                                    ; 41
    mov dx, strict word 0007ch                ; ba 7c 00
    xor ax, ax                                ; 31 c0
    call 02f5ah                               ; e8 24 04
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 29 04
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, strict word 0007eh                ; ba 7e 00
    xor ax, ax                                ; 31 c0
    call 02f5ah                               ; e8 11 04
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 16 04
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, 0010ch                            ; ba 0c 01
    xor ax, ax                                ; 31 c0
    call 02f5ah                               ; e8 fe 03
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 03 04
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, 0010eh                            ; ba 0e 01
    xor ax, ax                                ; 31 c0
    call 02f5ah                               ; e8 eb 03
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 f0 03
    inc cx                                    ; 41
    inc cx                                    ; 41
    test byte [bp-00ch], 004h                 ; f6 46 f4 04
    je short 02be7h                           ; 74 67
    mov dx, 003c7h                            ; ba c7 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 bc 03
    inc cx                                    ; 41
    mov dx, 003c8h                            ; ba c8 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 ab 03
    inc cx                                    ; 41
    mov dx, 003c6h                            ; ba c6 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 9a 03
    inc cx                                    ; 41
    xor al, al                                ; 30 c0
    mov dx, 003c8h                            ; ba c8 03
    out DX, AL                                ; ee
    xor ah, ah                                ; 30 e4
    mov word [bp-00ah], ax                    ; 89 46 f6
    jmp short 02bc7h                          ; eb 07
    cmp word [bp-00ah], 00300h                ; 81 7e f6 00 03
    jnc short 02bddh                          ; 73 16
    mov dx, 003c9h                            ; ba c9 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 75 03
    inc cx                                    ; 41
    inc word [bp-00ah]                        ; ff 46 f6
    jmp short 02bc0h                          ; eb e3
    xor bx, bx                                ; 31 db
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 66 03
    inc cx                                    ; 41
    mov ax, cx                                ; 89 c8
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bp                                    ; 5d
    retn                                      ; c3
biosfn_restore_video_state_:                 ; 0xc2bf1 LB 0x326
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00006h                ; 83 ec 06
    push ax                                   ; 50
    mov si, dx                                ; 89 d6
    mov cx, bx                                ; 89 d9
    test byte [bp-00eh], 001h                 ; f6 46 f2 01
    je near 02d51h                            ; 0f 84 4a 01
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    lea dx, [bx+040h]                         ; 8d 57 40
    mov ax, si                                ; 89 f0
    call 02f5ah                               ; e8 45 03
    mov di, ax                                ; 89 c7
    mov word [bp-008h], strict word 00001h    ; c7 46 f8 01 00
    lea cx, [bx+005h]                         ; 8d 4f 05
    jmp short 02c27h                          ; eb 06
    cmp word [bp-008h], strict byte 00004h    ; 83 7e f8 04
    jnbe short 02c3fh                         ; 77 18
    mov al, byte [bp-008h]                    ; 8a 46 f8
    mov dx, 003c4h                            ; ba c4 03
    out DX, AL                                ; ee
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 09 03
    mov dx, 003c5h                            ; ba c5 03
    out DX, AL                                ; ee
    inc cx                                    ; 41
    inc word [bp-008h]                        ; ff 46 f8
    jmp short 02c21h                          ; eb e2
    xor al, al                                ; 30 c0
    mov dx, 003c4h                            ; ba c4 03
    out DX, AL                                ; ee
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 f2 02
    mov dx, 003c5h                            ; ba c5 03
    out DX, AL                                ; ee
    inc cx                                    ; 41
    mov ax, strict word 00011h                ; b8 11 00
    mov dx, di                                ; 89 fa
    out DX, ax                                ; ef
    mov word [bp-008h], strict word 00000h    ; c7 46 f8 00 00
    jmp short 02c64h                          ; eb 06
    cmp word [bp-008h], strict byte 00018h    ; 83 7e f8 18
    jnbe short 02c81h                         ; 77 1d
    cmp word [bp-008h], strict byte 00011h    ; 83 7e f8 11
    je short 02c7bh                           ; 74 11
    mov al, byte [bp-008h]                    ; 8a 46 f8
    mov dx, di                                ; 89 fa
    out DX, AL                                ; ee
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 c7 02
    lea dx, [di+001h]                         ; 8d 55 01
    out DX, AL                                ; ee
    inc cx                                    ; 41
    inc word [bp-008h]                        ; ff 46 f8
    jmp short 02c5eh                          ; eb dd
    mov dx, 003cch                            ; ba cc 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and AL, strict byte 0feh                  ; 24 fe
    mov word [bp-00ah], ax                    ; 89 46 f6
    cmp di, 003d4h                            ; 81 ff d4 03
    jne short 02c96h                          ; 75 04
    or byte [bp-00ah], 001h                   ; 80 4e f6 01
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    mov dx, 003c2h                            ; ba c2 03
    out DX, AL                                ; ee
    mov AL, strict byte 011h                  ; b0 11
    mov dx, di                                ; 89 fa
    out DX, AL                                ; ee
    mov dx, cx                                ; 89 ca
    add dx, strict byte 0fff9h                ; 83 c2 f9
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 92 02
    lea dx, [di+001h]                         ; 8d 55 01
    out DX, AL                                ; ee
    lea dx, [bx+003h]                         ; 8d 57 03
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 86 02
    xor ah, ah                                ; 30 e4
    mov word [bp-00ch], ax                    ; 89 46 f4
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov word [bp-008h], strict word 00000h    ; c7 46 f8 00 00
    jmp short 02cd0h                          ; eb 06
    cmp word [bp-008h], strict byte 00013h    ; 83 7e f8 13
    jnbe short 02ceeh                         ; 77 1e
    mov ax, word [bp-00ch]                    ; 8b 46 f4
    and ax, strict word 00020h                ; 25 20 00
    or ax, word [bp-008h]                     ; 0b 46 f8
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 5a 02
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    inc cx                                    ; 41
    inc word [bp-008h]                        ; ff 46 f8
    jmp short 02ccah                          ; eb dc
    mov al, byte [bp-00ch]                    ; 8a 46 f4
    mov dx, 003c0h                            ; ba c0 03
    out DX, AL                                ; ee
    mov dx, 003dah                            ; ba da 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov word [bp-008h], strict word 00000h    ; c7 46 f8 00 00
    jmp short 02d08h                          ; eb 06
    cmp word [bp-008h], strict byte 00008h    ; 83 7e f8 08
    jnbe short 02d20h                         ; 77 18
    mov al, byte [bp-008h]                    ; 8a 46 f8
    mov dx, 003ceh                            ; ba ce 03
    out DX, AL                                ; ee
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 28 02
    mov dx, 003cfh                            ; ba cf 03
    out DX, AL                                ; ee
    inc cx                                    ; 41
    inc word [bp-008h]                        ; ff 46 f8
    jmp short 02d02h                          ; eb e2
    add cx, strict byte 00006h                ; 83 c1 06
    mov dx, bx                                ; 89 da
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 14 02
    mov dx, 003c4h                            ; ba c4 03
    out DX, AL                                ; ee
    inc bx                                    ; 43
    mov dx, bx                                ; 89 da
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 08 02
    mov dx, di                                ; 89 fa
    out DX, AL                                ; ee
    inc bx                                    ; 43
    mov dx, bx                                ; 89 da
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 fd 01
    mov dx, 003ceh                            ; ba ce 03
    out DX, AL                                ; ee
    lea dx, [bx+002h]                         ; 8d 57 02
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 f1 01
    lea dx, [di+006h]                         ; 8d 55 06
    out DX, AL                                ; ee
    test byte [bp-00eh], 002h                 ; f6 46 f2 02
    je near 02ec0h                            ; 0f 84 67 01
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 de 01
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00049h                ; ba 49 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 e0 01
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f5ah                               ; e8 e6 01
    mov bx, ax                                ; 89 c3
    mov dx, strict word 0004ah                ; ba 4a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 e9 01
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f5ah                               ; e8 d2 01
    mov bx, ax                                ; 89 c3
    mov dx, strict word 0004ch                ; ba 4c 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 d5 01
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f5ah                               ; e8 be 01
    mov bx, ax                                ; 89 c3
    mov dx, strict word 00063h                ; ba 63 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 c1 01
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 8e 01
    movzx bx, al                              ; 0f b6 d8
    mov dx, 00084h                            ; ba 84 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 90 01
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f5ah                               ; e8 96 01
    mov bx, ax                                ; 89 c3
    mov dx, 00085h                            ; ba 85 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 99 01
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 66 01
    movzx bx, al                              ; 0f b6 d8
    mov dx, 00087h                            ; ba 87 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 68 01
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 52 01
    movzx bx, al                              ; 0f b6 d8
    mov dx, 00088h                            ; ba 88 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 54 01
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 3e 01
    movzx bx, al                              ; 0f b6 d8
    mov dx, 00089h                            ; ba 89 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 40 01
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f5ah                               ; e8 46 01
    mov bx, ax                                ; 89 c3
    mov dx, strict word 00060h                ; ba 60 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 49 01
    mov word [bp-008h], strict word 00000h    ; c7 46 f8 00 00
    inc cx                                    ; 41
    inc cx                                    ; 41
    jmp short 02e2eh                          ; eb 06
    cmp word [bp-008h], strict byte 00008h    ; 83 7e f8 08
    jnc short 02e4ch                          ; 73 1e
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f5ah                               ; e8 25 01
    mov bx, ax                                ; 89 c3
    mov dx, word [bp-008h]                    ; 8b 56 f8
    add dx, dx                                ; 01 d2
    add dx, strict byte 00050h                ; 83 c2 50
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 23 01
    inc cx                                    ; 41
    inc cx                                    ; 41
    inc word [bp-008h]                        ; ff 46 f8
    jmp short 02e28h                          ; eb dc
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f5ah                               ; e8 07 01
    mov bx, ax                                ; 89 c3
    mov dx, strict word 0004eh                ; ba 4e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 0a 01
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 d7 00
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00062h                ; ba 62 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 d9 00
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f5ah                               ; e8 df 00
    mov bx, ax                                ; 89 c3
    mov dx, strict word 0007ch                ; ba 7c 00
    xor ax, ax                                ; 31 c0
    call 02f68h                               ; e8 e3 00
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f5ah                               ; e8 cc 00
    mov bx, ax                                ; 89 c3
    mov dx, strict word 0007eh                ; ba 7e 00
    xor ax, ax                                ; 31 c0
    call 02f68h                               ; e8 d0 00
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f5ah                               ; e8 b9 00
    mov bx, ax                                ; 89 c3
    mov dx, 0010ch                            ; ba 0c 01
    xor ax, ax                                ; 31 c0
    call 02f68h                               ; e8 bd 00
    inc cx                                    ; 41
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f5ah                               ; e8 a6 00
    mov bx, ax                                ; 89 c3
    mov dx, 0010eh                            ; ba 0e 01
    xor ax, ax                                ; 31 c0
    call 02f68h                               ; e8 aa 00
    inc cx                                    ; 41
    inc cx                                    ; 41
    test byte [bp-00eh], 004h                 ; f6 46 f2 04
    je short 02f0dh                           ; 74 47
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 70 00
    xor ah, ah                                ; 30 e4
    mov word [bp-00ah], ax                    ; 89 46 f6
    inc cx                                    ; 41
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 63 00
    mov dx, 003c6h                            ; ba c6 03
    out DX, AL                                ; ee
    inc cx                                    ; 41
    xor al, al                                ; 30 c0
    mov dx, 003c8h                            ; ba c8 03
    out DX, AL                                ; ee
    xor ah, ah                                ; 30 e4
    mov word [bp-008h], ax                    ; 89 46 f8
    jmp short 02ef4h                          ; eb 07
    cmp word [bp-008h], 00300h                ; 81 7e f8 00 03
    jnc short 02f05h                          ; 73 11
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 43 00
    mov dx, 003c9h                            ; ba c9 03
    out DX, AL                                ; ee
    inc cx                                    ; 41
    inc word [bp-008h]                        ; ff 46 f8
    jmp short 02eedh                          ; eb e8
    inc cx                                    ; 41
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    mov dx, 003c8h                            ; ba c8 03
    out DX, AL                                ; ee
    mov ax, cx                                ; 89 c8
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bp                                    ; 5d
    retn                                      ; c3
find_vga_entry_:                             ; 0xc2f17 LB 0x27
    push bx                                   ; 53
    push dx                                   ; 52
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov dl, al                                ; 88 c2
    mov AH, strict byte 0ffh                  ; b4 ff
    xor al, al                                ; 30 c0
    jmp short 02f2ah                          ; eb 06
    db  0feh, 0c0h
    ; inc al                                    ; fe c0
    cmp AL, strict byte 00fh                  ; 3c 0f
    jnbe short 02f38h                         ; 77 0e
    movzx bx, al                              ; 0f b6 d8
    sal bx, 003h                              ; c1 e3 03
    cmp dl, byte [bx+0462eh]                  ; 3a 97 2e 46
    jne short 02f24h                          ; 75 ee
    mov ah, al                                ; 88 c4
    mov al, ah                                ; 88 e0
    pop bp                                    ; 5d
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    retn                                      ; c3
read_byte_:                                  ; 0xc2f3e LB 0xe
    push bx                                   ; 53
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov bx, dx                                ; 89 d3
    mov es, ax                                ; 8e c0
    mov al, byte [es:bx]                      ; 26 8a 07
    pop bp                                    ; 5d
    pop bx                                    ; 5b
    retn                                      ; c3
write_byte_:                                 ; 0xc2f4c LB 0xe
    push si                                   ; 56
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov si, dx                                ; 89 d6
    mov es, ax                                ; 8e c0
    mov byte [es:si], bl                      ; 26 88 1c
    pop bp                                    ; 5d
    pop si                                    ; 5e
    retn                                      ; c3
read_word_:                                  ; 0xc2f5a LB 0xe
    push bx                                   ; 53
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov bx, dx                                ; 89 d3
    mov es, ax                                ; 8e c0
    mov ax, word [es:bx]                      ; 26 8b 07
    pop bp                                    ; 5d
    pop bx                                    ; 5b
    retn                                      ; c3
write_word_:                                 ; 0xc2f68 LB 0xe
    push si                                   ; 56
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov si, dx                                ; 89 d6
    mov es, ax                                ; 8e c0
    mov word [es:si], bx                      ; 26 89 1c
    pop bp                                    ; 5d
    pop si                                    ; 5e
    retn                                      ; c3
read_dword_:                                 ; 0xc2f76 LB 0x12
    push bx                                   ; 53
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov bx, dx                                ; 89 d3
    mov es, ax                                ; 8e c0
    mov ax, word [es:bx]                      ; 26 8b 07
    mov dx, word [es:bx+002h]                 ; 26 8b 57 02
    pop bp                                    ; 5d
    pop bx                                    ; 5b
    retn                                      ; c3
write_dword_:                                ; 0xc2f88 LB 0x7f
    push si                                   ; 56
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov si, dx                                ; 89 d6
    mov es, ax                                ; 8e c0
    mov word [es:si], bx                      ; 26 89 1c
    mov word [es:si+002h], cx                 ; 26 89 4c 02
    pop bp                                    ; 5d
    pop si                                    ; 5e
    retn                                      ; c3
    dec di                                    ; 4f
    sbb AL, strict byte 01bh                  ; 1c 1b
    adc dx, word [bp+si]                      ; 13 12
    adc word [bx+si], dx                      ; 11 10
    push CS                                   ; 0e
    or ax, 00a0ch                             ; 0d 0c 0a
    or word [bx+si], cx                       ; 09 08
    pop ES                                    ; 07
    push ES                                   ; 06
    add ax, 00304h                            ; 05 04 03
    add al, byte [bx+di]                      ; 02 01
    add dl, cl                                ; 00 ca
    xor si, word [di]                         ; 33 35
    xor byte [bp+di+030h], dh                 ; 30 73 30
    xchg word [bx+si], si                     ; 87 30
    cbw                                       ; 98
    xor byte [si-042d0h], ch                  ; 30 ac 30 bd
    xor bh, al                                ; 30 c7
    xor byte [bx+di], al                      ; 30 01
    xor word [di], ax                         ; 31 05
    xor word [03331h], dx                     ; 31 16 31 33
    xor word [bx+si+031h], dx                 ; 31 50 31
    jo short 02ffch                           ; 70 31
    lea si, [bx+di]                           ; 8d 31
    movsb                                     ; a4
    xor word [bx+si-074cfh], si               ; 31 b0 31 8b
    db  032h, 0c6h
    ; xor al, dh                                ; 32 c6
    db  032h, 0f6h
    ; xor dh, dh                                ; 32 f6
    xor cl, byte [bp+di]                      ; 32 0b
    xor cx, word [di+033h]                    ; 33 4d 33
    xor byte [si], ah                         ; 30 24
    and sp, word [bp+si]                      ; 23 22
    and word [bx+si], sp                      ; 21 20
    adc AL, strict byte 012h                  ; 14 12
    adc word [bx+si], dx                      ; 11 10
    add AL, strict byte 002h                  ; 04 02
    add word [bx+si], ax                      ; 01 00
    retf 0d133h                               ; ca 33 d1
    xor di, si                                ; 31 f7
    xor word [bx+si], cx                      ; 31 08
    xor bl, byte [bx+di]                      ; 32 19
    db  032h, 0d1h
    ; xor dl, cl                                ; 32 d1
    xor di, si                                ; 31 f7
    xor word [bx+si], cx                      ; 31 08
    xor bl, byte [bx+di]                      ; 32 19
    xor ch, byte [bp+si]                      ; 32 2a
    xor dh, byte [05132h]                     ; 32 36 32 51
    xor bl, byte [si+032h]                    ; 32 5c 32
    xor dh, byte [edx+032h]                   ; 67 32 72 32
_int10_func:                                 ; 0xc3007 LB 0x3ca
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    mov si, word [bp+004h]                    ; 8b 76 04
    mov ax, word [bp+012h]                    ; 8b 46 12
    shr ax, 008h                              ; c1 e8 08
    cmp ax, strict word 0004fh                ; 3d 4f 00
    jnbe near 033cah                          ; 0f 87 ad 03
    push CS                                   ; 0e
    pop ES                                    ; 07
    mov cx, strict word 00016h                ; b9 16 00
    mov di, 02f9ah                            ; bf 9a 2f
    repne scasb                               ; f2 ae
    sal cx, 1                                 ; d1 e1
    mov di, cx                                ; 89 cf
    mov ax, word [cs:di+02fafh]               ; 2e 8b 85 af 2f
    mov cl, byte [bp+012h]                    ; 8a 4e 12
    jmp ax                                    ; ff e0
    mov al, byte [bp+012h]                    ; 8a 46 12
    xor ah, ah                                ; 30 e4
    call 00fdch                               ; e8 9f df
    mov ax, word [bp+012h]                    ; 8b 46 12
    and ax, strict word 0007fh                ; 25 7f 00
    cmp ax, strict word 00007h                ; 3d 07 00
    je short 0305dh                           ; 74 15
    cmp ax, strict word 00006h                ; 3d 06 00
    je short 03054h                           ; 74 07
    cmp ax, strict word 00005h                ; 3d 05 00
    jbe short 0305dh                          ; 76 0b
    jmp short 03066h                          ; eb 12
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor al, al                                ; 30 c0
    or AL, strict byte 03fh                   ; 0c 3f
    jmp short 0306dh                          ; eb 10
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor al, al                                ; 30 c0
    or AL, strict byte 030h                   ; 0c 30
    jmp short 0306dh                          ; eb 07
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor al, al                                ; 30 c0
    or AL, strict byte 020h                   ; 0c 20
    mov word [bp+012h], ax                    ; 89 46 12
    jmp near 033cah                           ; e9 57 03
    mov al, byte [bp+010h]                    ; 8a 46 10
    movzx dx, al                              ; 0f b6 d0
    mov ax, word [bp+010h]                    ; 8b 46 10
    shr ax, 008h                              ; c1 e8 08
    xor ah, ah                                ; 30 e4
    call 00dbah                               ; e8 36 dd
    jmp near 033cah                           ; e9 43 03
    mov dx, word [bp+00eh]                    ; 8b 56 0e
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    shr ax, 008h                              ; c1 e8 08
    xor ah, ah                                ; 30 e4
    call 00e5eh                               ; e8 c9 dd
    jmp near 033cah                           ; e9 32 03
    lea bx, [bp+00eh]                         ; 8d 5e 0e
    lea dx, [bp+010h]                         ; 8d 56 10
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    shr ax, 008h                              ; c1 e8 08
    xor ah, ah                                ; 30 e4
    call 00a88h                               ; e8 df d9
    jmp near 033cah                           ; e9 1e 03
    xor ax, ax                                ; 31 c0
    mov word [bp+012h], ax                    ; 89 46 12
    mov word [bp+00ch], ax                    ; 89 46 0c
    mov word [bp+010h], ax                    ; 89 46 10
    mov word [bp+00eh], ax                    ; 89 46 0e
    jmp near 033cah                           ; e9 0d 03
    mov al, cl                                ; 88 c8
    xor ah, ah                                ; 30 e4
    call 00f00h                               ; e8 3c de
    jmp near 033cah                           ; e9 03 03
    mov ax, strict word 00001h                ; b8 01 00
    push ax                                   ; 50
    mov ax, 000ffh                            ; b8 ff 00
    push ax                                   ; 50
    mov al, byte [bp+00eh]                    ; 8a 46 0e
    xor ah, ah                                ; 30 e4
    push ax                                   ; 50
    mov ax, word [bp+00eh]                    ; 8b 46 0e
    shr ax, 008h                              ; c1 e8 08
    xor ah, ah                                ; 30 e4
    push ax                                   ; 50
    mov al, byte [bp+010h]                    ; 8a 46 10
    movzx cx, al                              ; 0f b6 c8
    mov ax, word [bp+010h]                    ; 8b 46 10
    shr ax, 008h                              ; c1 e8 08
    movzx bx, al                              ; 0f b6 d8
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    shr ax, 008h                              ; c1 e8 08
    movzx dx, al                              ; 0f b6 d0
    mov al, byte [bp+012h]                    ; 8a 46 12
    xor ah, ah                                ; 30 e4
    call 01572h                               ; e8 74 e4
    jmp near 033cah                           ; e9 c9 02
    xor ax, ax                                ; 31 c0
    jmp short 030cah                          ; eb c5
    lea dx, [bp+012h]                         ; 8d 56 12
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    shr ax, 008h                              ; c1 e8 08
    xor ah, ah                                ; 30 e4
    call 00acbh                               ; e8 b8 d9
    jmp near 033cah                           ; e9 b4 02
    mov cx, word [bp+010h]                    ; 8b 4e 10
    mov al, byte [bp+00ch]                    ; 8a 46 0c
    movzx bx, al                              ; 0f b6 d8
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    shr ax, 008h                              ; c1 e8 08
    movzx dx, al                              ; 0f b6 d0
    mov al, byte [bp+012h]                    ; 8a 46 12
    xor ah, ah                                ; 30 e4
    call 01d12h                               ; e8 e2 eb
    jmp near 033cah                           ; e9 97 02
    mov cx, word [bp+010h]                    ; 8b 4e 10
    mov al, byte [bp+00ch]                    ; 8a 46 0c
    movzx bx, al                              ; 0f b6 d8
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    shr ax, 008h                              ; c1 e8 08
    movzx dx, al                              ; 0f b6 d0
    mov al, byte [bp+012h]                    ; 8a 46 12
    xor ah, ah                                ; 30 e4
    call 01e7ah                               ; e8 2d ed
    jmp near 033cah                           ; e9 7a 02
    mov cx, word [bp+00eh]                    ; 8b 4e 0e
    mov bx, word [bp+010h]                    ; 8b 5e 10
    mov al, byte [bp+012h]                    ; 8a 46 12
    movzx dx, al                              ; 0f b6 d0
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    shr ax, 008h                              ; c1 e8 08
    mov word [bp-006h], ax                    ; 89 46 fa
    mov al, byte [bp-006h]                    ; 8a 46 fa
    xor ah, ah                                ; 30 e4
    call 01fe9h                               ; e8 7c ee
    jmp near 033cah                           ; e9 5a 02
    lea cx, [bp+012h]                         ; 8d 4e 12
    mov bx, word [bp+00eh]                    ; 8b 5e 0e
    mov dx, word [bp+010h]                    ; 8b 56 10
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    shr ax, 008h                              ; c1 e8 08
    mov word [bp-006h], ax                    ; 89 46 fa
    mov al, byte [bp-006h]                    ; 8a 46 fa
    xor ah, ah                                ; 30 e4
    call 00bf5h                               ; e8 6b da
    jmp near 033cah                           ; e9 3d 02
    mov cx, strict word 00002h                ; b9 02 00
    mov al, byte [bp+00ch]                    ; 8a 46 0c
    movzx bx, al                              ; 0f b6 d8
    mov dx, 000ffh                            ; ba ff 00
    mov al, byte [bp+012h]                    ; 8a 46 12
    xor ah, ah                                ; 30 e4
    call 02153h                               ; e8 b2 ef
    jmp near 033cah                           ; e9 26 02
    mov dx, word [bp+010h]                    ; 8b 56 10
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    call 00d2eh                               ; e8 81 db
    jmp near 033cah                           ; e9 1a 02
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor ah, ah                                ; 30 e4
    cmp ax, strict word 00030h                ; 3d 30 00
    jnbe near 033cah                          ; 0f 87 0e 02
    push CS                                   ; 0e
    pop ES                                    ; 07
    mov cx, strict word 0000fh                ; b9 0f 00
    mov di, 02fdbh                            ; bf db 2f
    repne scasb                               ; f2 ae
    sal cx, 1                                 ; d1 e1
    mov di, cx                                ; 89 cf
    mov ax, word [cs:di+02fe9h]               ; 2e 8b 85 e9 2f
    jmp ax                                    ; ff e0
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    shr ax, 008h                              ; c1 e8 08
    xor ah, ah                                ; 30 e4
    push ax                                   ; 50
    mov al, byte [bp+00ch]                    ; 8a 46 0c
    xor ah, ah                                ; 30 e4
    push ax                                   ; 50
    push word [bp+00eh]                       ; ff 76 0e
    mov al, byte [bp+012h]                    ; 8a 46 12
    xor ah, ah                                ; 30 e4
    mov cx, word [bp+010h]                    ; 8b 4e 10
    mov bx, word [bp+008h]                    ; 8b 5e 08
    mov dx, word [bp+016h]                    ; 8b 56 16
    call 024bbh                               ; e8 c7 f2
    jmp near 033cah                           ; e9 d3 01
    mov al, byte [bp+00ch]                    ; 8a 46 0c
    movzx dx, al                              ; 0f b6 d0
    mov al, byte [bp+012h]                    ; 8a 46 12
    xor ah, ah                                ; 30 e4
    call 02538h                               ; e8 33 f3
    jmp near 033cah                           ; e9 c2 01
    mov al, byte [bp+00ch]                    ; 8a 46 0c
    movzx dx, al                              ; 0f b6 d0
    mov al, byte [bp+012h]                    ; 8a 46 12
    xor ah, ah                                ; 30 e4
    call 025a8h                               ; e8 92 f3
    jmp near 033cah                           ; e9 b1 01
    mov al, byte [bp+00ch]                    ; 8a 46 0c
    movzx dx, al                              ; 0f b6 d0
    mov al, byte [bp+012h]                    ; 8a 46 12
    xor ah, ah                                ; 30 e4
    call 0261ah                               ; e8 f3 f3
    jmp near 033cah                           ; e9 a0 01
    mov dx, word [bp+008h]                    ; 8b 56 08
    mov ax, word [bp+016h]                    ; 8b 46 16
    call 0268ch                               ; e8 59 f4
    jmp near 033cah                           ; e9 94 01
    mov al, byte [bp+00eh]                    ; 8a 46 0e
    xor ah, ah                                ; 30 e4
    push ax                                   ; 50
    mov al, byte [bp+00ch]                    ; 8a 46 0c
    movzx cx, al                              ; 0f b6 c8
    mov bx, word [bp+010h]                    ; 8b 5e 10
    mov dx, word [bp+008h]                    ; 8b 56 08
    mov ax, word [bp+016h]                    ; 8b 46 16
    call 02691h                               ; e8 43 f4
    jmp near 033cah                           ; e9 79 01
    mov al, byte [bp+00ch]                    ; 8a 46 0c
    xor ah, ah                                ; 30 e4
    call 02698h                               ; e8 3f f4
    jmp near 033cah                           ; e9 6e 01
    mov al, byte [bp+00ch]                    ; 8a 46 0c
    xor ah, ah                                ; 30 e4
    call 0269dh                               ; e8 39 f4
    jmp near 033cah                           ; e9 63 01
    mov al, byte [bp+00ch]                    ; 8a 46 0c
    xor ah, ah                                ; 30 e4
    call 026a2h                               ; e8 33 f4
    jmp near 033cah                           ; e9 58 01
    lea ax, [bp+00eh]                         ; 8d 46 0e
    push ax                                   ; 50
    lea cx, [bp+010h]                         ; 8d 4e 10
    lea bx, [bp+008h]                         ; 8d 5e 08
    lea dx, [bp+016h]                         ; 8d 56 16
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    shr ax, 008h                              ; c1 e8 08
    call 00b73h                               ; e8 eb d8
    jmp near 033cah                           ; e9 3f 01
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    xor ah, ah                                ; 30 e4
    cmp ax, strict word 00036h                ; 3d 36 00
    je short 032bdh                           ; 74 28
    cmp ax, strict word 00035h                ; 3d 35 00
    je short 032a7h                           ; 74 0d
    cmp ax, strict word 00020h                ; 3d 20 00
    jne near 033cah                           ; 0f 85 29 01
    call 026a7h                               ; e8 03 f4
    jmp near 033cah                           ; e9 23 01
    movzx ax, cl                              ; 0f b6 c1
    mov bx, word [bp+00eh]                    ; 8b 5e 0e
    mov dx, word [bp+016h]                    ; 8b 56 16
    call 026ach                               ; e8 f9 f3
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor al, al                                ; 30 c0
    or AL, strict byte 012h                   ; 0c 12
    jmp near 0306dh                           ; e9 b0 fd
    mov al, cl                                ; 88 c8
    xor ah, ah                                ; 30 e4
    call 026b1h                               ; e8 ed f3
    jmp short 032b3h                          ; eb ed
    push word [bp+008h]                       ; ff 76 08
    push word [bp+016h]                       ; ff 76 16
    mov al, byte [bp+00eh]                    ; 8a 46 0e
    xor ah, ah                                ; 30 e4
    push ax                                   ; 50
    mov ax, word [bp+00eh]                    ; 8b 46 0e
    shr ax, 008h                              ; c1 e8 08
    xor ah, ah                                ; 30 e4
    push ax                                   ; 50
    mov al, byte [bp+00ch]                    ; 8a 46 0c
    movzx bx, al                              ; 0f b6 d8
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    shr ax, 008h                              ; c1 e8 08
    movzx dx, al                              ; 0f b6 d0
    movzx ax, cl                              ; 0f b6 c1
    mov cx, word [bp+010h]                    ; 8b 4e 10
    call 026b6h                               ; e8 c3 f3
    jmp near 033cah                           ; e9 d4 00
    mov bx, si                                ; 89 f3
    mov dx, word [bp+016h]                    ; 8b 56 16
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    call 02752h                               ; e8 51 f4
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor al, al                                ; 30 c0
    or AL, strict byte 01bh                   ; 0c 1b
    jmp near 0306dh                           ; e9 62 fd
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor ah, ah                                ; 30 e4
    cmp ax, strict word 00002h                ; 3d 02 00
    je short 03337h                           ; 74 22
    cmp ax, strict word 00001h                ; 3d 01 00
    je short 03329h                           ; 74 0f
    test ax, ax                               ; 85 c0
    jne short 03343h                          ; 75 25
    lea dx, [bp+00ch]                         ; 8d 56 0c
    mov ax, word [bp+010h]                    ; 8b 46 10
    call 02876h                               ; e8 4f f5
    jmp short 03343h                          ; eb 1a
    mov bx, word [bp+00ch]                    ; 8b 5e 0c
    mov dx, word [bp+016h]                    ; 8b 56 16
    mov ax, word [bp+010h]                    ; 8b 46 10
    call 02888h                               ; e8 53 f5
    jmp short 03343h                          ; eb 0c
    mov bx, word [bp+00ch]                    ; 8b 5e 0c
    mov dx, word [bp+016h]                    ; 8b 56 16
    mov ax, word [bp+010h]                    ; 8b 46 10
    call 02bf1h                               ; e8 ae f8
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor al, al                                ; 30 c0
    or AL, strict byte 01ch                   ; 0c 1c
    jmp near 0306dh                           ; e9 20 fd
    call 007fah                               ; e8 aa d4
    test ax, ax                               ; 85 c0
    je near 033c5h                            ; 0f 84 6f 00
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor ah, ah                                ; 30 e4
    cmp ax, strict word 00002h                ; 3d 02 00
    jc short 03373h                           ; 72 13
    jbe short 03399h                          ; 76 37
    cmp ax, strict word 0000ah                ; 3d 0a 00
    je short 033beh                           ; 74 57
    cmp ax, strict word 00009h                ; 3d 09 00
    je short 033beh                           ; 74 52
    cmp ax, strict word 00004h                ; 3d 04 00
    je short 033a9h                           ; 74 38
    jmp short 033beh                          ; eb 4b
    cmp ax, strict word 00001h                ; 3d 01 00
    je short 03389h                           ; 74 11
    test ax, ax                               ; 85 c0
    jne short 033beh                          ; 75 42
    mov bx, si                                ; 89 f3
    mov dx, word [bp+016h]                    ; 8b 56 16
    lea ax, [bp+012h]                         ; 8d 46 12
    call 03503h                               ; e8 7c 01
    jmp short 033cah                          ; eb 41
    mov cx, si                                ; 89 f1
    mov bx, word [bp+016h]                    ; 8b 5e 16
    mov dx, word [bp+010h]                    ; 8b 56 10
    lea ax, [bp+012h]                         ; 8d 46 12
    call 0362ch                               ; e8 95 02
    jmp short 033cah                          ; eb 31
    mov cx, si                                ; 89 f1
    mov bx, word [bp+016h]                    ; 8b 5e 16
    mov dx, word [bp+00ch]                    ; 8b 56 0c
    lea ax, [bp+012h]                         ; 8d 46 12
    call 036e4h                               ; e8 3d 03
    jmp short 033cah                          ; eb 21
    lea ax, [bp+00ch]                         ; 8d 46 0c
    push ax                                   ; 50
    mov cx, word [bp+016h]                    ; 8b 4e 16
    mov bx, word [bp+00eh]                    ; 8b 5e 0e
    mov dx, word [bp+010h]                    ; 8b 56 10
    lea ax, [bp+012h]                         ; 8d 46 12
    call 038cbh                               ; e8 0f 05
    jmp short 033cah                          ; eb 0c
    mov word [bp+012h], 00100h                ; c7 46 12 00 01
    jmp short 033cah                          ; eb 05
    mov word [bp+012h], 00100h                ; c7 46 12 00 01
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
dispi_set_xres_:                             ; 0xc33d1 LB 0x1f
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push dx                                   ; 52
    mov bx, ax                                ; 89 c3
    mov ax, strict word 00001h                ; b8 01 00
    mov dx, 001ceh                            ; ba ce 01
    call 00570h                               ; e8 8f d1
    mov ax, bx                                ; 89 d8
    mov dx, 001cfh                            ; ba cf 01
    call 00570h                               ; e8 87 d1
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
dispi_set_yres_:                             ; 0xc33f0 LB 0x1f
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push dx                                   ; 52
    mov bx, ax                                ; 89 c3
    mov ax, strict word 00002h                ; b8 02 00
    mov dx, 001ceh                            ; ba ce 01
    call 00570h                               ; e8 70 d1
    mov ax, bx                                ; 89 d8
    mov dx, 001cfh                            ; ba cf 01
    call 00570h                               ; e8 68 d1
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
dispi_set_bpp_:                              ; 0xc340f LB 0x1f
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push dx                                   ; 52
    mov bx, ax                                ; 89 c3
    mov ax, strict word 00003h                ; b8 03 00
    mov dx, 001ceh                            ; ba ce 01
    call 00570h                               ; e8 51 d1
    mov ax, bx                                ; 89 d8
    mov dx, 001cfh                            ; ba cf 01
    call 00570h                               ; e8 49 d1
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
in_word_:                                    ; 0xc342e LB 0x12
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    mov bx, ax                                ; 89 c3
    mov ax, dx                                ; 89 d0
    mov dx, bx                                ; 89 da
    out DX, ax                                ; ef
    in ax, DX                                 ; ed
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
in_byte_:                                    ; 0xc3440 LB 0x14
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    mov bx, ax                                ; 89 c3
    mov ax, dx                                ; 89 d0
    mov dx, bx                                ; 89 da
    out DX, ax                                ; ef
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
dispi_get_id_:                               ; 0xc3454 LB 0x14
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push dx                                   ; 52
    xor ax, ax                                ; 31 c0
    mov dx, 001ceh                            ; ba ce 01
    out DX, ax                                ; ef
    mov dx, 001cfh                            ; ba cf 01
    in ax, DX                                 ; ed
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop dx                                    ; 5a
    pop bp                                    ; 5d
    retn                                      ; c3
dispi_set_id_:                               ; 0xc3468 LB 0x1a
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push dx                                   ; 52
    mov bx, ax                                ; 89 c3
    xor ax, ax                                ; 31 c0
    mov dx, 001ceh                            ; ba ce 01
    out DX, ax                                ; ef
    mov ax, bx                                ; 89 d8
    mov dx, 001cfh                            ; ba cf 01
    out DX, ax                                ; ef
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
vbe_init_:                                   ; 0xc3482 LB 0x2c
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push dx                                   ; 52
    mov ax, 0b0c0h                            ; b8 c0 b0
    call 03468h                               ; e8 db ff
    call 03454h                               ; e8 c4 ff
    cmp ax, 0b0c0h                            ; 3d c0 b0
    jne short 034a7h                          ; 75 12
    mov bx, strict word 00001h                ; bb 01 00
    mov dx, 000b9h                            ; ba b9 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 ab fa
    mov ax, 0b0c4h                            ; b8 c4 b0
    call 03468h                               ; e8 c1 ff
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
mode_info_find_mode_:                        ; 0xc34ae LB 0x55
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    mov di, ax                                ; 89 c7
    mov si, dx                                ; 89 d6
    xor dx, dx                                ; 31 d2
    mov ax, 003b6h                            ; b8 b6 03
    call 0342eh                               ; e8 6d ff
    cmp ax, 077cch                            ; 3d cc 77
    jne short 034f8h                          ; 75 32
    mov bx, strict word 00004h                ; bb 04 00
    mov dx, bx                                ; 89 da
    mov ax, 003b6h                            ; b8 b6 03
    call 0342eh                               ; e8 5d ff
    mov cx, ax                                ; 89 c1
    cmp cx, strict byte 0ffffh                ; 83 f9 ff
    je short 034f8h                           ; 74 20
    lea dx, [bx+002h]                         ; 8d 57 02
    mov ax, 003b6h                            ; b8 b6 03
    call 0342eh                               ; e8 4d ff
    lea dx, [bx+044h]                         ; 8d 57 44
    cmp cx, di                                ; 39 f9
    jne short 034f4h                          ; 75 0c
    test si, si                               ; 85 f6
    jne short 034f0h                          ; 75 04
    mov ax, bx                                ; 89 d8
    jmp short 034fah                          ; eb 0a
    test AL, strict byte 080h                 ; a8 80
    jne short 034ech                          ; 75 f8
    mov bx, dx                                ; 89 d3
    jmp short 034cbh                          ; eb d3
    xor ax, ax                                ; 31 c0
    lea sp, [bp-008h]                         ; 8d 66 f8
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
vbe_biosfn_return_controller_information_: ; 0xc3503 LB 0x129
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0000ah                ; 83 ec 0a
    mov si, ax                                ; 89 c6
    mov di, dx                                ; 89 d7
    mov word [bp-00ah], bx                    ; 89 5e f6
    mov word [bp-00ch], strict word 00022h    ; c7 46 f4 22 00
    call 005b7h                               ; e8 9c d0
    mov word [bp-010h], ax                    ; 89 46 f0
    mov bx, word [bp-00ah]                    ; 8b 5e f6
    mov word [bp-008h], di                    ; 89 7e f8
    xor dx, dx                                ; 31 d2
    mov ax, 003b6h                            ; b8 b6 03
    call 0342eh                               ; e8 02 ff
    cmp ax, 077cch                            ; 3d cc 77
    je short 0353bh                           ; 74 0a
    push SS                                   ; 16
    pop ES                                    ; 07
    mov word [es:si], 00100h                  ; 26 c7 04 00 01
    jmp near 03624h                           ; e9 e9 00
    mov cx, strict word 00004h                ; b9 04 00
    mov word [bp-00eh], strict word 00000h    ; c7 46 f2 00 00
    mov es, [bp-008h]                         ; 8e 46 f8
    cmp word [es:bx+002h], 03245h             ; 26 81 7f 02 45 32
    jne short 03555h                          ; 75 07
    cmp word [es:bx], 04256h                  ; 26 81 3f 56 42
    je short 03564h                           ; 74 0f
    cmp word [es:bx+002h], 04153h             ; 26 81 7f 02 53 41
    jne short 03569h                          ; 75 0c
    cmp word [es:bx], 04556h                  ; 26 81 3f 56 45
    jne short 03569h                          ; 75 05
    mov word [bp-00eh], strict word 00001h    ; c7 46 f2 01 00
    mov es, [bp-008h]                         ; 8e 46 f8
    db  066h, 026h, 0c7h, 007h, 056h, 045h, 053h, 041h
    ; mov dword [es:bx], strict dword 041534556h ; 66 26 c7 07 56 45 53 41
    mov word [es:bx+004h], 00200h             ; 26 c7 47 04 00 02
    mov word [es:bx+006h], 07c5eh             ; 26 c7 47 06 5e 7c
    mov [es:bx+008h], ds                      ; 26 8c 5f 08
    db  066h, 026h, 0c7h, 047h, 00ah, 001h, 000h, 000h, 000h
    ; mov dword [es:bx+00ah], strict dword 000000001h ; 66 26 c7 47 0a 01 00 00 00
    mov word [es:bx+010h], di                 ; 26 89 7f 10
    mov ax, word [bp-00ah]                    ; 8b 46 f6
    add ax, strict word 00022h                ; 05 22 00
    mov word [es:bx+00eh], ax                 ; 26 89 47 0e
    mov dx, strict word 0ffffh                ; ba ff ff
    mov ax, 003b6h                            ; b8 b6 03
    call 0342eh                               ; e8 8a fe
    mov es, [bp-008h]                         ; 8e 46 f8
    mov word [es:bx+012h], ax                 ; 26 89 47 12
    cmp word [bp-00eh], strict byte 00000h    ; 83 7e f2 00
    je short 035d5h                           ; 74 24
    mov word [es:bx+014h], strict word 00003h ; 26 c7 47 14 03 00
    mov word [es:bx+016h], 07c73h             ; 26 c7 47 16 73 7c
    mov [es:bx+018h], ds                      ; 26 8c 5f 18
    mov word [es:bx+01ah], 07c86h             ; 26 c7 47 1a 86 7c
    mov [es:bx+01ch], ds                      ; 26 8c 5f 1c
    mov word [es:bx+01eh], 07ca7h             ; 26 c7 47 1e a7 7c
    mov [es:bx+020h], ds                      ; 26 8c 5f 20
    mov dx, cx                                ; 89 ca
    add dx, strict byte 0001bh                ; 83 c2 1b
    mov ax, 003b6h                            ; b8 b6 03
    call 03440h                               ; e8 60 fe
    xor ah, ah                                ; 30 e4
    cmp ax, word [bp-010h]                    ; 3b 46 f0
    jnbe short 03600h                         ; 77 19
    mov dx, cx                                ; 89 ca
    mov ax, 003b6h                            ; b8 b6 03
    call 0342eh                               ; e8 3f fe
    mov bx, ax                                ; 89 c3
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    add dx, word [bp-00ch]                    ; 03 56 f4
    mov ax, di                                ; 89 f8
    call 02f68h                               ; e8 6c f9
    add word [bp-00ch], strict byte 00002h    ; 83 46 f4 02
    add cx, strict byte 00044h                ; 83 c1 44
    mov dx, cx                                ; 89 ca
    mov ax, 003b6h                            ; b8 b6 03
    call 0342eh                               ; e8 23 fe
    mov bx, ax                                ; 89 c3
    cmp ax, strict word 0ffffh                ; 3d ff ff
    jne short 035d5h                          ; 75 c3
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    add dx, word [bp-00ch]                    ; 03 56 f4
    mov ax, di                                ; 89 f8
    call 02f68h                               ; e8 4b f9
    push SS                                   ; 16
    pop ES                                    ; 07
    mov word [es:si], strict word 0004fh      ; 26 c7 04 4f 00
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bp                                    ; 5d
    retn                                      ; c3
vbe_biosfn_return_mode_information_:         ; 0xc362c LB 0xb8
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    push ax                                   ; 50
    mov ax, dx                                ; 89 d0
    mov si, bx                                ; 89 de
    mov word [bp-006h], cx                    ; 89 4e fa
    test dh, 040h                             ; f6 c6 40
    db  00fh, 095h, 0c2h
    ; setne dl                                  ; 0f 95 c2
    xor dh, dh                                ; 30 f6
    and ah, 001h                              ; 80 e4 01
    call 034aeh                               ; e8 65 fe
    mov word [bp-008h], ax                    ; 89 46 f8
    test ax, ax                               ; 85 c0
    je near 036d2h                            ; 0f 84 80 00
    mov cx, 00100h                            ; b9 00 01
    xor ax, ax                                ; 31 c0
    mov di, word [bp-006h]                    ; 8b 7e fa
    mov es, bx                                ; 8e c3
    cld                                       ; fc
    jcxz 03661h                               ; e3 02
    rep stosb                                 ; f3 aa
    xor cx, cx                                ; 31 c9
    jmp short 0366ah                          ; eb 05
    cmp cx, strict byte 00042h                ; 83 f9 42
    jnc short 03687h                          ; 73 1d
    mov dx, word [bp-008h]                    ; 8b 56 f8
    inc dx                                    ; 42
    inc dx                                    ; 42
    add dx, cx                                ; 01 ca
    mov ax, 003b6h                            ; b8 b6 03
    call 03440h                               ; e8 c9 fd
    movzx bx, al                              ; 0f b6 d8
    mov dx, word [bp-006h]                    ; 8b 56 fa
    add dx, cx                                ; 01 ca
    mov ax, si                                ; 89 f0
    call 02f4ch                               ; e8 c8 f8
    inc cx                                    ; 41
    jmp short 03665h                          ; eb de
    mov dx, word [bp-006h]                    ; 8b 56 fa
    inc dx                                    ; 42
    inc dx                                    ; 42
    mov ax, si                                ; 89 f0
    call 02f3eh                               ; e8 ad f8
    test AL, strict byte 001h                 ; a8 01
    je short 036b1h                           ; 74 1c
    mov dx, word [bp-006h]                    ; 8b 56 fa
    add dx, strict byte 0000ch                ; 83 c2 0c
    mov bx, 00629h                            ; bb 29 06
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 c5 f8
    mov dx, word [bp-006h]                    ; 8b 56 fa
    add dx, strict byte 0000eh                ; 83 c2 0e
    mov bx, 0c000h                            ; bb 00 c0
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 b7 f8
    mov ax, strict word 0000bh                ; b8 0b 00
    mov dx, 001ceh                            ; ba ce 01
    call 00570h                               ; e8 b6 ce
    mov dx, 001cfh                            ; ba cf 01
    call 00577h                               ; e8 b7 ce
    mov dx, word [bp-006h]                    ; 8b 56 fa
    add dx, strict byte 0002ah                ; 83 c2 2a
    mov bx, ax                                ; 89 c3
    mov ax, si                                ; 89 f0
    call 02f68h                               ; e8 9b f8
    mov ax, strict word 0004fh                ; b8 4f 00
    jmp short 036d5h                          ; eb 03
    mov ax, 00100h                            ; b8 00 01
    push SS                                   ; 16
    pop ES                                    ; 07
    mov bx, word [bp-00ah]                    ; 8b 5e f6
    mov word [es:bx], ax                      ; 26 89 07
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
vbe_biosfn_set_mode_:                        ; 0xc36e4 LB 0xe9
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00006h                ; 83 ec 06
    mov si, ax                                ; 89 c6
    mov word [bp-00ah], dx                    ; 89 56 f6
    test byte [bp-009h], 040h                 ; f6 46 f7 40
    db  00fh, 095h, 0c0h
    ; setne al                                  ; 0f 95 c0
    movzx dx, al                              ; 0f b6 d0
    mov ax, dx                                ; 89 d0
    test dx, dx                               ; 85 d2
    je short 03704h                           ; 74 03
    mov dx, strict word 00040h                ; ba 40 00
    mov byte [bp-006h], dl                    ; 88 56 fa
    test byte [bp-009h], 080h                 ; f6 46 f7 80
    je short 03712h                           ; 74 05
    mov dx, 00080h                            ; ba 80 00
    jmp short 03714h                          ; eb 02
    xor dx, dx                                ; 31 d2
    mov byte [bp-008h], dl                    ; 88 56 f8
    and byte [bp-009h], 001h                  ; 80 66 f7 01
    cmp word [bp-00ah], 00100h                ; 81 7e f6 00 01
    jnc short 03734h                          ; 73 12
    xor ax, ax                                ; 31 c0
    call 005ddh                               ; e8 b6 ce
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    call 00fdch                               ; e8 ae d8
    mov ax, strict word 0004fh                ; b8 4f 00
    jmp near 037c3h                           ; e9 8f 00
    mov dx, ax                                ; 89 c2
    mov ax, word [bp-00ah]                    ; 8b 46 f6
    call 034aeh                               ; e8 72 fd
    mov bx, ax                                ; 89 c3
    test ax, ax                               ; 85 c0
    je near 037c0h                            ; 0f 84 7c 00
    lea dx, [bx+014h]                         ; 8d 57 14
    mov ax, 003b6h                            ; b8 b6 03
    call 0342eh                               ; e8 e1 fc
    mov cx, ax                                ; 89 c1
    lea dx, [bx+016h]                         ; 8d 57 16
    mov ax, 003b6h                            ; b8 b6 03
    call 0342eh                               ; e8 d6 fc
    mov di, ax                                ; 89 c7
    lea dx, [bx+01bh]                         ; 8d 57 1b
    mov ax, 003b6h                            ; b8 b6 03
    call 03440h                               ; e8 dd fc
    mov bl, al                                ; 88 c3
    mov dl, al                                ; 88 c2
    xor ax, ax                                ; 31 c0
    call 005ddh                               ; e8 71 ce
    cmp bl, 004h                              ; 80 fb 04
    jne short 03777h                          ; 75 06
    mov ax, strict word 0006ah                ; b8 6a 00
    call 00fdch                               ; e8 65 d8
    movzx ax, dl                              ; 0f b6 c2
    call 0340fh                               ; e8 92 fc
    mov ax, cx                                ; 89 c8
    call 033d1h                               ; e8 4f fc
    mov ax, di                                ; 89 f8
    call 033f0h                               ; e8 69 fc
    xor ax, ax                                ; 31 c0
    call 00603h                               ; e8 77 ce
    mov al, byte [bp-008h]                    ; 8a 46 f8
    or AL, strict byte 001h                   ; 0c 01
    movzx dx, al                              ; 0f b6 d0
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    or ax, dx                                 ; 09 d0
    call 005ddh                               ; e8 40 ce
    call 0070dh                               ; e8 6d cf
    mov bx, word [bp-00ah]                    ; 8b 5e f6
    mov dx, 000bah                            ; ba ba 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f68h                               ; e8 bc f7
    mov al, byte [bp-008h]                    ; 8a 46 f8
    or AL, strict byte 060h                   ; 0c 60
    movzx bx, al                              ; 0f b6 d8
    mov dx, 00087h                            ; ba 87 00
    mov ax, strict word 00040h                ; b8 40 00
    call 02f4ch                               ; e8 8f f7
    jmp near 0372eh                           ; e9 6e ff
    mov ax, 00100h                            ; b8 00 01
    mov word [ss:si], ax                      ; 36 89 04
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
vbe_biosfn_read_video_state_size_:           ; 0xc37cd LB 0x8
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov ax, strict word 00012h                ; b8 12 00
    pop bp                                    ; 5d
    retn                                      ; c3
vbe_biosfn_save_video_state_:                ; 0xc37d5 LB 0x5b
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    mov di, ax                                ; 89 c7
    mov cx, dx                                ; 89 d1
    mov ax, strict word 00004h                ; b8 04 00
    mov dx, 001ceh                            ; ba ce 01
    out DX, ax                                ; ef
    mov dx, 001cfh                            ; ba cf 01
    in ax, DX                                 ; ed
    mov word [bp-00ah], ax                    ; 89 46 f6
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    mov ax, di                                ; 89 f8
    call 02f68h                               ; e8 70 f7
    inc cx                                    ; 41
    inc cx                                    ; 41
    test byte [bp-00ah], 001h                 ; f6 46 f6 01
    je short 03827h                           ; 74 27
    mov si, strict word 00001h                ; be 01 00
    jmp short 0380ah                          ; eb 05
    cmp si, strict byte 00009h                ; 83 fe 09
    jnbe short 03827h                         ; 77 1d
    cmp si, strict byte 00004h                ; 83 fe 04
    je short 03824h                           ; 74 15
    mov ax, si                                ; 89 f0
    mov dx, 001ceh                            ; ba ce 01
    out DX, ax                                ; ef
    mov dx, 001cfh                            ; ba cf 01
    in ax, DX                                 ; ed
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    mov ax, di                                ; 89 f8
    call 02f68h                               ; e8 46 f7
    inc cx                                    ; 41
    inc cx                                    ; 41
    inc si                                    ; 46
    jmp short 03805h                          ; eb de
    lea sp, [bp-008h]                         ; 8d 66 f8
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
vbe_biosfn_restore_video_state_:             ; 0xc3830 LB 0x9b
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push si                                   ; 56
    push ax                                   ; 50
    mov cx, ax                                ; 89 c1
    mov bx, dx                                ; 89 d3
    call 02f5ah                               ; e8 1c f7
    mov word [bp-008h], ax                    ; 89 46 f8
    inc bx                                    ; 43
    inc bx                                    ; 43
    test byte [bp-008h], 001h                 ; f6 46 f8 01
    jne short 03859h                          ; 75 10
    mov ax, strict word 00004h                ; b8 04 00
    mov dx, 001ceh                            ; ba ce 01
    out DX, ax                                ; ef
    mov ax, word [bp-008h]                    ; 8b 46 f8
    mov dx, 001cfh                            ; ba cf 01
    out DX, ax                                ; ef
    jmp short 038c3h                          ; eb 6a
    mov ax, strict word 00001h                ; b8 01 00
    mov dx, 001ceh                            ; ba ce 01
    out DX, ax                                ; ef
    mov dx, bx                                ; 89 da
    mov ax, cx                                ; 89 c8
    call 02f5ah                               ; e8 f3 f6
    mov dx, 001cfh                            ; ba cf 01
    out DX, ax                                ; ef
    inc bx                                    ; 43
    inc bx                                    ; 43
    mov ax, strict word 00002h                ; b8 02 00
    mov dx, 001ceh                            ; ba ce 01
    out DX, ax                                ; ef
    mov dx, bx                                ; 89 da
    mov ax, cx                                ; 89 c8
    call 02f5ah                               ; e8 df f6
    mov dx, 001cfh                            ; ba cf 01
    out DX, ax                                ; ef
    inc bx                                    ; 43
    inc bx                                    ; 43
    mov ax, strict word 00003h                ; b8 03 00
    mov dx, 001ceh                            ; ba ce 01
    out DX, ax                                ; ef
    mov dx, bx                                ; 89 da
    mov ax, cx                                ; 89 c8
    call 02f5ah                               ; e8 cb f6
    mov dx, 001cfh                            ; ba cf 01
    out DX, ax                                ; ef
    inc bx                                    ; 43
    inc bx                                    ; 43
    mov ax, strict word 00004h                ; b8 04 00
    mov dx, 001ceh                            ; ba ce 01
    out DX, ax                                ; ef
    mov ax, word [bp-008h]                    ; 8b 46 f8
    mov dx, 001cfh                            ; ba cf 01
    out DX, ax                                ; ef
    mov si, strict word 00005h                ; be 05 00
    jmp short 038adh                          ; eb 05
    cmp si, strict byte 00009h                ; 83 fe 09
    jnbe short 038c3h                         ; 77 16
    mov ax, si                                ; 89 f0
    mov dx, 001ceh                            ; ba ce 01
    out DX, ax                                ; ef
    mov dx, bx                                ; 89 da
    mov ax, cx                                ; 89 c8
    call 02f5ah                               ; e8 a0 f6
    mov dx, 001cfh                            ; ba cf 01
    out DX, ax                                ; ef
    inc bx                                    ; 43
    inc bx                                    ; 43
    inc si                                    ; 46
    jmp short 038a8h                          ; eb e5
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
vbe_biosfn_save_restore_state_:              ; 0xc38cb LB 0x8c
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    mov si, ax                                ; 89 c6
    mov word [bp-006h], dx                    ; 89 56 fa
    mov ax, bx                                ; 89 d8
    mov bx, word [bp+004h]                    ; 8b 5e 04
    mov di, strict word 0004fh                ; bf 4f 00
    xor ah, ah                                ; 30 e4
    cmp ax, strict word 00002h                ; 3d 02 00
    je short 0392ah                           ; 74 45
    cmp ax, strict word 00001h                ; 3d 01 00
    je short 0390eh                           ; 74 24
    test ax, ax                               ; 85 c0
    jne short 03946h                          ; 75 58
    mov ax, word [bp-006h]                    ; 8b 46 fa
    call 02853h                               ; e8 5f ef
    mov cx, ax                                ; 89 c1
    test byte [bp-006h], 008h                 ; f6 46 fa 08
    je short 03901h                           ; 74 05
    call 037cdh                               ; e8 ce fe
    add ax, cx                                ; 01 c8
    add ax, strict word 0003fh                ; 05 3f 00
    shr ax, 006h                              ; c1 e8 06
    push SS                                   ; 16
    pop ES                                    ; 07
    mov word [es:bx], ax                      ; 26 89 07
    jmp short 03949h                          ; eb 3b
    push SS                                   ; 16
    pop ES                                    ; 07
    mov bx, word [es:bx]                      ; 26 8b 1f
    mov dx, cx                                ; 89 ca
    mov ax, word [bp-006h]                    ; 8b 46 fa
    call 02888h                               ; e8 6d ef
    test byte [bp-006h], 008h                 ; f6 46 fa 08
    je short 03949h                           ; 74 28
    mov dx, ax                                ; 89 c2
    mov ax, cx                                ; 89 c8
    call 037d5h                               ; e8 ad fe
    jmp short 03949h                          ; eb 1f
    push SS                                   ; 16
    pop ES                                    ; 07
    mov bx, word [es:bx]                      ; 26 8b 1f
    mov dx, cx                                ; 89 ca
    mov ax, word [bp-006h]                    ; 8b 46 fa
    call 02bf1h                               ; e8 ba f2
    test byte [bp-006h], 008h                 ; f6 46 fa 08
    je short 03949h                           ; 74 0c
    mov dx, ax                                ; 89 c2
    mov ax, cx                                ; 89 c8
    call 03830h                               ; e8 ec fe
    jmp short 03949h                          ; eb 03
    mov di, 00100h                            ; bf 00 01
    push SS                                   ; 16
    pop ES                                    ; 07
    mov word [es:si], di                      ; 26 89 3c
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00002h                               ; c2 02 00

  ; Padding 0xaa9 bytes at 0xc3957
  times 2729 db 0

section VBE32 progbits vstart=0x4400 align=1 ; size=0x115 class=CODE group=AUTO
vesa_pm_start:                               ; 0xc4400 LB 0x114
    sbb byte [bx+si], al                      ; 18 00
    dec di                                    ; 4f
    add byte [bx+si], dl                      ; 00 10
    add word [bx+si], cx                      ; 01 08
    add dh, cl                                ; 00 ce
    add di, cx                                ; 01 cf
    add di, cx                                ; 01 cf
    add ax, dx                                ; 01 d0
    add word [bp-048fdh], si                  ; 01 b6 03 b7
    db  003h, 0ffh
    ; add di, di                                ; 03 ff
    db  0ffh
    db  0ffh
    jmp word [bp-07dh]                        ; ff 66 83
    sti                                       ; fb
    add byte [si+005h], dh                    ; 00 74 05
    mov eax, strict dword 066c30100h          ; 66 b8 00 01 c3 66
    db  08bh, 0c2h
    ; mov ax, dx                                ; 8b c2
    push edx                                  ; 66 52
    push eax                                  ; 66 50
    mov edx, strict dword 0b86601ceh          ; 66 ba ce 01 66 b8
    add ax, 06600h                            ; 05 00 66
    out DX, ax                                ; ef
    pop eax                                   ; 66 58
    mov edx, strict dword 0ef6601cfh          ; 66 ba cf 01 66 ef
    in eax, DX                                ; 66 ed
    pop edx                                   ; 66 5a
    db  066h, 03bh, 0d0h
    ; cmp edx, eax                              ; 66 3b d0
    jne short 0444ah                          ; 75 05
    mov eax, strict dword 066c3004fh          ; 66 b8 4f 00 c3 66
    mov ax, 0014fh                            ; b8 4f 01
    retn                                      ; c3
    cmp bl, 080h                              ; 80 fb 80
    je short 0445eh                           ; 74 0a
    cmp bl, 000h                              ; 80 fb 00
    je short 0446eh                           ; 74 15
    mov eax, strict dword 052c30100h          ; 66 b8 00 01 c3 52
    mov edx, strict dword 0a8ec03dah          ; 66 ba da 03 ec a8
    or byte [di-005h], dh                     ; 08 75 fb
    in AL, DX                                 ; ec
    test AL, strict byte 008h                 ; a8 08
    je short 04468h                           ; 74 fb
    pop dx                                    ; 5a
    push ax                                   ; 50
    push cx                                   ; 51
    push dx                                   ; 52
    push si                                   ; 56
    push di                                   ; 57
    sal dx, 010h                              ; c1 e2 10
    and cx, strict word 0ffffh                ; 81 e1 ff ff
    add byte [bx+si], al                      ; 00 00
    db  00bh, 0cah
    ; or cx, dx                                 ; 0b ca
    sal cx, 002h                              ; c1 e1 02
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    push ax                                   ; 50
    mov edx, strict dword 0b86601ceh          ; 66 ba ce 01 66 b8
    push ES                                   ; 06
    add byte [bp-011h], ah                    ; 00 66 ef
    mov edx, strict dword 0ed6601cfh          ; 66 ba cf 01 66 ed
    db  00fh, 0b7h, 0c8h
    ; movzx cx, ax                              ; 0f b7 c8
    mov edx, strict dword 0b86601ceh          ; 66 ba ce 01 66 b8
    add ax, word [bx+si]                      ; 03 00
    out DX, eax                               ; 66 ef
    mov edx, strict dword 0ed6601cfh          ; 66 ba cf 01 66 ed
    db  00fh, 0b7h, 0f0h
    ; movzx si, ax                              ; 0f b7 f0
    pop ax                                    ; 58
    cmp si, strict byte 00004h                ; 83 fe 04
    je short 044c7h                           ; 74 17
    add si, strict byte 00007h                ; 83 c6 07
    shr si, 003h                              ; c1 ee 03
    imul cx, si                               ; 0f af ce
    db  033h, 0d2h
    ; xor dx, dx                                ; 33 d2
    div cx                                    ; f7 f1
    db  08bh, 0f8h
    ; mov di, ax                                ; 8b f8
    db  08bh, 0c2h
    ; mov ax, dx                                ; 8b c2
    db  033h, 0d2h
    ; xor dx, dx                                ; 33 d2
    div si                                    ; f7 f6
    jmp short 044d3h                          ; eb 0c
    shr cx, 1                                 ; d1 e9
    db  033h, 0d2h
    ; xor dx, dx                                ; 33 d2
    div cx                                    ; f7 f1
    db  08bh, 0f8h
    ; mov di, ax                                ; 8b f8
    db  08bh, 0c2h
    ; mov ax, dx                                ; 8b c2
    sal ax, 1                                 ; d1 e0
    push edx                                  ; 66 52
    push eax                                  ; 66 50
    mov edx, strict dword 0b86601ceh          ; 66 ba ce 01 66 b8
    or byte [bx+si], al                       ; 08 00
    out DX, eax                               ; 66 ef
    pop eax                                   ; 66 58
    mov edx, strict dword 0ef6601cfh          ; 66 ba cf 01 66 ef
    pop edx                                   ; 66 5a
    db  066h, 08bh, 0c7h
    ; mov eax, edi                              ; 66 8b c7
    push edx                                  ; 66 52
    push eax                                  ; 66 50
    mov edx, strict dword 0b86601ceh          ; 66 ba ce 01 66 b8
    or word [bx+si], ax                       ; 09 00
    out DX, eax                               ; 66 ef
    pop eax                                   ; 66 58
    mov edx, strict dword 0ef6601cfh          ; 66 ba cf 01 66 ef
    pop edx                                   ; 66 5a
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop ax                                    ; 58
    mov eax, strict dword 066c3004fh          ; 66 b8 4f 00 c3 66
    mov ax, 0014fh                            ; b8 4f 01
vesa_pm_end:                                 ; 0xc4514 LB 0x1
    retn                                      ; c3

  ; Padding 0xeb bytes at 0xc4515
  times 235 db 0

section _DATA progbits vstart=0x4600 align=1 ; size=0x371e class=DATA group=DGROUP
_msg_vga_init:                               ; 0xc4600 LB 0x2e
    db  'Oracle VM VirtualBox Version 5.0.2 VGA BIOS', 00dh, 00ah, 000h
_vga_modes:                                  ; 0xc462e LB 0x80
    db  000h, 000h, 000h, 004h, 000h, 0b8h, 0ffh, 002h, 001h, 000h, 000h, 004h, 000h, 0b8h, 0ffh, 002h
    db  002h, 000h, 000h, 004h, 000h, 0b8h, 0ffh, 002h, 003h, 000h, 000h, 004h, 000h, 0b8h, 0ffh, 002h
    db  004h, 001h, 002h, 002h, 000h, 0b8h, 0ffh, 001h, 005h, 001h, 002h, 002h, 000h, 0b8h, 0ffh, 001h
    db  006h, 001h, 002h, 001h, 000h, 0b8h, 0ffh, 001h, 007h, 000h, 001h, 004h, 000h, 0b0h, 0ffh, 000h
    db  00dh, 001h, 004h, 004h, 000h, 0a0h, 0ffh, 001h, 00eh, 001h, 004h, 004h, 000h, 0a0h, 0ffh, 001h
    db  00fh, 001h, 003h, 001h, 000h, 0a0h, 0ffh, 000h, 010h, 001h, 004h, 004h, 000h, 0a0h, 0ffh, 002h
    db  011h, 001h, 003h, 001h, 000h, 0a0h, 0ffh, 002h, 012h, 001h, 004h, 004h, 000h, 0a0h, 0ffh, 002h
    db  013h, 001h, 005h, 008h, 000h, 0a0h, 0ffh, 003h, 06ah, 001h, 004h, 004h, 000h, 0a0h, 0ffh, 002h
_line_to_vpti:                               ; 0xc46ae LB 0x10
    db  017h, 017h, 018h, 018h, 004h, 005h, 006h, 007h, 00dh, 00eh, 011h, 012h, 01ah, 01bh, 01ch, 01dh
_dac_regs:                                   ; 0xc46be LB 0x4
    dd  0ff3f3f3fh
_video_param_table:                          ; 0xc46c2 LB 0x780
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  028h, 018h, 008h, 000h, 008h, 009h, 003h, 000h, 002h, 063h, 02dh, 027h, 028h, 090h, 02bh, 080h
    db  0bfh, 01fh, 000h, 0c1h, 000h, 000h, 000h, 000h, 000h, 000h, 09ch, 08eh, 08fh, 014h, 000h, 096h
    db  0b9h, 0a2h, 0ffh, 000h, 013h, 015h, 017h, 002h, 004h, 006h, 007h, 010h, 011h, 012h, 013h, 014h
    db  015h, 016h, 017h, 001h, 000h, 003h, 000h, 000h, 000h, 000h, 000h, 000h, 030h, 00fh, 00fh, 0ffh
    db  028h, 018h, 008h, 000h, 008h, 009h, 003h, 000h, 002h, 063h, 02dh, 027h, 028h, 090h, 02bh, 080h
    db  0bfh, 01fh, 000h, 0c1h, 000h, 000h, 000h, 000h, 000h, 000h, 09ch, 08eh, 08fh, 014h, 000h, 096h
    db  0b9h, 0a2h, 0ffh, 000h, 013h, 015h, 017h, 002h, 004h, 006h, 007h, 010h, 011h, 012h, 013h, 014h
    db  015h, 016h, 017h, 001h, 000h, 003h, 000h, 000h, 000h, 000h, 000h, 000h, 030h, 00fh, 00fh, 0ffh
    db  050h, 018h, 008h, 000h, 010h, 001h, 001h, 000h, 006h, 063h, 05fh, 04fh, 050h, 082h, 054h, 080h
    db  0bfh, 01fh, 000h, 0c1h, 000h, 000h, 000h, 000h, 000h, 000h, 09ch, 08eh, 08fh, 028h, 000h, 096h
    db  0b9h, 0c2h, 0ffh, 000h, 017h, 017h, 017h, 017h, 017h, 017h, 017h, 017h, 017h, 017h, 017h, 017h
    db  017h, 017h, 017h, 001h, 000h, 001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 00dh, 00fh, 0ffh
    db  050h, 018h, 010h, 000h, 010h, 000h, 003h, 000h, 002h, 066h, 05fh, 04fh, 050h, 082h, 055h, 081h
    db  0bfh, 01fh, 000h, 04fh, 00dh, 00eh, 000h, 000h, 000h, 000h, 09ch, 08eh, 08fh, 028h, 00fh, 096h
    db  0b9h, 0a3h, 0ffh, 000h, 008h, 008h, 008h, 008h, 008h, 008h, 008h, 010h, 018h, 018h, 018h, 018h
    db  018h, 018h, 018h, 00eh, 000h, 00fh, 008h, 000h, 000h, 000h, 000h, 000h, 010h, 00ah, 00fh, 0ffh
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  028h, 018h, 008h, 000h, 020h, 009h, 00fh, 000h, 006h, 063h, 02dh, 027h, 028h, 090h, 02bh, 080h
    db  0bfh, 01fh, 000h, 0c0h, 000h, 000h, 000h, 000h, 000h, 000h, 09ch, 08eh, 08fh, 014h, 000h, 096h
    db  0b9h, 0e3h, 0ffh, 000h, 001h, 002h, 003h, 004h, 005h, 006h, 007h, 010h, 011h, 012h, 013h, 014h
    db  015h, 016h, 017h, 001h, 000h, 00fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 005h, 00fh, 0ffh
    db  050h, 018h, 008h, 000h, 040h, 001h, 00fh, 000h, 006h, 063h, 05fh, 04fh, 050h, 082h, 054h, 080h
    db  0bfh, 01fh, 000h, 0c0h, 000h, 000h, 000h, 000h, 000h, 000h, 09ch, 08eh, 08fh, 028h, 000h, 096h
    db  0b9h, 0e3h, 0ffh, 000h, 001h, 002h, 003h, 004h, 005h, 006h, 007h, 010h, 011h, 012h, 013h, 014h
    db  015h, 016h, 017h, 001h, 000h, 00fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 005h, 00fh, 0ffh
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  050h, 018h, 00eh, 000h, 080h, 001h, 00fh, 000h, 006h, 0a3h, 05fh, 04fh, 050h, 082h, 054h, 080h
    db  0bfh, 01fh, 000h, 040h, 000h, 000h, 000h, 000h, 000h, 000h, 083h, 085h, 05dh, 028h, 00fh, 063h
    db  0bah, 0e3h, 0ffh, 000h, 008h, 000h, 000h, 018h, 018h, 000h, 000h, 000h, 008h, 000h, 000h, 000h
    db  018h, 000h, 000h, 001h, 000h, 001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 005h, 00fh, 0ffh
    db  050h, 018h, 00eh, 000h, 080h, 001h, 00fh, 000h, 006h, 0a3h, 05fh, 04fh, 050h, 082h, 054h, 080h
    db  0bfh, 01fh, 000h, 040h, 000h, 000h, 000h, 000h, 000h, 000h, 083h, 085h, 05dh, 028h, 00fh, 063h
    db  0bah, 0e3h, 0ffh, 000h, 001h, 002h, 003h, 004h, 005h, 014h, 007h, 038h, 039h, 03ah, 03bh, 03ch
    db  03dh, 03eh, 03fh, 001h, 000h, 00fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 005h, 00fh, 0ffh
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  050h, 018h, 00eh, 000h, 010h, 000h, 003h, 000h, 002h, 067h, 05fh, 04fh, 050h, 082h, 055h, 081h
    db  0bfh, 01fh, 000h, 04fh, 00dh, 00eh, 000h, 000h, 000h, 000h, 09ch, 08eh, 08fh, 028h, 01fh, 096h
    db  0b9h, 0a3h, 0ffh, 000h, 001h, 002h, 003h, 004h, 005h, 014h, 007h, 038h, 039h, 03ah, 03bh, 03ch
    db  03dh, 03eh, 03fh, 00ch, 000h, 00fh, 008h, 000h, 000h, 000h, 000h, 000h, 010h, 00eh, 00fh, 0ffh
    db  028h, 018h, 010h, 000h, 008h, 008h, 003h, 000h, 002h, 067h, 02dh, 027h, 028h, 090h, 02bh, 0a0h
    db  0bfh, 01fh, 000h, 04fh, 00dh, 00eh, 000h, 000h, 000h, 000h, 09ch, 08eh, 08fh, 014h, 01fh, 096h
    db  0b9h, 0a3h, 0ffh, 000h, 001h, 002h, 003h, 004h, 005h, 014h, 007h, 038h, 039h, 03ah, 03bh, 03ch
    db  03dh, 03eh, 03fh, 00ch, 000h, 00fh, 008h, 000h, 000h, 000h, 000h, 000h, 010h, 00eh, 00fh, 0ffh
    db  050h, 018h, 010h, 000h, 010h, 000h, 003h, 000h, 002h, 067h, 05fh, 04fh, 050h, 082h, 055h, 081h
    db  0bfh, 01fh, 000h, 04fh, 00dh, 00eh, 000h, 000h, 000h, 000h, 09ch, 08eh, 08fh, 028h, 01fh, 096h
    db  0b9h, 0a3h, 0ffh, 000h, 001h, 002h, 003h, 004h, 005h, 014h, 007h, 038h, 039h, 03ah, 03bh, 03ch
    db  03dh, 03eh, 03fh, 00ch, 000h, 00fh, 008h, 000h, 000h, 000h, 000h, 000h, 010h, 00eh, 00fh, 0ffh
    db  050h, 018h, 010h, 000h, 010h, 000h, 003h, 000h, 002h, 066h, 05fh, 04fh, 050h, 082h, 055h, 081h
    db  0bfh, 01fh, 000h, 04fh, 00dh, 00eh, 000h, 000h, 000h, 000h, 09ch, 08eh, 08fh, 028h, 00fh, 096h
    db  0b9h, 0a3h, 0ffh, 000h, 008h, 008h, 008h, 008h, 008h, 008h, 008h, 010h, 018h, 018h, 018h, 018h
    db  018h, 018h, 018h, 00eh, 000h, 00fh, 008h, 000h, 000h, 000h, 000h, 000h, 010h, 00ah, 00fh, 0ffh
    db  050h, 01dh, 010h, 000h, 000h, 001h, 00fh, 000h, 006h, 0e3h, 05fh, 04fh, 050h, 082h, 054h, 080h
    db  00bh, 03eh, 000h, 040h, 000h, 000h, 000h, 000h, 000h, 000h, 0eah, 08ch, 0dfh, 028h, 000h, 0e7h
    db  004h, 0e3h, 0ffh, 000h, 03fh, 000h, 03fh, 000h, 03fh, 000h, 03fh, 000h, 03fh, 000h, 03fh, 000h
    db  03fh, 000h, 03fh, 001h, 000h, 00fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 005h, 00fh, 0ffh
    db  050h, 01dh, 010h, 000h, 000h, 001h, 00fh, 000h, 006h, 0e3h, 05fh, 04fh, 050h, 082h, 054h, 080h
    db  00bh, 03eh, 000h, 040h, 000h, 000h, 000h, 000h, 000h, 000h, 0eah, 08ch, 0dfh, 028h, 000h, 0e7h
    db  004h, 0e3h, 0ffh, 000h, 001h, 002h, 003h, 004h, 005h, 014h, 007h, 038h, 039h, 03ah, 03bh, 03ch
    db  03dh, 03eh, 03fh, 001h, 000h, 00fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 005h, 00fh, 0ffh
    db  028h, 018h, 008h, 000h, 000h, 001h, 00fh, 000h, 00eh, 063h, 05fh, 04fh, 050h, 082h, 054h, 080h
    db  0bfh, 01fh, 000h, 041h, 000h, 000h, 000h, 000h, 000h, 000h, 09ch, 08eh, 08fh, 028h, 040h, 096h
    db  0b9h, 0a3h, 0ffh, 000h, 001h, 002h, 003h, 004h, 005h, 006h, 007h, 008h, 009h, 00ah, 00bh, 00ch
    db  00dh, 00eh, 00fh, 041h, 000h, 00fh, 000h, 000h, 000h, 000h, 000h, 000h, 040h, 005h, 00fh, 0ffh
    db  064h, 024h, 010h, 000h, 000h, 001h, 00fh, 000h, 006h, 0e3h, 07fh, 063h, 063h, 083h, 06bh, 01bh
    db  072h, 0f0h, 000h, 060h, 000h, 000h, 000h, 000h, 000h, 000h, 059h, 08dh, 057h, 032h, 000h, 057h
    db  073h, 0e3h, 0ffh, 000h, 001h, 002h, 003h, 004h, 005h, 014h, 007h, 038h, 039h, 03ah, 03bh, 03ch
    db  03dh, 03eh, 03fh, 001h, 000h, 00fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 005h, 00fh, 0ffh
_palette0:                                   ; 0xc4e42 LB 0xc0
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah
    db  02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah
    db  02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah
    db  02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh
    db  03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah
    db  02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah
    db  02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah
    db  02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 02ah, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh
    db  03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh, 03fh
_palette1:                                   ; 0xc4f02 LB 0xc0
    db  000h, 000h, 000h, 000h, 000h, 02ah, 000h, 02ah, 000h, 000h, 02ah, 02ah, 02ah, 000h, 000h, 02ah
    db  000h, 02ah, 02ah, 015h, 000h, 02ah, 02ah, 02ah, 000h, 000h, 000h, 000h, 000h, 02ah, 000h, 02ah
    db  000h, 000h, 02ah, 02ah, 02ah, 000h, 000h, 02ah, 000h, 02ah, 02ah, 015h, 000h, 02ah, 02ah, 02ah
    db  015h, 015h, 015h, 015h, 015h, 03fh, 015h, 03fh, 015h, 015h, 03fh, 03fh, 03fh, 015h, 015h, 03fh
    db  015h, 03fh, 03fh, 03fh, 015h, 03fh, 03fh, 03fh, 015h, 015h, 015h, 015h, 015h, 03fh, 015h, 03fh
    db  015h, 015h, 03fh, 03fh, 03fh, 015h, 015h, 03fh, 015h, 03fh, 03fh, 03fh, 015h, 03fh, 03fh, 03fh
    db  000h, 000h, 000h, 000h, 000h, 02ah, 000h, 02ah, 000h, 000h, 02ah, 02ah, 02ah, 000h, 000h, 02ah
    db  000h, 02ah, 02ah, 015h, 000h, 02ah, 02ah, 02ah, 000h, 000h, 000h, 000h, 000h, 02ah, 000h, 02ah
    db  000h, 000h, 02ah, 02ah, 02ah, 000h, 000h, 02ah, 000h, 02ah, 02ah, 015h, 000h, 02ah, 02ah, 02ah
    db  015h, 015h, 015h, 015h, 015h, 03fh, 015h, 03fh, 015h, 015h, 03fh, 03fh, 03fh, 015h, 015h, 03fh
    db  015h, 03fh, 03fh, 03fh, 015h, 03fh, 03fh, 03fh, 015h, 015h, 015h, 015h, 015h, 03fh, 015h, 03fh
    db  015h, 015h, 03fh, 03fh, 03fh, 015h, 015h, 03fh, 015h, 03fh, 03fh, 03fh, 015h, 03fh, 03fh, 03fh
_palette2:                                   ; 0xc4fc2 LB 0xc0
    db  000h, 000h, 000h, 000h, 000h, 02ah, 000h, 02ah, 000h, 000h, 02ah, 02ah, 02ah, 000h, 000h, 02ah
    db  000h, 02ah, 02ah, 02ah, 000h, 02ah, 02ah, 02ah, 000h, 000h, 015h, 000h, 000h, 03fh, 000h, 02ah
    db  015h, 000h, 02ah, 03fh, 02ah, 000h, 015h, 02ah, 000h, 03fh, 02ah, 02ah, 015h, 02ah, 02ah, 03fh
    db  000h, 015h, 000h, 000h, 015h, 02ah, 000h, 03fh, 000h, 000h, 03fh, 02ah, 02ah, 015h, 000h, 02ah
    db  015h, 02ah, 02ah, 03fh, 000h, 02ah, 03fh, 02ah, 000h, 015h, 015h, 000h, 015h, 03fh, 000h, 03fh
    db  015h, 000h, 03fh, 03fh, 02ah, 015h, 015h, 02ah, 015h, 03fh, 02ah, 03fh, 015h, 02ah, 03fh, 03fh
    db  015h, 000h, 000h, 015h, 000h, 02ah, 015h, 02ah, 000h, 015h, 02ah, 02ah, 03fh, 000h, 000h, 03fh
    db  000h, 02ah, 03fh, 02ah, 000h, 03fh, 02ah, 02ah, 015h, 000h, 015h, 015h, 000h, 03fh, 015h, 02ah
    db  015h, 015h, 02ah, 03fh, 03fh, 000h, 015h, 03fh, 000h, 03fh, 03fh, 02ah, 015h, 03fh, 02ah, 03fh
    db  015h, 015h, 000h, 015h, 015h, 02ah, 015h, 03fh, 000h, 015h, 03fh, 02ah, 03fh, 015h, 000h, 03fh
    db  015h, 02ah, 03fh, 03fh, 000h, 03fh, 03fh, 02ah, 015h, 015h, 015h, 015h, 015h, 03fh, 015h, 03fh
    db  015h, 015h, 03fh, 03fh, 03fh, 015h, 015h, 03fh, 015h, 03fh, 03fh, 03fh, 015h, 03fh, 03fh, 03fh
_palette3:                                   ; 0xc5082 LB 0x300
    db  000h, 000h, 000h, 000h, 000h, 02ah, 000h, 02ah, 000h, 000h, 02ah, 02ah, 02ah, 000h, 000h, 02ah
    db  000h, 02ah, 02ah, 015h, 000h, 02ah, 02ah, 02ah, 015h, 015h, 015h, 015h, 015h, 03fh, 015h, 03fh
    db  015h, 015h, 03fh, 03fh, 03fh, 015h, 015h, 03fh, 015h, 03fh, 03fh, 03fh, 015h, 03fh, 03fh, 03fh
    db  000h, 000h, 000h, 005h, 005h, 005h, 008h, 008h, 008h, 00bh, 00bh, 00bh, 00eh, 00eh, 00eh, 011h
    db  011h, 011h, 014h, 014h, 014h, 018h, 018h, 018h, 01ch, 01ch, 01ch, 020h, 020h, 020h, 024h, 024h
    db  024h, 028h, 028h, 028h, 02dh, 02dh, 02dh, 032h, 032h, 032h, 038h, 038h, 038h, 03fh, 03fh, 03fh
    db  000h, 000h, 03fh, 010h, 000h, 03fh, 01fh, 000h, 03fh, 02fh, 000h, 03fh, 03fh, 000h, 03fh, 03fh
    db  000h, 02fh, 03fh, 000h, 01fh, 03fh, 000h, 010h, 03fh, 000h, 000h, 03fh, 010h, 000h, 03fh, 01fh
    db  000h, 03fh, 02fh, 000h, 03fh, 03fh, 000h, 02fh, 03fh, 000h, 01fh, 03fh, 000h, 010h, 03fh, 000h
    db  000h, 03fh, 000h, 000h, 03fh, 010h, 000h, 03fh, 01fh, 000h, 03fh, 02fh, 000h, 03fh, 03fh, 000h
    db  02fh, 03fh, 000h, 01fh, 03fh, 000h, 010h, 03fh, 01fh, 01fh, 03fh, 027h, 01fh, 03fh, 02fh, 01fh
    db  03fh, 037h, 01fh, 03fh, 03fh, 01fh, 03fh, 03fh, 01fh, 037h, 03fh, 01fh, 02fh, 03fh, 01fh, 027h
    db  03fh, 01fh, 01fh, 03fh, 027h, 01fh, 03fh, 02fh, 01fh, 03fh, 037h, 01fh, 03fh, 03fh, 01fh, 037h
    db  03fh, 01fh, 02fh, 03fh, 01fh, 027h, 03fh, 01fh, 01fh, 03fh, 01fh, 01fh, 03fh, 027h, 01fh, 03fh
    db  02fh, 01fh, 03fh, 037h, 01fh, 03fh, 03fh, 01fh, 037h, 03fh, 01fh, 02fh, 03fh, 01fh, 027h, 03fh
    db  02dh, 02dh, 03fh, 031h, 02dh, 03fh, 036h, 02dh, 03fh, 03ah, 02dh, 03fh, 03fh, 02dh, 03fh, 03fh
    db  02dh, 03ah, 03fh, 02dh, 036h, 03fh, 02dh, 031h, 03fh, 02dh, 02dh, 03fh, 031h, 02dh, 03fh, 036h
    db  02dh, 03fh, 03ah, 02dh, 03fh, 03fh, 02dh, 03ah, 03fh, 02dh, 036h, 03fh, 02dh, 031h, 03fh, 02dh
    db  02dh, 03fh, 02dh, 02dh, 03fh, 031h, 02dh, 03fh, 036h, 02dh, 03fh, 03ah, 02dh, 03fh, 03fh, 02dh
    db  03ah, 03fh, 02dh, 036h, 03fh, 02dh, 031h, 03fh, 000h, 000h, 01ch, 007h, 000h, 01ch, 00eh, 000h
    db  01ch, 015h, 000h, 01ch, 01ch, 000h, 01ch, 01ch, 000h, 015h, 01ch, 000h, 00eh, 01ch, 000h, 007h
    db  01ch, 000h, 000h, 01ch, 007h, 000h, 01ch, 00eh, 000h, 01ch, 015h, 000h, 01ch, 01ch, 000h, 015h
    db  01ch, 000h, 00eh, 01ch, 000h, 007h, 01ch, 000h, 000h, 01ch, 000h, 000h, 01ch, 007h, 000h, 01ch
    db  00eh, 000h, 01ch, 015h, 000h, 01ch, 01ch, 000h, 015h, 01ch, 000h, 00eh, 01ch, 000h, 007h, 01ch
    db  00eh, 00eh, 01ch, 011h, 00eh, 01ch, 015h, 00eh, 01ch, 018h, 00eh, 01ch, 01ch, 00eh, 01ch, 01ch
    db  00eh, 018h, 01ch, 00eh, 015h, 01ch, 00eh, 011h, 01ch, 00eh, 00eh, 01ch, 011h, 00eh, 01ch, 015h
    db  00eh, 01ch, 018h, 00eh, 01ch, 01ch, 00eh, 018h, 01ch, 00eh, 015h, 01ch, 00eh, 011h, 01ch, 00eh
    db  00eh, 01ch, 00eh, 00eh, 01ch, 011h, 00eh, 01ch, 015h, 00eh, 01ch, 018h, 00eh, 01ch, 01ch, 00eh
    db  018h, 01ch, 00eh, 015h, 01ch, 00eh, 011h, 01ch, 014h, 014h, 01ch, 016h, 014h, 01ch, 018h, 014h
    db  01ch, 01ah, 014h, 01ch, 01ch, 014h, 01ch, 01ch, 014h, 01ah, 01ch, 014h, 018h, 01ch, 014h, 016h
    db  01ch, 014h, 014h, 01ch, 016h, 014h, 01ch, 018h, 014h, 01ch, 01ah, 014h, 01ch, 01ch, 014h, 01ah
    db  01ch, 014h, 018h, 01ch, 014h, 016h, 01ch, 014h, 014h, 01ch, 014h, 014h, 01ch, 016h, 014h, 01ch
    db  018h, 014h, 01ch, 01ah, 014h, 01ch, 01ch, 014h, 01ah, 01ch, 014h, 018h, 01ch, 014h, 016h, 01ch
    db  000h, 000h, 010h, 004h, 000h, 010h, 008h, 000h, 010h, 00ch, 000h, 010h, 010h, 000h, 010h, 010h
    db  000h, 00ch, 010h, 000h, 008h, 010h, 000h, 004h, 010h, 000h, 000h, 010h, 004h, 000h, 010h, 008h
    db  000h, 010h, 00ch, 000h, 010h, 010h, 000h, 00ch, 010h, 000h, 008h, 010h, 000h, 004h, 010h, 000h
    db  000h, 010h, 000h, 000h, 010h, 004h, 000h, 010h, 008h, 000h, 010h, 00ch, 000h, 010h, 010h, 000h
    db  00ch, 010h, 000h, 008h, 010h, 000h, 004h, 010h, 008h, 008h, 010h, 00ah, 008h, 010h, 00ch, 008h
    db  010h, 00eh, 008h, 010h, 010h, 008h, 010h, 010h, 008h, 00eh, 010h, 008h, 00ch, 010h, 008h, 00ah
    db  010h, 008h, 008h, 010h, 00ah, 008h, 010h, 00ch, 008h, 010h, 00eh, 008h, 010h, 010h, 008h, 00eh
    db  010h, 008h, 00ch, 010h, 008h, 00ah, 010h, 008h, 008h, 010h, 008h, 008h, 010h, 00ah, 008h, 010h
    db  00ch, 008h, 010h, 00eh, 008h, 010h, 010h, 008h, 00eh, 010h, 008h, 00ch, 010h, 008h, 00ah, 010h
    db  00bh, 00bh, 010h, 00ch, 00bh, 010h, 00dh, 00bh, 010h, 00fh, 00bh, 010h, 010h, 00bh, 010h, 010h
    db  00bh, 00fh, 010h, 00bh, 00dh, 010h, 00bh, 00ch, 010h, 00bh, 00bh, 010h, 00ch, 00bh, 010h, 00dh
    db  00bh, 010h, 00fh, 00bh, 010h, 010h, 00bh, 00fh, 010h, 00bh, 00dh, 010h, 00bh, 00ch, 010h, 00bh
    db  00bh, 010h, 00bh, 00bh, 010h, 00ch, 00bh, 010h, 00dh, 00bh, 010h, 00fh, 00bh, 010h, 010h, 00bh
    db  00fh, 010h, 00bh, 00dh, 010h, 00bh, 00ch, 010h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
_static_functionality:                       ; 0xc5382 LB 0x10
    db  0ffh, 0e0h, 00fh, 000h, 000h, 000h, 000h, 007h, 002h, 008h, 0e7h, 00ch, 000h, 000h, 000h, 000h
_dcc_table:                                  ; 0xc5392 LB 0x24
    db  010h, 001h, 007h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h
_secondary_save_area:                        ; 0xc53b6 LB 0x1a
    db  01ah, 000h, 092h, 053h, 000h, 0c0h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
_video_save_pointer_table:                   ; 0xc53d0 LB 0x1c
    db  0c2h, 046h, 000h, 0c0h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  0b6h, 053h, 000h, 0c0h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
_vgafont8:                                   ; 0xc53ec LB 0x800
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 07eh, 081h, 0a5h, 081h, 0bdh, 099h, 081h, 07eh
    db  07eh, 0ffh, 0dbh, 0ffh, 0c3h, 0e7h, 0ffh, 07eh, 06ch, 0feh, 0feh, 0feh, 07ch, 038h, 010h, 000h
    db  010h, 038h, 07ch, 0feh, 07ch, 038h, 010h, 000h, 038h, 07ch, 038h, 0feh, 0feh, 07ch, 038h, 07ch
    db  010h, 010h, 038h, 07ch, 0feh, 07ch, 038h, 07ch, 000h, 000h, 018h, 03ch, 03ch, 018h, 000h, 000h
    db  0ffh, 0ffh, 0e7h, 0c3h, 0c3h, 0e7h, 0ffh, 0ffh, 000h, 03ch, 066h, 042h, 042h, 066h, 03ch, 000h
    db  0ffh, 0c3h, 099h, 0bdh, 0bdh, 099h, 0c3h, 0ffh, 00fh, 007h, 00fh, 07dh, 0cch, 0cch, 0cch, 078h
    db  03ch, 066h, 066h, 066h, 03ch, 018h, 07eh, 018h, 03fh, 033h, 03fh, 030h, 030h, 070h, 0f0h, 0e0h
    db  07fh, 063h, 07fh, 063h, 063h, 067h, 0e6h, 0c0h, 099h, 05ah, 03ch, 0e7h, 0e7h, 03ch, 05ah, 099h
    db  080h, 0e0h, 0f8h, 0feh, 0f8h, 0e0h, 080h, 000h, 002h, 00eh, 03eh, 0feh, 03eh, 00eh, 002h, 000h
    db  018h, 03ch, 07eh, 018h, 018h, 07eh, 03ch, 018h, 066h, 066h, 066h, 066h, 066h, 000h, 066h, 000h
    db  07fh, 0dbh, 0dbh, 07bh, 01bh, 01bh, 01bh, 000h, 03eh, 063h, 038h, 06ch, 06ch, 038h, 0cch, 078h
    db  000h, 000h, 000h, 000h, 07eh, 07eh, 07eh, 000h, 018h, 03ch, 07eh, 018h, 07eh, 03ch, 018h, 0ffh
    db  018h, 03ch, 07eh, 018h, 018h, 018h, 018h, 000h, 018h, 018h, 018h, 018h, 07eh, 03ch, 018h, 000h
    db  000h, 018h, 00ch, 0feh, 00ch, 018h, 000h, 000h, 000h, 030h, 060h, 0feh, 060h, 030h, 000h, 000h
    db  000h, 000h, 0c0h, 0c0h, 0c0h, 0feh, 000h, 000h, 000h, 024h, 066h, 0ffh, 066h, 024h, 000h, 000h
    db  000h, 018h, 03ch, 07eh, 0ffh, 0ffh, 000h, 000h, 000h, 0ffh, 0ffh, 07eh, 03ch, 018h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 030h, 078h, 078h, 030h, 030h, 000h, 030h, 000h
    db  06ch, 06ch, 06ch, 000h, 000h, 000h, 000h, 000h, 06ch, 06ch, 0feh, 06ch, 0feh, 06ch, 06ch, 000h
    db  030h, 07ch, 0c0h, 078h, 00ch, 0f8h, 030h, 000h, 000h, 0c6h, 0cch, 018h, 030h, 066h, 0c6h, 000h
    db  038h, 06ch, 038h, 076h, 0dch, 0cch, 076h, 000h, 060h, 060h, 0c0h, 000h, 000h, 000h, 000h, 000h
    db  018h, 030h, 060h, 060h, 060h, 030h, 018h, 000h, 060h, 030h, 018h, 018h, 018h, 030h, 060h, 000h
    db  000h, 066h, 03ch, 0ffh, 03ch, 066h, 000h, 000h, 000h, 030h, 030h, 0fch, 030h, 030h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 030h, 030h, 060h, 000h, 000h, 000h, 0fch, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 030h, 030h, 000h, 006h, 00ch, 018h, 030h, 060h, 0c0h, 080h, 000h
    db  07ch, 0c6h, 0ceh, 0deh, 0f6h, 0e6h, 07ch, 000h, 030h, 070h, 030h, 030h, 030h, 030h, 0fch, 000h
    db  078h, 0cch, 00ch, 038h, 060h, 0cch, 0fch, 000h, 078h, 0cch, 00ch, 038h, 00ch, 0cch, 078h, 000h
    db  01ch, 03ch, 06ch, 0cch, 0feh, 00ch, 01eh, 000h, 0fch, 0c0h, 0f8h, 00ch, 00ch, 0cch, 078h, 000h
    db  038h, 060h, 0c0h, 0f8h, 0cch, 0cch, 078h, 000h, 0fch, 0cch, 00ch, 018h, 030h, 030h, 030h, 000h
    db  078h, 0cch, 0cch, 078h, 0cch, 0cch, 078h, 000h, 078h, 0cch, 0cch, 07ch, 00ch, 018h, 070h, 000h
    db  000h, 030h, 030h, 000h, 000h, 030h, 030h, 000h, 000h, 030h, 030h, 000h, 000h, 030h, 030h, 060h
    db  018h, 030h, 060h, 0c0h, 060h, 030h, 018h, 000h, 000h, 000h, 0fch, 000h, 000h, 0fch, 000h, 000h
    db  060h, 030h, 018h, 00ch, 018h, 030h, 060h, 000h, 078h, 0cch, 00ch, 018h, 030h, 000h, 030h, 000h
    db  07ch, 0c6h, 0deh, 0deh, 0deh, 0c0h, 078h, 000h, 030h, 078h, 0cch, 0cch, 0fch, 0cch, 0cch, 000h
    db  0fch, 066h, 066h, 07ch, 066h, 066h, 0fch, 000h, 03ch, 066h, 0c0h, 0c0h, 0c0h, 066h, 03ch, 000h
    db  0f8h, 06ch, 066h, 066h, 066h, 06ch, 0f8h, 000h, 0feh, 062h, 068h, 078h, 068h, 062h, 0feh, 000h
    db  0feh, 062h, 068h, 078h, 068h, 060h, 0f0h, 000h, 03ch, 066h, 0c0h, 0c0h, 0ceh, 066h, 03eh, 000h
    db  0cch, 0cch, 0cch, 0fch, 0cch, 0cch, 0cch, 000h, 078h, 030h, 030h, 030h, 030h, 030h, 078h, 000h
    db  01eh, 00ch, 00ch, 00ch, 0cch, 0cch, 078h, 000h, 0e6h, 066h, 06ch, 078h, 06ch, 066h, 0e6h, 000h
    db  0f0h, 060h, 060h, 060h, 062h, 066h, 0feh, 000h, 0c6h, 0eeh, 0feh, 0feh, 0d6h, 0c6h, 0c6h, 000h
    db  0c6h, 0e6h, 0f6h, 0deh, 0ceh, 0c6h, 0c6h, 000h, 038h, 06ch, 0c6h, 0c6h, 0c6h, 06ch, 038h, 000h
    db  0fch, 066h, 066h, 07ch, 060h, 060h, 0f0h, 000h, 078h, 0cch, 0cch, 0cch, 0dch, 078h, 01ch, 000h
    db  0fch, 066h, 066h, 07ch, 06ch, 066h, 0e6h, 000h, 078h, 0cch, 0e0h, 070h, 01ch, 0cch, 078h, 000h
    db  0fch, 0b4h, 030h, 030h, 030h, 030h, 078h, 000h, 0cch, 0cch, 0cch, 0cch, 0cch, 0cch, 0fch, 000h
    db  0cch, 0cch, 0cch, 0cch, 0cch, 078h, 030h, 000h, 0c6h, 0c6h, 0c6h, 0d6h, 0feh, 0eeh, 0c6h, 000h
    db  0c6h, 0c6h, 06ch, 038h, 038h, 06ch, 0c6h, 000h, 0cch, 0cch, 0cch, 078h, 030h, 030h, 078h, 000h
    db  0feh, 0c6h, 08ch, 018h, 032h, 066h, 0feh, 000h, 078h, 060h, 060h, 060h, 060h, 060h, 078h, 000h
    db  0c0h, 060h, 030h, 018h, 00ch, 006h, 002h, 000h, 078h, 018h, 018h, 018h, 018h, 018h, 078h, 000h
    db  010h, 038h, 06ch, 0c6h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0ffh
    db  030h, 030h, 018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 078h, 00ch, 07ch, 0cch, 076h, 000h
    db  0e0h, 060h, 060h, 07ch, 066h, 066h, 0dch, 000h, 000h, 000h, 078h, 0cch, 0c0h, 0cch, 078h, 000h
    db  01ch, 00ch, 00ch, 07ch, 0cch, 0cch, 076h, 000h, 000h, 000h, 078h, 0cch, 0fch, 0c0h, 078h, 000h
    db  038h, 06ch, 060h, 0f0h, 060h, 060h, 0f0h, 000h, 000h, 000h, 076h, 0cch, 0cch, 07ch, 00ch, 0f8h
    db  0e0h, 060h, 06ch, 076h, 066h, 066h, 0e6h, 000h, 030h, 000h, 070h, 030h, 030h, 030h, 078h, 000h
    db  00ch, 000h, 00ch, 00ch, 00ch, 0cch, 0cch, 078h, 0e0h, 060h, 066h, 06ch, 078h, 06ch, 0e6h, 000h
    db  070h, 030h, 030h, 030h, 030h, 030h, 078h, 000h, 000h, 000h, 0cch, 0feh, 0feh, 0d6h, 0c6h, 000h
    db  000h, 000h, 0f8h, 0cch, 0cch, 0cch, 0cch, 000h, 000h, 000h, 078h, 0cch, 0cch, 0cch, 078h, 000h
    db  000h, 000h, 0dch, 066h, 066h, 07ch, 060h, 0f0h, 000h, 000h, 076h, 0cch, 0cch, 07ch, 00ch, 01eh
    db  000h, 000h, 0dch, 076h, 066h, 060h, 0f0h, 000h, 000h, 000h, 07ch, 0c0h, 078h, 00ch, 0f8h, 000h
    db  010h, 030h, 07ch, 030h, 030h, 034h, 018h, 000h, 000h, 000h, 0cch, 0cch, 0cch, 0cch, 076h, 000h
    db  000h, 000h, 0cch, 0cch, 0cch, 078h, 030h, 000h, 000h, 000h, 0c6h, 0d6h, 0feh, 0feh, 06ch, 000h
    db  000h, 000h, 0c6h, 06ch, 038h, 06ch, 0c6h, 000h, 000h, 000h, 0cch, 0cch, 0cch, 07ch, 00ch, 0f8h
    db  000h, 000h, 0fch, 098h, 030h, 064h, 0fch, 000h, 01ch, 030h, 030h, 0e0h, 030h, 030h, 01ch, 000h
    db  018h, 018h, 018h, 000h, 018h, 018h, 018h, 000h, 0e0h, 030h, 030h, 01ch, 030h, 030h, 0e0h, 000h
    db  076h, 0dch, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 010h, 038h, 06ch, 0c6h, 0c6h, 0feh, 000h
    db  078h, 0cch, 0c0h, 0cch, 078h, 018h, 00ch, 078h, 000h, 0cch, 000h, 0cch, 0cch, 0cch, 07eh, 000h
    db  01ch, 000h, 078h, 0cch, 0fch, 0c0h, 078h, 000h, 07eh, 0c3h, 03ch, 006h, 03eh, 066h, 03fh, 000h
    db  0cch, 000h, 078h, 00ch, 07ch, 0cch, 07eh, 000h, 0e0h, 000h, 078h, 00ch, 07ch, 0cch, 07eh, 000h
    db  030h, 030h, 078h, 00ch, 07ch, 0cch, 07eh, 000h, 000h, 000h, 078h, 0c0h, 0c0h, 078h, 00ch, 038h
    db  07eh, 0c3h, 03ch, 066h, 07eh, 060h, 03ch, 000h, 0cch, 000h, 078h, 0cch, 0fch, 0c0h, 078h, 000h
    db  0e0h, 000h, 078h, 0cch, 0fch, 0c0h, 078h, 000h, 0cch, 000h, 070h, 030h, 030h, 030h, 078h, 000h
    db  07ch, 0c6h, 038h, 018h, 018h, 018h, 03ch, 000h, 0e0h, 000h, 070h, 030h, 030h, 030h, 078h, 000h
    db  0c6h, 038h, 06ch, 0c6h, 0feh, 0c6h, 0c6h, 000h, 030h, 030h, 000h, 078h, 0cch, 0fch, 0cch, 000h
    db  01ch, 000h, 0fch, 060h, 078h, 060h, 0fch, 000h, 000h, 000h, 07fh, 00ch, 07fh, 0cch, 07fh, 000h
    db  03eh, 06ch, 0cch, 0feh, 0cch, 0cch, 0ceh, 000h, 078h, 0cch, 000h, 078h, 0cch, 0cch, 078h, 000h
    db  000h, 0cch, 000h, 078h, 0cch, 0cch, 078h, 000h, 000h, 0e0h, 000h, 078h, 0cch, 0cch, 078h, 000h
    db  078h, 0cch, 000h, 0cch, 0cch, 0cch, 07eh, 000h, 000h, 0e0h, 000h, 0cch, 0cch, 0cch, 07eh, 000h
    db  000h, 0cch, 000h, 0cch, 0cch, 07ch, 00ch, 0f8h, 0c3h, 018h, 03ch, 066h, 066h, 03ch, 018h, 000h
    db  0cch, 000h, 0cch, 0cch, 0cch, 0cch, 078h, 000h, 018h, 018h, 07eh, 0c0h, 0c0h, 07eh, 018h, 018h
    db  038h, 06ch, 064h, 0f0h, 060h, 0e6h, 0fch, 000h, 0cch, 0cch, 078h, 0fch, 030h, 0fch, 030h, 030h
    db  0f8h, 0cch, 0cch, 0fah, 0c6h, 0cfh, 0c6h, 0c7h, 00eh, 01bh, 018h, 03ch, 018h, 018h, 0d8h, 070h
    db  01ch, 000h, 078h, 00ch, 07ch, 0cch, 07eh, 000h, 038h, 000h, 070h, 030h, 030h, 030h, 078h, 000h
    db  000h, 01ch, 000h, 078h, 0cch, 0cch, 078h, 000h, 000h, 01ch, 000h, 0cch, 0cch, 0cch, 07eh, 000h
    db  000h, 0f8h, 000h, 0f8h, 0cch, 0cch, 0cch, 000h, 0fch, 000h, 0cch, 0ech, 0fch, 0dch, 0cch, 000h
    db  03ch, 06ch, 06ch, 03eh, 000h, 07eh, 000h, 000h, 038h, 06ch, 06ch, 038h, 000h, 07ch, 000h, 000h
    db  030h, 000h, 030h, 060h, 0c0h, 0cch, 078h, 000h, 000h, 000h, 000h, 0fch, 0c0h, 0c0h, 000h, 000h
    db  000h, 000h, 000h, 0fch, 00ch, 00ch, 000h, 000h, 0c3h, 0c6h, 0cch, 0deh, 033h, 066h, 0cch, 00fh
    db  0c3h, 0c6h, 0cch, 0dbh, 037h, 06fh, 0cfh, 003h, 018h, 018h, 000h, 018h, 018h, 018h, 018h, 000h
    db  000h, 033h, 066h, 0cch, 066h, 033h, 000h, 000h, 000h, 0cch, 066h, 033h, 066h, 0cch, 000h, 000h
    db  022h, 088h, 022h, 088h, 022h, 088h, 022h, 088h, 055h, 0aah, 055h, 0aah, 055h, 0aah, 055h, 0aah
    db  0dbh, 077h, 0dbh, 0eeh, 0dbh, 077h, 0dbh, 0eeh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  018h, 018h, 018h, 018h, 0f8h, 018h, 018h, 018h, 018h, 018h, 0f8h, 018h, 0f8h, 018h, 018h, 018h
    db  036h, 036h, 036h, 036h, 0f6h, 036h, 036h, 036h, 000h, 000h, 000h, 000h, 0feh, 036h, 036h, 036h
    db  000h, 000h, 0f8h, 018h, 0f8h, 018h, 018h, 018h, 036h, 036h, 0f6h, 006h, 0f6h, 036h, 036h, 036h
    db  036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 000h, 000h, 0feh, 006h, 0f6h, 036h, 036h, 036h
    db  036h, 036h, 0f6h, 006h, 0feh, 000h, 000h, 000h, 036h, 036h, 036h, 036h, 0feh, 000h, 000h, 000h
    db  018h, 018h, 0f8h, 018h, 0f8h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0f8h, 018h, 018h, 018h
    db  018h, 018h, 018h, 018h, 01fh, 000h, 000h, 000h, 018h, 018h, 018h, 018h, 0ffh, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 0ffh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 01fh, 018h, 018h, 018h
    db  000h, 000h, 000h, 000h, 0ffh, 000h, 000h, 000h, 018h, 018h, 018h, 018h, 0ffh, 018h, 018h, 018h
    db  018h, 018h, 01fh, 018h, 01fh, 018h, 018h, 018h, 036h, 036h, 036h, 036h, 037h, 036h, 036h, 036h
    db  036h, 036h, 037h, 030h, 03fh, 000h, 000h, 000h, 000h, 000h, 03fh, 030h, 037h, 036h, 036h, 036h
    db  036h, 036h, 0f7h, 000h, 0ffh, 000h, 000h, 000h, 000h, 000h, 0ffh, 000h, 0f7h, 036h, 036h, 036h
    db  036h, 036h, 037h, 030h, 037h, 036h, 036h, 036h, 000h, 000h, 0ffh, 000h, 0ffh, 000h, 000h, 000h
    db  036h, 036h, 0f7h, 000h, 0f7h, 036h, 036h, 036h, 018h, 018h, 0ffh, 000h, 0ffh, 000h, 000h, 000h
    db  036h, 036h, 036h, 036h, 0ffh, 000h, 000h, 000h, 000h, 000h, 0ffh, 000h, 0ffh, 018h, 018h, 018h
    db  000h, 000h, 000h, 000h, 0ffh, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 03fh, 000h, 000h, 000h
    db  018h, 018h, 01fh, 018h, 01fh, 000h, 000h, 000h, 000h, 000h, 01fh, 018h, 01fh, 018h, 018h, 018h
    db  000h, 000h, 000h, 000h, 03fh, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 0ffh, 036h, 036h, 036h
    db  018h, 018h, 0ffh, 018h, 0ffh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 0f8h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 01fh, 018h, 018h, 018h, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh
    db  000h, 000h, 000h, 000h, 0ffh, 0ffh, 0ffh, 0ffh, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h
    db  00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 0ffh, 0ffh, 0ffh, 0ffh, 000h, 000h, 000h, 000h
    db  000h, 000h, 076h, 0dch, 0c8h, 0dch, 076h, 000h, 000h, 078h, 0cch, 0f8h, 0cch, 0f8h, 0c0h, 0c0h
    db  000h, 0fch, 0cch, 0c0h, 0c0h, 0c0h, 0c0h, 000h, 000h, 0feh, 06ch, 06ch, 06ch, 06ch, 06ch, 000h
    db  0fch, 0cch, 060h, 030h, 060h, 0cch, 0fch, 000h, 000h, 000h, 07eh, 0d8h, 0d8h, 0d8h, 070h, 000h
    db  000h, 066h, 066h, 066h, 066h, 07ch, 060h, 0c0h, 000h, 076h, 0dch, 018h, 018h, 018h, 018h, 000h
    db  0fch, 030h, 078h, 0cch, 0cch, 078h, 030h, 0fch, 038h, 06ch, 0c6h, 0feh, 0c6h, 06ch, 038h, 000h
    db  038h, 06ch, 0c6h, 0c6h, 06ch, 06ch, 0eeh, 000h, 01ch, 030h, 018h, 07ch, 0cch, 0cch, 078h, 000h
    db  000h, 000h, 07eh, 0dbh, 0dbh, 07eh, 000h, 000h, 006h, 00ch, 07eh, 0dbh, 0dbh, 07eh, 060h, 0c0h
    db  038h, 060h, 0c0h, 0f8h, 0c0h, 060h, 038h, 000h, 078h, 0cch, 0cch, 0cch, 0cch, 0cch, 0cch, 000h
    db  000h, 0fch, 000h, 0fch, 000h, 0fch, 000h, 000h, 030h, 030h, 0fch, 030h, 030h, 000h, 0fch, 000h
    db  060h, 030h, 018h, 030h, 060h, 000h, 0fch, 000h, 018h, 030h, 060h, 030h, 018h, 000h, 0fch, 000h
    db  00eh, 01bh, 01bh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 0d8h, 0d8h, 070h
    db  030h, 030h, 000h, 0fch, 000h, 030h, 030h, 000h, 000h, 076h, 0dch, 000h, 076h, 0dch, 000h, 000h
    db  038h, 06ch, 06ch, 038h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 018h, 018h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 018h, 000h, 000h, 000h, 00fh, 00ch, 00ch, 00ch, 0ech, 06ch, 03ch, 01ch
    db  078h, 06ch, 06ch, 06ch, 06ch, 000h, 000h, 000h, 070h, 018h, 030h, 060h, 078h, 000h, 000h, 000h
    db  000h, 000h, 03ch, 03ch, 03ch, 03ch, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
_vgafont14:                                  ; 0xc5bec LB 0xe00
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  07eh, 081h, 0a5h, 081h, 081h, 0bdh, 099h, 081h, 07eh, 000h, 000h, 000h, 000h, 000h, 07eh, 0ffh
    db  0dbh, 0ffh, 0ffh, 0c3h, 0e7h, 0ffh, 07eh, 000h, 000h, 000h, 000h, 000h, 000h, 06ch, 0feh, 0feh
    db  0feh, 0feh, 07ch, 038h, 010h, 000h, 000h, 000h, 000h, 000h, 000h, 010h, 038h, 07ch, 0feh, 07ch
    db  038h, 010h, 000h, 000h, 000h, 000h, 000h, 000h, 018h, 03ch, 03ch, 0e7h, 0e7h, 0e7h, 018h, 018h
    db  03ch, 000h, 000h, 000h, 000h, 000h, 018h, 03ch, 07eh, 0ffh, 0ffh, 07eh, 018h, 018h, 03ch, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 018h, 03ch, 03ch, 018h, 000h, 000h, 000h, 000h, 000h
    db  0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0e7h, 0c3h, 0c3h, 0e7h, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 000h, 000h
    db  000h, 000h, 03ch, 066h, 042h, 042h, 066h, 03ch, 000h, 000h, 000h, 000h, 0ffh, 0ffh, 0ffh, 0ffh
    db  0c3h, 099h, 0bdh, 0bdh, 099h, 0c3h, 0ffh, 0ffh, 0ffh, 0ffh, 000h, 000h, 01eh, 00eh, 01ah, 032h
    db  078h, 0cch, 0cch, 0cch, 078h, 000h, 000h, 000h, 000h, 000h, 03ch, 066h, 066h, 066h, 03ch, 018h
    db  07eh, 018h, 018h, 000h, 000h, 000h, 000h, 000h, 03fh, 033h, 03fh, 030h, 030h, 030h, 070h, 0f0h
    db  0e0h, 000h, 000h, 000h, 000h, 000h, 07fh, 063h, 07fh, 063h, 063h, 063h, 067h, 0e7h, 0e6h, 0c0h
    db  000h, 000h, 000h, 000h, 018h, 018h, 0dbh, 03ch, 0e7h, 03ch, 0dbh, 018h, 018h, 000h, 000h, 000h
    db  000h, 000h, 080h, 0c0h, 0e0h, 0f8h, 0feh, 0f8h, 0e0h, 0c0h, 080h, 000h, 000h, 000h, 000h, 000h
    db  002h, 006h, 00eh, 03eh, 0feh, 03eh, 00eh, 006h, 002h, 000h, 000h, 000h, 000h, 000h, 018h, 03ch
    db  07eh, 018h, 018h, 018h, 07eh, 03ch, 018h, 000h, 000h, 000h, 000h, 000h, 066h, 066h, 066h, 066h
    db  066h, 066h, 000h, 066h, 066h, 000h, 000h, 000h, 000h, 000h, 07fh, 0dbh, 0dbh, 0dbh, 07bh, 01bh
    db  01bh, 01bh, 01bh, 000h, 000h, 000h, 000h, 07ch, 0c6h, 060h, 038h, 06ch, 0c6h, 0c6h, 06ch, 038h
    db  00ch, 0c6h, 07ch, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0feh, 0feh, 0feh, 000h
    db  000h, 000h, 000h, 000h, 018h, 03ch, 07eh, 018h, 018h, 018h, 07eh, 03ch, 018h, 07eh, 000h, 000h
    db  000h, 000h, 018h, 03ch, 07eh, 018h, 018h, 018h, 018h, 018h, 018h, 000h, 000h, 000h, 000h, 000h
    db  018h, 018h, 018h, 018h, 018h, 018h, 07eh, 03ch, 018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  018h, 00ch, 0feh, 00ch, 018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 030h, 060h
    db  0feh, 060h, 030h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0c0h, 0c0h, 0c0h
    db  0feh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 028h, 06ch, 0feh, 06ch, 028h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 010h, 038h, 038h, 07ch, 07ch, 0feh, 0feh, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0feh, 0feh, 07ch, 07ch, 038h, 038h, 010h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  018h, 03ch, 03ch, 03ch, 018h, 018h, 000h, 018h, 018h, 000h, 000h, 000h, 000h, 066h, 066h, 066h
    db  024h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 06ch, 06ch, 0feh, 06ch
    db  06ch, 06ch, 0feh, 06ch, 06ch, 000h, 000h, 000h, 018h, 018h, 07ch, 0c6h, 0c2h, 0c0h, 07ch, 006h
    db  086h, 0c6h, 07ch, 018h, 018h, 000h, 000h, 000h, 000h, 000h, 0c2h, 0c6h, 00ch, 018h, 030h, 066h
    db  0c6h, 000h, 000h, 000h, 000h, 000h, 038h, 06ch, 06ch, 038h, 076h, 0dch, 0cch, 0cch, 076h, 000h
    db  000h, 000h, 000h, 030h, 030h, 030h, 060h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 00ch, 018h, 030h, 030h, 030h, 030h, 030h, 018h, 00ch, 000h, 000h, 000h, 000h, 000h
    db  030h, 018h, 00ch, 00ch, 00ch, 00ch, 00ch, 018h, 030h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  066h, 03ch, 0ffh, 03ch, 066h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 018h, 018h
    db  07eh, 018h, 018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  018h, 018h, 018h, 030h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0feh, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 018h, 018h, 000h
    db  000h, 000h, 000h, 000h, 002h, 006h, 00ch, 018h, 030h, 060h, 0c0h, 080h, 000h, 000h, 000h, 000h
    db  000h, 000h, 07ch, 0c6h, 0ceh, 0deh, 0f6h, 0e6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h, 000h
    db  018h, 038h, 078h, 018h, 018h, 018h, 018h, 018h, 07eh, 000h, 000h, 000h, 000h, 000h, 07ch, 0c6h
    db  006h, 00ch, 018h, 030h, 060h, 0c6h, 0feh, 000h, 000h, 000h, 000h, 000h, 07ch, 0c6h, 006h, 006h
    db  03ch, 006h, 006h, 0c6h, 07ch, 000h, 000h, 000h, 000h, 000h, 00ch, 01ch, 03ch, 06ch, 0cch, 0feh
    db  00ch, 00ch, 01eh, 000h, 000h, 000h, 000h, 000h, 0feh, 0c0h, 0c0h, 0c0h, 0fch, 006h, 006h, 0c6h
    db  07ch, 000h, 000h, 000h, 000h, 000h, 038h, 060h, 0c0h, 0c0h, 0fch, 0c6h, 0c6h, 0c6h, 07ch, 000h
    db  000h, 000h, 000h, 000h, 0feh, 0c6h, 006h, 00ch, 018h, 030h, 030h, 030h, 030h, 000h, 000h, 000h
    db  000h, 000h, 07ch, 0c6h, 0c6h, 0c6h, 07ch, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h, 000h
    db  07ch, 0c6h, 0c6h, 0c6h, 07eh, 006h, 006h, 00ch, 078h, 000h, 000h, 000h, 000h, 000h, 000h, 018h
    db  018h, 000h, 000h, 000h, 018h, 018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 018h, 018h, 000h
    db  000h, 000h, 018h, 018h, 030h, 000h, 000h, 000h, 000h, 000h, 006h, 00ch, 018h, 030h, 060h, 030h
    db  018h, 00ch, 006h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 07eh, 000h, 000h, 07eh, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 060h, 030h, 018h, 00ch, 006h, 00ch, 018h, 030h, 060h, 000h
    db  000h, 000h, 000h, 000h, 07ch, 0c6h, 0c6h, 00ch, 018h, 018h, 000h, 018h, 018h, 000h, 000h, 000h
    db  000h, 000h, 07ch, 0c6h, 0c6h, 0deh, 0deh, 0deh, 0dch, 0c0h, 07ch, 000h, 000h, 000h, 000h, 000h
    db  010h, 038h, 06ch, 0c6h, 0c6h, 0feh, 0c6h, 0c6h, 0c6h, 000h, 000h, 000h, 000h, 000h, 0fch, 066h
    db  066h, 066h, 07ch, 066h, 066h, 066h, 0fch, 000h, 000h, 000h, 000h, 000h, 03ch, 066h, 0c2h, 0c0h
    db  0c0h, 0c0h, 0c2h, 066h, 03ch, 000h, 000h, 000h, 000h, 000h, 0f8h, 06ch, 066h, 066h, 066h, 066h
    db  066h, 06ch, 0f8h, 000h, 000h, 000h, 000h, 000h, 0feh, 066h, 062h, 068h, 078h, 068h, 062h, 066h
    db  0feh, 000h, 000h, 000h, 000h, 000h, 0feh, 066h, 062h, 068h, 078h, 068h, 060h, 060h, 0f0h, 000h
    db  000h, 000h, 000h, 000h, 03ch, 066h, 0c2h, 0c0h, 0c0h, 0deh, 0c6h, 066h, 03ah, 000h, 000h, 000h
    db  000h, 000h, 0c6h, 0c6h, 0c6h, 0c6h, 0feh, 0c6h, 0c6h, 0c6h, 0c6h, 000h, 000h, 000h, 000h, 000h
    db  03ch, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h, 000h, 01eh, 00ch
    db  00ch, 00ch, 00ch, 00ch, 0cch, 0cch, 078h, 000h, 000h, 000h, 000h, 000h, 0e6h, 066h, 06ch, 06ch
    db  078h, 06ch, 06ch, 066h, 0e6h, 000h, 000h, 000h, 000h, 000h, 0f0h, 060h, 060h, 060h, 060h, 060h
    db  062h, 066h, 0feh, 000h, 000h, 000h, 000h, 000h, 0c6h, 0eeh, 0feh, 0feh, 0d6h, 0c6h, 0c6h, 0c6h
    db  0c6h, 000h, 000h, 000h, 000h, 000h, 0c6h, 0e6h, 0f6h, 0feh, 0deh, 0ceh, 0c6h, 0c6h, 0c6h, 000h
    db  000h, 000h, 000h, 000h, 038h, 06ch, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 06ch, 038h, 000h, 000h, 000h
    db  000h, 000h, 0fch, 066h, 066h, 066h, 07ch, 060h, 060h, 060h, 0f0h, 000h, 000h, 000h, 000h, 000h
    db  07ch, 0c6h, 0c6h, 0c6h, 0c6h, 0d6h, 0deh, 07ch, 00ch, 00eh, 000h, 000h, 000h, 000h, 0fch, 066h
    db  066h, 066h, 07ch, 06ch, 066h, 066h, 0e6h, 000h, 000h, 000h, 000h, 000h, 07ch, 0c6h, 0c6h, 060h
    db  038h, 00ch, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h, 000h, 07eh, 07eh, 05ah, 018h, 018h, 018h
    db  018h, 018h, 03ch, 000h, 000h, 000h, 000h, 000h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h
    db  07ch, 000h, 000h, 000h, 000h, 000h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 06ch, 038h, 010h, 000h
    db  000h, 000h, 000h, 000h, 0c6h, 0c6h, 0c6h, 0c6h, 0d6h, 0d6h, 0feh, 07ch, 06ch, 000h, 000h, 000h
    db  000h, 000h, 0c6h, 0c6h, 06ch, 038h, 038h, 038h, 06ch, 0c6h, 0c6h, 000h, 000h, 000h, 000h, 000h
    db  066h, 066h, 066h, 066h, 03ch, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h, 000h, 0feh, 0c6h
    db  08ch, 018h, 030h, 060h, 0c2h, 0c6h, 0feh, 000h, 000h, 000h, 000h, 000h, 03ch, 030h, 030h, 030h
    db  030h, 030h, 030h, 030h, 03ch, 000h, 000h, 000h, 000h, 000h, 080h, 0c0h, 0e0h, 070h, 038h, 01ch
    db  00eh, 006h, 002h, 000h, 000h, 000h, 000h, 000h, 03ch, 00ch, 00ch, 00ch, 00ch, 00ch, 00ch, 00ch
    db  03ch, 000h, 000h, 000h, 010h, 038h, 06ch, 0c6h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0ffh, 000h
    db  030h, 030h, 018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 078h, 00ch, 07ch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h, 000h, 0e0h, 060h
    db  060h, 078h, 06ch, 066h, 066h, 066h, 07ch, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 07ch
    db  0c6h, 0c0h, 0c0h, 0c6h, 07ch, 000h, 000h, 000h, 000h, 000h, 01ch, 00ch, 00ch, 03ch, 06ch, 0cch
    db  0cch, 0cch, 076h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 07ch, 0c6h, 0feh, 0c0h, 0c6h
    db  07ch, 000h, 000h, 000h, 000h, 000h, 038h, 06ch, 064h, 060h, 0f0h, 060h, 060h, 060h, 0f0h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 076h, 0cch, 0cch, 0cch, 07ch, 00ch, 0cch, 078h, 000h
    db  000h, 000h, 0e0h, 060h, 060h, 06ch, 076h, 066h, 066h, 066h, 0e6h, 000h, 000h, 000h, 000h, 000h
    db  018h, 018h, 000h, 038h, 018h, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h, 000h, 006h, 006h
    db  000h, 00eh, 006h, 006h, 006h, 006h, 066h, 066h, 03ch, 000h, 000h, 000h, 0e0h, 060h, 060h, 066h
    db  06ch, 078h, 06ch, 066h, 0e6h, 000h, 000h, 000h, 000h, 000h, 038h, 018h, 018h, 018h, 018h, 018h
    db  018h, 018h, 03ch, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0ech, 0feh, 0d6h, 0d6h, 0d6h
    db  0c6h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0dch, 066h, 066h, 066h, 066h, 066h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 07ch, 0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0dch, 066h, 066h, 066h, 07ch, 060h, 060h, 0f0h, 000h, 000h, 000h
    db  000h, 000h, 000h, 076h, 0cch, 0cch, 0cch, 07ch, 00ch, 00ch, 01eh, 000h, 000h, 000h, 000h, 000h
    db  000h, 0dch, 076h, 066h, 060h, 060h, 0f0h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 07ch
    db  0c6h, 070h, 01ch, 0c6h, 07ch, 000h, 000h, 000h, 000h, 000h, 010h, 030h, 030h, 0fch, 030h, 030h
    db  030h, 036h, 01ch, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0cch, 0cch, 0cch, 0cch, 0cch
    db  076h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 066h, 066h, 066h, 066h, 03ch, 018h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 0c6h, 0c6h, 0d6h, 0d6h, 0feh, 06ch, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0c6h, 06ch, 038h, 038h, 06ch, 0c6h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 0c6h, 0c6h, 0c6h, 0c6h, 07eh, 006h, 00ch, 0f8h, 000h, 000h, 000h, 000h, 000h
    db  000h, 0feh, 0cch, 018h, 030h, 066h, 0feh, 000h, 000h, 000h, 000h, 000h, 00eh, 018h, 018h, 018h
    db  070h, 018h, 018h, 018h, 00eh, 000h, 000h, 000h, 000h, 000h, 018h, 018h, 018h, 018h, 000h, 018h
    db  018h, 018h, 018h, 000h, 000h, 000h, 000h, 000h, 070h, 018h, 018h, 018h, 00eh, 018h, 018h, 018h
    db  070h, 000h, 000h, 000h, 000h, 000h, 076h, 0dch, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 010h, 038h, 06ch, 0c6h, 0c6h, 0feh, 000h, 000h, 000h, 000h
    db  000h, 000h, 03ch, 066h, 0c2h, 0c0h, 0c0h, 0c2h, 066h, 03ch, 00ch, 006h, 07ch, 000h, 000h, 000h
    db  0cch, 0cch, 000h, 0cch, 0cch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h, 00ch, 018h, 030h
    db  000h, 07ch, 0c6h, 0feh, 0c0h, 0c6h, 07ch, 000h, 000h, 000h, 000h, 010h, 038h, 06ch, 000h, 078h
    db  00ch, 07ch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h, 000h, 0cch, 0cch, 000h, 078h, 00ch, 07ch
    db  0cch, 0cch, 076h, 000h, 000h, 000h, 000h, 060h, 030h, 018h, 000h, 078h, 00ch, 07ch, 0cch, 0cch
    db  076h, 000h, 000h, 000h, 000h, 038h, 06ch, 038h, 000h, 078h, 00ch, 07ch, 0cch, 0cch, 076h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 03ch, 066h, 060h, 066h, 03ch, 00ch, 006h, 03ch, 000h, 000h
    db  000h, 010h, 038h, 06ch, 000h, 07ch, 0c6h, 0feh, 0c0h, 0c6h, 07ch, 000h, 000h, 000h, 000h, 000h
    db  0cch, 0cch, 000h, 07ch, 0c6h, 0feh, 0c0h, 0c6h, 07ch, 000h, 000h, 000h, 000h, 060h, 030h, 018h
    db  000h, 07ch, 0c6h, 0feh, 0c0h, 0c6h, 07ch, 000h, 000h, 000h, 000h, 000h, 066h, 066h, 000h, 038h
    db  018h, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h, 018h, 03ch, 066h, 000h, 038h, 018h, 018h
    db  018h, 018h, 03ch, 000h, 000h, 000h, 000h, 060h, 030h, 018h, 000h, 038h, 018h, 018h, 018h, 018h
    db  03ch, 000h, 000h, 000h, 000h, 0c6h, 0c6h, 010h, 038h, 06ch, 0c6h, 0c6h, 0feh, 0c6h, 0c6h, 000h
    db  000h, 000h, 038h, 06ch, 038h, 000h, 038h, 06ch, 0c6h, 0c6h, 0feh, 0c6h, 0c6h, 000h, 000h, 000h
    db  018h, 030h, 060h, 000h, 0feh, 066h, 060h, 07ch, 060h, 066h, 0feh, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 0cch, 076h, 036h, 07eh, 0d8h, 0d8h, 06eh, 000h, 000h, 000h, 000h, 000h, 03eh, 06ch
    db  0cch, 0cch, 0feh, 0cch, 0cch, 0cch, 0ceh, 000h, 000h, 000h, 000h, 010h, 038h, 06ch, 000h, 07ch
    db  0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h, 000h, 0c6h, 0c6h, 000h, 07ch, 0c6h, 0c6h
    db  0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h, 060h, 030h, 018h, 000h, 07ch, 0c6h, 0c6h, 0c6h, 0c6h
    db  07ch, 000h, 000h, 000h, 000h, 030h, 078h, 0cch, 000h, 0cch, 0cch, 0cch, 0cch, 0cch, 076h, 000h
    db  000h, 000h, 000h, 060h, 030h, 018h, 000h, 0cch, 0cch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h
    db  000h, 000h, 0c6h, 0c6h, 000h, 0c6h, 0c6h, 0c6h, 0c6h, 07eh, 006h, 00ch, 078h, 000h, 000h, 0c6h
    db  0c6h, 038h, 06ch, 0c6h, 0c6h, 0c6h, 0c6h, 06ch, 038h, 000h, 000h, 000h, 000h, 0c6h, 0c6h, 000h
    db  0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h, 018h, 018h, 03ch, 066h, 060h
    db  060h, 066h, 03ch, 018h, 018h, 000h, 000h, 000h, 000h, 038h, 06ch, 064h, 060h, 0f0h, 060h, 060h
    db  060h, 0e6h, 0fch, 000h, 000h, 000h, 000h, 000h, 066h, 066h, 03ch, 018h, 07eh, 018h, 07eh, 018h
    db  018h, 000h, 000h, 000h, 000h, 0f8h, 0cch, 0cch, 0f8h, 0c4h, 0cch, 0deh, 0cch, 0cch, 0c6h, 000h
    db  000h, 000h, 000h, 00eh, 01bh, 018h, 018h, 018h, 07eh, 018h, 018h, 018h, 018h, 0d8h, 070h, 000h
    db  000h, 018h, 030h, 060h, 000h, 078h, 00ch, 07ch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h, 00ch
    db  018h, 030h, 000h, 038h, 018h, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h, 018h, 030h, 060h
    db  000h, 07ch, 0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h, 018h, 030h, 060h, 000h, 0cch
    db  0cch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h, 000h, 076h, 0dch, 000h, 0dch, 066h, 066h
    db  066h, 066h, 066h, 000h, 000h, 000h, 076h, 0dch, 000h, 0c6h, 0e6h, 0f6h, 0feh, 0deh, 0ceh, 0c6h
    db  0c6h, 000h, 000h, 000h, 000h, 03ch, 06ch, 06ch, 03eh, 000h, 07eh, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 038h, 06ch, 06ch, 038h, 000h, 07ch, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 030h, 030h, 000h, 030h, 030h, 060h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 0feh, 0c0h, 0c0h, 0c0h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 0feh, 006h, 006h, 006h, 000h, 000h, 000h, 000h, 000h, 0c0h, 0c0h, 0c6h, 0cch, 0d8h
    db  030h, 060h, 0dch, 086h, 00ch, 018h, 03eh, 000h, 000h, 0c0h, 0c0h, 0c6h, 0cch, 0d8h, 030h, 066h
    db  0ceh, 09eh, 03eh, 006h, 006h, 000h, 000h, 000h, 018h, 018h, 000h, 018h, 018h, 03ch, 03ch, 03ch
    db  018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 036h, 06ch, 0d8h, 06ch, 036h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 0d8h, 06ch, 036h, 06ch, 0d8h, 000h, 000h, 000h, 000h, 000h
    db  011h, 044h, 011h, 044h, 011h, 044h, 011h, 044h, 011h, 044h, 011h, 044h, 011h, 044h, 055h, 0aah
    db  055h, 0aah, 055h, 0aah, 055h, 0aah, 055h, 0aah, 055h, 0aah, 055h, 0aah, 0ddh, 077h, 0ddh, 077h
    db  0ddh, 077h, 0ddh, 077h, 0ddh, 077h, 0ddh, 077h, 0ddh, 077h, 018h, 018h, 018h, 018h, 018h, 018h
    db  018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 0f8h
    db  018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 0f8h, 018h, 0f8h, 018h, 018h
    db  018h, 018h, 018h, 018h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 0f6h, 036h, 036h, 036h, 036h
    db  036h, 036h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0feh, 036h, 036h, 036h, 036h, 036h, 036h
    db  000h, 000h, 000h, 000h, 000h, 0f8h, 018h, 0f8h, 018h, 018h, 018h, 018h, 018h, 018h, 036h, 036h
    db  036h, 036h, 036h, 0f6h, 006h, 0f6h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 000h, 000h, 000h, 000h, 000h, 0feh
    db  006h, 0f6h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 0f6h, 006h, 0feh
    db  000h, 000h, 000h, 000h, 000h, 000h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 0feh, 000h, 000h
    db  000h, 000h, 000h, 000h, 018h, 018h, 018h, 018h, 018h, 0f8h, 018h, 0f8h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0f8h, 018h, 018h, 018h, 018h, 018h, 018h
    db  018h, 018h, 018h, 018h, 018h, 018h, 018h, 01fh, 000h, 000h, 000h, 000h, 000h, 000h, 018h, 018h
    db  018h, 018h, 018h, 018h, 018h, 0ffh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 0ffh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  018h, 01fh, 018h, 018h, 018h, 018h, 018h, 018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0ffh
    db  000h, 000h, 000h, 000h, 000h, 000h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 0ffh, 018h, 018h
    db  018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 01fh, 018h, 01fh, 018h, 018h, 018h, 018h
    db  018h, 018h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 037h, 036h, 036h, 036h, 036h, 036h, 036h
    db  036h, 036h, 036h, 036h, 036h, 037h, 030h, 03fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 03fh, 030h, 037h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  036h, 0f7h, 000h, 0ffh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0ffh
    db  000h, 0f7h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 037h, 030h, 037h
    db  036h, 036h, 036h, 036h, 036h, 036h, 000h, 000h, 000h, 000h, 000h, 0ffh, 000h, 0ffh, 000h, 000h
    db  000h, 000h, 000h, 000h, 036h, 036h, 036h, 036h, 036h, 0f7h, 000h, 0f7h, 036h, 036h, 036h, 036h
    db  036h, 036h, 018h, 018h, 018h, 018h, 018h, 0ffh, 000h, 0ffh, 000h, 000h, 000h, 000h, 000h, 000h
    db  036h, 036h, 036h, 036h, 036h, 036h, 036h, 0ffh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 0ffh, 000h, 0ffh, 018h, 018h, 018h, 018h, 018h, 018h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 0ffh, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  036h, 03fh, 000h, 000h, 000h, 000h, 000h, 000h, 018h, 018h, 018h, 018h, 018h, 01fh, 018h, 01fh
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 01fh, 018h, 01fh, 018h, 018h
    db  018h, 018h, 018h, 018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 03fh, 036h, 036h, 036h, 036h
    db  036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 0ffh, 036h, 036h, 036h, 036h, 036h, 036h
    db  018h, 018h, 018h, 018h, 018h, 0ffh, 018h, 0ffh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  018h, 018h, 018h, 018h, 018h, 0f8h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 01fh, 018h, 018h, 018h, 018h, 018h, 018h, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh
    db  0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0ffh
    db  0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h
    db  0f0h, 0f0h, 0f0h, 0f0h, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh
    db  00fh, 00fh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 076h, 0dch, 0d8h, 0d8h, 0dch, 076h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 07ch, 0c6h, 0fch, 0c6h, 0c6h, 0fch, 0c0h, 0c0h, 040h, 000h, 000h, 000h, 0feh, 0c6h
    db  0c6h, 0c0h, 0c0h, 0c0h, 0c0h, 0c0h, 0c0h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0feh, 06ch
    db  06ch, 06ch, 06ch, 06ch, 06ch, 000h, 000h, 000h, 000h, 000h, 0feh, 0c6h, 060h, 030h, 018h, 030h
    db  060h, 0c6h, 0feh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 07eh, 0d8h, 0d8h, 0d8h, 0d8h
    db  070h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 066h, 066h, 066h, 066h, 07ch, 060h, 060h, 0c0h
    db  000h, 000h, 000h, 000h, 000h, 000h, 076h, 0dch, 018h, 018h, 018h, 018h, 018h, 000h, 000h, 000h
    db  000h, 000h, 07eh, 018h, 03ch, 066h, 066h, 066h, 03ch, 018h, 07eh, 000h, 000h, 000h, 000h, 000h
    db  038h, 06ch, 0c6h, 0c6h, 0feh, 0c6h, 0c6h, 06ch, 038h, 000h, 000h, 000h, 000h, 000h, 038h, 06ch
    db  0c6h, 0c6h, 0c6h, 06ch, 06ch, 06ch, 0eeh, 000h, 000h, 000h, 000h, 000h, 01eh, 030h, 018h, 00ch
    db  03eh, 066h, 066h, 066h, 03ch, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 07eh, 0dbh, 0dbh
    db  07eh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 003h, 006h, 07eh, 0dbh, 0dbh, 0f3h, 07eh, 060h
    db  0c0h, 000h, 000h, 000h, 000h, 000h, 01ch, 030h, 060h, 060h, 07ch, 060h, 060h, 030h, 01ch, 000h
    db  000h, 000h, 000h, 000h, 000h, 07ch, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 000h, 000h, 000h
    db  000h, 000h, 000h, 0feh, 000h, 000h, 0feh, 000h, 000h, 0feh, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 018h, 018h, 07eh, 018h, 018h, 000h, 000h, 0ffh, 000h, 000h, 000h, 000h, 000h, 030h, 018h
    db  00ch, 006h, 00ch, 018h, 030h, 000h, 07eh, 000h, 000h, 000h, 000h, 000h, 00ch, 018h, 030h, 060h
    db  030h, 018h, 00ch, 000h, 07eh, 000h, 000h, 000h, 000h, 000h, 00eh, 01bh, 01bh, 018h, 018h, 018h
    db  018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 0d8h, 0d8h
    db  070h, 000h, 000h, 000h, 000h, 000h, 000h, 018h, 018h, 000h, 07eh, 000h, 018h, 018h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 076h, 0dch, 000h, 076h, 0dch, 000h, 000h, 000h, 000h, 000h
    db  000h, 038h, 06ch, 06ch, 038h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 018h, 018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 00fh, 00ch, 00ch, 00ch, 00ch
    db  00ch, 0ech, 06ch, 03ch, 01ch, 000h, 000h, 000h, 000h, 0d8h, 06ch, 06ch, 06ch, 06ch, 06ch, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 070h, 0d8h, 030h, 060h, 0c8h, 0f8h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 07ch, 07ch, 07ch, 07ch, 07ch, 07ch, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
_vgafont16:                                  ; 0xc69ec LB 0x1000
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 07eh, 081h, 0a5h, 081h, 081h, 0bdh, 099h, 081h, 081h, 07eh, 000h, 000h, 000h, 000h
    db  000h, 000h, 07eh, 0ffh, 0dbh, 0ffh, 0ffh, 0c3h, 0e7h, 0ffh, 0ffh, 07eh, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 06ch, 0feh, 0feh, 0feh, 0feh, 07ch, 038h, 010h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 010h, 038h, 07ch, 0feh, 07ch, 038h, 010h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 018h, 03ch, 03ch, 0e7h, 0e7h, 0e7h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 018h, 03ch, 07eh, 0ffh, 0ffh, 07eh, 018h, 018h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 018h, 03ch, 03ch, 018h, 000h, 000h, 000h, 000h, 000h, 000h
    db  0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0e7h, 0c3h, 0c3h, 0e7h, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh
    db  000h, 000h, 000h, 000h, 000h, 03ch, 066h, 042h, 042h, 066h, 03ch, 000h, 000h, 000h, 000h, 000h
    db  0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0c3h, 099h, 0bdh, 0bdh, 099h, 0c3h, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh
    db  000h, 000h, 01eh, 00eh, 01ah, 032h, 078h, 0cch, 0cch, 0cch, 0cch, 078h, 000h, 000h, 000h, 000h
    db  000h, 000h, 03ch, 066h, 066h, 066h, 066h, 03ch, 018h, 07eh, 018h, 018h, 000h, 000h, 000h, 000h
    db  000h, 000h, 03fh, 033h, 03fh, 030h, 030h, 030h, 030h, 070h, 0f0h, 0e0h, 000h, 000h, 000h, 000h
    db  000h, 000h, 07fh, 063h, 07fh, 063h, 063h, 063h, 063h, 067h, 0e7h, 0e6h, 0c0h, 000h, 000h, 000h
    db  000h, 000h, 000h, 018h, 018h, 0dbh, 03ch, 0e7h, 03ch, 0dbh, 018h, 018h, 000h, 000h, 000h, 000h
    db  000h, 080h, 0c0h, 0e0h, 0f0h, 0f8h, 0feh, 0f8h, 0f0h, 0e0h, 0c0h, 080h, 000h, 000h, 000h, 000h
    db  000h, 002h, 006h, 00eh, 01eh, 03eh, 0feh, 03eh, 01eh, 00eh, 006h, 002h, 000h, 000h, 000h, 000h
    db  000h, 000h, 018h, 03ch, 07eh, 018h, 018h, 018h, 07eh, 03ch, 018h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 066h, 066h, 066h, 066h, 066h, 066h, 066h, 000h, 066h, 066h, 000h, 000h, 000h, 000h
    db  000h, 000h, 07fh, 0dbh, 0dbh, 0dbh, 07bh, 01bh, 01bh, 01bh, 01bh, 01bh, 000h, 000h, 000h, 000h
    db  000h, 07ch, 0c6h, 060h, 038h, 06ch, 0c6h, 0c6h, 06ch, 038h, 00ch, 0c6h, 07ch, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0feh, 0feh, 0feh, 0feh, 000h, 000h, 000h, 000h
    db  000h, 000h, 018h, 03ch, 07eh, 018h, 018h, 018h, 07eh, 03ch, 018h, 07eh, 000h, 000h, 000h, 000h
    db  000h, 000h, 018h, 03ch, 07eh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 000h, 000h, 000h, 000h
    db  000h, 000h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 07eh, 03ch, 018h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 018h, 00ch, 0feh, 00ch, 018h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 030h, 060h, 0feh, 060h, 030h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 0c0h, 0c0h, 0c0h, 0feh, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 024h, 066h, 0ffh, 066h, 024h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 010h, 038h, 038h, 07ch, 07ch, 0feh, 0feh, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 0feh, 0feh, 07ch, 07ch, 038h, 038h, 010h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 018h, 03ch, 03ch, 03ch, 018h, 018h, 018h, 000h, 018h, 018h, 000h, 000h, 000h, 000h
    db  000h, 066h, 066h, 066h, 024h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 06ch, 06ch, 0feh, 06ch, 06ch, 06ch, 0feh, 06ch, 06ch, 000h, 000h, 000h, 000h
    db  018h, 018h, 07ch, 0c6h, 0c2h, 0c0h, 07ch, 006h, 006h, 086h, 0c6h, 07ch, 018h, 018h, 000h, 000h
    db  000h, 000h, 000h, 000h, 0c2h, 0c6h, 00ch, 018h, 030h, 060h, 0c6h, 086h, 000h, 000h, 000h, 000h
    db  000h, 000h, 038h, 06ch, 06ch, 038h, 076h, 0dch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h
    db  000h, 030h, 030h, 030h, 060h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 00ch, 018h, 030h, 030h, 030h, 030h, 030h, 030h, 018h, 00ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 030h, 018h, 00ch, 00ch, 00ch, 00ch, 00ch, 00ch, 018h, 030h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 066h, 03ch, 0ffh, 03ch, 066h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 018h, 018h, 07eh, 018h, 018h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 018h, 018h, 018h, 030h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 0feh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 018h, 018h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 002h, 006h, 00ch, 018h, 030h, 060h, 0c0h, 080h, 000h, 000h, 000h, 000h
    db  000h, 000h, 03ch, 066h, 0c3h, 0c3h, 0dbh, 0dbh, 0c3h, 0c3h, 066h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 018h, 038h, 078h, 018h, 018h, 018h, 018h, 018h, 018h, 07eh, 000h, 000h, 000h, 000h
    db  000h, 000h, 07ch, 0c6h, 006h, 00ch, 018h, 030h, 060h, 0c0h, 0c6h, 0feh, 000h, 000h, 000h, 000h
    db  000h, 000h, 07ch, 0c6h, 006h, 006h, 03ch, 006h, 006h, 006h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 00ch, 01ch, 03ch, 06ch, 0cch, 0feh, 00ch, 00ch, 00ch, 01eh, 000h, 000h, 000h, 000h
    db  000h, 000h, 0feh, 0c0h, 0c0h, 0c0h, 0fch, 006h, 006h, 006h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 038h, 060h, 0c0h, 0c0h, 0fch, 0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 0feh, 0c6h, 006h, 006h, 00ch, 018h, 030h, 030h, 030h, 030h, 000h, 000h, 000h, 000h
    db  000h, 000h, 07ch, 0c6h, 0c6h, 0c6h, 07ch, 0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 07ch, 0c6h, 0c6h, 0c6h, 07eh, 006h, 006h, 006h, 00ch, 078h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 018h, 018h, 000h, 000h, 000h, 018h, 018h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 018h, 018h, 000h, 000h, 000h, 018h, 018h, 030h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 006h, 00ch, 018h, 030h, 060h, 030h, 018h, 00ch, 006h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 07eh, 000h, 000h, 07eh, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 060h, 030h, 018h, 00ch, 006h, 00ch, 018h, 030h, 060h, 000h, 000h, 000h, 000h
    db  000h, 000h, 07ch, 0c6h, 0c6h, 00ch, 018h, 018h, 018h, 000h, 018h, 018h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 07ch, 0c6h, 0c6h, 0deh, 0deh, 0deh, 0dch, 0c0h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 010h, 038h, 06ch, 0c6h, 0c6h, 0feh, 0c6h, 0c6h, 0c6h, 0c6h, 000h, 000h, 000h, 000h
    db  000h, 000h, 0fch, 066h, 066h, 066h, 07ch, 066h, 066h, 066h, 066h, 0fch, 000h, 000h, 000h, 000h
    db  000h, 000h, 03ch, 066h, 0c2h, 0c0h, 0c0h, 0c0h, 0c0h, 0c2h, 066h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 0f8h, 06ch, 066h, 066h, 066h, 066h, 066h, 066h, 06ch, 0f8h, 000h, 000h, 000h, 000h
    db  000h, 000h, 0feh, 066h, 062h, 068h, 078h, 068h, 060h, 062h, 066h, 0feh, 000h, 000h, 000h, 000h
    db  000h, 000h, 0feh, 066h, 062h, 068h, 078h, 068h, 060h, 060h, 060h, 0f0h, 000h, 000h, 000h, 000h
    db  000h, 000h, 03ch, 066h, 0c2h, 0c0h, 0c0h, 0deh, 0c6h, 0c6h, 066h, 03ah, 000h, 000h, 000h, 000h
    db  000h, 000h, 0c6h, 0c6h, 0c6h, 0c6h, 0feh, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 000h, 000h, 000h, 000h
    db  000h, 000h, 03ch, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 01eh, 00ch, 00ch, 00ch, 00ch, 00ch, 0cch, 0cch, 0cch, 078h, 000h, 000h, 000h, 000h
    db  000h, 000h, 0e6h, 066h, 066h, 06ch, 078h, 078h, 06ch, 066h, 066h, 0e6h, 000h, 000h, 000h, 000h
    db  000h, 000h, 0f0h, 060h, 060h, 060h, 060h, 060h, 060h, 062h, 066h, 0feh, 000h, 000h, 000h, 000h
    db  000h, 000h, 0c3h, 0e7h, 0ffh, 0ffh, 0dbh, 0c3h, 0c3h, 0c3h, 0c3h, 0c3h, 000h, 000h, 000h, 000h
    db  000h, 000h, 0c6h, 0e6h, 0f6h, 0feh, 0deh, 0ceh, 0c6h, 0c6h, 0c6h, 0c6h, 000h, 000h, 000h, 000h
    db  000h, 000h, 07ch, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 0fch, 066h, 066h, 066h, 07ch, 060h, 060h, 060h, 060h, 0f0h, 000h, 000h, 000h, 000h
    db  000h, 000h, 07ch, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0d6h, 0deh, 07ch, 00ch, 00eh, 000h, 000h
    db  000h, 000h, 0fch, 066h, 066h, 066h, 07ch, 06ch, 066h, 066h, 066h, 0e6h, 000h, 000h, 000h, 000h
    db  000h, 000h, 07ch, 0c6h, 0c6h, 060h, 038h, 00ch, 006h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 0ffh, 0dbh, 099h, 018h, 018h, 018h, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 0c3h, 0c3h, 0c3h, 0c3h, 0c3h, 0c3h, 0c3h, 066h, 03ch, 018h, 000h, 000h, 000h, 000h
    db  000h, 000h, 0c3h, 0c3h, 0c3h, 0c3h, 0c3h, 0dbh, 0dbh, 0ffh, 066h, 066h, 000h, 000h, 000h, 000h
    db  000h, 000h, 0c3h, 0c3h, 066h, 03ch, 018h, 018h, 03ch, 066h, 0c3h, 0c3h, 000h, 000h, 000h, 000h
    db  000h, 000h, 0c3h, 0c3h, 0c3h, 066h, 03ch, 018h, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 0ffh, 0c3h, 086h, 00ch, 018h, 030h, 060h, 0c1h, 0c3h, 0ffh, 000h, 000h, 000h, 000h
    db  000h, 000h, 03ch, 030h, 030h, 030h, 030h, 030h, 030h, 030h, 030h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 080h, 0c0h, 0e0h, 070h, 038h, 01ch, 00eh, 006h, 002h, 000h, 000h, 000h, 000h
    db  000h, 000h, 03ch, 00ch, 00ch, 00ch, 00ch, 00ch, 00ch, 00ch, 00ch, 03ch, 000h, 000h, 000h, 000h
    db  010h, 038h, 06ch, 0c6h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0ffh, 000h, 000h
    db  030h, 030h, 018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 078h, 00ch, 07ch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h
    db  000h, 000h, 0e0h, 060h, 060h, 078h, 06ch, 066h, 066h, 066h, 066h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 07ch, 0c6h, 0c0h, 0c0h, 0c0h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 01ch, 00ch, 00ch, 03ch, 06ch, 0cch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 07ch, 0c6h, 0feh, 0c0h, 0c0h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 038h, 06ch, 064h, 060h, 0f0h, 060h, 060h, 060h, 060h, 0f0h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 076h, 0cch, 0cch, 0cch, 0cch, 0cch, 07ch, 00ch, 0cch, 078h, 000h
    db  000h, 000h, 0e0h, 060h, 060h, 06ch, 076h, 066h, 066h, 066h, 066h, 0e6h, 000h, 000h, 000h, 000h
    db  000h, 000h, 018h, 018h, 000h, 038h, 018h, 018h, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 006h, 006h, 000h, 00eh, 006h, 006h, 006h, 006h, 006h, 006h, 066h, 066h, 03ch, 000h
    db  000h, 000h, 0e0h, 060h, 060h, 066h, 06ch, 078h, 078h, 06ch, 066h, 0e6h, 000h, 000h, 000h, 000h
    db  000h, 000h, 038h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0e6h, 0ffh, 0dbh, 0dbh, 0dbh, 0dbh, 0dbh, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0dch, 066h, 066h, 066h, 066h, 066h, 066h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 07ch, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0dch, 066h, 066h, 066h, 066h, 066h, 07ch, 060h, 060h, 0f0h, 000h
    db  000h, 000h, 000h, 000h, 000h, 076h, 0cch, 0cch, 0cch, 0cch, 0cch, 07ch, 00ch, 00ch, 01eh, 000h
    db  000h, 000h, 000h, 000h, 000h, 0dch, 076h, 066h, 060h, 060h, 060h, 0f0h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 07ch, 0c6h, 060h, 038h, 00ch, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 010h, 030h, 030h, 0fch, 030h, 030h, 030h, 030h, 036h, 01ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0cch, 0cch, 0cch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0c3h, 0c3h, 0c3h, 0c3h, 066h, 03ch, 018h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0c3h, 0c3h, 0c3h, 0dbh, 0dbh, 0ffh, 066h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0c3h, 066h, 03ch, 018h, 03ch, 066h, 0c3h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 07eh, 006h, 00ch, 0f8h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0feh, 0cch, 018h, 030h, 060h, 0c6h, 0feh, 000h, 000h, 000h, 000h
    db  000h, 000h, 00eh, 018h, 018h, 018h, 070h, 018h, 018h, 018h, 018h, 00eh, 000h, 000h, 000h, 000h
    db  000h, 000h, 018h, 018h, 018h, 018h, 000h, 018h, 018h, 018h, 018h, 018h, 000h, 000h, 000h, 000h
    db  000h, 000h, 070h, 018h, 018h, 018h, 00eh, 018h, 018h, 018h, 018h, 070h, 000h, 000h, 000h, 000h
    db  000h, 000h, 076h, 0dch, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 010h, 038h, 06ch, 0c6h, 0c6h, 0c6h, 0feh, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 03ch, 066h, 0c2h, 0c0h, 0c0h, 0c0h, 0c2h, 066h, 03ch, 00ch, 006h, 07ch, 000h, 000h
    db  000h, 000h, 0cch, 000h, 000h, 0cch, 0cch, 0cch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h
    db  000h, 00ch, 018h, 030h, 000h, 07ch, 0c6h, 0feh, 0c0h, 0c0h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 010h, 038h, 06ch, 000h, 078h, 00ch, 07ch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h
    db  000h, 000h, 0cch, 000h, 000h, 078h, 00ch, 07ch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h
    db  000h, 060h, 030h, 018h, 000h, 078h, 00ch, 07ch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h
    db  000h, 038h, 06ch, 038h, 000h, 078h, 00ch, 07ch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 03ch, 066h, 060h, 060h, 066h, 03ch, 00ch, 006h, 03ch, 000h, 000h, 000h
    db  000h, 010h, 038h, 06ch, 000h, 07ch, 0c6h, 0feh, 0c0h, 0c0h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 0c6h, 000h, 000h, 07ch, 0c6h, 0feh, 0c0h, 0c0h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 060h, 030h, 018h, 000h, 07ch, 0c6h, 0feh, 0c0h, 0c0h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 066h, 000h, 000h, 038h, 018h, 018h, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 018h, 03ch, 066h, 000h, 038h, 018h, 018h, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 060h, 030h, 018h, 000h, 038h, 018h, 018h, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 0c6h, 000h, 010h, 038h, 06ch, 0c6h, 0c6h, 0feh, 0c6h, 0c6h, 0c6h, 000h, 000h, 000h, 000h
    db  038h, 06ch, 038h, 000h, 038h, 06ch, 0c6h, 0c6h, 0feh, 0c6h, 0c6h, 0c6h, 000h, 000h, 000h, 000h
    db  018h, 030h, 060h, 000h, 0feh, 066h, 060h, 07ch, 060h, 060h, 066h, 0feh, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 06eh, 03bh, 01bh, 07eh, 0d8h, 0dch, 077h, 000h, 000h, 000h, 000h
    db  000h, 000h, 03eh, 06ch, 0cch, 0cch, 0feh, 0cch, 0cch, 0cch, 0cch, 0ceh, 000h, 000h, 000h, 000h
    db  000h, 010h, 038h, 06ch, 000h, 07ch, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 0c6h, 000h, 000h, 07ch, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 060h, 030h, 018h, 000h, 07ch, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 030h, 078h, 0cch, 000h, 0cch, 0cch, 0cch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h
    db  000h, 060h, 030h, 018h, 000h, 0cch, 0cch, 0cch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h
    db  000h, 000h, 0c6h, 000h, 000h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 07eh, 006h, 00ch, 078h, 000h
    db  000h, 0c6h, 000h, 07ch, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 0c6h, 000h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 018h, 018h, 07eh, 0c3h, 0c0h, 0c0h, 0c0h, 0c3h, 07eh, 018h, 018h, 000h, 000h, 000h, 000h
    db  000h, 038h, 06ch, 064h, 060h, 0f0h, 060h, 060h, 060h, 060h, 0e6h, 0fch, 000h, 000h, 000h, 000h
    db  000h, 000h, 0c3h, 066h, 03ch, 018h, 0ffh, 018h, 0ffh, 018h, 018h, 018h, 000h, 000h, 000h, 000h
    db  000h, 0fch, 066h, 066h, 07ch, 062h, 066h, 06fh, 066h, 066h, 066h, 0f3h, 000h, 000h, 000h, 000h
    db  000h, 00eh, 01bh, 018h, 018h, 018h, 07eh, 018h, 018h, 018h, 018h, 018h, 0d8h, 070h, 000h, 000h
    db  000h, 018h, 030h, 060h, 000h, 078h, 00ch, 07ch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h
    db  000h, 00ch, 018h, 030h, 000h, 038h, 018h, 018h, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 018h, 030h, 060h, 000h, 07ch, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 018h, 030h, 060h, 000h, 0cch, 0cch, 0cch, 0cch, 0cch, 0cch, 076h, 000h, 000h, 000h, 000h
    db  000h, 000h, 076h, 0dch, 000h, 0dch, 066h, 066h, 066h, 066h, 066h, 066h, 000h, 000h, 000h, 000h
    db  076h, 0dch, 000h, 0c6h, 0e6h, 0f6h, 0feh, 0deh, 0ceh, 0c6h, 0c6h, 0c6h, 000h, 000h, 000h, 000h
    db  000h, 03ch, 06ch, 06ch, 03eh, 000h, 07eh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 038h, 06ch, 06ch, 038h, 000h, 07ch, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 030h, 030h, 000h, 030h, 030h, 060h, 0c0h, 0c6h, 0c6h, 07ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 0feh, 0c0h, 0c0h, 0c0h, 0c0h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 0feh, 006h, 006h, 006h, 006h, 000h, 000h, 000h, 000h, 000h
    db  000h, 0c0h, 0c0h, 0c2h, 0c6h, 0cch, 018h, 030h, 060h, 0ceh, 09bh, 006h, 00ch, 01fh, 000h, 000h
    db  000h, 0c0h, 0c0h, 0c2h, 0c6h, 0cch, 018h, 030h, 066h, 0ceh, 096h, 03eh, 006h, 006h, 000h, 000h
    db  000h, 000h, 018h, 018h, 000h, 018h, 018h, 018h, 03ch, 03ch, 03ch, 018h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 036h, 06ch, 0d8h, 06ch, 036h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0d8h, 06ch, 036h, 06ch, 0d8h, 000h, 000h, 000h, 000h, 000h, 000h
    db  011h, 044h, 011h, 044h, 011h, 044h, 011h, 044h, 011h, 044h, 011h, 044h, 011h, 044h, 011h, 044h
    db  055h, 0aah, 055h, 0aah, 055h, 0aah, 055h, 0aah, 055h, 0aah, 055h, 0aah, 055h, 0aah, 055h, 0aah
    db  0ddh, 077h, 0ddh, 077h, 0ddh, 077h, 0ddh, 077h, 0ddh, 077h, 0ddh, 077h, 0ddh, 077h, 0ddh, 077h
    db  018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  018h, 018h, 018h, 018h, 018h, 018h, 018h, 0f8h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  018h, 018h, 018h, 018h, 018h, 0f8h, 018h, 0f8h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  036h, 036h, 036h, 036h, 036h, 036h, 036h, 0f6h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 0feh, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  000h, 000h, 000h, 000h, 000h, 0f8h, 018h, 0f8h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  036h, 036h, 036h, 036h, 036h, 0f6h, 006h, 0f6h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  000h, 000h, 000h, 000h, 000h, 0feh, 006h, 0f6h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  036h, 036h, 036h, 036h, 036h, 0f6h, 006h, 0feh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  036h, 036h, 036h, 036h, 036h, 036h, 036h, 0feh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  018h, 018h, 018h, 018h, 018h, 0f8h, 018h, 0f8h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 0f8h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  018h, 018h, 018h, 018h, 018h, 018h, 018h, 01fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  018h, 018h, 018h, 018h, 018h, 018h, 018h, 0ffh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 0ffh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  018h, 018h, 018h, 018h, 018h, 018h, 018h, 01fh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 0ffh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  018h, 018h, 018h, 018h, 018h, 018h, 018h, 0ffh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  018h, 018h, 018h, 018h, 018h, 01fh, 018h, 01fh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  036h, 036h, 036h, 036h, 036h, 036h, 036h, 037h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  036h, 036h, 036h, 036h, 036h, 037h, 030h, 03fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 03fh, 030h, 037h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  036h, 036h, 036h, 036h, 036h, 0f7h, 000h, 0ffh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0ffh, 000h, 0f7h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  036h, 036h, 036h, 036h, 036h, 037h, 030h, 037h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  000h, 000h, 000h, 000h, 000h, 0ffh, 000h, 0ffh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  036h, 036h, 036h, 036h, 036h, 0f7h, 000h, 0f7h, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  018h, 018h, 018h, 018h, 018h, 0ffh, 000h, 0ffh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  036h, 036h, 036h, 036h, 036h, 036h, 036h, 0ffh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 0ffh, 000h, 0ffh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 0ffh, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  036h, 036h, 036h, 036h, 036h, 036h, 036h, 03fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  018h, 018h, 018h, 018h, 018h, 01fh, 018h, 01fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 01fh, 018h, 01fh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 03fh, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  036h, 036h, 036h, 036h, 036h, 036h, 036h, 0ffh, 036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    db  018h, 018h, 018h, 018h, 018h, 0ffh, 018h, 0ffh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  018h, 018h, 018h, 018h, 018h, 018h, 018h, 0f8h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 01fh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh
    db  0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h
    db  00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh
    db  0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 076h, 0dch, 0d8h, 0d8h, 0d8h, 0dch, 076h, 000h, 000h, 000h, 000h
    db  000h, 000h, 078h, 0cch, 0cch, 0cch, 0d8h, 0cch, 0c6h, 0c6h, 0c6h, 0cch, 000h, 000h, 000h, 000h
    db  000h, 000h, 0feh, 0c6h, 0c6h, 0c0h, 0c0h, 0c0h, 0c0h, 0c0h, 0c0h, 0c0h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 0feh, 06ch, 06ch, 06ch, 06ch, 06ch, 06ch, 06ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 0feh, 0c6h, 060h, 030h, 018h, 030h, 060h, 0c6h, 0feh, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 07eh, 0d8h, 0d8h, 0d8h, 0d8h, 0d8h, 070h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 066h, 066h, 066h, 066h, 066h, 07ch, 060h, 060h, 0c0h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 076h, 0dch, 018h, 018h, 018h, 018h, 018h, 018h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 07eh, 018h, 03ch, 066h, 066h, 066h, 03ch, 018h, 07eh, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 038h, 06ch, 0c6h, 0c6h, 0feh, 0c6h, 0c6h, 06ch, 038h, 000h, 000h, 000h, 000h
    db  000h, 000h, 038h, 06ch, 0c6h, 0c6h, 0c6h, 06ch, 06ch, 06ch, 06ch, 0eeh, 000h, 000h, 000h, 000h
    db  000h, 000h, 01eh, 030h, 018h, 00ch, 03eh, 066h, 066h, 066h, 066h, 03ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 07eh, 0dbh, 0dbh, 0dbh, 07eh, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 003h, 006h, 07eh, 0dbh, 0dbh, 0f3h, 07eh, 060h, 0c0h, 000h, 000h, 000h, 000h
    db  000h, 000h, 01ch, 030h, 060h, 060h, 07ch, 060h, 060h, 060h, 030h, 01ch, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 07ch, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 0c6h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 0feh, 000h, 000h, 0feh, 000h, 000h, 0feh, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 018h, 018h, 07eh, 018h, 018h, 000h, 000h, 0ffh, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 030h, 018h, 00ch, 006h, 00ch, 018h, 030h, 000h, 07eh, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 00ch, 018h, 030h, 060h, 030h, 018h, 00ch, 000h, 07eh, 000h, 000h, 000h, 000h
    db  000h, 000h, 00eh, 01bh, 01bh, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    db  018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h, 0d8h, 0d8h, 0d8h, 070h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 018h, 018h, 000h, 07eh, 000h, 018h, 018h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 076h, 0dch, 000h, 076h, 0dch, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 038h, 06ch, 06ch, 038h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 018h, 018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 00fh, 00ch, 00ch, 00ch, 00ch, 00ch, 0ech, 06ch, 06ch, 03ch, 01ch, 000h, 000h, 000h, 000h
    db  000h, 0d8h, 06ch, 06ch, 06ch, 06ch, 06ch, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 070h, 0d8h, 030h, 060h, 0c8h, 0f8h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 07ch, 07ch, 07ch, 07ch, 07ch, 07ch, 07ch, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
_vgafont14alt:                               ; 0xc79ec LB 0x12d
    db  01dh, 000h, 000h, 000h, 000h, 024h, 066h, 0ffh, 066h, 024h, 000h, 000h, 000h, 000h, 000h, 022h
    db  000h, 063h, 063h, 063h, 022h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 02bh, 000h
    db  000h, 000h, 018h, 018h, 018h, 0ffh, 018h, 018h, 018h, 000h, 000h, 000h, 000h, 02dh, 000h, 000h
    db  000h, 000h, 000h, 000h, 0ffh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 04dh, 000h, 000h, 0c3h
    db  0e7h, 0ffh, 0dbh, 0c3h, 0c3h, 0c3h, 0c3h, 0c3h, 000h, 000h, 000h, 054h, 000h, 000h, 0ffh, 0dbh
    db  099h, 018h, 018h, 018h, 018h, 018h, 03ch, 000h, 000h, 000h, 056h, 000h, 000h, 0c3h, 0c3h, 0c3h
    db  0c3h, 0c3h, 0c3h, 066h, 03ch, 018h, 000h, 000h, 000h, 057h, 000h, 000h, 0c3h, 0c3h, 0c3h, 0c3h
    db  0dbh, 0dbh, 0ffh, 066h, 066h, 000h, 000h, 000h, 058h, 000h, 000h, 0c3h, 0c3h, 066h, 03ch, 018h
    db  03ch, 066h, 0c3h, 0c3h, 000h, 000h, 000h, 059h, 000h, 000h, 0c3h, 0c3h, 0c3h, 066h, 03ch, 018h
    db  018h, 018h, 03ch, 000h, 000h, 000h, 05ah, 000h, 000h, 0ffh, 0c3h, 086h, 00ch, 018h, 030h, 061h
    db  0c3h, 0ffh, 000h, 000h, 000h, 06dh, 000h, 000h, 000h, 000h, 000h, 0e6h, 0ffh, 0dbh, 0dbh, 0dbh
    db  0dbh, 000h, 000h, 000h, 076h, 000h, 000h, 000h, 000h, 000h, 0c3h, 0c3h, 0c3h, 066h, 03ch, 018h
    db  000h, 000h, 000h, 077h, 000h, 000h, 000h, 000h, 000h, 0c3h, 0c3h, 0dbh, 0dbh, 0ffh, 066h, 000h
    db  000h, 000h, 091h, 000h, 000h, 000h, 000h, 06eh, 03bh, 01bh, 07eh, 0d8h, 0dch, 077h, 000h, 000h
    db  000h, 09bh, 000h, 018h, 018h, 07eh, 0c3h, 0c0h, 0c0h, 0c3h, 07eh, 018h, 018h, 000h, 000h, 000h
    db  09dh, 000h, 000h, 0c3h, 066h, 03ch, 018h, 0ffh, 018h, 0ffh, 018h, 018h, 000h, 000h, 000h, 09eh
    db  000h, 0fch, 066h, 066h, 07ch, 062h, 066h, 06fh, 066h, 066h, 0f3h, 000h, 000h, 000h, 0f1h, 000h
    db  000h, 018h, 018h, 018h, 0ffh, 018h, 018h, 018h, 000h, 0ffh, 000h, 000h, 000h, 0f6h, 000h, 000h
    db  018h, 018h, 000h, 000h, 0ffh, 000h, 000h, 018h, 018h, 000h, 000h, 000h, 000h
_vgafont16alt:                               ; 0xc7b19 LB 0x145
    db  01dh, 000h, 000h, 000h, 000h, 000h, 024h, 066h, 0ffh, 066h, 024h, 000h, 000h, 000h, 000h, 000h
    db  000h, 030h, 000h, 000h, 03ch, 066h, 0c3h, 0c3h, 0dbh, 0dbh, 0c3h, 0c3h, 066h, 03ch, 000h, 000h
    db  000h, 000h, 04dh, 000h, 000h, 0c3h, 0e7h, 0ffh, 0ffh, 0dbh, 0c3h, 0c3h, 0c3h, 0c3h, 0c3h, 000h
    db  000h, 000h, 000h, 054h, 000h, 000h, 0ffh, 0dbh, 099h, 018h, 018h, 018h, 018h, 018h, 018h, 03ch
    db  000h, 000h, 000h, 000h, 056h, 000h, 000h, 0c3h, 0c3h, 0c3h, 0c3h, 0c3h, 0c3h, 0c3h, 066h, 03ch
    db  018h, 000h, 000h, 000h, 000h, 057h, 000h, 000h, 0c3h, 0c3h, 0c3h, 0c3h, 0c3h, 0dbh, 0dbh, 0ffh
    db  066h, 066h, 000h, 000h, 000h, 000h, 058h, 000h, 000h, 0c3h, 0c3h, 066h, 03ch, 018h, 018h, 03ch
    db  066h, 0c3h, 0c3h, 000h, 000h, 000h, 000h, 059h, 000h, 000h, 0c3h, 0c3h, 0c3h, 066h, 03ch, 018h
    db  018h, 018h, 018h, 03ch, 000h, 000h, 000h, 000h, 05ah, 000h, 000h, 0ffh, 0c3h, 086h, 00ch, 018h
    db  030h, 060h, 0c1h, 0c3h, 0ffh, 000h, 000h, 000h, 000h, 06dh, 000h, 000h, 000h, 000h, 000h, 0e6h
    db  0ffh, 0dbh, 0dbh, 0dbh, 0dbh, 0dbh, 000h, 000h, 000h, 000h, 076h, 000h, 000h, 000h, 000h, 000h
    db  0c3h, 0c3h, 0c3h, 0c3h, 066h, 03ch, 018h, 000h, 000h, 000h, 000h, 077h, 000h, 000h, 000h, 000h
    db  000h, 0c3h, 0c3h, 0c3h, 0dbh, 0dbh, 0ffh, 066h, 000h, 000h, 000h, 000h, 078h, 000h, 000h, 000h
    db  000h, 000h, 0c3h, 066h, 03ch, 018h, 03ch, 066h, 0c3h, 000h, 000h, 000h, 000h, 091h, 000h, 000h
    db  000h, 000h, 000h, 06eh, 03bh, 01bh, 07eh, 0d8h, 0dch, 077h, 000h, 000h, 000h, 000h, 09bh, 000h
    db  018h, 018h, 07eh, 0c3h, 0c0h, 0c0h, 0c0h, 0c3h, 07eh, 018h, 018h, 000h, 000h, 000h, 000h, 09dh
    db  000h, 000h, 0c3h, 066h, 03ch, 018h, 0ffh, 018h, 0ffh, 018h, 018h, 018h, 000h, 000h, 000h, 000h
    db  09eh, 000h, 0fch, 066h, 066h, 07ch, 062h, 066h, 06fh, 066h, 066h, 066h, 0f3h, 000h, 000h, 000h
    db  000h, 0abh, 000h, 0c0h, 0c0h, 0c2h, 0c6h, 0cch, 018h, 030h, 060h, 0ceh, 09bh, 006h, 00ch, 01fh
    db  000h, 000h, 0ach, 000h, 0c0h, 0c0h, 0c2h, 0c6h, 0cch, 018h, 030h, 066h, 0ceh, 096h, 03eh, 006h
    db  006h, 000h, 000h, 000h, 000h
_vbebios_copyright:                          ; 0xc7c5e LB 0x15
    db  'VirtualBox VESA BIOS', 000h
_vbebios_vendor_name:                        ; 0xc7c73 LB 0x13
    db  'Oracle Corporation', 000h
_vbebios_product_name:                       ; 0xc7c86 LB 0x21
    db  'Oracle VM VirtualBox VBE Adapter', 000h
_vbebios_product_revision:                   ; 0xc7ca7 LB 0x23
    db  'Oracle VM VirtualBox Version 5.0.2', 000h
_vbebios_info_string:                        ; 0xc7cca LB 0x2b
    db  'VirtualBox VBE Display Adapter enabled', 00dh, 00ah, 00dh, 00ah, 000h
_no_vbebios_info_string:                     ; 0xc7cf5 LB 0x29
    db  'No VirtualBox VBE support available!', 00dh, 00ah, 00dh, 00ah, 000h

section CONST progbits vstart=0x7d1e align=1 ; size=0x0 class=DATA group=DGROUP

section CONST2 progbits vstart=0x7d1e align=1 ; size=0x0 class=DATA group=DGROUP

  ; Padding 0x2e2 bytes at 0xc7d1e
    db  001h, 000h, 000h, 000h, 000h, 001h, 000h, 000h, 000h, 000h, 000h, 000h, 02fh, 068h, 06fh, 06dh
    db  065h, 02fh, 066h, 06dh, 033h, 02fh, 073h, 072h, 063h, 02fh, 076h, 062h, 06fh, 078h, 02fh, 06fh
    db  075h, 074h, 02fh, 06ch, 069h, 06eh, 075h, 078h, 02eh, 061h, 06dh, 064h, 036h, 034h, 02fh, 072h
    db  065h, 06ch, 065h, 061h, 073h, 065h, 02fh, 06fh, 062h, 06ah, 02fh, 056h, 042h, 06fh, 078h, 056h
    db  067h, 061h, 042h, 069h, 06fh, 073h, 02fh, 056h, 042h, 06fh, 078h, 056h, 067h, 061h, 042h, 069h
    db  06fh, 073h, 02eh, 073h, 079h, 06dh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 07eh
