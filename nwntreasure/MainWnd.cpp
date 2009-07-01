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
#include "nwntreasure.h"
#include "RootObject.h"
#include "TableObject.h"
#include "ItemObject.h"
#include "TableRefObject.h"
#include "GoldObject.h"
#include "NoneObject.h"
#include "DataParser.h"
#include "TableItemDlg.h"
#include "ExportDlg.h"
#include "ProfileDlg.h"

//
// Debug NEW
//

#if defined (_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Global variables
//

extern TCHAR g_szAppName [];
extern TCHAR g_szSettingsFile [];
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

LPCTSTR g_pszXMLFilter = 
_T("NW Treasure XML File (*.xml)\0*.xml\0")
_T("All Files (*.*)\0*.*\0")
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

//-----------------------------------------------------------------------------
//
// @mfunc <c CMainWnd> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CMainWnd::CMainWnd () : m_tv (this, 1)
{
    m_pTreasure = NULL;
    m_pEncounter = NULL;
    m_pPlaceable = NULL;
    m_pProfile = NULL;
    m_fModified = false;
    m_fForceUpdate = FALSE;
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
                              MAKEINTRESOURCE (IDR_NWNTREASURE));

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
    // Create the report view image list
    //

    m_ilList .Create (16, 16, ILC_COLOR16 | ILC_MASK, 16, 16);
    m_nTableIcon = m_ilList .AddIcon (LoadIcon (
            _Module .GetResourceInstance (), MAKEINTRESOURCE (IDI_TABLE)));
    m_nTableNoRefIcon = m_ilList .AddIcon (LoadIcon (
            _Module .GetResourceInstance (), MAKEINTRESOURCE (IDI_TABLE_NOREF)));
    m_nTableGlobalIcon = m_ilList .AddIcon (LoadIcon (
            _Module .GetResourceInstance (), MAKEINTRESOURCE (IDI_TABLE_GLOBAL)));
    m_nEncounterIcon = m_ilList .AddIcon (LoadIcon (
            _Module .GetResourceInstance (), MAKEINTRESOURCE (IDI_ENCOUNTER)));
    m_nItemIcon = m_ilList .AddIcon (LoadIcon (
            _Module .GetResourceInstance (), MAKEINTRESOURCE (IDI_ITEM)));
    m_nGoldIcon = m_ilList .AddIcon (LoadIcon (
            _Module .GetResourceInstance (), MAKEINTRESOURCE (IDI_GOLD)));
    m_nNoneIcon = m_ilList .AddIcon (LoadIcon (
            _Module .GetResourceInstance (), MAKEINTRESOURCE (IDI_NONE)));
    m_nPlaceableIcon = m_ilList .AddIcon (LoadIcon (
            _Module .GetResourceInstance (), MAKEINTRESOURCE (IDI_PLACEABLE)));
    m_nProfileIcon = m_ilList .AddIcon (LoadIcon (
            _Module .GetResourceInstance (), MAKEINTRESOURCE (IDI_PROFILE)));

    //
    // Create the list view
    //

    DWORD dwStyle = WS_CHILD | TVS_HASLINES | TVS_HASBUTTONS | 
        TVS_LINESATROOT | TVS_SHOWSELALWAYS | WS_VISIBLE | WS_CLIPSIBLINGS;
    m_tv .Create (m_hWnd, 0, _T (""), dwStyle, 
                  WS_EX_CLIENTEDGE, (HMENU) IDC_EXP_TREEVIEW, NULL);
    m_tv .SetBkColor (GetSysColor (COLOR_WINDOW));

    //
    // Initialize
    //

    m_tv .SetImageList (m_ilList .m_hImageList, TVSIL_NORMAL);
    m_hWndClient = m_tv .m_hWnd;

    //
    // Create a black document
    //

    NewDocument (true);

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
    // Return FALSE
    //

    bHandled = FALSE;
    return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a close
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

