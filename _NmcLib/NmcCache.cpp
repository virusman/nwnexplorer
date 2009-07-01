//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NmcCache.cpp - Model/Texture cache |
//
// This module contains the model and texture cache support.
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

#include "NmcCache.h"
#include "../_NwnLib/NwnMdlGeometry.h"

//-----------------------------------------------------------------------------
//
// @mfunc <c CNmcCache> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNmcCache::CNmcCache ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNmcCache> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNmcCache::~CNmcCache ()
{
    ClearCache ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Add a model to the cache
//
// @parm CNwnMdlModel * | pModel | Model to add
//
// @parm const char * | pszModelName | Model name
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNmcCache::AddModel (CNwnMdlModel *pModel, const char *pszName)
{

    //
    // Lowercase the string 
    //

    size_t nSize = strlen (pszName);
    char *pszLower = (char *) alloca (nSize + 1);
    strcpy (pszLower, pszName);
    strlwr (pszLower);

    //
    // Addref the model
    //

    pModel ->AddRef ();

    //
    // Add to the map
    //

    m_mapModel [pszLower] = pModel;
}

//-----------------------------------------------------------------------------
//
// @mfunc Find a model
//
// @parm const char * | pszModelName | Model name
//
// @rdesc Address of the model or NULL if not found
//
//-----------------------------------------------------------------------------

CNwnMdlModel *CNmcCache::FindModel (const char *pszName)
{

    //
    // Lowercase the string 
    //

    size_t nSize = strlen (pszName);
    char *pszLower = (char *) alloca (nSize + 1);
    strcpy (pszLower, pszName);
    strlwr (pszLower);

    //
    // Locate the model
    //

    std::map <std::string, CNwnMdlModel *>::iterator ix;
    ix = m_mapModel .find (pszLower);
    if (ix != m_mapModel .end ())
        return (*ix) .second;
    else
        return NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc Clear the cache.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNmcCache::ClearCache ()
{

    //
    // Release all the models
    //

    std::map <std::string, CNwnMdlModel *>::iterator ix;
    for (ix = m_mapModel .begin (); ix != m_mapModel .end (); ++ix)
    {
        (*ix) .second ->Release ();
    }
    m_mapModel .clear ();

    //
    // Clear the textures
    //

    m_mapTexture .clear ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Add a texture to the cache
//
// @parm const char * | pszName | Texture name
//
// @parm bool | fBumpmapped | If true, texture if bumpmapped
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNmcCache::AddTexture (const char *pszName, bool fBumpmapped)
{

    //
    // Lowercase the string 
    //

    size_t nSize = strlen (pszName);
    char *pszLower = (char *) alloca (nSize + 1);
    strcpy (pszLower, pszName);
    strlwr (pszLower);

    //
    // Add to the map
    //

    m_mapTexture [pszLower] = fBumpmapped;
}

//-----------------------------------------------------------------------------
//
// @mfunc Find a texture
//
// @parm const char * | pszName | Texture name
//
// @parm bool * | pfBumpmapped | Set to the bumpmap flag
//
// @rdesc TRUE if the texture was found.
//
//-----------------------------------------------------------------------------

bool CNmcCache::FindTexture (const char *pszName, bool *pfBumpmapped)
{

    //
    // Lowercase the string 
    //

    size_t nSize = strlen (pszName);
    char *pszLower = (char *) alloca (nSize + 1);
    strcpy (pszLower, pszName);
    strlwr (pszLower);

    //
    // Locate the model
    //

    std::map <std::string, bool>::iterator ix;
    ix = m_mapTexture .find (pszLower);
    if (ix != m_mapTexture .end ())
    {
        *pfBumpmapped = (*ix) .second;
        return true;
    }
    else
        return false;
}

/* vim: set sw=4: */
