#include "uiCtlVal.h"
#include <babel/str.h>
#include <babel/mem.h>
#include <ptGC.h>
#include "uiWin.h"

static inline uiCtlValData* uiCtlValGetData(uiWINH const hWin)
{
    if (!hWin)
        return NULL;
    return (uiCtlValData*) ::GetWindowLongPtr(hWin, 0);
}

static bbERR uiCtlValOnCreate(uiWINH const hWin)
{
    static const uiCtlValData init = { 0, (bbS64)((bbU64)1 << 63), (bbS64)(((bbU64)1 << 63)-1), '%','I','6','4','d',0 };

    uiCtlValData* const pData = new uiCtlValData;
    if (!pData)
        return bbErrSet(bbENOMEM);

    ::SetWindowLongPtr(hWin, 0, (LONG_PTR)pData);

    bbMemMove(pData, &init, sizeof(uiCtlValData));

    bbU32 const style = uiWinGetStyle(hWin);
    if (style & uiWS_VAL_RANGEBYTE)
        uiCtlValSetMinMax(hWin, 0, 255);

    return bbEOK;
}

static void uiCtlValOnDestroy(uiWINH const hWin)
{
    uiCtlValData* const pData = uiCtlValGetData(hWin);
    delete pData;
}

static void uiCtlValOnPaint(uiWINH const hWin)
{
    ptGC*       pGC;
    uiCtlValData* const pData = uiCtlValGetData(hWin);
    uiWinPaint  ps;
    uiRect      rect;
    union {
    bbCHAR      str[32];
    ptCoord     pt[4];
    };

    if (pData && ((pGC = uiWinPaintStart(hWin, &ps)) != NULL))
    {
        bbU32 const style = uiWinGetStyle(hWin);
    
        int len = bbSprintf(str, pData->mFmt, pData->mVal);

        uiWinGetClientRect(hWin, &rect);

        if ((style & uiWS_DISABLED) == 0)
        {
            HFONT hFont = (HFONT)SendMessage(GetParent(hWin), WM_GETFONT, 0, 0);
            if (hFont)
                SelectObject(ps.ps.hdc, hFont);

            if ((bbU64)(pData->mVal - pData->mMin) > (bbU64)(pData->mMax - pData->mMin))
                SetTextColor(ps.ps.hdc, RGB(255,0,0));
            else
                SetTextColor(ps.ps.hdc, RGB(0,0,0));
            SetBkMode(ps.ps.hdc, TRANSPARENT);

            DrawText(ps.ps.hdc, str, len, &rect, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | (style & (DT_CENTER|DT_RIGHT)));
        }

        int const m = 5;
        pt[0].x = rect.left + m;
        pt[0].y = rect.bottom - 1;
        pt[1].x = rect.left;
        pt[1].y = rect.top + ((rect.bottom-rect.top)>>1);
        pt[2].x = rect.left + m;
        pt[2].y = rect.top;
        pt[3].x = rect.right - m;
        pt[3].y = rect.top;
        pGC->LineString(pt, 4, pData->mDrag?0x88:240);
        pt[2].x = rect.right - 1;
        pt[2].y = pt[1].y;
        pt[1].x = rect.right - m;
        pt[1].y = rect.bottom - 1;
        pGC->LineString(pt, 4, pData->mDrag?240:0x88);

        uiWinPaintEnd(hWin, &ps);
    }
}

void uiCtlValSetFormat(uiWINH const hWin, const bbCHAR* const pFmt)
{
    uiCtlValData* const pData = uiCtlValGetData(hWin);

    if (pData)
    {
        pData->mFmt[11] = 0;
        bbStrNCpy(pData->mFmt, pFmt, 11);
    }
}

void uiCtlValSet(uiWINH const hWin, bbS64 val)
{
    uiCtlValData* const pData = uiCtlValGetData(hWin);

    if (pData)
    {
        if (uiWinGetStyle(hWin) & uiWS_VAL_LOCKINRANGE)
        {
            if (val < pData->mMin)
                val = pData->mMin;
            if (val > pData->mMax)
                val = pData->mMax;
        }
        pData->mVal = val;

        uiWinSendMessage(uiWinGetParent(hWin), uiWM_CTLVAL_VALCHANGED, (void*)uiWinGetID(hWin), (void*)val);
    }
}

void uiCtlValGet64(uiWINH const hWin, bbS64* const pVal)
{
    uiCtlValData* const pData = uiCtlValGetData(hWin);
    if (pData)
        *pVal = pData->mVal;
    else
        *pVal = 0;
}

bbS32 uiCtlValGet(uiWINH const hWin)
{
    uiCtlValData* const pData = uiCtlValGetData(hWin);
    return pData ? (bbS32)pData->mVal : 0;
}

void uiCtlValSetMinMax(uiWINH const hWin, bbS64 const min, bbS64 const max)
{
    uiCtlValData* const pData = uiCtlValGetData(hWin);
    if (!pData)
        return;
    
    pData->mMin = min;
    pData->mMax = max;
    if (uiWinGetStyle(hWin) & uiWS_VAL_LOCKINRANGE)
    {
        if (pData->mVal < pData->mMin)
            pData->mVal = pData->mMin;
        if (pData->mVal > pData->mMax)
            pData->mVal = pData->mMax;
    }
}

#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)

LRESULT CALLBACK uiCtlValWindowProc(HWND hWin, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    uiCtlValData* pData;

    switch (uMsg)
    {
    case WM_CREATE:
        if (bbEOK != uiCtlValOnCreate(hWin))
            return -1;
        break;

    case WM_DESTROY:
        uiCtlValOnDestroy(hWin);
        break;

    case WM_PAINT:
        uiCtlValOnPaint(hWin);
        break;

    case WM_MOUSEMOVE:
        if ((pData = uiCtlValGetData(hWin)) != NULL)
        {
            if (pData->mDrag)
            {
                uiCtlValSet(hWin, pData->mDragStart + (((int)lParam << 16) >> 16));
                uiWinErase(hWin);
            }
        }
        return 0;
        
    case WM_LBUTTONDOWN:
        if ((pData = uiCtlValGetData(hWin)) != NULL)
        {
            pData->mDrag = 1;
            pData->mDragStart = uiCtlValGet(hWin) - (((int)lParam << 16) >> 16);
            uiWinSetCapture(hWin);
            uiWinErase(hWin);
        }
        return 0;

    case uiWM_LBUTTONUP:
        if ((pData = uiCtlValGetData(hWin)) != NULL)
        {
            if (pData->mDrag)
            {
                pData->mDrag = 0;
                uiWinReleaseCapture(hWin);
                uiWinErase(hWin);
            }
        }
        return 0;
    }

    return DefWindowProc(hWin, uMsg, wParam, lParam);
}

#endif
