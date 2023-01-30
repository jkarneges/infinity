; Copyright (C) 1999-2001 Affinix Software, LLC
;
; This file is part of Infinity.
;
; This file may be used under the terms of the Creative Commons Attribution-
; NonCommercial-ShareAlike 4.0 International License as published by Creative
; Commons.
;
; Alternatively, this file may be used under the terms of the GNU General
; Public License as published by the Free Software Foundation, either version
; 3 of the License, or (at your option) any later version.
;
; In addition, as a special exception, Affinix Software gives you certain
; additional rights. These rights are described in the LICENSE file in this
; package.

   .include "gbasm.h"

ldi_v_a     = 0x08
ldi_v_aa    = 0x20

   .area _CODE_13

   .include "font.h"

;----------------------------------------------------------------------------
;[ DrawChar ]
;parameters:
; A = Character to Draw
; B = X offset (0-7)
; HL -> Starting Tile
;modifies:
; ABCDEFHL
;----------------------------------------------------------------------------
DrawChar::
   PUSH  HL
   SWAP  A
   LD    H, A
   AND   #0xF0
   LD    L, A
   XOR   H
   LD    H, A

   PUSH  HL
   CALL  getsmallfont
   POP   HL

   ADD   HL, DE
   LD    D, H
   LD    E, L
   POP   HL

   LD    A, B
   ADD   A, A
   LD    A, H
   ADC   A, #0
   LD    H, A

   LD    A, B
   ADD   A, A
   AND   #0xF0

   ADD   A, L
   LD    L, A
   ADC   A, H
   SUB   L
   LD    H, A

   LD    A, B
   AND   #0x07

   CALL  JumpTable
.DW DC_align,  DC_shift1, DC_shift2, DC_shift3
.DW DC_shift4, DC_shift5, DC_shift6, DC_shift7

DC_align:
   LD    B, #16
   JP    dfs_align_loop

DC_shift1:
   LD    BC, #0x087F
   PUSH  DE
   CALL  dfs_shift1_loop
   POP   DE
   LD    BC, #0x0880
   JP    dfs_shift1_loop

DC_shift2:
   LD    BC, #0x083F
   PUSH  DE
   CALL  dfs_shift2_loop
   POP   DE
   LD    BC, #0x08C0
   JP    dfs_shift2_loop

DC_shift3:
   LD    BC, #0x081F
   PUSH  DE
   CALL  dfs_shift3_loop
   POP   DE
   LD    BC, #0x08E0
   JP    dfs_shift3_loop

DC_shift4:
   LD    BC, #0x080F
   PUSH  DE
   CALL  dfs_shift4_loop
   POP   DE
   LD    BC, #0x08F0
   JP    dfs_shift4_loop

DC_shift5:
   LD    BC, #0x0807
   PUSH  DE
   CALL  dfs_shift5_loop
   POP   DE
   LD    BC, #0x08F8
   JP    dfs_shift5_loop

DC_shift6:
   LD    BC, #0x0803
   PUSH  DE
   CALL  dfs_shift6_loop
   POP   DE
   LD    BC, #0x08FC
   JP    dfs_shift6_loop

DC_shift7:
   LD    BC, #0x0801
   PUSH  DE
   CALL  dfs_shift7_loop
   POP   DE
   LD    BC, #0x08FE
   JP    dfs_shift7_loop

;----------------------------------------------------------------------------
;[ BC_getsize ]
;parms:
; A = Character
; B = coordinate of char (len)
; DE -> Font Table
;returns:
; NC if non-extended
; CY if extended
;modifies:
; ABDE
;----------------------------------------------------------------------------
BC_getsize::
   ADD   A, E                          ; DE->size of font
   LD    E, A                          ;
   ADC   A, D                          ;
   SUB   E                             ;
   LD    D, A                          ;

   LD    A, (DE)                       ; get size
   RLCA                                ; shift a bit
   SRL   A                             ; shift out a bit
   RET   NC                            ; NC if not extended

   PUSH  AF                            ; extended character adds 8 to width
   LD    A, B                          ;
   ADD   A, #0x08                      ;
   LD    B, A                          ;
   POP   AF                            ;

   RET



_dfs_pschar_main::
;   LD    DE, #bigfont
   CALL  getbigfont

   LD    A, (HLD)
   LD    C, A                           ; chr
   LD    A, (HLD)
   LD    B, A                           ; x

   LD    A, (HL)                        ; start_tile
   CALL  Calc_PA_in_PT_at_A

   LD    A, C

