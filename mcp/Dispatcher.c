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
#include <utility/tagitem.h>
#include <libraries/mui.h>
#include <proto/intuition.h>
#include <proto/utility.h>

#include "Locale.h"
#include "private.h"

#include "SDI_compiler.h"
#include "SDI_hook.h"

// the main mcp dispatcher
DISPATCHERPROTO(_DispatcherP)
{
  switch(msg->MethodID)
  {
    case OM_NEW:                              return(New(cl, obj, (APTR)msg));
    case OM_DISPOSE:                          return(Dispose(cl, obj, (APTR)msg));
    case MUIM_Settingsgroup_ConfigToGadgets:  return(ConfigToGadgets(cl, obj, (APTR)msg));
    case MUIM_Settingsgroup_GadgetsToConfig:  return(GadgetsToConfig(cl, obj, (APTR)msg));
  }

  return(DoSuperMethodA(cl, obj, msg));
}

DISPATCHERPROTO(Text_Dispatcher)
{
  switch(msg->MethodID)
  {
    case OM_SET:
    {
      struct opSet *set_msg = (struct opSet *)msg;
      struct TagItem *tag;

      if((tag = FindTagItem(MUIA_UserData, set_msg->ops_AttrList)))
        set(obj, MUIA_Text_Contents, FunctionName(tag->ti_Data));
    }
    break;
  }
  
  return(DoSuperMethodA(cl, obj, msg));
}

DISPATCHERPROTO(WidthSlider_Dispatcher)
{
  if(msg->MethodID == MUIM_Numeric_Stringify)
  {
    struct MUIP_Numeric_Stringify *smsg = (struct MUIP_Numeric_Stringify *)msg;

    if(smsg->value == 1)
      return((ULONG)CatString(MSG_SliderText_MinWidth, "1 pixel"));

    if(smsg->value == 6)
      return((ULONG)CatString(MSG_SliderText_MaxWidth, "Same as char"));
  }

  return(DoSuperMethodA(cl, obj, msg));
}

DISPATCHERPROTO(SpeedSlider_Dispatcher)
{
  if(msg->MethodID == MUIM_Numeric_Stringify)
  {
    struct MUIP_Numeric_Stringify *smsg = (struct MUIP_Numeric_Stringify *)msg;

    if(smsg->value == 0)
      return((ULONG)CatString(MSG_SliderText_MinSpeed, "Off"));
    else
    {
      static char buf[20];

      sprintf(buf, "%ld ms", smsg->value*25);

      return (ULONG)buf;
    }
  }

  return(DoSuperMethodA(cl, obj, msg));
}
