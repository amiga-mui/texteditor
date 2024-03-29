/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2021 TextEditor.mcc Open Source Team

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

#include <exec/memory.h>
#include <intuition/classes.h>
#include <clib/alib_protos.h>
#include <graphics/gfxmacros.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/exec.h>
#include <proto/layers.h>

#ifdef __MORPHOS__
#include <proto/alib.h>
#endif

#include <proto/muimaster.h>
#include <libraries/mui.h>

#include "private.h"
#include "Debug.h"

DISPATCHERPROTO(_Dispatcher);

/// ResetDisplay()
void ResetDisplay(struct InstData *data)
{
  ENTER();

  data->blockinfo.enabled = FALSE;
  data->visual_y = 1;
  data->xpos = 0;
  data->CPos_X = 0;
  data->pixel_x = 0;
  data->actualline = GetFirstLine(&data->linelist);

  data->cursor_shown = FALSE;
  if(data->shown == TRUE)
  {
    data->totallines = CountLines(data, &data->linelist);
    data->longestline = data->WrapMode == MUIV_TextEditor_WrapMode_NoWrap ? LongestLine(data) : _mwidth(data->object);
    GetColor(data, data->CPos_X, data->actualline, &data->Pen);
    data->Flow = data->actualline->line.Flow;
    data->Separator = data->actualline->line.Separator;
    data->NoNotify = TRUE;
    SetAttrs(data->object,
      MUIA_TextEditor_Pen,              data->Pen,
      MUIA_TextEditor_Flow,             data->Flow,
      MUIA_TextEditor_Separator,        data->Separator,
      MUIA_TextEditor_Prop_Entries,     data->totallines*data->fontheight,
      MUIA_TextEditor_Prop_Visible,     data->maxlines*data->fontheight,
      MUIA_TextEditor_Prop_First,       (data->visual_y-1)*data->fontheight,
      MUIA_TextEditor_Prop_DeltaFactor, data->fontheight,
      MUIA_TextEditor_HSlider_Ent,      data->longestline,
      MUIA_TextEditor_HSlider_Vis,      _mwidth(data->object),
      MUIA_TextEditor_HSlider_Pos,      data->xpos,
      TAG_DONE);
    data->NoNotify = FALSE;

    UpdateStyles(data);

    DumpText(data, data->visual_y, 0, data->maxlines, FALSE);
  }

  LEAVE();
}

///
/// RequestInput()
void RequestInput(struct InstData *data)
{
  ENTER();

  if(data->scrollaction == FALSE && data->mousemove == FALSE)
    DoMethod(_app(data->object), MUIM_Application_AddInputHandler, &data->ihnode);

  LEAVE();
}

///
/// RejectInput()
void RejectInput(struct InstData *data)
{
  ENTER();

  if(data->scrollaction == FALSE && data->mousemove == FALSE)
    DoMethod(_app(data->object), MUIM_Application_RemInputHandler, &data->ihnode);

  LEAVE();
}

///
/// DoSuperNew()
#if !defined(__MORPHOS__)
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...)
{
  Object *rc;
  VA_LIST args;
  struct opSet msg;

  VA_START(args, obj);
  msg.MethodID = OM_NEW;
  msg.ops_AttrList = VA_ARG(args, struct TagItem *);
  msg.ops_GInfo = NULL;
  rc = (Object *)DoSuperMethodA(cl, obj, (Msg)&msg);
  VA_END(args);

  return rc;
}
#endif // !__MORPHOS

