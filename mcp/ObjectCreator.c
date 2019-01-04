/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2019 TextEditor.mcc Open Source Team

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

#include <clib/alib_protos.h>
#include <devices/inputevent.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

#include <mui/TextEditor_mcc.h>

#include "locale.h"
#include "private.h"
#include "muiextra.h"

const char *FunctionName (UWORD func)
{
  const char *name;

  switch(func)
  {
    case MUIV_TextEditor_KeyAction_Up:
      name = tr(MSG_Function_Up);
      break;

    case MUIV_TextEditor_KeyAction_Down:
      name = tr(MSG_Function_Down);
      break;

    case MUIV_TextEditor_KeyAction_Left:
      name = tr(MSG_Function_Left);
      break;

    case MUIV_TextEditor_KeyAction_Right:
      name = tr(MSG_Function_Right);
      break;

    case MUIV_TextEditor_KeyAction_PageUp:
      name = tr(MSG_Function_PrvPage);
      break;

    case MUIV_TextEditor_KeyAction_PageDown:
      name = tr(MSG_Function_NxtPage);
      break;

    case MUIV_TextEditor_KeyAction_StartOfLine:
      name = tr(MSG_Function_BOL);
      break;

    case MUIV_TextEditor_KeyAction_EndOfLine:
      name = tr(MSG_Function_EOL);
      break;

    case MUIV_TextEditor_KeyAction_Top:
      name = tr(MSG_Function_Top);
      break;

    case MUIV_TextEditor_KeyAction_Bottom:
      name = tr(MSG_Function_Bottom);
      break;

    case MUIV_TextEditor_KeyAction_PrevWord:
      name = tr(MSG_Function_PrvWord);
      break;

    case MUIV_TextEditor_KeyAction_NextWord:
      name = tr(MSG_Function_NxtWord);
      break;

    case MUIV_TextEditor_KeyAction_PrevLine:
      name = tr(MSG_Function_PrvPara);
      break;

    case MUIV_TextEditor_KeyAction_NextLine:
      name = tr(MSG_Function_NxtPara);
      break;

    case MUIV_TextEditor_KeyAction_PrevSentence:
      name = tr(MSG_Function_PrvSent);
      break;

    case MUIV_TextEditor_KeyAction_NextSentence:
      name = tr(MSG_Function_NxtSent);
      break;

    case MUIV_TextEditor_KeyAction_SuggestWord:
      name = tr(MSG_Function_SuggestSpelling);
      break;

    case MUIV_TextEditor_KeyAction_Backspace:
      name = tr(MSG_Function_Backspace);
      break;

    case MUIV_TextEditor_KeyAction_Delete:
      name = tr(MSG_Function_Delete);
      break;

    case MUIV_TextEditor_KeyAction_Return:
      name = tr(MSG_Function_Return);
      break;

    case MUIV_TextEditor_KeyAction_Tab:
      name = tr(MSG_Function_Tab);
      break;

    case MUIV_TextEditor_KeyAction_Cut:
      name = tr(MSG_Function_Cut);
      break;

    case MUIV_TextEditor_KeyAction_Copy:
      name = tr(MSG_Function_Copy);
      break;

    case MUIV_TextEditor_KeyAction_Paste:
      name = tr(MSG_Function_Paste);
      break;

    case MUIV_TextEditor_KeyAction_Undo:
      name = tr(MSG_Function_Undo);
      break;

    case MUIV_TextEditor_KeyAction_Redo:
      name = tr(MSG_Function_Redo);
      break;

    case MUIV_TextEditor_KeyAction_DelEOL:
      name = tr(MSG_Function_DelEOL);
      break;

    case MUIV_TextEditor_KeyAction_DelBOL:
      name = tr(MSG_Function_DelBOL);
      break;

    case MUIV_TextEditor_KeyAction_DelEOW:
      name = tr(MSG_Function_DelEOW);
      break;

    case MUIV_TextEditor_KeyAction_DelBOW:
      name = tr(MSG_Function_DelBOW);
      break;

    case MUIV_TextEditor_KeyAction_DelLine:
      name = tr(MSG_Function_DelLine);
      break;

    case MUIV_TextEditor_KeyAction_NextGadget:
      name = tr(MSG_Function_NextGadget);
      break;

    case MUIV_TextEditor_KeyAction_GotoBookmark1:
      name = tr(MSG_Function_GotoBookmark1);
      break;

    case MUIV_TextEditor_KeyAction_GotoBookmark2:
      name = tr(MSG_Function_GotoBookmark2);
      break;

    case MUIV_TextEditor_KeyAction_GotoBookmark3:
      name = tr(MSG_Function_GotoBookmark3);
      break;

    case MUIV_TextEditor_KeyAction_SetBookmark1:
      name = tr(MSG_Function_SetBookmark1);
      break;

    case MUIV_TextEditor_KeyAction_SetBookmark2:
      name = tr(MSG_Function_SetBookmark2);
      break;

    case MUIV_TextEditor_KeyAction_SetBookmark3:
      name = tr(MSG_Function_SetBookmark3);
      break;

    case MUIV_TextEditor_KeyAction_SelectAll:
      name = tr(MSG_Function_SelectAll);
      break;

    case MUIV_TextEditor_KeyAction_SelectNone:
      name = tr(MSG_Function_SelectNone);
      break;

    default:
      name = "";
  }

  return(name);
}

