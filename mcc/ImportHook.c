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
#include <proto/dos.h>
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



/*************************************************************************/

STATIC LONG GetHex(char *src)
{
	if ((src[0] >= '0' && src[0] <= '9')) return src[0] - '0';
	if ((src[0] >= 'a' && src[0] <= 'f')) return src[0] - 'a' + 10;
	if ((src[0] >= 'A' && src[0] <= 'F')) return src[0] - 'A' + 10;
	return -1;
}

/************************************************************************
 
*************************************************************************/
STATIC BOOL GetQP(char **src_ptr, unsigned char *val)
{
	unsigned char v;
	char *src = *src_ptr;
	int rc;

	rc = GetHex(src);
	if (rc != -1)
	{
		v = rc << 4;

		rc = GetHex(&src[1]);
		if (rc != -1)
		{
			v |= rc;
			*val = v;
			*src_ptr = src + 2;
			return TRUE;
		}
	}
	return FALSE;
}

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
 Returns the last character in the current line
*************************************************************************/
STATIC char *FindEOL(char *src)
{
	/* Find the end of the line */
	char *eol = strchr(src,'\n');
	if (!eol)
	{
		int len = strlen(src);
		if (!len) return NULL;
		eol = src + len - 1;
	}

	if (eol > src && eol[-1] == '\r')
		eol--;

	return eol;
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
HOOKPROTONHNO(PlainImportHookFunc, STRPTR, struct ImportMessage *msg)
{
	char *eol;
	char *src = msg->Data;
	int len;
	struct LineNode *line = msg->linenode;

	if (!(eol = FindEOL(src)))
		return NULL;

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
  return eol+1;
}
MakeHook(ImPlainHook, PlainImportHookFunc);

/************************************************************************
 The MIME import hook. It supports following escape sequences:
*************************************************************************/
STATIC STRPTR MimeImport(struct ImportMessage *msg, LONG type)
{
	char *eol;
	char *src = msg->Data;
	int len;
	struct LineNode *line = msg->linenode;
	ULONG wrap = msg->ImportWrap;

	if (!(eol = FindEOL(src)))
		return NULL;

	/* Clear the flow */
	line->Flow = 0;

	len = eol - src;

 /* allocate some more memory for the possible quote mark '>', note that if
  * a '=' is detected at the end of a line this memory is not sufficient! */
	if ((line->Contents = MyAllocPooled(msg->PoolHandle,len+4)))
	{
		unsigned char *dest_start = (unsigned char*)line->Contents;
		unsigned char *dest = dest_start;
		unsigned char *src_word_start = src;
		unsigned char *dest_word_start = dest_start;

		/* Style and color state */
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

		if (type == 2)
		{
			*dest++ = '>';
			line->Color = TRUE;
		}

		/* Copy loop */
		while (src < eol)
		{
			unsigned char c = *src++;

			if (c == '/')
			{
				AddToGrow(&style_grow, dest - dest_start + 1, (state & ITALIC)?~ITALIC:ITALIC);
				state ^= ITALIC;
				continue;
			} else
			if (c == '*')
			{
				AddToGrow(&style_grow, dest - dest_start + 1, (state & BOLD)?~BOLD:BOLD);
				state ^= BOLD;
				continue;
			} else
			if (c == '_')
			{
				AddToGrow(&style_grow, dest - dest_start + 1, (state & UNDERLINE)?~UNDERLINE:UNDERLINE);
				state ^= UNDERLINE;
				continue;
			} else
			if (c == '#')
			{
				AddToGrow(&color_grow, dest - dest_start + 1, (state & COLOURED)?0:7);
				state ^= COLOURED;
				continue;
			} else
			if (c == '=' && type > 0)
			{
				if (!GetQP(&src,&c))
				{
					int i;
					
					i = 0;
					if (src[0] == '\r') i++;

					if (src[i] == '\n')
					{
						src += i + 1;

						if (!(eol = FindEOL(src)))
							break;
						continue;
					}
				}
			}

			if (c == ' ')
			{
				/* src is already advanced */
				src_word_start = src;
				dest_word_start = dest;
			}

			if (wrap && ((ULONG)(dest - dest_start)) >= wrap)
			{
				/* Only leave the loop, if we really have added some characters to the line */
				if (dest_word_start != dest_start)
				{
					/* src points to the real word start, but we add one when we return eol */
					eol = src_word_start - 1;
					dest = dest_word_start;
					break;
				}
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

  return eol?eol + 1:NULL;
}

HOOKPROTONHNO(EMailImportHookFunc, STRPTR , struct ImportMessage *msg)
{
	return MimeImport(msg,0);
}
MakeHook(ImEMailHook, EMailImportHookFunc);

HOOKPROTONHNO(MIMEImportHookFunc, STRPTR, struct ImportMessage *msg)
{
	return MimeImport(msg,1);
}
MakeHook(ImMIMEHook, MIMEImportHookFunc);

HOOKPROTONHNO(MIMEQuoteImportHookFunc, STRPTR, struct ImportMessage *msg)
{
	return MimeImport(msg,2);
}
MakeHook(ImMIMEQuoteHook, MIMEQuoteImportHookFunc);
