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

#include <string.h>

#include <exec/io.h>
#include <devices/clipboard.h>
#include <libraries/iffparse.h>

#include <clib/alib_protos.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/iffparse.h>

#include "private.h"

/// RedrawArea()
VOID RedrawArea(UWORD startx, struct line_node *startline, UWORD stopx, struct line_node *stopline, struct InstData *data)
{
  struct pos_info pos1, pos2;
  LONG line_nr1 = LineToVisual(startline, data) - 1;
  LONG line_nr2 = LineToVisual(stopline, data) - 1;

  ENTER();

  OffsetToLines(startx, startline, &pos1, data);

  if(stopx >= stopline->line.Length)
    stopx = stopline->line.Length-1;

  OffsetToLines(stopx, stopline, &pos2, data);

  if((line_nr1 += pos1.lines-1) < 0)
    line_nr1 = 0;
  if((line_nr2 += pos2.lines-1) >= data->maxlines)
    line_nr2 = data->maxlines-1;
  if(line_nr1 <= line_nr2)
  {
    DumpText(data->visual_y+line_nr1, line_nr1, line_nr2+1, TRUE, data);
  }

  LEAVE();
}

///
/// GetBlock()
char *GetBlock(struct marking *block, struct InstData *data)
{
  LONG    startx, stopx;
  struct  line_node *startline, *stopline, *act;
  char    *text = NULL;
  struct  ExportMessage emsg;

  ENTER();

  startx    = block->startx;
  stopx     = block->stopx;
  startline = block->startline;
  stopline  = block->stopline;

  data->CPos_X = startx;
  data->actualline = startline;

  // clear the export message
  memset(&emsg, 0, sizeof(struct ExportMessage));

  // fill it afterwards
  emsg.UserData = NULL;
  emsg.ExportWrap = 0;
  emsg.Last = FALSE;
  emsg.data = data;

  if(startline != stopline)
  {
    /* Create a firstline look-a-like */
    emsg.Contents = (STRPTR)MyAllocPooled(data->mypool, startline->line.Length-startx);
    if(startline->line.Styles != NULL && startline->line.Styles[0].column != EOS)
    {
      UWORD startstyle = GetStyle(startx, startline);

      if((emsg.Styles = (struct LineStyle *)MyAllocPooled(data->mypool, GetAllocSize(startline->line.Styles)+sizeof(struct LineStyle)*5)))
      {
        struct LineStyle *styles = emsg.Styles;
        struct LineStyle *oldstyles = startline->line.Styles;

        if(isFlagSet(startstyle, BOLD))
        {
          styles->column = 1;
          styles->style = BOLD;
          styles++;
        }
        if(isFlagSet(startstyle, ITALIC))
        {
          styles->column = 1;
          styles->style = ITALIC;
          styles++;
        }
        if(isFlagSet(startstyle, UNDERLINE))
        {
          styles->column = 1;
          styles->style = UNDERLINE;
          styles++;
        }

        while(oldstyles->column <= startx)
          oldstyles++;

        while(oldstyles->column != EOS)
        {
          styles->column = oldstyles->column - startx;
          styles->style = oldstyles->style;
          styles++;
          oldstyles++;
        }
        styles->column = EOS;
      }
    }
    else
      emsg.Styles = NULL;

    emsg.Colors = NULL;
    if(emsg.Contents != NULL)
    {
      memcpy(emsg.Contents, startline->line.Contents + startx, startline->line.Length - startx);
      emsg.Length = startline->line.Length - startx;
      emsg.Flow = startline->line.Flow;
      emsg.Separator = startline->line.Separator;
      emsg.Highlight = startline->line.Color;
      emsg.UserData = (APTR)CallHookA(&ExportHookPlain, NULL, &emsg);
      MyFreePooled(data->mypool, emsg.Contents);
    }

    if(emsg.Styles != NULL)
      MyFreePooled(data->mypool, emsg.Styles);

    /* Start iterating... */
    act = startline->next;
    while(act != stopline)
    {
      emsg.Contents = act->line.Contents;
      emsg.Length   = act->line.Length;
      emsg.Styles   = act->line.Styles;
      emsg.Colors   = act->line.Colors;
      emsg.Flow   = act->line.Flow;
      emsg.Separator = act->line.Separator;
      emsg.Highlight = act->line.Color;
      emsg.UserData = (APTR)CallHookA(&ExportHookPlain, (APTR)NULL, &emsg);
      act = act->next;
    }

    /* Create a Lastline look-a-like */
    emsg.Contents = (STRPTR)MyAllocPooled(data->mypool, stopx);
    if(stopline->line.Styles != NULL && stopline->line.Styles->column != EOS)
    {
      UWORD stopstyle = GetStyle(stopx, stopline);

      if((emsg.Styles = (struct LineStyle *)MyAllocPooled(data->mypool, GetAllocSize(stopline->line.Styles) + sizeof(struct LineStyle)*5)))
      {
        struct LineStyle *styles = emsg.Styles;
        struct LineStyle *oldstyles = stopline->line.Styles;

        while(oldstyles->column <= stopx)
        {
          styles->column = oldstyles->column;
          styles->style = oldstyles->style;
          styles++;
          oldstyles++;
        }

        if(isFlagSet(stopstyle, BOLD))
        {
          styles->column = stopx+1;
          styles->style = ~BOLD;
          styles++;
        }
        if(isFlagSet(stopstyle, ITALIC))
        {
          styles->column = stopx+1;
          styles->style = ~ITALIC;
          styles++;
        }
        if(isFlagSet(stopstyle, UNDERLINE))
        {
          styles->column = stopx+1;
          styles->style = ~UNDERLINE;
          styles++;
        }
        styles->column = EOS;
      }
    }
    else
      emsg.Styles = NULL;

    emsg.Colors = NULL;
    if(emsg.Contents != NULL)
    {
      memcpy(emsg.Contents, stopline->line.Contents, stopx);
      emsg.Length = stopx;
      emsg.Flow = stopline->line.Flow;
      emsg.Separator = stopline->line.Separator;
      emsg.Highlight = stopline->line.Color;
      emsg.Last = TRUE;
      text = (STRPTR)CallHookA(&ExportHookPlain, NULL, &emsg);
      MyFreePooled(data->mypool, emsg.Contents);
    }

    if(emsg.Styles != NULL)
      MyFreePooled(data->mypool, emsg.Styles);
  }
  else
  {
    /* Create a single line */
    emsg.Contents = (STRPTR)MyAllocPooled(data->mypool, stopx-startx);
    if(startline->line.Styles != NULL && startline->line.Styles->column != EOS)
    {
      UWORD startstyle = GetStyle(startx, startline);
      UWORD stopstyle = GetStyle(stopx, stopline);

      if((emsg.Styles = (struct LineStyle *)MyAllocPooled(data->mypool, GetAllocSize(startline->line.Styles) + sizeof(struct LineStyle)*5)))
      {
        struct LineStyle *styles = emsg.Styles;
        struct LineStyle *oldstyles = startline->line.Styles;

        if(isFlagSet(startstyle, BOLD))
        {
          styles->column = 1;
          styles->style = BOLD;
          styles++;
        }
        if(isFlagSet(startstyle, ITALIC))
        {
          styles->column = 1;
          styles->style = ITALIC;
          styles++;
        }
        if(isFlagSet(startstyle, UNDERLINE))
        {
          styles->column = 1;
          styles->style = UNDERLINE;
          styles++;
        }

        while(oldstyles->column <= startx)
          oldstyles++;

        while(oldstyles->column <= stopx)
        {
          styles->column = oldstyles->column - startx;
          styles->style = oldstyles->style;
          styles++;
          oldstyles++;
        }

        if(isFlagSet(stopstyle, BOLD))
        {
          styles->column = stopx-startx+1;
          styles->style = ~BOLD;
          styles++;
        }
        if(isFlagSet(stopstyle, ITALIC))
        {
          styles->column = stopx-startx+1;
          styles->style = ~ITALIC;
          styles++;
        }
        if(isFlagSet(stopstyle, UNDERLINE))
        {
          styles->column = stopx-startx+1;
          styles->style = ~UNDERLINE;
          styles++;
        }
        styles->column = EOS;
      }
    }
    else
      emsg.Styles = NULL;

    emsg.Colors = NULL;
    if(emsg.Contents != NULL)
    {
      memcpy(emsg.Contents, startline->line.Contents+startx, stopx-startx);
      emsg.Length = stopx-startx;
      emsg.Flow = startline->line.Flow;
      emsg.Separator = startline->line.Separator;
      emsg.Highlight = startline->line.Color;
      emsg.Last = TRUE;
      text = (STRPTR)CallHookA(&ExportHookPlain, NULL, &emsg);
      MyFreePooled(data->mypool, emsg.Contents);
    }

    if(emsg.Styles != NULL)
      MyFreePooled(data->mypool, emsg.Styles);
  }

  RETURN(text);
  return(text);
}

