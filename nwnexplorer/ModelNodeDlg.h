#ifndef ETS_MODELNODEDLG_H
#define ETS_MODELNODEDLG_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelNodeDlg.h - Model position dialog |
//
// This module contains the definition for model position dialog.
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
// $History: ModelNodeDlg.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "nwnexplorer.h"
#include "../_MathLib/Constants.h"

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

class CModelNodeDlg : 
    public CDialogImpl <CModelNodeDlg>,
    public CThemeImpl <CModelNodeDlg>
{
    // @access Enumerations and types
    public:

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CModelNodeDlg (ModelContext *pModelContext = NULL);

        // @cmember General destructor

        ~CModelNodeDlg ();

        // @access COM interface list
    public:
        enum { IDD = IDD_MODEL_NODE };
        BEGIN_MSG_MAP (CModelNodeDlg)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)
            MESSAGE_HANDLER (WM_DRAWITEM, OnDrawItem)
            MESSAGE_HANDLER (WM_TIMER, OnTimer)

            REFLECT_NOTIFICATIONS ();
        END_MSG_MAP ()

            // @access Message handlers
    protected:

            // @cmember Initialize the dialog box

            LRESULT OnInitDialog (UINT uMsg, 
                                  WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle draw item

            LRESULT OnDrawItem (UINT uMsg, 
                                WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle timer

            LRESULT OnTimer (UINT uMsg, 
                             WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                return TRUE;
            }

            // @access Protected members
    public:

            // @cmember Pointer to the model context

            ModelContext                        *m_pModelContext;

};

#endif // ETS_MODELNODEDLG_H

/* vim: set sw=4: */
