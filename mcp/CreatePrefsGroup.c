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

#include <clib/alib_protos.h>
#include <libraries/asl.h>
#include <libraries/gadtools.h>
#include <libraries/mui.h>
#include <mui/BetterString_mcc.h>
#include <mui/HotkeyString_mcc.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

#include "private.h"
#include "Locale.h"
#include "muiextra.h"
#include "TextEditor_mcc.h"
#include "rev.h"

#include "SDI_hook.h"

HOOKPROTONH(ListDisplayFunc, void, char **array, struct te_key *entry)
{
  static char buffer[118];
  struct KeyAction ka;

  if(entry)
  {
    ka.key = entry->code;
    
    if(ka.key >= 500)
    {
      ka.vanilla = TRUE;
      ka.key -= 500;
    }
    else
      ka.vanilla = FALSE;
    
    ka.qualifier = entry->qual;
    KeyToString(buffer, &ka);

    *array++ = buffer;
    *array++ = "=";
    *array = FunctionName(entry->act);
  }
  else
  {
    *array++ = CatString(MSG_LVLabel_Key, "\33c\0333Key:");
    *array++ = "";
    *array = CatString(MSG_LVLabel_Action, "\33c\0333Action:");
  }
}
MakeStaticHook(ListDisplayHook, ListDisplayFunc);

HOOKPROTONH(ListConstructFunc, APTR, APTR pool, struct te_key *entry)
{
  struct te_key *newplace = AllocPooled(pool, sizeof(struct te_key));

  if(newplace)
  {
    CopyMem(entry, newplace, sizeof(struct te_key));
  }

  return(newplace);
}
MakeStaticHook(ListConstructHook, ListConstructFunc);

HOOKPROTONH(ListDestructFunc, void, APTR pool, struct te_key *entry)
{
  FreePooled(pool, entry, sizeof(struct te_key));
}
MakeStaticHook(ListDestructHook, ListDestructFunc);

HOOKPROTONH(Popstring_WindowCode, void, Object *pop, Object *win)
{
  set(win, MUIA_Window_DefaultObject, pop);
}
MakeStaticHook(Popstring_WindowHook, Popstring_WindowCode);

HOOKPROTONH(Popstring_OpenCode, BOOL, Object *pop, Object *text)
{
  LONG active;

  get(text, MUIA_UserData, &active);
  set(pop, MUIA_List_Active, active);
  
  return(TRUE);
}
MakeStaticHook(Popstring_OpenHook, Popstring_OpenCode);

HOOKPROTONH(Popstring_CloseCode, void, Object *pop, Object *text)
{
  LONG active;

  get(pop, MUIA_List_Active, &active);
  set(text, MUIA_UserData, active);
}
MakeStaticHook(Popstring_CloseHook, Popstring_CloseCode);

HOOKPROTONHNO(InsertCode, void, APTR **array)
{
  static const struct te_key constentry = {76, 0, 0};
  Object *keylist = (Object *)*array++;
  ULONG entry;

  get(keylist, MUIA_List_Active, &entry);

  if((LONG)entry != MUIV_List_Active_Off)
    nnset(keylist, MUIA_List_Active, MUIV_List_Active_Off);
  else
    entry = 0;

  DoMethod(keylist, MUIM_List_InsertSingle, &constentry, entry);
  set(keylist, MUIA_List_Active, entry);
}
MakeStaticHook(InsertHook, InsertCode);

HOOKPROTONHNO(SelectCode, void, APTR **array)
{
  struct InstData_MCP *data = (struct InstData_MCP *)*array++;
  Object *keylist = (Object *)*array++;
  struct te_key *entry;

  DoMethod(keylist, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &entry);
  
  if(entry)
  {
    UBYTE buffer[100];
    struct KeyAction ka;
    ULONG result;

    get(data->keyfunctions, MUIA_Popstring_String, &result);
    nnset((Object *)result, MUIA_UserData, entry->act);

    ka.key = entry->code;
    
    if(ka.key >= 500)
    {
      ka.vanilla = TRUE;
      ka.key -= 500;
    } 
    else
      ka.vanilla = FALSE;
    
    ka.qualifier = entry->qual;
    KeyToString(buffer, &ka);
    nnset(data->hotkey, MUIA_String_Contents, buffer);
  }
}
MakeStaticHook(SelectHook, SelectCode);

HOOKPROTONHNO(UpdateCode, void, APTR **array)
{
  struct InstData_MCP *data = (struct InstData_MCP *)*array++;
  Object *keylist = (Object *)*array++;
  struct te_key entry;
  ULONG result;
  ULONG active;

  get(keylist, MUIA_List_Active, &active);
  
  if((LONG)active != MUIV_List_Active_Off)
  {
    SetAttrs(keylist, MUIA_List_Quiet,  TRUE,
                MUIA_List_Active, MUIV_List_Active_Off,
                MUIA_NoNotify,    TRUE,
                TAG_DONE);
    DoMethod(keylist, MUIM_List_Remove, active);

    get(data->keyfunctions, MUIA_Popstring_String, &result);
    get((Object *)result, MUIA_UserData, &result);

    entry.act = result;

    get(data->hotkey, MUIA_String_Contents, &result);

    if(result)
    {
        struct KeyAction keyaction;

      ConvertKeyString((STRPTR)result, entry.act, &keyaction);
      entry.code = keyaction.key;
      if(keyaction.vanilla)
      {
        entry.code += 500;
      }
      entry.qual = keyaction.qualifier;
    }

    DoMethod(keylist, MUIM_List_InsertSingle, &entry, active);
    SetAttrs(keylist, MUIA_List_Active, active,
                MUIA_List_Quiet,  FALSE,
                MUIA_NoNotify,    TRUE,
                TAG_DONE);
  }
}
MakeStaticHook(UpdateHook, UpdateCode);

