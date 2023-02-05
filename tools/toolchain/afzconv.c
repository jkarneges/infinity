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

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h> // isalnum
#include"enemy.h"

int base_enemy = 0;
int base_map   = 180;

int errors = 0;

struct ENEMYREF
{
	char *name;
	int index;
	int exp, au, item;
	char ai[16];
	int level, hp, sp;
	int str, end, agl, wis;
	int att, def, spd, mgd;
	char fire, water, wind, weapon;
};

struct ZONEREF
{
	int num;
	int index;
};

struct ENEMYREF *enemyref = NULL;
int enemyref_num = 0;

struct ZONEREF *zoneref = NULL;
int zoneref_num = 0;

static char* getToken(char** str, const char* delims)
{
    char* token;

    if (*str==NULL) {
        /* No more tokens */
        return NULL;
    }

    token=*str;
    while (**str!='\0') {
        if (strchr(delims,**str)!=NULL) {
            **str='\0';
           (*str)++;
            return token;
        }
        (*str)++;
    }
    /* There is no other token */
    *str=NULL;
    return token;
}

int findref_enemy(char *name)
{
	int n;

	for(n = 0; n < enemyref_num; ++n) {
		if(!strcmp(enemyref[n].name, name))
			return n;
	}
	return -1;
}

int xfindref_enemy(int x)
{
	int n;

	for(n = 0; n < enemyref_num; ++n) {
		if(enemyref[n].index == x)
			return n;
	}
	return -1;
}

int xref_zone(int x)
{
	int n;

	for(n = 0; n < zoneref_num; ++n) {
		if(zoneref[n].num == x)
			return zoneref[n].index;
	}
	return -1;
}

int xrevref_zone(int x)
{
	int n;

	for(n = 0; n < zoneref_num; ++n) {
		if(zoneref[n].index == x)
			return zoneref[n].num;
	}
	return -1;
}

void addref_enemy(int index, char *name, int level, int hp, int sp, int str, int end, int agl, int wis, int att, int def, int spd, int mgd, char fire, char water, char wind, char weapon, int exp, int au, int item, char *ai)
{
	if(index >= 150) {
		printf("ERROR: [%s] is set out of range (%d).\n", name, index);
		exit(1);
	}

	enemyref = realloc(enemyref, sizeof(struct ENEMYREF) * (enemyref_num+1));
	enemyref[enemyref_num].name = strdup(name);
	enemyref[enemyref_num].index = index;
	enemyref[enemyref_num].exp   = exp;
	enemyref[enemyref_num].au    = au;
	enemyref[enemyref_num].item  = item;

	enemyref[enemyref_num].level  = level;
	enemyref[enemyref_num].hp     = hp;
	enemyref[enemyref_num].sp     = sp;
	enemyref[enemyref_num].str    = str;
	enemyref[enemyref_num].end    = end;
	enemyref[enemyref_num].agl    = agl;
	enemyref[enemyref_num].wis    = wis;
	enemyref[enemyref_num].att    = att;
	enemyref[enemyref_num].def    = def;
	enemyref[enemyref_num].spd    = spd;
	enemyref[enemyref_num].mgd    = mgd;
	enemyref[enemyref_num].fire   = fire;
	enemyref[enemyref_num].water  = water;
	enemyref[enemyref_num].wind   = wind;
	enemyref[enemyref_num].weapon = weapon;

	memcpy(enemyref[enemyref_num].ai, ai, 16);

	++enemyref_num;
}

void addref_zone(int num, int index)
{
	zoneref = realloc(zoneref, sizeof(struct ZONEREF) * (zoneref_num+1));
	zoneref[zoneref_num].num   = num;
	zoneref[zoneref_num].index = index;

	++zoneref_num;
}

char elem_char(char value) {
	switch(value) {
		case 0: return 'W';
		case 1: return '-';
		case 2: return 'S';
		default: return '-';
	}
}

