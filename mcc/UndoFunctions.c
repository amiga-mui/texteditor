/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2009 by TextEditor.mcc Open Source Team

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

/// FreeUndoStep()
// free the memory occupated an undo step
static void FreeUndoStep(struct InstData *data, struct UserAction *step)
{
  ENTER();

  if(step->type == ET_DELETEBLOCK || step->type == ET_PASTEBLOCK)
  {
    MyFreePooled(data->mypool, step->clip);
  }

  LEAVE();
}

///
/// Undo()
BOOL Undo(struct InstData *data)
{
  BOOL success = FALSE;

  ENTER();

  D(DBF_UNDO, "before maxUndoSteps=%ld nextUndoStep=%ld usedUndoSteps=%ld", data->maxUndoSteps, data->nextUndoStep, data->usedUndoSteps);

  // check if there is something in the undo buffer available
  if(data->nextUndoStep > 0)
  {
    struct UserAction *action;
    BOOL moveCursor = TRUE;

    if(Enabled(data))
    {
      data->blockinfo.enabled = FALSE;
      MarkText(data->blockinfo.startx, data->blockinfo.startline, data->blockinfo.stopx, data->blockinfo.stopline, data);
    }

    // as the redo operation automatically
    // becomes available when undo is used we just
    // check here if we didn't yet set RedoAvailable
    // as we only want to set it once
    if(data->nextUndoStep == data->usedUndoSteps)
      set(data->object, MUIA_TextEditor_RedoAvailable, TRUE);

    // go one step back
    data->nextUndoStep--;
    action = &data->undoSteps[data->nextUndoStep];

//    if(data->actualline != LineNode(buffer->y, data) || data->CPos_X != buffer->x)
    SetCursor(data->CPos_X, data->actualline, FALSE, data);

    data->CPos_X = action->x;
    data->actualline = LineNode(action->y, data);
    ScrollIntoDisplay(data);

    // perform the saved undo action
    switch(action->type)
    {
      case ET_PASTECHAR:
      {
        D(DBF_UNDO, "undo PASTECHAR");
        action->del.character = *(data->actualline->line.Contents+data->CPos_X);
        action->del.style = GetStyle(data->CPos_X, data->actualline);
        action->del.flow = data->actualline->line.Flow;
        action->del.separator = data->actualline->line.Separator;
        #warning is buffer->del.highlight missing here?
        RemoveChars(data->CPos_X, data->actualline, 1, data);
      }
      break;

      case ET_BACKSPACECHAR:
      {
        D(DBF_UNDO, "undo BACKSPACECHAR");
        PasteChars(data->CPos_X, data->actualline, 1, (char *)&action->del.character, action, data);
        data->CPos_X++;
      }
      break;

      case ET_DELETECHAR:
      {
        D(DBF_UNDO, "undo DELETECHAR");
        PasteChars(data->CPos_X, data->actualline, 1, (char *)&action->del.character, action, data);
      }
      break;

      case ET_SPLITLINE:
      {
        D(DBF_UNDO, "undo SPLITLINE");
        MergeLines(data->actualline, data);
      }
      break;

      case ET_MERGELINES:
      {
        D(DBF_UNDO, "undo MERGELINES");
        SplitLine(data->CPos_X, data->actualline, FALSE, action, data);
      }
      break;

      case ET_BACKSPACEMERGE:
      {
        D(DBF_UNDO, "undo BACKSPACEMARGE");
        SplitLine(data->CPos_X, data->actualline, TRUE, action, data);
      }
      break;

      case ET_PASTEBLOCK:
      {
        struct marking block =
        {
          TRUE,
          LineNode(action->y, data),
          action->x,
          LineNode(action->blk.y, data),
          action->blk.x
        };
        char *clip = GetBlock(&block, data);

        D(DBF_UNDO, "undo PASTEBLOCK");
        CutBlock2(data, FALSE, FALSE, &block, TRUE);
        action->clip = (unsigned char *)clip;
      }
      break;

      case ET_DELETEBLOCK_NOMOVE:
      {
        D(DBF_UNDO, "undo DELETEBLOCK_NOMOVE");
        moveCursor = FALSE;
      }
      // fall through...

      case ET_DELETEBLOCK:
      {
        struct Hook *oldhook = data->ImportHook;
        char *clip = (char *)action->clip;

        D(DBF_UNDO, "undo DELETEBLOCK");
        data->ImportHook = &ImPlainHook;
        InsertText(data, clip, moveCursor);
        data->ImportHook = oldhook;
        MyFreePooled(data->mypool, clip);

        action->blk.x = data->CPos_X;
        action->blk.y = LineNr(data->actualline, data);

        if(moveCursor == FALSE)
        {
          data->CPos_X = action->x;
          data->actualline = LineNode(action->y, data);
        }
      }
      break;

      default:
        // nothing to do
      break;
    }

    ScrollIntoDisplay(data);

    if(isFlagSet(data->flags, FLG_Active))
      SetCursor(data->CPos_X, data->actualline, TRUE, data);

    // if there are no further undo levels we
    // have to set UndoAvailable to FALSE
    if(data->nextUndoStep == 0)
    {
      set(data->object, MUIA_TextEditor_UndoAvailable, FALSE);

      if(isFlagClear(data->flags, FLG_UndoLost))
        data->HasChanged = FALSE;
    }

    success = TRUE;
  }
  else
  {
    DoMethod(data->object, MUIM_TextEditor_HandleError, Error_NothingToUndo);
  }

  D(DBF_UNDO, "after  maxUndoSteps=%ld nextUndoStep=%ld usedUndoSteps=%ld", data->maxUndoSteps, data->nextUndoStep, data->usedUndoSteps);

  RETURN(success);
  return success;
}
///
/// Redo()
BOOL Redo(struct InstData *data)
{
  BOOL success = FALSE;

  ENTER();

  D(DBF_UNDO, "before maxUndoSteps=%ld nextUndoStep=%ld usedUndoSteps=%ld", data->maxUndoSteps, data->nextUndoStep, data->usedUndoSteps);

  // check if there something to redo at all
  if(data->nextUndoStep < data->usedUndoSteps)
  {
    struct UserAction *action;

    if(Enabled(data))
    {
      data->blockinfo.enabled = FALSE;
      MarkText(data->blockinfo.startx, data->blockinfo.startline, data->blockinfo.stopx, data->blockinfo.stopline, data);
    }

    // in case nextUndoStep is equal zero then we have to
    // set the undoavailable attribute to true to signal
    // others that undo is available
    if(data->nextUndoStep == 0)
      set(data->object, MUIA_TextEditor_UndoAvailable, TRUE);

    action = &data->undoSteps[data->nextUndoStep];
    data->nextUndoStep++;

//    if(data->actualline != LineNode(buffer->y, data) || data->CPos_X != buffer->x)
    SetCursor(data->CPos_X, data->actualline, FALSE, data);
    data->CPos_X = action->x;
    data->actualline = LineNode(action->y, data);
    ScrollIntoDisplay(data);

    switch(action->type)
    {
      case ET_PASTECHAR:
      {
        D(DBF_UNDO, "redo PASTECHAR");
        PasteChars(data->CPos_X, data->actualline, 1, (char *)&action->del.character, action, data);
        data->CPos_X++;
      }
      break;

      case ET_BACKSPACECHAR:
      case ET_DELETECHAR:
      {
        D(DBF_UNDO, "redo BACKSPACECHAR/DELETECHAR");
        RemoveChars(data->CPos_X, data->actualline, 1, data);
      }
      break;

      case ET_SPLITLINE:
      {
        D(DBF_UNDO, "redo SPLITLINE");
        SplitLine(data->CPos_X, data->actualline, TRUE, NULL, data);
      }
      break;

      case ET_MERGELINES:
      case ET_BACKSPACEMERGE:
      {
        D(DBF_UNDO, "redo MERGELINES/BACKSPACEMERGE");
        MergeLines(data->actualline, data);
      }
      break;

      case ET_PASTEBLOCK:
      {
        struct Hook *oldhook = data->ImportHook;

        D(DBF_UNDO, "redo PASTEBLOCK");
        data->ImportHook = &ImPlainHook;
        InsertText(data, (char *)action->clip, TRUE);
        data->ImportHook = oldhook;
        MyFreePooled(data->mypool, action->clip);

        action->blk.x = data->CPos_X;
        action->blk.y = LineNr(data->actualline, data);
      }
      break;

      case ET_DELETEBLOCK_NOMOVE:
      case ET_DELETEBLOCK:
      {
        struct marking block =
        {
          TRUE,
          LineNode(action->y, data),
          action->x,
          LineNode(action->blk.y, data),
          action->blk.x
        };
        char *clip = GetBlock(&block, data);

        D(DBF_UNDO, "redo DELETEBLOCK/DELETEBLOCK_NOMOVE");
        CutBlock2(data, FALSE, FALSE, &block, TRUE);
        action->clip = (unsigned char *)clip;
      }
      break;

      default:
        // nothing to do
      break;
    }

    ScrollIntoDisplay(data);

    if(isFlagSet(data->flags, FLG_Active))
      SetCursor(data->CPos_X, data->actualline, TRUE, data);

    // if nextUndoStep == usedUndoSteps this signals that we
    // don't have any things to redo anymore.
    if(data->nextUndoStep == data->usedUndoSteps)
      set(data->object, MUIA_TextEditor_RedoAvailable, FALSE);

    success = TRUE;
  }
  else
  {
    DoMethod(data->object, MUIM_TextEditor_HandleError, Error_NothingToRedo);
  }

  D(DBF_UNDO, "after  maxUndoSteps=%ld nextUndoStep=%ld usedUndoSteps=%ld", data->maxUndoSteps, data->nextUndoStep, data->usedUndoSteps);

  RETURN(success);
  return success;
}

