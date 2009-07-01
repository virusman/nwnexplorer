#ifndef ETS_NMCCONTEXT_H
#define ETS_NMCCONTEXT_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NmcContext.h - Model compiler context |
//
// This module contains the definition of the NmcContext.
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

#include "../_NwnLib/NwnStreams.h"
#include "../_NwnLib/NwnMdlGeometry.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CNwnKeyFile;
class CNwnMdlModel;
class CNmcCache;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CNmcContext
{
    enum Constants
    {
        Max_Line_Length         = 0x8000,
        Max_Token_Length        = 0x1000,
    };

    enum TokenState
    {
        TokenState_None         = 0,
        TokenState_Current      = 1,
        TokenState_Pushed       = 2,
    };

    struct Entry
    {
        CNwnStream *pStream;
        Entry *pNext;
        char *pszLine;
        char *pszToken;
        char *pszNextTokenPos;
        TokenState nTokenState;
        int nLine;
    };

    // @access Constructors and destructors
    public:

    // @cmember General constructor

    CNmcContext ();

    // @cmember Delete the streams

    ~CNmcContext ();

    // @access Public methods
    public:

    // @cmember Load the resource from the current dir or key file

    unsigned char *LoadResource (const char *pszName, 
                                 NwnResType nResType, UINT32 *pulSize, bool *pfAllocated);

    // @cmember Load a super model

    CNwnMdlModel *LoadModel (const char *pszModelName);

    // @cmember Test to see if texture is bumpmapped

    bool IsTextureBumpmapped (const char *pszName);

    // @cmember Get the next token in the line

    char *GetNextToken ();

    // @cmember Get the current token

    char *GetCurrentToken ();

    // @cmember Unget the current token

    void UnGetToken ()
    {
        assert (m_pStreamTop != NULL);
        assert (m_pStreamTop ->nTokenState == TokenState_Current);
        m_pStreamTop ->nTokenState = TokenState_Pushed;
    }

    // @cmember Read the next line

    bool ReadNextLine ()
    {

        //
        // While we have streams
        //

        while (m_pStreamTop != NULL)
        {

            //
            // Read the next line
            //

            m_pStreamTop ->nTokenState = TokenState_None;
            m_pStreamTop ->pszNextTokenPos = m_pStreamTop ->pszLine;
            m_pStreamTop ->nLine++;
            if (m_pStreamTop ->pStream ->ReadLine (
                    m_pStreamTop ->pszLine, Max_Line_Length))
                return true;

            //
            // Can't read, assume EOF (sigh) and pop top stream
            //

            RemoveTopStream ();
        }
        return false;
    }

    // @cmember Return the current line

    const char *GetCurrentLine ()
    {
        if (m_pStreamTop == NULL)
            return NULL;
        else
            return m_pStreamTop ->pszLine;
    }

    // @cmember Read a binary block from the top file

    bool ReadBinary (void *pData, size_t nSize)
    {
        if (m_pStreamTop == NULL)
            return false;

        //
        // Read the block
        //

        size_t nRead = m_pStreamTop ->pStream ->Read (pData, nSize);
        return nRead == nSize;
    }

    // @cmember Generate a warning

    void GenerateWarning (const char *pszText, ...)
    {
        va_list marker;
        va_start (marker, pszText);
        GenerateError ("Warning", pszText, marker);
        va_end (marker);
        m_nWarnings++;
    }

    // @cmember Generate an error

    void GenerateError (const char *pszText, ...)
    {
        va_list marker;
        va_start (marker, pszText);
        GenerateError ("Error", pszText, marker);
        va_end (marker);
        m_nErrors++;
    }

    // @access Public inline methods
    public:

    // @cmember Get the current model

    CNwnMdlModel *GetCurrentModel () const
    {
        return m_pCurrentModel;
    }

    // @cmember Set the current model

    void SetCurrentModel (CNwnMdlModel *pModel) 
    {
        m_pCurrentModel = pModel;
        m_pCurrentGeometry = pModel;
    }

    // @cmember Get a pointer to the current geometry

    CNwnMdlGeometryHeader *GetCurrentGeometry ()
    {
        return m_pCurrentGeometry;
    }

