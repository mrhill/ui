#ifndef uiCtlVal_H_
#define uiCtlVal_H_

#include "uidefs.h"

/** uiCtlVal window styles */
enum uiWS_VAL
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)

    uiWS_VAL_CENTER      = DT_CENTER,   //!< Text is horizontally centered
    uiWS_VAL_RIGHT       = DT_RIGHT,    //!< Text is right aligned
    uiWS_VAL_LOCKINRANGE = 0x4,         //!< Value is kept inside min-max range
    uiWS_VAL_RANGEBYTE   = 0x10         //!< uiWM_CREATE inits range to 0..255

#else
#endif
};

/** uiCtlVal Control data. */
struct uiCtlValData
{
    bbS64   mVal;       //!< Current value
    bbS64   mMin;       //!< Minimum value
    bbS64   mMax;       //!< Maximum value
    bbCHAR  mFmt[12];   //!< printf format string
    bbU8    mDrag;
    int     mDragStart;
};

#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
extern LRESULT CALLBACK uiCtlValWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

/** Set value.
    @param hWin Window handle
    @param val Value.
*/
void uiCtlValSet(uiWINH const hWin, bbS64 val);

/** Get low 32 bit of value.
    @param hWin Window handle
    @return Value.
*/
bbS32 uiCtlValGet(uiWINH const hWin);

/** Get value.
    @param hWin Window handle
    @param pVal Returns value.
*/
void uiCtlValGet64(uiWINH const hWin, bbS64* const pVal);

/** Set min-max range value.
    If the current value is outside the new range, and style bit uiWS_VAL_LOCKINRANGE is set,
    the current value will be snapped into the range.
    @param hWin Window handle
    @param min Minimum value, inclusive
    @param max Maximum value, inclusive
*/
void uiCtlValSetMinMax(uiWINH const hWin, bbS64 const min, bbS64 const max);

/** Set value format string.
    @param hWin Window handle
    @param pFmt 0-terminated format string, will be copied. Must not be longer than 12 characters including terminator.
*/
void uiCtlValSetFormat(uiWINH const hWin, const bbCHAR* const pFmt);

#endif /* uiCtlVal_H_ */

