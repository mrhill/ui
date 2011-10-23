#include "uiApp.h"
#include "babel/log.h"
#include "babel/mem.h"
#include "babel/str.h"
#include "ptGCGDI.h"

static bbCHAR* uigpAppPath; // Pointer to app executable path

#if (bbOS==bbOS_WINCE) || (bbOS==bbOS_WIN32)

#ifdef _DEBUG
#include <crtdbg.h> // for memleaks
#endif

static void LogHandler(const bbLOGLEVEL loglevel, const bbCHAR* pMsg)
{
    OutputDebugString(pMsg);
    if (pMsg[bbStrLen(pMsg)-1] != '\n')
        OutputDebugString(bbT("\n"));
}

#include <shellapi.h> // command line parser
#include <commctrl.h>

HINSTANCE uighInstance;
ptGCGDI* uigpGC = NULL;
ptPal uigPal;
bbU32 uigPalRGB[16] = { 0x000000,0xFFFFFF };

extern bbERR uiRegisterWinClass();

static void SetupPal()
{
    uigPalRGB[uiCOL_APPWORKSPACE] = GetSysColor(COLOR_APPWORKSPACE);
    uigPalRGB[uiCOL_WINBG] = GetSysColor(COLOR_WINDOW);
}

extern "C" bbERR uiInitialize(bbCHAR* pAppPath, void* hInstance)
{
    #ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF /*| _CRTDBG_LEAK_CHECK_DF*/ );
    //_CrtSetBreakAlloc(34300);
    #endif

    uighInstance = (HINSTANCE)hInstance;
    uigpAppPath = pAppPath;
    bbASSERT(uigpAppPath);

    INITCOMMONCONTROLSEX sex;
    sex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    sex.dwICC = ICC_LISTVIEW_CLASSES|/*ICC_COOL_CLASSES|*/ ICC_BAR_CLASSES/* | ICC_TREEVIEW_CLASSES | ICC_WIN95_CLASSES | ICC_UPDOWN_CLASS*/;
    if (!InitCommonControlsEx(&sex))
    {
        goto err;
    }

    //
    // Register ui WindowProc
    //
    if (uiRegisterWinClass() != bbEOK)
    {
        goto err;
    }

    //
    // babel initialization
    //
    bbLogSetHandler(LogHandler);

    //
    // picotype initialization
    //
    uigpGC = new ptGCGDI;
    if (!uigpGC || (uigpGC->Init() != bbEOK))
    {
        goto err;
    }

    SetupPal();
    uigPal.Init();
    uigPal.mpRGB     = uigPalRGB;
    uigPal.mOpt      = ptPALOPT_EXTRGB;
    uigPal.mColCount = 16;

    uigpGC->SetLogPal(&uigPal);

    return bbEOK;

    err:
    uiDestroy();
    return bbELAST;
}

void uiDestroy()
{
    if (uigpGC)
    {
        uigpGC->Destroy();
        delete uigpGC;
    }
    uigPal.Destroy();

    #ifdef _DEBUG
    //_CrtDumpMemoryLeaks();
    #endif
}

