#ifndef uiApp_H_
#define uiApp_H_

#include "uidefs.h"
#include <babel/mem.h>

/** @addtogroup uiApp Application
@{*/

/** Application main entry point.
    This function should be implemented by the application.
    ui will encapsulate the platform specific main entry point and
    forward application startup to this call.
    @param argc Number of command line arguments, incl. executable name
    @param argv Command line arguments, index 0 is executable name
    @return Error indicator to return on application exit
*/
extern "C" int uiMain(int argc, bbCHAR** argv);

extern "C" bbERR uiInitialize(bbCHAR* pAppPath, void* hInstance);
void uiDestroy();


/** Run UI main message loop.
    This function will run the UI. It is blocking and will return
    once the UI quits.
*/
bbERR uiMessageLoopRun();

/** Terminate UI main message loop. */
void uiMessageLoopTerm();

/** Get path to application directory.
    The path string is returned with a trailing directory delimitter.
    If a previous call of this function succeeded, any succeeding calls are guaranted to succeed.
    @param pDir Pointer to buffer to receive the 0-terminated path, or NULL to read string length.
    @return String length in character code units without 0-terminator, or -1 on error
*/
bbUINT uiGetAppDir(bbCHAR* pDir);

/** Get path constructed from application directory appended by given path trail.
    @param Path trail to append
    @return Full path in heap block to be freed externally, NULL on failure.
*/
bbCHAR* uiGetAppPath(const bbCHAR* pTail);

/*@}*/

/** @addtogroup uiConfig Configuration
@{*/

/** Config database handle. */
typedef void* uiCONFIGH;

/** Open config database.
    @param pConfigFile Database name
    @return Database handle, or NULL on error
*/
uiCONFIGH uiConfigOpen(const bbCHAR* const pConfigFile);

/** Close config database.
    @param hConfig Database handle
*/
inline void uiConfigClose(uiCONFIGH const hConfig)
{
    bbMemFree(hConfig);
}

/** Get config string.
    Key and section strings are not case sensitive. If the key is not found, NULL
    is returned, use bbErrGet() to distinguish error and check for bbENOTFOUND.
    @param hConfig  Database handle
    @param pSection Section name, i.e. 'group-key'
    @param pKey     Key, i.e. 'sub-key'
    @return Pointer to heap block containing 0-terminated string, NULL if not found or error.
            Block must be freed externally.
*/
bbCHAR* uiConfigGetStr(uiCONFIGH const hConfig, const bbCHAR* pSection, const bbCHAR* pKey);

/** Write config string.
    @param hConfig  Database handle
    @param pSection Section name, i.e. 'group-key'
    @param pKey     Key, i.e. 'sub-key'
    @param pVal     String value to write
*/
bbERR uiConfigSetStr(uiCONFIGH const hConfig, const bbCHAR* pSection, const bbCHAR* pKey, const bbCHAR* pVal);

/** Get config string parsed as bbS64.
    @param hConfig  Database handle
    @param pSection Section name, i.e. 'group-key'
    @param pKey     Key, i.e. 'sub-key'
    @return Parsed value, returns 0 on error, check bbErrGet() to distinguish between 0 and error case.
*/
bbS64 uiConfigGetS64(uiCONFIGH const hConfig, const bbCHAR* pSection, const bbCHAR* pKey);

bbERR uiConfigSetS64(uiCONFIGH const hConfig, const bbCHAR* pSection, const bbCHAR* pKey, bbS64 val);

/** Get all section names from a database.
    @param hConfig  Database handle
    @return Pointer to heap block containing 0-terminated strings, string list
            is terminated by a final 0-terminator. Block must be freed externally.
            NULL on error. 
*/
bbCHAR* uiConfigGetSectionList(uiCONFIGH const hConfig);

/** Get all config strings in a section.
    Section names are not case sensitive. If the key is not found, NULL
    is returned, use bbErrGet() to distinguish error and check for bbENOTFOUND.
    @param hConfig  Database handle
    @param pSection Section name, i.e. 'group-key'
    @return Pointer to heap block containing 0-terminated strings with form of key=val, string list
            is terminated by a final 0-terminator. Block must be freed externally.
            NULL if not found or error. 
*/
bbCHAR* uiConfigGetSection(uiCONFIGH const hConfig, const bbCHAR* pSection);

/** Set or delete all strings in a section.
    @param hConfig  Database handle
    @param pSection Section name, i.e. 'group-key'
    @param pKeyVal  List of 0-terminated strings in form key=val with final 0-terminator.
                    NULL to delete this section.
*/
bbERR uiConfigSetSection(uiCONFIGH const hConfig, const bbCHAR* pSection, const bbCHAR* pKeyVal);

/** Get config data.
    Key and section strings are not case sensitive. If the key is not found, NULL
    is returned, use bbErrGet() to distinguish error and check for bbENOTFOUND.
    @param hConfig  Database handle
    @param pSection Section name, i.e. 'group-key'
    @param pKey     Key, i.e. 'sub-key'
    @param size     Expected size of data, should match the size the data was stored with.
    @return Pointer to heap block containing data. Block must be freed externally.
*/
bbU8* uiConfigGet(uiCONFIGH const hConfig, const bbCHAR* pSection, const bbCHAR* pKey, bbUINT size);

/** Write config data.
    @param hConfig  Database handle
    @param pSection Section name, i.e. 'group-key'
    @param pKey     Key, i.e. 'sub-key'
    @param pData    Data block to write
    @param size     Size of data in bytes
*/
bbERR uiConfigSet(uiCONFIGH const hConfig, const bbCHAR* pSection, const bbCHAR* pKey, const bbU8* pData, bbUINT const size);

