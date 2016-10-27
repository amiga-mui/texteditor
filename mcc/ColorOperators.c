/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2016 TextEditor.mcc Open Source Team

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

#include <proto/exec.h>
#include <proto/graphics.h>

#include "private.h"
#include "Debug.h"

/// GetColor()
void GetColor(struct InstData *data, LONG x, struct line_node *line, struct TEColor *color)
{
  struct LineColor *colors = line->line.Colors;

  ENTER();

  SetDefaultColor(data, color);

  if(colors != NULL && x >= 0)
  {
    while(colors->column != EOC && colors->column <= x+1)
    {
      *color = colors->color;
      colors++;
    }
  }

  LEAVE();
}

///
/// AddColorToLine()
static void AddColorToLine(struct InstData *data, LONG x, struct line_node *line, LONG length, const struct TEColor *color)
{
  struct Grow colorGrow;
  struct LineColor *colors;
  struct TEColor oldcol;

  ENTER();

  SetDefaultColor(data, &oldcol);
  x++;

  InitGrow(&colorGrow, data->mypool, sizeof(struct LineColor));

  if((colors = line->line.Colors) != NULL)
  {
    // keep all color changes ahead of the new color
    while(colors->column != EOC && colors->column < x)
    {
      oldcol = colors->color;
      AddToGrow(&colorGrow, colors);
      colors++;
    }
  }
  // add the new color if it is different from the last one
  if(IsSameColor(color, &oldcol) == FALSE)
  {
    struct LineColor newColor;

    newColor.column = x;
    newColor.color = *color;
    AddToGrow(&colorGrow, &newColor);
  }
  // skip and forget all color changes in the new range
  if(colors != NULL)
  {
    while(colors->column != EOC && colors->column <= x+length)
    {
      oldcol = colors->color;
      colors++;
    }
  }
  // add another color change if the new color is different from the last skipped one within the range
  if(IsSameColor(color, &oldcol) == FALSE)
  {
    struct LineColor newColor;

    newColor.column = x+length;
    newColor.color = oldcol;
    AddToGrow(&colorGrow, &newColor);
  }
  // keep all color changes until the end of the line
  if(colors != NULL)
  {
    while(colors->column != EOC)
    {
      AddToGrow(&colorGrow, colors);
      colors++;
    }
  }

  // terminate the color array if we have any colors at all
  if(colorGrow.itemCount > 0)
  {
    struct LineColor newColor;

    newColor.column = EOC;
    newColor.color.color = 0;
    newColor.color.isRGB = FALSE;
    AddToGrow(&colorGrow, &newColor);
  }

  // the old colors are not needed anymore
  if(line->line.Colors != NULL)
    FreeVecPooled(data->mypool, line->line.Colors);

  line->line.Colors = (struct LineColor *)colorGrow.array;

  LEAVE();
}

///
/// AddColor()
void AddColor(struct InstData *data, struct marking *realblock, const struct TEColor *color)
{
  struct marking newblock;
  struct line_node *startline;
  struct line_node *stopline;
  LONG startx;
  LONG stopx;

  ENTER();

  data->HasChanged = TRUE;
//data->ChangeEvent = TRUE;    // Color changes won't make any difference to textlengths, that's why it is commented out!

  if(realblock->enabled == TRUE && (realblock->startx != realblock->stopx || realblock->startline != realblock->stopline))
  {
    NiceBlock(realblock, &newblock);
    startx    = newblock.startx;
    stopx     = newblock.stopx;
    startline = newblock.startline;
    stopline  = newblock.stopline;
  }
  else
  {
    startx    = data->CPos_X;
    stopx     = startx+1;
    startline = data->actualline;
    stopline  = startline;
  }

  if(startline == stopline)
  {
    AddColorToLine(data, startx, startline, stopx-startx, color);
  }
  else
  {
    struct line_node *line = GetNextLine(startline);

    AddColorToLine(data, startx, startline, startline->line.Length-startx-1, color);
    while(line != stopline)
    {
      AddColorToLine(data, 0, line, line->line.Length-1, color);
      line = GetNextLine(line);
    }
    AddColorToLine(data, 0, line, stopx, color);
  }
  RedrawArea(data, startx, startline, stopx, stopline);

  LEAVE();
}

///
/// SetDefaultColor()
void SetDefaultColor(struct InstData *data, struct TEColor *c)
{
  ENTER();

  if(data != NULL && data->rgbMode == TRUE)
  {
    c->color = data->textRGB;
    c->isRGB = TRUE;
  }
  else
  {
    c->color = 0;
    c->isRGB = FALSE;
  }

  LEAVE();
}

///
/// IsDefaultColor()
BOOL IsDefaultColor(struct InstData *data, const struct TEColor *c)
{
  BOOL isDefault;
  struct TEColor def;

  ENTER();

  SetDefaultColor(data, &def);
  isDefault = IsSameColor(c, &def);

  RETURN(isDefault);
  return isDefault;
}

///
/// ConvertSinglePenToRGB()
ULONG ConvertSinglePenToRGB(struct InstData *data, LONG pen)
{
  ULONG rgb3[3];
  ULONG rgb;

  ENTER();

  // get the RGB values of the pen and turn the pen based color change into a RGB base color change
  GetRGB32(_screen(data->object)->ViewPort.ColorMap, pen, 1, rgb3);
  rgb = 0xff000000 | ((rgb3[0] >> 24) & 0xff) << 16 | ((rgb3[1] >> 24) & 0xff) << 8 | ((rgb3[2] >> 24) & 0xff) << 0;

  RETURN(rgb);
  return rgb;
}
///
/// ConvertPensToRGB()
void ConvertPensToRGB(struct InstData *data)
{
  struct line_node *line;
  struct ColorMap *cm;

  ENTER();

  cm = _screen(data->object)->ViewPort.ColorMap;

  // iterate over all lines
  line = GetFirstLine(&data->linelist);
  while(line != NULL)
  {
    if(line->line.Colors != NULL)
    {
      struct LineColor *colors = line->line.Colors;

      // iterate over all color changes of the line
      while(colors->column != EOC)
      {
        if(IsRGBColor(&colors->color) == FALSE)
        {
          ULONG rgb3[3];

          // get the RGB values of the pen and turn the pen based color change into a RGB base color change
          GetRGB32(cm, ConvertPen(data, colors->color.color, FALSE), 1, rgb3);
          colors->color.color = 0xff000000 | ((rgb3[0] >> 24) & 0xff) << 16 | ((rgb3[1] >> 24) & 0xff) << 8 | ((rgb3[2] >> 24) & 0xff) << 0;
          colors->color.isRGB = TRUE;
        }

        colors++;
      }
    }

    line = GetNextLine(line);
  }

  LEAVE();
}

///
