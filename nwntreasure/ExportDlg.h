#ifndef ETS_EXPORTDLG_H
#define ETS_EXPORTDLG_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ExportDlg.h - Export dialog |
//
// This module contains the definition of the export dialog.
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
// $History: ExportDlg.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "nwntreasure.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CMainWnd;
class CTableObject;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CExportDlg :
    public CFileDialogImpl <CExportDlg>
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CExportDlg ();

        // @cmember General destructor

        ~CExportDlg ();

        // @access Public static methods
    public:

        // @cmember Display and perform export

        static void DoExport (CMainWnd *pMainWnd);

        // @access ATL dialog support
    public:
        BEGIN_MSG_MAP (CExportDlg)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)
            MESSAGE_HANDLER (WM_SIZE, OnSize)

            COMMAND_HANDLER (IDC_EXPORT_STYLE, CBN_SELCHANGE, OnCBSelChange) 
            COMMAND_HANDLER (IDC_DEBUG_STYLE, CBN_SELCHANGE, OnCBSelChange) 

            CHAIN_MSG_MAP (CFileDialogImpl <CExportDlg>);
        END_MSG_MAP ()

            // @access Message handlers
    public:

            // @cmember Initialize the dialog

            LRESULT OnInitDialog (UINT uMsg, WPARAM wParam, 
                                  LPARAM lParam, BOOL &bHandled);

            // @cmember Handle a resize

            LRESULT OnSize (UINT uMsg, WPARAM wParam, 
                            LPARAM lParam, BOOL &bHandled)
            {
                RepositionControls ();
                bHandled = FALSE;
                return FALSE;
            }

            // @cmember Selection change

            LRESULT OnCBSelChange (WORD wNotifyCode, 
                                   WORD wID, HWND hWndCtl, BOOL& bHandled)
            {
                SaveExportStyle ();
                SaveDebugStyle ();
                return TRUE;
            }

            // @access Protected methods
    protected:

            // @cmember Reposition the controls

            void RepositionControls ();

            // @cmember Reposition a control

            void RepositionControl (CWindow &wnd, UINT nID, bool fSize);

            // @cmember Save the current export style

            void SaveExportStyle ()
            {
                m_nExportStyle = m_cbExportStyle .GetCurSel ();
                if (m_nExportStyle < 0)
                    m_nExportStyle = 0;
            }

            // @cmember Save the current debug style

            void SaveDebugStyle ()
            {
                m_nDebugStyle = m_cbDebugStyle .GetCurSel ();
                if (m_nDebugStyle < 0)
                    m_nDebugStyle = 0;
            }

            // @cmember Sort the table array

            static void SortTableArray (int nTables, CTableObject **ppTables);

            // @cmember Process references

            static int ProcessReferences (int nTables, CTableObject **ppTables);

            // @cmember Copy the resource to the file

            static bool CopyResource (FILE *fp, LPCTSTR pszResType, UINT nID);

            // @cmember Export the tables

            static void ExportTables (FILE *fp, CMainWnd *pMainWnd, 
                                      CTableObject **ppTables, int nTables, bool fDebug, 
                                      Table_Type nTableType);

            // @access Protected member variables
    protected:

            // @cmember Export style

            CComboBox                   m_cbExportStyle;

            // @cmember Export static

            CStatic                             m_statExportStyle;

            // @cmember Selected export style

            int                                 m_nExportStyle;

            // @cmember Debug style

            CComboBox                   m_cbDebugStyle;

            // @cmember Debug static

            CStatic                             m_statDebugStyle;

            // @cmember Selected debug style

            int                                 m_nDebugStyle;
};

#endif // ETS_EXPORTDLG_H

/* vim: set sw=4: */
