#ifndef uiWin_H_
#define uiWin_H_

/** @file
    uiWin, Windows implementaion
*/

#include "uidefs.h"
#include "uiApp.h"
#include "uiMenu.h"

class ptGC;

/** @addtogroup uiWin Window

    The uiWin class wraps a window and its handle (uiWINH).
    A window has an optional title bar and can receive messages.
  
    @par Window Message Handling

    Each window class can provide its own message handler, by overriding
    uiWin::HandleMsg().

@{*/

/** uiWin window styles. */
enum uiWS
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    uiWS_BORDER       = WS_BORDER,          //!< Thin border
    uiWS_CAPTION      = WS_CAPTION,         //!< Window has a title bar, includes uiWS_BORDER
    uiWS_CHILD        = WS_CHILD,           //!< Child window, cannot have title bar, mutually exclusive with uiWS_POPUP
    uiWS_CLIPCHILDREN = WS_CLIPCHILDREN,    //!< Do not redraw area covered by child windows, see uiWM_PAINT
    uiWS_CLIPSIBLINGS = WS_CLIPSIBLINGS,    //!< Do not redraw area covered by other sibling windows, see uiWM_PAINT
    uiWS_DISABLED     = WS_DISABLED,        //!< Window is disabled and cannot receive input, see uiWin::Enable()
    uiWS_DLGFRAME     = WS_DLGFRAME,        //!< Dialog frame border
    uiWS_GROUP        = WS_GROUP,           //!< Marks window as part of a group, usually used to TAB through input controls of a dialog
    uiWS_HSCROLL      = WS_HSCROLL,         //!< Window has horizontal scrollbar
    uiWS_MINIMIZE     = WS_MINIMIZE,        //!< Window is iconized
    uiWS_MINIMIZEBOX  = WS_MINIMIZEBOX,     //!< Window had minimize box
    uiWS_MAXIMIZE     = WS_MAXIMIZE,        //!< Window is maximized
    uiWS_MAXIMIZEBOX  = WS_MAXIMIZEBOX,     //!< Window has maximize box
    uiWS_POPUP        = WS_POPUP,           //!< Popup window, mutually exclusive with uiWS_CHILD
    uiWS_SIZEBOX      = WS_SIZEBOX|WS_THICKFRAME,//!< Sizing border or box
    uiWS_SYSMENU      = WS_SYSMENU,         //!< Window has system menu, must be used with uiWS_CAPTION
    uiWS_TABSTOP      = WS_TABSTOP,         //!< Window is stop point for Tab navigation
    uiWS_VISIBLE      = WS_VISIBLE,         //!< Window is visiable, see uiWin::Show()
    uiWS_VSCROLL      = WS_VSCROLL,         //!< Window has horizontal scrollbar
#else
#endif
    /** Shortcut for style mask for overlapped windows such as top level windows */
    uiWS_OVERLAPPEDWINDOW = uiWS_CAPTION | uiWS_SYSMENU | uiWS_SIZEBOX | uiWS_MINIMIZEBOX | uiWS_MAXIMIZEBOX,
    /** Shortcut for style mask for popup windows */
    uiWS_POPUPWINDOW = uiWS_POPUP | uiWS_BORDER | uiWS_SYSMENU,
};

/** uiWin window extended styles. */
enum uiWSE
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    uiWSE_ACCEPTFILES   = WS_EX_ACCEPTFILES,    //!< Window accepts to drag&drop files
    uiWSE_CLIENTEDGE    = WS_EX_CLIENTEDGE,     //!< Sunken border
    uiWSE_DLGMODALFRAME = WS_EX_DLGMODALFRAME,  //!< Modal (double) border
    uiWSE_STATICEDGE    = WS_EX_STATICEDGE,     //!< Static (3D) border
    uiWSE_TOOLWINDOW    = WS_EX_TOOLWINDOW,     //!< Tool window, has a smaller title bar
    uiWSE_TRANSPARENT   = WS_EX_TRANSPARENT,    //!< Window received uiWM_PAINT message only until after child windows have been drawn
    uiWSE_WINDOWEDGE    = WS_EX_WINDOWEDGE,     //!< Raised border
#else
#endif
};

/** uiWin::Show command IDs. */
enum uiWINSHOW
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    uiWINSHOW_SHOW    = SW_SHOW,
    uiWINSHOW_DEFAULT = SW_SHOWDEFAULT,
    uiWINSHOW_HIDE    = SW_HIDE,
    uiWINSHOW_RESTORE = SW_RESTORE
#else
#endif
};

/** uiWin::GetScrollBar and uiWin::SetScrollBar scrollbar type. */
enum uiWINSBTYPE
{
    uiWINSBTYPE_HORZ = SB_HORZ,
    uiWINSBTYPE_VERT = SB_VERT
};

/** uiWinScroll::mask scrollbar flags. */
enum uiWINSBFLAG
{
    uiWINSBFLAG_PAGE            = SIF_PAGE,
    uiWINSBFLAG_POS             = SIF_POS,
    uiWINSBFLAG_RANGE           = SIF_RANGE,
    uiWINSBFLAG_TRACKPOS        = SIF_TRACKPOS,
    uiWINSBFLAG_DISABLENOSCROLL = SIF_DISABLENOSCROLL,
    uiWINSBFLAG_ALL = (uiWINSBFLAG_PAGE | uiWINSBFLAG_POS | uiWINSBFLAG_RANGE | uiWINSBFLAG_TRACKPOS)
};

