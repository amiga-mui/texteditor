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

#include <string.h>
#include <stdio.h>

#include <clib/alib_protos.h>
#include <devices/inputevent.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

#include "Locale.h"
#include "private.h"
#include "muiextra.h"

#include "TextEditor_mcc.h"

STRPTR FunctionName (UWORD func)
{
  STRPTR name;

  switch(func)
  {
    case mUp:
      name = CatString(MSG_Function_Up, "Up");
      break;

    case mDown:
      name = CatString(MSG_Function_Down, "Down");
      break;

    case mLeft:
      name = CatString(MSG_Function_Left, "Left");
      break;

    case mRight:
      name = CatString(MSG_Function_Right, "Right");
      break;

    case mPreviousPage:
      name = CatString(MSG_Function_PrvPage, "Prev Page");
      break;

    case mNextPage:
      name = CatString(MSG_Function_NxtPage, "Next Page");
      break;

    case mStartOfLine:
      name = CatString(MSG_Function_BOL, "Begin Of Line");
      break;

    case mEndOfLine:
      name = CatString(MSG_Function_EOL, "End Of Line");
      break;

    case mTop:
      name = CatString(MSG_Function_Top, "Top");
      break;

    case mBottom:
      name = CatString(MSG_Function_Bottom, "Bottom");
      break;

    case mPreviousWord:
      name = CatString(MSG_Function_PrvWord, "Prev Word");
      break;

    case mNextWord:
      name = CatString(MSG_Function_NxtWord, "Next Word");
      break;

    case mPreviousLine:
      name = CatString(MSG_Function_PrvPara, "Prev Paragraph");
      break;

    case mNextLine:
      name = CatString(MSG_Function_NxtPara, "Next Paragraph");
      break;

    case mPreviousSentence:
      name = CatString(MSG_Function_PrvSent, "Prev Sentence");
      break;

    case mNextSentence:
      name = CatString(MSG_Function_NxtSent, "Next Sentence");
      break;

    case kSuggestWord:
      name = CatString(MSG_Function_SuggestSpelling, "Suggest spelling");
      break;

    case kBackspace:
      name = CatString(MSG_Function_Backspace, "Backspace");
      break;

    case kDelete:
      name = CatString(MSG_Function_Delete, "Delete");
      break;

    case kReturn:
      name = CatString(MSG_Function_Return, "Return");
      break;

    case kTab:
      name = CatString(MSG_Function_Tab, "Tab");
      break;

    case kCut:
      name = CatString(MSG_Function_Cut, "Cut");
      break;

    case kCopy:
      name = CatString(MSG_Function_Copy, "Copy");
      break;

    case kPaste:
      name = CatString(MSG_Function_Paste, "Paste");
      break;

    case kUndo:
      name = CatString(MSG_Function_Undo, "Undo");
      break;
    
    case kRedo:
      name = CatString(MSG_Function_Redo, "Redo");
      break;
    
    case kDelEOL:
      name = CatString(MSG_Function_DelEOL, "Delete to EOL");
      break;
    
    case kDelBOL:
      name = CatString(MSG_Function_DelBOL, "Delete to BOL");
      break;
    
    case kDelEOW:
      name = CatString(MSG_Function_DelEOW, "Delete to EOW");
      break;
    
    case kDelBOW:
      name = CatString(MSG_Function_DelBOW, "Delete to BOW");
      break;
    
    case kDelLine:
      name = CatString(MSG_Function_DelLine, "Delete line");
      break;
    
    case kNextGadget:
      name = CatString(MSG_Function_NextGadget, "Next gadget");
      break;
    
    case kGotoBookmark1:
      name = CatString(MSG_Function_GotoBookmark1, "Goto Bookmark 1");
      break;
    
    case kGotoBookmark2:
      name = CatString(MSG_Function_GotoBookmark2, "Goto Bookmark 2");
      break;
    
    case kGotoBookmark3:
      name = CatString(MSG_Function_GotoBookmark3, "Goto Bookmark 3");
      break;
    
    case kSetBookmark1:
      name = CatString(MSG_Function_SetBookmark1, "Set Bookmark 1");
      break;
    
    case kSetBookmark2:
      name = CatString(MSG_Function_SetBookmark2, "Set Bookmark 2");
      break;
    
    case kSetBookmark3:
      name = CatString(MSG_Function_SetBookmark3, "Set Bookmark 3");
      break;
    
    default:
      name = "";
  }
  return(name);
}