LRESULT CMainWnd::OnClose (UINT uMsg, 
                           WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    bHandled = !QueryClose ();
    return FALSE;
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
    // Get the currently selected item
    //

    CObject *pObject = NULL;
    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem != NULL)
        pObject = (CObject *) m_tv .GetItemData (hItem);

    //
    // Get the parent
    //

    CObject *pParent = NULL;
    HTREEITEM hParent = hItem != NULL ? m_tv .GetParentItem (hItem) : NULL;
    if (hParent != NULL)
        pParent = (CObject *) m_tv .GetItemData (hParent);

    //
    // Commands we support
    //

    UIEnable (ID_EDIT_PROPERTIES, pObject && pObject ->CanEdit (), m_fForceUpdate);
    UIEnable (ID_EDIT_TABLEPROPERTIES, pObject && pObject ->CanTableEdit (), m_fForceUpdate);
    UIEnable (ID_EDIT_DELETE, pObject && pObject ->CanDelete (), m_fForceUpdate);
    UIEnable (ID_EDIT_NEW_TABLE, pObject != NULL && pObject ->CanTableNew (), m_fForceUpdate);
    UIEnable (ID_FILE_SAVE, m_fModified, m_fForceUpdate);

    //
    // See if we can new
    //

    bool fCanNew = false;
    if (pObject)
        fCanNew = pObject ->CanNew ();
    UIEnable (ID_EDIT_NEW, fCanNew, m_fForceUpdate);

    //
    // See if we can new inside
    //

    fCanNew = false;
    if (pParent != NULL)
        fCanNew = pParent ->CanNew ();
    UIEnable (ID_EDIT_NEW_AFTER, fCanNew, m_fForceUpdate);

    //
    // Check for move up/move down
    //

    UIEnable (ID_EDIT_MOVEUP, FALSE, m_fForceUpdate);
    UIEnable (ID_EDIT_MOVEDOWN, FALSE, m_fForceUpdate);
    if (pParent && pParent ->CanMoveChild ())
    {
        UIEnable (ID_EDIT_MOVEDOWN, m_tv .GetNextSiblingItem (hItem) != NULL, m_fForceUpdate);
        UIEnable (ID_EDIT_MOVEUP, m_tv .GetPrevSiblingItem (hItem) != NULL, m_fForceUpdate);
    }

    //
    // Update the toolbar
    //

    UIUpdateToolBar (m_fForceUpdate);
    m_fForceUpdate = FALSE;
    return FALSE;
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
                    _T ( "%d,%d,%d,0,%d,%d,%d,0,%d,%d,0,%d,0,%d,%d"),
                    ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, ID_EDIT_NEW,
                    ID_EDIT_NEW_AFTER, ID_EDIT_NEW_TABLE, ID_EDIT_PROPERTIES,
                    ID_EDIT_TABLEPROPERTIES, ID_EDIT_DELETE, ID_EDIT_MOVEUP, 
                    ID_EDIT_MOVEDOWN);
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