PC_extended2:
   PUSH  AF
   PUSH  BC
   PUSH  DE
   PUSH  HL
   CALL  DrawBigCharSprite
   POP   HL
   POP   DE
   POP   BC
   POP   AF

   PUSH  DE
   CALL  BC_getsize
   POP   DE

   JR    C, PC_extended2

   ADD   A, B
   LD    E, A
   RET

;----------------------------------------------------------------------------
;[ DrawBigCharSprite ]
;parameters:
; A = Character to Draw
; B = X offset
; DE -> Font Table
; HL -> Starting Tile
;modifies:
; ABCDEFHL
;----------------------------------------------------------------------------
DrawBigCharSprite::
   PUSH  HL          ; Save Starting Tile

   LD    L, A
   ADD   A, A        ; A = A * 2
   ADD   A, L
   LD    L, A
   LD    A, #0
   ADC   A, A
   LD    H, A        ; HL = A * 3

   ADD   HL, HL
   ADD   HL, HL
   ADD   HL, HL      ; HL = A * 24
   ADD   HL, DE      ; HL = A * 24 + Tile Data
   PUSH  HL          ; Save Font Address

   LD    A, B        ; HL=(B&0xF8)*4
   AND   #0xF8
   RLCA
   RLCA
   LD    C, A
   AND   #0xFC
   LD    L, A
   XOR   C
   LD    H, A

   LD    A, B
   AND   #0x07       ; Setup for jumptable

   POP   DE          ; DE = A * 24 + DE
   POP   BC          ; BC = Starting Tile
   ADD   HL, BC      ; HL = (B&0xF8)*4 + HL

;   LD    HL, #0x8800

   CALL  JumpTable

.DW DBCS_align,  DBCS_shift1, DBCS_shift2, DBCS_shift3
.DW DBCS_shift4, DBCS_shift5, DBCS_shift6, DBCS_shift7

DBCS_align:
   LD    B, #24
   JP    dfs_align_loop

DBCS_shift1:
   LD    BC, #0x0C7F
   PUSH  DE
   CALL  dfs_shift1_loop
   POP   DE
   LD    C, #0x08
   ADD   HL, BC
   LD    BC, #0x0C80
   JP    dfs_shift1_loop

DBCS_shift2:
   LD    BC, #0x0C3F
   PUSH  DE
   CALL  dfs_shift2_loop
   POP   DE
   LD    C, #0x08
   ADD   HL, BC
   LD    BC, #0x0CC0
   JP    dfs_shift2_loop

DBCS_shift3:
   LD    BC, #0x0C1F
   PUSH  DE
   CALL  dfs_shift3_loop
   POP   DE
   LD    C, #0x08
   ADD   HL, BC
   LD    BC, #0x0CE0
   JP    dfs_shift3_loop

DBCS_shift4:
   LD    BC, #0x0C0F
   PUSH  DE
   CALL  dfs_shift4_loop
   POP   DE
   LD    C, #0x08
   ADD   HL, BC
   LD    BC, #0x0CF0
   JP    dfs_shift4_loop

DBCS_shift5:
   LD    BC, #0x0C07
   PUSH  DE
   CALL  dfs_shift5_loop
   POP   DE
   LD    C, #0x08
   ADD   HL, BC
   LD    BC, #0x0CF8
   JP    dfs_shift5_loop

DBCS_shift6:
   LD    BC, #0x0C03
   PUSH  DE
   CALL  dfs_shift6_loop
   POP   DE
   LD    C, #0x08
   ADD   HL, BC
   LD    BC, #0x0CFC
   JP    dfs_shift6_loop

DBCS_shift7:
   LD    BC, #0x0C01
   PUSH  DE
   CALL  dfs_shift7_loop
   POP   DE
   LD    C, #0x08
   ADD   HL, BC
   LD    BC, #0x0CFE
   JP    dfs_shift7_loop





_dfs_pchar_main::
   LD    HL, #_barg1
   LD    A, (HLI)
   LD    C, (HL)
   LD    B, A

;   LD    DE, #bigfont
   CALL  getbigfont

   LD    A, #147
   CALL  Calc_PA_in_PT_at_A

   LD    A, (_warg4)
