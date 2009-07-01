//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      Quaternion.cpp - Quaternions |
//
// This module contains the support for quaternions
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

#include "Quaternion.h"
#include "Constants.h"

//-----------------------------------------------------------------------------
//
// @mfunc Set a quaternion from a set of angles
//
// @parm const CVector3 & | vRPY | Source angles in radians
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CQuaternion::SetRPY (const CVector3 &vRPY)
{
    float sr, sp, sy, cr, cp, cy;

    sr    = (float) sin (vRPY .m_x * 0.5f);
    cr    = (float) cos (vRPY .m_x * 0.5f);
    sp    = (float) sin (vRPY .m_y * 0.5f);
    cp    = (float) cos (vRPY .m_y * 0.5f);
    sy    = (float) sin (vRPY .m_z * 0.5f);
    cy    = (float) cos (vRPY .m_z * 0.5f);

    m_x = sr * cp * cy - cr * sp * sy;
    m_y = cr * sp * cy + sr * cp * sy;
    m_z = cr * cp * sy - sr * sp * cy;
    m_w = cr * cp * cy + sr * sp * sy;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set a quaternion from a set of angles
//
// @parm const CVector3 & | vHPR | Source angles in radians
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CQuaternion::SetHPR (const CVector3 &vHPR)
{
    float sr, sp, sh, cr, cp, ch;

    sh    = (float) sin (vHPR .m_x * 0.5f);
    ch    = (float) cos (vHPR .m_x * 0.5f);
    sp    = (float) sin (vHPR .m_y * 0.5f);
    cp    = (float) cos (vHPR .m_y * 0.5f);
    sr    = (float) sin (vHPR .m_z * 0.5f);
    cr    = (float) cos (vHPR .m_z * 0.5f);

    m_x = ch * cp * sr - sh * sp * cr;
    m_y = ch * sp * cr + sh * cp * sr;
    m_z = sh * cp * cr - ch * sp * sr;
    m_w = ch * cp * cr + sh * sp * sr;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set a quaternion from a smoothly interpolation
//
// @parm const CQuaternion & | p | Quaternion 1
//
// @parm const CQuaternion & | q | Quaternion 2
//
// @parm float | t | Proportion [0 - 1]
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CQuaternion::Slerp (const CQuaternion &p, const CQuaternion &q, float t) 
{
    float omega, cosom, sinom, sclp, sclq;
    CQuaternion q1;

    //
    // decide if one of the quaternions is backwards
    //

    float a = 
        (p .m_x - q .m_x) * (p .m_x - q .m_x) +
        (p .m_y - q .m_y) * (p .m_y - q .m_y) + 
        (p .m_z - q .m_z) * (p .m_z - q .m_z) +
        (p .m_w - q .m_w) * (p .m_w - q .m_w);
    float b = 
        (p .m_x + q .m_x) * (p .m_x + q .m_x) +
        (p .m_y + q .m_y) * (p .m_y + q .m_y) +
        (p .m_z + q .m_z) * (p .m_z + q .m_z) +
        (p .m_w + q .m_w) * (p .m_w + q .m_w);
    float flSign = a > b ? -1.0f : 1.0f;

    cosom = p .Dot (q) * flSign; 

    if ((1.0f + cosom) > 0.00000001f) 
    {
        if ((1.0f - cosom) > 0.00000001f) 
        {
            omega = (float) acos (cosom);
            sinom = (float) sin (omega);
            sclp = (float) sin ((1.0f - t) * omega) / sinom;
            sclq = (float) sin (t * omega ) / sinom;
        }
        else 
        {
            sclp = 1.0f - t;
            sclq = t;
        }
        sclq *= flSign;
        m_x = sclp * p .m_x + sclq * q .m_x;
        m_y = sclp * p .m_y + sclq * q .m_y;
        m_z = sclp * p .m_z + sclq * q .m_z;
        m_w = sclp * p .m_w + sclq * q .m_w;
    }
    else 
    {
        m_x = - p .m_y;
        m_y =   p .m_x;
        m_z = - p .m_w;
        m_w =   p .m_z;
        sclp = (float) sin ((1.0f - t) * 0.5f * PI);
        sclq = (float) sin (t * 0.5f * PI);
        m_x = sclp * p .m_x + sclq * m_x;
        m_y = sclp * p .m_y + sclq * m_y;
        m_z = sclp * p .m_z + sclq * m_z;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Rotate the given vector
//
// @parm const CVector3 & | vecIn | Source vector
//
// @parm CVector3 & | vecOut | Destination vector
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CQuaternion::Rotate (const CVector3 &vecIn, CVector3 &vecOut) const
{
    CQuaternion qtmp;

    //
    // Compute the QV part
    //

    qtmp .m_x =   m_w * vecIn .m_x - m_z * vecIn .m_y + m_y * vecIn .m_z + m_x;
    qtmp .m_y =   m_z * vecIn .m_x + m_w * vecIn .m_y - m_x * vecIn .m_z + m_y;
    qtmp .m_z = - m_y * vecIn .m_x + m_x * vecIn .m_y + m_w * vecIn .m_z + m_z;
    qtmp .m_w = - m_x * vecIn .m_x - m_y * vecIn .m_y - m_z * vecIn .m_z + m_w;

    //
    // Compute the VQ' part
    //

    vecOut .m_x = - qtmp .m_w * m_x + qtmp .m_z * m_y - qtmp .m_y * m_z + qtmp .m_x * m_w;
    vecOut .m_y = - qtmp .m_z * m_x - qtmp .m_w * m_y + qtmp .m_x * m_z + qtmp .m_y * m_w;
    vecOut .m_z = + qtmp .m_y * m_x - qtmp .m_x * m_y - qtmp .m_w * m_z + qtmp .m_z * m_w;
}

//-----------------------------------------------------------------------------
//
// @mfunc Rotate the given vector by the negative of the angle
//
// @parm const CVector3 & | vecIn | Source vector
//
// @parm CVector3 & | vecOut | Destination vector
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CQuaternion::ReverseRotate (const CVector3 &vecIn, CVector3 &vecOut) const
{
    CQuaternion qtmp;

    //
    // Compute the QV part
    //

    qtmp .m_x = - m_w * vecIn .m_x - m_z * vecIn .m_y + m_y * vecIn .m_z + m_x;
    qtmp .m_y =   m_z * vecIn .m_x - m_w * vecIn .m_y - m_x * vecIn .m_z + m_y;
    qtmp .m_z = - m_y * vecIn .m_x + m_x * vecIn .m_y - m_w * vecIn .m_z + m_z;
    qtmp .m_w = - m_x * vecIn .m_x - m_y * vecIn .m_y - m_z * vecIn .m_z - m_w;

    //
    // Compute the VQ' part
    //

    vecOut .m_x = - qtmp .m_w * m_x + qtmp .m_z * m_y - qtmp .m_y * m_z - qtmp .m_x * m_w;
    vecOut .m_y = - qtmp .m_z * m_x - qtmp .m_w * m_y + qtmp .m_x * m_z - qtmp .m_y * m_w;
    vecOut .m_z = + qtmp .m_y * m_x - qtmp .m_x * m_y - qtmp .m_w * m_z - qtmp .m_z * m_w;
}

/* vim: set sw=4: */