static APTR TxtLabel (STRPTR text, ULONG weight)
{
/*    APTR result = TextEditorObject,
        MUIA_Background, MUII_GroupBack,
        MUIA_TextEditor_ReadOnly, TRUE,
        MUIA_TextEditor_Contents, text,
        MUIA_Weight, weight,
        End;
*/
    APTR result = TextObject,
        MUIA_FramePhantomHoriz, TRUE,
        MUIA_Frame, MUIV_Frame_ImageButton,
        MUIA_Text_PreParse, "\33r",
        MUIA_Text_SetVMax, FALSE,
        MUIA_Text_Contents, text,
        MUIA_Weight, weight,
        End;

//    APTR result = MUI_MakeObject(MUIO_Label, text, 0);

  return(result);
}

Object *CreatePrefsGroup(struct InstData_MCP *data)
{
  Object *slider, *slider2, *readview, *button, *group,
         *editor, *keylist, *defaultkeys, *functionname,
         *plist, *popbutton;

  struct NewMenu editpopupdata[] =
  {
    { NM_TITLE, CatString(MSG_MenuTitle_Edit, "Edit"),    0, 0, 0, (APTR)0 },
    { NM_ITEM,  CatString(MSG_MenuItem_Cut, "Cut"),       NULL, NM_COMMANDSTRING, 0, (APTR)1 },
    { NM_ITEM,  CatString(MSG_MenuItem_Copy, "Copy"),     NULL, NM_COMMANDSTRING, 0, (APTR)2 },
    { NM_ITEM,  CatString(MSG_MenuItem_Paste, "Paste"),   NULL, NM_COMMANDSTRING, 0, (APTR)3 },
    { NM_ITEM,  CatString(MSG_MenuItem_Delete, "Delete"), NULL, NM_COMMANDSTRING, 0, (APTR)4 },
    { NM_ITEM,  NM_BARLABEL, 0, 0, 0, (APTR)0 },
    { NM_ITEM,  CatString(MSG_MenuItem_Undo, "Undo"), NULL, NM_COMMANDSTRING, 0, (APTR)5 },
    { NM_ITEM,  CatString(MSG_MenuItem_Redo, "Redo"), NULL, NM_COMMANDSTRING, 0, (APTR)6 },
    { NM_ITEM,  NM_BARLABEL, 0, 0, 0, (APTR)0 },
    { NM_ITEM,  CatString(MSG_MenuItem_Bold, "Bold"), NULL, NM_COMMANDSTRING | CHECKIT|MENUTOGGLE, 0, (APTR)7 },
    { NM_ITEM,  CatString(MSG_MenuItem_Italic, "Italic"), NULL, NM_COMMANDSTRING | CHECKIT|MENUTOGGLE, 0, (APTR)8 },
    { NM_ITEM,  CatString(MSG_MenuItem_Underline, "Underline"), NULL, NM_COMMANDSTRING | CHECKIT|MENUTOGGLE, 0, (APTR)9 },
    { NM_ITEM,  NM_BARLABEL, 0, 0, 0, (APTR)0 },
    { NM_ITEM,  CatString(MSG_MenuSubTitle_Alignment, "Alignment"), 0, 0, 0, (APTR)0 },
    { NM_SUB,   CatString(MSG_MenuItem_Left, "Left"), NULL, NM_COMMANDSTRING | CHECKIT|CHECKED, ~1, (APTR)10 },
    { NM_SUB,   CatString(MSG_MenuItem_Center, "Center"), NULL, NM_COMMANDSTRING | CHECKIT, ~2, (APTR)11 },
    { NM_SUB,   CatString(MSG_MenuItem_Right, "Right"), NULL, NM_COMMANDSTRING | CHECKIT, ~4, (APTR)12 },

    { NM_ITEM,  CatString(MSG_MenuSubTitle_Color, "Color"), 0, 0, 0, (APTR)0 },
    { NM_SUB,   CatString(MSG_MenuItem_Normal, "Normal"), NULL, NM_COMMANDSTRING | CHECKIT|CHECKED, ~1, (APTR)13 },
    { NM_SUB,   CatString(MSG_MenuItem_Shine, "Shine"), NULL, NM_COMMANDSTRING | CHECKIT, ~2, (APTR)14 },
    { NM_SUB,   CatString(MSG_MenuItem_Halfshine, "Halfshine"), NULL, NM_COMMANDSTRING | CHECKIT, ~4, (APTR)15 },
    { NM_SUB,   CatString(MSG_MenuItem_Background, "Background"), NULL, NM_COMMANDSTRING | CHECKIT, ~8, (APTR)16 },
    { NM_SUB,   CatString(MSG_MenuItem_Halfshadow, "Halfshadow"), NULL, NM_COMMANDSTRING | CHECKIT, ~16, (APTR)17 },
    { NM_SUB,   CatString(MSG_MenuItem_Shadow, "Shadow"), NULL, NM_COMMANDSTRING | CHECKIT, ~32, (APTR)18 },
    { NM_SUB,   CatString(MSG_MenuItem_Text, "Text"), NULL, NM_COMMANDSTRING | CHECKIT, ~64, (APTR)19 },
    { NM_SUB,   CatString(MSG_MenuItem_Fill, "Fill"), NULL, NM_COMMANDSTRING | CHECKIT, ~128, (APTR)20 },
    { NM_SUB,   CatString(MSG_MenuItem_Mark, "Mark"), NULL, NM_COMMANDSTRING | CHECKIT, ~256, (APTR)21 },

    { NM_END,   NULL, 0, 0, 0, (APTR)0 }
  };

  data->editpopup = MUI_MakeObject(MUIO_MenustripNM, editpopupdata, NULL);

  group = VGroup,
    Child, RegisterGroup(data->gTitles),
      MUIA_CycleChain, TRUE,
      MUIA_Background, MUII_RegisterBack,
      Child, HGroup,
        Child, VGroup,
          Child, VGroup,
            MUIA_VertWeight, 0,
            MUIA_Background, MUII_GroupBack,
            MUIA_Frame, MUIV_Frame_Group,
            MUIA_FrameTitle, CatString(MSG_GroupTitle_Control, "Control"),
            Child, RectangleObject,
            End,
            Child, ColGroup(2),
              Child, TxtLabel(CatString(MSG_Label_UndoLevel, "\33rUndo levels:"), 0),
              Child, data->undosize = SliderObject,
                MUIA_ShortHelp, CatString(HelpBubble_UndoLevel, "Each undo eats 10 bytes of memory.\nCut and Copy takes a little more,\ndepends on the block size!"),
                MUIA_Numeric_Min, 20,
                MUIA_Numeric_Max, 2000,
                MUIA_Numeric_Format, "%ld (± 5)",
              End,
              Child, TxtLabel(CatString(MSG_Label_TabSize, "\33rTab size:"), 0),
              Child, data->tabsize = SliderObject,
                MUIA_Numeric_Min, 2,
                MUIA_Numeric_Max, 12,
                MUIA_Numeric_Format, CatString(MSG_SliderText_TabSize, "%ld spaces"),
              End,
              Child, TxtLabel(CatString(MSG_Label_Smooth, "\33rSmooth:"), 0),
              Child, HGroup,
                MUIA_ShortHelp, CatString(HelpBubble_Smooth, "This will enable pixel smooth scrolling."),
                Child, data->smooth = MUI_MakeObject(MUIO_Checkmark, NULL),
                Child, RectangleObject,
                End,
              End,
            End,
            Child, RectangleObject,
            End,
          End,

          Child, ColGroup(4),
//            MUIA_VertWeight, 0,
            MUIA_Group_SameHeight, FALSE,
            MUIA_Background, MUII_GroupBack,
            MUIA_Frame, MUIV_Frame_Group,
            MUIA_FrameTitle, CatString(MSG_GroupTitle_Design, "Design:"),

            Child, TxtLabel(CatString(MSG_Label_Frame, "\33rFrame:"), 0),
            Child, data->frame = MUI_NewObject("Popframe.mui",
              MUIA_Window_Title, CatString(MSG_PopWinTitle_Frame, "Select frame"),
            End,
            Child, TxtLabel(CatString(MSG_Label_Background, "\33rBackground:"), 0),
            Child, data->background = MUI_NewObject("Popimage.mui",
              MUIA_Window_Title, CatString(MSG_PopWinTitle_Background, "Select background"),
              MUIA_Imageadjust_Type, 2,
            End,
            Child, TxtLabel(CatString(MSG_Label_Text, "\33rText"), 0),
            Child, data->textcolor = PoppenObject,
              MUIA_Window_Title, CatString(MSG_PopWinTitle_Text, "Select text color"),
            End,
            Child, TxtLabel(CatString(MSG_Label_Highlight, "\33rHighlight"), 0),
            Child, data->highlightcolor = PoppenObject,
              MUIA_Window_Title, CatString(MSG_PopWinTitle_Highlight, "Select highlight color"),
            End,
          End,

          Child, HGroup,
            MUIA_VertWeight, 60,
            MUIA_Background, MUII_GroupBack,
            MUIA_Frame, MUIV_Frame_Group,
            MUIA_FrameTitle, CatString(MSG_GroupTitle_Separator, "Separator"),
            Child, TxtLabel(CatString(MSG_Label_SeparatorShine, "\33rShine:"), 0),
            Child, data->separatorshine = PoppenObject,
              MUIA_Window_Title, CatString(MSG_PopWinTitle_SeparatorShine, "Select separators shine color"),
            End,
            Child, TxtLabel(CatString(MSG_Label_SeparatorShadow, "\33rShadow"), 0),
            Child, data->separatorshadow = PoppenObject,
              MUIA_Window_Title, CatString(MSG_PopWinTitle_SeparatorShadow, "Select separators shadow color"),
            End,
          End,
        End,

        Child, VGroup,
          Child, VGroup,
//            MUIA_VertWeight, 0,
            MUIA_Background, MUII_GroupBack,
            MUIA_Frame, MUIV_Frame_Group,
            MUIA_FrameTitle, CatString(MSG_GroupTitle_Fonts, "Fonts"),
            Child, RectangleObject,
            End,
            Child, ColGroup(2),
              Child, TxtLabel(CatString(MSG_Label_Normal, "\33rNormal"), 0),
              Child, PopaslObject,
                MUIA_Popstring_String,  data->normalfont = BetterStringObject, StringFrame, End,
                MUIA_Popstring_Button,  MUI_MakeObject(MUIO_PopButton, MUII_PopUp),
                MUIA_Popasl_Type,     ASL_FontRequest,
              End,
              Child, TxtLabel(CatString(MSG_Label_Fixed, "\33rFixed:"), 0),
              Child, PopaslObject,
                MUIA_Popstring_String,  data->fixedfont = BetterStringObject, StringFrame, End,
                MUIA_Popstring_Button,  MUI_MakeObject(MUIO_PopButton, MUII_PopUp),
                MUIA_Popasl_Type,     ASL_FontRequest,
                ASLFO_Flags,        FOF_FIXEDWIDTHONLY,
              End,
            End,
            Child, RectangleObject,
            End,
          End,

          Child, VGroup,
//            MUIA_VertWeight, 0,
            MUIA_Background, MUII_GroupBack,
            MUIA_Frame, MUIV_Frame_Group,
            MUIA_FrameTitle, CatString(MSG_GroupTitle_Cursor, "Cursor"),
            MUIA_Group_Columns, 2,
            Child, TxtLabel(CatString(MSG_Label_Cursor, "\33rCursor:"), 0),
            Child, data->cursorcolor = PoppenObject,
              MUIA_Window_Title, CatString(MSG_PopWinTitle_Cursor, "Select cursor color"),
            End,
            Child, TxtLabel(CatString(MSG_Label_Selected, "\33Marked:"), 0),
            Child, data->markedcolor = PoppenObject,
              MUIA_Window_Title, CatString(MSG_PopWinTitle_Selected, "Select mark color"),
            End,
            Child, TxtLabel(CatString(MSG_Label_Width, "\33rWidth:"), 0),
            Child, data->cursorwidth = NewObject(widthslider_mcc->mcc_Class, NULL,
              MUIA_Numeric_Min, 1,
              MUIA_Numeric_Max, 6,
              MUIA_Numeric_Format, CatString(MSG_SliderText_StdWidth, "%ld pixels"),
            End,
            Child, TxtLabel(CatString(MSG_Label_BlinkSpeed, "\33rBlinkspeed:"), 0),
            Child, data->blinkspeed = NewObject(speedslider_mcc->mcc_Class, NULL,
              MUIA_Numeric_Min, 0,
              MUIA_Numeric_Max, 20,
              MUIA_Numeric_Format, CatString(MSG_SliderText_StdSpeed, "\33r%ld ms"),
            End,
          End,

        End,

      End,

      Child, VGroup,
        Child, HGroup,
          Child, defaultkeys = SimpleButton(CatString(MSG_Button_DefaultKeys, "Default _keys")),
          Child, TxtLabel(CatString(MSG_Label_BlkQual, "\33rBlock qualifier:"), 1000),
          Child, data->blockqual = MUI_MakeObject(MUIO_Cycle, NULL, data->cycleentries),
        End,
        Child, data->keybindings = ListviewObject,
          MUIA_CycleChain, TRUE,
          MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
          MUIA_Listview_List, keylist = ListObject,
            MUIA_Frame,               MUIV_Frame_InputList,
            MUIA_Background,          MUII_ListBack,
            MUIA_List_Title,          TRUE,
            MUIA_List_ConstructHook,  &ListConstructHook,
            MUIA_List_DestructHook,   &ListDestructHook,
            MUIA_List_DisplayHook,    &ListDisplayHook,
            MUIA_List_DragSortable,   TRUE,
            MUIA_List_Format,         "P=\33r W=100 0, P=\33c 1, 2",
//            MUIA_List_Format, "P=\33r W=100 0, P=\33c NoTB NoBar 1, 2",
//            MUIA_List_SourceArray, keybindings,
//            MUIA_List_TitleClick, TRUE,
//            MUIA_List_DragType, MUIV_List_DragType_Default,
          End,
        End,
        Child, VGroup,
          Child, HGroup,
            Child, data->hotkey = HotkeyStringObject,
              MUIA_CycleChain, TRUE,
              MUIA_Frame, MUIV_Frame_String,
              MUIA_HotkeyString_Snoop, FALSE,
              MUIA_Weight, 500,
            End,
            Child, button = TextObject, ButtonFrame,
              MUIA_CycleChain, TRUE,
              MUIA_Background, MUII_ButtonBack,
              MUIA_Text_Contents, CatString(MSG_Button_Snoop, "Snoop"),
              MUIA_Text_SetMax, TRUE,
              MUIA_InputMode, MUIV_InputMode_Toggle,
            End,

            Child, BalanceObject, End,

            Child, MUI_MakeObject(MUIO_Label, "=", MUIO_Label_Centered),
//            Child, MUI_MakeObject(MUIO_Cycle, NULL, data->functions),
            Child, data->keyfunctions = PopobjectObject,
              MUIA_Weight, 400,
              MUIA_Popstring_String,    functionname = NewObject(data->text_class->mcc_Class, NULL, TextFrame, MUIA_Background, MUII_TextBack, End,
              MUIA_Popstring_Button,    popbutton = MUI_MakeObject(MUIO_PopButton, MUII_PopUp),
              MUIA_Popobject_StrObjHook,  &Popstring_OpenHook,
              MUIA_Popobject_ObjStrHook,  &Popstring_CloseHook,
              MUIA_Popobject_WindowHook,  &Popstring_WindowHook,

              MUIA_Popobject_Object, plist = ListviewObject,
                MUIA_CycleChain, TRUE,
                MUIA_Listview_List, ListObject,
                  InputListFrame,
                  MUIA_List_AutoVisible, TRUE,
                  MUIA_List_SourceArray, data->functions,
                End,
              End,

//              MUIA_Poplist_Array,     data->functions,
            End,

          End,
          Child, HGroup,
            Child, data->insertkey = SimpleButton(CatString(MSG_Button_Insert, "_Insert")),
            Child, data->deletekey = SimpleButton(CatString(MSG_Button_Delete, "_Delete")),
          End,
        End,
      End,

      Child, VGroup,

        Child, HGroup,
          GroupSpacing(0),
          Child, readview = TextEditorObject,
            ReadListFrame,
            MUIA_CycleChain, TRUE,
            MUIA_TextEditor_ReadOnly, TRUE,
            MUIA_TextEditor_Contents, CatString(MSG_HelpTxt_SpellChecker, " \33u Intro \33n\n\nThe TextEditor gadget offers you easy "
            "integration of external spell checkers, this allows you to have both type'n'spell and also normal word-guessing.\n\n \33u"
            " Lookup command \33n\n\nFor the type'n'spell you'll have to set the lookup command to a script that will lookup the word "
            "and set the environment variable \"Found\" to either 0 (for not found) or 1 (for a match).\nI have chosen this behavior "
            "because it is the default for AlphaSpell.\nYou write %s where you want the word.\nIf you use AlphaSpell, then you can "
            "write:\n\33bARexx:\33n \"Options Results;Address ALPHASPELL Search For '%s' Path 'T:' Danish.ald\"\n\nYou \33bmust\33n remember the quotes, this will let ARexx execute it as a command instead of a script.\n\n \33u Suggest command \33n\n\nIf you press HELP over a word, then the editor will launch this command, again with %s substituted with the current word.\nYour script should produce a file in T: named \"Matches\". This file are then read by the gadget, and the contents are shown in a popup-listview, from where you can choose a word which will replace the misspelled one.\nAgain, AlphaSpell users can write:\n\33bARexx:\33n \"Address ALPHASPELL Match '%s' 'T:Matches' Path 'T:' Danish.ald\"\n\nIf you use the methods described above, then you must start AlphaSpell with the \"ARexx\" switch.\n\nIt is advised to keep your dictionary in RAM if you enable the type'n'spell feature."),
          End,
          Child, slider2 = ScrollbarObject,
          End,
        End,

        Child, BalanceObject, End,

        Child, ColGroup(3),
          Child, TxtLabel(CatString(MSG_Label_LookupCmd, "\33rLookup command:"), 0),
          Child, data->LookupExeType = CycleObject,
            MUIA_Cycle_Entries, data->execution,
            MUIA_Weight, 0,
          End,
          Child, data->lookupcmd = BetterStringObject, StringFrame, End,
          Child, TxtLabel(CatString(MSG_Label_SuggestCmd, "\33rSuggest command:"), 0),
          Child, data->SuggestExeType = CycleObject,
            MUIA_Cycle_Entries, data->execution,
            MUIA_Weight, 0,
          End,
          Child, data->suggestcmd = BetterStringObject, StringFrame, MUIA_String_AdvanceOnCR, TRUE, End,
        End,
        Child, ColGroup(2),
          Child, TxtLabel(CatString(MSG_Label_SpellNType, "\33rSpell check as you type?"), 1000),
          Child, data->typenspell = MUI_MakeObject(MUIO_Checkmark, NULL),
          Child, TxtLabel(CatString(MSG_Label_LookupWords, "\33rLookup word before suggesting?"), 1000),
          Child, data->CheckWord = MUI_MakeObject(MUIO_Checkmark, NULL),
          MUIA_Weight, 0,
        End,

        Child, RectangleObject,
          MUIA_Weight, 25,
        End,

      End,

      Child, HGroup,
        GroupSpacing(0),
        Child, editor = TextEditorObject,
          MUIA_CycleChain, TRUE,
          MUIA_ContextMenu, data->editpopup,
          MUIA_TextEditor_Contents,
            "\033r\033b" LIB_DATE "\033n\n"
            "\n\33cTextEditor.mcp " LIB_REV_STRING CPU " (" LIB_DATE ")\n"
            "Copyright (C) 1997-2000 Allan Odgaard\n"
            LIB_COPYRIGHT
            "\033l\n\033[s:9]\n"
            "For the latest version, try: \33u\33p[7]http://www.sf.net/projects/texteditor-mcc/\33p[0]\33n\n"
            "\n"
            "\33hThis gadget is \33ifree software\33n. You can redistribute it and/or modify it under the terms of "
            "the GNU Lesser General Public License as publishd by the Free Software Foundation; either version 2.1 "
            "of the License, or (at your option) any later version.\n"
            "\n"
            "\33[s:2]\33c\33u\33b Usage: \33n\n"
            "\33l\n"
            "Besides the many keys you can configure on the \33iKeybindings\33n page, then there is also the following you should know:\n"
            "You can doubleclick a word to select it, if you hold LMB after a doubleclick, then it will only mark \33bcomplete\33n words. Tripleclicking has the same effect, but for lines.\n"
            "You can extend your block by holding down shift while you press LMB where you want the block to end.\n"
            "While you drag to scroll, the farther away from the gadget your mouse pointer is, the faster the gadget will scroll.\n"
            "\n"
            "\33[s:2]\33c\33u\33b ARexx: \33n\n"
            "\33l\n"
            "The gadget offers the using application to extend its arexx port to also cover this editorgadget. If this is done, then you'll have the following commands available:\n"
            "\n"
            "CLEAR, CUT, COPY, PASTE, ERASE, GOTOLINE (\"/N/A\"), GOTOCOLUMN (\"/N/A\"), CURSOR (\"Up/S,Down/S,Left/S,Right/S\"), LINE (\"/N/A\"), COLUMN (\"/N/A\"), NEXT (\"Word/S,Sentence/S,Paragraph/S,Page/S\"), PREVIOUS (\"Word/S,Sentence/S,Paragraph/S,Page/S\"), POSITION (\"SOF/S,EOF/S,SOL/S,EOL/S,SOW/S,EOW/S,SOV/S,EOV/S\"), SETBOOKMARK (\"/N/A\"), GOTOBOOKMARK (\"/N/A\"), TEXT (\"/F\"), UNDO, REDO, GETLINE, GETCURSOR (\"Line/S,Column/S\"), MARK(\"On/S,Off/S\"), DELETE, BACKSPACE.\n"
            "\n"
            "Please look either in the Amiga StyleGuide or in the TextEditor.mcc documentation for a further explanation.\n"
            "\n\33[s:10]\n\33cThis instance has a contextsenstive menu, from where you can play with textstyles, colors, alignments etc.",
        End,
        Child, slider = ScrollbarObject,
        End,
      End,
    End,
  End;

  if(group)
  {
    set(readview, MUIA_TextEditor_Slider, slider2);
    set(editor, MUIA_TextEditor_Slider, slider);

    set(data->blockqual, MUIA_ShortHelp, CatString(HelpBubble_BlockQual, "Move the cursor key and hold down the qualifer\nthat you set here to mark text."));
    set(data->typenspell, MUIA_ShortHelp, CatString(HelpBubble_TypeNSpell, "Enable this to let the editor\nlookup the words while you type them."));
    set(data->CheckWord, MUIA_ShortHelp, CatString(HelpBubble_CheckWord, "If enabled, then the editor will lookup the word,\nand only show suggestions if the word is misspelled."));

    set(data->hotkey, MUIA_String_AttachedList, data->keybindings);
    set(popbutton, MUIA_CycleChain, TRUE);

    DoMethod(data->lookupcmd, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUIV_Notify_Window, 3, MUIM_Set, MUIA_Window_ActiveObject, data->suggestcmd);

    DoMethod(button, MUIM_Notify, MUIA_Selected, TRUE, MUIV_Notify_Window, 3, MUIM_Set, MUIA_Window_ActiveObject, data->hotkey);
//    DoMethod(button, MUIM_Notify, MUIA_Selected, FALSE, data->hotkey, 3, MUIM_Set, MUIA_String_Acknowledge, TRUE);
    DoMethod(button, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, data->hotkey, 3, MUIM_Set, MUIA_HotkeyString_Snoop, MUIV_TriggerValue);
    DoMethod(data->hotkey, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, button, 3, MUIM_Set, MUIA_Selected, FALSE);

    DoMethod(plist, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, data->keyfunctions, 2, MUIM_Popstring_Close, TRUE);

    DoMethod(defaultkeys, MUIM_Notify,
        MUIA_Pressed, FALSE,
        keylist, 1, MUIM_List_Clear);
    DoMethod(defaultkeys, MUIM_Notify,
        MUIA_Pressed, FALSE,
        keylist, 4, MUIM_List_Insert, keybindings, -1, MUIV_List_Insert_Bottom);

    DoMethod(data->insertkey, MUIM_Notify,
        MUIA_Pressed, FALSE,
        MUIV_Notify_Self, 4, MUIM_CallHook, &InsertHook, data, keylist);

    DoMethod(keylist, MUIM_Notify,
        MUIA_List_Active, MUIV_EveryTime,
        MUIV_Notify_Self, 8, MUIM_MultiSet, MUIA_Disabled, FALSE,
        data->hotkey, button, data->keyfunctions, data->deletekey, NULL);
    DoMethod(keylist, MUIM_Notify,
        MUIA_List_Active, MUIV_List_Active_Off,
        MUIV_Notify_Self, 8, MUIM_MultiSet, MUIA_Disabled, TRUE,
        data->hotkey, button, data->keyfunctions, data->deletekey, NULL);

    DoMethod(keylist, MUIM_Notify,
        MUIA_List_Active, MUIV_EveryTime,
        MUIV_Notify_Self, 4, MUIM_CallHook, &SelectHook, data, keylist);

    DoMethod(functionname, MUIM_Notify,
        MUIA_UserData, MUIV_EveryTime,
        MUIV_Notify_Self, 4, MUIM_CallHook, &UpdateHook, data, keylist);

    DoMethod(data->hotkey, MUIM_Notify,
        MUIA_String_Acknowledge, MUIV_EveryTime,
        MUIV_Notify_Self, 4, MUIM_CallHook, &UpdateHook, data, keylist);

    DoMethod(data->deletekey, MUIM_Notify,
        MUIA_Pressed, FALSE,
        keylist, 2, MUIM_List_Remove, MUIV_List_Remove_Active);


    DoMethod(data->hotkey, MUIM_MultiSet,
        MUIA_Disabled, TRUE,
        data->hotkey, button, data->keyfunctions, data->deletekey, NULL);

    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,1), MUIM_Notify,
        MUIA_Menuitem_Trigger, MUIV_EveryTime,
        editor, 2, MUIM_TextEditor_ARexxCmd, "Cut");
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,2), MUIM_Notify,
        MUIA_Menuitem_Trigger, MUIV_EveryTime,
        editor, 2, MUIM_TextEditor_ARexxCmd, "Copy");
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,3), MUIM_Notify,
        MUIA_Menuitem_Trigger, MUIV_EveryTime,
        editor, 2, MUIM_TextEditor_ARexxCmd, "Paste");
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,4), MUIM_Notify,
        MUIA_Menuitem_Trigger, MUIV_EveryTime,
        editor, 2, MUIM_TextEditor_ARexxCmd, "Erase");
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,5), MUIM_Notify,
        MUIA_Menuitem_Trigger, MUIV_EveryTime,
        editor, 2, MUIM_TextEditor_ARexxCmd, "Undo");
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,6), MUIM_Notify,
        MUIA_Menuitem_Trigger, MUIV_EveryTime,
        editor, 2, MUIM_TextEditor_ARexxCmd, "Redo");

    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,7), MUIM_Notify,
        MUIA_Menuitem_Checked, MUIV_EveryTime,
        editor, 3, MUIM_Set, MUIA_TextEditor_StyleBold, MUIV_TriggerValue);
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,8), MUIM_Notify,
        MUIA_Menuitem_Checked, MUIV_EveryTime,
        editor, 3, MUIM_Set, MUIA_TextEditor_StyleItalic, MUIV_TriggerValue);
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,9), MUIM_Notify,
        MUIA_Menuitem_Checked, MUIV_EveryTime,
        editor, 3, MUIM_Set, MUIA_TextEditor_StyleUnderline, MUIV_TriggerValue);

    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,10), MUIM_Notify,
        MUIA_Menuitem_Checked, TRUE,
        editor, 3, MUIM_Set, MUIA_TextEditor_Flow, MUIV_TextEditor_Flow_Left);
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,11), MUIM_Notify,
        MUIA_Menuitem_Checked, TRUE,
        editor, 3, MUIM_Set, MUIA_TextEditor_Flow, MUIV_TextEditor_Flow_Center);
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,12), MUIM_Notify,
        MUIA_Menuitem_Checked, TRUE,
        editor, 3, MUIM_Set, MUIA_TextEditor_Flow, MUIV_TextEditor_Flow_Right);

    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,13), MUIM_Notify, MUIA_Menuitem_Checked, TRUE, editor, 3, MUIM_Set, MUIA_TextEditor_Pen, 0);
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,14), MUIM_Notify, MUIA_Menuitem_Checked, TRUE, editor, 3, MUIM_Set, MUIA_TextEditor_Pen, 1);
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,15), MUIM_Notify, MUIA_Menuitem_Checked, TRUE, editor, 3, MUIM_Set, MUIA_TextEditor_Pen, 2);
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,16), MUIM_Notify, MUIA_Menuitem_Checked, TRUE, editor, 3, MUIM_Set, MUIA_TextEditor_Pen, 3);
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,17), MUIM_Notify, MUIA_Menuitem_Checked, TRUE, editor, 3, MUIM_Set, MUIA_TextEditor_Pen, 4);
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,18), MUIM_Notify, MUIA_Menuitem_Checked, TRUE, editor, 3, MUIM_Set, MUIA_TextEditor_Pen, 5);
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,19), MUIM_Notify, MUIA_Menuitem_Checked, TRUE, editor, 3, MUIM_Set, MUIA_TextEditor_Pen, 6);
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,20), MUIM_Notify, MUIA_Menuitem_Checked, TRUE, editor, 3, MUIM_Set, MUIA_TextEditor_Pen, 7);
    DoMethod((Object *)DoMethod(data->editpopup,MUIM_FindUData,21), MUIM_Notify, MUIA_Menuitem_Checked, TRUE, editor, 3, MUIM_Set, MUIA_TextEditor_Pen, 8);

    DoMethod(editor, MUIM_Notify,
        MUIA_TextEditor_StyleBold, MUIV_EveryTime,
        (Object *)DoMethod(data->editpopup,MUIM_FindUData,7), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);
    DoMethod(editor, MUIM_Notify,
        MUIA_TextEditor_StyleItalic, MUIV_EveryTime,
        (Object *)DoMethod(data->editpopup,MUIM_FindUData,8), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);
    DoMethod(editor, MUIM_Notify,
        MUIA_TextEditor_StyleUnderline, MUIV_EveryTime,
        (Object *)DoMethod(data->editpopup,MUIM_FindUData,9), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);

    DoMethod(editor, MUIM_Notify,
        MUIA_TextEditor_Flow, MUIV_TextEditor_Flow_Left,
        (Object *)DoMethod(data->editpopup,MUIM_FindUData,10), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);
    DoMethod(editor, MUIM_Notify,
        MUIA_TextEditor_Flow, MUIV_TextEditor_Flow_Center,
        (Object *)DoMethod(data->editpopup,MUIM_FindUData,11), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);
    DoMethod(editor, MUIM_Notify,
        MUIA_TextEditor_Flow, MUIV_TextEditor_Flow_Right,
        (Object *)DoMethod(data->editpopup,MUIM_FindUData,12), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);

    DoMethod(editor, MUIM_Notify, MUIA_TextEditor_Pen, 0, (Object *)DoMethod(data->editpopup,MUIM_FindUData,13), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);
    DoMethod(editor, MUIM_Notify, MUIA_TextEditor_Pen, 1, (Object *)DoMethod(data->editpopup,MUIM_FindUData,14), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);
    DoMethod(editor, MUIM_Notify, MUIA_TextEditor_Pen, 2, (Object *)DoMethod(data->editpopup,MUIM_FindUData,15), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);
    DoMethod(editor, MUIM_Notify, MUIA_TextEditor_Pen, 3, (Object *)DoMethod(data->editpopup,MUIM_FindUData,16), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);
    DoMethod(editor, MUIM_Notify, MUIA_TextEditor_Pen, 4, (Object *)DoMethod(data->editpopup,MUIM_FindUData,17), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);
    DoMethod(editor, MUIM_Notify, MUIA_TextEditor_Pen, 5, (Object *)DoMethod(data->editpopup,MUIM_FindUData,18), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);
    DoMethod(editor, MUIM_Notify, MUIA_TextEditor_Pen, 6, (Object *)DoMethod(data->editpopup,MUIM_FindUData,19), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);
    DoMethod(editor, MUIM_Notify, MUIA_TextEditor_Pen, 7, (Object *)DoMethod(data->editpopup,MUIM_FindUData,20), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);
    DoMethod(editor, MUIM_Notify, MUIA_TextEditor_Pen, 8, (Object *)DoMethod(data->editpopup,MUIM_FindUData,21), 3, MUIM_Set, MUIA_Menuitem_Checked, MUIV_TriggerValue);

    DoMethod(editor, MUIM_Notify, MUIA_TextEditor_AreaMarked, MUIV_EveryTime, MUIV_Notify_Self, 7, MUIM_MultiSet, MUIA_Menuitem_Enabled, MUIV_TriggerValue, (Object *)DoMethod(data->editpopup,MUIM_FindUData,1), (Object *)DoMethod(data->editpopup,MUIM_FindUData,2), (Object *)DoMethod(data->editpopup,MUIM_FindUData,4), NULL);

    DoMethod(editor, MUIM_Notify, MUIA_TextEditor_UndoAvailable, MUIV_EveryTime, (Object *)DoMethod(data->editpopup,MUIM_FindUData, 5), 3, MUIM_Set, MUIA_Menuitem_Enabled, MUIV_TriggerValue);
    DoMethod(editor, MUIM_Notify, MUIA_TextEditor_RedoAvailable, MUIV_EveryTime, (Object *)DoMethod(data->editpopup,MUIM_FindUData, 6), 3, MUIM_Set, MUIA_Menuitem_Enabled, MUIV_TriggerValue);
  }

  return(group);
}
