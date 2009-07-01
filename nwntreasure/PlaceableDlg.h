#ifndef ETS_PLACEABLEDLG_H
#define ETS_PLACEABLEDLG_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      PlaceableDlg.h - Edit a gold treasure |
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
// $History: PlaceableDlg.h $
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

class CPlaceableObject;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CPlaceableDlg : 
    public CDialogImpl <CPlaceableDlg>
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CPlaceableDlg ();

        // @cmember General destructor

        ~CPlaceableDlg ();

        // @cmember Create the dialog

        HWND Create (CPlaceableObject *pPlaceable, HWND hWndParent, RECT &rc);

        // @cmember Save changes

        bool SaveChanges ();

        // @access COM interface list
    public:
        enum { IDD = IDD_PLACEABLE };
        BEGIN_MSG_MAP (CPlaceableDlg)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)

            COMMAND_ID_HANDLER (IDC_ITEM_BLUEPRINT_BROWSE, OnBrowseItem)

            END_MSG_MAP ()

            // @access Message handlers
    protected:

            // @cmember Initialize the dialog box

            LRESULT OnInitDialog (UINT uMsg, WPARAM wParam,
                                  LPARAM lParam, BOOL &bHandled);

            // @cmember Browse for an item

            LRESULT OnBrowseItem (WORD wNotifyCode, 
                                  WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @access Protected members
    protected:

            // @cmember Object being edited

            CPlaceableObject    *m_pObject;

            // @cmember Blueprint

            CEdit                               m_editBlueprint;
};

#endif // ETS_PLACEABLEDLG_H

/* vim: set sw=4: */