///
/// mNew()
static IPTR mNew(struct IClass *cl, Object *obj, struct opSet *msg)
{
  ENTER();

  if((obj = (Object *)DoSuperNew(cl, obj,
    MUIA_PointerType, MUIV_PointerType_Text,
    TAG_MORE, msg->ops_AttrList)) != NULL)
  {
    struct InstData *data = INST_DATA(cl, obj);
    data->object = obj;

    InitLines(&data->linelist);

    #if defined(__amigaos4__)
    data->mypool = AllocSysObjectTags(ASOT_MEMPOOL, ASOPOOL_MFlags, MEMF_SHARED|MEMF_CLEAR,
                                                    ASOPOOL_Puddle, 3*1024,
                                                    ASOPOOL_Threshold, 512,
                                                    ASOPOOL_Name, "TextEditor.mcc pool",
                                                    ASOPOOL_LockMem, FALSE,
                                                    TAG_DONE);
    #else
    data->mypool = CreatePool(MEMF_ANY|MEMF_CLEAR, 3*1024, 512);
    #endif
    if(data->mypool != NULL)
    {
      if((data->mylocale = OpenLocale(NULL)) != NULL)
      {
        struct line_node *firstLine;

        if((firstLine = AllocVecPooled(data->mypool, sizeof(struct line_node))) != NULL)
        {
          if(Init_LineNode(data, firstLine, "\n") == TRUE)
          {
            if(Init_LineNode(data, &data->inactiveContents, "\n") == TRUE)
            {
              AddLine(&data->linelist, firstLine);

              data->actualline = firstLine;
              data->update = TRUE;
              data->ImportHook = &ImPlainHook;
              data->ImportWrap = 1023;
              data->WrapBorder = 0;
              data->WrapMode = MUIV_TextEditor_WrapMode_HardWrap;
              data->WrapWords = TRUE; // wrap at word boundaries
              data->TabSize = 4;       // default to 4 spaces per TAB
              data->GlobalTabSize = 4; // default to 4 spaces per TAB
              data->TabSizePixels = 4*8; // assume a fixed space width of 8 pixels per default
              data->ConvertTabs = TRUE; // convert tab to spaces per default

              data->ExportHook = &ExportHookPlain;
              setFlag(data->flags, FLG_AutoClip);
              setFlag(data->flags, FLG_ActiveOnClick);
              setFlag(data->flags, FLG_PasteStyles);
              setFlag(data->flags, FLG_PasteColors);
              setFlag(data->flags, FLG_HScroll);

              #if defined(__amigaos3__) || defined(__amigaos4__)
              if(MUIMasterBase->lib_Version > 20 || (MUIMasterBase->lib_Version == 20 && MUIMasterBase->lib_Revision >= 5640))
              {
                // MUI 4.0 for AmigaOS4 does the disabled pattern drawing itself,
                // no need to do this on our own
                setFlag(data->flags, FLG_MUI4);
              }
              #elif defined(__MORPHOS__)
              if(MUIMasterBase->lib_Version > 20 || (MUIMasterBase->lib_Version == 20 && MUIMasterBase->lib_Revision >= 6906))
              {
                // MUI 4.0 for MorphOS does the disabled pattern drawing itself,
                // no need to do this on our own
                setFlag(data->flags, FLG_MUI4);
              }
              #endif

              if(FindTagItem(MUIA_Background, msg->ops_AttrList))
                setFlag(data->flags, FLG_OwnBackground);
              if(FindTagItem(MUIA_Frame, msg->ops_AttrList))
                setFlag(data->flags, FLG_OwnFrame);
              if(FindTagItem(MUIA_Font, msg->ops_AttrList))
                setFlag(data->flags, FLG_OwnFont);

              // initialize our temporary rastport
              InitRastPort(&data->tmprp);

              // walk through all attributes and check if
              // they were set during OM_NEW
              mSet(cl, obj, msg);
              data->visual_y = 1;
              data->xpos = 0;

              // start with an inactive cursor
              data->currentCursorState = CS_INACTIVE;

              data->textColor = -1;
              data->textRGB = 0xff000000;
              data->highlightColor = -1;
              data->highlightRGB = 0xff00000;
              data->cursorcolor = -1;
              data->markedcolor = -1;
              data->separatorshine = -1;
              data->separatorshadow = -1;
              data->inactivecolor = -1;
              data->backgroundcolor = -1;

              // forget about any possible detected changes due to the initial text import
              data->HasChanged = FALSE;
              data->ContentsChanged = FALSE;
              data->MetaDataChanged = FALSE;
              data->ChangeEvent = FALSE;

              RETURN((IPTR)obj);
              return (IPTR)obj;
            }
          }
        }
      }
    }
    CoerceMethod(cl, obj, OM_DISPOSE);
  }

  RETURN(FALSE);
  return FALSE;
}

///
/// mDispose()
static IPTR mDispose(struct IClass *cl, Object *obj, Msg msg)
{
  struct InstData *data = INST_DATA(cl, obj);

  ENTER();

  FreeUndoBuffer(data);

  data->blockinfo.startline = NULL;
  data->blockinfo.stopline = NULL;

  // free all lines with their contents
  FreeTextMem(data, &data->linelist);
  Free_LineNode(data, &data->inactiveContents);

  FreeKeywords(data);

  if(data->mylocale != NULL)
  {
    CloseLocale(data->mylocale);
    data->mylocale = NULL;
  }

  if(data->mypool != NULL)
  {
    #if defined(__amigaos4__)
    FreeSysObject(ASOT_MEMPOOL, data->mypool);
    #else
    DeletePool(data->mypool);
    #endif
    data->mypool = NULL;
  }

  LEAVE();
  return DoSuperMethodA(cl, obj, msg);
}

