#ifndef uiMenu_H_
#define uiMenu_H_

#include "uidefs.h"

/** @addtogroup uiMenu Menus

@{*/

/** Menu handle. */
#if bbQT
#include <QMenu>
#include <QAction>
typedef QMenu* uiMENUH;
#elif (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
typedef HMENU uiMENUH;
#else
typedef void* uiMENUH;
#endif

enum uiMENUITEMFLAG
{
    uiMENUITEMFLAG_ENABLED    = 0x0,              //!< uiMenuItem::flags bit: Item is enabled (default)
    uiMENUITEMFLAG_UNCHECK    = 0x0,              //!< uiMenuItem::flags bit: Item is unchecked (default)
    uiMENUITEMFLAG_GREY       = 0x1/*MF_GRAYED*/, //!< uiMenuItem::flags bit: Item is greyed
    uiMENUITEMFLAG_ICON       = 0x2,              //!< uiMenuItem::flags bit: Display icon
    uiMENUITEMFLAG_ALLOCATED  = 0x4,              //!< uiMenuItem::flags bit: pStr points to a heap block, internal use
    uiMENUITEMFLAG_CHECK      = 0x8/*MF_CHECKED*/,//!< uiMenuItem::flags bit: Item is checked
    uiMENUITEMFLAG_STRINGLIST = 0x10,             //!< uiMenuItem::flags bit: uiMenuItem::data points to a string list (bbCHAR**) forming submenu, uiMenuItem::cmdid contains size of stringlist
    uiMENUITEMFLAG_SUBMENU    = 0x20,             //!< uiMenuItem::flags bit: uiMenuItem::data points to uiMenuItem[] forming submenu, can be NULL for empty submenu
    uiMENUITEMFLAG_SEP        = 0x40,             //!< uiMenuItem::flags bit: Item is preceeded by separator line
    uiMENUITEMFLAG_SEPN       = 0x80,             //!< uiMenuItem::flags bit: Item is followed by separator line
    uiMENUITEMFLAG_LAST       = 0x100             //!< uiMenuItem::flags bit: Item is last in array
};

#ifdef _MSC_VER
#pragma pack(push,1)
#endif

typedef struct
{
    const bbCHAR* pStr;   //!< Pointer to menu string

    bbU16         flags;  /**< Flag bitmask, see uiMENUITEMFLAG.
                               If uiMENUITEMFLAG_STRINGLIST is set, this bitmask can also
                               contain uiMENUAUTOKEY flags. uiMENUITEMFLAG_ALLOCATED and uiMENUITEMFLAG_CHECK
                               must not be used in this case.
                          */
    bbU16         group;  //!< Radio group ID, local to containing menu
    bbU32         cmdid;  //!< Command ID, or size of stringlist for uiMENUITEMFLAG_STRINGLIST

    const void*   data;   /**< Meaning dependent on flags:
                               <table>
                               <tr><td>uiMENUITEMFLAG_SUBMENU</td><td>uiMenuItem* Pointer to submenu, can be NULL to create empty submenu</td></tr>
                               <tr><td>uiMENUITEMFLAG_STRINGLIST</td><td>const bbCHAR** Stringlist forming submenu</td></tr>
                               <tr><td>otherwise             </td><td>user data</td></tr>
                               </table>
                          */

} __attribute__((packed)) uiMenuItem;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

/** Flags for uiMenuAutoKey::Init. */
enum uiMENUAUTOKEY
{
    uiMENUAUTOKEY_ALPHA = uiMENUITEMFLAG_ALLOCATED, //!< Allow alphabetic characters
    uiMENUAUTOKEY_NUM   = uiMENUITEMFLAG_CHECK,     //!< Allow numeric characters
};

/** Helper to auto-assign activation letters to menu strings. */
struct uiMenuAutoKey
{
    bbU32 mMask[3];   //!< Bitmask for allowed key usage

    /** Init auto key enumeration.
        @param flags Flag bitmask, see uiMENUAUTOKEY
    */
    void Init(bbUINT const flags);

