#ifndef ETS_NWNSOUNDSET_H
#define ETS_NWNSOUNDSET_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnSoundSet.h - Nwn sound set (SSF) file support |
//
// This module contains the definition of the NwnSoundSet.
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
// $History: CnfMainWnd.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "NwnMemoryFile.h"

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

template <class TClass>
class CNwnSoundSetImpl
{
    public:

        struct Header 
        {
            char        achSig [4];
            char        achVersion [4];
            UINT32      ulSoundCount;
            UINT32      ulSoundOffset;
            UINT32      ulUnknown [6];
        };

        struct SoundOffset
        {
            UINT32      ulOffset;
        };

        struct Sound
        {
            char        szName [16];
            UINT32      ulStrRes;
        };

        // @access Public inline methods
    public:

        // @cmember Get the header

        Header *GetHeader () 
        {
            TClass *pT = static_cast <TClass *> (this);
            return (Header *) pT ->GetData (0);
        }

        // @cmember Get the sound count

        int GetSoundCount () 
        {
            Header *pHeader = GetHeader ();
            return (int) pHeader ->ulSoundCount;
        }

        // @cmember Get the n'th sound offset

        SoundOffset *GetSoundOffset (int nIndex) 
        {
            TClass *pT = static_cast <TClass *> (this);
            Header *pHeader = GetHeader ();
            _ASSERTE (nIndex >= 0 && nIndex < (int) pHeader ->ulSoundCount);
            SoundOffset *pasSoundOffset = (SoundOffset *)
                pT ->GetData (pHeader ->ulSoundOffset);
            return &pasSoundOffset [nIndex];
        }

        // @cmember Get the n'th sound

        Sound *GetSound (int nIndex) 
        {
            TClass *pT = static_cast <TClass *> (this);
            SoundOffset *pSO = GetSoundOffset (nIndex);
            return (Sound *) pT ->GetData (pSO ->ulOffset);
        }
};

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CNwnSoundSet :
    public CNwnSoundSetImpl <CNwnSoundSet>,
    public CNwnMemoryFile
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnSoundSet (unsigned char *pauchData = NULL, 
                      unsigned long ulSize = 0, bool fAllocated = true) :
            CNwnMemoryFile (pauchData, ulSize, fAllocated)
    {
    }
};

#endif // ETS_NWNSOUNDSET_H

/* vim: set sw=4: */
