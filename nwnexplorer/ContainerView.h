#ifndef ETS_CONTAINERVIEW_H
#define ETS_CONTAINERVIEW_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ContainerView.h - Container view for all the different views |
//
// This module contains the definition for all the different views.
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
// $History: ContainerView.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "DataRef.h"
#include "CommonView.h"

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

class CContainerView
{

    // @access Construction and destruction
    public:

        // @cmember General constructor

        CContainerView (CData *pFile, const CString &strResName,
                        NwnResType nResType);

        // @cmember General destructor

        virtual ~CContainerView ();

        // @access Public inline methods
    public:

        // @cmember Get the support formats/button text

        bool GetModeText (int nMode, LPTSTR pszText, int cchMaxText)
        {
            if (nMode < 0 || nMode >= m_apViews .GetCount ())
                return false;
            return m_apViews [nMode] ->GetModeText (pszText, cchMaxText);
        }

        // @cmember Get the given window

        CWindow *GetWindow (HWND hWnd, int nMode)
        {
            if (nMode < 0 || nMode >= m_apViews .GetCount ())
                return NULL;
            return m_apViews [nMode] ->GetWindow (hWnd);
        }

        // @cmember Add a new view

        void AddView (CCommonView *pView)
        {
            m_apViews .Add (pView);
        }

        // @cmember Get the resource name

        const CString &GetResName () const
        {
            return m_strResName;
        }

        // @cmember Get the resource type

        NwnResType GetResType () const
        {
            return m_nResType;
        }

        // @cmember Get the view new window flag

        bool GetViewNewWindow () const
        {
            return m_fViewNewWindow;
        }

        // @cmember Get the current mode

        int GetCurrentMode () const
        {
            return m_nCurrentMode;
        }

        // @cmember Set the current mode

        void SetCurrentMode (int nCurrentMode)
        {
            m_nCurrentMode = nCurrentMode;
        }

        // @cmember Get the current view

        CCommonView *GetCurrentView ()
        {
            return m_apViews [m_nCurrentMode];
        }

        // @access Protected members
    protected:

        // @cmember Name of the resource

        CString                         m_strResName;

        // @cmember Resource type

        NwnResType                      m_nResType;

        // @cmember User view new window flag

        bool                            m_fViewNewWindow;

        // @cmember Pointer to the memory file

        CDataRef                        m_sRes;

        // @cmember Array of views

        CAtlArray <CCommonView *> m_apViews;

        // @cmember Current mode

        int                                     m_nCurrentMode;
};

#endif // ETS_CONTAINERVIEW_H


/* vim: set sw=4: */
