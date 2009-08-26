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

#include <devices/clipboard.h>
#include <libraries/iffparse.h>

#include <clib/alib_protos.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/exec.h>
#include <proto/iffparse.h>
#include <proto/intuition.h>

#include "private.h"
#include "Debug.h"

BOOL InitClipboard(struct InstData *data, ULONG flags);
void EndClipSession(struct InstData *data);

#if defined(__MORPHOS__)
#include <proto/keymap.h>
#include <proto/locale.h>

///utf8_to_ansi()
static char *utf8_to_ansi(struct InstData *data, STRPTR src)
{
   static struct KeyMap *keymap;
   CONST_STRPTR ptr;
   STRPTR dst;
   ULONG octets, strlength;

   ENTER();

   keymap = AskKeyMapDefault();

   strlength = 0;
   ptr = src;

   do
   {
      WCHAR wc;
      UBYTE c;

      ptr += (octets = UTF8_Decode(ptr, &wc));
      c = ToANSI(wc, keymap);

      strlength++;

      /* ToANSI() returns '?' if there is not matching code point in the current keymap */
      if (c == '?' && wc != '?')
      {
         /* If direct conversion fails try compatibility decomposition (but without recursion) */
         CONST_WSTRPTR p = UCS4_Decompose(wc);

         if (p)
         {
            while (p[1])
            {
               strlength++;
               p++;
            }
         }
      }
   }
   while (octets > 0);

   dst = MyAllocPooled(data->mypool, strlength);

   if (dst)
   {
      STRPTR bufptr = dst;

      ptr = src;

      do
      {
         WCHAR wc;
         UBYTE c;

         ptr += (octets = UTF8_Decode(ptr, &wc));
         c = ToANSI(wc, keymap);

         *bufptr++ = c;

         if (c == '?' && wc != '?')
         {
            CONST_WSTRPTR p = UCS4_Decompose(wc);

            if (p)
            {
               bufptr--;

               while (*p)
               {
                  *bufptr++ = ToANSI(*p, keymap);
                  p++;
               }
            }
         }
      }
      while (octets > 0);

      MyFreePooled(data->mypool, src);   // Free original buffer
   }

   if(dst == NULL)
     dst = src;

   RETURN(dst);
   return dst;
}

///
#endif

/// PasteClip()
/*----------------------*
 * Paste from Clipboard *
 *----------------------*/