///
/// NiceBlock()
void NiceBlock(struct marking *realblock, struct marking *newblock)
{
  LONG  startx = realblock->startx, stopx = realblock->stopx;
  struct line_node *startline = realblock->startline;
  struct line_node *stopline = realblock->stopline;

  ENTER();

  if(startline == stopline)
  {
    if(startx > stopx)
    {
      LONG c_x = startx;

      startx = stopx;
      stopx = c_x;
    }
  }
  else
  {
    struct  line_node *c_startline = startline,
                      *c_stopline = stopline;

    while((c_startline != stopline) && (c_stopline != startline))
    {
      if(c_startline->next)
        c_startline = c_startline->next;
      if(c_stopline->next)
        c_stopline = c_stopline->next;
    }

    if(c_stopline == startline)
    {
      LONG  c_x = startx;

      startx = stopx;
      stopx = c_x;

      c_startline = startline;
      startline = stopline;
      stopline = c_startline;
    }
  }
  newblock->startx    = startx;
  newblock->stopx     = stopx;
  newblock->startline = startline;
  newblock->stopline  = stopline;

  LEAVE();
}

///
/// InitClipboard()
BOOL InitClipboard(struct InstData *data, ULONG flags)
{
  struct IFFHandle *iff;

  ENTER();

  if((iff = AllocIFF()) != NULL)
  {
    SHOWVALUE(DBF_CLIPBOARD, iff);
    if((iff->iff_Stream = (ULONG)OpenClipboard(0)) != 0)
    {
      SHOWVALUE(DBF_CLIPBOARD, iff->iff_Stream);
      InitIFFasClip(iff);

      if(OpenIFF(iff, flags) == 0)
      {
        data->iff = iff;

        SHOWVALUE(DBF_CLIPBOARD, flags);

        RETURN(TRUE);
        return TRUE;
      }

      CloseClipboard((struct ClipboardHandle *)iff->iff_Stream);
    }

    FreeIFF(iff);
  }

  RETURN(FALSE);
  return(FALSE);
}

