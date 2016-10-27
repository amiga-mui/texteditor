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

#if defined(__amigaos4__)
#define __USE_CLASSIC_MINTERM__
#include <graphics/minterm.h>
#endif

#include <graphics/gfxmacros.h>
#include <graphics/rpattr.h>
#include <graphics/text.h>
#include <clib/alib_protos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include "private.h"
#include "Debug.h"

#if defined(__amigaos3__)
#ifndef RPTAG_PenMode
#define RPTAG_PenMode         0x80000080
#endif
#ifndef RPTAG_FgColor
#define RPTAG_FgColor         0x80000081
#endif
#ifndef RPTAG_BgColor
#define RPTAG_BgColor         0x80000082
#endif
#endif

/// ConvertStyle()
ULONG ConvertStyle(UWORD style)
{
  ULONG result = FS_NORMAL;

  ENTER();

  if(isFlagSet(style, BOLD))
    setFlag(result, FSF_BOLD);
  if(isFlagSet(style, ITALIC))
    setFlag(result, FSF_ITALIC);
  if(isFlagSet(style, UNDERLINE))
    setFlag(result, FSF_UNDERLINED);

  RETURN(result);
  return(result);
}

///
/// ConvertPen()
ULONG ConvertPen(struct InstData *data, UWORD color, BOOL highlight)
{
  ULONG pen;

  ENTER();

  if(color != 0)
  {
    if(data->colormap != NULL)
      pen = data->colormap[color-1];
    else if(color <= 8)
      pen = _pens(data->object)[color-1];
    else
      pen = color-9;
  }
  else
  {
    if(highlight == TRUE)
      pen = MUIPEN(data->highlightColor);
    else
      pen = MUIPEN(data->textColor);
  }

  RETURN(pen);
  return pen;
}

///
/// DrawSeparator()
void DrawSeparator(struct InstData *data, struct RastPort *rp, LONG X, LONG Y, LONG Width, LONG Height)
{
  ENTER();

  if(Width > 3*Height)
  {
    SetAPen(rp, MUIPEN(data->separatorshadow));
    RectFill(rp, X, Y, X+Width-2, Y);
    RectFill(rp, X, Y, X, Y+Height);

    SetAPen(rp, MUIPEN(data->separatorshine));
    RectFill(rp, X+1, Y+Height, X+Width-1, Y+Height);
    RectFill(rp, X+Width-1, Y, X+Width-1, Y+Height);
  }

  LEAVE();
}

