#ifndef ETS_VECTOR3_H
#define ETS_VECTOR3_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      Vector3.h - 3d vectors |
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

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include <math.h>

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

#ifdef _WIN32
#pragma warning (push)
#pragma warning (disable : 4201)
#endif

class CVector3
{
    // @access Construction and destruction
    public:

        // @cmember General constructor

        inline CVector3 ()
        {
        }

        // @cmember Constructor from 3 values

        inline CVector3 (float x, float y, float z) : m_x (x), m_y (y), m_z (z) 
    {
    }

        // @cmember Copy constructor

        inline CVector3 (const CVector3 &src) : m_x (src .m_x), 
        m_y (src .m_y), m_z (src .m_z)
    {
    }

        // @cmember 2d constructor (or something like that)

        inline CVector3 (float x, float y) : m_x (x) , m_y (y)
    {
        float z = 1.0f - ((x * x) + (y * y));
        if (z <= 0.0)
        {
            m_z = 0.0;
            Normalize ();
        } 
        else 
        {
            m_z = (float) sqrt (z);
        }
    }

        // @access Public methods
    public:

        // @cmember Compute the light factors 

        void CalcLightFactors (const CVector3 &vSource, float fSpecularExponent, 
                               float *pfDiffuse, float *pfSpecular);

        // @access Public static methods
    public:

        // @cmember Compute a 2d intersection

        static CVector3 Intersection (const CVector3 &p1, const CVector3 &v1, 
                                      const CVector3 &p2, const CVector3 &v2);

        // @access Public inline methods
    public:

        // @cmember Compute the dot product

        inline float Dot (const CVector3 &rhs) const 
        {
            return m_x * rhs .m_x + m_y * rhs .m_y + m_z * rhs .m_z;
        }

        // @cmember Compute the cross product returned as a vector

        inline CVector3 Cross (const CVector3 &rhs) const 
        {
            return CVector3 (
                m_y * rhs .m_z - m_z * rhs .m_y,
                m_z * rhs .m_x - m_x * rhs .m_z,
                m_x * rhs .m_y - m_y * rhs .m_x);
        }

        // @cmember Compute the cross assigned to the "this"

        inline void Cross (const CVector3 &lhs, const CVector3 &rhs)
        {
            *this = lhs .Cross (rhs);
        }

        // @cmember Compute the length of the vector

        inline float Length () const 
        {
            return (float) sqrt (LengthSqr ());
        }

        // @cmember compute the length squared

        inline float LengthSqr () const 
        {
            return m_x * m_x + m_y * m_y + m_z * m_z;
        }

        // @cmember Normalize the vector and return the length

        inline float Normalize ()
        {
            float fLength = Length ();
            if (fLength == 0) 
                return 0;
            m_x /= fLength;
            m_y /= fLength;
            m_z /= fLength;
            return fLength;
        }

        // @cmember Normalize the vector with an epsilon check for null vectors

        inline float EpsilonNormalize (float fEpsilon = 0x000001f)
        {
            float fLength = Length ();
            if (fLength < fEpsilon) 
            {
                m_x = m_y = m_z = 0.0;
                return 0;
            }
            m_x /= fLength;
            m_y /= fLength;
            m_z /= fLength;
            return fLength;
        }

        // @cmember Initialize the vector to (0, 0, 0)

        inline void Clear () 
        {
            m_x = m_y = m_z = 0;
        }

        // @cmember Return the index of the largest component

        inline int LargestComponent () const 
        {
            if (fabs (m_x) > fabs (m_y) && fabs (m_x) > fabs (m_z)) return 0;
            if (fabs (m_y) > fabs (m_z)) return 1;
            return 2;
        }

        // @cmember Get the 2d quadrent of the vector

        inline int GetQuadrent2D () const 
        {
            if (m_x == 0.0) 
            {
                if (m_y == 0.0) 
                    return 0;
                return m_y > 0.0 ? 1 : 3;
            } 
            else if (m_x > 0.0)
            {
                return m_y >= 0.0 ? 0 : 3;
            } 
            else 
            {
                return m_y > 0.0 ? 1 : 2;
            }
        }