char from_elem_char(char ch) {
	switch(ch) {
		case 'W': return 0;
		case '-': return 1;
		case 'S': return 2;
		default: return 1;
	}
}

int getnameindex(char *inbuf, int *index, char *name, char **remainder)
{
	char *p;
	int a_index;
	char a_name[256];
	char *left;

	p = getToken(&inbuf, ":");
	if(!p)
		return 0;
	a_index = atoi(p);
	if(!inbuf)
		return 0;
	char *next = strchr(inbuf, ':');
	if(!next)
		return 0;
	*next = '\0';
	p = inbuf;
	left = next + 1;
	strcpy(a_name, p);

	*index = a_index;
	strcpy(name, a_name);
	*remainder = left;

	return 1;
}

int readrefs_enemy(char *fname)
{
	FILE *f;
	char *p, *p2;
	char name[256];
	int index, exp, au, item;
	char line[256];
	char ai[16];
	int n;

	f = fopen(fname, "r");
	if(!f)
		return 0;
	while(1) {
		fgets(line, 255, f);
		if(feof(f))
			break;

		int line_len = strlen(line);
		if(line_len > 0 && line[line_len-1] == '\n') {
			line[line_len-1] = 0;
			--line_len;
		}
		if(line_len > 0 && line[line_len-1] == '\r') {
			line[line_len-1] = 0;
			--line_len;
		}

		p = strchr(line, '#');
		if(p)
			*p = 0;

		if(strlen(line) == 0)
			continue;

		char *remainder = NULL;
		if(!getnameindex(line, &index, name, &remainder) || remainder == NULL)
			return 0;

		if(strlen(remainder) > 0 && remainder[0] == ' ') {
			p = remainder;

			p2 = strtok(p, " \t\r\n"); if(!p2) return 0;
			int level = atoi(p2);
			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			int hp = atoi(p2);
			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			int sp = atoi(p2);

			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			int str = atoi(p2);
			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			int end = atoi(p2);
			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			int agl = atoi(p2);
			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			int wis = atoi(p2);

			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			int att = atoi(p2);
			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			int def = atoi(p2);
			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			int spd = atoi(p2);
			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			int mgd = atoi(p2);

			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			if(strlen(p2) != 4)
				return 0;
			char fire = from_elem_char(p2[0]);
			char water = from_elem_char(p2[1]);
			char wind = from_elem_char(p2[2]);
			char weapon = from_elem_char(p2[3]);

			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			int exp = atoi(p2);
			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			int au = atoi(p2);
			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			int item = 0;
			if(strcmp(p2, "-") != 0)
				item = atoi(p2);

			char ai[16];
			memset(ai, 0, 16);

			p2 = strtok(NULL, " \t\r\n"); if(!p2) return 0;
			if(strcmp(p2, "-") != 0) {
				p = p2;
				for(n = 15; n >= 0; --n) {
					p2 = getToken(&p, ",:\r\n");
					if(!p2)
						continue;
					ai[n] = atoi(p2);
				}
			}

			addref_enemy(index, name, level, hp, sp, str, end, agl, wis, att, def, spd, mgd, fire, water, wind, weapon, exp, au, item, ai);
		}
		else {
			p = remainder;

			p2 = getToken(&p, ",:\r\n");
			if(!p2)
				return 0;
			exp = atoi(p2);
			p2 = getToken(&p, ",:\r\n");
			if(!p2)
				return 0;
			au = atoi(p2);
			p2 = getToken(&p, ",:\r\n");
			if(!p2)
				return 0;
			item = atoi(p2);

			memset(ai, 0, 16);
			// check for ai
			for(n = 15; n >= 0; --n) {
				p2 = getToken(&p, ",:\r\n");
				if(!p2)
					continue;
				ai[n] = atoi(p2);
			}

			addref_enemy(index, name, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, exp, au, item, ai);
		}
	}
	fclose(f);

	return 1;
}

