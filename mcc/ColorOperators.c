/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2009 by TextEditor.mcc Open Source Team

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

#include "private.h"

/// GetColor()
UWORD GetColor(UWORD x, struct line_node *line)
{
  UWORD color = 0;
  struct LineColor *colors = line->line.Colors;

  ENTER();

  if(colors != NULL)
  {
    while(colors->column <= x+1)
    {
      color = colors->color;
      colors++;
    }
  }

  RETURN(color);
  return(color);
}

///
/// AddColorToLine()
void AddColorToLine(UWORD x, struct line_node *line, UWORD length, UWORD color, struct InstData *data)
{
  struct LineColor *colors = line->line.Colors;
  struct LineColor *oldcolors = colors;
  struct LineColor *newcolors;

  ENTER();

  x++;

  if(colors != NULL)
    newcolors = MyAllocPooled(data->mypool, GetAllocSize(colors)+sizeof(struct LineColor)*4);
  else
    newcolors = MyAllocPooled(data->mypool, sizeof(struct LineColor)*8);

  if(newcolors != NULL)
  {
    UWORD oldcol = 0;

    line->line.Colors = newcolors;
    if(colors != NULL)
    {
      while(colors->column != EOC && colors->column < x)
      {
        newcolors->column = colors->column;
        oldcol = colors->color;
        newcolors->color = colors->color;
        newcolors++;
        colors++;
      }
    }
    if(color != oldcol)
    {
      newcolors->column = x;
      newcolors->color = color;
      newcolors++;
    }
    if(colors != NULL)
    {
      while(colors->column != EOC && colors->column <= x+length)
      {
        oldcol = colors->color;
        colors++;
      }
    }
    if(color != oldcol)
    {
      newcolors->column = x+length;
      newcolors->color = oldcol;
      newcolors++;
    }
    if(colors != NULL)
    {
      while(colors->column != EOC)
      {
        newcolors->column = colors->column;
        newcolors->color = colors->color;
        newcolors++;
        colors++;
      }
    }
    newcolors->column = EOC;

    if(oldcolors != NULL)
    {
      MyFreePooled(data->mypool, oldcolors);
    }
  }

  LEAVE();
}
///
/// AddColor()
VOID AddColor(struct marking *realblock, UWORD color, struct InstData *data)
{
  struct marking    newblock;
  struct line_node  *startline, *stopline;
  UWORD         startx, stopx;

  ENTER();

  data->HasChanged = TRUE;
  if(realblock->enabled && (realblock->startx != realblock->stopx || realblock->startline != realblock->stopline))
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
    AddColorToLine(startx, startline, stopx-startx, color, data);
  }
  else
  {
    struct  line_node *line = startline->next;

    AddColorToLine(startx, startline, startline->line.Length-startx-1, color, data);
    while(line != stopline)
    {
      AddColorToLine(0, line, line->line.Length-1, color, data);
      line = line->next;
    }
    AddColorToLine(0, line, stopx, color, data);
  }
  RedrawArea(startx, startline, stopx, stopline, data);

  LEAVE();
}

///

