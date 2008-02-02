/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2008 by TextEditor.mcc Open Source Team

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

#include <proto/exec.h>
#include <proto/intuition.h>

#include "private.h"

unsigned short LineNr(struct line_node *line, struct InstData *data)
{
  unsigned short result = 1;
  struct line_node *actual = data->firstline;

  ENTER();

  while(line != actual)
  {
    result++;
    actual = actual->next;
  }

  RETURN(result);
  return(result);
}

struct line_node *LineNode(unsigned short linenr, struct InstData *data)
{
  struct line_node *actual = data->firstline;

  ENTER();

  while(--linenr && actual->next)
  {
    actual = actual->next;
  }

  RETURN(actual);
  return(actual);
}

long Undo(struct InstData *data)
{
  ENTER();

  if(data->undosize != 0 && data->undobuffer != data->undopointer)
  {
    struct UserAction *buffer;
    BOOL  crsr_move = TRUE;

    if(*(short *)data->undopointer == 0xff)
      set(data->object, MUIA_TextEditor_RedoAvailable, TRUE);

    if(Enabled(data))
    {
      data->blockinfo.enabled = FALSE;
      MarkText(data->blockinfo.startx, data->blockinfo.startline, data->blockinfo.stopx, data->blockinfo.stopline, data);
    }

    data->undopointer -= sizeof(struct UserAction);
    buffer = (struct UserAction *)data->undopointer;

//    if(data->actualline != LineNode(buffer->y, data) || data->CPos_X != buffer->x)
    SetCursor(data->CPos_X, data->actualline, FALSE, data);

    data->CPos_X = buffer->x;
    data->actualline = LineNode(buffer->y, data);
    ScrollIntoDisplay(data);

    switch(buffer->type)
    {
      case ET_PASTECHAR:
      {
        buffer->del.character = *(data->actualline->line.Contents+data->CPos_X);
        buffer->del.style = GetStyle(data->CPos_X, data->actualline);
        buffer->del.flow = data->actualline->line.Flow;
        buffer->del.separator = data->actualline->line.Separator;
        RemoveChars(data->CPos_X, data->actualline, 1, data);
      }
      break;

      case ET_BACKSPACECHAR:
      {
        PasteChars(data->CPos_X++, data->actualline, 1, (char *)&buffer->del.character, buffer, data);
      }
      break;

      case ET_DELETECHAR:
      {
        PasteChars(data->CPos_X, data->actualline, 1, (char *)&buffer->del.character, buffer, data);
      }
      break;

      case ET_SPLITLINE:
      {
        MergeLines(data->actualline, data);
      }
      break;

      case ET_MERGELINES:
      {
        SplitLine(data->CPos_X, data->actualline, FALSE, buffer, data);
      }
      break;

      case ET_BACKSPACEMERGE:
      {
        SplitLine(data->CPos_X, data->actualline, TRUE, buffer, data);
      }
      break;

      case ET_PASTEBLOCK:
      {
        struct marking block =
        {
          TRUE,
          LineNode(buffer->y, data),
          buffer->x,
          LineNode(buffer->blk.y, data),
          buffer->blk.x
        };
        char *clip = GetBlock(&block, data);

        CutBlock2(data, FALSE, FALSE, &block, TRUE);
        buffer->clip = (unsigned char *)clip;
      }
      break;

      case ET_DELETEBLOCK_NOMOVE:
        crsr_move = FALSE;
      // continue

      case ET_DELETEBLOCK:
      {
        struct Hook *oldhook = data->ImportHook;
        char *clip = (char *)buffer->clip;

        data->ImportHook = &ImPlainHook;
        InsertText(data, clip, crsr_move);
        data->ImportHook = oldhook;
        MyFreePooled(data->mypool, clip);

        buffer->blk.x = data->CPos_X;
        buffer->blk.y = LineNr(data->actualline, data);

        if(!crsr_move)
        {
          data->CPos_X = buffer->x;
          data->actualline = LineNode(buffer->y, data);
        }
      }
      break;
    }

    ScrollIntoDisplay(data);

    if(data->flags & FLG_Active)
      SetCursor(data->CPos_X, data->actualline, TRUE, data);

    if(data->undobuffer == data->undopointer)
    {
      set(data->object, MUIA_TextEditor_UndoAvailable, FALSE);

      if(!(data->flags & FLG_UndoLost))
        data->HasChanged = FALSE;
    }

    RETURN(TRUE);
    return(TRUE);
  }
  else
  {
    DoMethod(data->object, MUIM_TextEditor_HandleError, Error_NothingToUndo);

    RETURN(FALSE);
    return(FALSE);
  }
}

