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

// gbr2bin.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

typedef unsigned char byte;
typedef std::vector<byte> byteVector_t;
typedef unsigned short word;
typedef unsigned int dword;

#define OBJECT_TYPE_TILEDATA 0x0002
#define OBJECT_TYPE_PAL      0x000D
#define OBJECT_TYPE_TILEPAL  0x000E

// opens header and checks for header id
bool GBR_ReadHeader( FILE *fp )
{
	byte cmpbuf[] = "GBO0";
	byte buf[4];

	fread( buf, sizeof(byte), 4, fp );

	return !memcmp( buf, cmpbuf, 4 );
}

// returns object type
//
// if block -> 0 then nothing is read but we move the fp to the next block
// if objectID -> 0 then no object ID is retrieved
word GBR_ReadBlock( byteVector_t *block, word *pObjectID, FILE *fp )
{
	word objectType;
	dword recordLength;
	long fpos;
	word objectID;

	fread( &objectType, sizeof(word), 1, fp );
	if( feof( fp ) )
		return 0;
	fread( &objectID, sizeof(word), 1, fp );
	fread( &recordLength, sizeof(dword), 1, fp );

	if( pObjectID )
		*pObjectID = objectID;

	fpos = ftell( fp );

	if(block) {
		(*block).resize( recordLength );
		fread( &(*block)[0], sizeof(byte), recordLength, fp );
	} else {
		fseek( fp, fpos + recordLength, SEEK_SET );
	}

	return objectType;
}

// find tiledata within array and export into vector
word GBR_ExportTiles( byte *array, byteVector_t *result, bool &bBigTiles ) {
	word width = *(word *)&array[30];
	word height = *(word *)&array[32];
	word count = *(word *)&array[34];
	byte *data = &array[40];
	word bytewidth = width / 8;
	word blockwidth = ( bytewidth + 1 ) / 2;
	word localwidth = (bytewidth < 2) ? bytewidth : 2;
	word blockheight = ( height + 15 ) / 16;
	word localheight = (height < 16) ? height : 16;

	if( blockwidth > 1 && blockheight > 1 )
		bBigTiles = true;

	int offsetPerTile = width * height;
	int i, j, k;
	int x, y;
	int xb;
	int byte1;
	int byte2;
	int color;

	if( !result )
		return 0;

	for( i = 0; i < count; i++ ) {
		byte *tiledata = &data[ offsetPerTile * i];

		for( k = 0; k < blockwidth; k++ ) {
			for( j = 0; j < blockheight; j++ ) {
				for( x = 0; x < localwidth; x++ ) {
					for( y = 0; y < localheight; y++ ) {
						byte1 = 0;
						byte2 = 0;
						for( xb = 0; xb < 8; xb++ ) {
							color = tiledata[ (j * 16 + y) * width + (k * 2 + x) * 8 + xb ];
							byte1 = (byte1 << 1) | ( ( color & 1 ) ? 1 : 0 );
							byte2 = (byte2 << 1) | ( ( color & 2 ) ? 1 : 0 );
						}
						result->push_back( byte1 );
						result->push_back( byte2 );
					}
				}
			}
		}
	}

	return count;
}

// find paldata within array and export into vector
word GBR_ExportPals( byte *array, byteVector_t *result ) {
	word count = *(word *)&array[2];
	dword *paldata = (dword *)&array[4];

	int i;

	if( !result )
		return 0;

	for( i = 0; i < count; i++ )
		result->push_back( paldata[i] );

	return count;
}

struct Palette {
	word color[4];
};

Palette ParsePalette( dword *colors ) {
	Palette out;

	for( int i = 0; i < 4; i++ ) {
		// deconstruct 32-bit RGBx
		byte r = colors[i] & 0xff;
		byte g = (colors[i] >> 8) & 0xff;
		byte b = (colors[i] >> 16) & 0xff;

		// construct 15-bit RGB
		out.color[i] = (((word)b >> 3) << 10) + (((word)g >> 3) << 5) + (r >> 3);
	}

	return out;
}