    /** Find next free letter for string, and return '&' escaped string in heap block.
        @param pStr String to scan.
        @return 0-terminated, &-escaped string, must be freed externally.
                NULL on error or if no letter could be assigned.
    */
    bbCHAR* GetEscapedString(const bbCHAR* pStr);
};

/** Create menu.
    @param hWin Owning window, commands will be sent to the message handler of this window, can be NULL.
    @return Handle to created menu, will also be saved in the object, NULL on failure
*/
uiMENUH uiMenuCreate(uiWINH const hWin = NULL);

/** Create menu as menu bar.
    @param hWin Owning window, commands will be sent to the message handler of this window, can be NULL.
    @return Handle to created menu, will also be saved in the object, NULL on failure
*/
uiMENUH uiMenuCreateBar(uiWINH const hWin = NULL);

/** Create menu from item list definition.
    @param pList Menu item definitions to populate menu with.
    @return Handle to created menu, will also be saved in the object, NULL on failure
*/
uiMENUH uiMenuCreate(const uiMenuItem* const pList);

/** Destroy menu.

    This call is recursive, it will destroy the menu and all its submenus.
    
    If a menu handle is attached to a window, it must not be destroyed. If that window is destroyed,
    it will also destroy the attached menu automatically.

    @param phMenu Pointer to menu handle, will be reset to NULL
*/
void uiMenuDestroy(uiMENUH* const phMenu);

/** Get owning window for this menu.
    This function returns the parent window handle that was passed when the menu was created.
    @param hMenu Menu handle
    @return Window handle, NULL on error, or menu was created without parent window.
*/
uiWINH uiMenuGetOwner(uiMENUH const hMenu);

/** Set new owning window for this menu.
    @param hMenu Menu handle
    @param hWin Owning window
    @return Window handle, NULL on error, or menu was created without parent window.
*/
void uiMenuSetOwner(uiMENUH const hMenu, uiWINH const hWin);

/** Delete menu item, if the menu item is a submenu, it will be destroyed.
    @param hMenu Menu handle
    @param pos   0-based menu item index
*/
inline void uiMenuDeleteItem(uiMENUH const hMenu, bbUINT pos)
{
    #if bbQT
    QAction* pAct = hMenu->actions().value(pos, NULL);
    if (pAct)
        hMenu->removeAction(pAct);
    delete pAct;
    #elif (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::DeleteMenu(hMenu, pos, MF_BYPOSITION);
    #else
    #endif
}

/** Detach menu item, if the menu item is a submenu, it will not be destroyed.
    @param hMenu Menu handle
    @param pos   0-based menu item index
*/
inline void uiMenuDetachItem(uiMENUH const hMenu, bbUINT pos)
{
    #if bbQT
    QAction* pAct = hMenu->actions().value(pos, NULL);
    if (pAct)
        hMenu->removeAction(pAct);
    #elif (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::RemoveMenu(hMenu, pos, MF_BYPOSITION);
    #else
    #endif
}

/** Insert or append a menu item.
    @param hMenu Menu handle
    @param pos   Position to insert after, if (bbUINT)-1 the item will be appended to the menu
    @param id    Menu item ID
    @param pStr  0-terminated menu item name
    @param user  User data
*/
bbERR uiMenuInsertItem(uiMENUH const hMenu, bbUINT pos, bbUINT id, const bbCHAR* pStr, const void* const user);

/** Update a menu item.
    @param hMenu Menu handle
    @param pos   Position to insert after, if (bbUINT)-1 the item will be appended to the menu
    @param id    Menu item ID, (bbUINT)-1 to do not update ID
    @param pStr  0-terminated menu item name, NULL to do not update name
    @param user  User data (void*)-1 to do not update
*/
bbERR uiMenuUpdateItem(uiMENUH const hMenu, bbUINT pos, bbUINT id, const bbCHAR* pStr, const void* const user);

