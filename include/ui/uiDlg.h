#ifndef uiDlg_H_
#define uiDlg_H_

#include "uiWin.h"

#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
#include "commdlg.h"
#include "CDERR.H"
#endif

/** @addtogroup uiDlg Dialogs
@{*/

struct uiDlg : uiWin
{
    bbU16 mMinWidth;   //!< Minimum window width
    bbU16 mMinHeight;  //!< Minimum window height
    bbU16 mMaxWidth;   //!< Minimum window width
    bbU16 mMaxHeight;  //!< Minimum window height

    uiDlg()
    {
        mMinWidth = mMinHeight =
        mMaxWidth = mMaxHeight = 0;
    }

    /** Create window as modeless dialog. */
    uiWINH Create(uiWINH hParent, const bbCHAR* pName, bbUINT id, bbU32 style, bbU32 styleEx, uiRect const* pRect);

    /** Create window as modal dialog.
    */
    bbUINT CreateModal(uiWINH hParent, const bbCHAR* pName, bbU32 style=0, bbU32 styleEx=0, uiRect const* pRect = NULL);

    /** Show window as modal dialog.
        The parent window passed in parameter \a hParent will be disabled.
        ShowModal will not return, until it is ended externally via uiDlg::EndModal().

        @param hParent Parent window to open the dialog under
        @return Return code as passed via uiDlg::EndModal(), or -1 on error.
    */
    //bbUINT ShowModal(uiWINH hParent);

    /** End modal dialog with specified reason code.
        @param code Reason code to return via uiDlg::ShowModal().
    */
    void EndModal(bbUINT code);

    bbERR CreateChildList(const uiWinDesc* pList, bbUINT count);

    /** Set a minimum dialog window size for resizable dialog windows.
        @param width Minimum width
        @param height Minimum height
    */
    inline void SetMinSize(bbUINT const width, bbUINT const height)
    {
        mMinWidth = (bbU16)width;
        mMinHeight = (bbU16)height;
    }

    /** Set a maximum dialog window size for resizable dialog windows.
        @param width Maximum width
        @param height Maximum height
    */
    inline void SetMaxSize(bbUINT const width, bbUINT const height)
    {
        mMaxWidth = (bbU16)width;
        mMaxHeight = (bbU16)height;
    }

    /** uiDlg default command handler.
        Handles default commands uiCMD_OK and uiCMD_CANCEL.
        Overrides should call this default handler for unhandled messages.
    */
    virtual bbERR HandleCmd(uiCMDSRC src, bbUINT cmd, bbUINT pos, void* hSrc, void* user);
};

enum uiDLGMSGOPT
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    uiDLGMSGOPT_OK           = MB_OK,          //!< Dialog with an OK button
    uiDLGMSGOPT_YESNOCANCEL  = MB_YESNOCANCEL, //!< Dialog with Yes, No, Cancel buttons
    uiDLGMSGOPT_YESNO        = MB_YESNO,       //!< Dialog with Yes, No buttons
    uiDLGMSGOPT_ICONQUESTION = MB_ICONQUESTION,//!< Display Question mark icon
#else
#endif
};

/** Create, display and execute a message box.
    @param hWin     Parent window handle
    @param pCaption Window caption string, 0-terminated
    @param pMsg     Message string, 0-terminated
    @param opt      Message box option bitmask, see uiDLGMSGOPT
    @return Returns the uiCMD ID of the button selected, or 0 on error
*/
bbUINT uiDlgMsg(uiWINH hWin, const bbCHAR* pCaption, const bbCHAR* pMsg, bbUINT opt);

enum uiDLGFILEOPT
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)
    uiDLGFILEOPT_MULTISELECT        = OFN_ALLOWMULTISELECT,     //!< Allow multiple file selection
    uiDLGFILEOPT_NODEREFERENCELINKS = OFN_NODEREFERENCELINKS,   //!< Do not resolve links, return link instead
    uiDLGFILEOPT_OVERWRITEPROMPT    = OFN_OVERWRITEPROMPT,      //!< Prompt user to confirm overwrite
#else
#endif
    uiDLGFILEOPT_OPEN               = 0,                        //!< Create a file-open dialog (default)
    uiDLGFILEOPT_SAVE               = 0x80000000UL              //!< Create a file-save dialog
};

/** Filetype filter parameter structure for uiDlgFileName. */
struct uiDlgFileNameFilter
{
    bbUINT        FilterIndex;  /**< 1-based index into filter list, on entry this is the initial
                                     filter selection, on exit this is the filter selection made
                                     by the user.
                                */
    const bbCHAR* pFilter;      /**< Filter list, this is a list of Description, Filetype "*.EXT"
                                     pairs. Filetype can be an empty string. Each string is
                                     0-terminated, the whole list is 0,0-terminated.
                                */
};

/** Create, display and execute a file open or save dialog box.
    @param hWin     Parent window handle
    @param pPath    Path to initialize dialog box with, can be NULL.
                    If the string contains a complete path, the dialog box changes the directory to that location.
                    Otherwise only the filename is copied into the filename edit box.
    @param opt      Option bitmask, see uiDLGFILEOPT
    @param pFilter  Pointer to filetype filter option, or NULL if no filtering to be performed.
    @return If the uiDLGFILEOPT_MULTISELECT flag was not set, a heap block with the full path of the filename
            is returned. If the uiDLGFILEOPT_MULTISELECT flag was set, an oqaque block describing the
            selected filenames is returned, use uiDlgFileNameNext() to enumerate the individual pathes.
            On success the returned block must be freed externally using bbMemFree().
            On failure or cancel, NULL is returned. If the return value is NULL and bbErrGet() returns bbEEND,
            the user pressed Cancel.
*/
bbCHAR* uiDlgFileName(uiWINH hWin, const bbCHAR* pPath, bbUINT opt, uiDlgFileNameFilter* const pFilter = NULL);

/** Enumerate filenames from the result of a multi-selection uiDlgFileName() dialog box.
    If the enumeration ended, and this is called again, the enumeration will start over.
    @param pFileNames Opaque filename block returned from uiDlgFileName()
    @param pBuffer Path block to re-use, or NULL to allocate a new heap block.
                   A block returned from a previous call can be reused to reduce memory reallocation
                   and copy overhead. If pBuffer pointed to a block, and NULL or -1 is returned,
                   the pBuffer will be freed.
    @return Full path of next filename in selection, or -1 on end of enumeration, or NULL on error.
            The returned block is a heap block and must be freed either by passing it as pBuffer
            parameter, or by calling bbMemFree() explicitely.
*/
bbCHAR* uiDlgFileNameNext(bbCHAR* pFileNames, bbCHAR* pBuffer);

/*@}*/

#endif