BOOL PasteClip(LONG x, struct line_node *actline, struct InstData *data)
{
  struct line_node *line = NULL;
  struct line_node *startline = NULL;
  struct line_node *previous = NULL;
  struct LineStyle *styles = NULL;
  struct LineColor *colors = NULL;
  STRPTR textline;
  BOOL newline = TRUE;
  BOOL res = FALSE;

  ENTER();

  if(InitClipboard(data, IFFF_READ))
  {
    if(StopChunk(data->iff, ID_FTXT, ID_CHRS) == 0 &&
       StopChunk(data->iff, ID_FTXT, ID_FLOW) == 0 &&
       StopChunk(data->iff, ID_FTXT, ID_HIGH) == 0 &&
       StopChunk(data->iff, ID_FTXT, ID_SBAR) == 0 &&
       StopChunk(data->iff, ID_FTXT, ID_COLS) == 0 &&
       StopChunk(data->iff, ID_FTXT, ID_STYL) == 0 &&
       StopChunk(data->iff, ID_FTXT, ID_CSET) == 0)
    {
      LONG error, codeset = 0;
      UWORD flow = MUIV_TextEditor_Flow_Left;
      UWORD color = FALSE;
      UWORD separator = 0;
      BOOL ownclip = FALSE;
      LONG updatefrom;

      while(TRUE)
      {
        struct ContextNode *cn;

        error = ParseIFF(data->iff, IFFPARSE_SCAN);
        SHOWVALUE(DBF_CLIPBOARD, error);
        if(error == IFFERR_EOC)
          continue;
        else if(error)
          break;

        if((cn = CurrentChunk(data->iff)) != NULL)
        {
          switch (cn->cn_ID)
          {
            case ID_CSET:
              D(DBF_CLIPBOARD, "reading FLOW");
              SHOWVALUE(DBF_CLIPBOARD, cn->cn_Size);
              if(cn->cn_Size >= 4)
              {
                /* Only the first four bytes are interesting */
                if(ReadChunkBytes(data->iff, &codeset, 4) != 4)
                {
                  codeset = 0;
                }
                SHOWVALUE(DBF_CLIPBOARD, codeset);
              }
              break;

            case ID_FLOW:
              D(DBF_CLIPBOARD, "reading FLOW");
              SHOWVALUE(DBF_CLIPBOARD, cn->cn_Size);
              if(cn->cn_Size == 2)
              {
                if(ReadChunkBytes(data->iff, &flow, 2) == 2)
                  if(flow > MUIV_TextEditor_Flow_Right)
                    flow = MUIV_TextEditor_Flow_Left;
                SHOWVALUE(DBF_CLIPBOARD, flow);
              }
              break;

            case ID_HIGH:
              D(DBF_CLIPBOARD, "reading HIGH");
              SHOWVALUE(DBF_CLIPBOARD, cn->cn_Size);
              if (cn->cn_Size == 2)
              {
                error = ReadChunkBytes(data->iff, &color, 2);
                SHOWVALUE(DBF_CLIPBOARD, color);
                SHOWVALUE(DBF_CLIPBOARD, error);
              }
              break;

            case ID_SBAR:
              D(DBF_CLIPBOARD, "reading SBAR");
              SHOWVALUE(DBF_CLIPBOARD, cn->cn_Size);
              if (cn->cn_Size == 2)
              {
                error = ReadChunkBytes(data->iff, &separator, 2);
                SHOWVALUE(DBF_CLIPBOARD, separator);
                SHOWVALUE(DBF_CLIPBOARD, error);
              }
              break;

            case ID_COLS:
              D(DBF_CLIPBOARD, "reading COLS");
              SHOWVALUE(DBF_CLIPBOARD, cn->cn_Size);
              if(colors != NULL)
              {
                MyFreePooled(data->mypool, colors);
                colors = NULL;
              }
              // allocate one word more than the chunk tell us, because we terminate the array with an additional value
              if(cn->cn_Size > 0 && (colors = (struct LineColor *)MyAllocPooled(data->mypool, cn->cn_Size + sizeof(struct LineColor))) != NULL)
              {
                error = ReadChunkBytes(data->iff, colors, cn->cn_Size);
                SHOWVALUE(DBF_CLIPBOARD, error);
                colors[cn->cn_Size / sizeof(struct LineColor)].column = 0xffff;
              }
              break;

            case ID_STYL:
              D(DBF_CLIPBOARD, "reading STYL");
              SHOWVALUE(DBF_CLIPBOARD, cn->cn_Size);
              ownclip = TRUE;
              if(styles != NULL)
              {
                MyFreePooled(data->mypool, styles);
                styles = NULL;
              }
              // allocate one word more than the chunk tell us, because we terminate the array with an additional value
              if(cn->cn_Size > 0 && (styles = (struct LineStyle *)MyAllocPooled(data->mypool, cn->cn_Size + sizeof(struct LineStyle))) != NULL)
              {
                error = ReadChunkBytes(data->iff, styles, cn->cn_Size);
                SHOWVALUE(DBF_CLIPBOARD, error);
                styles[cn->cn_Size / sizeof(struct LineStyle)].column = EOS;
              }
              break;

            case ID_CHRS:
            {
              D(DBF_CLIPBOARD, "reading CHRS");
              SHOWVALUE(DBF_CLIPBOARD, cn->cn_Size);

              data->HasChanged = TRUE;

              if(cn->cn_Size > 0 && !ownclip)
              {
                char *contents;
                ULONG length = cn->cn_Size;

                if((contents = (char *)MyAllocPooled(data->mypool, length + 1)) != NULL)
                {
                  error = ReadChunkBytes(data->iff, contents, length);
                  SHOWVALUE(DBF_CLIPBOARD, error);

                  if(contents[length - 1] != '\n')
                  {
                    newline = FALSE;
                  }
                  else
                  {
                    length--;
                  }
                  contents[length] = '\0';

                  #if defined(__MORPHOS__)
                  if (codeset == CODESET_UTF8)
                  {
                    if (IS_MORPHOS2)
                      contents = utf8_to_ansi(data, contents);
                  }
                  #endif

                  if((line = ImportText(contents, data, &ImPlainHook, data->ImportWrap)))
                  {
                    if(!startline)
                      startline = line;
                    if(previous)
                      previous->next  = line;

                    line->previous    = previous;
                    line->visual    = VisualHeight(line, data);
                    data->totallines += line->visual;
                    while(line->next)
                    {
                      line = line->next;
                      line->visual    = VisualHeight(line, data);
                      data->totallines += line->visual;
                    }
                    previous = line;
                  }
                  MyFreePooled(data->mypool, contents);
                }
              }
              else
              {
                ULONG length = cn->cn_Size;

                if(length > 0 && (textline = (char *)MyAllocPooled(data->mypool, length + 2)) != NULL)
                {
                  error = ReadChunkBytes(data->iff, textline, length);
                  SHOWVALUE(DBF_CLIPBOARD, error);

                  if (textline[length - 1] != '\n')
                  {
                    newline = FALSE;
                    textline[length] = '\n';
                    length++;
                  }
                  textline[length] = '\0';

                  if((line = AllocLine(data)))
                  {
                    line->next     = NULL;
                    line->previous   = previous;
                    line->line.Contents   = textline;
                    line->line.Length   = length;
                    line->visual   = VisualHeight(line, data);
                    line->line.Color    = color;
                    line->line.Flow     = flow;
                    line->line.Separator = separator;
                    line->line.Styles   = styles;
                    line->line.Colors   = colors;
                    data->totallines += line->visual;

                    if(!startline)
                      startline = line;
                    if(previous)
                      previous->next  = line;

                    previous = line;
                  }
                  else
                  {
                    if(styles != NULL)
                      MyFreePooled(data->mypool, styles);
                    if(colors != NULL)
                      MyFreePooled(data->mypool, colors);
                  }
                }
                else
                {
                  if(styles != NULL)
                    MyFreePooled(data->mypool, styles);
                  if(colors != NULL)
                    MyFreePooled(data->mypool, colors);
                }
                styles    = NULL;
                colors    = NULL;
                flow      = MUIV_TextEditor_Flow_Left;
                color     = FALSE;
                separator = 0;
                ownclip   = FALSE;
              }
            }
            break;
          }
        }
      }

      if(line)
      {
        BOOL oneline = FALSE;

        SplitLine(x, actline, FALSE, NULL, data);
        line->next = actline->next;
        actline->next->previous = line;
        actline->next = startline;
        startline->previous = actline;
        data->CPos_X = line->line.Length-1;
        if(actline->next == line)
        {
          data->CPos_X += actline->line.Length-1;
          oneline = TRUE;
        }
        if(!newline)
          MergeLines(line, data);
        MergeLines(actline, data);
        if(oneline)
          line = actline;
        if(newline)
        {
          line = line->next;
          data->CPos_X = 0;
        }
        data->actualline = line;
      }
      else
      {
        switch(error)
        {
          case IFFERR_MANGLED:
          case IFFERR_SYNTAX:
          case IFFERR_NOTIFF:
            D(DBF_CLIPBOARD, "no FTXT clip!");
            DoMethod(data->object, MUIM_TextEditor_HandleError, Error_ClipboardIsNotFTXT);
            break;
          default:
            D(DBF_CLIPBOARD, "clipboard is empty!");
            DoMethod(data->object, MUIM_TextEditor_HandleError, Error_ClipboardIsEmpty);
            break;
        }
      }
      data->update = TRUE;

      ScrollIntoDisplay(data);
      updatefrom = LineToVisual(actline, data)-1;
      if(updatefrom < 0)
        updatefrom = 0;
      DumpText(data->visual_y+updatefrom, updatefrom, data->maxlines, TRUE, data);

      if(data->update)
        res = TRUE;
      else
        data->update = TRUE;
    }

    EndClipSession(data);
  }

  RETURN(res);
  return res;
}

