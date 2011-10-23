#ifndef uictl_H_
#define uictl_H_

#include "uidefs.h"

/** uiCtlButton window styles */
enum uiWS_BUTTON
{
    uiWS_BUTTON_DEFAULT = BS_DEFPUSHBUTTON, //!< Button is the default button in a dialog box.
    uiWS_BUTTON_FLAT = BS_FLAT,             //!< Button is drawn in 2D style.
    uiWS_BUTTON_LEFT = BS_LEFT,             //!< Left justify text.
    uiWS_BUTTON_RIGHT = BS_RIGHT,           //!< Right justify text.
    uiWS_BUTTON_CENTER = BS_CENTER,         //!< Center justify text.
    uiWS_BUTTON_TOP = BS_TOP,               //!< Top justify text.
    uiWS_BUTTON_BOTTOM = BS_BOTTOM,         //!< Bottom justify text.
};

struct uiCtlButton : uiWinBase
{
    uiCtlButton(uiWINH hWin = NULL) : uiWinBase(hWin) {}
    uiCtlButton& operator =(uiWINH const hWin) { mhWin=hWin; return *this; }

    /** Simulate a button click. */
    inline void Click()
    {
        #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
        ::SendMessage(mhWin, BM_CLICK, 0, 0L);
        #else
        #endif
    }
};

/** uiCtlEdit window styles */
enum uiWS_EDIT
{
    uiWS_EDIT_NUMBER = ES_NUMBER,       //!< Only digits can be entered
    uiWS_EDIT_PASSWORD = ES_PASSWORD,   //!< All characters displayed as *
    uiWS_EDIT_READONLY = ES_READONLY,   //!< Text is read-only
    uiWS_EDIT_RIGHT = ES_RIGHT,         //!< Text is right-aligned
    uiWS_MULTILINE = ES_MULTILINE,      //!< Text is multi-line
};

struct uiCtlEdit : uiWinBase
{
    uiCtlEdit(uiWINH hWin = NULL) : uiWinBase(hWin) {}
    uiCtlEdit& operator =(uiWINH const hWin) { mhWin=hWin; return *this; }
};

/** uiCtlRadio window styles */
enum uiWS_RADIO
{
};

/** uiCtlRadio radio button states */
enum uiCTLRADIOSTATE
{
    uiCTLRADIOSTATECHECKMASK  = 1,             //!< Bitmask to mask checkstate
    uiCTLRADIOSTATE_UNCHECKED = BST_UNCHECKED, //!< Radio button is not checked
    uiCTLRADIOSTATE_CHECKED   = BST_CHECKED,   //!< Radio button is checked
    uiCTLRADIOSTATE_FOCUS     = BST_FOCUS      //!< Radio button has focus
};

/** uiCtlRadio: Get state.
    @param hWin  Window handle of control
    @return Bitmask with checkstate
*/
inline uiCTLRADIOSTATE uiCtlRadioGetState(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    bbASSERT(::IsWindow(hWin));
    return (uiCTLRADIOSTATE)::SendMessage(hWin, BM_GETSTATE, 0, 0L);
    #else
    #endif
}

/** uiCtlRadio: Set radio button control state.
    @param hWin  Window handle of control
    @param state uiCTLRADIOSTATE_UNCHECKED or uiCTLRADIOSTATE_CHECKED
*/
inline void uiCtlRadioSetState(uiWINH const hWin, uiCTLRADIOSTATE const state)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    bbASSERT(::IsWindow(hWin));
    ::SendMessage(hWin, BM_SETCHECK, (WPARAM)state, 0L);
    #else
    #endif
}
    
struct uiCtlRadio : uiCtlButton
{
    /** Get state.
        @return Bitmask with checkstate
    */
    inline uiCTLRADIOSTATE GetState() const
    {
        return uiCtlRadioGetState(mhWin);
    }

    /** Set state.
        @param state uiCTLRADIOSTATE_UNCHECKED or uiCTLRADIOSTATE_CHECKED
    */
    inline void SetState(uiCTLRADIOSTATE const state)
    {
        uiCtlRadioSetState(mhWin, state);
    }
};

/** uiCtlRadio radio button states */
enum uiCTLCHECKSTATE
{
    uiCTLCHECKSTATE_UNCHECKED = BST_UNCHECKED, //!< is not checked
    uiCTLCHECKSTATE_CHECKED   = BST_CHECKED,   //!< is checked
    uiCTLCHECKSTATE_FOCUS     = BST_FOCUS      //!< has focus
};