/** uiWM_VSCROLL and uiWM_HSCROLL message scroll reason codes. */
enum uiWMSB
{
    uiWMSB_BOTTOM        = SB_BOTTOM,       //!< Scrolls to bottom-right.
    uiWMSB_ENDSCROLL     = SB_ENDSCROLL,    //!< End scroll tracking, allows application to perform final heavy drawing.
    uiWMSB_LINEDOWN      = SB_LINEDOWN,     //!< Scrolls one line down.
    uiWMSB_LINEUP        = SB_LINEUP,       //!< Scrolls one line up.
    uiWMSB_PAGEDOWN      = SB_PAGEDOWN,     //!< Scrolls one page down.
    uiWMSB_PAGEUP        = SB_PAGEUP,       //!< Scrolls one page up.
    uiWMSB_THUMBPOSITION = SB_THUMBPOSITION,//!< User dragged the scroll box and released the mouse button. 
    uiWMSB_THUMBTRACK    = SB_THUMBTRACK,   //!< User is dragging the scroll box, sent repeatedly.
    uiWMSB_TOP           = SB_TOP           //!< Scrolls to the upper left.
};
    
/** uiWin::GetScrollBar and uiWin::SetScrollBar scrollbar info.
    This is defined to map on Win32 SCROLLINFO struct
*/
typedef struct
{ 
    bbUINT reserved0;   //!< Reserved, maps on SCROLLINFO::cbSize
    bbUINT mask;        //!< Flag bitmask, see uiWINSBFLAG, only used for uiWin::SetScrollBar.
    int    min;         //!< Scroll range minimum value
    int    max;         //!< Scroll range maximum value
    bbUINT pagesize;    //!< Scroll page size
    int    pos;         //!< Current scroll position
    int    trackpos;    //!< Current scroll position during track

} uiWinScroll;

enum uiMOUSEWHEEL
{
    uiMOUSEWHEEL_DELTA = WHEEL_DELTA
};

/** Virtual keycodes for the uiWM_KEYDOWN and uiWM_KEYUP messages. */
enum uiVK
{
    uiVK_NONE      = 0,
    uiVKMASK       = 255,

    uiVK_PAGEUP    = VK_PRIOR,
    uiVK_PAGEDOWN  = VK_NEXT,
    uiVK_HOME      = VK_HOME,
    uiVK_END       = VK_END,
    uiVK_INS       = VK_INSERT,
    uiVK_BACKSPACE = VK_BACK,
    uiVK_DEL       = VK_DELETE,
    uiVK_LEFT      = VK_LEFT,
    uiVK_UP        = VK_UP,
    uiVK_RIGHT     = VK_RIGHT,
    uiVK_DOWN      = VK_DOWN, 
    uiVK_TAB       = VK_TAB,
    uiVK_RETURN    = VK_RETURN,
    uiVK_SPACE     = VK_SPACE,
    uiVK_SHIFT     = VK_SHIFT,
    uiVK_CONTROL   = VK_CONTROL,
    uiVK_ALT       = VK_MENU,
    uiVK_PRINT     = VK_PRINT,
    uiVK_SNAPSHOT  = VK_SNAPSHOT,
    uiVK_PAUSE     = VK_PAUSE,
    uiVK_CAPSLOCK  = VK_CAPITAL,
    uiVK_NUMLOCK   = VK_NUMLOCK,
    uiVK_SCROLL    = VK_SCROLL,
    uiVK_HELP      = VK_HELP,
    uiVK_F1        = VK_F1,
    uiVK_F2        = VK_F2,
    uiVK_F3        = VK_F3,
    uiVK_F4        = VK_F4,
    uiVK_F5        = VK_F5,
    uiVK_F6        = VK_F6,
    uiVK_F7        = VK_F7,
    uiVK_F8        = VK_F8,
    uiVK_F9        = VK_F9,
    uiVK_F10       = VK_F10,
    uiVK_F11       = VK_F11,
    uiVK_F12       = VK_F12,

    uiVK_ESCAPE    = VK_ESCAPE,
    uiVK_NUM0      = VK_NUMPAD0,
    uiVK_NUM1      = VK_NUMPAD1,
    uiVK_NUM2      = VK_NUMPAD2,
    uiVK_NUM3      = VK_NUMPAD3,
    uiVK_NUM4      = VK_NUMPAD4,
    uiVK_NUM5      = VK_NUMPAD5,
    uiVK_NUM6      = VK_NUMPAD6,
    uiVK_NUM7      = VK_NUMPAD7,
    uiVK_NUM8      = VK_NUMPAD8,
    uiVK_NUM9      = VK_NUMPAD9,
    uiVK_NUMMUL    = VK_MULTIPLY,
    uiVK_NUMADD    = VK_ADD,
    uiVK_NUMSEP    = VK_SEPARATOR,
    uiVK_NUMSUB    = VK_SUBTRACT,
    uiVK_NUMDIV    = VK_DIVIDE,
    uiVK_DECIMAL   = VK_DECIMAL, 
    uiVK_NUMRET     = VK_OEM_NEC_EQUAL,
    uiVK_SEMICOLON = VK_OEM_1,      // ';:' for US
    uiVK_PLUS      = VK_OEM_PLUS,   // '+' any country
    uiVK_COMMA     = VK_OEM_COMMA,  // ',' any country
    uiVK_MINUS     = VK_OEM_MINUS,  // '-' any country
    uiVK_PERIOD    = VK_OEM_PERIOD, // '.' any country
    uiVK_SLASH     = VK_OEM_2,      // '/?' for US
    uiVK_TILDE     = VK_OEM_3,      // '`~' for US
    uiVK_SQBOPEN   = VK_OEM_4,      // '[{' for US
    uiVK_BACKSLASH = VK_OEM_5,      // '\|' for US
    uiVK_SQBCLOSE  = VK_OEM_6,      // ']}' for US
    uiVK_QUOT      = VK_OEM_7,      // ''"' for US

    // IME keys    
    uiVK_KANA      = VK_KANA,
    uiVK_KANJI     = VK_KANJI,
};
    
/** uiWin window message IDs. */
enum uiWM
{
    /** Null message. */
    uiWM_NULL           = WM_NULL,

