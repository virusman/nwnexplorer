//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      Vector3.cpp - 3d Vector support |
//
// This module contains the support for 3d vectors
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

#include "Vector3.h"

//
// Globals
//

const CVector3 g_vOrigin (0, 0, 0);

//-----------------------------------------------------------------------------
//
// @mfunc Compute the 2d intersection of two lines
//
// @parm const CVector3 & | p1 | Point on line 1
//
// @parm const CVector3 & | v1 | Vector of the line
//
// @parm const CVector3 & | p2 | Point on line 2
//
// @parm const CVector3 & | v2 | Vector of the line
//
// @rdesc Point of intersection
//
//-----------------------------------------------------------------------------

CVector3 CVector3::Intersection (const CVector3 &p1, const CVector3 &v1, 
                                 const CVector3 &p2, const CVector3 &v2)
{
    CVector3 vOut;
    CVector3 vd (p1 - p2);

    float ua = ((v2 .m_x * vd .m_y) - (v2 .m_y * vd .m_x)) /
        ((v2 .m_y * v1 .m_x) - (v2 .m_x * v1 .m_y));
    vOut .m_x = p1 .m_x + ua * (v1 .m_x);
    vOut .m_y = p1 .m_y + ua * (v1 .m_y);
    vOut .m_z = 0;
    return vOut;
}

//-----------------------------------------------------------------------------
//
// @mfunc Compute the color of the light
//
// @parm const CVector3 & | vSource | Source vector of the light
//
// @parm float | fSpecularExponent | Specular exponent of the surface (?)
//
// @parm float * | pfDiffuse | Diffuse component
//
// @parm float * | pfSpecular | Specular component
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CVector3::CalcLightFactors (const CVector3 &vSource, 
                                 float fSpecularExponent, float *pfDiffuse, float *pfSpecular)
{

    //
    // Compute and clamp the diffuse coefficient
    //

    float fNL = Dot (vSource);
    if (fNL < 0.0) 
        fNL = 0.0;

    //
    // Compute the specular scalar
    //

    CVector3 vR (vSource - (*this * fNL * 2.0f));
    float fRV = vR .m_z < 0 ? - vR .m_z : 0.0f;
    fRV = (float) pow (fRV, fSpecularExponent);

    //
    // Return the values
    //

    *pfDiffuse = fNL;
    *pfSpecular = fRV;
    return;
}


/* vim: set sw=4: */
