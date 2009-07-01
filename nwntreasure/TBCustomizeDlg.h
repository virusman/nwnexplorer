#ifndef ETS_TBCUSTOMIZEDLG_H
#define ETS_TBCUSTOMIZEDLG_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TBCustomizeDlg.h - Extra toolbar customization |
//
// This module contains the definition for extra toolbar customization elements.
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
// $History: TBCustomizeDlg.h $
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

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CTBCustomizeDlg : 
    public CDialogImpl <CTBCustomizeDlg>
{
    // @access Enumerations and types
    public:

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CTBCustomizeDlg ();

        // @cmember General destructor

        ~CTBCustomizeDlg ();

        // @access Public methods
    public:

        // @cmember Set the current text style

        void SetTextStyle (TB_TextStyle nTextStyle);

        // @cmember Set the current icon style

        void SetIconStyle (TB_IconStyle nIconStyle);

        // @access Public inline methods
    public:

        // @cmember Get the selected text style

        TB_TextStyle GetTextStyle () const
        {
            return m_nTextStyle;
        }

        // @cmember Get the select icon style

        TB_IconStyle GetIconStyle () const
        {
            return m_nIconStyle;
        }

        // @access COM interface list
    public:
        enum { IDD = IDD_TBCUSTOMIZE };
        BEGIN_MSG_MAP (CTBCustomizeDlg)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)
            COMMAND_HANDLER (IDC_TBCUSTOMIZE_TEXT_OPTIONS, CBN_SELCHANGE, OnTextSelChange) 
            COMMAND_HANDLER (IDC_TBCUSTOMIZE_ICON_OPTIONS, CBN_SELCHANGE, OnIconSelChange) 
            END_MSG_MAP ()

            // @access Command handlers
    public:

            // @cmember Handle a change in the selection

            LRESULT OnTextSelChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);

            // @cmember Handle a change in the selection

            LRESULT OnIconSelChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);

            // @access Message handlers
    protected:

            // @cmember Initialize the dialog box

            LRESULT OnInitDialog (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @access Protected members
    protected:

            // @cmember Text style combo

            CComboBox                   m_cbTextStyle;

            // @cmember Icon style combo

            CComboBox                   m_cbIconStyle;

            // @cmember Current text type

            TB_TextStyle                m_nTextStyle;

            // @cmember Current icon style

            TB_IconStyle                m_nIconStyle;
};

#endif // ETS_TBCUSTOMIZEDLG_H

/* vim: set sw=4: */
