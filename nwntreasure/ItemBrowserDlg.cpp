//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ItemBrowserDlg.cpp - None treasure dialog |
//
// This module contains the definition None treasure dialog
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
// $History: ItemBrowserDlg.h $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "ItemBrowserDlg.h"
#include "../_NwnLib/NwnKeyFile.h"
#include "../_NwnLib/NwnDialogTlkFile.h"
#include "../_NwnLib/NwnModuleFile.h"
#include "../_NwnLib/NwnStdLoader.h"

//
// Global variables
//

extern TCHAR g_szAppName [];

//
// My globals
//

CString g_strNwnDirectory;
CString g_strNwnModuleDir;
CNwnStdLoader g_sStdLoader;
CNwnDialogTlkFile g_sDialogTlkFile;
CString g_strLastSelection;

//-----------------------------------------------------------------------------
//
// @mfunc <c CItemBrowserDlg> constructor.
//
// @parm Browse_What | nBrowseWhat | What are we browsing
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CItemBrowserDlg::CItemBrowserDlg (Browse_What nBrowseWhat)
{
    m_nBrowseWhat = nBrowseWhat;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CItemBrowserDlg> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CItemBrowserDlg::~CItemBrowserDlg ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Initialize the dialog
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

LRESULT CItemBrowserDlg::OnInitDialog (UINT uMsg, 
                                       WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    CString str;

    //
    // Center the dialog
    //

    CenterWindow ();

    //
    // Get the controls
    //

    m_tv = GetDlgItem (IDC_ITEMS);
    m_cbSource = GetDlgItem (IDC_SOURCE);
    m_cbBrowseFor = GetDlgItem (IDC_BROWSE_FOR);
    m_statBrowseFor = GetDlgItem (IDC_BROWSE_FOR_STATIC);

    //
    // Initialize browse for
    //

    switch (m_nBrowseWhat)
    {
        case Browse_What_Items:
            str .LoadString (IDS_BROWSE_FOR_ITEMS);
            m_cbBrowseFor .AddString (str);
            break;

        case Browse_What_Encounters:
            str .LoadString (IDS_BROWSE_FOR_ENCOUNTERS);
            m_cbBrowseFor .AddString (str);
            break;

        case Browse_What_Placeables:
            str .LoadString (IDS_BROWSE_FOR_PLACEABLES);
            m_cbBrowseFor .AddString (str);
            break;

        case Browse_What_EncPlace:
            str .LoadString (IDS_BROWSE_FOR_ENCOUNTERS);
            m_cbBrowseFor .AddString (str);
            str .LoadString (IDS_BROWSE_FOR_PLACEABLES);
            m_cbBrowseFor .AddString (str);
            break;
    }
    m_cbBrowseFor .SetCurSel (0);
    if (m_cbBrowseFor .GetCount () <= 1)
    {
        m_cbBrowseFor .EnableWindow (false);
        m_statBrowseFor .EnableWindow (false);
    }

    //
    // If we don't have a NWN directory, initialize
    //

    if (g_strNwnDirectory .IsEmpty ())
    {

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
                g_strNwnDirectory = szText;
                g_strNwnDirectory += _T ("\\");
            }
        }

        //
        // If we are still ok
        //

        if (lRet == ERROR_SUCCESS)
        {

            //
            // Open the key and dialog files
            //

            {
                CWaitCursor sWC;
                g_sStdLoader .Initialize (g_strNwnDirectory);
                g_sDialogTlkFile .Open (g_strNwnDirectory + _T ("dialog.tlk"));
            }

            //
            // Get the module directory
            //

            TCHAR szText [_MAX_PATH];
            ::GetPrivateProfileString (_T ("Alias"), _T ("Modules"),
                                       _T (""), szText, _countof (szText), 
                                       g_strNwnDirectory + _T ("nwn.ini"));
            if (szText [0])
            {
                g_strNwnModuleDir = g_strNwnDirectory + szText + _T ("\\");
            }
        }
    }

    //
    // If the NWN files opened
    //

    if (g_sDialogTlkFile .IsOpen ())
    {
        CString str;

        //
        // The combo box beings with NWN
        //

        str .LoadString (IDS_NWN);
        m_cbSource .AddString (str);

        //
        // Get a list of the mods
        //

        CAtlArray <CString> vstr;
        CString strPath = g_strNwnModuleDir + "*.mod";
        WIN32_FIND_DATA fileData;
        HANDLE hSearch = FindFirstFile (strPath, &fileData);
        if (hSearch != INVALID_HANDLE_VALUE)
        {
            while (true) 
            {
                vstr .Add (CString (fileData .cFileName));
                if (!FindNextFile (hSearch, &fileData)) 
                    break;
            }
            FindClose (hSearch);
        }

        //
        // Sort the list the stupid way
        //

        bool fSwapped = true;
        while (fSwapped)
        {
            fSwapped = false;
            for (int i = 0; i < ((int) vstr .GetCount ()) - 1; i++)
            {
                if (vstr [i] > vstr [i+1])
                {
                    fSwapped = true;
                    str = vstr [i];
                    vstr [i] = vstr [i+1];
                    vstr [i+1] = str;
                }
            }
        }

        //
        // Add the strings
        //

        for (int i = 0; i < vstr .GetCount (); i++)
            m_cbSource .AddString (vstr [i]);

        //
        // Set the current selection
        //

        int nSel = 0;
        if (!g_strLastSelection .IsEmpty ())
        {
            nSel = m_cbSource .FindStringExact (-1, g_strLastSelection);
            if (nSel < 0)
                nSel = 0;
        }
        m_cbSource .SetCurSel (nSel);

        //
        // Show the current data
        //

        ShowCurrent ();
    }

    //
    // Otherwise, display an error
    //

    else
    {
        CString str;
        str .LoadString (IDS_ERR_NWN_DIRECTORY);
        ::MessageBox (NULL, str, g_szAppName, MB_OK);
    }
    m_tv .SetFocus ();
    OnTVSelChanged (0, NULL, bHandled);
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user press Ok
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

