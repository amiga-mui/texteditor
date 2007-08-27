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

static void SpecPointerColors(LONG blacknum)
{
  int i;

  ENTER();

  for(i=0; i<16; i++)
    selectPointer_bp2[i] = selectPointer_bp0[i] | selectPointer_bp1[i];

  if(blacknum == 1)
  {
    selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp2;
    selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp0;
  }
  else if(blacknum == 3)
  {
    selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp2;
    selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp1;
  }
  else
  {
    selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp0;
    selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp1;
  }

  LEAVE();
}

void SetMousePointer(Object *obj, struct InstData *data)
{
  ENTER();

	if(data->PointerObj == NULL)
  {
    if(IntuitionBase->lib_Version >= 39)
    {
      SpecPointerColors(1);

      if((data->PointerObj = (Object *)NewObject(NULL, "pointerclass",
          POINTERA_BitMap,      (LONG)&selectPointerBitmap,
          POINTERA_XOffset,     (LONG)selectPointerXOffset,
          POINTERA_YOffset,     (LONG)selectPointerYOffset,
          POINTERA_WordWidth,   (ULONG)1,
          POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
          POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
          TAG_DONE)))
      {
        SetWindowPointer(_window(obj), WA_Pointer, data->PointerObj, TAG_DONE);
      }
      else
        SetWindowPointer(_window(obj), TAG_DONE);
    }
    else
    {
    	if((data->PointerObj = (Object *)AllocVec(sizeof(selectPointer), MEMF_CHIP|MEMF_PUBLIC)))
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

  if(data->PointerObj)
  {
    if(IntuitionBase->lib_Version >= 39)
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

