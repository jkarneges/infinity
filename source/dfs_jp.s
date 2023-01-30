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

   .area _CODE_98

;;----------------------------------------------------------------------------
;;[ DrawChar ]
;;parameters:
;; A = Character to Draw
;; B = X offset (0-7)
;; HL -> Starting Tile
;;modifies:
;; ABCDEFHL
;;----------------------------------------------------------------------------
;DrawChar::
;   PUSH  HL
;   SWAP  A
;   LD    H, A
;   AND   #0xF0
;   LD    L, A
;   XOR   H
;   LD    H, A
;
;   PUSH  HL
;   CALL  getsmallfont
;   POP   HL
;
;   ADD   HL, DE
;   LD    D, H
;   LD    E, L
;   POP   HL
;
;   LD    A, B
;   ADD   A, A
;   LD    A, H
;   ADC   A, #0
;   LD    H, A
;
;   LD    A, B
;   ADD   A, A
;   AND   #0xF0
;
;   ADD   A, L
;   LD    L, A
;   ADC   A, H
;   SUB   L
;   LD    H, A
;
;   LD    A, B
;   AND   #0x07
;
;   CALL  JumpTable
;.DW DC_align,  DC_shift1, DC_shift2, DC_shift3
;.DW DC_shift4, DC_shift5, DC_shift6, DC_shift7
;
;DC_align:
;   LD    B, #16
;   JP    dfs_align_loop
;
;DC_shift1:
;   LD    BC, #0x087F
;   PUSH  DE
;   CALL  dfs_shift1_loop
;   POP   DE
;   LD    BC, #0x0880
;   JP    dfs_shift1_loop
;
;DC_shift2:
;   LD    BC, #0x083F
;   PUSH  DE
;   CALL  dfs_shift2_loop
;   POP   DE
;   LD    BC, #0x08C0
;   JP    dfs_shift2_loop
;
;DC_shift3:
;   LD    BC, #0x081F
;   PUSH  DE
;   CALL  dfs_shift3_loop
;   POP   DE
;   LD    BC, #0x08E0
;   JP    dfs_shift3_loop
;
;DC_shift4:
;   LD    BC, #0x080F
;   PUSH  DE
;   CALL  dfs_shift4_loop
;   POP   DE
;   LD    BC, #0x08F0
;   JP    dfs_shift4_loop
;
;DC_shift5:
;   LD    BC, #0x0807
;   PUSH  DE
;   CALL  dfs_shift5_loop
;   POP   DE
;   LD    BC, #0x08F8
;   JP    dfs_shift5_loop
;
;DC_shift6:
;   LD    BC, #0x0803
;   PUSH  DE
;   CALL  dfs_shift6_loop
;   POP   DE
;   LD    BC, #0x08FC
;   JP    dfs_shift6_loop
;
;DC_shift7:
;   LD    BC, #0x0801
;   PUSH  DE
;   CALL  dfs_shift7_loop
;   POP   DE
;   LD    BC, #0x08FE
;   JP    dfs_shift7_loop

;----------------------------------------------------------------------------
;[ BC_getsize_jp ]
;parms:
; DE = Character
; B = coordinate of char (len)
;returns:
; NC if non-extended
; CY if extended
;modifies:
; ADE
;----------------------------------------------------------------------------
BC_getsize_jp::
   PUSH  DE          ; Save Character
   PUSH  HL          ; Save Starting Tile

   LD    H, D
   LD    L, E
   ADD   HL, HL      ; HL = A * 2
   ADD   HL, DE      ; HL = A * 3
   ADD   HL, HL      ; HL = A * 6
   ADD   HL, HL      ; HL = A * 12
   ADD   HL, DE      ; HL = A * 13

   LD    DE, #bigfont_noshadow - 128 * 13 ; offset by 128 characters
   ADD   HL, DE      ; HL = A * 13 + Tile Data
   LD    A, (HL)

   POP   HL
   POP   DE

   RLCA                                ; shift a bit
   RRCA                                ; put it back (carry does not change)
   RET   NC                            ; NC if not extended

   INC   DE                            ; next character if extended
   LD    A, B                          ; extended character adds 8 to width
   SUB   #-8                           ; need to set carry so subtract by -8
   LD    B, A                          ;

   RET




