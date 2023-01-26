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
 
#include<map>
#include<vector>
#include<iostream>
#include<string>

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

FILE *_log;

typedef unsigned long code_point;
typedef uint16_t  gamechar;

void mem_error()
{
	printf("memory error!\n");
	exit(1);
}

void csize_error()
{
	printf("csize error!\n");
	exit(1);
}

void undec_char_error()
{
	printf("undeclared character error!\n");
	exit(1);
}

std::map<code_point, int> codes;

int highest;
short offsets[1024];
int dataindex;
char *data;

int allocpages;

int numtexts;
int bufsize;
int page, deep;

int csize( unsigned char c )
{
	int size = 0;
	if( ( c >= 0x00 ) && ( c <= 0x7f ) ) {
		size = 1;
	} else if( ( c >= 0xc2 ) && ( c <= 0xdf ) ) {
		size = 2;
	} else if( ( c >= 0xe0 ) && ( c <= 0xef ) ) {
		size = 3;
	} else if( ( c >= 0xf0 ) && ( c <= 0xf7 ) ) {
		size = 4;
	} else if( ( c >= 0xf8 ) && ( c <= 0xfb ) ) {
		size = 5;
	} else if( ( c >= 0xfc ) && ( c <= 0xfd ) ) {
		size = 6;
	}
	return size;
}

int cstrlen( const char *s )
{
	int clen = 0;
	while( *s ) {
		clen++;
		s += csize( *s );
	}
	return clen;
}

code_point takechar( const char *&s )
{
	code_point lchar = 0;
	int cs = csize( *s );
	if( cs == 1 ){
		lchar = s[0] & 0x7F;
	} else if( cs == 2 ){
		lchar = ( (code_point)( s[0] & 0x1F ) << 6 ) | (code_point)( s[1] & 0x3F );
	} else if( cs == 3 ){
		lchar = ( (code_point)( s[0] & 0x0F ) << 12 ) | ( (code_point)( s[1] & 0x3F ) << 6 ) | (code_point)( s[2] & 0x3F );
	} else if( cs == 4 ){
		lchar = ( (code_point)( s[0] & 0x07 ) << 18 ) | ( (code_point)( s[1] & 0x3F ) << 12 ) | ( (code_point)( s[2] & 0x3F ) << 6 ) | (long)( s[3] & 0x3F );
	}
	s += cs;
	return lchar;
}

const char *conv_codepoint( code_point cp )
{
	static char tmpstr[5] = { 0, 0, 0, 0, 0 };
	tmpstr[0] = 0;
	if( cp <= 0x7F ){	// 7 bits
		tmpstr[0] = cp & 0x7F;
		tmpstr[1] = 0;
	} else if( cp <= 0x7FF ){	// 11 bits ( 5 bits + 6 bits )
		tmpstr[0] = ( ( cp >> 6 ) & 0x1F ) | 0xC0;
		tmpstr[1] = ( ( cp      ) & 0x3F ) | 0x80;
		tmpstr[2] = 0;
	} else if( cp <= 0xFFFF ){	// 16 bits ( 4 bits + 6 bits + 6 bits )
		tmpstr[0] = ( ( cp >> 12 ) & 0x1F ) | 0xE0;
		tmpstr[1] = ( ( cp >>  6 ) & 0x3F ) | 0x80;
		tmpstr[2] = ( ( cp       ) & 0x3F ) | 0x80;
		tmpstr[3] = 0;
	} else if( cp <= 0xFFFF ){	// 21 bits ( 3 bits + 6 bits + 6 bits + 6 bits )
		tmpstr[0] = ( ( cp >> 18 ) & 0x07 ) | 0xF0;
		tmpstr[1] = ( ( cp >> 12 ) & 0x3F ) | 0x80;
		tmpstr[2] = ( ( cp >>  6 ) & 0x3F ) | 0x80;
		tmpstr[3] = ( ( cp       ) & 0x3F ) | 0x80;
		tmpstr[4] = 0;
	}
	return tmpstr;
}

std::basic_string<code_point> lstrdup( const char *s )
{
	std::basic_string<code_point> ls;
	while( *s ) {
		code_point lc = takechar( s );
		if( codes.find( lc ) == codes.end() ){
			codes[lc] = 0;
		}
		ls += lc;
	}
	return ls;
}