/** Insert or append a submenu.
    @param hMenu Menu handle
    @param pos   Position to insert after, if (bbUINT)-1 the item will be appended to the menu
    @param id    Menu item ID
    @param pStr  0-terminated menu item name
*/
bbERR uiMenuInsertSubMenu(uiMENUH const hMenu, bbUINT pos, uiMENUH hSubMenu, const bbCHAR* pStr);

/** Insert or append a menu separator.
    @param hMenu Menu handle
    @param pos   Position to insert after, if (bbUINT)-1 the item will be appended to the menu
*/
bbERR uiMenuInsertSeparator(uiMENUH const hMenu, bbUINT pos);

/** Insert or append a list of menu items.
    @param hMenu Menu handle
    @param pos   Position to insert after, if (bbUINT)-1 the item will be appended to the menu
    @param pList Pointer to array of items to insert, terminated with uiMENUITEMFLAG_LAST
*/
bbERR uiMenuInsertItemList(uiMENUH const hMenu, bbUINT pos, const uiMenuItem* pList);

/** Insert or append a list of strings as menu items.
    The string list can be optionally escaped with menu shortcuts.
    The user data for inserted menu items will be set to the string's index in pStringList[].
    @param hMenu Menu handle
    @param pos   Position to insert after, if (bbUINT)-1 the item will be appended to the menu
    @param pStringList Pointer to array of string pointers
    @param count Size of array pStringList[]
    @param opt   Low 16 bit: Options, combined bitmask from uiMENUINSERTOPT and uiMENUAUTOKEY flags <br>
                 High 16 bit: Command ID to assign with all menu items
*/
bbERR uiMenuInsertStringList(uiMENUH const hMenu, bbUINT startpos, const bbCHAR*const* const pStringList, bbUINT count, bbU32 opt);

enum uiMENUITEMHISTORY
{
    uiMENUITEMHISTORY_AUTOKEY     = 0x1,
    uiMENUITEMHISTORY_ALLOWDOUBLE = 0x2,
    uiMENUITEMHISTORY_CMPUSERDATA = 0x4,
    uiMENUITEMHISTORY_CMPID = 0x8
};

/** Insert item into menu item history (FIFO).
    This function is intended to manage recent file or window histories in a menu.
    It treats a range of menu items as a history FIFO: A new item will be inserted
    at the start of the list. An item of the same name will be deleted, if uiMENUITEMHISTORY_ALLOWDOUBLE is not set.
    If the item range exceeds \a maxcount, the last item in the list will be deleted.
    @param hMenu    Menu handle
    @param start    Start position of item history
    @param maxcount Maximum size of history
    @param pStr     New item to insert at start of list, pointer to 0-terminated string.
    @param opt      Low 16 bit flag bitmask (see uiMENUITEMHISTORY), top 16 bit menu item ID
    @param user     User data for item
*/
bbERR uiMenuItemHistoryInsert(uiMENUH const hMenu, bbUINT const start, bbUINT const maxcount, const bbCHAR* pStr, bbU32 const opt, void* user);

/** Get number of items in menu.
    @param hMenu Menu handle
    return Number of item, or -1 on failure
*/
inline int uiMenuGetItemCount(uiMENUH const hMenu)
{
    #if bbQT
    return hMenu->actions().size();
    #elif (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    int const count = GetMenuItemCount(hMenu);
    if (count==-1) bbErrSet(bbESYS);
    return count;
    #else
    #endif
}

/** Find menu item position by name.
    @param hMenu Menu handle
    @param pName Menu item name, 0-terminated string
    @return 0-based menu item position, or -1 if not found, or -2 on error.
            On error check bbErrGet() for failure reason.
*/
int uiMenuFindItem(uiMENUH const hMenu, const bbCHAR* const pName);

/** Find menu item position by name with offset.
    @param hMenu Menu handle
    @param pName Menu item name, 0-terminated string
    @param stroffset Offset to add to menuitem name before string compare against pName
    @return 0-based menu item position, or -1 if not found, or -2 on error.
            On error check bbErrGet() for failure reason.
*/
int uiMenuFindItemOffs(uiMENUH const hMenu, const bbCHAR* const pName, bbUINT const stroffset);

