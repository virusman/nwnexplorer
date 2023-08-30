//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      nwnexplorer.cpp - Main module |
//
// This module contains the main.
//
// Copyright (c) 2002-2003 - Edward T. Smith
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met:
// 
// 1. Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
// 2. Neither the name of Edward T. Smith nor the names of its contributors 
//    may be used to endorse or promote products derived from this software 
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// @end
//
// $History: Cnf.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "nwnexplorer.h"
#include "MainWnd.h"
#include "ModelWnd.h"
#include <fmod.h>

//
// Debug NEW
//

#if defined (_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Global strings
//

TCHAR g_szAppName [256];
HWND g_hWndMain;
CPrinter g_sPrinter;
CDevMode g_sDevMode;
CImageList g_ilSmall;
CImageList g_ilLarge;
HMODULE g_hFMODLibrary = NULL;
D3DCOLOR *g_apPalettes [NwnPalette__Count];

//
// Constant file names
//

LPCTSTR g_pszKeyRoot = _T ("Software\\Torlack\\nwnexplorer");

//
// Toolbar command list
//

TB_Command g_asCommands [] =
{
    { ID_FILE_OPEN,                                     TBSTYLE_BUTTON, 0 },
    { ID_APP_ABOUT,                 TBSTYLE_BUTTON, 11 },
};
int g_nCommands = _countof (g_asCommands);

//
// ATL module
//

CAppModule _Module;
    BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

    //
    // Correct an ommision in the OnIdle
    //

    class CMyMessageLoop : public CMessageLoop
{
    public:
        // override to change idle processing
        virtual BOOL OnIdle(int /*nIdleCount*/)
        {
            BOOL fContinue = FALSE;
            for(int i = 0; i < m_aIdleHandler.GetSize(); i++)
            {
                CIdleHandler* pIdleHandler = m_aIdleHandler[i];
                if(pIdleHandler != NULL)
                {
                    if (pIdleHandler->OnIdle())
                        fContinue = TRUE;
                }
            }
            return fContinue;
        }
};

#pragma pack(1)
typedef struct tagTgaHeader
{
    BYTE   IdLength;            // Image ID Field Length
    BYTE   CmapType;            // Color Map Type
    BYTE   ImageType;           // Image Type

    WORD   CmapIndex;           // First Entry Index
    WORD   CmapLength;          // Color Map Length
    BYTE   CmapEntrySize;       // Color Map Entry Size

    WORD   X_Origin;            // X-origin of Image
    WORD   Y_Origin;            // Y-origin of Image
    WORD   ImageWidth;          // Image Width
    WORD   ImageHeight;         // Image Height
    BYTE   PixelDepth;          // Pixel Depth
    BYTE   ImagDesc;            // Image Descriptor
} TGAHEADER;
#pragma pack()

#define TGA_Null 0
#define TGA_Map 1
#define TGA_RGB 2
#define TGA_Mono 3
#define TGA_RLEMap 9
#define TGA_RLERGB 10
#define TGA_RLEMono 11
#define TGA_CompMap 32
#define TGA_CompMap4 33

