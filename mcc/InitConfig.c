/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2014 TextEditor.mcc Open Source Team

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
#include <libraries/mui.h>
#include <clib/alib_protos.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/diskfont.h>
#include <proto/graphics.h>
#include <proto/keymap.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include "TextEditor_mcp.h"
#include "private.h"
#include "Debug.h"

/// GetFont()
static struct TextFont *GetFont(Object *obj, long attr, const char *def)
{
  struct TextFont *font = NULL;
  char *setting;

  ENTER();

  if(DoMethod(obj, MUIM_GetConfigItem, attr, &setting) == FALSE)
    setting = (char *)def;

  if(setting != NULL && setting[0] != '\0')
  {
    char *fontname;
    int fontnameLen;

    fontnameLen = strlen(setting)+6;
    if((fontname = AllocVecShared(fontnameLen, MEMF_CLEAR)) != NULL)
    {
      char *sizePtr;
      struct TextAttr textAttr;

      textAttr.ta_Name = fontname;
      textAttr.ta_YSize = 8;
      textAttr.ta_Style = FS_NORMAL;
      textAttr.ta_Flags = 0;

      strlcpy(fontname, setting, fontnameLen);
      if((sizePtr = strchr(fontname, '/')) != NULL)
      {
        LONG size;

        StrToLong(sizePtr + 1, &size);
        strlcpy(sizePtr, ".font", fontnameLen-(sizePtr-fontname));
        textAttr.ta_YSize = size;
      }

      font = OpenDiskFont(&textAttr);
    }

    FreeVec(fontname);
  }

  RETURN(font);
  return font;
}

///
/// SetCol()
static LONG SetCol(Object *obj, long item, const void *def)
{
  struct MUI_PenSpec *spec;
  LONG pen;

  ENTER();

  if(DoMethod(obj, MUIM_GetConfigItem, item, &spec))
    pen = MUI_ObtainPen(muiRenderInfo(obj), spec, 0L);
  else
    pen = MUI_ObtainPen(muiRenderInfo(obj), (struct MUI_PenSpec *)def, 0L);

  RETURN(pen);
  return pen;
}

