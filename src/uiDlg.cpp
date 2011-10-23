#include "uiDlg.h"
#include "babel/log.h"
#include "babel/str.h"
#include "babel/mem.h"

extern bbERR uiWindowProcFilterMsg(HWND hWnd, bbUINT uMsg, WPARAM wParam, LPARAM lParam);

struct uiDlgCreate
{
    uiDlg* pThis;
    uiRect rect;
};

static INT_PTR CALLBACK uiDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG_PTR const userdata = GetWindowLongPtr(hWnd, GWLP_USERDATA);

    if (userdata == (LONG_PTR)uiDialogProc)
        return FALSE;

    if (uMsg == WM_INITDIALOG)
    {
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)uiDialogProc);
        uiWinMove(hWnd, &((uiDlgCreate*)lParam)->rect, 0);
        uiDlg* const pThis = ((uiDlgCreate*)lParam)->pThis;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
        pThis->mhWin = hWnd;
        uMsg = uiWM_CREATE;
    }
    else if (uMsg == WM_GETMINMAXINFO)
    {
        uiDlg* const pThis = (uiDlg*)userdata;
        MINMAXINFO* const pMinMax = (MINMAXINFO*)lParam;
        INT_PTR ret = 1;

        if (pThis->mMinWidth)
        {
            pMinMax->ptMinTrackSize.x = (LONG)pThis->mMinWidth + (uiSysGet(uiSYS_BORDERSIZINGWIDTH)<<1);
            pMinMax->ptMinTrackSize.y = (LONG)pThis->mMinHeight + uiSysGet(uiSYS_CAPTIONHEIGHT) + (uiSysGet(uiSYS_BORDERSIZINGHEIGHT)<<1);
            ret = 0;
        }

        if (pThis->mMaxWidth)
        {
            pMinMax->ptMaxTrackSize.x = (LONG)pThis->mMaxWidth + (uiSysGet(uiSYS_BORDERSIZINGWIDTH)<<1);
            pMinMax->ptMaxTrackSize.y = (LONG)pThis->mMaxHeight + uiSysGet(uiSYS_CAPTIONHEIGHT) + (uiSysGet(uiSYS_BORDERSIZINGHEIGHT)<<1);
            ret = 0;
        }

        return ret;
    }

    bbERR err = uiWindowProcFilterMsg(hWnd, uMsg, wParam, lParam);

    if (err == uiEPASS)
        return FALSE;

    return TRUE;
}

static bbUINT uiDlgCreateModal(uiDlg* const pThis, uiWINH hParent, const bbCHAR* pName, bbU32 style, bbU32 styleEx, uiRect const* pRect, int const modal)
{
    bbUINT          ret = -1;
    HGLOBAL         hgbl = NULL;
    LPDLGTEMPLATE   lpdt = NULL;
    LPWORD          lpw;
    int             nchar;
    uiDlgCreate     create;

    bbASSERT(hParent);

    create.pThis = pThis;

    if (!pRect)
        ::GetClientRect(hParent, (RECT*)&create.rect);
    else
        create.rect = *pRect;

    #define uiDLGTEMPLATESIZE 1024

    nchar = 1 + bbStrLen(pName);
    if (nchar > ((uiDLGTEMPLATESIZE - sizeof(DLGTEMPLATE) - 2*sizeof(WORD) / sizeof(bbCHAR) - 8*2))) // adjust this if code changed below
    {
        bbErrSet(bbENOMEM);
        goto uiDlg_CreateModal_err;
    }

    if ( ((hgbl = GlobalAlloc(GMEM_ZEROINIT, uiDLGTEMPLATESIZE)) == NULL) ||
         ((lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl)) == NULL))
    {
        bbErrSet(bbENOMEM);
        goto uiDlg_CreateModal_err;
    }
 
    lpdt->style = style | WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION | DS_SETFONT;
    lpdt->dwExtendedStyle = styleEx;
    lpdt->cdit = 0;  // number of controls

    lpdt->x  = (short)create.rect.left;
    lpdt->y  = (short)create.rect.top;
    lpdt->cx = (short)(create.rect.right - create.rect.left);
    lpdt->cy = (short)(create.rect.bottom - create.rect.top);

    lpw = (LPWORD) (lpdt + 1);
    *lpw++ = 0;   // no menu
    *lpw++ = 0;   // predefined dialog box class (by default)

    bbMemMove(lpw, pName, nchar*sizeof(bbCHAR)); // title
    lpw   += nchar;

    *lpw++ = 8;   // font
    bbStrCpy((bbCHAR*)lpw, bbT("Tahoma"));


    if (lpdt) GlobalUnlock(hgbl); 

    if (modal)
    {
        ret = DialogBoxIndirectParam(uighInstance, (LPDLGTEMPLATE) hgbl, hParent, (DLGPROC) uiDialogProc, (LPARAM)&create); 
    }
    else
    {
        HWND hWin = CreateDialogIndirectParam(uighInstance, (LPDLGTEMPLATE) hgbl, hParent, (DLGPROC) uiDialogProc, (LPARAM)&create);
        if (!hWin)
            bbErrSet(uiEWINCREATE);
        else
            ret = 0;

        bbASSERT(pThis->mhWin==hWin);
    }

    uiDlg_CreateModal_err:
    if (hgbl) GlobalFree(hgbl); 
    return ret;
}