void *PrefsObject(struct InstData_MCP *data)
{
  data->gTitles[0] = CatString(MSG_Page_Settings, "Settings");
  data->gTitles[1] = CatString(MSG_Page_Keybindings, "Keybindings");
  data->gTitles[2] = CatString(MSG_Page_SpellChecker, "Spell checker");
  data->gTitles[3] = CatString(MSG_Page_Sample, "Sample");
  data->gTitles[4] = NULL;

  data->functions[0] = GetString(MSG_Function_Up);
  data->functions[1] = GetString(MSG_Function_Down);
  data->functions[2] = GetString(MSG_Function_Left);
  data->functions[3] = GetString(MSG_Function_Right);
  data->functions[4] = GetString(MSG_Function_PrvPage);
  data->functions[5] = GetString(MSG_Function_NxtPage);
  data->functions[6] = GetString(MSG_Function_BOL);
  data->functions[7] = GetString(MSG_Function_EOL);
  data->functions[8] = GetString(MSG_Function_Top);
  data->functions[9] = GetString(MSG_Function_Bottom);
  data->functions[10] = GetString(MSG_Function_PrvWord);
  data->functions[11] = GetString(MSG_Function_NxtWord);
  data->functions[12] = GetString(MSG_Function_PrvPara);
  data->functions[13] = GetString(MSG_Function_NxtPara);
  data->functions[14] = GetString(MSG_Function_PrvSent);
  data->functions[15] = GetString(MSG_Function_NxtSent);

  data->functions[16] = GetString(MSG_Function_SuggestSpelling);
  data->functions[17] = GetString(MSG_Function_Backspace);
  data->functions[18] = GetString(MSG_Function_Delete);
  data->functions[19] = GetString(MSG_Function_Return);
  data->functions[20] = GetString(MSG_Function_Tab);
  data->functions[21] = GetString(MSG_Function_Cut);
  data->functions[22] = GetString(MSG_Function_Copy);
  data->functions[23] = GetString(MSG_Function_Paste);
  data->functions[24] = GetString(MSG_Function_Undo);
  data->functions[25] = GetString(MSG_Function_Redo);
  data->functions[26] = GetString(MSG_Function_DelBOL);
  data->functions[27] = GetString(MSG_Function_DelEOL);
  data->functions[28] = GetString(MSG_Function_DelBOW);
  data->functions[29] = GetString(MSG_Function_DelEOW);
  data->functions[30] = GetString(MSG_Function_NextGadget);
  data->functions[31] = GetString(MSG_Function_GotoBookmark1);
  data->functions[32] = GetString(MSG_Function_GotoBookmark2);
  data->functions[33] = GetString(MSG_Function_GotoBookmark3);
  data->functions[34] = GetString(MSG_Function_SetBookmark1);
  data->functions[35] = GetString(MSG_Function_SetBookmark2);
  data->functions[36] = GetString(MSG_Function_SetBookmark3);
  data->functions[37] = GetString(MSG_Function_DelLine);
  data->functions[38] = NULL;

  data->execution[0] = CatString(MSG_Execution_CLI, "CLI");
  data->execution[1] = CatString(MSG_Execution_ARexx, "ARexx");
  data->execution[2] = NULL;

  data->cycleentries[0] = CatString(MSG_CycleItem_Shift, "Shift");
  data->cycleentries[1] = CatString(MSG_CycleItem_Ctrl, "Control");
  data->cycleentries[2] = CatString(MSG_CycleItem_Alt, "Alt");
  data->cycleentries[3] = CatString(MSG_CycleItem_Mouse, "(mouse only)");
  data->cycleentries[4] = NULL;

  data->obj = CreatePrefsGroup(data);

  if(data->obj)
  {
    set(data->normalfont, MUIA_String_AdvanceOnCR, TRUE);
    set(data->fixedfont, MUIA_String_AdvanceOnCR, TRUE);

    DoMethod(data->blockqual, MUIM_MultiSet, MUIA_CycleChain, TRUE,
            data->blockqual, data->tabsize, data->smooth,
            data->normalfont, data->fixedfont, data->textcolor, data->frame,
            data->highlightcolor, data->background, data->cursorcolor,
            data->markedcolor, data->cursorwidth, data->deletekey,
            data->blinkspeed, data->suggestcmd, data->lookupcmd,
            data->typenspell, data->undosize, data->LookupExeType,
            data->SuggestExeType, data->CheckWord, data->insertkey,
            data->separatorshadow, data->separatorshine, NULL);

  }
  return(data->obj);
}

