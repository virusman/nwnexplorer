#ifndef ETS_QUATERNION_H
#define ETS_QUATERNION_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      Quaternion.h - Quaternions |
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

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include <math.h>
#include <assert.h>
#include "Vector3.h"

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

#ifdef _WIN32
#pragma warning (push)
#pragma warning (disable : 4201)
#endif

class CQuaternion
{
    // @access Construction and destruction
    public:

        // @cmember General constructor

        CQuaternion ()
        {
        }

        // @cmember Constructor with initial values

        CQuaternion (float x, float y, float z, float w)
            : m_x (x), m_y (y), m_z (z), m_w (w)
        {
        }

        // @cmember Copy constructor

        CQuaternion (const CQuaternion &src) 
            : m_x (src .m_x), m_y (src .m_y), m_z (src .m_z), m_w (src .m_w)
        {
        }

        // @cmember Constructor from an HPR

        CQuaternion (const CVector3 &vHPR)
        {
            SetHPR (vHPR);
        }

        // @cmember Axis rotation constructor

        CQuaternion (const CVector3 &vAxis, float fAngle)
        {
            SetAxisRotation (vAxis, fAngle);
        }

        // @access Public methods
    public:

        // @cmember Set the quaternion from an RPY vector

        void SetRPY (const CVector3 &vRPY);

        // @cmember Set the quaternion from an HPR vector

        void SetHPR (const CVector3 &vHPR);

        // @cmember Slerp between two quaternions

        void Slerp (const CQuaternion &p, const CQuaternion &q, float t);

        // @cmember Rotate the given vector

        void Rotate (const CVector3 &vecIn, CVector3 &vecOut) const;

        // @cmember Rotate the given vector by the negative of the angle

        void ReverseRotate (const CVector3 &vecIn, CVector3 &vecOut) const;

        // @access Public inline methods
    public:

        // @cmember Clear a quaternion

        void Clear ()
        {
            m_x = m_y = m_z = 0;
            m_w = 1;
        }

        // @cmember Compute the dot product

        float Dot (const CQuaternion &rhs) const 
        {
            return m_x * rhs .m_x + m_y * rhs .m_y + m_z * rhs .m_z + m_w * rhs .m_w;
        }

        // @cmember Return the length squared

        float LengthSqr () const
        {
            return m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w;
        }

        // @cmember Return the length of the quaternion

        float Length () const 
        {
            return (float) sqrt (LengthSqr ());
        }

        // @cmember normalize the quaternion

        float Normalize () 
        {
            float fLength = Length ();
            if (fLength == 0) 
                return 0;
            m_x /= fLength;
            m_y /= fLength;
            m_z /= fLength;
            m_w /= fLength;
            return fLength;
        };

        // @cmember Intert the quaternion

        void Invert ()
        {
            float flOneOverLenSqr = 1.0f / LengthSqr ();
            m_x = - m_x / flOneOverLenSqr;
            m_y = - m_y / flOneOverLenSqr;
            m_z = - m_z / flOneOverLenSqr;
            m_w =   m_w / flOneOverLenSqr;
        }

        // @cmember Intert the quaternion

        void InvertUnit ()
        {
            Conjugate ();
        }

        // @cmeber Conjugate the quaternion

        void Conjugate ()
        {
            m_x = - m_x;
            m_y = - m_y;
            m_z = - m_z;
        }

        // @cmember Set the qauternion from an axis rotation

        void SetAxisRotation (const CVector3 &vAxis, float fAngle)
        {
            CVector3 *pv = (CVector3 *) &m_x;
            *pv = vAxis;
            (*pv) .Normalize ();
            (*pv) *= (float) sin (fAngle * 0.5f);
            m_w = (float) cos (fAngle * 0.5f);
        }

        // @cmember Get the axis rotation