    /** Window is being created.
        This message is sent to the window after instanciation, but before it is visible.

        Return bbEOK or bbEPASS to let the system continue to create the value,
        any other returned error code will cancel the creation process. If the
        window creation process is canceled, an uiWM_DESTROY message will be
        sent, and uiWinCreate will return a NULL handle.

        uiDlg windows

        This message is sent to uiDlg instances just before uiDlg::CreateModal() or
        uiDlg::Create() displays the window. Dialog box initialization like adding
        controls can be on this message.

        If HandleMsg returns bbEOK, the system will set the default key
        focus on the first enabled control which has the uiWS_TABSTOP style. You can
        return uiEPASS to prevent the system to set the focus.
    */
    uiWM_CREATE         = WM_CREATE,

    /** Window is about to be destroyed.
        This message is sent after the window has been removed from the screen, it is sent to the
        parent window first, and then to its child windows.
    */
    uiWM_DESTROY        = WM_DESTROY,

    /** Window is about to be closed.
        This message is sent before the window is to be destroyed. The application can intercept
        this message and perform actions such as saving files. Return bbEPASS to proceed with the
        closing procedure, any other return value will cancel the sequence.
    */
    uiWM_CLOSE          = WM_CLOSE,

    uiWM_PAINT          = WM_PAINT,

    /** Erase window background.
        If bbEEND is returned, the system assumes that the window procedure already did
        the processing, and no further background erasing is performed.
        @param p1 (ptGC*) Picotype graphics context
    */
    uiWM_ERASEBKGND     = WM_ERASEBKGND,

    /** Window size was changed.
        @param p0 (bbU32) Resize type, todo
        @param p1 (bbU32) Low 16 bit new width, top 16 bit new height
    */
    uiWM_SIZE           = WM_SIZE,

    /** Vertical scroll request.
        @param p0 (bbU32)  Low 16 bit Scroll reason code, see uiWMSB.
    */
    uiWM_VSCROLL        = WM_VSCROLL,

    /** Horizontal scroll request.
        @param p0 (bbU32)  Low 16 bit Scroll reason code, see uiWMSB.
    */
    uiWM_HSCROLL        = WM_HSCROLL,

    uiWM_MOUSEFIRST     = WM_MOUSEFIRST,
    uiWM_MOUSEMOVE      = WM_MOUSEMOVE,
    uiWM_LBUTTONDOWN    = WM_LBUTTONDOWN,
    uiWM_LBUTTONUP      = WM_LBUTTONUP,
    uiWM_LBUTTONDBLCLK  = WM_LBUTTONDBLCLK,
    uiWM_RBUTTONDOWN    = WM_RBUTTONDOWN,
    uiWM_RBUTTONUP      = WM_RBUTTONUP,
    uiWM_RBUTTONDBLCLK  = WM_RBUTTONDBLCLK,
    uiWM_MBUTTONDOWN    = WM_MBUTTONDOWN,
    uiWM_MBUTTONUP      = WM_MBUTTONUP,
    uiWM_MBUTTONDBLCLK  = WM_MBUTTONDBLCLK,
    uiWM_MOUSEWHEEL     = WM_MOUSEWHEEL,
    uiWM_MOUSELAST      = WM_MOUSELAST,

    uiWM_KILLFOCUS      = WM_KILLFOCUS,
    uiWM_SETFOCUS       = WM_SETFOCUS,

    /** Virtual key went down.
        @param p0 (uiVK) Virtual key code
        Return bbEEND to filter keypress and prevent it to be converted into a uiWM_CHAR message
    */
    uiWM_KEYDOWN        = WM_KEYDOWN,

    /** Virtual key went up.
        @param p0 (uiVK) Virtual key code
    */
    uiWM_KEYUP          = WM_KEYUP,

    /** Character was entered.
        @param p0 (bbU32) Character code point
    */
    uiWM_CHAR           = WM_CHAR,

    /** Command notification from menu, control, or accelerator.
        @param p0 (bbU32)  Low 16 bit Menu, control, or accelerator ID. <br>
                           Top 16 bit Notification code
        @param p1 (uiWINH) Window handle of control, or NULL for menu/accelerator.
    */
    uiWM_COMMAND        = WM_COMMAND,

    /** User activated context menu.
        @param p0 (uiWINH) Window handle the menu is to be activated for, can be child window.
        @param p1 (bbU32)  Low 16 bit X screen coordinate, top 16 bit Y screen coordinate.
                           Coordinates can be -1 if the menu was not activated by a mouse click (e.g. Shift-F10 on Windows).
    */
    uiWM_CONTEXTMENU    = WM_CONTEXTMENU,

    /** One or more files were drag and dropped onto window.
        @param p0 (uiDRAGH) Drop handle, use with uiDrag* functions
        If this message is being processed, HandleMsg() must call
        uiDragFinish((uiDRAGH)p0) and return bbEOK. Otherwise uiEPASS
        must be returned.
    */
    uiWM_DROPFILES      = WM_DROPFILES,

    /** Notification from child window.
        @param p0 (bbU32)  Low 16 bit: uiWM received by child
    */
    uiWM_PARENTNOTIFY   = WM_PARENTNOTIFY,

    uiWM_TIMER          = WM_TIMER,

    uiWM_CTL            = WM_APP,

    /** uiCtlList item text callback.
        @param p0 (bbU32) ID of control
        @param p1 (uiCtlListTextCB*) Parameter struct
    */
    uiWM_CTLLIST_TEXTCB = uiWM_CTL,

    /** uiCtlList selection changed.
        @param p0 (bbU32) ID of control
        @param p1 (uiCtlListSelChanged*) Parameter struct
    */
    uiWM_CTLLIST_SELCHANGED,

    /** uiCtlList item activated.
        @param p0 (bbU32) ID of control
        @param p1 (int) Item index
    */
    uiWM_CTLLIST_ACTIVATE,