    // @cmember Set the current geometry

    void SetCurrentGeometry (CNwnMdlGeometryHeader *pGeometry)
    {
        m_pCurrentGeometry = pGeometry;
    }

    // @cmember Get the top stream

    CNwnStream *GetCurrentStream ()
    {
        if (m_pStreamTop == NULL)
            return NULL;
        else
            return m_pStreamTop ->pStream;
    }

    // @cmember Add a new stream

    void AddStream (CNwnStream *pStream)
    {
        Entry *pEntry = new Entry;
        pEntry ->pNext = m_pStreamTop;
        pEntry ->pStream = pStream;
        pEntry ->pszLine = new char [Max_Line_Length + Max_Token_Length];
        pEntry ->pszToken = &pEntry ->pszLine [Max_Line_Length];
        pEntry ->nTokenState = TokenState_None;
        pEntry ->pszNextTokenPos = NULL;
        pEntry ->nLine = 0;
        m_pStreamTop = pEntry;
        m_nStreamDepth++;
        return;
    }

    // @cmember Remove the top stream

    void RemoveTopStream ()
    {
        if (m_pStreamTop == NULL)
            return;
        m_nStreamDepth--;
        Entry *pEntry = m_pStreamTop;
        m_pStreamTop = pEntry ->pNext;
        delete pEntry ->pStream;
        delete [] pEntry ->pszLine;
        delete pEntry;
        return;
    }

    // @cmember Get the number of warnings

    int GetWarnings () const
    {
        return m_nWarnings;
    }

    // @cmember Get the number of errors

    int GetErrors () const
    {
        return m_nErrors;
    }

    // @cmember Get the purge faces flag

    bool GetPurgeNullFaces () const
    {
        return m_fPurgeNullFaces;
    }

    // @cmember Set the purge faces flag

    void SetPurgeNullFaces (bool fPurgeNullFaces)
    {
        m_fPurgeNullFaces = fPurgeNullFaces;
        return;
    }

    // @cmember Get the number of key files

    size_t GetKeyFileCount () const
    {
        return m_vpKeyFiles .GetCount ();
    }

    // @cmember Get the n'th key file

    CNwnKeyFile *GetNthKeyFile (int nKeyFile)
    {
        return m_vpKeyFiles [nKeyFile];
    }

    // @cmember Add a key file

    void AddKeyFile (CNwnKeyFile *pKeyFile)
    {
        m_vpKeyFiles .Add (pKeyFile);
    }

    // @cmember Get the cache

    CNmcCache *GetCache ()
    {
        return m_pCache;
    }

    // @cmember Set the cache

    void SetCache (CNmcCache *pCache)
    {
        m_pCache = pCache;
    }

    // @access Protected methods
    protected:

    // @cmember Generate an error

    void GenerateError (const char *pszType, 
                        const char *pszText, va_list marker)
    {
        if (m_pStreamTop == NULL)
            printf ("%s: ", pszType);
        else
        {
            printf ("%s(%d): %s: ", 
                    m_pStreamTop ->pStream ->GetFileName (), 
                    m_pStreamTop ->nLine, pszType);
        }
        vprintf (pszText, marker);
        printf ("\n");
    }

    // @cmember Protected members
    protected:

    // @cmember Current top of stream

    Entry                                       *m_pStreamTop;

    // @cmember Current stream depth

    int                                         m_nStreamDepth;

    // @cmember Pointer to the current model

    CNwnMdlModel                        *m_pCurrentModel;

    // @cmember Pointer to the current geometry

    CNwnMdlGeometryHeader       *m_pCurrentGeometry;

    // @cmember If true, purge NULL faces

    bool                                        m_fPurgeNullFaces;

    // @cmember Number of warnings

    int                                         m_nWarnings;

    // @cmember Number of errors

    int                                         m_nErrors;

    // @cmember Key file array

    CNwnArray <CNwnKeyFile *> m_vpKeyFiles;

    // @cmember Cache

    CNmcCache                           *m_pCache;
};

#endif // ETS_NMCCONTEXT_H

/* vim: set sw=4: */
