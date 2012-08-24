/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2010 by TextEditor.mcc Open Source Team

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

#include <proto/exec.h>

#include "private.h"
#include "Debug.h"

/// AllocLine()
struct line_node *AllocLine(struct InstData *data)
{
  struct line_node *line;

  ENTER();

  line = AllocVecPooled(data->mypool, sizeof(*line));
  MEMTRACK("AllocLine", line, sizeof(*line));

  RETURN(line);
  return line;
}

///
/// FreeLine()
void FreeLine(struct InstData *data, struct line_node *line)
{
  ENTER();

  // we make sure the line is not referenced by other
  // structures as well such as the global blockinfo structure.
  if(data->blockinfo.startline == line)
  {
    data->blockinfo.startline = GetNextLine(line);
    if(data->blockinfo.startline == NULL)
    {
      data->blockinfo.enabled = FALSE;
    }
  }

  if(data->blockinfo.stopline == line)
  {
    data->blockinfo.stopline = GetPrevLine(line);
    if(data->blockinfo.stopline == NULL)
    {
      data->blockinfo.enabled = FALSE;
    }
  }

  // finally free the line itself
  FreeVecPooled(data->mypool, line);
  UNMEMTRACK("AllocLine", line);

  LEAVE();
}

///