bbUINT uiDlg::CreateModal(uiWINH hParent, const bbCHAR* pName, bbU32 style, bbU32 styleEx, uiRect const* pRect)
{
    return uiDlgCreateModal(this, hParent, pName, style, styleEx, pRect, 1);
}

uiWINH uiDlg::Create(uiWINH hParent, const bbCHAR* pName, bbUINT id, bbU32 style, bbU32 styleEx, uiRect const* pRect)
{
    if (uiDlgCreateModal(this, hParent, pName, style, styleEx, pRect, 0) != 0)
        return NULL;

    return mhWin;
}

/*
bbUINT uiDlg::ShowModal(uiWINH hParent)
{
    uiWin parent(hParent);
    bbUINT ret = -1;
    MSG msg;

    parent.Enable(0);       // Disable parent
    Show(uiWINSHOW_SHOW);
    SetFocus();

    msg.message = WM_NULL;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!IsDialogMessage(mhWin, &msg)) // process messages for controls, e.g. Tab navigation
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == uiWM_DLGEND)
        {
            ret = msg.wParam;
            break;
        }
    }
    if (msg.message == WM_QUIT)
        PostQuitMessage((int)msg.wParam);

    parent.Enable(1);
    Show(uiWINSHOW_HIDE);
    //::SetFocus(parent.mhWin);
    //::SetWindowPos(parent.mhWin, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    return ret;
}
*/

void uiDlg::EndModal(bbUINT code)
{
    if (!::EndDialog(mhWin, (INT_PTR) code))
        bbLog(bbErr, bbT("EndDialog failed %x\n"), GetLastError());

    //PostMessage(mhWin, uiWM_DLGEND, (WPARAM)code, 0);
}

bbERR uiDlg::HandleCmd(uiCMDSRC src, bbUINT cmd, bbUINT pos, void* hSrc, void* user)
{
    if ((cmd == uiCMD_OK) || (cmd == uiCMD_CANCEL))
    {
        EndModal(cmd);
        return bbEOK;
    }

    return uiEPASS; // indicate message was not handled
}

extern bbERR uiWinCreateChildList(uiWin* const pThis, const uiWinDesc* pList, bbUINT count, bbU32 styleFix, bbU32 styleExFix);

bbERR uiDlg::CreateChildList(const uiWinDesc* pList, bbUINT count)
{
    return uiWinCreateChildList(this, pList, count, uiWS_CHILD|uiWS_VISIBLE, WS_EX_NOPARENTNOTIFY);
}

bbUINT uiDlgMsg(uiWINH hWin, const bbCHAR* pCaption, const bbCHAR* pMsg, bbUINT opt)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return MessageBox(hWin, pMsg, pCaption, opt);
#else
#endif
}

struct uiDlgFileNameBlock
{
    bbU32 CurFileOffset;
    bbU32 FirstFileOffset;
};