;_dfs_pschar_main::
;;   LD    DE, #bigfont
;   CALL  getbigfont
;
;   LD    A, (HLD)
;   LD    C, A                           ; chr
;   LD    A, (HLD)
;   LD    B, A                           ; x
;
;   LD    A, (HL)                        ; start_tile
;   CALL  Calc_PA_in_PT_at_A
;
;   LD    A, C
;
;PC_extended2:
;   PUSH  AF
;   PUSH  BC
;   PUSH  DE
;   PUSH  HL
;   CALL  DrawBigCharSprite
;   POP   HL
;   POP   DE
;   POP   BC
;   POP   AF
;
;   PUSH  DE
;   CALL  BC_getsize
;   POP   DE
;
;   JR    C, PC_extended2
;
;   ADD   A, B
;   LD    E, A
;   RET

;;----------------------------------------------------------------------------
;;[ DrawBigCharSprite ]
;;parameters:
;; A = Character to Draw
;; B = X offset
;; DE -> Font Table
;; HL -> Starting Tile
;;modifies:
;; ABCDEFHL
;;----------------------------------------------------------------------------
;DrawBigCharSprite::
;   PUSH  HL          ; Save Starting Tile
;
;   LD    L, A
;   ADD   A, A        ; A = A * 2
;   ADD   A, L
;   LD    L, A
;   LD    A, #0
;   ADC   A, A
;   LD    H, A        ; HL = A * 3
;
;   ADD   HL, HL
;   ADD   HL, HL
;   ADD   HL, HL      ; HL = A * 24
;   ADD   HL, DE      ; HL = A * 24 + Tile Data
;   PUSH  HL          ; Save Font Address
;
;   LD    A, B        ; HL=(B&0xF8)*4
;   AND   #0xF8
;   RLCA
;   RLCA
;   LD    C, A
;   AND   #0xFC
;   LD    L, A
;   XOR   C
;   LD    H, A
;
;   LD    A, B
;   AND   #0x07       ; Setup for jumptable
;
;   POP   DE          ; DE = A * 24 + DE
;   POP   BC          ; BC = Starting Tile
;   ADD   HL, BC      ; HL = (B&0xF8)*4 + HL
;
;;   LD    HL, #0x8800
;
;   CALL  JumpTable
;
;.DW DBCS_align,  DBCS_shift1, DBCS_shift2, DBCS_shift3
;.DW DBCS_shift4, DBCS_shift5, DBCS_shift6, DBCS_shift7
;
;DBCS_align:
;   LD    B, #24
;   JP    dfs_align_loop
;
;DBCS_shift1:
;   LD    BC, #0x0C7F
;   PUSH  DE
;   CALL  dfs_shift1_loop
;   POP   DE
;   LD    C, #0x08
;   ADD   HL, BC
;   LD    BC, #0x0C80
;   JP    dfs_shift1_loop
;
;DBCS_shift2:
;   LD    BC, #0x0C3F
;   PUSH  DE
;   CALL  dfs_shift2_loop
;   POP   DE
;   LD    C, #0x08
;   ADD   HL, BC
;   LD    BC, #0x0CC0
;   JP    dfs_shift2_loop
;
;DBCS_shift3:
;   LD    BC, #0x0C1F
;   PUSH  DE
;   CALL  dfs_shift3_loop
;   POP   DE
;   LD    C, #0x08
;   ADD   HL, BC
;   LD    BC, #0x0CE0
;   JP    dfs_shift3_loop
;
;DBCS_shift4:
;   LD    BC, #0x0C0F
;   PUSH  DE
;   CALL  dfs_shift4_loop
;   POP   DE
;   LD    C, #0x08
;   ADD   HL, BC
;   LD    BC, #0x0CF0
;   JP    dfs_shift4_loop
;
;DBCS_shift5:
;   LD    BC, #0x0C07
;   PUSH  DE
;   CALL  dfs_shift5_loop
;   POP   DE
;   LD    C, #0x08
;   ADD   HL, BC
;   LD    BC, #0x0CF8
;   JP    dfs_shift5_loop
;
;DBCS_shift6:
;   LD    BC, #0x0C03
;   PUSH  DE
;   CALL  dfs_shift6_loop
;   POP   DE
;   LD    C, #0x08
;   ADD   HL, BC
;   LD    BC, #0x0CFC
;   JP    dfs_shift6_loop
;
;DBCS_shift7:
;   LD    BC, #0x0C01
;   PUSH  DE
;   CALL  dfs_shift7_loop
;   POP   DE
;   LD    C, #0x08
;   ADD   HL, BC
;   LD    BC, #0x0CFE
;   JP    dfs_shift7_loop