PC_extended:
   PUSH  AF
   PUSH  BC
   PUSH  DE
   PUSH  HL
   CALL  DrawBigChar
   POP   HL
   POP   DE
   POP   BC
   POP   AF

   PUSH  DE
   CALL  BC_getsize
   POP   DE

   JR    C, PC_extended

   ADD   A, B
   LD    (_barg1), A
   RET

;----------------------------------------------------------------------------
;[ DrawBigChar ]
;parameters:
; A = Character to Draw
; (B,C) = X,Y offset
; DE -> Font Table
; HL -> Starting Tile
;modifies:
; ABCDEFHL
;----------------------------------------------------------------------------
DrawBigChar::
   CP    A, #128
   JP    NC, test_shadow_precompose

   PUSH  HL          ; Save Starting Tile

   LD    L, A
   ADD   A, A        ; A = A * 2
   ADD   A, L
   LD    L, A
   LD    A, #0
   ADC   A, A
   LD    H, A        ; HL = A * 3

   ADD   HL, HL
   ADD   HL, HL
   ADD   HL, HL      ; HL = A * 24
   ADD   HL, DE      ; HL = A * 24 + Tile Data
   PUSH  HL          ; Save Font Address

   LD    H, #0
   LD    L, C
   ADD   HL, HL      ; HL = C * 2

   LD    A, B
   AND   #0xF8
   RRCA              ; A = (B & 0xF8) / 2
   LD    C, A        ; C = (B & 0xF8) / 2
   RRCA              ; A = (B & 0xF8) / 4
   ;RRCA              ; A = (B & 0xF8) / 8
   ADD   A, C        ; A = (B & 0xF8) / 2 + (B & 0xF8) / 4 = ((B & 0xF8) * 6) / 8
   SWAP  A
   LD    C, A
   AND   #0xF0
   LD    E, A
   XOR   C
   LD    D, A        ; DE = (B & 0xF8) * 12
   ADD   HL, DE      ; HL = ((B & 0xF8) * 6 + C) * 2

   LD    A, B
   AND   #0x07       ; Setup for jumptable

   POP   DE          ; DE = A * 24 + DE
   POP   BC          ; BC = Starting Tile
   ADD   HL, BC      ; HL = ((B & 0xF8) * 5 + C) * 2 + HL

   CALL  JumpTable

.DW DBC_align,  DBC_shift1, DBC_shift2, DBC_shift3
.DW DBC_shift4, DBC_shift5, DBC_shift6, DBC_shift7

DBC_align:
   LD    B, #24

dfs_align_loop:
   LD    A, (DE)

   INC   DE
   CALL  or_a_vram

   DEC   B
   JR    NZ, dfs_align_loop
   RET

DBC_shift1:
   LD    BC, #0x0C7F
   PUSH  DE
   CALL  dfs_shift1_loop
   POP   DE
   LD    C, #72                         ;6*16-2*12
   ADD   HL, BC
   LD    BC, #0x0C80

dfs_shift1_loop:
   LD    A, (DE)
   INC   DE
   RRCA
   AND   C

   CALL  or_a_vram

   LD    A, (DE)
   RRCA
   AND   C

   CALL  or_a_vram
   INC   DE
   DEC   B
   JR    NZ, dfs_shift1_loop
   RET

DBC_shift2:
   LD    BC, #0x0C3F
   PUSH  DE
   CALL  dfs_shift2_loop
   POP   DE
   LD    C, #72                         ;6*16-2*12
   ADD   HL, BC
   LD    BC, #0x0CC0

dfs_shift2_loop:
   LD    A, (DE)
   INC   DE
   RRCA
   RRCA
   AND   C

   CALL  or_a_vram

   LD    A, (DE)
   RRCA
   RRCA
   AND   C

   CALL  or_a_vram
   INC   DE
   DEC   B
   JR    NZ, dfs_shift2_loop
   RET

DBC_shift3:
   LD    BC, #0x0C1F
   PUSH  DE
   CALL  dfs_shift3_loop
   POP   DE
   LD    C, #72                         ;6*16-2*12
   ADD   HL, BC
   LD    BC, #0x0CE0

dfs_shift3_loop:
   LD    A, (DE)
   INC   DE
   RRCA
   RRCA
   RRCA
   AND   C

   CALL  or_a_vram

   LD    A, (DE)
   RRCA
   RRCA
   RRCA
   AND   C

   CALL  or_a_vram
   INC   DE
   DEC   B
   JR    NZ, dfs_shift3_loop
   RET