///
/// mSetup()
static IPTR mSetup(struct IClass *cl, Object *obj, Msg msg)
{
  IPTR result = FALSE;
  struct InstData *data = INST_DATA(cl, obj);

  ENTER();

  if(GetBitMapAttr(_screen(obj)->RastPort.BitMap, BMA_DEPTH) > 8)
    setFlag(data->flags, FLG_Truecolor);
  else
    clearFlag(data->flags, FLG_Truecolor);

  // initialize the configuration of our TextEditor
  // object from the configuration set by the user
  InitConfig(cl, obj);

  if(DoSuperMethodA(cl, obj, msg))
  {
    // disable that the object will automatically get a border when
    // the ActiveObjectOnClick option is active
    if(isFlagSet(data->flags, FLG_ActiveOnClick))
      _flags(obj) |= (1<<7);

    // now we check whether we have a valid font or not
    // and if not we take the default one of our muiAreaData
    if(isFlagClear(data->flags, FLG_OwnFont))
      data->font = (data->use_fixedfont == TRUE) ? data->fixedfont : data->normalfont;
    else
      data->font = NULL;

    if(data->font == NULL)
      data->font = _font(obj);

    // initialize our temporary rastport
    InitRastPort(&data->tmprp);
    SetFont(&data->tmprp, data->font);

    // calculate the amount of pixels a Tab<>Spaces conversion will take
    data->TabSizePixels = data->TabSize*TextLength(&data->tmprp, " ", 1);
    D(DBF_INPUT, "TabSizePixels: %d", data->TabSizePixels);

    data->mousemove = FALSE;
    data->ehnode.ehn_Priority = 0;
    data->ehnode.ehn_Flags    = MUI_EHF_GUIMODE;
    data->ehnode.ehn_Object   = obj;
    data->ehnode.ehn_Class    = cl;
    data->ehnode.ehn_Events   = IDCMP_INACTIVEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY;

    #if defined(__amigaos4__)
    data->ehnode.ehn_Events  |= IDCMP_EXTENDEDMOUSE;
    #endif

    // setup the selection pointer if this is requested and MUI doesn't already
    // handle this for us
    if(data->selectPointer == TRUE && xget(obj, MUIA_PointerType) == MUIV_PointerType_Normal)
    {
      setFlag(data->ehnode.ehn_Events, IDCMP_MOUSEMOVE);
      SetupSelectPointer(data);
    }

    data->ihnode.ihn_Object   = obj;
    data->ihnode.ihn_Millis   = 20;
    data->ihnode.ihn_Method   = MUIM_TextEditor_InputTrigger;
    data->ihnode.ihn_Flags    = MUIIHNF_TIMER;

    DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->ehnode);

    data->smooth_wait = 0;
    data->scrollaction = FALSE;

    #if defined(__amigaos3__)
    // allocate an exclusive pen for truecolor screens
    // this will be SetRGB32()'d before it is actually SetAPen()'d
    if(isFlagSet(data->flags, FLG_Truecolor))
	  data->exclusivePen = ObtainPen(_screen(obj)->ViewPort.ColorMap, -1, 0, 0, 0, PENF_EXCLUSIVE|PENF_NO_SETCOLOR);
	else
	  data->exclusivePen = -1;
    #else
    data->exclusivePen = -1;
    #endif

    // convert all pens to RGB values if RGB mode is active
    if(data->rgbMode == TRUE)
      ConvertPensToRGB(data);

    setFlag(data->flags, FLG_SetupDone);

    result = TRUE;
  }

  if(result == FALSE)
    FreeConfig(cl, obj);

  RETURN(result);
  return result;
}

///
/// mCleanup
static IPTR mCleanup(struct IClass *cl, Object *obj, Msg msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  IPTR result = 0;

  ENTER();

  if(data->exclusivePen != -1)
  {
    ReleasePen(_screen(obj)->ViewPort.ColorMap, data->exclusivePen);
    data->exclusivePen = -1;
  }

  // cleanup the selection pointer
  CleanupSelectPointer(data);

  DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->ehnode);

  DisposeSuggestWindow(data);

  // enable that the object will automatically get a border when
  // the ActiveObjectOnClick option is active
  if(isFlagSet(data->flags, FLG_ActiveOnClick))
    _flags(obj) &= ~(1<<7);

  if(data->mousemove == TRUE)
  {
    data->mousemove = FALSE;
    RejectInput(data);
  }

  FreeConfig(cl, obj);

  clearFlag(data->flags, FLG_SetupDone);

  result = DoSuperMethodA(cl, obj, msg);

  RETURN(result);
  return result;
}

///
/// mAskMinMax()
static IPTR mAskMinMax(struct IClass *cl, Object *obj, struct MUIP_AskMinMax *msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  struct MUI_MinMax *mi;
  LONG fontheight;

  ENTER();

  // call the supermethod first
  DoSuperMethodA(cl, obj, (Msg)msg);

  mi = ((struct MUIP_AskMinMax *)msg)->MinMaxInfo;

  if(data->Columns != 0)
  {
    // for the font width we take the nominal font width provided by the tf_XSize attribute
    LONG width = data->Columns * (data->font ? data->font->tf_XSize : _font(obj)->tf_XSize);

    mi->MinWidth += width;
    mi->DefWidth += width;
    if(isFlagSet(data->flags, FLG_FreeHoriz))
      mi->MaxWidth = MUI_MAXMAX;
    else
      mi->MaxWidth += width;
  }
  else
  {
    mi->MinWidth += 40;
    mi->DefWidth += 300;
    mi->MaxWidth = MUI_MAXMAX;
  }

  fontheight = data->font ? data->font->tf_YSize : _font(obj)->tf_YSize;
  if(data->Rows != 0)
  {
    LONG height = data->Rows * fontheight;

    mi->MinHeight += height;
    mi->DefHeight += height;
    if(isFlagSet(data->flags, FLG_FreeVert))
      mi->MaxHeight = MUI_MAXMAX;
    else
      mi->MaxHeight += height;
  }
  else
  {
    mi->MinHeight +=  1 * fontheight;
    mi->DefHeight += 15 * fontheight;
    mi->MaxHeight = MUI_MAXMAX;
  }

  RETURN(0);
  return 0;
}

