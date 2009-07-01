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

#include "Matrix.h"
#include "Constants.h"

//-----------------------------------------------------------------------------
//
// @mfunc Multiply a vector by the matrix
//
//              Return the result of vM when v is the vector and M is the matrix.
//              Not all elements of the matrix are used in this version.  Do not
//              used this method for projections.
//
// @parm const CVector3 & | vec | Vector to be mutliplied
//
// @parm CVector3 & | vecOut | Results of the vector mutiplication
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMatrix::PreTranslate (const CVector3 &vec, CVector3 &vecOut) const
{
    //
    // Compute the results of the multiplication
    //

    vecOut .m_x = m_m [0] [0] * vec .m_x + 
        m_m [1] [0] * vec .m_y +
        m_m [2] [0] * vec .m_z +
        m_m [3] [0];
    vecOut .m_y = m_m [0] [1] * vec .m_x + 
        m_m [1] [1] * vec .m_y +
        m_m [2] [1] * vec .m_z +
        m_m [3] [1];
    vecOut .m_z = m_m [0] [2] * vec .m_x + 
        m_m [1] [2] * vec .m_y +
        m_m [2] [2] * vec .m_z +
        m_m [3] [2];
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Multiply a vector by the matrix
//
//              Return the result of vM when v is the vector and M is the matrix.
//              All elements of the matrix are used in this version.  This method can
//              be used to project a vector.
//
// @parm const CVector3 & | vec | Vector to be mutliplied
//
// @parm CVector4 & | vecOut | Results of the vector mutiplication
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMatrix::PreTranslate (const CVector3 &vec, CVector4 &vecOut) const
{
    //
    // Compute the projected vector
    //

    vecOut .m_x = m_m [0] [0] * vec .m_x + 
        m_m [1] [0] * vec .m_y +
        m_m [2] [0] * vec .m_z +
        m_m [3] [0];
    vecOut .m_y = m_m [0] [1] * vec .m_x + 
        m_m [1] [1] * vec .m_y +
        m_m [2] [1] * vec .m_z +
        m_m [3] [1];
    vecOut .m_z = m_m [0] [2] * vec .m_x + 
        m_m [1] [2] * vec .m_y +
        m_m [2] [2] * vec .m_z +
        m_m [3] [2];
    vecOut .m_w = m_m [0] [3] * vec .m_x + 
        m_m [1] [3] * vec .m_y +
        m_m [2] [3] * vec .m_z +
        m_m [3] [3];
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Multiply a normal by the matrix
//
//              Return the result of vM when v is the normal and M is the matrix.
//              Only the rotation elements of the matrix are used.  If the matrix
//              has had a scale applied to it, then this routine will fail.
//
// @parm const CVector3 & | vec | Vector to be mutliplied
//
// @parm CVector3 & | vecOut | Results of the vector mutiplication
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMatrix::PreRotate (const CVector3 &vec, CVector3 &vecOut) const
{
    //
    // Compute the results
    //

    vecOut .m_x = m_m [0] [0] * vec .m_x + 
        m_m [1] [0] * vec .m_y +
        m_m [2] [0] * vec .m_z;
    vecOut .m_y = m_m [0] [1] * vec .m_x + 
        m_m [1] [1] * vec .m_y +
        m_m [2] [1] * vec .m_z;
    vecOut .m_z = m_m [0] [2] * vec .m_x + 
        m_m [1] [2] * vec .m_y +
        m_m [2] [2] * vec .m_z;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Multiply a normal by the matrix
//
//              Return the result of Mv when v is the normal and M is the matrix.
//              Only the rotation elements of the matrix are used.  If the matrix
//              has had a scale applied to it, then this routine will fail.
//
// @parm const CVector3 & | vec | Vector to be mutliplied
//
// @parm CVector3 & | vecOut | Results of the vector mutiplication
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMatrix::PostRotate (const CVector3 &vec, CVector3 &vecOut) const
{
    //
    // Compute the results
    //

    vecOut .m_x = m_m [0] [0] * vec .m_x + 
        m_m [0] [1] * vec .m_y +
        m_m [0] [2] * vec .m_z;
    vecOut .m_y = m_m [1] [0] * vec .m_x + 
        m_m [1] [1] * vec .m_y +
        m_m [1] [2] * vec .m_z;
    vecOut .m_z = m_m [2] [0] * vec .m_x + 
        m_m [2] [1] * vec .m_y +
        m_m [2] [2] * vec .m_z;
}

//-----------------------------------------------------------------------------
//
// @mfunc Compute the sin and cosine of an angle
//
// @parm float | fAngle | The angle in degrees
//
// @parm float & | fCos | The computed cosine of the angle
//
// @parm float & | fSin | The computed sine of the angle
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMatrix::GetCosSin (float fAngle, float &fCos, float &fSin)
{
    //
    // Place the angle in range
    //

    fAngle = (float) fmod (fAngle, 360.0f);
    if (fAngle < 0.0) fAngle += 360.0;

    //
    // Check for angles that lie on an axis.  If not on an axis,
    // then compute using the trig routines.
    //

    if (fAngle == 0.0) 
    {
        fCos = 1;
        fSin = 0;
    } 
    else if (fAngle == 90.0) 
    {
        fCos = 0;
        fSin = 1;
    } 
    else if (fAngle == 180.0) 
    {
        fCos = -1;
        fSin = 0;
    } 
    else if (fAngle == 270.0) 
    {
        fCos = 0;
        fSin = -1;
    } 
    else 
    {
        fCos = (float) cos (fAngle * PI / 180.0);
        fSin = (float) sin (fAngle * PI / 180.0);
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the rotation part of the matrix
//
//              Only the rotation part of the matrix is set by this routine.  The
//              previous setting for the rotation part of the matrix is lost.
//
// @parm float | fAngle | The angle of the rotation in degrees
//
// @parm float | fX | The x coordinate of the normal that specifies
//              the axis of the rotation.
//
// @parm float | fY | The y coordinate of the normal that specifies
//              the axis of the rotation.
//
// @parm float | fZ | The z coordinate of the normal that specifies
//              the axis of the rotation.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMatrix::SetRotation (float fAngle, float fX, float fY, float fZ)
{
    //
    // Local definitions
    //

    CVector3 v (fX, fY, fZ);
    float cosa;
    float sina;

    //
    // Get the angles
    //

    GetCosSin (fAngle, cosa, sina);

    //
    // Normalize the vector
    //

    v .Normalize ();

    //
    // Populate the matrix
    //

    m_m [0] [0] = ((1.0f - cosa) * v .m_x * v .m_x) + cosa;
    m_m [1] [0] = ((1.0f - cosa) * v .m_x * v .m_y) - v .m_z * sina;
    m_m [2] [0] = ((1.0f - cosa) * v .m_x * v .m_z) + v .m_y * sina;

    m_m [0] [1] = ((1.0f - cosa) * v .m_y * v .m_x) + v .m_z * sina;
    m_m [1] [1] = ((1.0f - cosa) * v .m_y * v .m_y) + cosa;
    m_m [2] [1] = ((1.0f - cosa) * v .m_y * v .m_z) - v .m_x * sina;

    m_m [0] [2] = ((1.0f - cosa) * v .m_z * v .m_x) - v .m_y * sina;
    m_m [1] [2] = ((1.0f - cosa) * v .m_z * v .m_y) + v .m_x * sina;
    m_m [2] [2] = ((1.0f - cosa) * v .m_z * v .m_z) + cosa;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the rotation part of the matrix
//
//              Only the rotation part of the matrix is set by this routine.  The
//              previous setting for the rotation part of the matrix is lost.
//
// @parm float | fH | The heading of the rotation
//
// @parm float | fP | The pitch of the rotation
//
// @parm float | fR | The roll of the rotation
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMatrix::SetHPR (float fH, float fP, float fR)
{
    //
    // Local definitions
    //

    float cosa;
    float cosb;
    float cosc;
    float sina;
    float sinb;
    float sinc;

    //
    // Get the sine and cosine of the angles
    //

    GetCosSin (fH, cosa, sina);
    GetCosSin (fP, cosb, sinb);
    GetCosSin (fR, cosc, sinc);

    //
    // Set the matrix
    //

    m_m [0] [0] = cosa * cosb;
    m_m [1] [0] = - sina * cosc + cosa * sinb * sinc;
    m_m [2] [0] = sina * sinc + cosa * sinb * cosc;

    m_m [0] [1] = sina * cosb;
    m_m [1] [1] = cosa * cosc + sina * sinb * sinc;
    m_m [2] [1] = - cosa * sinc + sina * sinb * cosc;

    m_m [0] [2] = - sinb;
    m_m [1] [2] = cosb * sinc;
    m_m [2] [2] = cosb * cosc;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the matrix to look at the given eye and direction
//
//              The whole matrix is modified to look in the given direction from
//              the eye.  The up direction is used to orient the camera.
//              
//
// @parm const CVector3 & | vEye | Position of the eye
//
// @parm const CVector3 & | vCenter | Where the eye is looking
//
// @parm const CVector3 & | vUp | Vertical orientation of the camera
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMatrix::MakeLookAt (const CVector3 &vEye, 
                          const CVector3 &vCenter, const CVector3 &vUp)
{
    //
    //  Compute the unit vector direction from the eye to the center
    //

    CVector3 z (vEye - vCenter);
    z .Normalize ();

    //
    //  Using the eye and the up, compute the normal from those two
    //  vectors.
    //

    CVector3 x (vUp .Cross (z));

    //
    //  Compute y which is a perpindicular version of vUp (y == vUp basically)
    //

    CVector3 y (z .Cross (x));

    //
    //  Normalize the two vectors
    //

    x .Normalize ();
    y .Normalize ();

    //
    //  Initialize the matrix
    //

    MakeIdentity ();
    SetCol (0, x);
    SetCol (1, y);
    SetCol (2, z);

    //
    //  Add the translation
    //
    Translate (-vEye);
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the matrix from a quaternion
//
// @parm const CQuaternion & | v | Source quaternion
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMatrix::SetQuaternion (const CQuaternion &v)
{
    m_m [0] [0] = 1.0f - 2.0f * v .m_y * v .m_y - 2.0f * v .m_z * v .m_z;
    m_m [0] [1] =        2.0f * v .m_x * v .m_y + 2.0f * v .m_w * v .m_z;
    m_m [0] [2] =        2.0f * v .m_x * v .m_z - 2.0f * v .m_w * v .m_y;

    m_m [1] [0] =        2.0f * v .m_x * v .m_y - 2.0f * v .m_w * v .m_z;
    m_m [1] [1] = 1.0f - 2.0f * v .m_x * v .m_x - 2.0f * v .m_z * v .m_z;
    m_m [1] [2] =        2.0f * v .m_y * v .m_z + 2.0f * v .m_w * v .m_x;

    m_m [2] [0] =        2.0f * v .m_x * v .m_z + 2.0f * v .m_w * v .m_y;
    m_m [2] [1] =        2.0f * v .m_y * v .m_z - 2.0f * v .m_w * v .m_x;
    m_m [2] [2] = 1.0f - 2.0f * v .m_x * v .m_x - 2.0f * v .m_y * v .m_y;
}

//-----------------------------------------------------------------------------
//
// @mfunc Multiply two matricies.
//
// @parm const CMatrix & | rhs | Matrix to multiply
//
// @parm CMatrix & | out | Resulting matrix
//
// @rdesc The results of the multiplication
//
//-----------------------------------------------------------------------------

void CMatrix::Multiply4 (const CMatrix &rhs, CMatrix &out) const 
{
    //
    // Loop through the two matricies and multiply the elements.  Store
    // the results in the temporary matrix
    //

    for (int i = 0; i < 4; i++) 
    {
        for (int j = 0; j < 4; j++) 
        {
            out .m_m [j] [i] = 
                (m_m [0] [i] * rhs .m_m [j] [0]) +
                (m_m [1] [i] * rhs .m_m [j] [1]) +
                (m_m [2] [i] * rhs .m_m [j] [2]) +
                (m_m [3] [i] * rhs .m_m [j] [3]);
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Multiply two matricies.
//
// @parm const CMatrix & | rhs | Matrix to multiply
//
// @parm CMatrix & | out | Resulting matrix
//
// @rdesc The results of the multiplication
//
//-----------------------------------------------------------------------------

void CMatrix::Multiply3 (const CMatrix &rhs, CMatrix &out) const 
{
    //
    // Loop through the two matricies and multiply the elements.  Store
    // the results in the temporary matrix
    //

    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            out .m_m [j] [i] = 
                (m_m [0] [i] * rhs .m_m [j] [0]) +
                (m_m [1] [i] * rhs .m_m [j] [1]) +
                (m_m [2] [i] * rhs .m_m [j] [2]) +
                (m_m [3] [i] * rhs .m_m [j] [3]);
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the rotation of a matrix based on the difference between
//              to vectors.
//
//              Set the rotation of the matrix so that sM = t;
//
// @parm const CVector3 & | s | Initial vector
//
// @parm const CVector3 & | t | Destination vector
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMatrix::SetRotation (const CVector3 &s, const CVector3 &t) 
{

    //
    // Compute the cosine between the two lines
    //

    float e = s .Dot (t);

    //
    // If close to one, then same line, use identity
    //

    if (e > 1 - NORMAL_EPSILON)
    {
        m_m [0] [0] = 1;
        m_m [0] [1] = 0;
        m_m [0] [2] = 0;

        m_m [1] [0] = 0;
        m_m [1] [1] = 1;
        m_m [1] [2] = 0;

        m_m [2] [0] = 0;
        m_m [2] [1] = 0;
        m_m [2] [2] = 1;
    }

    //
    // If close to negative one, then same line in other direction
    //

    else if (e < -1 + NORMAL_EPSILON)
    {
        m_m [0] [0] = -1;
        m_m [0] [1] =  0;
        m_m [0] [2] =  0;

        m_m [1] [0] =  0;
        m_m [1] [1] = -1;
        m_m [1] [2] =  0;

        m_m [2] [0] =  0;
        m_m [2] [1] =  0;
        m_m [2] [2] =  1;
    }

    //
    // Otherwise, use normal method to compute values
    //

    else
    {
        CVector3 v (s .Cross (t));
        float h = (1 - e) / v .Dot (v);

        m_m [0] [0] = e + h * v .m_x * v .m_x;
        m_m [0] [1] =     h * v .m_x * v .m_y + v .m_z;
        m_m [0] [2] =     h * v .m_x * v .m_z - v .m_y;

        m_m [1] [0] =     h * v .m_x * v .m_y - v .m_z;
        m_m [1] [1] = e + h * v .m_y * v .m_y;
        m_m [1] [2] =     h * v .m_y * v .m_z + v .m_x;

        m_m [2] [0] =     h * v .m_x * v .m_z + v .m_y;
        m_m [2] [1] =     h * v .m_y * v .m_z - v .m_x;
        m_m [2] [2] = e + h * v .m_z * v .m_z;
    }
}


/* vim: set sw=4: */
