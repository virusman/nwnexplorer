#ifndef ETS_GOLDDLG_H
#define ETS_GOLDDLG_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      GoldDlg.h - Edit a gold treasure |
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
// $History: GoldDlg.h $
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

class CGoldObject;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CGoldDlg : 
    public CDialogImpl <CGoldDlg>
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CGoldDlg ();

        // @cmember General destructor

        ~CGoldDlg ();

        // @cmember Create the dialog

        HWND Create (CGoldObject *pGold, HWND hWndParent, RECT &rc);

        // @cmember Save changes

        bool SaveChanges ();

        // @access COM interface list
    public:
        enum { IDD = IDD_GOLD };
        BEGIN_MSG_MAP (CGoldDlg)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)
            END_MSG_MAP ()

            // @access Message handlers
    protected:

            // @cmember Initialize the dialog box

            LRESULT OnInitDialog (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @access Protected members
    protected:

            // @cmember Object being edited

            CGoldObject         *m_pObject;

            // @cmember Dice count edit

            CEdit                       m_editDieCount;

            // @cmember Dice count spinner

            CUpDownCtrl         m_spinDieCount;

            // @cmember Die edit

            CEdit                       m_editDie;

            // @cmember Die spinner

            CUpDownCtrl         m_spinDie;

            // @cmember Multiplier edit

            CEdit                       m_editMultiplier;
};

#endif // ETS_GOLDDLG_H

/* vim: set sw=4: */