/** Find menu item position by command ID.
    @param hMenu Menu handle
    @param user User data to search for
    @param cmdID Command ID to search for, or -1 for any
    @return 0-based menu item position, or -1 if not found, or -2 on error.
            On error check bbErrGet() for failure reason.
*/
int uiMenuFindItemData(uiMENUH const hMenu, bbUINT const start, bbUINT const max, int cmdID, void* user);

/** Get menu item user data by position.
    @param hMenu Menu handle
    @param pos   0-based menu item position
    @return Returns void* with user data, or NULL on error. 
            Test bbGetERR() to distinguish between NULL data and error.
*/
void* uiMenuGetItemData(uiMENUH const hMenu, bbUINT pos);

/** Get menu item ID by position.
    @param hMenu Menu handle
    @param pos   0-based menu item position
    @return ID or (bbUINT)-1 on error
*/
inline bbUINT uiMenuGetItemID(uiMENUH const hMenu, bbUINT pos)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return ::GetMenuItemID(hMenu, pos);
    #else
    #endif
}
    
/** Get menu item string by position.
    @param hMenu  Menu handle
    @param pos    0-based menu item position
    @param pStr   Buffer to return string, or NULL to read string length. Buffer must
                  be large enough to hold string including 0-terminator.
    @param buflen Length of buffer pStr in character code units, must be 0 to read string length
    @return String length in character code units without 0-terminator, or 0 on failure
*/
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
inline int uiMenuGetItemString(uiMENUH const hMenu, bbUINT pos, bbCHAR* pStr, bbUINT const buflen)
{
    return ::GetMenuString(hMenu, pos, pStr, buflen, MF_BYPOSITION);
}
#else
int uiMenuGetItemString(uiMENUH const hMenu, bbUINT pos, bbCHAR* pStr, bbUINT const buflen);
#endif

/** Get menu item string by position in heap block.
    @param hMenu  Menu handle
    @param pos    0-based menu item position
    @param pLen   Returns length of string in code units, without 0-terminator, can be NULL.
                  Unchanged on error.
    @return Heap block containing 0-terminated string, must be freed externally. NULL on error.
*/
bbCHAR* uiMenuGetItemStringMem(uiMENUH const hMenu, bbUINT pos, bbUINT* const pLen);

/** Get menu item state.
    @param hMenu  Menu handle
    @param pos    0-based menu item position
    @return Bitmask with item state, check for the following bits: uiMENUITEMFLAG_GREY, uiMENUITEMFLAG_CHECK
*/
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
inline bbUINT uiMenuGetItemState(uiMENUH const hMenu, bbUINT pos)
{
    return ::GetMenuState(hMenu, pos, MF_BYPOSITION);
}
#else
bbUINT uiMenuGetItemState(uiMENUH const hMenu, bbUINT pos);
#endif

/** Get menu item submenu handle by position.
    @param hMenu Menu handle
    @param pos   0-based menu item position
    @return Handle to submenu or NULL on error
*/
inline uiMENUH uiMenuGetSubMenu(uiMENUH const hMenu, bbUINT pos)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    return ::GetSubMenu(hMenu, pos);
    #else
    #endif
}

/** Enable or grey a menu item.
    @param hMenu Menu handle
    @param pos   Position to change
    @param state !=0 Enable item, 0 grey item
*/
inline void uiMenuEnableItem(uiMENUH const hMenu, bbUINT pos, int const state)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::EnableMenuItem(hMenu, pos, state ? uiMENUITEMFLAG_ENABLED|MF_BYPOSITION : uiMENUITEMFLAG_GREY|MF_BYPOSITION);
    #else
    #endif
}

/** Check or uncheck a menu item.
    @param hMenu Menu handle
    @param pos   Position to change
    @param state !=0 to check, 0 to uncheck
*/
inline void uiMenuCheckItem(uiMENUH const hMenu, bbUINT pos, bbUINT state)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    if (state) state = uiMENUITEMFLAG_CHECK;
    ::CheckMenuItem(hMenu, pos, MF_BYPOSITION | state);
    #else
    #endif
}

