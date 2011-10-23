#include "uiSizer.h"
#include "uiWin.h"

uiSizer::uiSizer()
{
}

uiSizer::~uiSizer()
{
}

bbERR uiSizer::AddWin(uiWINH const hWin, bbU32 const opt)
{
    if (hWin)
    {
        if (!mItems.Grow(1))
            return bbELAST;

        uiSizerItem* const pItem = mItems.GetPtrLast();
        pItem->hWin = hWin;
        pItem->opt  = opt;
    }
    return bbEOK;
}

bbERR uiSizer::AddWinList(uiWINH const hParent, const uiSizerWin* pItem, bbUINT count)
{
    bbERR err = bbEOK;

    const uiSizerWin* const pItemEnd = pItem + count;

    while (pItem < pItemEnd)
    {
        uiWINH hWin = uiWinGetChild(hParent, pItem->id);

        if (AddWin(hWin, pItem->opt) != bbEOK)
            err = bbELAST; // sticky error state

        pItem++;
    }

    return err;
}

void uiSizer_line::OnSize(const uiRect* const pRect)
{
    int w;
    uiSizerItem*       pItem    = mItems.GetPtr();
    uiSizerItem* const pItemEnd = mItems.GetPtrEnd();

    uiRect rect;

    while (pItem < pItemEnd)
    {
        uiWINH const hParent = uiWinGetParent(pItem->hWin);
        if (hParent)
        {
            uiWinGetRect(pItem->hWin, &rect);
            uiWinScreenToClient(hParent, &rect);

            int const spacing = pItem->opt & uiSIZERITEM_SPACEMASK;

            if (mVerticalLine)
            {
                w = rect.right - rect.left;

                if (pItem->opt & uiSIZERITEM_LINE_RIGHT)
                {
                    if ((rect.right=mLinePos) < 0) rect.right+=pRect->right;
                    rect.right -= spacing;
                    if (pItem->opt & uiSIZERITEM_LINE_FIXED)
                        rect.left = rect.right - w;
                    else
                        if (rect.right < rect.left) rect.left = rect.right;
                }
                else
                {                    
                    if ((rect.left=mLinePos) < 0) rect.left+=pRect->right;
                    rect.left += spacing;
                    if (pItem->opt & uiSIZERITEM_LINE_FIXED)
                        rect.right = rect.left + w;
                    else
                        if (rect.left > rect.right) rect.right = rect.left;
                }
            }
            else
            {
                w = rect.bottom - rect.top;

                if (pItem->opt & uiSIZERITEM_LINE_BOTTOM)
                {
                    if ((rect.bottom=mLinePos)<0) rect.bottom+=pRect->bottom;
                    rect.bottom -= spacing;
                    if (pItem->opt & uiSIZERITEM_LINE_FIXED)
                        rect.top = rect.bottom - w;
                    else
                        if (rect.bottom < rect.top) rect.top = rect.bottom;
                }
                else
                {
                    if ((rect.top=mLinePos) < 0) rect.top+=pRect->bottom;
                    rect.top += spacing;
                    if (pItem->opt & uiSIZERITEM_LINE_FIXED)
                        rect.bottom = rect.top + w;
                    else
                        if (rect.top > rect.bottom) rect.bottom = rect.top;
                }
            }

            uiWinMove(pItem->hWin, &rect, 1);
        }
        pItem++;
    }
}

void uiSizer_chain::OnSize(const uiRect* const pRect)
{
    uiSizerItem*       pItem    = mItems.GetPtr();
    uiSizerItem* const pItemEnd = mItems.GetPtrEnd();
    uiRect rect, oldrect;

    while (pItem < pItemEnd)
    {
        uiWINH const hParent = uiWinGetParent(pItem->hWin);
        if (hParent)
        {
            uiWinGetRect(pItem->hWin, &rect);
            uiWinScreenToClient(hParent, &rect);

            bbU32 const opt = pItem->opt;

            if ((pItem != mItems.GetPtr()) &&  // first item is fixed
                ((opt & uiSIZERITEM_CHAIN_ABSOLUTE)==0))
            {
                int const spacing = (int)opt & uiSIZERITEM_SPACEMASK;
                int const w = rect.right-rect.left;
                int const h = rect.bottom-rect.top;

                switch (opt >> 14)
                {
                case (bbU32)uiSIZERITEM_CHAIN_LEFT >> 14:
                    rect.left = oldrect.right + spacing;
                    rect.right = rect.left + w;
                    break;
                case (bbU32)uiSIZERITEM_CHAIN_RIGHT >> 14:
                    rect.right = oldrect.left - spacing;
                    rect.left = rect.right - w;
                    break;
                case (bbU32)uiSIZERITEM_CHAIN_TOP >> 14:
                    rect.top = oldrect.bottom + spacing;
                    rect.bottom = rect.top + h;
                    break;
                case (bbU32)uiSIZERITEM_CHAIN_BOTTOM >> 14:
                    rect.bottom = oldrect.top - spacing;
                    rect.top = rect.bottom - h;
                    break;
                }

                if (opt >= uiSIZERITEM_CHAIN_TOP)
                {
                    if (opt & uiSIZERITEM_CHAIN_ALIGNRIGHT)
                    {
                        rect.right = oldrect.right;
                        rect.left = rect.right - w;
                    }
                    else
                    {
                        rect.left = oldrect.left;
                        rect.right = rect.left + w;
                    }
                }
                else
                {
                    if (opt & uiSIZERITEM_CHAIN_ALIGNBOTTOM)
                    {
                        rect.bottom = oldrect.bottom;
                        rect.top = rect.bottom - h;
                    }
                    else
                    {
                        rect.top = oldrect.top;
                        rect.bottom = rect.top + h;
                    }
                }

                uiWinMove(pItem->hWin, &rect, 1);
            }
            oldrect = rect;
        }
        pItem++;
    }
}