///
/// DumpLine()
#if defined(DEBUG)
static void DumpLine(struct line_node *line)
{
  ENTER();

  D(DBF_DUMP, "length %3ld, contents '%s'", line->line.Length, line->line.Contents);

  if(line->line.Styles != NULL)
  {
    struct LineStyle *styles = line->line.Styles;

    D(DBF_DUMP, "styles:");
    while(styles->column != EOS)
    {
      D(DBF_DUMP, "style %04lx starting at column %3ld", styles->style, styles->column);
      styles++;
    }
  }

  if(line->line.Colors != NULL)
  {
    struct LineColor *colors = line->line.Colors;

    D(DBF_DUMP, "colors:");
    while(colors->column != 0xffff)
    {
      D(DBF_DUMP, "color %3ld starting at column %3ld", colors->color, colors->column);
      colors++;
    }
  }

  LEAVE();
}
#endif

///
/// MergeLines()
/*--------------------------*
 * Merge two lines into one *
 *--------------------------*/
BOOL MergeLines(struct line_node *line, struct InstData *data)
{
  struct line_node *next;
  char  *newbuffer;
  LONG  visual, oldvisual, line_nr;
  LONG  emptyline = FALSE;
  LONG  color = line->line.Color;
  UWORD flow = line->line.Flow;
  UWORD separator = line->line.Separator;

  ENTER();

  data->HasChanged = TRUE;
  if(line->line.Length == 1)
  {
    emptyline = TRUE;
    color = line->next->line.Color;
    flow = line->next->line.Flow;
    separator = line->next->line.Separator;
  }
  visual = line->visual + line->next->visual;

  if((newbuffer = MyAllocPooled(data->mypool, line->line.Length+line->next->line.Length+1)) != NULL)
  {
    memcpy(newbuffer, line->line.Contents, line->line.Length-1);
    memcpy(newbuffer+line->line.Length-1, line->next->line.Contents, line->next->line.Length+1);
    MyFreePooled(data->mypool, line->line.Contents);
    MyFreePooled(data->mypool, line->next->line.Contents);

    if(emptyline == TRUE)
    {
      if(line->line.Styles != NULL)
        MyFreePooled(data->mypool, line->line.Styles);

      line->line.Styles = line->next->line.Styles;

      if(line->line.Colors != NULL)
        MyFreePooled(data->mypool, line->line.Colors);

      line->line.Colors = line->next->line.Colors;
    }
    else
    {
      struct LineStyle *styles;
      struct LineStyle *styles1 = line->line.Styles;
      struct LineStyle *styles2 = line->next->line.Styles;
      struct LineColor *colors;
      struct LineColor *colors1 = line->line.Colors;
      struct LineColor *colors2 = line->next->line.Colors;
      UWORD length = 12;

      if(styles1 != NULL)
        length += GetAllocSize(styles1);
      if(styles2 != NULL)
        length += GetAllocSize(styles2);

      if((styles = MyAllocPooled(data->mypool, length)) != NULL)
      {
        struct LineStyle* t_styles = styles;
        UWORD style = 0;

        SHOWVALUE(DBF_CLIPBOARD, length);
        SHOWVALUE(DBF_CLIPBOARD, styles);
        SHOWVALUE(DBF_CLIPBOARD, styles1);
        SHOWVALUE(DBF_CLIPBOARD, styles2);

        if(styles2 != NULL)
        {
          struct LineStyle* t_styles2 = styles2;

          while(t_styles2->column == 1)
          {
            if(t_styles->style > 0xff)
              style &= t_styles2->style;
            else
              style |= t_styles2->style;

            t_styles2++;
          }
        }

        if(styles1 != NULL)
        {
          while(styles1->column != EOS)
          {
            if(styles1->column == line->line.Length && ((~styles1->style & style) == (styles1->style  ^ 0xffff)))
            {
              style &= styles1->style;
              styles1++;
            }
            else
            {
              styles->column = styles1->column;
              styles->style = styles1->style;
              styles++;
              styles1++;
            }
          }
          SHOWVALUE(DBF_CLIPBOARD, line->line.Styles);
          MyFreePooled(data->mypool, line->line.Styles);
        }

        if(styles2 != NULL)
        {
          while(styles2->column != EOS)
          {
            if(styles2->column == 1 && isFlagClear(styles2->style, style))
            {
              styles2++;
            }
            else
            {
              styles->column = styles2->column + line->line.Length - 1;
              styles->style = styles2->style;
              styles++;
            }
          }
          SHOWVALUE(DBF_CLIPBOARD, line->next->line.Styles);
          MyFreePooled(data->mypool, line->next->line.Styles);
        }
        styles->column = EOS;
        line->line.Styles = t_styles;
      }

      length = 12;

      if(colors1 != NULL)
        length += GetAllocSize(colors1);
      if(colors2 != NULL)
        length += GetAllocSize(colors2);

      if((colors = MyAllocPooled(data->mypool, length)) != NULL)
      {
        struct LineColor *t_colors = colors;
        UWORD end_color = 0;

        SHOWVALUE(DBF_CLIPBOARD, length);
        SHOWVALUE(DBF_CLIPBOARD, colors);
        SHOWVALUE(DBF_CLIPBOARD, colors1);
        SHOWVALUE(DBF_CLIPBOARD, colors2);

        if(colors1 != NULL)
        {
          while(colors1->column < line->line.Length && colors1->column != 0xffff)
          {
            colors->column = colors1->column;
            end_color = colors1->color;
            colors->color = colors1->color;
            colors++;
            colors1++;
          }
          SHOWVALUE(DBF_CLIPBOARD, line->line.Colors);
          MyFreePooled(data->mypool, line->line.Colors);
        }

        if(end_color && (colors2 == NULL || colors2->column != 1))
        {
          colors->column = line->line.Length;
          colors->color = 0;
          colors++;
        }

        if(colors2 != NULL)
        {
          if(colors2->column == 1 && colors2->color == end_color)
            colors2++;

          while(colors2->column != 0xffff)
          {
            colors->column = colors2->column + line->line.Length - 1;
            colors->color = colors2->color;
            colors++;
            colors2++;
          }
          SHOWVALUE(DBF_CLIPBOARD, line->next->line.Colors);
          MyFreePooled(data->mypool, line->next->line.Colors);
        }
        colors->column = 0xffff;
        line->line.Colors = t_colors;
      }
    }

    line->line.Contents = newbuffer;
    line->line.Length  = strlen(newbuffer);

    next = line->next;
    line->next = line->next->next;
    if(line->next != NULL)
      line->next->previous = line;
    oldvisual = line->visual;
    line->visual = VisualHeight(line, data);
    line->line.Color = color;
    line->line.Flow = flow;
    line->line.Separator = separator;

    FreeLine(next, data);

    line_nr = LineToVisual(line, data);

    // handle that we have to scroll up/down due to word wrapping
    // that occurrs when merging lines
    if(visual > line->visual)
    {
      data->totallines -= 1;
      if(line_nr+line->visual-1 < data->maxlines)
      {
        if(emptyline && line_nr > 0)
        {
          if(data->fastbackground)
          {
            ScrollUp(line_nr - 1, 1, data);
            SetCursor(data->CPos_X, data->actualline, TRUE, data);
          }
          else
            DumpText(data->visual_y+line_nr-1, line_nr-1, data->maxlines, TRUE, data);
        }
        else
        {
          if(data->fastbackground)
            ScrollUp(line_nr + line->visual - 1, 1, data);
          else
            DumpText(data->visual_y+line_nr+line->visual-1, line_nr+line->visual-1, data->maxlines, TRUE, data);
        }
      }
    }
    else if(visual < line->visual)
    {
      data->totallines += 1;
      if(line_nr+line->visual-1 < data->maxlines)
        ScrollDown(line_nr + line->visual - 2, 1, data);
    }

    if(!(emptyline && (line_nr + line->visual - 1 < data->maxlines)))
    {
      LONG t_oldvisual = oldvisual;
      LONG t_line_nr   = line_nr;
      ULONG c = 0;

      while((--t_oldvisual) && (t_line_nr++ <= data->maxlines))
        c = c + LineCharsWidth(line->line.Contents+c, data);

      while((c < line->line.Length) && (t_line_nr <= data->maxlines))
        c = c + PrintLine(c, line, t_line_nr++, TRUE, data);
    }

    if(line_nr + oldvisual == 1 && line->visual == visual-1)
    {
      data->visual_y--;
      data->totallines -= 1;

      if(data->fastbackground)
        DumpText(data->visual_y, 0, visual-1, TRUE, data);
      else
        DumpText(data->visual_y, 0, data->maxlines, TRUE, data);
    }

    RETURN(TRUE);
    return(TRUE);
  }
  else
  {
    RETURN(FALSE);
    return(FALSE);
  }
}