/** Check one menu item and uncheck all other items from a group.
    @param hMenu Menu handle
    @param first Index of first menu item of the group
    @param last  Index of first menu item of the group, inclusive
    @param check Index of item to check
*/
inline void uiMenuCheckRadioItem(uiMENUH const hMenu, bbUINT first, bbUINT last, bbUINT check)
{
    #if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    ::CheckMenuRadioItem(hMenu, first, last, check, MF_BYPOSITION);
    #else
    #endif
}

/** Show this menu as popup anywhere on the screen.
    @param hMenu Menu handle
    @param x X position on screen, top-left corner
    @param y Y position on screen, top-left corner
    @param hWin Window handle of window to receive menu selection notifications
*/
bbERR uiMenuShowPopup(uiMENUH const hMenu, int x, int y, uiWINH hWin);


struct uiMenu
{
    uiMENUH mhMenu; //!< Menu handle

    //
    // Initialization and destruction
    //

    /** Construct menu object.
        @param hMenu Handle of excisting menu to attach to object, or 
                     NULL to construct an empty object
    */
    uiMenu(uiMENUH hMenu = NULL)
    {
        mhMenu = hMenu;
    }

    /** @param hWin Window handle, can be NULL */
    uiMenu& operator=(uiMENUH hMenu)
    {
        mhMenu = hMenu;
        return *this;
    }

    operator uiMENUH() const
    {
        return mhMenu;
    }

    /** Attach menu handle to object.
        The menu object must have been unitialized.
        @param Menu handle, can be NULL
    */
    inline void Attach(uiMENUH hMenu)
    {
        bbASSERT(!mhMenu);
        mhMenu = hMenu;
    }

    /** Detach menu handle.
        @return Detached menu handle
    */
    inline uiMENUH Detach()
    {
        uiMENUH const hMenu = mhMenu;
        mhMenu = NULL;
        return hMenu;
    }
    
    /** Create menu.
        @param hWin Owning window, commands will be sent to the message handler of this window, can be NULL.
        @return Handle to created menu, will also be saved in the object, NULL on failure
    */
    uiMENUH Create(uiWINH const hWin = NULL)
    {
        bbASSERT(!mhMenu);
        return mhMenu = uiMenuCreate(hWin);
    }

    /** Create menu as menu bar.
        @param hWin Owning window, commands will be sent to the message handler of this window, can be NULL.
        @return Handle to created menu, will also be saved in the object, NULL on failure
    */
    inline uiMENUH CreateBar(uiWINH const hWin = NULL)
    {
        bbASSERT(!mhMenu);
        return mhMenu = uiMenuCreateBar(hWin);
    }

    /** Create menu from item list definition.
        @param pList Menu item definitions to populate menu with.
        @return Handle to created menu, will also be saved in the object, NULL on failure
    */
    inline uiMENUH Create(const uiMenuItem* const pList)
    {
        bbASSERT(!mhMenu);
        return mhMenu = uiMenuCreate(pList);
    }

    /** Destroy menu attached to object. 
        The object's menu handle will be reset to NULL. Save to call on object with NULL handle.
        This call is recursive, it will destroy the menu and all its submenus.
        
        If a menu handle is attached to a window, it must not be destroyed. If that window is destroyed,
        it will also destroy the attached menu automatically. If there was still an uiMenu class
        attached to the same handle, it must not be used anymore.
    */
    inline void Destroy()
    {
        uiMenuDestroy(&mhMenu);
    }

    /** Get owning window for this menu.
        This function returns the parent window handle that was passed when the menu was created.
        @return Window handle, NULL on error, or menu was created without parent window.
    */
    inline uiWINH GetOwner()
    {
        return uiMenuGetOwner(mhMenu);
    }

    //
    // Menu items
    //

    /** Delete menu item, if the menu item is a submenu, it will be destroyed.
        @param pos   0-based menu item index
    */
    inline void DeleteItem(bbUINT pos)
    {
        uiMenuDeleteItem(mhMenu, pos);
    }

