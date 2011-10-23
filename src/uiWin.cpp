#include "uiWin.h"
#include "uictl.h"
#include "uiCtlVal.h"
#include "babel/log.h"
#include "babel/str.h"
#include "babel/mem.h"
#include "ptGCGDI.h"

uiRect uiWin::sDefaultRect = {
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    CW_USEDEFAULT, CW_USEDEFAULT, 0, 0
    #else
    0, 0, 0, 0
    #endif
};

#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)

extern ptGCGDI* uigpGC;

static void uiWinPrepareGC(uiWINH const hWin, HDC const hDC)
{
    uiRect rect;

    uigpGC->AttachDC0(hDC);

    if (bbEOK == uiWinGetClientRect(hWin, &rect))
    {
        uigpGC->SetWidth(rect.right - rect.left);
        uigpGC->SetHeight(rect.bottom - rect.top);
    }
    else
    {
        uigpGC->SetWidth(::GetDeviceCaps(hDC, HORZRES));
        uigpGC->SetHeight(::GetDeviceCaps(hDC, VERTRES));
    }
}

bbERR uiWindowProcFilterMsg(HWND hWnd, bbUINT uMsg, WPARAM wParam, LPARAM lParam)
{
    uiWin* const pThis = (uiWin*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    bbU32        style;
    bbERR        err = uiEPASS;
    uiCMDSRC     src;

    union
    {
        uiCtlListTextCB textCB;
        uiCtlListSelChanged selChanged;
        TCHAR classname[4];
    };

    if (pThis)
    {
        if (uMsg == WM_COMMAND)
        {
            if (lParam) // from control?
            {
                src=uiCMDSRC_CONTROL;

                switch (wParam >> 16)
                {
                case LBN_SELCHANGE: // same as CBN_SELCHANGE
                    ::GetClassName((uiWINH)lParam, classname, 2);
                    if (classname[0] == 'L')
                    {
                        uMsg = uiWM_CTLLISTBOX_SELCHANGED;
                        lParam = (LPARAM)uiCtlListBoxGetSel((uiWINH)lParam);
                    }
                    else
                    {
                        uMsg = uiWM_CTLCOMBO_SELCHANGED;
                        lParam = (LPARAM)uiCtlComboGetSel((uiWINH)lParam);
                    }
                    break;

                case BN_CLICKED:
                    style = uiWinGetStyle((uiWINH)lParam);
                    if (style & (BS_CHECKBOX|BS_RADIOBUTTON))
                    {
                        uMsg = (style & BS_CHECKBOX) ? uiWM_CTLCHECK_STATECHANGED : uiWM_CTLRADIO_STATECHANGED;
                        lParam = (LPARAM)::SendMessage((uiWINH)lParam, BM_GETSTATE, 0, 0L);
                    }
                    break;

                case EN_KILLFOCUS:
                    uMsg = uiWM_CTLEDIT_KILLFOCUS;
                    break;
                }
            }
            else // from menu or accelerator
            {
                src=(uiCMDSRC)(wParam >> 16); // 0 or 1
            }

            wParam &= 0xFFFF; // ID

            if (uMsg == WM_COMMAND)
                return pThis->HandleCmd(src, wParam, 0, (uiWINH)lParam, NULL);
        }

        if (uMsg == WM_MENUCOMMAND) // command from menu selection
        {
            uiWin*       pOwner = pThis;
            uiWINH const hOwner = uiMenuGetOwner((uiMENUH)lParam);
            void*        user;
            if (hOwner) // does this menu require messages to be redirected?
            {
                pOwner = (uiWin*)GetWindowLongPtr(hOwner, GWLP_USERDATA);
                if (!pOwner) pOwner=pThis;
            }
            user = uiMenuGetItemData((uiMENUH)lParam, wParam);
            return pOwner->HandleCmd(uiCMDSRC_MENU, uiMenuGetItemID((uiMENUH)lParam, wParam), wParam, (uiMENUH)lParam, user);
        }

        if (uMsg == WM_NOTIFY)
        {
            switch (((LPNMHDR)lParam)->code)
            {
            case LVN_GETDISPINFO:
                if (((NMLVDISPINFO*)lParam)->item.mask & LVIF_TEXT)
                {
                    textCB.mItem    = ((NMLVDISPINFO*)lParam)->item.iItem;
                    textCB.mSubItem = ((NMLVDISPINFO*)lParam)->item.iSubItem;
                    textCB.mUser    = (void*)((NMLVDISPINFO*)lParam)->item.lParam;
                    textCB.mppStr   = &((NMLVDISPINFO*)lParam)->item.pszText;
                    textCB.mStrLen  = ((NMLVDISPINFO*)lParam)->item.cchTextMax;

                    uMsg = uiWM_CTLLIST_TEXTCB;
                    wParam = (WPARAM)((LPNMHDR)lParam)->idFrom;
                    lParam = (LPARAM)&textCB;
                }
                break;

            case LVN_ITEMCHANGED:
                if (((LPNMLISTVIEW)lParam)->uChanged & LVIF_STATE)
                {
                    selChanged.mItem     = ((LPNMLISTVIEW)lParam)->iItem;
                    selChanged.mSubItem  = ((LPNMLISTVIEW)lParam)->iSubItem;
                    selChanged.mUser     = (void*)((LPNMLISTVIEW)lParam)->lParam;
                    selChanged.mNewState = ListView_GetItemState( (((LPNMHDR)lParam)->hwndFrom), ((LPNMLISTVIEW)lParam)->iItem, -1);

                    uMsg = uiWM_CTLLIST_SELCHANGED;
                    wParam = (WPARAM)((LPNMHDR)lParam)->idFrom;
                    lParam = (LPARAM)&selChanged;
                }
                break;

            case LVN_ITEMACTIVATE:
                uMsg = uiWM_CTLLIST_ACTIVATE;
                wParam = (WPARAM)((LPNMITEMACTIVATE)lParam)->hdr.idFrom;
                lParam = (LPARAM)((LPNMITEMACTIVATE)lParam)->iItem;
                break;

            case UDN_DELTAPOS:
                uMsg = uiWM_CTLSPIN_POSCHANGE;
                wParam = (WPARAM)((LPNMUPDOWN)lParam)->hdr.idFrom;
                lParam = (LPARAM)(((LPNMUPDOWN)lParam)->iPos + ((LPNMUPDOWN)lParam)->iDelta);
                break;
            }
        }

        if (uMsg == WM_ERASEBKGND)
        {
            uiWinPrepareGC(hWnd, (HDC)wParam);
            lParam = (LPARAM)uigpGC;
        }

        err = pThis->HandleMsg(uMsg, (void*)wParam, (void*)lParam);

        if (uMsg == WM_DESTROY)
            pThis->mhWin = NULL;
    }

    return err;
}

static LRESULT CALLBACK uiWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_CREATE)
    {
        uiWin* pThis = (uiWin*) (((CREATESTRUCT*)lParam)->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
        if (pThis)
            pThis->mhWin = hWnd;
    }

    bbERR err = uiWindowProcFilterMsg(hWnd, uMsg, wParam, lParam);

    if (err == uiEPASS)
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    return err;
}