    /** uiCtlListBox selection changed.
        @param p0 (bbU32) ID of control
        @param p1 (int)   New index of selected item, or -1 if none selected
    */
    uiWM_CTLLISTBOX_SELCHANGED,

    /** uiCtlCombo selection changed.
        @param p0 (bbU32) ID of control
        @param p1 (int)   New index of selected item, or -1 if none selected
    */
    uiWM_CTLCOMBO_SELCHANGED,

    /** uiCtlCheck selection changed.
        @param p0 (bbU32) ID of control
        @param p1 (uiCTLCHECKSTATE) New state
    */
    uiWM_CTLCHECK_STATECHANGED,

    /** uiCtlRadio selection changed.
        @param p0 (bbU32) ID of control
        @param p1 (uiCTLRADIOSTATE) New state
    */
    uiWM_CTLRADIO_STATECHANGED,

    /** uiCtlEdit looses focus.
        @param p0 (bbU32) ID of control
    */
    uiWM_CTLEDIT_KILLFOCUS,

    /** uiCtlSpin position changes.
        @param p0 (bbU32) ID of control
        @param p1 (bbS32) New position
    */
    uiWM_CTLSPIN_POSCHANGE,

    /** uiCtlVal value changed.
        @param p0 (bbU32) ID of control
        @param p1 (bbS32) Low 32 bit of new value
    */
    uiWM_CTLVAL_VALCHANGED,

    /** Start ID for application defined window messages. */
    uiWM_APP            = WM_APP + 0x1000
};

enum uiCMD
{
    uiCMD_OK = IDOK,            //!< Default command ID: OK
    uiCMD_CANCEL = IDCANCEL,    //!< Default command ID: Cancel
    uiCMD_YES = IDYES,          //!< Default command ID: Yes
    uiCMD_NO = IDNO,            //!< Default command ID: No
    uiCMD_EXIT = 0x4000,        //!< Default command ID: Application exit
    uiCMD_NEW,                  //!< Default command ID: File New
    uiCMD_OPEN,                 //!< Default command ID: File Open
    uiCMD_SAVE,                 //!< Default command ID: File Save
    uiCMD_SAVEAS,               //!< Default command ID: File Save As
    uiCMD_SAVEALL,              //!< Default command ID: Save all open data
    uiCMD_CLOSE,                //!< Default command ID: File Close
    uiCMD_UNDO,                 //!< Default command ID: Undo edit
    uiCMD_REDO,                 //!< Default command ID: Redo edit
    uiCMD_CUT,                  //!< Default command ID: Cut
    uiCMD_COPY,                 //!< Default command ID: Copy
    uiCMD_PASTE,                //!< Default command ID: Paste
    uiCMD_DEL,                  //!< Default command ID: Delete
    uiCMD_WINLIST,              //!< Default command ID: Window list
    uiCMD_STATUSBAR,            //!< Default window ID for status bar
    uiCMD_7E = 0x4100,          //!< ID range 0x4100-0x4FFF reserved for 7e lib
    uiCMD_APP = 0x5000          //!< First command ID for application use
};

enum uiCMDSRC
{
    uiCMDSRC_MENU=0,
    uiCMDSRC_ACCEL=1,
    uiCMDSRC_CONTROL=2
};

/** Window class IDs. */
enum uiWC
{
    uiWC_Win = 0,      //!< uiWin
    uiWC_CtlVal,       //!< uiCtlVal
    uiWC_CtlButton,    //!< uiCtlButton
    uiWC_CtlEdit,      //!< uiCtlEdit
    uiWC_CtlRadio,     //!< uiCtlRadio
    uiWC_CtlStatic,    //!< uiCtlStatic
    uiWC_CtlListBox,   //!< uiCtlListBox
    uiWC_CtlCheck,     //!< uiCtlCheck
    uiWC_CtlList,      //!< uiCtlList
    uiWC_CtlSpin,      //!< uiCtlSpin
    uiWC_CtlCombo,     //!< uiCtlCombo
    uiWCCOUNT
};

#define uiPREV (-32768)

typedef struct
{
    uiWC  wc;               //!< Window class ID
    bbU32 id;               //!< Window ID
    bbS16 x, y;             /**< Positive: absolut window pos <br>
                                 Negative: relative window pos to previous window <br>
                                 uiPREV:   copy x or y coordinate from previous window
                            */
    bbS16 w, h;             //!< Window size
    bbU32 style, styleEx;   //!< Window style and extended style
    const bbCHAR* pText;    //!< Window text, 0-terminated string, or NULL

} uiWinDesc;

/** Destroy window and reset window handle to NULL.
    @param phWin Pointer to window handle
*/
void uiWinDestroy(uiWINH* const phWin);

/** Get window style bitmask.
    @param hWin Window handle
    @return Bitmask with window styles
    @see uiWS
*/
inline bbU32 uiWinGetStyle(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return (bbU32)::GetWindowLong(hWin, GWL_STYLE);
    #else
    #endif
}

/** Get window area in screen coordinates.
    @param hWin  Window handle
    @param pRect Returns area coordinates
*/
inline bbERR uiWinGetRect(uiWINH const hWin, uiRect* const pRect)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    bbERR const err = bbErrSet(1^::GetWindowRect(hWin, pRect));
    bbASSERT(err == bbEOK); // Look at GetLastError()
    return err;
    #else
    #endif
}

/** Get window client area.
    @param hWin  Window handle
    @param pRect Returns client area coordinates
*/
inline bbERR uiWinGetClientRect(uiWINH const hWin, uiRect* const pRect)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return bbErrSet(1^::GetClientRect(hWin, pRect));
    #else
    #endif
}

/** Convert screen coordinates to window client area coordinates.
    @param hWin  Window handle
    @param pRect Passes screen coordinates, returns client coordinates
*/
bbERR uiWinScreenToClient(uiWINH const hWin, uiRect* const pRect);

