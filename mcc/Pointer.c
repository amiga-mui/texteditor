/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2007 by TextEditor.mcc Open Source Team

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

#include <string.h>

#include <clib/alib_protos.h>

#include <intuition/pointerclass.h>

#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/graphics.h>

#include "private.h"

static const UWORD selectPointer_bp0[] =
{
  0x8800,    /* #...#........... */
  0x5000,    /* .#.#............ */
  0x2000,    /* ..#............. */
  0x2000,    /* ..#............. */
  0x2000,    /* ..#............. */
  0x2000,    /* ..#............. */
  0x2000,    /* ..#............. */
  0x2000,    /* ..#............. */
  0x4000,    /* .#.............. */
  0x2000,    /* ..#............. */
  0x2000,    /* ..#............. */
  0x2000,    /* ..#............. */
  0x2000,    /* ..#............. */
  0x2000,    /* ..#............. */
  0x5000,    /* .#.#............ */
  0x8800,    /* #...#........... */
};

static const UWORD selectPointer_bp1[] =
{
  0x4600,    /* .#...##......... */
  0x2800,    /* ..#.#........... */
  0x1000,    /* ...#............ */
  0x1000,    /* ...#............ */
  0x1000,    /* ...#............ */
  0x1000,    /* ...#............ */
  0x1000,    /* ...#............ */
  0x1000,    /* ...#............ */
  0x3800,    /* ..###........... */
  0x1000,    /* ...#............ */
  0x1000,    /* ...#............ */
  0x1000,    /* ...#............ */
  0x1000,    /* ...#............ */
  0x1000,    /* ...#............ */
  0x2800,    /* ..#.#........... */
  0x4600,    /* .#...##......... */
};

static UWORD selectPointer_bp2[] =
{
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
};

static const UWORD selectPointer[] =
{
//plane1    plane2
  0x0000,   0x0000,

  0x8800,   0x4600,
  0x5000,   0x2800,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x4000,   0x3800,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x5000,   0x2800,
  0x8800,   0x4600,

  0x0000,   0x0000
};

#define selectPointerWidth   15
#define selectPointerHeight  16
#define selectPointerXOffset -5
#define selectPointerYOffset -7

static struct BitMap selectPointerBitmap =
{
  2, 16, 0, 2, 0,
  { (PLANEPTR)selectPointer_bp0, (PLANEPTR)selectPointer_bp1, NULL, NULL, NULL, NULL, NULL }
};

static void SpecPointerColors(Object *obj)
{
  int i;
  ULONG colors[3*3];
  LONG blackDiff[3];
  LONG whiteDiff[3];
  LONG blackIndex;
  LONG whiteIndex;

  ENTER();

  for(i=0; i<16; i++)
    selectPointer_bp2[i] = selectPointer_bp0[i] | selectPointer_bp1[i];

  // get the current screen's pointer colors (17 to 19)
  GetRGB32(_window(obj)->WScreen->ViewPort.ColorMap, 17, 3, colors);

  for(i = 0; i < 3; i++)
  {
    LONG dr, dg, db;

    // normalize the colors to 8 bit per gun as GetRGB32() returns
    // 32bit left aligned values
    colors[i*3+0] >>= 24;
    colors[i*3+1] >>= 24;
    colors[i*3+2] >>= 24;

    // calculate the geometric difference to the color black (=0x00000000)
	dr = 0x00000000 - colors[i*3+0];
	dg = 0x00000000 - colors[i*3+1];
	db = 0x00000000 - colors[i*3+2];
    blackDiff[i] = dr * dr + dg * dg + db * db;
    // calculate the geometric difference to the color white (=0x000000ff)
	dr = 0x000000ff - colors[i*3+0];
	dg = 0x000000ff - colors[i*3+1];
	db = 0x000000ff - colors[i*3+2];
    whiteDiff[i] = dr * dr + dg * dg + db * db;
  }

  // the smallest difference defines the color which is closest to black or
  // equal to black
  if(blackDiff[0] > blackDiff[1])
    if(blackDiff[1] > blackDiff[2])
      blackIndex = 19;
    else
      blackIndex = 18;
  else if(blackDiff[0] > blackDiff[2])
  	blackIndex = 19;
  else
    blackIndex = 17;

  // the smallest difference defines the color which is closest to white or
  // equal to white
  if(whiteDiff[0] > whiteDiff[1])
    if(whiteDiff[1] > whiteDiff[2])
      whiteIndex = 19;
    else
      whiteIndex = 18;
  else if(whiteDiff[0] > whiteDiff[2])
  	whiteIndex = 19;
  else
    whiteIndex = 17;

  // Here we expect the user to have set up quite "different" colors. That
  // means the color closest to white will never be close to black and vice
  // versa. According to these differences we spread the required bitplanes.
  if(whiteIndex == 17)
  {
    if(blackIndex == 18) {
      selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp0;
      selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp1;
    }
    else // blackIndex == 19
    {
      selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp2;
      selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp1;
    }
  }
  else if(whiteIndex == 18)
  {
    if(blackIndex == 17) {
      selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp1;
      selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp0;
    }
    else // blackIndex == 19
    {
      selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp1;
      selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp2;
    }
  }
  else // whiteIndex == 19
  {
    if(blackIndex == 17) {
      selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp2;
      selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp0;
    }
    else // blackIndex == 18
    {
      selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp0;
      selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp2;
    }
  }

  LEAVE();
}

void SetMousePointer(Object *obj, struct InstData *data)
{
  ENTER();

  if(data->PointerObj == NULL)
  {
    if(IntuitionBase->LibNode.lib_Version >= 39)
    {
      SpecPointerColors(obj);

      if((data->PointerObj = (Object *)NewObject(NULL, "pointerclass",
          POINTERA_BitMap,      (LONG)&selectPointerBitmap,
          POINTERA_XOffset,     (LONG)selectPointerXOffset,
          POINTERA_YOffset,     (LONG)selectPointerYOffset,
          POINTERA_WordWidth,   (ULONG)1,
          POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
          POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
          TAG_DONE)) != NULL)
      {
        SetWindowPointer(_window(obj), WA_Pointer, data->PointerObj, TAG_DONE);
      }
      else
        SetWindowPointer(_window(obj), TAG_DONE);
    }
    else
    {
      if((data->PointerObj = (Object *)AllocVec(sizeof(selectPointer), MEMF_CHIP|MEMF_PUBLIC)) != NULL)
      {
        memcpy(data->PointerObj, selectPointer, sizeof(selectPointer));
        SetPointer(_window(obj), (APTR)data->PointerObj, selectPointerHeight, selectPointerWidth, selectPointerXOffset, selectPointerYOffset);
      }
      else
        ClearPointer(_window(obj));
    }
  }

  LEAVE();
}

void ClearMousePointer(Object *obj, struct InstData *data)
{
  ENTER();

  if(data->PointerObj != NULL)
  {
    if(IntuitionBase->LibNode.lib_Version >= 39)
    {
      SetWindowPointer(_window(obj), TAG_DONE);
      DisposeObject(data->PointerObj);
      data->PointerObj = NULL;
    }
    else
    {
      ClearPointer(_window(obj));
      FreeVec(data->PointerObj);
      data->PointerObj = NULL;
    }
  }

  LEAVE();
}
