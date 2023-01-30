/*
 * Copyright (C) 1999-2001 Affinix Software, LLC
 *
 * This file is part of Infinity.
 *
 * This file may be used under the terms of the Creative Commons Attribution-
 * NonCommercial-ShareAlike 4.0 International License as published by Creative
 * Commons.
 *
 * Alternatively, this file may be used under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either version
 * 3 of the License, or (at your option) any later version.
 *
 * In addition, as a special exception, Affinix Software gives you certain
 * additional rights. These rights are described in the LICENSE file in this
 * package.
 */

#define REGBASE      0xF8

#define MOREDELAY 30

#define CASE_STEP 0
#define CASE_TALK 1
#define CASE_SEARCH  2
#define CASE_GUY  3
#define CASE_HSTEP   4
#define CASE_VSTEP   5
#define CASE_QSTEP   6
#define CASE_TREASURE   7

#define ACTION_WALK  3
#define ACTION_CATCHUP  19
#define ACTION_SAY   1
#define ACTION_POSXY 0
#define ACTION_SLEEP 20
#define ACTION_GROUP 2
#define ACTION_WARP  0x06
#define ACTION_FRAME 0x45
#define ACTION_FACE  7

#define ACTION_LOCKON   0x34
#define ACTION_PAN   0x35
#define ACTION_LOCKOFF  0x36
#define ACTION_CAMERA   0x37

#define ACTION_LMAP  10
#define ACTION_LDUDE 9
#define ACTION_DEL   0x0B
#define ACTION_LGUY  0x0C

#define ACTION_CSTEP 0x1F
#define ACTION_CTALK 0x20
#define ACTION_CSEARCH  0x21
#define ACTION_CHSTEP   0x22
#define ACTION_CVSTEP   0x23

#define ACTION_SONG  0x24
#define ACTION_SFX   0x48
#define ACTION_CONTROL  0x25

#define ACTION_GOTO  0x27
#define ACTION_FADEOUT  0x28
#define ACTION_TILE  0x29

#define ACTION_IF 0x15
#define ACTION_SET   0x16

#define ACTION_ADD   0x17
#define ACTION_SUB   0x18
#define ACTION_MUL   0x19
#define ACTION_DIV   0x1a

#define ACTION_INC   0x1b
#define ACTION_DEC   0x1c

#define ACTION_GET   0x1d
#define ACTION_PUT   0x1e

#define ACTION_GIVE  0x30
#define ACTION_TAKE  0x31
#define ACTION_HAVE  0x32

#define ACTION_PARTYSET 0x40
#define ACTION_PARTYNOR 0x41
#define ACTION_PARTYARR 0x42

#define ACTION_PANSPEED 0x43
#define ACTION_FADEIN   0x44

#define ACTION_REFORM   0x46
#define ACTION_DETACH   0x47

#define ACTION_CALL  0x49

#define ACTION_CGOLD 0x4a
#define ACTION_TGOLD 0x4b
#define ACTION_SHOPRES  0x4c
#define ACTION_SHOPADD  0x4d
#define ACTION_SHOP  0x4e
#define ACTION_YESNO 0x4f
#define ACTION_SAVE  0x50

#define ACTION_ZONE  0x51
#define ACTION_GETXY 0x52

#define ACTION_TREASURE 0x53
#define ACTION_HEALALL  0x54
#define ACTION_OPMENU   0x55
#define ACTION_COLSHIFT 0x56
#define ACTION_CIF   0x57
#define ACTION_PROPHECY 0x58
#define ACTION_TBASE 0x59

#define ACTION_BATTLE   0x5a
#define ACTION_SONGOFF  0x5b
#define ACTION_SETSKL   0x5c

#define ACTION_SONGPAUSE  0x5d
#define ACTION_SONGRESUME 0x5e

#define ACTION_CQSTEP   0x5f

#define ACTION_GIVEGEM  0x60
#define ACTION_REVIVE   0x61
#define ACTION_SETFORM  0x62
#define ACTION_EQUIP    0x63

#define ACTION_WALK2 0x64
#define ACTION_BOATON   0x65
#define ACTION_BOATOFF  0x66

#define ACTION_MAPSTATE 0x67
#define ACTION_HITSTATE 0x68

#define ACTION_QUAKE 0x69
#define ACTION_SETZ  0x70
#define ACTION_SETWOLF  0x71
#define ACTION_RAND  0x72
#define ACTION_FADEIN2  0x73
#define ACTION_PANREL   0x74

#define ACTION_PALSET   0x75
#define ACTION_PALNORM  0x76
#define ACTION_LEVELUP  0x77
#define ACTION_SETBOAT  0x78
#define ACTION_SHOPADD2 0x79
#define ACTION_SETWOLFP 0x7a

#define ACTION_GIVE2    0x7b
#define ACTION_SETSKL2  0x7c

#define ACTION_ANIM     0x7d
#define ACTION_WARP2    0x7e
#define ACTION_LDUDEHI  0x7f
#define ACTION_DARKEN   0x6a

#define ACTION_COLSHIFT2 0x6b

#define ACTION_DEQUIP   0x6c
#define ACTION_SAYAUTO  0x6d

#define ACTION_LASTMOVS 0x6e
#define ACTION_DRAWDUDES 0x6f

#define ACTION_UNFRAME 0x80

#define ACTION_BSONG 0x81

#define ACTION_CUTSCENE 0x82

#define ACTIONSIZE   12
#define MAXACTIONS   8
#define MAXCASES        36

extern BYTE numactions;
extern BYTE action_table[MAXACTIONS][ACTIONSIZE];
extern UWORD actionp, actionptmp[12];
extern UBYTE actionpstack;
extern UBYTE in_event;
extern UBYTE action_catchup;
extern WORD action_sleep;
extern UBYTE isyesno;
extern BYTE numcases;
extern BYTE event_cases[MAXCASES][8];
extern UWORD tbase;
extern UBYTE tbaselen, tbasemul;
extern UBYTE battleresult;

extern UWORD frame;