bbERR uiMessageLoopRun()
{
    int ret;
    MSG msg;

    while ((ret = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (ret == -1)
        {
            bbLog(bbWarning, bbT("GetMessage error 0x%08X\n"), GetLastError());
        }
        else
        {
            if ((bbEEND != DispatchMessage(&msg)) || (msg.message != WM_KEYDOWN))
                TranslateMessage(&msg);
        }
    }

    return bbEOK;
}

void uiMessageLoopTerm()
{
    ::PostQuitMessage(1);
}

uiCONFIGH uiConfigOpen(const bbCHAR* const pConfigFile)
{
    bbCHAR* pConfigPath = bbStrAlloc(uiGetAppDir(NULL) + bbStrLen(pConfigFile));
    if (!pConfigPath)
        return NULL;
    bbStrCpy(pConfigPath + uiGetAppDir(pConfigPath), pConfigFile);
    return (uiCONFIGH)pConfigPath;
}

bbCHAR* uiConfigGetStr(uiCONFIGH const hConfig, const bbCHAR* pSection, const bbCHAR* pKey)
{
    const bbCHAR* const pNotFound = bbT("ui23");
    bbCHAR* pStr = bbStrAlloc(4096);

    if (pStr)
    {
        pStr[0] = 0;
        bbU32 len = GetPrivateProfileString(pSection, pKey, pNotFound, pStr, 4096, (const bbCHAR*)hConfig);
        if (bbStrCmp(pStr, pNotFound) == 0)
        {
            bbErrSet(bbENOTFOUND);
            bbMemFree(pStr);
            return NULL;
        }
        bbMemRealloc(sizeof(bbCHAR)*(len+1), (void**)&pStr);
    }

    return pStr;
}

bbS64 uiConfigGetS64(uiCONFIGH const hConfig, const bbCHAR* pSection, const bbCHAR* pKey)
{
    bbCHAR* pStr = uiConfigGetStr(hConfig, pSection, pKey);
    if (!pStr)
        return 0;

    bbS64 val;

    if (bbStrToS64(pStr, NULL, &val, bbSTROPT_ALLFMT|bbSTROPT_MINUS) == bbEOK)
        bbErrSet(bbEOK);
    else
        val = 0;

    bbMemFree(pStr);

    return val;
}

bbERR uiConfigSetS64(uiCONFIGH const hConfig, const bbCHAR* pSection, const bbCHAR* pKey, bbS64 val)
{
    bbCHAR str[32];
    bbSprintf(str, bbT("%I64d"), val);
    return uiConfigSetStr(hConfig, pSection, pKey, str);
}

bbERR uiConfigSetStr(uiCONFIGH const hConfig, const bbCHAR* pSection, const bbCHAR* pKey, const bbCHAR* pVal)
{
    if (!WritePrivateProfileString(pSection, pKey, pVal, (const bbCHAR*)hConfig))
        return bbErrSet(bbESYS);

    return bbEOK;
}

bbCHAR* uiConfigGetSectionList(uiCONFIGH const hConfig)
{
    bbCHAR* pBuf = bbStrAlloc(6144);
    if (pBuf)
    {
        DWORD ret = GetPrivateProfileSectionNames(pBuf, 6144, (const bbCHAR*)hConfig);

        if ((int)(ret-1) >= (6144-2-1))
        {
            if (ret == 6144-2)
                bbErrSet(bbEFULL);
            else
                bbErrSet(bbESYS);

            bbMemFree(pBuf);
            pBuf = NULL;
        }
    }
    return pBuf;
}

bbCHAR* uiConfigGetSection(uiCONFIGH const hConfig, const bbCHAR* pSection)
{
    bbCHAR* pBuf = bbStrAlloc(6144);
    if (pBuf)
    {
        DWORD ret = GetPrivateProfileSection(pSection, pBuf, 6144, (const bbCHAR*)hConfig);

        if (((int)(ret-1) >= (6144-2-1)) || (*pBuf == 0))
        {
            if (ret == 6144-2)
                bbErrSet(bbEFULL);
            else
                bbErrSet(bbENOTFOUND);

            bbMemFree(pBuf);
            pBuf = NULL;
        }
    }
    return pBuf;
}

bbERR uiConfigSetSection(uiCONFIGH const hConfig, const bbCHAR* pSection, const bbCHAR* pKeyVal)
{
    if (!WritePrivateProfileSection(pSection, pKeyVal, (const bbCHAR*)hConfig))
            return bbErrSet(bbESYS);

    return bbEOK;
}

bbU8* uiConfigGet(uiCONFIGH const hConfig, const bbCHAR* pSection, const bbCHAR* pKey, bbUINT size)
{
    bbU8* pBuf = (bbU8*)bbMemAlloc(size);

    if (pBuf)
    {
        if (!GetPrivateProfileStruct(pSection, pKey, pBuf, size, (const bbCHAR*)hConfig))
        {
            bbErrSet(bbENOTFOUND);
            bbMemFree(pBuf);
            return NULL;
        }
    }

    return pBuf;
}

bbERR uiConfigSet(uiCONFIGH const hConfig, const bbCHAR* pSection, const bbCHAR* pKey, const bbU8* pData, bbUINT const size)
{
    if (!WritePrivateProfileStruct(pSection, pKey, (void*)pData, size, (const bbCHAR*)hConfig))
        return bbErrSet(bbESYS);

    return bbEOK;
}


#else

int main(int argc, char** argv)
{
    return uiMain(argc, argv);
}

#endif

bbUINT uiGetAppDir(bbCHAR* pDir)
{
    bbUINT length = bbStrLen(uigpAppPath);

    while (length--)
    {
        if ((uigpAppPath[length] == '\\') || (uigpAppPath[length] == '/'))
            break;
    }

    if (!length)
    {
        bbErrSet(bbEUK);
        return (bbUINT)-1;
    }

    length++;

    if (pDir)
    {
        bbMemMove(pDir, uigpAppPath, length*sizeof(bbCHAR));
        pDir[length] = 0;
    }

    return length;
}

bbCHAR* uiGetAppPath(const bbCHAR* pTail)
{
    bbUINT taillen = bbStrLen(pTail);
    bbCHAR* pPath = bbStrAlloc(uiGetAppDir(NULL) + taillen);
    if (pPath)
        bbStrCpy(pPath+uiGetAppDir(pPath), pTail);
    return pPath;
}

#if (bbOS!=bbOS_WINCE) && (bbOS!=bbOS_WIN32)
bbERR uiMonitorGetInfo(uiMONH const hMon, uiMonInfo* const pInfo)
{
    return bbErrSet(bbENOTSUP);
}
#endif

void uiSysSetPal(ptPal* const pPal)
{
    uigpGC->SetLogPal(pPal ? pPal : &uigPal);
}

struct uiClipHandle
{
    HGLOBAL hMem;
    bbUINT  mFmt;
    bbU8    mWrite;
};

uiCLIPH uiClipOpen(uiWINH hWin)
{
    static uiClipHandle hClip;

    if (!OpenClipboard(hWin))
    {
        bbErrSet(bbESYS);
        return NULL;
    }

    bbMemClear(&hClip, sizeof(hClip));
    return &hClip;
}


void uiClipClose(uiCLIPH hClip)
{
    if (hClip)
    {
        uiClipDataClose(hClip);
        CloseClipboard();
    }
}

bbU8* uiClipDataRead(uiCLIPH hClip, bbUINT fmt)
{
    if (hClip)
    {
        uiClipDataClose(hClip);
        uiClipHandle& clip = *(uiClipHandle*)hClip;
        clip.mWrite = 0;

        if (!IsClipboardFormatAvailable(fmt))
        {
            bbErrSet(bbENOTSUP);
            return NULL;
        }

        if ((clip.hMem = GetClipboardData(fmt)) != NULL)
        { 
            bbU8* pMem = (bbU8*)GlobalLock(clip.hMem);
            bbASSERT(((bbU32)pMem & 3) == 0);
            if (pMem)
                return pMem;
        }
    }

    bbErrSet(bbESYS);
    return NULL;
}

void uiClipDataClose(uiCLIPH hClip)
{
    if (hClip)
    {
        uiClipHandle& clip = *(uiClipHandle*)hClip;

        if(clip.hMem)
        {
            GlobalUnlock(clip.hMem);

            if (clip.mWrite)
                SetClipboardData(clip.mFmt, clip.hMem);

            clip.hMem = NULL;
            clip.mWrite = 0;
        }
    }
}

bbU8* uiClipDataWrite(uiCLIPH hClip, bbUINT fmt, bbU32 size)
{
    if (hClip)
    {
        uiClipHandle& clip = *(uiClipHandle*)hClip;

        if (!clip.mWrite)
            EmptyClipboard();

        uiClipDataClose(hClip);
        clip.mWrite = 1;
        clip.mFmt = fmt;

        if ((clip.hMem = GlobalAlloc(GMEM_MOVEABLE, size)) != NULL)
        {
            bbU8* pMem = (bbU8*)GlobalLock(clip.hMem); 
            if (pMem)
            {
                bbASSERT(((bbU32)pMem & 3) == 0);
                return pMem;
            }
        }
    }

    bbErrSet(bbESYS);
    return NULL;
}