gamechar *cstrcnv( std::basic_string<code_point> ls )
{
	gamechar *ws = (gamechar*)malloc( ( ls.length() + 1 ) * sizeof( uint16_t ) );
	int ofs = 0;

	for( std::basic_string<code_point>::iterator it = ls.begin(), end = ls.end(); it != end; it++ ) {
		code_point lc = *it;
		if( codes.find( lc ) == codes.end() ){
			std::cout << *(char*)&lc << " " << lc << std::endl;
			std::cout << (char*)&lc << std::endl;
			undec_char_error();
		}
		ws[ofs] = codes[ lc ];
		ofs++;
	}
	ws[ofs] = 0;
	return ws;
}

struct TEXT_BLOCK
{
	int at;
	std::basic_string<code_point> s;
	gamechar *str;
	int len;
};

std::vector<TEXT_BLOCK> text;

struct FONT_INFO
{
	int x;
	int y;
	int width;
	int height;
	int xoffset;
	int yoffset;
	int xadvance;
	int page;
	int chnl;
};

std::map<code_point, FONT_INFO> font;

void proc_line(char *s)
{
	char *p;
	int n;

	p = strchr(s, ':');
	if(!p)
		return;

	*(p++) = 0;
//	n = atoi(s);
	n = highest + 1;
	fprintf(_log, "#define %s %d\n", s, n);

	if(n > highest)
		highest = n;
	
	TEXT_BLOCK tb;
//	text = (TEXT_BLOCK*)realloc(text, sizeof(struct TEXT_BLOCK) * (numtexts + 1));
//	if(!text)
//		mem_error();

	tb.s = lstrdup(p);
	//if(!text[numtexts].str)
	//	mem_error();

	tb.at = n;
	tb.len = ( cstrlen(p) + 1 ) * sizeof( gamechar );
	text.push_back(tb);

	++numtexts;

//	offsets[n] = dataindex;
//	data = realloc(data, dataindex + strlen(str) + 1);
//	strcpy(data + dataindex, str);
//	dataindex += strlen(str) + 1;

//	printf("index = %d\n[%s]\n", n, str);
//	++numtexts;
}