bbCHAR* uiDlgFileName(uiWINH hWin, const bbCHAR* pPath, bbUINT opt, uiDlgFileNameFilter* const pFilter)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)

    bbCHAR* pFileNameBuffer;
    bbUINT const bufferstart = (opt & uiDLGFILEOPT_MULTISELECT) ? sizeof(uiDlgFileNameBlock)/sizeof(bbCHAR) : 0;
    OPENFILENAME ofn;

    bbMemClear(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = hWin;

    if (pPath)
        ofn.nMaxFile = bbStrLen(pPath) + 1;

    if (ofn.nMaxFile < 512)
        ofn.nMaxFile = 512;

    if ((pFileNameBuffer = (bbCHAR*) bbMemAlloc(sizeof(bbCHAR) * ofn.nMaxFile + sizeof(uiDlgFileNameBlock))) == NULL)
        return NULL;

    ofn.lpstrFile = pFileNameBuffer + bufferstart;

    if (pPath)
        bbStrCpy(ofn.lpstrFile, pPath);
    else
        *ofn.lpstrFile = 0;

    if (pFilter)
    {
        ofn.nFilterIndex = pFilter->FilterIndex;
        ofn.lpstrFilter  = pFilter->pFilter;
    }

    ofn.Flags = (opt & (uiDLGFILEOPT_MULTISELECT|uiDLGFILEOPT_NODEREFERENCELINKS|uiDLGFILEOPT_OVERWRITEPROMPT))
        | OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_NOTESTFILECREATE | OFN_HIDEREADONLY | OFN_EXPLORER;

    BOOL (__stdcall *fnGetFileName)(LPOPENFILENAME) = (opt & uiDLGFILEOPT_SAVE) ? GetSaveFileName : GetOpenFileName;

    if (fnGetFileName(&ofn) == 0)
    {
        DWORD err = CommDlgExtendedError();

        if (err == FNERR_BUFFERTOOSMALL)
        {
            ofn.nMaxFile = *(WORD*)ofn.lpstrFile;
            if (bbMemRealloc(sizeof(bbCHAR) * ofn.nMaxFile + sizeof(uiDlgFileNameBlock), (void**)&pFileNameBuffer) != bbEOK)
                goto uiDlgFileSave_err;
            ofn.lpstrFile = pFileNameBuffer + bufferstart;

            if (fnGetFileName(&ofn))
                goto uiDlgFileSave_ok;
            err = CommDlgExtendedError();
        }

        if (err == 0)
        {
            bbErrSet(bbEEND);
        }
        else
        {
            bbErrSet(bbESYS); //xxx add error codes
            bbLog(bbErr, bbT("uiDlgFileSave: error %X\n"), err);
        }

        goto uiDlgFileSave_err;
    }

    uiDlgFileSave_ok:

    if (pFilter)
    {
        pFilter->FilterIndex = ofn.nFilterIndex;
    }

    if (opt & uiDLGFILEOPT_MULTISELECT)
    {
        ((uiDlgFileNameBlock*)pFileNameBuffer)->CurFileOffset = 
        ((uiDlgFileNameBlock*)pFileNameBuffer)->FirstFileOffset = ofn.nFileOffset;

        if (ofn.nFileOffset > 0)
            pFileNameBuffer[ofn.nFileOffset + bufferstart - 1] = '\0'; // 0-term path also for single-selection case
    }

    return pFileNameBuffer;

    uiDlgFileSave_err:
    bbMemFree(pFileNameBuffer);
    return NULL;
#else
#endif
}

bbCHAR* uiDlgFileNameNext(bbCHAR* pFileNames, bbCHAR* pBuffer)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)

    uiDlgFileNameBlock* pBlock = (uiDlgFileNameBlock*)pFileNames;
    pFileNames = (bbCHAR*)((bbU8*)pFileNames + sizeof(uiDlgFileNameBlock));

    bbCHAR* pNewBuffer;
    bbU32 pathlen = pBlock->FirstFileOffset - 1;
    bbU32 offset  = pBlock->CurFileOffset;
    bbU32 filelen = bbStrLen(pFileNames + offset);

    if (filelen == 0)
    {
        pBlock->CurFileOffset = pBlock->FirstFileOffset;
        bbMemFree(pBuffer);
        return (bbCHAR*)-1;
    }

    pNewBuffer = pBuffer;
    if (bbMemRealloc(sizeof(bbCHAR) * (filelen + pathlen + 2), (void**)&pNewBuffer) != bbEOK) // space for path + '\' + file + 0-terminator
    {
        bbMemFree(pBuffer);
        return NULL;
    }

    if (!pBuffer)
    {
        bbStrCpy(pNewBuffer, pFileNames); // copy path
        pNewBuffer[pathlen] = bbDIRDELIM;
    }
    bbStrCpy(pNewBuffer + pathlen + 1, pFileNames + offset); // copy filename

    pBlock->CurFileOffset = offset + filelen + 1;

    return pNewBuffer;
#else
#endif
}