///
/// mShow()
static IPTR mShow(struct IClass *cl, Object *obj, Msg msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  LONG bmwidth;
  LONG bmheight;

  ENTER();

  DoSuperMethodA(cl, obj, msg);

  // now we check whether we have a valid font or not
  // and if not we take the default one of our muiAreaData
  if(data->font == NULL)
    data->font = _font(obj);

  data->rport       = _rp(obj);
  data->fontheight  = data->font->tf_YSize;
  data->maxlines    = _mheight(obj) / data->fontheight;
  data->ypos        = _mtop(obj);

  data->totallines = CountLines(data, &data->linelist);
  data->longestline = data->WrapMode == MUIV_TextEditor_WrapMode_NoWrap ? LongestLine(data) : _mwidth(data->object);

  data->shown = TRUE;
  data->update = FALSE;
  ScrollIntoDisplay(data);
  data->update = TRUE;
  data->shown = FALSE;

  SetAttrs(obj,
    MUIA_TextEditor_Prop_DeltaFactor, data->fontheight,
    MUIA_TextEditor_Prop_Entries,
              ((data->totallines-(data->visual_y-1) < data->maxlines) ?
                ((data->visual_y-1)+data->maxlines) :
                ((data->maxlines > data->totallines) ?
                  data->maxlines :
                  data->totallines))
                * data->fontheight,
    MUIA_TextEditor_Prop_First,     (data->visual_y-1)*data->fontheight,
    MUIA_TextEditor_Prop_Visible,     data->maxlines*data->fontheight,
    TAG_DONE);

  SetAttrs(obj,
    MUIA_TextEditor_HSlider_Ent,
              (data->longestline - (LONG)(data->xpos) < _mwidth(obj)) ?
               ((data->xpos) + _mwidth(obj)) :
               ((_mwidth(obj) > data->longestline) ?
                 _mwidth(obj) : data->longestline),
    MUIA_TextEditor_HSlider_Pos, data->xpos,
    MUIA_TextEditor_HSlider_Vis, _mwidth(obj),
    TAG_DONE);

  // initialize the doublebuffering rastport
  bmwidth = _mwidth(obj)+((data->fontheight-data->font->tf_Baseline+1)>>1)+1;
  bmheight = data->fontheight;
  if((data->doublebuffer = MUIG_AllocBitMap(bmwidth, bmheight, GetBitMapAttr(data->rport->BitMap, BMA_DEPTH), (BMF_CLEAR | BMF_INTERLEAVED), data->rport->BitMap)) != NULL)
  {
    struct Layer_Info *li;

    if((li = NewLayerInfo()) != NULL)
    {
      struct Layer *l;

      if((l = CreateUpfrontLayer(li, data->doublebuffer, 0, 0, bmwidth-1, bmheight-1, LAYERSIMPLE, NULL)) != NULL)
      {
        data->doublerp = l->rp;
        SetFont(data->doublerp, data->font);
      }
	  else
	  {
	    DisposeLayerInfo(li);
	  }
	}
  }

  // initialize the copyrp rastport
  data->copyrp = *_rp(obj);
  SetFont(&data->copyrp, data->font);

  // initialize our temporary rastport
  InitRastPort(&data->tmprp);
  SetFont(&data->tmprp, data->font);

  if(data->SuggestWindow != NULL)
    set(data->SuggestWindow, MUIA_Window_Open, isFlagSet(data->flags, FLG_PopWindow));

  data->shown = TRUE;

  RETURN(TRUE);
  return TRUE;
}

///
/// mHide()
static IPTR mHide(struct IClass *cl, Object *obj, Msg msg)
{
  struct InstData *data = INST_DATA(cl, obj);

  ENTER();

  data->shown = FALSE;
  HideSelectPointer(data, obj);
  if(data->SuggestWindow != NULL)
    set(data->SuggestWindow, MUIA_Window_Open, FALSE);
  set(_win(obj), MUIA_Window_DisableKeys, 0L);

  if(data->doublerp != NULL)
  {
    struct Layer_Info *li = data->doublerp->Layer->LayerInfo;

    DeleteLayer(0, data->doublerp->Layer);
    DisposeLayerInfo(li);
    data->doublerp = NULL;
  }

  if(data->doublebuffer != NULL)
    MUIG_FreeBitMap(data->doublebuffer);

  data->rport = NULL;

  LEAVE();
  return DoSuperMethodA(cl, obj, msg);
}

