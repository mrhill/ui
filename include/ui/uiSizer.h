#ifndef uiSizer_H_
#define uiSizer_H_

#include "uidefs.h"
#include "babel/Arr.h"

struct uiSizerItem
{
    uiWINH hWin;
    bbUINT opt;
};

bbDECLAREARR(uiSizerItem, uiArrSizerItem, 8);

#define uiSIZERITEM_SPACEMASK 0x3FF  //!< To mask spacing from uiSizerItem::opt

/** uiSizer::AddWinList array item */
struct uiSizerWin
{
    bbU16 id;   //!< Child window ID
    bbU16 opt;  //!< uiSizer option
};

struct uiSizer
{
    uiArrSizerItem mItems;

    uiSizer();
    ~uiSizer();

    /** Add window to the sizer.
        The added window must have a parent.
        @param hWin Window handle, if NULL the call does nothing and returns bbEOK,
                    (for easier error handling, i.e. AddWin(uiWinGetChild(ID), opt) ).
        @param opt CSizer implementation specific parameter, optional spacing in low 8 bit
    */
    bbERR AddWin(uiWINH const hWin, bbU32 const opt);

    /** Add child window list to the size.
        @param hParent Window handle of parent window
        @param pID     Pointer to <window ID>, <size option> pairs
        @param count   Number of pairs
    */
    bbERR AddWinList(uiWINH const hParent, const uiSizerWin* pItem, bbUINT count);
};

enum uiSIZERITEM_LINE
{
    uiSIZERITEM_LINE_RIGHT  = 0x4000U, //!< Align right edge to line, default is left
    uiSIZERITEM_LINE_BOTTOM = 0x4000U, //!< Align bottom edge to line, default is top
    uiSIZERITEM_LINE_FIXED  = 0x8000U, //!< Keep item width/height fixed, i.e. move the item
};

enum uiSIZER_LINE
{
    uiSIZER_LINE_HORZ = 0,
    uiSIZER_LINE_VERT = 1
};

/** Sizer: align child windows to a horizontal or vertical line. */
struct uiSizer_line : uiSizer
{
    int mLinePos;
    int mVerticalLine;

    /** Set line to use as reference.
        @param pos x or y position, if negative the position is relative to right/bottom edge of parent rectangle
        @param orientation uiSIZER_LINE_VERT = vertical line and \a pos is x, <br>
                           uiSIZER_LINE_HORZ = horizontal line and \a pos is y
    */
    inline void SetLine(int const pos, uiSIZER_LINE const orientation)
    {
        mLinePos = pos;
        mVerticalLine = orientation;
    }

    void OnSize(const uiRect* const pRect);
};

enum uiSIZERITEM_CHAIN
{
    uiSIZERITEM_CHAIN_LEFT        = 0x0000U, //!< Align left edge to right of last item
    uiSIZERITEM_CHAIN_RIGHT       = 0x4000U, //!< Align right edge to left of last item
    uiSIZERITEM_CHAIN_TOP         = 0x8000U, //!< Align top to edge to bottom of last item
    uiSIZERITEM_CHAIN_BOTTOM      = 0xC000U, //!< Align bottom edge to top of last item
    uiSIZERITEM_CHAIN_ALIGNRIGHT  = 0x1000U, //!< Align to right edge of last item for vertical chaining
    uiSIZERITEM_CHAIN_ALIGNBOTTOM = 0x1000U, //!< Align to bottom edge of last item for horizontal chaining
    uiSIZERITEM_CHAIN_ABSOLUTE    = 0x2000U  //!< Treat this item as absolute reference
};

/** Sizer: align child windows to each other by chaining them together.
    The first item added will always be used as absolute reference.
*/
struct uiSizer_chain : uiSizer
{
    void OnSize(const uiRect* const pRect);
};

#endif /* uiSizer_H_ */