///
/// SplitLine()
/*---------------------*
 * Split line into two *
 *---------------------*/
BOOL SplitLine(LONG x, struct line_node *line, BOOL move_crsr, struct UserAction *buffer, struct InstData *data)
{
  struct line_node *newline;
  struct line_node *next;
  struct pos_info pos;
  LONG line_nr, lines;
  ULONG c;
  UWORD crsr_x = data->CPos_X;
  struct line_node *crsr_l = data->actualline;

  ENTER();

  #if defined(DEBUG)
  D(DBF_DUMP, "before split");
  DumpLine(line);
  #endif

  OffsetToLines(x, line, &pos, data);
  lines = pos.lines;

  next = line->next;
  if((newline = AllocLine(data)) != NULL)
  {
    struct LineStyle *styles = line->line.Styles;
    struct LineStyle *newstyles = NULL;
    struct LineColor *colors = line->line.Colors;
    struct LineColor *newcolors = NULL;

    data->HasChanged = TRUE;
    Init_LineNode(newline, line, line->line.Contents+x, data);
    newline->line.Color = line->line.Color;
    newline->line.Flow = line->line.Flow;
    newline->line.Separator = line->line.Separator;
    if(buffer != NULL)
    {
      newline->line.Color = buffer->del.style;
      newline->line.Flow = buffer->del.flow;
      newline->line.Separator = buffer->del.separator;
    }

    if(styles != NULL)
    {
      UWORD style = 0;
      LONG  numStyles = 0;
      struct LineStyle *ostyles;

      while(styles->column <= x+1)
      {
        if(styles->style > 0xff)
          style &= styles->style;
        else
          style |= styles->style;

        styles++;
      }
      styles--;
      ostyles = styles;
      while(styles[numStyles].column != EOS)
        numStyles++;
      // we can get up to 4 new style changes
      numStyles += 4;

      if((newstyles = MyAllocPooled(data->mypool, numStyles * sizeof(struct LineStyle))) != NULL)
      {
        struct LineStyle *nstyles = newstyles;

        if(isFlagSet(style, BOLD))
        {
          nstyles->column = 1;
          nstyles->style = BOLD;
          nstyles++;
        }
        if(isFlagSet(style, ITALIC))
        {
          nstyles->column = 1;
          nstyles->style = ITALIC;
          nstyles++;
        }
        if(isFlagSet(style, UNDERLINE))
        {
          nstyles->column = 1;
          nstyles->style = UNDERLINE;
          nstyles++;
        }

        while(styles->column != EOS)
        {
          nstyles->column = styles->column - x;
          nstyles->style = styles->style;
          nstyles++;
          style++;
        }
        nstyles->column = EOS;
      }

      if(isFlagSet(style, BOLD))
      {
        ostyles->column = x+1;
        ostyles->style = ~BOLD;
      }
      if(isFlagSet(style, ITALIC))
      {
        ostyles->column = x+1;
        ostyles->style = ~ITALIC;
      }
      if(isFlagSet(style, UNDERLINE))
      {
        ostyles->column = x+1;
        ostyles->style = ~UNDERLINE;
      }
      if(x == 0)
        ostyles = line->line.Styles;

      ostyles->column = EOS;
    }
    newline->line.Styles = newstyles;

    if(colors != NULL)
    {
      UWORD color = GetColor(x, line);
      ULONG numColors = 0;
      struct LineColor *ocolors;

      while(colors->column <= x+1)
      {
        colors++;
      }
      ocolors = colors;

      while(colors[numColors].column != 0xffff)
        numColors++;
      // we can get up to 4 new color changes
      numColors += 4;

      if((newcolors = MyAllocPooled(data->mypool, numColors+sizeof(struct LineColor))) != NULL)
      {
        struct LineColor *ncolors = newcolors;

        if(color && colors->column-x != 1)
        {
          ncolors->column = 1;
          ncolors->color = color;
          ncolors++;
        }

        while(colors->column != 0xffff)
        {
          ncolors->column = colors->column - x;
          ncolors->color = colors->color;
          ncolors++;
          colors++;
        }
        ncolors->column = 0xffff;
      }
      if(x == 0)
        ocolors = line->line.Colors;
      ocolors->column = 0xffff;
    }
    newline->line.Colors = newcolors;


    newline->next = next;
    if(next != NULL)
      next->previous = newline;

    line->line.Contents[x] = '\n';
    line->line.Contents[x+1] = '\0';
    line->line.Length = x+1;

/*------------------*/
    c = line->visual;
    line->visual = VisualHeight(line, data);
    CompressLine(line, data);

    line_nr = LineToVisual(line, data) + line->visual - 1;
    if(line_nr < 0)
      line_nr = 0;

    if(move_crsr)
    {
      data->CPos_X = 0;
      data->actualline = data->actualline->next;
    }

    if(x == 0)
    {
      line->line.Color = 0;
      line->line.Separator = 0;
      if(!(line->previous && line->previous->line.Flow == line->line.Flow))
      {
        line->line.Flow = MUIV_TextEditor_Flow_Left;
      }
      if(line_nr != data->maxlines)
      {
        data->totallines += 1;
        if(data->fastbackground)
        {
          if(line_nr)
          {
            ScrollDown(line_nr-1, 1, data);
            PrintLine(0, line, line_nr, FALSE, data);
          }
          else
          {
            ScrollDown(line_nr, 1, data);
          }
        }
        else
          DumpText(data->visual_y+line_nr-1, line_nr-1, data->maxlines, TRUE, data);
      }
      else
      {
        data->visual_y++;
        data->totallines += 1;
        if(isFlagClear(data->flags, FLG_Quiet))
        {
          struct Hook *oldhook;

          oldhook = InstallLayerHook(data->rport->Layer, LAYERS_NOBACKFILL);
          ScrollRasterBF(data->rport, 0, data->height,
                    data->xpos, data->ypos,
                    data->xpos + data->innerwidth - 1, (data->ypos + ((data->maxlines-1) * data->height)) - 1);
          InstallLayerHook(data->rport->Layer, oldhook);

          PrintLine(0, line, data->maxlines-1, FALSE, data);
          if(!data->fastbackground)
          {
            DumpText(data->visual_y+data->maxlines-1, data->maxlines-1, data->maxlines, TRUE, data);
          }
        }
      }

      #if defined(DEBUG)
      D(DBF_DUMP, "after split, old line");
      DumpLine(line);
      D(DBF_DUMP, "after split, new line");
      DumpLine(newline);
      #endif

      RETURN(TRUE);
      return(TRUE);
    }

    if(x == (LONG)(line->line.Length + newline->line.Length - 2))
    {
      data->totallines += 1;
      if(buffer == NULL)
      {
        line->next->line.Color = 0;
        line->next->line.Separator = 0;
      }
      SetCursor(crsr_x, crsr_l, FALSE, data);
      if(line_nr < data->maxlines)
      {
        if(data->fastbackground)
        {
          ScrollDown(line_nr, 1, data);
          if(line_nr+1 <= data->maxlines)
            PrintLine(0, line->next, line_nr+1, FALSE, data);
        }
        else
          DumpText(data->visual_y+line_nr, line_nr, data->maxlines, TRUE, data);
      }

      #if defined(DEBUG)
      D(DBF_DUMP, "after split, old line");
      DumpLine(line);
      D(DBF_DUMP, "after split, new line");
      DumpLine(newline);
      #endif

      RETURN(TRUE);
      return(TRUE);
    }
    x = line->line.Length;

    OffsetToLines(x-1, line, &pos, data);
    if(((ULONG)(line->visual + line->next->visual) >= c) && (line->visual == lines))
    {
      if((ULONG)(line->visual + line->next->visual) > c)
        data->totallines += 1;

      PrintLine(pos.bytes, line, line_nr, TRUE, data);

      if((line_nr+line->next->visual-1 < data->maxlines) && ((ULONG)(line->visual + line->next->visual) > c))
      {
        ScrollDown(line_nr+line->next->visual-1, 1, data);
      }
    }
    else
    {
      PrintLine((x-1)-pos.x, line, line_nr, TRUE, data);

      if((line_nr < data->maxlines) && ((ULONG)(line->visual + line->next->visual) < c))
      {
        data->totallines -= 1;
        ScrollUp(line_nr, 1, data);
      }
    }
/*------------------*/
    line = line->next;
    line_nr++;
    c = 0;
    while((c < line->line.Length) && (line_nr <= data->maxlines))
      c = c + PrintLine(c, line, line_nr++, TRUE, data);
  /* Her printes !HELE! den nye linie, burde optimeres! */

    #if defined(DEBUG)
    D(DBF_DUMP, "after split, old line");
    DumpLine(line);
    D(DBF_DUMP, "after split, new line");
    DumpLine(newline);
    #endif

    RETURN(TRUE);
    return (TRUE);
  }
  else
  {
    RETURN(FALSE);
    return (FALSE);
  }
}