#define ARRAY_SIZE(X) (sizeof(X)/sizeof(X[0]))

static Object *PrefsObject(struct InstData_MCP *data)
{
  static const void *titles[] = {
    MSG_Page_Settings,
    MSG_Page_Keybindings,
    MSG_Page_SpellChecker,
    MSG_Page_Sample
  };
  static const void *functions[] = {
    MSG_Function_Up,
    MSG_Function_Down,
    MSG_Function_Left,
    MSG_Function_Right,
    MSG_Function_PrvPage,
    MSG_Function_NxtPage,
    MSG_Function_BOL,
    MSG_Function_EOL,
    MSG_Function_Top,
    MSG_Function_Bottom,
    MSG_Function_PrvWord,
    MSG_Function_NxtWord,
    MSG_Function_PrvPara,
    MSG_Function_NxtPara,
    MSG_Function_PrvSent,
    MSG_Function_NxtSent,
    MSG_Function_SuggestSpelling,
    MSG_Function_Backspace,
    MSG_Function_Delete,
    MSG_Function_Return,
    MSG_Function_Tab,
    MSG_Function_Cut,
    MSG_Function_Copy,
    MSG_Function_Paste,
    MSG_Function_Undo,
    MSG_Function_Redo,
    MSG_Function_DelBOL,
    MSG_Function_DelEOL,
    MSG_Function_DelBOW,
    MSG_Function_DelEOW,
    MSG_Function_NextGadget,
    MSG_Function_GotoBookmark1,
    MSG_Function_GotoBookmark2,
    MSG_Function_GotoBookmark3,
    MSG_Function_SetBookmark1,
    MSG_Function_SetBookmark2,
    MSG_Function_SetBookmark3,
    MSG_Function_DelLine,
    MSG_Function_SelectAll,
    MSG_Function_SelectNone
  };
  static const void *cycleentries[] = {
    MSG_CycleItem_Shift,
    MSG_CycleItem_Ctrl,
    MSG_CycleItem_Alt,
    MSG_CycleItem_Mouse
  };
  unsigned int i;

  ASSERT( ARRAY_SIZE(data->gTitles) == (ARRAY_SIZE(titles)+1) );
  for(i=0; i<ARRAY_SIZE(titles); i++)
    data->gTitles[i] = tr(titles[i]);
  data->gTitles[ARRAY_SIZE(titles)] = NULL;

  ASSERT( ARRAY_SIZE(data->functions) == (ARRAY_SIZE(functions)+1) );
  for(i=0; i<ARRAY_SIZE(functions); i++)
    data->functions[i] = tr(functions[i]);
  data->functions[ARRAY_SIZE(functions)] = NULL;

  ASSERT( ARRAY_SIZE(data->execution) == 3 );
  data->execution[0] = tr(MSG_Execution_CLI);
  data->execution[1] = tr(MSG_Execution_ARexx);
  data->execution[2] = NULL;

  ASSERT( ARRAY_SIZE(data->cycleentries) == (ARRAY_SIZE(cycleentries)+1) );
  for(i=0; i<ARRAY_SIZE(cycleentries); i++)
    data->cycleentries[i] = tr(cycleentries[i]);
  data->cycleentries[ARRAY_SIZE(cycleentries)] = NULL;

  data->obj = CreatePrefsGroup(data);
  if(data->obj)
  {
    set(data->o[MCP_NormalFont], MUIA_String_AdvanceOnCR, TRUE);
    set(data->o[MCP_FixedFont], MUIA_String_AdvanceOnCR, TRUE);

    DoMethod(data->obj, MUIM_MultiSet, MUIA_CycleChain, TRUE,
            data->o[MCP_BlockQual],
            data->o[MCP_TabSize],
            data->o[MCP_Smooth],
            data->o[MCP_NormalFont],
            data->o[MCP_FixedFont],
            data->o[MCP_TextColor],
            data->o[MCP_Frame],
            data->o[MCP_HighlightColor],
            data->o[MCP_Background],
            data->o[MCP_CursorColor],
            data->o[MCP_MarkedColor],
            data->o[MCP_CursorWidth],
            data->deletekey,
            data->o[MCP_BlinkSpeed],
            data->o[MCP_SuggestExeType],
            data->o[MCP_SuggestCommand],
            data->o[MCP_LookupExeType],
            data->o[MCP_LookupCommand],
            data->o[MCP_TypeNSpell],
            data->o[MCP_UndoSize],
            data->o[MCP_CheckWord],
            data->insertkey,
            data->o[MCP_SeparatorShadow],
            data->o[MCP_SeparatorShine],
            data->o[MCP_InactiveCursor],
            data->o[MCP_SelectPointer],
            data->o[MCP_InactiveColor],
            NULL);
  }

  return(data->obj);
}