int main(int argc, char* argv[])
{
	// gbr2bin gbr_file bin_file [first_tilenum] [last_tilenum]
	FILE *fGBR = 0;
	FILE *fBIN = 0;
	int firstTileNum = -1;
	int lastTileNum = -1;

	// argument count
	if( argc < 3 ) {
		printf("Usage:\n");
		printf("gbr2bin gbr_file bin_file [/nopal] [ [first_tilenum] [last_tilenum] ... ]\n");
		printf("gbr2bin gbr_file /printpal\n");
		return 1;
	}

	// open gbr file for input
	if( !( fGBR = fopen( argv[1], "rb" ) ) ) {
		printf("couldn't open %s!\n", argv[1]);
		return 1;
	}

	if( !strcmp( argv[2], "/printpal" ) ) {
		// read and check for valid header
		if( !GBR_ReadHeader( fGBR ) ) {
			printf("%s invalid file format!\n", argv[1]);
			return 1;
		}

		while( !feof( fGBR ) ) {
			byteVector_t block;
			word type = GBR_ReadBlock( &block, 0, fGBR );

			if( type != OBJECT_TYPE_PAL )
				continue;

			word count = *(word *)&block[2];
			dword *paldata = (dword *)&block[4];

			for( int i = 0; i < count; i++ ) {
				Palette pal = ParsePalette(paldata + (i * 4));
				printf("%d: 0x%04x, 0x%04x, 0x%04x, 0x%04x\n", i, pal.color[0], pal.color[1], pal.color[2], pal.color[3]);
			}
		}

		return 0;
        }

	// open bin file for output
	if( !( fBIN = fopen( argv[2], "wb" ) ) ) {
		printf("couldn't open %s!\n", argv[2]);
		return 1;
	}

	//check if we don't want to export palettes
	int start_tile_arg = 3;
	bool bExportPalettes = true;
	if( argc >= 4 && !strcmp( argv[3], "/nopal" ) ) {
		bExportPalettes = false;
		start_tile_arg++;
	}

	// read and check for valid header
	if( !GBR_ReadHeader( fGBR ) ) {
		printf("%s invalid file format!\n", argv[1]);
		return 1;
	}

	// find a block
	byteVector_t block;
	byteVector_t tiledata;
	byteVector_t paldata;
	int tileCount;
	//int palCount;
	bool bBigTiles = false;
	word type;
	while( !feof( fGBR ) ) {
		type = GBR_ReadBlock( &block, 0, fGBR );
		if( type == OBJECT_TYPE_TILEDATA )
			tileCount = GBR_ExportTiles( &block[0], &tiledata, bBigTiles );
		if( type == OBJECT_TYPE_TILEPAL )
			/*palCount =*/ GBR_ExportPals( &block[0], &paldata );
	}

	// tilelist
	std::vector<int> tilelist;
	std::vector<int>::iterator it;
	if( argc >= start_tile_arg + 1 ) {
		for( int i = start_tile_arg; i < argc - 1; i += 2 ) {
			firstTileNum = atoi( argv[ i ] );
			lastTileNum = atoi( argv[ i + 1 ] );
			for( int j = firstTileNum; j <= lastTileNum; j++ ) {
				tilelist.push_back( j );
			}
		}
	} else {
		firstTileNum = 0;
		lastTileNum = tileCount - 1;
		for( int j = firstTileNum; j <= lastTileNum; j++ ) {
			tilelist.push_back( j );
		}
	}

	int palCountPerTile = 1;
	if( bBigTiles )
		palCountPerTile = 4;

	int i, j;

//	printf("foo[%d]\n", palCountPerTile);

	if( bExportPalettes ){
		// palette out
		for( it = tilelist.begin(); it != tilelist.end(); it++ ) {
			i = *it;
			for( j = 0; j < palCountPerTile; j++ )
				fwrite( &paldata[ i ], sizeof(byte), 1, fBIN );
		}
	}

	int sizePerTile = tiledata.size() / tileCount;
	// tile out
	for( it = tilelist.begin(); it != tilelist.end(); it++ ) {
		i = *it;
		fwrite( &tiledata[ i * sizePerTile ], sizePerTile, 1, fBIN );
	}

	// close files
	fclose( fBIN );
	fclose( fGBR );

	printf("converted %s to %s.\n", argv[1], argv[2]);

	return 0;
}