_dfs_pchar_jp_main::
   LD    HL, #_barg1
   LD    A, (HLI)
   LD    C, (HL)
   LD    B, A

;   LD    DE, #bigfont
;   CALL  getbigfont

   LD    HL, #_warg4+1
   LD    A, (HLD)
   LD    E, (HL)
   AND   #0x07
   LD    D, A

   LD    A, #147
   CALL  Calc_PA_in_PT_at_A

PC_extended:
   PUSH  AF
   PUSH  BC
   PUSH  DE
   PUSH  HL
   CALL  DrawBigCharJp
   POP   HL
   POP   DE
   POP   BC
   POP   AF

   PUSH  DE
   CALL  BC_getsize_jp
   POP   DE

   JR    C, PC_extended

   ADD   A, B
   LD    (_barg1), A
   RET

;----------------------------------------------------------------------------
;[ DrawBigCharJp ]
;parameters:
; (B,C) = X,Y offset
; DE = Character to Draw
; HL -> Starting Tile
;modifies:
; ABCDEFHL
;----------------------------------------------------------------------------
DrawBigCharJp:
   PUSH  HL          ; Save Starting Tile

   LD    H, D
   LD    L, E
   ADD   HL, HL      ; HL = char * 2
   ADD   HL, DE      ; HL = char * 3
   ADD   HL, HL      ; HL = char * 6
   ADD   HL, HL      ; HL = char * 12
   ADD   HL, DE      ; HL = char * 13
   INC   HL          ; HL = char * 13 + 1

   LD    DE, #bigfont_noshadow - 128 * 13 ; offset by 128 characters
   ADD   HL, DE      ; HL = char * 13 + Tile Data + 1
   PUSH  HL          ; Save Font Address

   LD    H, #0
   LD    L, C
   ADD   HL, HL      ; HL = y * 2

   LD    A, B
   AND   #0xF8
   RRCA              ; A = (x & 0xF8) / 2
   LD    C, A        ; C = (x & 0xF8) / 2
   RRCA              ; A = (x & 0xF8) / 4
   ;RRCA              ; A = (x & 0xF8) / 8
   ADD   A, C        ; A = (x & 0xF8) / 2 + (x & 0xF8) / 4 = ((x & 0xF8) * 6) / 8
   SWAP  A
   LD    C, A
   AND   #0xF0
   LD    E, A
   XOR   C
   LD    D, A        ; DE = (x & 0xF8) * 12
   ADD   HL, DE      ; HL = ((x & 0xF8) * 6 + C) * 2

   LD    A, B
   AND   #0x07       ; Setup for jumptable

   POP   DE          ; DE = char * 13 + Tile Data + 1
   POP   BC          ; BC = Starting Tile
   ADD   HL, BC      ; HL = ((x & 0xF8) * 6 + y) * 2 + Starting Tile

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

   JP    or_a_vram      ;output shadow to low

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

   JP    or_a_vram      ;output shadow to low

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

   JP    or_a_vram      ;output shadow to low

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

   JP    or_a_vram      ;output shadow to low

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

   JP    or_a_vram      ;output shadow to low

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

   JP    or_a_vram      ;output shadow to low

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

   JP    or_a_vram      ;output shadow to low

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

   JP    or_a_vram      ;output shadow to low

bigfont_noshadow::
   .include "eve/font_jp.h"
