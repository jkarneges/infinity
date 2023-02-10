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

#include "defs.h"
#include "vars.h"

#define guy nextguy

extern UBYTE tu, tu2;
extern WORD tpx, tpy;
extern WORD tqueue[16][2];
extern BYTE tz, tv, tw;

// table stuff
BYTE *slurptab = (BYTE *)0xDC00;
BYTE (*panel_rel)[32][2] = (BYTE(*)[32][2])0xDC00;
UBYTE (*jolttab)[3] = (UBYTE(*)[3])0xDC00;

BYTE whirltab[4][3][3] = {
    {
     {-14, 0, 0},
     {-14, 16, 1},
     {2, 16, 2},
     },
    {
     {-2, 16, 6},
     {14, 0, 7},
     {14, 16, 8},
     },
    {
     {-2, -16, 3},
     {14, -16, 4},
     {14, 0, 5},
     },
    {
     {-14, -16, 9},
     {-14, 0, 10},
     {2, -16, 11},
     },
};

UBYTE whirltab2[4] = {18, 26, 30, 22};

extern UBYTE nextguy;
extern UBYTE curaction;

void baction_nextmore()
{
    UBYTE u, u2;
    UBYTE a1, a2;
    UBYTE d;
    BYTE s;
    WORD px2;

    switch (curaction) {

    case BACT_ARROW:

        f_bpro_next(0);
        px = bpro[0].rx - 3;
        py = bpro[0].ry - 3;
        switch (tu) {
        case 3:
            n = 0;
            break;
        case 0:
            n = 2;
            break;
        case 1:
            n = 3;
            break;
        case 2:
            n = 1;
            break;
        }
        bspr_8set(22, px, py, 32 + (n << 2), 0);
        bspr_8set(23, px + 8, py, 33 + (n << 2), 0);
        bfocus = guy;

        u = bch[guy].count;
        if (u == 1) {
            play_sfx(SFX_ARROWHIT);

            px = bch[guy].rx;
            py = bch[guy].ry;
            f_gemcheck(guy);
            f_text_init(px, py, bch[guy].damage, BDIR_UP);
            f_bresolve(guy);
        }
        break;

    case BACT_JOLT:

        if (bch[guy].count == 0) {
            bch[guy].count = 1;
        }
        if (bch[guy].progress != 13) {
            u = 12 - bch[guy].progress;
            if (u == 0)
                play_sfx(SFX_LIGHTNING);
            for (u2 = 0; u2 != 3; ++u2) {
                a1 = u2 << 1;
                if (!jolttab[u][u2]) {
                    bspr_8set(a1 + 22, 0, 0, -1, 0);
                    bspr_8set(a1 + 23, 0, 0, -1, 0);
                    continue;
                }
                a2 = jolttab[u][u2] - 1;
                a2 <<= 1;

                px = bch[guy].rx - 3;
                py = bch[guy].ry - 3;
                py -= 32 - (u2 << 4);
                bspr_8set(a1 + 22, px, py, a2 + 32, 3);
                bspr_8set(a1 + 23, px + 8, py, a2 + 33, 3);
            }
            if (bch[guy].progress == 1 && bch[guy].count == 1) {
                px = bch[guy].rx;
                py = bch[guy].ry;
                f_gemcheck(guy);
                f_text_init(px, py, bch[guy].damage, BDIR_UP);
                f_bresolve(guy);
            }
        }
        bfocus = guy;
        break;

    case BACT_TIDAL:

        u = 24 - bch[guy].count;
        if (u == 0)
            play_sfx(SFX_TIDAL);

        u2 = (u / 3) << 2;
        px = bch[guy].rx + 32 - (u << 1);
        py = bch[guy].ry - 11;
        bspr_8set(22, px, py, u2 + 32, 1);
        bspr_8set(23, px + 8, py, u2 + 33, 1);
        bspr_8set(24, px, py + 16, u2 + 34, 1);
        bspr_8set(25, px + 8, py + 16, u2 + 35, 1);
        bfocus = guy;

        if (bch[guy].count == 1) {
            px = bch[guy].rx;
            py = bch[guy].ry;
            f_gemcheck(guy);
            f_text_init(px, py, bch[guy].damage, BDIR_UP);
            f_bresolve(guy);
        }
        break;

    case BACT_WAKE:

        u = 64 - bch[guy].count;
        if (u == 0)
            play_sfx(SFX_REVIVE);
        if (u < 22)
            u2 = u & 1;
        else {
            u2 = ((u - 22) % 14) + 2;
        }
        u2 <<= 1;

        px = bch[guy].rx - 3;
        py = bch[guy].ry - 3;

        bspr_8set(22, px, py, u2 + 32, 5);
        bspr_8set(23, px + 8, py, u2 + 33, 5);
        bfocus = guy;

        break;

    case BACT_PANEL:

        u = 32 - bch[guy].count;
        if (u == 0) {
            if (tu2) {
                if (bch[guy].control == BCPU)
                    play_sfx(SFX_ENEMYCAST);
                else
                    play_sfx(SFX_SACRIFICE);
            }
        }

        for (u2 = 0; u2 != 6; ++u2) {
            px = bch[guy].rx + 1 + panel_rel[u2][u][0];
            py = bch[guy].ry - 7 + panel_rel[u2][u][1];
            if (u2 == 0)
                a1 = u & 3;
            if (u2 == 1)
                a1 = (u & 3) + 4;
            if (u2 == 2)
                a1 = (u & 3) + 8;
            if (u2 == 3)
                a1 = 3 - (u & 3);
            if (u2 == 4)
                a1 = 7 - (u & 3);
            if (u2 == 5)
                a1 = 11 - (u & 3);

            if (u2 == 3)
                py -= 6;
            bspr_8set(u2 + 22, px, py, 32 + a1, 5);
        }
        bch[guy].frame = 0;
        bfocus = guy;
        break;

    case BACT_SLURP:

        /*if(bch[guy].count == 0) {
                u = bch[guy].progress;
                if(u == 1) {
                        bch[guy].count = 40;
                        f_text_init(px, py, bch[guy].damage, BDIR_UP);
                        f_bresolve(guy);
                }
        }*/

        if (!(tu & 128)) {
            // if(bch[guy].progress == 2) {
            u = 52 - bch[guy].count;
            if (u == 0)
                play_sfx(SFX_SOULSLURP);

            if (u < 8) {
                u2 = u & 254;
            } else {
                u2 = ((u & 3) + 4) << 1;
            }
            if (u < 28)
                a1 = 0;
            else
                a1 = u - 28;

            if (u == 51) {
                // bch[guy].count = 40;
                f_text_init(px, py, bch[guy].damage, BDIR_UP);
                f_bresolve(guy);
            }

            px2 = px = bch[guy].rx - 3;
            py = bch[guy].ry - 3;
            bfocus = guy;
        } else {
            u = 40 - bch[guy].count;
            if (u < 8) {
                u2 = (u + 16) & 254;
                bfocus = guy;
            } else {
                u2 = (u & 2) + 24;
                bfocus = tu & 15;
            }

            a1 = u + 24;

            f_bpro_next(0);
            px2 = px = bpro[0].rx - 3;
            py = bpro[0].ry - 3;
        }
        px -= slurptab[a1];

        bspr_8set(22, px, py, u2 + 32, 4);
        bspr_8set(23, px + 8, py, u2 + 33, 4);

        px2 += slurptab[a1];

        bspr_8set(24, px2, py, u2 + 32, 4);
        bspr_8set(25, px2 + 8, py, u2 + 33, 4);

        break;

    case BACT_THROW:

        f_bpro_next(0);
        px = bpro[0].rx - 3;
        py = bpro[0].ry - 3;
        if (bch[guy].count & 1)
            tz += tv++;
        bspr_8set(22, px, py + tz, 32, tu);
        bspr_8set(23, px + 8, py + tz, 33, tu);
        if (bch[guy].count > 13)
            bfocus = turn;
        else
            bfocus = guy;

        break;

    case BACT_SPAWN:

        if (bch[guy].count == 0)
            bch[guy].count = 4;

        u = 5 - bch[guy].progress;

        if (u == 0)
            play_sfx(SFX_SPAWN);

        px = bch[guy].rx - 3;
        py = bch[guy].ry - 3;

        u2 = u << 1;
        bspr_8set(22, px, py, 32 + u2, tu);
        bspr_8set(23, px + 8, py, 33 + u2, tu);

        if (u == 4) {
            // revive him
            bch[guy].st.hp = bch[guy].st.hpmax;
            bch[guy].active = 1;
            bch[guy].poof = 0;
            bch[guy].frame = 0;
            bch[guy].recover = getcount(bch[guy].st.spd, 3) / 2;
            bch[guy].points = 0;
            bch[guy].damage = 0;
        }

        bfocus = guy;
        break;

    case BACT_CHARM:

        u = 67 - bch[guy].count;

        if (u == 0)
            play_sfx(SFX_CHARM);
        if (u < 48)
            f_bpro_next(0);
        if (u == 66) {
            bch[guy].ail = BAIL_CHARM;
            bch[guy].ailtime = f_bgetrecover(&bch[guy]) * 4;
        }

        px = bpro[0].rx - 3;
        py = bpro[0].ry - 3 + tw;
        tw += tz;
        tz += tv;
        if (tz == 3 || tz == -3)
            tv = -tv;

        for (u2 = 0; u2 != 18; ++u2) {
            tqueue[u2][0] = tqueue[u2 + 1][0];
            tqueue[u2][1] = tqueue[u2 + 1][1];
        }
        tqueue[18][0] = px;
        tqueue[18][1] = py;

        bspr_8set(22, px, py, 44, 0);
        bspr_8set(23, px + 8, py, 45, 0);

        px = tqueue[9][0];
        py = tqueue[9][1];
        bspr_8set(24, px, py, 44, 0);
        bspr_8set(25, px + 8, py, 45, 0);

        px = tqueue[0][0];
        py = tqueue[0][1];
        bspr_8set(26, px, py, 44, 0);
        bspr_8set(27, px + 8, py, 45, 0);

        bfocus = guy;
        break;

    case BACT_EFFECT:

        bch[guy].ail = tu;
        bch[guy].ailtime = f_bgetrecover(&bch[guy]) * 4;

        if (tu == BAIL_WOLF)
            bch[guy].type = VICTOR_WOLF;
        break;

    case BACT_SLASH:

        u = 56 - bch[guy].count;
        u2 = u & 1;
        u2 <<= 2;

        f_bpro_next(0);

        px = bpro[0].rx - 3 - 8;
        py = bpro[0].ry - 3;

        bspr_8set(22, px, py, u2 + 32, 4);
        bspr_8set(23, px + 8, py, u2 + 33, 4);
        bspr_8set(24, px + 16, py, u2 + 34, 4);
        bspr_8set(25, px + 24, py, u2 + 35, 4);

        bfocus = guy;
        break;

    case BACT_WHIRL:

        u = 32 - bch[guy].count;
        u2 = (u >> 1) & 0x03;

        px = bch[guy].rx - 3;
        py = bch[guy].ry - 4;
        if (u2 == 2)
            py += 2;
        bch[guy].frame = whirltab2[u2];

        for (a1 = 0; a1 < 3; ++a1) {
            tpx = px + whirltab[u2][a1][0];
            tpy = py + whirltab[u2][a1][1];
            a2 = (whirltab[u2][a1][2] << 1) + 32;
            bspr_8set(22 + (a1 << 1), tpx, tpy, a2, 4);
            bspr_8set(23 + (a1 << 1), tpx + 8, tpy, a2 + 1, 4);
        }

        bfocus = guy;

        break;

    case BACT_DAMAGE:

        u = 32 - bch[guy].count;

        // deal damage on first frame
        if (u == 0) {
            px = bch[guy].rx;
            py = bch[guy].ry;
            f_gemcheck(guy);
            f_text_init(px, py, bch[guy].damage, BDIR_UP);
            f_bresolve(guy);
        }

        bfocus = guy;

        break;
    }
}

