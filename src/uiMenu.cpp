#include "uiMenu.h"
#include "babel/Arr.h"
#include "babel/str.h"
#include "babel/bit.h"

void uiMenuSetOwner(uiMENUH const hMenu, uiWINH const hOwner)
{
    #if !bbQT
    MENUINFO mi;

    memset(&mi, 0, sizeof(mi));
    mi.cbSize     = sizeof(mi);
    mi.fMask      = MIM_STYLE | MIM_MENUDATA;
    mi.dwStyle    = MNS_NOTIFYBYPOS;
    mi.dwMenuData = (DWORD)hOwner;

    ::SetMenuInfo(hMenu, &mi);
    #endif
}

uiWINH uiMenuGetOwner(uiMENUH const hMenu)
{
    #if !bbQT
    MENUINFO mi;

    memset(&mi, 0, sizeof(mi));
    mi.cbSize     = sizeof(mi);
    mi.fMask      = MIM_MENUDATA;

    if (!::GetMenuInfo(hMenu, &mi))
        return NULL;

    return (uiWINH)mi.dwMenuData;
    #endif
}

uiMENUH uiMenuCreate(uiWINH const hOwner)
{
#if bbQT
    return new QMenu;
#else
    uiMENUH hMenu;

    if ((hMenu = ::CreatePopupMenu()) == NULL)
    {
        bbErrSet(bbENOMEM);
        return NULL;
    }

    uiMenuSetOwner(hMenu, hOwner);

    return hMenu;
#endif
}

uiMENUH uiMenuCreateBar(uiWINH const hOwner)
{
    uiMENUH hMenu;

    if ((hMenu = ::CreateMenu()) == NULL)
    {
        bbErrSet(bbENOMEM);
        return NULL;
    }

    uiMenuSetOwner(hMenu, hOwner);

    return hMenu;
}

void uiMenuDestroy(uiMENUH* const phMenu)
{
    if (*phMenu)
    {
        BOOL success = ::DestroyMenu(*phMenu);
        bbASSERT(success);
        *phMenu = NULL;
    }
}

uiMENUH uiMenuCreate(const uiMenuItem* const pList)
{
    uiMENUH hMenu = uiMenuCreate();

    if (hMenu)
    {
        if (bbEOK != uiMenuInsertItemList(hMenu, 0, pList))
            uiMenuDestroy(&hMenu);
    }

    return hMenu;
}

bbERR uiMenuInsertSubMenu(uiMENUH const hMenu, bbUINT pos, uiMENUH hSubMenu, const bbCHAR* pStr)
{
    #if bbQT

    QAction* before = hMenu->actions().value(pos, NULL);
    QAction* pNew = hMenu->insertMenu(before, hSubMenu);
    hSubMenu->setParent(hMenu);
    pNew->setText(QString::fromUtf16((const ushort*)pStr));
    return bbEOK;

    #elif (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)

    if (!InsertMenu(hMenu, pos, MF_BYPOSITION | MF_STRING | MF_POPUP, (UINT_PTR)hSubMenu, pStr))
        return bbErrSet(bbENOMEM);
    return bbEOK;

    #else
    #endif
}

bbERR uiMenuInsertItem(uiMENUH const hMenu, bbUINT pos, bbUINT id, const bbCHAR* pStr, const void* const user)
{
    #if bbQT

    QAction* before = hMenu->actions().value(pos, NULL);
    QAction* act = new QAction(QString::fromUtf16((const ushort*)pStr));
    act->setData(user);
    hMenu->insertAction(before, act)
    return bbEOK;

    #elif (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)

    MENUITEMINFO mi;

    mi.cbSize = sizeof(mi);
    mi.fMask = MIIM_DATA | MIIM_ID | MIIM_STRING;
    mi.wID = id;
    mi.dwItemData = (ULONG_PTR)user;
    mi.dwTypeData = (LPTSTR)pStr;

    return bbErrSet((bbERR)(1^::InsertMenuItem(hMenu, pos, TRUE, &mi)));

    #else
    #endif
}

bbERR uiMenuInsertSeparator(uiMENUH const hMenu, bbUINT pos)
{
    #if bbQT

    QAction* before = hMenu->actions().value(pos, NULL);
    hMenu->insertSeparator(before);
    return bbEOK;

    #elif (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    if (!::InsertMenu(hMenu, pos, MF_BYPOSITION | MF_SEPARATOR, 0, NULL))
        return bbErrSet(bbENOMEM);
    return bbEOK;
    #else
    #endif
}