bbERR uiRegisterWinClass()
{
    ATOM atom;
    WNDCLASSEX wc;

    memset(&wc, 0, sizeof(WNDCLASSEX));

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = uiWindowProc;
    wc.hInstance     = uighInstance;
    wc.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
    wc.lpszClassName = bbT("uiWin");

    atom = ::RegisterClassEx(&wc);
    if (!atom)
        return bbErrSet(uiEWINCLASSREG);

    wc.lpfnWndProc   = uiCtlValWindowProc;
    wc.hbrBackground = (HBRUSH)(COLOR_MENU + 1);
    wc.lpszClassName = bbT("uiCtlVal");
    wc.cbWndExtra    = bbSIZEOF_UPTR;

    atom = ::RegisterClassEx(&wc);
    if (!atom)
        return bbErrSet(uiEWINCLASSREG);

    return bbEOK;
}

uiWINH uiWinCreate(uiWC winclass, uiWINH hParent, const bbCHAR* pName, bbUINT id, bbU32 style, bbU32 styleEx, uiRect const* pRect, void* user)
{
    static const bbCHAR* const gWinClassNames[] = {
        bbT("uiWin"),
        bbT("uiCtlVal"),
        bbT("BUTTON"),
        bbT("EDIT"),
        bbT("BUTTON"),
        bbT("STATIC"),
        bbT("LISTBOX"),
        bbT("BUTTON"),
        WC_LISTVIEW,
        UPDOWN_CLASS,
        bbT("COMBOBOX")
    };

    static const bbU32 defaultstyles[] = {
        0,
        0,
        BS_PUSHBUTTON,
        ES_AUTOHSCROLL,
        BS_AUTORADIOBUTTON,
        0,
        LBS_NOINTEGRALHEIGHT | LBS_NOTIFY,
        BS_AUTOCHECKBOX,
        LVS_AUTOARRANGE | LVS_SHOWSELALWAYS,
        0,
        CBS_AUTOHSCROLL|CBS_DROPDOWN|CBS_NOINTEGRALHEIGHT 
    };

    static const bbU32 defaultexstyles[] = {
        0,
        0,
        0,
        uiWSE_CLIENTEDGE,
        0,
        0,
        0,
        0,
        0,
        0,
        0
    };

    if (!pRect)
        pRect = &uiWin::sDefaultRect;

    bbASSERT(winclass < sizeof(gWinClassNames)/sizeof(bbCHAR*));
    bbASSERT(sizeof(gWinClassNames)/sizeof(void*)==sizeof(defaultstyles)/sizeof(bbU32));
    bbASSERT(sizeof(defaultstyles)==sizeof(defaultexstyles));

    uiWINH hWin = ::CreateWindowEx(styleEx | defaultexstyles[winclass], gWinClassNames[winclass], pName, style | defaultstyles[winclass], 
                                   pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top,
                                   hParent, (style & uiWS_CHILD) ? (HMENU)id : NULL, uighInstance, user);

    if (!hWin)
        bbErrSet(uiEWINCREATE);

    if (winclass == uiWC_CtlList)
    {
        ListView_SetExtendedListViewStyle(hWin, ListView_GetExtendedListViewStyle(hWin) | LVS_EX_FULLROWSELECT);
    }

    return hWin;
}

