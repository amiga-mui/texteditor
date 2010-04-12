/***************************************************************************
**
** MUI - MagicUserInterface
** (c) 1993-2006 Stefan Stuntz
**
** Main Header File
**
****************************************************************************
** Class Tree
****************************************************************************
**
** rootclass                    (BOOPSI's base class)
** +--Notify                   (implements notification mechanism)
** !  +--Family                (handles multiple children)
** !  !  +--Menustrip          (describes a complete menu strip)
** !  !  +--Menu               (describes a single menu)
** !  !  \--Menuitem           (describes a single menu item)
** !  +--Application           (main class for all applications)
** !  +--Window                (main class for all windows)
** !  !  \--Aboutmui           (About window of MUI preferences)
** !  +--Area                  (base class for all GUI elements)
** !     +--Rectangle          (spacing object)
** !     +--Balance            (balancing separator bar)
** !     +--Image              (image display)
** !     +--Bitmap             (draws bitmaps)
** !     !  \--Bodychunk       (makes bitmap from ILBM body chunk)
** !     +--Text               (text display)
** !     +--Gadget             (base class for intuition gadgets)
** !     !  +--String          (string gadget)
** !     !  +--Boopsi          (interface to BOOPSI gadgets)
** !     !  \--Prop            (proportional gadget)
** !     +--Gauge              (fule gauge)
** !     +--Scale              (percentage scale)
** !     +--Colorfield         (field with changeable color)
** !     +--List               (line-oriented list)
** !     !  +--Floattext       (special list with floating text)
** !     !  +--Volumelist      (special list with volumes)
** !     !  +--Scrmodelist     (special list with screen modes)
** !     !  \--Dirlist         (special list with files)
** !     +--Numeric            (base class for slider gadgets)
** !     !  +--Knob            (turning knob)
** !     !  +--Levelmeter      (level display)
** !     !  +--Numericbutton   (space saving popup slider)
** !     !  \--Slider          (traditional slider)
** !     +--Framedisplay       (private)
** !     !  \--Popframe        (private)
** !     +--Imagedisplay       (private)
** !     !  \--Popimage        (private)
** !     +--Pendisplay         (displays a pen specification)
** !     !  \--Poppen          (popup button to adjust a pen spec)
** !     +--Group              (groups other GUI elements)
** !        +--Mccprefs        (private)
** !        +--Register        (handles page groups with titles)
** !        !  \--Penadjust    (group to adjust a pen)
** !        +--Settingsgroup   (private)
** !        +--Settings        (private)
** !        +--Frameadjust     (private)
** !        +--Imageadjust     (private)
** !        +--Virtgroup       (handles virtual groups)
** !        +--Scrollgroup     (virtual groups with scrollbars)
** !        +--Scrollbar       (traditional scrollbar)
** !        +--Listview        (listview)
** !        +--Radio           (radio button)
** !        +--Cycle           (cycle gadget)
** !        +--Coloradjust     (several gadgets to adjust a color)
** !        +--Palette         (complete palette gadget)
** !        +--Popstring       (base class for popup objects)
** !           +--Popobject    (popup aynthing in a separate window)
** !           !  +--Poplist   (popup a simple listview)
** !           !  \--Popscreen (popup a list of public screens)
** !           \--Popasl       (popup an asl requester)
** +--Semaphore                (semaphore equipped objects)
**    +--Applist               (private)
**    +--Dataspace             (handles general purpose data spaces)
**       \--Configdata         (private)
**
****************************************************************************
** General Header File Information
****************************************************************************
**
** All macro and structure definitions follow these rules:
**
** Name                       Meaning
**
** MUIC_<class>               Name of a class
** MUIM_<class>_<method>      Method
** MUIP_<class>_<method>      Methods parameter structure
** MUIV_<class>_<method>_<x>  Special method value
** MUIA_<class>_<attrib>      Attribute
** MUIV_<class>_<attrib>_<x>  Special attribute value
** MUIE_<error>               Error return code from MUI_Error()
** MUII_<name>                Standard MUI image
** MUIX_<code>                Control codes for text strings
** MUIO_<name>                Object type for MUI_MakeObject()
**
** MUIA_... attribute definitions are followed by a comment
** consisting of the three possible letters I, S and G.
** I: it's possible to specify this attribute at object creation time.
** S: it's possible to change this attribute with SetAttrs().
** G: it's possible to get this attribute with GetAttr().
**
** Items marked with "Custom Class" are for use in custom classes only!
*/


#ifndef LIBRARIES_MUI_H
#define LIBRARIES_MUI_H

#ifndef MBX /* private */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

#ifndef INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif

#ifndef INTUITION_SCREENS_H
#include <intuition/screens.h>
#endif

#ifndef PROTO_INTUITION_H
#include <proto/intuition.h>
#endif

#if defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack(2)
  #elif defined(__VBCC__)
    #pragma amiga-align
  #endif
#endif

#endif /* private */



/***************************************************************************
** Library specification
***************************************************************************/

#define MUIMASTER_NAME    "muimaster.library"
#define MUIMASTER_VMIN    20
#define MUIMASTER_VLATEST 20

/*
** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
** Warning, some of the macros in this header file work only with
** up2date versions of muimaster.library. If you recompile your programs,
** be sure to open muimaster.library with MUIMASTER_VMIN as version number.
** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/


#define MUI_OBSOLETE


/*************************************************************************
** Config items for MUIM_GetConfigItem
*************************************************************************/

#define MUICFG_PublicScreen            36



/*************************************************************************
** Black box specification structures for images, pens, frames
*************************************************************************/

struct MUI_PenSpec
{
    char buf[32];
};



/*************************************************************************
** Public Screen Stuff
*************************************************************************/

/*
** NOTE: This stuff is only included to allow compilation of the supplied
**       public screen manager for educational purposes. Everything
**       here is subject to change without notice and I guarantee to
**       do that just for fun!
**       More info can be found in the screen manager source file.
*/

#define PSD_INITIAL_NAME   "(unnamed)"
#define PSD_INITIAL_TITLE  "MUI Public Screen"
#define PSD_ID_MPUB        MAKE_ID('M','P','U','B')

#define PSD_NAME_FRONTMOST "«Frontmost»"

#define PSD_FILENAME_SAVE "envarc:mui/PublicScreens.iff"
#define PSD_FILENAME_USE  "env:mui/PublicScreens.iff"

#define PSD_MAXLEN_NAME         32
#define PSD_MAXLEN_TITLE       128
#define PSD_MAXLEN_FONT         48
#define PSD_MAXLEN_BACKGROUND  256
#define PSD_NUMCOLS              8
#define PSD_MAXSYSPENS          20
#define PSD_NUMSYSPENS          12
#define PSD_MAXMUIPENS          10
#define PSD_NUMMUIPENS  MPEN_COUNT

struct MUI_RGBcolor
{
    ULONG red;
    ULONG green;
    ULONG blue;
};

struct MUI_PubScreenDesc
{
    LONG  Version;

    char  Name[PSD_MAXLEN_NAME];
    char  Title[PSD_MAXLEN_TITLE];
    char  Font[PSD_MAXLEN_FONT];
    char  Background[PSD_MAXLEN_BACKGROUND];

    ULONG DisplayID;

    UWORD DisplayWidth;
    UWORD DisplayHeight;

    UBYTE DisplayDepth;
    UBYTE OverscanType;
    UBYTE AutoScroll;
    UBYTE NoDrag;
    UBYTE Exclusive;
    UBYTE Interleaved;
    UBYTE SysDefault;
    UBYTE Behind;
    UBYTE AutoClose;
    UBYTE CloseGadget;
    UBYTE DummyWasForeign;

    BYTE SystemPens[PSD_MAXSYSPENS];
    UBYTE psd_Reserved[1+7*4-PSD_MAXSYSPENS];

    struct MUI_RGBcolor Palette[PSD_NUMCOLS];
    struct MUI_RGBcolor rsvd[PSD_MAXSYSPENS-PSD_NUMCOLS];

    struct MUI_PenSpec rsvd2[PSD_MAXMUIPENS];

    LONG Changed;
    APTR UserData;
};

struct MUIS_InfoClient
{
    struct MinNode node;
    struct Task *task;
    ULONG sigbit;
};


/***************************************************************************
** Object Types for MUI_MakeObject()
***************************************************************************/

#define MUIO_Label          1   /* STRPTR label, ULONG flags */
#define MUIO_Button         2   /* STRPTR label */
#define MUIO_Checkmark      3   /* STRPTR label */
#define MUIO_Cycle          4   /* STRPTR label, STRPTR *entries */
#define MUIO_Radio          5   /* STRPTR label, STRPTR *entries */
#define MUIO_Slider         6   /* STRPTR label, LONG min, LONG max */
#define MUIO_String         7   /* STRPTR label, LONG maxlen */
#define MUIO_PopButton      8   /* STRPTR imagespec */
#define MUIO_HSpace         9   /* LONG space   */
#define MUIO_VSpace        10   /* LONG space   */
#define MUIO_HBar          11   /* LONG space   */
#define MUIO_VBar          12   /* LONG space   */
#define MUIO_MenustripNM   13   /* struct NewMenu *nm, ULONG flags */
#define MUIO_Menuitem      14   /* STRPTR label, STRPTR shortcut, ULONG flags, ULONG data  */
#define MUIO_BarTitle      15   /* STRPTR label */
#define MUIO_NumericButton 16   /* STRPTR label, LONG min, LONG max, STRPTR format */

#define MUIO_Menuitem_CopyStrings (1<<30)

#define MUIO_Label_SingleFrame   (1<< 8)
#define MUIO_Label_DoubleFrame   (1<< 9)
#define MUIO_Label_LeftAligned   (1<<10)
#define MUIO_Label_Centered      (1<<11)
#define MUIO_Label_FreeVert      (1<<12)
#define MUIO_Label_Tiny          (1<<13)
#define MUIO_Label_DontCopy      (1<<14)

#define MUIO_MenustripNM_CommandKeyCheck (1<<0) /* check for "localized" menu items such as "O\0Open" */



/***************************************************************************
** ARexx Interface
***************************************************************************/

struct MUI_Command
{
    char        *mc_Name;
    char        *mc_Template;
    LONG         mc_Parameters;
    struct Hook *mc_Hook;
    LONG         mc_Reserved[5];
};

#define MC_TEMPLATE_ID ((STRPTR)~0)

#define MUI_RXERR_BADDEFINITION  -1
#define MUI_RXERR_OUTOFMEMORY    -2
#define MUI_RXERR_UNKNOWNCOMMAND -3
#define MUI_RXERR_BADSYNTAX      -4


/***************************************************************************
** Return values for MUI_Error()
***************************************************************************/

#define MUIE_OK                  0
#define MUIE_OutOfMemory         1
#define MUIE_OutOfGfxMemory      2
#define MUIE_InvalidWindowObject 3
#define MUIE_MissingLibrary      4
#define MUIE_NoARexx             5
#define MUIE_SingleTask          6



/***************************************************************************
** Standard MUI Images & Backgrounds
***************************************************************************/

#define MUII_WindowBack      0   /* These images are configured   */
#define MUII_RequesterBack   1   /* with the preferences program. */
#define MUII_ButtonBack      2
#define MUII_ListBack        3
#define MUII_TextBack        4
#define MUII_PropBack        5
#define MUII_PopupBack       6
#define MUII_SelectedBack    7
#define MUII_ListCursor      8
#define MUII_ListSelect      9
#define MUII_ListSelCur     10
#define MUII_ArrowUp        11
#define MUII_ArrowDown      12
#define MUII_ArrowLeft      13
#define MUII_ArrowRight     14
#define MUII_CheckMark      15
#define MUII_RadioButton    16
#define MUII_Cycle          17
#define MUII_PopUp          18
#define MUII_PopFile        19
#define MUII_PopDrawer      20
#define MUII_PropKnob       21
#define MUII_Drawer         22
#define MUII_HardDisk       23
#define MUII_Disk           24
#define MUII_Chip           25
#define MUII_Volume         26
#define MUII_RegisterBack   27
#define MUII_Network        28
#define MUII_Assign         29
#define MUII_TapePlay       30
#define MUII_TapePlayBack   31
#define MUII_TapePause      32
#define MUII_TapeStop       33
#define MUII_TapeRecord     34
#define MUII_GroupBack      35
#define MUII_SliderBack     36
#define MUII_SliderKnob     37
#define MUII_TapeUp         38
#define MUII_TapeDown       39
#define MUII_PageBack       40
#define MUII_ReadListBack   41
#define MUII_PopFont        42
#define MUII_Count          43

#define MUII_BACKGROUND     128    /* These are direct color    */
#define MUII_SHADOW         129    /* combinations and are not  */
#define MUII_SHINE          130    /* affected by users prefs.  */
#define MUII_FILL           131
#define MUII_SHADOWBACK     132    /* Generally, you should     */
#define MUII_SHADOWFILL     133    /* avoid using them. Better  */
#define MUII_SHADOWSHINE    134    /* use one of the customized */
#define MUII_FILLBACK       135    /* images above.             */
#define MUII_FILLSHINE      136
#define MUII_SHINEBACK      137
#define MUII_FILLBACK2      138
#define MUII_HSHINEBACK     139
#define MUII_HSHADOWBACK    140
#define MUII_HSHINESHINE    141
#define MUII_HSHADOWSHADOW  142
#define MUII_MARKSHINE      143
#define MUII_MARKHALFSHINE  144
#define MUII_MARKBACKGROUND 145
#define MUII_BARBLOCK       146
#define MUII_BARDETAIL      147
#ifdef MBX /* private */
#define MUII_SHINETHROUGH   148 /* private */
#define MUII_LASTPAT        148 /* private */
#else /* private */
#define MUII_LASTPAT        147
#endif /* private */



/***************************************************************************
** Special values for some methods
***************************************************************************/

#define MUIV_TriggerValue    0x49893131
#define MUIV_NotTriggerValue 0x49893133
#define MUIV_EveryTime       0x49893131

#define MUIV_Notify_Self        1
#define MUIV_Notify_Window      2
#define MUIV_Notify_Application 3
#define MUIV_Notify_Parent      4

#define MUIV_Application_Save_ENV     ((STRPTR) 0)
#define MUIV_Application_Save_ENVARC  ((STRPTR)~0)
#define MUIV_Application_Load_ENV     ((STRPTR) 0)
#define MUIV_Application_Load_ENVARC  ((STRPTR)~0)

#define MUIV_Application_ReturnID_Quit -1

#define MUIV_List_Insert_Top             0
#define MUIV_List_Insert_Active         -1
#define MUIV_List_Insert_Sorted         -2
#define MUIV_List_Insert_Bottom         -3

#define MUIV_List_Remove_First           0
#define MUIV_List_Remove_Active         -1
#define MUIV_List_Remove_Last           -2
#define MUIV_List_Remove_Selected       -3

#define MUIV_List_Select_Off             0
#define MUIV_List_Select_On              1
#define MUIV_List_Select_Toggle          2
#define MUIV_List_Select_Ask             3

#define MUIV_List_GetEntry_Active       -1
#define MUIV_List_Select_Active         -1
#define MUIV_List_Select_All            -2

#define MUIV_List_Redraw_Active         -1
#define MUIV_List_Redraw_All            -2

#define MUIV_List_Move_Top               0
#define MUIV_List_Move_Active           -1
#define MUIV_List_Move_Bottom           -2
#define MUIV_List_Move_Next             -3 /* only valid for second parameter */
#define MUIV_List_Move_Previous         -4 /* only valid for second parameter */

#define MUIV_List_Exchange_Top           0
#define MUIV_List_Exchange_Active       -1
#define MUIV_List_Exchange_Bottom       -2
#define MUIV_List_Exchange_Next         -3 /* only valid for second parameter */
#define MUIV_List_Exchange_Previous     -4 /* only valid for second parameter */

#define MUIV_List_Jump_Top               0
#define MUIV_List_Jump_Active           -1
#define MUIV_List_Jump_Bottom           -2
#define MUIV_List_Jump_Up               -4
#define MUIV_List_Jump_Down             -3

#define MUIV_List_NextSelected_Start    -1
#define MUIV_List_NextSelected_End      -1

#define MUIV_DragQuery_Refuse 0
#define MUIV_DragQuery_Accept 1

#define MUIV_DragReport_Abort    0
#define MUIV_DragReport_Continue 1
#define MUIV_DragReport_Lock     2
#define MUIV_DragReport_Refresh  3

#define MUIV_CreateBitMapBuffer_HideFrame (1<<0) /* private */
#define MUIV_CreateBitMapBuffer_OldStyle  (1<<1) /* private */

#define MUIV_CreateBubble_DontHidePointer (1<<0)

#define MUIV_Application_OCW_ScreenPage (1<<1) /* show just the screen page of the config window */

#define MUIV_ContextMenuBuild_Default 0xffffffff

#define MUIV_PushMethod_Delay(millis) (((ULONG)millis)<<8)



/***************************************************************************
** Control codes for text strings
***************************************************************************/

#define MUIX_R "\033r"    /* right justified */
#define MUIX_C "\033c"    /* centered        */
#define MUIX_L "\033l"    /* left justified  */

#define MUIX_N "\033n"    /* normal     */
#define MUIX_B "\033b"    /* bold       */
#define MUIX_I "\033i"    /* italic     */
#define MUIX_U "\033u"    /* underlined */

#define MUIX_PT "\0332"   /* text pen           */
#define MUIX_PH "\0338"   /* highlight text pen */



/***************************************************************************
** Parameter structures for some classes
***************************************************************************/

struct MUI_Palette_Entry
{
    LONG  mpe_ID;
    ULONG mpe_Red;
    ULONG mpe_Green;
    ULONG mpe_Blue;
    LONG  mpe_Group;
};

#define MUIV_Palette_Entry_End -1


/* Popmenu class item spec and definitions */

struct MUIP_Popmenu_Item
{
    ULONG        pi_Type;
    CONST_STRPTR pi_Key;
    CONST_STRPTR pi_Text;
    ULONG        pi_Flags;
    struct       MUIP_Popmenu_Item *pi_SubMenu;
    APTR         pi_UserData;
    ULONG        pi_MXField;
};

/* values for pi_Type */

#define PIT_END             0
#define PIT_TITLE           1
#define PIT_TEXT            2
#define PIT_IMAGE           3
#define PIT_SEPARATOR       4
#define PIT_SUBTITLE        5
#define PIT_IGNORE          6


/* values for pi_Flags */

#define PIB_TOGGLE          0
#define PIB_RADIO           1
#define PIB_DISABLED        2
#define PIB_SELECTED        3
#define PIB_RELATIVEMX      4
#define PIB_READONLY        5
#define PIB_PRESSED         6   /* Kind of private... */
#define PIB_CREATESUBMENU   7
#define PIB_MULTICOLUMNS    8

#define PIF_TOGGLE          (1<<PIB_TOGGLE)
#define PIF_RADIO           (1<<PIB_RADIO)
#define PIF_DISABLED        (1<<PIB_DISABLED)
#define PIF_SELECTED        (1<<PIB_SELECTED)
#define PIF_RELATIVEMX      (1<<PIB_RELATIVEMX)
#define PIF_READONLY        (1<<PIB_READONLY)
#define PIF_PRESSED         (1<<PIB_PRESSED)
#define PIF_CREATESUBMENU   (1<<PIB_CREATESUBMENU)
#define PIF_MULTICOLUMNS    (1<<PIB_MULTICOLUMNS)


/*****************************/
/* Application Input Handler */
/*****************************/

struct MUI_InputHandlerNode
{
    struct MinNode ihn_Node;
    Object        *ihn_Object;

    union
    {
        ULONG ihn_sigs;
        struct
        {
            UWORD ihn_millis;
            UWORD ihn_current;
        } ihn_timer;
    } ihn_stuff;

    ULONG          ihn_Flags; /* see below */
    ULONG          ihn_Method;
};

#define ihn_Signals ihn_stuff.ihn_sigs
#define ihn_Millis  ihn_stuff.ihn_timer.ihn_millis
#define ihn_Current ihn_stuff.ihn_timer.ihn_current

/* Flags for ihn_Flags */
#define MUIIHNF_TIMER           (1<<0) /* set ihn_Millis to number of 1/1000 sec ticks you want to be triggered */
#define MUIIHNF_TIMER_SCALE10   (1<<1) /* ihn_Millis is in 1/100 seconds instead */
#define MUIIHNF_TIMER_SCALE100  (1<<2) /* ihn_Millis is in 1/10 seconds instead */
                                       /* setting both SCALE10|SCALE100 makes ihn_Millis 1/1 seconds */
#define MUIIHNF_ISENABLED       (1<<3) /* private */



/************************/
/* Window Event Handler */
/************************/

struct MUI_EventHandlerNode
{
    struct MinNode ehn_Node;
    #ifdef MBX            /* private */
    /* we rebuild a CaOS ListNode here since used with Enqueue() */ /* private */
    STRPTR  ln_Name;      /* private */
    UWORD   ln_Type;      /* private */
    WORD    ehn_Priority; /* private */
    WORD    ehn_Reserved; /* private */
    #else                 /* private */
    BYTE           ehn_Reserved; /* don't touch! */
    BYTE           ehn_Priority; /* event handlers are inserted according to their priority. */
    #endif                /* private */
    UWORD          ehn_Flags;    /* certain flags, see below for definitions. */
    Object        *ehn_Object;   /* object which should receive MUIM_HandleEvent. */
    struct IClass *ehn_Class;    /* if !=NULL, MUIM_HandleEvent is invoked on exactly this class with CoerceMethod(). */
    ULONG          ehn_Events;   /* one or more IDCMP flags this handler should react on. */
};

/* flags for ehn_Flags */

#define MUI_EHF_ALWAYSKEYS  (1<<0)  /* not for public use */

#define MUI_EHF_GUIMODE     (1<<1)  /* set this if you dont want your handler to be called */
                                    /* when your object is disabled or invisible */

#define MUI_EHF_ISACTIVEGRP (1<<12) /* not for public use */

#define MUI_EHF_ISACTIVE    (1<<13) /* this flag is maintained by MUI and READ-ONLY: */
                                    /* set when ehn_Object is a window's active or default object. */

#define MUI_EHF_ISCALLING   (1<<14) /* not for public use */

#define MUI_EHF_ISENABLED   (1<<15) /* this flag is maintained by MUI and READ-ONLY: */
                                    /* it is set when the handler is added (after MUIM_Window_AddEventHandler) */
                                    /* and cleared when the handler is removed (after MUIM_Window_RemEventHandler). */
                                    /* you may not change the state of this flag yourself, but you may read it */
                                    /* to find out whether your handler is currently added to a window or not. */


/* other values reserved for future use */

/* return values for MUIM_HandleEvent (bit-masked, all other bits must be 0) */
#define MUI_EventHandlerRC_Eat (1<<0) /* stop MUI from calling other handlers */


/**********************/
/* List Position Test */
/**********************/

struct MUI_List_TestPos_Result
{
    LONG  entry;   /* number of entry, -1 if mouse not over valid entry */
    WORD  column;  /* numer of column, -1 if no valid column */
    UWORD flags;   /* see below */
    WORD  xoffset; /* x offset of mouse click relative to column start */
    WORD  yoffset; /* y offset of mouse click from center of line
                      (negative values mean click was above center,
                       positive values mean click was below center) */
};

#define MUI_LPR_ABOVE  (1<<0)
#define MUI_LPR_BELOW  (1<<1)
#define MUI_LPR_LEFT   (1<<2)
#define MUI_LPR_RIGHT  (1<<3)


/***************************************************************************
**
** Macro Section
** -------------
**
** To make GUI creation more easy and understandable, you can use the
** macros below. If you dont want, just define MUI_NOSHORTCUTS to disable
** them.
**
** These macros are available to C programmers only.
**
***************************************************************************/

#ifndef MUI_NOSHORTCUTS



/***************************************************************************
**
** Object Generation
** -----------------
**
** The xxxObject (and xChilds) macros generate new instances of MUI classes.
** Every xxxObject can be followed by tagitems specifying initial create
** time attributes for the new object and must be terminated with the
** End macro:
**
** obj = StringObject,
**          MUIA_String_Contents, "foo",
**          MUIA_String_MaxLen  , 40,
**          End;
**
** With the Child, SubWindow and WindowContents shortcuts you can
** construct a complete GUI within one command:
**
** app = ApplicationObject,
**
**          ...
**
**          SubWindow, WindowObject,
**             WindowContents, VGroup,
**                Child, String("foo",40),
**                Child, String("bar",50),
**                Child, HGroup,
**                   Child, CheckMark(TRUE),
**                   Child, CheckMark(FALSE),
**                   End,
**                End,
**             End,
**
**          SubWindow, WindowObject,
**             WindowContents, HGroup,
**                Child, ...,
**                Child, ...,
**                End,
**             End,
**
**          ...
**
**          End;
**
***************************************************************************/