///
/// strcpyback()
/*------------------------------------------------------------------*
 * Backwards string copy, please replace with some assembler stuff! *
 *------------------------------------------------------------------*/
static void strcpyback(char *dest, char *src)
{
  size_t length;

  ENTER();

  length = strlen(src)+1;
  dest = dest + length;
  src = src + length;

  length++;
  while(--length)
    *--dest = *--src;

  LEAVE();
}

///
/// OptimizedPrint()
/* ------------------------------------ *
 *  Functions which updates the display *
 * ------------------------------------ */
void OptimizedPrint(LONG x, struct line_node *line, LONG line_nr, LONG width, struct InstData *data)
{
  ENTER();

  do
  {
    LONG twidth;

    twidth = PrintLine(x, line, line_nr, TRUE, data);
    line_nr++;

    if(twidth != width && x+twidth < (LONG)line->line.Length && line_nr <= data->maxlines)
    {
      x += twidth;
      width = LineCharsWidth(line->line.Contents+x+width, data) + width - twidth;
    }
    else
      break;
  }
  while(TRUE);

  LEAVE();
}

///
/// UpdateChange()
static void UpdateChange(LONG x, struct line_node *line, LONG length, const char *characters, struct UserAction *buffer, struct InstData *data)
{
  LONG diff;
  LONG skip=0;
  LONG line_nr;
  LONG orgline_nr;
  LONG width=0;
  LONG lineabove_width=0;

  ENTER();

  line_nr = LineToVisual(line, data);
  orgline_nr = line_nr;

  do
  {
    width = LineCharsWidth(line->line.Contents + skip, data);

    // don't exceed the line length!
    if(skip > (LONG)line->line.Length)
      break;

    if(width <= 0 || skip + width >= x)
      break;

    lineabove_width = width;
    skip += width;
    line_nr++;
  }
  while(TRUE);

  if(characters != NULL)
  {
    strcpyback(line->line.Contents+x+length, line->line.Contents+x);
    memcpy(line->line.Contents+x, characters, length);
    width += length;
    line->line.Length += length;
    if(buffer != NULL)
    {
      UWORD style = buffer->del.style;

      AddStyleToLine(x, line, 1, (style & BOLD) ? BOLD : ~BOLD, data);
      AddStyleToLine(x, line, 1, (style & ITALIC) ? ITALIC : ~ITALIC, data);
      AddStyleToLine(x, line, 1, (style & UNDERLINE) ? UNDERLINE : ~UNDERLINE, data);
      line->line.Flow = buffer->del.flow;
      line->line.Separator = buffer->del.separator;
    }
  }
  else
  {
    strlcpy(line->line.Contents+x, line->line.Contents+x+length, line->line.Length);
    width -= length;
    line->line.Length -= length;
  }

  diff = VisualHeight(line, data) - line->visual;
  if(diff != 0)
  {
    LONG movement;

    movement = orgline_nr + line->visual - 1;

    line->visual += diff;
    data->totallines += diff;

    if(diff > 0)
    {
      if(movement < data->maxlines)
        ScrollDown(movement, diff, data);
    }
    else
    {
      movement = orgline_nr + line->visual - 1;
      if(movement <= data->maxlines)
        ScrollUp(movement, -diff, data);
    }
  }

  if(orgline_nr != line_nr)
  {
    if(lineabove_width != LineCharsWidth(line->line.Contents+skip-lineabove_width, data))
    {
      LONG newwidth;

      newwidth = PrintLine(skip-lineabove_width, line, line_nr-1, TRUE, data) - lineabove_width;
      skip  += newwidth;
      width -= newwidth;
      if(skip >= (LONG)line->line.Length)
      {
        LEAVE();
        return;
      }
    }
  }

  OptimizedPrint(skip, line, line_nr, width, data);
  ScrollIntoDisplay(data);
  data->HasChanged = TRUE;

  LEAVE();
}

