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

tile_base = 0

   .area _BSS

   .area _CODE

_draw_menu::
   LDHL  SP, #2

   LDH   A, (_rompage)
   PUSH  AF
   LD    A, #_rom_page_dfs
   RST   0x30
   CALL  _draw_menu_main
   POP   AF
   RST   0x30
   RET

;----------------------------------------------------------------------------
;[[ menu_clearbox(ubyte x, ubyte y, ubyte w, ubyte h) ]]
;----------------------------------------------------------------------------
_menu_clearbox::
   LDHL  SP, #0x05
   LDH   A, (_rompage)
   PUSH  AF
   LD    A, #_rom_page_dfs
   RST   0x30
   CALL  _menu_clearbox_main
   POP   AF
   RST   0x30
   RET

; x, y, length, height
_menu_drawbox::
   LDHL  SP, #5
   LDH   A, (_rompage)
   PUSH  AF
   LD    A, #_rom_page_dfs
   RST   0x30
   CALL  _menu_drawbox_main
   POP   AF
   RST   0x30
   RET