///
/// EndClipSession()
void EndClipSession(struct InstData *data)
{
  ENTER();

  if(data->iff != NULL)
  {
    CloseIFF(data->iff);

    CloseClipboard((struct ClipboardHandle *)data->iff->iff_Stream);

    FreeIFF(data->iff);
    data->iff = NULL;
  }

  LEAVE();
}

///
/// ClipInfo()
void ClipInfo(struct line_node *line, struct InstData *data)
{
  LONG error;

  ENTER();

  if(line->line.Flow != MUIV_TextEditor_Flow_Left)
  {
    D(DBF_CLIPBOARD, "writing FLOW");
    error = PushChunk(data->iff, 0, ID_FLOW, IFFSIZE_UNKNOWN);
    SHOWVALUE(DBF_CLIPBOARD, error);
    error = WriteChunkBytes(data->iff, &line->line.Flow, 2);
    SHOWVALUE(DBF_CLIPBOARD, error);
    error = PopChunk(data->iff);
    SHOWVALUE(DBF_CLIPBOARD, error);
  }

  if(line->line.Separator)
  {
    D(DBF_CLIPBOARD, "writing SBAR");
    error = PushChunk(data->iff, 0, ID_SBAR, IFFSIZE_UNKNOWN);
    SHOWVALUE(DBF_CLIPBOARD, error);
    error = WriteChunkBytes(data->iff, &line->line.Separator, 2);
    SHOWVALUE(DBF_CLIPBOARD, error);
    error = PopChunk(data->iff);
    SHOWVALUE(DBF_CLIPBOARD, error);
  }

  if(line->line.Color)
  {
    D(DBF_CLIPBOARD, "writing HIGH");
    error = PushChunk(data->iff, 0, ID_HIGH, IFFSIZE_UNKNOWN);
    SHOWVALUE(DBF_CLIPBOARD, error);
    error = WriteChunkBytes(data->iff, &line->line.Color, 2);
    SHOWVALUE(DBF_CLIPBOARD, error);
    error = PopChunk(data->iff);
    SHOWVALUE(DBF_CLIPBOARD, error);
  }

  LEAVE();
}

