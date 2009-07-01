//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnMdlGeometry.cpp - Model geometry support |
//
// This module contains the general model geometry support.
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

#include "NwnMdlGeometry.h"
#include "NwnMdlNodes.h"
#include "NwnModel.h"

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlGeometryHeader> constructor.
//
// @parm const char * | pszName | Name of the geometry
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlGeometryHeader::CNwnMdlGeometryHeader (const char *pszName)
{

    //
    // Copy the name (this is an ultra-clean version)
    //

    memset (m_szName, 0, _countof (m_szName));
    strncpy (m_szName, pszName, _countof (m_szName));
    m_szName [_countof (m_szName) - 1] = 0;

    //
    // Set the type
    //

    m_ucGeometryType = NwnMdlGT_Geometry;

    //
    // Intialize the other variables
    //

    m_ulNodeCount = 0;
    m_ulUnknown2 = 0;

    //
    // Initialize variables not initialized by Bioware
    //

    m_auchPad [0] = 0;
    m_auchPad [1] = 0;
    m_auchPad [2] = 0;

    //
    // Initialize the routines
    //

    m_aulGeomRoutines [0] = 0x0046A144;
    m_aulGeomRoutines [1] = 0x0046A154;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlGeometryHeader> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlGeometryHeader::~CNwnMdlGeometryHeader ()
{

    //
    // Free the geometry
    //

    if (m_pGeometry .GetData () != NULL)
        delete m_pGeometry .GetData ();
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlModel> constructor.
//
// @parm const char * | pszName | Name of the geometry
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

    CNwnMdlModel::CNwnMdlModel (const char *pszName) 
: CNwnMdlGeometryHeader (pszName)
{
    //
    // Initialize type
    //

    m_ucGeometryType = NwnMdlGT_Model;

    //
    // Initialize other values
    //

    m_aucFlags [0] = 0;
    m_aucFlags [1] = 0;
    m_ucClassification = 0;
    m_ucFog = 1;
    m_ulRefCount = 0;
    m_vBMin = CVector3 (-5.0f, -5.0f, -1.0f);
    m_vBMax = CVector3 ( 5.0f,  5.0f, 10.0f);
    m_fRadius = 7.0f;
    m_fAnimationScale = 1.0f;
    memset (m_szSuperModel, 0, _countof (m_szSuperModel));

    //
    // Initialize the routine pointers
    //

    m_aulGeomRoutines [0] = 0x0046AB0C;
    m_aulGeomRoutines [1] = 0x0046AB1C;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlModel> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlModel::~CNwnMdlModel ()
{

    //
    // If we have a supermodel, release
    //

    if (m_pSuperModel .GetData () != NULL)
        m_pSuperModel .GetData () ->Release ();

    //
    // Free the animations
    //

    if (m_apAnimations .GetCount () > 0)
    {
        CNwnPointer <CNwnMdlAnimation> *ppAnimations = m_apAnimations .GetData ();
        for (int i = 0; i < (int) m_apAnimations .GetCount (); i++)
        {
            CNwnMdlAnimation *pAnimation = ppAnimations [i] .GetData ();
            delete pAnimation;
        }
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Delete the model
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlModel::DeleteModel ()
{

    //
    // If this is an ascii model
    //

    if ((m_ucGeometryType & NwnMdlGF_LoadedBinary) == 0)
    {
        delete this;
    }

    //
    // Otherwise, free the geometry by hand
    //

    else
    {

        //
        // Release the super model
        //

        if (m_pSuperModel .GetData () != NULL)
            m_pSuperModel .GetData () ->Release ();

        //
        // Free the data 
        //

        unsigned char *p = (unsigned char *) this;
        p -= sizeof (CNwnModel::Header);
        free (p);
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlAnimation> constructor.
//
// @parm const char * | pszName | Name of the geometry
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

    CNwnMdlAnimation::CNwnMdlAnimation (const char *pszName) 
: CNwnMdlGeometryHeader (pszName)
{
    //
    // Initialize type
    //

    m_ucGeometryType = NwnMdlGT_Animation;

    //
    // Initialize other values
    //

    m_fLength = 1.0f;
    m_fTransTime = 0.25f;
    memset (m_szAnimRoot, 0, _countof (m_szAnimRoot));


    //
    // Initialize the routine pointers
    //

    m_aulGeomRoutines [1] = 0x004C64E4;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlAnimation> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlAnimation::~CNwnMdlAnimation ()
{
    return;
}


/* vim: set sw=4: */
