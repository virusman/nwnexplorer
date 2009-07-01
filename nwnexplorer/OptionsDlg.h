#ifndef ETS_OPTIONSDLG_H
#define ETS_OPTIONSDLG_H

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

#include "nwnexplorer.h"

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

class COptionsDlg : 
    public CDialogImpl <COptionsDlg>
{
    // @access Enumerations and types
    public:

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        COptionsDlg ();

        // @cmember General destructor

        ~COptionsDlg ();

        // @access Public methods
    public:

        // @access Public inline methods
    public:


        // @access COM interface list
    public:
        enum { IDD = IDD_OPTIONS };
        BEGIN_MSG_MAP (COptionsDlg)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)
            END_MSG_MAP ()

            // @access Command handlers
    public:

            // @access Message handlers
    protected:

            // @cmember Initialize the dialog box

            LRESULT OnInitDialog (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @access Protected members
    protected:

            CTreeViewCtrl       m_tv;
};

#endif // ETS_OPTIONSDLG_H

/* vim: set sw=4: */