#endif

bbERR uiWinCreateChildList(uiWin* const pThis, const uiWinDesc* pList, bbUINT count, bbU32 styleFix, bbU32 styleExFix)
{
    bbUINT i;
    int    d;
    uiWINH hChild;
    uiRect rect;

    bbASSERT(pThis->mhWin);

    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    HFONT hFont = (HFONT) ::SendMessage(pThis->mhWin, WM_GETFONT, 0,0);  
    #endif

    for (i=0; i<count; i++)
    {
        const uiWinDesc* const p = pList + i;

        d=(int)p->x; if (d!=uiPREV) { if (d<=0) rect.left=rect.right-d; else rect.left=d; }
        d=(int)p->y; if (d!=uiPREV) { if (d<=0) rect.top=rect.bottom-d; else rect.top=d; }
        rect.right  = rect.left + p->w;
        rect.bottom = rect.top  + p->h;

        if (!(hChild = uiWinCreate(p->wc, pThis->mhWin, p->pText, p->id, p->style | styleFix, p->styleEx | styleExFix, &rect, NULL)))
        {
            bbLog(bbWarning, bbT("CreateChildList cannot create child %d, 0x%08X\n"), i, GetLastError());
        }

        #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
        if (hFont && hChild) ::SendMessage(hChild, WM_SETFONT, (WPARAM)hFont, FALSE);
        #endif
    }

    return bbEOK;
}

