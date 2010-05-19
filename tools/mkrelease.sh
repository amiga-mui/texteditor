#!/bin/sh

############################################################################
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
############################################################################

# TextEditor.mcc release build script
# invoke this script as "./mkrelease.sh" to build the release archives

rm -rf "release"
mkdir -p "release"
mkdir -p "release/MCC_TextEditor"
mkdir -p "release/MCC_TextEditor/Demos"
mkdir -p "release/MCC_TextEditor/Demos/Rexx"
mkdir -p "release/MCC_TextEditor/Developer"
mkdir -p "release/MCC_TextEditor/Developer/Autodocs"
mkdir -p "release/MCC_TextEditor/Developer/C"
mkdir -p "release/MCC_TextEditor/Developer/C/Examples"
mkdir -p "release/MCC_TextEditor/Developer/C/include"
mkdir -p "release/MCC_TextEditor/Developer/C/include/mui"
mkdir -p "release/MCC_TextEditor/Docs"
mkdir -p "release/MCC_TextEditor/Libs"
mkdir -p "release/MCC_TextEditor/Libs/MUI"
mkdir -p "release/MCC_TextEditor/Locale"
mkdir -p "release/MCC_TextEditor/Locale/Catalogs"

make -C mcc release
make -C mcp release
make -C demo release

#for os in os3 os4 mos aros-i386 aros-ppc aros-x86_64; do
for os in os3 os4 mos; do
	case $os in
	    os3)         fullsys="AmigaOS3";;
	    os4)         fullsys="AmigaOS4";;
	    mos)         fullsys="MorphOS";;
	    aros-i386)   fullsys="AROS-i386";;
	    aros-ppc)    fullsys="AROS-ppc";;
	    aros-x86_64) fullsys="AROS-x86_64";;
	esac
	mkdir -p "release/MCC_TextEditor/Libs/MUI/$fullsys"
	cp demo/bin_$os/TextEditor-Demo "release/MCC_TextEditor/Demos/TextEditor-Demo-$fullsys"
	cp dist/MCC_TextEditor/Demos/TextEditor-Demo.info "release/MCC_TextEditor/Demos/TextEditor-Demo-$fullsys.info"
	cp mcc/bin_$os/TextEditor.mcc "release/MCC_TextEditor/Libs/MUI/$fullsys/"
	cp mcp/bin_$os/TextEditor.mcp "release/MCC_TextEditor/Libs/MUI/$fullsys/"
done

make -C mcp catalogs
for language in czech danish french german russian swedish; do
	mkdir -p "release/MCC_TextEditor/Locale/Catalogs/$language"
	cp mcp/locale/$language.catalog "release/MCC_TextEditor/Locale/Catalogs/$language/TextEditor_mcp.catalog"
done

cp -R dist/* "release/"
rm "release/MCC_TextEditor/Demos/TextEditor-Demo.info"
cp AUTHORS ChangeLog COPYING "release/MCC_TextEditor/"
cp doc/MCC_TextEditor.readme "release/MCC_TextEditor/ReadMe"
cp demo/*.ilbm "release/MCC_TextEditor/Demos/"
cp -R demo/rexx/* "release/MCC_TextEditor/Demos/Rexx/"
cp doc/MCC_TextEditor.doc "release/MCC_TextEditor/Developer/Autodocs/"
cp demo/*.c "release/MCC_TextEditor/Developer/C/Examples/"
cp demo/Makefile "release/MCC_TextEditor/Developer/C/Examples/"
cp include/mui/TextEditor_mcc.h "release/MCC_TextEditor/Developer/C/include/mui/"
cp doc/MCC_TextEditor.guide "release/MCC_TextEditor/Docs/"
cp mcp/locale/TextEditor_mcp.cd "release/MCC_TextEditor/Locale/"

releasever=`grep "#define LIB_VERSION" mcc/version.h | awk '{ print $3 }'`
releaserev=`grep "#define LIB_REVISION" mcc/version.h | awk '{ print $3 }'`

echo "  MK MCC_TextEditor-$releasever.$releaserev.lha"
find release -nowarn -name ".svn" -exec rm -rf {} \; 2>/dev/null
pushd release >/dev/null
lha -aq2 ../MCC_TextEditor-$releasever.$releaserev.lha *
popd >/dev/null
