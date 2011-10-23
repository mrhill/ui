#ifndef uiFrame_H_
#define uiFrame_H_

#include "uiWin.h"

/** @addtogroup uiFrame Frame

    The uiFrame class implements frame windows. Frames have the same features as windows (uiWin),
    with the following additions:
    - Menu Bar
    - Tool Bar
    - Status Bar

    Frames will always have the uiWS_CAPTION style set.
@{*/

struct uiFrame : uiWin
{
    /** Create frame and attach to this class instance.
        For parameter documentation see uiWin::Create().
    */
    inline uiWINH Create(uiWINH hParent, const bbCHAR* pName, bbUINT id = 0, bbU32 style = 0, bbU32 styleEx = 0, uiRect const* pRect = NULL)
    {
        return uiWin::Create(hParent, pName, id, style | uiWS_CAPTION, styleEx, pRect);
    }
};

/*@}*/

#endif