        // @access Public operators
    public:

        // @cmember Vector assignment

        inline const CVector3 &operator = (const CVector3 &rhs)
        {
            m_x = rhs .m_x;
            m_y = rhs .m_y;
            m_z = rhs .m_z;
            return *this;
        }

        // @cmember Vector assignment addition 

        inline const CVector3 &operator += (const CVector3 &rhs)
        {
            m_x += rhs .m_x;
            m_y += rhs .m_y;
            m_z += rhs .m_z;
            return *this;
        }

        // @cmember Vector addition 

        inline const CVector3 operator + (const CVector3 &rhs) const 
        {
            return CVector3 (m_x + rhs .m_x, m_y + rhs .m_y, m_z + rhs .m_z);
        }

        // @cmember Vector assignment subtraction 

        inline CVector3 &operator -= (const CVector3 &rhs)
        {
            m_x -= rhs .m_x;
            m_y -= rhs .m_y;
            m_z -= rhs .m_z;
            return *this;
        }

        // @cmember Vector subtraction 

        inline CVector3 operator - (const CVector3 &rhs) const
        {
            return CVector3 (m_x - rhs .m_x, m_y - rhs .m_y, m_z - rhs .m_z);
        }

        // @cmember Vector assignment scaling 

        inline const CVector3 &operator *= (float fScale) 
        {
            m_x *= fScale;
            m_y *= fScale;
            m_z *= fScale;
            return *this;
        }

        // @cmember Vector scaling

        inline CVector3 operator * (float fScale) const
        {
            return CVector3 (m_x * fScale, m_y * fScale, m_z * fScale);
        }

        // @cmember Vector assignment multiplication 

        inline const CVector3 &operator *= (const CVector3 &rhs)
        {
            m_x *= rhs .m_x;
            m_y *= rhs .m_y;
            m_z *= rhs .m_z;
            return *this;
        }

        // @cmember Vector multiplication 

        CVector3 operator * (const CVector3 &rhs) const 
        {
            return CVector3 (m_x * rhs .m_x, m_y * rhs .m_y, m_z * rhs .m_z);
        }

        // @cmember Vector assignment scalar division

        inline const CVector3 operator /= (float fScale) 
        {
            m_x /= fScale;
            m_y /= fScale;
            m_z /= fScale;
            return (*this);
        }

        // @cmember Vector scalar division

        inline CVector3 operator / (float fScale) const
        {
            return CVector3 (m_x / fScale, m_y / fScale, m_z / fScale);
        }

        // @cmember Vector negation

        inline CVector3 operator - () const 
        {
            return CVector3 (- m_x, - m_y, - m_z);
        }

        // @cmember Vector epsilon equality check

        inline bool AlmostEqual (const CVector3 &rhs, float fEpsilon) const 
        {
            if (fabs (m_x - rhs .m_x) > fEpsilon) return false;
            if (fabs (m_y - rhs .m_y) > fEpsilon) return false;
            if (fabs (m_z - rhs .m_z) > fEpsilon) return false;
            return true;
        }

        // @cmember Vector epsilon zero check

        inline bool AlmostZero (float fEpsilon) const 
        {
            if (fabs (m_x) > fEpsilon) return false;
            if (fabs (m_y) > fEpsilon) return false;
            if (fabs (m_z) > fEpsilon) return false;
            return true;
        }

        // @cmember Vector equality

        inline bool operator == (const CVector3 &rhs) const 
        {
            return m_x == rhs .m_x && m_y == rhs .m_y && m_z == rhs .m_z;
        }

        // @cmember Vector non-equality

        inline bool operator != (const CVector3 &rhs) const 
        {
            return m_x != rhs .m_x || m_y != rhs .m_y || m_z != rhs .m_z;
        }

        // @cmember Vector self negation

        inline const CVector3 &Negate ()
        {
            m_x = - m_x;
            m_y = - m_y;
            m_z = - m_z;
            return *this;
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
            };
            float m_v [3];
        };
};

extern const CVector3 g_vOrigin;

#ifdef _WIN32
#pragma warning (pop)
#endif

#endif // ETS_VECTOR3_H

/* vim: set sw=4: */