LRESULT CMainWnd::OnTVGetDispInfoW (int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
{

    //
    // Get the object
    //

    TV_DISPINFOW *pTVDispInfo = (TV_DISPINFOW *) pnmh;
    TVITEMW *pTVItem = &pTVDispInfo ->item;
    CObject *pObject = (CObject *) pTVItem ->lParam;

    //
    // Get the text
    //

#if defined (UNICODE) || defined (_UNICODE)
    pObject ->GetDescription (this, m_szwTVText, _countof (m_szwTVText));
#else
    pObject ->GetDescription (this, m_szaTVText, _countof (m_szaTVText));
    ::MultiByteToWideChar (CP_ACP, 0, m_szaTVText, -1,
                           m_szwTVText, _countof (m_szwTVText));
#endif

    //
    // Get the image number
    //

    int nImage;
    Object_Image nOImage = pObject ->GetImage ();
    if (nOImage == Object_Image_Table)
        nImage = m_nTableIcon;
    else if (nOImage == Object_Image_Table_NoRef)
        nImage = m_nTableNoRefIcon;
    else if (nOImage == Object_Image_Table_Global)
        nImage = m_nTableGlobalIcon;
    else if (nOImage == Object_Image_Item)
        nImage = m_nItemIcon;
    else if (nOImage == Object_Image_Gold)
        nImage = m_nGoldIcon;
    else if (nOImage == Object_Image_None)
        nImage = m_nNoneIcon;
    else if (nOImage == Object_Image_Placeable)
        nImage = m_nPlaceableIcon;
    else if (nOImage == Object_Image_Profile)
        nImage = m_nProfileIcon;
    else 
        nImage = m_nEncounterIcon;

    //
    // Populate the items
    //

    if (pTVItem ->mask & TVIF_CHILDREN)
        pTVItem ->cChildren = pObject ->HasChildren (this);
    if (pTVItem ->mask & TVIF_IMAGE)
        pTVItem ->iImage = nImage;
    if (pTVItem ->mask & TVIF_SELECTEDIMAGE)
        pTVItem ->iSelectedImage = nImage;
    if (pTVItem ->mask & TVIF_TEXT)
        pTVItem ->pszText = m_szwTVText;
    return TRUE;
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

LRESULT CMainWnd::OnTVGetDispInfoA (int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
{

    //
    // Get the object
    //

    TV_DISPINFOA *pTVDispInfo = (TV_DISPINFOA *) pnmh;
    TVITEMA *pTVItem = &pTVDispInfo ->item;
    CObject *pObject = (CObject *) pTVItem ->lParam;

    //
    // Get the text
    //

#if defined (UNICODE) || defined (_UNICODE)
    pObject ->GetDescription (this, m_szwTVText, _countof (m_szwTVText));
    ::WideCharToMultiByte (CP_ACP, 0, m_szwTVText, -1,
                           m_szaTVText, _countof (m_szaTVText), NULL, NULL);
#else
    pObject ->GetDescription (this, m_szaTVText, _countof (m_szaTVText));
#endif

    //
    // Get the image number
    //

    int nImage;
    Object_Image nOImage = pObject ->GetImage ();
    if (nOImage == Object_Image_Table)
        nImage = m_nTableIcon;
    else if (nOImage == Object_Image_Table_NoRef)
        nImage = m_nTableNoRefIcon;
    else if (nOImage == Object_Image_Table_Global)
        nImage = m_nTableGlobalIcon;
    else if (nOImage == Object_Image_Item)
        nImage = m_nItemIcon;
    else if (nOImage == Object_Image_Gold)
        nImage = m_nGoldIcon;
    else if (nOImage == Object_Image_None)
        nImage = m_nNoneIcon;
    else if (nOImage == Object_Image_Placeable)
        nImage = m_nPlaceableIcon;
    else if (nOImage == Object_Image_Profile)
        nImage = m_nProfileIcon;
    else 
        nImage = m_nEncounterIcon;

    //
    // Populate the items
    //

    if (pTVItem ->mask & TVIF_CHILDREN)
        pTVItem ->cChildren = pObject ->HasChildren (this);
    if (pTVItem ->mask & TVIF_IMAGE)
        pTVItem ->iImage = nImage;
    if (pTVItem ->mask & TVIF_SELECTEDIMAGE)
        pTVItem ->iSelectedImage = nImage;
    if (pTVItem ->mask & TVIF_TEXT)
        pTVItem ->pszText = m_szaTVText;
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
    CObject *pObject = (CObject *) pTVItem ->lParam;

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
            // If there are elements to add, then add
            //

            m_tv .SendMessage (WM_SETREDRAW, FALSE, 0);
            pObject ->AddChildren (this, m_tv, pTVItem ->hItem);
            m_tv .SendMessage (WM_SETREDRAW, TRUE, 0);
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
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle key down event
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

LRESULT CMainWnd::OnTVKeyDown (int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
{
    NMTVKEYDOWN *pNMKeyDown = (NMTVKEYDOWN *) pnmh;

    //
    // If delete, then delete
    //

    if (pNMKeyDown ->wVKey == VK_DELETE && 
        (pNMKeyDown ->flags & KF_ALTDOWN) == 0)
        OnEditDelete (0, 0, 0, bHandled);
    else
        bHandled = FALSE;
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
    // Get the currently selected item
    //

    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem == NULL)
        return NULL;
    CObject *pObject = (CObject *) m_tv .GetItemData (hItem);

    //
    // If we have an edit, then edit
    //

    if (pObject ->CanEdit ())
        OnEditProperties (0, 0, 0, bHandled);
    else if (pObject ->CanTableEdit ())
        OnEditTableProperties (0, 0, 0, bHandled);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user pressing modify
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

LRESULT CMainWnd::OnEditProperties (WORD wNotifyCode, 
                                    WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

    //
    // Get the currently selected item
    //

    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem == NULL)
        return NULL;
    CObject *pObject = (CObject *) m_tv .GetItemData (hItem);

    //
    // Edit the object
    //

    int nID;
    CTableItemObject *pTObject = static_cast <CTableItemObject *> (pObject);
    if (pTObject ->GetObjectType () == Object_Type_Profile)
    {
        CProfileDlg dlg (static_cast <CProfileObject *> (pTObject),
                         this, hItem);
        nID = dlg .DoModal ();
    }
    else
    {
        CTableItemDlg dlg (this, hItem, pTObject ->GetTable (), pTObject);
        nID = dlg .DoModal ();
    }
    if (nID == IDOK)
    {
        m_fModified = true;
        m_tv .InvalidateRect (NULL);
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user pressing modify
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

LRESULT CMainWnd::OnEditTableProperties (WORD wNotifyCode, 
                                         WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

    //
    // Get the currently selected item
    //

    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem == NULL)
        return NULL;
    CObject *pObject = (CObject *) m_tv .GetItemData (hItem);

    //
    // Edit the object
    //

    if (pObject ->TableEdit (this, hItem))
    {
        m_fModified = true;
        m_tv .InvalidateRect (NULL);
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user pressing delete
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

LRESULT CMainWnd::OnEditDelete (WORD wNotifyCode, 
                                WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

    //
    // Get the currently selected item
    //

    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem == NULL)
        return NULL;
    CObject *pObject = (CObject *) m_tv .GetItemData (hItem);

    //
    // Edit the object
    //

    if (pObject ->Delete (this, m_tv, hItem, true))
    {
        m_fModified = true;
        m_tv .InvalidateRect (NULL);
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user pressing new
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

LRESULT CMainWnd::OnEditNew (WORD wNotifyCode, 
                             WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    //
    // Get the currently selected item
    //

    HTREEITEM hInsertAfter = TVI_LAST;
    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem == NULL)
        return FALSE;
    CObject *pObject = (CObject *) m_tv .GetItemData (hItem);
    if (!pObject ->CanNew ())
        return FALSE;

    //
    // Invoke the helper
    //

    OnEditNew (wID, pObject, hInsertAfter);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user pressing new
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

LRESULT CMainWnd::OnEditNewAfter (WORD wNotifyCode, 
                                  WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

    //
    // Get the currently selected item
    //

    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem == NULL)
        return FALSE;
    CObject *pObject = (CObject *) m_tv .GetItemData (hItem);

    //
    // Get the parent
    //

    HTREEITEM hInsertAfter = hItem;
    hItem = m_tv .GetParentItem (hItem);
    if (hItem == NULL)
        return FALSE;
    pObject = (CObject *) m_tv .GetItemData (hItem);
    if (!pObject ->CanNew ())
        return FALSE;

    //
    // Invoke the helper
    //

    OnEditNew (wID, pObject, hInsertAfter);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user pressing new
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

void CMainWnd::OnEditNew (WORD wID, CObject *pObject, HTREEITEM hInsertAfter) 
{
    CTableObject *pTable = NULL;
    CTableRefObject *pTableRef = NULL;

    //
    // If this object is a table ref, the get the table
    //

    if (pObject ->GetObjectType () == Object_Type_Table_Ref)
    {
        pTableRef = static_cast <CTableRefObject *> (pObject);
        pTable = pTableRef ->GetTableRef (this);
    }
    else if (pObject ->GetObjectType () == Object_Type_Table)
    {
        pTable = static_cast <CTableObject *> (pObject);
    }
    else
    {
        _ASSERTE (FALSE);
        return;
    }

    //
    // Display the dialog
    //

    CTableItemObject *pNewObject = NULL;
    if (pTable ->GetTableType () != Table_Type_Profile)
    {
        CTableItemDlg dlg (this, NULL, pTable, NULL);
        if (dlg .DoModal () == IDOK)
            pNewObject = dlg .m_pObject;
    }
    else
    {
        CProfileObject *pProfile = new CProfileObject;
        pProfile ->SetTable (pTable);
        CProfileDlg dlg (pProfile, this, NULL);
        if (dlg .DoModal () == IDOK)
            pNewObject = pProfile;
        else
            delete pProfile;
    }

    //
    // If a new object was created
    //

    if (pNewObject)
    {
        m_fModified = true;
        m_tv .InvalidateRect (NULL);
        int nIndex = pTable ->AddNewItem (
            this, m_tv, pNewObject, hInsertAfter);
        for (int i = 0; i < m_apTables .GetCount (); i++)
        {
            m_apTables [i] ->AddNewItemToTableRefs (this,
                                                    m_tv, pNewObject, pTable ->GetName ());
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user pressing new
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

LRESULT CMainWnd::OnEditNewTable (WORD wNotifyCode, 
                                  WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    //
    // Search down until we find a root
    //

    CObject *pObject;
    HTREEITEM hItem = m_tv .GetSelectedItem ();
    while (hItem)
    {
        pObject = (CObject *) m_tv .GetItemData (hItem);
        if (pObject ->GetObjectType () == Object_Type_Root)
            break;
        hItem = m_tv .GetParentItem (hItem);
    }
    if (hItem == NULL)
        return FALSE;


    //
    // Depending on the id, create the dialog
    //

    CRootObject *pRoot = static_cast <CRootObject *> (pObject);
    CTableObject *pNew = new CTableObject;
    if (pRoot == m_pTreasure)
        pNew ->SetTableType (Table_Type_Treasure);
    else if (pRoot == m_pEncounter)
        pNew ->SetTableType (Table_Type_Encounter);
    else if (pRoot == m_pPlaceable)
        pNew ->SetTableType (Table_Type_Placeable);
    else if (pRoot == m_pProfile)
    {
        delete pNew;
        _ASSERTE (FALSE);
        return TRUE;
    }
    else
    {
        ::MessageBox (m_hWnd, _T ("Error, unknown table type"), g_szAppName, MB_OK);
        return TRUE;
    }
    if (!pNew ->TableEdit (this, NULL))
    {
        delete pNew;
        return TRUE;
    }

    //
    // We we have something, then add
    //

    if (pNew)
    {
        m_fModified = true;
        m_apTables .Add (pNew);
        pRoot ->AddNewItem (this, m_tv, pNew);
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user requesting a move
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

LRESULT CMainWnd::OnEditMove (WORD wNotifyCode, 
                              WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    //
    // Get the currently selected item
    //

    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem == NULL)
        return FALSE;
    CObject *pObject = (CObject *) m_tv .GetItemData (hItem);

    //
    // Get the parent
    //

    HTREEITEM hParent = m_tv .GetParentItem (hItem);
    if (hParent == NULL)
        return FALSE;
    CObject *pParent = (CObject *) m_tv .GetItemData (hParent);

    //
    // Move the child
    //

    m_fModified = true;
    pParent ->MoveChild (this, m_tv, static_cast <CTableItemObject *> (pObject), 
                         wID == ID_EDIT_MOVEUP ? -1 : 1);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Create an empty document
//
// @parm bool | fCreateProfileTable | If true, create the profile table
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMainWnd::NewDocument (bool fCreateProfileTable) 
{

    //
    // Delete the current document
    //

    DeleteDocument ();

    //
    // Add all of the protocol items
    //

    m_tv .SendMessage (WM_SETREDRAW, FALSE, 0);
    m_pTreasure = new CRootObject (Table_Type_Treasure);
    m_pEncounter = new CRootObject (Table_Type_Encounter);
    m_pPlaceable = new CRootObject (Table_Type_Placeable);
    m_pProfile = new CRootObject (Table_Type_Profile);
    HTREEITEM hInitial = m_pTreasure ->AddToTV (m_tv, TVI_ROOT, TVI_LAST);
    m_pEncounter ->AddToTV (m_tv, TVI_ROOT, TVI_LAST);
    m_pPlaceable ->AddToTV (m_tv, TVI_ROOT, TVI_LAST);
    m_pProfile ->AddToTV (m_tv, TVI_ROOT, TVI_LAST);
    m_tv .SelectItem (hInitial);
    m_tv .SendMessage (WM_SETREDRAW, TRUE, 0);
    m_tv .SetFocus ();

    //
    // If requested, create profile table
    //

    if (fCreateProfileTable)
        CreateProfileTable ();

    //
    // Initialize other variables
    //

    m_strFile .Empty ();
    m_fModified = false;
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
    // Delete all the tree view items
    //

    if (m_tv .IsWindow ())
        m_tv .DeleteAllItems ();

    //
    // Delete the 2 root objects
    //

    if (m_pTreasure)
        delete m_pTreasure;
    if (m_pEncounter)
        delete m_pEncounter;
    if (m_pPlaceable)
        delete m_pPlaceable;
    if (m_pProfile)
        delete m_pProfile;

    //
    // Delete all the tables
    //

    for (int i = 0; i < m_apTables .GetCount (); i++)
        delete m_apTables [i];
    m_apTables .RemoveAll ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle export
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

LRESULT CMainWnd::OnFileExportScript (WORD wNotifyCode, 
                                      WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    CExportDlg::DoExport (this);
    return TRUE;
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
// @mfunc Find a table
//
// @parm LPCTSTR | pszName | Name of the table
//
// @parm Table_Type | nTableType | Type of table to search for
// 
// @rdesc Address of the table or NULL if not found
//
//-----------------------------------------------------------------------------

CTableObject *CMainWnd::FindTable (LPCTSTR pszName, Table_Type nTableType)
{
    for (int i = 0; i < m_apTables .GetCount (); i++)
    {
        if (nTableType != Table_Type_Any)
        {
            if (m_apTables [i] ->GetTableType () != nTableType)
                continue;
        }
        if (_tcscmp (m_apTables [i] ->GetName (), pszName) == 0)
            return m_apTables [i];
    }
    return NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a table ref name change
//
// @parm CTableRefObject * | pTableRef | Table ref being changed
//
// @parm LPCTSTR | pszNewName | Name of the table
//
// @parm HTREEITEM | hItem | Tree view item of the table ref
// 
// @rdesc TRUE if the new name has been handled
//
//-----------------------------------------------------------------------------

bool CMainWnd::HandleTableRefNameChange (CTableRefObject *pTableRef, 
                                         LPCTSTR pszNewName, HTREEITEM hItem)
{
    CString str;

    //
    // Get the treasure table flag
    //

    Table_Type nTableType = pTableRef ->GetTable () ->GetTableType ();

    //
    // Check for no change
    //

    if (_tcscmp (pTableRef ->GetName (), pszNewName) == 0)
        return true;

    //
    // Locate the name in the table
    //

    CTableObject *pNewTable = FindTable (
        pszNewName, Table_Type_Any);
    bool fNewTable = false;
    bool fDeleteTable = false;

    //
    // If the name was not found
    // 

    if (pNewTable == NULL)
    {

        //
        // Prompt for creation
        //

        str .Format (IDS_ERR_NEW_TABLE);
        if (::MessageBox (m_hWnd, str, g_szAppName, MB_YESNO) == IDNO)
            return false;

        //
        // Create a new table
        //

        pNewTable = new CTableObject ();
        pNewTable ->SetName (CString (pszNewName));
        pNewTable ->SetTableType (nTableType);
        fNewTable = true;
    }

    //
    // Otherwise, check for mismatch
    //

    else
    {
        if (pNewTable ->GetTableType () != nTableType)
        {
            str .Format (IDS_ERR_TABLE_MISMATCH);
            ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
            return false;
        }
    }

    //
    // If we have an old table
    //

    CTableObject *pOldTable = FindTable (
        pTableRef ->GetName (), nTableType);
    if (pOldTable)
    {

        //
        // If the table will be fully derefed
        //

        if (pOldTable ->GetRefCount () == 1 && 
            !pOldTable ->GetGlobal ())
        {

            //
            // Prompt the user
            //

            str .Format (IDS_ERR_DEREF_TABLE);
            int nID = ::MessageBox (m_hWnd, str, g_szAppName, MB_YESNOCANCEL);
            if (nID == IDCANCEL)
            {
                if (fNewTable)
                    delete pNewTable;
                return false;
            }
            fDeleteTable = nID == IDYES;
        }
    }

    //
    // If this is an old table with a zero ref count and not global
    //

    if (!fNewTable && !pNewTable ->GetGlobal () && pNewTable ->GetRefCount () == 0)
    {
        pNewTable ->DeleteFromTV (this, m_tv, NULL);
    }

    //
    // If thisi s a new table, add to the list
    //

    if (fNewTable)
    {
        m_apTables .Add (pNewTable);
    }

    //
    // Remove all children from myself
    //

    if (hItem != NULL)
        pTableRef ->DeleteChildrenFromTV (this, m_tv, hItem);

    //
    // Update
    //

    HandleTableDeRef (pOldTable, fDeleteTable);
    if (pNewTable)
        pNewTable ->AddRef ();
    pTableRef ->SetName (CString (pszNewName));
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a table ref delete
//
// @parm CTableRefObject * | pTableRef | Table ref being changed
//
// @parm HTREEITEM | hItem | Tree view item of the table ref
//
// @parm bool | fPrompt | If true, prompt for delete.  Otherwise, take defaults
// 
// @rdesc TRUE if the new name has been handled
//
//-----------------------------------------------------------------------------

bool CMainWnd::HandleTableRefDelete (CTableRefObject *pTableRef, 
                                     HTREEITEM hItem, bool fPrompt)
{
    CString str;

    //
    // Get the treasure table flag
    //

    Table_Type nTableType = pTableRef ->GetTable () ->GetTableType ();

    //
    // If we have an old table
    //

    bool fDeleteTable = false;
    CTableObject *pOldTable = FindTable (
        pTableRef ->GetName (), nTableType);
    if (pOldTable)
    {

        //
        // If the table will be fully derefed
        //

        if (pOldTable ->GetRefCount () == 1 && 
            !pOldTable ->GetGlobal ())
        {

            //
            // Prompt the user
            //

            if (fPrompt)
            {
                str .Format (IDS_ERR_DEREF_TABLE);
                int nID = ::MessageBox (m_hWnd, str, g_szAppName, MB_YESNOCANCEL);
                if (nID == IDCANCEL)
                    return false;
                fDeleteTable = nID == IDYES;
            }
            else
                fPrompt = false;
        }
    }

    //
    // Update
    //

    HandleTableDeRef (pOldTable, fDeleteTable);
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle table global change
//
// @parm CTableObject * | pTable | Table being changed
//
// @parm bool | fGlobal | New setting
//
// @parm HTREEITEM | hItem | Tree view item of the table 
// 
// @rdesc TRUE if the new name has been handled
//
//-----------------------------------------------------------------------------

bool CMainWnd::HandleTableGlobalChange (CTableObject *pTable, 
                                        bool fGlobal, HTREEITEM hItem)
{

    //
    // Check for no change
    //

    if (pTable ->GetGlobal () == fGlobal)
        return true;

    //
    // If we are going global and have references
    //

    if (fGlobal && pTable ->GetRefCount () != 0)
    {
        AddTableToRoot (pTable);
    }

    //
    // If we are going local and have references
    //

    else if (!fGlobal && pTable ->GetRefCount () != 0)
    {

        //
        // Enum the root items and request a delete
        //

        HTREEITEM hParent;
        if (pTable ->GetTableType () == Table_Type_Treasure)
            hParent = m_pTreasure ->GetHItem ();
        else if (pTable ->GetTableType () == Table_Type_Encounter)
            hParent = m_pEncounter ->GetHItem ();
        else
            _ASSERTE (FALSE);
        HTREEITEM hItem = m_pTreasure ->GetHItem ();
        HTREEITEM hChild = m_tv .GetChildItem (hItem);
        while (hItem != NULL)
        {
            if (pTable ->DeleteFromTV (this, m_tv, hChild))
                break;
            hChild = m_tv .GetNextSiblingItem (hChild);
        }
    }

    //
    // Save the new setting
    //

    pTable ->SetGlobal (fGlobal);
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle table name change
//
// @parm CTableObject * | pTable | Table being changed
//
// @parm LPCTSTR | pszNewName | New name of the table
//
// @parm HTREEITEM | hItem | Tree view item of the table 
// 
// @rdesc TRUE if the new name has been handled
//
//-----------------------------------------------------------------------------

bool CMainWnd::HandleTableNameChange (CTableObject *pTable, 
                                      LPCTSTR pszNewName, HTREEITEM hItem)
{
    CString str;

    //
    // Check for no change
    //

    if (pTable ->GetName () == CString (pszNewName))
        return true;

    //
    // Check for duplicate
    //

    CTableObject *pT1 = FindTable (pszNewName, Table_Type_Treasure);
    CTableObject *pT2 = FindTable (pszNewName, Table_Type_Encounter);
    if ((pT1 != NULL && pT1 != pTable) ||
        (pT2 != NULL && pT2 != pTable))
    {
        str .Format (IDS_ERR_DUPLICATE);
        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
        return false;
    }

    //
    // Change all the table refs
    //

    for (int i = 0; i < m_apTables .GetCount (); i++)
    {
        m_apTables [i] ->ChangeTableName (pTable, 
                                          pTable ->GetName (), CString (pszNewName));
    }
    pTable ->SetName (CString (pszNewName));
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a table de-ref
//
// @parm CTableObject * | pTable | Old table
//
// @parm bool | fDeleteTable | If true, table is to be deleted.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMainWnd::HandleTableDeRef (CTableObject *pTable, bool fDeleteTable)
{
    if (pTable)
    {
        pTable ->ReleaseRef ();
        if (pTable ->GetRefCount () == 0)
        {
            if (fDeleteTable)
            {
                for (int i = 0; i < m_apTables .GetCount (); i++)
                {
                    if (m_apTables [i] == pTable)
                    {
                        m_apTables .RemoveAt (i);
                        break;
                    }
                }
                delete pTable;
                pTable = NULL;
            }
            else if (!pTable ->GetGlobal ())
            {
                AddTableToRoot (pTable);
            }
        }
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Add a table to the root
//
// @parm CTableObject * | pTable | Table
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMainWnd::AddTableToRoot (CTableObject *pTable)
{
    if (pTable ->GetTableType () == Table_Type_Treasure)
    {
        HTREEITEM hParent = m_pTreasure ->GetHItem ();
        pTable ->AddToTV (m_tv, hParent, TVI_LAST);
        m_pTreasure ->SortTV (this, m_tv, hParent);
    }
    else if (pTable ->GetTableType () == Table_Type_Encounter)
    {
        HTREEITEM hParent = m_pEncounter ->GetHItem ();
        pTable ->AddToTV (m_tv, hParent, TVI_LAST);
        m_pEncounter ->SortTV (this, m_tv, hParent);
    }
    else if (pTable ->GetTableType () == Table_Type_Placeable)
    {
        HTREEITEM hParent = m_pPlaceable ->GetHItem ();
        pTable ->AddToTV (m_tv, hParent, TVI_LAST);
        m_pPlaceable ->SortTV (this, m_tv, hParent);
    }
    else if (pTable ->GetTableType () == Table_Type_Profile)
    {
        HTREEITEM hParent = m_pProfile ->GetHItem ();
        pTable ->AddToTV (m_tv, hParent, TVI_LAST);
        m_pProfile ->SortTV (this, m_tv, hParent);
    }
    else
        _ASSERTE (FALSE);
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle file new
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

LRESULT CMainWnd::OnFileNew (WORD wNotifyCode, 
                             WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    if (!QueryClose ())
        return TRUE;
    NewDocument (true);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle file save
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

LRESULT CMainWnd::OnFileSave (WORD wNotifyCode, 
                              WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    DoSave ();
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle file save as
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

LRESULT CMainWnd::OnFileSaveAs (WORD wNotifyCode, 
                                WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    DoSaveAs ();
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle file close
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

LRESULT CMainWnd::OnFileClose (WORD wNotifyCode, 
                               WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    if (!QueryClose ())
        return TRUE;
    m_fModified = false;
    m_strFile = _T ("");
    NewDocument (true);
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

LRESULT CMainWnd::OnFileOpen (WORD wNotifyCode, 
                              WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    CFileDialog dlg (TRUE, _T (".xml"), _T(""), 
                     OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                     g_pszXMLFilter);
    if (dlg .DoModal () == IDOK)
    {
        if (!QueryClose ())
            return TRUE;
        if (DoOpen (dlg .m_ofn .lpstrFile))
        {
            m_strFile = dlg .m_ofn .lpstrFile;
            m_mru .AddToList (dlg .m_ofn .lpstrFile);
            m_mru .WriteToRegistry (g_pszKeyRoot);
        }
    }
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
    // Allow user to save current
    //

    if (!QueryClose ())
        return TRUE;

    //
    // Get file name from the MRU list
    //

    TCHAR szFile [MAX_PATH];
    if(m_mru .GetFromList (wID, szFile))
    {
        if (DoOpen (szFile))
            m_mru .MoveToTop (wID);
        else
            m_mru .RemoveFromList (wID);
        m_strFile = szFile;
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
// @mfunc Save the current document
//
// @parm LPCTSTR | pszName | Name of the document
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

bool CMainWnd::SaveDocument (LPCTSTR pszName)
{
    USES_CONVERSION;

    //
    // Open the file
    //

    FILE *fp = fopen (T2CA (pszName), "wb");
    if (fp == NULL)
    {
        CString str;
        str .Format (IDS_ERR_OPEN_WRITE);
        ::MessageBox (NULL, str, g_szAppName, MB_OK);
        return false;
    }

    //
    // Start the document
    //

    fprintf (fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
    fprintf (fp, "<nwntreasure-data>\r\n");

    //
    // Write the tables
    //

    for (int i = 0; i < m_apTables .GetCount (); i++)
    {
        m_apTables [i] ->WriteXML (fp);
    }

    //
    // End the document
    //

    fprintf (fp, "</nwntreasure-data>\r\n");
    fclose (fp);
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle file save
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

bool CMainWnd::DoSave () 
{
    if (m_strFile .IsEmpty ())
    {
        return DoSaveAs ();
    }
    else
    {
        if (SaveDocument (m_strFile))
        {
            m_fModified = false;
            m_mru .AddToList (m_strFile);
            m_mru .WriteToRegistry (g_pszKeyRoot);
            return true;
        }
        else
            return false;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle file save as
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

bool CMainWnd::DoSaveAs () 
{
    BOOL bRet = FALSE;
    CFileDialog dlg (FALSE, _T (".xml"), _T(""), 
                     OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                     g_pszXMLFilter);
    if (dlg .DoModal () == IDOK)
    {
        if (SaveDocument (dlg .m_ofn .lpstrFile))
        {
            m_strFile = dlg .m_ofn .lpstrFile;
            m_fModified = false;
            m_mru .AddToList (m_strFile);
            m_mru .WriteToRegistry (g_pszKeyRoot);
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Open a new file
//
// @parm LPCTSTR | pszName | Name of the file
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

bool CMainWnd::DoOpen (LPCTSTR pszName) 
{
    USES_CONVERSION;

    //
    // Create a new document
    //

    NewDocument (false);

    //
    // Create the parser
    //

    CDataParser sParser (this);
    if (!sParser .Create ())
    {
        CString str (MAKEINTRESOURCE (IDS_ERR_EXPAT_CREATE));
        ::MessageBox (NULL, str, g_szAppName, MB_OK);
        return false;
    }

    //
    // Open the file
    //

    FILE *fp = fopen (T2CA (pszName), "r");
    if (fp == NULL)
    {
        CString str (MAKEINTRESOURCE (IDS_ERR_OPEN));
        ::MessageBox (NULL, str, g_szAppName, MB_OK);
        return false;
    }

    //
    // Loop while there is data
    //

    bool fSuccess = true;
    while (!feof (fp))
    {
        LPSTR pszBuffer = (LPSTR) sParser .GetBuffer (1024);
        if (pszBuffer == NULL)
            fSuccess = false;
        else
        {
            int nLength = (int) fread (pszBuffer, 1, 1024, fp);
            if (nLength < 0)
            {
                fSuccess = false;
                break;
            }
            if (nLength == 0)
                break;
            fSuccess = sParser .ParseBuffer (nLength, nLength == 0);
        }       
    }

    //
    // Close the file
    //

    fclose (fp);

    //
    // Check for errors
    //

    if (!fSuccess || !sParser .m_strError .IsEmpty ())
    {
        CString strError;
        int nLine, nColumn;
        if (!fSuccess)
        {
            strError = sParser .GetErrorString ();
            nLine = sParser .GetCurrentLineNumber ();
            nColumn = sParser .GetCurrentColumnNumber ();
        }
        else
        {
            strError = sParser .m_strError;
            nLine = sParser .m_nLineNumber;
            nColumn = sParser .m_nColumnNumber;
        }
        CString str;
        str .Format (IDS_ERR_XML, pszName,
                     (LPCTSTR) strError, nLine, nColumn);
        ::MessageBox (NULL, str, g_szAppName, MB_OK);
        NewDocument (true);
        return false;
    }
    else
    {

        //
        // Compute the refs
        //

        for (int i = 0; i < m_apTables .GetCount (); i++)
        {
            m_apTables [i] ->ComputeRefCounts (this);
        }

        //
        // Create profile table if not found
        //

        CreateProfileTable ();
        return true;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Allow the user to close the current doc
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

bool CMainWnd::QueryClose ()
{
    if (!m_fModified)
        return true;

    CString str;
    str .Format (IDS_ERR_PROMPT_SAVE);
    int nID = ::MessageBox (m_hWnd, str, g_szAppName, MB_YESNOCANCEL);
    if (nID == IDCANCEL)
        return false;
    if (nID == IDYES)
    {
        if (!DoSave ())
            return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Create a profile table
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

void CMainWnd::CreateProfileTable ()
{

    //
    // Search for a profile table
    //

    for (int i = 0; i < m_apTables .GetCount (); i++)
    {
        CTableObject *pTable = m_apTables [i];
        if (pTable ->GetTableType () == Table_Type_Profile)
            return;
    }

    //
    // Create if not found
    //

    CTableObject *pTable = new CTableObject ();
    pTable ->SetTableType (Table_Type_Profile);
    pTable ->SetGlobal (true);
    pTable ->SetName (CString (_T ("Profiles")));
    m_apTables .Add (pTable);
    m_pProfile ->AddNewItem (this, m_tv, pTable);
}


/* vim: set sw=4: */