///
/// PrintLine()
LONG PrintLine(struct InstData *data, LONG x, struct line_node *line, LONG line_nr, BOOL doublebuffer)
{
  STRPTR text = line->line.Contents;
  LONG length;
  struct RastPort *rp = data->doublerp;

  ENTER();

  length = LineCharsWidth(data, text+x);

  // fall back to non-doublebuffered drawing in case we have no layered double buffer rastport
  if(rp == NULL)
    doublebuffer = FALSE;

  if(doublebuffer == FALSE)
    rp = &data->copyrp;

  if(line_nr > 0 && data->update == TRUE && isFlagClear(data->flags, FLG_Quiet) && data->rport != NULL && data->shown == TRUE)
  {
    LONG c_length = length;
    LONG startx = 0, stopx = 0;
    LONG starty = 0, xoffset = 0;
    LONG flow = 0;
    LONG maxwidth;
    struct LineStyle *styles = line->line.Styles;
    struct LineColor *colors = line->line.Colors;
    struct marking block;
    BOOL cursor = FALSE;
    struct TEColor textColor;
    struct TEColor highlightColor;
    const LONG dleft  = (doublebuffer) ? 0 : _mleft(data->object);
    const LONG dright = (doublebuffer) ? _mwidth(data->object) : _mright(data->object);
    LONG o_width;
    LONG pen_pos;

    if(data->rgbMode == TRUE)
    {
      textColor.isRGB = TRUE;
      textColor.color = data->textRGB;
      highlightColor.isRGB = TRUE;
      highlightColor.color = data->highlightRGB;
	}
	else
	{
      textColor.isRGB = FALSE;
      textColor.color = data->textColor;
      highlightColor.isRGB = FALSE;
      highlightColor.color = data->highlightColor;
	}

    if(line->line.Highlight == TRUE && x == 0 && line->line.Length == 1)
      line->line.Highlight = FALSE;

    if(doublebuffer == FALSE)
    {
      starty = data->ypos+(data->fontheight * (line_nr-1));
      xoffset = _mleft(data->object) - data->xpos;
    }
    else
      xoffset -= data->xpos;

    flow = FlowSpace(data, line->line.Flow, text+x);

    if(Enabled(data))
    {
      struct line_node *blkline;

      NiceBlock(&data->blockinfo, &block);

      blkline = GetNextLine(block.startline);

      if(block.startline == block.stopline)
      {
        if(block.startline == line)
        {
          startx = block.startx;
          stopx = block.stopx;
        }
      }
      else
      {
        if(block.startline == line)
        {
          startx = block.startx;
          stopx = line->line.Length;
        }
        else
        {
          if(block.stopline == line)
          {
            stopx = block.stopx;
          }
          else
          {
            while((blkline != block.stopline) && (!stopx))
            {
              if(blkline == line)
              {
                stopx = line->line.Length;
              }
              blkline = GetNextLine(blkline);
            }
          }
        }
      }
    }

    {
      LONG blockstart = 0;
      LONG blockwidth = 0;
      struct RastPort *old = _rp(data->object);

      LONG clr_left, clr_width;

      if(startx < x+c_length && stopx > x)
      {
        if(startx > x)
          blockstart = TextLengthNew(&data->tmprp, text+x, startx-x, data->TabSizePixels);
        else
          startx = x;

        blockwidth = ((stopx >= c_length+x) ? _mwidth(data->object)-(blockstart+flow) + data->xpos : TextLengthNew(&data->tmprp, text+startx, stopx-startx, data->TabSizePixels));
      }
      else if(isFlagClear(data->flags, FLG_ReadOnly) &&
              isFlagClear(data->flags, FLG_Ghosted) &&
              line == data->actualline &&
              data->CPos_X >= x &&
              data->CPos_X < x+c_length &&
              !Enabled(data) &&
              (isFlagSet(data->flags, FLG_Active) || data->inactiveCursor == TRUE))
      {
        cursor = TRUE;
        blockstart = TextLengthNew(&data->tmprp, text+x, data->CPos_X-x, data->TabSizePixels);

        // calculate the cursor width
        // if it is set to 6 then we should find out how the width of the current char is
        if(data->CursorWidth == 6)
          blockwidth = TextLengthNew(&data->tmprp, (text[data->CPos_X] < ' ') ? (char *)" " : (char *)&text[data->CPos_X], 1, data->TabSizePixels);
        else
          blockwidth = data->CursorWidth;
      }

      SetDrMd(rp, JAM1);
      _rp(data->object) = rp;

      clr_left  = dleft;
      clr_width = MIN(flow+blockstart - data->xpos, (ULONG)_mwidth(data->object));
                  
      if(clr_width > 0)
      {
        // clear the background first
        DoMethod(data->object, MUIM_DrawBackground, clr_left, starty,
                                                    clr_width, data->fontheight,
                                                    isFlagSet(data->flags, FLG_InVGrp) ? 0 : _mleft(data->object),
                                                    isFlagSet(data->flags, FLG_InVGrp) ? data->fontheight*(data->visual_y+line_nr-2) : _mtop(data->object)+data->fontheight * (data->visual_y+line_nr-2),
                                                    0);
      }

      if(blockwidth)
      {
        LONG color;

        // in case the gadget is in inactive state we use a different background
        // color for our selected area
        if(isFlagClear(data->flags, FLG_Active) &&
           isFlagClear(data->flags, FLG_Activated) &&
           isFlagSet(data->flags, FLG_ActiveOnClick))
        {
          color = MUIPEN(data->inactivecolor);
        }
        else
          color = MUIPEN(data->markedcolor);

        // if selectmode == 2 then a whole line should be drawn as being marked, so
        // we have to start at xoffset instead of xoffset+flow+blockstart.
        // Please note that the second part of the following "empiric" evaluation should
        // prevent that centered or right aligned lines are not correctly marked right
        // from the beginning of the line. However, it seems to be not cover 100% of all different
        // cases so that the evaluation if a line should be completely marked should be probably
        // moved elsewhere in future.
        if(data->selectmode == 2 ||
           (flow && data->selectmode != 1 && startx-x == 0 && cursor == FALSE &&
            ((data->blockinfo.startline != data->blockinfo.stopline) || x > 0)))
        {               
          LONG left  = MIN(MAX(xoffset, dleft), dright);
          LONG right = MIN(dright, xoffset+flow+blockwidth-1);

          SetAPen(rp, color);
          if (left < dright || right > dleft)
            RectFill(rp, left, starty, right, starty+data->fontheight-1);
        }
        else
        {              
          LONG left  = MIN(MAX(xoffset+flow+blockstart, dleft), dright);
          LONG right = MIN(dright, xoffset+flow+blockstart+blockwidth-1);
                       
          SetAPen(rp, cursor ? MUIPEN(data->cursorcolor) : color);
          if(left < dright || right > dleft)
          {
            RectFill(rp, left, starty, right, starty+data->fontheight-1);

            // if the gadget is in inactive state we just draw a skeleton cursor instead
            if(cursor == TRUE &&
               isFlagClear(data->flags, FLG_Active) &&
               isFlagClear(data->flags, FLG_Activated))
            {
              DoMethod(data->object, MUIM_DrawBackground, left+1, starty+1,
                                                          blockwidth-2, data->fontheight-2,
                                                          isFlagSet(data->flags, FLG_InVGrp) ? 0 : _mleft(data->object),
                                                          isFlagSet(data->flags, FLG_InVGrp) ? data->fontheight*(data->visual_y+line_nr-2) : _mtop(data->object)+data->fontheight * (data->visual_y+line_nr-2),
                                                          0);
            }
          }
        }
      }

      {
        LONG x_start = MAX(dleft, (blockwidth ? xoffset+blockstart+blockwidth+flow : xoffset+blockstart+blockwidth));
        LONG y_start = starty;
        LONG x_width = dright - x_start + 1;
        LONG y_width = data->fontheight;
        LONG x_ptrn = (blockwidth) ? (blockstart+blockwidth+flow) : (blockstart+blockwidth);
        LONG y_ptrn = data->fontheight*(data->visual_y+line_nr-2);

        if(isFlagClear(data->flags, FLG_InVGrp))
        {
          x_ptrn += _mleft(data->object);
          y_ptrn += _mtop(data->object);
        }
        if(x_start < dright)
          DoMethod(data->object, MUIM_DrawBackground, x_start, y_start, x_width, y_width, x_ptrn, y_ptrn, 0);
      }
      _rp(data->object) = old;
    }

    if(doublebuffer == FALSE)
      AddClipping(data);

    SetColor(data, rp, line->line.Highlight ? &highlightColor : &textColor, FALSE, FALSE);

    pen_pos  = xoffset + flow;
    o_width  = ((ULONG)flow > data->xpos ? 0 : data->xpos - flow);
    maxwidth = data->WrapMode == MUIV_TextEditor_WrapMode_NoWrap ?
              (MIN((_mwidth(data->object) + data->xpos - flow + rp->TxWidth), (ULONG)TextLengthNew(rp, text+x, c_length, data->TabSizePixels))) :
                    _mwidth(data->object) - flow; 

    while(c_length > 0)
    {
      LONG p_length = c_length;
      struct TextExtentNew te;

      SetSoftStyle(rp, ConvertStyle(GetStyle(x, line)), AskSoftStyle(rp));
      if(styles != NULL)
      {
        while(styles->column-1 <= x)
          styles++;

        if(styles->column-x-1 < p_length)
          p_length = styles->column-x-1;
      }

      if(colors != NULL)
      {
        while(colors->column-1 <= x)
        {
          SetColor(data, rp, &colors->color, line->line.Highlight, TRUE);
          colors++;
        }

        if(colors->column-x-1 < p_length)
          p_length = colors->column-x-1;
      }

/*      if(stopx)
      {
        if((startx > x) && (startx-x < p_length))
        {
          p_length = startx-x;
          SetAPen(rp, 3);
        }
        else
        {
          if((stopx > x) && (stopx-x < p_length))
          {
            p_length = stopx-x;
            SetAPen(rp, 4);
          }
          else
            SetAPen(rp, line->color ? MUIPEN(data->highlightcolor) : MUIPEN(data->textcolor));
        }
      }
*/
      {
        ULONG o_chars = 0;
        LONG o_length = 0;

        // calculate how many chars will be ommited from start because of xpos
        if(o_width && pen_pos < dleft)
        {
          o_chars   = TextFitNew(rp, text+x, p_length, &te, NULL, 1, o_width, data->fontheight, data->TabSizePixels);
          o_length  = te.te_Width;
          o_width  -= o_length;
          maxwidth -= o_length;
          pen_pos  += o_length;
        }

        // set the rastport pen pos to appropriate coordinates
        Move(rp, pen_pos, starty+rp->TxBaseline);
        
        // check if there is space left to print some text
        if(maxwidth > 0)
        {
          // calculate how many characters really fit in the remaining space
          ULONG fitting = TextFitNew(rp, text+x+o_chars, p_length - o_chars, &te, NULL, 1, maxwidth, data->fontheight, data->TabSizePixels);

          if(fitting > 0)
          {
            if(text[x+o_chars+fitting-1] < ' ')
              TextNew(rp, text+x+o_chars, fitting-1, data->TabSizePixels);
            else
              TextNew(rp, text+x+o_chars, fitting, data->TabSizePixels);

            pen_pos += te.te_Width;
          }

          // adjust the available horizontal pixel space
          maxwidth -= te.te_Width;
        }

        // add the length calculated before no matter how many character really fitted
        x += p_length;
        c_length -= p_length;
      }
    }
    SetSoftStyle(rp, FS_NORMAL, AskSoftStyle(rp));

    if(line->line.Separator != LNSF_None)
    {
      LONG LeftX, LeftWidth;
      LONG RightX, RightWidth;
      LONG Y, Height;

      LeftX = dleft;
      LeftWidth = flow - data->xpos - 3;
      LeftWidth = LeftWidth < 0 ? 0 : LeftWidth;
      RightX = pen_pos - _mleft(data->object) + 4;
      RightX = RightX < dleft ? dleft : RightX;
      RightWidth = dright > RightX ? dright - RightX : _mwidth(data->object);
      Y = starty;
      Height = isFlagSet(line->line.Separator, LNSF_Thick) ? 2 : 1;

      if(isFlagSet(line->line.Separator, LNSF_Middle))
        Y += (data->fontheight/2)-Height;
      else if(isFlagSet(line->line.Separator, LNSF_Bottom))
        Y += data->fontheight-(2*Height);

      if(isFlagSet(line->line.Separator, LNSF_StrikeThru) || line->line.Length == 1)
      {
        LeftWidth = _mwidth(data->object);
      }
      else
      {
        DrawSeparator(data, rp, RightX, Y, RightWidth, Height);
      }
      DrawSeparator(data, rp, LeftX, Y, LeftWidth, Height);
    }


    if(isFlagSet(data->flags, FLG_Ghosted) && isFlagClear(data->flags, FLG_MUI4))
    {
      UWORD newPattern[] = {0x1111, 0x4444};

      if(doublebuffer == TRUE)
      {
        ULONG ptrn1 = 0x11111111UL;
        ULONG ptrn2 = 0x44444444UL;

        ptrn1 = ptrn1>>((_mleft(data->object)-xoffset)%16);
        ptrn2 = ptrn2>>((_mleft(data->object)-xoffset)%16);

        if((data->fontheight*(data->visual_y+line_nr-2))%2 == 0)
        {
          newPattern[0] = ptrn2;
          newPattern[1] = ptrn1;
        }
        else
        {
          newPattern[0] = ptrn1;
          newPattern[1] = ptrn2;
        }
      }
      else
      {
        if((data->fontheight*(data->visual_y-1))%2 == 0)
        {
          newPattern[0] = 0x4444;
          newPattern[1] = 0x1111;
        }
      }

      SetDrMd(rp, JAM1);
      SetAPen(rp, _pens(data->object)[MPEN_SHADOW]);
      SetAfPt(rp, newPattern, 1);
      RectFill(rp, xoffset, starty, xoffset+_mwidth(data->object)-1, starty+data->fontheight-1);
      SetAfPt(rp, NULL, (UBYTE)-1);
    }

    if(doublebuffer == FALSE)
      RemoveClipping(data);
    else
    {
      if(line_nr == 1)
      {
        BltBitMapRastPort(rp->BitMap, 0, _mtop(data->object)-data->ypos, data->rport, _mleft(data->object), _mtop(data->object)+(data->fontheight * (line_nr-1)), _mwidth(data->object), data->fontheight-(_mtop(data->object)-data->ypos), (ABC|ABNC));
      }
      else
      {
        if(line_nr == data->maxlines+1)
        {
          if(_mtop(data->object) != data->ypos)
          {
            BltBitMapRastPort(rp->BitMap, 0, 0, data->rport, _mleft(data->object), data->ypos+(data->fontheight * (line_nr-1)), _mwidth(data->object), _mtop(data->object)-data->ypos, (ABC|ABNC));
          }
        }
        else
        {
          BltBitMapRastPort(rp->BitMap, 0, 0, data->rport, _mleft(data->object), data->ypos+(data->fontheight * (line_nr-1)), _mwidth(data->object), data->fontheight, (ABC|ABNC));
        }
      }
    }
  }

  if(data->WrapMode == MUIV_TextEditor_WrapMode_NoWrap)
    length = line->line.Length;

  RETURN(length);
  return(length);
}