///
/// PasteChars()
/*------------------------------*
 * Paste n characters to a line *
 *------------------------------*/
BOOL PasteChars(LONG x, struct line_node *line, LONG length, const char *characters, struct UserAction *buffer, struct InstData *data)
{
  ENTER();

  if(line->line.Styles != NULL)
  {
    if(line->line.Styles[0].column != EOS)
    {
      ULONG c = 0;

      while(line->line.Styles[c].column <= x+1)
        c++;
      while(line->line.Styles[c].column != EOS)
      {
        line->line.Styles[c].column += length;
        c++;
      }
    }
  }

  if(line->line.Colors != NULL)
  {
    if(line->line.Colors[0].column != 0xffff)
    {
      ULONG c = 0;

      while(line->line.Colors[c].column <= x+1)
        c++;
      while(line->line.Colors[c].column != 0xffff)
      {
        line->line.Colors[c].column += length;
        c++;
      }
    }
  }

  if(GetAllocSize(line->line.Contents) < line->line.Length + length + 1)
  {
    if(ExpandLine(line, length, data) == FALSE)
    {
      RETURN(FALSE);
      return(FALSE);
    }
  }

  UpdateChange(x, line, length, characters, buffer, data);

  RETURN(TRUE);
  return(TRUE);
}

///
/// RemoveChars()
/*----------------------------*
 * Remove n chars from a line *
 *----------------------------*/
