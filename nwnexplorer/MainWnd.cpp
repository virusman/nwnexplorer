//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      MainWnd.cpp - Main window |
//
// This module contains the main window.
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
#include "MainWnd.h"
#include "nwnexplorer.h"
#include "../_NwnLib/NwnDialogTlkFile.h"
#include "../_NwnLib/NwnBifFile.h"
#include "../_NwnLib/NwnHierarchy.h"
#include "../_NwnLib/NwnStreams.h"
#include "../_NwnLib/NwnStdLoader.h"
#include "../_NscLib/Nsc.h"
#include "ModelRawView.h"
#include "NwnOptionsPage.h"
#include "DataCache.h"
#include "DataSourceKey.h"
#include "DataSourceMod.h"
#include "DataSourceDir.h"
#include "DataSourceBif.h"
#include <io.h>

//
// Debug NEW
//

#if defined (_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Constants
//

static const char DEFAULT_STEAM_FOLDER[] = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\";

//
// Global variables
//

BOOL g_fViewModelsFullAmbient;
BOOL g_fViewModelsPreferDds;
BOOL g_fViewModelsPreventDds;
BOOL g_fViewModelsPreventTga;
BOOL g_fViewModelsRunEmit;
BOOL g_fViewModelsShowAABB;
BOOL g_fViewModelsShowDummy;
BOOL g_fViewModelsShowEmit;
BOOL g_fViewModelsShowLight;
BOOL g_fViewModelsShowRoot;
BOOL g_fViewModelsMdlNameTex;
BOOL g_fViewModelsOrthographic;
BOOL g_fViewModelsBackfaceCull;
BOOL g_fViewImagesExportColoredPlt;
BOOL g_fViewModelsOutlinePolys;
BOOL g_fViewModelsDrawAxes;
DWORD g_nViewImagesAlphaBehavior;
DWORD g_nViewImagesAlphaTint;
DWORD g_nViewModelsAlphaBehavior;
DWORD g_nViewModelsBackground;
DWORD g_nViewModelsClipDistance;
DWORD g_nViewModelsHideFade;
DWORD g_nViewModelsOrthoViewSz;
DWORD g_nViewModelsViewAngleP;
DWORD g_nViewModelsViewAngleR;
DWORD g_nViewModelsViewAngleY;
DWORD g_nViewModelsViewDistance;
DWORD g_nViewModelsViewportX;
DWORD g_nViewModelsViewportY;
extern TCHAR g_szAppName [];
extern CImageList g_ilSmall;
extern CImageList g_ilLarge;
extern TB_Command g_asCommands [];
extern int g_nCommands;
extern LPCTSTR g_pszKeyRoot;

//
// This is a hacked out routine from WTL's control bar support.  There
// really should be a better way of doing this.
//

BOOL UpdateSimpleReBarBandCtrl(HWND hWndReBar, HWND hWndBand, int nRBIndex, 
                               int cxWidth = 0, BOOL bFullWidthAlways = FALSE)
{
    ATLASSERT(::IsWindow(hWndReBar));   // must be already created
#ifdef _DEBUG
    // block - check if this is really a rebar
    {
        TCHAR lpszClassName[sizeof(REBARCLASSNAME)];
        ::GetClassName(hWndReBar, lpszClassName, sizeof(REBARCLASSNAME));
        ATLASSERT(lstrcmp(lpszClassName, REBARCLASSNAME) == 0);
    }
#endif //_DEBUG
    ATLASSERT(::IsWindow(hWndBand));    // must be already created

    // Get number of buttons on the toolbar
    ::SendMessage (hWndBand, TB_AUTOSIZE, 0, 0);
    int nBtnCount = (int)::SendMessage(hWndBand, TB_BUTTONCOUNT, 0, 0L);

    // Set band info structure
    REBARBANDINFO rbBand;
    rbBand.cbSize = sizeof(REBARBANDINFO);
#if (_WIN32_IE >= 0x0400)
    rbBand.fMask = RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_IDEALSIZE;
#else
    rbBand.fMask = RBBIM_CHILDSIZE | RBBIM_SIZE;
#endif //!(_WIN32_IE >= 0x0400)

    // Calculate the size of the band
    BOOL bRet;
    RECT rcTmp;
    if(nBtnCount > 0)
    {
        bRet = (BOOL)::SendMessage(hWndBand, TB_GETITEMRECT, nBtnCount - 1, (LPARAM)&rcTmp);
        ATLASSERT(bRet);
        rbBand.cx = (cxWidth != 0) ? cxWidth : rcTmp.right;
        rbBand.cyMinChild = rcTmp.bottom - rcTmp.top;
        if(bFullWidthAlways)
        {
            rbBand.cxMinChild = rbBand.cx;
        }
        //                      else if(lpstrTitle == 0)
        //                      {
        //                              bRet = (BOOL)::SendMessage(hWndBand, TB_GETITEMRECT, 0, (LPARAM)&rcTmp);
        //                              ATLASSERT(bRet);
        //                              rbBand.cxMinChild = rcTmp.right;
        //                      }
        else
        {
            rbBand.cxMinChild = 0;
        }
    }
    else        // no buttons, either not a toolbar or really has no buttons
    {
        bRet = ::GetWindowRect(hWndBand, &rcTmp);
        ATLASSERT(bRet);
        rbBand.cx = (cxWidth != 0) ? cxWidth : (rcTmp.right - rcTmp.left);
        rbBand.cxMinChild = (bFullWidthAlways) ? rbBand.cx : 0;
        rbBand.cyMinChild = rcTmp.bottom - rcTmp.top;
    }

#if (_WIN32_IE >= 0x0400)
    rbBand.cxIdeal = rbBand.cx;
#endif //(_WIN32_IE >= 0x0400)

    // Add the band
    LRESULT lRes = ::SendMessage(hWndReBar, RB_SETBANDINFO, nRBIndex, (LPARAM)&rbBand);
    if(lRes == 0)
    {
        ATLTRACE2(atlTraceUI, 0, _T("Failed to add a band to the rebar.\n"));
        return FALSE;
    }
    ::InvalidateRect (hWndBand, NULL, TRUE);
    return TRUE;
}

LPCTSTR g_pszOpenFilter = 
_T("All Files (*.*)\0*.*\0")
_T("User Created Modules (*.mod)\0*.mod\0")
_T("NeverwinterNights Modules (*.nwm)\0*.nwm\0")
_T("Hak Packs (*.hak)\0*.hak\0")
_T("Saved Games (*.sav)\0*.sav\0")
_T("ERF Files (*.erf)\0*.erf\0")
_T("");

class CAboutDlg : public CDialogImpl<CAboutDlg>
{
    public:
        enum { IDD = IDD_ABOUTBOX };

        BEGIN_MSG_MAP(CAboutDlg)
            MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
            COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
            COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
            END_MSG_MAP()

            LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
            {
                CenterWindow(GetParent());
                return TRUE;
            }

        LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
        {
            EndDialog(wID);
            return 0;
        }
};

//
// My globals
//

CString g_strNwnDirectory;
CString g_strUserDirectory;
CString g_strTexSearchPath;
bool g_bIsNWNEE = false;
CNwnDialogTlkFile g_sDialogTlkFile;
CNwnKeyFile *g_sKeyFiles = new CNwnKeyFile[IDS_TOTAL_KEY_FILES];
CNwnModuleFile *g_sHakFiles = new CNwnModuleFile[IDS_TOTAL_USER_HAKS];
CDataCache g_sResourceCache (g_sKeyFiles, g_sHakFiles, &g_sDialogTlkFile);

CFont g_fontFixed;
CSize g_sizeFixed;
FILE *g_fpDebug = NULL;
CCommandBarCtrl *g_pCommandBar = NULL;
CMainWnd *g_pMainWnd = NULL;

//
// Settings
//

const int g_nMaxSlowDelete = 32;