long Redo(struct InstData *data)
{
  ENTER();

  if(data->undosize != 0 && *(short *)data->undopointer != 0xff)
  {
    struct UserAction *buffer = (struct UserAction *)data->undopointer;

    if(data->undopointer == data->undobuffer)
      set(data->object, MUIA_TextEditor_UndoAvailable, TRUE);

    if(Enabled(data))
    {
      data->blockinfo.enabled = FALSE;
      MarkText(data->blockinfo.startx, data->blockinfo.startline, data->blockinfo.stopx, data->blockinfo.stopline, data);
    }

    data->undopointer += sizeof(struct UserAction);
//    if(data->actualline != LineNode(buffer->y, data) || data->CPos_X != buffer->x)
    SetCursor(data->CPos_X, data->actualline, FALSE, data);
    data->CPos_X = buffer->x;
    data->actualline = LineNode(buffer->y, data);
    ScrollIntoDisplay(data);

    switch(buffer->type)
    {
      case ET_PASTECHAR:
        PasteChars(data->CPos_X++, data->actualline, 1, (char *)&buffer->del.character, buffer, data);
        break;

      case ET_BACKSPACECHAR:
      case ET_DELETECHAR:
        RemoveChars(data->CPos_X, data->actualline, 1, data);
        break;

      case ET_SPLITLINE:
        SplitLine(data->CPos_X, data->actualline, TRUE, NULL, data);
        break;

      case ET_MERGELINES:
      case ET_BACKSPACEMERGE:
        MergeLines(data->actualline, data);
        break;

      case ET_PASTEBLOCK:
      {
          struct Hook *oldhook = data->ImportHook;

          data->ImportHook = &ImPlainHook;
          InsertText(data, (char *)buffer->clip, TRUE);
          data->ImportHook = oldhook;
          MyFreePooled(data->mypool, buffer->clip);

          buffer->blk.x = data->CPos_X;
          buffer->blk.y = LineNr(data->actualline, data);
      }
      break;

      case ET_DELETEBLOCK_NOMOVE:
      case ET_DELETEBLOCK:
      {
            struct marking block =
            {
              TRUE,
              LineNode(buffer->y, data),
              buffer->x,
              LineNode(buffer->blk.y, data),
              buffer->blk.x
            };
            char  *clip = GetBlock(&block, data);

          CutBlock2(data, FALSE, FALSE, &block, TRUE);
          buffer->clip = (unsigned char *)clip;
      }
      break;
    }

    ScrollIntoDisplay(data);

    if(data->flags & FLG_Active)
      SetCursor(data->CPos_X, data->actualline, TRUE, data);

    if(*(short *)data->undopointer == 0xff)
      set(data->object, MUIA_TextEditor_RedoAvailable, FALSE);

    RETURN(TRUE);
    return(TRUE);
  }
  else
  {
    DoMethod(data->object, MUIM_TextEditor_HandleError, Error_NothingToRedo);

    RETURN(FALSE);
    return(FALSE);
  }
}

void ResetUndoBuffer(struct InstData *data)
{
  ENTER();

  if(data->undosize != 0)
  {
    struct UserAction *buffer = (struct UserAction *)data->undobuffer;

    while(buffer != data->undopointer)
    {
      if(buffer->type == ET_DELETEBLOCK)
        MyFreePooled(data->mypool, buffer->clip);

      buffer++;
    }

    while(buffer->type != 0xff)
    {
      if(buffer->type == ET_PASTEBLOCK)
        MyFreePooled(data->mypool, buffer->clip);

      buffer++;
    }

    data->undopointer = data->undobuffer;
    *(short *)data->undopointer = 0xff;
  }

  LEAVE();
}