#define MenustripObject     MUI_NewObject(MUIC_Menustrip
#define MenuObject          MUI_NewObject(MUIC_Menu
#define MenuObjectT(name)   MUI_NewObject(MUIC_Menu,MUIA_Menu_Title,name
#define PopmenuObject       MUI_NewObject(MUIC_Popmenu
#define MenuitemObject      MUI_NewObject(MUIC_Menuitem
#define WindowObject        MUI_NewObject(MUIC_Window
#define ImageObject         MUI_NewObject(MUIC_Image
#define BitmapObject        MUI_NewObject(MUIC_Bitmap
#define BodychunkObject     MUI_NewObject(MUIC_Bodychunk
#define NotifyObject        MUI_NewObject(MUIC_Notify
#define ApplicationObject   MUI_NewObject(MUIC_Application
#define TextObject          MUI_NewObject(MUIC_Text
#define RectangleObject     MUI_NewObject(MUIC_Rectangle
#define BalanceObject       MUI_NewObject(MUIC_Balance
#define ListObject          MUI_NewObject(MUIC_List
#define PropObject          MUI_NewObject(MUIC_Prop
#define StringObject        MUI_NewObject(MUIC_String
#define ScrollbarObject     MUI_NewObject(MUIC_Scrollbar
#define ListviewObject      MUI_NewObject(MUIC_Listview
#define RadioObject         MUI_NewObject(MUIC_Radio
#define VolumelistObject    MUI_NewObject(MUIC_Volumelist
#define FloattextObject     MUI_NewObject(MUIC_Floattext
#define DirlistObject       MUI_NewObject(MUIC_Dirlist
#define CycleObject         MUI_NewObject(MUIC_Cycle
#define GaugeObject         MUI_NewObject(MUIC_Gauge
#define ScaleObject         MUI_NewObject(MUIC_Scale
#define NumericObject       MUI_NewObject(MUIC_Numeric
#define SliderObject        MUI_NewObject(MUIC_Slider
#define NumericbuttonObject MUI_NewObject(MUIC_Numericbutton
#define KnobObject          MUI_NewObject(MUIC_Knob
#define LevelmeterObject    MUI_NewObject(MUIC_Levelmeter
#define BoopsiObject        MUI_NewObject(MUIC_Boopsi
#define ColorfieldObject    MUI_NewObject(MUIC_Colorfield
#define PenadjustObject     MUI_NewObject(MUIC_Penadjust
#define ColoradjustObject   MUI_NewObject(MUIC_Coloradjust
#define PaletteObject       MUI_NewObject(MUIC_Palette
#define GroupObject         MUI_NewObject(MUIC_Group
#define RegisterObject      MUI_NewObject(MUIC_Register
#define VirtgroupObject     MUI_NewObject(MUIC_Virtgroup
#define ScrollgroupObject   MUI_NewObject(MUIC_Scrollgroup
#define PopstringObject     MUI_NewObject(MUIC_Popstring
#define PopobjectObject     MUI_NewObject(MUIC_Popobject
#define PoplistObject       MUI_NewObject(MUIC_Poplist
#define PopaslObject        MUI_NewObject(MUIC_Popasl
#define PendisplayObject    MUI_NewObject(MUIC_Pendisplay
#define PoppenObject        MUI_NewObject(MUIC_Poppen
#define AboutmuiObject      MUI_NewObject(MUIC_Aboutmui
#define ScrmodelistObject   MUI_NewObject(MUIC_Scrmodelist
#define KeyentryObject      MUI_NewObject(MUIC_Keyentry
#define VGroup              MUI_NewObject(MUIC_Group
#define HGroup              MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE
#define ColGroup(cols)      MUI_NewObject(MUIC_Group,MUIA_Group_Columns,(cols)
#define RowGroup(rows)      MUI_NewObject(MUIC_Group,MUIA_Group_Rows   ,(rows)
#define PageGroup           MUI_NewObject(MUIC_Group,MUIA_Group_PageMode,TRUE
#define VGroupV             MUI_NewObject(MUIC_Virtgroup
#define HGroupV             MUI_NewObject(MUIC_Virtgroup,MUIA_Group_Horiz,TRUE
#define ColGroupV(cols)     MUI_NewObject(MUIC_Virtgroup,MUIA_Group_Columns,(cols)
#define RowGroupV(rows)     MUI_NewObject(MUIC_Virtgroup,MUIA_Group_Rows   ,(rows)
#define PageGroupV          MUI_NewObject(MUIC_Virtgroup,MUIA_Group_PageMode,TRUE
#define RegisterGroup(t)    MUI_NewObject(MUIC_Register,MUIA_Register_Titles,(t)
#define End                 TAG_DONE)

#define Child             MUIA_Group_Child
#define SubWindow         MUIA_Application_Window
#define WindowContents    MUIA_Window_RootObject



/***************************************************************************
**
** Frame Types
** -----------
**
** These macros may be used to specify one of MUI's different frame types.
** Note that every macro consists of one { ti_Tag, ti_Data } pair.
**
** GroupFrameT() is a special kind of frame that contains a centered
** title text.
**
** HGroup, GroupFrameT("Horiz Groups"),
**    Child, RectangleObject, TextFrame  , End,
**    Child, RectangleObject, StringFrame, End,
**    Child, RectangleObject, ButtonFrame, End,
**    Child, RectangleObject, ListFrame  , End,
**    End,
**
***************************************************************************/

#define NoFrame          MUIA_Frame, MUIV_Frame_None
#define ButtonFrame      MUIA_Frame, MUIV_Frame_Button
#define ImageButtonFrame MUIA_Frame, MUIV_Frame_ImageButton
#define TextFrame        MUIA_Frame, MUIV_Frame_Text
#define StringFrame      MUIA_Frame, MUIV_Frame_String
#define ReadListFrame    MUIA_Frame, MUIV_Frame_ReadList
#define InputListFrame   MUIA_Frame, MUIV_Frame_InputList
#define PropFrame        MUIA_Frame, MUIV_Frame_Prop
#define SliderFrame      MUIA_Frame, MUIV_Frame_Slider
#define GaugeFrame       MUIA_Frame, MUIV_Frame_Gauge
#define VirtualFrame     MUIA_Frame, MUIV_Frame_Virtual
#define GroupFrame       MUIA_Frame, MUIV_Frame_Group
#define GroupFrameT(s)   MUIA_Frame, MUIV_Frame_Group, MUIA_FrameTitle, s, MUIA_Background, MUII_GroupBack



/***************************************************************************
**
** Spacing Macros
** --------------
**
***************************************************************************/

#define HVSpace           MUI_NewObject(MUIC_Rectangle,TAG_DONE)
#define HSpace(x)         MUI_MakeObject(MUIO_HSpace,(x))
#define VSpace(x)         MUI_MakeObject(MUIO_VSpace,(x))
#define HCenter(obj)      (HGroup, GroupSpacing(0), Child, HSpace(0), Child, (obj), Child, HSpace(0), End)
#define VCenter(obj)      (VGroup, GroupSpacing(0), Child, VSpace(0), Child, (obj), Child, VSpace(0), End)
#define InnerSpacing(h,v) MUIA_InnerLeft,(h),MUIA_InnerRight,(h),MUIA_InnerTop,(v),MUIA_InnerBottom,(v)
#define GroupSpacing(x)   MUIA_Group_Spacing,(x)



#ifdef MUI_OBSOLETE

/***************************************************************************
**
** String-Object
** -------------
**
** The following macro creates a simple string gadget.
**
***************************************************************************/

#define String(contents,maxlen)\
    StringObject,\
        StringFrame,\
        MUIA_String_MaxLen  , maxlen,\
        MUIA_String_Contents, contents,\
        End

#define KeyString(contents,maxlen,controlchar)\
    StringObject,\
        StringFrame,\
        MUIA_ControlChar    , controlchar,\
        MUIA_String_MaxLen  , maxlen,\
        MUIA_String_Contents, contents,\
        End

#endif



#ifdef MUI_OBSOLETE

/***************************************************************************
**
** CheckMark-Object
** ----------------
**
** The following macro creates a checkmark gadget.
**
***************************************************************************/

#define CheckMark(selected)\
    ImageObject,\
        ImageButtonFrame,\
        MUIA_InputMode        , MUIV_InputMode_Toggle,\
        MUIA_Image_Spec       , MUII_CheckMark,\
        MUIA_Image_FreeVert   , TRUE,\
        MUIA_Selected         , selected,\
        MUIA_Background       , MUII_ButtonBack,\
        MUIA_ShowSelState     , FALSE,\
        End

#define KeyCheckMark(selected,control)\
    ImageObject,\
        ImageButtonFrame,\
        MUIA_InputMode        , MUIV_InputMode_Toggle,\
        MUIA_Image_Spec       , MUII_CheckMark,\
        MUIA_Image_FreeVert   , TRUE,\
        MUIA_Selected         , selected,\
        MUIA_Background       , MUII_ButtonBack,\
        MUIA_ShowSelState     , FALSE,\
        MUIA_ControlChar      , control,\
        End

#endif


/***************************************************************************
**
** Button-Objects
** --------------
**
** Note: Use small letters for KeyButtons, e.g.
**       KeyButton("Cancel",'c')  and not  KeyButton("Cancel",'C') !!
**
***************************************************************************/

#define SimpleButton(label) MUI_MakeObject(MUIO_Button,(unsigned long)label)

#ifdef MUI_OBSOLETE

#define KeyButton(name,key)\
    TextObject,\
        ButtonFrame,\
        MUIA_Font, MUIV_Font_Button,\
        MUIA_Text_Contents, name,\
        MUIA_Text_PreParse, "\33c",\
        MUIA_Text_HiChar  , key,\
        MUIA_ControlChar  , key,\
        MUIA_InputMode    , MUIV_InputMode_RelVerify,\
        MUIA_Background   , MUII_ButtonBack,\
        End

#endif


#ifdef MUI_OBSOLETE

/***************************************************************************
**
** Cycle-Object
** ------------
**
***************************************************************************/

#define Cycle(entries)        CycleObject, MUIA_Font, MUIV_Font_Button, MUIA_Cycle_Entries, entries, End
#define KeyCycle(entries,key) CycleObject, MUIA_Font, MUIV_Font_Button, MUIA_Cycle_Entries, entries, MUIA_ControlChar, key, End



/***************************************************************************
**
** Radio-Object
** ------------
**
***************************************************************************/

#define Radio(name,array)\
    RadioObject,\
        GroupFrameT(name),\
        MUIA_Radio_Entries,array,\
        End

#define KeyRadio(name,array,key)\
    RadioObject,\
        GroupFrameT(name),\
        MUIA_Radio_Entries,array,\
        MUIA_ControlChar, key,\
        End



/***************************************************************************
**
** Slider-Object
** -------------
**
***************************************************************************/


#define Slider(min,max,level)\
    SliderObject,\
        MUIA_Numeric_Min  , min,\
        MUIA_Numeric_Max  , max,\
        MUIA_Numeric_Value, level,\
        End

#define KeySlider(min,max,level,key)\
    SliderObject,\
        MUIA_Numeric_Min  , min,\
        MUIA_Numeric_Max  , max,\
        MUIA_Numeric_Value, level,\
        MUIA_ControlChar , key,\
        End

#endif



/***************************************************************************
**
** Button to be used for popup objects
**
***************************************************************************/

#define PopButton(img) MUI_MakeObject(MUIO_PopButton,(unsigned long)img)



/***************************************************************************
**
** Labeling Objects
** ----------------
**
** Labeling objects, e.g. a group of string gadgets,
**
**   Small: |foo   |
**  Normal: |bar   |
**     Big: |foobar|
**    Huge: |barfoo|
**
** is done using a 2 column group:
**
** ColGroup(2),
**  Child, Label2("Small:" ),
**    Child, StringObject, End,
**  Child, Label2("Normal:"),
**    Child, StringObject, End,
**  Child, Label2("Big:"   ),
**    Child, StringObject, End,
**  Child, Label2("Huge:"  ),
**    Child, StringObject, End,
**    End,
**
** Note that we have three versions of the label macro, depending on
** the frame type of the right hand object:
**
** Label1(): For use with standard frames (e.g. checkmarks).
** Label2(): For use with double high frames (e.g. string gadgets).
** Label() : For use with objects without a frame.
**
** These macros ensure that your label will look fine even if the
** user of your application configured some strange spacing values.
** If you want to use your own labeling, you'll have to pay attention
** on this topic yourself.
**
***************************************************************************/

#define Label(label)   MUI_MakeObject(MUIO_Label,(unsigned long)label,0)
#define Label1(label)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_SingleFrame)
#define Label2(label)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_DoubleFrame)
#define LLabel(label)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_LeftAligned)
#define LLabel1(label) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_LeftAligned|MUIO_Label_SingleFrame)
#define LLabel2(label) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_LeftAligned|MUIO_Label_DoubleFrame)
#define CLabel(label)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_Centered)
#define CLabel1(label) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_Centered|MUIO_Label_SingleFrame)
#define CLabel2(label) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_Centered|MUIO_Label_DoubleFrame)

#define FreeLabel(label)   MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert)
#define FreeLabel1(label)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_SingleFrame)
#define FreeLabel2(label)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_DoubleFrame)
#define FreeLLabel(label)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_LeftAligned)
#define FreeLLabel1(label) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_LeftAligned|MUIO_Label_SingleFrame)
#define FreeLLabel2(label) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_LeftAligned|MUIO_Label_DoubleFrame)
#define FreeCLabel(label)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_Centered)
#define FreeCLabel1(label) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_Centered|MUIO_Label_SingleFrame)
#define FreeCLabel2(label) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_Centered|MUIO_Label_DoubleFrame)

#define KeyLabel(label,key)   MUI_MakeObject(MUIO_Label,(unsigned long)label,key)
#define KeyLabel1(label,key)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_SingleFrame|(key))
#define KeyLabel2(label,key)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_DoubleFrame|(key))
#define KeyLLabel(label,key)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_LeftAligned|(key))
#define KeyLLabel1(label,key) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_LeftAligned|MUIO_Label_SingleFrame|(key))
#define KeyLLabel2(label,key) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_LeftAligned|MUIO_Label_DoubleFrame|(key))
#define KeyCLabel(label,key)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_Centered|(key))
#define KeyCLabel1(label,key) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_Centered|MUIO_Label_SingleFrame|(key))
#define KeyCLabel2(label,key) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_Centered|MUIO_Label_DoubleFrame|(key))

#define FreeKeyLabel(label,key)   MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|(key))
#define FreeKeyLabel1(label,key)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_SingleFrame|(key))
#define FreeKeyLabel2(label,key)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_DoubleFrame|(key))
#define FreeKeyLLabel(label,key)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_LeftAligned|(key))
#define FreeKeyLLabel1(label,key) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_LeftAligned|MUIO_Label_SingleFrame|(key))
#define FreeKeyLLabel2(label,key) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_LeftAligned|MUIO_Label_DoubleFrame|(key))
#define FreeKeyCLabel(label,key)  MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_Centered|(key))
#define FreeKeyCLabel1(label,key) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_Centered|MUIO_Label_SingleFrame|(key))
#define FreeKeyCLabel2(label,key) MUI_MakeObject(MUIO_Label,(unsigned long)label,MUIO_Label_FreeVert|MUIO_Label_Centered|MUIO_Label_DoubleFrame|(key))



/***************************************************************************
**
** Controlling Objects
** -------------------
**
** set() and get() are two short stubs for BOOPSI GetAttr() and SetAttrs()
** calls:
**
** {
**    char *x;
**
**    set(obj,MUIA_String_Contents,"foobar");
**    get(obj,MUIA_String_Contents,&x);
**
**    printf("gadget contains '%s'\n",x);
** }
**
** nnset() sets an attribute without triggering a possible notification.
**
***************************************************************************/

#ifndef __cplusplus

#define get(obj,attr,store) GetAttr(attr,obj,(ULONG *)(void *)store)
#define set(obj,attr,value) SetAttrs(obj,attr,value,TAG_DONE)
#define nnset(obj,attr,value) SetAttrs(obj,MUIA_NoNotify,TRUE,attr,value,TAG_DONE)

#define setmutex(obj,n)     set(obj,MUIA_Radio_Active,n)
#define setcycle(obj,n)     set(obj,MUIA_Cycle_Active,n)
#define setstring(obj,s)    set(obj,MUIA_String_Contents,s)
#define setcheckmark(obj,b) set(obj,MUIA_Selected,b)
#define setslider(obj,l)    set(obj,MUIA_Numeric_Value,l)

#endif


#endif /* MUI_NOSHORTCUTS */


/***************************************************************************
**
** For Boopsi Image Implementors Only:
**
** If MUI is using a boopsi image object, it will send a special method
** immediately after object creation. This method has a parameter structure
** where the boopsi can fill in its minimum and maximum size and learn if
** its used in a horizontal or vertical context.
**
** The boopsi image must use the method id (MUIM_BoopsiQuery) as return
** value. That's how MUI sees that the method is implemented.
**
** Note: MUI does not depend on this method. If the boopsi image doesn't
**       implement it, minimum size will be 0 and maximum size unlimited.
**
***************************************************************************/

#define MUIM_BoopsiQuery 0x80427157 /* this is send to the boopsi and */
                                    /* must be used as return value   */

struct MUI_BoopsiQuery              /* parameter structure */
{
    ULONG mbq_MethodID;              /* always MUIM_BoopsiQuery */

    struct Screen *mbq_Screen;       /* obsolete, use mbq_RenderInfo */
    ULONG mbq_Flags;                 /* read only, see below */

    LONG mbq_MinWidth ;              /* write only, fill in min width  */
    LONG mbq_MinHeight;              /* write only, fill in min height */
    LONG mbq_MaxWidth ;              /* write only, fill in max width  */
    LONG mbq_MaxHeight;              /* write only, fill in max height */
    LONG mbq_DefWidth ;              /* write only, fill in def width  */
    LONG mbq_DefHeight;              /* write only, fill in def height */

    struct MUI_RenderInfo *mbq_RenderInfo;  /* read only, display context */

    /* may grow in future ... */
};

#define MUIP_BoopsiQuery MUI_BoopsiQuery  /* old structure name */

#define MBQF_HORIZ (1<<0)           /* object used in a horizontal */
                                    /* context (else vertical)     */

#define MBQ_MUI_MAXMAX (10000)          /* use this for unlimited MaxWidth/Height */

#define IDCMP_MOUSEOBJECT 0x40000000 /* special idcmp message created by MUI */


/*******************************************/
/* Begin of automatic header file creation */
/*******************************************/




/****************************************************************************/
/** Notify                                                                 **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Notify[];
#else
#define MUIC_Notify "Notify.mui"
#endif

/* Methods */

#define MUIM_CallHook                       0x8042b96b /* V4  */
#define MUIM_Export                         0x80420f1c /* V12 */
#define MUIM_FindObject                     0x8042038f /* private */ /* V13 */
#define MUIM_FindUData                      0x8042c196 /* V8  */
#define MUIM_GetConfigItem                  0x80423edb /* V11 */
#define MUIM_GetSettingsClassID             0x8042e1cd /* private */ /* V20 */
#define MUIM_GetUData                       0x8042ed0c /* V8  */
#define MUIM_GlobalSetup                    0x8042d532 /* private */ /* V4  */
#define MUIM_Import                         0x8042d012 /* V12 */
#define MUIM_KamelleDD_Move                 0x8042a4d8 /* private */ /* V17 */
#define MUIM_KamelleDD_Start                0x804234d2 /* private */ /* V17 */
#define MUIM_KamelleDD_Stop                 0x8042ba10 /* private */ /* V17 */
#define MUIM_KillNotify                     0x8042d240 /* V4  */
#define MUIM_KillNotifyObj                  0x8042b145 /* V16 */
#define MUIM_MultiSet                       0x8042d356 /* V7  */
#define MUIM_NoNotifySet                    0x8042216f /* V9  */
#define MUIM_Notify                         0x8042c9cb /* V4  */
#define MUIM_Set                            0x8042549a /* V4  */
#define MUIM_SetAsString                    0x80422590 /* V4  */
#define MUIM_SetUData                       0x8042c920 /* V8  */
#define MUIM_SetUDataOnce                   0x8042ca19 /* V11 */
#define MUIM_WriteLong                      0x80428d86 /* V6  */
#define MUIM_WriteString                    0x80424bf4 /* V6  */
struct  MUIP_CallHook                       { ULONG MethodID; struct Hook *Hook; ULONG param1; /* ... */ };
struct  MUIP_Export                         { ULONG MethodID; Object *dataspace; };
struct  MUIP_FindObject                     { ULONG MethodID; Object *findme; }; /* private */
struct  MUIP_FindUData                      { ULONG MethodID; ULONG udata; };
struct  MUIP_GetConfigItem                  { ULONG MethodID; ULONG id; ULONG *storage; };
struct  MUIP_GetSettingsClassID             { ULONG MethodID; }; /* private */
struct  MUIP_GetUData                       { ULONG MethodID; ULONG udata; ULONG attr; ULONG *storage; };
struct  MUIP_GlobalSetup                    { ULONG MethodID; struct MUI_GlobalInfo *mgi; }; /* private */
struct  MUIP_Import                         { ULONG MethodID; Object *dataspace; };
struct  MUIP_KamelleDD_Move                 { ULONG MethodID; LONG x; LONG y; Object *dest; }; /* private */
struct  MUIP_KamelleDD_Start                { ULONG MethodID; LONG x; LONG y; }; /* private */
struct  MUIP_KamelleDD_Stop                 { ULONG MethodID; LONG x; LONG y; Object *dest; }; /* private */
struct  MUIP_KillNotify                     { ULONG MethodID; ULONG TrigAttr; };
struct  MUIP_KillNotifyObj                  { ULONG MethodID; ULONG TrigAttr; Object *dest; };
struct  MUIP_MultiSet                       { ULONG MethodID; ULONG attr; ULONG val; APTR obj; /* ... */ };
struct  MUIP_NoNotifySet                    { ULONG MethodID; ULONG attr; char *format; ULONG val; /* ... */ };
struct  MUIP_Notify                         { ULONG MethodID; ULONG TrigAttr; ULONG TrigVal; APTR DestObj; ULONG FollowParams; /* ... */ };
struct  MUIP_Set                            { ULONG MethodID; ULONG attr; ULONG val; };
struct  MUIP_SetAsString                    { ULONG MethodID; ULONG attr; char *format; ULONG val; /* ... */ };
struct  MUIP_SetUData                       { ULONG MethodID; ULONG udata; ULONG attr; ULONG val; };
struct  MUIP_SetUDataOnce                   { ULONG MethodID; ULONG udata; ULONG attr; ULONG val; };
struct  MUIP_WriteLong                      { ULONG MethodID; ULONG val; ULONG *memory; };
struct  MUIP_WriteString                    { ULONG MethodID; char *str; char *memory; };

/* Attributes */

#define MUIA_ApplicationObject              0x8042d3ee /* V4  ..g Object *          */
#define MUIA_AppMessage                     0x80421955 /* V5  ..g struct AppMessage * */
#define MUIA_HelpFile                       0x80423a6e /* V4  isg STRPTR            */ /* private */
#define MUIA_HelpLine                       0x8042a825 /* V4  isg LONG              */
#define MUIA_HelpNode                       0x80420b85 /* V4  isg STRPTR            */
#define MUIA_NoNotify                       0x804237f9 /* V7  .s. BOOL              */
#define MUIA_ObjectID                       0x8042d76e /* V11 isg ULONG             */
#define MUIA_Parent                         0x8042e35f /* V11 ..g Object *          */
#define MUIA_Revision                       0x80427eaa /* V4  ..g LONG              */
#define MUIA_UserData                       0x80420313 /* V4  isg ULONG             */
#define MUIA_Version                        0x80422301 /* V4  ..g LONG              */



/****************************************************************************/
/** Family                                                                 **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Family[];
#else
#define MUIC_Family "Family.mui"
#endif

/* Methods */

#define MUIM_Family_AddHead                 0x8042e200 /* V8  */
#define MUIM_Family_AddTail                 0x8042d752 /* V8  */
#define MUIM_Family_Insert                  0x80424d34 /* V8  */
#define MUIM_Family_MutualExclude           0x8042c399 /* private */ /* V9  */
#define MUIM_Family_Remove                  0x8042f8a9 /* V8  */
#define MUIM_Family_Sort                    0x80421c49 /* V8  */
#define MUIM_Family_Transfer                0x8042c14a /* V8  */
struct  MUIP_Family_AddHead                 { ULONG MethodID; Object *obj; };
struct  MUIP_Family_AddTail                 { ULONG MethodID; Object *obj; };
struct  MUIP_Family_Insert                  { ULONG MethodID; Object *obj; Object *pred; };
struct  MUIP_Family_MutualExclude           { ULONG MethodID; Object *originator; ULONG exclude; }; /* private */
struct  MUIP_Family_Remove                  { ULONG MethodID; Object *obj; };
struct  MUIP_Family_Sort                    { ULONG MethodID; Object *obj[1]; };
struct  MUIP_Family_Transfer                { ULONG MethodID; Object *family; };