DBC_shift4:
   LD    BC, #0x0C0F
   PUSH  DE
   CALL  dfs_shift4_loop
   POP   DE
   LD    C, #72                         ;6*16-2*12
   ADD   HL, BC
   LD    BC, #0x0CF0

dfs_shift4_loop:
   LD    A, (DE)
   INC   DE
   SWAP  A
   AND   C

   CALL  or_a_vram

   LD    A, (DE)
   SWAP  A
   AND   C

   CALL  or_a_vram
   INC   DE
   DEC   B
   JR    NZ, dfs_shift4_loop
   RET

DBC_shift5:
   LD    BC, #0x0C07
   PUSH  DE
   CALL  dfs_shift5_loop
   POP   DE
   LD    C, #72                         ;6*16-2*12
   ADD   HL, BC
   LD    BC, #0x0CF8

dfs_shift5_loop:
   LD    A, (DE)
   INC   DE
   RLCA
   RLCA
   RLCA
   AND   C

   CALL  or_a_vram

   LD    A, (DE)
   RLCA
   RLCA
   RLCA
   AND   C

   CALL  or_a_vram
   INC   DE
   DEC   B
   JR    NZ, dfs_shift5_loop
   RET

DBC_shift6:
   LD    BC, #0x0C03
   PUSH  DE
   CALL  dfs_shift6_loop
   POP   DE
   LD    C, #72                         ;6*16-2*12
   ADD   HL, BC
   LD    BC, #0x0CFC

dfs_shift6_loop:
   LD    A, (DE)
   INC   DE
   RLCA
   RLCA
   AND   C

   CALL  or_a_vram

   LD    A, (DE)
   RLCA
   RLCA
   AND   C

   CALL  or_a_vram
   INC   DE
   DEC   B
   JR    NZ, dfs_shift6_loop
   RET

DBC_shift7:
   LD    BC, #0x0C01
   PUSH  DE
   CALL  dfs_shift7_loop
   POP   DE
   LD    C, #72                         ;6*16-2*12
   ADD   HL, BC
   LD    BC, #0x0CFE

dfs_shift7_loop:
   LD    A, (DE)
   INC   DE
   RLCA
   AND   C

   CALL  or_a_vram

   LD    A, (DE)
   RLCA
   AND   C

   CALL  or_a_vram
   INC   DE
   DEC   B
   JR    NZ, dfs_shift7_loop
   RET

; A = Character to Draw
; (B,C) = X,Y offset
; DE -> Font Table
; HL -> Starting Tile
;modifies:
; ABCDEFHL
test_shadow_precompose::
   LD    DE, #bigfont_noshadow ; debug hardcode
   ;LD    A, #0                 ; debug hardcode
   SUB   #128

   PUSH  HL          ; Save Starting Tile
   PUSH  DE

   LD    H, #0
   LD    L, A
   LD    D, H
   LD    E, L
   ADD   HL, HL      ; HL = A * 2
   ADD   HL, DE      ; HL = A * 3
   ADD   HL, HL      ; HL = A * 6
   ADD   HL, HL      ; HL = A * 12
   ADD   HL, DE      ; HL = A * 13
   INC   HL          ; HL = A * 13 + 1

   POP   DE
   ADD   HL, DE      ; HL = A * 13 + Tile Data + 1
   PUSH  HL          ; Save Font Address

   LD    H, #0
   LD    L, C
   ADD   HL, HL      ; HL = C * 2

   LD    A, B
   AND   #0xF8
   RRCA              ; A = (B & 0xF8) / 2
   LD    C, A        ; C = (B & 0xF8) / 2
   RRCA              ; A = (B & 0xF8) / 4
   ;RRCA              ; A = (B & 0xF8) / 8
   ADD   A, C        ; A = (B & 0xF8) / 2 + (B & 0xF8) / 4 = ((B & 0xF8) * 6) / 8
   SWAP  A
   LD    C, A
   AND   #0xF0
   LD    E, A
   XOR   C
   LD    D, A        ; DE = (B & 0xF8) * 12
   ADD   HL, DE      ; HL = ((B & 0xF8) * 6 + C) * 2

   LD    A, B
   AND   #0x07       ; Setup for jumptable

   POP   DE          ; DE = A * 24 + DE
   POP   BC          ; BC = Starting Tile
   ADD   HL, BC      ; HL = ((B & 0xF8) * 6 + C) * 2 + HL

   CALL  JumpTable