///
/// mDraw()
static IPTR mDraw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
  struct InstData *data = INST_DATA(cl, obj);

  ENTER();

  DoSuperMethodA(cl, obj, (Msg)msg);

  if(isFlagSet(msg->flags, MADF_DRAWUPDATE) && data->UpdateInfo != NULL)
  {
    setFlag(data->flags, FLG_Draw);
    data->UpdateInfo = (APTR)_Dispatcher(cl, obj, (Msg)data->UpdateInfo);
    clearFlag(data->flags, FLG_Draw);
  }

  if(isFlagSet(msg->flags, MADF_DRAWOBJECT))
  {
    SetFont(data->rport, data->font);

/*    This cases crash on simplerefresh,
    when something  covers  window and
    contents  scroll  (gfxcard  only!)
*/ /* data->update = FALSE;
    ScrollIntoDisplay(data);
    data->update = TRUE;
*/
    // dump all text now
    DumpText(data, data->visual_y, 0, data->maxlines, FALSE);

    // make sure we ghost out the whole area in case
    // the gadget was flagged as being ghosted.
    if(isFlagSet(data->flags, FLG_Ghosted) && isFlagClear(data->flags, FLG_MUI4))
    {
      UWORD newPattern[] = {0x1111, 0x4444};

      SetDrMd(data->rport, JAM1);
      SetAPen(data->rport, _pens(obj)[MPEN_SHADOW]);
      SetAfPt(data->rport, newPattern, 1);
      RectFill(data->rport, _left(obj), _top(obj), _right(obj), _bottom(obj));
      SetAfPt(data->rport, NULL, (UBYTE)-1);
    }
  }

  RETURN(0);
  return 0;
}

///
/// mGoActive
IPTR mGoActive(struct IClass *cl, Object *obj, Msg msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  IPTR result;

  ENTER();

  // set the gadgets flags to active and also "activated" so that
  // other functions know that the gadget was activated recently.
  setFlag(data->flags, FLG_Active);
  setFlag(data->flags, FLG_Activated);

  if(data->shown == TRUE)
  {
    if(IsEmptyContents(data) == TRUE)
      DumpText(data, data->visual_y, 0, data->maxlines, FALSE);

    SetCursor(data, data->CPos_X, data->actualline, TRUE);

    // in case we ought to show a selected area in a different
    // color than in inactive state we call MarkText()
    if(isFlagSet(data->flags, FLG_ActiveOnClick) && Enabled(data))
      MarkText(data, data->blockinfo.startx, data->blockinfo.startline, data->blockinfo.stopx, data->blockinfo.stopline);

    if(isFlagClear(data->flags, FLG_ReadOnly))
      set(_win(obj), MUIA_Window_DisableKeys, MUIKEYF_GADGET_NEXT);
  }

  if(data->BlinkSpeed == 1)
  {
    DoMethod(_app(obj), MUIM_Application_AddInputHandler, &data->blinkhandler);
    data->BlinkSpeed = 2;
  }

  result = DoSuperMethodA(cl, obj, msg);

  RETURN(result);
  return result;
}

///
/// mGoInactive
IPTR mGoInactive(struct IClass *cl, Object *obj, Msg msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  IPTR result;

  ENTER();

  // clear the active and activated flag so that others know about it
  clearFlag(data->flags, FLG_Active);
  clearFlag(data->flags, FLG_Activated);

  if(data->shown == TRUE)
    set(_win(obj), MUIA_Window_DisableKeys, 0L);

  if(data->mousemove == TRUE)
  {
    data->mousemove = FALSE;
    RejectInput(data);
  }

  if(data->scrollaction == TRUE)
    data->smooth_wait = 1;

  if(data->BlinkSpeed == 2)
  {
    DoMethod(_app(obj), MUIM_Application_RemInputHandler, &data->blinkhandler);
    data->BlinkSpeed = 1;
  }

  if(IsEmptyContents(data) == TRUE)
  {
    PrintLine(data, 0, &data->inactiveContents, 1, TRUE);
  }
  else
  {
    SetCursor(data, data->CPos_X, data->actualline, FALSE);

    // in case we ought to show a selected area in a different
    // color than in inactive state we call MarkText()
    if(isFlagSet(data->flags, FLG_ActiveOnClick) && Enabled(data))
      MarkText(data, data->blockinfo.startx, data->blockinfo.startline, data->blockinfo.stopx, data->blockinfo.stopline);
  }

  result = DoSuperMethodA(cl, obj, msg);

  RETURN(result);
  return result;
}

