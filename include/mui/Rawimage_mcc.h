#ifndef RAWIMAGE_MCC_H
#define RAWIMAGE_MCC_H

/***************************************************************************

 Magic User Interface - MUI
 Copyright (C) 1992-2003 by Stefan Stuntz <stefan@stuntz.com>
 All Rights Reserved.

 This program/documents may not in whole or in part, be copied, photocopied
 reproduced, translated or reduced to any electronic medium or machine
 readable form without prior consent, in writing, from the above authors.

 With this document the authors makes no warrenties or representations,
 either expressed or implied, with respect to MUI. The information
 presented herein is being supplied on an "AS IS" basis and is expressly
 subject to change without notice. The entire risk as to the use of this
 information is assumed by the user.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 IN NO EVENT WILL THE AUTHORS BE LIABLE FOR DIRECT, INDIRECT, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES RESULTING FROM ANY CLAIM ARISING OUT OF THE
 INFORMATION PRESENTED HEREIN, EVEN IF IT HAS BEEN ADVISED OF THE
 POSSIBILITIES OF SUCH DAMAGES.

 MUI Official Support Site:  http://www.sasg.com/

 $Id$

***************************************************************************/

#if defined(__PPC__)
	#pragma pack(2)
#endif

/*** MUI Defines ***/

#define MUIC_Rawimage  "Rawimage.mcc"
#define RawimageObject MUI_NewObject(MUIC_Rawimage

/*** Methods ***/

#if defined(__MORPHOS__)
/*
 * MorphOS' Rawimage.mcc is Copyright (C) 2008 Christian Rosentreter <tokai@binaryriot.org>
 * All rights reserved
 */
#include <libraries/iffparse.h> /* for MAKE_ID() */

struct MUI_RawimageData
{
  ULONG ri_Width;
  ULONG ri_Height;
  ULONG ri_Format;
  ULONG ri_Size;
  ULONG ri_Data[0];
};

#define RAWIMAGE_FORMAT_RAW_ARGB_ID        0
#define RAWIMAGE_FORMAT_BZ2_ARGB_ID        MAKE_ID('B', 'Z', '2', '\0')
/*** Attributes ***/

#define MUIA_Rawimage_Data                 0xfed10014 /* [IS.]  struct MUI_RawimageData * v20.1 (06.01.2007) */
#else
/*** Attributes ***/

#define RAWIMAGE_TAGBASE                   (TAG_USER|(0x2677<<16))
#define MUIA_Rawimage_Data                 (RAWIMAGE_TAGBASE+1)  /* [ISG] APTR  * v20.1 */
#define MUIA_Rawimage_Width                (RAWIMAGE_TAGBASE+2)  /* [ISG] LONG  * v20.1 */
#define MUIA_Rawimage_Height               (RAWIMAGE_TAGBASE+3)  /* [ISG] LONG  * v20.1 */
#define MUIA_Rawimage_DataFormat           (RAWIMAGE_TAGBASE+4)  /* [ISG] ULONG * v20.1 */
#define MUIA_Rawimage_CLUT                 (RAWIMAGE_TAGBASE+5)  /* [ISG] APTR  * v20.1 */
#define MUIA_Rawimage_Alpha                (RAWIMAGE_TAGBASE+6)  /* [ISG] ULONG * v20.1 */

/*** Special attribute values ***/

#define MUIV_Rawimage_DataFormat_CLUT8     0
#define MUIV_Rawimage_DataFormat_RGB24     1
#define MUIV_Rawimage_DataFormat_ARGB32    2
#endif

#if defined(__PPC__)
	#pragma pack()
#endif

#endif /* RAWIMAGE_MCC_H */