/** Convert window client area coordinates to screen coordinates.
    @param hWin  Window handle
    @param pRect Passes client coordinates, returns screen coordinates
*/
bbERR uiWinClientToScreen(uiWINH const hWin, uiRect* const pRect);

/** Get rectangle centered on window and position-adjusted within monitor workarea.
    @param hWin Window handle
    @param w Width of rectangle
    @param h Height of rectangle
    @param pRect Returns centered rectangle on success
*/
void uiWinCenterRect(uiWINH const hWin, bbUINT const w, bbUINT const h, uiRect* const pRect);

/** Return centered window rectangle for sizing border dialog from client area size.
    @param hParent Parent window handle to center rectangle on, or NULL for center on primary monitor.
    @param width Client area width
    @param height Client area height
    @param pRect Returns rectangle with sized to cover client area and dialog window frame and caption.
*/
void uiWinSizingCenterRect(uiWINH const hParent, bbUINT const width, bbUINT const height, uiRect* const pRect);

/** Return centered window rectangle for static border dialog from client area size.
    @param hParent Parent window handle to center rectangle on, or NULL for center on primary monitor.
    @param width Client area width
    @param height Client area height
    @param pRect Returns rectangle with sized to cover client area and dialog window frame and caption.
*/
void uiWinStaticCenterRect(uiWINH const hParent, bbUINT const width, bbUINT const height, uiRect* const pRect);

/** Repaint window.
    @param hWin  Window handle
    Sends the window an uiWM_PAINT message.
*/
inline void uiWinUpdate(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    InvalidateRect(hWin, NULL, FALSE);
    #else
    #endif
}

/** Clear background and repaint window.
    @param hWin  Window handle
    Sends the window uiWM_ERASEBKGND and uiWM_PAINT message.
*/
inline void uiWinErase(uiWINH const hWin)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    InvalidateRect(hWin, NULL, TRUE);
#else
#endif
}

/** Resize window.
    @param hWin  Window handle
    @param pRect Pointer to new window coordinates, relative to parent window
    @param redraw !=0 Redraw after moving
*/
inline bbERR uiWinMove(uiWINH const hWin, const uiRect* const pRect, int const redraw)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    bbERR const err = 1^::MoveWindow(hWin, pRect->left, pRect->top, pRect->right-pRect->left, pRect->bottom-pRect->top, redraw);
    bbASSERT(err == bbEOK); // Look at GetLastError()
    return err;
    #else
    #endif
}

/** Set parameters of scrollbar attached to window.
    uiWinScroll::mask specifies a bitmask of valid uiWinScroll members to be set.
    @param hWin  Window handle
    @param type  Scrollbar type
    @param pInfo Parameter structure 
*/
inline void uiWinSetScrollBar(uiWINH const hWin, uiWINSBTYPE const type, uiWinScroll* const pInfo, int const redraw)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    pInfo->reserved0 = sizeof(SCROLLINFO);
    ::SetScrollInfo(hWin, type, (SCROLLINFO*)pInfo, (BOOL)redraw);
    #else
    #endif
}

/** Get parameters of scrollbar attached to window.
    uiWinScroll::mask is ignored and all members of uiWinScroll will be returned.
    @param hWin  Window handle
    @param type  Scrollbar type
    @param pInfo Parameter structure which will be filled on return.
*/
inline bbERR uiWinGetScrollBar(uiWINH const hWin, uiWINSBTYPE const type, uiWinScroll* const pInfo)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    pInfo->reserved0 = sizeof(SCROLLINFO);
    pInfo->mask      = SIF_ALL;
    return 1^::GetScrollInfo(hWin, type, (SCROLLINFO*)pInfo);
    #else
    #endif
}

/** Send mouse-related messages to window even, if pointer exits window area.
    @param hWin  Window handle
*/
inline void uiWinSetCapture(uiWINH const hWin)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::SetCapture(hWin);
#else
#endif
}

/** Stop mouse message capture, previously activated with uiWinSetCapture().
    @param hWin Window handle
*/
inline void uiWinReleaseCapture(uiWINH const /*hWin*/)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::ReleaseCapture();
#else
#endif
}

/** Get window ID.
    @param hWin Window handle
    @return Returns ID
*/
inline bbUINT uiWinGetID(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return ::GetWindowLong(hWin, GWL_ID);
    #else
    #endif
}


/** Get parent window handle.
    @param hWin  Window handle
    @return Returns handle to parent window, or NULL if not found or error.
*/
inline uiWINH uiWinGetParent(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return ::GetParent(hWin);
    #else
    #endif
}

/** Get child window by ID.
    @param hWin  Window handle
    @param id ID of child window
    @return Returns handle to child window, or NULL if not found or error.
*/
inline uiWINH uiWinGetChild(uiWINH const hWin, bbUINT const id)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return ::GetDlgItem(hWin, id);
    #else
    #endif
}

struct uiWinPaint
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    PAINTSTRUCT ps;
#else
    bbU8 dummy;
#endif
};


ptGC* uiWinPaintStart(uiWINH const hWin, uiWinPaint* const pPaint);

void uiWinPaintEnd(uiWINH const hWin, uiWinPaint* const pPaint);

/** Get window text length in code units, without 0-terminator.
    @param hWin Window handle
    @return Length, or 0 on error
    @see uiWin::GetText, uiWin::SetText
*/
inline int uiWinGetTextLen(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    int const ret = ::GetWindowTextLength(hWin);
    if (!ret) bbErrSet(bbESYS);
    return ret;
    #else
    #endif
}

/** Get window text.
    To determine the required buffer size, call uiWin::GetTextLen().
    @param hWin Window handle
    @param pTextBuf Pointer to buffer to receive 0-terminated string
    @param len      Length of buffer in character code units
    @return Length of string without 0-terminator on success, or 0 if no text is present
    @see uiWin::SetText, uiWin::GetTextLen
*/
inline int uiWinGetText(uiWINH const hWin, bbCHAR* const pTextBuf, int const len)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    int const ret = ::GetWindowText(hWin, pTextBuf, len);
    if (!ret) bbErrSet(bbESYS);
    return ret;
    #else
    #endif
}