void stats_recalc(struct DUDESTAT *guy);

void reset_data()
{
    UBYTE n, n2;
    struct DUDESTAT *st;

    set_24(gold, 0);

    // Connor
    st = &stats[0];
    strcpy(st->name, "Connor");
    strcpy(st->class, "Hero");
    st->type = 0;
    st->level = 1;
    set_24(st->exp, 0);
    st->hpmax = 40;
    st->spmax = 20;
    st->str = 6;
    st->end = 4;
    st->agl = 5;
    st->wis = 7;
    st->e[0] = 0; // (he is given conscript in event code)
    st->e[1] = IT_HTunic;
    st->e[2] = IT_ICloak;
    st->e[3] = 0;
    st->skl[0] = 1; // Engulf
    st->skl[1] = 0;
    st->skl[2] = 0;
    st->skl[3] = 0;
    stats_recalc(st);
    st->hp = st->hpmax;
    st->sp = st->spmax;

    // Ryan
    st = &stats[1];
    strcpy(st->name, "Ryan");
    strcpy(st->class, "Guide");
    st->type = 1;
    st->level = 1;
    set_24(st->exp, 0);
    st->hpmax = 30;
    st->spmax = 13;
    st->str = 5;
    st->end = 3;
    st->agl = 8;
    st->wis = 6;
    st->e[0] = IT_DBasic;
    st->e[1] = IT_HLeather;
    st->e[2] = 0;
    st->e[3] = 0;
    st->skl[0] = 16; // Longshot
    st->skl[1] = 3;  // Tidal
    st->skl[2] = 0;
    st->skl[3] = 0;
    stats_recalc(st);
    st->hp = st->hpmax;
    st->sp = st->spmax;

    // Elya
    st = &stats[2];
    strcpy(st->name, "Elya");
    strcpy(st->class, "Wizard");
    st->type = 2;
    st->level = 1;
    set_24(st->exp, 0);
    st->hpmax = 22;
    st->spmax = 30;
    st->str = 3;
    st->end = 3;
    st->agl = 2;
    st->wis = 10;
    st->e[0] = IT_EOak;
    st->e[1] = IT_PApprentice;
    st->e[2] = IT_KWizard;
    st->e[3] = 0;
    st->skl[0] = 8; // Embrace
    st->skl[1] = 3; // Tidal
    st->skl[2] = 0;
    st->skl[3] = 0;
    stats_recalc(st);
    st->hp = st->hpmax;
    st->sp = st->spmax;

    // Victor
    st = &stats[3];
    strcpy(st->name, "Victor");
    strcpy(st->class, "Wolf");
    st->type = VICTOR_WOLF;
    st->level = 1;
    set_24(st->exp, 0);
    st->wolfpow = 0;
    st->hpmax = 70;
    st->spmax = 10;
    st->str = 8;
    st->end = 5;
    st->agl = 9;
    st->wis = 3;
    st->e[0] = IT_FWolf;
    st->e[1] = IT_HWolf;
    st->e[2] = 0;
    st->e[3] = 0;
    st->skl[0] = 15; // Leap
    st->skl[1] = 0;
    st->skl[2] = 0;
    st->skl[3] = 0;
    stats_recalc(st);
    st->hp = st->hpmax;
    st->sp = st->spmax;

    // Roland
    st = &stats[4];
    strcpy(st->name, "Roland");
    strcpy(st->class, "Knight");
    st->type = 4;
    st->level = 1;
    set_24(st->exp, 0);
    st->hpmax = 50;
    st->spmax = 10;
    st->str = 9;
    st->end = 7;
    st->agl = 3;
    st->wis = 4;
    st->e[0] = IT_OBastard;
    st->e[1] = IT_HPlate;
    st->e[2] = 0;
    st->e[3] = 0;
    st->skl[0] = 2;  // jolt
    st->skl[1] = 19; // slash
    st->skl[2] = 0;
    st->skl[3] = 0;
    stats_recalc(st);
    st->hp = st->hpmax;
    st->sp = st->spmax;

    // Alutha
    st = &stats[5];
    strcpy(st->name, "Alutha");
    strcpy(st->class, "Hero");
    st->type = ALUTHA;
    st->level = 1;
    set_24(st->exp, 0);
    st->hpmax = 80;
    st->spmax = 20;
    st->str = 9;
    st->end = 7;
    st->agl = 4;
    st->wis = 6;
    st->e[0] = 0;
    st->e[1] = 0;
    st->e[2] = 0;
    st->e[3] = 0;
    st->skl[0] = 0;
    st->skl[1] = 0;
    st->skl[2] = 0;
    st->skl[3] = 0;
    stats_recalc(st);
    st->hp = st->hpmax;
    st->sp = st->spmax;

    party[0] = 0;
    party[1] = 0;
    party[2] = 0;
    party[3] = 0;
    area = 0;
    lastsaveslot = 3;
    for (n = 0; n != 96; ++n)
        vars[n] = 0;
    for (n = 0; n != 8; ++n)
        regs[n] = 0;
    for (n = 0; n != 32; ++n)
        flags[n] = 0;
    disable_interrupts();
    gametime[0] = 0;
    gametime[1] = 0;
    gametime[2] = 0;
    gametime[3] = 0;
    enable_interrupts();

    options[0] = 0;
    memcpy(options + 1, defaultcolors, 7);

    boat_x = 0;
    boat_y = 0;
    boat_dir = 0;
    on_boat = 0;
}