///
/// mInsertText
IPTR mInsertText(struct IClass *cl, Object *obj, struct MUIP_TextEditor_InsertText *msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  struct marking block;
  IPTR result;

  ENTER();

  switch(msg->pos)
  {
    case MUIV_TextEditor_InsertText_Top:
    {
      GoTop(data);
    }
    break;

    case MUIV_TextEditor_InsertText_Bottom:
    {
      GoBottom(data);
    }
    break;
  }

  block.startx = data->CPos_X;
  block.startline = data->actualline;
  result = InsertText(data, msg->text, TRUE);
  block.stopx = data->CPos_X;
  block.stopline = data->actualline;
  AddToUndoBuffer(data, ET_PASTEBLOCK, &block);

  RETURN(result);
  return result;
}

///
/// mExport
IPTR mExport(UNUSED struct IClass *cl, Object *obj, struct MUIP_Export *msg)
{
  ULONG id;

  ENTER();

  if((id = (muiNotifyData(obj)->mnd_ObjectID)) != 0)
  {
    STRPTR contents;

    if((contents = (STRPTR)DoMethod(obj, MUIM_TextEditor_ExportText)) != NULL)
    {
      DoMethod(msg->dataspace, MUIM_Dataspace_Add, contents, strlen(contents)+1, id);
      FreeVec(contents);
    }
  }

  RETURN(0);
  return 0;
}

///
/// mImport
IPTR mImport(UNUSED struct IClass *cl, Object *obj, struct MUIP_Import *msg)
{
  ULONG id;

  ENTER();

  if((id = (muiNotifyData(obj)->mnd_ObjectID)) != 0)
  {
    STRPTR contents = (STRPTR)DoMethod(msg->dataspace, MUIM_Dataspace_Find, id);

    set(obj, MUIA_TextEditor_Contents, contents != NULL ? (IPTR)contents : (IPTR)"");
  }

  RETURN(0);
  return 0;
}

///
/// mRedraw
IPTR mRedraw(UNUSED struct IClass *cl, Object *obj, struct MUIP_TextEditor_Redraw *msg)
{
  ENTER();

  MUI_Redraw(obj, msg->flags);

  RETURN(0);
  return 0;
}

