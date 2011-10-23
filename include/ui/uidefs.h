#ifndef uidefs_H_
#define uidefs_H_

#include <babel/defs.h>
#include <ptdefs.h>

#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)

#include <wingdi.h>
#include <commctrl.h>
#include <shellapi.h>
#endif // bbOS

/** Rectangle descriptor.
    Defined to match the WINAPI RECT structure.
*/
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
typedef RECT uiRect;
#else
typedef ptRect uiRect;
#endif

/** Window handle. */
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
typedef HWND uiWINH;
#else
typedef void* uiWINH;
#endif

/** Drag and drop handle. */
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
typedef HDROP uiDRAGH;
#else
typedef void* uiDRAGH;
#endif

/** Monitor handle. */
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
typedef HMONITOR uiMONH;
#else
typedef void* uiMONH;
#endif

enum uiERR
{
    uiEPASS = bbEBASE_UI,       //!< uiWin::HandleMsg return code to indicate message was not handled
    uiEWININVALIDHANDLE,        //!< Invalid uiWINH window handle
    uiEWINCREATE,               //!< uiWin::Create() failed
    uiEWINDESTROY,              //!< uiWin::Destroy() failed
    uiEWINSETTEXT,              //!< uiWin::SetText() failed
    uiEWINCLASSREG,             //!< uiWinClass::Register() failed
    uiEMENUPOPUP,               //!< uiMenuShowPopup() failed
};

/** System metrics IDs.
    @see uiSysGet
*/
enum uiSYS
{
#if (bbOS == bbOS_WIN32) ||  (bbOS == bbOS_WINCE)
    uiSYS_CAPTIONHEIGHT      = SM_CYCAPTION,        //!< Height of a window caption
    uiSYS_TOOLCAPTIONHEIGHT  = SM_CYSMCAPTION,      //!< Height of a tool window caption
    uiSYS_BORDERSIZINGWIDTH  = SM_CXSIZEFRAME,      //!< Sizing window border width
    uiSYS_BORDERSIZINGHEIGHT = SM_CYSIZEFRAME,      //!< Sizing window border height
    uiSYS_BORDERFIXEDWIDTH   = SM_CXFIXEDFRAME,     //!< Static window border width
    uiSYS_BORDERFIXEDHEIGHT  = SM_CYFIXEDFRAME,     //!< Static window border height
    uiSYS_SCROLLWIDTH        = SM_CXVSCROLL,        //!< Width of scrollbar
    uiSYS_SCROLLHEIGHT       = SM_CYHSCROLL         //!< Height of scrollbar
#else
#endif
};

/** Get system metric.
    @param id Metric ID
    @return Value, specific to ID
*/
inline bbU32 uiSysGet(uiSYS const id)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return (bbU32)::GetSystemMetrics((int)id);
#else
    return 0;
#endif
}

/** Set UI logicial palette.
    This palette is used for the ptGC object returned from uiWinPaintStart().
    @param pPal Palette object, must be valid until another object is selected. NULL to set default palette.
*/
void uiSysSetPal(ptPal* const pPal);

/** Default ui logical colour numbers. */
enum uiCOL
{
    uiCOL_BLACK = 0,    //!< Black
    uiCOL_WHITE,        //!< White
    uiCOL_APPWORKSPACE, //!< Application workspace
    uiCOL_WINBG,        //!< Window background
    uiCOLCOUNT
};

struct uiWinBase
{
    uiWINH mhWin;            //!< Window handle

    /** @param hWin Window handle, can be NULL */
    inline uiWinBase(uiWINH hWin = NULL)
    {
        mhWin = hWin;
    }

    inline bool operator ==(uiWINH const hWin) { return hWin==mhWin; }

    /** Attach window handle to object.
        The window object must have been unitialized.
        @param Window handle, can be NULL
    */
    inline void Attach(uiWINH hWin)
    {
        bbASSERT(!mhWin);
        mhWin = hWin;
    }

    /** Detach window handle.
        @return Detached window handle
    */
    inline uiWINH Detach()
    {
        uiWINH const hWin = mhWin;
        mhWin = NULL;   
        return hWin;
    }
};

#endif /* uidefs_H_ */