UBYTE *fileptr;
static void write(void *block, UBYTE size)
{
    memcpy(fileptr, block, (UWORD)size);
    fileptr += size;
}

static void read(void *block, UBYTE size)
{
    memcpy(block, fileptr, (UWORD)size);
    fileptr += size;
}

UBYTE ss_active;
UBYTE ss_level, ss_area, ss_hour, ss_min;
UBYTE ss_num;
UBYTE ss_type[4];
UBYTE ss_options[7];
UBYTE savesig[8];
char slsig[8] = "BONJAHL";

void load_saveinfo(UBYTE slot)
{
    UBYTE lastpage;
    UBYTE gametime[4];

    fopen(slot);
    lastpage = ram_page(5);
    fileptr = (UBYTE *)0xD800;

    read(savesig, 7);
    savesig[7] = 0;
    if (strcmp((UBYTE *)savesig, slsig)) {
        ss_active = 0;
        ram_page(lastpage);
        return;
    }
    ss_active = 1;

    read(&ss_num, 1);
    if (ss_num > 4)
        ss_num = 4;
    read(&ss_type, 4);
    read(&ss_level, 1);
    read(&ss_area, 1);
    read(gametime, 4);
    read(ss_options, 7);

    ss_hour = gametime[0];
    ss_min = gametime[1];

    ram_page(lastpage);
}

