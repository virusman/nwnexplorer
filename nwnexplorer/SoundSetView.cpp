//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      SoundSetView.cpp - Sound set view window |
//
// This module contains the definition of the sound set view window.
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
// $History: SoundSetView.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "SoundSetView.h"
#include "../_NwnLib/NwnDialogTlkFile.h"

//
// Debug NEW
//

#if defined (_DEBUG) && !defined (_WIN32_WCE)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Globals
//

extern CNwnDialogTlkFile g_sDialogTlkFile;

//-----------------------------------------------------------------------------
//
// @mfunc <c CSoundSetView> constructor.
//
// @parm CData * | pFile | Sound set file
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CSoundSetView::CSoundSetView (CData *pFile) : 
    CTextViewImpl (IDS_SOUND_SET)
{

    //
    // Initialize variables
    //

    m_sRes .Set (pFile);

    //
    // Format the text
    //

    m_strText .Format (IDS_SOUND_SET_SOUND_HEADER);
    for (int i = 0; i < m_sRes .GetSoundCount (); i++)
    {
        char szText [17];
        char szStrRes [256];
        const CResSoundSetRef::Sound *pSound = m_sRes .GetSound (i);
        strncpy (szText, pSound ->szName, _countof (szText));
        szText [_countof (szText) - 1] = 0;
        if (pSound ->ulStrRes != 0xFFFFFFFF && g_sDialogTlkFile .IsOpen ())
        {
            szStrRes [0] = '(';
            szStrRes [1] = 0;
            g_sDialogTlkFile .GetString (pSound ->ulStrRes, 
                                         &szStrRes [1], _countof (szStrRes) - 2);
            strcat (szStrRes, ")");
        }
        else
            szStrRes [0] = 0;
        CStringA str;
        str .Format (IDS_SOUND_SET_SOUND, i, szText, pSound ->ulStrRes, szStrRes);
        m_strText += str + "\r\n";
    }
    const CResSoundSetRef::Header *pHeader = m_sRes .GetHeader ();
    CString str;
    str .Format (IDS_SOUND_SET_SOUND_OTHER,
                 pHeader ->ulUnknown [0], pHeader ->ulUnknown [1], 
                 pHeader ->ulUnknown [2], pHeader ->ulUnknown [3], 
                 pHeader ->ulUnknown [4], pHeader ->ulUnknown [5]);
    m_strText += str;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CSoundSetView> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CSoundSetView::~CSoundSetView ()
{
}


/* vim: set sw=4: */
