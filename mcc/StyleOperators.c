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

#include <proto/intuition.h>

#include "private.h"

/// UpdateStyles()
void UpdateStyles(struct InstData *data)
{
  UWORD style;

  ENTER();

  if(Enabled(data))
  {
    struct marking newblock;

    NiceBlock(&data->blockinfo, &newblock);
    style = GetStyle(data->blockinfo.stopx - ((newblock.startx == data->blockinfo.startx && newblock.startline == data->blockinfo.startline) ? 1 : 0), data->blockinfo.stopline);
  }
  else
  {
    style = GetStyle(data->CPos_X, data->actualline);
  }

  if(style != data->style)
  {
    UWORD oldstyle = data->style;

    data->style = style;

    if(isFlagSet(style, BOLD))
    {
      if(isFlagClear(oldstyle, BOLD))
        set(data->object, MUIA_TextEditor_StyleBold, TRUE);
    }
    else
    {
      if(isFlagClear(oldstyle, BOLD))
        set(data->object, MUIA_TextEditor_StyleBold, FALSE);
    }
    if(isFlagSet(style, ITALIC))
    {
      if(isFlagClear(oldstyle, ITALIC))
        set(data->object, MUIA_TextEditor_StyleItalic, TRUE);
    }
    else
    {
      if(isFlagClear(oldstyle, ITALIC))
        set(data->object, MUIA_TextEditor_StyleItalic, FALSE);
    }
    if(isFlagSet(style, UNDERLINE))
    {
      if(isFlagClear(oldstyle, UNDERLINE))
        set(data->object, MUIA_TextEditor_StyleUnderline, TRUE);
    }
    else
    {
      if(isFlagClear(oldstyle, UNDERLINE))
        set(data->object, MUIA_TextEditor_StyleUnderline, FALSE);
    }
  }

  LEAVE();
}

///
/// GetStyle()
UWORD GetStyle(LONG x, struct line_node *line)
{
  UWORD style = 0;

  ENTER();

  if(line->line.Styles != NULL)
  {
    struct LineStyle *styles = line->line.Styles;

    while(styles->column <= x+1)
    {
      if(styles->style > 0xff)
        style &= styles->style;
      else
        style |= styles->style;

      styles++;
    }
  }

  RETURN(style);
  return(style);
}

///
/// AddStyleToLine()
void AddStyleToLine(LONG x, struct line_node *line, LONG length, UWORD style, struct InstData *data)
{
  struct LineStyle *styles = line->line.Styles;
  struct LineStyle *oldstyles = styles;
  struct LineStyle *newstyles;
  UWORD cur_style = 0;
  UWORD end_style = GetStyle(x+length, line);

  ENTER();

  x++;

  if(styles != NULL)
    newstyles = MyAllocPooled(data->mypool, GetAllocSize(styles)+sizeof(struct LineStyle)*4);
  else
    newstyles = MyAllocPooled(data->mypool, sizeof(struct LineStyle)*8);

  if(newstyles != NULL)
  {
    line->line.Styles = newstyles;
    if(styles != NULL)
    {
      while(styles->column != EOS && styles->column < x)
      {
        newstyles->column = styles->column;
        newstyles->style = styles->style;

        if(styles->style > 0xff)
          cur_style &= styles->style;
        else
          cur_style |= styles->style;

        newstyles++;
        styles++;
      }
    }
    if(style > 0xff)
    {
      if(cur_style & ~style)
      {
        newstyles->column = x;
        newstyles->style = style;
        newstyles++;
      }
    }
    else
    {
      if(!(cur_style & style))
      {
        newstyles->column = x;
        newstyles->style = style;
        newstyles++;
      }
    }
    if(styles != NULL)
    {
      while(styles->column != EOS && styles->column <= x+length)
      {
        if(styles->style != style && styles->style != (UWORD)~style)
        {
          newstyles->column = styles->column;
          newstyles->style = styles->style;
          newstyles++;
        }
        styles++;
      }
    }
    if(!(((style > 0xff) && (!(end_style & ~style))) ||
        ((style < 0xff) && ((end_style & style)))))
    {
      newstyles->column = x+length;
      newstyles->style = ~style;
      newstyles++;
    }

    if(styles != NULL)
    {
      while(styles->column != EOS)
      {
        newstyles->column = styles->column;
        newstyles->style = styles->style;
        newstyles++;
        styles++;
      }
    }
    newstyles->column = EOS;
    if(oldstyles != NULL)
    {
      MyFreePooled(data->mypool, oldstyles);
    }
  }

  LEAVE();
}

///
/// AddStyle()
void AddStyle(struct marking *realblock, UWORD style, long Set, struct InstData *data)
{
  struct  marking newblock;
  LONG  startx, stopx;
  struct  line_node *startline, *stopline;

  ENTER();

  if(!Set)
  {
    if(!(data->style & style))
    {
      LEAVE();
      return;
    }

    style = ~style;
  }
  else
  {
    if(data->style & style)
    {
      LEAVE();
      return;
    }
  }
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
    AddStyleToLine(startx, startline, stopx-startx, style, data);
  }
  else
  {
    struct line_node *line = startline->next;

    AddStyleToLine(startx, startline, startline->line.Length-startx-1, style, data);
    while(line != stopline)
    {
      AddStyleToLine(0, line, line->line.Length-1, style, data);
      line = line->next;
    }
    AddStyleToLine(0, line, stopx, style, data);
  }
  RedrawArea(startx, startline, stopx, stopline, data);

  if(style > 0xff)
      data->style &= style;
  else  data->style |= style;

  LEAVE();
}

///