.DW DBCP_align,  DBCP_shift1, DBCP_shift2, DBCP_shift3
.DW DBCP_shift4, DBCP_shift5, DBCP_shift6, DBCP_shift7

; shadow is low on, high off
; white is low on, high on
; gray is low off, high on (only for precomposed)
DBCP_align:
   PUSH  DE

   LD    B, #12
   XOR   A
dfsp_align_loop_left:
   LD    C, A           ;save shadow
   LD    A, (DE)        ;white part (write to low and high)
   OR    C              ;shadow (write only to low)
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   CALL  or_a_vram      ;output precomposed result to high

   ; load shifted shadow part in A
   LD    A, (DE)
   SRL   A
   INC   DE

   DEC   B
   JR    NZ, dfsp_align_loop_left

   CALL  or_a_vram      ;output shadow to low

   LD    C, #71         ;6*16-2*12-1
   ADD   HL, BC
   POP   DE

   LD    B, #12
   XOR   A
dfsp_align_loop_right:
   CALL  or_a_vram      ;output shadow to low

   INC   HL             ;skip high

   ; load shifted shadow part in A
   LD    A, (DE)
   RRCA
   AND   #0x80
   INC   DE

   DEC   B
   JR    NZ, dfsp_align_loop_right

   CALL  or_a_vram      ;output shadow to low
   RET

DBCP_shift1:
   PUSH  DE

   LD    B, #12
   XOR   A
dfsp_shift1_loop_left:
   LD    C, A           ;save shadow
   LD    A, (DE)        ;white part (write to low and high)
   SRL   A
   OR    C              ;shadow (write only to low)
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   SRL   A
   CALL  or_a_vram      ;output precomposed result to high

   ; load shifted shadow part in A
   LD    A, (DE)
   RRCA
   RRCA
   AND   #0x3F
   INC   DE

   DEC   B
   JR    NZ, dfsp_shift1_loop_left

   CALL  or_a_vram      ;output shadow to low

   LD    C, #71         ;6*16-2*12-1
   ADD   HL, BC
   POP   DE

   LD    B, #12
   XOR   A
dfsp_shift1_loop_right:
   LD    C, A           ;save shadow
   LD    A, (DE)        ;white part (write to low and high)
   RRCA
   AND   #0x80
   OR    C              ;shadow (write only to low)
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   RRCA
   AND   #0x80
   CALL  or_a_vram      ;output precomposed result to high

   ; load shifted shadow part in A
   LD    A, (DE)
   RRCA
   RRCA
   AND   #0xC0
   INC   DE

   DEC   B
   JR    NZ, dfsp_shift1_loop_right

   CALL  or_a_vram      ;output shadow to low
   RET

DBCP_shift2:
   PUSH  DE

   LD    B, #12
   XOR   A
dfsp_shift2_loop_left:
   LD    C, A           ;save shadow
   LD    A, (DE)        ;white part (write to low and high)
   RRCA
   RRCA
   AND   #0x3F
   OR    C              ;shadow (write only to low)
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   RRCA
   RRCA
   AND   #0x3F
   CALL  or_a_vram      ;output precomposed result to high

   ; load shifted shadow part in A
   LD    A, (DE)
   RRCA
   RRCA
   RRCA
   AND   #0x1F
   INC   DE

   DEC   B
   JR    NZ, dfsp_shift2_loop_left

   CALL  or_a_vram      ;output shadow to low

   LD    C, #71         ;6*16-2*12-1
   ADD   HL, BC
   POP   DE

   LD    B, #12
   XOR   A
dfsp_shift2_loop_right:
   LD    C, A           ;save shadow
   LD    A, (DE)        ;white part (write to low and high)
   RRCA
   RRCA
   AND   #0xC0
   OR    C              ;shadow (write only to low)
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   RRCA
   RRCA
   AND   #0xC0
   CALL  or_a_vram      ;output precomposed result to high

   ; load shifted shadow part in A
   LD    A, (DE)
   RRCA
   RRCA
   RRCA
   AND   #0xE0
   INC   DE

   DEC   B
   JR    NZ, dfsp_shift2_loop_right

   CALL  or_a_vram      ;output shadow to low
   RET