void savegame(UBYTE slot)
{
    UBYTE a[4];
    UBYTE n, at;
    UBYTE lastpage;
    UBYTE level;

    area = regs[0];

    fopen(slot);
    lastpage = ram_page(5);
    fileptr = (UBYTE *)0xD800;

    write(slsig, 7);
    for (at = 0, n = 0; n != 4; ++n) {
        st = get_realparty(n);
        if (st) {
            if (at == 0)
                level = st->level;
            a[at++] = st->type;
        }
    }
    write(&at, 1);
    write(a, 4);
    write(&level, 1);
    write(&area, 1);
    disable_interrupts();
    a[0] = gametime[0];
    a[1] = gametime[1];
    a[2] = gametime[2];
    a[3] = gametime[3];
    enable_interrupts();
    write(a, 4);
    write(options, 7);
    write(&lastsaveslot, 1);
    write(party, 4);
    write(realparty, 4);
    for (n = 0; n != 6; ++n)
        write(&stats[n], sizeof(struct DUDESTAT));
    write(vars, 96);
    write(flags, 32);
    write(regs, 8);
    write(item_list_type, 100);
    write(item_list_num, 100);
    write(gems_list, 9);
    write(gems_charge, 9);
    write(gold, 3);
    write(&boat_x, 1);
    write(&boat_y, 1);
    write(&boat_dir, 1);

    ram_page(lastpage);

    fclose(slot);
}