/* Attributes */

#define MUIA_Family_Child                   0x8042c696 /* V8  i.. Object *          */
#define MUIA_Family_List                    0x80424b9e /* V8  ..g struct MinList *  */



/****************************************************************************/
/** Menustrip                                                              **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Menustrip[];
#else
#define MUIC_Menustrip "Menustrip.mui"
#endif

/* Methods */

#define MUIM_Menustrip_CreatePopmenuList    0x8042b4c5 /* private */ /* V20 */
#define MUIM_Menustrip_DeletePopmenuList    0x80421c0e /* private */ /* V20 */
#define MUIM_Menustrip_ExitChange           0x8042ce4d /* V20 */
#define MUIM_Menustrip_FindShortcut         0x8042d045 /* private */ /* V20 */
#define MUIM_Menustrip_InitChange           0x8042dcd9 /* V20 */
#define MUIM_Menustrip_TriggerPopmenu       0x80420859 /* private */ /* V20 */
#define MUIM_Menustrip_Update               0x8042ed51 /* private */ /* V8  */
struct  MUIP_Menustrip_CreatePopmenuList    { ULONG MethodID; Object *singlemenu; ULONG flags; }; /* private */
struct  MUIP_Menustrip_DeletePopmenuList    { ULONG MethodID; struct MUIP_Popmenu_Item *pmi; }; /* private */
struct  MUIP_Menustrip_ExitChange           { ULONG MethodID; };
struct  MUIP_Menustrip_FindShortcut         { ULONG MethodID; STRPTR shortcut; }; /* private */
struct  MUIP_Menustrip_InitChange           { ULONG MethodID; };
struct  MUIP_Menustrip_TriggerPopmenu       { ULONG MethodID; Object *menuitem; struct MUIP_Popmenu_Item *pmi; Object *winobj; }; /* private */
struct  MUIP_Menustrip_Update               { ULONG MethodID; Object *item; int level; }; /* private */

/* Attributes */

#define MUIA_Menustrip_ChangeID             0x804250ec /* V20 ... LONG              */ /* private */
#define MUIA_Menustrip_Enabled              0x8042815b /* V8  isg BOOL              */
#define MUIA_Menustrip_Updated              0x80424f8b /* V20 ... LONG              */ /* private */



/****************************************************************************/
/** Menu                                                                   **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Menu[];
#else
#define MUIC_Menu "Menu.mui"
#endif

/* Attributes */

#define MUIA_Menu_Enabled                   0x8042ed48 /* V8  isg BOOL              */
#define MUIA_Menu_Title                     0x8042a0e3 /* V8  isg STRPTR            */



/****************************************************************************/
/** Menuitem                                                               **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Menuitem[];
#else
#define MUIC_Menuitem "Menuitem.mui"
#endif

/* Methods */

#define MUIM_Menuitem_FillPopmenu           0x8042fbcf /* private */ /* V20 */
#define MUIM_Menuitem_WriteStructure        0x804241d4 /* private */ /* V8  */
struct  MUIP_Menuitem_FillPopmenu           { ULONG MethodID; APTR pool; struct MUIP_Popmenu_Item *pmi; }; /* private */
struct  MUIP_Menuitem_WriteStructure        { ULONG MethodID; struct MenuItem *mi; }; /* private */

/* Attributes */

#define MUIA_Menuitem_Checked               0x8042562a /* V8  isg BOOL              */
#define MUIA_Menuitem_Checkit               0x80425ace /* V8  isg BOOL              */
#define MUIA_Menuitem_CommandString         0x8042b9cc /* V16 isg BOOL              */
#define MUIA_Menuitem_CopyStrings           0x8042dc1b /* V16 i.. BOOL              */ /* private */
#define MUIA_Menuitem_Enabled               0x8042ae0f /* V8  isg BOOL              */
#define MUIA_Menuitem_Exclude               0x80420bc6 /* V8  isg LONG              */
#define MUIA_Menuitem_Shortcut              0x80422030 /* V8  isg STRPTR            */
#define MUIA_Menuitem_Title                 0x804218be /* V8  isg STRPTR            */
#define MUIA_Menuitem_Toggle                0x80424d5c /* V8  isg BOOL              */
#define MUIA_Menuitem_Trigger               0x80426f32 /* V8  ..g struct MenuItem * */

#define MUIV_Menuitem_Shortcut_Check -1


/****************************************************************************/
/** Application                                                            **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Application[];
#else
#define MUIC_Application "Application.mui"
#endif

/* Methods */

#define MUIM_Application_AboutMUI           0x8042d21d /* V14 */
#define MUIM_Application_Activate           0x8042a2e1 /* private */ /* V20 */
#define MUIM_Application_AddInputHandler    0x8042f099 /* V11 */
#define MUIM_Application_CheckRefresh       0x80424d68 /* V11 */
#define MUIM_Application_CloseConfigWindow  0x8042a08c /* private */ /* V11 */
#define MUIM_Application_ClosePublic        0x80429d2f /* private */ /* V4  */
#define MUIM_Application_ConfigNotify       0x8042fe91 /* private */ /* V11 */
#ifdef MUI_OBSOLETE
#define MUIM_Application_GetMenuCheck       0x8042c0a7 /* V4  */
#endif /* MUI_OBSOLETE */
#ifdef MUI_OBSOLETE
#define MUIM_Application_GetMenuState       0x8042a58f /* V4  */
#endif /* MUI_OBSOLETE */
#define MUIM_Application_IdentifyLayer      0x8042f5e1 /* private */ /* V11 */
#define MUIM_Application_ImageObject        0x8042ba68 /* private */ /* V11 */
#ifdef MUI_OBSOLETE
#define MUIM_Application_Input              0x8042d0f5 /* V4  */
#endif /* MUI_OBSOLETE */
#define MUIM_Application_InputBuffered      0x80427e59 /* V4  */
#define MUIM_Application_KillPushMethod     0x80429954 /* private */ /* V15 */
#define MUIM_Application_Load               0x8042f90d /* V4  */
#define MUIM_Application_LoadConfig         0x8042c862 /* private */ /* V11 */
#define MUIM_Application_NewInput           0x80423ba6 /* V11 */
#define MUIM_Application_OpenConfigWindow   0x804299ba /* V11 */
#define MUIM_Application_OpenPublic         0x80426b99 /* private */ /* V4  */
#define MUIM_Application_PreConfig          0x80425e4b /* private */ /* V18 */
#define MUIM_Application_PushMethod         0x80429ef8 /* V4  */
#define MUIM_Application_RawKeyHandler      0x8042295f /* private */ /* V20 */
#define MUIM_Application_RefreshConfig      0x8042c58b /* private */ /* V11 */
#define MUIM_Application_RefreshScreen      0x804278d2 /* private */ /* V20 */
#define MUIM_Application_RemInputHandler    0x8042e7af /* V11 */
#define MUIM_Application_ReturnID           0x804276ef /* V4  */
#define MUIM_Application_Save               0x804227ef /* V4  */
#define MUIM_Application_SaveConfig         0x80420b19 /* private */ /* V11 */
#define MUIM_Application_ScreenNotify       0x8042c3c7 /* private */ /* V20 */
#define MUIM_Application_SetAppIcon         0x8042c5b9 /* private */ /* V18 */
#define MUIM_Application_SetConfigItem      0x80424a80 /* V11 */
#ifdef MUI_OBSOLETE
#define MUIM_Application_SetMenuCheck       0x8042a707 /* V4  */
#endif /* MUI_OBSOLETE */
#ifdef MUI_OBSOLETE
#define MUIM_Application_SetMenuState       0x80428bef /* V4  */
#endif /* MUI_OBSOLETE */
#define MUIM_Application_SetPushMethodDelay 0x80423713 /* private */ /* V20 */
#define MUIM_Application_ShowHelp           0x80426479 /* V4  */
#define MUIM_Application_SnapshotWindow     0x8042df8a /* private */ /* V11 */
#define MUIM_Application_SyncRexx           0x80426e36 /* private */ /* V10 */
#define MUIM_Application_WelcomeObject      0x80424a8b /* private */ /* V11 */
#define MUIM_Application_WindowCheckO       0x80423a3d /* private */ /* V4  */
#define MUIM_GetMenuCheckMark               0x804205b2 /* private */ /* V4  */
#define MUIM_GetMenuState                   0x80421526 /* private */ /* V4  */
#define MUIM_SetMenuCheckMark               0x804233d4 /* private */ /* V4  */
#define MUIM_SetMenuState                   0x80426697 /* private */ /* V4  */
struct  MUIP_Application_AboutMUI           { ULONG MethodID; Object *refwindow; };
struct  MUIP_Application_Activate           { ULONG MethodID; }; /* private */
struct  MUIP_Application_AddInputHandler    { ULONG MethodID; struct MUI_InputHandlerNode *ihnode; };
struct  MUIP_Application_CheckRefresh       { ULONG MethodID; };
struct  MUIP_Application_CloseConfigWindow  { ULONG MethodID; ULONG flags; }; /* private */
struct  MUIP_Application_ClosePublic        { ULONG MethodID; }; /* private */
struct  MUIP_Application_ConfigNotify       { ULONG MethodID; }; /* private */
struct  MUIP_Application_GetMenuCheck       { ULONG MethodID; ULONG MenuID; };
struct  MUIP_Application_GetMenuState       { ULONG MethodID; ULONG MenuID; };
struct  MUIP_Application_IdentifyLayer      { ULONG MethodID; struct Layer *layer; }; /* private */
struct  MUIP_Application_ImageObject        { ULONG MethodID; LONG width; LONG height; }; /* private */
struct  MUIP_Application_Input              { ULONG MethodID; ULONG *signal; };
struct  MUIP_Application_InputBuffered      { ULONG MethodID; };
struct  MUIP_Application_KillPushMethod     { ULONG MethodID; Object *o; ULONG id; }; /* private */
struct  MUIP_Application_Load               { ULONG MethodID; STRPTR name; };
struct  MUIP_Application_LoadConfig         { ULONG MethodID; char *name; ULONG flags; }; /* private */
struct  MUIP_Application_NewInput           { ULONG MethodID; ULONG *signal; };
struct  MUIP_Application_OpenConfigWindow   { ULONG MethodID; ULONG flags; STRPTR classid; };
struct  MUIP_Application_OpenPublic         { ULONG MethodID; }; /* private */
struct  MUIP_Application_PreConfig          { ULONG MethodID; Object *configdata; }; /* private */
struct  MUIP_Application_PushMethod         { ULONG MethodID; Object *dest; LONG count; /* ... */ };
struct  MUIP_Application_RawKeyHandler      { ULONG MethodID; struct IntuiMessage *imsg; }; /* private */
struct  MUIP_Application_RefreshConfig      { ULONG MethodID; }; /* private */
struct  MUIP_Application_RefreshScreen      { ULONG MethodID; }; /* private */
struct  MUIP_Application_RemInputHandler    { ULONG MethodID; struct MUI_InputHandlerNode *ihnode; };
struct  MUIP_Application_ReturnID           { ULONG MethodID; ULONG retid; };
struct  MUIP_Application_Save               { ULONG MethodID; STRPTR name; };
struct  MUIP_Application_SaveConfig         { ULONG MethodID; char *name; ULONG flags; }; /* private */
struct  MUIP_Application_ScreenNotify       { ULONG MethodID; struct MUI_PubScreenDesc *desc; }; /* private */
struct  MUIP_Application_SetAppIcon         { ULONG MethodID; LONG x; LONG y; struct DiskObject *dobj; char *title; }; /* private */
struct  MUIP_Application_SetConfigItem      { ULONG MethodID; ULONG item; APTR data; };
struct  MUIP_Application_SetMenuCheck       { ULONG MethodID; ULONG MenuID; LONG stat; };
struct  MUIP_Application_SetMenuState       { ULONG MethodID; ULONG MenuID; LONG stat; };
struct  MUIP_Application_SetPushMethodDelay { ULONG MethodID; ULONG id; LONG millisecs; }; /* private */
struct  MUIP_Application_ShowHelp           { ULONG MethodID; Object *window; char *name; char *node; LONG line; };
struct  MUIP_Application_SnapshotWindow     { ULONG MethodID; char *filename; ULONG id; ULONG flags; }; /* private */
struct  MUIP_Application_SyncRexx           { ULONG MethodID; char *command; }; /* private */
struct  MUIP_Application_WelcomeObject      { ULONG MethodID; }; /* private */
struct  MUIP_Application_WindowCheckO       { ULONG MethodID; Object *win; }; /* private */
struct  MUIP_GetMenuCheckMark               { ULONG MethodID; ULONG MenuID; }; /* private */
struct  MUIP_GetMenuState                   { ULONG MethodID; ULONG MenuID; }; /* private */
struct  MUIP_SetMenuCheckMark               { ULONG MethodID; ULONG MenuID; LONG stat; }; /* private */
struct  MUIP_SetMenuState                   { ULONG MethodID; ULONG MenuID; LONG stat; }; /* private */

/* Attributes */

#define MUIA_Application_Active             0x804260ab /* V4  isg BOOL              */
#define MUIA_Application_Author             0x80424842 /* V4  i.g STRPTR            */
#define MUIA_Application_Base               0x8042e07a /* V4  i.g STRPTR            */
#define MUIA_Application_Broker             0x8042dbce /* V4  ..g Broker *          */
#define MUIA_Application_BrokerHook         0x80428f4b /* V4  isg struct Hook *     */
#define MUIA_Application_BrokerPort         0x8042e0ad /* V6  ..g struct MsgPort *  */
#define MUIA_Application_BrokerPri          0x8042c8d0 /* V6  i.g LONG              */
#define MUIA_Application_Commands           0x80428648 /* V4  isg struct MUI_Command * */
#define MUIA_Application_Config             0x804261ca /* V4  ..g struct MUI_Configuration * */ /* private */
#define MUIA_Application_Copyright          0x8042ef4d /* V4  i.g STRPTR            */
#define MUIA_Application_Data               0x80429dda /* V4  ..g APTR              */ /* private */
#define MUIA_Application_DefConfig          0x8042c8af /* V9  i.. struct MUI_Configuration * */ /* private */
#define MUIA_Application_DefConfigHook      0x804270a3 /* V9  i.. struct Hook *     */ /* private */
#define MUIA_Application_DefConfigSize      0x8042e0c1 /* V9  i.. LONG              */ /* private */
#define MUIA_Application_Description        0x80421fc6 /* V4  i.g STRPTR            */
#define MUIA_Application_DiskObject         0x804235cb /* V4  isg struct DiskObject * */
#define MUIA_Application_DoubleStart        0x80423bc6 /* V4  ..g BOOL              */
#define MUIA_Application_DropObject         0x80421266 /* V5  is. Object *          */
#define MUIA_Application_ForceQuit          0x804257df /* V8  ..g BOOL              */
#define MUIA_Application_HelpFile           0x804293f4 /* V8  isg STRPTR            */
#define MUIA_Application_Iconified          0x8042a07f /* V4  .sg BOOL              */
#define MUIA_Application_IconifyTitle       0x80422cb8 /* V18 isg STRPTR            */ /* private */
#define MUIA_Application_LightSleep         0x80423a69 /* V4  .s. BOOL              */ /* private */
#ifdef MUI_OBSOLETE
#define MUIA_Application_Menu               0x80420e1f /* V4  i.g struct NewMenu *  */
#endif /* MUI_OBSOLETE */
#define MUIA_Application_MenuAction         0x80428961 /* V4  ..g ULONG             */
#define MUIA_Application_MenuHelp           0x8042540b /* V4  ..g ULONG             */
#define MUIA_Application_Menustrip          0x804252d9 /* V8  i.. Object *          */
#define MUIA_Application_MsgPort            0x8042d4ef /* V4  ..g struct MsgPort *  */ /* private */
#define MUIA_Application_Originator         0x80424cda /* V16 ..g Object *          */ /* private */
#define MUIA_Application_Package            0x804248a3 /* V18 i.. ULONG             */ /* private */
#define MUIA_Application_PubScreenName      0x80427690 /* V4  ..g char *            */ /* private */
#define MUIA_Application_RexxHook           0x80427c42 /* V7  isg struct Hook *     */
#define MUIA_Application_RexxMsg            0x8042fd88 /* V4  ..g struct RxMsg *    */
#define MUIA_Application_RexxString         0x8042d711 /* V4  .s. STRPTR            */
#define MUIA_Application_SingleTask         0x8042a2c8 /* V4  i.. BOOL              */
#define MUIA_Application_Sleep              0x80425711 /* V4  .s. BOOL              */
#define MUIA_Application_Title              0x804281b8 /* V4  i.g STRPTR            */
#define MUIA_Application_UseCommodities     0x80425ee5 /* V10 i.. BOOL              */
#define MUIA_Application_UsedClasses        0x8042e9a7 /* V20 isg STRPTR *          */
#define MUIA_Application_UseRexx            0x80422387 /* V10 i.. BOOL              */
#define MUIA_Application_Version            0x8042b33f /* V4  i.g STRPTR            */
#define MUIA_Application_Window             0x8042bfe0 /* V4  i.. Object *          */
#define MUIA_Application_WindowList         0x80429abe /* V13 ..g struct List *     */

#define MUIV_Application_Package_NetConnect 0xa3ff7b49


/****************************************************************************/
/** Window                                                                 **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Window[];
#else
#define MUIC_Window "Window.mui"
#endif

/* Methods */

#define MUIM_Window_ActionIconify           0x80422cc0 /* private */ /* V18 */
#define MUIM_Window_AddEventHandler         0x804203b7 /* V16 */
#define MUIM_Window_CalcMinMax              0x80427c9b /* private */ /* V11 */
#define MUIM_Window_CheckRefresh            0x80426771 /* private */ /* V11 */
#define MUIM_Window_Cleanup                 0x8042ab26 /* V18 */
#ifdef MUI_OBSOLETE
#define MUIM_Window_GetMenuCheck            0x80420414 /* V4  */
#endif /* MUI_OBSOLETE */
#ifdef MUI_OBSOLETE
#define MUIM_Window_GetMenuState            0x80420d2f /* V4  */
#endif /* MUI_OBSOLETE */
#define MUIM_Window_HandleRMB               0x80421786 /* private */ /* V20 */
#define MUIM_Window_HandleWinborderMenu     0x80427358 /* private */ /* V20 */
#define MUIM_Window_HideBubble              0x8042174e /* private */ /* V13 */
#define MUIM_Window_Input                   0x80426a85 /* private */ /* V4  */
#define MUIM_Window_LockMode                0x80423e12 /* private */ /* V20 */
#define MUIM_Window_OlliHack                0x8042926f /* private */ /* V18 */
#define MUIM_Window_Recalc                  0x80425ebe /* private */ /* V4  */
#define MUIM_Window_RemEventHandler         0x8042679e /* V16 */
#define MUIM_Window_ResyncBubbles           0x80426688 /* private */ /* V20 */
#define MUIM_Window_ScreenToBack            0x8042913d /* V4  */
#define MUIM_Window_ScreenToFront           0x804227a4 /* V4  */
#ifdef MUI_OBSOLETE
#define MUIM_Window_SetCycleChain           0x80426510 /* V4  */
#endif /* MUI_OBSOLETE */
#ifdef MUI_OBSOLETE
#define MUIM_Window_SetMenuCheck            0x80422243 /* V4  */
#endif /* MUI_OBSOLETE */
#ifdef MUI_OBSOLETE
#define MUIM_Window_SetMenuState            0x80422b5e /* V4  */
#endif /* MUI_OBSOLETE */
#define MUIM_Window_Setup                   0x8042c34c /* V18 */
#define MUIM_Window_ShowBubble              0x8042671f /* private */ /* V11 */
#define MUIM_Window_ShowHelp                0x80424240 /* private */ /* V11 */
#define MUIM_Window_Snapshot                0x8042945e /* V11 */
#define MUIM_Window_ToBack                  0x8042152e /* V4  */
#define MUIM_Window_ToFront                 0x8042554f /* V4  */
struct  MUIP_Window_ActionIconify           { ULONG MethodID; }; /* private */
struct  MUIP_Window_AddEventHandler         { ULONG MethodID; struct MUI_EventHandlerNode *ehnode; };
struct  MUIP_Window_CalcMinMax              { ULONG MethodID; Object *special; }; /* private */
struct  MUIP_Window_CheckRefresh            { ULONG MethodID; }; /* private */
struct  MUIP_Window_Cleanup                 { ULONG MethodID; };
struct  MUIP_Window_GetMenuCheck            { ULONG MethodID; ULONG MenuID; };
struct  MUIP_Window_GetMenuState            { ULONG MethodID; ULONG MenuID; };
struct  MUIP_Window_HandleRMB               { ULONG MethodID; struct IntuiMessage *imsg; }; /* private */
struct  MUIP_Window_HandleWinborderMenu     { ULONG MethodID; }; /* private */
struct  MUIP_Window_HideBubble              { ULONG MethodID; }; /* private */
struct  MUIP_Window_Input                   { ULONG MethodID; struct IntuiMessage *imsg; LONG muikey; }; /* private */
struct  MUIP_Window_LockMode                { ULONG MethodID; BOOL onoff; }; /* private */
struct  MUIP_Window_OlliHack                { ULONG MethodID; Object *newwin; }; /* private */
struct  MUIP_Window_Recalc                  { ULONG MethodID; }; /* private */
struct  MUIP_Window_RemEventHandler         { ULONG MethodID; struct MUI_EventHandlerNode *ehnode; };
struct  MUIP_Window_ResyncBubbles           { ULONG MethodID; }; /* private */
struct  MUIP_Window_ScreenToBack            { ULONG MethodID; };
struct  MUIP_Window_ScreenToFront           { ULONG MethodID; };
struct  MUIP_Window_SetCycleChain           { ULONG MethodID; Object *obj[1]; };
struct  MUIP_Window_SetMenuCheck            { ULONG MethodID; ULONG MenuID; LONG stat; };
struct  MUIP_Window_SetMenuState            { ULONG MethodID; ULONG MenuID; LONG stat; };
struct  MUIP_Window_Setup                   { ULONG MethodID; };
struct  MUIP_Window_ShowBubble              { ULONG MethodID; Object *mo; LONG x; LONG y; }; /* private */
struct  MUIP_Window_ShowHelp                { ULONG MethodID; }; /* private */
struct  MUIP_Window_Snapshot                { ULONG MethodID; LONG flags; };
struct  MUIP_Window_ToBack                  { ULONG MethodID; };
struct  MUIP_Window_ToFront                 { ULONG MethodID; };

/* Attributes */