///
/// AddToUndoBuffer()
BOOL AddToUndoBuffer(enum EventType eventtype, char *eventdata, struct InstData *data)
{
  ENTER();

  D(DBF_UNDO, "before maxUndoSteps=%ld nextUndoStep=%ld usedUndoSteps=%ld", data->maxUndoSteps, data->nextUndoStep, data->usedUndoSteps);

  if(data->maxUndoSteps > 0)
  {
    struct UserAction *action;

    // check if there is still enough space in our undo buffer
    // and if not we clean it up one entry
    if(data->nextUndoStep == data->maxUndoSteps)
    {
      ULONG i;

      // free the oldest stored action and forget about it
      FreeUndoStep(data, &data->undoSteps[0]);
      data->nextUndoStep--;
      data->usedUndoSteps--;

      // shift all remaining actions one step to the front
      for(i = 1; i < data->maxUndoSteps; i++)
        memcpy(&data->undoSteps[i-1], &data->undoSteps[i], sizeof(data->undoSteps[i]));

      // signal the user that something in the undo buffer was lost
      setFlag(data->flags, FLG_UndoLost);
      D(DBF_UNDO, "one undo step was lost");
    }

    action = &data->undoSteps[data->nextUndoStep];

    // advance the index for the next undo step
    data->nextUndoStep++;
    // and count this new step
    data->usedUndoSteps++;

    // as we are about to set something new for an undo
    // operation we have to signal that redo operation
    // is cleared now.
    if(data->nextUndoStep == data->usedUndoSteps)
      set(data->object, MUIA_TextEditor_RedoAvailable, FALSE);

    // and we definitely have something to undo now
    set(data->object, MUIA_TextEditor_UndoAvailable, TRUE);

    action->x = data->CPos_X;
    action->y = LineNr(data->actualline, data);
    action->type = eventtype;

    switch(eventtype)
    {
      case ET_BACKSPACEMERGE:
      case ET_MERGELINES:
      {
        D(DBF_UNDO, "add undo MERGELINES/BACKSPACEMERGE");
        action->del.highlight = data->actualline->next->line.Highlight;
        action->del.flow = data->actualline->next->line.Flow;
        action->del.separator = data->actualline->next->line.Separator;
      }
      break;

      case ET_DELETECHAR:
      case ET_BACKSPACECHAR:
      {
        D(DBF_UNDO, "add undo DELETECHAR/BACKSPACECHAR");
        action->del.character = *eventdata;
        action->del.style = GetStyle(data->CPos_X, data->actualline);
        action->del.flow = data->actualline->line.Flow;
        action->del.separator = data->actualline->line.Separator;
        #warning is buffer->del.highlight missing here?
      }
      break;

      case ET_PASTEBLOCK:
      {
        struct marking *block = (struct marking *)eventdata;

        D(DBF_UNDO, "add undo PASTEBLOCK");
        action->x = block->startx;
        action->y = LineNr(block->startline, data);
        action->blk.x = block->stopx;
        action->blk.y = LineNr(block->stopline, data);
      }
      break;

      case ET_DELETEBLOCK:
      {
        char *text;
        struct marking *block = (struct marking *)eventdata;

        D(DBF_UNDO, "add undo DELETEBLOCK");
        if((text = GetBlock((struct marking *)eventdata, data)) != NULL)
        {
          action->x = block->startx;
          action->y = LineNr(block->startline, data);
          action->clip = (unsigned char *)text;

          if(isFlagSet(data->flags, FLG_FreezeCrsr))
            action->type = ET_DELETEBLOCK_NOMOVE;
        }
        else
        {
          ResetUndoBuffer(data);
          DoMethod(data->object, MUIM_TextEditor_HandleError, Error_NotEnoughUndoMem);
        }
      }
      break;

      default:
      {
        D(DBF_UNDO, "add undo PASTECHARS");
        // nothing to do
      }
      break;
    }
  }

  D(DBF_UNDO, "after  maxUndoSteps=%ld nextUndoStep=%ld usedUndoSteps=%ld", data->maxUndoSteps, data->nextUndoStep, data->usedUndoSteps);

  RETURN(TRUE);
  return(TRUE);
}