bbERR uiMenuUpdateItem(uiMENUH const hMenu, bbUINT pos, bbUINT id, const bbCHAR* pStr, const void* const user)
{
    #if bbQT

    QAction* act = hMenu->actions().value(pos, NULL);
    if (!act)
        return bbErrSet(bbENOTFOUND);
    act->setText(QString::fromUtf16((const ushort*)pStr));
    act->setData(user);
    return bbEOK;

    #elif (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)

    MENUITEMINFO mi;

    mi.cbSize = sizeof(mi);
    mi.fMask = 0;
    if (user != (void*)-1) mi.fMask |= MIIM_DATA;
    if (id != (bbUINT)-1)  mi.fMask |= MIIM_ID;
    if (pStr != NULL)      mi.fMask |= MIIM_STRING;
    mi.wID = id;
    mi.dwItemData = (ULONG_PTR)user;
    mi.dwTypeData = (LPTSTR)pStr;

    return bbErrSet((bbERR)(1^::SetMenuItemInfo(hMenu, pos, TRUE, &mi)));

    #else
    #endif
}

void* uiMenuGetItemData(uiMENUH const hMenu, bbUINT pos)
{
    #if bbQT

    QAction* act = hMenu->actions().value(pos, NULL);
    if (!act)
        return bbErrSet(bbENOTFOUND);

    return act->data().value<void*>();

    #elif (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)

    MENUITEMINFO mi;

    mi.cbSize = sizeof(mi);
    mi.fMask = MIIM_DATA;

    if (!::GetMenuItemInfo(hMenu, pos, TRUE, &mi))
    {
        bbErrSet(bbESYS);
        return NULL;
    }

    bbErrSet(bbEOK);
    return (void*)mi.dwItemData;

    #else
    #endif
}

#if bbQT

int uiMenuGetItemString(uiMENUH const hMenu, bbUINT pos, bbCHAR* pStr, bbUINT const buflen)
{
    QAction* act = hMenu->actions().value(pos, NULL);
    if (!act)
        return bbErrSet(bbENOTFOUND);

    int len = act->text().size();
    if (pStr)
    {
        const bbCHAR* pSrc = (const bbCHAR*) act->text().utf16();
        bbStrNCpy(pStr, pSrc, buflen);
    }
    return len;
}
#endif

#if bbQT
bbUINT uiMenuGetItemState(uiMENUH const hMenu, bbUINT pos)
{
    QAction* act = hMenu->actions().value(pos, NULL);
    if (!act)
        return bbErrSet(bbENOTFOUND);

    bbUINT state = 0;
    if (act->checked()) state |= uiMENUITEMFLAG_CHECK;
    if (!act->enabled()) state |= uiMENUITEMFLAG_GREY;
    return state;
}
#endif

bbCHAR* uiMenuGetItemStringMem(uiMENUH const hMenu, bbUINT pos, bbUINT* const pLen)
{
    bbUINT strlen = uiMenuGetItemString(hMenu, pos, NULL, 0) + 1;
    if (strlen == 1)
        return NULL;

    bbCHAR* pStr = (bbCHAR*)bbMemAlloc(sizeof(bbCHAR) * strlen);
    if (!pStr)
        return NULL;

    strlen = uiMenuGetItemString(hMenu, pos, pStr, strlen);
    if (pLen) *pLen = strlen;
    bbASSERT((int)strlen > 0);

    return pStr;    
}

int uiMenuFindItemOffs(uiMENUH const hMenu, const bbCHAR* const pName, bbUINT const stroffset)
{
    int count = uiMenuGetItemCount(hMenu);
    bbUINT strlen;
    
    while (--count >= 0) // resolves to -1 on not found, and -2 on error from uiMenuGetItemCount()
    {
        bbCHAR* const pStr = uiMenuGetItemStringMem(hMenu, count, &strlen);

        if (pStr)
        {
            int const cmp = (stroffset < strlen) ? bbStrCmp(pStr+stroffset, pName) : -1;

            bbMemFree(pStr);

            if (cmp == 0)
                break;
        }
    }

    return count;
}

int uiMenuFindItem(uiMENUH const hMenu, const bbCHAR* const pName)
{
    return uiMenuFindItemOffs(hMenu, pName, 0);
}

int uiMenuFindItemData(uiMENUH const hMenu, bbUINT const start, bbUINT const max, int cmdID, void* user)
{
    int found = -1;
    int end = uiMenuGetItemCount(hMenu);

    if ((int)(start + max) < end)
        end = start + max;

    for(int i=start; i<end; ++i)
    {
        if (uiMenuGetItemData(hMenu, i) == user)
        {
            if ((cmdID != -1) && ((int)uiMenuGetItemID(hMenu, i) != cmdID))
               continue;
            found=i;
            break;
        }
    }

    return found;
}