#define MUIA_Window_Activate                0x80428d2f /* V4  isg BOOL              */
#define MUIA_Window_ActivationPriority      0x8042b7a9 /* V20 isg ULONG             */ /* private */
#define MUIA_Window_ActiveObject            0x80427925 /* V4  .sg Object *          */
#define MUIA_Window_AlphaValue              0x80421dfa /* V20 isg ULONG             */ /* private */
#define MUIA_Window_AltHeight               0x8042cce3 /* V4  i.g LONG              */
#define MUIA_Window_AltLeftEdge             0x80422d65 /* V4  i.g LONG              */
#define MUIA_Window_AltTopEdge              0x8042e99b /* V4  i.g LONG              */
#define MUIA_Window_AltWidth                0x804260f4 /* V4  i.g LONG              */
#define MUIA_Window_Application             0x80428dbb /* V11 ..g Object *          */ /* private */
#define MUIA_Window_AppWindow               0x804280cf /* V5  i.. BOOL              */
#define MUIA_Window_Backdrop                0x8042c0bb /* V4  i.. BOOL              */
#define MUIA_Window_BackfillHook            0x80428863 /* V20 i.. struct Hook *     */
#define MUIA_Window_Background              0x804253aa /* V4  i.. char *            */ /* private */
#define MUIA_Window_Borderless              0x80429b79 /* V4  i.. BOOL              */
#define MUIA_Window_CloseGadget             0x8042a110 /* V4  i.. BOOL              */
#define MUIA_Window_CloseRequest            0x8042e86e /* V4  ..g BOOL              */
#define MUIA_Window_DefaultObject           0x804294d7 /* V4  isg Object *          */
#define MUIA_Window_DepthGadget             0x80421923 /* V4  i.. BOOL              */
#define MUIA_Window_DisableKeys             0x80424c36 /* V15 isg ULONG             */
#define MUIA_Window_DoClose                 0x8042954a /* V4  .s. BOOL              */ /* private */
#define MUIA_Window_DragBar                 0x8042045d /* V4  i.. BOOL              */
#define MUIA_Window_DrawInfo                0x80423726 /* V4  ..g ULONG             */ /* private */
#define MUIA_Window_FancyDrawing            0x8042bd0e /* V8  isg BOOL              */
#define MUIA_Window_FillBackDraw            0x80427028 /* V20 isg BOOL              */ /* private */
#define MUIA_Window_FixFocus                0x8042f814 /* V13 isg Object *          */ /* private */
#define MUIA_Window_Frontdrop               0x80426411 /* V20 isg BOOL              */ /* private */
#define MUIA_Window_Height                  0x80425846 /* V4  i.g LONG              */
#define MUIA_Window_ID                      0x804201bd /* V4  isg ULONG             */
#define MUIA_Window_InputEvent              0x804247d8 /* V4  ..g struct InputEvent * */
#define MUIA_Window_IsPopup                 0x8042b5aa /* V4  i.g BOOL              */ /* private */
#define MUIA_Window_IsSubWindow             0x8042b5aa /* V4  isg BOOL              */
#define MUIA_Window_LeftEdge                0x80426c65 /* V4  i.g LONG              */
#define MUIA_Window_LightSleep              0x8042be05 /* V4  .s. BOOL              */ /* private */
#ifdef MUI_OBSOLETE
#define MUIA_Window_Menu                    0x8042db94 /* V4  i.. struct NewMenu *  */
#endif /* MUI_OBSOLETE */
#define MUIA_Window_MenuAction              0x80427521 /* V8  isg ULONG             */
#define MUIA_Window_Menustrip               0x8042855e /* V8  i.g Object *          */
#define MUIA_Window_MouseEnabled            0x80429b2d /* V20 i.. BOOL              */ /* private */
#define MUIA_Window_MouseObject             0x8042bf9b /* V10 ..g Object *          */
#define MUIA_Window_NeedsMouseObject        0x8042372a /* V10 i.. BOOL              */
#define MUIA_Window_NeverActive             0x8042290a /* V20 isg BOOL              */ /* private */
#define MUIA_Window_NoMenus                 0x80429df5 /* V4  is. BOOL              */
#define MUIA_Window_Open                    0x80428aa0 /* V4  .sg BOOL              */
#define MUIA_Window_PublicScreen            0x804278e4 /* V6  isg STRPTR            */
#define MUIA_Window_RefWindow               0x804201f4 /* V4  is. Object *          */
#define MUIA_Window_Remember                0x8042f6b6 /* V4  i.. BOOL              */ /* private */
#define MUIA_Window_RootObject              0x8042cba5 /* V4  isg Object *          */
#define MUIA_Window_Screen                  0x8042df4f /* V4  isg struct Screen *   */
#define MUIA_Window_ScreenTitle             0x804234b0 /* V5  isg STRPTR            */
#define MUIA_Window_ShowAbout               0x80429c1e /* V11 isg BOOL              */ /* private */
#define MUIA_Window_ShowIconify             0x8042bc26 /* V11 isg BOOL              */ /* private */
#define MUIA_Window_ShowJump                0x80422f40 /* V11 isg BOOL              */ /* private */
#define MUIA_Window_ShowPopup               0x8042324e /* V11 isg BOOL              */ /* private */
#define MUIA_Window_ShowPrefs               0x8042e262 /* V11 isg BOOL              */ /* private */
#define MUIA_Window_ShowSnapshot            0x80423c55 /* V11 isg BOOL              */ /* private */
#define MUIA_Window_SizeGadget              0x8042e33d /* V4  i.. BOOL              */
#define MUIA_Window_SizeRight               0x80424780 /* V4  i.. BOOL              */
#define MUIA_Window_Sleep                   0x8042e7db /* V4  .sg BOOL              */
#define MUIA_Window_Timeout                 0x80425d29 /* V20 isg BOOL              */ /* private */
#define MUIA_Window_Title                   0x8042ad3d /* V4  isg STRPTR            */
#define MUIA_Window_TopEdge                 0x80427c66 /* V4  i.g LONG              */
#define MUIA_Window_TopMenuPos              0x8042861b /* V20 isg LONG              */ /* private */
#define MUIA_Window_Transparency            0x8042ac47 /* V20 isg ULONG             */ /* private */
#define MUIA_Window_UseBottomBorderScroller 0x80424e79 /* V13 isg BOOL              */
#define MUIA_Window_UseLeftBorderScroller   0x8042433e /* V13 isg BOOL              */
#define MUIA_Window_UseRightBorderScroller  0x8042c05e /* V13 isg BOOL              */
#define MUIA_Window_Width                   0x8042dcae /* V4  i.g LONG              */
#define MUIA_Window_Window                  0x80426a42 /* V4  ..g struct Window *   */

#define MUIV_Window_ActiveObject_None 0
#define MUIV_Window_ActiveObject_Next -1
#define MUIV_Window_ActiveObject_Prev -2
#define MUIV_Window_ActiveObject_Left -3
#define MUIV_Window_ActiveObject_Right -4
#define MUIV_Window_ActiveObject_Up -5
#define MUIV_Window_ActiveObject_Down -6
#define MUIV_Window_AltHeight_MinMax(p) (0-(p))
#define MUIV_Window_AltHeight_Visible(p) (-100-(p))
#define MUIV_Window_AltHeight_Screen(p) (-200-(p))
#define MUIV_Window_AltHeight_Scaled -1000
#define MUIV_Window_AltLeftEdge_Centered -1
#define MUIV_Window_AltLeftEdge_Moused -2
#define MUIV_Window_AltLeftEdge_NoChange -1000
#define MUIV_Window_AltTopEdge_Centered -1
#define MUIV_Window_AltTopEdge_Moused -2
#define MUIV_Window_AltTopEdge_Delta(p) (-3-(p))
#define MUIV_Window_AltTopEdge_NoChange -1000
#define MUIV_Window_AltWidth_MinMax(p) (0-(p))
#define MUIV_Window_AltWidth_Visible(p) (-100-(p))
#define MUIV_Window_AltWidth_Screen(p) (-200-(p))
#define MUIV_Window_AltWidth_Scaled -1000
#define MUIV_Window_Height_MinMax(p) (0-(p))
#define MUIV_Window_Height_Visible(p) (-100-(p))
#define MUIV_Window_Height_Screen(p) (-200-(p))
#define MUIV_Window_Height_Scaled -1000
#define MUIV_Window_Height_Default -1001
#define MUIV_Window_LeftEdge_Centered -1
#define MUIV_Window_LeftEdge_Moused -2
#define MUIV_Window_LeftEdge_Right(n) (-1000-(n))
#ifdef MUI_OBSOLETE
#define MUIV_Window_Menu_NoMenu -1
#endif /* MUI_OBSOLETE */
#define MUIV_Window_Timeout_Off 0
#define MUIV_Window_Timeout_Restart -1
#define MUIV_Window_TopEdge_Centered -1
#define MUIV_Window_TopEdge_Moused -2
#define MUIV_Window_TopEdge_Delta(p) (-3-(p))
#define MUIV_Window_TopEdge_Bottom(n) (-1000-(n))
#define MUIV_Window_TopMenuPos_Off 0
#define MUIV_Window_TopMenuPos_Top 1
#define MUIV_Window_TopMenuPos_Bottom 2
#define MUIV_Window_TopMenuPos_Left 3
#define MUIV_Window_TopMenuPos_Right 4
#define MUIV_Window_Width_MinMax(p) (0-(p))
#define MUIV_Window_Width_Visible(p) (-100-(p))
#define MUIV_Window_Width_Screen(p) (-200-(p))
#define MUIV_Window_Width_Scaled -1000
#define MUIV_Window_Width_Default -1001


/****************************************************************************/
/** Aboutmui                                                               **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Aboutmui[];
#else
#define MUIC_Aboutmui "Aboutmui.mui"
#endif

/* Methods */

#define MUIM_Aboutwindow_Register           0x8042a312 /* private */ /* V11 */
struct  MUIP_Aboutwindow_Register           { ULONG MethodID; }; /* private */

/* Attributes */

#define MUIA_Aboutmui_Application           0x80422523 /* V11 i.. Object *          */



/****************************************************************************/
/** Area                                                                   **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Area[];
#else
#define MUIC_Area "Area.mui"
#endif

/* Methods */

#define MUIM_AdjustLayout                   0x80424d50 /* private */ /* V11 */
#define MUIM_AskMinMax                      0x80423874 /* Custom Class */ /* V4  */
#define MUIM_Backfill                       0x80428d73 /* private */ /* V11 */
#define MUIM_CheckShortHelp                 0x80423c79 /* V20 */
#define MUIM_Cleanup                        0x8042d985 /* Custom Class */ /* V4  */
#define MUIM_ContextMenuBuild               0x80429d2e /* V11 */
#define MUIM_ContextMenuChoice              0x80420f0e /* V11 */
#define MUIM_CreateBitMapBuffer             0x80424f05 /* private */ /* V18 */
#define MUIM_CreateBubble                   0x80421c41 /* V18 */
#define MUIM_CreateDragImage                0x8042eb6f /* private */ /* V18 */
#define MUIM_CreateShortHelp                0x80428e93 /* V11 */
#define MUIM_DeleteBitMapBuffer             0x80428daf /* private */ /* V18 */
#define MUIM_DeleteBubble                   0x804211af /* V18 */
#define MUIM_DeleteDragImage                0x80423037 /* private */ /* V18 */
#define MUIM_DeleteShortHelp                0x8042d35a /* V11 */
#define MUIM_Disable                        0x8042af9f /* private */ /* V11 */
#define MUIM_DoDrag                         0x804216bb /* V20 */
#define MUIM_DragBegin                      0x8042c03a /* V11 */
#define MUIM_DragDrop                       0x8042c555 /* V11 */
#define MUIM_DragFinish                     0x804251f0 /* V11 */
#define MUIM_DragQuery                      0x80420261 /* V11 */
#define MUIM_DragReport                     0x8042edad /* V11 */
#define MUIM_Draw                           0x80426f3f /* Custom Class */ /* V4  */
#define MUIM_DrawBackground                 0x804238ca /* V11 */
#define MUIM_Enable                         0x80428f6c /* private */ /* V11 */
#define MUIM_GoActive                       0x8042491a /* private */ /* V8  */
#define MUIM_GoInactive                     0x80422c0c /* private */ /* V8  */
#define MUIM_HandleEvent                    0x80426d66 /* Custom Class */ /* V16 */
#define MUIM_HandleInput                    0x80422a1a /* Custom Class */ /* V4  */
#define MUIM_Hide                           0x8042f20f /* Custom Class */ /* V4  */
#define MUIM_Hittest                        0x8042f8a4 /* private */ /* V11 */
#define MUIM_KillFrame                      0x80421407 /* private */ /* V4  */
#define MUIM_Layout                         0x8042845b /* private */ /* V4  */
#define MUIM_Redraw                         0x8042fa0c /* private */ /* V4  */
#define MUIM_Setup                          0x80428354 /* Custom Class */ /* V4  */
#define MUIM_Show                           0x8042cc84 /* Custom Class */ /* V4  */
#define MUIM_WhichObject                    0x8042867c /* private */ /* V11 */
struct  MUIP_AdjustLayout                   { ULONG MethodID; LONG pass; }; /* private */
struct  MUIP_AskMinMax                      { ULONG MethodID; struct MUI_MinMax *MinMaxInfo; }; /* Custom Class */
struct  MUIP_Backfill                       { ULONG MethodID; LONG left; LONG top; LONG right; LONG bottom; LONG xoffset; LONG yoffset; }; /* private */
struct  MUIP_CheckShortHelp                 { ULONG MethodID; STRPTR help; LONG mx; LONG my; };
struct  MUIP_Cleanup                        { ULONG MethodID; }; /* Custom Class */
struct  MUIP_ContextMenuBuild               { ULONG MethodID; LONG mx; LONG my; };
struct  MUIP_ContextMenuChoice              { ULONG MethodID; Object *item; };
struct  MUIP_CreateBitMapBuffer             { ULONG MethodID; LONG *reswidth; LONG *resheight; ULONG flags; LONG clipleft; LONG cliptop; LONG clipwidth; LONG clipheight; LONG layoutwidth; LONG layoutheight; }; /* private */
struct  MUIP_CreateBubble                   { ULONG MethodID; LONG x; LONG y; char *txt; ULONG flags; };
struct  MUIP_CreateDragImage                { ULONG MethodID; LONG touchx; LONG touchy; ULONG flags; }; /* private */
struct  MUIP_CreateShortHelp                { ULONG MethodID; LONG mx; LONG my; };
struct  MUIP_DeleteBitMapBuffer             { ULONG MethodID; struct BitMap *bitmap; }; /* private */
struct  MUIP_DeleteBubble                   { ULONG MethodID; APTR bubble; };
struct  MUIP_DeleteDragImage                { ULONG MethodID; struct MUI_DragImage *di; }; /* private */
struct  MUIP_DeleteShortHelp                { ULONG MethodID; STRPTR help; };
struct  MUIP_Disable                        { ULONG MethodID; }; /* private */
struct  MUIP_DoDrag                         { ULONG MethodID; LONG touchx; LONG touchy; ULONG flags; };
struct  MUIP_DragBegin                      { ULONG MethodID; Object *obj; };
struct  MUIP_DragDrop                       { ULONG MethodID; Object *obj; LONG x; LONG y; };
struct  MUIP_DragFinish                     { ULONG MethodID; Object *obj; };
struct  MUIP_DragQuery                      { ULONG MethodID; Object *obj; };
struct  MUIP_DragReport                     { ULONG MethodID; Object *obj; LONG x; LONG y; LONG update; };
struct  MUIP_Draw                           { ULONG MethodID; ULONG flags; }; /* Custom Class */
struct  MUIP_DrawBackground                 { ULONG MethodID; LONG left; LONG top; LONG width; LONG height; LONG xoffset; LONG yoffset; LONG flags; };
struct  MUIP_Enable                         { ULONG MethodID; }; /* private */
struct  MUIP_GoActive                       { ULONG MethodID; ULONG flags; }; /* private */
struct  MUIP_GoInactive                     { ULONG MethodID; ULONG flags; }; /* private */
struct  MUIP_HandleEvent                    { ULONG MethodID; struct IntuiMessage *imsg; LONG muikey; }; /* Custom Class */
struct  MUIP_HandleInput                    { ULONG MethodID; struct IntuiMessage *imsg; LONG muikey; }; /* Custom Class */
struct  MUIP_Hide                           { ULONG MethodID; }; /* Custom Class */
struct  MUIP_Hittest                        { ULONG MethodID; LONG x; LONG y; }; /* private */
struct  MUIP_KillFrame                      { ULONG MethodID; }; /* private */
struct  MUIP_Layout                         { ULONG MethodID; LONG left; LONG top; LONG width; LONG height; ULONG flags; }; /* private */
struct  MUIP_Redraw                         { ULONG MethodID; }; /* private */
struct  MUIP_Setup                          { ULONG MethodID; struct MUI_RenderInfo *RenderInfo; }; /* Custom Class */
struct  MUIP_Show                           { ULONG MethodID; struct LongRect *clip; }; /* Custom Class */
struct  MUIP_WhichObject                    { ULONG MethodID; LONG x; LONG y; }; /* private */

/* Attributes */

#define MUIA_Background                     0x8042545b /* V4  is. LONG              */
#define MUIA_BottomEdge                     0x8042e552 /* V4  ..g LONG              */
#define MUIA_ContextMenu                    0x8042b704 /* V11 isg Object *          */
#define MUIA_ContextMenuTrigger             0x8042a2c1 /* V11 ..g Object *          */
#define MUIA_ControlChar                    0x8042120b /* V4  isg char              */
#define MUIA_CustomBackfill                 0x80420a63 /* V11 isg BOOL              */ /* private */
#define MUIA_CustomFont                     0x80423fc5 /* V20 i.. STRPTR            */ /* private */
#define MUIA_CycleChain                     0x80421ce7 /* V11 isg LONG              */
#define MUIA_DefHeight                      0x80427dd9 /* V4  ... LONG              */ /* private */
#define MUIA_DefWidth                       0x8042c7fe /* V4  ... LONG              */ /* private */
#define MUIA_Disabled                       0x80423661 /* V4  isg BOOL              */
#define MUIA_Draggable                      0x80420b6e /* V11 isg BOOL              */
#define MUIA_Dropable                       0x8042fbce /* V11 isg BOOL              */
#ifdef MUI_OBSOLETE
#define MUIA_ExportID                       0x8042d76e /* V4  isg ULONG             */
#endif /* MUI_OBSOLETE */
#define MUIA_FillArea                       0x804294a3 /* V4  is. BOOL              */
#define MUIA_FixHeight                      0x8042a92b /* V4  i.. LONG              */
#define MUIA_FixHeightTxt                   0x804276f2 /* V4  i.. STRPTR            */
#define MUIA_FixWidth                       0x8042a3f1 /* V4  i.. LONG              */
#define MUIA_FixWidthTxt                    0x8042d044 /* V4  i.. STRPTR            */
#define MUIA_Font                           0x8042be50 /* V4  i.g struct TextFont * */
#define MUIA_Frame                          0x8042ac64 /* V4  i.. LONG              */
#define MUIA_FrameDynamic                   0x804223c9 /* V20 isg BOOL              */
#define MUIA_FramePhantomHoriz              0x8042ed76 /* V4  i.. BOOL              */
#define MUIA_FrameTitle                     0x8042d1c7 /* V4  i.. STRPTR            */
#define MUIA_FrameVisible                   0x80426498 /* V20 isg BOOL              */
#define MUIA_Height                         0x80423237 /* V4  ..g LONG              */
#define MUIA_HorizDisappear                 0x80429615 /* V11 isg LONG              */
#define MUIA_HorizWeight                    0x80426db9 /* V4  isg WORD              */
#define MUIA_InnerBottom                    0x8042f2c0 /* V4  i.g LONG              */
#define MUIA_InnerLeft                      0x804228f8 /* V4  i.g LONG              */
#define MUIA_InnerRight                     0x804297ff /* V4  i.g LONG              */
#define MUIA_InnerTop                       0x80421eb6 /* V4  i.g LONG              */
#define MUIA_InputMode                      0x8042fb04 /* V4  i.. LONG              */
#define MUIA_LeftEdge                       0x8042bec6 /* V4  ..g LONG              */
#define MUIA_LeftOffset                     0x8042f7c3 /* V4  ..g LONG              */ /* private */
#define MUIA_MaxHeight                      0x804293e4 /* V11 i.. LONG              */
#define MUIA_MaxWidth                       0x8042f112 /* V11 i.. LONG              */
#define MUIA_Maybe                          0x80429705 /* V4  ... BOOL              */ /* private */
#define MUIA_NeedsLayoutMethod              0x80422e90 /* V20 .s. BOOL              */ /* private */
#define MUIA_ObjectType                     0x8042c070 /* V4  is. LONG              */ /* private */
#define MUIA_PageTitles                     0x80427219 /* V4  ... BOOL              */ /* private */
#define MUIA_Pressed                        0x80423535 /* V4  ..g BOOL              */
#define MUIA_RightEdge                      0x8042ba82 /* V4  ..g LONG              */
#define MUIA_Selected                       0x8042654b /* V4  isg BOOL              */
#define MUIA_ShortHelp                      0x80428fe3 /* V11 isg STRPTR            */
#define MUIA_ShowMe                         0x80429ba8 /* V4  isg BOOL              */
#define MUIA_ShowSelState                   0x8042caac /* V4  i.. BOOL              */
#define MUIA_Timer                          0x80426435 /* V4  ..g LONG              */
#define MUIA_TopEdge                        0x8042509b /* V4  ..g LONG              */
#define MUIA_TopOffset                      0x8042eb12 /* V4  ..g LONG              */ /* private */
#define MUIA_VertDisappear                  0x8042d12f /* V11 isg LONG              */
#define MUIA_VertWeight                     0x804298d0 /* V4  isg WORD              */
#define MUIA_Weight                         0x80421d1f /* V4  i.. WORD              */
#define MUIA_Width                          0x8042b59c /* V4  ..g LONG              */
#define MUIA_Window                         0x80421591 /* V4  ..g struct Window *   */
#define MUIA_WindowObject                   0x8042669e /* V4  ..g Object *          */

#define MUIV_Font_Inherit 0
#define MUIV_Font_Normal -1
#define MUIV_Font_List -2
#define MUIV_Font_Tiny -3
#define MUIV_Font_Fixed -4
#define MUIV_Font_Title -5
#define MUIV_Font_Big -6
#define MUIV_Font_Button -7
#define MUIV_Font_Slider -8
#define MUIV_Font_Gauge -9
#define MUIV_Font_Menudisplay -10
#define MUIV_Frame_None 0
#define MUIV_Frame_Button 1
#define MUIV_Frame_ImageButton 2
#define MUIV_Frame_Text 3
#define MUIV_Frame_String 4
#define MUIV_Frame_ReadList 5
#define MUIV_Frame_InputList 6
#define MUIV_Frame_Prop 7
#define MUIV_Frame_Gauge 8
#define MUIV_Frame_Group 9
#define MUIV_Frame_PopUp 10
#define MUIV_Frame_Virtual 11
#define MUIV_Frame_Slider 12
#define MUIV_Frame_SliderKnob 13
#define MUIV_Frame_GaugeInner 14
#define MUIV_Frame_Menudisplay 15
#define MUIV_Frame_MenudisplayMenu 16
#define MUIV_Frame_Count 17
#define MUIV_InputMode_None 0
#define MUIV_InputMode_RelVerify 1
#define MUIV_InputMode_Immediate 2
#define MUIV_InputMode_Toggle 3


/****************************************************************************/
/** Dtpic                                                                  **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Dtpic[];
#else
#define MUIC_Dtpic "Dtpic.mui"
#endif

/* Attributes */

#define MUIA_Dtpic_Name                     0x80423d72 /* V18 i.. STRPTR            */ /* private */



/****************************************************************************/
/** Rectangle                                                              **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Rectangle[];
#else
#define MUIC_Rectangle "Rectangle.mui"
#endif

/* Attributes */

#define MUIA_Rectangle_BarHalf              0x8042a6f4 /* V20 isg BOOL              */ /* private */
#define MUIA_Rectangle_BarInvert            0x8042f50d /* V20 isg BOOL              */ /* private */
#define MUIA_Rectangle_BarTitle             0x80426689 /* V11 i.g STRPTR            */
#define MUIA_Rectangle_HBar                 0x8042c943 /* V7  i.g BOOL              */
#define MUIA_Rectangle_VBar                 0x80422204 /* V7  i.g BOOL              */



/****************************************************************************/
/** Balance                                                                **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Balance[];
#else
#define MUIC_Balance "Balance.mui"
#endif

/* Attributes */

#define MUIA_Balance_Quiet                  0x80427486 /* V20 i.. LONG              */



/****************************************************************************/
/** Image                                                                  **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Image[];
#else
#define MUIC_Image "Image.mui"
#endif

/* Attributes */

#define MUIA_Image_FontMatch                0x8042815d /* V4  i.. BOOL              */
#define MUIA_Image_FontMatchHeight          0x80429f26 /* V4  i.. BOOL              */
#define MUIA_Image_FontMatchWidth           0x804239bf /* V4  i.. BOOL              */
#define MUIA_Image_FreeHoriz                0x8042da84 /* V4  i.. BOOL              */
#define MUIA_Image_FreeVert                 0x8042ea28 /* V4  i.. BOOL              */
#define MUIA_Image_OldImage                 0x80424f3d /* V4  i.. struct Image *    */
#define MUIA_Image_Scalable                 0x8042e966 /* V4  i.. BOOL              */ /* private */
#define MUIA_Image_Spec                     0x804233d5 /* V4  i.. char *            */
#define MUIA_Image_State                    0x8042a3ad /* V4  is. LONG              */



/****************************************************************************/
/** Menubar                                                                **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Menubar[];
#else
#define MUIC_Menubar "Menubar.mui"
#endif

/* Methods */

#define MUIM_Menubar_Attach                 0x80421010 /* private */ /* V20 */
#define MUIM_Menubar_Detach                 0x80425957 /* private */ /* V20 */
struct  MUIP_Menubar_Attach                 { ULONG MethodID; }; /* private */
struct  MUIP_Menubar_Detach                 { ULONG MethodID; }; /* private */

/* Attributes */

#define MUIA_Menubar_Strip                  0x8042a41f /* V20 isg Object *          */ /* private */



/****************************************************************************/
/** Bitmap                                                                 **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Bitmap[];
#else
#define MUIC_Bitmap "Bitmap.mui"
#endif

/* Attributes */