    /** Detach menu item, if the menu item is a submenu, it will not be destroyed.
        @param pos   0-based menu item index
    */
    inline void DetachItem(bbUINT pos)
    {
        uiMenuDetachItem(mhMenu, pos);
    }

    /** Insert or append a menu item.
        @param pos  Position to insert after, if (bbUINT)-1 the item will be appended to the menu
        @param id   Menu item ID
        @param pStr 0-terminated menu item name
        @param user  User data
    */
    inline bbERR InsertItem(bbUINT pos, bbUINT id, const bbCHAR* pStr, const void* const user)
    {
        return uiMenuInsertItem(mhMenu, pos, id, pStr, user);
    }

    /** Update a menu item.
        @param pos   Position to insert after, if (bbUINT)-1 the item will be appended to the menu
        @param id    Menu item ID, (bbUINT)-1 to do not update ID
        @param pStr  0-terminated menu item name, NULL to do not update name
        @param user  User data (void*)-1 to do not update
    */
    inline bbERR UpdateItem(bbUINT const pos, bbUINT const id, const bbCHAR* const pStr, const void* const user)
    {
        return uiMenuUpdateItem(mhMenu, pos, id, pStr, user);
    }

    /** Insert or append a submenu.
        @param pos  Position to insert after, if (bbUINT)-1 the item will be appended to the menu
        @param id   Menu item ID
        @param pStr 0-terminated menu item name
    */
    inline bbERR InsertSubMenu(bbUINT pos, uiMENUH hSubMenu, const bbCHAR* pStr)
    {
        return uiMenuInsertSubMenu(mhMenu, pos, hSubMenu, pStr);
    }

    inline bbERR InsertSeparator(bbUINT pos)
    {
        return uiMenuInsertSeparator(mhMenu, pos);
    }

    /** Insert or append a list of menu items.
        @param pos   Position to insert after, if (bbUINT)-1 the item will be appended to the menu
        @param pList Pointer to array of items to insert, terminated with uiMENUITEMFLAG_LAST
    */
    inline bbERR InsertItemList(bbUINT pos, const uiMenuItem* const pList)
    {
        return uiMenuInsertItemList(mhMenu, pos, pList);
    }

    /** Insert or append a list of strings as menu items.
        @param pos   Position to insert after, if (bbUINT)-1 the item will be appended to the menu
        @param pStringList Pointer to array of string pointers
        @param count Size of array pStringList[]
        @param opt   Low 16 bit: Options, combined bitmask from uiMENUINSERTOPT and uiMENUAUTOKEY flags <br>
                     High 16 bit: Command ID to assign with all menu items
    */
    inline bbERR InsertStringList(bbUINT startpos, const bbCHAR* const* const pStringList, bbUINT count, bbU32 opt)
    {
        return uiMenuInsertStringList(mhMenu, startpos, pStringList, count, opt);
    }

    /** Insert item into menu item history (FIFO).
        This function is intended to manage recent file or window histories in a menu.
        It treats a range of menu items as a history FIFO: A new item will be inserted
        at the start of the list. An item of the same name will be deleted. If the item range
        exceeds \a maxcount, the last item in the list will be deleted.
        @param start    Start position of item history
        @param maxcount Maximum size of history
        @param pStr     New item to insert at start of list, pointer to 0-terminated string.
        @param opt      Low 16 bit flag bitmask (see uiMENUITEMHISTORY), top 16 bit menu item ID
        @param user     User data for item
    */
    inline bbERR ItemHistoryInsert(bbUINT const start, bbUINT const maxcount, const bbCHAR* const pStr, bbU32 const opt, void* user)
    {
        return uiMenuItemHistoryInsert(mhMenu, start, maxcount, pStr, opt, user);
    }

    /** Get number of items in menu.
        return Number of item, or -1 on failure
    */
    inline int GetItemCount()
    {
        return uiMenuGetItemCount(mhMenu);
    }