/*@}*/

/** @addtogroup uiMonitor Monitors

/** Return monitor which is closest to rectangle.
    @param pRect Rectangle
    @return Monitor ID, or NULL on error
*/
inline uiMONH uiMonitorFromRect(uiRect* const pRect)
{
    #if (bbOS==bbOS_WINCE) || (bbOS==bbOS_WIN32)
    return (uiMONH)MonitorFromRect((RECT*)pRect, MONITOR_DEFAULTTONEAREST);
    #else
    bbErrSet(bbENOTSUP);
    return NULL;
    #endif    
}

/** Return monitor which is closest to point.
    @param x X coordinate
    @param y Y coordinate
    @return Monitor ID, or NULL on error
*/
inline uiMONH uiMonitorGetDefault()
{
    #if (bbOS==bbOS_WINCE) || (bbOS==bbOS_WIN32)
    POINT pt = { -1, -1 };
    return (uiMONH)MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    #else
    bbErrSet(bbENOTSUP);
    return NULL;
    #endif    
}

/** uiMonInfo::opt flagbits */
enum uiMONINFOOPT
{
    uiMONINFOOPT_PRIMARY = MONITORINFOF_PRIMARY, //!< Monitor is primary
};

/** uiMonitorGetInfo parameter structure */
struct uiMonInfo
{
    #if (bbOS==bbOS_WINCE) || (bbOS==bbOS_WIN32)
    bbU32   cbSize;     // dummy member to make struct compatible to MONITORINFO
    #endif
    uiRect  pos;        //!< Monitor area on the global desktop
    uiRect  workarea;   //!< Monitor work area on the global desktop, i.e. area not occupied by toolbars and taskbars
    bbU32   opt;        //!< Flagbits, see uiMONINFOOPT
};

/** Get monitor info.
    @param hMon Monitor handle
    @param pInfo Returns info
*/
#if (bbOS==bbOS_WINCE) || (bbOS==bbOS_WIN32)
inline bbERR uiMonitorGetInfo(uiMONH const hMon, uiMonInfo* const pInfo)
{
    pInfo->cbSize = sizeof(MONITORINFO);
    return bbErrSet(1^::GetMonitorInfo(hMon, (MONITORINFO*)pInfo));
}
#else
bbERR uiMonitorGetInfo(uiMONH const hMon, uiMonInfo* const pInfo);
#endif

/*@}*/

/** @addtogroup uiClip Clipboard */

/** Clipboard handle. */
typedef void* uiCLIPH;

uiCLIPH uiClipOpen(uiWINH hWin);

enum uiCLIPFMT
{
    uiCLIPFMT_TEXT        = CF_TEXT,
    uiCLIPFMT_UNICODETEXT = CF_UNICODETEXT,
    uiCLIPFMT_PRIVATE     = CF_PRIVATEFIRST,
    uiCLIPFMT_PRIVATELAST = CF_PRIVATELAST,
    uiCLIPFMT_APP         = CF_GDIOBJFIRST,
    uiCLIPFMT_APPLAST     = CF_GDIOBJLAST
};

void uiClipClose(uiCLIPH hClip);

inline int uiClipIsEmpty(uiCLIPH /*hClip*/)
{
    return !IsClipboardFormatAvailable(CF_TEXT);
}

/** Get clipboard data in specified format.
    If a pointer was returned, must be called later uiClipDataClose() to finish.
    @param hClip Clipboard handle
    @param fmt   Format ID, one of uiCLIPFMT
                 or custom ID registered via uiClipRegisterFormat().
    @return Pointer to clipboard data on aligned address, or NULL on error.
*/
bbU8* uiClipDataRead(uiCLIPH hClip, bbUINT fmt);

/** Start writing to clipboard in specified format.
    Call this function to allocate space on the clipboard. If a pointer
    is returned, copy data and then call uiClipDataClose() to finish.
    In order to write the same data in multiple formats to the clipboard,
    simply call uiClipDataWrite() several times before calling uiClipDataClose().
    @param hClip Clipboard handle
    @param fmt   Format ID, one of uiCLIPFMT
                 or custom ID registered via uiClipRegisterFormat().
    @param size  Size of requested block in bytes (for uiCLIPFMT_UNICODETEXT size should be sizeof(bbCHAR)*string length)
    @return Pointer to clipboard data to fill on aligned address, or NULL on error.
*/
bbU8* uiClipDataWrite(uiCLIPH hClip, bbUINT fmt, bbU32 size);

/** Finish a clipboard read or write operation.
    Call after call to uiClipDataRead() or uiClipDataWrite() to finish the
    transaction. This function is save to call, even if uiClipDataRead() or uiClipDataWrite()
    did not return a valid pointer.
    @param hClip Clipboard handle
*/
void uiClipDataClose(uiCLIPH hClip);

/** Register custom format ID.
    @param pName Format name, 0-terminated string
    @return Format ID or 0 on error
*/
inline bbUINT uiClipRegisterFormat(const bbCHAR* pName)
{
    bbUINT ret = RegisterClipboardFormat(pName);
    if (!ret)
        bbErrSet(bbESYS);
    return ret;
}

inline int uiClipIsFormatAvailable(uiCLIPH /*hClip*/, bbUINT fmt)
{
    return IsClipboardFormatAvailable(fmt);
}

inline uiWINH uiClipGetOwner()
{
    return GetClipboardOwner();
}

/*@}*/

//
// Windows specific globals
//

#if (bbOS==bbOS_WINCE) || (bbOS==bbOS_WIN32)
/** Module instance handle.
    Will be initialized before uiMain() is called.
*/
extern HINSTANCE uighInstance;
#endif

#endif /* uiApp_H_ */
