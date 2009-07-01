//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      nwntreasure.cpp - Main module |
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
#include "nwntreasure.h"
#include "mainwnd.h"
#include <direct.h>

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
TCHAR g_szAppPath [_MAX_PATH];
TCHAR g_szWorkingPath [_MAX_PATH];
TCHAR g_szSettingsFile [_MAX_PATH];

HWND g_hWndMain;
CPrinter g_sPrinter;
CDevMode g_sDevMode;
CImageList g_ilSmall;
CImageList g_ilLarge;


//
// Constant file names
//

LPCTSTR g_pszTreasureXMLName = _T ( "treasure.xml");
LPCTSTR g_pszIniFile = _T ("nwntreasure.ini");
LPCTSTR g_pszKeyRoot = _T ("Software\\Torlack\\nwntreasure");

//
// Toolbar command list
//

TB_Command g_asCommands [] =
{
    { ID_FILE_NEW,                                      TBSTYLE_BUTTON, 1 },
    { ID_FILE_OPEN,                                     TBSTYLE_BUTTON, 0 },
    { ID_FILE_SAVE,                                     TBSTYLE_BUTTON, 4 },
    { ID_EDIT_NEW,                                      TBSTYLE_BUTTON, 7 },
    { ID_EDIT_NEW_AFTER,                        TBSTYLE_BUTTON, 9 },
    { ID_EDIT_NEW_TABLE,                        TBSTYLE_BUTTON, 6 },
    { ID_EDIT_PROPERTIES,                       TBSTYLE_BUTTON, 5 },
    { ID_EDIT_TABLEPROPERTIES,          TBSTYLE_BUTTON, 8 },
    { ID_EDIT_MOVEUP,                           TBSTYLE_BUTTON, 10 },
    { ID_EDIT_MOVEDOWN,                         TBSTYLE_BUTTON, 3 },
    { ID_EDIT_DELETE,                           TBSTYLE_BUTTON, 2 },
    { ID_APP_ABOUT,                 TBSTYLE_BUTTON, 11 },
};
int g_nCommands = _countof (g_asCommands);

//
// Bioware standard tables
//

BiowareTables g_asBiowareTables [] = 
{
    { "TREASURE_LOW",                   "nw_low",               1, },
    { "TREASURE_MEDIUM",                "nw_med",               2, },
    { "TREASURE_HIGH",                  "nw_high",              3, },
    { "TREASURE_BOSS",                  "nw_boss",              4, },
    { "TREASURE_BOOK",                  "nw_book",              5, },
    { "TREASURE_FEAT",                  "nw_feat",              11, },
    { "TREASURE_NPC_LOW",               "nw_npc_low",   12, },
    { "TREASURE_NPC_MEDIUM",    "nw_npc_med",   13, },
    { "TREASURE_NPC_HIGH",              "nw_npc_high",  14, },
    { "TREASURE_NPC_BOSS",              "nw_npc_boss",  15, },
    { "TREASURE_NONE",                  "nw_none",              16, },
};
int g_nBiowareTables = _countof (g_asBiowareTables);

//
// ATL module
//

CAppModule _Module;
    BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

    //
    // Local routines
    //

    void LocateIniFile (LPCTSTR pszIniFile);

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
                  IDR_NWNTREASURE, g_szAppName, _countof (g_szAppName));

    //
    // Locate the ini file
    //

    LocateIniFile (g_pszIniFile);

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

    CMessageLoop theLoop;
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

    //
    // Pump messages
    //

    theLoop .Run ();
    _Module .RemoveMessageLoop ();

    //
    // Close up printer (Not required, but keeps Purify happy)
    //

    g_sPrinter .ClosePrinter ();
    g_sDevMode .Detach ();

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
// @func Locate the ini file and generate the directory names
//
// @parm LPCTSTR | pszIniFile | Name of the ini file
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void LocateIniFile (LPCTSTR pszIniFile)
{
    TCHAR szAppName [_MAX_PATH];
    TCHAR szBuff [_MAX_PATH];

    //
    // Get the module name
    //

    ::GetModuleFileName (NULL, szBuff, _MAX_PATH);

    //
    // Now, locate the end of the path string
    //

    LPTSTR pszTemp = szBuff;
    for (LPTSTR psz = szBuff; *psz; psz = _tcsinc (psz)) 
    {
        if (*psz == '\\' || *psz == '/' || *psz == ':')
            pszTemp = (LPTSTR) _tcsinc (psz);
    }
    int c = *pszTemp;
    *pszTemp = 0;
    _tcsncpy (g_szAppPath, szBuff, _countof (g_szAppPath));

    //
    // Remove extension
    //

    *pszTemp = c;
    LPTSTR pszExt = _tcsrchr (pszTemp, '.');
    _ASSERTE (pszExt != NULL);
    _ASSERTE (*pszExt == '.');
    *pszExt = 0;
    _tcsncpy (szAppName, pszTemp, _countof (szAppName));

    //
    // Get the current working directory
    //

    _tgetcwd (szBuff, _countof (szBuff));
    _tcscat (szBuff, _T ( "\\"));

    //
    // First, try the app path
    //

    TCHAR szIni [_MAX_PATH];
    _tcscpy (szIni, g_szAppPath);
    _tcscat (szIni, pszIniFile);
    FILE *fp = fopen (szIni, "r");
    LPCTSTR pszPath = fp ? g_szAppPath : szBuff;
    if (fp != NULL)
        fclose (fp);

    //
    // Generate the paths
    //

    _tcscpy (g_szWorkingPath, pszPath);
    _tcscpy (g_szSettingsFile, pszPath);
    _tcscat (g_szSettingsFile, pszIniFile);
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

/* vim: set sw=4: */