///
/// ClipChars()
void ClipChars(LONG x, struct line_node *line, LONG length, struct InstData *data)
{
  struct LineStyle style = {1, GetStyle(x-1, line)};
  struct LineColor color = {1, 0};
  struct LineColor *colors = line->line.Colors;
  LONG error;

  ENTER();

  D(DBF_CLIPBOARD, "ClipChars()");

  ClipInfo(line, data);

  if(colors)
  {
    D(DBF_CLIPBOARD, "writing COLS");
    error = PushChunk(data->iff, 0, ID_COLS, IFFSIZE_UNKNOWN);
    SHOWVALUE(DBF_CLIPBOARD, error);

    while(colors->column <= x && colors->column != EOC)
    {
      color.color = colors->color;
      colors++;
    }

    if(color.color != 0 && colors->column-x != 1)
    {
      error = WriteChunkBytes(data->iff, &color, sizeof(color));
      SHOWVALUE(DBF_CLIPBOARD, error);
    }

    if(colors->column != EOC)
    {
      while(colors->column <= x+length)
      {
        color.column = colors->column - x;
        color.color = colors->color;
        colors++;

        error = WriteChunkBytes(data->iff, &color, sizeof(color));
        SHOWVALUE(DBF_CLIPBOARD, error);
      }
    }

    error = PopChunk(data->iff);
    SHOWVALUE(DBF_CLIPBOARD, error);
  }

  D(DBF_CLIPBOARD, "writing STYL");
  error = PushChunk(data->iff, 0, ID_STYL, IFFSIZE_UNKNOWN);
  SHOWVALUE(DBF_CLIPBOARD, error);

  if(style.style != 0)
  {
    UWORD t_style = style.style;

    if(isFlagSet(t_style, BOLD))
    {
      style.style = BOLD;
      error = WriteChunkBytes(data->iff, &style, sizeof(style));
      SHOWVALUE(DBF_CLIPBOARD, error);
    }
    if(isFlagSet(t_style, ITALIC))
    {
      style.style = ITALIC;
      error = WriteChunkBytes(data->iff, &style, sizeof(style));
      SHOWVALUE(DBF_CLIPBOARD, error);
    }
    if(isFlagSet(t_style, UNDERLINE))
    {
      style.style = UNDERLINE;
      error = WriteChunkBytes(data->iff, &style, sizeof(style));
      SHOWVALUE(DBF_CLIPBOARD, error);
    }
  }

  if(line->line.Styles != NULL)
  {
    struct LineStyle *styles = line->line.Styles;

    while(styles->column <= x && styles->column != EOS)
      styles++;

    if(styles->column != EOS)
    {
      while(styles->column <= x+length)
      {
        style.column = styles->column - x;
        style.style = styles->style;
        styles++;
        error = WriteChunkBytes(data->iff, &style, sizeof(style));
        SHOWVALUE(DBF_CLIPBOARD, error);
      }

      style.column = length+1;
      style.style = GetStyle(x+length-1, line);
      if(style.style != 0)
      {
        UWORD t_style = style.style;

        if(isFlagSet(t_style, BOLD))
        {
          style.style = ~BOLD;
          error = WriteChunkBytes(data->iff, &style, sizeof(style));
          SHOWVALUE(DBF_CLIPBOARD, error);
        }
        if(isFlagSet(t_style, ITALIC))
        {
          style.style = ~ITALIC;
          error = WriteChunkBytes(data->iff, &style, sizeof(style));
          SHOWVALUE(DBF_CLIPBOARD, error);
        }
        if(isFlagSet(t_style, UNDERLINE))
        {
          style.style = ~UNDERLINE;
          error = WriteChunkBytes(data->iff, &style, sizeof(style));
          SHOWVALUE(DBF_CLIPBOARD, error);
        }
      }
    }
  }

  error = PopChunk(data->iff);
  SHOWVALUE(DBF_CLIPBOARD, error);

  D(DBF_CLIPBOARD, "writing CHRS");
  error = PushChunk(data->iff, 0, ID_CHRS, IFFSIZE_UNKNOWN);
  SHOWVALUE(DBF_CLIPBOARD, error);
  error = WriteChunkBytes(data->iff, line->line.Contents + x, length);
  SHOWVALUE(DBF_CLIPBOARD, error);
  error = PopChunk(data->iff);
  SHOWVALUE(DBF_CLIPBOARD, error);

  LEAVE();
}