void loadgame(UBYTE slot)
{
    UBYTE a[4];
    UBYTE n, at;
    UBYTE lastpage;
    UBYTE level;
    UBYTE c;

    fopen(slot);
    lastpage = ram_page(5);
    fileptr = (UBYTE *)0xD800;

    read(slsig, 7);
    read(&at, 1);
    read(a, 4);
    read(&level, 1);
    read(&area, 1);
    read(a, 4);
    disable_interrupts();
    gametime[0] = a[0];
    gametime[1] = a[1];
    gametime[2] = a[2];
    gametime[3] = a[3];
    enable_interrupts();
    read(options, 7);
    read(&lastsaveslot, 1);
    read(party, 4);
    read(realparty, 4);
    for (n = 0; n != 6; ++n)
        read(&stats[n], sizeof(struct DUDESTAT));
    read(vars, 96);
    read(flags, 32);
    //   for(n = 0; n != 32; ++n)
    //      flags[n] = 0;
    read(regs, 8);
    read(item_list_type, 100);
    read(item_list_num, 100);
    read(gems_list, 9);
    read(gems_charge, 9);
    read(gold, 3);
    read(&boat_x, 1);
    read(&boat_y, 1);
    read(&boat_dir, 1);

    ram_page(lastpage);

    fclose(slot);
}

void stats_recalc(struct DUDESTAT *guy)
{
    UBYTE n;
    BYTE att, def, spd, man;

    att = 1;
    def = 1;
    spd = 0;
    man = 0;

    // calculate speed
    spd += guy->agl;

    for (n = 0; n != 3; ++n) {
        att += item_info2(guy->e[n])[1];
        def += item_info2(guy->e[n])[2];
        spd += item_info2(guy->e[n])[3];
        man += item_info2(guy->e[n])[4];
    }

    if (guy->type == VICTOR_WOLF) {
        n = guy->wolfpow * 3;
        att += n;
        def += n;
    }

    if (att < 0)
        att = 0;
    if (def < 0)
        def = 0;
    if (spd < 0)
        spd = 0;
    if (man < 0)
        man = 0;

    guy->att = att;
    guy->def = def;
    guy->spd = spd;
    guy->man = man;
}