/** uiCtlCheck: Get state.
    @param hWin  Window handle of control
    @return Bitmask with checkstate
*/
inline bbUINT uiCtlCheckGetState(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    bbASSERT(::IsWindow(hWin));
    return (bbUINT)::SendMessage(hWin, BM_GETSTATE, 0, 0L);
    #else
    #endif
}

/** uiCtlCheck: Set state.
    @param hWin  Window handle of control
    @param state uiCTLCHECKSTATE_UNCHECKED or uiCTLCHECKSTATE_CHECKED
*/
inline void uiCtlCheckSetState(uiWINH const hWin, uiCTLCHECKSTATE const state)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    bbASSERT(::IsWindow(hWin));
    ::SendMessage(hWin, BM_SETCHECK, (WPARAM)state, 0L);
    #else
    #endif
}

struct uiCtlCheck : uiCtlButton
{
    uiCtlCheck(uiWINH hWin = NULL) : uiCtlButton(hWin) {}
    uiCtlCheck& operator =(uiWINH const hWin) { mhWin=hWin; return *this; }

    /** uiCtlCheck: Get state.
        @return Bitmask with checkstate
    */
    inline bbUINT GetState() const
    {
        return uiCtlCheckGetState(mhWin);
    }

    /** Set state.
        @param state uiCTLCHECKSTATE_UNCHECKED or uiCTLCHECKSTATE_CHECKED
    */
    inline void SetState(uiCTLCHECKSTATE const state)
    {
        uiCtlCheckSetState(mhWin, state);
    }
};

enum uiWS_STATIC
{
    uiWS_STATIC_RIGHT = SS_RIGHT,           //!< Right aligned text
    uiWS_STATIC_CENTER = SS_CENTER,         //!< Centered text
    uiWS_STATIC_VCENTER = SS_CENTERIMAGE    //!< Vertically center text
};

struct uiCtlStatic : uiWinBase
{
    uiCtlStatic(uiWINH hWin = NULL) : uiWinBase(hWin) {}
    uiCtlStatic& operator =(uiWINH const hWin) { mhWin=hWin; return *this; }

};

/** uiCtlList window styles */
enum uiWS_LIST
{
    uiWS_LIST_MODEREPORT     = LVS_REPORT,          //!< Report view
    uiWS_LIST_MODEICON       = LVS_ICON,            //!< Icon view
    uiWS_LIST_MODELIST       = LVS_LIST,            //!< List view
    uiWS_LIST_MODESMALLICON  = LVS_SMALLICON,       //!< Small icon view
    uiWS_LIST_SINGLESEL      = LVS_SINGLESEL,       //!< Only one item can be selected
    uiWS_LIST_EDITLABELS     = LVS_EDITLABELS,      //!< Item labels can be edited
    uiWS_LIST_NOWRAP         = LVS_NOLABELWRAP,     //!< Disable label wrap
    uiWS_LIST_SORTASCENDING  = LVS_SORTASCENDING,   //!< Sort ascending
    uiWS_LIST_SORTDESCENDING = LVS_SORTDESCENDING,  //!< Sort descending
};

/** uiCtlList control item states */
enum uiCTLLIST_ITEMSTATE
{
    uiCTLLIST_ITEMSTATE_FOCUSED = LVIS_FOCUSED,
    uiCTLLIST_ITEMSTATE_SELECTED = LVIS_SELECTED,
};

/** Parameter struct for uiWM_CTLLIST_TEXTCB message. */
struct uiCtlListTextCB
{
    void*    mUser;      //!< User data associated with item
    bbUINT   mItem;      //!< Item index, 0-based
    bbU16    mSubItem;   //!< Sub item index
    bbU16    mStrLen;    //!< Length of stringbuffer *mppStr
    bbCHAR** mppStr;     /**< Pointer to pointer stringbuffer to return item name.
                              On entry this points to a buffer of \a mStrLen code units, the application
                              can copy the string to that buffer. Alternatively the application can set
                              *mppStr to an address with a 0-terminated string, which must be preserved
                              over the lifetime of the list item.
                         */
};

/** Parameter struct for uiWM_CTLLIST_SELCHANGED message. */
struct uiCtlListSelChanged
{
    void*    mUser;     //!< User data associated with item
    bbUINT   mItem;     //!< Item index, 0-based
    bbU16    mSubItem;  //!< Sub item index
    bbU8     mNewState; //!< Bitmask of new item state, see uiCTLLIST_ITEMSTATE
};