#define MUIA_Bitmap_Alpha                   0x80423e71 /* V20 isg ULONG             */
#define MUIA_Bitmap_Bitmap                  0x804279bd /* V8  isg struct BitMap *   */
#define MUIA_Bitmap_Height                  0x80421560 /* V8  isg LONG              */
#define MUIA_Bitmap_JPegData                0x8042f8dd /* V20 isg UBYTE *           */ /* private */
#define MUIA_Bitmap_JPegSize                0x8042bc4f /* V20 isg LONG              */ /* private */
#define MUIA_Bitmap_MappingTable            0x8042e23d /* V8  isg UBYTE *           */
#define MUIA_Bitmap_PNGData                 0x8042f4ba /* V20 isg UBYTE *           */ /* private */
#define MUIA_Bitmap_PNGSize                 0x8042a0ea /* V20 isg LONG              */ /* private */
#define MUIA_Bitmap_Precision               0x80420c74 /* V11 isg LONG              */
#define MUIA_Bitmap_RawData                 0x8042f61a /* V20 isg APTR              */
#define MUIA_Bitmap_RawDataFormat           0x80429b85 /* V20 isg ULONG             */
#define MUIA_Bitmap_RemappedBitmap          0x80423a47 /* V11 ..g struct BitMap *   */
#define MUIA_Bitmap_RemappedPens            0x804239e0 /* V20 ..g UBYTE *           */ /* private */
#define MUIA_Bitmap_SourceColors            0x80425360 /* V8  isg ULONG *           */
#define MUIA_Bitmap_Transparent             0x80422805 /* V8  isg LONG              */
#define MUIA_Bitmap_UseFriend               0x804239d8 /* V11 i.. BOOL              */
#define MUIA_Bitmap_Width                   0x8042eb3a /* V8  isg LONG              */
#define MUIA_Bitmap_XOffset                 0x8042077c /* V20 isg LONG              */ /* private */
#define MUIA_Bitmap_YOffset                 0x8042a4c3 /* V20 isg LONG              */ /* private */

#define MUIV_Bitmap_RawDataFormat_CLUT8 0
#define MUIV_Bitmap_RawDataFormat_RGB24 1
#define MUIV_Bitmap_RawDataFormat_ARGB32 2


/****************************************************************************/
/** Bodychunk                                                              **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Bodychunk[];
#else
#define MUIC_Bodychunk "Bodychunk.mui"
#endif

/* Attributes */

#define MUIA_Bodychunk_Body                 0x8042ca67 /* V8  isg UBYTE *           */
#define MUIA_Bodychunk_Compression          0x8042de5f /* V8  isg UBYTE             */
#define MUIA_Bodychunk_Depth                0x8042c392 /* V8  isg LONG              */
#define MUIA_Bodychunk_Masking              0x80423b0e /* V8  isg UBYTE             */



/****************************************************************************/
/** Text                                                                   **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Text[];
#else
#define MUIC_Text "Text.mui"
#endif

/* Attributes */

#define MUIA_Text_Array                     0x80428b9d /* V4  ... STRPTR *          */ /* private */
#define MUIA_Text_ArrayPos                  0x80428398 /* V4  ... LONG              */ /* private */
#define MUIA_Text_Contents                  0x8042f8dc /* V4  isg STRPTR            */
#define MUIA_Text_ControlChar               0x8042e6d0 /* V20 isg char              */
#define MUIA_Text_Copy                      0x80427727 /* V20 i.. BOOL              */
#define MUIA_Text_HiChar                    0x804218ff /* V4  i.. char              */
#define MUIA_Text_HiIndex                   0x804214f5 /* V11 i.. LONG              */ /* private */
#define MUIA_Text_PreParse                  0x8042566d /* V4  isg STRPTR            */
#define MUIA_Text_SetMax                    0x80424d0a /* V4  i.. BOOL              */
#define MUIA_Text_SetMin                    0x80424e10 /* V4  i.. BOOL              */
#define MUIA_Text_SetVMax                   0x80420d8b /* V11 i.. BOOL              */



/****************************************************************************/
/** Gadget                                                                 **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Gadget[];
#else
#define MUIC_Gadget "Gadget.mui"
#endif

/* Attributes */

#define MUIA_Gadget_Foreign                 0x804232d2 /* V13 .s. BOOL              */ /* private */
#define MUIA_Gadget_Gadget                  0x8042ec1a /* V11 ..g struct Gadget *   */
#define MUIA_Gadget_Window                  0x804282bc /* V11 ..g struct Gadget *   */ /* private */



/****************************************************************************/
/** String                                                                 **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_String[];
#else
#define MUIC_String "String.mui"
#endif

/* Methods */

#define MUIM_String_BoopsiForward           0x80424137 /* private */ /* V13 */
struct  MUIP_String_BoopsiForward           { ULONG MethodID; ULONG BoopsiID; }; /* private */

/* Attributes */

#define MUIA_String_Accept                  0x8042e3e1 /* V4  isg STRPTR            */
#define MUIA_String_Acknowledge             0x8042026c /* V4  ..g STRPTR            */
#define MUIA_String_AdvanceOnCR             0x804226de /* V11 isg BOOL              */
#define MUIA_String_AttachedList            0x80420fd2 /* V4  isg Object *          */
#define MUIA_String_BufferPos               0x80428b6c /* V4  .sg LONG              */
#define MUIA_String_Contents                0x80428ffd /* V4  isg STRPTR            */
#define MUIA_String_DisplayPos              0x8042ccbf /* V4  .sg LONG              */
#define MUIA_String_DoBoopsiForward         0x80427467 /* V18 isg BOOL              */ /* private */
#define MUIA_String_EditHook                0x80424c33 /* V7  isg struct Hook *     */
#define MUIA_String_Format                  0x80427484 /* V4  i.g LONG              */
#define MUIA_String_HistoryEntries          0x8042e4a2 /* V11 isg LONG              */ /* private */
#define MUIA_String_Integer                 0x80426e8a /* V4  isg ULONG             */
#define MUIA_String_LonelyEditHook          0x80421569 /* V11 isg BOOL              */
#define MUIA_String_MaxLen                  0x80424984 /* V4  i.g LONG              */
#define MUIA_String_Popup                   0x80420d71 /* V9  i.. Object *          */ /* private */
#define MUIA_String_Reject                  0x8042179c /* V4  isg STRPTR            */
#define MUIA_String_Secret                  0x80428769 /* V4  i.g BOOL              */
#define MUIA_String_Self                    0x804288e1 /* V16 ..g LONG              */ /* private */

#define MUIV_String_Format_Left 0
#define MUIV_String_Format_Center 1
#define MUIV_String_Format_Right 2


/****************************************************************************/
/** Boopsi                                                                 **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Boopsi[];
#else
#define MUIC_Boopsi "Boopsi.mui"
#endif

/* Attributes */

#define MUIA_Boopsi_Class                   0x80426999 /* V4  isg struct IClass *   */
#define MUIA_Boopsi_ClassID                 0x8042bfa3 /* V4  isg char *            */
#define MUIA_Boopsi_Dumbness                0x80428aa2 /* V11 i.. ULONG             */ /* private */
#define MUIA_Boopsi_MaxHeight               0x8042757f /* V4  isg ULONG             */
#define MUIA_Boopsi_MaxWidth                0x8042bcb1 /* V4  isg ULONG             */
#define MUIA_Boopsi_MinHeight               0x80422c93 /* V4  isg ULONG             */
#define MUIA_Boopsi_MinWidth                0x80428fb2 /* V4  isg ULONG             */
#define MUIA_Boopsi_Object                  0x80420178 /* V4  ..g Object *          */
#define MUIA_Boopsi_Remember                0x8042f4bd /* V4  i.. ULONG             */
#define MUIA_Boopsi_Smart                   0x8042b8d7 /* V9  i.. BOOL              */
#define MUIA_Boopsi_TagDrawInfo             0x8042bae7 /* V4  isg ULONG             */
#define MUIA_Boopsi_TagScreen               0x8042bc71 /* V4  isg ULONG             */
#define MUIA_Boopsi_TagWindow               0x8042e11d /* V4  isg ULONG             */



/****************************************************************************/
/** Prop                                                                   **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Prop[];
#else
#define MUIC_Prop "Prop.mui"
#endif

/* Methods */

#define MUIM_Prop_Decrease                  0x80420dd1 /* V16 */
#define MUIM_Prop_Increase                  0x8042cac0 /* V16 */
struct  MUIP_Prop_Decrease                  { ULONG MethodID; LONG amount; };
struct  MUIP_Prop_Increase                  { ULONG MethodID; LONG amount; };

/* Attributes */

#define MUIA_Prop_Delta                     0x8042c5f4 /* V4  is. LONG              */ /* private */
#define MUIA_Prop_DeltaFactor               0x80427c5e /* V4  isg LONG              */ /* private */
#define MUIA_Prop_DoSmooth                  0x804236ce /* V4  i.. BOOL              */ /* private */
#define MUIA_Prop_Dynamic                   0x8042bdd6 /* V20 isg LONG              */ /* private */
#define MUIA_Prop_Entries                   0x8042fbdb /* V4  isg LONG              */
#define MUIA_Prop_First                     0x8042d4b2 /* V4  isg LONG              */
#define MUIA_Prop_Horiz                     0x8042f4f3 /* V4  i.g BOOL              */
#define MUIA_Prop_Pressed                   0x80422cd7 /* V6  ..g BOOL              */ /* private */
#define MUIA_Prop_Quiet                     0x804274a6 /* V4  .s. BOOL              */ /* private */
#define MUIA_Prop_Refresh                   0x80429839 /* V4  ... BOOL              */ /* private */
#define MUIA_Prop_Slider                    0x80429c3a /* V4  isg BOOL              */
#define MUIA_Prop_TrueFirst                 0x804205dc /* V16 ..g LONG              */ /* private */
#define MUIA_Prop_UseWinBorder              0x8042deee /* V13 i.. LONG              */
#define MUIA_Prop_Visible                   0x8042fea6 /* V4  isg LONG              */

#define MUIV_Prop_UseWinBorder_None 0
#define MUIV_Prop_UseWinBorder_Left 1
#define MUIV_Prop_UseWinBorder_Right 2
#define MUIV_Prop_UseWinBorder_Bottom 3


/****************************************************************************/
/** Gauge                                                                  **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Gauge[];
#else
#define MUIC_Gauge "Gauge.mui"
#endif

/* Attributes */

#define MUIA_Gauge_Current                  0x8042f0dd /* V4  isg LONG              */
#define MUIA_Gauge_Divide                   0x8042d8df /* V4  isg ULONG             */
#define MUIA_Gauge_EmptyImage               0x8042357f /* V20 isg STRPTR            */
#define MUIA_Gauge_FullFrame                0x8042881b /* V20 isg STRPTR            */
#define MUIA_Gauge_FullImage                0x8042e14a /* V20 isg STRPTR            */
#define MUIA_Gauge_Horiz                    0x804232dd /* V4  i.. BOOL              */
#define MUIA_Gauge_InfoRate                 0x804253c8 /* V4  isg LONG              */
#define MUIA_Gauge_InfoText                 0x8042bf15 /* V7  isg STRPTR            */
#define MUIA_Gauge_Max                      0x8042bcdb /* V4  isg LONG              */



/****************************************************************************/
/** Scale                                                                  **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Scale[];
#else
#define MUIC_Scale "Scale.mui"
#endif

/* Attributes */

#define MUIA_Scale_Horiz                    0x8042919a /* V4  isg BOOL              */
#define MUIA_Scale_Max                      0x80426288 /* V4  isg LONG              */ /* private */
#define MUIA_Scale_Min                      0x80426c4a /* V4  isg LONG              */ /* private */



/****************************************************************************/
/** Colorfield                                                             **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Colorfield[];
#else
#define MUIC_Colorfield "Colorfield.mui"
#endif

/* Attributes */

#define MUIA_Colorfield_Blue                0x8042d3b0 /* V4  isg ULONG             */
#define MUIA_Colorfield_Green               0x80424466 /* V4  isg ULONG             */
#define MUIA_Colorfield_Pen                 0x8042713a /* V4  ..g ULONG             */
#define MUIA_Colorfield_Red                 0x804279f6 /* V4  isg ULONG             */
#define MUIA_Colorfield_Reference           0x8042dc22 /* V4  i.. Object *          */ /* private */
#define MUIA_Colorfield_RGB                 0x8042677a /* V4  isg ULONG *           */



/****************************************************************************/
/** List                                                                   **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_List[];
#else
#define MUIC_List "List.mui"
#endif

/* Methods */

#define MUIM_List_Clear                     0x8042ad89 /* V4  */
#define MUIM_List_CreateDragMap             0x80424534 /* private */ /* V11 */
#define MUIM_List_CreateImage               0x80429804 /* V11 */
#define MUIM_List_DeleteImage               0x80420f58 /* V11 */
#define MUIM_List_Exchange                  0x8042468c /* V4  */
#define MUIM_List_FreeDragMap               0x80425a68 /* private */ /* V11 */
#define MUIM_List_GetEntry                  0x804280ec /* V4  */
#define MUIM_List_Insert                    0x80426c87 /* V4  */
#define MUIM_List_InsertSingle              0x804254d5 /* V7  */
#define MUIM_List_Jump                      0x8042baab /* V4  */
#define MUIM_List_Move                      0x804253c2 /* V9  */
#define MUIM_List_NextSelected              0x80425f17 /* V6  */
#define MUIM_List_QueryColumnWidth          0x8042e09e /* private */ /* V11 */
#define MUIM_List_Redraw                    0x80427993 /* V4  */
#define MUIM_List_RedrawIfSimple            0x8042083f /* private */ /* V4  */
#define MUIM_List_Remove                    0x8042647e /* V4  */
#define MUIM_List_Select                    0x804252d8 /* V4  */
#define MUIM_List_SetDropMark               0x80429e97 /* private */ /* V11 */
#define MUIM_List_Sort                      0x80422275 /* V4  */
#define MUIM_List_TestPos                   0x80425f48 /* V11 */
struct  MUIP_List_Clear                     { ULONG MethodID; };
struct  MUIP_List_CreateDragMap             { ULONG MethodID; LONG touchx; LONG touchy; LONG pos; }; /* private */
struct  MUIP_List_CreateImage               { ULONG MethodID; Object *obj; ULONG flags; };
struct  MUIP_List_DeleteImage               { ULONG MethodID; APTR listimg; };
struct  MUIP_List_Exchange                  { ULONG MethodID; LONG pos1; LONG pos2; };
struct  MUIP_List_FreeDragMap               { ULONG MethodID; struct DragMap *dragmap; }; /* private */
struct  MUIP_List_GetEntry                  { ULONG MethodID; LONG pos; APTR *entry; };
struct  MUIP_List_Insert                    { ULONG MethodID; APTR *entries; LONG count; LONG pos; };
struct  MUIP_List_InsertSingle              { ULONG MethodID; APTR entry; LONG pos; };
struct  MUIP_List_Jump                      { ULONG MethodID; LONG pos; };
struct  MUIP_List_Move                      { ULONG MethodID; LONG from; LONG to; };
struct  MUIP_List_NextSelected              { ULONG MethodID; LONG *pos; };
struct  MUIP_List_QueryColumnWidth          { ULONG MethodID; LONG num; }; /* private */
struct  MUIP_List_Redraw                    { ULONG MethodID; LONG pos; };
struct  MUIP_List_RedrawIfSimple            { ULONG MethodID; LONG pos; }; /* private */
struct  MUIP_List_Remove                    { ULONG MethodID; LONG pos; };
struct  MUIP_List_Select                    { ULONG MethodID; LONG pos; LONG seltype; LONG *state; };
struct  MUIP_List_SetDropMark               { ULONG MethodID; LONG pos; }; /* private */
struct  MUIP_List_Sort                      { ULONG MethodID; };
struct  MUIP_List_TestPos                   { ULONG MethodID; LONG x; LONG y; struct MUI_List_TestPos_Result *res; };

/* Attributes */

#define MUIA_List_Active                    0x8042391c /* V4  isg LONG              */
#define MUIA_List_ActiveYKo                 0x8042b56e /* V4  ... LONG              */ /* private */
#define MUIA_List_AdjustHeight              0x8042850d /* V4  i.. BOOL              */
#define MUIA_List_AdjustMinWidth            0x8042408a /* V4  i.. BOOL              */ /* private */
#define MUIA_List_AdjustWidth               0x8042354a /* V4  i.. BOOL              */
#define MUIA_List_Array                     0x8042a616 /* V10 ..g ULONG *           */ /* private */
#define MUIA_List_AutoVisible               0x8042a445 /* V11 isg BOOL              */
#define MUIA_List_CompareHook               0x80425c14 /* V4  is. struct Hook *     */
#define MUIA_List_ConstructHook             0x8042894f /* V4  is. struct Hook *     */
#define MUIA_List_CopyHook                  0x8042c0b3 /* V4  i.. struct Hook *     */ /* private */
#define MUIA_List_CursorType                0x8042c53e /* V4  is. LONG              */ /* private */
#define MUIA_List_DestructHook              0x804297ce /* V4  is. struct Hook *     */
#define MUIA_List_DisplayHook               0x8042b4d5 /* V4  is. struct Hook *     */
#define MUIA_List_DragSortable              0x80426099 /* V11 isg BOOL              */
#define MUIA_List_DropMark                  0x8042aba6 /* V11 ..g LONG              */
#define MUIA_List_Entries                   0x80421654 /* V4  ..g LONG              */
#define MUIA_List_First                     0x804238d4 /* V4  ..g LONG              */
#define MUIA_List_Format                    0x80423c0a /* V4  isg STRPTR            */
#define MUIA_List_InsertPosition            0x8042d0cd /* V9  ..g LONG              */
#define MUIA_List_LineHeight                0x80425880 /* V20 ..g ULONG             */ /* private */
#define MUIA_List_MinLineHeight             0x8042d1c3 /* V4  i.. LONG              */
#define MUIA_List_MultiTestHook             0x8042c2c6 /* V4  is. struct Hook *     */
#define MUIA_List_Pool                      0x80423431 /* V13 i.. APTR              */
#define MUIA_List_PoolPuddleSize            0x8042a4eb /* V13 i.. ULONG             */
#define MUIA_List_PoolThreshSize            0x8042c48c /* V13 i.. ULONG             */
#define MUIA_List_PreAllocCount             0x80429ca2 /* V20 i.. ULONG             */ /* private */
#define MUIA_List_Quiet                     0x8042d8c7 /* V4  .s. BOOL              */
#define MUIA_List_ShowDropMarks             0x8042c6f3 /* V11 isg BOOL              */
#define MUIA_List_Source                    0x8042beba /* V4  i.. Object *          */ /* private */
#define MUIA_List_SourceArray               0x8042c0a0 /* V4  i.. APTR              */
#define MUIA_List_Stretch                   0x804281f5 /* V4  i.. LONG              */ /* private */
#define MUIA_List_Title                     0x80423e66 /* V6  isg char *            */
#define MUIA_List_TopPixel                  0x80429df3 /* V4  ..g LONG              */ /* private */
#define MUIA_List_TotalPixel                0x8042a8f5 /* V4  ..g LONG              */ /* private */
#define MUIA_List_Visible                   0x8042191f /* V4  ..g LONG              */
#define MUIA_List_VisiblePixel              0x804273e9 /* V4  ..g LONG              */ /* private */

#define MUIV_List_Active_Off -1
#define MUIV_List_Active_Top -2
#define MUIV_List_Active_Bottom -3
#define MUIV_List_Active_Up -4
#define MUIV_List_Active_Down -5
#define MUIV_List_Active_PageUp -6
#define MUIV_List_Active_PageDown -7
#define MUIV_List_ConstructHook_String -1
#define MUIV_List_CopyHook_String -1
#define MUIV_List_CursorType_None 0
#define MUIV_List_CursorType_Bar 1
#define MUIV_List_CursorType_Rect 2
#define MUIV_List_DestructHook_String -1


/****************************************************************************/
/** Floattext                                                              **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Floattext[];
#else
#define MUIC_Floattext "Floattext.mui"
#endif

/* Attributes */

#define MUIA_Floattext_Justify              0x8042dc03 /* V4  isg BOOL              */
#define MUIA_Floattext_SkipChars            0x80425c7d /* V4  is. STRPTR            */
#define MUIA_Floattext_TabSize              0x80427d17 /* V4  is. LONG              */
#define MUIA_Floattext_Text                 0x8042d16a /* V4  isg STRPTR            */



/****************************************************************************/
/** Volumelist                                                             **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Volumelist[];
#else
#define MUIC_Volumelist "Volumelist.mui"
#endif

/* Attributes */

#define MUIA_Volumelist_ExampleMode         0x804246a5 /* V20 i.. BOOL              */



/****************************************************************************/
/** Scrmodelist                                                            **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Scrmodelist[];
#else
#define MUIC_Scrmodelist "Scrmodelist.mui"
#endif

/* Attributes */

#define MUIA_Scrmodelist_PropertyFlags      0x804216bd /* V4  isg STRPTR            */ /* private */
#define MUIA_Scrmodelist_PropertyMask       0x8042ef0e /* V4  isg STRPTR            */ /* private */



/****************************************************************************/
/** Dirlist                                                                **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Dirlist[];
#else
#define MUIC_Dirlist "Dirlist.mui"
#endif

/* Methods */

#define MUIM_Dirlist_AskSlave               0x8042b0e8 /* private */ /* V4  */
#define MUIM_Dirlist_Enter                  0x804209f0 /* private */ /* V20 */
#define MUIM_Dirlist_Parent                 0x804213f3 /* private */ /* V20 */
#define MUIM_Dirlist_ReRead                 0x80422d71 /* V4  */
struct  MUIP_Dirlist_AskSlave               { ULONG MethodID; }; /* private */
struct  MUIP_Dirlist_Enter                  { ULONG MethodID; char *dir; }; /* private */
struct  MUIP_Dirlist_Parent                 { ULONG MethodID; }; /* private */
struct  MUIP_Dirlist_ReRead                 { ULONG MethodID; };

/* Attributes */

#define MUIA_Dirlist_AcceptPattern          0x8042760a /* V4  is. STRPTR            */
#define MUIA_Dirlist_Directory              0x8042ea41 /* V4  isg STRPTR            */
#define MUIA_Dirlist_DrawersOnly            0x8042b379 /* V4  is. BOOL              */
#define MUIA_Dirlist_FilesOnly              0x8042896a /* V4  is. BOOL              */
#define MUIA_Dirlist_FilterDrawers          0x80424ad2 /* V4  is. BOOL              */
#define MUIA_Dirlist_FilterHook             0x8042ae19 /* V4  is. struct Hook *     */
#define MUIA_Dirlist_MultiSelDirs           0x80428653 /* V6  is. BOOL              */
#define MUIA_Dirlist_NumBytes               0x80429e26 /* V4  ..g LONG              */
#define MUIA_Dirlist_NumDrawers             0x80429cb8 /* V4  ..g LONG              */
#define MUIA_Dirlist_NumFiles               0x8042a6f0 /* V4  ..g LONG              */
#define MUIA_Dirlist_Path                   0x80426176 /* V4  ..g STRPTR            */
#define MUIA_Dirlist_RejectIcons            0x80424808 /* V4  is. BOOL              */
#define MUIA_Dirlist_RejectPattern          0x804259c7 /* V4  is. STRPTR            */
#define MUIA_Dirlist_SortDirs               0x8042bbb9 /* V4  is. LONG              */
#define MUIA_Dirlist_SortHighLow            0x80421896 /* V4  is. BOOL              */
#define MUIA_Dirlist_SortType               0x804228bc /* V4  is. LONG              */
#define MUIA_Dirlist_Status                 0x804240de /* V4  ..g LONG              */

#define MUIV_Dirlist_SortDirs_First 0
#define MUIV_Dirlist_SortDirs_Last 1
#define MUIV_Dirlist_SortDirs_Mix 2
#define MUIV_Dirlist_SortType_Name 0
#define MUIV_Dirlist_SortType_Date 1
#define MUIV_Dirlist_SortType_Size 2
#define MUIV_Dirlist_Status_Invalid 0
#define MUIV_Dirlist_Status_Reading 1
#define MUIV_Dirlist_Status_Valid 2


/****************************************************************************/
/** Numeric                                                                **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Numeric[];
#else
#define MUIC_Numeric "Numeric.mui"
#endif

/* Methods */

#define MUIM_Numeric_Decrease               0x804243a7 /* V11 */
#define MUIM_Numeric_GetPixelSize           0x8042a2f2 /* private */ /* V11 */
#define MUIM_Numeric_Increase               0x80426ecd /* V11 */
#define MUIM_Numeric_ScaleToValue           0x8042032c /* V11 */
#define MUIM_Numeric_SetAutoReset           0x80421ecc /* private */ /* V11 */
#define MUIM_Numeric_SetDefault             0x8042ab0a /* V11 */
#define MUIM_Numeric_Stringify              0x80424891 /* V11 */
#define MUIM_Numeric_StringifyValue         0x8042148f /* private */ /* V11 */
#define MUIM_Numeric_ValueToScale           0x80423e4f /* V11 */
#define MUIM_Numeric_ValueToScale2          0x804224b1 /* private */ /* V20 */
struct  MUIP_Numeric_Decrease               { ULONG MethodID; LONG amount; };
struct  MUIP_Numeric_GetPixelSize           { ULONG MethodID; LONG *width; LONG *height; struct TextFont *font; }; /* private */
struct  MUIP_Numeric_Increase               { ULONG MethodID; LONG amount; };
struct  MUIP_Numeric_ScaleToValue           { ULONG MethodID; LONG scalemin; LONG scalemax; LONG scale; };
struct  MUIP_Numeric_SetAutoReset           { ULONG MethodID; }; /* private */
struct  MUIP_Numeric_SetDefault             { ULONG MethodID; };
struct  MUIP_Numeric_Stringify              { ULONG MethodID; LONG value; };
struct  MUIP_Numeric_StringifyValue         { ULONG MethodID; }; /* private */
struct  MUIP_Numeric_ValueToScale           { ULONG MethodID; LONG scalemin; LONG scalemax; };
struct  MUIP_Numeric_ValueToScale2          { ULONG MethodID; LONG scalemin; LONG scalemax; }; /* private */