///
/// SetColor
void SetColor(struct InstData *data, struct RastPort *rp, const struct TEColor *c, BOOL highlight, BOOL convert)
{
  ENTER();

  if(IsRGBColor(c) == TRUE)
  {
    if(isFlagSet(data->flags, FLG_Truecolor))
    {
      if(data->exclusivePen != -1)
      {
        // set the appropriate RGB color for the exclusive pen and use it
        SetRGB32(&_screen(data->object)->ViewPort, data->exclusivePen, (c->color & 0xff0000) << 8, (c->color & 0x00ff00) << 16, (c->color & 0x0000ff) << 24);
        SetRPAttrs(rp,
          #if defined(__MORPHOS__)
          RPTAG_PenMode,   TRUE,
          RPTAG_AlphaMode, FALSE,
          #endif
          RPTAG_APen, data->exclusivePen,
          TAG_DONE);
      }
      else
      {
        // we can use RGB colors directly
        SetRPAttrs(rp,
          #if defined(__MORPHOS__)
          RPTAG_PenMode,   FALSE,
          RPTAG_AlphaMode, FALSE,
          #endif
          #if defined(__amigaos4__)
          RPTAG_APenColor, c->color,
          #else
          RPTAG_FgColor,   c->color,
          #endif
          TAG_DONE);
      }
    }
    else
    {
      // fall back to the standard foreground color
      SetRPAttrs(rp,
        #if defined(__MORPHOS__)
        RPTAG_PenMode,   TRUE,
        RPTAG_AlphaMode, FALSE,
        #endif
        RPTAG_APen, convert == TRUE ? ConvertPen(data, 1, highlight) : 1,
        TAG_DONE);
    }
  }
  else
  {
    SetRPAttrs(rp,
      #if defined(__MORPHOS__)
      RPTAG_PenMode,   TRUE,
      RPTAG_AlphaMode, FALSE,
      #endif
      RPTAG_APen, convert == TRUE ? ConvertPen(data, MUIPEN(c->color), highlight) : MUIPEN(c->color),
      TAG_DONE);
  }

  LEAVE();
}

///
