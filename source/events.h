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

#include "d1.h"
#include "events_def.h"

BYTE numactions;
BYTE action_table[MAXACTIONS][ACTIONSIZE];
UWORD actionp, actionptmp[12];
UBYTE actionpstack;
UBYTE in_event;
UBYTE action_catchup;
WORD action_sleep;
UBYTE isyesno;
BYTE numcases;
BYTE event_cases[MAXCASES][8];
UWORD tbase;
UBYTE tbaselen, tbasemul;
UBYTE battleresult;

extern UBYTE pfs_targetspr, pfs_targetbgr;
extern UWORD pal_cur[];
extern UWORD great_dark_fade_pal[];

void reset_events()
{
    numcases = 0;
    in_event = 0;
}

void init_event(UWORD eventnum)
{
    numactions = 0;
    in_event = 1;
    control = 0;

    action_catchup = 0;
    actionp = eventnum;
    actionpstack = 0;
    isyesno = 0;
    battleresult = BATTLE_WIN;
    bsong = 0;
}

UBYTE getreg(UBYTE op1)
{
    if (op1 == 255)
        return 255;

    if (op1 >= REGBASE)
        op1 = regs[op1 - REGBASE];

    return op1;
}

UBYTE enext()
{
    return (getreg(get_event_byte(actionp++)));
}

UBYTE enextraw()
{
    return (get_event_byte(actionp++));
}

extern UBYTE dfs_language;

void dialogue_scroll(UBYTE n);