/** uiCtlList: Insert column.
    @param hWin  Control handle
    @param col   0-based column index
    @param pName 0-terminated column name
    @param width Width of column in pixels
    @return 0-based column index of created column, or -1 on error
*/
int uiCtlListInsertColumn(uiWINH const hWin, int const col, const bbCHAR* const pName, bbUINT const width);

/** uiCtlList: Insert item.
    @param hWin  Control handle
    @param pos   0-based position
    @param pName 0-terminated column name
    @param user  Application defined value to be associated with the item
    @return 0-based inserted item, or -1 on error
*/
int uiCtlListInsertItem(uiWINH const hWin, int const pos, const bbCHAR* const pName, const void* const user);

/** uiCtlList: Set sub item.
    @param hWin   Control handle
    @param pos    0-based position of item
    @param subpos 1-based position of sub item
    @param pName  0-terminated string to set, or (bbCHAR*)-1 to enable string callback for this subitem.
                  If string callback is enabled, the controls parent window will receive uiWM_CTLLIST_TEXTCB
                  messages to provide the text for the item.
*/
bbERR uiCtlListSetSubItem(uiWINH const hWin, int const pos, int const subpos, const bbCHAR* const pName);

/** uiCtlList: Get item data.
    @param hWin   Control handle
    @param pos    0-based position of item
    @param pUser  Returns item's user data on success.
*/
bbERR uiCtlListGetItemData(uiWINH const hWin, int const pos, const void** const pUser);

bbCHAR* uiCtlListGetItemStringMem(uiWINH const hWin, bbUINT pos, bbUINT subpos, bbUINT* const pLen);

/** uiCtlList: Delete all items.
    @param hWin  Control handle, must be valid
    @param pos   0-based item index, must be valid
    @return Bitmask with item state, see uiCTLLIST_ITEMSTATE
*/
inline bbUINT uiCtlListGetItemState(uiWINH const hWin, int const pos)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return ListView_GetItemState(hWin, pos, LVIS_SELECTED|LVIS_FOCUSED);
    #else
    #endif
}

/** uiCtlList: Delete all items.
    @param hWin  Control handle
*/
inline void uiCtlListClear(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ListView_DeleteAllItems(hWin);
    #else
    #endif
}

/** uiCtlList: Get item count.
    @param hWin  Control handle
    @return Item count
*/
inline int uiCtlListGetItemCount(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return ListView_GetItemCount(hWin);
    #else
    #endif
}


/** uiCtlList: Set selected item.
    For multiple selection list views, this is the item where the selection starts.
    @param hWin  Control handle
    @param pos   0-based item index
*/
void uiCtlListSetSel(uiWINH const hWin, int const pos);

/** uiCtlList: Get first selected item.
    For multiple selection list views, this is the item where the selection starts.
    @param hWin  Control handle
    @param return 0-based item index, or -1 if no selection
*/
inline int uiCtlListGetSel(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return ListView_GetSelectionMark(hWin);
    #else
    #endif
}

/** uiCtlList: Get next selected item.
    Enumaration of selected items.
    @param hWin  Control handle
    @param last  0-based item index of last call, start with -1
    @return 0-based item index, or -1 on end of enumeration
*/
int uiCtlListNextSel(uiWINH const hWin, int last);

/** uiCtlList: Get number of selected items.
    @param hWin  Control handle, must be valid
    @param return Number of selected items
*/
inline bbUINT uiCtlListGetSelCount(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return ListView_GetSelectedCount(hWin);
    #else
    #endif
}

/** List view control */
struct uiCtlList : uiWinBase
{
    uiCtlList(uiWINH hWin = NULL) : uiWinBase(hWin) {}
    uiCtlList& operator =(uiWINH hWin) { mhWin=hWin; return *this; }

    inline int InsertColumn(int const col, const bbCHAR* const pName, bbUINT const width)
    {
        return uiCtlListInsertColumn(mhWin, col, pName, width);
    }

    inline int InsertItem(int const pos, const bbCHAR* const pName, const void* const user)
    {
        return uiCtlListInsertItem(mhWin, pos, pName, user);
    }

    inline bbERR SetSubItem(int const pos, int const subpos, const bbCHAR* const pName)
    {
        return uiCtlListSetSubItem(mhWin, pos, subpos, pName);
    }

    inline bbERR GetItemData(int const pos, const void** const pUser)
    {
        return uiCtlListGetItemData(mhWin, pos, pUser);
    }

    inline void Clear()
    {
        uiCtlListClear(mhWin);
    }

    inline void SetSel(int const pos)
    {
        uiCtlListSetSel(mhWin, pos);
    }

