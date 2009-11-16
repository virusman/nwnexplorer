#ifndef ETS_MAINWND_H
#define ETS_MAINWND_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      MainWnd.h - Main window |
//
// This module contains the definition of the main window.
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
// $History: CnfMainWnd.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "resource.h"
#include "nwnexplorer.h"
#include "TBCustomizeDlg.h"
#include "../_NwnLib/NwnKeyFile.h"
#include "../_NwnLib/NwnModuleFile.h"
#include "ContentWnd.h"
#include "DataSource.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CMainWnd :
    public CFrameWindowImpl <CMainWnd>,
    public CUpdateUI <CMainWnd>,
    public CSplitterImpl <CMainWnd, true>,
    public CMessageFilter,
    public CIdleHandler
{
    // @access Enumerations
    public:

        DECLARE_FRAME_WND_CLASS (NULL, IDR_NWNEXPLORER)

            // @access Constructors and destructors
    public:

            // @cmember General constructor

            CMainWnd ();

            // @cmember General destructor

            ~CMainWnd ();

            // @access Public methods
    public:

            // @cmember Create the window

            HWND Create ();

            // @cmember Set the status bar text

            void SetStatusBarText (LPCTSTR pszText);

            // @access Public inline methods
    public:

            // @cmember Update the layout of the window

            void UpdateLayout (BOOL bResizeBars = TRUE)
            {
                if (m_hWnd)
                {
                    CFrameWindowImpl <CMainWnd> ::UpdateLayout (bResizeBars);
                    UpdateSplitter ();
                }
            }

            // @access ATL interface list
    public:

            typedef CSplitterImpl <CMainWnd, true> _SplitterClass;

            BEGIN_MSG_MAP (CMainWnd)
                NOTIFY_HANDLER (IDC_EXP_TREEVIEW, TVN_DELETEITEM, OnTVDeleteItem)
                MESSAGE_HANDLER (WM_CREATE, OnCreate)
                MESSAGE_HANDLER (WM_ERASEBKGND, OnEraseBackground)
                MESSAGE_HANDLER (WM_DESTROY, OnDestroy)
                MESSAGE_HANDLER (WM_INITMENUPOPUP, OnInitMenuPopup)

                NOTIFY_HANDLER (IDC_EXP_TREEVIEW, TVN_GETDISPINFO, OnTVGetDispInfo)
                NOTIFY_HANDLER (IDC_EXP_TREEVIEW, TVN_ITEMEXPANDING, OnTVItemExpanding)
                NOTIFY_HANDLER (IDC_EXP_TREEVIEW, TVN_SELCHANGED, OnTVSelChanged)
                NOTIFY_HANDLER (IDC_EXP_TREEVIEW, NM_DBLCLK, OnNMDblClk);

            COMMAND_ID_HANDLER (ID_FILE_OPEN, OnFileOpen)
                COMMAND_ID_HANDLER (ID_FILE_OPEN_BINARY, OnFileOpenBinary)
                COMMAND_ID_HANDLER (ID_FILE_OPTIONS, OnFileOptions)
                COMMAND_ID_HANDLER (ID_FILE_OPENNWN, OnFileOpenNwn)
                COMMAND_ID_HANDLER (ID_FILE_RELOAD, OnFileReload)
                COMMAND_RANGE_HANDLER (ID_FILE_MRU_FIRST, ID_FILE_MRU_LAST, OnFileRecent)
                COMMAND_ID_HANDLER (ID_APP_EXIT, OnAppExit);
            COMMAND_ID_HANDLER (ID_APP_ABOUT, OnAppAbout);
            COMMAND_ID_HANDLER (ID_RESOURCES_VIEW, OnResourceView);
            COMMAND_ID_HANDLER (ID_RESOURCES_VIEWINNEWWINDOW, OnResourceView);
            COMMAND_ID_HANDLER (ID_RESOURCES_EXPORT, OnResourceExport);
            COMMAND_ID_HANDLER (ID_RESOURCES_EXPORTTGA, OnResourceExportTga);
            COMMAND_ID_HANDLER (ID_RESOURCES_SEARCH, OnResourceSearch);
            COMMAND_ID_HANDLER (ID_RESOURCES_MOVEUP, OnResourceTreeMove);
            COMMAND_ID_HANDLER (ID_RESOURCES_MOVEDOWN, OnResourceTreeMove);
            COMMAND_ID_HANDLER (ID_TOGGLES_TILEFADE_1, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_TILEFADE_2, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_TILEFADE_4, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_ALPHA, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_BACKFACE, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_USE_DDS, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_EMITTERS, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_FULLAMBIENT, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_MESHOUTLINE, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_ORTHOGRAPHIC, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_PREFERDDS, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_USE_TGA, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_AABB, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_SHOWDUMMY, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_SHOWEMITTER, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_SHOWLIGHT, OnToggle);
            COMMAND_ID_HANDLER (ID_TOGGLES_SHOWROOT, OnToggle);

            COMMAND_ID_HANDLER (ID_POPUP_CUSTOMIZE_TOOLBAR, OnPopupCustomizeToolbar);

            NOTIFY_CODE_HANDLER (RBN_HEIGHTCHANGE, OnReBarHeightChanged)
                NOTIFY_CODE_HANDLER (TBN_HOTITEMCHANGE, OnHotItemChange)
                NOTIFY_CODE_HANDLER (TBN_DROPDOWN, OnTBDropDown)
                NOTIFY_HANDLER (ATL_IDW_TOOLBAR, NM_RCLICK, OnToolbarRClick)
                NOTIFY_HANDLER (ATL_IDW_TOOLBAR, TBN_QUERYINSERT, OnCustomizeToolbar)
                NOTIFY_HANDLER (ATL_IDW_TOOLBAR, TBN_QUERYDELETE, OnCustomizeToolbar)
                NOTIFY_HANDLER (ATL_IDW_TOOLBAR, TBN_GETBUTTONINFO, OnCustomizeToolbar)
                NOTIFY_HANDLER (ATL_IDW_TOOLBAR, TBN_INITCUSTOMIZE, OnCustomizeToolbar)
                NOTIFY_HANDLER (ATL_IDW_TOOLBAR, TBN_BEGINADJUST, OnCustomizeToolbar)
                NOTIFY_HANDLER (ATL_IDW_TOOLBAR, TBN_ENDADJUST, OnCustomizeToolbar)
                NOTIFY_HANDLER (ATL_IDW_TOOLBAR, TBN_TOOLBARCHANGE, OnCustomizeToolbar)
                NOTIFY_ID_HANDLER (ATL_IDW_TOOLBAR, OnCustomizeToolbar)

                NOTIFY_CODE_HANDLER (TTN_GETDISPINFOA, OnToolTipTextA)
                NOTIFY_CODE_HANDLER (TTN_GETDISPINFOW, OnToolTipTextW)

            CHAIN_MSG_MAP (_SplitterClass);
            CHAIN_MSG_MAP (CUpdateUI<CMainWnd>);
            CHAIN_MSG_MAP (CFrameWindowImpl<CMainWnd>);

            //
            // Reflect toolbar notifications up
            //

            if ((uMsg == WM_NOTIFY && 
                 m_wndToolBar .m_hWnd == ((LPNMHDR)lParam)->hwndFrom) ||
                (uMsg == WM_COMMAND))
            {
                ::SendMessage (m_wndContent .m_hWnd, OCM__BASE + uMsg, wParam, lParam);
            }

            REFLECT_NOTIFICATIONS ();

            ALT_MSG_MAP (1)
                MESSAGE_HANDLER (WM_SIZE, OnTVSize)
                MESSAGE_HANDLER (WM_SETFOCUS, OnTVSetFocus)
                MESSAGE_HANDLER (WM_RBUTTONDOWN, OnTVRButtonDown)
                MESSAGE_HANDLER (WM_RBUTTONUP, OnTVRButtonUp)
                MESSAGE_HANDLER (WM_RBUTTONDBLCLK, OnTVRButtonDown)
                END_MSG_MAP ()

            BEGIN_UPDATE_UI_MAP (CMainWnd)
                UPDATE_ELEMENT (ID_RESOURCES_CLOSEVIEW,                 UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_RESOURCES_CLOSEALLVIEWS,             UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_RESOURCES_VIEW,                      UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_RESOURCES_VIEWINNEWWINDOW,   UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_RESOURCES_EXPORT,            UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_RESOURCES_EXPORTTEXT,        UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_RESOURCES_EXPORTTGA,         UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                END_UPDATE_UI_MAP ()


                // @access ATL message handlers
    public:

                // @cmember Handle post creation actions

                LRESULT OnCreate (UINT uMsg, WPARAM wParam, 
                                  LPARAM lParam, BOOL &bHandled);

                // @cmember Handle destroy

                LRESULT OnDestroy (UINT uMsg, WPARAM wParam, 
                                   LPARAM lParam, BOOL &bHandled);

                // @cmember Don't erase the background

                LRESULT OnEraseBackground (UINT uMsg, WPARAM wParam, 
                                           LPARAM lParam, BOOL &bHandled)
                {
                    return 1;
                }

                // @cmember Handle menu popup

                LRESULT OnInitMenuPopup (UINT uMsg, WPARAM wParam, 
                                         LPARAM lParam, BOOL &bHandled);

                // @cmember Handle a resize

                LRESULT CMainWnd::OnTVSize (UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam, BOOL &bHandled);

                // @cmember Handle gaining focus

                LRESULT CMainWnd::OnTVSetFocus (UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam, BOOL &bHandled);

                // @cmember Handle a right button down

                LRESULT OnTVRButtonDown (UINT uMsg, WPARAM wParam, 
                                         LPARAM lParam, BOOL &bHandled);

                // @cmember Handle a right button up

                LRESULT OnTVRButtonUp (UINT uMsg, WPARAM wParam, 
                                       LPARAM lParam, BOOL &bHandled);

                // Notification handlers
    public:

                // @cmember Hot item change

                LRESULT OnHotItemChange (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

                // @cmember Handle drop down press

                LRESULT OnTBDropDown (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

                // @cmember Handle right click of toolbar

                LRESULT OnToolbarRClick (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

                // @cmember Handle toolbar customization messages

                LRESULT OnCustomizeToolbar (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

                // @cmember Handle toolbar tooltip text (overridden because I have 3 
                //              fields in my string.)

                LRESULT OnToolTipTextA(int idCtrl, LPNMHDR pnmh, BOOL& /*bHandled*/)
                {
                    LPNMTTDISPINFOA pDispInfo = (LPNMTTDISPINFOA)pnmh;
                    pDispInfo->szText[0] = 0;

                    if((idCtrl != 0) && !(pDispInfo->uFlags & TTF_IDISHWND))
                    {
                        char szBuff[256];
                        szBuff[0] = 0;
                        int nRet = ::LoadStringA(_Module.GetResourceInstance(), idCtrl, szBuff, 256);
                        for(int i = 0; i < nRet; i++)
                        {
                            if(szBuff[i] == '\n')
                            {
                                for (int j = i + 1; j < nRet; j++)
                                {
                                    if (szBuff [j] == '\n')
                                    {
                                        szBuff [j] = 0;
                                        break;
                                    }
                                }
                                lstrcpynA(pDispInfo->szText, &szBuff[i + 1], sizeof(pDispInfo->szText) 
                                          / sizeof(pDispInfo->szText[0]));
                                break;
                            }
                        }
#if (_WIN32_IE >= 0x0300)
                        if(nRet > 0)    // string was loaded, save it
                            pDispInfo->uFlags |= TTF_DI_SETITEM;
#endif //(_WIN32_IE >= 0x0300)
                    }

                    return 0;
                }

                // @cmember Handle toolbar tooltip text (overridden because I have 3 
                //              fields in my string.)

                LRESULT OnToolTipTextW(int idCtrl, LPNMHDR pnmh, BOOL& /*bHandled*/)
                {
                    LPNMTTDISPINFOW pDispInfo = (LPNMTTDISPINFOW)pnmh;
                    pDispInfo->szText[0] = 0;

                    if((idCtrl != 0) && !(pDispInfo->uFlags & TTF_IDISHWND))
                    {
                        wchar_t szBuff[256];
                        szBuff[0] = 0;
                        int nRet = ::LoadStringW(_Module.GetResourceInstance(), idCtrl, szBuff, 256);
                        for(int i = 0; i < nRet; i++)
                        {
                            if(szBuff[i] == L'\n')
                            {
                                for (int j = i + 1; j < nRet; j++)
                                {
                                    if (szBuff [j] == '\n')
                                    {
                                        szBuff [j] = 0;
                                        break;
                                    }
                                }
                                lstrcpynW(pDispInfo->szText, &szBuff[i + 1], sizeof(pDispInfo->szText)
                                          / sizeof(pDispInfo->szText[0]));
                                break;
                            }
                        }
#if (_WIN32_IE >= 0x0300)
                        if(nRet > 0)    // string was loaded, save it
                            pDispInfo->uFlags |= TTF_DI_SETITEM;
#endif //(_WIN32_IE >= 0x0300)
                    }

                    return 0;
                }

                // Command handlers
    public:

                // @cmember Handle the changing of a rebar's height

                LRESULT OnReBarHeightChanged (int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
                {
                    HWND hWnd = GetSplitterPane (SPLIT_PANE_LEFT);
                    if (hWnd)
                        ::InvalidateRect (hWnd, NULL, TRUE);
                    return 0;
                }

                // @cmember Get the display information

                LRESULT OnTVGetDispInfo (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

                // @cmember Delete an item

                LRESULT OnTVDeleteItem (int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
                {
                    return FALSE;
                }

                // @cmember Handle item expansion

                LRESULT OnTVItemExpanding (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

                // @cmember Handle selection change

                LRESULT OnTVSelChanged (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

                // @cmember Handle a double click

                LRESULT OnNMDblClk (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

                // @cmember Handle an app exit request

                LRESULT OnAppExit (WORD wNotifyCode, 
                                   WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Handle about box

                LRESULT OnAppAbout (WORD wNotifyCode, 
                                    WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Handle customize of toolbar

                LRESULT OnPopupCustomizeToolbar (WORD wNotifyCode, 
                                                 WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Open an existing file

                LRESULT OnFileOpen (WORD wNotifyCode, 
                                    WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Open an existing file in binary mode

                LRESULT OnFileOpenBinary (WORD wNotifyCode, 
                                          WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Reload the current file

                LRESULT OnFileReload (WORD wNotifyCode, 
                                      WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Open nwn file

                LRESULT OnFileOpenNwn (WORD wNotifyCode, 
                                       WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Open an existing file

                LRESULT OnFileRecent (WORD wNotifyCode, 
                                      WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember View a resource

                LRESULT OnResourceView (WORD wNotifyCode, 
                                        WORD wID, HWND hWndCtl, BOOL& bHandled);

                LRESULT OnResourceSearch (WORD wNotifyCode, 
                                          WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Export files

                LRESULT OnResourceExport (WORD wNotifyCode, 
                                          WORD wID, HWND hWndCtl, BOOL& bHandled);

                LRESULT OnResourceExportTga (WORD wNotifyCode, 
                                             WORD wID, HWND hWndCtl, BOOL& bHandled);

                LRESULT OnResourceTreeMove (WORD wNotifyCode, 
                                            WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember View the file options

                LRESULT OnFileOptions (WORD wNotifyCode, 
                                       WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Perform toggles
                //
                LRESULT OnToggle (WORD wNotifyCode, 
                                  WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Handle generic open

                bool DoOpen (LPCTSTR pszName, bool fBinary);

                // @cmember Open a resource

                void OpenResource (bool fNewWindow, DataElement *pElement);

                // @cmember Search for a resource

                BOOL SearchResources (const char *pszName, HTREEITEM hItem, HTREEITEM &hSkip, BOOL fRecurse = FALSE);

                // @access Virtual ATL methods
    public:

                // @cmember Translate the messages

                virtual BOOL PreTranslateMessage (MSG *pMsg)
                {
                    if (GetFocus () == m_tvsb .m_hWnd &&
                        pMsg->message == WM_KEYDOWN   &&
                        (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)) {

                        if (pMsg->wParam == VK_ESCAPE) {
                            ::SetFocus(m_tv .m_hWnd);
                        } else {
                            CString str;
                            HTREEITEM hSkip = m_tv .GetSelectedItem();

                            m_tvsb .GetWindowText(str);
                            SearchResources(str, NULL, hSkip);
                        }

                        return TRUE;
                    }

                    if (CFrameWindowImpl <CMainWnd>::PreTranslateMessage (pMsg))
                        return TRUE;
                    return FALSE;
                }

                // @cmember Idle update

                virtual BOOL OnIdle ();

                // @cmember Handle the final message

                virtual void OnFinalMessage (HWND hWnd);

                // @access Protected methods
    protected:

                // @cmember Update the splitter

                void UpdateSplitter ()
                {
                    RECT rc;
                    GetClientRect (&rc);
                    RECT rcBar;
                    ::GetClientRect (m_hWndToolBar, &rcBar);
                    rc .top = rcBar .bottom;
                    if (::IsWindowVisible (m_hWndStatusBar))
                    {
                        ::GetClientRect (m_hWndStatusBar, &rcBar);
                        rc .bottom -= rcBar .bottom;
                    }
                    SetSplitterRect (&rc);
                }

                // @cmember Load the profile

                void LoadProfile ();

                // @cmember Load the toolbar buttons

                void LoadToolbarButtons ();

                // @cmember Update the toolbar

                void UpdateToolbar ();

                // @cmember Update the status of a button

                void UpdateButton (TBBUTTON *pButton);

                // @cmember Save the toolbar buttons

                void SaveToolbarButtons ();

                // @cmember Delete the current document

                void DeleteDocument ();

                // @cmember Add  the item to the tree view

                HTREEITEM AddToTV (const void *psRes, HTREEITEM hParent);

                // @cmember Create the tree view

                void CreateTreeView ();

                // @cmember Delete tree view

                void DeleteTreeView (HWND hWnd, HTREEITEM hTreeItem, int &nCount);

                // @cmember Open NWN

                void OpenNWN ();

                // @cmember Tree view item comparison

                static int CALLBACK OnCompareFunc (LPARAM lParam1, 
                                                   LPARAM lParam2, LPARAM lParamSort)
                {
                    CMainWnd *pWnd = (CMainWnd *) lParamSort;
                    TCHAR szText1 [256];
                    TCHAR szText2 [256];
                    DataElement *psElement1 = (DataElement *) lParam1;
                    DataElement *psElement2 = (DataElement *) lParam2;
                    psElement1 ->pDataSource ->GetResourceText (
                        psElement1, szText1, _countof (szText1));
                    psElement2 ->pDataSource ->GetResourceText (
                        psElement2, szText2, _countof (szText2));
                    return _tcscmp (szText1, szText2);
                }

                // @access Protected variables
    protected:

                // @cmember The REAL tool bar

                CToolBarCtrl                    m_wndToolBar;

                // @cmember Command bar (toolbar)

                CCommandBarCtrl                 m_wndCommandBar;

                // @cmember Extra customization dialog

                CTBCustomizeDlg                 m_sTBDlg;

                // @cmember Tree view tree

                CContainedWindowT<CTreeViewCtrl>        m_tv;

                // @cmember Tree view search box

                CEdit m_tvsb;

                // @cmember Content window

                CContentWnd                             m_wndContent;

                // @cmember Report view icons

                CImageList                              m_ilList;

                // @cmember MRU

                CRecentDocumentList             m_mru;

                // @cmember Toolbar text style

                TB_TextStyle                    m_nTextStyle;

                // @cmember Toolbar icon style

                TB_IconStyle                    m_nIconStyle;

                // @cmember Scratch TV strings

                char                                    m_szTVText [256];

                // @cmember Current file name

                char                                    m_szCurrentFile [_MAX_PATH];

                // @cmember True if the current file was opened as a binary

                BOOL                                    m_fCurrentFileBinary;

                // @cmember This is a hack

                BOOL                                    m_fForceUpdate;

                // @cmember Open resources in a new tab by default

                BOOL                                    m_fResourceOpenNewTab;

                // @cmember Open resources with a single click

                BOOL                                    m_fResourceOpenSelected;

                // @cmember Redraw the tree view

                BOOL                                    m_fTvRedraw;

                // @cmember Current data source

                CAtlArray <CDataSource *> m_apSources;

                // @cmember Slow tree view delete list

                CAtlArray <HWND>                m_atvDelete;
};

#endif // ETS_MAINWND_H

/* vim: set sw=4: */