bbERR uiMenuItemHistoryInsert(uiMENUH const hMenu, bbUINT const start, bbUINT const maxcount, const bbCHAR* pStr, bbU32 const opt, void* user)
{
    bbUINT count, i, key, deleteMe = (bbUINT)-1;
    bbCHAR* pStrEsc = NULL;

    // - Check if string already in list
    int found;
    if (opt & uiMENUITEMHISTORY_CMPUSERDATA)
    {
        found = uiMenuFindItemData(hMenu, start, maxcount, (opt & uiMENUITEMHISTORY_CMPID) ? (opt>>16) : -1, user);
    }
    else
    {
        found = uiMenuFindItemOffs(hMenu, pStr, (opt & uiMENUITEMHISTORY_AUTOKEY) ? 3 : 0);
        if (found < -1)
            return bbELAST;
    }

    // - Escape string with key
    if (opt & uiMENUITEMHISTORY_AUTOKEY)
    {
        bbUINT strlen = bbStrLen(pStr) + 4;
        if ((pStrEsc = (bbCHAR*)bbMemAlloc(sizeof(bbCHAR) * strlen)) == NULL)
            return bbELAST;

        pStrEsc[0] = '&';
        pStrEsc[1] = '1';
        pStrEsc[2] = ' ';
        bbStrCpy(pStrEsc+3, pStr);
        pStr = pStrEsc;
    }

    // - Insert new item to start of list
    bbERR err = uiMenuInsertItem(hMenu, start, opt>>16, pStr, user);
    bbMemFree(pStrEsc);
    if (err != bbEOK)
        return bbELAST;

    // - Delete overflowing or replaced item
    if ((found >= 0) && (!(opt & uiMENUITEMHISTORY_ALLOWDOUBLE)))
    {
        uiMenuDeleteItem(hMenu, found+1);
    }
    else
    {
        count = (bbUINT)uiMenuGetItemCount(hMenu);
        if ((int)count < 0)
        {
            uiMenuDeleteItem(hMenu, start);
            return bbELAST;
        }

        if ((count-start) > maxcount)
            deleteMe = start + maxcount;
    }

    // - Update keys for rest of list
    if (opt & uiMENUITEMHISTORY_AUTOKEY)
    {
        count = (bbUINT)uiMenuGetItemCount(hMenu);

        for (key='2', i=start+1; (int)i<(int)count; i++)
        {
            bbUINT strLen;
            bbCHAR* pStrEx = uiMenuGetItemStringMem(hMenu, i, &strLen);
            if (pStrEx)
            {
                if ((strLen>=3) && (pStrEx[2]==' '))
                {
                    if (i < deleteMe)
                    {
                        if (key <= 'Z')
                        {
                            pStrEx[1] = (bbCHAR)key;
                        }
                        else
                        {
                            pStrEx[0] = pStrEx[1] = ' ';
                        }

                        if (++key == ('9'+1)) key='0';
                        if (key == '1') key='A';

                        uiMenuUpdateItem(hMenu, i, (bbUINT)-1, pStrEx, (void*)-1); // ignore error
                    }
                }
                else if (deleteMe == i)
                {
                    deleteMe = (bbUINT)-1;
                }

                bbMemFree(pStrEx);
            }
        }
    }

    if (deleteMe != (bbUINT)-1)
        uiMenuDeleteItem(hMenu, deleteMe);

    return bbEOK;
}

bbERR uiMenuInsertStringList(uiMENUH const hMenu, bbUINT startpos, const bbCHAR* const* const pStringList, bbUINT count, bbU32 opt)
{
    bbUINT i;
    uiMenuAutoKey autokey;
    autokey.Init(opt);

    if (!count)
        return bbEOK;

    // - temporary block for uiMenuItem[] array
    uiMenuItem* pItemList = (uiMenuItem*) bbMemAlloc(sizeof(uiMenuItem) * count);
    if (!pItemList)
        return bbELAST;

    bbMemClear(pItemList, sizeof(uiMenuItem) * count);

    // - convert string list into uiMenuItem[] array, optionally use autokey
    i=0;
    do
    {
        if ((opt & (uiMENUAUTOKEY_ALPHA|uiMENUAUTOKEY_NUM)) && 
            ((pItemList[i].pStr = autokey.GetEscapedString(pStringList[i])) != NULL))
        {
            pItemList[i].flags = uiMENUITEMFLAG_ALLOCATED;
        }
        else
        {
            pItemList[i].pStr = pStringList[i]; // otherwise use non-escaped string
        }

        pItemList[i].cmdid = opt >> 16;
        pItemList[i].data = (void*)i;

    } while (++i < count);
    pItemList[count-1].flags |= uiMENUITEMFLAG_LAST;

    // - insert into menu
    const bbERR err = uiMenuInsertItemList(hMenu, startpos, pItemList);

    // - free temporary memory
    uiMenuItem* pTmp = pItemList;
    do
    {
        if (pTmp->flags & uiMENUITEMFLAG_ALLOCATED)
            bbMemFree((void*)pTmp->pStr);
    } while (!((pTmp++)->flags & uiMENUITEMFLAG_LAST));

    bbMemFree(pItemList);

    return err;
}