/** Get window text in heap buffer.
    Returned block must be free with bbMemFree externally.
    @param hWin Window handle
    @return Pointer to heap block with 0-terminated string, or NULL on error.
*/
bbCHAR* uiWinGetTextMem(uiWINH const hWin);

/** Set window text of a child window to numerical value.
    This function is intended to easily access child controls of a dialog.
    @param hWin Parent window handle
    @param id   ID of child window
    @param val  Value to be converted to string and set as new window text
*/
inline bbERR uiWinChildSetVal(uiWINH const hWin, bbUINT id, int val)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    bbERR const err = bbErrSet(1^::SetDlgItemInt(hWin, id, val, TRUE));
    bbASSERT(err == bbEOK); // Look at GetLastError()
    return err;
    #else
    #endif
}

inline bbERR uiWinChildGetVal(uiWINH const hWin, bbUINT id, int* const pVal)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    BOOL ok;
    *pVal = (int)::GetDlgItemInt(hWin, id, &ok, TRUE);
    if (!ok)
        return bbErrSet(bbESYS);
    return bbEOK;
    #else
    #endif
}

/** Set window text of a child window.
    This function is intended to easily access child controls of a dialog.
    @param hWin Parent window handle
    @param id   ID of child window
    @param pStr 0-terminated text
*/
inline bbERR uiWinChildSetText(uiWINH const hWin, bbUINT id, const bbCHAR* const pStr)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return bbErrSet(1^::SetDlgItemText(hWin, id, pStr));
    #else
    #endif
}

/** Get window text of a child window.
    This function is intended to easily access child controls of a dialog.
    If the passed buffer is shorter than the window text, the returned string is truncated.
    @param hWin   Parent window handle
    @param id     ID of child window
    @param pStr   Pointer to buffer to receive 0-terminated string
    @param strlen Length of buffer \a pStr in characters
    @return Number of character written to buffer, excluding 0-terminator. 0 indicates error, check bbErrGet().
*/
inline bbUINT uiWinChildGetText(uiWINH const hWin, bbUINT id, bbCHAR* pStr, bbUINT strlen)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    bbUINT ret = ::GetDlgItemText(hWin, id, pStr, strlen);
    if (ret == 0)
        bbErrSet(GetLastError() ? bbESYS : bbEOK);
    return ret;
    #else
    #endif
}
    
/** Get window text of a child window in a heap block.
    This function is intended to easily access child controls of a dialog.
    @param hWin Parent window handle
    @param id   ID of child window
    @return     Heap block containing 0-terminated string, or NULL on error. Must be freed externally
*/
bbCHAR* uiWinChildGetTextMem(uiWINH const hWin, bbUINT const id);

/** Enable or disable window.
    If a window is disable, it cannot receive user input.
    @param hWin Window handle
    @param enable 0 to disable, !=0 to enable
    @return Returns previous enable state
    @see uiWS_DISABLED, e7Win::IsEnabled()
*/
inline int uiWinEnable(uiWINH const hWin, int const enable)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return ::EnableWindow(hWin, (BOOL)enable);
    #else
    #endif
}

/** Test if window is enabled
    @param hWin Window handle
    @return !=0 if enabled, 0 if disabled
    @see e7Win::Enable()
*/
inline int uiWinIsEnabled(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return (int)::IsWindowEnabled(hWin);
    #else
    #endif
}

/** Show window in specified manner.
    @param hWin Window handle
    @param action Show action ID
*/
inline void uiWinShow(uiWINH const hWin, uiWINSHOW action = uiWINSHOW_DEFAULT)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::ShowWindow(hWin, action);
#else
#endif
}

/** Test if window is shown.
    @param hWin Window handle
    @return !=0 if visible
*/
inline int uiWinIsShown(uiWINH const hWin)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return ::IsWindowVisible(hWin);
#else
#endif
}

inline void uiWinSetFocus(uiWINH const hWin)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::SetFocus(hWin);
#else
#endif
}

inline int uiWinHasFocus(uiWINH const hWin)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return ::GetFocus() == hWin;
#else
#endif
}

inline uiWINH uiWinGetFocus(void)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
        return ::GetFocus();
#else
#endif
}

inline void uiWinPostMessage(uiWINH hWin, bbU32 msg, void* p0, void* p1)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::PostMessage(hWin, msg, (WPARAM)p0, (LPARAM)p1);
    #else
    #endif
}

inline bbERR uiWinSendMessage(uiWINH hWin, bbU32 msg, void* p0, void* p1)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return ::SendMessage(hWin, msg, (WPARAM)p0, (LPARAM)p1);
    #else
    #endif
}


/** Window class. */
struct uiWin : uiWinBase
{
    static uiRect sDefaultRect;     //!< Default rectangle

    //
    // Initialization and destruction
    //

    /** @param hWin Window handle, can be NULL */
    uiWin(uiWINH hWin = NULL) : uiWinBase(hWin) {}

    /** @param hWin Window handle, can be NULL */
    uiWin& operator=(uiWINH hWin)
    {
        mhWin = hWin;
        return *this;
    }

    operator uiWINH() const
    {
        return mhWin;
    }

    /** Create a window and attach to this window class instance.
        @param hParent Parent window handle, or NULL to create a top-level window
        @param pName   0-terminated window text, used as caption string or text for controls
        @param id      Window ID, ignored if style uiWS_CHILD is not set
        @param style   Bitmask with window styles, see uiWINSTYLE
        @param styleEx Bitmask with extended window styles, see uiWINSTYLEEX
        @param pRect   Pointer to rectangle specifying position and size, NULL for default position and size
        @return Window handle or NULL on failure
    */
    uiWINH Create(uiWINH hParent, const bbCHAR* pName, bbUINT id = 0, bbU32 style = 0, bbU32 styleEx = 0, uiRect const* pRect = NULL);

