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

/*************************************************************************/

struct grow
{
	UWORD *array;
	
	int current;
	int max;

	APTR pool;
};

/*************************************************************************/

/************************************************************************
 Reads out the next value at *src_ptr and advances src_ptr.
 Returns TRUE if succedded else FALSE
*************************************************************************/
STATIC BOOL GetLong(char **src_ptr, LONG *val)
{
	LONG chars = StrToLong(*src_ptr,val);
	if (chars != -1)
	{
		*src_ptr += chars;
		return TRUE;
	}
	return FALSE;
}

/************************************************************************
 Adds two new values to the given grow. This function guarantees
 that there is at least space for 2 additional values.
*************************************************************************/
STATIC VOID AddToGrow(struct grow *grow, UWORD val1, UWORD val2)
{
	if (grow->current >= grow->max)
	{
		UWORD *new_array;

		if ((new_array = MyAllocPooled(grow->pool, sizeof(grow->array[0])*2*(grow->max+9)))) /* we reserve one more for the ending */
		{
			/* Copy old contents into new array */
			if (grow->array)
			{
				memcpy(new_array,grow->array,sizeof(grow->array[0])*2*grow->current);
				MyFreePooled(grow->pool,grow->array);
			}
			grow->array = new_array;
			grow->max += 8;
		}
	}

	if (grow->current < grow->max)
	{
		grow->array[grow->current*2] = val1;
		grow->array[grow->current*2+1] = val2;
		grow->current++;
	}
}

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
		struct grow style_grow;

		int current_color = 0;
		int new_color = 0;
		struct grow color_grow;

		memset(&color_grow,0,sizeof(color_grow));
		memset(&style_grow,0,sizeof(style_grow));

		color_grow.pool = style_grow.pool = msg->PoolHandle;

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
					case	'h': line->Color = TRUE; break;
					case	'n': new_style = ~new_style; break;
					case	'l': line->Flow = 0; break;
					case	'c': line->Flow = 1; break;
					case	'r': line->Flow = 2; break;
					case	'p':
								if (*src == '[')
								{
									LONG pen;
									src++;
									if (GetLong(&src,&pen))
									{
										if (*src == ']')
										{
											new_color = pen;
											src++;
										}
									}
								}
								break;
					case	'[':
								if (*src == 's')
								{
									if (*(++src) == ':')
									{
										LONG flags;
										src++;
										if (GetLong(&src,&flags))
										{
											if (*src == ']')
											{
												line->Separator = flags;
												src++;
											}
										}
									}
								}
								break;
				}
				continue;
			}

			*dest++ = c;

			/* Handle color changes */
			if (new_color != current_color)
			{
				AddToGrow(&color_grow, dest - (unsigned char*)line->Contents, new_color);
				current_color = new_color;
			}

			/* Handle style changes */
			if (new_style != current_style)
			{
				AddToGrow(&style_grow, dest - (unsigned char*)line->Contents, new_style);
				current_style = new_style;
			}
		} /* while (src < eol) */

		line->Colors = color_grow.array;
		line->Styles = style_grow.array;

		/* Mark the end of the color array (space is ensured) */
		if (line->Colors)
		{
			line->Colors[color_grow.current*2] = ~0;
			line->Colors[color_grow.current*2+1] = 0;
		}

		/* Mark the end of the style array (space is ensured) */
		if (line->Styles)
		{
			line->Styles[style_grow.current*2] = ~0;
			line->Styles[style_grow.current*2+1] = 0;
		}

		*dest++ = '\n';
		*dest = 0;

		line->Length = dest - line->Contents; /* this excludes \n */
	}
  return ret;
}
MakeHook(ImPlainHook, PlainImportHookFunc);

HOOKPROTONO(EMailImportHookFunc, STRPTR , struct ImportMessage *msg)
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

	/* Clear the flow */
	line->Flow = 0;

	len = eol - src;
	if ((line->Contents = MyAllocPooled(msg->PoolHandle,len+2)))
	{
		unsigned char *dest = (unsigned char*)line->Contents;

		int state = 0;

		struct grow style_grow;
		struct grow color_grow;

		memset(&color_grow,0,sizeof(color_grow));
		memset(&style_grow,0,sizeof(style_grow));

		color_grow.pool = style_grow.pool = msg->PoolHandle;

		if (src[0] == '>') line->Color = TRUE;
		else if (src[0] == '<')
		{
			if (src[1] == 's' && src[2] == 'b' && src[3] == '>')
			{
				line->Separator = 2;	
				src+= 4;
				line->Flow = 1;
			}
			else if (src[1] == 't' && src[2] == 's' && src[3] == 'b' && src[4] == '>')
			{
				src += 5;
				line->Separator = 18;
				line->Flow = 1;
			}
		}

		/* Copy loop */
		while (src < eol)
		{
			unsigned char c = *src++;

			if (c == '/')
			{
				AddToGrow(&style_grow, dest - line->Contents + 1, (state & ITALIC)?~ITALIC:ITALIC);
				state ^= ITALIC;
				continue;
			} else
			if (c == '*')
			{
				AddToGrow(&style_grow, dest - line->Contents + 1, (state & BOLD)?~BOLD:BOLD);
				state ^= BOLD;
				continue;
			} else
			if (c == '_')
			{
				AddToGrow(&style_grow, dest - line->Contents + 1, (state & UNDERLINE)?~UNDERLINE:UNDERLINE);
				state ^= UNDERLINE;
				continue;
			} else
			if (c == '#')
			{
				AddToGrow(&color_grow, dest - line->Contents + 1, (state & COLOURED)?0:7);
				state ^= COLOURED;
				continue;
			}

			*dest++ = c;
		} /* while (src < eol) */

		line->Colors = color_grow.array;
		line->Styles = style_grow.array;

		/* Mark the end of the color array (space is ensured) */
		if (line->Colors)
		{
			line->Colors[color_grow.current*2] = ~0;
			line->Colors[color_grow.current*2+1] = 0;
		}

		/* Mark the end of the style array (space is ensured) */
		if (line->Styles)
		{
			line->Styles[style_grow.current*2] = ~0;
			line->Styles[style_grow.current*2+1] = 0;
		}

		*dest++ = '\n';
		*dest = 0;

		line->Length = dest - line->Contents; /* this excludes \n */
	}
  return ret;
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
