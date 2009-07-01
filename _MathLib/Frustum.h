#ifndef ETS_FRUSTUM_H
#define ETS_FRUSTUM_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      Frustum.h - Frustum base class |
//
// This module contains the frustum support.
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
// $History: DcComEngGenConfigurePage.h $
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "Vector4.h"

//-----------------------------------------------------------------------------
//
// @class CFrustum | The class provides frustum support
//
//-----------------------------------------------------------------------------

class CFrustum 
{
    // @access Enumerations and types
    public:
        enum ClipPlanes
        {
            ClipPlane_ZNear             = 0x01,
            ClipPlane_ZFar              = 0x02,
            ClipPlane_XLeft             = 0x04,
            ClipPlane_XRight    = 0x08,
            ClipPlane_YBottom   = 0x10,
            ClipPlane_YTop              = 0x20,
        };

        // @access Constructors and destructors
    public:

        // @access Public methods
    public:

        void SetFrustum (float fFOV, float fZNear, float fZFar, int cx, int cy);

        // @cmember Test a vertex

        unsigned short TestVertex (const CVector3 &v) const;

        // @cmember Test a landscape vertex

        unsigned short TestLandVertex (const CVector3 &v, const CVector3 &vUp) const;

        // @access Protected variables
    public:

        // @cmember Size of the viewport

        int                                     m_cx, m_cy;

        // @cmember Field of view in radian

        float                           m_fFOV;

        // @cmember Aspect ration

        float                           m_fAspect;

        // @cmember Near clip plane

        float                           m_fZNear;

        // @cmember Far clip plane

        float                           m_fZFar;
        float                           m_fZNegFar;

        // @cmember Min/Max values for the frustrum

        float                           m_fXMin;
        float                           m_fXMax;
        float                           m_fYMin;
        float                           m_fYMax;

        // @cmember Clip plane vectors

        float                           m_fLeftSin;
        float                           m_fLeftCos;
        float                           m_fRightSin;
        float                           m_fRightCos;
        float                           m_fTopSin;
        float                           m_fTopCos;
        float                           m_fBottomSin;
        float                           m_fBottomCos;

        // @cmember Projection vector

        CVector4                        m_vProjection;
};

#endif // ETS_FRUSTUM_H

/* vim: set sw=4: */