/* Attributes */

#define MUIA_Numeric_AutoReset              0x80426652 /* V11 isg BOOL              */ /* private */
#define MUIA_Numeric_CheckAllSizes          0x80421594 /* V11 isg BOOL              */
#define MUIA_Numeric_Default                0x804263e8 /* V11 isg LONG              */
#define MUIA_Numeric_Format                 0x804263e9 /* V11 isg STRPTR            */
#define MUIA_Numeric_Max                    0x8042d78a /* V11 isg LONG              */
#define MUIA_Numeric_Min                    0x8042e404 /* V11 isg LONG              */
#define MUIA_Numeric_QValue                 0x8042d608 /* V11 isg LONG              */ /* private */
#define MUIA_Numeric_Reverse                0x8042f2a0 /* V11 isg BOOL              */
#define MUIA_Numeric_RevLeftRight           0x804294a7 /* V11 isg BOOL              */
#define MUIA_Numeric_RevUpDown              0x804252dd /* V11 isg BOOL              */
#define MUIA_Numeric_Value                  0x8042ae3a /* V11 isg LONG              */



/****************************************************************************/
/** Knob                                                                   **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Knob[];
#else
#define MUIC_Knob "Knob.mui"
#endif


/****************************************************************************/
/** Levelmeter                                                             **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Levelmeter[];
#else
#define MUIC_Levelmeter "Levelmeter.mui"
#endif

/* Attributes */

#define MUIA_Levelmeter_Label               0x80420dd5 /* V11 isg STRPTR            */



/****************************************************************************/
/** Numericbutton                                                          **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Numericbutton[];
#else
#define MUIC_Numericbutton "Numericbutton.mui"
#endif


/****************************************************************************/
/** Slider                                                                 **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Slider[];
#else
#define MUIC_Slider "Slider.mui"
#endif

/* Attributes */

#define MUIA_Slider_Horiz                   0x8042fad1 /* V11 isg BOOL              */
#ifdef MUI_OBSOLETE
#define MUIA_Slider_Level                   0x8042ae3a /* V4  isg LONG              */
#endif /* MUI_OBSOLETE */
#ifdef MUI_OBSOLETE
#define MUIA_Slider_Max                     0x8042d78a /* V4  isg LONG              */
#endif /* MUI_OBSOLETE */
#ifdef MUI_OBSOLETE
#define MUIA_Slider_Min                     0x8042e404 /* V4  isg LONG              */
#endif /* MUI_OBSOLETE */
#define MUIA_Slider_Quiet                   0x80420b26 /* V6  i.. BOOL              */
#define MUIA_Slider_QuietLevel              0x8042c7a9 /* V11 isg LONG              */ /* private */
#ifdef MUI_OBSOLETE
#define MUIA_Slider_Reverse                 0x8042f2a0 /* V4  isg BOOL              */
#endif /* MUI_OBSOLETE */
#define MUIA_Slider_Size                    0x8042b69d /* V20 isg BOOL              */



/****************************************************************************/
/** Framedisplay                                                           **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Framedisplay[];
#else
#define MUIC_Framedisplay "Framedisplay.mui"
#endif

/* Attributes */

#define MUIA_Framedisplay_ShowSpacing       0x80428a94 /* V11 isg BOOL              */ /* private */
#define MUIA_Framedisplay_Spec              0x80421794 /* V11 isg struct MUI_FrameSpec * */ /* private */
#define MUIA_Framedisplay_Thickness         0x804298f3 /* V11 i.. LONG              */ /* private */



/****************************************************************************/
/** Popframe                                                               **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Popframe[];
#else
#define MUIC_Popframe "Popframe.mui"
#endif


/****************************************************************************/
/** Imagedisplay                                                           **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Imagedisplay[];
#else
#define MUIC_Imagedisplay "Imagedisplay.mui"
#endif

/* Attributes */

#define MUIA_Imagedisplay_Balance           0x80424c09 /* V20 isg LONG              */ /* private */
#define MUIA_Imagedisplay_BalanceBlue       0x80422195 /* V20 isg LONG              */ /* private */
#define MUIA_Imagedisplay_BalanceGreen      0x8042b221 /* V20 isg LONG              */ /* private */
#define MUIA_Imagedisplay_BalanceRed        0x80425674 /* V20 isg LONG              */ /* private */
#define MUIA_Imagedisplay_Spec              0x8042a547 /* V11 isg struct MUI_ImageSpec * */ /* private */
#define MUIA_Imagedisplay_UseDefSize        0x8042186d /* V11 i.. BOOL              */ /* private */
#define MUIA_Imagedisplay_UseFancyStuff     0x8042a004 /* V20 i.. BOOL              */ /* private */



/****************************************************************************/
/** Popimage                                                               **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Popimage[];
#else
#define MUIC_Popimage "Popimage.mui"
#endif


/****************************************************************************/
/** Pendisplay                                                             **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Pendisplay[];
#else
#define MUIC_Pendisplay "Pendisplay.mui"
#endif

/* Methods */

#define MUIM_Pendisplay_SetColormap         0x80426c80 /* V13 */
#define MUIM_Pendisplay_SetMUIPen           0x8042039d /* V13 */
#define MUIM_Pendisplay_SetRGB              0x8042c131 /* V13 */
#define MUIM_Pendisplay_SetSysPen           0x804261e1 /* private */ /* V13 */
struct  MUIP_Pendisplay_SetColormap         { ULONG MethodID; LONG colormap; };
struct  MUIP_Pendisplay_SetMUIPen           { ULONG MethodID; LONG muipen; };
struct  MUIP_Pendisplay_SetRGB              { ULONG MethodID; ULONG red; ULONG green; ULONG blue; };
struct  MUIP_Pendisplay_SetSysPen           { ULONG MethodID; LONG syspen; }; /* private */

/* Attributes */

#define MUIA_Pendisplay_BackReference       0x80428974 /* V13 .s. Object *          */ /* private */
#define MUIA_Pendisplay_Pen                 0x8042a748 /* V13 ..g ULONG             */
#define MUIA_Pendisplay_Reference           0x8042dc24 /* V13 isg Object *          */
#define MUIA_Pendisplay_RGBcolor            0x8042a1a9 /* V11 isg struct MUI_RGBcolor * */
#define MUIA_Pendisplay_Spec                0x8042a204 /* V11 isg struct MUI_PenSpec  * */



/****************************************************************************/
/** Poppen                                                                 **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Poppen[];
#else
#define MUIC_Poppen "Poppen.mui"
#endif


/****************************************************************************/
/** Group                                                                  **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Group[];
#else
#define MUIC_Group "Group.mui"
#endif

/* Methods */

#define MUIM_Group_Balance                  0x80425906 /* private */ /* V11 */
#define MUIM_Group_BuildCycleList           0x8042b22e /* private */ /* V11 */
#define MUIM_Group_ClearCycleFlags          0x8042c520 /* private */ /* V11 */
#define MUIM_Group_ExitChange               0x8042d1cc /* V11 */
#define MUIM_Group_ExitChange2              0x8042e541 /* private */ /* V12 */
#define MUIM_Group_InitChange               0x80420887 /* V11 */
#define MUIM_Group_MoveAround               0x80421f7b /* private */ /* V4  */
#define MUIM_Group_MoveMember               0x8042ff4e /* V16 */
#define MUIM_Group_Sort                     0x80427417 /* V4  */
#define MUIM_Group_TestXY                   0x804250d4 /* private */ /* V4  */
#define MUIM_Group_Transfer                 0x8042fb67 /* private */ /* V4  */
struct  MUIP_Group_Balance                  { ULONG MethodID; Object *bal; LONG clickx; LONG clicky; LONG mode; LONG mousex; LONG mousey; }; /* private */
struct  MUIP_Group_BuildCycleList           { ULONG MethodID; Object **chain; LONG count; }; /* private */
struct  MUIP_Group_ClearCycleFlags          { ULONG MethodID; }; /* private */
struct  MUIP_Group_ExitChange               { ULONG MethodID; };
struct  MUIP_Group_ExitChange2              { ULONG MethodID; ULONG flags; }; /* private */
struct  MUIP_Group_InitChange               { ULONG MethodID; };
struct  MUIP_Group_MoveAround               { ULONG MethodID; LONG col; LONG row; LONG x; LONG y; LONG dx; LONG dy; }; /* private */
struct  MUIP_Group_MoveMember               { ULONG MethodID; Object *o; LONG pos; };
struct  MUIP_Group_Sort                     { ULONG MethodID; Object *obj[1]; };
struct  MUIP_Group_TestXY                   { ULONG MethodID; LONG x; LONG y; }; /* private */
struct  MUIP_Group_Transfer                 { ULONG MethodID; Object *group; }; /* private */

/* Attributes */

#define MUIA_Group_ActivePage               0x80424199 /* V5  isg LONG              */
#define MUIA_Group_Child                    0x804226e6 /* V4  i.. Object *          */
#define MUIA_Group_ChildList                0x80424748 /* V4  ..g struct List *     */
#define MUIA_Group_Columns                  0x8042f416 /* V4  is. LONG              */
#define MUIA_Group_Forward                  0x80421422 /* V11 .s. BOOL              */
#define MUIA_Group_Horiz                    0x8042536b /* V4  i.. BOOL              */
#define MUIA_Group_HorizSpacing             0x8042c651 /* V4  isg LONG              */
#define MUIA_Group_LayoutHook               0x8042c3b2 /* V11 i.. struct Hook *     */
#define MUIA_Group_PageMax                  0x8042d777 /* V4  i.. BOOL              */ /* private */
#define MUIA_Group_PageMode                 0x80421a5f /* V5  i.. BOOL              */
#define MUIA_Group_Rows                     0x8042b68f /* V4  is. LONG              */
#define MUIA_Group_SameHeight               0x8042037e /* V4  i.. BOOL              */
#define MUIA_Group_SameSize                 0x80420860 /* V4  i.. BOOL              */
#define MUIA_Group_SameWidth                0x8042b3ec /* V4  i.. BOOL              */
#define MUIA_Group_Spacing                  0x8042866d /* V4  is. LONG              */
#define MUIA_Group_Type                     0x8042e866 /* V11 ..g LONG              */ /* private */
#define MUIA_Group_VertSpacing              0x8042e1bf /* V4  isg LONG              */

#define MUIV_Group_ActivePage_First 0
#define MUIV_Group_ActivePage_Last -1
#define MUIV_Group_ActivePage_Prev -2
#define MUIV_Group_ActivePage_Next -3
#define MUIV_Group_ActivePage_Advance -4


/****************************************************************************/
/** Selectgroup                                                            **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Selectgroup[];
#else
#define MUIC_Selectgroup "Selectgroup.mui"
#endif


/****************************************************************************/
/** Menudisplay                                                            **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Menudisplay[];
#else
#define MUIC_Menudisplay "Menudisplay.mui"
#endif

/* Methods */

#define MUIM_Menudisplay_Pop                0x804248d6 /* private */ /* V20 */
#define MUIM_Menudisplay_SetMenu            0x8042f7a7 /* private */ /* V20 */
struct  MUIP_Menudisplay_Pop                { ULONG MethodID; Object *txt; }; /* private */
struct  MUIP_Menudisplay_SetMenu            { ULONG MethodID; Object *strip; }; /* private */


/****************************************************************************/
/** Mccprefs                                                               **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Mccprefs[];
#else
#define MUIC_Mccprefs "Mccprefs.mui"
#endif

/* Methods */

#define MUIM_Mccprefs_CheckClass            0x8042925a /* private */ /* V20 */
#define MUIM_Mccprefs_ConfigToGadgets       0x80427043 /* V11 */
#define MUIM_Mccprefs_FindExtraTitle        0x8042b1e8 /* private */ /* V20 */
#define MUIM_Mccprefs_FindGadget            0x8042df61 /* private */ /* V20 */
#define MUIM_Mccprefs_GadgetsToConfig       0x80425242 /* V11 */
#define MUIM_Mccprefs_RegisterGadget        0x80424828 /* V20 */
#define MUIM_Mccprefs_SetContextMenus       0x804289f2 /* private */ /* V20 */
struct  MUIP_Mccprefs_CheckClass            { ULONG MethodID; STRPTR classname; }; /* private */
struct  MUIP_Mccprefs_ConfigToGadgets       { ULONG MethodID; Object *configdata; };
struct  MUIP_Mccprefs_FindExtraTitle        { ULONG MethodID; Object *gadget; }; /* private */
struct  MUIP_Mccprefs_FindGadget            { ULONG MethodID; ULONG id; }; /* private */
struct  MUIP_Mccprefs_GadgetsToConfig       { ULONG MethodID; Object *configdata; };
struct  MUIP_Mccprefs_RegisterGadget        { ULONG MethodID; Object *gadget; ULONG id; ULONG flags; STRPTR title; };
struct  MUIP_Mccprefs_SetContextMenus       { ULONG MethodID; Object *pagemenu; Object *gadgetmenu; }; /* private */

/* Attributes */

#define MUIA_Mccprefs_SettingsObject        0x804270f8 /* V20 i.. Object *          */ /* private */



/****************************************************************************/
/** Register                                                               **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Register[];
#else
#define MUIC_Register "Register.mui"
#endif

/* Attributes */

#define MUIA_Register_Frame                 0x8042349b /* V7  i.g BOOL              */
#define MUIA_Register_Titles                0x804297ec /* V7  i.g STRPTR *          */



/****************************************************************************/
/** Penadjust                                                              **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Penadjust[];
#else
#define MUIC_Penadjust "Penadjust.mui"
#endif

/* Methods */

#define MUIM_Penadjust_Update               0x8042bdfe /* private */ /* V11 */
struct  MUIP_Penadjust_Update               { ULONG MethodID; LONG type; }; /* private */

/* Attributes */

#define MUIA_Penadjust_DoColormap           0x8042deed /* V11 i.. BOOL              */ /* private */
#define MUIA_Penadjust_DoMUIpens            0x804289e7 /* V11 i.. BOOL              */ /* private */
#define MUIA_Penadjust_DoRGB                0x80424beb /* V11 i.. BOOL              */ /* private */
#define MUIA_Penadjust_DoSyspens            0x80424097 /* V11 i.. BOOL              */ /* private */
#define MUIA_Penadjust_PSIMode              0x80421cbb /* V11 i.. BOOL              */
#define MUIA_Penadjust_Spec                 0x80420d9f /* V11 isg struct MUI_PenSpec * */ /* private */
#define MUIA_Penadjust_Terminate            0x8042e0b0 /* V11 ..g BOOL              */ /* private */



/****************************************************************************/
/** Settingsgroup                                                          **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Settingsgroup[];
#else
#define MUIC_Settingsgroup "Settingsgroup.mui"
#endif

/* Methods */

#define MUIM_Settingsgroup_ConfigToGadgets  0x80427043 /* V11 */
#define MUIM_Settingsgroup_GadgetsToConfig  0x80425242 /* V11 */
#define MUIM_Settingsgroup_GetMenuTitle     0x80428152 /* private */ /* V20 */
#define MUIM_Settingsgroup_Init             0x8042558b /* private */ /* V20 */
#define MUIM_Settingsgroup_MakeTimeSlider   0x804276de /* private */ /* V20 */
#define MUIM_Settingsgroup_State            0x8042b9f9 /* private */ /* V11 */
struct  MUIP_Settingsgroup_ConfigToGadgets  { ULONG MethodID; Object *configdata; };
struct  MUIP_Settingsgroup_GadgetsToConfig  { ULONG MethodID; Object *configdata; };
struct  MUIP_Settingsgroup_GetMenuTitle     { ULONG MethodID; Object *cmobj; Object *cm; }; /* private */
struct  MUIP_Settingsgroup_Init             { ULONG MethodID; LONG group; Object *gadgetmenu; Object *settingsobj; }; /* private */
struct  MUIP_Settingsgroup_MakeTimeSlider   { ULONG MethodID; ULONG tags; }; /* private */
struct  MUIP_Settingsgroup_State            { ULONG MethodID; LONG disable; }; /* private */

/* Attributes */

#define MUIA_Settingsgroup_AppTitle         0x80428f3b /* V20 i.. STRPTR            */ /* private */
#define MUIA_Settingsgroup_GadgetMenu       0x8042ca26 /* V11 i.. Object *          */ /* private */
#define MUIA_Settingsgroup_Group            0x8042633c /* V11 i.. LONG              */ /* private */
#define MUIA_Settingsgroup_SettingsObject   0x804270f8 /* V11 i.. Object *          */ /* private */



/****************************************************************************/
/** Settingsgroupr                                                         **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Settingsgroupr[];
#else
#define MUIC_Settingsgroupr "Settingsgroupr.mui"
#endif


/****************************************************************************/
/** Settings                                                               **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Settings[];
#else
#define MUIC_Settings "Settings.mui"
#endif

/* Methods */

#define MUIM_Settings_GetConfig             0x80428604 /* private */ /* V11 */
#define MUIM_Settings_MenuAction            0x8042016f /* private */ /* V11 */
#define MUIM_Settings_Multiview             0x804276f4 /* private */ /* V16 */
#define MUIM_Settings_Profile               0x8042251a /* private */ /* V13 */
#define MUIM_Settings_SetConfig             0x8042a106 /* private */ /* V11 */
#define MUIM_Settings_SetGroup              0x8042de52 /* private */ /* V11 */
#define MUIM_Settings_SetTitle              0x804272e4 /* private */ /* V20 */
#define MUIM_Settings_Special               0x8042b662 /* private */ /* V11 */
struct  MUIP_Settings_GetConfig             { ULONG MethodID; }; /* private */
struct  MUIP_Settings_MenuAction            { ULONG MethodID; LONG id; Object *mcp; Object *menuitem; }; /* private */
struct  MUIP_Settings_Multiview             { ULONG MethodID; char *filename; char *options; }; /* private */
struct  MUIP_Settings_Profile               { ULONG MethodID; char *comline; }; /* private */
struct  MUIP_Settings_SetConfig             { ULONG MethodID; Object *configdata; LONG refresh; }; /* private */
struct  MUIP_Settings_SetGroup              { ULONG MethodID; }; /* private */
struct  MUIP_Settings_SetTitle              { ULONG MethodID; }; /* private */
struct  MUIP_Settings_Special               { ULONG MethodID; LONG val; }; /* private */

/* Attributes */

#define MUIA_Settings_Application           0x8042affc /* V20 i.. Object *          */ /* private */
#define MUIA_Settings_Disable               0x8042d533 /* V11 .s. BOOL              */ /* private */
#define MUIA_Settings_Hash                  0x80426692 /* V20 i.g ULONG             */ /* private */
#define MUIA_Settings_Local                 0x80426f4d /* V11 i.g Object *          */ /* private */
#define MUIA_Settings_LonelyPage            0x8042900d /* V20 i.. ULONG             */ /* private */
#define MUIA_Settings_Welcome               0x8042877b /* V11 i.. Object *          */ /* private */



/****************************************************************************/
/** Frameadjust                                                            **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Frameadjust[];
#else
#define MUIC_Frameadjust "Frameadjust.mui"
#endif

/* Methods */

#define MUIM_Frameadjust_Update             0x8042a35e /* private */ /* V11 */
struct  MUIP_Frameadjust_Update             { ULONG MethodID; LONG frame; LONG sel; LONG left; LONG right; LONG top; LONG bottom; }; /* private */

/* Attributes */

#define MUIA_Frameadjust_Spec               0x8042965c /* V11 isg struct MUI_FrameSpec * */ /* private */
#define MUIA_Frameadjust_Terminate          0x80422034 /* V11 ..g BOOL              */ /* private */



/****************************************************************************/
/** Imageadjust                                                            **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Imageadjust[];
#else
#define MUIC_Imageadjust "Imageadjust.mui"
#endif

/* Methods */

#define MUIM_Imageadjust_ObjToSpec          0x8042dec0 /* private */ /* V11 */
#define MUIM_Imageadjust_SpecToObj          0x8042d6e0 /* private */ /* V11 */
struct  MUIP_Imageadjust_ObjToSpec          { ULONG MethodID; }; /* private */
struct  MUIP_Imageadjust_SpecToObj          { ULONG MethodID; char *spec; }; /* private */

/* Attributes */

#define MUIA_Imageadjust_DoAlien            0x804230f3 /* V11 i.. BOOL              */ /* private */
#define MUIA_Imageadjust_DoBuiltin          0x80420d96 /* V11 i.. BOOL              */ /* private */
#define MUIA_Imageadjust_DoExternal         0x80421132 /* V11 i.. BOOL              */ /* private */
#define MUIA_Imageadjust_DoGradient         0x80420519 /* V20 i.. BOOL              */ /* private */
#define MUIA_Imageadjust_DoPattern          0x8042b676 /* V11 i.. BOOL              */ /* private */
#define MUIA_Imageadjust_DoPen              0x804231a4 /* V11 i.. BOOL              */ /* private */
#define MUIA_Imageadjust_Spec               0x804279e1 /* V11 isg struct MUI_ImageSpec * */ /* private */
#define MUIA_Imageadjust_Terminate          0x80428417 /* V11 ..g BOOL              */ /* private */
#define MUIA_Imageadjust_Type               0x80422f2b /* V11 i.. LONG              */ /* private */

#define MUIV_Imageadjust_Type_All 0
#define MUIV_Imageadjust_Type_Image 1
#define MUIV_Imageadjust_Type_Background 2
#define MUIV_Imageadjust_Type_Pen 3


/****************************************************************************/
/** Virtgroup                                                              **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Virtgroup[];
#else
#define MUIC_Virtgroup "Virtgroup.mui"
#endif

/* Methods */

#define MUIM_Virtgroup_AutoMove             0x8042b368 /* private */ /* V18 */
#define MUIM_Virtgroup_Correct              0x8042bcd2 /* private */ /* V11 */
#define MUIM_Virtgroup_GetClip              0x8042a0d6 /* private */ /* V4  */
#define MUIM_Virtgroup_InstallClip          0x80425b5c /* private */ /* V11 */
#define MUIM_Virtgroup_MakeVisible          0x8042d2d2 /* private */ /* V4  */
#define MUIM_Virtgroup_RemoveClip           0x8042da52 /* private */ /* V11 */
#define MUIM_Virtgroup_Smoothie             0x8042acbc /* private */ /* V20 */
#define MUIM_Virtgroup_Update               0x8042ffbf /* private */ /* V11 */
struct  MUIP_Virtgroup_AutoMove             { ULONG MethodID; }; /* private */
struct  MUIP_Virtgroup_Correct              { ULONG MethodID; LONG width; LONG height; LONG failwidth; LONG failheight; LONG offsx; LONG offsy; }; /* private */
struct  MUIP_Virtgroup_GetClip              { ULONG MethodID; struct Rectangle *rect; }; /* private */
struct  MUIP_Virtgroup_InstallClip          { ULONG MethodID; }; /* private */
struct  MUIP_Virtgroup_MakeVisible          { ULONG MethodID; Object *obj; ULONG flags; }; /* private */
struct  MUIP_Virtgroup_RemoveClip           { ULONG MethodID; }; /* private */
struct  MUIP_Virtgroup_Smoothie             { ULONG MethodID; }; /* private */
struct  MUIP_Virtgroup_Update               { ULONG MethodID; LONG layout; }; /* private */

/* Attributes */

#define MUIA_Virtgroup_FreeHoriz            0x8042dcde /* V9  i.. BOOL              */ /* private */
#define MUIA_Virtgroup_FreeVert             0x80422da9 /* V9  i.. BOOL              */ /* private */
#define MUIA_Virtgroup_Height               0x80423038 /* V6  ..g LONG              */
#define MUIA_Virtgroup_Input                0x80427f7e /* V11 i.. BOOL              */
#define MUIA_Virtgroup_Left                 0x80429371 /* V6  isg LONG              */
#define MUIA_Virtgroup_Scrollgroup          0x80420e88 /* V11 .s. Object *          */ /* private */
#define MUIA_Virtgroup_Smooth               0x8042b7c7 /* V20 isg LONG              */ /* private */
#define MUIA_Virtgroup_Top                  0x80425200 /* V6  isg LONG              */
#define MUIA_Virtgroup_Width                0x80427c49 /* V6  ..g LONG              */



/****************************************************************************/
/** Scrollgroup                                                            **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Scrollgroup[];
#else
#define MUIC_Scrollgroup "Scrollgroup.mui"
#endif

/* Methods */