// char test_str[] = { 128+6, 128+7, 128+8, 128+0, 128+1, 128+2, 128+3, 128+4, 128+5, '|', 128+9, 128+10, 128+11,
// 128+12, 128+13, 128+14, 128+15, 128+16, 128+17, 128+18, 128+19, 128+5, 0 }; char test_str[] = { 128+1, 128+2, 128+3,
// 128+4, 128+5, 128+6, 128+7, 128+8, 128+9, 128+10, 128+11, 128+12, 128+13, 128+14, 128+15, 128+16, 128+17, 128+18,
// 128+19, 0 }; //128+20, 128+21, 128+20, 128+21, 128+22, 128+23, 128+13, 128+14, 128+15, 128+16, 128+17, 128+18,
// 128+22, 128+16, 128+17, 128+16, 128+17, 128+18, 128+19, 128+10, 128+11, 128+17, 128+22, 128+13, 128+16, 128+17,
// 128+6, 128+7, 128+8, 128+13, 128+14, 128+15, 128+20, 128+21, 128+22, 128+14, 128+15, 128+16, 128+17, 128+18, 128+19,
// 128+17, 128+22, 128+23, 128+20, 128+21, 128+22, 128+23, 128+20, 128+20, 128+21, 128+16, 128+16, 128+17, 128+18,
// 128+22, 128+23, 128+24, 128+22, 128+16, 128+17, 128+15, 128+20, 128+22, 128+23, 128+24, 128+25, 128+10, 128+17,
// 128+19, 128+20, 128+21, 128+22, 128+23, 128+10, 128+13, 128+15, 128+13, 128+16, 128+22, 128+23, 128+2, 128+1, 128+23,
// 128+13, 128+14, 128+15, 128+16, 128+6, 128+6, 128+7, 128+14, 128+20, 128+20, 128+20, 128+21, 128+19, 128+22, 128+23,
// 128+22, 128+23, 128+24, 128+16, 128+19, 128+19, 128+18, 128+18, 128+22, 128+23, 128+22, 128+23, 128+16, 128+17,
// 128+18, 128+19, 128+20, 128+22, 128+19, 128+20, 128+22, 128+22, 128+23, 128+24, 128+20, 128+21, 128+16, 128+17,
// 128+16, 128+17, 128+15, 128+22, 128+16, 128+19, 128+19, 128+22, 128+6, 128+7, 128+14, 128+22, 128+23, 128+16, 128+17,
// 128+17, 128+19, 128+20, 128+17, 128+19, 128+19, 128+20, 128+17, 128+22, 128+13, 128+16, 128+22, 128+23, 128+24,
// 128+13, 128+14, 128+13, 128+14, 128+15, 128+20, 128+21, 128+19, 128+16, 128+13, 128+14, 128+15, 128+16, 128+22,
// 128+22, 128+19, 128+16, 128+17, 128+20, 128+22, 128+23, 128+24, 128+17, 128+22, 128+22, 128+23, 128+1, 128+23,
// 128+13, 128+13, 128+20, 128+21, 128+22, 128+18, 128+17, 128+22, 128+22, 128+19, 128+20, 128+22, 128+6, 128+7, 128+14,
// 128+22, 128+23, 128+16, 128+17, 128+18, 128+19, 128+16, 128+17, 128+19, 128+18, 128+19, 128+22, 128+23, 128+16,
// 128+16, 128+19, 128+22, 128+16, 128+19, 128+19, 128+20, 128+22, 128+23, 128+16, 128+16, 128+17, 128+22, 128+22,
// 128+23, 128+24, 128+14, 128+10, 128+11, 128+12, 128+13, 128+18, 128+20, 128+13, 128+14, 128+15, 128+16, 128+17,
// 128+16, 128+17, 128+16, 128+22, 128+20, 128+18, 128+10, 128+15, 128+19, 128+22, 128+6, 128+7, 128+14, 128+10, 128+17,
// 128+19, 128+19, 128+20, 128+17, 128+22, 128+13, 128+16, 128+16, 0 }; char test_str[] = { 128+0, 128+1, 128+2, 128+3,
// 128+4, 128+5, 128+6, 128+7, 128+8, 128+9, 128+10, 128+11, 128+12, 128+13, 128+14, 128+15, 128+16, 128+17, 128+18,
// 128+19, 128+20, 128+20, 128+21, 128+22, 128+23, 128+13, 128+24, 128+25, 128+26, 128+27, 128+28, 128+23, 128+27,
// 128+29, 128+29, 128+30, 128+31, 128+32, 128+10, 128+33, 128+31, 128+23, 128+13, 128+27, 128+34, 128+6, 128+35,
// 128+36, 128+13, 128+37, 128+38, 128+21, 128+39, 128+40, 128+25, 0, };//128+41, 128+42, 128+43, 128+44, 128+45,
// 128+31, 128+23, 128+46, 128+39, 128+47, 128+48, 128+49, 128+39, 128+47, 128+50, 128+29, 128+29, 128+51, 128+52,
// 128+49, 128+53, 128+54, 128+46, 128+30, 128+55, 128+26, 128+21, 128+23, 128+56, 128+57, 128+58, 128+33, 128+31,
// 128+19, 128+59, 128+60, 128+61, 128+62, 128+10, 128+13, 128+26, 128+13, 128+27, 128+49, 128+63, 128+2, 128+1, 128+54,
// 128+13, 128+64, 128+65, 128+34, 128+6, 128+35, 128+36, 128+25, 128+20, 128+20, 128+21, 128+22, 128+19, 128+49,
// 128+66, 128+66, 128+67, 128+68, 128+30, 128+59, 128+19, 128+45, 128+45, 128+56, 128+69, 128+49, 128+70, 128+16,
// 128+71, 128+72, 128+59, 128+73, 128+69, 128+59, 128+74, 128+23, 128+46, 128+75, 128+76, 128+21, 128+77, 128+16,
// 128+78, 128+55, 128+79, 128+26, 128+46, 128+30, 128+59, 128+19, 128+49, 128+35, 128+36, 128+25, 128+53, 128+54,
// 128+16, 128+80, 128+31, 128+19, 128+81, 128+28, 128+19, 128+59, 128+60, 128+28, 128+23, 128+13, 128+79, 128+46,
// 128+82, 128+83, 128+13, 128+84, 128+24, 128+85, 128+86, 128+21, 128+87, 128+59, 128+55, 128+24, 128+88, 128+26,
// 128+89, 128+62, 128+23, 128+74, 128+79, 128+90, 128+21, 128+46, 128+91, 128+92, 128+31, 128+23, 128+63, 128+93,
// 128+1, 128+54, 128+13, 128+84, 128+21, 128+94, 128+95, 128+45, 128+31, 128+62, 128+23, 128+74, 128+21, 128+46,
// 128+35, 128+36, 128+25, 128+53, 128+54, 128+27, 128+96, 128+97, 128+98, 128+16, 128+99, 128+81, 128+32, 128+100,
// 128+49, 128+101, 128+16, 128+99, 128+81, 128+49, 128+30, 128+59, 128+19, 128+102, 128+67, 128+68, 128+16, 128+99,
// 128+28, 128+23, 128+46, 128+103, 128+104, 128+25, 128+10, 128+105, 128+106, 128+107, 128+32, 128+20, 128+13, 128+108,
// 128+109, 128+16, 128+110, 128+90, 128+111, 128+90, 128+62, 128+20, 128+32, 128+10, 128+26, 128+81, 128+46, 128+35,
// 128+36, 128+25, 128+33, 128+31, 128+19, 128+59, 128+60, 128+28, 128+23, 128+13, 128+27, 128+34, 0 };