//-----------------------------------------------------------------------------
//
// @mfunc <c CMainWnd> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CMainWnd::CMainWnd () : m_tv (this, 1)
{
    m_fForceUpdate = FALSE;
    m_fCurrentFileBinary = FALSE;
    m_fTvRedraw = TRUE;
    m_szCurrentFile [0] = '\0';
    g_pCommandBar = &m_wndCommandBar;
    g_pMainWnd = this;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CMainWnd> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CMainWnd::~CMainWnd ()
{
    DeleteDocument ();
    g_fontFixed .DeleteObject ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Create the window
//
// @rdesc Handle of the window
//
//-----------------------------------------------------------------------------

HWND CMainWnd::Create ()
{

    //
    // Load the menu
    //

    HMENU hMenu = ::LoadMenu (_Module .GetResourceInstance (), 
                              MAKEINTRESOURCE (IDR_NWNEXPLORER));

    //
    // Create the window
    //

    DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
    DWORD dwExStyle = 0;
    HWND hWndMain = CFrameWindowImpl <CMainWnd>::Create (
        NULL, rcDefault, g_szAppName, dwStyle, dwExStyle, 
        hMenu);
    if (hWndMain == NULL)
        return hWndMain;

    //
    // Load acceleratiors
    //

    m_hAccel = ::LoadAccelerators (_Module .GetResourceInstance (), 
                                   MAKEINTRESOURCE (GetWndClassInfo() .m_uCommonResourceID));

    //
    // Show the window
    //

    TCHAR szText [256];
    GetSetting (PROFILE_WINPLACEMENT, _T (""), szText, _countof (szText));
    if (_tcslen (szText) > 0) 
    {
        WINDOWPLACEMENT place;
        place .length = sizeof (WINDOWPLACEMENT);
        _stscanf (szText, _T ("%d %d %d %d %d %d %d %d %d %d"),
                  &place .flags, &place .showCmd, &place .ptMinPosition .x,
                  &place .ptMinPosition .y, &place .ptMaxPosition .x,
                  &place .ptMaxPosition .y, &place .rcNormalPosition .left,
                  &place .rcNormalPosition .top, &place .rcNormalPosition .right,
                  &place .rcNormalPosition .bottom);
        if (place .showCmd == SW_SHOWMINIMIZED)
            place .showCmd = SW_SHOWNORMAL;
        ::SetWindowPlacement (hWndMain, &place);
    }
    else
        ::ShowWindow (hWndMain, SW_SHOWNORMAL);

    LoadProfile ();

    //
    // Open NWN
    //

    OpenNWN ();
    return hWndMain;
}

//-----------------------------------------------------------------------------
//
// @mfunc Initialize the window
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnCreate (UINT uMsg, 
                            WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    CString str;

    //
    // Get profile settings
    //

    m_nTextStyle = (TB_TextStyle) GetSetting (
        PROFILE_TBTEXTSTYLE, PROFILE_TBTEXTSTYLE_DEFAULT);
    m_nIconStyle = (TB_IconStyle) GetSetting (
        PROFILE_TBICONSTYLE, PROFILE_TBICONSTYLE_DEFAULT);

    //
    // Create command bar window
    //

    HWND hWndCmdBar = m_wndCommandBar .Create (m_hWnd, rcDefault,
                                               NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);

    //
    // Attach the menu
    //

    m_wndCommandBar .AttachMenu (GetMenu ());

    //
    // Load command bar images
    //

    CSize sizeIcon;
    g_ilSmall .GetIconSize (sizeIcon);
    m_wndCommandBar .SetImageSize (sizeIcon .cx, sizeIcon .cy);
    m_wndCommandBar .m_hImageList = g_ilSmall .Duplicate ();
    for (int i = 0; i < g_ilSmall .GetImageCount (); i++)
    {
        int j;
        for (j = 0; j < g_nCommands; j++)
        {
            if (g_asCommands [j] .iBitmap == i)
                break;
        }
        if (j < g_nCommands)
            m_wndCommandBar .m_arrCommand .Add (g_asCommands [j] .nCmd);
        else
            m_wndCommandBar .m_arrCommand .Add (-1);
    }

    //
    // Remove the old menu
    //

    SetMenu (NULL);

    //
    // Create the toolbar
    //

    m_wndToolBar = ::CreateWindowEx (0, TOOLBARCLASSNAME, NULL, 
                                     ATL_SIMPLE_TOOLBAR_PANE_STYLE, 0, 0, 100, 100, m_hWnd, 
                                     (HMENU) LongToHandle (ATL_IDW_TOOLBAR), _Module .GetModuleInstance (),
                                     NULL);

    //
    // Do extended initialization of the toolbar
    //

    m_wndToolBar .SetButtonStructSize (sizeof (TBBUTTON));
    m_wndToolBar .SetExtendedStyle (TBSTYLE_EX_DRAWDDARROWS);
    m_wndToolBar .SetMaxTextRows (0);

    //
    // Add the toolbar to the list of updated ui elements
    //

    UIAddToolBar (m_wndToolBar);

    //
    // Load all the buttons
    //

    LoadToolbarButtons ();
    UpdateToolbar ();

    //
    // Create the rebar
    //

    CreateSimpleReBar (ATL_SIMPLE_REBAR_NOBORDER_STYLE);
    AddSimpleReBarBand (hWndCmdBar);
    AddSimpleReBarBand (m_wndToolBar, NULL, TRUE);

    //
    // Remove the bars from the bands
    //

    REBARBANDINFO rbi;
    memset (&rbi, 0, sizeof (rbi));
    rbi .cbSize = REBARBANDINFO_V3_SIZE;
    rbi .fMask  = RBBIM_STYLE;
    ::SendMessage (m_hWndToolBar, RB_GETBANDINFO, 0, (LPARAM) &rbi);
    rbi .fStyle |= RBBS_NOGRIPPER;
    ::SendMessage (m_hWndToolBar, RB_SETBANDINFO, 0, (LPARAM) &rbi);
    ::SendMessage (m_hWndToolBar, RB_GETBANDINFO, 1, (LPARAM) &rbi);
    rbi .fStyle |= RBBS_NOGRIPPER;
    ::SendMessage (m_hWndToolBar, RB_SETBANDINFO, 1, (LPARAM) &rbi);

    //
    // Create status bar
    //

    CreateSimpleStatusBar ();

    //
    // Update the toolbars
    //

    UpdateSimpleReBarBandCtrl (m_hWndToolBar, m_wndCommandBar, 0); 
    UpdateSimpleReBarBandCtrl (m_hWndToolBar, m_wndToolBar, 1); 

    //
    // Add the idle handler
    //

    CMessageLoop *pLoop = _Module .GetMessageLoop ();
    pLoop ->AddIdleHandler (this);
    pLoop ->AddMessageFilter (this);

    //
    // Set the size of the splitter
    //

    SetSplitterExtendedStyle (0, SPLIT_PROPORTIONAL);
    SetSplitterPos (250);

    //
    // Create the tree view
    //

    CreateTreeView ();

    //
    // Create the content window
    //

    HWND hWndContent = m_wndContent .Create (m_hWnd, rcDefault, _T (""),
                                             WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 
                                             (HMENU) IDC_EXP_CONTENT, NULL);

    //
    // Set the splitter panes
    //

    SetSplitterPanes (m_tv .m_hWnd, m_wndContent .m_hWnd);
    SetSinglePaneMode (SPLIT_PANE_RIGHT);

    //
    // Initialize the MRU
    //

    {
        HMENU hMenu = m_wndCommandBar .GetMenu();
        HMENU hFileMenu = ::GetSubMenu (hMenu, 0);
        HMENU hMruMenu = ::GetSubMenu (hFileMenu, 
                                       ::GetMenuItemCount (hFileMenu) - 3);
        m_mru .SetMenuHandle (hMruMenu);
        m_mru .ReadFromRegistry (g_pszKeyRoot);
    }

    //
    // Create the fixed font
    //

    {
        //
        // Initialize the font size
        //

        g_sizeFixed .cx = 0;
        g_sizeFixed .cy = 0;

        //
        // Get a screen DC
        //

        CClientDC dc (m_hWnd);

        //
        // Compute the font height
        //

        int lfHeight = - MulDiv (10, dc .GetDeviceCaps (LOGPIXELSY), 72);

        //
        // Create the font
        //

        g_fontFixed .CreateFont (lfHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
                                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T ("Courier"));

        //
        // Get font size
        //

        TEXTMETRIC tm;
        HFONT hfontOld = dc .SelectFont (g_fontFixed);
        dc .GetTextMetrics (&tm);
        g_sizeFixed .cx = tm .tmAveCharWidth;
        g_sizeFixed .cy = (tm .tmHeight + tm .tmExternalLeading);
        dc .SelectFont (hfontOld);
    }

    //
    // Return FALSE
    //

    bHandled = FALSE;
    return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user press app exit
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnAppExit (WORD wNotifyCode, 
                             WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

    //
    // Close the window
    //

    PostMessage (WM_CLOSE, 0, 0);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user press about
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnAppAbout (WORD wNotifyCode, 
                              WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    CAboutDlg dlg;
    dlg .DoModal ();
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle idle processing
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

BOOL CMainWnd::OnIdle ()
{

    //
    // Disable the view closes
    //

    UIEnable (ID_RESOURCES_CLOSEVIEW, FALSE);
    UIEnable (ID_RESOURCES_CLOSEALLVIEWS, FALSE);
    UIEnable (ID_RESOURCES_EXPORTTEXT, FALSE);
    UIEnable (ID_RESOURCES_EXPORTTGA, FALSE);

    //
    // See if we allow view
    //

    bool fEnableView = false;
    bool fEnableExport = false;
    bool fEnableExportTga = false;
    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem != NULL)
    {
        DataElement *pElement = (DataElement *) m_tv .GetItemData (hItem);
        fEnableView = pElement ->pDataSource ->IsResource (pElement);
        fEnableExport = pElement ->pDataSource ->CanExport (pElement);
        fEnableExportTga = pElement ->pDataSource ->CanExportTga (pElement);
    }
    UIEnable (ID_RESOURCES_VIEW, fEnableView);
    UIEnable (ID_RESOURCES_VIEWINNEWWINDOW, fEnableView);
    UIEnable (ID_RESOURCES_EXPORT, fEnableExport);
    UIEnable (ID_RESOURCES_EXPORTTGA, fEnableExportTga);

    //
    // Invoke for the content window
    //

    m_wndContent .OnIdle (this);

    //
    // Update the toolbar
    //

    UIUpdateToolBar (m_fForceUpdate);
    m_fForceUpdate = FALSE;

    //
    // Do slow deletes
    //

    if (m_atvDelete .GetCount ())
    {
        int nCount = 0;
        DeleteTreeView (m_atvDelete [0], TVI_ROOT, nCount);
        if (nCount < g_nMaxSlowDelete)
        {
            ::DestroyWindow (m_atvDelete [0]);
            m_atvDelete .RemoveAt (0);
        }
    }
    return !m_atvDelete .IsEmpty ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle hot item change
//
// @parm int | idCtrl | Id of the control
//
// @parm LPNMHDR | pnmh | Notify message header
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnHotItemChange (int idCtrl, 
                                   LPNMHDR pnmh, BOOL &bHandled) 
{
    //
    // Get the tooltip notify structure
    //

    NMTBHOTITEM *pHIC = (NMTBHOTITEM *) pnmh;

    //
    // Return if this is not the toolbar
    //

    if (pHIC ->hdr .hwndFrom != m_wndToolBar)
        return FALSE;

    //
    // If no control
    //

    if (pHIC ->dwFlags & HICF_LEAVING)
    {
        SetStatusBarText (NULL);
    }

    //
    // If new control
    //

    else if (pHIC ->idNew != 0)
    {
        //
        // Get the control id
        //

        UINT nID = pHIC ->idNew;

        //
        // If disabled, then display nothing
        //

        UINT nState = m_wndToolBar .GetState (nID);
        if ((nState & TBSTATE_ENABLED) == 0)
        {
            SetStatusBarText (NULL);
        }

        //
        // Otherwise
        //

        else
        {
            TCHAR szText [128];
            TCHAR szText2 [128];
            ::LoadString (_Module .GetResourceInstance (), 
                          nID, szText, _countof (szText));
            ExtractString (szText2, _countof (szText2),
                           szText, 0, '\n');
            SetStatusBarText (szText2);
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the status bar text
//
// @parm LPCTSTR | pszText | Text to set or NULL for idle
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMainWnd::SetStatusBarText (LPCTSTR pszText)
{

    //
    // If NULL, display idle
    //

    if (pszText == NULL)
    {
        TCHAR szText [128];
        ::LoadString (_Module .GetResourceInstance (), 
                      ATL_IDS_IDLEMESSAGE, szText, _countof (szText));
        ::SetWindowText (m_hWndStatusBar, szText);
        ::SendMessage (m_hWndStatusBar, SB_SIMPLE, 0, 0);
    }

    //
    // Otherwise
    //

    else
    {
        ::SendMessage (m_hWndStatusBar, SB_SETTEXT, 
                       SBT_NOBORDERS | 255, (LPARAM) pszText);
        ::SendMessage (m_hWndStatusBar, SB_SIMPLE, 1, 0);
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle final window message
//
// @parm HWND | hWnd | Handle of the window
//
// @rdesc None
//
//-----------------------------------------------------------------------------

void CMainWnd::OnFinalMessage (HWND hWnd)
{
    //
    // Remove the idle handler
    //

    CMessageLoop *pLoop = _Module .GetMessageLoop ();
    pLoop ->RemoveIdleHandler (this);
    pLoop ->RemoveMessageFilter (this);
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle window destruction
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnDestroy (UINT uMsg, 
                             WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

    //
    // Mark as not handled so the WCE rundown code gets called
    //

    bHandled = FALSE;

    //
    // Save the window size 
    //

    TCHAR szText [255];
    WINDOWPLACEMENT place;
    place .length = sizeof (place);
    ::GetWindowPlacement (m_hWnd, &place);
    _sntprintf (szText, _countof (szText), 
                _T ( "%d %d %d %d %d %d %d %d %d %d"),
                place .flags, place .showCmd, place .ptMinPosition .x,
                place .ptMinPosition .y, place .ptMaxPosition .x,
                place .ptMaxPosition .y, place .rcNormalPosition .left, 
                place .rcNormalPosition .top, place .rcNormalPosition .right, 
                place .rcNormalPosition .bottom);
    SetSetting (PROFILE_WINPLACEMENT, szText);

    //
    // Close our files
    //

    g_sDialogTlkFile .Close ();

    for (int i = 0; i < IDS_TOTAL_KEY_FILES; i++)
        g_sKeyFiles[i] .Close ();

    for (int i = 0; i < IDS_TOTAL_USER_HAKS; i++)
        g_sHakFiles[i] .Close ();

    //
    // Return
    //

    return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle window menu popup
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnInitMenuPopup (UINT uMsg, 
                                   WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{

    //
    // Get information from the message
    //

    HMENU hMenu = (HMENU) wParam;
    UINT nPos = LOWORD (lParam);
    bool fIsWindowMenu = HIWORD (lParam) != 0;
    if (!fIsWindowMenu)
    {
    }
    bHandled = FALSE;
    return 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load the toolbar buttons
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMainWnd::LoadToolbarButtons ()
{
    TBBUTTON asTBBtn [100];
    TCHAR szText [1024];

    //
    // Get the array to toolbar commands
    //

    GetSetting (PROFILE_TBBUTTONS, _T (""), szText, _countof (szText));
    int nButtons = DecodeToolbarButtons (szText, asTBBtn, _countof (asTBBtn));
    if (nButtons <= 0)
    {
        _sntprintf (szText, _countof (szText), 
                    _T ( "%d"),
                    ID_FILE_OPEN);
        nButtons = DecodeToolbarButtons (szText, asTBBtn, _countof (asTBBtn));
    }

    //
    // Add all the buttons
    //

    m_wndToolBar .AddButtons (nButtons, asTBBtn);
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle drop down
//
// @parm int | idCtrl | Id of the control
//
// @parm LPNMHDR | pnmh | Notify message header
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnTBDropDown (int idCtrl, 
                                LPNMHDR pnmh, BOOL &bHandled) 
{
    NMTOOLBAR *pnmTB = (NMTOOLBAR *) pnmh;

    //
    // If this is the view regions button
    //

    return TBDDRET_TREATPRESSED;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle toolbar right click
//
// @parm int | idCtrl | Id of the control
//
// @parm LPNMHDR | pnmh | Notify message header
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnToolbarRClick (int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
    LPNMMOUSE pnmmouse = (LPNMMOUSE) pnmh;

    //
    // Load the popup menu
    //

    CMenu hMenu = ::LoadPopupMenu (_Module .GetResourceInstance (),
                                   IDR_TOOLBAR_POPUP, 0);
    if (hMenu .m_hMenu == NULL)
        return TRUE;

    //
    // Track the popup
    //

    CPoint pt (pnmmouse ->pt);
    ::ClientToScreen (m_wndToolBar, &pt);
    ::TrackPopupMenuEx (hMenu, 
                        TPM_LEFTALIGN | TPM_TOPALIGN, 
                        pt .x, pt .y, m_hWnd, NULL);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle customize of toolbar
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnPopupCustomizeToolbar (WORD wNotifyCode, 
                                           WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    m_wndToolBar .Customize ();
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle toolbar right click
//
// @parm int | idCtrl | Id of the control
//
// @parm LPNMHDR | pnmh | Notify message header
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnCustomizeToolbar (int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{

    //
    // Allow any button to be inserted anywhere and also
    // allow any button to be deleted
    //

    if (pnmh ->code == TBN_QUERYINSERT ||
        pnmh ->code == TBN_QUERYDELETE)
    {
        return TRUE;
    }

    //
    // Disable the help button
    //

    if (pnmh ->code == TBN_INITCUSTOMIZE)
    {

        //
        // Ok, this is an UNDOCUMENTED hack.  The initialize message
        // actually contains the handle of the dialog for customization.
        //

        typedef struct hack_tagNMTOOLBARINIT
        {
            NMHDR   hdr;
            HWND        hWndDialog;
        } hack_NMTOOLBARINIT;
        hack_NMTOOLBARINIT *pNMHack = (hack_NMTOOLBARINIT *) pnmh;
        HWND hDlg = pNMHack ->hWndDialog;

        //
        // If the dialog hasn't been created
        //

        if (m_sTBDlg .m_hWnd == NULL)
        {

            //
            // Get the rect of the dialog
            //

            CRect rectDlg;
            ::GetClientRect (hDlg, &rectDlg);
            CRect rectScreenDlg;
            ::GetWindowRect (hDlg, &rectScreenDlg);

            //
            // Load our dialog
            //

            m_sTBDlg .Create (pNMHack ->hWndDialog, 0);

            //
            // Get the extra dialog rect
            //

            CRect rectSubDlg;
            m_sTBDlg .GetClientRect (&rectSubDlg);

            //
            // Reposition the extra dialog rect
            //

            HWND hWndAfter = ::GetNextWindow (
                ::GetDlgItem (hDlg, IDCANCEL), GW_HWNDPREV);
            m_sTBDlg .SetWindowPos (hWndAfter, 0, rectDlg .bottom,
                                    rectSubDlg .Width (),rectSubDlg .Height (), 0);

            //
            // Resize the main dialog
            //

            ::SetWindowPos (hDlg, NULL, 0, 0, rectScreenDlg .Width (),
                            rectScreenDlg .Height () + rectSubDlg .Height (),
                            SWP_NOZORDER | SWP_NOMOVE);
        }

        //
        // Get the current settings
        //

        m_sTBDlg .SetTextStyle (m_nTextStyle);
        m_sTBDlg .SetIconStyle (m_nIconStyle);
        return TBNRF_HIDEHELP;
    }

    //
    // Get the button information
    //

    else if (pnmh ->code == TBN_GETBUTTONINFO)
    {
        LPTBNOTIFY pTbNotify = (LPTBNOTIFY) pnmh;

        //
        // If we have done all the entries, return
        //

        if (pTbNotify ->iItem >= g_nCommands)
            return FALSE;


        //
        // Otherwise, populate the structure
        //

        TCHAR szBuffer [256];
        pTbNotify ->tbButton .iBitmap = g_asCommands [pTbNotify ->iItem] .iBitmap;
        pTbNotify ->tbButton .idCommand = g_asCommands [pTbNotify ->iItem] .nCmd;
        pTbNotify ->tbButton .fsState = TBSTATE_ENABLED;
        pTbNotify ->tbButton .fsStyle = g_asCommands [pTbNotify ->iItem] .fsStyle;
        pTbNotify ->tbButton .dwData = 0;
        pTbNotify ->tbButton .iString = 0;
        UpdateButton (&pTbNotify ->tbButton);

        //
        // Use the tooltip text as the text
        //

        ::LoadString (_Module .GetResourceInstance (),
                      g_asCommands [pTbNotify ->iItem] .nCmd, szBuffer, 
                      _countof (szBuffer));
        ::ExtractString (pTbNotify ->pszText,
                         pTbNotify ->cchText, szBuffer, 1, '\n');
        return TRUE;
    }

    //
    // Begin adjustment
    //

    else if (pnmh ->code == TBN_BEGINADJUST)
    {
        return FALSE;
    }

    //
    // End adjustment
    //

    else if (pnmh ->code == TBN_ENDADJUST)
    {

        //
        // Save the toolbar changes
        //

        m_nTextStyle = m_sTBDlg .GetTextStyle ();
        m_nIconStyle = m_sTBDlg .GetIconStyle ();
        SetSetting (PROFILE_TBTEXTSTYLE, m_nTextStyle);
        SetSetting (PROFILE_TBICONSTYLE, m_nIconStyle);

        //
        // Update the toobar
        //

        UpdateToolbar ();

        //
        // Save the settings
        //

        SaveToolbarButtons ();

        //
        // Force toolbar update
        //

        m_fForceUpdate = TRUE;
        return FALSE;
    }

    //
    // Toolbar change
    //

    else if (pnmh ->code == TBN_TOOLBARCHANGE)
    {
        return FALSE;
    }

    //
    // Woops, bug
    //

    else
    {
        //assert (FALSE);
        return FALSE;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Load the profile
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMainWnd::LoadProfile ()
{
    m_fResourceOpenNewTab         = GetSetting(PROFILE_RESOURCEOPENNEWTAB,      0);
    m_fResourceOpenSelected       = GetSetting(PROFILE_RESOURCEOPENSELECTED,    0);

    g_fViewImagesExportColoredPlt = GetSetting(PROFILE_VIEWIMAGESCOLOREDPLT,    0);
    g_fViewModelsBackfaceCull     = GetSetting(PROFILE_VIEWMODELSBACKFACECULL,  0);
    g_fViewModelsDrawAxes         = GetSetting(PROFILE_VIEWMODELSDRAWAXES,      0);
    g_fViewModelsFullAmbient      = GetSetting(PROFILE_VIEWMODELSFULLAMBIENT,   0);
    g_fViewModelsMdlNameTex       = GetSetting(PROFILE_VIEWMODELSMDLNAMETEX,    0);
    g_fViewModelsOrthographic     = GetSetting(PROFILE_VIEWMODELSORTHOGRAPHIC,  0);
    g_fViewModelsOutlinePolys     = GetSetting(PROFILE_VIEWMODELSOUTLINEPOLYS,  0);
    g_fViewModelsPreferDds        = GetSetting(PROFILE_VIEWMODELSPREFERDDS,     0);
    g_fViewModelsPreventDds       = GetSetting(PROFILE_VIEWMODELSPREVENTDDS,    0);
    g_fViewModelsPreventTga       = GetSetting(PROFILE_VIEWMODELSPREVENTTGA,    0);
    g_fViewModelsRunEmit          = GetSetting(PROFILE_VIEWMODELSRUNEMIT,       1);
    g_fViewModelsShowAABB         = GetSetting(PROFILE_VIEWMODELSSHOWAABB,      0);
    g_fViewModelsShowDummy        = GetSetting(PROFILE_VIEWMODELSSHOWDUMMY,     0);
    g_fViewModelsShowEmit         = GetSetting(PROFILE_VIEWMODELSSHOWEMIT,      0);
    g_fViewModelsShowLight        = GetSetting(PROFILE_VIEWMODELSSHOWLIGHT,     0);
    g_fViewModelsShowRoot         = GetSetting(PROFILE_VIEWMODELSSHOWROOT,      0);
    g_nViewImagesAlphaBehavior    = GetSetting(PROFILE_VIEWIMAGESALPHABEHAVIOR, 2);
    g_nViewImagesAlphaTint        = GetSetting(PROFILE_VIEWIMAGESALPHATINT,     0);
    g_nViewModelsAlphaBehavior    = GetSetting(PROFILE_VIEWMODELSALPHABEHAVIOR, 0);
    g_nViewModelsBackground       = GetSetting(PROFILE_VIEWMODELSBACKGROUND,    0);
    g_nViewModelsClipDistance     = GetSetting(PROFILE_VIEWMODELSCLIPDISTANCE,  128);
    g_nViewModelsHideFade         = GetSetting(PROFILE_VIEWMODELSHIDEFADE,      0);
    g_nViewModelsOrthoViewSz      = GetSetting(PROFILE_VIEWMODELSORTHOVIEWSZ,   10);
    g_nViewModelsViewAngleP       = GetSetting(PROFILE_VIEWMODELSVIEWANGLEP,    0);
    g_nViewModelsViewAngleR       = GetSetting(PROFILE_VIEWMODELSVIEWANGLER,    0);
    g_nViewModelsViewAngleY       = GetSetting(PROFILE_VIEWMODELSVIEWANGLEY,    0);
    g_nViewModelsViewDistance     = GetSetting(PROFILE_VIEWMODELSVIEWDISTANCE,  0);
    g_nViewModelsViewportX        = GetSetting(PROFILE_VIEWMODELSVIEWPORTX,     0);
    g_nViewModelsViewportY        = GetSetting(PROFILE_VIEWMODELSVIEWPORTY,     0);
}

//-----------------------------------------------------------------------------
//
// @mfunc Update the toolbar settings
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMainWnd::UpdateToolbar ()
{

    //
    // Update the toolbar's list style
    //

    if (m_nTextStyle == TBTS_RightText)
        m_wndToolBar .ModifyStyle (0, TBSTYLE_LIST);
    else
        m_wndToolBar .ModifyStyle (TBSTYLE_LIST, 0);

    //
    // Next, update the mixed buttons flag
    //

    DWORD dwStyleEx = m_wndToolBar .GetExtendedStyle ();
    if (m_nTextStyle == TBTS_RightText)
        dwStyleEx |= TBSTYLE_EX_MIXEDBUTTONS;
    else
        dwStyleEx &= ~TBSTYLE_EX_MIXEDBUTTONS;
    m_wndToolBar .SetExtendedStyle (dwStyleEx);

    //
    // Set the text rows
    //

    if (m_nTextStyle == TBTS_NoText)
        m_wndToolBar .SetMaxTextRows (0);
    else
        m_wndToolBar .SetMaxTextRows (2);

    //
    // Update the image list
    //

    CSize sizeIcon;
    CImageList *pImageList = m_nIconStyle == 
        TBIS_SmallIcons ? &g_ilSmall : &g_ilLarge;
    pImageList ->GetIconSize (sizeIcon);
    m_wndToolBar .SetImageList (*pImageList);
    m_wndToolBar .SetBitmapSize (sizeIcon);
    m_wndToolBar .SetButtonSize (sizeIcon .cx + 7, sizeIcon .cy + 7);

    //
    // Collect information about all the buttons, update the 
    // data for the button, and delete them as we go.
    //

    int nCount = m_wndToolBar .GetButtonCount ();
    TBBUTTON *pTBBtn = (TBBUTTON *) alloca (sizeof (TBBUTTON) * nCount);
    for (int nIndex = 0; nIndex < nCount; nIndex++)
    {
        m_wndToolBar .GetButton (0, &pTBBtn [nIndex]);
        UpdateButton (&pTBBtn [nIndex]);
        m_wndToolBar .DeleteButton (0);
    }

    //
    // Readd the WHOLE bloody message
    //

    m_wndToolBar .AddButtons (nCount, pTBBtn);

    //
    // Update the rebars
    //

    if (m_hWndToolBar != NULL)
    {
        UpdateSimpleReBarBandCtrl (m_hWndToolBar, m_wndCommandBar, 0); 
        UpdateSimpleReBarBandCtrl (m_hWndToolBar, m_wndToolBar, 1); 
        UpdateLayout ();
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Update a button structure
//
// @parm TBBUTTON * | pButton | Pointer to the button
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMainWnd::UpdateButton (TBBUTTON *pButton)
{

    //
    // Return if separator
    //

    if ((pButton ->fsStyle & BTNS_SEP) != 0)
        return;

    //
    // Get the string
    //

    TCHAR szText [256];
    TCHAR szText2 [256];
    if (::LoadString (_Module .GetResourceInstance (),
                      pButton ->idCommand, szText, _countof (szText)) > 0)
    {
        ::ExtractString (szText2, _countof (szText2), 
                         szText, 2, '\n');
    }
    else
        szText2 [0] = 0;
    if (szText2 [0])
        pButton ->iString = m_wndToolBar .AddStrings (szText2);
    else
        pButton ->iString = 0;

    //
    // Update the style
    //

    if (m_nTextStyle == TBTS_NoText)
    {
        pButton ->fsStyle &= ~BTNS_SHOWTEXT;
        pButton ->fsStyle |= TBSTYLE_AUTOSIZE;
    }
    else if (m_nTextStyle == TBTS_BottomText)
    {
        pButton ->fsStyle &= ~(TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT);
    }
    else if (m_nTextStyle == TBTS_RightText)
    {
        pButton ->fsStyle |= (TBSTYLE_AUTOSIZE |
                              (szText2 [0] ? BTNS_SHOWTEXT : 0));
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Save the toolbar buttons
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMainWnd::SaveToolbarButtons ()
{
    //
    // Get the id for each of the buttons
    //

    int nCount = m_wndToolBar .GetButtonCount ();
    int *pnIds = (int *) alloca (sizeof (int) * nCount);
    for (int nIndex = 0; nIndex < nCount; nIndex++)
    {
        TBBUTTON tb;
        m_wndToolBar .GetButton (nIndex, &tb);
        if ((tb .fsStyle & BTNS_SEP) != 0)
            pnIds [nIndex] = 0;
        else
            pnIds [nIndex] = tb .idCommand;
    }

    //
    // Format the string
    //

    LPTSTR pszData = (LPTSTR) alloca (sizeof (TCHAR) * nCount * 10 + 1);
    LPTSTR psz = pszData;
    for (int nIndex = 0; nIndex < nCount; nIndex++)
    {
        if (psz != pszData)
            *psz++ = ',';
        _ltot (pnIds [nIndex], psz, 10);
        psz += _tcslen (psz);
    }
    *psz++ = 0;
    SetSetting (PROFILE_TBBUTTONS, pszData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a tree view size request
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnTVSize (UINT uMsg, 
                            WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    CRect rc;

    m_tv .GetClientRect(&rc);

     if (rc.bottom - rc.top >= 40)
         m_tvsb .SetWindowPos (NULL, rc .left, rc.bottom - 20, rc .Width (), 20, SWP_NOACTIVATE | SWP_NOZORDER);
     else
         m_tvsb .SetWindowPos (NULL, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER);

    bHandled = FALSE;
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle treeview display info request
//
// @parm int | idCtrl | Id of the control
//
// @parm LPNMHDR | pnmh | Notify message header
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnTVGetDispInfo (int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
{
    bool fHasChildren = false;

    //
    // Get the object
    //

    TV_DISPINFO *pTVDispInfo = (TV_DISPINFO *) pnmh;
    TVITEM *pTVItem = &pTVDispInfo ->item;

    //
    // Get the text and child count
    //

    DataElement *pElement = (DataElement *) m_tv .GetItemData (pTVItem ->hItem);
    pElement ->pDataSource ->GetResourceText (pElement, 
                                              m_szTVText, _countof (m_szTVText));
    fHasChildren = pElement ->pDataSource ->HasChildren (pElement);

    //
    // Populate the items
    //

    if (pTVItem ->mask & TVIF_CHILDREN)
        pTVItem ->cChildren = fHasChildren;
    if (pTVItem ->mask & TVIF_TEXT)
        pTVItem ->pszText = m_szTVText;
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle item expanding
//
// @parm int | idCtrl | Id of the control
//
// @parm LPNMHDR | pnmh | Notify message header
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnTVItemExpanding (int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
{
    NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *) pnmh;
    TVITEM *pTVItem = &pNMTreeView ->itemNew;

    //
    // If the action is expanding
    //

    if (pNMTreeView ->action == TVE_EXPAND) 
    {

        //
        // If this item has never been expanded
        //

        if ((pTVItem ->state & TVIS_EXPANDEDONCE) == 0) 
        {

            //
            // Set the expand once flag
            //

            m_tv .SetItemState (pTVItem ->hItem, 
                                TVIS_EXPANDEDONCE, TVIS_EXPANDEDONCE);

            //
            // Show a wait cursor
            //

            CWaitCursor wait;

            //
            // Turn off redraw
            //

            m_tv .SendMessage (WM_SETREDRAW, FALSE, 0);

            //
            // The routine is populated later
            //

            TVSORTCB tvs;
            tvs .hParent = pTVItem ->hItem;
            tvs .lpfnCompare = OnCompareFunc;
            tvs .lParam = (LPARAM) this;

            //
            // Invoke the add routine
            //

            DataElement *pElement = (DataElement *) pTVItem ->lParam;
            pElement ->pDataSource ->AddChildren (
                m_tv, pTVItem ->hItem, pElement);

            //
            // Sort this message
            //

            m_tv .SortChildrenCB (&tvs);

            //
            // Turn on redraw
            //

            m_tv .SendMessage (WM_SETREDRAW, m_fTvRedraw, 0);
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle selection change
//
// @parm int | idCtrl | Id of the control
//
// @parm LPNMHDR | pnmh | Notify message header
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnTVSelChanged (int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
{
    //
    // Invalidate to redraw
    //

#if !defined (_WIN32_WCE)
    //  m_pPdProtocolInfoWnd ->OnChanged (false);
#endif

    if (m_fResourceOpenSelected)
        OnNMDblClk(idCtrl, pnmh, bHandled);

    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a double click
//
// @parm int | idCtrl | Id of the control
//
// @parm LPNMHDR | pnmh | Notify message header
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnNMDblClk (int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
{

    //
    // Get the resource
    //

    bHandled = FALSE;
    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem == NULL)
        return TRUE;

    //
    // Open the resource
    //

    DataElement *pElement = (DataElement *) m_tv .GetItemData (hItem);
    OpenResource (!!m_fResourceOpenNewTab, pElement);

    //
    // If we have an edit, then edit
    //

    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Delete the current document
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMainWnd::DeleteDocument () 
{
    //
    // Show a wait cursor
    //

    CWaitCursor wait;

    //
    // Delete the source
    //

    for (int i = 0; i < m_apSources .GetCount (); i++)
        delete m_apSources [i];
    m_apSources .RemoveAll ();

    //
    // Delete the search box
    //
    if (m_tvsb .IsWindow ())
        ::DestroyWindow(m_tvsb);

    //
    // Delete all the tree view items
    //

    if (m_tv .IsWindow ())
    {
        //m_tv .SendMessage (WM_SETREDRAW, FALSE, 0);
        //m_tv .DeleteAllItems ();
        //m_tv .SendMessage (WM_SETREDRAW, TRUE, 0);

        HWND hWnd = m_tv .m_hWnd;
        m_tv .SetDlgCtrlID (0);
        m_tv .UnsubclassWindow (FALSE);
        m_tv .m_hWnd = NULL;
        CreateTreeView ();
        SetSplitterPanes (m_tv .m_hWnd, m_wndContent .m_hWnd);
        ::ShowWindow (hWnd, SW_HIDE);
        m_atvDelete .Add (hWnd);
    }

    //
    // Delete all the views
    //

    if (m_wndContent .m_hWnd != NULL)
    {
        m_wndContent .CloseAllViews ();
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle focus event
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnTVSetFocus (UINT uMsg, 
                                   WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    m_tvsb .ShowWindow (SW_HIDE);

    bHandled = FALSE;
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle button down event
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnTVRButtonDown (UINT uMsg, 
                                   WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{

    //
    // Get the event position
    //

    CPoint pt (lParam);
    m_tv .ClientToScreen (&pt);

    //
    // Test for button
    //

    HTREEITEM hItem = m_tv .HitTest (CPoint (lParam), NULL);

    //
    // Change the selection
    //

    m_tv .SelectItem (hItem);
    OnIdle ();

    //
    // Load the menu
    //

    HMENU hMenu = LoadPopupMenu (
        _Module .GetResourceInstance (), 
        IDR_POPUPS, 0);

    //
    // Display the popup
    //

    int nCmd = m_wndCommandBar .TrackPopupMenu (hMenu, 
                                                TPM_RIGHTBUTTON | TPM_LEFTALIGN, pt .x, pt .y);
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle button up event
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnTVRButtonUp (UINT uMsg, 
                                 WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle file open
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnFileOpen (WORD wNotifyCode, 
                              WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    CFileDialog dlg (TRUE, _T (".*"), _T(""), 
                     OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                     g_pszOpenFilter);
    if (dlg .DoModal () == IDOK)
    {
        if (DoOpen (dlg .m_ofn .lpstrFile, false))
        {
            m_mru .AddToList (dlg .m_ofn .lpstrFile);
            m_mru .WriteToRegistry (g_pszKeyRoot);
        }
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle file open
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnFileOpenBinary (WORD wNotifyCode, 
                                    WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    CFileDialog dlg (TRUE, _T (".*"), _T(""), 
                     OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                     g_pszOpenFilter);
    if (dlg .DoModal () == IDOK)
    {
        if (DoOpen (dlg .m_ofn .lpstrFile, true))
        {
            //m_mru .AddToList (dlg .m_ofn .lpstrFile);
            //m_mru .WriteToRegistry (g_pszKeyRoot);
        }
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle file reload
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnFileReload (WORD wNotifyCode, 
                                WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    if (m_szCurrentFile [0] == '\0')
        return OnFileOpenNwn(wNotifyCode, wID, hWndCtl, bHandled);

    DoOpen (m_szCurrentFile, !!m_fCurrentFileBinary);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle file options
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnFileOptions (WORD wNotifyCode, 
                                 WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

    //
    // Create the pages
    //

    CNwnOptionsPage sPage1;
    CPropertySheet sSheet (g_szAppName, 0, m_hWnd);
    sSheet .AddPage (sPage1);

    //
    // Initialize their settings
    //

    TCHAR szNwnDirectory [_MAX_PATH], szTexSearchPath[_MAX_PATH * 2];

    GetSetting (PROFILE_NWN_DIRECTORY, _T (""),
                szNwnDirectory, _countof (szNwnDirectory));
    GetSetting (PROFILE_TEX_SEARCH_PATH,
                _T ("texturepacks\\xp2_tex_tpa.erf texturepacks\\xp1_tex_tpa.erf texturepacks\\textures_tpa.erf texturepacks\\tiles_tpa.erf"),
                szTexSearchPath, _countof (szTexSearchPath));
    sPage1 .m_strNwnDirectory = szNwnDirectory;
    sPage1 .m_strTexSearchPath = szTexSearchPath;

    sPage1 .m_fResourceOpenNewTab         = m_fResourceOpenNewTab;
    sPage1 .m_fResourceOpenSelected       = m_fResourceOpenSelected;

    sPage1 .m_fViewImagesExportColoredPlt = g_fViewImagesExportColoredPlt;
    sPage1 .m_fViewModelsBackfaceCull     = g_fViewModelsBackfaceCull;
    sPage1 .m_fViewModelsDrawAxes         = g_fViewModelsDrawAxes;
    sPage1 .m_fViewModelsFullAmbient      = g_fViewModelsFullAmbient;
    sPage1 .m_fViewModelsMdlNameTex       = g_fViewModelsMdlNameTex;
    sPage1 .m_fViewModelsOrthographic     = g_fViewModelsOrthographic;
    sPage1 .m_fViewModelsOutlinePolys     = g_fViewModelsOutlinePolys;
    sPage1 .m_fViewModelsPreferDds        = g_fViewModelsPreferDds;
    sPage1 .m_fViewModelsPreventDds       = g_fViewModelsPreventDds;
    sPage1 .m_fViewModelsPreventTga       = g_fViewModelsPreventTga;
    sPage1 .m_fViewModelsRunEmit          = g_fViewModelsRunEmit;
    sPage1 .m_fViewModelsShowAABB         = g_fViewModelsShowAABB;
    sPage1 .m_fViewModelsShowDummy        = g_fViewModelsShowDummy;
    sPage1 .m_fViewModelsShowEmit         = g_fViewModelsShowEmit;
    sPage1 .m_fViewModelsShowLight        = g_fViewModelsShowLight;
    sPage1 .m_fViewModelsShowRoot         = g_fViewModelsShowRoot;

    sPage1 .m_fViewModelsHideFade1        = !!(g_nViewModelsHideFade & 1);
    sPage1 .m_fViewModelsHideFade2        = !!(g_nViewModelsHideFade & 2);
    sPage1 .m_fViewModelsHideFade4        = !!(g_nViewModelsHideFade & 4);

    sPage1 .m_nViewImagesAlphaBehavior    = g_nViewImagesAlphaBehavior;
    sPage1 .m_nViewModelsAlphaBehavior    = g_nViewModelsAlphaBehavior;


    //
    // Load the image viewer alpha tint setting
    //

    TCHAR szSetting[8];
    int nSetting = g_nViewImagesAlphaTint;

    snprintf(szSetting, sizeof(szSetting), "%d", (nSetting >> 16) & 0xFF);
    sPage1 .m_strViewImagesAlphaR = szSetting;

    snprintf(szSetting, sizeof(szSetting), "%d", (nSetting >>  8) & 0xFF);
    sPage1 .m_strViewImagesAlphaG = szSetting;

    snprintf(szSetting, sizeof(szSetting), "%d", (nSetting >>  0) & 0xFF);
    sPage1 .m_strViewImagesAlphaB = szSetting;

    //
    // Load the model viewer background color setting
    //

    nSetting = g_nViewModelsBackground;

    snprintf(szSetting, sizeof(szSetting), "%d", (nSetting >> 16) & 0xFF);
    sPage1 .m_strViewModelsBackR = szSetting;

    snprintf(szSetting, sizeof(szSetting), "%d", (nSetting >>  8) & 0xFF);
    sPage1 .m_strViewModelsBackG = szSetting;

    snprintf(szSetting, sizeof(szSetting), "%d", (nSetting >>  0) & 0xFF);
    sPage1 .m_strViewModelsBackB = szSetting;

    //
    // Load the model viewer viewport pixel size setting
    //

    snprintf(szSetting, sizeof(szSetting), "%d", g_nViewModelsViewportX);
    sPage1 .m_strViewModelsViewX = szSetting;

    snprintf(szSetting, sizeof(szSetting), "%d", g_nViewModelsViewportY);
    sPage1 .m_strViewModelsViewY = szSetting;

    //
    // Load the model viewer distance settings
    //

    snprintf(szSetting, sizeof(szSetting), "%d", g_nViewModelsClipDistance);
    sPage1 .m_strViewModelsClipDistance = szSetting;

    snprintf(szSetting, sizeof(szSetting), "%d", g_nViewModelsViewDistance);
    sPage1 .m_strViewModelsViewDistance = szSetting;

    //
    // Load the model viewer angle settings
    //

    snprintf(szSetting, sizeof(szSetting), "%d", g_nViewModelsViewAngleY);
    sPage1 .m_strViewModelsAngleY = szSetting;

    snprintf(szSetting, sizeof(szSetting), "%d", g_nViewModelsViewAngleP);
    sPage1 .m_strViewModelsAngleP = szSetting;

    snprintf(szSetting, sizeof(szSetting), "%d", g_nViewModelsViewAngleR);
    sPage1 .m_strViewModelsAngleR = szSetting;

    //
    // Load the model viewer orthographic viewport size setting
    //

    snprintf(szSetting, sizeof(szSetting), "%d", g_nViewModelsOrthoViewSz);
    sPage1 .m_strViewModelsOrthoV = szSetting;


    //
    // Show the dialog
    //

    if (sSheet .DoModal () == IDOK)
    {

        //
        // Save the NWN options
        //

        SetSetting (PROFILE_NWN_DIRECTORY,           sPage1 .m_strNwnDirectory);
        SetSetting (PROFILE_TEX_SEARCH_PATH,         sPage1 .m_strTexSearchPath);

        SetSetting (PROFILE_RESOURCEOPENNEWTAB,      (sPage1 .m_fResourceOpenNewTab         != 0));
        SetSetting (PROFILE_RESOURCEOPENSELECTED,    (sPage1 .m_fResourceOpenSelected       != 0));

        SetSetting (PROFILE_VIEWIMAGESCOLOREDPLT,    (sPage1 .m_fViewImagesExportColoredPlt != 0));
        SetSetting (PROFILE_VIEWMODELSBACKFACECULL,  (sPage1 .m_fViewModelsBackfaceCull     != 0));
        SetSetting (PROFILE_VIEWMODELSDRAWAXES,      (sPage1 .m_fViewModelsDrawAxes         != 0));
        SetSetting (PROFILE_VIEWMODELSFULLAMBIENT,   (sPage1 .m_fViewModelsFullAmbient      != 0));
        SetSetting (PROFILE_VIEWMODELSMDLNAMETEX,    (sPage1 .m_fViewModelsMdlNameTex       != 0));
        SetSetting (PROFILE_VIEWMODELSORTHOGRAPHIC,  (sPage1 .m_fViewModelsOrthographic     != 0));
        SetSetting (PROFILE_VIEWMODELSOUTLINEPOLYS,  (sPage1 .m_fViewModelsOutlinePolys     != 0));
        SetSetting (PROFILE_VIEWMODELSPREFERDDS,     (sPage1 .m_fViewModelsPreferDds        != 0));
        SetSetting (PROFILE_VIEWMODELSPREVENTDDS,    (sPage1 .m_fViewModelsPreventDds       != 0));
        SetSetting (PROFILE_VIEWMODELSPREVENTTGA,    (sPage1 .m_fViewModelsPreventTga       != 0));
        SetSetting (PROFILE_VIEWMODELSRUNEMIT,       (sPage1 .m_fViewModelsRunEmit          != 0));
        SetSetting (PROFILE_VIEWMODELSSHOWAABB,      (sPage1 .m_fViewModelsShowAABB         != 0));
        SetSetting (PROFILE_VIEWMODELSSHOWDUMMY,     (sPage1 .m_fViewModelsShowDummy        != 0));
        SetSetting (PROFILE_VIEWMODELSSHOWEMIT,      (sPage1 .m_fViewModelsShowEmit         != 0));
        SetSetting (PROFILE_VIEWMODELSSHOWLIGHT,     (sPage1 .m_fViewModelsShowLight        != 0));
        SetSetting (PROFILE_VIEWMODELSSHOWROOT,      (sPage1 .m_fViewModelsShowRoot         != 0));

        SetSetting (PROFILE_VIEWIMAGESALPHABEHAVIOR, sPage1 .m_nViewImagesAlphaBehavior);
        SetSetting (PROFILE_VIEWMODELSALPHABEHAVIOR, sPage1 .m_nViewModelsAlphaBehavior);

        nSetting = (!!(sPage1 .m_fViewModelsHideFade1) << 0) |
                   (!!(sPage1 .m_fViewModelsHideFade2) << 1) |
                   (!!(sPage1 .m_fViewModelsHideFade4) << 2);
        SetSetting (PROFILE_VIEWMODELSHIDEFADE, nSetting);

        nSetting = strtol(sPage1 .m_strViewModelsClipDistance, NULL, 10);
        if (nSetting < 1)
            nSetting = 128;
        SetSetting (PROFILE_VIEWMODELSCLIPDISTANCE, nSetting);

        nSetting = strtol(sPage1 .m_strViewModelsViewDistance, NULL, 10);
        if (nSetting < 0)
            nSetting = 0;
        SetSetting (PROFILE_VIEWMODELSVIEWDISTANCE, nSetting);

        nSetting = strtol(sPage1 .m_strViewModelsViewX, NULL, 10);
        if (nSetting < 0)
            nSetting = 0;
        SetSetting (PROFILE_VIEWMODELSVIEWPORTX, nSetting);

        nSetting = strtol(sPage1 .m_strViewModelsViewY, NULL, 10);
        if (nSetting < 0)
            nSetting = 0;
        SetSetting (PROFILE_VIEWMODELSVIEWPORTY, nSetting);

        nSetting = strtol(sPage1 .m_strViewModelsAngleY, NULL, 10);
        if (nSetting < -360)
            nSetting = 0;
        else if (nSetting > 360)
            nSetting = 0;
        SetSetting (PROFILE_VIEWMODELSVIEWANGLEY, nSetting);

        nSetting = strtol(sPage1 .m_strViewModelsAngleP, NULL, 10);
        if (nSetting < -360)
            nSetting = 0;
        else if (nSetting > 360)
            nSetting = 0;
        SetSetting (PROFILE_VIEWMODELSVIEWANGLEP, nSetting);

        nSetting = strtol(sPage1 .m_strViewModelsAngleR, NULL, 10);
        if (nSetting < -360)
            nSetting = 0;
        else if (nSetting > 360)
            nSetting = 0;
        SetSetting (PROFILE_VIEWMODELSVIEWANGLER, nSetting);

        nSetting = strtol(sPage1 .m_strViewModelsOrthoV, NULL, 10);
        if (nSetting < 1)
            nSetting = 10;
        SetSetting (PROFILE_VIEWMODELSORTHOVIEWSZ, nSetting);

        nSetting = (strtol(sPage1 .m_strViewModelsBackR, NULL, 10) & 0xFF) << 16 |
            (strtol(sPage1 .m_strViewModelsBackG, NULL, 10) & 0xFF) <<  8 |
            (strtol(sPage1 .m_strViewModelsBackB, NULL, 10) & 0xFF);
        SetSetting (PROFILE_VIEWMODELSBACKGROUND, nSetting);

        nSetting = (strtol(sPage1 .m_strViewImagesAlphaR, NULL, 10) & 0xFF) << 16 |
            (strtol(sPage1 .m_strViewImagesAlphaG, NULL, 10) & 0xFF) <<  8 |
            (strtol(sPage1 .m_strViewImagesAlphaB, NULL, 10) & 0xFF);
        SetSetting (PROFILE_VIEWIMAGESALPHATINT, nSetting);

        LoadProfile ();

        /* OpenNWN (); */
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle file open
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnFileOpenNwn (WORD wNotifyCode, 
                                 WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

    //
    // Try to open the key file
    //

    if (!g_sKeyFiles[IDS_TOTAL_KEY_FILES - 1] .IsOpen ())
    {
        CString str (MAKEINTRESOURCE (IDS_ERR_NWN_OPEN));
        ::MessageBox (NULL, str, g_szAppName, MB_OK);
        return TRUE;
    }

    //
    // Delete the current document
    //

    DeleteDocument ();

    CDataSource *pSource;

    if (g_bIsNWNEE)
    {
        pSource = new CDataSourceKey(&(g_sKeyFiles[7]), IDS_MAIN_KEY);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        if (g_sKeyFiles[6].IsOpen()) {
            pSource = new CDataSourceKey(&(g_sKeyFiles[6]), IDS_LANG_KEY);
            pSource->AddRoot(m_tv, TVI_ROOT);
            m_apSources.Add(pSource);
        }

        //
        // Add the other directories
        //
        pSource = new CDataSourceDir(IDS_GAME_MODULES, "data\\nwm\\", "*.*", true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_TEXTURE_PACKS, "data\\txpk\\", "*.*", true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_INSTALL_OVERRIDE, "ovr\\", "*.*", false);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_INSTALL_HAK, "data\\hk\\", "*.*", true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_INSTALL_AMBIENT, "data\\amb\\", "*.*", false);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_INSTALL_MUSIC, "data\\mus\\", "*.*", false);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_INSTALL_DM_VAULT, "data\\dmv\\", "*.*", false);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_INSTALL_LOCAL_VAULT, "data\\lcv\\", "*.*", false);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_INSTALL_MODULES, "data\\mod\\", "*.*", true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_OVERRIDE, "override\\", "*.*", false, true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_DEVELOPMENT, "development\\", "*.*", false, true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_ERF, "erf\\", "*.*", true, true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_HAK, "hak\\", "*.*", true, true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_AMBIENT, "ambient\\", "*.*", false, true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_MUSIC, "music\\", "*.*", false, true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_DM_VAULT, "dmvault\\", "*.*", false, true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_LOCAL_VAULT, "localvault\\", "*.*", false, true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_SERVER_VAULT, "servervault\\", "*.*", false, true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_USER_MODULES, "modules\\", "*.*", true, true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);
    }
    else
    {
        //
        // Add in the key files
        //
        for (int i = IDS_TOTAL_KEY_FILES - 1; i >= 0; i--) {
            if (!g_sKeyFiles[i] .IsOpen ())
                continue;

            pSource = new CDataSourceKey (&(g_sKeyFiles[i]), IDS_MAIN_KEY + (IDS_TOTAL_KEY_FILES - (i + 1)));
            HTREEITEM hRoot = pSource ->AddRoot (m_tv, TVI_ROOT);
            //m_tv .Expand (hRoot, TVE_EXPAND);
            m_apSources .Add (pSource);
        }

        //
        // Add the other directories
        //
        pSource = new CDataSourceDir(IDS_DEVELOPMENT, "development\\", "*.*", false);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_OVERRIDE, "override\\", "*.*", false);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_GAME_MODULES, "nwm\\", "*.*", true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_ERF, "erf\\", "*.*", true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_HAK, "hak\\", "*.*", true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_TEXTURE_PACKS, "texturepacks\\", "*.*", true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_AMBIENT, "ambient\\", "*.*", false);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_MUSIC, "music\\", "*.*", false);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_DM_VAULT, "dmvault\\", "*.*", false);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_LOCAL_VAULT, "localvault\\", "*.*", false);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_SERVER_VAULT, "servervault\\", "*.*", false);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);

        pSource = new CDataSourceDir(IDS_USER_MODULES, "modules\\", "*.*", true);
        pSource->AddRoot(m_tv, TVI_ROOT);
        m_apSources.Add(pSource);
    }

#if 0
    if (!g_strUserHakMask .IsEmpty()) {
        pSource = new CDataSourceDir (IDS_USER_HAKS, "hak\\", g_strUserHakMask, true);
        pSource ->AddRoot (m_tv, TVI_ROOT);
        m_apSources .Add (pSource);
    }
#endif

    //
    // Add the root item
    //

    SetSinglePaneMode (SPLIT_PANE_NONE);

    m_szCurrentFile [0] = '\0';
    m_fCurrentFileBinary = false;

    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle file open recent
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnFileRecent (WORD wNotifyCode, 
                                WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

    //
    // Get file name from the MRU list
    //

    TCHAR szFile [MAX_PATH];
    if (m_mru .GetFromList (wID, szFile, _countof(szFile)))
    {
        if (DoOpen (szFile, false))
            m_mru .MoveToTop (wID);
        else
            m_mru .RemoveFromList (wID);
        m_mru .WriteToRegistry (g_pszKeyRoot);
    }
    else
    {
        ::MessageBeep (MB_ICONERROR);
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a request to open a resource
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnResourceView (WORD wNotifyCode, 
                                  WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

    //
    // Get the resource
    //

    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem == NULL)
        return TRUE;

    //
    // Open the resource
    //

    DataElement *pElement = (DataElement *) m_tv .GetItemData (hItem);
    OpenResource (wID == ID_RESOURCES_VIEWINNEWWINDOW, pElement);

    ::SetFocus(m_tv .m_hWnd);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a request to search for a resource
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnResourceSearch (WORD wNotifyCode, 
                                    WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    if (m_tv .IsWindow()) {
        m_tvsb .ShowWindow (SW_SHOW);
        ::SetFocus(m_tvsb .m_hWnd);
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a request to export resources
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnResourceExport (WORD wNotifyCode, 
                                    WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

    //
    // Get the resource
    //

    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem == NULL)
        return TRUE;


    //
    // If the element is a resource
    //

    DataElement *pElement = (DataElement *) m_tv .GetItemData (hItem);
    if (pElement ->pDataSource ->IsResource (pElement))
    {

        //
        // Get the resource name
        //

        char szName [_MAX_PATH];
        NwnResType nResType;
        pElement ->pDataSource ->GetResourceName (pElement, 
                                                  szName, _countof (szName), &nResType);
        const char *pszExt = NwnGetResTypeExtension (nResType);
        strncat (szName, pszExt, _countof (szName)); 

        //
        // Invoke the dialog
        //

        CFileDialog dlg (FALSE, _T (""), szName, 
                         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                         NULL);
        if (dlg .DoModal () == IDOK)
        {
            if (pElement ->pDataSource ->Export (
                    pElement, dlg .m_ofn .lpstrFile) == 0)
            {
                CString str (MAKEINTRESOURCE (IDS_ERR_EXPORT));
                ::MessageBox (NULL, str, g_szAppName, MB_OK);
                return false;
            }
        }
    }

    //
    // If the element isn't a resource
    //

    else
    {

        //
        // Invoke the dialog
        //

        CFolderDialog dlg;
        if (dlg .DoModal () == IDOK)
        {
            CWaitCursor sWait;
            CString str (dlg .m_szFolderPath);
            str += "\\";
            if (pElement ->pDataSource ->Export (pElement, str) == 0)
            {
                CString str (MAKEINTRESOURCE (IDS_ERR_EXPORT));
                ::MessageBox (NULL, str, g_szAppName, MB_OK);
                return false;
            }
        }
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a request to export a TGA
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnResourceExportTga (WORD wNotifyCode, 
                                       WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

    //
    // Get the resource
    //

    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem == NULL)
        return TRUE;


    //
    // If the element is a resource
    //

    DataElement *pElement = (DataElement *) m_tv .GetItemData (hItem);
    if (pElement ->pDataSource ->IsResource (pElement))
    {

        //
        // Get the resource name
        //

        char szName [_MAX_PATH];
        NwnResType nResType;
        pElement ->pDataSource ->GetResourceName (pElement, 
                                                  szName, _countof (szName), &nResType);
        strncat (szName, ".tga", _countof (szName)); 

        //
        // Invoke the dialog
        //

        CFileDialog dlg (FALSE, _T (""), szName, 
                         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                         NULL);
        if (dlg .DoModal () == IDOK)
        {
            if (TgaWrite(pElement, dlg .m_ofn .lpstrFile, nResType) == 0)
            {
                CString str (MAKEINTRESOURCE (IDS_ERR_EXPORT));
                ::MessageBox (NULL, str, g_szAppName, MB_OK);
                return false;
            }
        }
    }

    //
    // If the element isn't a resource
    //

    else
    {

        //
        // Invoke the dialog
        //

        CFolderDialog dlg;
        if (dlg .DoModal () == IDOK)
        {
            CWaitCursor sWait;
            CString str (dlg .m_szFolderPath);
            str += "\\";
            if (pElement ->pDataSource ->ExportTga (pElement, str) == 0)
            {
                CString str (MAKEINTRESOURCE (IDS_ERR_EXPORT));
                ::MessageBox (NULL, str, g_szAppName, MB_OK);
                return false;
            }
        }
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a request move up in the tree view
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnResourceTreeMove (WORD wNotifyCode, 
                                      WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    ::SetFocus(m_tv .m_hWnd);

    HTREEITEM hMove, hItem = m_tv .GetSelectedItem ();

    if (wID == ID_RESOURCES_MOVEUP)
        hMove = m_tv .GetPrevVisibleItem(hItem);
    else
        hMove = m_tv .GetNextVisibleItem(hItem);

    if (hMove != NULL)
        m_tv .SelectItem(hMove);

    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a request to toggle an option
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CMainWnd::OnToggle (WORD wNotifyCode, 
                            WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    switch (wID) {
        case ID_TOGGLES_AABB:
            g_fViewModelsShowAABB = !g_fViewModelsShowAABB;
            break;

        case ID_TOGGLES_ALPHA:
            g_nViewModelsAlphaBehavior = (g_nViewModelsAlphaBehavior + 1) % 3;
            break;

        case ID_TOGGLES_AXES:
            g_fViewModelsDrawAxes = !g_fViewModelsDrawAxes;
            break;

        case ID_TOGGLES_BACKFACE:
            g_fViewModelsBackfaceCull = !g_fViewModelsBackfaceCull;
            break;

        case ID_TOGGLES_EMITTERS:
            g_fViewModelsRunEmit = !g_fViewModelsRunEmit;
            break;

        case ID_TOGGLES_FULLAMBIENT:
            g_fViewModelsFullAmbient = !g_fViewModelsFullAmbient;
            break;

        case ID_TOGGLES_MESHOUTLINE:
            g_fViewModelsOutlinePolys = !g_fViewModelsOutlinePolys;
            break;

        case ID_TOGGLES_ORTHOGRAPHIC:
            g_fViewModelsOrthographic = !g_fViewModelsOrthographic;
            break;

        case ID_TOGGLES_PREFERDDS:
            g_fViewModelsPreferDds = !g_fViewModelsPreferDds;
            break;

        case ID_TOGGLES_USE_DDS:
            g_fViewModelsPreventDds = !g_fViewModelsPreventDds;
            break;

        case ID_TOGGLES_USE_TGA:
            g_fViewModelsPreventTga = !g_fViewModelsPreventTga;
            break;

        case ID_TOGGLES_TILEFADE_1:
            g_nViewModelsHideFade ^= 1;
            break;

        case ID_TOGGLES_TILEFADE_2:
            g_nViewModelsHideFade ^= 2;
            break;

        case ID_TOGGLES_TILEFADE_4:
            g_nViewModelsHideFade ^= 4;
            break;

        case ID_TOGGLES_SHOWDUMMY:
            g_fViewModelsShowDummy = !g_fViewModelsShowDummy;
            break;

        case ID_TOGGLES_SHOWEMITTER:
            g_fViewModelsShowEmit = !g_fViewModelsShowEmit;
            break;

        case ID_TOGGLES_SHOWLIGHT:
            g_fViewModelsShowLight = !g_fViewModelsShowLight;
            break;

        case ID_TOGGLES_SHOWROOT:
            g_fViewModelsShowRoot = !g_fViewModelsShowRoot;
            break;

        default: return FALSE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Open a new file
//
// @parm LPCTSTR | pszName | Name of the file
//
// @parm bool | fBinary | If true, open as binary
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

bool CMainWnd::DoOpen (LPCTSTR pszName, bool fBinary) 
{
    USES_CONVERSION;

    //
    // Open the file
    //

    FILE *fp = fopen (T2CA (pszName), "rb");
    if (fp == NULL)
    {
        CString str (MAKEINTRESOURCE (IDS_ERR_OPEN));
        ::MessageBox (NULL, str, g_szAppName, MB_OK);
        return false;
    }

    //
    // Read the first 100 bytes
    //

    unsigned char auchHeader [100];
    size_t nRead = fread (auchHeader, 1, _countof (auchHeader), fp);
    fclose (fp);

    //
    // Extract the file name from the path
    //

    TCHAR szFileName [_MAX_PATH];
    LPCTSTR pszTemp = pszName;
    for (LPCTSTR psz = pszName; *psz; psz = _tcsinc (psz)) 
    {
        if (*psz == '\\' || *psz == '/' || *psz == ':')
            pszTemp = (LPTSTR) _tcsinc (psz);
    }
    _tcsncpy (szFileName, pszTemp, _countof (szFileName));

    //
    // Locate the extension
    //

    NwnResType nResType = NwnResType_Unknown;
    if (!fBinary)
    {
        LPTSTR pszExt = _tcsrchr (szFileName, '.');
        if (pszExt != NULL)
            nResType = NwnGetResTypeFromExtension (pszExt);
    }

    //
    // If this is a key file
    //

    if (nResType == NwnResType_KEY)
    {
        //
        // Create a new document
        //

        DeleteDocument ();

        //
        // Create the document
        //

        CNwnKeyFile *pKeyFile = new CNwnKeyFile ();
        pKeyFile ->Open (pszName);
        CDataSource *pSource = new CDataSourceKey (pKeyFile, pszName);
        HTREEITEM hRoot = pSource ->AddRoot (m_tv, TVI_ROOT);
        m_tv .Expand (hRoot, TVE_EXPAND);
        m_apSources .Add (pSource);
        SetSinglePaneMode (SPLIT_PANE_NONE);
    }

    //
    // If this is a bif file
    //

    else if (nResType == NwnResType_BIF)
    {
        //
        // Create a new document
        //

        DeleteDocument ();

        //
        // Create the document
        //

        CNwnBifFile *pBifFile = new CNwnBifFile ();
        pBifFile ->Open (pszName);
        CDataSource *pSource = new CDataSourceBif (pBifFile, pszName);
        HTREEITEM hRoot = pSource ->AddRoot (m_tv, TVI_ROOT);
        m_tv .Expand (hRoot, TVE_EXPAND);
        m_apSources .Add (pSource);
        SetSinglePaneMode (SPLIT_PANE_NONE);
    }

    //
    // If we have a module of some sort
    //

    else if (nResType == NwnResType_MOD || nResType == NwnResType_NWM ||
             nResType == NwnResType_HAK || nResType == NwnResType_ERF ||
             nResType == NwnResType_SAV)
    {
        //
        // Create a new document
        //

        DeleteDocument ();

        //
        // Create the document
        //

        CNwnModuleFile *pModFile = new CNwnModuleFile ();
        pModFile ->Open (pszName);
        CDataSource *pSource = new CDataSourceMod (pModFile, pszName);
        HTREEITEM hRoot = pSource ->AddRoot (m_tv, TVI_ROOT);
        m_tv .Expand (hRoot, TVE_EXPAND);
        m_apSources .Add (pSource);
        SetSinglePaneMode (SPLIT_PANE_NONE);
    }

    //
    // Otherwise, we single pane mode it
    //

    else
    {
        //
        // Load the file
        //

        CData *pFile = g_sResourceCache .LoadRes (pszName);

        //
        // Create a new document
        //

        DeleteDocument ();

        //
        // Open
        //

        m_wndContent .OpenResource (szFileName, nResType, pFile);
        SetSinglePaneMode (SPLIT_PANE_RIGHT);
    }

    strncpy(m_szCurrentFile, pszName, _countof (m_szCurrentFile));
    m_fCurrentFileBinary = fBinary;

    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Add this object to the treeview
//
// @parm const void * | pRes | Resource pointer
//
// @parm HTREEITEM | hParent | Parent of the item
//
// @rdesc Handle of the added item
//
//-----------------------------------------------------------------------------

HTREEITEM CMainWnd::AddToTV (const void *psRes, HTREEITEM hParent)
{
    TVINSERTSTRUCT tvi;

    //
    // Iniitalize tree view item
    //

    tvi .hParent = hParent;
    tvi .hInsertAfter = TVI_LAST;
    tvi .item .mask = TVIF_CHILDREN | TVIF_PARAM | TVIF_TEXT;
    tvi .item .hItem = 0;
    tvi .item .state = 0;
    tvi .item .stateMask = 0;
    tvi .item .pszText = LPSTR_TEXTCALLBACK;
    tvi .item .cchTextMax = 0;
    tvi .item .iImage = 0;
    tvi .item .iSelectedImage = 0;
    tvi .item .cChildren = I_CHILDRENCALLBACK;
    tvi .item .lParam = (LPARAM) psRes;
    HTREEITEM hTI = m_tv .InsertItem (&tvi);
    return hTI;
}

//-----------------------------------------------------------------------------
//
// @mfunc Open a resource
//
// @parm bool | fNewWindow | If true, open a new window
//
// @parm DataElement * | pElement | Element to be opened
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMainWnd::OpenResource (bool fNewWindow, DataElement *pElement)
{
    char szResName [256];

    //
    // If this isn't openable, then return
    //

    if (!pElement ->pDataSource ->IsResource (pElement))
        return;

    //
    // Get the name of the resource
    //

    NwnResType nResType;
    pElement ->pDataSource ->GetResourceName (pElement, 
                                              szResName, _countof (szResName), &nResType);

    //
    // Add the extension
    //

    TCHAR szTemp [32];
    LPCTSTR pszExt = NwnGetResTypeExtension (nResType);
    if (pszExt == NULL)
    {
        _stprintf (szTemp, ".%X", nResType);
        pszExt = szTemp;
    }
    _tcsncat (szResName, pszExt, _countof (szResName)); 
    CString strResName (szResName);

    SetStatusBarText (_T("Opening ") + strResName);

    //
    // Try to open existing
    //

    if (!fNewWindow)
    {
        if (m_wndContent .OpenExisting (strResName, nResType))
            return;

        //
        // Close the current window
        //

        BOOL bHandled;
        m_wndContent .OnResourcesCloseView (0, 0, m_wndContent, bHandled);
    }

    //
    // Wait cursor
    //

    CWaitCursor sCursor;

    //
    // Load the resource
    //

    CData *pRes = pElement ->pDataSource ->LoadRes (pElement);
    SetStatusBarText (NULL);

    if (pRes == NULL)
    {
        CString str (MAKEINTRESOURCE (IDS_ERR_RESOURCE_OPEN));
        ::MessageBox (NULL, str, g_szAppName, MB_OK);
        return;
    }

    //
    // Finish the loading process
    //

    m_wndContent .OpenResource (strResName, nResType, pRes);
}

//-----------------------------------------------------------------------------
//
// @mfunc Search for a resource
//
// @parm const char * | pszName | the name of the resource
//
// @parm HTREEITEM | hItem | the item to start at
//
// @parm bool | fRecurse | TRUE if this is a recursive call
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

BOOL CMainWnd::SearchResources (const char *pszName, HTREEITEM hItem, HTREEITEM &hSkip, BOOL fRecurse)
{
    if (!fRecurse) {
        BOOL ret;
        CWaitCursor wait;
        char szSearch[64];

        if (!m_tv .IsWindow ())
            return FALSE;

        snprintf(szSearch, _countof(szSearch), "%s", pszName);
        strlwr(szSearch);

        hItem = m_tv .GetNextItem (NULL, TVGN_ROOT);

        m_tv .SendMessage (WM_SETREDRAW, (m_fTvRedraw = FALSE), 0);
        ret = SearchResources(szSearch, hItem, hSkip, TRUE);
        m_tv .SendMessage (WM_SETREDRAW, (m_fTvRedraw = TRUE), 0);

        return ret;
    }

    if (hItem == NULL)
        return FALSE;

    char szItem[64];
    UINT fExpanded;

    for (; hItem != NULL; hItem = m_tv .GetNextItem (hItem, TVGN_NEXT)) {
        if (hSkip != NULL) {
            if (hItem == hSkip)
                hSkip = NULL;

            fExpanded = TRUE;
        } else {
            m_tv. GetItemText(hItem, szItem, _countof(szItem));
            strlwr(szItem);

            if (strstr(szItem, pszName) != NULL) {
                m_tv .SendMessage (WM_SETREDRAW, (m_fTvRedraw = TRUE), 0);
                m_tv .SelectItem (hItem);
                ::SetFocus(m_tv .m_hWnd);
                return TRUE;
            }
        }

        if (hSkip == NULL) {
            fExpanded = m_tv .GetItemState(hItem, TVIS_EXPANDED);
            m_tv .Expand (hItem, TVE_EXPAND);
        }

        HTREEITEM hChild = m_tv .GetNextItem (hItem, TVGN_CHILD);
        if (hChild != NULL) {
            if (SearchResources(pszName, hChild, hSkip, TRUE))
                return TRUE;
        }

        if (!fExpanded)
            m_tv .Expand (hItem, TVE_COLLAPSE);
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Create the tree view
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMainWnd::CreateTreeView ()
{
    //
    // Create the tree view and search box
    //

    DWORD dwStyle = WS_CHILD | TVS_HASLINES | TVS_HASBUTTONS | 
        TVS_LINESATROOT | TVS_SHOWSELALWAYS | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    m_tv .Create (m_hWnd, 0, _T (""), dwStyle, 
                  WS_EX_CLIENTEDGE, (HMENU) IDC_EXP_TREEVIEW, NULL);
    m_tv .SetBkColor (GetSysColor (COLOR_WINDOW));
    m_tv .SetImageList (m_ilList .m_hImageList, TVSIL_NORMAL);

    m_tvsb .Create (m_tv, 0, _T (""), WS_CHILD | WS_CLIPSIBLINGS | ES_AUTOHSCROLL,
                  WS_EX_CLIENTEDGE, (HMENU) IDC_EXP_TREESEARCH, NULL);
    m_tvsb .SetFont(AtlGetDefaultGuiFont());
}

//-----------------------------------------------------------------------------
//
// @mfunc Delete the given tree view slowly
//
// @parm HWND hWnd | Tree view window
//
// @parm HTREEITEM | hTreeItem | Delete point
//
// @parn int & | nCount | Current number of deleted items
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMainWnd::DeleteTreeView (HWND hWnd, HTREEITEM hTreeItem, int &nCount)
{
    //
    // Delete the children
    //

    HTREEITEM hChild = TreeView_GetChild (hWnd, hTreeItem);
    while (hChild != NULL && nCount < g_nMaxSlowDelete)
    {
        HTREEITEM hNext = TreeView_GetNextSibling (hWnd, hChild);
        DeleteTreeView (hWnd, hChild, nCount);
        hChild = hNext;
    }

    //
    // Delete myself
    //

    if (nCount < g_nMaxSlowDelete)
    {
        TreeView_DeleteItem (hWnd, hTreeItem);
        nCount++;
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Open NWN
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMainWnd::OpenNWN ()
{
    //
    // Try to open the file
    //

    {

        //
        // Begin wait cursor
        //

        CWaitCursor sWC;

        //
        // Close the current file
        //

        g_sDialogTlkFile .Close ();

        for (int i = 0; i < IDS_TOTAL_KEY_FILES; i++)
            g_sKeyFiles[i] .Close ();

        for (int i = 0; i < IDS_TOTAL_USER_HAKS; i++)
            g_sHakFiles[i] .Close ();

        //
        // Get the settings directory first
        //

        TCHAR szNwnDirectory [_MAX_PATH], szTexSearchPath[_MAX_PATH * 2];

        GetSetting (PROFILE_NWN_DIRECTORY, _T (""),
                    szNwnDirectory, _countof (szNwnDirectory));
        g_strNwnDirectory = szNwnDirectory;

        //
        // If we have something to try
        //

        if (!g_strNwnDirectory .IsEmpty ())
        {
            if (g_strNwnDirectory [g_strNwnDirectory .GetLength () - 1] != '\\' &&
                g_strNwnDirectory [g_strNwnDirectory .GetLength () - 1] != '/' &&
                g_strNwnDirectory [g_strNwnDirectory .GetLength () - 1] != ':')
                g_strNwnDirectory += _T ("\\");

            g_bIsNWNEE = false;
            if (PathFileExists(g_strNwnDirectory + _T("data\\nwn_base.key")))
            {
                g_bIsNWNEE = true;
            }

            if (g_bIsNWNEE)
            {
                g_sDialogTlkFile .Open (g_strNwnDirectory + _T ("data\\dialog.tlk"));
                if (!g_sDialogTlkFile.IsOpen()) {
                    g_sDialogTlkFile.Open(g_strNwnDirectory + _T ("lang\\en\\data\\dialog.tlk"));
                }
                g_sKeyFiles[7] .Open (g_strNwnDirectory  + _T ("data\\nwn_base.key"));
                g_sKeyFiles[6] .Open (g_strNwnDirectory  + _T ("data\\nwn_base_loc.key"));

                GetSetting (PROFILE_TEX_SEARCH_PATH,
                            _T ("data\\txpk\\xp2_tex_tpa.erf data\\txpk\\xp1_tex_tpa.erf data\\txpk\\textures_tpa.erf data\\txpk\\tiles_tpa.erf"),
                            szTexSearchPath, _countof (szTexSearchPath));
                g_strTexSearchPath = szTexSearchPath;

                CHAR szMyDocumentsPath[MAX_PATH];
                SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, NULL, szMyDocumentsPath);
                g_strUserDirectory = szMyDocumentsPath;
                g_strUserDirectory = g_strUserDirectory + "\\Neverwinter Nights\\";
            }
            else
            {
                g_sDialogTlkFile.Open(g_strNwnDirectory + _T("dialog.tlk"));

                g_sKeyFiles[7].Open(g_strNwnDirectory + _T("chitin.key"));
                g_sKeyFiles[6].Open(g_strNwnDirectory + _T("patch.key"));
                g_sKeyFiles[5].Open(g_strNwnDirectory + _T("xp1.key"));
                g_sKeyFiles[4].Open(g_strNwnDirectory + _T("xp1patch.key"));
                g_sKeyFiles[3].Open(g_strNwnDirectory + _T("xp2.key"));
                g_sKeyFiles[2].Open(g_strNwnDirectory + _T("xp2patch.key"));
                g_sKeyFiles[1].Open(g_strNwnDirectory + _T("xp3.key"));
                g_sKeyFiles[0].Open(g_strNwnDirectory + _T("xp3patch.key"));

                GetSetting (PROFILE_TEX_SEARCH_PATH,
                            _T ("texturepacks\\xp2_tex_tpa.erf texturepacks\\xp1_tex_tpa.erf texturepacks\\textures_tpa.erf texturepacks\\tiles_tpa.erf"),
                            szTexSearchPath, _countof (szTexSearchPath));
                g_strTexSearchPath = szTexSearchPath;
                g_strUserDirectory = g_strNwnDirectory;
            }

            if (!g_strTexSearchPath .IsEmpty ())
            {
                int i = 0;
                char *p;

                for (p = strtok(szTexSearchPath, " ");
                     p != NULL && i < IDS_TOTAL_USER_HAKS;
                     p = strtok(NULL, " ")) {

                    g_sHakFiles[i] .Open( g_strNwnDirectory + p );

                    if (g_sHakFiles[i] .IsOpen())
                        i++;
                }
            }
        }

        //
        // If we failed
        //

        if (!g_sDialogTlkFile .IsOpen () ||
            !g_sKeyFiles[IDS_TOTAL_KEY_FILES - 1] .IsOpen ())
        {

            //
            // Try the Nwn directory
            //

            g_strNwnDirectory = GetNwnDirectory ();

            //
            // Try known paths to EE installs
            //

            if (g_strNwnDirectory.IsEmpty())
            {
                if (PathFileExists(DEFAULT_STEAM_FOLDER))
                {
                    g_strNwnDirectory = DEFAULT_STEAM_FOLDER;
                }
                else
                {
                    CHAR szUserFolderPath[MAX_PATH];
                    SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, NULL, szUserFolderPath);
                    CString strBDCInstallPath = szUserFolderPath;
                    strBDCInstallPath = strBDCInstallPath + "Beamdog Library\\00785\\";
                    if (PathFileExists(strBDCInstallPath))
                    {
                        g_strNwnDirectory = strBDCInstallPath;
                    }
                }
            }

            if (!g_strNwnDirectory .IsEmpty ())
            {
                g_sDialogTlkFile .Open (g_strNwnDirectory + _T ("dialog.tlk"));

                g_sKeyFiles[7] .Open (g_strNwnDirectory  + _T ("chitin.key"));
                g_sKeyFiles[6] .Open (g_strNwnDirectory  + _T ("patch.key"));
                g_sKeyFiles[5] .Open (g_strNwnDirectory  + _T ("xp1.key"));
                g_sKeyFiles[4] .Open (g_strNwnDirectory  + _T ("xp1patch.key"));
                g_sKeyFiles[3] .Open (g_strNwnDirectory  + _T ("xp2.key"));
                g_sKeyFiles[2] .Open (g_strNwnDirectory  + _T ("xp2patch.key"));
                g_sKeyFiles[1] .Open (g_strNwnDirectory  + _T ("xp3.key"));
                g_sKeyFiles[0] .Open (g_strNwnDirectory  + _T ("xp3patch.key"));

                if (!g_strTexSearchPath .IsEmpty ())
                {
                    int i = 0;
                    char *p;

                    for (p = strtok(szTexSearchPath, " ");
                         p != NULL && i < IDS_TOTAL_USER_HAKS;
                         p = strtok(NULL, " ")) {

                        g_sHakFiles[i] .Open( g_strNwnDirectory + p );

                        if (g_sHakFiles[i] .IsOpen())
                            i++;
                    }

#if 0
                    int i = 0;
                    CString strSearch (g_strNwnDirectory + _T( "hak\\" ) + g_strUserHakMask);
                    struct _finddata_t sFind;
                    intptr_t id = _findfirst (strSearch, &sFind);

                    if (id > 0) {
                        do {
                            if ((sFind .attrib & (_A_HIDDEN | _A_SYSTEM | _A_SUBDIR)) != 0)
                                continue;

                            const char *pszExt = strrchr (sFind .name, '.');
                            if (pszExt == NULL)
                                continue;

                            if (NwnGetResTypeFromExtension (pszExt) != NwnResType_HAK)
                                continue;

                            g_sHakFiles[i++] .Open( g_strNwnDirectory + "hak\\" + sFind.name );
                        } while (i < IDS_TOTAL_USER_HAKS && _findnext (id, &sFind) >= 0);

                        _findclose (id);
                    }
#endif
                }
            }
        }
    }

    //
    // If we failed
    //

    if (!g_sDialogTlkFile .IsOpen () ||
        !g_sKeyFiles[IDS_TOTAL_KEY_FILES - 1] .IsOpen ())
    {
        g_sDialogTlkFile .Close ();

        for (int i = 0; i < IDS_TOTAL_KEY_FILES; i++)
            g_sKeyFiles[i] .Close ();

        for (int i = 0; i < IDS_TOTAL_USER_HAKS; i++)
            g_sHakFiles[i] .Close ();

        g_strNwnDirectory .Empty ();
        CString str;
        str .Format (IDS_ERR_NO_DIALOGTLK);
        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
    }

    //
    // Otherwise, initialize the compiler
    //

    else
    {
        CNwnStdLoader sLoader;
        sLoader .Initialize (g_strNwnDirectory);
        NscCompilerInitialize (&sLoader, 999999, false);
    }
    return;
}


/* vim: set sw=4: */