DBCP_shift3:
   PUSH  DE

   LD    B, #12
   XOR   A
dfsp_shift3_loop_left:
   LD    C, A           ;save shadow
   LD    A, (DE)        ;white part (write to low and high)
   RRCA
   RRCA
   RRCA
   AND   #0x1F
   OR    C              ;shadow (write only to low)
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   RRCA
   RRCA
   RRCA
   AND   #0x1F
   CALL  or_a_vram      ;output precomposed result to high

   ; load shifted shadow part in A
   LD    A, (DE)
   SWAP  A
   AND   #0x0F
   INC   DE

   DEC   B
   JR    NZ, dfsp_shift3_loop_left

   CALL  or_a_vram      ;output shadow to low

   LD    C, #71         ;6*16-2*12-1
   ADD   HL, BC
   POP   DE

   LD    B, #12
   XOR   A
dfsp_shift3_loop_right:
   LD    C, A           ;save shadow
   LD    A, (DE)        ;white part (write to low and high)
   RRCA
   RRCA
   RRCA
   AND   #0xE0
   OR    C              ;shadow (write only to low)
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   RRCA
   RRCA
   RRCA
   AND   #0xE0
   CALL  or_a_vram      ;output precomposed result to high

   ; load shifted shadow part in A
   LD    A, (DE)
   SWAP  A
   AND   #0xF0
   INC   DE

   DEC   B
   JR    NZ, dfsp_shift3_loop_right

   CALL  or_a_vram      ;output shadow to low
   RET

DBCP_shift4:
   PUSH  DE

   LD    B, #12
   XOR   A
dfsp_shift4_loop_left:
   LD    C, A           ;save shadow
   LD    A, (DE)        ;white part (write to low and high)
   SWAP  A
   AND   #0x0F
   OR    C              ;shadow (write only to low)
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   SWAP  A
   AND   #0x0F
   CALL  or_a_vram      ;output precomposed result to high

   ; load shifted shadow part in A
   LD    A, (DE)
   RLCA
   RLCA
   RLCA
   AND   #0x07
   INC   DE

   DEC   B
   JR    NZ, dfsp_shift4_loop_left

   CALL  or_a_vram      ;output shadow to low

   LD    C, #71         ;6*16-2*12-1
   ADD   HL, BC
   POP   DE

   LD    B, #12
   XOR   A
dfsp_shift4_loop_right:
   LD    C, A           ;save shadow
   LD    A, (DE)        ;white part (write to low and high)
   SWAP  A
   AND   #0xF0
   OR    C              ;shadow (write only to low)
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   SWAP  A
   AND   #0xF0
   CALL  or_a_vram      ;output precomposed result to high

   ; load shifted shadow part in A
   LD    A, (DE)
   ADD   A, A
   ADD   A, A
   ADD   A, A
   INC   DE

   DEC   B
   JR    NZ, dfsp_shift4_loop_right

   CALL  or_a_vram      ;output shadow to low
   RET

DBCP_shift5:
   PUSH  DE

   LD    B, #12
   XOR   A
dfsp_shift5_loop_left:
   LD    C, A           ;save shadow
   LD    A, (DE)        ;white part (write to low and high)
   RLCA
   RLCA
   RLCA
   AND   #0x07
   OR    C              ;shadow (write only to low)
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   RLCA
   RLCA
   RLCA
   AND   #0x07
   CALL  or_a_vram      ;output precomposed result to high

   ; load shifted shadow part in A
   LD    A, (DE)
   RLCA
   RLCA
   AND   #0x03
   INC   DE

   DEC   B
   JR    NZ, dfsp_shift5_loop_left

   CALL  or_a_vram      ;output shadow to low

   LD    C, #71         ;6*16-2*12-1
   ADD   HL, BC
   POP   DE

   LD    B, #12
   XOR   A
dfsp_shift5_loop_right:
   LD    C, A           ;save shadow
   LD    A, (DE)        ;white part (write to low and high)
   ADD   A, A
   ADD   A, A
   ADD   A, A
   OR    C              ;shadow (write only to low)
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   ADD   A, A
   ADD   A, A
   ADD   A, A
   CALL  or_a_vram      ;output precomposed result to high

   ; load shifted shadow part in A
   LD    A, (DE)
   ADD   A, A
   ADD   A, A
   INC   DE

   DEC   B
   JR    NZ, dfsp_shift5_loop_right

   CALL  or_a_vram      ;output shadow to low
   RET