    inline int GetSel()
    {
        return uiCtlListGetSel(mhWin);
    }

};

/** uiCtlSpin: Set range.
    @param hWin  Control handle
    @param min   Minimum value
    @param max   Maximum value
*/
inline void uiCtlSpinSetRange(uiWINH const hWin, bbS32 const min, bbS32 const max)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::SendMessage(hWin, UDM_SETRANGE32, (WPARAM)min, (LPARAM)max);
    #else
    #endif
}

/** uiCtlSpin: Get range.
    @param hWin Control handle
    @param pMin Returns minimum value, can be NULL
    @return Maximum value. Check bbErrGet() for errors.
*/
inline bbS32 uiCtlSpinGetRange(uiWINH const /*hWin*/, bbS32* pMin)
{
    bbS32 max;
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    bbU32 err = (bbU32)::SendMessage(0, UDM_GETRANGE32, (WPARAM)pMin, (LPARAM)&max);
    if (err)
        err = bbESYS;
    bbErrSet(err);
#else
#endif
    return max;
}

/** uiCtlSpin: Set new position.
    @param hWin  Control handle
    @param pos   Value
    @return Previous value
*/
inline bbS32 uiCtlSpinSetPos(uiWINH const hWin, bbS32 const pos)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return ::SendMessage(hWin, UDM_SETPOS32, (WPARAM)0, (LPARAM)pos);
    #else
    #endif
}

/** Spin button control. */
struct uiCtlSpin : uiWinBase
{
    uiCtlSpin(uiWINH hWin = NULL) : uiWinBase(hWin) {}
    uiCtlSpin& operator =(uiWINH const hWin) { mhWin=hWin; return *this; }

    inline void SetRange(bbS32 const min, bbS32 const max)
    {
        uiCtlSpinSetRange(mhWin, min, max);
    }

    inline bbS32 SetPos(bbS32 const pos)
    {
        return uiCtlSpinSetPos(mhWin, pos);
    }
};

/** uiCtlListBox window styles */
enum uiWS_LISTBOX
{
    uiWS_LISTBOX_NOSCROLLHIDE = LBS_DISABLENOSCROLL, //!< Do not hide vertical scrollbar
    uiWS_LISTBOX_MULTISEL     = LBS_EXTENDEDSEL,     //!< Allow multiple selections
    uiWS_LISTBOX_SORT         = LBS_SORT,            //!< Sort items alphabetically
};

/** Insert string into list box control.
    @param hWin Control handle
    @param pos  0-based position
    @param pStr 0-terminate string to add, will be copied
    @param user Application defined value associated with item,
                note that (void*)-1 is used to indicate error on uiCtlListBoxGetItemData().
    @return 0-based insert position, or negative value on error
*/
int uiCtlListBoxInsertItem(uiWINH const hWin, int const pos, const bbCHAR* const pStr, const void* const user);

/** uiCtlListBox: set user data.
    @param hWin  Control handle
*/
inline void uiCtlListBoxSetItemData(uiWINH const hWin, int const pos, void* const user)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    bbErrSet((bbU32)::SendMessage(hWin, LB_SETITEMDATA, pos, (LPARAM)user)>>31);
    #else
    #endif
}

/** uiCtlListBox: Get item associated user data.
    @param hWin  Control handle
    @param pos  0-based position
    @return User data, or (void*)-1 on error
*/
inline void* uiCtlListBoxGetItemData(uiWINH const hWin, int const pos)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    void* const user = (void*)::SendMessage(hWin, LB_GETITEMDATA, pos, 0);
    if (user == (void*)LB_ERR)
        bbErrSet(bbESYS);
    return user;
    #else
    #endif
}

/** uiCtlListBox: Delete item.
    @param hWin  Control handle
    @param pos   0-based item index
*/
inline void uiCtlListBoxDeleteItem(uiWINH const hWin, int const pos)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::SendMessage(hWin, LB_DELETESTRING, pos, 0);
    #else
    #endif
}

/** uiCtlListBox: Set current selection.
    @param hWin  Control handle
    @param pos   0-based item index, or -1 to clear selection
*/
inline void uiCtlListBoxSetSel(uiWINH const hWin, int const pos)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::SendMessage(hWin, LB_SETCURSEL, (WPARAM)pos, 0L);
    #else
    #endif
}

