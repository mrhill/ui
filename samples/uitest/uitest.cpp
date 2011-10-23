#include "ui/ui.h"
#include "babel/babel.h"

struct myDlg : uiDlg
{
    uiSizer_line mSizer_buttons;
    uiCtlButton mButtonOk;

    bbERR HandleMsg(bbU32 msg, void* p0, void* p1)
    {
        static uiWinDesc myDlg_buttons[] = {
            uiWC_CtlButton, uiCMD_OK,     0,0,64,32,  uiWS_TABSTOP|uiWS_BUTTON_DEFAULT,0, bbT("&OK"),
            uiWC_CtlButton, uiCMD_CANCEL, 80,0,64,32, uiWS_TABSTOP|uiWS_BUTTON_FLAT|uiWS_BUTTON_LEFT,0, bbT("&Cancel"),
        };

        uiRect rect;

        switch (msg)
        {
        case uiWM_CREATE:
            if (CreateChildList(myDlg_buttons, 2) != bbEOK)
                bbLog(bbErr, bbT("Error 0x%X creating mDlg controls\n"), bbErrGet());

            mSizer_buttons.SetLine(-40, uiSIZER_LINE_HORZ);
            mSizer_buttons.AddWin(GetChild(uiCMD_OK),     uiSIZERITEM_LINE_BOTTOM | uiSIZERITEM_LINE_FIXED);
            mSizer_buttons.AddWin(GetChild(uiCMD_CANCEL), uiSIZERITEM_LINE_BOTTOM | uiSIZERITEM_LINE_FIXED);

            GetClientRect(&rect);
            mSizer_buttons.OnSize(&rect);

            mButtonOk.Attach(GetChild(uiCMD_OK));

            return bbEOK;

        case uiWM_SIZE:
            rect.top = rect.left = 0;
            rect.right  = (bbU32)p1 & 0xFFFF;
            rect.bottom = (bbU32)p1 >> 16;
            mSizer_buttons.OnSize(&rect);
            return bbEOK;

        default:
            return uiDlg::HandleMsg(msg, p0, p1);
        }
    }

    bbERR HandleCmd(uiCMDSRC src, bbUINT cmd, bbUINT pos, void* hSrc, void* user)
    {
        return uiDlg::HandleCmd(src, cmd, pos, hSrc, user);
    }
};

struct myTopWin : uiWin
{
    uiMenu  mPopupMenu;
    myDlg   mDlg;

    bbERR HandleMsg(bbU32 msg, void* p0, void* p1)
    {
        switch (msg)
        {
        case uiWM_CREATE:

            // - Create popup menu
            if (mPopupMenu.Create())
            {
                mPopupMenu.InsertItem(-1, 123, bbT("Run myDlg"), NULL);
                mPopupMenu.InsertSeparator(-1);
                mPopupMenu.InsertItem(-1, 124, bbT("Hallo 2"), NULL);
            }
            else
            {
                bbLog(bbErr, bbT("Error 0x%X creating menu\n"), bbErrGet());
            }

            // - Create dialog
            /*
            if (!mDlg.Create(NULL, bbT("myDlg"), 0, uiWS_DLGFRAME | uiWS_SIZEBOX, uiWSE_DLGMODALFRAME))
                bbLog(bbErr, bbT("Error 0x%X creating mDlg\n"), bbErrGet());
                */
            return uiEPASS;

        case uiWM_DESTROY:
            mPopupMenu.Destroy();
            uiMessageLoopTerm();
            return bbEOK;

        case uiWM_CONTEXTMENU:
            if (mPopupMenu)
            {
                bbS32 x = ((bbS32)((bbS32)p1<<16)>>16);
                bbS32 y = (bbS32)p1 >> 16;
                mPopupMenu.ShowPopup(x, y, this->mhWin);
            }
            return bbEOK;

        case uiWM_PARENTNOTIFY:
            return uiEPASS;

        default:
            return uiEPASS;
        }
    }

    bbERR HandleCmd(uiCMDSRC src, bbUINT cmd, bbUINT pos, void* hSrc, void* user)
    {
        switch (cmd)
        {
        case 123:
            mDlg.CreateModal(mhWin, bbT("myDlg"));
            break;
        case 124:
            break;
        default:
            return uiEPASS;
        }
        return bbEOK;
    }
};

int uiMain(int argc, bbCHAR** argv)
{
    myTopWin win;
    uiWin    win2;
    uiRect   rect;
    uiFrame  frame;

    if (!win.Create(NULL, argv[0], 0, uiWS_OVERLAPPEDWINDOW | uiWS_CLIPCHILDREN | uiWS_HSCROLL | uiWS_VSCROLL, 0))
    {
        bbLog(bbErr, bbT("Error 0x%X creating window\n"), bbErrGet());
        return 1;
    }

    win.GetClientRect(&rect);
    rect.right >>=1;

    if (!win2.Create(win, argv[0], 0, uiWS_CHILD|uiWS_CAPTION|uiWS_VISIBLE|uiWS_SYSMENU|uiWS_MINIMIZEBOX|uiWS_MAXIMIZEBOX, 0, &rect))
    {
        bbLog(bbErr, bbT("Error 0x%X creating window\n"), bbErrGet());
        return 1;
    }

    win2.SetText(bbT("Child"));

    if (!frame.Create(NULL, argv[0]))
    {
        bbLog(bbErr, bbT("Error 0x%X creating frame\n"), bbErrGet());
        return 1;
    }

    uiMenu framemenu;
    framemenu.CreateBar();
    framemenu.InsertItem(-1, 0, bbT("puff"), NULL);
    frame.SetMenu(framemenu);

    win.Show();
    //frame.Show();
    uiMessageLoopRun();

	return 0;
}
