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
#include "nwntreasure.h"
#include "TBCustomizeDlg.h"
#include "Object.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CRootObject;
class CTableObject;
class CTableRefObject;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CMainWnd :
    public CFrameWindowImpl <CMainWnd>,
    public CUpdateUI <CMainWnd>,
    public CMessageFilter,
    public CIdleHandler
{
    // @access Enumerations
    public:

        DECLARE_FRAME_WND_CLASS (NULL, IDR_NWNTREASURE)

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

            // @cmember Get the table count

            size_t GetTableCount () const
            {
                return m_apTables .GetCount ();
            }

            // @cmember Get a table

            CTableObject *GetTable (size_t nTable)
            {
                return m_apTables [nTable];
            }

            // @cmember Find a table

            CTableObject *FindTable (LPCTSTR pszName, Table_Type nTableType);

            // @cmember Handle a table ref name change

            bool HandleTableRefNameChange (CTableRefObject *pTableRef, 
                                           LPCTSTR pszNewName, HTREEITEM hItem);

            // @cmember Handle a table global change

            bool HandleTableGlobalChange (CTableObject *pTable, 
                                          bool fGlobal, HTREEITEM hItem);

            // @cmember Handle a table name change

            bool HandleTableNameChange (CTableObject *pTable, 
                                        LPCTSTR pszNewName, HTREEITEM hItem);

            // @cmember Handle a table ref deletion

            bool HandleTableRefDelete (CTableRefObject *pTableRef, 
                                       HTREEITEM hItem, bool fPrompt);

            // @cmember Add a table

            void AddTable (CTableObject *pTable)
            {
                m_apTables .Add (pTable);
                return;
            }

            // @cmember Remove a table

            void RemoveTable (CTableObject *pTable)
            {
                for (int i = 0; i < m_apTables .GetCount (); i++)
                {
                    if (m_apTables [i] == pTable)
                    {
                        m_apTables .RemoveAt (i);
                        return;
                    }
                }
                return;
            }

            // @access ATL interface list
    public:

            BEGIN_MSG_MAP (CMainWnd)
                MESSAGE_HANDLER (WM_CREATE, OnCreate)
                MESSAGE_HANDLER (WM_ERASEBKGND, OnEraseBackground)
                MESSAGE_HANDLER (WM_DESTROY, OnDestroy)
                MESSAGE_HANDLER (WM_INITMENUPOPUP, OnInitMenuPopup)
                MESSAGE_HANDLER (WM_CLOSE, OnClose)

                NOTIFY_HANDLER (IDC_EXP_TREEVIEW, TVN_GETDISPINFOW, OnTVGetDispInfoW)
                NOTIFY_HANDLER (IDC_EXP_TREEVIEW, TVN_GETDISPINFOA, OnTVGetDispInfoA)
                NOTIFY_HANDLER (IDC_EXP_TREEVIEW, TVN_ITEMEXPANDING, OnTVItemExpanding)
                NOTIFY_HANDLER (IDC_EXP_TREEVIEW, TVN_SELCHANGED, OnTVSelChanged)
                NOTIFY_HANDLER (IDC_EXP_TREEVIEW, TVN_KEYDOWN, OnTVKeyDown)
                //       NOTIFY_HANDLER (IDC_EXP_TREEVIEW, NM_DBLCLK, OnNMDblClk);


                COMMAND_ID_HANDLER (ID_FILE_CLOSE, OnFileClose)
                COMMAND_ID_HANDLER (ID_FILE_NEW, OnFileNew)
                COMMAND_ID_HANDLER (ID_FILE_OPEN, OnFileOpen)
                COMMAND_ID_HANDLER (ID_FILE_SAVE, OnFileSave)
                COMMAND_ID_HANDLER (ID_FILE_SAVE_AS, OnFileSaveAs)
                COMMAND_RANGE_HANDLER (ID_FILE_MRU_FIRST, ID_FILE_MRU_LAST, OnFileRecent)

                COMMAND_ID_HANDLER (ID_EDIT_NEW_TABLE, OnEditNewTable);
            COMMAND_ID_HANDLER (ID_EDIT_MOVEUP, OnEditMove);
            COMMAND_ID_HANDLER (ID_EDIT_MOVEDOWN, OnEditMove);
            COMMAND_ID_HANDLER (ID_EDIT_NEW_AFTER, OnEditNewAfter);
            COMMAND_ID_HANDLER (ID_EDIT_NEW, OnEditNew);
            COMMAND_ID_HANDLER (ID_EDIT_DELETE, OnEditDelete);
            COMMAND_ID_HANDLER (ID_EDIT_PROPERTIES, OnEditProperties);
            COMMAND_ID_HANDLER (ID_EDIT_TABLEPROPERTIES, OnEditTableProperties);
            COMMAND_ID_HANDLER (ID_APP_EXIT, OnAppExit);
            COMMAND_ID_HANDLER (ID_APP_ABOUT, OnAppAbout);
            COMMAND_ID_HANDLER (ID_FILE_EXPORTSCRIPT, OnFileExportScript);

            COMMAND_ID_HANDLER (ID_POPUP_CUSTOMIZE_TOOLBAR, OnPopupCustomizeToolbar);

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

                CHAIN_MSG_MAP (CUpdateUI<CMainWnd>);
            CHAIN_MSG_MAP (CFrameWindowImpl<CMainWnd>);

            REFLECT_NOTIFICATIONS ();

            ALT_MSG_MAP (1)

                MESSAGE_HANDLER (WM_RBUTTONDOWN, OnTVRButtonDown)
                MESSAGE_HANDLER (WM_RBUTTONUP, OnTVRButtonUp)
                MESSAGE_HANDLER (WM_RBUTTONDBLCLK, OnTVRButtonDown)

                END_MSG_MAP ()

                BEGIN_UPDATE_UI_MAP (CMainWnd)
                UPDATE_ELEMENT (ID_FILE_CLOSE,                          UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_FILE_SAVE,                           UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_EDIT_NEW_TABLE,                      UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_EDIT_MOVEUP,                         UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_EDIT_MOVEDOWN,                       UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_EDIT_NEW_AFTER,                      UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_EDIT_NEW,                            UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_EDIT_DELETE,                         UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_EDIT_PROPERTIES,                     UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                UPDATE_ELEMENT (ID_EDIT_TABLEPROPERTIES,        UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
                END_UPDATE_UI_MAP ()

                // @access ATL message handlers
    public:

                // @cmember Handle post creation actions

                LRESULT OnCreate (UINT uMsg, WPARAM wParam, 
                                  LPARAM lParam, BOOL &bHandled);

                // @cmember Handle destroy

                LRESULT OnDestroy (UINT uMsg, WPARAM wParam, 
                                   LPARAM lParam, BOOL &bHandled);

                // @cmember Handle close

                LRESULT OnClose (UINT uMsg, WPARAM wParam, 
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

                // @cmember Get the display information

                LRESULT OnTVGetDispInfoW (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
                LRESULT OnTVGetDispInfoA (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

                // @cmember Handle item expansion

                LRESULT OnTVItemExpanding (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

                // @cmember Handle selection change

                LRESULT OnTVSelChanged (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

                // @cmember Handle a key down event

                LRESULT OnTVKeyDown (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

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

                // @cmember Handle script export

                LRESULT OnFileExportScript (WORD wNotifyCode, 
                                            WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Modify what the user has selected

                LRESULT OnEditProperties (WORD wNotifyCode, 
                                          WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Modify what the user has selected

                LRESULT OnEditTableProperties (WORD wNotifyCode, 
                                               WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Delete the object

                LRESULT OnEditDelete (WORD wNotifyCode, 
                                      WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Create a new object

                LRESULT OnEditNew (WORD wNotifyCode, 
                                   WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Create a new object

                LRESULT OnEditNewAfter (WORD wNotifyCode, 
                                        WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Move an item up or down

                LRESULT OnEditMove (WORD wNotifyCode, 
                                    WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Create a new object

                LRESULT OnEditNewTable (WORD wNotifyCode, 
                                        WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Create a new file

                LRESULT OnFileNew (WORD wNotifyCode, 
                                   WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Open an existing file

                LRESULT OnFileOpen (WORD wNotifyCode, 
                                    WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Save the file

                LRESULT OnFileSave (WORD wNotifyCode, 
                                    WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Save the file as

                LRESULT OnFileSaveAs (WORD wNotifyCode, 
                                      WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Open a recent file

                LRESULT OnFileRecent (WORD wNotifyCode, 
                                      WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @cmember Close the current file

                LRESULT OnFileClose (WORD wNotifyCode, 
                                     WORD wID, HWND hWndCtl, BOOL& bHandled);

                // @access Virtual ATL methods
    public:

                // @cmember Translate the messages

                virtual BOOL PreTranslateMessage (MSG *pMsg)
                {
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

                // @cmember Load the toolbar buttons

                void LoadToolbarButtons ();

                // @cmember Update the toolbar

                void UpdateToolbar ();

                // @cmember Update the status of a button

                void UpdateButton (TBBUTTON *pButton);

                // @cmember Save the toolbar buttons

                void SaveToolbarButtons ();

                // @cmember Create a new document

                void NewDocument (bool fCreateProfileTable);

                // @cmember Delete the current document

                void DeleteDocument ();

                // @cmember Create a new item

                void OnEditNew (WORD wID, CObject *pObject, HTREEITEM hInsertAfter);

                // @cmember Save the current document

                bool SaveDocument (LPCTSTR pszName);

                // @cmember Handle generic save

                bool DoSave ();

                // @cmember Handle generic save-as

                bool DoSaveAs ();

                // @cmember Handle generic open

                bool DoOpen (LPCTSTR pszName);

                // @cmember Query for user to close current

                bool QueryClose ();

                // @cmember Create the default profile table

                void CreateProfileTable ();

                // @cmember Handle the deref of a table

                void HandleTableDeRef (CTableObject *pTable, bool fDeleteTable);

                // @cmember Add the table to the root

                void AddTableToRoot (CTableObject *pTable);

                // @access Protected variables
    protected:

                // @cmember The REAL tool bar

                CToolBarCtrl                    m_wndToolBar;

                // @cmember Command bar (toolbar)

                CCommandBarCtrl                 m_wndCommandBar;

                // @cmember Extra customization dialog

                CTBCustomizeDlg                 m_sTBDlg;

                // @cmember Tree view 

                CContainedWindowT <CTreeViewCtrl> m_tv;

                // @cmember Report view icons

                CImageList                              m_ilList;

                // @cmember MRU

                CRecentDocumentList             m_mru;

                // @cmember Table icon

                int                                             m_nTableIcon;

                // @cmember Table noref icon

                int                                             m_nTableNoRefIcon;

                // @cmember Table global icon

                int                                             m_nTableGlobalIcon;

                // @cmember Encounter icon

                int                                             m_nEncounterIcon;

                // @cmember Item icon

                int                                             m_nItemIcon;

                // @cmember Gold icon

                int                                             m_nGoldIcon;

                // @cmember None icon

                int                                             m_nNoneIcon;

                // @cmember placeable icon

                int                                             m_nPlaceableIcon;

                // @cmember profile icon

                int                                             m_nProfileIcon;

                // @cmember Toolbar text style

                TB_TextStyle                    m_nTextStyle;

                // @cmember Toolbar icon style

                TB_IconStyle                    m_nIconStyle;

                // @cmember Scratch TV strings

                char                                    m_szaTVText [256];
                WCHAR                                   m_szwTVText [256];

                // @cmember Pointer to the root objects

                CRootObject                             *m_pTreasure;
                CRootObject                             *m_pEncounter;
                CRootObject                             *m_pPlaceable;
                CRootObject                             *m_pProfile;

                // @cmember Table array

                CAtlArray <CTableObject *>      m_apTables;

                // @cmember Current file name

                CString                                 m_strFile;

                // @cmember Modified flag

                bool                                    m_fModified;

                // @cmember This is a hack

                BOOL                                    m_fForceUpdate;
};

#endif // ETS_MAINWND_H

/* vim: set sw=4: */
