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

unsigned char buf[16384];

int main(int argc, char *argv[])
{
	int n, page, size, total;
	float r;
	FILE *f;

	if(argc < 2) {
		printf("usage: pagesize <.gb file>\n");
		return 0;
	}

	f = fopen(argv[1], "rb");
	if(!f)
		return 1;
	page = 0;
	total = 0;
	while(1) {
		fread(buf, 16384, 1, f);
		if(feof(f))
			break;

		// start at the end and go backwards
		for(n = 16382; n >= 0; --n) {
			if(buf[n] != buf[16383])
				break;
		}
		size = 16383 - n;
		printf("Page %3d = ", page);

		r = ( 16383 - n ) / (float)16384 * 100;
		printf("%5d bytes free\t(%5.1f%%)", 16383 - n, r);
		if(size == 16384)
			printf(" **BLANK**");
		if(size == 1)
			printf(" **probably 0**");
		printf("\n");

		++page;
		total += 16384 - size;
	}
	fclose(f);
	printf("----\nROM usage: %d/%d (%d%%).\n", total, page * 16384, total * 100 / (page * 16384));
}
