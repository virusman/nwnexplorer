#ifndef ETS_VECTOR4_H
#define ETS_VECTOR4_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      Vector4.h - 4d vector support |
//
// This module contains the support for 4d vectors
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

#include "Vector3.h"
#include <math.h>

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

#ifdef _WIN32
#pragma warning (push)
#pragma warning (disable : 4201)
#endif

class CVector4
{
    // @access Construction and destruction
    public:

        // @cmember General constructor

        CVector4 ()
        {
        }

        // @cmember Initial value constructor

        CVector4 (float x, float y, float z, float w) : m_x (x), 
        m_y (y), m_z (z), m_w (w)
    {
    }

        // @cmember Copy constructor

        CVector4 (const CVector4 &src) : m_x (src .m_x), 
        m_y (src .m_y), m_z (src .m_z), m_w (src .m_w)
    {
    }

        // @access Public inline methods
    public:

        // @cmember Compute the dot product

        float Dot (const CVector4 &rhs) const 
        {
            return m_x * rhs .m_x + m_y * rhs .m_y + 
                m_z * rhs .m_z + m_w * rhs .m_w;
        }

        // @cmember Compute the length

        float Length () const 
        {
            return (float) sqrt (m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w);
        }

        // @cmember Normalize and return the length

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
        }

        // @cmember Reset the vector to (0,0,0,0)

        void Clear () 
        {
            m_x = m_y = m_z = m_w = 0;
        }

        // @access Public operators
    public:

        // @cmember Assignment operator

        const CVector4 &operator = (const CVector4 &rhs)
        {
            m_x = rhs .m_x;
            m_y = rhs .m_y;
            m_z = rhs .m_z;
            m_w = rhs .m_w;
            return *this;
        }

        // @cmember Assignment operator from a 3d vector

        const CVector4 &operator = (const CVector3 &rhs)
        {
            m_x = rhs .m_x;
            m_y = rhs .m_y;
            m_z = rhs .m_z;
            m_w = 1.0f;
            return *this;
        }

        // @cmember Addition

        const CVector4 &operator += (const CVector4 &rhs)
        {
            m_x += rhs .m_x;
            m_y += rhs .m_y;
            m_z += rhs .m_z;
            m_w += rhs .m_w;
            return *this;
        }

        // @cmember Addition

        const CVector4 operator + (const CVector4 &rhs) const 
        {
            return CVector4 (m_x + rhs .m_x, m_y + rhs .m_y,
                             m_z + rhs .m_z, m_w + rhs .m_w);
        }

        // @cmember Subtraction

        CVector4 &operator -= (const CVector4 &rhs)
        {
            m_x -= rhs .m_x;
            m_y -= rhs .m_y;
            m_z -= rhs .m_z;
            m_w -= rhs .m_w;
            return *this;
        }

        // @cmember Subtraction

        CVector4 operator - (const CVector4 &rhs) const
        {
            return CVector4 (m_x - rhs .m_x, m_y - rhs .m_y,
                             m_z - rhs .m_z, m_w - rhs .m_w);
        }

        // @cmember Scale

        const CVector4 &operator *= (float fScale) 
        {
            m_x *= fScale;
            m_y *= fScale;
            m_z *= fScale;
            m_w *= fScale;
            return *this;
        }

        // @cmember Scale

        CVector4 operator * (float fScale) const
        {
            return CVector4 (m_x * fScale, m_y * fScale,
                             m_z * fScale, m_w * fScale);
        }

        // @cmember Scale

        CVector4 operator / (float fScale) const
        {
            return CVector4 (m_x / fScale, m_y / fScale,
                             m_z / fScale, m_w / fScale);
        }

        // @cmember Negation

        CVector4 operator - () const 
        {
            return CVector4 (- m_x, - m_y, - m_z, - m_w);
        }

        // @cmember Component multiplcation

        CVector4 operator * (const CVector4 &rhs) const 
        {
            return CVector4 (m_x * rhs .m_x, m_y * rhs .m_y,
                             m_z * rhs .m_z, m_w * rhs .m_w);
        }

        // @cmember Epsilon eqaulity check

        bool AlmostEqual (const CVector4 &rhs, float fEpsilon) const 
        {
            if (fabs (m_x - rhs .m_x) > fEpsilon) return false;
            if (fabs (m_y - rhs .m_y) > fEpsilon) return false;
            if (fabs (m_z - rhs .m_z) > fEpsilon) return false;
            if (fabs (m_w - rhs .m_w) > fEpsilon) return false;
            return true;
        }

        // @cmember Equality check

        bool operator == (const CVector4 &rhs) const 
        {
            return m_x == rhs .m_x && m_y == rhs .m_y &&
                m_z == rhs .m_z && m_w == rhs .m_w;
        }

        // @cmember Return value as a 3d vector

        operator CVector3 () const 
        {
            return CVector3 (m_x, m_y, m_z);
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

#endif // ETS_VECTOR4_H

/* vim: set sw=4: */