void do_text(UWORD num)
{
    BYTE c;
    BYTE n;
    UWORD tchar;
    UWORD offset;
    UBYTE count, state;
    UBYTE newlines;
    char *p;
    BYTE reqline, index;

    if (tbase && num >= tbase && num < tbase + tbaselen) {
        num += (UWORD)tbaselen * tbasemul;
    }

    if (!treasuremode)
        uo_translate_cap(num);

    sync_party();
    draw_dudes();

    // dfs_pschar(128, 0, 'A');
    // dfs_pschar(128,10, 'A');
    // while(1);

    owe_off();
    openwindow();

    // test font
    // strcpy( treasurestr, test_str );
    // treasuremode = 1;

#ifdef DEBUG_I18N
restart_text:
    if (key[CANCEL]) {
        do {
            input();
        } while (key[CANCEL]);
        w4vbl_done();
        dfs_language++; // next language
        if (dfs_language >= 6) {
            dfs_language = 0;
        }
        resetupwindow();
        barg1 = 0;

        // display custom menugfx tile (72-77) for language
        put_tile(1, 6, 72 + dfs_language);

        // //display downwards arrows for language ID
        // for(n=0;n<6;n++){
        //    put_tile(1+n, 6, n<=dfs_language ? 16 : 7);
        // }

        // display language in small font
        //  point_str(160, 2);
        //  for(tchar=0;tchar<dfs_language;tchar++){
        //     put_tile(1+tchar, 6, 160+tchar);
        //  }
        //  ipstr(0, 2, (char*)language_str[dfs_language]);
    }
#endif

    if (!treasuremode)
        dialog_setup(num);
    else
        p = treasurestr;

    index = 0;
top:
    dialogue_scroll(-4);
    newlines = 0;
    reqline = 0;

    while (1) {
        // room for this word?
        //      if(!treasuremode) {
        if (checkwindowlen())
            reqline = 1;
        //      }

        if (reqline) {
            // if index == -1, then don't reset index since we'll lose indentation
            if (index == -1)
                index = 1;
            else
                index = 0;

            reqline = 0;

            ++newlines;
            if (newlines == 3)
                break;
            win_newline();
            continue;
        }

        // get a byte
        do {
            tchar = treasuremode ? *p++ : get_dialog_char();
        } while (tchar == ' ' && index == 0);

        if (!tchar)
            break;
        if (tchar == '|') {
            reqline = 1;
            index = -1; // don't destroy indentation
            continue;
        }

        win_pchar(tchar);
        // pflush();
        ++index;

        input();
#ifdef DEBUG_I18N
        if (key[CANCEL]) { // debug
            goto restart_text;
        }
#endif
        if (!text_auto) {
            if (!key[ENTER]) {
                do_delay(2);
            }
        } else {
            do_delay(4);
        }
    }

    if (!tchar) {
        if (!text_auto) {
            w4vbl_done();
            do {
                input();
            } while (key[ENTER]);
            w4vbl_done();
            do {
                input();
#ifdef DEBUG_I18N
                if (key[CANCEL]) { // debug
                    goto restart_text;
                }
#endif
            } while (!key[ENTER]);
            w4vbl_done();
            do {
                input();
            } while (key[ENTER]);
        } else {
            for (n = 0; n < 127; n++) {
#ifdef DEBUG_I18N
                input();
                if (key[CANCEL]) { // debug
                    goto restart_text;
                }
#endif
                do_delay(1);
            }
        }
        do_delay(1);
        goto bottom;
    }

    if (!text_auto) {
        show_moreicon();
        w4vbl_done();

        do {
            input();
        } while (key[ENTER]);

        count = 0;
        state = 0;
        while (1) {
            w4vbl_done();
            c = inputb();
            if (c == ENTER) {
                break;
            }
#ifdef DEBUG_I18N
            if (c == CANCEL) { // debug
                hide_moreicon();
                goto restart_text;
            }
#endif

            ++count;
            if (count == MOREDELAY) {
                count = 0;
                state ^= 1;
                if (state)
                    hide_moreicon();
                else
                    show_moreicon();
            }
        }
        hide_moreicon();
    } else {
        for (n = 0; n < 127; n++) {
#ifdef DEBUG_I18N
            input();
            if (key[CANCEL]) { // debug
                goto restart_text;
            }
#endif
            do_delay(1);
        }
        do_delay(1);
    }

    for (n = 0; n <= 48; n += 2) {
        w4vbl_done();
        dialogue_scroll((UBYTE)n - 4);
    }

    for (n = 0; n < 10; ++n)
        w4vbl_done();

    if (tchar) {
        resetupwindow();
        // pflush();
        goto top;
    }

    w4vbl_done();
    do {
        input();
    } while (key[ENTER]);

bottom:
    w4vbl_done();
    grad_on();
    do_delay(4);

    if (isyesno) {
        n = f_do_yesnomenu();
        regs[isyesno - 1] = n;
        isyesno = 0;
    }

    closewindow();
    owe_on();
}