uiWINH uiWin::Create(uiWINH hParent, const bbCHAR* pName, bbUINT id, bbU32 style, bbU32 styleEx, uiRect const* pRect)
{
    bbASSERT(!mhWin);
    uiWINH hWin = uiWinCreate(uiWC_Win, hParent, pName, id, style, styleEx, pRect, this);
    bbASSERT(mhWin == hWin);
    return mhWin;
}

void uiWinDestroy(uiWINH* const phWin)
{
    if (*phWin)
    {
        bbASSERT(::IsWindow(*phWin));

        BOOL success = ::DestroyWindow(*phWin);
        bbASSERT(success);

        *phWin = NULL;
    }
}

bbERR uiWin::HandleMsg(bbU32 msg, void* p0, void* p1)
{
    return uiEPASS; // indicate message was not handled
}

bbERR uiWin::HandleCmd(uiCMDSRC src, bbUINT cmd, bbUINT pos, void* hSrc, void* user)
{
    return uiEPASS;
}

bbERR uiWin::CreateChildList(const uiWinDesc* pList, bbUINT count)
{
    return uiWinCreateChildList(this, pList, count, uiWS_CHILD|uiWS_VISIBLE, 0);
}

int uiWin::UpdateScrollBar(uiWINSBTYPE const type, uiWMSB const scrollreason)
{
    uiWinScroll si;
    si.pos = 0;

    if (GetScrollBar(type, &si) == bbEOK)
    {
        if ((scrollreason != uiWMSB_THUMBPOSITION) &&
            (scrollreason != uiWMSB_ENDSCROLL))
        {
            switch (scrollreason)
            {
            case uiWMSB_BOTTOM:     si.pos=si.max-si.pagesize; break;
            case uiWMSB_LINEDOWN:   si.pos++; break;
            case uiWMSB_LINEUP:     si.pos--; break;
            case uiWMSB_PAGEDOWN:   si.pos+=si.pagesize; break;
            case uiWMSB_PAGEUP:     si.pos-=si.pagesize; break;
            case uiWMSB_TOP:        si.pos=0; break;
            case uiWMSB_THUMBTRACK: si.pos=si.trackpos; break;
            }
            if (si.pos<0) si.pos=0;
            if (si.pos>si.max) si.pos=si.max;

            si.mask = uiWINSBFLAG_POS;
            SetScrollBar(type, &si, 1);
        }
    }

    return si.pos;
}

bbCHAR* uiWinGetTextMem(uiWINH const hWin)
{
    int strlen = uiWinGetTextLen(hWin);
    if (!strlen)
        return NULL;

    bbCHAR* pMem = (bbCHAR*)bbMemAlloc(sizeof(bbCHAR) * ++strlen);
    if (!pMem)
        return NULL;

    pMem[0] = 0; // guard for case of unexpected failure
    uiWinGetText(hWin, pMem, strlen); // should always succeed

    return pMem;
}

bbCHAR* uiWinChildGetTextMem(uiWINH const hWin, bbUINT const id)
{
    uiWINH const hChild = uiWinGetChild(hWin, id);
    if (!hChild)
        return NULL;

    return uiWinGetTextMem(hChild);
}

void uiWinCenterRect(uiWINH const hWin, bbUINT const w, bbUINT const h, uiRect* const pRect)
{
    uiMONH hMon;
    uiMonInfo mi;

    if (uiWinGetRect(hWin, pRect) != bbEOK)
    {
        uiMonitorGetInfo(uiMonitorGetDefault(), &mi);
        *pRect = mi.workarea;
    }

    int left   = (bbS32)(pRect->left+pRect->right - w) >> 1;
    int top    = (bbS32)(pRect->top+pRect->bottom - h) >> 1;

    hMon = uiMonitorFromRect(pRect);
    if (hMon)
    {
        if (bbEOK == uiMonitorGetInfo(hMon, &mi))
        {
            int right  = left + w;
            int bottom = top + h;

            if (right > mi.workarea.right)
                left -= (right-mi.workarea.right);

            if (bottom > mi.workarea.bottom)
                top -= (bottom-mi.workarea.bottom);
            
            if (left < mi.workarea.left)
                left = mi.workarea.left;

            if (top < mi.workarea.top)
                top = mi.workarea.top;
        }
    }

    pRect->left   = left;
    pRect->top    = top;
    pRect->right  = left + w;
    pRect->bottom = top + h;
}

