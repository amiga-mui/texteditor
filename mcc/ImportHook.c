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
#include <exec/types.h>
#include <clib/dos_protos.h>
#include <proto/exec.h>

#include "TextEditor_mcc.h"
#include "private.h"

HOOKPROTONO(PlainImportHookFunc, STRPTR, struct ImportMessage *msg)
{
	char *eol;
	char *ret;
	char *src = msg->Data;
	int len;
	struct LineNode *line = msg->linenode;

	eol = strchr(src,'\n');

	if (!eol)
	{
		int len = strlen(src);
		if (!len) return NULL;
		eol = msg->Data + len;
		ret = NULL;
	} else
	{
		ret = eol + 1;
		if (eol != (char*)msg->Data && eol[-1] == '\r')
			eol--;
	}

	len = eol - src;
	if ((line->Contents = MyAllocPooled(msg->PoolHandle,len+2)))
	{
		memcpy(line->Contents, src, len);
		line->Contents[len] = '\n';
		line->Contents[len+1] = 0;
		line->Length = len+1;
	}
  return ret;
}
MakeHook(ImPlainHook, PlainImportHookFunc);

HOOKPROTONO(EMailImportHookFunc, STRPTR , struct ImportMessage *msg)
{
  #warning "ImportHook not finished yet!!";
  return PlainImportHookFunc(hook, NULL, msg);
}
MakeHook(ImEMailHook, EMailImportHookFunc);

HOOKPROTONO(MIMEImportHookFunc, STRPTR, struct ImportMessage *msg)
{
  #warning "ImportHook not finished yet!!";
  return PlainImportHookFunc(hook, NULL, msg);
}
MakeHook(ImMIMEHook, MIMEImportHookFunc);

HOOKPROTONO(MIMEQuoteImportHookFunc, STRPTR, struct ImportMessage *msg)
{
  #warning "ImportHook not finished yet!!";
  return PlainImportHookFunc(hook, NULL, msg);
}
MakeHook(ImMIMEQuoteHook, MIMEQuoteImportHookFunc);