//-----------------------------------------------------------------------------
//
// @func Windows Main
//
// @rdesc Program exit code
//
//-----------------------------------------------------------------------------

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                    LPSTR lpCmdLine, int nCmdShow)
{
    USES_CONVERSION;

    //
    // Resolve UNICOWS's thunk (required)
    //

    ::DefWindowProc (NULL, 0, 0, 0);

    //
    // Load the application name
    //

    ::LoadString (_Module .GetResourceInstance (),
                  IDR_NWNEXPLORER, g_szAppName, _countof (g_szAppName));

    //
    // Test for FMOD
    //

    g_hFMODLibrary = ::LoadLibrary (_T ("FMOD.DLL"));

    //
    // Initialize FMOD
    //

    if (g_hFMODLibrary != NULL)
    {
        if (FSOUND_GetVersion () < FMOD_VERSION)
        {
            //FIXME
        }
        if (!FSOUND_Init (44100, 32, FSOUND_INIT_GLOBALFOCUS))
        {
            //FIXME
            FSOUND_Close ();
            return 0;
        }
    }

    //
    // Create our image lists
    //

    {
        CDC dcMem;
        dcMem .CreateCompatibleDC (NULL);
        if (dcMem .GetDeviceCaps (BITSPIXEL) > 8)
        {
            g_ilSmall .Create (16, 16, ILC_COLOR16 | ILC_MASK, 16, 16);
            g_ilLarge .Create (24, 24, ILC_COLOR16 | ILC_MASK, 16, 16);
            CBitmap bmSmall;
            bmSmall .LoadBitmap (IDB_TOOLBAR_16_256COLOR);
            g_ilSmall .Add (bmSmall, RGB (255, 0, 255));
            CBitmap bmLarge;
            bmLarge .LoadBitmap (IDB_TOOLBAR_24_256COLOR);
            g_ilLarge .Add (bmLarge, RGB (255, 0, 255));
        }
        else
        {
            g_ilSmall .Create (16, 16, ILC_COLOR | ILC_MASK, 16, 16);
            g_ilLarge .Create (24, 24, ILC_COLOR | ILC_MASK, 16, 16);
            CBitmap bmSmall;
            bmSmall .LoadBitmap (IDB_TOOLBAR_16_16COLOR);
            g_ilSmall .Add (bmSmall, RGB (255, 0, 255));
            CBitmap bmLarge;
            bmLarge .LoadBitmap (IDB_TOOLBAR_24_16COLOR);
            g_ilLarge .Add (bmLarge, RGB (255, 0, 255));
        }
        g_ilSmall .SetBkColor (::GetSysColor (COLOR_3DFACE));
        g_ilLarge .SetBkColor (::GetSysColor (COLOR_3DFACE));
    }

    //
    // Enable leak checking
    //

#if defined (_DEBUG)
    _CrtSetDbgFlag (_CrtSetDbgFlag (_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

    //
    // Initialize ATL
    //

    _Module.Init (ObjectMap, hInstance);

    //
    // Initialize the printer
    //

    g_sPrinter .OpenDefaultPrinter ();
    g_sDevMode .CopyFromPrinter (g_sPrinter);

    //
    // Validate the version of the common controls
    //

    DWORD dwComCtlMajor, dwComCtlMinor;
    if (!GetDllVersion (TEXT ("comctl32.dll"), 
                        dwComCtlMajor, dwComCtlMinor) || dwComCtlMajor < 5 ||
        (dwComCtlMajor == 5 && dwComCtlMinor < 80))
    {
        ::MessageBox (NULL, 
                      _T ("You are running an old version of comctl32.dll.\r\n")
                      _T ("Please download a new version from:\r\n\r\n")
                      _T ("http://www.torlack.com/other/50comupd.exe")
                      , g_szAppName, MB_OK | MB_ICONHAND);
        return 0;
    }

    //
    // Initialize the common controls
    //

    INITCOMMONCONTROLSEX icex;    
    typedef WINCOMMCTRLAPI BOOL WINAPI _x (LPINITCOMMONCONTROLSEX); 
    _x *px;
    HINSTANCE hComCtl = LoadLibraryA ("comctl32.dll");
    px = (_x *) GetProcAddress (hComCtl, "InitCommonControlsEx");
    bool fWorked = false;
    if (px != NULL)
    {
        icex .dwSize = sizeof (INITCOMMONCONTROLSEX);
        icex .dwICC = ICC_WIN95_CLASSES | ICC_DATE_CLASSES | 
            ICC_USEREX_CLASSES | ICC_COOL_CLASSES;
        fWorked = (*px) (&icex) != 0;
    }
    if (!fWorked)
    {
        ::MessageBox (NULL, 
                      _T ( "Unable to initialize COMCTL32.DLL"), 
                      g_szAppName, MB_OK | MB_ICONHAND);
        return 0;
    }

    //
    // Create the message loop
    //

    CMyMessageLoop theLoop;
    _Module .AddMessageLoop (&theLoop);

    //
    // Create the window
    //

    CMainWnd sMainWnd;
    g_hWndMain = sMainWnd .Create ();
    if (g_hWndMain == NULL)
    {
        CString str (MAKEINTRESOURCE (IDS_ERR_MAIN_WINDOW));
        ::MessageBox (NULL, str, g_szAppName, MB_OK | MB_ICONHAND);
        return 0;
    }

    {
        BOOL bHandled;

        if (strlen(lpCmdLine) > 0)
            sMainWnd .DoOpen(lpCmdLine, false);
        else
            sMainWnd .OnFileOpenNwn(ID_FILE_OPENNWN, 0, g_hWndMain, bHandled);
    }

    //
    // Pump messages
    //

    theLoop .Run ();
    _Module .RemoveMessageLoop ();

    //
    // Delete the palettes
    //

    for (int i = 0; i < _countof (g_apPalettes); i++)
    {
        if (g_apPalettes [i])
            delete [] g_apPalettes [i];
    }

    //
    // Close up printer (Not required, but keeps Purify happy)
    //

    g_sPrinter .ClosePrinter ();
    g_sDevMode .Detach ();

    //
    // Close FMOD
    //

    if (g_hFMODLibrary != NULL)
    {
        FSOUND_Close ();
    }

    //
    // Terminate the module
    //

    _Module .Term ();
    return 0;
}

//-----------------------------------------------------------------------------
//
// @func Extract a substring
//
// @parm LPTSTR | pszReturn | Return string space
//
// @parm int | nMaxChan | Size of the return buffer in character
//
// @parm LPCTSTR | pszSource | Source string
//
// @parm int | nSubString | Substring to extract
//
// @parm TCHAR | cSeperator | Seperator character
//
// @rdesc Results of the extraction
//
//              @flag true | Substring was extracted
//              @flag false | Substring not found
//
//-----------------------------------------------------------------------------

bool ExtractString (LPTSTR pszReturn, int nMaxChar, 
                    LPCTSTR pszSource, int nSubString, TCHAR cSeperator)
{

    //
    // Validate the return string
    //

    if (pszReturn == NULL || nMaxChar < 1) 
        return false;

    //
    // Validate substring
    //

    if (nSubString < 0 || pszSource == NULL)
    {
        pszReturn [0] = 0;
        return false;
    }

    //
    // Move the source string pointer forward until it points to
    // the start of the string to be extracted
    //

    for (; nSubString > 0; nSubString--) 
    {
        pszSource = _tcschr (pszSource, cSeperator);
        if (pszSource == NULL) 
        {
            pszReturn [0] = 0;
            return false;
        }
        pszSource++;
    }

    //
    // Locate the end of the string
    //

    LPCTSTR psz = _tcschr (pszSource, cSeperator);
    int nLen = (int) (psz == NULL ? _tcslen (pszSource) : psz - pszSource);

    //
    // Clamp the length
    //

    if (nLen > nMaxChar - 1) 
        nLen = nMaxChar - 1;

    //
    // Copy the string
    //

    _tcsncpy (pszReturn, pszSource, nLen);
    pszReturn [nLen] = 0;       
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Get a bitmap size
//
// @parm HBITMAP | hBitmap | Bitmap
//
// @rdesc Size of the bitmap
//
//-----------------------------------------------------------------------------

SIZE GetBitmapSize (HBITMAP hBitmap)
{
    BITMAP bm;
    ::GetObject (hBitmap, sizeof (BITMAP), &bm);
    return CSize (bm .bmWidth, bm .bmHeight);
}

//-----------------------------------------------------------------------------
//
// @func Center the property sheet
//
// @parm CWindow & | sSheet | The sheet
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CenterPropertySheet (CWindow &sSheet)
{
    static LPCTSTR pszProp = _T ( "SheetCentered");
    if (::GetProp (sSheet .m_hWnd, pszProp) != 0)
        return;
    sSheet .CenterWindow (sSheet .GetParent ());
    ::SetProp (sSheet .m_hWnd, pszProp, (HANDLE) 1);
}

//-----------------------------------------------------------------------------
//
// @func Load the given popup menu
//
// @parm HINSTANCE | hInstance | Instance of the resource
//
// @parm UINT | id | ID of the menu resource
//
// @parm UINT | uSubMenu | Sub menu index of the popup
//
// @rdesc Handle of the popup menu or NULL if invalid.
//
//-----------------------------------------------------------------------------

HMENU LoadPopupMenu (HINSTANCE hInstance, UINT id, UINT uSubMenu)
{
    HMENU hmParent = ::LoadMenu (hInstance, MAKEINTRESOURCE (id));
    if (hmParent == NULL) 
        return NULL;
    HMENU hmPopup = GetSubMenu (hmParent, uSubMenu);
    RemoveMenu (hmParent, uSubMenu, MF_BYPOSITION);
    DestroyMenu (hmParent);
    return hmPopup;
}

//-----------------------------------------------------------------------------
//
// @func Decode the buttons
//
// @parm LPCTSTR | pszText | Button string
//
// @parm TBBUTTON * | pTBBtns | Array of buttons
//
// @parm int | nMaxButtons | Max buttons
//
// @rdesc Number of buttons placed in the array or -1 if there is 
//              an error.
//
//-----------------------------------------------------------------------------

int DecodeToolbarButtons (LPCTSTR pszText, TBBUTTON *pTBBtns, int nMaxButtons)
{
    int c;

    //
    // Initialize to no buttons
    //

    int nButtons = 0;

    //
    // Loop while we have buttons
    //

    while (*pszText)
    {

        //
        // Skip whitespace
        //

        while (true)
        {
            c = _tcsnextc (pszText);
            if (!_istspace (c))
                break;
            pszText = _tcsinc (pszText);
        }

        //
        // Break if end of string
        //

        if (c == 0)
            break;

        //
        // Convert the next block of digits
        //

        LPTSTR pszEnd;
        errno = 0;
        unsigned long ulCmd = _tcstoul (pszText, &pszEnd, 10);
        if (errno != 0 || pszText == pszEnd)
            return -1;
        pszText = pszEnd;

        //
        // psz whitespace
        //

        while (true)
        {
            c = _tcsnextc (pszText);
            if (!_istspace (c))
                break;
            pszText = _tcsinc (pszText);
        }

        //
        // This MUST be a comma or 0
        //

        if (c == ',')
            pszText = _tcsinc (pszText);
        else if (c == 0)
            ;
        else
            return -1;

        //
        // We have a value
        //

        if (ulCmd == 0)
        {
            if (nButtons >= nMaxButtons)
                break;
            pTBBtns [nButtons].iBitmap = 8;
            pTBBtns [nButtons].idCommand = 0;
            pTBBtns [nButtons].fsState = 0;
            pTBBtns [nButtons].fsStyle = TBSTYLE_SEP;
            pTBBtns [nButtons].dwData = 0;
            pTBBtns [nButtons].iString = 0;
            nButtons++;
        }
        else
        {
            int i;
            for (i = 0; i < g_nCommands; i++)
            {
                if (g_asCommands [i] .nCmd == (int) ulCmd)
                    break;
            }
            if (i < g_nCommands)
            {
                if (nButtons >= nMaxButtons)
                    break;
                pTBBtns [nButtons].iBitmap = g_asCommands [i] .iBitmap;
                pTBBtns [nButtons].idCommand = g_asCommands [i] .nCmd;
                pTBBtns [nButtons].fsState = TBSTATE_ENABLED;
                pTBBtns [nButtons].fsStyle = g_asCommands [i] .fsStyle;
                pTBBtns [nButtons].dwData = 0;
                pTBBtns [nButtons].iString = 0;
                nButtons++;
            }
        }
    }
    return nButtons;
}

//-----------------------------------------------------------------------------
//
// @func Test if a name is valid
//
// @parm LPCTSTR | pszName | Name of the rune library
//
// @rdesc True if the name is valid
//
//-----------------------------------------------------------------------------

bool IsValidTableName (LPCTSTR pszName)
{
    if (*pszName == 0)
        return false;
    while (*pszName)
    {
        int c = _tcsnextc (pszName);
        pszName = _tcsinc (pszName);
        if (!_istalnum (c) && c != '_')
            return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Test if a name is valid
//
// @parm LPCTSTR | pszName | Name of the rune library
//
// @rdesc True if the name is valid
//
//-----------------------------------------------------------------------------

bool IsValidBlueprintName (LPCTSTR pszName)
{
    if (*pszName == 0)
        return false;
    while (*pszName)
    {
        int c = _tcsnextc (pszName);
        pszName = _tcsinc (pszName);
        if (!_istalnum (c) && c != '_')
            return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Test if a name is valid
//
// @parm LPCTSTR | pszName | Name of the rune library
//
// @rdesc True if the name is valid
//
//-----------------------------------------------------------------------------

bool IsValidProfileName (LPCTSTR pszName)
{
    bool fHasStar = false;
    if (*pszName == 0)
        return false;
    while (*pszName)
    {
        int c = _tcsnextc (pszName);
        pszName = _tcsinc (pszName);
        if (!_istalnum (c) && c != '_')
        {
            if (c == '*')
            {
                if (fHasStar)
                    return false;
                fHasStar = true;
            }
            else
                return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Get the version of the DLL
//
// @parm LPCTSTR | pszDll | Full path to the DLL
//
// @parm DWORD & | dwMajor | Major version number
//
// @parm DWORD & | dwMinor | Minor version number
//
// @rdesc Routine results
//
//              @flag true | if able to retrieve the version number
//              @flag false | if unable to retrieve the version number
//
//-----------------------------------------------------------------------------

BOOL GetDllVersion (LPCTSTR pszDll, DWORD &dwMajor, DWORD &dwMinor)
{
    HRESULT hResult = E_FAIL;

    //
    // Load the DLL
    //

    HINSTANCE hComCtl = LoadLibrary (pszDll);

    //
    // If loaded
    //

    if (hComCtl) 
    {

        //
        // Get the address of the DllGetVersion method.
        //

#if defined (_WIN32_WCE)
        DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC) 
            GetProcAddress (hComCtl, L"DllGetVersion");
#else
        DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC) 
            GetProcAddress (hComCtl, "DllGetVersion");
#endif

        //
        // If the method was found
        //

        if (pDllGetVersion) 
        {

            //
            // Get the version information
            //

            DLLVERSIONINFO dvi;
            memset (&dvi, 0, sizeof (dvi));
            dvi .cbSize = sizeof (dvi);
            hResult = (*pDllGetVersion) (&dvi);

            //
            // Extract the version information
            //

            if (SUCCEEDED (hResult)) 
            {
                dwMajor = dvi .dwMajorVersion;
                dwMinor = dvi .dwMinorVersion;
            }
        }

        //
        // Free the library
        //

        FreeLibrary (hComCtl);
    }
    return SUCCEEDED (hResult);
}

//-----------------------------------------------------------------------------
//
// @func Get a setting from the register
//
// @parm LPCTSTR | pszName | Name of the setting
//
// @parm DWORD | dwDefault | Default value
//
// @rdesc Value.
//
//-----------------------------------------------------------------------------

DWORD GetSetting (LPCTSTR pszName, DWORD dwDefault)
{

    //
    // Open the parent
    //

    CRegKey rkParent;
    LONG lRet = rkParent .Open (HKEY_CURRENT_USER, g_pszKeyRoot);
    if (lRet != ERROR_SUCCESS)
        return dwDefault;

    //
    // Query the value
    //

    DWORD dwRet;
    lRet = rkParent .QueryDWORDValue (pszName, dwRet);
    if (lRet != ERROR_SUCCESS)
        return dwDefault;
    else
        return dwRet;
}

//-----------------------------------------------------------------------------
//
// @func Set a setting from the register
//
// @parm LPCTSTR | pszName | Name of the setting
//
// @parm DWORD | dwValue | New value
//
// @rdesc Success status.
//
//-----------------------------------------------------------------------------

bool SetSetting (LPCTSTR pszName, DWORD dwValue)
{

    //
    // Open the parent
    //

    CRegKey rkParent;
    LONG lRet = rkParent .Create (HKEY_CURRENT_USER, g_pszKeyRoot);
    if (lRet != ERROR_SUCCESS)
        return false;

    //
    // Set the value
    //

    lRet = rkParent .SetDWORDValue (pszName, dwValue);
    return lRet == ERROR_SUCCESS;
}
//-----------------------------------------------------------------------------
//
// @func Get a setting from the register
//
// @parm LPCTSTR | pszName | Name of the setting
//
// @parm LPCTSTR | pszDefault | Default value
//
// @parm LPTSTR | pszValue | Value
//
// @parm int | cchMaxText | Length of the buffer
//
// @rdesc Success status.
//
//-----------------------------------------------------------------------------

bool GetSetting (LPCTSTR pszName, LPCTSTR pszDefault, 
                 LPTSTR pszValue, int cchMaxText)
{

    //
    // Open the parent
    //

    CRegKey rkParent;
    LONG lRet = rkParent .Open (HKEY_CURRENT_USER, g_pszKeyRoot);
    if (lRet != ERROR_SUCCESS)
    {
        _tcsncpy (pszValue, pszDefault, cchMaxText);
        pszValue [cchMaxText - 1] = 0;
        return false;
    }

    //
    // Query the value
    //

    DWORD dwLen = cchMaxText;
    lRet = rkParent .QueryStringValue (pszName, pszValue, &dwLen);
    if (lRet != ERROR_SUCCESS)
    {
        _tcsncpy (pszValue, pszDefault, cchMaxText);
        pszValue [cchMaxText - 1] = 0;
        return false;
    }
    else
        return true;
}

//-----------------------------------------------------------------------------
//
// @func Set a setting from the register
//
// @parm LPCTSTR | pszName | Name of the setting
//
// @parm LPCTSTR | pszValue | New value
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

bool SetSetting (LPCTSTR pszName, LPCTSTR pszValue)
{

    //
    // Open the parent
    //

    CRegKey rkParent;
    LONG lRet = rkParent .Create (HKEY_CURRENT_USER, g_pszKeyRoot);
    if (lRet != ERROR_SUCCESS)
        return false;

    //
    // Set the value
    //

    lRet = rkParent .SetStringValue (pszName, pszValue);
    return lRet == ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the resource class
//
// @parm NwnResType | nResType | Type of the resource
//
// @parm const char * | pszName | Name of the resource
//
// @rdesc Resource class
//
//-----------------------------------------------------------------------------

ResClass GetResourceClass (NwnResType nType, const char *pszName)
{
    switch (nType)
    {
        case NwnResType_2DA: return ResClass_GameData;
        case NwnResType_4PC: return ResClass_Misc;
        case NwnResType_ARE: return ResClass_Area;
        case NwnResType_BIC: return ResClass_Misc;
        case NwnResType_BMP: return ResClass_Image;
        case NwnResType_BMU: return ResClass_Sound;
        case NwnResType_BTD: return ResClass_Misc;
        case NwnResType_BTE: return ResClass_Misc;
        case NwnResType_BTG: return ResClass_Misc;
        case NwnResType_BTI: return ResClass_Misc;
        case NwnResType_BTM: return ResClass_Misc;
        case NwnResType_BTP: return ResClass_Misc;
        case NwnResType_CCS: return ResClass_Misc;
        case NwnResType_CSS: return ResClass_Misc;
        case NwnResType_DLG: return ResClass_Dialog;
        case NwnResType_DTF: return ResClass_Misc;
        case NwnResType_DWK: return ResClass_Model_Walk_Door;
        case NwnResType_FAC: return ResClass_Module;
        case NwnResType_GFF: return ResClass_UI;
        case NwnResType_GIC: return ResClass_Area;
        case NwnResType_GIT: return ResClass_Area;
        case NwnResType_GUI: return ResClass_UI;
        case NwnResType_IFO: return ResClass_Module;
        case NwnResType_INI: return ResClass_Ini;
        case NwnResType_ITP: return ResClass_Blueprint_Palette;
        case NwnResType_JRL: return ResClass_Module;
        case NwnResType_LTR: return ResClass_Misc;
        case NwnResType_MTR: return ResClass_Material;        
        case NwnResType_MVE: return ResClass_Misc;
        case NwnResType_NCS: return ResClass_Compiled_Script;
        case NwnResType_NDB: return ResClass_Debug_Script;
        case NwnResType_NSS: return ResClass_Script;
        case NwnResType_PLT: return ResClass_Graphical_Palette;
        case NwnResType_PTM: return ResClass_Plot;
        case NwnResType_PTT: return ResClass_Plot;
        case NwnResType_PWK: return ResClass_Model_Walk_Place;
        case NwnResType_RES: return ResClass_Misc;
        case NwnResType_SAV: return ResClass_Misc;
        case NwnResType_SET: return ResClass_Ini;
        case NwnResType_SSF: return ResClass_Sound_Set;
        case NwnResType_TXI: return ResClass_TextureInfo;
        case NwnResType_SHD: return ResClass_Shader;
        case NwnResType_UTC: return ResClass_BP_Creatures;
        case NwnResType_UTD: return ResClass_BP_Door;
        case NwnResType_UTE: return ResClass_BP_Encounter;
        case NwnResType_UTG: return ResClass_Misc;
        case NwnResType_UTI: return ResClass_BP_Item;
        case NwnResType_UTM: return ResClass_BP_Merchant;
        case NwnResType_UTP: return ResClass_BP_Placeable;
        case NwnResType_UTS: return ResClass_BP_Sound;
        case NwnResType_UTT: return ResClass_BP_Trigger;
        case NwnResType_UTW: return ResClass_BP_Waypoint;
        case NwnResType_WAV: return ResClass_Sound;
        case NwnResType_WOK: return ResClass_Model_Walk_Tile;

        case NwnResType_DDS:
        case NwnResType_TGA: {
            if (pszName != NULL) {
                size_t nLen = strlen(pszName);

                if (nLen > 16)
                    nLen = 16;

                if (nLen > 7                                 &&
                    (pszName[0] == 'M' || pszName[0] == 'm') &&
                    (pszName[1] == 'I' || pszName[1] == 'i')) {

                    if (pszName[6] == '_'                        &&
                        (pszName[4] >= '0' && pszName[4] <= '9') &&
                        (pszName[5] >= '0' && pszName[5] <= '9'))
                        return ResClass_Texture_Minimap;

                    if (nLen > 9 &&
                        pszName[2] == '_' && pszName[8] == '_'   &&
                        (pszName[6] >= '0' && pszName[6] <= '9') &&
                        (pszName[7] >= '0' && pszName[7] <= '9'))
                        return ResClass_Texture_Minimap;
                }

                if (nLen > 7                                 &&
                    pszName[6] == '_'                        &&
                    (pszName[0] == 'M' || pszName[0] == 'm') &&
                    (pszName[1] == 'Z' || pszName[1] == 'z') &&
                    (pszName[4] >= '0' && pszName[4] <= '9') &&
                    (pszName[5] >= '0' && pszName[5] <= '9')) {
                    return ResClass_Texture_Minimap;
                }

                if (nLen > 4 &&
                    pszName[2] == '_'                        &&
                    pszName[nLen - 2] == '_'                 &&
                    (pszName[0] == 'P' || pszName[0] == 'p') &&
                    (pszName[1] == 'O' || pszName[1] == 'o') &&
                    (pszName[nLen - 1] == 't' ||
                     pszName[nLen - 1] == 'T' ||
                     pszName[nLen - 1] == 's' ||
                     pszName[nLen - 1] == 'S' ||
                     pszName[nLen - 1] == 'm' ||
                     pszName[nLen - 1] == 'M' ||
                     pszName[nLen - 1] == 'l' ||
                     pszName[nLen - 1] == 'L' ||
                     pszName[nLen - 1] == 'h' ||
                     pszName[nLen - 1] == 'H'))
                    return ResClass_Texture_Portrait;

                if (pszName[0] == 'I' || pszName[0] == 'i') {
                    if (nLen > 5 && pszName[nLen - 4] == '_' &&
                        (pszName[nLen - 1] >= '0' && pszName[nLen - 1] <= '9') &&
                        (pszName[nLen - 2] >= '0' && pszName[nLen - 2] <= '9') &&
                        (pszName[nLen - 3] >= '0' && pszName[nLen - 3] <= '9'))
                        return ResClass_Texture_Icon_Inv;

                    if (nLen > 4 && pszName[3] == '_') {
                        if ((pszName[1] == 'E' || pszName[1] == 'e') &&
                            (pszName[2] == 'F' || pszName[2] == 'f'))
                            return ResClass_Texture_Icon_Effect;

                        if ((pszName[1] == 'F' || pszName[1] == 'f') &&
                            (pszName[2] == 'E' || pszName[2] == 'e'))
                            return ResClass_Texture_Icon_Feat;

                        if ((pszName[1] == 'S' || pszName[1] == 's') &&
                            (pszName[2] == 'K' || pszName[2] == 'k'))
                            return ResClass_Texture_Icon_Skill;

                        if ((pszName[1] == 'S' || pszName[1] == 's') &&
                            (pszName[2] == 'S' || pszName[2] == 's'))
                            return ResClass_Texture_Icon_Scroll;
                    }

                    if (nLen > 3 && pszName[2] == '_') {
                        if (pszName[1] == 'D' || pszName[1] == 'd')
                            return ResClass_Texture_Icon_Domain;

                        if (pszName[1] == 'R' || pszName[1] == 'r')
                            return ResClass_Texture_Icon_Other;

                        if (pszName[1] == 'S' || pszName[1] == 's')
                            return ResClass_Texture_Icon_Spell;
                    }
                }
            }

            return (nType == NwnResType_DDS) ? ResClass_Texture_DDS : ResClass_Texture;
        }

        case NwnResType_MDL: {
            int nCheck;
            char szCheck[17];

            if (pszName != NULL                   &&
                strlen(pszName) < sizeof(szCheck) && 
                sscanf(pszName, "%[Pp]%[MmFf]%*c%d_%[A-Za-z]%d", szCheck, szCheck, &nCheck, szCheck, &nCheck) >= 5)
                return ResClass_Model_Pheno;

            return ResClass_Model;
        }

        case 0x270C: return ResClass_Ini;
        case 0x000A: return ResClass_Ini;
        default: return ResClass_Misc;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the NWN directory from the registry
//
// @rdesc Directory
//
//-----------------------------------------------------------------------------

CString GetNwnDirectory ()
{
    CString str;

    //
    // Open the NWN registry key
    //

    CRegKey rkParent;
    LONG lRet = rkParent .Open (HKEY_LOCAL_MACHINE, 
                                _T ("SOFTWARE\\BioWare\\NWN\\Neverwinter"),
                                KEY_READ);

    //
    // Read the key
    //

    if (lRet == ERROR_SUCCESS)
    {
        TCHAR szText [_MAX_PATH];
        DWORD dwLen = _MAX_PATH;
        lRet = rkParent .QueryStringValue (_T ("Location"), szText, &dwLen);
        if (lRet == ERROR_SUCCESS)
        {
            str = szText;
            str += _T ("\\");
        }
    }
    return str;
}

//-----------------------------------------------------------------------------
//
// @func Read a single pixle from the TGA image
//
// @parm int | bpp | Bits per pixel
//
// @parm unsigned char * & | pauchData | Image data
//
// @parm D3DCOLOR * | pclrPalette | Color palette
//
// @rdesc Color in D3DCOLOR format
//
//-----------------------------------------------------------------------------

D3DCOLOR TgaReadPixel (int bpp, unsigned char *&pauchData,
                       D3DCOLOR *pclrPalette)
{
    switch (bpp)
    {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            return pclrPalette [*pauchData++];

        case 15:
            {
                UINT16 pixel = *((UINT16 *) pauchData);
                pauchData += sizeof (UINT16);
                return RGBA_MAKE ((pixel >> 7) & 0x0F8, 
                                  (pixel >> 2) & 0xF8, (pixel << 3) & 0xF8, 0xFF);
            }

        case 16:
            {
                UINT16 pixel = *((UINT16 *) pauchData);
                pauchData += sizeof (UINT16);
                return RGBA_MAKE ((pixel >> 7) & 0x0F8, 
                                  (pixel >> 2) & 0xF8, (pixel << 3) & 0xF8, 0xFF);
            }

        case 24:
            {
                D3DCOLOR clr = RGBA_MAKE (pauchData [0], pauchData [1], 
                                          pauchData [2], 0xFF);
                pauchData += 3;
                return clr;
            }

        case 32:
            {
                D3DCOLOR clr = RGBA_MAKE (pauchData [0], pauchData [1], 
                                          pauchData [2], pauchData [3]);
                pauchData += 4;
                return clr;
            }

        default:
            return RGBA_MAKE (0, 0, 0, 0xFF);
    }
}

//-----------------------------------------------------------------------------
//
// @func Read a line of uncompress TGA image
//
// @parm D3DCOLOR * | pout | Output data
//
// @parm int | nWidth | Width of the data
//
// @parm int | bpp | Bits per pixel
//
// @parm unsigned char * & | pauchData | Image data
//
// @parm D3DCOLOR * | pclrPalette | Color palette
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void TgaExpandUncompressedLine (D3DCOLOR *&pout, int nWidth, 
                                int bpp, unsigned char *&pauchData,
                                D3DCOLOR *pclrPalette)
{
    for (int x = 0; x < nWidth; x++)
    {
        *pout++ = TgaReadPixel (bpp, pauchData, pclrPalette);
    }
}

//-----------------------------------------------------------------------------
//
// @func Read a line of compress TGA image
//
// @parm D3DCOLOR * | pout | Output data
//
// @parm int | bpp | Bits per pixel
//
// @parm unsigned char * & | pauchData | Image data
//
// @parm D3DCOLOR * | pclrPalette | Color palette
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void TgaExpandCompressedData (D3DCOLOR *&pout, int bpp,
                              unsigned char *&pauchData, D3DCOLOR *pclrPalette,
                              D3DCOLOR *poutEnd)
{
    int nCount = *pauchData++;

    if (nCount >= 128)
    {
        nCount -= 127; // Calculate real repeat count.
        D3DCOLOR clr = TgaReadPixel (bpp, pauchData, pclrPalette);
        for (int i = 0; i < nCount && pout < poutEnd; i++)
            *pout++ = clr;
    }
    else
    {
        nCount++;
        for (int i = 0; i < nCount && pout < poutEnd; i++)
        {
            *pout++ = TgaReadPixel (bpp, pauchData, pclrPalette);
        }
    }
}

//-----------------------------------------------------------------------------
//
// @func Load a TGA image
//
// @parm unsigned char * | pauchData | Image data
//
// @parm int * | pnWidth | Width of the image
//
// @parm int * | pnHeight | Height of the image
//
// @rdesc Address of the ARGB data.
//
//-----------------------------------------------------------------------------

D3DCOLOR *TgaLoad (unsigned char *pauchData, int *pnWidth, int *pnHeight)
{
    D3DCOLOR clrPalette [256];

    //
    // Get the image information
    //

    TGAHEADER *ptga = (TGAHEADER *) pauchData;
    int nWidth = ptga ->ImageWidth;
    int nHeight = ptga ->ImageHeight;
    int bpp = ptga ->PixelDepth;
    bool fXReversed = ((ptga ->ImagDesc & 16) == 16);
    bool fYReversed = ((ptga ->ImagDesc & 32) == 32);

    //
    // Allocate the data
    //

    D3DCOLOR *pDstBits = new D3DCOLOR [nWidth * nHeight];
    if (pDstBits == NULL)
        return NULL;

    //
    // Skip the id information
    //

    pauchData += sizeof (TGAHEADER) + ptga ->IdLength;

    //
    // If we have a palette
    //

    if (ptga ->CmapType != 0)
    {
        for (int i = 0; i < ptga ->CmapLength; i++)
        {
            clrPalette [i] = TgaReadPixel (ptga ->CmapEntrySize, 
                                           pauchData, NULL);
        }
    }

    //
    // Get the compression flag
    //

    bool fCompressed;
    switch (ptga ->ImageType)
    {
        case TGA_Map:
        case TGA_RGB:
        case TGA_Mono:
            fCompressed = false;
            break;
        case TGA_RLEMap:
        case TGA_RLERGB:
        case TGA_RLEMono:
            fCompressed = true;
            break;
        default:
            delete [] pDstBits;
            return NULL;
    }

    //
    // Loop through the lines
    //

    D3DCOLOR *pout = pDstBits;
    D3DCOLOR *poutEnd = (D3DCOLOR *) pDstBits + (nHeight * nWidth);

    while (pout < poutEnd)
    {
        if (fCompressed)
            TgaExpandCompressedData (pout, bpp, pauchData, clrPalette, poutEnd);
        else
            TgaExpandUncompressedLine (pout, nWidth, bpp, pauchData, clrPalette);
    }

    if (fYReversed) {
        D3DCOLOR *pFlipBits = new D3DCOLOR [nWidth * nHeight];

        for (int y = 0; y < nHeight; y++) {
            memcpy(&(pFlipBits[y * nWidth]),
                   &(pDstBits[(nHeight - y - 1) * nWidth]),
                   (nWidth * sizeof(D3DCOLOR)));
        }

        delete[] pDstBits;
        pDstBits = pFlipBits;
    }

    //
    // Save the width and height
    //

    if (pnWidth)
        *pnWidth = ptga ->ImageWidth;
    if (pnHeight)
        *pnHeight = ptga ->ImageHeight;
    return pDstBits;
}

//-----------------------------------------------------------------------------
//
// @func Load a TGA image
//
// @parm UINT | nID | Resource ID
//
// @parm int * | pnWidth | Width of the image
//
// @parm int * | pnHeight | Height of the image
//
// @rdesc Address of the ARGB data.
//
//-----------------------------------------------------------------------------

D3DCOLOR *TgaLoad (UINT nID, int *pnWidth, int *pnHeight)
{
    //
    // Find the template
    //

    HRSRC hRsrc = FindResource (_Module .GetResourceInstance (),
                                MAKEINTRESOURCE (nID), _T ("TGA_INCLUDE"));
    if (hRsrc == NULL)
        return NULL;

    //
    // Write the resource
    //

    DWORD dwSize = ::SizeofResource (_Module .GetResourceInstance (), hRsrc);
    HGLOBAL hSource = ::LoadResource (_Module .GetResourceInstance (), hRsrc);
    LPVOID pSource = ::LockResource (hSource);
    D3DCOLOR *pTga = TgaLoad ((unsigned char *) pSource, pnWidth, pnHeight);
    UnlockResource (hSource);
    ::FreeResource (hSource);
    return pTga;
}

//-----------------------------------------------------------------------------
//
// @func Load a bitmap from a TGA or DDS image.
//
// @parm D3DCOLOR * | pData | Image data
//
// @parm int | nWidth | Image width
//
// @parm int | nHeight | Image height
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

HBITMAP HBitmapLoad (D3DCOLOR *pData, int nWidth, int nHeight)
{
    extern DWORD g_nViewImagesAlphaBehavior;
    extern DWORD g_nViewImagesAlphaTint;

    //
    // Initialize a DIB
    //

    BITMAPINFOHEADER BMI;
    BMI .biSize = sizeof (BITMAPINFOHEADER);
    BMI .biWidth = nWidth;
    BMI .biHeight = nHeight;
    BMI .biPlanes = 1;
    BMI .biBitCount = 32;
    BMI .biCompression = BI_RGB;
    BMI .biSizeImage = 0;
    BMI .biXPelsPerMeter = 0;
    BMI .biYPelsPerMeter = 0;
    BMI .biClrUsed = 0;
    BMI .biClrImportant = 0;

    //
    // Create DIB section in shared memory
    //

    COLORREF *pDstBits;
    CClientDC dc (::GetDesktopWindow ());
    HBITMAP hbm = ::CreateDIBSection (dc, (BITMAPINFO *)&BMI,
                                      DIB_RGB_COLORS, (void **) &pDstBits, 0, 0l);
    if (hbm == NULL)
        return NULL;

    //
    // Convert 
    //

    if (g_nViewImagesAlphaBehavior == 2)
    {
        //
        // Note that DIB colors are in reverse order
        //
        int ab = (g_nViewImagesAlphaTint >> 16) & 0xFF;
        int ag = (g_nViewImagesAlphaTint >>  8) & 0xFF;
        int ar = (g_nViewImagesAlphaTint >>  0) & 0xFF;

        for (int i = 0; i < nWidth * nHeight; i++)
        {
            if (RGBA_GETALPHA (pData [i]) == 0)
                pDstBits [i] = RGB (ar, ag, ab);
            else
                pDstBits [i] = RGB (
                    RGBA_GETRED (pData [i]),
                    RGBA_GETGREEN (pData [i]),
                    RGBA_GETBLUE (pData [i]));
        }
    }
    else if (g_nViewImagesAlphaBehavior == 1)
    {
        int r, g, b, a, ar, ag, ab, tint;

        //
        // Note that DIB colors are in reverse order
        //
        ab = (g_nViewImagesAlphaTint >> 16) & 0xFF;
        ag = (g_nViewImagesAlphaTint >>  8) & 0xFF;
        ar = (g_nViewImagesAlphaTint >>  0) & 0xFF;

        for (int i = 0; i < nWidth * nHeight; i++)
        {
            r = RGBA_GETRED (pData[i]);
            g = RGBA_GETGREEN (pData[i]);
            b = RGBA_GETBLUE (pData[i]);
            a = 255 - RGBA_GETALPHA (pData[i]);

            if (a > 0) {
                if (ar > 0) {
                    tint = ar - r;

                    if ((r += (tint * a) / 255) > 255)
                        r = 255;
                    else if (r < 0)
                        r = 0;
                }

                if (ag > 0) {
                    tint = ag - g;

                    if ((g += (tint * a) / 255) > 255)
                        g = 255;
                    else if (g < 0)
                        g = 0;
                }

                if (ab > 0) {
                    tint = ab - b;

                    if ((b += (tint * a) / 255) > 255)
                        b = 255;
                    else if (b < 0)
                        b = 0;
                }
            }

            pDstBits [i] = RGB(r, g, b);
        }
    }
    else
    {
        for (int i = 0; i < nWidth * nHeight; i++)
        {
            pDstBits [i] = RGB (
                RGBA_GETRED (pData [i]),
                RGBA_GETGREEN (pData [i]),
                RGBA_GETBLUE (pData [i]));
        }
    }

    delete [] pData;
    return hbm;
}

//-----------------------------------------------------------------------------
//
// @func Load a TGA image
//
// @parm unsigned char * | pauchData | Image data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

HBITMAP TgaLoad (unsigned char *pauchData)
{

    //
    // Load the image
    //

    int nWidth, nHeight;
    D3DCOLOR *pData = TgaLoad (pauchData, &nWidth, &nHeight);
    if (pData == NULL)
        return NULL;

    return HBitmapLoad(pData, nWidth, nHeight);
}

//-----------------------------------------------------------------------------
//
// @func Load a DDS image
//
// @parm unsigned char * | pauchData | Image data
//
// @parm int * | pnWidth | Width of the image
//
// @parm int * | pnHeight | Height of the image
//
// @rdesc Address of the ARGB data.
//
//-----------------------------------------------------------------------------

static __inline int DdsRound (double d)
{
    int ret = (int)(d + 0.500001);

    if (ret < 0)
        return 0;
    else if (ret > 255)
        return 255;

    return ret;
}

static __inline WORD DdsWordSwap (WORD w)
{
    unsigned char *p = (unsigned char *)&w;

    return (p[0] | (p[1] << 8));
}

static void DecompressDdsDXT3AlphaBlock (unsigned char *pauchData, D3DCOLOR *pDstBits, int x, int y, int width) {
    int i, j;
    unsigned char c = 0;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (j % 2)
                c >>= 4;
            else
                c = *(pauchData++);

            pDstBits[((y + i) * width) + (x + j)] = 
                RGBA_SETALPHA(pDstBits[((y + i) * width) + (x + j)],
                              DdsRound((double)(c & 0x0F) * 0xFF / 0x0F));
        }
    }
}

static void DecompressDdsDXT5AlphaBlock (unsigned char *pauchData, D3DCOLOR *pDstBits, int x, int y, int width) {
    unsigned char a[8];
    unsigned char alphaIndex;
    int bitPos;
    int i, j;

    a[0] = *pauchData;
    a[1] = *(pauchData+1);

    if (*pauchData > *(pauchData+1)) {
        a[2] = DdsRound(6.0/7*a[0] + 1.0/7*a[1]);
        a[3] = DdsRound(5.0/7*a[0] + 2.0/7*a[1]);
        a[4] = DdsRound(4.0/7*a[0] + 3.0/7*a[1]);
        a[5] = DdsRound(3.0/7*a[0] + 4.0/7*a[1]);
        a[6] = DdsRound(2.0/7*a[0] + 5.0/7*a[1]);
        a[7] = DdsRound(1.0/7*a[0] + 6.0/7*a[1]);
    } else {
        a[2] = DdsRound(4.0/5*a[1] + 1.0/5*a[0]);
        a[3] = DdsRound(3.0/5*a[1] + 2.0/5*a[0]);
        a[4] = DdsRound(2.0/5*a[1] + 3.0/5*a[0]);
        a[5] = DdsRound(1.0/5*a[1] + 4.0/5*a[0]);
        a[6] = 0x00;
        a[7] = 0xFF;
    }

    pauchData += 2;
    bitPos = 0;

#define DDS_GET_BIT(X) (((*(pauchData+(X)/8)) >> ((X)%8)) & 0x01)
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            alphaIndex = (DDS_GET_BIT(bitPos+2) << 2) |
                (DDS_GET_BIT(bitPos+1) << 1) |
                DDS_GET_BIT(bitPos);

            pDstBits[((y + i) * width) + (x + j)] = 
                RGBA_SETALPHA(pDstBits[((y + i) * width) + (x + j)], a[alphaIndex]);

            bitPos += 3;
        }
    }
#undef DDS_GET_BIT
}

static void DecompressDdsColorBlock (unsigned char *pauchData, D3DCOLOR *pDstBits, int x, int y, int width, int colors) {
    int i, j, oneBitTrans;
    unsigned char colorIndex;
    WORD w, word1, word2, *pData = (WORD *)pauchData;

    struct DDS_COLOR {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    } c[4];
    const unsigned char *alphas;

    word1 = (*pData);
    word2 = (*(pData + 1));
    pData += 2;

    if (word1 > word2){
        oneBitTrans = 0;
    } else {
        if (colors == 3)
            oneBitTrans = 1;
        else
            oneBitTrans = 0;
    }

    if (oneBitTrans)
        alphas = (const unsigned char *)"\xff\xff\xff\x00";
    else
        alphas = (const unsigned char *)"\xff\xff\xff\xff";

    c[0].r = (word1 >> 11) & 0x1F;
    c[0].r = DdsRound((double)c[0].r*0xFF/0x1F);
    c[0].g = (word1 >> 5) & 0x3F;
    c[0].g = DdsRound((double)c[0].g*0xFF/0x3F);
    c[0].b = (word1) & 0x1F;
    c[0].b = DdsRound((double)c[0].b*0xFF/0x1F);

    c[1].r = (word2 >> 11) & 0x1F;
    c[1].r = DdsRound((double)c[1].r*0xFF/0x1F);
    c[1].g = (word2 >> 5) & 0x3F;
    c[1].g = DdsRound((double)c[1].g*0xFF/0x3F);
    c[1].b = (word2) & 0x1F;
    c[1].b = DdsRound((double)c[1].b*0xFF/0x1F);

    if (oneBitTrans) {
        c[2].r = DdsRound(0.5*c[0].r + 0.5*c[1].r);
        c[2].g = DdsRound(0.5*c[0].g + 0.5*c[1].g);
        c[2].b = DdsRound(0.5*c[0].b + 0.5*c[1].b);

        c[3].r = 0;
        c[3].g = 0;
        c[3].b = 0;
    } else {
        c[2].r = DdsRound(2.0/3*c[0].r + 1.0/3*c[1].r);
        c[2].g = DdsRound(2.0/3*c[0].g + 1.0/3*c[1].g);
        c[2].b = DdsRound(2.0/3*c[0].b + 1.0/3*c[1].b);

        c[3].r = DdsRound(1.0/3*c[0].r + 2.0/3*c[1].r);
        c[3].g = DdsRound(1.0/3*c[0].g + 2.0/3*c[1].g);
        c[3].b = DdsRound(1.0/3*c[0].b + 2.0/3*c[1].b);
    }

    w = (*pData++);

    if (colors == 3) {
        for (i = 0; i < 4; i++) {
            if (i == 2)
                w = (*pData++);

            for (j = 0; j < 4; j++) {
                colorIndex = w & 0x03;

                pDstBits[((y + i) * width) + (x + j)] = RGBA_MAKE(c[colorIndex].b, c[colorIndex].g, c[colorIndex].r, alphas[colorIndex]);

                w >>= 2;
            }
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (i == 2)
                w = (*pData++);

            for (j = 0; j < 4; j++) {
                colorIndex = w & 0x03;

                pDstBits[((y + i) * width) + (x + j)] = RGBA_MAKE(c[colorIndex].b, c[colorIndex].g, c[colorIndex].r,
                                                                  RGBA_GETALPHA(pDstBits[((y + i) * width) + (x + j)]));
                w >>= 2;
            }
        }
    }
}

D3DCOLOR *DdsLoad (unsigned char *pauchData, int *pnWidth, int *pnHeight)
{
    typedef struct {
        DWORD width;
        DWORD height;
        DWORD colors;
        DWORD reserved1;
        DWORD reserved2;
    } DDS_HEADER;

    //
    // Get the image information
    //

    DDS_HEADER *pdds = (DDS_HEADER *) pauchData;
    if (pdds->colors != 3 && pdds->colors != 4)
        return NULL;

    //
    // Allocate the data
    //

    D3DCOLOR *pDstBits = new D3DCOLOR [pdds->width * pdds->height];
    if (pDstBits == NULL)
        return NULL;

    //
    // Skip the id information
    //

    pauchData += sizeof (DDS_HEADER);


    int x = 0, y = 0;

    while (y < pdds->height) {
        if (0) {
            DecompressDdsDXT3AlphaBlock(pauchData, pDstBits, x, y, pdds->width);
            pauchData += 8;
        } else if (pdds->colors == 4) {
            DecompressDdsDXT5AlphaBlock(pauchData, pDstBits, x, y, pdds->width);
            pauchData += 8;
        }

        DecompressDdsColorBlock(pauchData, pDstBits, x, y, pdds->width, pdds->colors);
        pauchData += 8;

        x += 4;
        if (x >= pdds->width) {
            x  = 0;
            y += 4;
        }
    }

    if (pnWidth)
        *pnWidth = pdds ->width;
    if (pnHeight)
        *pnHeight = pdds ->height;
    return pDstBits;
}

//-----------------------------------------------------------------------------
//
// @func Load a DDS image
//
// @parm unsigned char * | pauchData | Image data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

HBITMAP DdsLoad (unsigned char *pauchData)
{
    //
    // Load the image
    //

    int nWidth, nHeight;
    D3DCOLOR *pData = DdsLoad (pauchData, &nWidth, &nHeight);
    if (pData == NULL)
        return NULL;

    return HBitmapLoad(pData, nWidth, nHeight);
}


//-----------------------------------------------------------------------------
//
// @func Write a TGA image
//
// @parm DataElement * | pElement | Data element
//
// @parm const char * | pszFileName | File name to write to
//
// @parm NwnResType | nResType | Element resource type
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

int TgaWrite (DataElement *pElement, const char *pszFileName, NwnResType nResType) {
    FILE *fp[10];
    int i, nWidth, nHeight, nFile = 0, nFiles = 1, transparent = 0;
    CData *psRaw = pElement ->pDataSource ->LoadRes(pElement);
    D3DCOLOR *pData = NULL;
    CResTexturePltRef plt;

    extern BOOL g_fViewImagesExportColoredPlt;

    if (nResType != NwnResType_DDS &&
        nResType != NwnResType_PLT &&
        nResType != NwnResType_TGA &&
        nResType != NwnResType_MDL)
        return false;

    if (psRaw != NULL) {
        if (nResType == NwnResType_PLT && !g_fViewImagesExportColoredPlt) {
            plt. Set (psRaw);
            pData   = (D3DCOLOR *)1;
            nWidth  = plt .GetWidth();
            nHeight = plt .GetHeight();
        } else if (nResType == NwnResType_MDL) {
            extern CMainWnd *g_pMainWnd;
            extern CModelWnd g_wndModel;

            g_pMainWnd ->OpenResource(false, pElement);
            pData = g_wndModel .GetImage (&nWidth, &nHeight);
        } else if (nResType == NwnResType_TGA) {
            pData = TgaLoad (psRaw ->GetData (0), &nWidth, &nHeight);
        } else {
            pData = DdsLoad (psRaw ->GetData (0), &nWidth, &nHeight);
        }

        psRaw ->AddRef ();
        psRaw ->Release ();
    }

    if (pData == NULL)
        return false;

    if (nResType == NwnResType_PLT) {
        if (g_fViewImagesExportColoredPlt) {
            fp[0] = fopen(pszFileName, "wb");
            if (fp[0] == NULL) {
                delete[] pData;
                return false;
            }

            transparent = 1;
        } else {
            char szPath[_MAX_PATH], szEnd[8], *p;
            nFiles = 10;

            for (i = 0; i < nFiles; i++) {
                strncpy(szPath, pszFileName, _countof(szPath) - 10);
                if ((p = strrchr(szPath, '.')) != NULL)
                    *p = '\0';

                sprintf(szEnd, "_%d.tga", i);
                strcat(szPath, szEnd);

                fp[i] = fopen(szPath, "wb");

                if (fp[i] == NULL) {
                    for (i = 0; i < nFiles; i++) {
                        if (fp[i] != NULL)
                            fclose(fp[i]);
                    }

                    return false;
                }
            }
        }
    } else {
        fp[0] = fopen(pszFileName, "wb");
        if (fp[0] == NULL) {
            delete[] pData;
            return false;
        }

        /* Check the image for opacity */
        size_t nImageSize = nWidth * nHeight;

        for (i = 0; i < nImageSize; i++) {
            if (RGBA_GETALPHA(pData[i]) < 254) {
                transparent = 1;
                break;
            }
        }
    }

    int x, y, p;
    WORD ui16;

    /* write tga file header:
     *   ID Length: 0 
     *   Color Map Type: no color-map
     *   Image Type: Uncompressed, True-color
     *   Color Map Specification: no color-map
     *   X-origin of Image: 0
     *   Y-origin of Image: 0
     */
    for (i = 0; i < nFiles; i++) {
        fwrite("\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00", 1, 12, fp[i]);

        ui16 = nWidth; /* Image Width */
        fwrite(&ui16, 1, sizeof(ui16), fp[i]);

        ui16 = nHeight; /* Image Height */
        fwrite(&ui16, 1, sizeof(ui16), fp[i]);

        /* Pixel Depth: 32/24
         * Alpha Image Descriptor: 8/0 */
        if (transparent)
            fwrite("\x20\x28", 1, 2, fp[i]);
        else
            fwrite("\x18\x20", 1, 2, fp[i]);
    }

    /* write the image */
    if (nResType == NwnResType_PLT) {
        unsigned char c[4];
        const CResTexturePltRef::Pixel *pixels = plt .GetPixelData ();

        if (g_fViewImagesExportColoredPlt) {
            for (y = nHeight - 1; y >= 0; y--) {
                for (x = 0; x < nWidth; x++) {
                    p = (y * nWidth) + x;

#if 0
                    D3DCOLOR *pPal = g_apPalettes [p [i] .ucPalette];

                    if (pPal == NULL) {
                        c[0] = 0;
                        c[1] = 0;
                        c[2] = 0;
                        c[3] = 0;
                    } else {
                        pPal += (255 - panPalette [p [i] .ucPalette]) * 256;
                        D3DCOLOR clr = pPal [p [i] .ucIndex];
                        c[0] = RGBA_GETRED (clr);
                        c[1] = RGBA_GETGREEN (clr);
                        c[2] = RGBA_GETBLUE (clr);
                        c[3] = RGBA_GETALPHA (clr);
                    }
#else
                    c[0] = 0;
                    c[1] = 0;
                    c[2] = 0;
                    c[3] = 0;
#endif

                    fwrite(c, 1, 4, fp[nFile]);
                }
            }
        } else {
            for (y = nHeight - 1; y >= 0; y--) {
                for (x = 0; x < nWidth; x++) {
                    p = (y * nWidth) + x;

                    c[0]  = pixels[p]. ucIndex;
                    c[1]  = pixels[p]. ucIndex;
                    c[2]  = pixels[p]. ucIndex;
                    nFile = pixels[p]. ucPalette;

                    fwrite(c, 1, 3, fp[nFile]);

                    c[0] = 0;
                    c[1] = 255;
                    c[2] = 0;

                    for (i = 0; i < nFiles; i++) {
                        if (i != nFile)
                            fwrite(c, 1, 3, fp[i]);
                    }
                }
            }
        }
    } else if (transparent) {
        unsigned char c[4];

        for (y = nHeight - 1; y >= 0; y--) {
            for (x = 0; x < nWidth; x++) {
                p = (y * nWidth) + x;

                c[0] = RGBA_GETRED(pData[p]);
                c[1] = RGBA_GETGREEN(pData[p]);
                c[2] = RGBA_GETBLUE(pData[p]);
                c[3] = RGBA_GETALPHA(pData[p]);

                fwrite(c, 1, 4, fp[nFile]);
            }
        }
    } else {
        unsigned char c[3];

        for (y = nHeight - 1; y >= 0; y--) {
            for (x = 0; x < nWidth; x++) {
                p = (y * nWidth) + x;

                c[0] = RGBA_GETRED(pData[p]);
                c[1] = RGBA_GETGREEN(pData[p]);
                c[2] = RGBA_GETBLUE(pData[p]);

                fwrite(c, 1, 3, fp[nFile]);
            }
        }
    }

    for (i = 0; i < nFiles; i++) {
        fclose(fp[i]);
    }

    if (nResType != NwnResType_PLT)
        delete[] pData;

    return true;
}

/* vim: set sw=4: */