///
/// ClipLine()
void ClipLine(struct line_node *line, struct InstData *data)
{
  struct LineStyle *styles = line->line.Styles;
  struct LineColor *colors = line->line.Colors;
  LONG error;

  ENTER();

  D(DBF_CLIPBOARD, "ClipLine()");

  ClipInfo(line, data);

  if(colors != NULL)
  {
    LONG numColors = 0;

    D(DBF_CLIPBOARD, "writing COLS");
    error = PushChunk(data->iff, 0, ID_COLS, IFFSIZE_UNKNOWN);
    SHOWVALUE(DBF_CLIPBOARD, error);

    while(colors->column != EOC)
    {
      colors++;
      numColors++;
    }

    error = WriteChunkBytes(data->iff, line->line.Colors, numColors * sizeof(*colors));
    SHOWVALUE(DBF_CLIPBOARD, error);
    error = PopChunk(data->iff);
    SHOWVALUE(DBF_CLIPBOARD, error);
  }

  if(styles != NULL)
  {
    LONG numStyles = 0;

    D(DBF_CLIPBOARD, "writing STYL");
    error = PushChunk(data->iff, 0, ID_STYL, IFFSIZE_UNKNOWN);
    SHOWVALUE(DBF_CLIPBOARD, error);

    while(styles->column != EOS)
    {
      styles++;
      numStyles++;
    }

    error = WriteChunkBytes(data->iff, line->line.Styles, numStyles * sizeof(*styles));
    SHOWVALUE(DBF_CLIPBOARD, error);
    error = PopChunk(data->iff);
    SHOWVALUE(DBF_CLIPBOARD, error);
  }

  D(DBF_CLIPBOARD, "writing CHRS");
  error = PushChunk(data->iff, 0, ID_CHRS, IFFSIZE_UNKNOWN);
  SHOWVALUE(DBF_CLIPBOARD, error);
  error = WriteChunkBytes(data->iff, line->line.Contents, line->line.Length);
  SHOWVALUE(DBF_CLIPBOARD, error);
  error = PopChunk(data->iff);
  SHOWVALUE(DBF_CLIPBOARD, error);

  LEAVE();
}

///
/// CutBlock()
LONG CutBlock(struct InstData *data, BOOL Clipboard, BOOL NoCut, BOOL update)
{
  struct  marking newblock;
  LONG result;

  ENTER();

  //D(DBF_STARTUP, "CutBlock: %ld %ld %ld", Clipboard, NoCut, update);

  NiceBlock(&data->blockinfo, &newblock);
  if(!NoCut)
    AddToUndoBuffer(ET_DELETEBLOCK, (char *)&newblock, data);

  result = CutBlock2(data, Clipboard, NoCut, &newblock, update);

  RETURN(result);
  return(result);
}