static const struct
{
  LONG index;
  ULONG cfgAttr;
  ULONG attr;
  APTR defValue;
} prefsInfo[MCP_Count] =
{
  { MCP_Background,      MUICFG_TextEditor_Background,      MUIA_Imagedisplay_Spec, (APTR)CFG_TextEditor_Background_Def      },
  { MCP_BlinkSpeed,      MUICFG_TextEditor_BlinkSpeed,      MUIA_Numeric_Value,     (APTR)CFG_TextEditor_BlinkSpeed_Def      },
  { MCP_BlockQual,       MUICFG_TextEditor_BlockQual,       MUIA_Cycle_Active,      (APTR)CFG_TextEditor_BlockQual_Def       },
  { MCP_CheckWord,       MUICFG_TextEditor_CheckWord,       MUIA_Selected,          (APTR)CFG_TextEditor_CheckWord_Def       },
  { MCP_CursorColor,     MUICFG_TextEditor_CursorColor,     MUIA_Pendisplay_Spec,   (APTR)CFG_TextEditor_CursorColor_Def     },
  { MCP_CursorWidth,     MUICFG_TextEditor_CursorWidth,     MUIA_Numeric_Value,     (APTR)CFG_TextEditor_CursorWidth_Def     },
  { MCP_FixedFont,       MUICFG_TextEditor_FixedFont,       MUIA_String_Contents,   (APTR)CFG_TextEditor_FixedFont_Def       },
  { MCP_Frame,           MUICFG_TextEditor_Frame,           MUIA_Framedisplay_Spec, (APTR)CFG_TextEditor_Frame_Def           },
  { MCP_HighlightColor,  MUICFG_TextEditor_HighlightColor,  MUIA_Pendisplay_Spec,   (APTR)CFG_TextEditor_HighlightColor_Def  },
  { MCP_MarkedColor,     MUICFG_TextEditor_MarkedColor,     MUIA_Pendisplay_Spec,   (APTR)CFG_TextEditor_MarkedColor_Def     },
  { MCP_NormalFont,      MUICFG_TextEditor_NormalFont,      MUIA_String_Contents,   (APTR)CFG_TextEditor_NormalFont_Def      },
  { MCP_Smooth,          MUICFG_TextEditor_Smooth,          MUIA_Selected,          (APTR)CFG_TextEditor_Smooth_Def          },
  { MCP_TabSize,         MUICFG_TextEditor_TabSize,         MUIA_Numeric_Value,     (APTR)CFG_TextEditor_TabSize_Def         },
  { MCP_TextColor,       MUICFG_TextEditor_TextColor,       MUIA_Pendisplay_Spec,   (APTR)CFG_TextEditor_TextColor_Def       },
  { MCP_UndoSize,        MUICFG_TextEditor_UndoSize,        MUIA_Numeric_Value,     (APTR)CFG_TextEditor_UndoSize_Def        },
  { MCP_TypeNSpell,      MUICFG_TextEditor_TypeNSpell,      MUIA_Selected,          (APTR)CFG_TextEditor_TypeNSpell_Def      },
  { MCP_LookupExeType,   MUICFG_TextEditor_LookupExeType,   MUIA_Cycle_Active,      (APTR)CFG_TextEditor_LookupExeType_Def   },
  { MCP_LookupCommand,   MUICFG_TextEditor_LookupCommand,   MUIA_String_Contents,   (APTR)CFG_TextEditor_LookupCommand_Def   },
  { MCP_SuggestExeType,  MUICFG_TextEditor_SuggestCommand,  MUIA_Cycle_Active,      (APTR)CFG_TextEditor_SuggestExeType_Def  },
  { MCP_SuggestCommand,  MUICFG_TextEditor_SuggestCommand,  MUIA_String_Contents,   (APTR)CFG_TextEditor_SuggestCommand_Def  },
  { MCP_Keybindings,     MUICFG_TextEditor_Keybindings,     0,                      (APTR)0                                  },
  { MCP_SeparatorShine,  MUICFG_TextEditor_SeparatorShine,  MUIA_Pendisplay_Spec,   (APTR)CFG_TextEditor_SeparatorShine_Def  },
  { MCP_SeparatorShadow, MUICFG_TextEditor_SeparatorShadow, MUIA_Pendisplay_Spec,   (APTR)CFG_TextEditor_SeparatorShadow_Def },
  { MCP_InactiveCursor,  MUICFG_TextEditor_InactiveCursor,  MUIA_Selected,          (APTR)CFG_TextEditor_InactiveCursor_Def  },
  { MCP_SelectPointer,   MUICFG_TextEditor_SelectPointer,   MUIA_Selected,          (APTR)CFG_TextEditor_SelectPointer_Def   },
  { MCP_InactiveColor,   MUICFG_TextEditor_InactiveColor,   MUIA_Pendisplay_Spec,   (APTR)CFG_TextEditor_InactiveColor_Def   },
};