#define MUIM_Scrollgroup_Inform             0x804218a6 /* private */ /* V11 */
struct  MUIP_Scrollgroup_Inform             { ULONG MethodID; LONG xpos; LONG ypos; LONG viswidth; LONG visheight; LONG width; LONG height; }; /* private */

/* Attributes */

#define MUIA_Scrollgroup_Contents           0x80421261 /* V4  i.g Object *          */
#define MUIA_Scrollgroup_FreeHoriz          0x804292f3 /* V9  i.. BOOL              */
#define MUIA_Scrollgroup_FreeVert           0x804224f2 /* V9  i.. BOOL              */
#define MUIA_Scrollgroup_HorizBar           0x8042b63d /* V16 ..g Object *          */
#define MUIA_Scrollgroup_UseWinBorder       0x804284c1 /* V13 i.. BOOL              */
#define MUIA_Scrollgroup_VertBar            0x8042cdc0 /* V16 ..g Object *          */



/****************************************************************************/
/** Scrollbar                                                              **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Scrollbar[];
#else
#define MUIC_Scrollbar "Scrollbar.mui"
#endif

/* Attributes */

#define MUIA_Scrollbar_Type                 0x8042fb6b /* V11 i.. LONG              */

#define MUIV_Scrollbar_Type_Default 0
#define MUIV_Scrollbar_Type_Bottom 1
#define MUIV_Scrollbar_Type_Top 2
#define MUIV_Scrollbar_Type_Sym 3


/****************************************************************************/
/** Listview                                                               **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Listview[];
#else
#define MUIC_Listview "Listview.mui"
#endif

/* Attributes */

#define MUIA_Listview_ClickColumn           0x8042d1b3 /* V7  ..g LONG              */
#define MUIA_Listview_ClickColumnX          0x8042a18c /* V15 ..g LONG              */ /* private */
#define MUIA_Listview_DefClickColumn        0x8042b296 /* V7  isg LONG              */
#define MUIA_Listview_DoubleClick           0x80424635 /* V4  i.g BOOL              */
#define MUIA_Listview_DragBounds            0x80428dde /* V11 isg Object *          */ /* private */
#define MUIA_Listview_DragType              0x80425cd3 /* V11 isg LONG              */
#define MUIA_Listview_HandleRMB             0x804234c4 /* V20 isg BOOL              */ /* private */
#define MUIA_Listview_Input                 0x8042682d /* V4  i.. BOOL              */
#define MUIA_Listview_List                  0x8042bcce /* V4  i.g Object *          */
#define MUIA_Listview_MultiSelect           0x80427e08 /* V7  i.. LONG              */
#define MUIA_Listview_ScrollerPos           0x8042b1b4 /* V10 i.. BOOL              */
#define MUIA_Listview_SelectChange          0x8042178f /* V4  ..g BOOL              */

#define MUIV_Listview_DragType_None 0
#define MUIV_Listview_DragType_Immediate 1
#define MUIV_Listview_MultiSelect_None 0
#define MUIV_Listview_MultiSelect_Default 1
#define MUIV_Listview_MultiSelect_Shifted 2
#define MUIV_Listview_MultiSelect_Always 3
#define MUIV_Listview_ScrollerPos_Default 0
#define MUIV_Listview_ScrollerPos_Left 1
#define MUIV_Listview_ScrollerPos_Right 2
#define MUIV_Listview_ScrollerPos_None 3


/****************************************************************************/
/** Radio                                                                  **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Radio[];
#else
#define MUIC_Radio "Radio.mui"
#endif

/* Attributes */

#define MUIA_Radio_Active                   0x80429b41 /* V4  isg LONG              */
#define MUIA_Radio_Entries                  0x8042b6a1 /* V4  i.. STRPTR *          */



/****************************************************************************/
/** Cycle                                                                  **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Cycle[];
#else
#define MUIC_Cycle "Cycle.mui"
#endif

/* Attributes */

#define MUIA_Cycle_Active                   0x80421788 /* V4  isg LONG              */
#define MUIA_Cycle_Entries                  0x80420629 /* V4  i.. STRPTR *          */
#define MUIA_Cycle_Textmode                 0x8042a47a /* V20 i.. BOOL              */ /* private */

#define MUIV_Cycle_Active_Next -1
#define MUIV_Cycle_Active_Prev -2


/****************************************************************************/
/** Coloradjust                                                            **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Coloradjust[];
#else
#define MUIC_Coloradjust "Coloradjust.mui"
#endif

/* Methods */

#define MUIM_Coloradjust_Preset             0x8042497e /* private */ /* V13 */
struct  MUIP_Coloradjust_Preset             { ULONG MethodID; LONG num; }; /* private */

/* Attributes */

#define MUIA_Coloradjust_Blue               0x8042b8a3 /* V4  isg ULONG             */
#define MUIA_Coloradjust_DoField            0x8042855b /* V11 i.. BOOL              */ /* private */
#define MUIA_Coloradjust_DoPages            0x80429a08 /* V11 i.. BOOL              */ /* private */
#define MUIA_Coloradjust_Green              0x804285ab /* V4  isg ULONG             */
#define MUIA_Coloradjust_ModeID             0x8042ec59 /* V4  isg ULONG             */
#define MUIA_Coloradjust_Red                0x80420eaa /* V4  isg ULONG             */
#define MUIA_Coloradjust_RGB                0x8042f899 /* V4  isg ULONG *           */



/****************************************************************************/
/** Palette                                                                **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Palette[];
#else
#define MUIC_Palette "Palette.mui"
#endif

/* Attributes */

#define MUIA_Palette_Entries                0x8042a3d8 /* V6  i.g struct MUI_Palette_Entry * */
#define MUIA_Palette_Groupable              0x80423e67 /* V6  isg BOOL              */
#define MUIA_Palette_Names                  0x8042c3a2 /* V6  isg char **           */



/****************************************************************************/
/** Popstring                                                              **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Popstring[];
#else
#define MUIC_Popstring "Popstring.mui"
#endif

/* Methods */

#define MUIM_Popstring_Close                0x8042dc52 /* V7  */
#define MUIM_Popstring_Open                 0x804258ba /* V7  */
#define MUIM_Popstring_ReallyClose          0x8042303d /* private */ /* V7  */
struct  MUIP_Popstring_Close                { ULONG MethodID; LONG result; };
struct  MUIP_Popstring_Open                 { ULONG MethodID; };
struct  MUIP_Popstring_ReallyClose          { ULONG MethodID; LONG result; }; /* private */

/* Attributes */

#define MUIA_Popstring_Button               0x8042d0b9 /* V7  i.g Object *          */
#define MUIA_Popstring_CloseHook            0x804256bf /* V7  isg struct Hook *     */
#define MUIA_Popstring_OpenHook             0x80429d00 /* V7  isg struct Hook *     */
#define MUIA_Popstring_String               0x804239ea /* V7  i.g Object *          */
#define MUIA_Popstring_Toggle               0x80422b7a /* V7  isg BOOL              */



/****************************************************************************/
/** Pubscreenadjust                                                        **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Pubscreenadjust[];
#else
#define MUIC_Pubscreenadjust "Pubscreenadjust.mui"
#endif

/* Methods */

#define MUIM_Pubscreenadjust_GetScreen      0x80421671 /* private */ /* V20 */
#define MUIM_Pubscreenadjust_SetScreen      0x8042e582 /* private */ /* V20 */
#define MUIM_Pubscreenadjust_Startup        0x8042cf5b /* private */ /* V20 */
#define MUIM_Pubscreenadjust_Update         0x8042fd1e /* private */ /* V20 */
struct  MUIP_Pubscreenadjust_GetScreen      { ULONG MethodID; struct MUI_PubScreenDesc *desc; }; /* private */
struct  MUIP_Pubscreenadjust_SetScreen      { ULONG MethodID; struct MUI_PubScreenDesc *desc; }; /* private */
struct  MUIP_Pubscreenadjust_Startup        { ULONG MethodID; }; /* private */
struct  MUIP_Pubscreenadjust_Update         { ULONG MethodID; LONG level; }; /* private */


/****************************************************************************/
/** Pubscreenpanel                                                         **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Pubscreenpanel[];
#else
#define MUIC_Pubscreenpanel "Pubscreenpanel.mui"
#endif

/* Methods */

#define MUIM_Pubscreenpanel_Update          0x8042a5e7 /* private */ /* V20 */
struct  MUIP_Pubscreenpanel_Update          { ULONG MethodID; LONG level; }; /* private */

/* Attributes */

#define MUIA_Pubscreenpanel_ScreenName      0x80428ffd /* V20 isg STRPTR            */ /* private */



/****************************************************************************/
/** Pubscreenlist                                                          **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Pubscreenlist[];
#else
#define MUIC_Pubscreenlist "Pubscreenlist.mui"
#endif

/* Methods */

#define MUIM_Pubscreenlist_Activate         0x804218f3 /* private */ /* V20 */
#define MUIM_Pubscreenlist_CloneScreen      0x804249b5 /* private */ /* V20 */
#define MUIM_Pubscreenlist_DeleteScreen     0x8042213a /* private */ /* V20 */
#define MUIM_Pubscreenlist_EditScreen       0x8042ca95 /* private */ /* V20 */
#define MUIM_Pubscreenlist_Finish           0x8042f24b /* private */ /* V20 */
#define MUIM_Pubscreenlist_NewScreen        0x8042d0a2 /* private */ /* V20 */
#define MUIM_Pubscreenlist_OpenCloseJump    0x80422344 /* private */ /* V20 */
#define MUIM_Pubscreenlist_UndoScreen       0x8042d31f /* private */ /* V20 */
#define MUIM_Pubscreenlist_Update           0x80425e2e /* private */ /* V20 */
#define MUIM_Pubscreenlist_UpdateGadgets    0x8042efff /* private */ /* V20 */
#define MUIM_Pubscreenlist_UpdateSelection  0x804291c8 /* private */ /* V20 */
struct  MUIP_Pubscreenlist_Activate         { ULONG MethodID; char *name; }; /* private */
struct  MUIP_Pubscreenlist_CloneScreen      { ULONG MethodID; }; /* private */
struct  MUIP_Pubscreenlist_DeleteScreen     { ULONG MethodID; }; /* private */
struct  MUIP_Pubscreenlist_EditScreen       { ULONG MethodID; }; /* private */
struct  MUIP_Pubscreenlist_Finish           { ULONG MethodID; Object *panel; ULONG ok; }; /* private */
struct  MUIP_Pubscreenlist_NewScreen        { ULONG MethodID; }; /* private */
struct  MUIP_Pubscreenlist_OpenCloseJump    { ULONG MethodID; ULONG type; }; /* private */
struct  MUIP_Pubscreenlist_UndoScreen       { ULONG MethodID; }; /* private */
struct  MUIP_Pubscreenlist_Update           { ULONG MethodID; ULONG type; ULONG screenid; Object *originator; }; /* private */
struct  MUIP_Pubscreenlist_UpdateGadgets    { ULONG MethodID; }; /* private */
struct  MUIP_Pubscreenlist_UpdateSelection  { ULONG MethodID; }; /* private */

/* Attributes */

#define MUIA_Pubscreenlist_Selection        0x8042fe58 /* V20 ..g STRPTR            */
#define MUIA_Pubscreenlist_String           0x80422ce2 /* V20 .s. Object *          */



/****************************************************************************/
/** Popobject                                                              **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Popobject[];
#else
#define MUIC_Popobject "Popobject.mui"
#endif

/* Attributes */

#define MUIA_Popobject_Follow               0x80424cb5 /* V7  isg BOOL              */
#define MUIA_Popobject_Light                0x8042a5a3 /* V7  isg BOOL              */
#define MUIA_Popobject_Object               0x804293e3 /* V7  i.g Object *          */
#define MUIA_Popobject_ObjStrHook           0x8042db44 /* V7  isg struct Hook *     */
#define MUIA_Popobject_StrObjHook           0x8042fbe1 /* V7  isg struct Hook *     */
#define MUIA_Popobject_Volatile             0x804252ec /* V7  isg BOOL              */
#define MUIA_Popobject_WindowHook           0x8042f194 /* V9  isg struct Hook *     */



/****************************************************************************/
/** Poplist                                                                **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Poplist[];
#else
#define MUIC_Poplist "Poplist.mui"
#endif

/* Attributes */

#define MUIA_Poplist_Array                  0x8042084c /* V8  i.. char **           */
#define MUIA_Poplist_ListObject             0x80426211 /* V20 i.. Object *          */ /* private */



/****************************************************************************/
/** Popscreen                                                              **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Popscreen[];
#else
#define MUIC_Popscreen "Popscreen.mui"
#endif

/* Attributes */

#define MUIA_Popscreen_ShowCurrent          0x804238a7 /* V11 i.. BOOL              */ /* private */
#define MUIA_Popscreen_ShowDefault          0x804295ca /* V11 i.. BOOL              */ /* private */
#define MUIA_Popscreen_ShowFrontmost        0x804261a3 /* V11 i.. BOOL              */ /* private */
#define MUIA_Popscreen_ShowMUI              0x804298bf /* V11 i.. BOOL              */ /* private */



/****************************************************************************/
/** Popasl                                                                 **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Popasl[];
#else
#define MUIC_Popasl "Popasl.mui"
#endif

/* Attributes */

#define MUIA_Popasl_Active                  0x80421b37 /* V7  ..g BOOL              */
#define MUIA_Popasl_StartHook               0x8042b703 /* V7  isg struct Hook *     */
#define MUIA_Popasl_StopHook                0x8042d8d2 /* V7  isg struct Hook *     */
#define MUIA_Popasl_Type                    0x8042df3d /* V7  i.g ULONG             */



/****************************************************************************/
/** Semaphore                                                              **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Semaphore[];
#else
#define MUIC_Semaphore "Semaphore.mui"
#endif

/* Methods */

#define MUIM_Semaphore_Attempt              0x80426ce2 /* V11 */
#define MUIM_Semaphore_AttemptShared        0x80422551 /* V11 */
#define MUIM_Semaphore_Obtain               0x804276f0 /* V11 */
#define MUIM_Semaphore_ObtainShared         0x8042ea02 /* V11 */
#define MUIM_Semaphore_Release              0x80421f2d /* V11 */
struct  MUIP_Semaphore_Attempt              { ULONG MethodID; };
struct  MUIP_Semaphore_AttemptShared        { ULONG MethodID; };
struct  MUIP_Semaphore_Obtain               { ULONG MethodID; };
struct  MUIP_Semaphore_ObtainShared         { ULONG MethodID; };
struct  MUIP_Semaphore_Release              { ULONG MethodID; };


/****************************************************************************/
/** Applist                                                                **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Applist[];
#else
#define MUIC_Applist "Applist.mui"
#endif

/* Methods */

#define MUIM_Applist_Find                   0x8042e50f /* private */ /* V11 */
#define MUIM_Applist_Forward                0x8042615c /* private */ /* V11 */
#define MUIM_Applist_GetHead                0x80420918 /* private */ /* V20 */
struct  MUIP_Applist_Find                   { ULONG MethodID; ULONG tag; char *data; }; /* private */
struct  MUIP_Applist_Forward                { ULONG MethodID; ULONG fwid; /* ... */ }; /* private */
struct  MUIP_Applist_GetHead                { ULONG MethodID; }; /* private */


/****************************************************************************/
/** Cclist                                                                 **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Cclist[];
#else
#define MUIC_Cclist "Cclist.mui"
#endif

/* Methods */

#define MUIM_Cclist_AddMCC                  0x80421bc7 /* private */ /* V16 */
#define MUIM_Cclist_Update                  0x8042f821 /* private */ /* V16 */
struct  MUIP_Cclist_AddMCC                  { ULONG MethodID; struct MUI_CustomClass *mcc; }; /* private */
struct  MUIP_Cclist_Update                  { ULONG MethodID; Object *list; }; /* private */


/****************************************************************************/
/** Dataspace                                                              **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Dataspace[];
#else
#define MUIC_Dataspace "Dataspace.mui"
#endif

/* Methods */

#define MUIM_Dataspace_Add                  0x80423366 /* V11 */
#define MUIM_Dataspace_Clear                0x8042b6c9 /* V11 */
#define MUIM_Dataspace_Compare              0x8042b393 /* private */ /* V11 */
#define MUIM_Dataspace_Filter               0x8042e075 /* private */ /* V11 */
#define MUIM_Dataspace_Find                 0x8042832c /* V11 */
#define MUIM_Dataspace_Iterate              0x80421873 /* private */ /* V11 */
#define MUIM_Dataspace_Merge                0x80423e2b /* V11 */
#define MUIM_Dataspace_ReadIFF              0x80420dfb /* V11 */
#define MUIM_Dataspace_Remove               0x8042dce1 /* V11 */
#define MUIM_Dataspace_Trim                 0x8042032e /* private */ /* V11 */
#define MUIM_Dataspace_WriteIFF             0x80425e8e /* V11 */
struct  MUIP_Dataspace_Add                  { ULONG MethodID; APTR data; LONG len; ULONG id; };
struct  MUIP_Dataspace_Clear                { ULONG MethodID; };
struct  MUIP_Dataspace_Compare              { ULONG MethodID; Object *obj; }; /* private */
struct  MUIP_Dataspace_Filter               { ULONG MethodID; ULONG id; }; /* private */
struct  MUIP_Dataspace_Find                 { ULONG MethodID; ULONG id; };
struct  MUIP_Dataspace_Iterate              { ULONG MethodID; struct DataspaceEntry **counter; }; /* private */
struct  MUIP_Dataspace_Merge                { ULONG MethodID; Object *dataspace; };
struct  MUIP_Dataspace_ReadIFF              { ULONG MethodID; struct IFFHandle *handle; };
struct  MUIP_Dataspace_Remove               { ULONG MethodID; ULONG id; };
struct  MUIP_Dataspace_Trim                 { ULONG MethodID; Object *dataspace; }; /* private */
struct  MUIP_Dataspace_WriteIFF             { ULONG MethodID; struct IFFHandle *handle; ULONG type; ULONG id; };

/* Attributes */

#define MUIA_Dataspace_Pool                 0x80424cf9 /* V11 i.. APTR              */
#define MUIA_Dataspace_UseExFilter          0x80424be8 /* V11 isg BOOL              */ /* private */
#define MUIA_Dataspace_UseFilter            0x8042b377 /* V11 isg BOOL              */ /* private */



/****************************************************************************/
/** Configdata                                                             **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Configdata[];
#else
#define MUIC_Configdata "Configdata.mui"
#endif

/* Methods */

#define MUIM_Configdata_Check               0x80421162 /* private */ /* V18 */
#define MUIM_Configdata_Get                 0x8042539a /* private */ /* V11 */
#define MUIM_Configdata_Set                 0x80428b0e /* private */ /* V11 */
struct  MUIP_Configdata_Check               { ULONG MethodID; ULONG id; }; /* private */
struct  MUIP_Configdata_Get                 { ULONG MethodID; ULONG id; ULONG *storage; }; /* private */
struct  MUIP_Configdata_Set                 { ULONG MethodID; ULONG id; ULONG value; }; /* private */

/* Attributes */

#define MUIA_Configdata_FilterID            0x80420444 /* V11 .s. LONG              */ /* private */
#define MUIA_Configdata_FilterObject        0x8042801a /* V20 .s. Object *          */ /* private */
#define MUIA_Configdata_InitGlobal          0x80425017 /* V11 i.. BOOL              */ /* private */
#define MUIA_Configdata_MasterObject        0x8042e03a /* V11 isg Object *          */ /* private */



/****************************************************************************/
/** Rootgrp                                                                **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Rootgrp[];
#else
#define MUIC_Rootgrp "Rootgrp.mui"
#endif


/****************************************************************************/
/** Audiocontrols                                                          **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Audiocontrols[];
#else
#define MUIC_Audiocontrols "Audiocontrols.mui"
#endif

/* Methods */

#define MUIM_Audiocontrols_Update           0x80427301 /* V20 */
struct  MUIP_Audiocontrols_Update           { ULONG MethodID; Object *what; };

/* Attributes */

#define MUIA_Audiocontrols_Balance          0x80421fff /* V20 isg LONG              */
#define MUIA_Audiocontrols_Channel          0x80423e7a /* V20 isg LONG              */
#define MUIA_Audiocontrols_Mute             0x80426b53 /* V20 isg BOOL              */
#define MUIA_Audiocontrols_Title            0x8042a720 /* V20 isg LONG              */
#define MUIA_Audiocontrols_Volume           0x8042015a /* V20 isg LONG              */



/****************************************************************************/
/** Audiomixer                                                             **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Audiomixer[];
#else
#define MUIC_Audiomixer "Audiomixer.mui"
#endif

/* Attributes */

#define MUIA_Audiomixer_DummyFooBar         0x80423076 /* V20 isg LONG              */



/****************************************************************************/
/** Popmenu                                                                **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Popmenu[];
#else
#define MUIC_Popmenu "Popmenu.mui"
#endif

/* Methods */

#define MUIM_Popmenu_ConstructSubMenu       0xad00200d /* private */ /* V20 */
#define MUIM_Popmenu_DestructSubMenu        0xad00200e /* private */ /* V20 */
#define MUIM_Popmenu_GetNextResult          0xad002012 /* private */ /* V20 */
#define MUIM_Popmenu_HandleIMsg             0xad00200f /* private */ /* V20 */
#define MUIM_Popmenu_Open                   0xad002010 /* private */ /* V20 */
#define MUIM_Popmenu_Select                 0xad002011 /* private */ /* V20 */
struct  MUIP_Popmenu_ConstructSubMenu       { ULONG MethodID; struct MUIP_Popmenu_Item *entry; }; /* private */
struct  MUIP_Popmenu_DestructSubMenu        { ULONG MethodID; }; /* private */
struct  MUIP_Popmenu_GetNextResult          { ULONG MethodID; }; /* private */
struct  MUIP_Popmenu_HandleIMsg             { ULONG MethodID; struct IntuiMessage *imsg; }; /* private */
struct  MUIP_Popmenu_Open                   { ULONG MethodID; struct Window *window; }; /* private */
struct  MUIP_Popmenu_Select                 { ULONG MethodID; struct MUIP_Popmenu_Item *entry; }; /* private */

/* Attributes */

#define MUIA_Popmenu_Active                 0xad002008 /* V20 .s. UWORD             */ /* private */
#define MUIA_Popmenu_AppObj                 0xad00200c /* V20 .s. Object *          */ /* private */
#define MUIA_Popmenu_Menu                   0xad002001 /* V20 .s. struct MUIP_Popmenu_Item * */ /* private */
#define MUIA_Popmenu_MinWidth               0xad002002 /* V20 .s. UWORD             */ /* private */
#define MUIA_Popmenu_MultiSelect            0xad002003 /* V20 .s. BOOL              */ /* private */
#define MUIA_Popmenu_Parent                 0xad002004 /* V20 .s. Object *          */ /* private */
#define MUIA_Popmenu_PosX                   0xad002005 /* V20 .s. UWORD             */ /* private */
#define MUIA_Popmenu_PosX_Type              0xad002006 /* V20 .s. ULONG             */ /* private */
#define MUIA_Popmenu_PosY                   0xad002007 /* V20 .s. UWORD             */ /* private */
#define MUIA_Popmenu_PosY_Type              0xad002009 /* V20 .s. ULONG             */ /* private */
#define MUIA_Popmenu_ShadowThickness        0xad002013 /* V20 .s. UWORD             */ /* private */
#define MUIA_Popmenu_StayPressed            0xad00200a /* V20 .s. BOOL              */ /* private */
#define MUIA_Popmenu_Stickyness             0xad002014 /* V20 .s. UWORD             */ /* private */
#define MUIA_Popmenu_Type                   0xad00200b /* V20 .s. BOOL              */ /* private */

#define MUIV_Popmenu_Active_First -3
#define MUIV_Popmenu_Active_Last -2
#define MUIV_Popmenu_Active_None -1
#define MUIV_Popmenu_PosX_Type_Center 0
#define MUIV_Popmenu_PosX_Type_Left 1
#define MUIV_Popmenu_PosX_Type_Right 2
#define MUIV_Popmenu_PosY_Type_Middle 0
#define MUIV_Popmenu_PosY_Type_Top 1
#define MUIV_Popmenu_PosY_Type_Bottom 2
#define MUIV_Popmenu_PosY_Type_Active 3
#define MUIV_Popmenu_Stickyness_Auto 0
#define MUIV_Popmenu_Stickyness_Always 1
#define MUIV_Popmenu_Stickyness_Never 2
#define MUIV_Popmenu_Type_Context 0
#define MUIV_Popmenu_Type_Menu 1
#define MUIV_Popmenu_Type_Cycle 2
#define MUIV_Popmenu_Type_Popup 3
#define MUIV_Popmenu_Type_Other 255


/*****************************************/
/* End of automatic header file creation */
/*****************************************/







/*************************************************************************
** Structures and Macros for creating custom classes.
*************************************************************************/