    /** Destroy window and reset internal window handle to NULL. */
    inline void Destroy()
    {
        uiWinDestroy(&mhWin);
    }

    bbERR CreateChildList(const uiWinDesc* pList, bbUINT count);

    //
    // Attributes
    //

    /** Get window style bitmask.
        @return Bitmask with window styles
        @see uiWS
    */
    inline bbU32 GetStyle() const
    {
        return uiWinGetStyle(mhWin);
    }

    /** Get extended window style bitmask.
        @return Bitmask with window styles
        @see uiWSE
    */
    inline bbU32 GetStyleEx() const
    {
        #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
        return (bbU32)::GetWindowLong(mhWin, GWL_EXSTYLE);
        #else
        #endif
    }

    /** Get window ID.
        @return ID
    */
    inline bbUINT GetID() const
    {
        return uiWinGetID(mhWin);
    }

    /** Set new window ID.
        @param id ID
        @return Returns previous ID
    */
    inline bbUINT SetID(bbUINT id)
    {
        #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
        return ::SetWindowLong(mhWin, GWL_ID, id);
        #else
        #endif
    }

    /** Set window text.
        A window's text is usually used as caption string.
        @param pText 0-terminated string, will be copied
        @see uiWin::GetText, uiWin::GetTextLen
    */
    bbERR SetText(bbCHAR const* pText)
    {
        #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
        if (1^::SetWindowText(mhWin, pText))
            return bbErrSet(uiEWINSETTEXT);
        return bbEOK;
        #else
        #endif
    }

    /** Get window text.
        To determine the required buffer size, call uiWin::GetTextLen().
        @param pTextBuf Pointer to buffer to receive 0-terminated string
        @param len      Length of buffer in character code units
        @return Length of string without 0-terminator on success, or 0 on error
        @see uiWin::SetText, uiWin::GetTextLen
    */
    inline int GetText(bbCHAR* const pTextBuf, int const len) const
    {
        return uiWinGetText(mhWin, pTextBuf, len);
    }

    /** Get window text length in code units, without 0-terminator.
        @return Length, or 0 on error
        @see uiWin::GetText, uiWin::SetText
    */
    inline int GetTextLen() const
    {
        return uiWinGetTextLen(mhWin);
    }

    inline bbCHAR* GetTextMem() const
    {
        return uiWinGetTextMem(mhWin);
    }

    /** Set window text of a child window to numerical value.
        This function is intended to easily access child controls of a dialog.
        @param id   ID of child window
        @param val  Value to be converted to string and set as new window text
    */
    inline bbERR ChildSetVal(bbUINT id, int val)
    {
        return uiWinChildSetVal(mhWin, id, val);
    }

    inline bbERR ChildGetVal(bbUINT const id, int* const pVal)
    {
        return uiWinChildGetVal(mhWin, id, pVal);
    }

    /** Set window text of a child window.
        This function is intended to easily access child controls of a dialog.
        @param id   ID of child window
        @param pStr 0-terminated text
    */
    inline bbERR ChildSetText(bbUINT id, const bbCHAR* const pStr)
    {
        return uiWinChildSetText(mhWin, id, pStr);
    }

    /** Get window text of a child window.
        This function is intended to easily access child controls of a dialog.
        If the passed buffer is shorter than the window text, the returned string is truncated.
        @param id     ID of child window
        @param pStr   Pointer to buffer to receive 0-terminated string
        @param strlen Length of buffer \a pStr in characters
        @return Number of character written to buffer, excluding 0-terminator. 0 indicates error, check bbErrGet().
    */
    inline bbUINT ChildGetText(bbUINT id, bbCHAR* pStr, bbUINT strlen)
    {
        return uiWinChildGetText(mhWin, id, pStr, strlen);
    }

    /** Get window text of a child window in a heap block.
        This function is intended to easily access child controls of a dialog.
        @param hWin Parent window handle
        @param id   ID of child window
        @return     Heap block containing 0-terminated string, or NULL on error. Must be freed externally
    */
    inline bbCHAR* ChildGetTextMem(bbUINT const id) const
    {
        return uiWinChildGetTextMem(mhWin, id);
    }

    /** Get child window by ID.
        @param id ID of child window
        @return Returns handle to child window, or NULL if not found or error.
    */
    inline uiWINH GetChild(bbUINT const id) const
    {
        return uiWinGetChild(mhWin, id);
    }

    /** Get parent window handle.
        @return Returns handle to parent window, or NULL if not found or error.
    */
    inline uiWINH GetParent()
    {
        return uiWinGetParent(mhWin);
    }

    /** Set new menu for this window.
        This function replaces a previous menu, but does not destroy it.
        To destroy the previously attached menu, use the following code sequence:
        <pre>
        uiMenu oldmenu = frame.GetMenu();
        oldmenu.Destroy();
        </pre>
    */
    inline bbERR SetMenu(uiMENUH hMenu)
    {
        #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
        bbERR const err = 1^::SetMenu(mhWin, hMenu);
        bbASSERT(err == bbEOK);
        return err;
        #else
        #endif
    }

    /** Get menu assigned to window.
        @return Menu handle or NULL on failure
    */
    inline uiMENUH GetMenu() const
    {
        #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
        return ::GetMenu(mhWin);
        #else
        #endif
    }

    //
    // State, size and position
    //

    /** Enable or disable window.
        If a window is disable, it cannot receive user input.
        @param enable 0 to disable, !=0 to enable
        @return Returns previous enable state
        @see uiWS_DISABLED, e7Win::IsEnabled()
    */
    inline int Enable(int const enable)
    {
        return uiWinEnable(mhWin, enable);
    }
    
