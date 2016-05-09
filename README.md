# TextEditor.mcc

[![Build Status](https://travis-ci.org/amiga-mui/texteditor.svg?branch=master)](https://travis-ci.org/amiga-mui/texteditor) [![Code Climate](https://codeclimate.com/github/amiga-mui/texteditor/badges/gpa.svg)](https://codeclimate.com/github/amiga-mui/texteditor) [![License](http://img.shields.io/:license-lgpl2-blue.svg?style=flat)](http://www.gnu.org/licenses/lgpl-2.1.html) [![Github Issues](http://githubbadges.herokuapp.com/amiga-mui/texteditor/issues.svg)](https://github.com/amiga-mui/texteditor/issues)

TextEditor.mcc is a multiline string gadget for Amiga systems which offers most
of the common features you can find in a typical text editor, but embedded in a
MUI custom class. It is used by a wide range of well-known MUI based applications like
[YAM](https://github.com/jens-maus/yam) or [SimpleMail](https://github.com/sba1/simplemail)
as the main text editing gadget.

TextEditor.mcc is available for AmigaOS3, AmigaOS4, MorphOS and AROS.

## Features

Its main features are:

* Text can be colored (limited syntax highlighting)
* WYSIWYG for bold, italic and underline soft styles
* Multi-level undo & redo
* Lines with different alignments (left, right, centered)
* Ability to insert thick or thin separators with an optional title
* Easy marking either using the mouse or keyboard
* Full clipboard support, offering very fast cut & paste routines
* Light text highlighting for e-mail based on an opening ">" character
* Support for external spellcheckers, including type'n'spell and word guessing
* Configurable keys, colors, cursor types, blink speed, and much more
* Ability to have a bitmap as a background
* Support for proportional fonts with floating word wrap, as well as antialiased fonts
* Pixel smooth scrolling
* AREXX interface for scripted programming

## Downloads/Releases

All releases up to the most current ones can be downloaded from our
[central releases management](https://github.com/amiga-mui/texteditor/releases).

## Bug Reports / Enhancement Requests

To report bugs use the [bug/issue tracker](https://github.com/amiga-mui/texteditor/issues).

## Manual Installation

1. Extract the archive to a temporary directory.
   ```
   > cd RAM:
   > lha x MCC_TextEditor.lha
   ```

2. Go to the `MCC_TextEditor/Libs/MUI/<OS>` directory where `<OS>` is the directory
   matching the operating system you want to install TexiEditor.mcc for:
   ```
   > cd MCC_TextEditor/Libs/MUI/AmigaOS4
   ```

3. copy all `#?.mcc` and `#?.mcp` files found in that `<OS>` directory to the
   global `MUI:Libs/mui/` directory on your system partition:
   ```
   > copy #?.mcc MUI:Libs/mui/
   > copy #?.mcp MUI:Libs/mui/
   ```

4. reboot and enjoy the new version ;)

## License / Copyright

TextEditor.mcc was originally written in 1997 and is Copyright (C) 1997-2000 by Allan Odgaard.
As of version 15.10, released in April 2005, the gadget is maintained and
Copyright (C) 2005-2016 TextEditor.mcc Open Source Team.

TextEditor is distributed and licensed under the GNU Lesser General Public License Version 2.1.
See [COPYING](COPYING) for more detailed information.

## Authors

* Allan Odgaard
* Gianfranco Gignina
* Ilkka Lehtoranta
* Jens Maus
* Nicolas Rybkin
* Sebastian Bauer
* Thore Böckelmann
