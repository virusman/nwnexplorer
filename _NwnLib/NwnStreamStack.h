#ifndef ETS_NWNSTREAMSTACK_H
#define ETS_NWNSTREAMSTACK_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnStreamStack.h - Stream stack support |
//
// This module contains the definition of the NwnStreamStack.
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

#include "NwnStreams.h"

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

class CNwnStreamStack
{
    struct Entry
    {
        CNwnStream *pStream;
        Entry *pNext;
    };

    // @access Constructors and destructors
    public:

    // @cmember General constructor

    CNwnStreamStack ()
    {
        m_pTop = NULL;
    }

    // @cmember Delete the streams

    ~CNwnStreamStack ()
    {
        while (m_pTop)
            RemoveTopStream ();
    }

    // @access Public methods
    public:

    // @cmember Get the top stream

    CNwnStream *GetCurrentStream ()
    {
        if (m_pTop == NULL)
            return NULL;
        else
            return m_pTop ->pStream;
    }

    // @cmember Add a new stream

    void AddStream (CNwnStream *pStream)
    {
        Entry *pEntry = new Entry;
        pEntry ->pNext = m_pTop;
        pEntry ->pStream = pStream;
        m_pTop = pEntry;
        return;
    }

    // @cmember Remove the top stream

    void RemoveTopStream ()
    {
        if (m_pTop == NULL)
            return;
        Entry *pEntry = m_pTop;
        m_pTop = pEntry ->pNext;
        delete pEntry ->pStream;
        delete pEntry;
        return;
    }

    // @cmember Protected members
    protected:

    // @cmember Current top of stream

    Entry                       *m_pTop;
};

#endif // ETS_NWNSTREAMSTACK_H

/* vim: set sw=4: */
