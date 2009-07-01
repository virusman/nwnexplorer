#ifndef ETS_PROFILEDLG_H
#define ETS_PROFILEDLG_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ProfileDlg.h - Edit a gold treasure |
//
// This module contains the definition for the gold dialog.
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
// $History: ProfileDlg.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CProfileObject;
class CMainWnd;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CProfileDlg : 
    public CDialogImpl <CProfileDlg>
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CProfileDlg (CProfileObject *pObject,   
                     CMainWnd *pMainWnd, HTREEITEM hItem);

        // @cmember General destructor

        ~CProfileDlg ();

        // @access COM interface list
    public:
        enum { IDD = IDD_PROFILE };
        BEGIN_MSG_MAP (CProfileDlg)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)

            COMMAND_ID_HANDLER (IDOK, OnOk)
            COMMAND_ID_HANDLER (IDCANCEL, OnCancel)

            COMMAND_ID_HANDLER (IDC_RESPAWN_NAME_BROWSE, OnBrowseRespawnName)
            COMMAND_ID_HANDLER (IDC_DEATHOPEN_SPAWN_BROWSE, OnBrowseDeathOpenSpawn)
            COMMAND_HANDLER (IDC_RESPAWN_TYPE, CBN_SELCHANGE, OnRespawnTypeChange) 
            END_MSG_MAP ()

            // @access Message handlers
    protected:

            // @cmember Handle the ok button

            LRESULT OnOk (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @cmember Handle the cancel button

            LRESULT OnCancel (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @cmember Handle a change in the selection

            LRESULT OnRespawnTypeChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
            {
                UpdateWindows ();
                return TRUE;
            }

            // @cmember Browse for an respawn name

            LRESULT OnBrowseRespawnName (WORD wNotifyCode, 
                                         WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @cmember Browse for an respawn name

            LRESULT OnBrowseDeathOpenSpawn (WORD wNotifyCode, 
                                            WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @cmember Initialize the dialog box

            LRESULT OnInitDialog (UINT uMsg, WPARAM wParam,
                                  LPARAM lParam, BOOL &bHandled);

            // @access Protected members
    protected:

            // @cmember Update the look of the windows

            void UpdateWindows ();

            // @access Protected members
    protected:

            // @cmember List of tables

            CMainWnd            *m_pMainWnd;

            // @cmember tree item handle

            HTREEITEM           m_hItem;

            // @cmember Object being edited

            CProfileObject      *m_pObject;

            // @cmember Profile name

            CEdit                       m_editName;

            // @cmember Respawn type

            CComboBox           m_cbRespawnType;

            // @cmember Respawn time static

            CStatic                     m_statRespawnTime;

            // @cmember Respawn time edit

            CEdit                       m_editRespawnTime;

            // @cmember Respawn time spin

            CUpDownCtrl         m_spinRespawnTime;

            // @cmember Respawn name static

            CStatic                     m_statRespawnName;

            // @cmember Respawn name combo

            CComboBox           m_cbRespawnName;

            // @cmember Respawn name browse

            CButton                     m_btnRespawnNameBrowse;

            // @cmember Deathopen spawn combo

            CComboBox           m_cbDeathOpenSpawn;

            // @cmember Deathopen spawn browse

            CButton                     m_btnDeathOpenSpawnBrowse;

            // @cmember Deathopen sound edit

            CEdit                       m_editDeathOpenSound;

            // @cmember Deathopen damage

            CEdit                       m_editDeathOpenDamage;

            // @cmember DeathOpen damage spinner

            CUpDownCtrl         m_spinDeathOpenDamage;

            // @cmember Treasure table

            CComboBox           m_cbTreasureTable;
};

#endif // ETS_PROFILEDLG_H

/* vim: set sw=4: */