        void GetAxisRotation (CVector3 *pvAxis, float *pfAngle)
        {
            double d = m_x * m_x + m_y * m_y + m_z * m_z;
            if (d > 1.0e-6)
            {
                d = sqrt (d);
                double dI = 1.0 / d;
                pvAxis ->m_x = (float) (- m_x * dI);
                pvAxis ->m_y = (float) (- m_y * dI);
                pvAxis ->m_z = (float) (- m_z * dI);
                *pfAngle = (float) (-2.0 * atan2 (d, (double)m_w));
            }
            else
            {
                pvAxis ->m_x = 0.0f;
                pvAxis ->m_y = 0.0f;
                pvAxis ->m_z = 0.0f;
                *pfAngle = 0.0f;
            }
        }

        // @access Public operators
    public:

        // @cmember Copy a quaternion

        const CQuaternion &operator = (const CQuaternion &rhs)
        {
            m_x = rhs .m_x;
            m_y = rhs .m_y;
            m_z = rhs .m_z;
            m_w = rhs .m_w;
            return *this;
        }

        // @cmember Add quaternion to source quaternion

        const CQuaternion &operator += (const CQuaternion &rhs)
        {
            m_x += rhs .m_x;
            m_y += rhs .m_y;
            m_z += rhs .m_z;
            m_w += rhs .m_w;
            return *this;
        }

        // @cmember Return the sum of two quaternions

        const CQuaternion operator + (const CQuaternion &rhs) const 
        {
            return CQuaternion (
                m_x + rhs .m_x, 
                m_y + rhs .m_y,
                m_z + rhs .m_z, 
                m_w + rhs .m_w);
        }

        // @cmember Subtract quaternion from source quaternion

        CQuaternion &operator -= (const CQuaternion &rhs)
        {
            m_x -= rhs .m_x;
            m_y -= rhs .m_y;
            m_z -= rhs .m_z;
            m_w -= rhs .m_w;
            return *this;
        }

        // @cmember Return the difference of two quaternions

        CQuaternion operator - (const CQuaternion &rhs) const
        {
            return CQuaternion (
                m_x - rhs .m_x, 
                m_y - rhs .m_y,
                m_z - rhs .m_z, 
                m_w - rhs .m_w);
        }

        // @cmember Multiplication operator

        CQuaternion operator * (const CQuaternion &rhs) const
        {
            CQuaternion qout;
            Multiply (*this, rhs, qout);
            return qout;
        }

        // @cmember Multiplication assignment operator

        const CQuaternion &operator *= (const CQuaternion &rhs) 
        {
            Multiply (*this, rhs, *this);
            return *this;
        }

        // @access Public static methods
    public:

        // @cmember Multiply two quaternions and set the third

        static void Multiply (const CQuaternion &q1, const CQuaternion &q2, CQuaternion &qout)
        {
            float x =   q1 .m_w * q2 .m_x - q1 .m_z * q2 .m_y + q1 .m_y * q2 .m_z + q1 .m_x * q2 .m_w;
            float y =   q1 .m_z * q2 .m_x + q1 .m_w * q2 .m_y - q1 .m_x * q2 .m_z + q1 .m_y * q2 .m_w;
            float z = - q1 .m_y * q2 .m_x + q1 .m_x * q2 .m_y + q1 .m_w * q2 .m_z + q1 .m_z * q2 .m_w;
            float w = - q1 .m_x * q2 .m_x - q1 .m_y * q2 .m_y - q1 .m_z * q2 .m_z + q1 .m_w * q2 .m_w;
            qout .m_x = x;
            qout .m_y = y;
            qout .m_z = z;
            qout .m_w = w;
        }

        // @access Public variables
    public:
        union
        {
            struct 
            {
                float       m_x;
                float       m_y;
                float       m_z;
                float       m_w;
            };
            float m_v [4];
        };
};

#ifdef _WIN32
#pragma warning (pop)
#endif

#endif // ETS_QUATERNION_H

/* vim: set sw=4: */
