/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005 by TextEditor.mcc Open Source Team

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

#include <stdio.h>
#include <string.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/iffparse.h>
#include <proto/layers.h>
#include <graphics/text.h>
#include <dos/dostags.h>

#include "TextEditor_mcc.h"
#include "private.h"

BOOL InitClipboard (struct InstData *);

/*----------------------*
 * Paste from Clipboard *
 *----------------------*/
#ifdef ClassAct

struct PasteArgs
{
  LONG x;
  struct line_node *actline;
  struct InstData *data;
  struct Task *task;
  LONG sigbit;
  LONG res;
};

VOID PasteClipProcess (REG(a0) STRPTR arguments);

LONG PasteClip (LONG x, struct line_node *actline, struct InstData *data)
{
  struct PasteArgs args = { x, actline, data, FindTask(NULL), AllocSignal(-1) };
  if(args.sigbit != -1)
  {
    UBYTE str_args[10];
    sprintf(str_args, "%lx", &args);

    ReleaseGIRPort(data->rport);
    ReleaseSemaphore(&data->semaphore);
    if(CreateNewProcTags(NP_Entry, PasteClipProcess, NP_Name, "Texteditor slave", NP_StackSize, 2*4096, NP_Arguments, str_args, TAG_DONE))
      Wait(1 << args.sigbit);
    FreeSignal(args.sigbit);
    ObtainSemaphore(&data->semaphore);
    data->rport = ObtainGIRPort(data->GInfo);
  }
  return args.res;
}

