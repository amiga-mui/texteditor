#/***************************************************************************
#
# TextEditor.mcc - Textediting MUI Custom Class
# Copyright (C) 1997-2000 Allan Odgaard
# Copyright (C) 2005-2010 by TextEditor.mcc Open Source Team
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# TextEditor class Support Site:  http://www.sf.net/projects/texteditor-mcc
#
# $Id$
#
#***************************************************************************/

.PHONY: all
all: mcc mcp demo

.PHONY: mcc
mcc:
	@$(MAKE) OS=os3 -C mcc --no-print-directory
	@$(MAKE) OS=os4 -C mcc --no-print-directory
	@$(MAKE) OS=mos -C mcc --no-print-directory

.PHONY: mcp
mcp:
	@$(MAKE) OS=os3 -C mcp --no-print-directory
	@$(MAKE) OS=os4 -C mcp --no-print-directory
	@$(MAKE) OS=mos -C mcp --no-print-directory

.PHONY: demo
demo:
	@$(MAKE) OS=os3 -C demo --no-print-directory
	@$(MAKE) OS=os4 -C demo --no-print-directory
	@$(MAKE) OS=mos -C demo --no-print-directory

.PHONY: clean
clean:
	@$(MAKE) OS=os3 -C mcc --no-print-directory clean
	@$(MAKE) OS=os4 -C mcp --no-print-directory clean
	@$(MAKE) OS=mos -C demo --no-print-directory clean

.PHONY: cleanall
cleanall:
	@$(MAKE) OS=os3 -C mcc --no-print-directory cleanall
	@$(MAKE) OS=os4 -C mcp --no-print-directory cleanall
	@$(MAKE) OS=mos -C demo --no-print-directory cleanall

.PHONY: install
install:
	@$(MAKE) OS=os3 -C mcc --no-print-directory install
	@$(MAKE) OS=os4 -C mcp --no-print-directory install
	@$(MAKE) OS=mos -C demo --no-print-directory install

.PHONY: bumprev
bumprev:
	@sh tools/bumprev.sh all

.PHONY: release
release:
	@sh tools/mkrelease.sh