ULONG New(REG(a0, struct IClass *cl), REG(a2, Object *obj), REG(a1, struct opSet *msg))
{
  if((obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg)))
  {
    struct InstData_MCP *data = INST_DATA(cl, obj);

    if((data->text_class = MUI_CreateCustomClass(NULL, "Text.mui", NULL, 0, (APTR)Text_Dispatcher)))
    {
      Object *prefsobject = PrefsObject(data);

      if((data->CfgObj = prefsobject))
      {
        DoMethod(obj, OM_ADDMEMBER, prefsobject);

        if(MUIMasterBase->lib_Version >= 20)
        {
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->LookupExeType, MUICFG_TextEditor_LookupCmd, 1, GetString(MSG_Label_LookupCmd));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->lookupcmd, MUICFG_TextEditor_LookupCmd, 1, GetString(MSG_Label_LookupCmd));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->SuggestExeType, MUICFG_TextEditor_SuggestCmd, 1, GetString(MSG_Label_SuggestCmd));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->suggestcmd, MUICFG_TextEditor_SuggestCmd, 1, GetString(MSG_Label_SuggestCmd));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->keybindings, MUICFG_TextEditor_Keybindings, 1, GetString(MSG_Page_Keybindings));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->frame, MUICFG_TextEditor_Frame, 1, GetString(MSG_Label_Frame));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->background, MUICFG_TextEditor_Background, 1, GetString(MSG_Label_Background));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->blinkspeed, MUICFG_TextEditor_BlinkSpeed, 1, GetString(MSG_Label_BlinkSpeed));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->blockqual, MUICFG_TextEditor_BlockQual, 1, GetString(MSG_Label_BlkQual));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->cursorcolor, MUICFG_TextEditor_CursorColor, 1, GetString(MSG_Label_Cursor));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->cursorwidth, MUICFG_TextEditor_CursorWidth, 1, GetString(MSG_Label_Width));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->fixedfont, MUICFG_TextEditor_FixedFont, 1, GetString(MSG_Label_Fixed));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->highlightcolor, MUICFG_TextEditor_HighlightColor, 1, GetString(MSG_Label_Highlight));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->markedcolor, MUICFG_TextEditor_MarkedColor, 1, GetString(MSG_Label_Selected));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->normalfont, MUICFG_TextEditor_NormalFont, 1, GetString(MSG_Label_Normal));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->smooth, MUICFG_TextEditor_Smooth, 1, GetString(MSG_Label_Smooth));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->typenspell, MUICFG_TextEditor_TypeNSpell, 1, CatString(MSG_ConfigMenu_TypeNSpell, "Type'n'spell"));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->CheckWord, MUICFG_TextEditor_CheckWord, 1, CatString(MSG_ConfigMenu_CheckWord, "Check word"));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->tabsize, MUICFG_TextEditor_TabSize, 1, GetString(MSG_Label_TabSize));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->undosize, MUICFG_TextEditor_UndoSize, 1, GetString(MSG_Label_UndoLevel));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->textcolor, MUICFG_TextEditor_TextColor, 1, GetString(MSG_Label_Text));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->separatorshine, MUICFG_TextEditor_SeparatorShine, 1, GetString(MSG_Label_SeparatorShine));
          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->separatorshadow, MUICFG_TextEditor_SeparatorShadow, 1, GetString(MSG_Label_SeparatorShadow));
        }
        else
        {
          set(data->frame, MUIA_Disabled, TRUE);
        }

        return((ULONG)obj);
      }
    }
    CoerceMethod(cl, obj, OM_DISPOSE);
  }
  return(FALSE);
}

ULONG Dispose(REG(a0, struct IClass *cl), REG(a2, Object *obj), REG(a1, Msg msg))
{
    struct InstData_MCP *data = INST_DATA(cl, obj);
    void   *editpopup = data->editpopup;

  DoMethod(obj, OM_REMMEMBER, data->CfgObj);
  MUI_DisposeObject(data->CfgObj);

  if(editpopup)
    MUI_DisposeObject(editpopup);

  if(data->text_class)
    MUI_DeleteCustomClass(data->text_class);

  return(DoSuperMethodA(cl, obj, msg));
}

