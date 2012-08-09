/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2010 by TextEditor.mcc Open Source Team

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

#include <graphics/gfxmacros.h>
#include <graphics/text.h>
#include <clib/alib_protos.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/intuition.h>

#include "Debug.h"

LONG TextLengthNew(struct RastPort *rp, CONST_STRPTR string, ULONG count, LONG tabSizePixels)
{
  LONG result = 0;
  char c;
  char *tptr;
  char *tptr0;
  LONG count0 = 0;
  tptr = (char *)string;
  tptr0 = tptr;

  ENTER();

  do
  {
    c=*tptr;
    tptr++;

    if(count == 0)
    {
      if(count0 != 0)
        result += TextLength(rp, tptr0, count0);

      RETURN(result);
      return(result);
    }

    if(c == '\t')
    {
      if(count0 != 0)
        result += TextLength(rp, tptr0, count0);

      tptr0=tptr;
      count0=0;

      result += tabSizePixels;
    }
    else
      count0++;

    count--;
  }
  while(1);
}

ULONG TextFitNew(struct RastPort *rp, CONST_STRPTR string, ULONG strLen, struct TextExtent *textExtent, CONST struct TextExtent *constrainingExtent, LONG strDirection, LONG constrainingBitWidth, LONG constrainingBitHeight, LONG tabSizePixels)
{
  ULONG result=0;
  ULONG strLen0=0;
  char c;
  char *tptr = (char *)string;
  char *tptr0;

  ENTER();

  tptr0 = tptr;

  do
  {
    c = *tptr;
    tptr++;

    if(!strLen)
    {
      result += TextFit(rp, tptr0, strLen0, textExtent, constrainingExtent, strDirection, constrainingBitWidth, constrainingBitHeight);

      RETURN(result);
      return(result);
    }

    if(c == '\t')
    {
      if(strLen0)
      {
        result += TextFit(rp, tptr0, strLen0, textExtent, constrainingExtent, strDirection, constrainingBitWidth, constrainingBitHeight);
      }

      if((constrainingBitWidth -= TextLengthNew(rp, tptr0, strLen0+1, tabSizePixels)) <= 0)
      {
        RETURN(result);
        return(result);
      }

      result++;
      tptr0 = tptr;
      strLen0 = 0;
    }
    else
      strLen0++;

    strLen--;
  }
  while(1);
}

void TextNew(struct RastPort *rp, CONST_STRPTR string, ULONG count, LONG tabSizePixels)
{
  char c;
  char *tptr;
  char *tptr0;
  LONG count0=0;

  ENTER();

  tptr=(char *)string;
  tptr0=tptr;

  do
  {
    c=*tptr;
    tptr++;

    if(count == 0)
    {
      Text(rp, tptr0, count0);

      LEAVE();
      return;
    }

    if(c == '\t')
    {
      if(count0 != 0)
        Text(rp, tptr0, count0);

      tptr0=tptr;
      count0=0;

      rp->cp_x += tabSizePixels;
    }
    else
      count0++;

    count--;
  }
  while(1);
}