/** uiCtlListBox: Get current selection.
    @param hWin  Control handle
    @return 0-based item index, or -1 on error
*/
inline int uiCtlListBoxGetSel(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    int const ret = ::SendMessage(hWin, LB_GETCURSEL, 0, 0L);
    if (ret == LB_ERR)
        bbErrSet(bbESYS);
    return ret;
    #else
    #endif
}

/** uiCtlListBox: Clear contents.
    @param hWin  Control handle
*/
inline void uiCtlListBoxClear(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::SendMessage(hWin, LB_RESETCONTENT, 0, 0L);
    #else
    #endif
}

/** uiCtlListBox: Get item count.
    @param hWin  Control handle
    @return Number of items, or -1 on error
*/
inline int uiCtlListBoxGetItemCount(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    int const ct = ::SendMessage(hWin, LB_GETCOUNT, 0, 0L);
    if (ct == LB_ERR) bbErrSet(bbESYS);
    return ct;
    #else
    #endif
}

/** Simple list box control. */
struct uiCtlListBox : uiWinBase
{
    uiCtlListBox(uiWINH hWin = NULL) : uiWinBase(hWin) {}
    uiCtlListBox& operator =(uiWINH const hWin) { mhWin=hWin; return *this; }

    inline int InsertItem(int const pos, const bbCHAR* const pStr, void* const user)
    {
        return uiCtlListBoxInsertItem(mhWin, pos, pStr, user);
    }

    inline void* GetItemData(int const pos)
    {
        return uiCtlListBoxGetItemData(mhWin, pos);
    }

    inline void SetSel(int const pos)
    {
        uiCtlListBoxSetSel(mhWin, pos);
    }

    inline int GetSel()
    {
        return uiCtlListBoxGetSel(mhWin);
    }

    inline void Clear()
    {
        uiCtlListBoxClear(mhWin);
    }

    inline int GetItemCount()
    {
        return uiCtlListBoxGetItemCount(mhWin);
    }
};

/** uiCtlCombo window styles */
enum uiWS_COMBO
{
    uiWS_COMBO_NOEDIT       = CBS_DROPDOWNLIST,     //!< Selected item cannot be edited
    uiWS_COMBO_NOSCROLLHIDE = CBS_DISABLENOSCROLL,  //!< Do not hide vertical scrollbar
    uiWS_COMNO_SORT         = CBS_SORT,             //!< Sort items alphabetically
};

/** uiCtlCombo: Insert item.
    @param hWin Control handle
    @param pos  0-based position to insert it, -1 for end of list
    @param pStr 0-terminate string to add, will be copied
    @param user Application defined value associated with item, cannot be (void*) because
                this is reserved to indicate error.
    @return 0-based insert position, or negative value on error
*/
int uiCtlComboInsertItem(uiWINH const hWin, const int pos, const bbCHAR* const pStr, const void* const user);

/** uiCtlCombo: Delete item.
    @param hWin  Control handle
    @param pos   0-based item index
*/
inline void uiCtlComboDeleteItem(uiWINH const hWin, const int pos)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::SendMessage(hWin, CB_DELETESTRING, pos, 0);
    #else
    #endif
}

/** uiCtlCombo: Set current selection.
    @param hWin  Control handle
    @param pos   0-based item index, or -1 to clear selection
*/
inline void uiCtlComboSetSel(uiWINH const hWin, int const pos)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::SendMessage(hWin, CB_SETCURSEL, (WPARAM)pos, 0L);
    #else
    #endif
}

/** uiCtlCombo: Get current selection.
    @param hWin  Control handle
    @return      0-based item index, or -1 if no selection
*/
inline int uiCtlComboGetSel(uiWINH const hWin)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    int const ret = ::SendMessage(hWin, CB_GETCURSEL, 0, 0L);
    if (ret == CB_ERR)
        bbErrSet(bbESYS);
    return ret;
    #else
    #endif
}

/** Combo box control. */
struct uiCtlCombo : uiWinBase
{
    uiCtlCombo(uiWINH hWin = NULL) : uiWinBase(hWin) {}
    uiCtlCombo& operator =(uiWINH const hWin) { mhWin=hWin; return *this; }

    inline int InsertItem(const int pos, const bbCHAR* const pStr, const void* const user)
    {
        return uiCtlComboInsertItem(mhWin, pos, pStr, user);
    }

    inline void DeleteItem(const int pos)
    {
        uiCtlComboDeleteItem(mhWin, pos);
    }

    inline void SetSel(int const pos)
    {
        uiCtlComboSetSel(mhWin, pos);
    }

    inline int GetSel()
    {
        return uiCtlComboGetSel(mhWin);
    }
};

#endif /* uictl_H_ */