ULONG GadgetsToConfig(REG(a0, struct IClass *cl), REG(a2, Object *obj), REG(a1, struct MUIP_Settingsgroup_GadgetsToConfig *msg))
{
  struct InstData_MCP *data = INST_DATA(cl, obj);
  LONG cfg_data = 2;

  // first save the config version
  DoMethod(msg->configdata, MUIM_Dataspace_Add, &cfg_data, sizeof(LONG), MUICFG_TextEditor_ConfigVersion);

  ExportKeys(msg->configdata, data);

  get(data->frame, MUIA_Framedisplay_Spec, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, cfg_data, sizeof(struct MUI_FrameSpec), MUICFG_TextEditor_Frame);

  get(data->background, MUIA_Imagedisplay_Spec, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, cfg_data, sizeof(struct MUI_ImageSpec), MUICFG_TextEditor_Background);

  get(data->blinkspeed, MUIA_Numeric_Value, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, &cfg_data, sizeof(LONG), MUICFG_TextEditor_BlinkSpeed);

  get(data->blockqual, MUIA_Cycle_Active, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, &cfg_data, sizeof(LONG), MUICFG_TextEditor_BlockQual);

  get(data->cursorcolor, MUIA_Pendisplay_Spec, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, cfg_data, sizeof(struct MUI_PenSpec), MUICFG_TextEditor_CursorColor);

  get(data->cursorwidth, MUIA_Numeric_Value, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, &cfg_data, sizeof(LONG), MUICFG_TextEditor_CursorWidth);

  get(data->fixedfont, MUIA_String_Contents, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, cfg_data, strlen((char *)cfg_data)+1, MUICFG_TextEditor_FixedFont);

  get(data->frame, MUIA_Framedisplay_Spec, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, cfg_data, sizeof(struct MUI_FrameSpec), MUICFG_TextEditor_Frame);

  get(data->highlightcolor, MUIA_Pendisplay_Spec, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, cfg_data, sizeof(struct MUI_PenSpec), MUICFG_TextEditor_HighlightColor);

  get(data->markedcolor, MUIA_Pendisplay_Spec, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, cfg_data, sizeof(struct MUI_PenSpec), MUICFG_TextEditor_MarkedColor);

  get(data->normalfont, MUIA_String_Contents, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, cfg_data, strlen((char *)cfg_data)+1, MUICFG_TextEditor_NormalFont);

  {
    ULONG buffer[260/4];

    get(data->LookupExeType, MUIA_Cycle_Active, &buffer);
    get(data->lookupcmd, MUIA_String_Contents, &cfg_data);
    CopyMem((void *)cfg_data, buffer+1, 256);
    DoMethod(msg->configdata, MUIM_Dataspace_Add, buffer, strlen((char *)cfg_data)+5, MUICFG_TextEditor_LookupCmd);

    get(data->SuggestExeType, MUIA_Cycle_Active, &buffer);
    get(data->suggestcmd, MUIA_String_Contents, &cfg_data);
    CopyMem((void *)cfg_data, buffer+1, 256);
    DoMethod(msg->configdata, MUIM_Dataspace_Add, buffer, strlen((char *)cfg_data)+5, MUICFG_TextEditor_SuggestCmd);
  }

  get(data->smooth, MUIA_Selected, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, &cfg_data, sizeof(LONG), MUICFG_TextEditor_Smooth);

  get(data->typenspell, MUIA_Selected, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, &cfg_data, sizeof(LONG), MUICFG_TextEditor_TypeNSpell);

  get(data->CheckWord, MUIA_Selected, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, &cfg_data, sizeof(LONG), MUICFG_TextEditor_CheckWord);

  get(data->tabsize, MUIA_Numeric_Value, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, &cfg_data, sizeof(LONG), MUICFG_TextEditor_TabSize);

  get(data->undosize, MUIA_Numeric_Value, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, &cfg_data, sizeof(LONG), MUICFG_TextEditor_UndoSize);

  get(data->textcolor, MUIA_Pendisplay_Spec, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, cfg_data, sizeof(struct MUI_PenSpec), MUICFG_TextEditor_TextColor);

  get(data->separatorshine, MUIA_Pendisplay_Spec, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, cfg_data, sizeof(struct MUI_PenSpec), MUICFG_TextEditor_SeparatorShine);

  get(data->separatorshadow, MUIA_Pendisplay_Spec, &cfg_data);
  DoMethod(msg->configdata, MUIM_Dataspace_Add, cfg_data, sizeof(struct MUI_PenSpec), MUICFG_TextEditor_SeparatorShadow);

  return(0);
}