int main(int argc, char *argv[])
{
	FILE *in, *font_info; /* *used,*/
	FILE *out, *font_out;
	char *font_image;
	char *p, *p2;
	char *buf/*, *usedbuf*/, line[8192];
	int size/*, usedsize*/;
	int n;
	int len;
	int at;

	_log = fopen("etxt.h", "w");

	highest = -1;
	numtexts = 0;
	dataindex = 0;
	data = NULL;

	if(argc < 6)
		exit(0);
	
	in = fopen(argv[1], "r");
	font_info = fopen(argv[2], "r");
	font_image = argv[3];
	//used = fopen(argv[2], "r");
	out = fopen(argv[4], "wb");
	font_out = fopen(argv[5], "w");

	fseek(in, 0l, SEEK_END);
	size = ftell(in);
	rewind(in);
	buf = (char*)malloc(size+1);
	if(!buf) {
		printf("mem\n");
		exit(0);
	}
	size = fread(buf, 1, size, in);
	fclose(in);
	buf[size] = 0;

	while( fgets( line, 8192, font_info ) )
	{
		if( !strncmp( line, "char id", 7 ) ){
			code_point char_id;
			FONT_INFO fi;
			sscanf( line, "char id=%lu x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d page=%d chnl=%d",
				&char_id, &fi.x, &fi.y, &fi.width, &fi.height, &fi.xoffset, &fi.yoffset, &fi.xadvance, &fi.page, &fi.chnl );
			
			//printf( "char id=%lu x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d page=%d chnl=%d\n",
			//	char_id, fi.x, fi.y, fi.width, fi.height, fi.xoffset, fi.yoffset, fi.xadvance, fi.page, fi.chnl );
			
			font[ char_id ] = fi;
		}
	}

	//fseek(used, 0l, SEEK_END);
	//usedsize = ftell(used);
	//rewind(used);
	//usedbuf = (char*)malloc(usedsize+1);
	//if(!usedbuf) {
	//	printf("mem\n");
	//	exit(0);
	//}
	//usedsize = fread(usedbuf, 1, usedsize, used);
	//fclose(used);
	//usedbuf[usedsize] = 0;
	//
	//p = usedbuf;
	//int code = 32;
	//while(1) {
	//	p2 = strchr(p, '\n');
	//	if( !p2 ){
	//		break;
	//	}
	//	*p2 = 0;
	//	int cs = csize( *p );
	//	if( cs == 0 || cs != ( p2 - p ) ){
	//		csize_error();
	//	}
	//	const char *pp = p;
	//	unsigned long long id = takechar(pp);
	//	std::cout << p << " " << id << " ";
	//	codes[id] = code;
	//	code++;
	//	p = p2 + 1;
	//}
	//std::cout << std::endl;

	// force 0-127 used
	for( code_point lc = 0; lc < 128; lc++ ){
		codes[lc] = lc;
	}
	
	// read through the dialog text and pick up used characters
	p = buf;
	while(1) {
		p = strchr(p, '[');
		if(!p)
			break;
		p2 = strchr(p, ']');
		if(!p2)
			break;
		++p;
		len = p2 - p;
		strncpy(line, p, len);
		line[len] = 0;

		proc_line(line);

		p = p2 + 1;
	}
	free(buf);

	unsigned char* pixels;
	int width;
	int height;
	int bpp;

	// read image and get meta data
	pixels = stbi_load( font_image, &width, &height, &bpp, 0);

	// create 16bit internal character code and font data for used characters (leave 0-127 for default) start from 128
	int code = 128;
	int fontbuf[ 8 * 12 ];
	
	for( std::map<code_point,int>::iterator it = codes.begin(), end = codes.end(); it != end; it++ ) {
		if( it->first >= 128 ){
			if( font.find( it->first ) != font.end() ){
				FONT_INFO fi = font[ it->first ];
				//int charwidth = fi.width + 1;
				int charwidth = fi.xadvance;
				
				memset( fontbuf, 0, sizeof( fontbuf ) );
				
				for( int dy = 0; dy < fi.height + 1; dy++ ){
					for( int dx = 0; dx < fi.width + 1; dx++ ){
						int dstx = fi.xoffset + dx;
						int dsty = fi.yoffset + dy;
						int srcx = fi.x + dx;
						int srcy = fi.y + dy;
						if( dstx >= 0 && dstx < 8 && dsty >= 0 && dsty < 12 ){
							if( srcx >= 0 && srcx < width && srcy >= 0 && srcy < height ){
								fontbuf[ dsty * 8 + dstx ] = pixels[ ( srcy * width + srcx ) * bpp ];
							}
						}
					}
				}
				
				fprintf( font_out, ".db %d   ; %d %lu %s\n", charwidth, code, it->first, conv_codepoint( it->first ) );
				for( int dy = 0; dy < 12; dy++ ) {
					std::string fontstr;
					fontstr += ".db 0b";
					for( int dx = 0; dx < 8; dx++ ) {
						int a = fontbuf[ dy * 8 + dx ];
						fontstr += a ? "1" : "0";
					}
					fprintf( font_out, "%s\n", fontstr.c_str() );
				}
				fprintf( font_out, "\n" );
			} else {
				printf( "can't find code point %lu in font definition\n", it->first );
			}
			it->second = code++;
		}
	}
	
	fclose( font_out );

	// free image data
	stbi_image_free( pixels );
	
	++highest;
	bufsize = highest * 3;

	for(n = 0; n < numtexts; ++n) {
		text[n].str = cstrcnv( text[n].s );
	}

	buf = NULL;
	allocpages = 0;
	for(n = 0; n < numtexts; ++n) {
		page = bufsize / 16384;
		deep = bufsize % 16384;

		if(deep + text[n].len > 16384) {
			++page;
			deep = 0;
			bufsize = page * 16384;
		}

		if(allocpages <= page) {
			buf = (char*)realloc(buf, (page+1) * 16384);
			if(!buf)
				mem_error();
			memset(&buf[allocpages*16384],0,(page+1-allocpages)*16384);
			allocpages = page + 1;
		}

		at = text[n].at;
		buf[at * 3 + 0] = page;
		buf[at * 3 + 1] = deep & 0xff;
		buf[at * 3 + 2] = (deep >> 8) & 0xff;
		memcpy(buf + bufsize, text[n].str, text[n].len);
		bufsize += text[n].len;
	}

	fwrite(buf, bufsize, 1, out);
	fclose(out);

	printf("Success!  Wrote [%d] strings in a [%d] entry table\n", numtexts, highest);
}