LRESULT CItemBrowserDlg::OnOk (WORD wNotifyCode, 
                               WORD wID, HWND hWndCtl, BOOL& bHandled) 
{

    //
    // Get the current tree view item
    //

    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem == NULL)
        return TRUE;

    //
    // Get the text
    //

    CString str;
    m_tv .GetItemText (hItem, str);

    //
    // Extract the blueprint
    //

    TCHAR szText [128];
    LPTSTR psz = _tcschr (str, '[');
    if (psz == NULL)
        return TRUE;
    _tcsncpy (szText, psz + 1, _countof (szText));
    psz = _tcschr (szText, ']');
    if (psz == NULL)
        return TRUE;
    *psz = 0;
    m_strSelection = szText;    

    //
    // Save the current selection
    //

    int nSel = m_cbSource .GetCurSel ();
    if (nSel >= 0)
        m_cbSource .GetLBText (nSel, g_strLastSelection);
    EndDialog (IDOK);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user press cancel
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

LRESULT CItemBrowserDlg::OnCancel (WORD wNotifyCode, 
                                   WORD wID, HWND hWndCtl, BOOL& bHandled) 
{
    EndDialog (IDCANCEL);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a change in the action
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

LRESULT CItemBrowserDlg::OnSourceSelChange (WORD wNotifyCode, 
                                            WORD wID, HWND hWndCtl, BOOL &bHandled)
{
    ShowCurrent ();
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a change in the action
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

LRESULT CItemBrowserDlg::OnCbnSelchangeBrowseFor (WORD wNotifyCode, 
                                                  WORD wID, HWND hWndCtl, BOOL &bHandled)
{
    ShowCurrent ();
    return TRUE;
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

LRESULT CItemBrowserDlg::OnTVSelChanged (int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
{
    bool fEnable = false;

    //
    // Get the current tree view item
    //

    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem != NULL)
        fEnable = m_tv .GetItemData (hItem) != 0;
    ::EnableWindow (GetDlgItem (IDOK), fEnable);
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

LRESULT CItemBrowserDlg::OnNMDblClk (int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
{

    //
    // Get the currently selected item
    //

    HTREEITEM hItem = m_tv .GetSelectedItem ();
    if (hItem == NULL)
        return NULL;
    OnOk (0, 0, 0, bHandled);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Show the current combo setting
//
// @rdesc None
//
//-----------------------------------------------------------------------------

void CItemBrowserDlg::ShowCurrent ()
{
    HTREEITEM hSel = NULL;
    unsigned char *pauchData = NULL;

    //
    // Just put a big FAT ASS wait cursor around the whole thing
    //

    CWaitCursor sWC;

    //
    // Reset the treeview
    //

    m_tv .DeleteAllItems ();

    //
    // Get what we are browsing
    //

    Browse_What nBrowseWhat = m_nBrowseWhat;
    if (nBrowseWhat == Browse_What_EncPlace)
    {
        if (m_cbBrowseFor .GetCurSel () == 0)
            nBrowseWhat = Browse_What_Encounters;
        else
            nBrowseWhat = Browse_What_Placeables;
    }

    //
    // Get the current combo setting
    //

    int nSel = m_cbSource .GetCurSel ();
    if (nSel < 0)
        return;

    //
    // If this is the first selection, then we use the NWN files
    //

    UINT32 ulSize;
    bool fAllocated;
    if (nSel == 0)
    {
        if (nBrowseWhat == Browse_What_Items)
        {
            pauchData = g_sStdLoader .LoadResource ("itempalstd", 
                                                    NwnResType_ITP, &ulSize, &fAllocated);
        }
        else if (nBrowseWhat == Browse_What_Encounters)
        {
            pauchData = g_sStdLoader .LoadResource ("creaturepalstd", 
                                                    NwnResType_ITP, &ulSize, &fAllocated);
        }
        else if (nBrowseWhat == Browse_What_Placeables)
        {
            pauchData = g_sStdLoader .LoadResource ("placeablepalstd", 
                                                    NwnResType_ITP, &ulSize, &fAllocated);
        }
        else
            _ASSERTE (FALSE);
    }
    else
    {
        CString str;
        m_cbSource .GetLBText (nSel, str);
        CNwnModuleFile sMod;
        if (sMod .Open (g_strNwnModuleDir + str))
        {
            if (nBrowseWhat == Browse_What_Items)
            {
                pauchData = sMod .LoadRes ("itempalcus", 
                                           NwnResType_ITP, &ulSize, &fAllocated);
            }
            else if (nBrowseWhat == Browse_What_Encounters)
            {
                pauchData = sMod .LoadRes ("creaturepalcus", 
                                           NwnResType_ITP, &ulSize, &fAllocated);
            }
            else if (nBrowseWhat == Browse_What_Placeables)
            {
                pauchData = sMod .LoadRes ("placeablepalcus", 
                                           NwnResType_ITP, &ulSize, &fAllocated);
            }
            else
                _ASSERTE (FALSE);
        }
    }

    //
    // If nothing was found
    //

    if (pauchData == NULL)
    {
        CString str;
        str .LoadString (IDS_ERR_NOPALETTE);
        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
        return;
    }

    //
    // Load up the tree
    //

    CNwnHierarchy sPal (pauchData, ulSize, fAllocated);
    CNwnHierarchy::Entry *pEntry = sPal .GetEntry (0);
    UINT32 *plMap = sPal .GetMultiMapTable (pEntry);
    for (int j = 0; j < pEntry ->ulCount; j++)
    {
        int nElement = plMap [j];
        const CNwnHierarchy::Element *pElement = 
            sPal .GetElement (nElement);
        if (pElement ->ulType == CNwnHierarchy::ElementType_LIST)
        {
            UINT32 *plList = sPal .GetListTable (pElement);
            long lCount = *plList++;
            for (j = 0; j < lCount; j++)
                AddTree (sPal, plList [j], TVI_ROOT, hSel);
        }
    }

    //
    // If there is a selection, then select it
    //

    if (hSel != NULL)
    {
        m_tv .EnsureVisible (hSel);
        m_tv .SelectItem (hSel);
        m_strSelection .Empty ();
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Add elements to the tree
//
// @parm CNwnHierarchy & | sPal | Palette
//
// @parm int | nEntry | Entry number
//
// @parm HTREEITEM | hParent | Parent item
// 
// @parm HTREEITEM & | hSel | Selection item
//
// @rdesc None
//
//-----------------------------------------------------------------------------

void CItemBrowserDlg::AddTree (CNwnHierarchy &sPal, 
                               int nEntry, HTREEITEM hParent, HTREEITEM &hSel)
{
    char szName [64];
    char szBlueprint [64];
    int nList = -1;

    //
    // Init the string
    //

    szName [0] = szBlueprint [0] = 0;

    //
    // Get the entry in question
    //

    CNwnHierarchy::Entry *pEntry = sPal .GetEntry (nEntry);

    //
    // Collect information about what we need to add
    //

    UINT32 *plMap = sPal .GetMultiMapTable (pEntry);
    for (int j = 0; j < pEntry ->ulCount; j++)
    {
        int nElement = plMap [j];
        const CNwnHierarchy::Element *pElement = 
            sPal .GetElement (nElement);

        //
        // If this is a list, get the element
        //

        if (pElement ->ulType == CNwnHierarchy::ElementType_LIST)
        {
            nList = nElement;
        }

        //
        // Otherwise, inspect the variable name to get 
        // the other information
        //

        else 
        {

            //
            // Get the name
            //

            CNwnHierarchy::VarName *pVarName = 
                sPal .GetVarName (pElement ->ulVarName);

            //
            // If this is the strref, get the name
            //

            if (stricmp (pVarName ->szName, "STRREF") == 0)
            {
                g_sDialogTlkFile .GetString (pElement ->u32Data, 
                                             szName, _countof (szName));
            }

            //
            // If this is a name, get the name
            //

            else if (stricmp (pVarName ->szName, "NAME") == 0)
            {
                sPal .GetElementLongString (pElement, 
                                            szName, _countof (szName));
            }

            //
            // If this is a resref, get the blueprint
            //

            else if (stricmp (pVarName ->szName, "RESREF") == 0)
            {
                sPal .GetElementShortString (pElement, 
                                             szBlueprint, _countof (szBlueprint));
            }
        }
    }

    //
    // Generate the name
    //

    CString str (szName);
    if (szBlueprint [0] != 0)
    {
        str += _T (" [");
        str += szBlueprint;
        str += _T ("]");
    }

    //
    // Add to the tree view
    //

    TVINSERTSTRUCT tvi;
    tvi .hParent = hParent;
    tvi .hInsertAfter = TVI_LAST;
    tvi .item .mask = TVIF_PARAM | TVIF_TEXT;
    tvi .item .hItem = 0;
    tvi .item .state = 0;
    tvi .item .stateMask = 0;
    tvi .item .pszText = (LPTSTR) (LPCTSTR) str;
    tvi .item .cchTextMax = 0;
    tvi .item .iImage = 0;
    tvi .item .iSelectedImage = 0;
    tvi .item .cChildren = 0;
    tvi .item .lParam = (nList == -1);
    HTREEITEM hTI = m_tv .InsertItem (&tvi);

    //
    // If we found our selection
    //

    if (szBlueprint [0] && _tcsicmp (m_strSelection, szBlueprint) == 0)
        hSel = hTI;

    //
    // If we have a list, add this list
    //

    if (hTI != NULL && nList != -1)
    {
        const CNwnHierarchy::Element *pElement = sPal .GetElement (nList);
        UINT32 *plList = sPal .GetListTable (pElement);
        long lCount = *plList++;
        for (j = 0; j < lCount; j++)
            AddTree (sPal, plList [j], hTI, hSel);
    }
}

/* vim: set sw=4: */