///
/// InitConfig()
void kprintf(const char *,...);
void InitConfig(struct IClass *cl, Object *obj)
{
  struct InstData *data = INST_DATA(cl, obj);
  IPTR setting = 0;
  LONG value;
  BOOL loadDefaultKeys = FALSE;
  LONG oldTabSize = data->TabSize;

  ENTER();

  data->textcolor         = SetCol(obj, MUICFG_TextEditor_TextColor,       CFG_TextEditor_TextColor_Def);
  data->cursorcolor       = SetCol(obj, MUICFG_TextEditor_CursorColor,     CFG_TextEditor_CursorColor_Def);
  data->cursortextcolor   = SetCol(obj, MUICFG_TextEditor_CursorTextColor, CFG_TextEditor_CursorTextColor_Def);
  data->highlightcolor    = SetCol(obj, MUICFG_TextEditor_HighlightColor,  CFG_TextEditor_HighlightColor_Def);
  data->markedcolor       = SetCol(obj, MUICFG_TextEditor_MarkedColor,     CFG_TextEditor_MarkedColor_Def);
  data->separatorshine    = SetCol(obj, MUICFG_TextEditor_SeparatorShine,  CFG_TextEditor_SeparatorShine_Def);
  data->separatorshadow   = SetCol(obj, MUICFG_TextEditor_SeparatorShadow, CFG_TextEditor_SeparatorShadow_Def);
  data->inactivecolor     = SetCol(obj, MUICFG_TextEditor_InactiveColor,   CFG_TextEditor_InactiveColor_Def);

  if(isFlagClear(data->flags, FLG_OwnBackground))
  {
    IPTR background = MUII_BACKGROUND;

    data->backgroundcolor = -1;

    if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_Background, &setting) && setting != 0)
    {
      char *bg_setting = (char *)setting;

      if(bg_setting[0] == '2' && bg_setting[1] == ':' )
      {
        struct MUI_PenSpec *spec = (struct MUI_PenSpec *)(bg_setting+2);

        data->backgroundcolor = MUI_ObtainPen(muiRenderInfo(obj), spec, 0L);
      }

      background = (IPTR)setting;
    }
    set(obj, MUIA_Background, background);
  }
  else
  {
    data->backgroundcolor   = SetCol(obj, MUICFG_TextEditor_Background, CFG_TextEditor_Background_Def);
  }

  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_TabSize, &setting))
  {
    ULONG size = MINMAX(2, *(ULONG *)setting, 12);

    // use the configured value only if the TAB size is not yet overridden
    if(isFlagClear(data->flags, FLG_ForcedTabSize))
      data->TabSize = size;

    // remember the configured value in case the TAB size is being reset to the default value
    data->GlobalTabSize = size;
  }
  else
  {
    // assume the default value for both
    data->TabSize = CFG_TextEditor_TabSize_Def;
    data->GlobalTabSize = CFG_TextEditor_TabSize_Def;
  }

  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_CursorWidth, &setting))
    data->CursorWidth = MINMAX(1, *(long *)setting, 6);
  else
    data->CursorWidth = CFG_TextEditor_CursorWidth_Def;

  data->normalfont = GetFont(obj, MUICFG_TextEditor_NormalFont, CFG_TextEditor_NormalFont_Def);
  data->fixedfont = GetFont(obj, MUICFG_TextEditor_FixedFont, CFG_TextEditor_FixedFont_Def);

  if(data->fixedfont != NULL && isFlagSet(data->fixedfont->tf_Flags, FPF_PROPORTIONAL))
  {
    // somehow the user managed to choose a proportional font as fixed font,
    // reject this one and fall back to MUI's internal fixed font if required
    CloseFont(data->fixedfont);
    data->fixedfont = NULL;
  }

  if(data->use_fixedfont == TRUE && data->fixedfont == NULL)
    set(obj, MUIA_Font, MUIV_Font_Fixed);
  else if(data->normalfont == NULL)
    set(obj, MUIA_Font, MUIV_Font_Normal);

  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_BlockQual, &setting))
    value = *(LONG *)setting;
  else
    value = CFG_TextEditor_BlockQual_Def;
  switch(value)
  {
    default:
    case 0:
      data->blockqual = IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT;
      break;
    case 1:
      data->blockqual = IEQUALIFIER_CONTROL;
      break;
    case 2:
      data->blockqual = IEQUALIFIER_LALT | IEQUALIFIER_RALT;
      break;
    case 3:
      data->blockqual = 0;
      break;
  }

  data->BlinkSpeed = 0;
  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_BlinkSpeed, &setting))
    value = *(LONG *)setting;
  else
    value = CFG_TextEditor_BlinkSpeed_Def;
  if(value != 0)
  {
    data->blinkhandler.ihn_Object    = obj;
    data->blinkhandler.ihn_Millis    = MINMAX(1, value, 20) * 25;
    data->blinkhandler.ihn_Method    = MUIM_TextEditor_ToggleCursor;
    data->blinkhandler.ihn_Flags     = MUIIHNF_TIMER;
    data->BlinkSpeed = 1;
  }

  if(isFlagClear(data->flags, FLG_OwnFrame))
  {
    if(MUIMasterBase->lib_Version >= 20)
      set(obj, MUIA_Frame, DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_Frame, &setting) ? (STRPTR)setting : (STRPTR)CFG_TextEditor_Frame_Def);
    else
      set(obj, MUIA_Frame, MUIV_Frame_String);
  }

  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_TypeNSpell, &setting))
    data->TypeAndSpell = *(LONG *)setting;
  else
    data->TypeAndSpell = CFG_TextEditor_TypeNSpell_Def;
  set(obj, MUIA_TextEditor_TypeAndSpell, data->TypeAndSpell);

  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_CheckWord, &setting))
    value = *(LONG *)setting;
  else
    value = CFG_TextEditor_CheckWord_Def;
  if(value)
    setFlag(data->flags, FLG_CheckWords);
  else
    clearFlag(data->flags, FLG_CheckWords);

  data->Flow = data->actualline->line.Flow;
  data->Pen = GetColor(data->CPos_X, data->actualline);

  if(isFlagClear(data->flags, FLG_FirstInit))
  {
    setFlag(data->flags, FLG_FirstInit);
    data->NoNotify = TRUE;
    SetAttrs(obj,
      MUIA_FillArea, FALSE,
      MUIA_TextEditor_Flow, data->Flow,
      MUIA_TextEditor_Pen, data->Pen,
      MUIA_TextEditor_AreaMarked, FALSE,
      MUIA_TextEditor_UndoAvailable, FALSE,
      MUIA_TextEditor_RedoAvailable, FALSE,
      MUIA_TextEditor_HasChanged, FALSE,
      TAG_DONE);
    data->NoNotify = FALSE;
  }

  UpdateStyles(data);

  if(data->slider != NULL)
  {
    if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_Smooth, &setting))
      value = *(LONG *)setting != 0;
    else
      value = CFG_TextEditor_Smooth_Def;
    set(data->slider, MUIA_Prop_DoSmooth, value);
  }

  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_SelectPointer, &setting))
    data->selectPointer = *(LONG *)setting != 0;
  else
    data->selectPointer = CFG_TextEditor_SelectPointer_Def;

  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_InactiveCursor, &setting))
    data->inactiveCursor =  *(LONG *)setting != 0;
  else
    data->inactiveCursor = CFG_TextEditor_InactiveCursor_Def;

  {
    ULONG undoSteps;

    // get the saved undo size only if it was not yet set by the application
    if(data->userUndoBufferSize == FALSE)
    {
      if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_UndoSize, &setting))
      {
        undoSteps = *(long *)setting;

        // constrain the number of undo levels only if undo is enabled
        if(undoSteps != 0)
          undoSteps = MAX(undoSteps, 20);
      }
      else
        undoSteps = CFG_TextEditor_UndoSize_Def;
    }
    else
      undoSteps = data->maxUndoSteps;

    ResizeUndoBuffer(data, undoSteps);
  }

  data->LookupSpawn = CFG_TextEditor_LookupExeType_Def;
  strlcpy(data->LookupCmd, CFG_TextEditor_LookupCommand_Def, sizeof(data->LookupCmd));
  // we still handle the obsolete combined setting of exe type and command
  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_LookupCmd, &setting))
  {
    data->LookupSpawn = (BOOL)*(ULONG *)setting;
    strlcpy(data->LookupCmd, (char *)setting+4, sizeof(data->LookupCmd));
  }
  // but we prefer the new separated settings
  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_LookupExeType, &setting))
  {
    data->LookupSpawn = (BOOL)*(ULONG *)setting;
  }
  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_LookupCommand, &setting))
  {
    strlcpy(data->LookupCmd, (char *)setting, sizeof(data->LookupCmd));
  }

  data->SuggestSpawn = CFG_TextEditor_SuggestExeType_Def;
  strlcpy(data->SuggestCmd, CFG_TextEditor_SuggestCommand_Def, sizeof(data->SuggestCmd));
  // we still handle the obsolete combined setting of exe type and command
  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_SuggestCmd, &setting))
  {
    data->SuggestSpawn = (BOOL)*(ULONG *)setting;
    strlcpy(data->SuggestCmd, (char *)setting+4, sizeof(data->SuggestCmd));
  }
  // but we prefer the new separated settings
  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_SuggestExeType, &setting))
  {
    data->SuggestSpawn = (BOOL)*(ULONG *)setting;
  }
  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_SuggestCommand, &setting))
  {
    strlcpy(data->SuggestCmd, (char *)setting, sizeof(data->SuggestCmd));
  }

  if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_ConfigVersion, &setting))
  {
    if(*(ULONG *)setting != CONFIG_VERSION)
    {
      if(MUI_Request(_app(obj), NULL, 0L, "TextEditor.mcc Warning", "Ok|Abort",
                                          "Your current keybindings setup of TextEditor.mcc\n"
                                          "was found to be incompatible with this version of\n"
                                          "TextEditor.mcc.\n"
                                          "\n"
                                          "The keybindings of this object will be temporarly\n"
                                          "set to the default. Please visit the MUI preferences\n"
                                          "of TextEditor.mcc to permanently update the keybindings.") == 1)
      {
        loadDefaultKeys = TRUE;
      }
    }
  }

  {
    struct te_key *userkeys;
    ULONG count = 0;
    ULONG size;

    setting = 0;
    if(loadDefaultKeys == TRUE || !DoMethod(obj, MUIM_GetConfigItem, MUICFG_TextEditor_Keybindings, &setting) || setting == 0)
      userkeys = (struct te_key *)default_keybindings;
    else
      userkeys = (struct te_key *)setting;

    while((WORD)userkeys[count].code != -1)
      count++;

    // now we calculate the memory size
    size = (count+1)*sizeof(struct te_key);

    if((data->RawkeyBindings = AllocVecPooled(data->mypool, size)) != NULL)
    {
      ULONG i;
      struct te_key *mykeys = data->RawkeyBindings;

      memcpy(mykeys, userkeys, size);

      for(i=0; i < count && (WORD)mykeys[i].code != -1; i++)
      {
        struct te_key *curKey = &mykeys[i];

        //D(DBF_STARTUP, "checking curKey[%d]: %08lx", i, curKey);

        if(curKey->code >= 500)
        {
          char RAW[4];
          char code = curKey->code-500;

          MapANSI(&code, 1, RAW, 1, NULL);

          curKey->code = RAW[0];
          curKey->qual |= RAW[1];

          if(RAW[0] == 67 && isFlagClear(curKey->qual, IEQUALIFIER_NUMERICPAD))
          {
            curKey->code = 68;
          }
        }
      }
    }
  }

  if(data->TabSize != oldTabSize)
  {
    // reimport the current text if the TAB size has changed
    ReimportText(cl, obj);
  }

  LEAVE();
}

