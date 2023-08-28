//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ContentWnd.cpp - Main window |
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
#include "ContentWnd.h"
#include "nwnexplorer.h"
#include "../_NwnLib/NwnDialogTlkFile.h"
#include "../_NwnLib/NwnBifFile.h"
#include "../_NwnLib/NwnHierarchy.h"
#include "HexView.h"
#include "TextView.h"
#include "SoundView.h"
#include "SoundSetView.h"
#include "HierarchyView.h"
#include "HierarchyRawView.h"
#include "ModelRawView.h"
#include "ModelView.h"
#include "ModelRawHierView.h"
#include "ScriptRawView.h"
#include "ITPView.h"

//
// Debug NEW
//

#if defined (_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Globals
//

extern CString g_strNwnDirectory;
extern CNwnDialogTlkFile g_sDialogTlkFile;
extern CCommandBarCtrl *g_pCommandBar;
extern CMainWnd *g_pMainWnd;
extern TCHAR g_szAppName [];

//-----------------------------------------------------------------------------
//
// @mfunc <c CContentWnd> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CContentWnd::CContentWnd ()
{
    m_pCurrent = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CContentWnd> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CContentWnd::~CContentWnd ()
{
    //
    // Delete all the files
    //

    for (int i = 0; i < m_apViews .GetCount (); i++)
        delete m_apViews [i];
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

LRESULT CContentWnd::OnCreate (UINT uMsg, 
                               WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

    //
    // Create the top tab
    //

    m_tcTop .Create (m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | 
                     WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CTCS_BOLDSELECTEDTAB | 
                     CTCS_SCROLL | CTCS_CLOSEBUTTON, 0, (HMENU) IDC_EXP_TABSTOP);
    m_nTopHeight = CalcTabAreaHeight (m_tcTop);

    //
    // Create the bottom tab
    //

    m_tcBottom .Create (m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | 
                        WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, (HMENU) IDC_EXP_TABSBOTTOM);
    m_nBottomHeight = CalcTabAreaHeight (m_tcBottom);

    //
    // Create the blank window
    //

    m_wndBlank .Create (m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | 
                        WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
    m_pwndCurrent = &m_wndBlank;

    //
    // Add two sample tables
    //

    //  m_tcBottom .InsertItem (m_tcBottom .GetItemCount (), _T ("Audio"));
    //  m_tcBottom .InsertItem (m_tcBottom .GetItemCount (), _T ("Binary"));
    //  m_tcTop .InsertItem (m_tcTop .GetItemCount (), _T ("nw_io_generic.nss"));
    //  m_tcTop .InsertItem (m_tcTop .GetItemCount (), _T ("nwscript.nss"));

    //
    // Return TRUE
    //

    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a size request
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

LRESULT CContentWnd::OnSize (UINT uMsg, 
                             WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

    //
    // Get the current rect
    //

    CRect rc;
    GetClientRect (&rc);

    //
    // Set the top tab
    //

    m_tcTop .SetWindowPos (NULL, rc .left, rc .top, 
                           rc .Width (), m_nTopHeight, SWP_NOACTIVATE | SWP_NOZORDER);
    m_tcBottom .SetWindowPos (NULL, rc .left, rc .bottom - m_nBottomHeight, 
                              rc .Width (), m_nBottomHeight, SWP_NOACTIVATE | SWP_NOZORDER);
    m_tcTop .UpdateLayout ();
    m_tcBottom .UpdateLayout ();

    //
    // Update the main window
    //

    m_pwndCurrent ->SetWindowPos (NULL, rc .left, m_nTopHeight,
                                  rc .Width (), rc .Height () - m_nTopHeight - m_nBottomHeight, 
                                  SWP_NOACTIVATE | SWP_NOZORDER);

    //
    // Pretend we didn't process
    //

    bHandled = FALSE;
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Given a window handle, calc the hight of the tab
//
// @parm CWindow & | wndTab | Tab control window
//
// @rdesc Height of the tab control
//
//-----------------------------------------------------------------------------

int CContentWnd::CalcTabAreaHeight (CWindow &wndTab)
{
    const int nNominalHeight = 24;
    const int nNominalFontLogicalUnits = 11;    // 8 point Tahoma with 96 DPI

    //
    // Initialize nFontLogicalUnits to the typical case
    // appropriate for CDotNetTabCtrl
    //

    LOGFONT lfIcon = { 0 };
    ::SystemParametersInfo (SPI_GETICONTITLELOGFONT, sizeof (lfIcon), &lfIcon, 0);
    int nFontLogicalUnits = -lfIcon. lfHeight;

    //
    // Use the actual font of the tab control
    //

    if(wndTab .IsWindow ())
    {
        HFONT hFont = wndTab .GetFont ();
        if (hFont != NULL)
        {
            CDC dc = wndTab .GetDC ();
            CFontHandle hFontOld = dc .SelectFont (hFont);
            TEXTMETRIC tm = { 0 };
            dc .GetTextMetrics (&tm);
            nFontLogicalUnits = tm .tmAscent;
            dc .SelectFont (hFontOld);
        }
    }

    return nNominalHeight + 
        ( ::MulDiv(nNominalHeight, nFontLogicalUnits, 
                   nNominalFontLogicalUnits) - nNominalHeight ) / 2;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle idle processing
//
// @parm CUpdateUIBase * | pUpdateUI | Pointer to update UI object
//
// @rdesc Idle return status.
//
//-----------------------------------------------------------------------------

BOOL CContentWnd::OnIdle (CUpdateUIBase *pUpdateUI)
{

    //
    // Enable/Disable the closes
    //

    bool fEnableClose = m_apViews .GetCount () > 0;
    pUpdateUI ->UIEnable (ID_RESOURCES_CLOSEVIEW, fEnableClose);
    pUpdateUI ->UIEnable (ID_RESOURCES_CLOSEALLVIEWS, fEnableClose);

    //
    // Enable/Disable the export text option
    //

    if (fEnableClose)
    {
        CCommonView *pView = m_pCurrent ->GetCurrentView ();
        pUpdateUI ->UIEnable (ID_RESOURCES_EXPORTTEXT, 
                              pView ->CanExportText ());
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Open a resource
//
// @parm LPCSTR | pszResName | Resource name
//
// @parm NwnResType | nResType | Resource type
//
// @rdesc TRUE if the resource is already opened.
//
//-----------------------------------------------------------------------------

bool CContentWnd::OpenExisting (LPCSTR pszResName, NwnResType nResType)
{

    //
    // Search the existing windows
    //

    for (int i = 0; i < m_apViews .GetCount (); i++)
    {
        CContainerView *pView = m_apViews [i];
        if (_stricmp (pszResName, pView ->GetResName ()) == 0 &&
            nResType == pView ->GetResType ())
        {
            ShowView (i, -1);
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Open a resource
//
// @parm LPCTSTR | pszResName | Resource name
//
// @parm NwnResType | nResType | Resource type
//
// @parm CData * | pFile | Memory file
//
// @rdesc None
//
//-----------------------------------------------------------------------------

void CContentWnd::OpenResource (LPCSTR pszResName, NwnResType nResType, 
                                CData *pFile)
{
    //
    // Create a new view
    //

    CContainerView *pView = new CContainerView (
        pFile, pszResName, nResType);
    switch (GetResourceClass (nResType, NULL))
    {
        case ResClass_GameData:
        case ResClass_Material:
        case ResClass_TextureInfo:
        case ResClass_Shader:
        case ResClass_Ini:
        case ResClass_Script:
        case ResClass_Debug_Script:
            pView ->AddView (new CTextView (pFile));
            break;

        case ResClass_Image:
        case ResClass_Texture:
        case ResClass_Texture_DDS:
        case ResClass_Graphical_Palette:
            pView ->AddView (new CModelView (pFile, nResType));
            break;

#if 0
        case ResClass_Model_Server:
            pView ->AddView (new CModelRawView (pFile, true));
            break;
#endif

        case ResClass_Model:
        case ResClass_Model_Walk_Door:
        case ResClass_Model_Walk_Place:
        case ResClass_Model_Walk_Tile:
        case ResClass_Model_Pheno: {
            CModelView *mv = new CModelView (pFile, nResType);

            pView ->AddView (mv);

            if (*((long *) pFile ->GetData ()) == 0) {
                pView ->AddView (new CModelRawView (pFile, true));
                pView ->AddView (new CModelRawHierView (pFile, true));
                pView ->AddView (new CModelRawView (pFile, false));
                pView ->AddView (new CModelRawHierView (pFile, false));
            }
            else
            {
                CData *pCompiled = *(mv ->GetCompiledModel());

                pView ->AddView (new CModelRawView (pFile, true));

                if (pCompiled != NULL)
                    pView ->AddView (new CModelRawHierView (pCompiled, true, true));

                if (pCompiled != NULL) {
                    pView ->AddView (new CModelRawView (pCompiled, false));
                    pView ->AddView (new CModelRawHierView (pCompiled, false, true));
                }
            }
        }
        break;

        case ResClass_Sound:
            pView ->AddView (new CSoundView (pFile));
            break;

        case ResClass_Sound_Set:
            pView ->AddView (new CSoundSetView (pFile));
            break;

        case ResClass_Blueprint_Palette:
            pView ->AddView (new CITPView (pFile));
            pView ->AddView (new CHierarchyView (pFile));
            pView ->AddView (new CHierarchyRawView (pFile));
            break;

        case ResClass_Dialog:
        case ResClass_BP_Waypoint:
        case ResClass_BP_Trigger:
        case ResClass_BP_Sound:
        case ResClass_BP_Placeable:
        case ResClass_BP_Item:
        case ResClass_BP_Encounter:
        case ResClass_BP_Door:
        case ResClass_BP_Creatures:
        case ResClass_BP_Merchant:
        case ResClass_UI:
        case ResClass_Area:
        case ResClass_Module:
        case ResClass_Plot:
            pView ->AddView (new CHierarchyView (pFile));
            pView ->AddView (new CHierarchyRawView (pFile));
            break;

        case ResClass_Compiled_Script:
            pView ->AddView (new CScriptRawView (pFile));
            break;

        default:
            if (nResType == NwnResType_BIC)
            {
                pView ->AddView (new CHierarchyView (pFile));
                pView ->AddView (new CHierarchyRawView (pFile));
            }
            break;
    }
    pView ->AddView (new CHexView (pFile));

    //
    // Create a new top tab
    //

    int nIndex = m_tcTop .InsertItem (
        m_tcTop .GetItemCount (), pszResName);

    //
    // Add the view
    //

    m_apViews .Add (pView);

    //
    // Make the given tag current
    //

    ShowView (nIndex, -1);
}

//-----------------------------------------------------------------------------
//
// @mfunc Show the given view
//
// @parm int | nIndex | View index
//
// @parm int | nMode | Mode index
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CContentWnd::ShowView (int nIndex, int nMode)
{

    //
    // If the view isn't changing, return
    //

    CContainerView *pView = m_apViews [nIndex];
    if (m_pCurrent == pView && nMode == pView ->GetCurrentMode ())
        return;

    //
    // Get the window for the current view
    //

    if (nMode == -1)
        nMode = pView ->GetCurrentMode ();
    pView ->SetCurrentMode (nMode);
    CWindow *pNew;
    {
        CWaitCursor sWC;
        pNew = pView ->GetWindow (m_hWnd, nMode);
    }

    //
    // Populate the bottom buttons
    //

    if (m_pCurrent != pView)
    {
        m_tcBottom .DeleteAllItems ();
        for (int i = 0; i < 100; i++)
        {
            TCHAR szText [256];
            if (!pView ->GetModeText (i, szText, _countof (szText)))
                break;
            m_tcBottom .InsertItem (i, szText);
        }
        m_tcBottom .SetCurSel (nMode);
    }

    //
    // Set the upper tag
    //

    m_tcTop .SetCurSel (nIndex);

    //
    // Set the window position
    //

    CRect rc;
    GetClientRect (&rc);
    pNew ->SetWindowPos (NULL, rc .left, m_nTopHeight,
                         rc .Width (), rc .Height () - m_nTopHeight - m_nBottomHeight, 
                         SWP_NOACTIVATE | SWP_NOZORDER);

    //
    // Change the windows
    //

    g_pMainWnd ->SetStatusBarText(NULL);

    if (m_pwndCurrent != pNew)
    {
        if (m_pwndCurrent)
            m_pwndCurrent ->ShowWindow (SW_HIDE);
    }
    pNew ->ShowWindow (SW_SHOW);
    pNew ->SetFocus ();
    m_pwndCurrent = pNew;
    m_pCurrent = pView;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Show the given view
//
// @parm CContainerView * | pView | The view
//
// @parm int | nMode | Mode index
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CContentWnd::ShowView (CContainerView *pView, int nMode)
{
    for (int i = 0; i < m_apViews .GetCount (); i++)
    {
        if (m_apViews [i] == pView)
        {
            ShowView (i, nMode);
            break;
        }
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle selection change in a tab control
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

LRESULT CContentWnd::OnSelChangeTabsTop (int idCtrl, 
                                         LPNMHDR pnmh, BOOL &bHandled)
{
    NMCTC2ITEMS *pCTItems = (NMCTC2ITEMS *) pnmh;
    ShowView (pCTItems ->iItem2, -1);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a tab close
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

LRESULT CContentWnd::OnCloseTabsTop (int idCtrl, 
                                     LPNMHDR pnmh, BOOL &bHandled)
{
    NMCTCITEM *pCTItems = (NMCTCITEM *) pnmh;
    CloseItem (pCTItems ->iItem);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle selection change in a tab control
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

LRESULT CContentWnd::OnSelChangeTabsBottom (int idCtrl,
                                            LPNMHDR pnmh, BOOL &bHandled)
{
    NMCTC2ITEMS *pCTItems = (NMCTC2ITEMS *) pnmh;
    ShowView (m_pCurrent, pCTItems ->iItem2);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Close the current view
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

LRESULT CContentWnd::OnResourcesCloseView (WORD wNotifyCode, 
                                           WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    CloseItem (m_tcTop .GetCurSel ());
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Close the current view
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

LRESULT CContentWnd::OnResourcesCloseAllViews (WORD wNotifyCode, 
                                               WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    CloseAllViews ();
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Export the text
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

LRESULT CContentWnd::OnResourcesExportText (WORD wNotifyCode, 
                                            WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

    //
    // If we have no current, return
    //

    if (m_pCurrent == NULL)
        return TRUE;

    //
    // Get the resource name
    //

    char szName [_MAX_PATH];
    strncpy (szName, m_pCurrent ->GetResName (), _countof (szName)); 
    strncat (szName, ".txt", _countof (szName)); 

    //
    // Invoke the dialog
    //

    CFileDialog dlg (FALSE, _T (""), szName, 
                     OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                     NULL);
    if (dlg .DoModal () == IDOK)
    {
        CCommonView *pView = m_pCurrent ->GetCurrentView ();
        FILE *fp = fopen (dlg .m_ofn .lpstrFile, "wb");
        if (fp == NULL || !pView ->ExportText (fp))
        {
            CString str (MAKEINTRESOURCE (IDS_ERR_EXPORT));
            ::MessageBox (NULL, str, g_szAppName, MB_OK);
        }
        if (fp)
            fclose (fp);
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Close all views
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CContentWnd::CloseAllViews () 
{
    m_tcTop .DeleteAllItems ();
    m_tcBottom .DeleteAllItems ();
    for (int i = 0; i < m_apViews .GetCount (); i++)
        delete m_apViews [i];
    m_apViews .RemoveAll ();
    m_pwndCurrent = &m_wndBlank;
    m_pwndCurrent ->ShowWindow (SW_SHOW);
    m_pCurrent = NULL;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Delete the given item
//
// @parm int | nItem | Item to be deleted
// 
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CContentWnd::CloseItem (int nItem)
{
    if (nItem >= 0 && nItem < m_apViews .GetCount ())
    {
        m_tcTop .DeleteItem (nItem);
        delete m_apViews [nItem];
        m_apViews .RemoveAt (nItem);
        int nNewSel = m_tcTop .GetCurSel ();
        m_pCurrent = NULL;
        m_pwndCurrent = NULL;
        if (nNewSel < 0)
        {
            m_pwndCurrent = &m_wndBlank;
            m_pwndCurrent ->ShowWindow (SW_SHOW);
            m_tcBottom .DeleteAllItems ();
            m_pCurrent = NULL;
        }
        else
        {
            ShowView (nNewSel, -1);
        }
    }
    return;
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

LRESULT CContentWnd::OnTabBarRClick (int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
    NMCTCITEM *pn = (NMCTCITEM *) pnmh;

    //
    // Load the popup menu
    //

    CMenu hMenu = ::LoadPopupMenu (_Module .GetResourceInstance (),
                                   IDR_POPUPS, 1);
    if (hMenu .m_hMenu == NULL)
        return TRUE;

    //
    // Change the selection
    //

    if (pn ->iItem != -1)
    {
        if (idCtrl == IDC_EXP_TABSTOP)
            m_tcTop .SetCurSel (pn ->iItem, true);
        else
            m_tcBottom .SetCurSel (pn ->iItem, true);
    }

    //
    // Track the popup
    //

    CPoint pt (pn ->pt);
    if (idCtrl == IDC_EXP_TABSTOP)
        m_tcTop .ClientToScreen (&pt);
    else
        m_tcBottom .ClientToScreen (&pt);
    g_pCommandBar ->TrackPopupMenu (hMenu, 
                                    TPM_LEFTALIGN | TPM_TOPALIGN, 
                                    pt .x, pt .y, NULL);
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Context menu
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

LRESULT CContentWnd::OnContextMenu (UINT uMsg, 
                                    WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{

    //
    // Load the popup menu
    //

    CMenu hMenu = ::LoadPopupMenu (_Module .GetResourceInstance (),
                                   IDR_POPUPS, 1);
    if (hMenu .m_hMenu == NULL)
        return TRUE;

    //
    // Get the point
    //

    POINT pt;
    pt .x = GET_X_LPARAM (lParam);
    pt .y = GET_Y_LPARAM (lParam);

    //
    // Track the popup
    //

    g_pCommandBar ->TrackPopupMenu (hMenu, 
                                    TPM_LEFTALIGN | TPM_TOPALIGN, 
                                    pt .x, pt .y, NULL);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Move one tab to the left
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

LRESULT CContentWnd::OnResourcesMoveLeft (WORD wNotifyCode, 
                                          WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    int nIndex = m_tcTop .GetCurSel ();

    if (nIndex > 0)
        ShowView(nIndex - 1, -1);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Move one tab to the right
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

LRESULT CContentWnd::OnResourcesMoveRight (WORD wNotifyCode, 
                                           WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    int nIndex = m_tcTop .GetCurSel ();

    if (nIndex < m_apViews .GetCount() - 1)
        ShowView(nIndex + 1, -1);
    return TRUE;
}

/* vim: set sw=4: */