UBYTE pass_c;

BYTE get_action()
{
    UBYTE c;
    BYTE i, i2;
    WORD x, y;
    UBYTE num;
    UBYTE n, n2;
    UBYTE type;
    UBYTE dir, con;
    UBYTE tileset;
    UBYTE op1, op2, reg, imm;
    UWORD var, eve;
    UBYTE lo, hi;
    UBYTE ask[3];
    struct DUDESTRUCT *ds;

    if (numactions >= MAXACTIONS)
        while (1)
            ;

    if (numactions == 0 && action_sleep == -1)
        action_catchup = 0;

    if (action_catchup) {
        if (action_sleep == -1) {
            return -1;
        }

        --action_sleep;
        if (action_sleep == 0) {
            action_catchup = 0;
        } else
            return -1;
    }

    c = enextraw();
    if (c == 255) {
        if (actionpstack) {
            actionp = actionptmp[--actionpstack];
            return 1;
        } else {
            in_event = 0;
            return 0;
        }
    }

    local_event = event_cases[numcases];

    switch (c) {
    case ACTION_SAY:
        lo = enextraw();
        hi = enextraw();
        var = (UWORD)hi << 8;
        var += lo;

        do_text(var);
        resync();
        break;

    case ACTION_GROUP:
        if (on_boat) {
            for (n = 1; n < 4; ++n) {
                ds = &ch[n];
                ds->x = ch[0].x;
                ds->y = ch[0].y;
                ds->gx = ch[0].gx;
                ds->gy = ch[0].gy;
                ds->dir = ch[0].dir;
            }
            reset_lastmoves();
        } else {
            local_action = action_table[numactions];
            local_action[0] = c;
            local_action[1] = 8;
            ++numactions;
        }
        break;

    case ACTION_LMAP:
        num = enext();
        x = enext();
        y = enext();

        deldudes();

        loadmap(num, x, y);

        // set alutha special-case palette, if an alutha map is detected
        f_setaluthapal(num);

        // copy_page(1, 3);
        // debug_on = 1;

        // load in the party
        load_party(x, y);

        numcases = 0;

        ch[0].req = NORM;

        anim = 0;
        pullparty = 1;
        whotalk = -1;
        instep = 1;
        insearch = 0;
        encounter_steps = ((UBYTE)randfunc() % 64) + 2;
        encounter_protect = 0;
        encounter_zone = 0;
        map_state = hit_state = 0;
        in_fade = 0;
        show_realparty = 0;

        game_loops = 0;
        vblank_timer = 0;
        atmenu = 0;

        sync_party();
        draw_dudes();
        break;

    case ACTION_LDUDEHI:
        num = enext();
        frame = getbaseframe(enext());
        n = enext();
        n2 = enext();
        op1 = enext();

        // hi-sprites aren't normal dudes
        owe_spr(num, (WORD)n << 4, (WORD)n2 << 4, 0, frame + op1);
        break;
    case ACTION_DRAWDUDES:
        draw_dudes();
        break;

    case ACTION_DEL:
        n = enext();
        deldude(n);
        break;

    case ACTION_CUTSCENE:
        f_viewcutscene(enext());
        break;

    case ACTION_FADEIN:
    case ACTION_FADEIN2:
        op1 = enext();

        owe_on();
        sync_party();
        draw_dudes();
        do_delay(4);

        if (op1 == 0 || op1 == 2 || op1 == 3) {
            n2 = op1 == 2 ? 180 : 32;
            //            fadeout(n2);

            n = op1 == 3 ? COLOR_WHITE : COLOR_BLACK;

            fadefrom(n, n2);
            if (c == ACTION_FADEIN2) {
                in_fade = n2;
            } else {
                for (n = 0; n != n2; ++n) {
                    w4vbl_done();
                    fade_step();
                }
            }
        } else if (op1 == 1) {
            if (map_cur)
                owe_off();
            else
                owe_deinit();

            // start the transition
            scl_init();
            scl_roll(108);
            w4vbl_done();
            setcurpal();
            for (n = 0; n < 108; n += 2) {
                do_delay(1);
                scl_roll(108 - n);
            }
            w4vbl_done();
            grad_off();
            scl_off();
            owe_on();
        }

        resync();
        game_mode = 1;
        return 0;
        break;

    case ACTION_FADEOUT:
        do_delay(4);
        op1 = enext();
        if (op1) {
            if (map_cur)
                owe_off();
            else
                owe_deinit();
        }

        // owe_off();

        if (op1 == 0 || op1 == 2 || op1 == 3) {
            n2 = op1 == 2 ? 180 : 32;
            //            fadeout(n2);

            n = op1 == 3 ? COLOR_WHITE : COLOR_BLACK;

            fadeto(n, n2);
            for (n = 0; n != n2; ++n) {
                w4vbl_done();
                fade_step();
            }
        } else if (op1 == 1) {
            // start the transition
            scl_init();
            for (n = 0; n < 108; n += 2) {
                do_delay(1);
                scl_roll(n);
            }
            scl_roll(108);
            w4vbl_done();
            grad_off();
            scl_off();
            blackout();
        }

        owe_deinit();

        resync();
        game_mode = 0;
        return 0;
        break;

        /*case ACTION_ADD:
           op1 = enextraw();
           op2 = enext();

           if(op1 < REGBASE)
              while(1);
           n = op1 - REGBASE;
           op1 = getreg(op1);
           regs[n] = op1 + op2;
           break;

        case ACTION_SUB:
           op1 = enextraw();
           op2 = enext();

           if(op1 < REGBASE)
              while(1);
           n = op1 - REGBASE;
           op1 = getreg(op1);
           regs[n] = op1 - op2;
           break;*/

    case ACTION_PARTYNOR:
        show_realparty = 0;
        sync_party();
        draw_dudes();
        break;

    case ACTION_PARTYARR:
        show_realparty = 1;
        sync_party();
        draw_dudes();
        break;

    case ACTION_REFORM:
        show_realparty = 0;
        // sync_party();
        // draw_dudes();
        pullparty = 1;
        load_party(ch[0].x >> 1, ch[0].y >> 1);
        sync_party();
        draw_dudes();
        break;

    case ACTION_GIVE:
        n = enext();
        get_item(n);
        break;

    case ACTION_SHOP:
    case ACTION_OPMENU:
        owe_off();

        optionmode = c == ACTION_OPMENU ? 1 : 0;
        f_do_shopmenu();

        // in case the party order was altered
        sync_party();

        owe_spr_reset();
        draw_dudes();

        owe_on();
        resync();
        break;

    case ACTION_SAVE:
        owe_off();

        start_gradient_slm();

        // predraw some stuff
        menu_drawbox(0, 0, 20, 3);
        reset_str(0);
        sl_borderloc = read_str();
        ppstr(1, 1, 52, 18, "Save Game");

        if (map_cur == 0)
            setdudepal();

        slm2_init();
        f_do_slmenu(1);
        slm_off();

        // in case the party order was altered
        sync_party();

        owe_spr_reset();
        draw_dudes();

        owe_on();
        do_delay(2);

        resync();
        break;

    case ACTION_BATTLE:
        n = enext();
        bflag_run = enext();
        bflag_lose = enext();
        bflag_sp_guy = enext();
        bflag_sp_hp = enext();

        evebattle = 1;
        battleresult = f_battlestart(n);
        evebattle = 0;
        if (battleresult == BATTLE_DEMOSKIP) {
            init_event(0x0027);
            break;
        }

        if (battleresult == BATTLE_LOSE)
            return 0;
        break;

    case ACTION_SETSKL:
    case ACTION_SETSKL2:
        n = enext();
        op1 = enext();
        op2 = enext();
        st = get_realparty(n);
        if (st) {
            st->skl[op1] = op2;
            if (c == ACTION_SETSKL) {
                f_load_skill(op2 - 1);
                get_skill(st);
            }
        }
        break;

    case ACTION_GIVEGEM:
        play_sfx(SFX_SEARCH);
        n = enext();
        get_item(gems_preorder[n] + GEMS_INDEX);
        break;

    case ACTION_SETFORM:
        for (n = 0; n != 4; ++n) {
            party[n] = realparty[n];
        }
        break;

    case ACTION_BOATON:
        boatmode();
        break;

    case ACTION_MAPSTATE:
        map_state = enext();
        f_map_restate();
        draw_dudes();
        set_camera();
        owe_redraw(sx, sy);
        break;

    default:
        pass_c = c;
        return f_get_action2();
    }

    return 1;
}

void get_actions()
{
    BYTE c;

    while (1) {
        c = get_action();
        if (c == 0 || c == -1) {
            break;
        }
    }
}

void eat_actions(BYTE num)
{
    BYTE n, n2;

    for (n = num; n < numactions; ++n) {
        for (n2 = 0; n2 < ACTIONSIZE; ++n2) {
            action_table[n][n2] = action_table[n + 1][n2];
        }
    }
    --numactions;
}

void cleanup_actions()
{
    BYTE n;

    for (n = 0; n < numactions; ++n) {
        if (action_table[n][0] == -1) {
            eat_actions(n);
        }
    }
}
