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

/************************************************************************
 The plain import hook. It supports following escape sequences:

  <ESC> + u      Set the soft style to underline.
  <ESC> + b      Set the soft style to bold.
  <ESC> + i      Set the soft style to italic.
  <ESC> + n      Set the soft style back to normal.
  <ESC> + h      Highlight the current line.
  <ESC> + p[x]   Change to color x, where x is taken from the colormap.
                 0 means normal. The color is reset for each new line.


 The following sequences are only valid at the beginning of a line.

  <ESC> + l      Left justify current and following lines.
  <ESC> + r      Right justify current and following lines.
  <ESC> + c      Center current and following lines.
  <ESC> + [s:x]  Create a separator. x is a bit combination of flags:
                 Placement (mutually exclusive):
                     1 = Top
                     2 = Middle
                     4 = Bottom
                 Cosmetical:
                     8 = StrikeThru   - Draw separator ontop of text.
                     16 = Thick        - Make separator extra thick.

*************************************************************************/
HOOKPROTONO(PlainImportHookFunc, STRPTR, struct ImportMessage *msg)
{
	char *eol;
	char *ret;
	char *src = msg->Data;
	int len;
	struct LineNode *line = msg->linenode;

	/* Find the end of the line */
	eol = strchr(src,'\n');
	if (!eol)
	{
		int len = strlen(src);
		if (!len) return NULL;
		eol = src + len;
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
		unsigned char *dest = (unsigned char*)line->Contents;
		int current_style = 0;
		int new_style = 0;
		int cur_styles = 0;
		int max_styles = 0;

		/* Copy loop */
		while (src < eol)
		{
			unsigned char c = *src++;
			if (c == '\033')
			{
				switch (*src++)
				{
					case	'b': new_style |= BOLD; break;
					case	'i': new_style |= ITALIC; break;
					case	'u': new_style |= UNDERLINE; break;
					case	'h': new_style |= COLOURED; break;
					case	'n': new_style = ~new_style; break;
					case	'l': line->Flow = 0; break;
					case	'c': line->Flow = 1; break;
					case	'r': line->Flow = 2; break;
					case	'p':
								if (*src == '[')
								{
									LONG pen;
									LONG chars;

									src++;
									chars = StrToLong(src,&pen) ;
									if (chars != -1)
									{
										src += chars;
										if (*src == ']') src++;
									}
								}
								break;
					case	'[':
								if (*src == 's')
								{
									if (*(++src) == ':')
									{
										LONG flags;
										LONG chars;

										src++;

										chars = StrToLong(src,&flags) ;
										if (chars != -1)
										{
											src += chars;
											if (*src == ']')
											{
												src++;
												line->Separator = flags;
											}
										}
									}
								}
								break;
				}
				continue;
			}

			*dest++ = c;

			if (new_style != current_style)
			{
				if (cur_styles >= max_styles)
				{
					UWORD *new_styles;
					if ((new_styles = MyAllocPooled(msg->PoolHandle, sizeof(line->Styles[0])*2*(max_styles+9)))) /* we reserve one for the ending */
					{
						if (line->Styles)
						{
							memcpy(new_styles,line->Styles,sizeof(line->Styles[0])*2*max_styles);
							MyFreePooled(msg->PoolHandle,line->Styles);
						}
						line->Styles = new_styles;
						max_styles += 8;
					}
				}

				if (line->Styles)
				{
					line->Styles[cur_styles*2] = dest - (unsigned char*)line->Contents;
					line->Styles[cur_styles*2+1] = new_style;
					cur_styles++;
				}

				current_style = new_style;
			}
		}

		/* Mark the end of the style array */
		if (line->Styles)
		{
			line->Styles[cur_styles*2] = ~0;
			line->Styles[cur_styles*2+1] = 0;
		}

		*dest++ = '\n';
		*dest = 0;
		line->Length = dest - line->Contents; /* excluding \n */
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