IPTR New(REG(a0, struct IClass *cl), REG(a2, Object *obj), REG(a1, struct opSet *msg))
{
  if((obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg)) != NULL)
  {
    struct InstData_MCP *data = INST_DATA(cl, obj);

    // create the main prefs object
    Object *prefsobject = PrefsObject(data);

    if((data->CfgObj = prefsobject) != NULL)
    {
      DoMethod(obj, OM_ADDMEMBER, prefsobject);

      if(MUIMasterBase->lib_Version >= 20)
      {
        ULONG i;

        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->o[MCP_Keybindings], MUICFG_TextEditor_Keybindings, 1, tr(MSG_Page_Keybindings)       );

		for(i = 0; i < MCP_Count; i++)
		{
          if(i == MCP_Keybindings)
            continue;

          DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->o[prefsInfo[i].index], prefsInfo[i].cfgAttr, 2, NULL, prefsInfo[i].attr);
        }
      }
      else
        set(data->o[MCP_Frame], MUIA_Disabled, TRUE);

      return (IPTR)obj;
    }

    CoerceMethod(cl, obj, OM_DISPOSE);
  }

  return (IPTR)NULL;
}

IPTR Dispose(REG(a0, struct IClass *cl), REG(a2, Object *obj), REG(a1, Msg msg))
{
  struct InstData_MCP *data = INST_DATA(cl, obj);
  Object *editpopup = data->editpopup;

  if(data->CfgObj)
  {
    DoMethod(obj, OM_REMMEMBER, data->CfgObj);
    MUI_DisposeObject(data->CfgObj);
  }

  if(editpopup)
    MUI_DisposeObject(editpopup);

  return(DoSuperMethodA(cl, obj, msg));
}