bbERR uiMenuInsertItemList(uiMENUH const hMenu, bbUINT startpos, const uiMenuItem* pItem)
{
    bbUINT  pos;
    bbUINT  flags;
    bbERR   err;
    uiMENUH hSubMenu;

    if ((pos=startpos) == (bbUINT)-1)
    {
        if ((int)(pos = uiMenuGetItemCount(hMenu)) < 0)
            return bbELAST;
    }

    do
    {
        flags = (bbUINT) pItem->flags;

        if (flags & uiMENUITEMFLAG_SEP)
        {
            if (uiMenuInsertSeparator(hMenu, pos) != bbEOK)
                goto uiMenu_InsertItemList_err;
            pos++;
        }

        if (flags & (uiMENUITEMFLAG_SUBMENU|uiMENUITEMFLAG_STRINGLIST))
        {
            if ((hSubMenu = uiMenuCreate(uiMenuGetOwner(hMenu))) == NULL)
                goto uiMenu_InsertItemList_err;

            if (!pItem->data)
            {
                err = bbEOK;
            }
            else
            {
                if (flags & uiMENUITEMFLAG_STRINGLIST)
                    err = uiMenuInsertStringList(hSubMenu, 0, (const bbCHAR*const*)pItem->data, pItem->cmdid, pItem->flags);
                else
                    err = uiMenuInsertItemList(hSubMenu, 0, (uiMenuItem*)pItem->data);
            }

            if ((err != bbEOK) || (uiMenuInsertSubMenu(hMenu, pos, hSubMenu, pItem->pStr) != bbEOK))
            {
                uiMenuDestroy(&hSubMenu);
                goto uiMenu_InsertItemList_err;
            }
        }
        else
        {
            if (uiMenuInsertItem(hMenu, pos, pItem->cmdid, pItem->pStr, pItem->data) != bbEOK)
                goto uiMenu_InsertItemList_err;

            if (flags & uiMENUITEMFLAG_CHECK)
                uiMenuCheckItem(hMenu, pos, uiMENUITEMFLAG_CHECK);
        }

        if (flags & uiMENUITEMFLAG_GREY) uiMenuEnableItem(hMenu, pos, 0);
        pos++;

        if (flags & uiMENUITEMFLAG_SEPN)
        {
            if (uiMenuInsertSeparator(hMenu, pos) != bbEOK)
                goto uiMenu_InsertItemList_err;
            pos++;
        }

        pItem++;

    } while (!(flags & uiMENUITEMFLAG_LAST));

    return bbEOK;

    uiMenu_InsertItemList_err:

    while ((int)(--pos) >= (int)startpos)
    {
        uiMenuDeleteItem(hMenu, pos);
    }

    return bbELAST;
}

bbERR uiMenuShowPopup(uiMENUH const hMenu, int x, int y, uiWINH hWin)
{
    if (!::TrackPopupMenu(hMenu, TPM_LEFTALIGN|TPM_TOPALIGN, x, y, 0, hWin, NULL))
        return bbErrSet(uiEMENUPOPUP);
    return bbEOK;
}

void uiMenuAutoKey::Init(bbUINT const flags)
{
    mMask[0] =
    mMask[1] =
    mMask[2] = 0;
    if (flags & uiMENUAUTOKEY_NUM)
        mMask[1] = 0x03FF0000UL;
    if (flags & uiMENUAUTOKEY_ALPHA)
        mMask[2] = 0x07FFFFFEUL;
}

bbCHAR* uiMenuAutoKey::GetEscapedString(const bbCHAR* pStr)
{
    bbUINT pos = 0;
    for(;;)
    {
        bbU32 cp = (bbU32)pStr[pos];
        if (cp == 0)
            break;

        if (cp >= 'a')
            cp &= 0xDFU;

        if ((cp < sizeof(mMask)*8) && (bbBitTst(mMask, cp)))
        {
            bbCHAR* pEscaped = (bbCHAR*) bbMemAlloc(sizeof(bbCHAR) * (bbStrLen(pStr) + 2));
            if (pEscaped)
            {
                bbMemMove(pEscaped, pStr, sizeof(bbCHAR) * pos);
                pEscaped[pos] = '&';
                bbStrCpy(pEscaped+pos+1, pStr+pos);
                bbBitClr(mMask, cp);
            }
            return pEscaped;
        }

        pos++;
    }
    return NULL;
}

