//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NmcContext.cpp - Compiler context support |
//
// This module contains the compiler context support.
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
// $History: Cnf.cpp $
//      
//-----------------------------------------------------------------------------

#include "Nmc.h"
#include "../_NwnLib/NwnMdlNodes.h"
#include "../_NwnLib/NwnKeyFile.h"
#include "../_NwnLib/NwnModel.h"

//-----------------------------------------------------------------------------
//
// @mfunc <c CNmcContext> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNmcContext::CNmcContext ()
{
    m_pStreamTop = NULL;
    m_nStreamDepth = 0;
    m_pCurrentGeometry = NULL;
    m_pCurrentModel = NULL;
    m_nWarnings = 0;
    m_nErrors = 0;
    m_fPurgeNullFaces = true;
    m_pCache = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNmcContext> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNmcContext::~CNmcContext ()
{
    while (m_nStreamDepth)
        RemoveTopStream ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the next token from the current line or NULL if out
//
// @rdesc Address of the token.
//
//-----------------------------------------------------------------------------

char *CNmcContext::GetNextToken ()
{

    //
    // If we have no stream, return nothing
    //

    if (m_pStreamTop == NULL)
        return NULL;

    //
    // If we pushed our current token, then return it
    //

    if (m_pStreamTop ->nTokenState == TokenState_Pushed)
    {
        m_pStreamTop ->nTokenState = TokenState_Current;
        return m_pStreamTop ->pszToken;
    }

    //
    // Otherwise, we need to get the next token
    //

    else
    {

        //
        // Initialize the new state
        //

        m_pStreamTop ->nTokenState = TokenState_None;
        assert (m_pStreamTop ->pszNextTokenPos != NULL);

        //
        // If we are at the end of a line, then return NULL
        //

        if (m_pStreamTop ->pszNextTokenPos == NULL)
            return NULL;

        //
        // Locate the first non-space character
        //

        while (*m_pStreamTop ->pszNextTokenPos != 0)
        {
            if (!isspace (*m_pStreamTop ->pszNextTokenPos))
                break;
            m_pStreamTop ->pszNextTokenPos++;
        }

        //
        // Copy the token into the token buffer
        //

        bool fWarned = false;
        int nChars = 0;
        while (*m_pStreamTop ->pszNextTokenPos != 0)
        {
            if (isspace (*m_pStreamTop ->pszNextTokenPos))
                break;
            if (nChars >= Max_Token_Length - 1)
            {
                if (!fWarned)
                {
                    GenerateWarning ("Token too long.");
                    fWarned = true;
                }
            }
            else
            {
                m_pStreamTop ->pszToken [nChars++] = 
                    *m_pStreamTop ->pszNextTokenPos;
            }
            m_pStreamTop ->pszNextTokenPos++;
        }
        m_pStreamTop ->pszToken [nChars] = 0;

        //
        // If the token is NULL, then return NULL.  Otherwise,
        // update our current state and return the token
        //

        if (m_pStreamTop ->pszToken [0] == 0)
            return NULL;
        else
        {
            m_pStreamTop ->nTokenState = TokenState_Current;
            return m_pStreamTop ->pszToken;
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Return the current token.  If there is no current token, then
//              the next token is retrived from the stream.
//
// @rdesc Address of the token.
//
//-----------------------------------------------------------------------------

char *CNmcContext::GetCurrentToken ()
{
    if (m_pStreamTop == NULL)
        return NULL;
    if (m_pStreamTop ->nTokenState == TokenState_Current)
        return m_pStreamTop ->pszToken;
    else
        return GetNextToken ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Load the resource
//
// @parm const char * | pszName | Name of the resource to load
//
// @parm NwnResType | nResType | Type of the resource
//
// @parm UINT32 * | pulSize | If loaded, populated with the resource size
//
// @parm bool * | pfAllocated | Pointer to a flag to received 
//              if the data is allocated or not.
//
// @rdesc Address of the allocated buffer.  Use free to free.
//
//-----------------------------------------------------------------------------

unsigned char *CNmcContext::LoadResource (const char *pszName, 
                                          NwnResType nResType, UINT32 *pulSize, bool *pfAllocated)
{


    //
    // First, try to load the file
    //

    char szText [512];
    strcpy (szText, pszName);
    strcat (szText, NwnGetResTypeExtension (nResType));
    unsigned char *pauchData = NwnLoadFile (szText, pulSize);
    if (pauchData != NULL)
    {
        if (pfAllocated)
            *pfAllocated = true;
        return pauchData;
    }

    //
    // Try the key files
    //

    for (int i = (int) m_vpKeyFiles .GetCount (); i-- > 0;)
    {
        if (m_vpKeyFiles [i] ->IsOpen ())
        {
            pauchData = m_vpKeyFiles [i] ->LoadRes (pszName, 
                                                    nResType, pulSize, pfAllocated);
            if (pauchData != NULL)
                return pauchData;
        }
    }
    return NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load the given model
//
// @parm const char * | pszModelName | Name of the model to load
//
// @rdesc Address of the model or NULL if it failed to load
//
//-----------------------------------------------------------------------------

CNwnMdlModel *CNmcContext::LoadModel (const char *pszName)
{

    //
    // Search the cache
    //

    if (m_pCache)
    {
        CNwnMdlModel *pModel = m_pCache ->FindModel (pszName);
        if (pModel)
            return pModel;
    }

    //
    // Load the file
    //

    bool fAllocated;
    UINT32 ulSize;
    unsigned char *pauchData = LoadResource (pszName, 
                                             NwnResType_MDL, &ulSize, &fAllocated);
    if (pauchData == NULL)
        return NULL;

    //
    // If the model is binary
    //

    if (ulSize > 12 &&
        pauchData [0] == 0 && pauchData [1] == 0 &&
        pauchData [2] == 0 && pauchData [3] == 0)
    {

        //
        // If the buffer isn't allocated, then copy
        //

        if (!fAllocated)
        {
            unsigned char *p = (unsigned char *) calloc (1, ulSize);
            if (p == NULL)
                return NULL;
            memcpy (p, pauchData, ulSize);
            pauchData = p;
            fAllocated = true;
        }

        //
        // Get pointer to all the information we are interested in
        //

        CNwnModel::Header *pHeader = 
            (CNwnModel::Header *) pauchData;
        unsigned char *pauchModelData = &
            pauchData [sizeof (CNwnModel::Header)];
        unsigned char *pauchRawData = &
            pauchData [sizeof (CNwnModel::Header) + 
            pHeader ->ulRawDataOffset];
        CNwnMdlModel *pModel = (CNwnMdlModel *) pauchModelData;

        //
        // Convert the absolute
        //

        pModel ->RelativeToAbsolute (pauchModelData, pauchRawData);

        //
        // If we have a cache, add the model
        //

        if (m_pCache)
            m_pCache ->AddModel (pModel, pszName);

        //
        // Return the model
        //

        return pModel;
    }

    //
    // Otherwise, this is ASCII
    //

    else
    {

        //
        // Create the stream
        //

        CNwnMemoryStream *pStream = new CNwnMemoryStream (
            pszName, pauchData, ulSize, fAllocated);

        //
        // Start a new compiler
        //

        CNmcContext sCtx;
        sCtx .AddStream (pStream);
        sCtx .SetCache (GetCache ());
        for (int i = 0; i < (int) m_vpKeyFiles .GetCount (); i++)
            sCtx .AddKeyFile (m_vpKeyFiles [i]);
        sCtx .SetPurgeNullFaces (GetPurgeNullFaces ());

        //
        // Compile
        //

        NmcParseModelFile (&sCtx);
        if (sCtx .GetErrors ())
        {
            GenerateError ("Supermodel failed to compile.");
            if (sCtx .GetCurrentModel ())
                delete sCtx .GetCurrentModel ();
            return NULL;
        }
        else
        {
            if (m_pCache)
                m_pCache ->AddModel (sCtx .GetCurrentModel (), pszName);
            return sCtx .GetCurrentModel ();
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Test to see if the texture is bumpmapped
//
// @parm const char * | pszName | Name of the texture
//
// @rdesc TRUE if the texture is bumpmapped
//
//-----------------------------------------------------------------------------

bool CNmcContext::IsTextureBumpmapped (const char *pszName)
{

    //
    // Search the cache
    //

    if (m_pCache)
    {
        bool fBumpmapped;
        if (m_pCache ->FindTexture (pszName, &fBumpmapped))
            return fBumpmapped;
    }

    //
    // If we have no key file, error
    //

    if (m_vpKeyFiles .GetCount () == 0)
        return false;

    //
    // Load the file
    //

    bool fAllocated;
    UINT32 ulSize;
    unsigned char *pauchData = LoadResource (pszName, 
                                             NwnResType_TXI, &ulSize, &fAllocated);
    if (pauchData == NULL)
        return false;

    //
    // Parse the file
    //

    bool fBumpmapped = false;
    CNwnMemoryStream *pStream = new CNwnMemoryStream 
        (pszName, pauchData, ulSize, fAllocated);
    CNmcContext sCtx;
    sCtx .AddStream (pStream);
    while (sCtx .ReadNextLine ())
    {
        char *pszToken = sCtx .GetNextToken ();
        if (pszToken != NULL &&
            stricmp (pszToken, "bumpmaptexture") == 0)
        {
            pszToken = sCtx .GetNextToken ();
            if (pszToken != NULL && pszToken [0] != 0 && 
                stricmp (pszToken, "NULL") != 0)
            {
                fBumpmapped = true;
                break;
            }
        }
    }

    //
    // Add to cache
    //

    if (m_pCache)
        m_pCache ->AddTexture (pszName, fBumpmapped);
    return fBumpmapped;
}

/* vim: set sw=4: */
