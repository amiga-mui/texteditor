/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2018 TextEditor.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TextEditor class Support Site:  http://www.sf.net/projects/texteditor-mcc

 $Id$

***************************************************************************/

#include <stdint.h>

#if !defined(__MORPHOS__)
// uncompressed ARGB data
#if defined(__AROS__)
extern const uint8_t icon32[];
#else
extern const uint32_t icon32[];
#endif

#define ICON32_WIDTH       24
#define ICON32_HEIGHT      20
#define ICON32_DEPTH       32
#else
// bzip2 compressed ARGB data
extern const uint8_t icon32[];
#endif

#ifdef USE_ICON8_COLORS
const ULONG icon8_colors[24] =
{
	0x00000000,0x00000000,0x00000000,
	0x43434343,0x5b5b5b5b,0x97979797,
	0x7b7b7b7b,0x7b7b7b7b,0x7b7b7b7b,
	0x86868686,0x90909090,0x8b8b8b8b,
	0xafafafaf,0xafafafaf,0xafafafaf,
	0x00000000,0x00000000,0x00000000,
	0xffffffff,0x9b9b9b9b,0x9b9b9b9b,
	0xffffffff,0xffffffff,0xffffffff,
};
#endif

#define ICON8_WIDTH        24
#define ICON8_HEIGHT       14
#define ICON8_DEPTH         3
#define ICON8_COMPRESSION   0
#define ICON8_MASKING       2

#ifdef USE_ICON8_HEADER
const struct BitMapHeader icon8_header =
{ 23, 14, 364, 388, 3, 2, 0, 0, 0, 14, 14, 23, 14 };
#endif

#ifdef USE_ICON8_BODY
const UBYTE icon8_body[168] = {
0x00,0x00,0x16,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x0c,0x00,0x00,0x60,0x3c,
0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x1c,0x00,0x00,0xf0,0x68,0x00,0x00,0x60,
0x30,0x00,0x00,0x60,0x38,0x00,0x01,0xd8,0xd0,0x00,0x00,0xf0,0x60,0x00,0x00,
0xf0,0x70,0x00,0x03,0xad,0xa0,0x00,0x01,0xe8,0xc0,0x00,0x01,0xf8,0xe0,0x00,
0x07,0x57,0x40,0x00,0x03,0xd5,0x80,0x00,0x03,0xff,0xc0,0x00,0x0e,0xae,0x84,
0x00,0x07,0xab,0x18,0x00,0x07,0xff,0x80,0x00,0x1d,0x5d,0xa8,0x00,0x0f,0x56,
0x54,0x00,0x0f,0xf7,0x80,0x00,0x3a,0xb2,0x40,0x00,0x1e,0xad,0xb0,0x00,0x1f,
0xee,0x40,0x00,0x75,0x4c,0x20,0x00,0x3d,0x52,0x80,0x00,0x3f,0xf5,0x40,0x00,
0xea,0x98,0xb0,0x00,0x7a,0x84,0xa0,0x00,0x7f,0xf3,0xe0,0x00,0xd5,0x55,0x58,
0x00,0xf5,0x55,0x50,0x00,0xff,0xff,0xf0,0x00,0xaa,0xaa,0xac,0x00,0xea,0xaa,
0xa8,0x00,0xff,0xff,0xf8,0x00,0x00,0x00,0x02,0x00,0x2a,0xaa,0xa8,0x00,0x55,
0x55,0x54,0x00, };
#endif

#ifdef USE_ICON8_BITMAP
const struct BitMap icon8_bitmap =
{
  4, 14, 0, ICON8_DEPTH, 0,
  { (UBYTE *)icon8_body+(0*14*4),
    (UBYTE *)icon8_body+(1*14*4),
    (UBYTE *)icon8_body+(2*14*4),
    NULL, NULL, NULL, NULL, NULL }
};
#endif