DBCP_shift6:
   PUSH  DE

   LD    B, #12
   XOR   A
dfsp_shift6_loop_left:
   LD    C, A           ;save shadow
   LD    A, (DE)        ;white part (write to low and high)
   RLCA
   RLCA
   AND   #0x03
   OR    C              ;shadow (write only to low)
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   RLCA
   RLCA
   AND   #0x03
   CALL  or_a_vram      ;output precomposed result to high

   ; load shifted shadow part in A
   LD    A, (DE)
   RLCA
   AND   #0x01
   INC   DE

   DEC   B
   JR    NZ, dfsp_shift6_loop_left

   CALL  or_a_vram      ;output shadow to low

   LD    C, #71         ;6*16-2*12-1
   ADD   HL, BC
   POP   DE

   LD    B, #12
   XOR   A
dfsp_shift6_loop_right:
   LD    C, A           ;save shadow
   LD    A, (DE)        ;white part (write to low and high)
   ADD   A, A
   ADD   A, A
   OR    C              ;shadow (write only to low)
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   ADD   A, A
   ADD   A, A
   CALL  or_a_vram      ;output precomposed result to high

   ; load shifted shadow part in A
   LD    A, (DE)
   ADD   A, A
   INC   DE

   DEC   B
   JR    NZ, dfsp_shift6_loop_right

   CALL  or_a_vram      ;output shadow to low
   RET

DBCP_shift7:
   PUSH  DE

   LD    B, #12
dfsp_shift7_loop_left:
   LD    A, (DE)        ;white part (write to low and high)
   RLCA
   AND   #0x01
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   RLCA
   AND   #0x01
   CALL  or_a_vram      ;output precomposed result to high

   ; no need for shadow part yet
   INC   DE

   DEC   B
   JR    NZ, dfsp_shift7_loop_left

   LD    C, #72         ;6*16-2*12
   ADD   HL, BC
   POP   DE

   LD    B, #12
   XOR   A
dfsp_shift7_loop_right:
   LD    C, A           ;save shadow
   LD    A, (DE)        ;white part (write to low and high)
   ADD   A, A
   OR    C              ;shadow (write only to low)
   CALL  or_a_vram      ;output precomposed result to low

   LD    A, (DE)        ;white part (write to low and high)
   ADD   A, A
   CALL  or_a_vram      ;output precomposed result to high

   ; load shifted shadow part in A
   LD    A, (DE)
   INC   DE

   DEC   B
   JR    NZ, dfsp_shift7_loop_right

   CALL  or_a_vram      ;output shadow to low
   RET



_draw_menu_main::
   PUSH  BC

   LD    A, (HL)
   PUSH  AF
   XOR   #3
   LD    H, #0
   LD    L, A
   ADD   HL, HL
   LD    BC, #tab_table
   ADD   HL, BC
   LD    B, H
   LD    C, L

   XOR   #3
   RRCA
   RRCA
   RRCA
   LD    E, A
   AND   #0x1F
   LD    D, A
   XOR   E
   LD    E, A                 ; DE -> row
   LD    HL, #tab_bottom_table
   ADD   HL, DE
   LD    D, H
   LD    E, L

   LD    HL, #_window_ptr
   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A                 ; HL -> window

   PUSH  DE
   LD    DE, #4
   PUSH  HL
   ADD   HL, DE
   CALL  puttab
   POP   HL
   LD    E, #10
   PUSH  HL
   ADD   HL, DE
   CALL  puttab
   POP   HL
   LD    E, #14
   PUSH  HL
   ADD   HL, DE
   CALL  puttab
   POP   HL

   LD    E, #64
   ADD   HL, DE
   POP   DE

   PUSH  HL

   LD    B, #20
dm_loop:
   LD    A, (DE)
   INC   DE
   RST   ldi_v_a
   DEC   B
   JR    NZ, dm_loop

   POP   HL

   LD    BC, #33
   ADD   HL, BC

   POP   AF
   CALL  JumpTable

   .dw   menu_ability, menu_backpack
   .dw   menu_equip,   menu_gems

menu_ability:
   LD    A, #0x04
   LD    E, #0x0E