///
/// _Dispatcher()
DISPATCHER(_Dispatcher)
{
  struct InstData *data;
  LONG t_totallines;
  LONG t_visual_y;
  BOOL t_hasChanged;
  BOOL t_contentsChanged;
  BOOL t_metaDataChanged;
  LONG t_xpos;
  LONG t_longestline;
  struct TEColor t_pen;
  BOOL areamarked;
  IPTR result = 0;

  ENTER();

  //D(DBF_STARTUP, "Method: 0x%lx\n", msg->MethodID);
  //D(DBF_STARTUP, "Stack usage: %ld %lx", (ULONG)FindTask(NULL)->tc_SPUpper - (ULONG)FindTask(NULL)->tc_SPReg);

  // this one must be catched before we try to obtain the instance data, because nobody
  // will guarantee that the pointer returned by INST_DATA() is valid if no object has
  // been created yet!!
  if(msg->MethodID == OM_NEW)
  {
    result = mNew(cl, obj, (struct opSet *)msg);

    RETURN(result);
    return result;
  }

  // now get the instance data
  if((data = INST_DATA(cl, obj)) == NULL)
  {
    ASSERT(data != NULL);

    RETURN(0);
    return 0;
  }

  // set some variables:
  t_totallines = data->totallines;
  t_visual_y = data->visual_y;
  t_hasChanged = data->HasChanged;
  t_contentsChanged = data->ContentsChanged;
  t_metaDataChanged = data->MetaDataChanged;
  t_pen = data->Pen;
  areamarked = Enabled(data);
  t_xpos        = data->xpos;
  t_longestline = data->longestline;

//  D(DBF_STARTUP, "cont...");

  if(data->shown == TRUE && isFlagClear(data->flags, FLG_Draw))
  {
    switch(msg->MethodID)
    {
      case MUIM_TextEditor_ARexxCmd:
      case MUIM_TextEditor_InsertText:
      case MUIM_TextEditor_InputTrigger:
      case MUIM_TextEditor_ToggleCursor:
      case MUIM_TextEditor_MarkText:
      case MUIM_TextEditor_ClearText:
      case MUIM_TextEditor_SetBlock:
      case MUIM_HandleEvent:
      case MUIM_GoInactive:
      case MUIM_GoActive:
      {
        data->UpdateInfo = msg;
        MUI_Redraw(obj, MADF_DRAWUPDATE);
        result = (IPTR)data->UpdateInfo;
        data->UpdateInfo = NULL;

        RETURN(result);
        return result;
      }
    }
  }

  switch(msg->MethodID)
  {
    case OM_DISPOSE:                      result = mDispose(cl, obj, msg);                  RETURN(result); return result; break;
    case OM_GET:                          result = mGet(cl, obj, (APTR)msg);                RETURN(result); return result; break;
    case OM_SET:                          result = mSet(cl, obj, (APTR)msg);                                               break;
    case MUIM_Setup:                      result = mSetup(cl, obj, msg);                    RETURN(result); return result; break;
    case MUIM_Show:                       result = mShow(cl, obj, msg);                     RETURN(result); return result; break;
    case MUIM_AskMinMax:                  result = mAskMinMax(cl, obj, (APTR)msg);          RETURN(result); return result; break;
    case MUIM_Draw:                       result = mDraw(cl, obj, (APTR)msg);               RETURN(result); return result; break;
    case MUIM_Hide:                       result = mHide(cl, obj, msg);                     RETURN(result); return result; break;
    case MUIM_Cleanup:                    result = mCleanup(cl, obj, msg);                  RETURN(result); return result; break;
    case MUIM_Export:                     result = mExport(cl, obj, (APTR)msg);                                            break;
    case MUIM_Import:                     result = mImport(cl, obj, (APTR)msg);                                            break;
    case MUIM_GoActive:                   result = mGoActive(cl, obj, msg);                 RETURN(result); return result; break;
    case MUIM_GoInactive:                 result = mGoInactive(cl, obj, msg);               RETURN(result); return result; break;
    case MUIM_HandleEvent:
    {
      LONG oldx = data->CPos_X;
      struct line_node *oldy = data->actualline;

      // process all input events
      result = mHandleInput(cl, obj, (struct MUIP_HandleEvent *)msg);

      // see if the cursor was moved and if so we go and trigger the notifications
      if(data->CPos_X != oldx || data->actualline != oldy)
      {
        LONG cposY = LineNr(data, data->actualline)-1;
        LONG index;

        DoMethod(obj, MUIM_TextEditor_CursorXYToIndex, data->CPos_X, cposY, &index);
        SetSuperAttrs(cl, obj,
          (data->CPos_X != oldx)     ? MUIA_TextEditor_CursorX : TAG_IGNORE, data->CPos_X,
          (data->actualline != oldy) ? MUIA_TextEditor_CursorY : TAG_IGNORE, cposY,
          MUIA_TextEditor_CursorIndex, index,
          TAG_DONE);
      }

      if(data->WrapMode == MUIV_TextEditor_WrapMode_NoWrap)
      {
        /* If we are in the NoWrapMode and the cursor position changed horizontally
           we should check if the cursor got out of gadget bounds and scroll it into view! */
        if(data->CPos_X != oldx)
          ScrollIntoView(data);

        if(data->hslider)
        {
          LONG prop_entries = xget(data->hslider, MUIA_Prop_Entries);

          if(prop_entries != data->longestline)
          {
            SetAttrs(data->object, MUIA_TextEditor_HSlider_Ent,
                    (data->longestline - (LONG)(data->xpos) < _mwidth(obj)) ?
                    ((data->xpos) + _mwidth(obj)) :
                    ((_mwidth(obj) > data->longestline) ?
                      _mwidth(obj) : data->longestline),
                      TAG_DONE);
          }
        }
      }

      // if the HandleInput() function didn't return
      // an MUI_EventHandlerRC_Eat we can return immediately
      if(result == 0)
      {
        RETURN(0);
        return 0;
      }
    }
    break;

    case MUIM_TextEditor_ClearText:       result = mClearText(cl, obj, msg);                                               break;
    case MUIM_TextEditor_ToggleCursor:    result = mToggleCursor(cl, obj, msg);             RETURN(result); return result; break;
    case MUIM_TextEditor_InputTrigger:    result = mInputTrigger(cl, obj, msg);                                            break;
    case MUIM_TextEditor_InsertText:      result = mInsertText(cl, obj, (APTR)msg);                                        break;
    case MUIM_TextEditor_ExportBlock:     result = mExportBlock(cl, obj, (APTR)msg);        RETURN(result); return result; break;
    case MUIM_TextEditor_ExportText:      result = mExportText(cl, obj, (APTR)msg);         RETURN(result); return result; break;
    case MUIM_TextEditor_ARexxCmd:        result = mHandleARexx(cl, obj, (APTR)msg);                                       break;
    case MUIM_TextEditor_MarkText:        result = mMarkText(data, (APTR)msg);                                             break;
    case MUIM_TextEditor_BlockInfo:       result = mBlockInfo(data, (APTR)msg);                                            break;
    case MUIM_TextEditor_Search:          result = mSearch(cl, obj, (APTR)msg);                                            break;
    case MUIM_TextEditor_Replace:         result = mReplace(cl, obj, (APTR)msg);                                           break;
    case MUIM_TextEditor_QueryKeyAction:  result = mQueryKeyAction(cl, obj, (APTR)msg);                                    break;
    case MUIM_TextEditor_SetBlock:        result = mSetBlock(data, (APTR)msg);              RETURN(result); return result; break;
    case MUIM_TextEditor_CursorXYToIndex: result = mCursorXYToIndex(data, (APTR)msg);       RETURN(result); return result; break;
    case MUIM_TextEditor_IndexToCursorXY: result = mIndexToCursorXY(data, (APTR)msg);       RETURN(result); return result; break;
    case MUIM_TextEditor_Redraw:          result = mRedraw(cl, obj, (APTR)msg);             RETURN(result); return result; break;
    case MUIM_TextEditor_TestPos:         result = mTestPos(data, (APTR)msg);               RETURN(result); return result; break;
    default:                              result = DoSuperMethodA(cl, obj, msg);            RETURN(result); return result; break;
  }

  if(t_hasChanged != data->HasChanged || t_contentsChanged != data->ContentsChanged || t_metaDataChanged != data->MetaDataChanged)
  {
    // don't trigger unwanted notifications in case an application sets an attribute
    // like MUIA_TextEditor_HasChanged in parallel with MUIA_NoNotify=TRUE
    BOOL nonotify;
    if(msg->MethodID == OM_SET)
      nonotify = GetTagData(MUIA_NoNotify, FALSE, ((struct opSet *)msg)->ops_AttrList);
    else
      nonotify = FALSE;

    SetAttrs(obj,
      MUIA_NoNotify, nonotify,
      t_hasChanged != data->HasChanged           ? MUIA_TextEditor_HasChanged      : TAG_IGNORE, data->HasChanged,
      t_contentsChanged != data->ContentsChanged ? MUIA_TextEditor_ContentsChanged : TAG_IGNORE, data->ContentsChanged,
      t_metaDataChanged != data->MetaDataChanged ? MUIA_TextEditor_MetaDataChanged : TAG_IGNORE, data->MetaDataChanged,
      TAG_DONE);
  }

  if(data->ChangeEvent && data->WrapMode == MUIV_TextEditor_WrapMode_NoWrap)
  {
    data->longestline = LongestLine(data);
    data->ChangeEvent = FALSE;
  }

  if(msg->MethodID == OM_SET)
  {
    ULONG newresult = DoSuperMethodA(cl, obj, msg);

    if(result)
      result = newresult;
    else
    {
      RETURN(newresult);
      return newresult;
    }
  }

  if(data->visual_y != t_visual_y || data->totallines != t_totallines)
  {
    SetAttrs(obj, MUIA_TextEditor_Prop_Entries,
              ((data->totallines-(data->visual_y-1) < data->maxlines) ?
                ((data->visual_y-1)+data->maxlines) :
                ((data->maxlines > data->totallines) ?
                  data->maxlines :
                  data->totallines))
                * data->fontheight,
              MUIA_TextEditor_Prop_First, (data->visual_y-1)*data->fontheight,
              TAG_DONE);
  }

  if(data->WrapMode == MUIV_TextEditor_WrapMode_NoWrap && data->hslider)
  {
    if(data->xpos != t_xpos || data->longestline != t_longestline)
    {
      SetAttrs(obj, MUIA_TextEditor_HSlider_Ent,
                 (data->longestline - data->xpos < _mwidth(obj)) ?
                  ((data->xpos) + _mwidth(obj)) :
                  ((_mwidth(obj) > data->longestline) ?
                    _mwidth(obj) : data->longestline),
                MUIA_TextEditor_HSlider_Pos, data->xpos,
                TAG_DONE);
    }
  }

  data->NoNotify = TRUE;
  if(Enabled(data))
  {
    struct marking newblock;

    NiceBlock(&data->blockinfo, &newblock);
    GetColor(data, data->blockinfo.stopx - ((data->blockinfo.stopx && newblock.startx == data->blockinfo.startx && newblock.startline == data->blockinfo.startline) ? 1 : 0), data->blockinfo.stopline, &data->Pen);
  }
  else
  {
    GetColor(data, data->CPos_X, data->actualline, &data->Pen);
  }

  if(IsSameColor(&t_pen, &data->Pen) == FALSE)
    set(obj, MUIA_TextEditor_Pen, data->Pen.color);

  if(isFlagClear(data->flags, FLG_GlobalFlow) && data->actualline->line.Flow != data->Flow)
  {
    data->Flow = data->actualline->line.Flow;
    set(obj, MUIA_TextEditor_Flow, data->actualline->line.Flow);
  }
  else if(isFlagSet(data->flags, FLG_GlobalFlow) && data->GlobalFlow != data->Flow)
  {
    data->Flow = data->GlobalFlow;
    set(obj, MUIA_TextEditor_Flow, data->GlobalFlow);
  }

  if(data->actualline->line.Separator != data->Separator)
  {
    data->Separator = data->actualline->line.Separator;
    set(obj, MUIA_TextEditor_Separator, data->actualline->line.Separator);
  }

  if(areamarked != Enabled(data))
    set(obj, MUIA_TextEditor_AreaMarked, Enabled(data));

  data->NoNotify = FALSE;
  UpdateStyles(data);

  RETURN(result);
  return result;
}

///