BOOL RemoveChars(LONG x, struct line_node *line, LONG length, struct InstData *data)
{
  ENTER();

  if(line->line.Styles != NULL)
  {
    if(line->line.Styles[0].column != EOS)
    {
      UWORD start_style = GetStyle(x-1, line);
      UWORD end_style = GetStyle(x+length, line);
      ULONG c = 0, store;

      while(line->line.Styles[c].column <= x)
        c++;

      if(start_style != end_style)
      {
        UWORD turn_off = start_style & ~end_style;
        UWORD turn_on  = end_style & ~start_style;

        if(isFlagSet(turn_off, BOLD))
        {
          line->line.Styles[c].column = x+1;
          line->line.Styles[c].style = ~BOLD;
          c++;
        }
        if(isFlagSet(turn_off, ITALIC))
        {
          line->line.Styles[c].column = x+1;
          line->line.Styles[c].style = ~ITALIC;
          c++;
        }
        if(isFlagSet(turn_off, UNDERLINE))
        {
          line->line.Styles[c].column = x+1;
          line->line.Styles[c].style = ~UNDERLINE;
          c++;
        }
        if(isFlagSet(turn_on, BOLD))
        {
          line->line.Styles[c].column = x+1;
          line->line.Styles[c].style = BOLD;
          c++;
        }
        if(isFlagSet(turn_on, ITALIC))
        {
          line->line.Styles[c].column = x+1;
          line->line.Styles[c].style = ITALIC;
          c++;
        }
        if(isFlagSet(turn_on, UNDERLINE))
        {
          line->line.Styles[c].column = x+1;
          line->line.Styles[c].style = UNDERLINE;
          c++;
        }
      }

      store = c;
      while(line->line.Styles[c].column <= x+length+1)
        c++;

      while(line->line.Styles[c].column != EOS)
      {
        line->line.Styles[store].column = line->line.Styles[c].column-length;
        line->line.Styles[store].style = line->line.Styles[c].style;
        c++;
        store++;
      }
      line->line.Styles[store].column = EOS;
    }
  }

  if(line->line.Colors != NULL)
  {
    if(line->line.Colors[0].column != 0xffff)
    {
      UWORD start_color = x ? GetColor(x-1, line) : 0;
      UWORD end_color = GetColor(x+length, line);
      ULONG c = 0, store;

      while(line->line.Colors[c].column <= x)
        c++;

      if(start_color != end_color)
      {
        line->line.Colors[c].column = x+1;
        line->line.Colors[c].column = end_color;
        c++;
      }

      store = c;
      while(line->line.Colors[c].column <= x+length+1)
        c++;

      while(line->line.Colors[c].column != 0xffff)
      {
        line->line.Colors[store].column = line->line.Colors[c].column-length;
        line->line.Colors[store].column = line->line.Colors[c].column;
        store++;
      }
      line->line.Colors[store].column = 0xffff;
    }
  }

  UpdateChange(x, line, length, NULL, NULL, data);

  RETURN(TRUE);
  return(TRUE);
}

///