    /** Test if window is enabled
        @return !=0 if enabled, 0 if disabled
        @see e7Win::Enable()
    */
    inline int IsEnabled()
    {
        return uiWinIsEnabled(mhWin);
    }
    
    /** Show window in specified manner.
        @param action Show action ID
    */
    inline void Show(uiWINSHOW action = uiWINSHOW_DEFAULT)
    {
        uiWinShow(mhWin, action);
    }

    /** Test if window is shown.
        @param hWin Window handle
        @return !=0 if visible
    */
    inline int IsShown()
    {
        return uiWinIsShown(mhWin);
    }

    inline void SetFocus()
    {
        uiWinSetFocus(mhWin);
    }

    inline int HasFocus()
    {
        return uiWinHasFocus(mhWin);
    }

    inline void SetCapture()
    {
        uiWinSetCapture(mhWin);
    }

    inline void ReleaseCapture()
    {
        uiWinReleaseCapture(mhWin);
    }

    /** Repaint window.
        Sends the window an uiWM_PAINT message.
    */
    inline void Update()
    {
        uiWinUpdate(mhWin);
    }

    /** Clear background and repaint window.
        @param hWin  Window handle
        Sends the window uiWM_ERASEBKGND and uiWM_PAINT message.
    */
    inline void Erase()
    {
        uiWinErase(mhWin);
    }

    /** Get window area.
        @param pRect Returns area coordinates
    */
    inline bbERR GetRect(uiRect* const pRect)
    {
        return uiWinGetRect(mhWin, pRect);
    }

    /** Get window client area.
        @param pRect Returns client area coordinates
    */
    inline bbERR GetClientRect(uiRect* const pRect)
    {
        return uiWinGetClientRect(mhWin, pRect);
    }

    inline void CenterRect(bbUINT const w, bbUINT const h, uiRect* const pRect)
    {
        uiWinCenterRect(mhWin, w, h, pRect);
    }

    /** Resize window.
        @param pRect Pointer to new window coordinates, relative to parent window
        @param redraw !=0 Redraw after moving
    */
    inline bbERR Move(const uiRect* const pRect, int const redraw)
    {
        return uiWinMove(mhWin, pRect, redraw);
    }

    /** Get parameters of scrollbar attached to window.
        uiWinScroll::mask is ignored and all members of uiWinScroll will be returned.
        @param type  Scrollbar type
        @param pInfo Parameter structure which will be filled on return.
    */
    inline bbERR GetScrollBar(uiWINSBTYPE const type, uiWinScroll* const pInfo)
    {
        return uiWinGetScrollBar(mhWin, type, pInfo);
    }

    /** Set parameters of scrollbar attached to window.
        uiWinScroll::mask specifies a bitmask of valid uiWinScroll members to be set.
        @param type  Scrollbar type
        @param pInfo Parameter structure 
    */
    inline void SetScrollBar(uiWINSBTYPE const type, uiWinScroll* const pInfo, int const redraw)
    {
        uiWinSetScrollBar(mhWin, type, pInfo, redraw);
    }

    /** Standard handler for uiWM_VSCROLL and uiWM_HSCROLL messages.
        This will update and redraw the scrollbar according to \a scrollreason
        and the current scrollbar metrics.
        @param type Scrollbar type
        @param scrollreason Scroll reason, as received in uiWM_VSCROLL or uiWM_HSCROLL message
        @return New scrollposition
    */
    int UpdateScrollBar(uiWINSBTYPE const type, uiWMSB const scrollreason);

    //
    // Message handling
    //
    inline void PostMessage(bbU32 msg, void* p0, void* p1)
    {
        uiWinPostMessage(mhWin, msg, p0, p1);
    }

    inline bbERR SendMessage(bbU32 msg, void* p0, void* p1)
    {
        return uiWinSendMessage(mhWin, msg, p0, p1);
    }


    /** Window message handler.

        Override this default handler in a uiWin subclass. If this function is overridden,
        this default implementation may be chain-called from the child class.

        @param msg Message ID
        @param p0  Message specific parameter 0
        @param p1  Message specific parameter 1

        @return Return bbEOK if the message was successfully handled, return uiEPASS
                to indicate to the operating system that the message was not handled.
    */
    virtual bbERR HandleMsg(bbU32 msg, void* p0, void* p1);

    /** Window command handler.
        The default implementation does nothing and returns uiEPASS.
        @param cmd   Command ID
        @param pos   Menu position
        @param hSrc  Handle to window or menu from which this command originated
        @param user  User data associated with control, menu item, or accelerator
        @return Return bbEOK if the message was successfully handled, return uiEPASS
                to indicate to the operating system that the message was not handled.
    */
    virtual bbERR HandleCmd(uiCMDSRC src, bbUINT cmd, bbUINT pos, void* hSrc, void* user);
};

/** Query number of filenames in a drag and drop operation.
    @see uiWM_DROPFILES, uiDragGetPathMem
    @param hDrag Drag handle
    @return Number of files
*/
inline bbUINT uiDragGetPathCount(uiDRAGH const hDrag)
{
    return ::DragQueryFile(hDrag, 0xFFFFFFFFU, NULL, 0);
}

/** Get filename from a drag and drop operation.
    @see uiWM_DROPFILES, uiDragGetPathCount
    @param hDrag Drag handle
    @param index 0-based index of filename to query, must be within 0 and uiDragGetPathCount(hDrag)-1
    @return Pointer to heap block containing 0-terminated path, must be freed with bbMemFree. NULL on error.
*/
bbCHAR* uiDragGetPathMem(uiDRAGH const hDrag, bbUINT const index);

/** Finish drag and drop operation
    @param hDrag Drag handle
    @see uiWM_DROPFILES
*/
inline void uiDragFinish(uiDRAGH const hDrag)
{
    ::DragFinish(hDrag);
}

/*@}*/

#endif /* uiWin_H_ */

