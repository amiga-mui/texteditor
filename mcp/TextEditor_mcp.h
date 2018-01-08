/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2018 TextEditor.mcc Open Source Team

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

#ifndef MUI_TEXTEDITOR_MCP_H
#define MUI_TEXTEDITOR_MCP_H

#ifndef LIBRARIES_MUI_H
#include <libraries/mui.h>
#endif

#include <devices/inputevent.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__AROS__) && defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack(2)
  #elif defined(__VBCC__)
    #pragma amiga-align
  #endif
#endif

#define MUIC_TextEditor_mcp "TextEditor.mcp"
#define TextEditorMcpObject MUI_NewObject(MUIC_TextEditor_mcp

#define MUICFG_TextEditor_Background       0xad000051UL
#define MUICFG_TextEditor_BlinkSpeed       0xad000052UL
#define MUICFG_TextEditor_BlockQual        0xad000053UL
#define MUICFG_TextEditor_CheckWord        0xad000050UL
#define MUICFG_TextEditor_CursorColor      0xad000054UL
#define MUICFG_TextEditor_CursorTextColor  0xad000055UL /* UNUSED */
#define MUICFG_TextEditor_CursorWidth      0xad000056UL
#define MUICFG_TextEditor_FixedFont        0xad000057UL
#define MUICFG_TextEditor_Frame            0xad000058UL
#define MUICFG_TextEditor_HighlightColor   0xad000059UL
#define MUICFG_TextEditor_MarkedColor      0xad00005aUL
#define MUICFG_TextEditor_NormalFont       0xad00005bUL
#define MUICFG_TextEditor_SetMaxPen        0xad00005cUL /* OBSOLETE */
#define MUICFG_TextEditor_Smooth           0xad00005dUL
#define MUICFG_TextEditor_TabSize          0xad00005eUL
#define MUICFG_TextEditor_TextColor        0xad00005fUL
#define MUICFG_TextEditor_UndoSize         0xad000060UL
#define MUICFG_TextEditor_TypeNSpell       0xad000061UL
#define MUICFG_TextEditor_LookupCmd        0xad000062UL /* OBSOLETE */
#define MUICFG_TextEditor_SuggestCmd       0xad000063UL /* OBSOLETE */
#define MUICFG_TextEditor_Keybindings      0xad000064UL
#define MUICFG_TextEditor_SuggestKey       0xad000065UL /* OBSOLETE */
#define MUICFG_TextEditor_SeparatorShine   0xad000066UL
#define MUICFG_TextEditor_SeparatorShadow  0xad000067UL
#define MUICFG_TextEditor_ConfigVersion    0xad000068UL
#define MUICFG_TextEditor_InactiveCursor   0xad000069UL
#define MUICFG_TextEditor_SelectPointer    0xad00006aUL
#define MUICFG_TextEditor_InactiveColor    0xad00006bUL
#define MUICFG_TextEditor_LookupExeType    0xad00006cUL
#define MUICFG_TextEditor_LookupCommand    0xad00006dUL
#define MUICFG_TextEditor_SuggestExeType   0xad00006eUL
#define MUICFG_TextEditor_SuggestCommand   0xad00006fUL

#define CFG_TextEditor_Background_Def       "2:m2"
#define CFG_TextEditor_BlinkSpeed_Def       0
#define CFG_TextEditor_BlockQual_Def        0
#define CFG_TextEditor_CheckWord_Def        FALSE
#define CFG_TextEditor_CursorColor_Def      "m0"
#define CFG_TextEditor_CursorWidth_Def      6
#define CFG_TextEditor_FixedFont_Def        ""
#define CFG_TextEditor_Frame_Def            "302222"
#define CFG_TextEditor_HighlightColor_Def   "m0"
#define CFG_TextEditor_MarkedColor_Def      "m6"
#define CFG_TextEditor_NormalFont_Def       ""
#define CFG_TextEditor_Smooth_Def           TRUE
#define CFG_TextEditor_TabSize_Def          4
#define CFG_TextEditor_TextColor_Def        "m5"
#define CFG_TextEditor_UndoSize_Def         500
#define CFG_TextEditor_TypeNSpell_Def       FALSE
#define CFG_TextEditor_SeparatorShine_Def   "m1"
#define CFG_TextEditor_SeparatorShadow_Def  "m3"
#define CFG_TextEditor_InactiveCursor_Def   TRUE
#define CFG_TextEditor_SelectPointer_Def    TRUE
#define CFG_TextEditor_InactiveColor_Def    "m3"
#define CFG_TextEditor_LookupExeType_Def    0
#define CFG_TextEditor_LookupCommand_Def    ""
#define CFG_TextEditor_SuggestExeType_Def   1
#define CFG_TextEditor_SuggestCommand_Def   "\"Open('f', 'T:Matches', 'W');WriteLn('f', '%s');Close('f')"

#if !defined(__AROS__) && defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack(2)
  #elif defined(__VBCC__)
    #pragma amiga-align
  #endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* MUI_TEXTEDITOR_MCP_H */
