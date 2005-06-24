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

#include <proto/exec.h>

#include "Debug.h"

APTR MyAllocPooled(APTR pool, ULONG length)
{
  ULONG *mem;

  ENTER();

  length += sizeof(ULONG);
  if((mem = AllocPooled(pool, length)))
  {
    *mem = length;
    mem += 1;
  }

  RETURN(mem);
  return(mem);
}

VOID MyFreePooled(APTR pool, APTR mem)
{
  ULONG *memptr, length;

  ENTER();

  memptr = &((ULONG *)mem)[-1];
  length = *memptr;

  FreePooled(pool, memptr, length);

  LEAVE();
}
