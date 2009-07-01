//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      SoundView.cpp - Image view window |
//
// This module contains the definition of the image view window.
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
// $History: SoundView.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "SoundView.h"
#include "nwnexplorer.h"

//
// Debug NEW
//

#if defined (_DEBUG) && !defined (_WIN32_WCE)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern TCHAR g_szAppName [];
FSOUND_STREAM *g_pLastStream = NULL;
extern HMODULE g_hFMODLibrary;

//-----------------------------------------------------------------------------
//
// @mfunc <c CSoundView> constructor.
//
// @parm CData * | pFile | Memory file
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CSoundView::CSoundView (CData *pFile)
{

    //
    // Initialize variables
    //

    m_sRes .Set (pFile);
    m_pStream = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CSoundView> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CSoundView::~CSoundView ()
{
    if (m_pStream)
    {
        FSOUND_Stream_Stop (m_pStream);
        FSOUND_Stream_Close (m_pStream);
    }
    if (m_pStream == g_pLastStream)
        g_pLastStream = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a request to play the wave
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

LRESULT CSoundView::OnPlay (WORD wNotifyCode, 
                            WORD wID, HWND hWndCtl, BOOL &bHandled)
{

    //
    // Make sure FMOD is loaded
    //

    if (g_hFMODLibrary == NULL)
    {
        CString str;
        str .Format (IDS_ERR_NO_FMOD);
        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
        return TRUE;
    }

    //
    // If we don't have a current stream
    //

    if (g_pLastStream == NULL)
    {

        //
        // If our stream hasn't been built, build it
        //

        if (m_pStream == NULL)
        {
            char *pData = (char *) m_sRes .GetData ();
            int nSize = (int) m_sRes .GetSize ();
            if (pData [0] == 'B' && pData [1] == 'M' && 
                pData [2] == 'U' && pData [3] == ' ')
            {
                pData += 8;
                nSize -= 8;
            }
            m_pStream = FSOUND_Stream_Open (pData,
                                            FSOUND_NORMAL | FSOUND_LOADMEMORY, 0, nSize);
            if (m_pStream != NULL)
            {
                FSOUND_Stream_SetEndCallback (m_pStream, 
                                              (FSOUND_STREAMCALLBACK)StreamCallback, (void *)this);
            }
        }

        //
        // If we created the stream, play it
        //

        if (m_pStream != NULL)
        {
            int nChannel = FSOUND_Stream_PlayEx (
                FSOUND_FREE, m_pStream, NULL, FALSE);
            g_pLastStream = m_pStream;
        }
        else
        {
            CString str;
            str .Format (IDS_ERR_FMOD_STREAM);
            ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
            return TRUE;
        }
    }

    //
    // Otherwise, stop the stream
    //

    else
    {
        FSOUND_Stream_Stop (g_pLastStream);
        g_pLastStream = NULL;
    }

    //
    // Update the button
    //

    OnTimer (0, 0, 0, bHandled);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Process a timer
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

LRESULT CSoundView::OnTimer (UINT uMsg, WPARAM wParam, 
                             LPARAM lParam, BOOL &bHandled)
{
    CString strCur;
    CWindow wnd = GetDlgItem (IDC_PLAY);
    wnd .GetWindowText (strCur);
    CString strNew (g_pLastStream != NULL ? "Stop" : "Play");
    if (strCur != strNew)
        wnd .SetWindowText (strNew);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc FMOD Stream callback
//
// @parm FSOUND_STREAM | stream | Pointer to the stream
//
// @parm void * | buff | Data buffer
//
// @parm int | len | Length of the buffer
//
// @parm int | param | User supplied parameter
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

signed char F_CALLBACKAPI CSoundView::StreamCallback (
    FSOUND_STREAM *stream, void *buff, int len, void *userdata)
{
    g_pLastStream = NULL;
    return 0;
}


/* vim: set sw=4: */