long AddToUndoBuffer(enum EventType eventtype, char *eventdata, struct InstData *data)
{
  ENTER();

  if(data->undosize != 0)
  {
    struct UserAction *buffer;

    if((char *)data->undobuffer+data->undosize <= (char *)data->undopointer+sizeof(struct UserAction)+1)
    {
      LONG    c;
      struct  UserAction *t_buffer;
      char    *t_undobuffer = (char *)data->undobuffer;

      for(c = 1; c <= 10; c++)
      {
        t_buffer = (struct UserAction *)t_undobuffer;
        if(t_buffer->type == ET_DELETEBLOCK)
          MyFreePooled(data->mypool, t_buffer->clip);

        t_undobuffer += sizeof(struct UserAction);
      }

      memcpy(data->undobuffer, t_undobuffer, data->undosize-(t_undobuffer-(char *)data->undobuffer));

      data->undopointer -= t_undobuffer-(char *)data->undobuffer;

      data->flags |= FLG_UndoLost;
    }
    buffer = data->undopointer;
    if(*(short *)data->undopointer != 0xff)
      set(data->object, MUIA_TextEditor_RedoAvailable, FALSE);
    if(data->undopointer == data->undobuffer)
      set(data->object, MUIA_TextEditor_UndoAvailable, TRUE);

    data->undopointer += sizeof(struct UserAction);

    *(short *)data->undopointer = 0xff;
    buffer->x  = data->CPos_X;
    buffer->y  = LineNr(data->actualline, data);

    buffer->type = eventtype;
    switch(eventtype)
    {
      case ET_BACKSPACEMERGE:
      case ET_MERGELINES:
      {
        buffer->del.style = data->actualline->next->line.Color;
        buffer->del.flow = data->actualline->next->line.Flow;
        buffer->del.separator = data->actualline->next->line.Separator;
      }
      break;

      case ET_DELETECHAR:
      case ET_BACKSPACECHAR:
      {
        buffer->del.character = *eventdata;
        buffer->del.style = GetStyle(data->CPos_X, data->actualline);
        buffer->del.flow = data->actualline->line.Flow;
        buffer->del.separator = data->actualline->line.Separator;
      }
      break;

      case ET_PASTEBLOCK:
      {
        struct marking *block = (struct marking *)eventdata;

        buffer->x  = block->startx;
        buffer->y  = LineNr(block->startline, data);
        buffer->blk.x = block->stopx;
        buffer->blk.y = LineNr(block->stopline, data);
      }
      break;

      case ET_DELETEBLOCK:
      {
        char *text;
        struct marking *block = (struct marking *)eventdata;

        if((text = GetBlock((struct marking *)eventdata, data)))
        {
          buffer->x = block->startx;
          buffer->y = LineNr(block->startline, data);
          buffer->clip = (unsigned char *)text;

          if(data->flags & FLG_FreezeCrsr)
            buffer->type = ET_DELETEBLOCK_NOMOVE;
        }
        else
        {
          ResetUndoBuffer(data);
          DoMethod(data->object, MUIM_TextEditor_HandleError, Error_NotEnoughUndoMem);
        }
      }
      break;
    }
  }

  RETURN(TRUE);
  return(TRUE);
}

void ResizeUndoBuffer(struct InstData *data, ULONG level)
{
  ULONG newSize;

  ENTER();

  // calculate number of bytes from number of undo levels
  newSize = (level * sizeof(struct UserAction)) + 1;

  if(data->undosize != newSize)
  {
    if(level == 0)
    {
      // if zero steps are requested then free everything
      if(data->undobuffer != NULL)
      {
        ResetUndoBuffer(data);
        MyFreePooled(data->mypool, data->undobuffer);
        data->undobuffer = NULL;
        data->undopointer = NULL;
      }
      data->undosize = 0;
    }
    else
    {
      // allocate a new undo buffer
      if((data->undobuffer = MyAllocPooled(data->mypool, newSize)) != NULL)
      {
        data->undopointer = data->undobuffer;
        *(short *)data->undopointer = 0xff;
        data->undosize = newSize;
      }
      else
      {
        data->undosize = 0;
      }
    }
  }

  LEAVE();
}