IPTR GadgetsToConfig(REG(a0, struct IClass *cl), REG(a2, Object *obj), REG(a1, struct MUIP_Mccprefs_GadgetsToConfig *msg))
{
  struct InstData_MCP *data = INST_DATA(cl, obj);
  ULONG i;
  ULONG version = CONFIG_VERSION;
  BOOL mui38;

  ENTER();

  mui38 = !LIB_VERSION_IS_AT_LEAST(MUIMasterBase, 20, 0);
  // first save the config version
  DoMethod(msg->configdata, MUIM_Dataspace_Add, &version, sizeof(version), MUICFG_TextEditor_ConfigVersion);

  ExportKeys(data, msg->configdata);

  for(i = 0; i < MCP_Count; i++)
  {
    if(i == MCP_Keybindings)
      continue;

    if(mui38 == TRUE || msg->originator == NULL || msg->originator == data->o[prefsInfo[i].index])
    {
      switch(prefsInfo[i].attr)
      {
        case MUIA_Pendisplay_Spec:
        case MUIA_Framedisplay_Spec:
        case MUIA_Imagedisplay_Spec:
        {
          char *v = (char *)xget(data->o[prefsInfo[i].index], prefsInfo[i].attr);

          if(v != NULL && stricmp(v, prefsInfo[i].defValue) != 0)
            DoMethod(msg->configdata, MUIM_Dataspace_Add, v, strlen(v)+1, prefsInfo[i].cfgAttr);
          else
            DoMethod(msg->configdata, MUIM_Dataspace_Remove, prefsInfo[i].cfgAttr);
        }
        break;

        case MUIA_String_Contents:
        {
          char *v = (char *)xget(data->o[prefsInfo[i].index], prefsInfo[i].attr);

          if(v != NULL && strcmp(v, prefsInfo[i].defValue) != 0)
            DoMethod(msg->configdata, MUIM_Dataspace_Add, v, strlen(v)+1, prefsInfo[i].cfgAttr);
          else
            DoMethod(msg->configdata, MUIM_Dataspace_Remove, prefsInfo[i].cfgAttr);
        }
        break;

        default:
        {
          ULONG v = xget(data->o[prefsInfo[i].index], prefsInfo[i].attr);

          if(v != (ULONG)prefsInfo[i].defValue)
            DoMethod(msg->configdata, MUIM_Dataspace_Add, &v, sizeof(v), prefsInfo[i].cfgAttr);
          else
            DoMethod(msg->configdata, MUIM_Dataspace_Remove, prefsInfo[i].cfgAttr);
        }
        break;
      }
    }
  }

  // erase old obsolete items
  DoMethod(msg->configdata, MUIM_Dataspace_Remove, MUICFG_TextEditor_LookupCmd);
  DoMethod(msg->configdata, MUIM_Dataspace_Remove, MUICFG_TextEditor_SuggestCmd);

  RETURN(0);
  return(0);
}

IPTR ConfigToGadgets(REG(a0, struct IClass *cl), REG(a2, Object *obj), REG(a1, struct MUIP_Mccprefs_ConfigToGadgets *msg))
{
  struct InstData_MCP *data = INST_DATA(cl, obj);
  APTR cfg_data;
  ULONG i;
  BOOL importKeys = TRUE;

  if((cfg_data = (APTR)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_ConfigVersion)) != NULL)
  {
    D(DBF_ALWAYS, "config version %ld vs %ld\n", *((ULONG *)cfg_data), CONFIG_VERSION);
    if(*((ULONG *)cfg_data) != CONFIG_VERSION)
    {
      if(MUI_Request(NULL, NULL, 0L, tr(MSG_WarnConfigVersion_Title), tr(MSG_ResetAbort), tr(MSG_WarnConfigVersion)) == 1)
      {
        // reset the keybindings to their default values.
        ImportKeys(data, NULL);
        importKeys = FALSE;
      }
    }
  }

  if(importKeys == TRUE)
    ImportKeys(data, msg->configdata);

  for(i = 0; i < MCP_Count; i++)
  {
    APTR v;

    if(i == MCP_Keybindings)
      continue;

    if((v = (APTR)DoMethod(msg->configdata, MUIM_Dataspace_Find, prefsInfo[i].cfgAttr)) != NULL)
    {
      switch(prefsInfo[i].attr)
      {
        case MUIA_Framedisplay_Spec:
        case MUIA_Imagedisplay_Spec:
        case MUIA_Pendisplay_Spec:
        {
          // don't accept empty frame, image or pen specs
          if(*(char *)v == '\0')
            v = prefsInfo[i].defValue;
        }
        break;

        case MUIA_String_Contents:
        {
          // nothing to do
        }
        break;

        default:
        {
          v = (APTR)(*(LONG *)v);
        }
        break;
      }
    }
    else
    {
      // use default value
      v = prefsInfo[i].defValue;
    }

    nnset(data->o[prefsInfo[i].index], prefsInfo[i].attr, v);
  }

  // handle some old obsolete items
  if((cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_LookupCmd)) != NULL)
  {
    nnset(data->o[MCP_LookupExeType], MUIA_Cycle_Active, *(ULONG *)cfg_data);
    nnset(data->o[MCP_LookupCommand], MUIA_String_Contents, (ULONG *)cfg_data+1);
  }
  if((cfg_data = (void *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TextEditor_SuggestCmd)) != NULL)
  {
    nnset(data->o[MCP_SuggestExeType], MUIA_Cycle_Active, *(ULONG *)cfg_data);
    nnset(data->o[MCP_SuggestCommand], MUIA_String_Contents, (ULONG *)cfg_data+1);
  }

  return(0);
}