///
/// FreeConfig()
void FreeConfig(struct IClass *cl, Object *obj)
{
  struct InstData *data = INST_DATA(cl, obj);

  ENTER();

  if(data->RawkeyBindings != NULL)
    FreeVecPooled(data->mypool, data->RawkeyBindings);

  if(data->textcolor != -1)
  {
    MUI_ReleasePen(muiRenderInfo(obj), data->textcolor);
    data->textcolor = -1;
  }
  if(data->cursorcolor != -1)
  {
    MUI_ReleasePen(muiRenderInfo(obj), data->cursorcolor);
    data->cursorcolor = -1;
  }
  if(data->cursortextcolor != -1)
  {
    MUI_ReleasePen(muiRenderInfo(obj), data->cursortextcolor);
    data->cursortextcolor = -1;
  }
  if(data->highlightcolor != -1)
  {
    MUI_ReleasePen(muiRenderInfo(obj), data->highlightcolor);
    data->highlightcolor = -1;
  }
  if(data->markedcolor != -1)
  {
    MUI_ReleasePen(muiRenderInfo(obj), data->markedcolor);
    data->markedcolor = -1;
  }
  if(data->separatorshine != -1)
  {
    MUI_ReleasePen(muiRenderInfo(obj), data->separatorshine);
    data->separatorshine = -1;
  }
  if(data->separatorshadow != -1)
  {
    MUI_ReleasePen(muiRenderInfo(obj), data->separatorshadow);
    data->separatorshadow = -1;
  }
  if(data->inactivecolor != -1)
  {
    MUI_ReleasePen(muiRenderInfo(obj), data->inactivecolor);
    data->inactivecolor = -1;
  }
  if(data->backgroundcolor != -1)
  {
    MUI_ReleasePen(muiRenderInfo(obj), data->backgroundcolor);
    data->backgroundcolor = -1;
  }

  if(data->normalfont != NULL)
    CloseFont(data->normalfont);
  if(data->fixedfont != NULL)
    CloseFont(data->fixedfont);

  if(data->BlinkSpeed == 2)
  {
    DoMethod(_app(data->object), MUIM_Application_RemInputHandler, &data->blinkhandler);
    data->BlinkSpeed = 1;
  }

  LEAVE();
}

///