/*
** GENERAL NOTES:
**
** - Everything described in this header file is only valid within
**   MUI classes. You may never use any of these things out of
**   a class, e.g. in a traditional MUI application.
**
** - Except when otherwise stated, all structures are strictly read only.
*/


/* Global information for every object */

struct MUI_GlobalInfo
{
	Object *mgi_ConfigData; /* private4 */
	Object *mgi_ApplicationObject;

	/* ... private data follows ... */
};


/* Instance data of notify class */

struct MUI_NotifyData
{
	struct MUI_GlobalInfo *mnd_GlobalInfo;
	ULONG                  mnd_UserData;
	ULONG                  mnd_ObjectID;
	struct MinList *       mnd_NotifyList; /* private4 */
	Object *               mnd_Parent;     /* private4 */
	char *                 mnd_HelpNode;   /* private4 */
	ULONG                  mnd_HelpLine;   /* private4 */
};


/* MUI_MinMax structure holds information about minimum, maximum
   and default dimensions of an object. */

struct MUI_MinMax
{
	WORD MinWidth;
	WORD MinHeight;
	WORD MaxWidth;
	WORD MaxHeight;
	WORD DefWidth;
	WORD DefHeight;
};

#define MUI_MAXMAX 10000 /* use this if a dimension is not limited. */


/* Hook message for custom layout */

struct MUI_LayoutMsg
{
	ULONG                  lm_Type;     /* type of message (see defines below)                      */
	struct MinList        *lm_Children; /* list of this groups children, traverse with NextObject() */
	struct MUI_MinMax      lm_MinMax;   /* results for MUILM_MINMAX                                 */
	struct
	{
		LONG Width;
		LONG Height;
		LONG OffsetX; /* private4 */
		LONG OffsetY; /* private4 */
	} lm_Layout;   /* size (and result) for MUILM_LAYOUT                       */
};

#define MUILM_MINMAX    1  /* MUI wants you to calc your min & max sizes */
#define MUILM_LAYOUT    2  /* MUI wants you to layout your children      */

#define MUILM_UNKNOWN  -1  /* return this if your hook doesn't implement lm_Type */


/* (partial) instance data of area class */

struct MUI_AreaData
{
	struct MUI_RenderInfo *mad_RenderInfo;     /* RenderInfo for this object */
	struct MUI_Image      *mad_Background;     /* private4 */
	struct TextFont       *mad_Font;           /* Font */
	struct MUI_MinMax      mad_MinMax;         /* min/max/default sizes */
	struct IBox            mad_Box;            /* position and dimension */
	BYTE                   mad_addleft;        /* frame & innerspacing left offset */
	BYTE                   mad_addtop;         /* frame & innerspacing top offset  */
	BYTE                   mad_subwidth;       /* frame & innerspacing add. width  */
	BYTE                   mad_subheight;      /* frame & innerspacing add. height */
	ULONG                  mad_Flags;          /* see definitions below */
	WORD                   mad_HorizWeight;    /* private */
	WORD                   mad_VertWeight;     /* private */
	WORD                   mad_HorizDisappear; /* private */
	WORD                   mad_VertDisappear;  /* private */
	ULONG                  mad_IDCMPFlags;     /* private */
	STRPTR                 mad_BackSpec;       /* private */
	struct TextFont       *mad_FontSpec;       /* private */
	CONST_STRPTR           mad_ShortHelp;      /* private */
	LONG                   mad_FixWidth;       /* private */
	LONG                   mad_FixHeight;      /* private */
	LONG                   mad_VirtualTop;     /* private */
	ULONG                  mad_Flags2;         /* private */

	/* ... private data follows ... */
};

/* Definitions for mad_Flags, other flags are private */

#define MADF_DRAWOBJECT        (1<< 0) /* completely redraw yourself */
#define MADF_DRAWUPDATE        (1<< 1) /* only update yourself */
#define MADF_DRAWACTIVE        (1<< 2) /* private */
#define MADF_DRAGGABLE         (1<< 3) /* private */
#define MADF_CUSTOMBACKFILL    (1<< 4) /* private */
#define MADF_CYCLECHAIN        (1<< 5) /* private */
#define MADF_FIXMEANSMAX       (1<< 6) /* private */
#define MADF_KNOWSACTIVE       (1<< 7) /* private */
#define MADF_GROUP             (1<< 8) /* private */
#define MADF_SHOWME            (1<< 9) /* private */
#define MADF_PARTIAL           (1<<10) /* private */
#define MADF_DRAWOUTER         (1<<11) /* private */
#define MADF_DRAWCHILD         (1<<12) /* private */
#define MADF_DROPABLE          (1<<13) /* private */
#define MADF_VISIBLE           (1<<14) /* private */
#define MADF_DISABLED          (1<<15) /* private */
#define MADF_SHOWSELSTATE      (1<<16) /* private */
#define MADF_PRESSED           (1<<17) /* private */
#define MADF_SELECTED          (1<<18) /* private */
#define MADF_PAGETITLES        (1<<19) /* private */
#define MADF_FILLAREA          (1<<20) /* private */
#define MADF_FIXWIDTHTXT       (1<<21) /* private */
#define MADF_FIXHEIGHTTXT      (1<<22) /* private */
#define MADF_INNERLEFTGIVEN    (1<<23) /* private */
#define MADF_INNERTOPGIVEN     (1<<24) /* private */
#define MADF_INNERRIGHTGIVEN   (1<<25) /* private */
#define MADF_INNERBOTTOMGIVEN  (1<<26) /* private */
#define MADF_FRAMEPHANTOMHORIZ (1<<27) /* private */
#define MADF_DRAWDROPBOX       (1<<28) /* private */
#define MADF_INVIRTUAL         (1<<29) /* private */
#define MADF_VIRTUAL           (1<<30) /* private */
#define MADF_FRAMEOFFSET       (1<<31) /* private */

#define MADF_DRAWALL    (MADF_DRAWACTIVE | MADF_DRAWOUTER | MADF_DRAWOBJECT) /* private */
#define MADF_DRAWMASK   (MADF_DRAWOUTER | MADF_DRAWOBJECT | MADF_DRAWCHILD | MADF_DRAWUPDATE | MADF_DRAWACTIVE) /* private */

#define MADF2_CUSTOMFONT (1<< 0) /* private */


/* MUI's draw pens */

#define MPEN_SHINE      0
#define MPEN_HALFSHINE  1
#define MPEN_BACKGROUND 2
#define MPEN_HALFSHADOW 3
#define MPEN_SHADOW     4
#define MPEN_TEXT       5
#define MPEN_FILL       6
#define MPEN_MARK       7
#define MPEN_COUNT      8


/* Mask for pens from MUI_ObtainPen() */

#ifdef MBX /* private */
#define MUIPEN_MASK 0xffffffff /* private */
#define MUIPEN(pen) (pen) /* private */
#else /* private */
#define MUIPEN_MASK 0x0000ffff
#define MUIPEN(pen) ((pen) & MUIPEN_MASK)
#endif /* private */

/* Information on display environment */

#define MAXCLIPCOUNT 10 /* private */
struct MUI_RenderInfo
{
	Object          *mri_WindowObject;  /* valid between MUIM_Setup/MUIM_Cleanup */

	struct Screen   *mri_Screen;        /* valid between MUIM_Setup/MUIM_Cleanup */
	struct DrawInfo *mri_DrawInfo;      /* valid between MUIM_Setup/MUIM_Cleanup */
	#ifdef MBX /* private */
	LONG *mri_Pens; /* private */
	#else /* private */
	UWORD           *mri_Pens;          /* valid between MUIM_Setup/MUIM_Cleanup */
	#endif /* private */
	struct Window   *mri_Window;        /* valid between MUIM_Show/MUIM_Hide */
	struct RastPort *mri_RastPort;      /* valid between MUIM_Show/MUIM_Hide */

	ULONG            mri_Flags;         /* valid between MUIM_Setup/MUIM_Cleanup */

	struct MUI_ScreenInfo    *mri_ScreenInfo;           /* private */
	struct TextFont          *mri_ListFont;             /* private */
	struct TextFont          *mri_NormalFont;           /* private */
	struct TextFont          *mri_TinyFont;             /* private */
	struct TextFont          *mri_FixedFont;            /* private */
	struct TextFont          *mri_TitleFont;            /* private */
	struct TextFont          *mri_BigFont;              /* private */
	struct TextFont          *mri_ButtonFont;           /* private */
	struct TextFont          *mri_SliderFont;           /* private */
	APTR                      mri_SkipBackground;       /* private */
	LONG                      mri_ClipCount;            /* private */
	Object                   *mri_ConfigData;           /* private */
	Object                   *mri_ActiveObject;         /* private */
	LONG                      mri_ActiveObjectPen;      /* private */
	struct Region            *mri_ClipStack[MAXCLIPCOUNT]; /* private */
	struct TextFont          *mri_GaugeFont;            /* private */
	#ifdef USE_CGXHOOKS                   /* private */
	struct CGXHook *mri_ColorImposeHook;  /* private */
	struct CGXHook *mri_BitMapImposeHook; /* private */
	struct CGXHook *mri_BalanceHook;      /* private */
	#endif                                /* private */
	struct TextFont          *mri_MenudisplayFont;            /* private */
	#ifdef MBX /* private */
	LONG mri_PenArray[MPEN_COUNT]; /* private */
	#else /* private */
	WORD mri_PenArray[MPEN_COUNT]; /* private */
	#endif /* private */
	struct RastPort          *mri_FakeRP;               /* private */
	#ifdef USE_NEWREGIONS /* private */
	RegionHeap_p mri_RegionHeap; /* private */
	struct Rectangle mri_RefreshRect; /* private */
	#endif /* private */
	/* ... private data follows ... */
};

/*
** If mri_Flags & MUIMRI_RECTFILL, RectFill() is quicker
** than Move()/Draw() for horizontal or vertical lines.
** on the current display.
*/
#define MUIMRI_RECTFILL (1<<0)

/*
** If mri_Flags & MUIMRI_TRUECOLOR, display environment is a
** cybergraphics emulated hicolor or true color display.
*/
#define MUIMRI_TRUECOLOR (1<<1)

/*
** If mri_Flags & MUIMRI_THINFRAMES, MUI uses thin frames
** (1:1) apsect ratio instead of standard 2:1 frames.
*/
#define MUIMRI_THINFRAMES (1<<2)

/*
** If mri_Flags & MUIMRI_REFRESHMODE, MUI is currently
** refreshing a WFLG_SIMPLEREFRESH window and is inbetween
** a BeginRefresh()/EndRefresh() pair.
*/
#define MUIMRI_REFRESHMODE (1<<3)

/*
** If mri_Flags & MUIMRI_PLANAR, display environment is an
** old-style planar display.
*/
#define MUIMRI_PLANAR (1<<4)

#define MUIMRI_RESIZEREDRAW (1<<5) /* private */
#define MUIMRI_CLONE (1<<6) /* private */


/* the following macros can be used to get pointers to an objects
   GlobalInfo and RenderInfo structures. */

struct __dummyXFC2__
{
	struct MUI_NotifyData mnd;
	struct MUI_AreaData   mad;
};

#define muiNotifyData(obj) (&(((struct __dummyXFC2__ *)(obj))->mnd))
#define muiAreaData(obj)   (&(((struct __dummyXFC2__ *)(obj))->mad))

#define muiGlobalInfo(obj) (((struct __dummyXFC2__ *)(obj))->mnd.mnd_GlobalInfo)
#define muiUserData(obj)   (((struct __dummyXFC2__ *)(obj))->mnd.mnd_UserData)
#define muiRenderInfo(obj) (((struct __dummyXFC2__ *)(obj))->mad.mad_RenderInfo)



/* User configurable keyboard events coming with MUIM_HandleInput */

enum
{
	MUIKEY_RELEASE = -2, /* not a real key, faked when MUIKEY_PRESS is released */
	MUIKEY_NONE    = -1,
	MUIKEY_PRESS,
	MUIKEY_TOGGLE,
	MUIKEY_UP,
	MUIKEY_DOWN,
	MUIKEY_PAGEUP,
	MUIKEY_PAGEDOWN,
	MUIKEY_TOP,
	MUIKEY_BOTTOM,
	MUIKEY_LEFT,
	MUIKEY_RIGHT,
	MUIKEY_WORDLEFT,
	MUIKEY_WORDRIGHT,
	MUIKEY_LINESTART,
	MUIKEY_LINEEND,
	MUIKEY_GADGET_NEXT,
	MUIKEY_GADGET_PREV,
	MUIKEY_GADGET_OFF,
	MUIKEY_WINDOW_CLOSE,
	MUIKEY_WINDOW_NEXT,
	MUIKEY_WINDOW_PREV,
	MUIKEY_HELP,
	MUIKEY_POPUP,
	MUIKEY_PRESS2,
	MUIKEY_COUNT /* counter */
};

#define MUIKEYF_PRESS        (1<<MUIKEY_PRESS)
#define MUIKEYF_TOGGLE       (1<<MUIKEY_TOGGLE)
#define MUIKEYF_UP           (1<<MUIKEY_UP)
#define MUIKEYF_DOWN         (1<<MUIKEY_DOWN)
#define MUIKEYF_PAGEUP       (1<<MUIKEY_PAGEUP)
#define MUIKEYF_PAGEDOWN     (1<<MUIKEY_PAGEDOWN)
#define MUIKEYF_TOP          (1<<MUIKEY_TOP)
#define MUIKEYF_BOTTOM       (1<<MUIKEY_BOTTOM)
#define MUIKEYF_LEFT         (1<<MUIKEY_LEFT)
#define MUIKEYF_RIGHT        (1<<MUIKEY_RIGHT)
#define MUIKEYF_WORDLEFT     (1<<MUIKEY_WORDLEFT)
#define MUIKEYF_WORDRIGHT    (1<<MUIKEY_WORDRIGHT)
#define MUIKEYF_LINESTART    (1<<MUIKEY_LINESTART)
#define MUIKEYF_LINEEND      (1<<MUIKEY_LINEEND)
#define MUIKEYF_GADGET_NEXT  (1<<MUIKEY_GADGET_NEXT)
#define MUIKEYF_GADGET_PREV  (1<<MUIKEY_GADGET_PREV)
#define MUIKEYF_GADGET_OFF   (1<<MUIKEY_GADGET_OFF)
#define MUIKEYF_WINDOW_CLOSE (1<<MUIKEY_WINDOW_CLOSE)
#define MUIKEYF_WINDOW_NEXT  (1<<MUIKEY_WINDOW_NEXT)
#define MUIKEYF_WINDOW_PREV  (1<<MUIKEY_WINDOW_PREV)
#define MUIKEYF_HELP         (1<<MUIKEY_HELP)
#define MUIKEYF_POPUP        (1<<MUIKEY_POPUP)
#define MUIKEYF_PRESS2       (1<<MUIKEY_PRESS2)


/* Some useful shortcuts. define MUI_NOSHORTCUTS to get rid of them */
/* NOTE: These macros may only be used in custom classes and are    */
/* only valid if your class is inbetween the specified methods!     */

#ifndef MUI_NOSHORTCUTS

#define _app(obj)         (muiGlobalInfo(obj)->mgi_ApplicationObject) /* valid between MUIM_Setup/Cleanup */
#define _cfg(obj)         (muiGlobalInfo(obj)->mgi_ConfigData)        /* private */
#define _win(obj)         (muiRenderInfo(obj)->mri_WindowObject)      /* valid between MUIM_Setup/Cleanup */
#define _dri(obj)         (muiRenderInfo(obj)->mri_DrawInfo)          /* valid between MUIM_Setup/Cleanup */
#define _screen(obj)      (muiRenderInfo(obj)->mri_Screen)            /* valid between MUIM_Setup/Cleanup */
#define _pens(obj)        (muiRenderInfo(obj)->mri_Pens)              /* valid between MUIM_Setup/Cleanup */
#define _window(obj)      (muiRenderInfo(obj)->mri_Window)            /* valid between MUIM_Show/Hide */
#define _rp(obj)          (muiRenderInfo(obj)->mri_RastPort)          /* valid between MUIM_Show/Hide */
#define _left(obj)        (muiAreaData(obj)->mad_Box.Left)            /* valid during MUIM_Draw */
#define _top(obj)         (muiAreaData(obj)->mad_Box.Top)             /* valid during MUIM_Draw */
#define _width(obj)       (muiAreaData(obj)->mad_Box.Width)           /* valid during MUIM_Draw */
#define _height(obj)      (muiAreaData(obj)->mad_Box.Height)          /* valid during MUIM_Draw */
#define _right(obj)       (_left(obj)+_width(obj)-1)                  /* valid during MUIM_Draw */
#define _bottom(obj)      (_top(obj)+_height(obj)-1)                  /* valid during MUIM_Draw */
#define _addleft(obj)     (muiAreaData(obj)->mad_addleft  )           /* valid during MUIM_Draw */
#define _addtop(obj)      (muiAreaData(obj)->mad_addtop   )           /* valid during MUIM_Draw */
#define _subwidth(obj)    (muiAreaData(obj)->mad_subwidth )           /* valid during MUIM_Draw */
#define _subheight(obj)   (muiAreaData(obj)->mad_subheight)           /* valid during MUIM_Draw */
#define _fakerp(obj)      (&muiRenderInfo(obj)->mri_FakeRP) /* private */
#define _size(obj,d)      (((struct MUI_Size *)&((muiAreaData(obj)->mad_Box.Width)))->size[d]) /* private */
#define _ltpos(obj,d)     (((struct MUI_Pos  *)&((muiAreaData(obj)->mad_Box.Left )))->pos[d] ) /* private */
#define _ltadd(obj,d)     (((struct MUI_Add  *)&((muiAreaData(obj)->mad_addleft  )))->add[d] ) /* private */
#define _whsub(obj,d)     (((struct MUI_Add  *)&((muiAreaData(obj)->mad_subwidth )))->add[d] ) /* private */
#define _msize(obj,d)     (_size(obj,d)-_whsub(obj,d))                                         /* private */
#define _mltpos(obj,d)    (_ltpos(obj,d)+_ltadd(obj,d))                                        /* private */
#define _mrbpos(obj,d)    (_mltpos(obj,d)+_msize(obj,d)-1)                                     /* private */
#define _vleft(obj)       (_left(obj))                       /* private */
#define _vtop(obj)        (muiAreaData(obj)->mad_VirtualTop) /* private */
#define _vright(obj)      (_vleft(obj)+_width(obj)-1)        /* private */
#define _vbottom(obj)     (_vtop(obj)+_height(obj)-1)        /* private */
#define _vmleft(obj)      (_vleft(obj)+_addleft(obj))        /* private */
#define _vmtop(obj)       (_vtop(obj)+_addtop(obj))          /* private */
#define _vmright(obj)     (_vmleft(obj)+_mwidth(obj)-1)      /* private */
#define _vmbottom(obj)    (_vmtop(obj)+_mheight(obj)-1)      /* private */
#define _mleft(obj)       (_left(obj)+_addleft(obj))                  /* valid during MUIM_Draw */
#define _mtop(obj)        (_top(obj)+_addtop(obj))                    /* valid during MUIM_Draw */
#define _mwidth(obj)      (_width(obj)-_subwidth(obj))                /* valid during MUIM_Draw */
#define _mheight(obj)     (_height(obj)-_subheight(obj))              /* valid during MUIM_Draw */
#define _mright(obj)      (_mleft(obj)+_mwidth(obj)-1)                /* valid during MUIM_Draw */
#define _mbottom(obj)     (_mtop(obj)+_mheight(obj)-1)                /* valid during MUIM_Draw */
#define _font(obj)        (muiAreaData(obj)->mad_Font)                /* valid between MUIM_Setup/Cleanup */
#define _minwidth(obj)    (muiAreaData(obj)->mad_MinMax.MinWidth)     /* valid between MUIM_Show/Hide */
#define _minheight(obj)   (muiAreaData(obj)->mad_MinMax.MinHeight)    /* valid between MUIM_Show/Hide */
#define _maxwidth(obj)    (muiAreaData(obj)->mad_MinMax.MaxWidth)     /* valid between MUIM_Show/Hide */
#define _maxheight(obj)   (muiAreaData(obj)->mad_MinMax.MaxHeight)    /* valid between MUIM_Show/Hide */
#define _defwidth(obj)    (muiAreaData(obj)->mad_MinMax.DefWidth)     /* valid between MUIM_Show/Hide */
#define _defheight(obj)   (muiAreaData(obj)->mad_MinMax.DefHeight)    /* valid between MUIM_Show/Hide */
#define _minmax(obj)      (&(muiAreaData(obj)->mad_MinMax)) /* private */
#define _minsize(obj,dim) (((struct MUI_MinMaxArray *)&(muiAreaData(obj)->mad_MinMax))->MinSize[dim]) /* private */
#define _maxsize(obj,dim) (((struct MUI_MinMaxArray *)&(muiAreaData(obj)->mad_MinMax))->MaxSize[dim]) /* private */
#define _defsize(obj,dim) (((struct MUI_MinMaxArray *)&(muiAreaData(obj)->mad_MinMax))->DefSize[dim]) /* private */
#define _hweight(obj)     (muiAreaData(obj)->mad_HorizWeight) /* private */
#define _vweight(obj)     (muiAreaData(obj)->mad_VertWeight) /* private */
#define _weight(obj,dim)  (((struct MUI_Weight *)&((muiAreaData(obj)->mad_HorizWeight)))->weight[dim]) /* private */
#define _idcmpflags(obj)  (muiAreaData(obj)->mad_IDCMPFlags) /* private */
#define _gidcmpflags(obj) (_groupdata(obj)->gidcmp) /* private */
#define _background(obj)  (muiAreaData(obj)->mad_Background) /* private */
#define _backspec(obj)    (muiAreaData(obj)->mad_BackSpec) /* private */
#define _fontspec(obj)    (muiAreaData(obj)->mad_FontSpec) /* private */
#define _hdis(obj)        (muiAreaData(obj)->mad_HorizDisappear) /* private */
#define _vdis(obj)        (muiAreaData(obj)->mad_VertDisappear) /* private */
#define _shorthelp(obj)   (muiAreaData(obj)->mad_ShortHelp) /* private */
#define _parent(obj)      (muiNotifyData(obj)->mnd_Parent) /* private */
#define _helpnode(obj)    (muiNotifyData(obj)->mnd_HelpNode) /* private */
#define _helpline(obj)    (muiNotifyData(obj)->mnd_HelpLine) /* private */
#define _flags(obj)       (muiAreaData(obj)->mad_Flags)
#define _controlchar(obj) (((struct AreaData *)muiAreaData(obj))->ControlChar) /* private */
#define _contextmenu(obj) (((struct AreaData *)muiAreaData(obj))->ContextMenu) /* private */
#define _isvisible(obj)   (_flags(obj) & MADF_VISIBLE) /* private */
#define _isdisabled(obj)  (_flags(obj) & MADF_DISABLED) /* private */
#define _isinvirtual(obj) (_flags(obj) & MADF_INVIRTUAL) /* private */
#define _isvirtual(obj)   (_flags(obj) & MADF_VIRTUAL) /* private */
#define _isselected(obj)  (_flags(obj) & MADF_SELECTED) /* private */
#define _isgroup(obj)     (_flags(obj) & MADF_GROUP) /* private */
#define _isdraggable(obj) (_flags(obj) & MADF_DRAGGABLE) /* private */
#define _isdropable(obj)  (_flags(obj) & MADF_DROPABLE) /* private */
#define _ispartial(obj)   (_flags(obj) & MADF_PARTIAL) /* private */
#define _needslayout(obj) (_idcmpflags(obj) & IDCMP_LONELYMESSAGE) /* private */

#endif

#define _get(x,f)        ((x) & (f)) /* private */
#define _set(x,f)        (x)|=(f) /* private */
#define _clr(x,f)        (x)&=~(f) /* private */
#define _setclr(x,b,f)   if (b) _set(x,f); else _clr(x,f) /* private */


/* MUI_CustomClass returned by MUI_CreateCustomClass() */

struct MUI_CustomClass
{
	APTR mcc_UserData;                  /* use for whatever you want */

	struct Library *mcc_UtilityBase;    /* MUI has opened these libraries */
	struct Library *mcc_DOSBase;        /* for you automatically. You can */
	struct Library *mcc_GfxBase;        /* use them or decide to open     */
	struct Library *mcc_IntuitionBase;  /* your libraries yourself.       */

	struct IClass *mcc_Super;           /* pointer to super class   */
	struct IClass *mcc_Class;           /* pointer to the new class */

	#ifdef __MORPHOS__                      /* private */
	struct EmulLibEntry mcc_GateDispatcher; /* private */
	#endif                                  /* private */

	/* ... private data follows ... */
};


#define MUI_GOACTIVE_KEYFOCUS (1<<0) /* private */

#if defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack()
  #elif defined(__VBCC__)
    #pragma default-align
  #endif
#endif

#endif /* MUI_H */