dm_loop3:
   LD    BC, #0x120E
dm_loop2:
   RST   ldi_v_a
   DEC   B
   JR    NZ, dm_loop2
   ADD   HL, BC

   DEC   E
   JR    NZ, dm_loop3

   JR    menu_finish

menu_backpack:
   LD    A, #0x09
   LD    DE, #0x070A
   CALL  dm_routine
   LD    A, #0x0B
   RST   ldi_v_a
   ADD   HL, BC

   LD    BC, #0x050C
   LD    DE, #0x0403
dm_loop5:
   PUSH  BC
   LD    A, B
   CALL  dm_routine
   LD    A, #15
   RST   ldi_v_a
   ADD   HL, BC
   POP   BC
   DEC   C
   JR    NZ, dm_loop5

   LD    A, #0x0C
   LD    DE, #0x010D
   CALL  dm_routine
   LD    A, #0x0E
   RST   ldi_v_a

menu_gems:
menu_equip:
menu_finish:
   POP   BC
   RET

_menu_clearbox_main::
   PUSH  BC
   LD    A, (HLD)
   LD    C, A
   LD    A, (HLD)
   LD    B, A
   LD    A, (HLD)
   RRCA
   RRCA
   RRCA
   LD    E, A
   AND   #0x1F
   LD    D, A
   XOR   E
   LD    E, (HL)
   ADD   A, E
   LD    L, A

   LD    A, D
   ADD   A, #0x9C
   LD    H, A

   LD    A, #32
   SUB   B
   LD    E, A
   LD    D, #0x00

   LD    A, #0x04

_mcb_loop:
   PUSH  BC
   CALL  _STIR_B_VRAM
   ADD   HL, DE
   POP   BC
   DEC   C
   JR    NZ, _mcb_loop

   POP   BC
   RET

_menu_drawbox_main::
   PUSH  BC

   LD    A, (HLD)   ; height
   DEC   A
   DEC   A
   LD    C, A
   LD    A, (HLD)   ; length
   DEC   A
   DEC   A
   LD    B, A
   LD    A, (HLD)   ; y
   RRCA
   RRCA
   RRCA
   LD    E, A
   AND   #0x1F
   LD    D, A
   XOR   E
   ADD   A, (HL)

   LD    L, A
   LD    A, D
   ADD   A, #0x9C
   LD    H, A

   XOR   A
   LD    DE, #0x0102
   PUSH  BC
   CALL  dm_custom
   POP   BC
   LD    A, #30
   SUB   B
   LD    D, #0x00
   LD    E, A
   ADD   HL, DE

dm_loop_1:
   LD    A, #0x03
   LD    DE, #0x0405
   PUSH  BC
   CALL  dm_custom
   POP   BC
   LD    A, #30
   SUB   B
   LD    D, #0x00
   LD    E, A
   ADD   HL, DE

   DEC   C
   JR    NZ, dm_loop_1

   LD    A, #0x06
   LD    DE, #0x0708
   CALL  dm_custom

   POP   BC

   RET

; draws tiles:
; ADDDDDDDE     <- D comes up B times
dm_routine:
   LD    BC, #0x0F0E
dm_custom:
   RST   ldi_v_a
   LD    A, D
dm_loop4:
   RST   ldi_v_a
   DEC   B
   JR    NZ, dm_loop4
   LD    A, E
   RST   ldi_v_a
   RET

puttab:
   LD    A, (BC)
   INC   BC
   RST   ldi_v_a
   LD    E, #31
   ADD   HL, DE
   LD    A, (BC)
   INC   BC
   RST   ldi_v_a
   RET

tab_bottom_table:
   .DB   3, 4, 4, 4,12, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2
tab_table:
   .DB   0, 3, 0, 3, 0, 3, 2, 5, 2, 5, 2, 5
   .DB   0, 1, 1, 1,13, 4, 4, 4, 4, 4,12, 1, 1, 1, 1, 1, 1, 1, 1, 2
   .DB  72,73,68,69,65,75,73, 0, 0, 0, 0, 0
   .DB   0, 1, 1, 1, 1, 1, 1, 1, 1, 1,13, 4, 4, 4,12, 1, 1, 1, 1, 2
   .DB  74,85,83,84,73,78, 0, 0, 0, 0, 0, 0
   .DB   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,13, 4, 4, 4, 4, 5