VOID PasteClipProcess (REG(a0) STRPTR arguments)
{
  struct PasteArgs *args;
  if(sscanf(arguments, "%x", &args))
  {
    LONG x = args->x;
    struct line_node *actline = args->actline;
    struct InstData *data = args->data;
#else
LONG PasteClip (LONG x, struct line_node *actline, struct InstData *data)
{
#endif
    struct line_node *line = NULL;
    struct line_node *startline = NULL;
    struct line_node *previous = NULL;
    ULONG   header[3];
    LONG    length;
    UWORD   *styles = NULL;
    UWORD   *colors = NULL;
    STRPTR  textline;
    BOOL    newline = TRUE;
    LONG    res = FALSE;

#ifdef ClassAct
  ObtainSemaphore(&data->semaphore);
  data->rport = ObtainGIRPort(data->GInfo);
#endif

  if(InitClipboard(data))
  {
    data->clipboard->io_ClipID    = 0;
    data->clipboard->io_Command = CMD_READ;
    data->clipboard->io_Offset    = 0;
  
    data->clipboard->io_Data    = (void *)header;
    data->clipboard->io_Length    = 12;
    DoIO(data->iorequest);
  
    if(data->clipboard->io_Actual != 12)
    {
      DoMethod(data->object, MUIM_TextEditor_HandleError, Error_ClipboardIsEmpty);
    }
    else
    {
      length = header[1] - 4;
      if((header[0] == MAKE_ID('F','O','R','M')) && (header[2] == MAKE_ID('F','T','X','T')))
      {
          UWORD flow = MUIV_TextEditor_Flow_Left;
          UWORD separator = 0;
          BOOL  color = FALSE;
          long  ownclip = FALSE;
          long  chunksize;
          LONG  updatefrom;

        while((length > 0) && (data->clipboard->io_Length == data->clipboard->io_Actual))
        {
            BOOL chunk_used;
  
          data->clipboard->io_Data  = (void *)header;
          data->clipboard->io_Length  = 8;
          DoIO(data->iorequest);
          chunksize = (header[1]+1) & (ULONG)-2;
          length -= 8 + chunksize;
  
          chunk_used = FALSE;
          switch(header[0])
          {
            case MAKE_ID('F','L','O','W'):
              if(header[1] == 2)
              {
                data->clipboard->io_Data  = (APTR)&flow;
                data->clipboard->io_Length  = 2;
                DoIO(data->iorequest);
                if(flow > MUIV_TextEditor_Flow_Right)
                  flow = MUIV_TextEditor_Flow_Left;
                chunk_used = TRUE;
              }
              break;

            case MAKE_ID('H','I','G','H'):
              if(header[1] == 2)
              {
                data->clipboard->io_Data  = (APTR)&color;
                data->clipboard->io_Length  = 2;
                DoIO(data->iorequest);
                chunk_used = TRUE;
              }
              break;

            case MAKE_ID('S','B','A','R'):
              if(header[1] == 2)
              {
                data->clipboard->io_Data  = (APTR)&separator;
                data->clipboard->io_Length  = 2;
                DoIO(data->iorequest);
                chunk_used = TRUE;
              }
              break;

            case MAKE_ID('C','O','L','S'):
              if(colors)
              {
                MyFreePooled(data->mypool, colors);
              }
              if(chunksize && (colors = (UWORD *)MyAllocPooled(data->mypool, header[1]+4)))
              {
                data->clipboard->io_Data  = (void *)colors;
                data->clipboard->io_Length  = header[1];
                DoIO(data->iorequest);
                colors[header[1]/2] = 0xffff;
                chunk_used = TRUE;
              }
              break;

            case MAKE_ID('S','T','Y','L'):
              ownclip = TRUE;
              if(styles)
              {
                MyFreePooled(data->mypool, styles);
              }
              if(chunksize && (styles = (UWORD *)MyAllocPooled(data->mypool, header[1]+4)))
              {
                data->clipboard->io_Data  = (void *)styles;
                data->clipboard->io_Length  = header[1];
                DoIO(data->iorequest);
                styles[header[1]/2] = EOS;
                chunk_used = TRUE;
              }
              break;

            case MAKE_ID('C','H','R','S'):
              data->HasChanged = TRUE;
              if(chunksize && !ownclip)
              {
                char *contents;
  
                if((contents = (char *)MyAllocPooled(data->mypool, header[1]+4)))
                {
                  data->clipboard->io_Data    = contents;
                  data->clipboard->io_Length    = header[1];
                  DoIO(data->iorequest);
                  data->clipboard->io_Offset    += header[1] & 1;
  
                  if(*(contents+header[1]-1) != '\n')
                  {
                    newline = FALSE;
                  }
                  else
                  {
                    header[1]--;
                  }
                  *(contents+header[1]) = '\0';
  
                  if((line = ImportText(contents, data->mypool, &ImPlainHook, sizeof(struct line_node), data->ImportWrap)))
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
                  chunk_used = TRUE;
                }
              }
              else
              {
                if(chunksize && (textline = (char *)MyAllocPooled(data->mypool, header[1]+4)))
                {
                  data->clipboard->io_Data    = textline;
                  data->clipboard->io_Length    = header[1];
                  DoIO(data->iorequest);
                  data->clipboard->io_Offset    += header[1] & 1;
                  if(textline[header[1]-1] != '\n')
                  {
                    newline = FALSE;
                    textline[header[1]] = '\n';
                    header[1]++;
                  }
                  textline[header[1]] = '\0';
  
                  if((line = AllocPooled(data->mypool, sizeof(struct line_node))))
                  {
                    line->next     = NULL;
                    line->previous   = previous;
                    line->contents   = textline;
                    line->length   = header[1];
                    line->visual   = VisualHeight(line, data);
                    line->color    = color;
                    line->flow     = flow;
                    line->separator = separator;
                    line->styles   = styles;
                    line->colors   = colors;
                    data->totallines += line->visual;
  
                    if(!startline)
                      startline = line;
                    if(previous)
                      previous->next  = line;
  
                    previous = line;
                  }
                  else
                  {
                    if(styles)
                      MyFreePooled(data->mypool, (void *)styles);
                    if(colors)
                      MyFreePooled(data->mypool, (void *)colors);
                  }
                  chunk_used  = TRUE;
                }
                else
                {
                  if(styles)
                    MyFreePooled(data->mypool, styles);
                  if(colors)
                    MyFreePooled(data->mypool, (void *)colors);
                }
                styles    = NULL;
                colors    = NULL;
                flow      = MUIV_TextEditor_Flow_Left;
                color     = FALSE;
                separator = 0;
                ownclip   = FALSE;
              }
              break;
          }
          if(chunk_used == FALSE)
            data->clipboard->io_Offset += chunksize;
        }
        data->clipboard->io_Data  = NULL;
        data->clipboard->io_Length  = (ULONG)-1;
        data->clipboard->io_Offset  = (ULONG)-1;
        DoIO(data->iorequest);
  
        if(line)
        {
            long  oneline = FALSE;
  
          SplitLine(x, actline, FALSE, NULL, data);
          line->next = actline->next;
          actline->next->previous = line;
          actline->next = startline;
          startline->previous = actline;
          data->CPos_X = line->length-1;
          if(actline->next == line)
          {
            data->CPos_X += actline->length-1;
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
        data->update = TRUE;
  
        ScrollIntoDisplay(data);
        updatefrom = LineToVisual(actline, data)-1;
        if(updatefrom < 0)
          updatefrom = 0;
        DumpText(data->visual_y+updatefrom, updatefrom, data->maxlines, TRUE, data);
      }
      else
      {
        DoMethod(data->object, MUIM_TextEditor_HandleError, Error_ClipboardIsNotFTXT);
      }
    }
    data->clipboard->io_Data  = NULL;
    data->clipboard->io_Length  = (ULONG)-1;
    data->clipboard->io_Offset  = (ULONG)-1;
    DoIO(data->iorequest);
  
    CloseDevice(data->iorequest);
    DeleteIORequest(data->iorequest);
    DeleteMsgPort(data->clipport);
  
    if(data->update)
        res = TRUE;
    else  data->update = TRUE;
  }

#ifdef ClassAct
    args->res = res;
    Forbid();
    ReleaseGIRPort(data->rport);
    ReleaseSemaphore(&data->semaphore);
    Signal(args->task, 1 << args->sigbit);
  }
#else
  return res;
#endif
}
/*--------------------------*
 * Merge two lines into one *
 *--------------------------*/
long  MergeLines    (struct line_node *line, struct InstData *data)
{
  struct line_node *next;
  char  *newbuffer;
  LONG  visual, oldvisual, line_nr, c;
  LONG  emptyline = FALSE;
  LONG  color = line->color;
  UWORD flow = line->flow;
  UWORD separator = line->separator;

  data->HasChanged = TRUE;
  if(line->length == 1)
  {
    emptyline = TRUE;
    color = line->next->color;
    flow = line->next->flow;
    separator = line->next->separator;
  }
  visual = line->visual + line->next->visual;

  if((newbuffer = MyAllocPooled(data->mypool, strlen(line->contents)+strlen(line->next->contents)+1)))
  {
    CopyMem(line->contents, newbuffer, line->length-1);
    CopyMem(line->next->contents, newbuffer+line->length-1, line->next->length+1);
    MyFreePooled(data->mypool, line->contents);
    MyFreePooled(data->mypool, line->next->contents);

    if(emptyline)
    {
      if(line->styles)
        MyFreePooled(data->mypool, line->styles);
      line->styles = line->next->styles;

      if(line->colors)
        MyFreePooled(data->mypool, line->colors);
      line->colors = line->next->colors;
    }
    else
    {
        UWORD *styles;
        UWORD *styles1 = line->styles;
        UWORD *styles2 = line->next->styles;
        UWORD *colors;
        UWORD *colors1 = line->colors;
        UWORD *colors2 = line->next->colors;
        UWORD length = 12;

      if(styles1)
        length += *((long *)styles1-1) - 4;
      if(styles2)
        length += *((long *)styles2-1) - 4;

      if((styles = MyAllocPooled(data->mypool, length)))
      {
          unsigned short* t_styles = styles;
          unsigned short  style = 0;

        if(styles2)
        {
            unsigned short* t_styles2 = styles2;

          while(*t_styles2++ == 1)
          {
            if(*t_styles2 > 0xff)
                style &= *t_styles2++;
            else  style |= *t_styles2++;
          }
        }

        if(styles1)
        {

          while(*styles1 != EOS)
          {
            if((*styles1 == line->length) && ((~*(styles1+1) & style) == (*(styles1+1)  ^ 0xffff)))
            {
              style   &= *(styles1+1);
              styles1 += 2;
            }
            else
            {
              *styles++ = *styles1++;
              *styles++ = *styles1++;
            }
          }
          MyFreePooled(data->mypool, line->styles);
        }

        if(styles2)
        {
          while(*styles2 != EOS)
          {
            if((*styles2 == 1)  && (!(*(styles2+1) & style)))
            {
              styles2 += 2;
            }
            else
            {
              *styles++ = *styles2++ + line->length - 1;
              *styles++ = *styles2++;
            }
          }
          MyFreePooled(data->mypool, line->next->styles);
        }
        *styles = EOS;
        line->styles = t_styles;
      }

      length = 12;

      if(colors1)
        length += *((long *)colors1-1) - 4;
      if(colors2)
        length += *((long *)colors2-1) - 4;

      if((colors = MyAllocPooled(data->mypool, length)))
      {
          UWORD *t_colors = colors;
          UWORD end_color = 0;

        if(colors1)
        {
          while(*colors1 < line->length && *colors1 != 0xffff)
          {
            *colors++ = *colors1++;
            end_color = *colors1;
            *colors++ = *colors1++;
          }
          MyFreePooled(data->mypool, line->colors);
        }

        if(end_color && (colors2 == NULL || *colors2 != 1))
        {
          *colors++ = line->length;
          *colors++ = 0;
        }

        if(colors2)
        {
          if(*colors2 == 1 && *(colors2+1) == end_color)
            colors2 += 2;

          while(*colors2 != 0xffff)
          {
            *colors++ = *colors2++ + line->length - 1;
            *colors++ = *colors2++;
          }
          MyFreePooled(data->mypool, line->next->colors);
        }
        *colors = 0xffff;
        line->colors = t_colors;
      }
    }

    line->contents = newbuffer;
    line->length  = strlen(newbuffer);

    next = line->next;
    line->next = line->next->next;
    if(line->next)
      line->next->previous = line;
    oldvisual = line->visual;
    line->visual = VisualHeight(line, data);
    line->color = color;
    line->flow = flow;
    line->separator = separator;

    FreePooled(data->mypool, next, sizeof(struct line_node));

    line_nr = LineToVisual(line, data);
    if(!(emptyline && (line_nr + line->visual - 1 < data->maxlines)))
    {
      LONG t_oldvisual = oldvisual;
      LONG t_line_nr   = line_nr;
      
      c = 0;
      
      while((--t_oldvisual) && (t_line_nr++ <= data->maxlines))
        c = c + LineCharsWidth(line->contents+c, data);
      
      while((c < line->length) && (t_line_nr <= data->maxlines))
        c = c + PrintLine(c, line, t_line_nr++, TRUE, data);
    }

    if(line_nr + oldvisual == 1 && line->visual == visual-1)
    {
      data->visual_y--;
      data->totallines -= 1;
      if(data->fastbackground)
          DumpText(data->visual_y, 0, visual-1, TRUE, data);
      else  DumpText(data->visual_y, 0, data->maxlines, TRUE, data);
      return(TRUE);
    }

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
          else  DumpText(data->visual_y+line_nr-1, line_nr-1, data->maxlines, TRUE, data);
        }
        else
        {
          if(data->fastbackground)
              ScrollUp(line_nr + line->visual - 1, 1, data);
          else  DumpText(data->visual_y+line_nr+line->visual-1, line_nr+line->visual-1, data->maxlines, TRUE, data);
        }
      }
    }
    else
    {
      if(visual < line->visual)
      {
        data->totallines += 1;
        if(line_nr+line->visual-1 < data->maxlines)
          ScrollDown(line_nr + line->visual - 2, 1, data);
      }
    }
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
}
/*---------------------*
 * Split line into two *
 *---------------------*/
long  SplitLine   (LONG x, struct line_node *line, BOOL move_crsr, struct UserAction *buffer, struct InstData *data)
{
  struct line_node  *newline;
  struct line_node  *next;
  struct pos_info pos;
  LONG c, line_nr, lines;
  UWORD   crsr_x = data->CPos_X;
  struct  line_node *crsr_l = data->actualline;

  OffsetToLines(x, line, &pos, data);
  lines = pos.lines;

  next = line->next;
  if((newline = AllocPooled(data->mypool, sizeof(struct line_node))))
  {
      UWORD *styles = line->styles;
      UWORD *newstyles = NULL;
      UWORD *colors = line->colors;
      UWORD *newcolors = NULL;

    data->HasChanged = TRUE;
    Init_LineNode(newline, line, line->contents+x, data);
    newline->color = line->color;
    newline->flow = line->flow;
    newline->separator = line->separator;
    if(buffer)
    {
      newline->color = buffer->del.style;
      newline->flow = buffer->del.flow;
      newline->separator = buffer->del.separator;
    }

    if(styles)
    {
        LONG  style = 0;
        LONG  length = 0;
        UWORD *ostyles;

      while(*styles++ <= x+1)
      {
        if(*styles > 0xff)
            style &= *styles++;
        else  style |= *styles++;
      }
      styles--;
      ostyles = styles;
      while(*(styles+length) != EOS)
        length += 2;
      length = (length*2) + 16;

      if((newstyles = MyAllocPooled(data->mypool, length)))
      {
          UWORD *nstyles = newstyles;

        if(style & BOLD)
        { *nstyles++ = 1; *nstyles++ = BOLD; }
        if(style & ITALIC)
        { *nstyles++ = 1; *nstyles++ = ITALIC; }
        if(style & UNDERLINE)
        { *nstyles++ = 1; *nstyles++ = UNDERLINE; }

        while(*styles != EOS)
        {
          *nstyles++ = (*styles++) - x;
          *nstyles++ = *styles++;
        }
        *nstyles = EOS;
      }
      if(style & BOLD)
      { *ostyles++ = x+1; *ostyles++ = ~BOLD; }
      if(style & ITALIC)
      { *ostyles++ = x+1; *ostyles++ = ~ITALIC; }
      if(style & UNDERLINE)
      { *ostyles++ = x+1; *ostyles++ = ~UNDERLINE; }
      if(!x)
        ostyles = line->styles;
      *ostyles = EOS;
    }
    newline->styles = newstyles;

    if(colors)
    {
      UWORD color = GetColor(x, line);
      UWORD length = 0;
      UWORD *ocolors;

      while(*colors <= x+1)
      {
        colors += 2;
      }
      ocolors = colors;

      while(*(colors+length) != 0xffff)
        length += 2;
      length = (length*2) + 16;

      if((newcolors = MyAllocPooled(data->mypool, length)))
      {
          UWORD *ncolors = newcolors;

        if(color && *colors-x != 1)
        {
          *ncolors++ = 1;
          *ncolors++ = color;
        }

        while(*colors != 0xffff)
        {
          *ncolors++ = (*colors++) - x;
          *ncolors++ = *colors++;
        }
        *ncolors = 0xffff;
      }
      if(!x)
        ocolors = line->colors;
      *ocolors = 0xffff;
    }
    newline->colors = newcolors;


    newline->next = next;
    if(next)
      next->previous = newline;

    *(line->contents+x) = '\n';
    *(line->contents+x+1) = '\0';
    line->length = x+1;

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
      line->color = 0;
      line->separator = 0;
      if(!(line->previous && line->previous->flow == line->flow))
      {
        line->flow = MUIV_TextEditor_Flow_Left;
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
        else  DumpText(data->visual_y+line_nr-1, line_nr-1, data->maxlines, TRUE, data);
      }
      else
      {
        data->visual_y++;
        data->totallines += 1;
        if(!(data->flags & FLG_Quiet))
        {
            struct  Hook  *oldhook;

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
      return(TRUE);
    }

    if(x == line->length + newline->length - 2)
    {
      data->totallines += 1;
      if(!buffer)
      {
        line->next->color = 0;
        line->next->separator = 0;
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
        else  DumpText(data->visual_y+line_nr, line_nr, data->maxlines, TRUE, data);
      }
      return(TRUE);
    }
    x = line->length;

    OffsetToLines(x-1, line, &pos, data);
    if((line->visual + line->next->visual >= c) && (line->visual == lines))
    {
      if(line->visual + line->next->visual > c)
        data->totallines += 1;
      PrintLine(pos.bytes, line, line_nr, TRUE, data);
      if((line_nr+line->next->visual-1 < data->maxlines) && (line->visual + line->next->visual > c))
      {
        ScrollDown(line_nr+line->next->visual-1, 1, data);
      }
    }
    else
    {
      PrintLine((x-1)-pos.x, line, line_nr, TRUE, data);
      if((line_nr < data->maxlines) && (line->visual + line->next->visual < c))
      {
        data->totallines -= 1;
        ScrollUp(line_nr, 1, data);
      }
    }
/*------------------*/
    line = line->next;
    line_nr++;
    c = 0;
    while((c < line->length) && (line_nr <= data->maxlines))
      c = c + PrintLine(c, line, line_nr++, TRUE, data);
  /* Her printes !HELE! den nye linie, burde optimeres! */

    return (TRUE);
  }
  else
  {
    return (FALSE);
  }
}
/*------------------------------------------------------------------*
 * Backwards string copy, please replace with some assembler stuff! *
 *------------------------------------------------------------------*/
void  strcpyback    (char *dest, char *src)
{
//  ULONG length = strlen(src)+1;
//  memmove(dest, src, length);

    LONG  length;

  length = strlen(src)+1;
  dest = dest + length;
  src = src + length;

  length++;
  while (--length)
  {
    *--dest = *--src;
//    printf("%d, %d\n", *src, *dest);
  }
}

/* ------------------------------------ *
 *  Functions which updates the display *
 * ------------------------------------ */
void  OptimizedPrint  (LONG x, struct line_node *line, LONG line_nr, LONG width, struct InstData *data)
{
    LONG twidth = PrintLine(x, line, line_nr++, TRUE, data);

  if((twidth != width) && (x+twidth < line->length) && (line_nr <= data->maxlines))
  {
    OptimizedPrint(x+twidth, line, line_nr, LineCharsWidth(line->contents+x+width, data) + (width - twidth), data);
  }
}

void  UpdateChange(LONG x, struct line_node *line, LONG length, char *characters, struct UserAction *buffer, struct InstData *data)
{
  LONG diff;
  LONG skip=0;
  LONG line_nr;
  LONG orgline_nr;
  LONG width;
  LONG lineabove_width=0;

  line_nr   = LineToVisual(line, data);
  orgline_nr  = line_nr;

  while((skip + (width = LineCharsWidth(line->contents+skip, data))) < x)
  {
    lineabove_width = width;
    skip += width;
    line_nr++;
  }

  if(characters)
  {
//    if((data->flags & FLG_InsertMode) || (x+length >= line->length))
    {
      strcpyback(line->contents+x+length, line->contents+x);
      strncpy(line->contents+x, characters, length);
      width += length;
      line->length += length;
      if(buffer)
      {
          UWORD style = buffer->del.style;
  
        AddStyleToLine(x, line, 1, (style & BOLD) ? BOLD : ~BOLD, data);
        AddStyleToLine(x, line, 1, (style & ITALIC) ? ITALIC : ~ITALIC, data);
        AddStyleToLine(x, line, 1, (style & UNDERLINE) ? UNDERLINE : ~UNDERLINE, data);
        line->flow = buffer->del.flow;
        line->separator = buffer->del.separator;
      }
    }
/*    else    // Attempt of doing non-InsertMode (overwrite)
    {
      strncpy(line->contents+x, characters, length);
    }
*/  }
  else
  {
    strcpy(line->contents+x, line->contents+x+length);
    width -= length;
    line->length -= length;
  }

  diff = VisualHeight(line, data) - line->visual;
  if(diff)
  {
      LONG  movement;

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
    if(lineabove_width != LineCharsWidth(line->contents+skip-lineabove_width, data))
    {
        LONG  newwidth;

      newwidth = PrintLine(skip-lineabove_width, line, line_nr-1, TRUE, data) - lineabove_width;
      skip  += newwidth;
      width -= newwidth;
      if(skip >= line->length)
        return;
    }
  }
  OptimizedPrint(skip, line, line_nr, width, data);
  data->HasChanged = TRUE;
}

/*------------------------------*
 * Paste n characters to a line *
 *------------------------------*/
long  PasteChars    (LONG x, struct line_node *line, LONG length, char *characters, struct UserAction *buffer, struct InstData *data)
{
  if(line->styles)
  {
    if(*line->styles != EOS)
    {
        ULONG c = 0;

      while(*(line->styles+c) <= x+1)
        c += 2;
      while(*(line->styles+c) != EOS)
      {
        *(line->styles+c) += length;
        c += 2;
      }
    }
  }

  if(line->colors)
  {
    if(*line->colors != 0xffff)
    {
        ULONG c = 0;

      while(*(line->colors+c) <= x+1)
        c += 2;
      while(*(line->colors+c) != 0xffff)
      {
        *(line->colors+c) += length;
        c += 2;
      }
    }
  }


  if((*((long *)line->contents-1))-4 < line->length + length + 1)
  {
    if(!ExpandLine(line, length, data))
      return(FALSE);
  }

  UpdateChange(x, line, length, characters, buffer, data);
  return(TRUE);
}
/*----------------------------*
 * Remove n chars from a line *
 *----------------------------*/
long  RemoveChars   (LONG x, struct line_node *line, LONG length, struct InstData *data)
{
  if(line->styles)
  {
    if(*line->styles != EOS)
    {
        UWORD start_style = GetStyle(x-1, line);
        UWORD end_style = GetStyle(x+length, line);
        ULONG c = 0, store;

      while(*(line->styles+c) <= x)
        c += 2;

      if(start_style != end_style)
      {
          UWORD turn_off = start_style & ~end_style,
              turn_on  = end_style & ~start_style;

        if(turn_off & BOLD)
        { *(line->styles+c++) = x+1;  *(line->styles+c++) = ~BOLD;  }
        if(turn_off & ITALIC)
        { *(line->styles+c++) = x+1;  *(line->styles+c++) = ~ITALIC;  }
        if(turn_off & UNDERLINE)
        { *(line->styles+c++) = x+1;  *(line->styles+c++) = ~UNDERLINE; }
        if(turn_on & BOLD)
        { *(line->styles+c++) = x+1;  *(line->styles+c++) = BOLD; }
        if(turn_on & ITALIC)
        { *(line->styles+c++) = x+1;  *(line->styles+c++) = ITALIC; }
        if(turn_on & UNDERLINE)
        { *(line->styles+c++) = x+1;  *(line->styles+c++) = UNDERLINE;  }
      }

      store = c;
      while(*(line->styles+c) <= x+length+1)
        c += 2;

      while(*(line->styles+c) != EOS)
      {
        *(line->styles+store++) = *(line->styles+c++)-length;
        *(line->styles+store++) = *(line->styles+c++);
      }
      *(line->styles+store) = EOS;
    }
  }

  if(line->colors)
  {
    if(*line->colors != 0xffff)
    {
        UWORD start_color = x ? GetColor(x-1, line) : 0;
        UWORD end_color = GetColor(x+length, line);
        ULONG c = 0, store;

      while(*(line->colors+c) <= x)
        c += 2;

      if(start_color != end_color)
      {
        *(line->colors+c++) = x+1;
        *(line->colors+c++) = end_color;
      }

      store = c;
      while(*(line->colors+c) <= x+length+1)
        c += 2;

      while(*(line->colors+c) != 0xffff)
      {
        *(line->colors+store++) = *(line->colors+c++)-length;
        *(line->colors+store++) = *(line->colors+c++);
      }
      *(line->colors+store) = 0xffff;
    }
  }



  UpdateChange(x, line, length, NULL, NULL, data);
  return(TRUE);
}