///
/// CutBlock2()
LONG CutBlock2(struct InstData *data, BOOL Clipboard, BOOL NoCut, struct marking *newblock, BOOL update)
{
  LONG  tvisual_y, error;
  LONG  startx, stopx;
  LONG  res = 0;
  struct  line_node *startline, *stopline;

  ENTER();

  startx    = newblock->startx;
  stopx     = newblock->stopx;
  startline = newblock->startline;
  stopline  = newblock->stopline;

  //D(DBF_STARTUP, "CutBlock2: %ld-%ld %lx-%lx %ld %ld", startx, stopx, startline, stopline, Clipboard, NoCut);

  if(startline != stopline)
  {
    struct line_node *c_startline = startline->next;

    data->update = FALSE;

    if(Clipboard == TRUE)
    {
      if(InitClipboard(data, IFFF_WRITE))
      {
        D(DBF_CLIPBOARD, "writing FORM");
        error = PushChunk(data->iff, ID_FTXT, ID_FORM, IFFSIZE_UNKNOWN);
        SHOWVALUE(DBF_CLIPBOARD, error);

        ClipChars(startx, startline, startline->line.Length-startx, data);
      }
      else
      {
        Clipboard = FALSE;
      }
    }

    while(c_startline != stopline)
    {
      if(Clipboard == TRUE)
      {
        ClipLine(c_startline, data);
      }

      if(NoCut == FALSE)
      {
        struct line_node *cc_startline = c_startline;

        MyFreePooled(data->mypool, c_startline->line.Contents);
        if(c_startline->line.Styles != NULL)
          MyFreePooled(data->mypool, c_startline->line.Styles);
        data->totallines -= c_startline->visual;
        c_startline = c_startline->next;

        //D(DBF_STARTUP, "FreeLine %08lx", cc_startline);

        FreeLine(cc_startline, data);
      }
      else
        c_startline = c_startline->next;
    }

    if(Clipboard == TRUE)
    {
      if(stopx != 0)
        ClipChars(0, stopline, stopx, data);

      EndClipSession(data);
    }

    if(NoCut == FALSE)
    {
      startline->next = stopline;
      stopline->previous = startline;

      //D(DBF_STARTUP, "RemoveChars: %ld %ld %08lx %ld", startx, stopx, startline, startline->line.Length);

      if(startline->line.Length-startx-1 > 0)
        RemoveChars(startx, startline, startline->line.Length-startx-1, data);

      if(stopx != 0)
        RemoveChars(0, stopline, stopx, data);

      data->CPos_X = startx;
      data->actualline = startline;
      MergeLines(startline, data);
    }
  }
  else
  {
    if(Clipboard == TRUE)
    {
      if(InitClipboard(data, IFFF_WRITE))
      {
        D(DBF_CLIPBOARD, "writing FORM");
        error = PushChunk(data->iff, ID_FTXT, ID_FORM, IFFSIZE_UNKNOWN);
        SHOWVALUE(DBF_CLIPBOARD, error);

        ClipChars(startx, startline, stopx-startx, data);
        EndClipSession(data);
      }

      if(update == TRUE && NoCut == TRUE)
      {
        MarkText(data->blockinfo.startx, data->blockinfo.startline, data->blockinfo.stopx, data->blockinfo.stopline, data);
          goto end;
      }
    }

    if(NoCut == FALSE)
    {
      data->CPos_X = startx;
      RemoveChars(startx, startline, stopx-startx, data);
      if(update == TRUE)
        goto end;
    }
  }

  tvisual_y = LineToVisual(startline, data)-1;
  if(tvisual_y < 0 || tvisual_y > data->maxlines)
  {
    //D(DBF_STARTUP, "ScrollIntoDisplay");
    ScrollIntoDisplay(data);
    tvisual_y = 0;
  }

  if(update == TRUE)
  {
    //D(DBF_STARTUP, "DumpText! %ld %ld %ld", data->visual_y, tvisual_y, data->maxlines);
    data->update = TRUE;
    DumpText(data->visual_y+tvisual_y, tvisual_y, data->maxlines, TRUE, data);
  }
  res = tvisual_y;

end:

  RETURN(res);
  return res;
}

///