void uiWinSizingCenterRect(uiWINH const hParent, bbUINT const width, bbUINT const height, uiRect* const pRect)
{
    pRect->left = pRect->top = 0;
    pRect->right = width + (uiSysGet(uiSYS_BORDERSIZINGWIDTH)<<1);
    pRect->bottom = height + (uiSysGet(uiSYS_BORDERSIZINGHEIGHT)<<1) + uiSysGet(uiSYS_CAPTIONHEIGHT);

    uiWinCenterRect(hParent, pRect->right, pRect->bottom, pRect);
}

void uiWinStaticCenterRect(uiWINH const hParent, bbUINT const width, bbUINT const height, uiRect* const pRect)
{
    pRect->left = pRect->top = 0;
    pRect->right = width + (uiSysGet(uiSYS_BORDERFIXEDWIDTH)<<1);
    pRect->bottom = height + (uiSysGet(uiSYS_BORDERFIXEDHEIGHT)<<1) + uiSysGet(uiSYS_CAPTIONHEIGHT);

    uiWinCenterRect(hParent, pRect->right, pRect->bottom, pRect);
}

#ifdef bbDEBUG
static int uiWinPaintDebugMutex = 0;
#endif

ptGC* uiWinPaintStart(uiWINH const hWin, uiWinPaint* const pPaint)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)

    HDC hDC = ::BeginPaint(hWin, &pPaint->ps);
    bbASSERT(hDC);

    #ifdef bbDEBUG
    bbASSERT(uiWinPaintDebugMutex == 0);
    uiWinPaintDebugMutex = 1;
    #endif

    uiWinPrepareGC(hWin, hDC);

    if (pPaint->ps.rcPaint.right | pPaint->ps.rcPaint.bottom)
        uigpGC->SetClipBox((ptRect*)&pPaint->ps.rcPaint);

    return uigpGC;

    #else
    return NULL;
    #endif
}

void uiWinPaintEnd(uiWINH const hWin, uiWinPaint* const pPaint)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    #ifdef bbDEBUG
    bbASSERT(uiWinPaintDebugMutex == 1);
    uiWinPaintDebugMutex = 0;
    #endif
    ::EndPaint(hWin, &pPaint->ps);
    #else
    #endif
}

bbCHAR* uiDragGetPathMem(uiDRAGH const hDrag, bbUINT const index)
{
    bbUINT strlen = ::DragQueryFile(hDrag, index, NULL, 0);
    bbCHAR* const pBuf = bbStrAlloc(strlen);
    if (pBuf)
    {
        ::DragQueryFile(hDrag, index, pBuf, strlen+1);
    }
    return pBuf;
}

bbERR uiWinScreenToClient(uiWINH const hWin, uiRect* const pRect)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    if (!::ScreenToClient(hWin, (LPPOINT)pRect) ||
        !::ScreenToClient(hWin, (LPPOINT)pRect + 1))
    {
        return bbErrSet(bbESYS);
    }

    return bbEOK;
    #else
    #endif
}

bbERR uiWinClientToScreen(uiWINH const hWin, uiRect* const pRect)
{
    if (!::ClientToScreen(hWin, (LPPOINT)pRect) ||
        !::ClientToScreen(hWin, (LPPOINT)pRect + 1))
    {
        return bbErrSet(bbESYS);
    }
    return bbEOK;
}