int readrefs_zone(char *fname)
{
	FILE *f;
	char *p, *p2;
	int num, index;
	char line[256];

	f = fopen(fname, "r");
	if(!f)
		return 0;
	while(1) {
		fgets(line, 255, f);
		if(feof(f))
			break;
		p = line;
		p2 = getToken(&p, ",:\r\n");
		if(!p2)
			continue;
		num = atoi(p2);
		p2 = getToken(&p, ",:\r\n");
		if(!p2)
			continue;
		index = atoi(p2);

		addref_zone(num, index);
	}
	fclose(f);

	return 1;
}

char *zonestr(int x, struct ZONES *z)
{
	int n, at, num;

	at = 0;
	for(n = 0; n < z->num; ++n) {
		num = z->ptr[n].numforms;
		if(at + num > x) {
			return z->ptr[n].name;
		}
		at += num;
	}

	return NULL;
}

struct FORM_STRUCT *findform(int x, struct ZONES *z)
{
	int n, at, num;
	struct FORM_STRUCT *frm_ptr;

	at = 0;
	for(n = 0; n < z->num; ++n) {
		num = z->ptr[n].numforms;
		if(at + num > x) {
			frm_ptr = &z->ptr[n].ptr[x - at];
			return frm_ptr;
		}
		at += num;
	}

	return NULL;
}

int xfindform(int x, int zone, struct ZONES *z)
{
	int n, at, num;

	at = 0;
	for(n = 0; n < z->num; ++n) {
		num = z->ptr[n].numforms;
		if(n == zone) {
			return at + x;
		}
		at += num;
	}

	return 0;
}

void fix(char *s)
{
	int n;

	for(n = 0; n < strlen(s); ++n) {
		if(!isalnum(s[n]))
			s[n] = '_';
	}
}

int genformdefs(char *fname, struct ZONES *z)
{
	FILE *f;
	int n, n2, at, num;
	char zone[256], form[256];

	f = fopen(fname, "w");
	if(!f)
		return 0;
	at = 0;
	for(n = 0; n < z->num; ++n) {
		num = z->ptr[n].numforms;
		strcpy(zone, z->ptr[n].name);
		fix(zone);
		for(n2 = 0; n2 < num; ++n2) {
			strcpy(form, z->ptr[n].ptr[n2].name);
			fix(form);
			fprintf(f, "#define %s_%s %d\n", zone, form, at);
			++at;
		}
	}
	fclose(f);

	return 1;
}