///
/// ResetUndoBuffer()
void ResetUndoBuffer(struct InstData *data)
{
  ENTER();

  if(data->maxUndoSteps != 0)
  {
    ULONG i;

    for(i = 0; i < data->usedUndoSteps; i++)
      FreeUndoStep(data, &data->undoSteps[i]);

    data->usedUndoSteps = 0;
    data->nextUndoStep = 0;
  }

  LEAVE();
}

///
/// ResizeUndoBuffer()
void ResizeUndoBuffer(struct InstData *data, ULONG maxSteps)
{
  ENTER();

  if(data->maxUndoSteps != maxSteps)
  {
    D(DBF_UNDO, "resizing undo buffer for %ld undo steps", maxSteps);

    if(data->undoSteps != NULL)
    {
      ResetUndoBuffer(data);
      MyFreePooled(data->mypool, data->undoSteps);
    }

    // reset everything to zero
    data->undoSteps = NULL;
    data->maxUndoSteps = 0;
    data->usedUndoSteps = 0;
    data->nextUndoStep = 0;

    if(maxSteps > 0)
    {
      ULONG newSize;

      // calculate number of bytes from number of undo levels
      newSize = (maxSteps * sizeof(struct UserAction));

      // allocate a new undo buffer
      if((data->undoSteps = MyAllocPooled(data->mypool, newSize)) != NULL)
        data->maxUndoSteps = maxSteps;
    }
  }

  LEAVE();
}

///
