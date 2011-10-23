#include "uictl.h"
#include "babel/mem.h"
#include "babel/str.h"

int uiCtlListInsertColumn(uiWINH const hWin, int const col, const bbCHAR* const pName, bbUINT const width)
{
    LVCOLUMN c;
    c.mask    = LVCF_TEXT | LVCF_WIDTH;
    c.cx      = width;
    c.pszText = (TCHAR*)pName;
    const int ret = ListView_InsertColumn(hWin, col, &c);
    if (ret < 0)
        bbErrSet(bbESYS);
    return ret;
}

void uiCtlListSetSel(uiWINH const hWin, int const pos)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ListView_SetSelectionMark(hWin, pos);
    ListView_SetItemState(hWin, pos, pos>=0 ? LVIS_SELECTED : 0, LVIS_SELECTED);
    ListView_EnsureVisible(hWin, pos, TRUE);
#else
#endif
}

int uiCtlListBoxInsertItem(uiWINH const hWin, int const pos, const bbCHAR* const pStr, const void* const user)
{
    int const ret = ::SendMessage(hWin, LB_INSERTSTRING, (WPARAM)pos, (LPARAM)pStr);

    if (ret >= 0)
    {
        if (::SendMessage(hWin, LB_SETITEMDATA, ret, (LPARAM)user) != LB_ERR)
            return ret;

        uiCtlListBoxDeleteItem(hWin, ret);
    }

    return bbErrSet(bbESYS);
}

int uiCtlComboInsertItem(uiWINH const hWin, const int pos, const bbCHAR* const pStr, const void* const user)
{
    int const ret = ::SendMessage(hWin, CB_INSERTSTRING, (WPARAM)pos, (LPARAM)pStr);

    if (ret >= 0)
    {
        if (::SendMessage(hWin, CB_SETITEMDATA, ret, (LPARAM)user) != CB_ERR)
            return ret;

        uiCtlComboDeleteItem(hWin, ret);
    }

    return bbErrSet(bbESYS);
}

int uiCtlListInsertItem(uiWINH const hWin, int const pos, const bbCHAR* const pName, const void* const user)
{
    int listindex;
    LVITEM lvitem;

    lvitem.mask     = LVIF_TEXT | LVIF_PARAM;
    lvitem.iItem    = pos;
    lvitem.iSubItem = 0;
    lvitem.pszText  = pName == (bbCHAR*)-1 ? LPSTR_TEXTCALLBACK : (TCHAR*)pName;
    lvitem.lParam   = (long)user;

    if ((listindex = ListView_InsertItem(hWin, &lvitem)) < 0)
        bbErrSet(bbESYS);

    return listindex;
}

bbERR uiCtlListSetSubItem(uiWINH const hWin, int const pos, int const subpos, const bbCHAR* const pName)
{

    LVITEM lvitem;

    lvitem.mask     = LVIF_TEXT;
    lvitem.iItem    = pos;
    lvitem.iSubItem = subpos;
    lvitem.pszText  = pName == (bbCHAR*)-1 ? LPSTR_TEXTCALLBACK : (TCHAR*)pName; 

    if (!ListView_SetItem(hWin, &lvitem))
        return bbErrSet(bbESYS);

    return bbEOK;
}

bbERR uiCtlListGetItemData(uiWINH const hWin, int const pos, const void** const pUser)
{
    LVITEM lvitem;

    lvitem.mask = LVIF_PARAM;
    lvitem.iItem = pos;
    lvitem.iSubItem = 0;

    if (!ListView_GetItem(hWin, &lvitem))
        return bbErrSet(bbESYS);

    *pUser = (const void*)lvitem.lParam;
    return bbEOK;
}

bbCHAR* uiCtlListGetItemStringMem(uiWINH const hWin, bbUINT pos, bbUINT subpos, bbUINT* const pLen)
{
    bbCHAR* pStr = bbStrAlloc(1024);

    if (pStr)
    {
        LVITEM lvitem;

        lvitem.mask       = LVIF_TEXT;
        lvitem.iItem      = pos;
        lvitem.iSubItem   = subpos;
        lvitem.pszText    = pStr;
        lvitem.cchTextMax = 1024-1;

        if (!ListView_GetItem(hWin, &lvitem))
        {
            bbErrSet(bbESYS);
            bbMemFree(pStr);
            return NULL;
        }

        bbUINT len = bbStrLen(lvitem.pszText);

        if (lvitem.pszText != pStr)
        {
            if (len > 1024-1)
                len = 1024-1;

            bbStrNCpy(pStr, lvitem.pszText, len+1);
        }

        if (pLen) 
            *pLen=len;
    }

    return pStr;
}

int uiCtlListNextSel(uiWINH const hWin, int last)
{
    int const count = uiCtlListGetItemCount(hWin);

    for(;;)
    {
        if (++last >= count)
            return -1;

        bbUINT const state = uiCtlListGetItemState(hWin, last);

        if (state & uiCTLLIST_ITEMSTATE_SELECTED)
            return last;
    }
}