int main(int argc, char *argv[])
{
	struct FORM_STRUCT form, *formp;
	struct ENEMY_STRUCT en;
	struct ZONE_STRUCT zone;
	struct ENEMIES e;
	struct ZONES z;
	FILE *f;
	char sig[16];
	int n, n2, x, y;
	int at;
	int num, exp, au;
	FILE *out;

	if(argc < 2) {
		printf("usage: afzconv <.afz file> [cmd] [arg1] [arg2]\n");
		return 1;
	}

	enemy_reset(&e);
	zones_reset(&z);

	f = fopen(argv[1], "rb");
	if(!f) {
		printf("ERROR: Couldn't open [%s]\n", argv[1]);
		return 1;
	}

	int zones_only = 0;

	fread(sig, 3, 1, f);
	if(!strncmp(sig, "AFZ", 3)) {
		// nothing to do
	}
	else if(!strncmp(sig, "AF2", 3)) {
		zones_only = 1;
	}
	else {
		printf("ERROR: [%s] has an invalid format.\n", argv[1]);
		fclose(f);
		return 1;
	}

	if(!zones_only) {
		// read enemies
		fread(&x, 4, 1, f);
		printf("Enemies: %d\n", x);
		for(n = 0; n < x; ++n) {
			fread(&en, sizeof(struct ENEMY_STRUCT) - 12 - 16, 1, f);
			enemy_add(&e, &en);
		}
	}

	// read zones
	fread(&x, 4, 1, f);
	printf("Zones: %d\n", x);
	for(n = 0; n < x; ++n) {
		fread(&zone, ZONE_STRUCT_SIZE, 1, f);
		y = zone.numforms;
		zone.numforms = 0;
		zone.ptr = NULL;

		// read formations
		printf("Zone [%s]:\n", zone.name);
		printf("  Formations: %d\n", y);
		for(n2 = 0; n2 < y; ++n2) {
			fread(&form, sizeof(struct FORM_STRUCT), 1, f);
			printf("    Form [%s]\n", form.name);
			form_add(&zone, &form);
		}

		zones_add(&z, &zone);
	}
	fread(sig, 3, 1, f);
	if((!zones_only && strncmp(sig, "AFZ", 3)) || (zones_only && strncmp(sig, "AF2", 3))) {
		printf("ERROR: [%s] has an invalid format.\n", argv[1]);
		fclose(f);
		return 1;
	}
	fclose(f);

	if(!readrefs_enemy("enemies.ref")) {
		printf("ERROR: trying to read enemies.ref\n");
		return 1;
	}
	if(!readrefs_zone("zones.ref")) {
		printf("ERROR: trying to read zones.ref\n");
		return 1;
	}

	printf("read %d enemy references.\n", enemyref_num);
	for(n = 0; n < enemyref_num; ++n) {
		printf("[%d] = [%s]\n", enemyref[n].index, enemyref[n].name);
	}

	printf("read %d zone references.\n", zoneref_num);
	for(n = 0; n < zoneref_num; ++n) {
		printf("[zone%d] -> [%d]\n", zoneref[n].num, zoneref[n].index);
	}

	if(!zones_only) {
		// verify enemy references
		for(n = 0; n < e.num; ++n) {
			x = findref_enemy(e.ptr[n].name);
			if(x == -1) {
				printf("ERROR: no reference for enemy %d: [%s]\n", n, e.ptr[n].name);
				return 1;
			}

			e.ptr[n].exp  = enemyref[x].exp;
			e.ptr[n].au   = enemyref[x].au;
			e.ptr[n].item = enemyref[x].item;
			memcpy(e.ptr[n].ai, enemyref[x].ai, 16);
			printf("[%s]->[%s]\n", e.ptr[n].name, enemyref[x].name);
		}
	}

	// verify zone references
	for(n = 0; n < z.num; ++n) {
		x = xref_zone(n+1);
		if(x == -1) {
			printf("ERROR: no reference for zone:%d\n", n+1);
			return 1;
		}
	}

	printf("done.\n");

	if(argc > 2) {
		if(!strcmp(argv[2], "renzone")) {
			for(n = 0; n < z.num; ++n) {
				if(!strcmp(z.ptr[n].name, argv[3]))
					break;
			}
			if(n >= z.num) {
				printf("no such zone: %s\n", argv[3]);
				return 1;
			}

			// rename the zone
			strcpy(z.ptr[n].name, argv[4]);
			printf("Zone is now %d:%s\n", n+1, z.ptr[n].name);
		}
		else if(!strcmp(argv[2], "swapzone")) {
			for(n = 0; n < z.num; ++n) {
                                if(!strcmp(z.ptr[n].name, argv[3]))
                                        break;
                        }
                        if(n >= z.num) {
                                printf("no such zone: %s\n", argv[3]);
                                return 1;
                        }

			for(n2= 0; n2 < z.num; ++n2) {
                                if(!strcmp(z.ptr[n2].name, argv[4]))
                                        break;
                        }
                        if(n2 >= z.num) {
                                printf("no such zone: %s\n", argv[4]);
                                return 1;
                        }
 
                        // swap the zones
			zone = z.ptr[n];
			z.ptr[n] = z.ptr[n2];
			z.ptr[n2] = zone;
			
                        printf("Zones are now swapped.\n");
		}
		else if(!strcmp(argv[2], "renform") || !strcmp(argv[2], "delform")) {
			for(n = 0; n < z.num; ++n) {
				if(!strcmp(z.ptr[n].name, argv[3]))
					break;
			}
			if(n >= z.num) {
				printf("no such zone: %s\n", argv[3]);
				return 1;
			}

			for(n2 = 0; n2 < z.ptr[n].numforms; ++n2) {
				if(!strcmp(z.ptr[n].ptr[n2].name, argv[4]))
					break;
			}
			if(n2 >= z.ptr[n].numforms) {
				printf("no such form: %s\n", argv[4]);
				return 1;
			}

			if(!strcmp(argv[2], "renform")) {
				// rename the formation
				strcpy(z.ptr[n].ptr[n2].name, argv[5]);
				printf("Formation is now %d:%s:%s\n", n+1, z.ptr[n].name, z.ptr[n].ptr[n2].name);
			}
			else {
				// delete the formation
				printf("Formation %d:%s:%s deleted.\n", n+1, z.ptr[n].name, z.ptr[n].ptr[n2].name);
				form_del(&z.ptr[n], n2);
			}
		}
		else if(!strcmp(argv[2], "addenemy")) {
			if(zones_only) {
				printf("ERROR: cannot add enemy to zones-only file format\n");
				return 1;
			}

			if(argc < 19) {
				printf("usage: afzconv <.afz file> addenemy <name> <level> <hp> <sp> <str> <end> <agl> <wis> <att> <def> <spd> <mgd> <fire> <water> <wind> <weapon>\n");
				return 1;
			}

			struct ENEMY_STRUCT en;
			strcpy(en.name, argv[3]);
			en.level = atoi(argv[4]);
			en.hp = atoi(argv[5]);
			en.sp = atoi(argv[6]);
			en.str = atoi(argv[7]);
			en.end = atoi(argv[8]);
			en.agl = atoi(argv[9]);
			en.wis = atoi(argv[10]);
			en.att = atoi(argv[11]);
			en.def = atoi(argv[12]);
			en.spd = atoi(argv[13]);
			en.mgd = atoi(argv[14]);
			en.fire = atoi(argv[15]);
			en.water = atoi(argv[16]);
			en.wind = atoi(argv[17]);
			en.weapon = atoi(argv[18]);

			enemy_add(&e, &en);
		}
		else if(!strcmp(argv[2], "exportenemies")) {
			if(zones_only) {
				printf("ERROR: cannot export enemies from zones-only file format\n");
				return 1;
			}

			FILE *out = fopen("enemies_out.ref", "w");
			fprintf(out, "#############################################################################\n");
			fprintf(out, "# Infinity Enemies\n");
			fprintf(out, "#############################################################################\n");
			fprintf(out, "#\n");
			fprintf(out, "# All lines beginning with '#' are ignored by the program.\n");
			fprintf(out, "#\n");
			fprintf(out, "# Format:\n");
			fprintf(out, "#\n");
			fprintf(out, "# X:Name: Lvl HP SP Str End Agl Wis Att Def Spd Mgd FWAP Exp Au Item AI\n");
			fprintf(out, "#\n");
			fprintf(out, "#  X    = enemy number reference\n");
			fprintf(out, "#  Name = name of enemy\n");
			fprintf(out, "#\n");
			fprintf(out, "#  FWAP = elemental attributes for fire (F), water (W), wind/air (A),\n");
			fprintf(out, "#         weapon/phys (P), as a 4-character string.\n");
			fprintf(out, "#         W=weak, S=strong, -=normal.\n");
			fprintf(out, "#         Ex: W--S = weak against fire, strong against weapons.\n");
			fprintf(out, "#\n");
			fprintf(out, "#  Item = item to drop or '-'\n");
			fprintf(out, "#\n");
			fprintf(out, "#  AI   = optional comma-separated list of up to 16 action numbers, or '-'\n");
			fprintf(out, "#\n");
			fprintf(out, "#############################################################################\n");
			fprintf(out, "\n");

			fprintf(out, "# Name           Lvl   HP   SP Str End Agl Wis Att Def Spd Mgd FWAP  Exp   Au Itm AI\n");
			fprintf(out, "# ---------------------------------------------------------------------------------------------------\n");
			for(n = 0; n < 150; ++n) {
				x = xfindref_enemy(n);
				if(x == -1)
					continue;

				struct ENEMYREF *er = &enemyref[x];

				// match the enemy
				for(n2 = 0; n2 < e.num; ++n2) {
					if(!strcmp(enemyref[x].name, e.ptr[n2].name))
						break;
				}
				if(n2 >= e.num) {
					printf("ERROR: no enemy named:%s\n", enemyref[x].name);
					return 1;
				}

				struct ENEMY_STRUCT *en = &e.ptr[n2];

				char idstr[256];
				sprintf(idstr, "%d:%s:", er->index, en->name);

				char elemstr[5];
				strcpy(elemstr, "----");
				elemstr[0] = elem_char(en->fire);
				elemstr[1] = elem_char(en->water);
				elemstr[2] = elem_char(en->wind);
				elemstr[3] = elem_char(en->weapon);

				char itemstr[256];
				if(en->item)
					sprintf(itemstr, "%d", en->item);
				else
					strcpy(itemstr, "-");

				char aistr[256];
				int pos = 0;
				for(int k = 15; k >= 0; --k) {
					if(en->ai[k] > 0) {
						if(pos > 0) {
							aistr[pos] = ',';
							++pos;
						}

						pos += sprintf(aistr + pos, "%d", en->ai[k]);
					}
				}
				aistr[pos] = 0;
				if(pos == 0)
					strcpy(aistr, "-");

				fprintf(out, "%-17s %2d %4d %4d %3d %3d %3d %3d %3d %3d %3d %3d %s %4d %4d %3s %s\n", idstr, en->level, en->hp, en->sp, en->str, en->end, en->agl, en->wis, en->att, en->def, en->spd, en->mgd, elemstr, en->exp, en->au, itemstr, aistr);
			}
			fclose(out);
			printf("wrote enemies_out.ref\n");
			return 0;
		}
		else if(!strcmp(argv[2], "converttozonesonly")) {
			if(zones_only) {
				printf("ERROR: file format is already zones-only\n");
				return 1;
			}

			zones_only = 1;

			// use enemies.ref indexes instead of afz indexes
			for(n = 0; n < z.num; ++n) {
				for(int k = 0; k < z.ptr[n].numforms; ++k) {
					struct FORM_STRUCT *formp = &z.ptr[n].ptr[k];

					for(int j = 4; j < 12; ++j) {
						if(formp->guy_active[j]) {
							int s = formp->guy_type[j]-1; // get afz index
							x = findref_enemy(e.ptr[s].name); // find enemies.ref entry by name
							if(x == -1) {
								printf("ERROR: no reference for enemy %d: [%s]\n", s, e.ptr[s].name);
								return 1;
							}
							formp->guy_type[j] = enemyref[x].index + 1; // set enemies.ref index
						}
					}
				}
			}
		}
		else {
			printf("no such command: [%s]\n", argv[2]);
		}

		// save back, and exit
		printf("Press enter to save.\n");
		getchar();


		f = fopen("result.afz", "wb");
 
		if(zones_only) {
			// write signature
			fwrite("AF2", 3, 1, f);

			// write zones
			fwrite(&z.num, 4, 1, f);
			for(n = 0; n < z.num; ++n) {
					fwrite(&z.ptr[n], ZONE_STRUCT_SIZE, 1, f);
					for(n2 = 0; n2 < z.ptr[n].numforms; ++n2) {
							fwrite(&z.ptr[n].ptr[n2], sizeof(struct FORM_STRUCT), 1, f);
					}
			}

			fwrite("AF2", 3, 1, f);
			fclose(f);
		}
		else {
			// write signature
			fwrite("AFZ", 3, 1, f);

			// write enemies
			fwrite(&e.num, 4, 1, f);
			for(n = 0; n < e.num; ++n) {
					fwrite(&e.ptr[n], sizeof(struct ENEMY_STRUCT) - 12 - 16, 1, f);
			}

			// write zones
			fwrite(&z.num, 4, 1, f);
			for(n = 0; n < z.num; ++n) {
					fwrite(&z.ptr[n], ZONE_STRUCT_SIZE, 1, f);
					for(n2 = 0; n2 < z.ptr[n].numforms; ++n2) {
							fwrite(&z.ptr[n].ptr[n2], sizeof(struct FORM_STRUCT), 1, f);
					}
			}

			fwrite("AFZ", 3, 1, f);
			fclose(f);
		}

		printf("Changes saved.\n");
		return 1;
	}

	//out = stdout;
	out = fopen("eform.txt", "w");

	fprintf(out, "; ----------------------------------------\n");
	fprintf(out, ";\n");
	fprintf(out, "; Enemy formation info / Enemy info\n");
	fprintf(out, ";\n");
	fprintf(out, "; ----------------------------------------\n");
	fprintf(out, "#define b(label)                      dw label\n");
	fprintf(out, "#define info(map,enum,exp,gold)       db map, enum __NL__ dw exp, gold\n");
	fprintf(out, "#define bp(x,y)                       db x, y\n");
	fprintf(out, "#define be(type,x,y)                  db type, x, y\n");
	fprintf(out, "#define  e(lvl,hp,sp,str,end,agl,wis) db lvl __NL__ dw hp,hp,sp,sp __NL__ db str,end,agl,wis\n");
	fprintf(out, "#define  a(att,def,spd,md)            db att,def,spd,md\n");
	fprintf(out, "#define  z(item)                      db item\n");
	fprintf(out, "#define eblank                        db 0 __NL__ dw 0,0,0,0 __NL__ db 0,0,0,0, 0,0,0,0 __NL__ db 0 __NL__ dw 0,0,0,0,0,0,0,0 __NL__ db 0,0,0,0 \n");
	fprintf(out, "; ----------------------------------------\n");
	fprintf(out, "\n");
	fprintf(out, "    dw	zones\n");
	fprintf(out, "    dw	formations\n");
	fprintf(out, "    dw	enemies\n");
	fprintf(out, "\n");

	// skip defines..

	// zones
	fprintf(out, "zones:\n");
	at = 0;
	for(n = 0; n < 64; ++n) {
		x = xrevref_zone(n+1);
		if(x == -1) {
			fprintf(out, "    db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ; blank\n");
		}
		else {
			--x;
			printf("[%d]->[%d]\n", n, x);

			fprintf(out, "    db %d", z.ptr[x].numforms);
			for(n2 = 0; n2 < z.ptr[x].numforms; ++n2) {
				fprintf(out, ",%2d,%2d", xfindform(n2, x, &z), z.ptr[x].ptr[n2].priority);
				++at;
			}
			for(; n2 < 12; ++n2) {
				fprintf(out, ", 0, 0");
			}
			fprintf(out, "\n");
		}
	}
	fprintf(out, "\n");

	// make formdefs.h
	genformdefs("formdefs.h", &z);

	// formations
	fprintf(out, "formations:\n");
	for(n = 0; n < at; ++n) {
		fprintf(out, "    b( form%03d )\n", n + 1);
	}
	fprintf(out, "\n");

	// enemies:
	fprintf(out, "enemies:\n");
	for(n = 0; n < 150; ++n) {
		x = xfindref_enemy(n);
		if(x == -1) {
			fprintf(out, "    eblank\n");
		}
		else {
			struct ENEMY_STRUCT tmp;
			struct ENEMY_STRUCT *en;

			if(zones_only) {
				struct ENEMYREF *er = &enemyref[x];

				strcpy(tmp.name, er->name);
				tmp.level = er->level;
				tmp.hp = er->hp;
				tmp.sp = er->sp;
				tmp.str = er->str;
				tmp.end = er->end;
				tmp.agl = er->agl;
				tmp.wis = er->wis;
				tmp.att = er->att;
				tmp.def = er->def;
				tmp.spd = er->spd;
				tmp.mgd = er->mgd;
				tmp.fire = er->fire;
				tmp.water = er->water;
				tmp.wind = er->wind;
				tmp.weapon = er->weapon;
				tmp.exp = er->exp;
				tmp.au = er->au;
				tmp.item = er->item;
				memcpy(tmp.ai, er->ai, sizeof(tmp.ai));

				en = &tmp;
			} else {
				// match the enemy
				for(n2 = 0; n2 < e.num; ++n2) {
					if(!strcmp(enemyref[x].name, e.ptr[n2].name))
						break;
				}
				if(n2 >= e.num) {
					printf("ERROR: no enemy named:%s\n", enemyref[x].name);
					return 1;
				}

				en = &e.ptr[n2];
			}

			fprintf(out, "    ; %s - %d exp, %d au\n", en->name, en->exp, en->au);
			fprintf(out, "    e(%2d,%2d,%2d,  %2d,%2d,%2d,%2d)\n", en->level, en->hp, en->sp, en->str, en->end, en->agl, en->wis);
			fprintf(out, "    a(%2d,%2d,%2d,%2d)\n", en->att, en->def, en->spd, en->mgd);
			fprintf(out, "    z(%2d)\n", en->item);
			fprintf(out, "    db ");
			for(n2 = 0; n2 < 16; ++n2) {
				if(n2 != 0)
					fprintf(out, ",");
				fprintf(out, "%2d", en->ai[n2]);
			}
			fprintf(out, "\n");
			fprintf(out, "    db %d,%d,%d,%d\n", en->weapon, en->fire, en->water, en->wind);

			fprintf(out, "\n");
		}
	}
	fprintf(out, "\n");

	fprintf(out, "    ; Formation data\n");
	for(n = 0; n < at; ++n) {
		formp = findform(n, &z);
		fprintf(out, "form%03d:\n", n + 1);
		fprintf(out, "    ; %s\n", formp->name);
		num = 0;
		exp = 0;
		au = 0;
		y = 0;
		for(n2 = 0; n2 < 8; ++n2) {
			if(formp->guy_active[n2+4]) {
				x = formp->guy_type[n2+4];
				if(x == 0) {
					if(errors >= 10) {
						printf("Quitting because of too many errors.\n");
						return 1;
					}
					printf("ERROR: uninitialized enemy in [%s]:[%s] at slot %d\n", zonestr(n, &z), formp->name, n2+4+1);
					++errors;
				}
				x = x - 1 + base_enemy;

				if(zones_only) {
					x = xfindref_enemy(x);
					if(x == -1) {
						printf("ERROR: no enemy with index %d\n", x);
						return 1;
					}

					exp += enemyref[x].exp;
					au  += enemyref[x].au;
					y   += enemyref[x].level;
				} else {
					exp += e.ptr[x].exp;
					au  += e.ptr[x].au;
					y   += e.ptr[x].level;
				}

				++num;
			}
		}
		y /= 2;
		exp += y;

		fprintf(out, "    info(%2d,%2d,  %d,%d)\n", formp->map + base_map, num, exp, au);
		for(n2 = 0; n2 < 4; ++n2)
			fprintf(out, "    bp(%2d,%2d)\n", formp->guy_x[n2], formp->guy_y[n2]);

		for(n2 = 4; n2 < 12; ++n2) {
			if(formp->guy_active[n2]) {
				x = formp->guy_type[n2]-1;
				if(zones_only) {
					x = xfindref_enemy(x);
				} else {
					x = findref_enemy(e.ptr[x].name);
				}

				fprintf(out, "    be(%2d, %2d,%2d)\n", enemyref[x].index + base_enemy, formp->guy_x[n2], formp->guy_y[n2]);
			}
		}
		fprintf(out, "\n");
	}
	fprintf(out, "\n");
	fclose(out);

	if(errors)
		return 1;

	printf("Success.\n");
	return 0;
}