ULONG ConfigToGadgets(REG(a0, struct IClass *cl), REG(a2, Object *obj), REG(a1, struct MUIP_Settingsgroup_ConfigToGadgets *msg))
{
  struct InstData_MCP *data = INST_DATA(cl, obj);
  APTR  cfg_data;

/*  if(cfg_data = (APTR)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_ConfigVersion))
      kprintf("Config version: %ld\n", *((ULONG *)cfg_data));
  else  kprintf("Obsolete config\n");
*/
  ImportKeys(msg->configdata, data);

  if((cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_LookupCmd)))
  {
    set(data->LookupExeType, MUIA_Cycle_Active, *(ULONG *)cfg_data);
    set(data->lookupcmd, MUIA_String_Contents, (ULONG *)cfg_data+1);
  }
  else
  {
    set(data->LookupExeType, MUIA_Cycle_Active, 0);
    set(data->lookupcmd, MUIA_String_Contents, "");
  }

  if((cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_SuggestCmd)))
  {
    set(data->SuggestExeType, MUIA_Cycle_Active, *(ULONG *)cfg_data);
    set(data->suggestcmd, MUIA_String_Contents, (ULONG *)cfg_data+1);
  }
  else
  {
    set(data->SuggestExeType, MUIA_Cycle_Active, 1);
    set(data->suggestcmd, MUIA_String_Contents, "\"Open('f', 'T:Matches', 'W');WriteLn('f', '%s');Close('f')");
  }

  set(data->frame, MUIA_Framedisplay_Spec, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_Frame)) ?  cfg_data : "302200");
  set(data->background, MUIA_Imagedisplay_Spec, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_Background)) ? cfg_data : "2:m2");
  set(data->blinkspeed, MUIA_Numeric_Value, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_BlinkSpeed)) ? *(ULONG *)cfg_data : 0);
  set(data->blockqual, MUIA_Cycle_Active, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_BlockQual)) ? *(ULONG *)cfg_data : 0);
  set(data->cursorcolor, MUIA_Pendisplay_Spec, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_CursorColor)) ? cfg_data : "m0");
  set(data->cursorwidth, MUIA_Numeric_Value, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_CursorWidth)) ? *(ULONG *)cfg_data : 6);
  set(data->fixedfont, MUIA_String_Contents, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_FixedFont)) ? cfg_data : "");
  set(data->highlightcolor, MUIA_Pendisplay_Spec, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_HighlightColor)) ? cfg_data : "m0");
  set(data->markedcolor, MUIA_Pendisplay_Spec, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_MarkedColor)) ? cfg_data : "m6");
  set(data->normalfont, MUIA_String_Contents, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_NormalFont)) ? cfg_data : "");
  set(data->smooth, MUIA_Selected, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_Smooth)) ? *(ULONG *)cfg_data : TRUE);
  set(data->typenspell, MUIA_Selected, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_TypeNSpell)) ? *(ULONG *)cfg_data : FALSE);
  set(data->CheckWord, MUIA_Selected, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_CheckWord)) ? *(ULONG *)cfg_data : FALSE);
  set(data->tabsize, MUIA_Numeric_Value, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_TabSize)) ? *(ULONG *)cfg_data : 4);
  set(data->undosize, MUIA_Numeric_Value, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_UndoSize)) ? *(ULONG *)cfg_data : 500);
  set(data->textcolor, MUIA_Pendisplay_Spec, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_TextColor)) ? cfg_data : "m5");
  set(data->separatorshine, MUIA_Pendisplay_Spec, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_SeparatorShine)) ? cfg_data : "m1");
  set(data->separatorshadow, MUIA_Pendisplay_Spec, (cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_SeparatorShadow)) ? cfg_data : "m3");

  return(0);
}