    /** Find menu item position by name.
        @param pName Menu item name, 0-terminated string
        @return 0-based menu item position, or -1 if not found, or -2 on error.
                On error check bbErrGet() for failure reason.
    */
    inline int FindItem(const bbCHAR* const pName)
    {
        return uiMenuFindItem(mhMenu, pName);
    }

    /** Find menu item position by user data and command ID.
        @param start Start menu index to search from
        @param max Maximum number of menu items to search
        @param user User data to search
        @param cmdID Command ID, -1 for any
        @return 0-based menu item position, or -1 if not found or error.
    */
    inline int FindItemData(bbUINT const start, bbUINT const max, int cmdID, void* user)
    {
        return uiMenuFindItemData(mhMenu, start, max, cmdID, user);
    }

    /** Get menu item ID by position.
        @param pos   0-based menu item position
        @return ID or (bbUINT)-1 on error
    */
    inline bbUINT GetItemID(bbUINT pos)
    {
        return uiMenuGetItemID(mhMenu, pos);
    }
        
    inline void* GetItemData(bbUINT pos)
    {
        return uiMenuGetItemData(mhMenu, pos);
    }

    /** Get menu item string by position.
        @param pos    0-based menu item position
        @param pStr   Buffer to return string, or NULL to read string length. Buffer must
                      be large enough to hold string including 0-terminator.
        @param buflen Length of buffer pStr in character code units, must be 0 to read string length
        @return String length in character code units without 0-terminator, or 0 on failure
    */
    inline int GetItemString(bbUINT const pos, bbCHAR* const pStr, bbUINT const buflen)
    {
        return uiMenuGetItemString(mhMenu, pos, pStr, buflen);
    }

    /** Get menu item string by position in heap block.
        @param pos    0-based menu item position
        @param pLen   Returns length of string in code units, without 0-terminator, can be NULL.
                      Unchanged on error.
        @return Heap block containing 0-terminated string, must be freed externally. NULL on error.
    */
    bbCHAR* GetItemStringMem(bbUINT pos, bbUINT* const pLen)
    {
        return uiMenuGetItemStringMem(mhMenu, pos, pLen);
    }

    /** Get menu item submenu handle by position.
        @param pos   0-based menu item position
        @return Handle to submenu or NULL on error
    */
    inline uiMENUH GetSubMenu(bbUINT pos)
    {
        return uiMenuGetSubMenu(mhMenu, pos);
    }

    /** Get menu item state.
        @param pos    0-based menu item position
        @return Bitmask with item state, check for the following bits: uiMENUITEMFLAG_GREY, uiMENUITEMFLAG_CHECK
    */
    inline bbUINT GetItemState(bbUINT pos)
    {
        return uiMenuGetItemState(mhMenu, pos);
    }

    /** Enable or grey a menu item.
        @param pos    Position to change
        @param state  !=0 to enable, 0 to grey
    */
    inline void EnableItem(bbUINT pos, int const state)
    {
        uiMenuEnableItem(mhMenu, pos, state);
    }

    /** Check or uncheck a menu item.
        @param pos    Position to change
        @param state  !=0 to check, 0 to uncheck
    */
    inline void CheckItem(bbUINT pos, bbUINT state)
    {
        uiMenuCheckItem(mhMenu, pos, state);
    }

    /** Check one menu item and uncheck all other items from a group.
        @param first Index of first menu item of the group
        @param last  Index of first menu item of the group, inclusive
        @param check Index of item to check
    */
    inline void CheckRadioItem(bbUINT first, bbUINT last, bbUINT check)
    {
        uiMenuCheckRadioItem(mhMenu, first, last, check);
    }

    //
    // Popup
    //

    /** Show this menu as popup anywhere on the screen.
        @param x X position on screen, top-left corner
        @param y Y position on screen, top-left corner
        @param hWin Window handle of window to receive menu selection notifications
    */
    inline bbERR ShowPopup(int x, int y, uiWINH hWin)
    {
        return uiMenuShowPopup(mhMenu, x, y, hWin);
    }
};

/*@}*/

#endif /* uiMenu_H_ */

