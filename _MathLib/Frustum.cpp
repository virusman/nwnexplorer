//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      Frustum.cpp - Core Frustum supprt |
//
// This module contains the support for frustums.
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

#include "Frustum.h"
#include "Constants.h"

//-----------------------------------------------------------------------------
//
// @mfunc Set the frustum
//
//              Set the values of the frustum.
//
// @parm float | fFOV | Field of view
//
// @parm float | fNear | Near clip plane
//
// @parm float | fFar | Far clip plane
//
// @parm int | cx | Width of the rendered scene
//
// @parm int | cy | Hegiht of the rendered scene
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CFrustum::SetFrustum (float fFOV, float fZNear, float fZFar, int cx, int cy)
{
    //
    // Convert the FOV to radians
    //

    m_fFOV = (float) (fFOV * PI / 180.0f);

    //
    // Save the viewport size
    //

    m_cx = cx;
    m_cy = cy;

    //
    // Compute the values for the frustum
    //

    m_fAspect  =   (float) cx / (float) cy;
    m_fZNear   =   fZNear;
    m_fZFar    =   fZFar;
    m_fZNegFar = - fZFar;
    if (m_fAspect >= 1.0f)
    {
        m_fXMax    =  fZNear * (float) tan (m_fFOV / 2.0f);
        m_fXMin    = -m_fXMax;
        m_fYMin    =  m_fXMin / m_fAspect;
        m_fYMax    =  m_fXMax / m_fAspect;
    }
    else
    {
        m_fYMax    =  fZNear * (float) tan (m_fFOV / 2.0f);
        m_fYMin    = -m_fYMax;
        m_fXMin    =  m_fYMin * m_fAspect;
        m_fXMax    =  m_fYMax * m_fAspect;
    }

    //
    // Compute the sin and cos values that will be used
    // to test for the left, right, top, and bottom
    // clip planes.  (Reduces the amount of multiplications
    // if we just do the dot products by hand instead of the
    // normal way.)
    //

    float fa;
    fa = (float) atan (m_fXMax / m_fZNear);
    m_fLeftSin   =   (float) sin (fa);
    m_fLeftCos   =   (float) cos (fa);
    m_fRightSin  = - (float) sin (fa);
    m_fRightCos  =   (float) cos (fa);
    fa = (float) atan (m_fYMax / m_fZNear);
    m_fTopSin    =   (float) sin (fa);
    m_fTopCos    =   (float) cos (fa);
    m_fBottomSin = - (float) sin (fa);
    m_fBottomCos =   (float) cos (fa);

    //
    // Compute the projection vector
    //

    m_vProjection .m_x = (2.0f * m_fZNear) / (m_fXMax - m_fXMin);
    m_vProjection .m_y = (2.0f * m_fZNear) / (m_fYMax - m_fYMin);
    m_vProjection .m_z = (m_fZFar + m_fZNear) / (m_fZFar - m_fZNear);
    m_vProjection .m_w = (2.0f * m_fZFar * m_fZNear) / (m_fZFar - m_fZNear);
}

//-----------------------------------------------------------------------------
//
// @mfunc Test a vertex against the frustum
//
//              Test a vertex against the frustum.
//
// @parm const CVector3 & | v | Vector to be tested
//
// @rdesc Bitmask of which frustum planes the point is in front of.
//
//-----------------------------------------------------------------------------

unsigned short CFrustum::TestVertex (const CVector3 &v) const
{
    //
    // Do the near and far test
    //

    unsigned short uFlags = 0;
    if (v .m_z > 0) 
        uFlags |= ClipPlane_ZNear;
    else if (v .m_z < m_fZNegFar) 
        uFlags |= ClipPlane_ZFar;

    //
    // Do the left and right test
    //

    if (m_fLeftSin * v .m_x < m_fLeftCos * v .m_z)
        uFlags |= ClipPlane_XLeft;
    if (m_fRightSin * v .m_x < m_fRightCos * v .m_z)
        uFlags |= ClipPlane_XRight;


    //
    // Do the top and bottom test
    //

    if (m_fTopSin * v .m_y < m_fTopCos * v .m_z)
        uFlags |= ClipPlane_YTop;
    if (m_fBottomSin * v .m_y < m_fBottomCos * v .m_z)
        uFlags |= ClipPlane_YBottom;

    //
    // Return the flags
    //

    return uFlags;
}

//-----------------------------------------------------------------------------
//
// @mfunc Test a landscape vertex against the frustum
//
//              Test a vertex against the frustum.
//
// @parm const CVector3 & | v | Vector to be tested
//
// @parm const CVector3 & | vUp | Up error vector
//
// @rdesc Bitmask of which frustum planes the point is in front of.
//
//-----------------------------------------------------------------------------

unsigned short CFrustum::TestLandVertex (
    const CVector3 &v, const CVector3 &vUp) const
{
    CVector3 v1 (v + vUp);
    CVector3 v2 (v - vUp);

    //
    // Do the near and far test
    //

    unsigned short uFlags = 0;
    if (v1 .m_z > 0 && v2 .m_z > 0) 
        uFlags |= ClipPlane_ZNear;
    else if (v1 .m_z < m_fZNegFar && v2 .m_z < m_fZNegFar) 
        uFlags |= ClipPlane_ZFar;

    //
    // Do the left and right test
    //

    if (m_fLeftSin * v1 .m_x < m_fLeftCos * v1 .m_z &&
        m_fLeftSin * v2 .m_x < m_fLeftCos * v2 .m_z)
        uFlags |= ClipPlane_XLeft;
    if (m_fRightSin * v1 .m_x < m_fRightCos * v1 .m_z &&
        m_fRightSin * v2 .m_x < m_fRightCos * v2 .m_z)
        uFlags |= ClipPlane_XRight;


    //
    // Do the top and bottom test
    //

    if (m_fTopSin * v1 .m_y < m_fTopCos * v1 .m_z &&
        m_fTopSin * v2 .m_y < m_fTopCos * v2 .m_z)
        uFlags |= ClipPlane_YTop;
    if (m_fBottomSin * v1 .m_y < m_fBottomCos * v1 .m_z &&
        m_fBottomSin * v2 .m_y < m_fBottomCos * v2 .m_z)
        uFlags |= ClipPlane_YBottom;

    //
    // Return the flags
    //

    return uFlags;
}


/* vim: set sw=4: */
