/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2020 TextEditor.mcc Open Source Team

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

#include <proto/graphics.h>

#include "private.h"

#include "Debug.h"

#define C_LIMIT 4000

/// TextLengthNew

  /* The TextLengthNew() function uses the system function TextLength() in its implementation.
     TextLength() has WORD as its return value type. Very long lines can overflow this value.
     So here I've implemented a loop to build up the value in segments.
  */
LONG TextLengthNew(struct RastPort *rp, const char *string, ULONG count, LONG tabSizePixels)
{
  LONG result = 0;
  const char *tptr = string;
  const char *tptr0 = string;
  LONG count0 = 0;

  ENTER();

  do
  {
    char c;

    if(count == 0)
    {
      // we parsed the string until the end
      // add the remaining characters' width
      if(count0 != 0)
      {
        if(count0 <= C_LIMIT)
          result += TextLength(rp, tptr0, count0);
        else
        {
          LONG l_count = count0;

          while(l_count > C_LIMIT)
          {
            result += TextLength(rp, tptr0, C_LIMIT);
            l_count -= C_LIMIT;
            tptr0 += C_LIMIT;
          }

          result += TextLength(rp, tptr0, l_count);
        }
      }
      break;
    }

    // check the next character
    c = *tptr++;
    if(c == '\t')
    {
      // we found a TAB, calculate the characters' width so far
      if(count0 != 0)
      {
        if(count0 <= C_LIMIT)
          result += TextLength(rp, tptr0, count0);
        else
        {
          LONG l_count = count0;

          while(l_count > C_LIMIT)
          {
            result += TextLength(rp, tptr0, C_LIMIT);
            l_count -= C_LIMIT;
            tptr0 += C_LIMIT;
          }

          result += TextLength(rp, tptr0, l_count);
        }
      }

      tptr0 = tptr;
      count0 = 0;

      // add the size of a TAB
      result += tabSizePixels;
    }
    else
      count0++;

    count--;
  }
  while(TRUE);

  RETURN(result);
  return(result);
}

///
/// TextFitNew
ULONG TextFitNew(struct RastPort *rp, const char *string, ULONG strLen, struct TextExtentNew *textExtentNew, const struct TextExtent *constrainingExtent, LONG strDirection, LONG constrainingBitWidth, LONG constrainingBitHeight, LONG tabSizePixels)
{
  ULONG result = 0;
  ULONG strLen0 = 0;
  const char *tptr = string;
  const char *tptr0 = string;
  struct TextExtent textExtent;
  ULONG te_width  = 0;
  ULONG te_height;

  ENTER();

  te_height = constrainingBitHeight < rp->TxHeight ? 0 : rp->TxHeight;

  do
  {
    char c;

    if(strLen == 0)
    {
      // we parsed the string until the end
      // add the number of fitting remaining characters

      if(constrainingBitWidth <= SHRT_MAX)
      {
        if(strLen0 != 0)
        {
          ULONG l_chars;

          l_chars   = TextFit(rp, tptr0, strLen0, &textExtent, constrainingExtent, strDirection, constrainingBitWidth, constrainingBitHeight);
          te_width += textExtent.te_Width;
          result   += l_chars;
        }
      }
      else
      {
        ULONG l_constBit = constrainingBitWidth;
        ULONG l_chars;

        while(l_constBit > SHRT_MAX && strLen0 > 0)
        {
          LONG l_length;

          l_chars  = TextFit(rp, tptr0, strLen0, &textExtent, constrainingExtent, strDirection, SHRT_MAX, constrainingBitHeight);
          l_length = textExtent.te_Width;

          l_constBit -= l_length;
          te_width   += l_length;
          strLen0 -= l_chars;
          tptr0   += l_chars;
          result  += l_chars;
        }
        if(strLen0 > 0)
        {
          l_chars = TextFit(rp, tptr0, strLen0, &textExtent, constrainingExtent, strDirection, l_constBit, constrainingBitHeight);
          te_width += TextLengthNew(rp, tptr0, l_chars, tabSizePixels);
          result += l_chars;
        }
      }
      break;
    }

    // check the character (and move forward)
    c = *tptr++;
    if(c == '\t')
    {
      ULONG t_length = 0;
      ULONG c_length = 0;

      // we found a TAB, calculate the number of fitting characters so far
      if(constrainingBitWidth <= SHRT_MAX)
      {
        ULONG l_chars;

        if(strLen0 != 0)
        {
          l_chars  = TextFit(rp, tptr0, strLen0, &textExtent, constrainingExtent, strDirection, constrainingBitWidth, constrainingBitHeight);
          t_length = textExtent.te_Width;
          c_length = TextLengthNew(rp, tptr0, strLen0, 0);
          result  += l_chars;
        }
      }
      else
      {
        LONG l_constBit = constrainingBitWidth;
        LONG l_length = 0;
        LONG l_chars;

        c_length += TextLengthNew(rp, tptr0, strLen0, 0);

        while(l_constBit > SHRT_MAX && strLen0 > 0)
        {
          l_chars   = TextFit(rp, tptr0, strLen0, &textExtent, constrainingExtent, strDirection, SHRT_MAX, constrainingBitHeight);
          l_length += textExtent.te_Width;

          l_constBit -= l_length;
          t_length   += l_length;
          strLen0 -= l_chars;
          tptr0   += l_chars;
          result  += l_chars;
        }

        if(strLen0 > 0)
        {
          l_chars   = TextFit(rp, tptr0, strLen0, &textExtent, constrainingExtent, strDirection, l_constBit, constrainingBitHeight);
          t_length += textExtent.te_Width;
          result   += l_chars;
        }
      }

      constrainingBitWidth -= c_length;
      te_width += t_length;

      // bail out if no space is left
      if(constrainingBitWidth <= 0)
        break;

      constrainingBitWidth -= tabSizePixels;
      te_width += tabSizePixels;
      result++;

      tptr0 = tptr;
      strLen0 = 0;
    }
    else
      strLen0++;

    strLen--;
  }
  while(TRUE);

  // Fill in the TextExtentNew structure:
  textExtentNew->te_Width  = te_width;
  textExtentNew->te_Height = te_width ? te_height : 0;

  textExtentNew->te_Extent.MinX = rp->cp_x;
  textExtentNew->te_Extent.MaxX = rp->cp_x + te_width;
  textExtentNew->te_Extent.MinY = rp->cp_y - rp->TxBaseline;
  textExtentNew->te_Extent.MaxY = textExtentNew->te_Extent.MinY + textExtentNew->te_Height;

  RETURN(result);
  return(result);
}

///
/// TextNew
void TextNew(struct RastPort *rp, const char *string, ULONG count, LONG tabSizePixels)
{
  const char *tptr = string;
  const char *tptr0 = string;
  LONG count0 = 0;

  ENTER();

  do
  {
    char c;

    if(count == 0)
    {
      // we parsed the string until the end
      // print out the remaining characters
      Text(rp, tptr0, count0);
      break;
    }

    // check the next character
    c = *tptr++;
    if(c == '\t')
    {
      // we found a TAB, print out the characters so far
      if(count0 != 0)
        Text(rp, tptr0, count0);

      tptr0 = tptr;
      count0 = 0;

      // advance the rastport's cursor position
      rp->cp_x += tabSizePixels;
    }
    else
      count0++;

    count--;
  }
  while(TRUE);

  LEAVE();
}

///
