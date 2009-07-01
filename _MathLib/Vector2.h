#ifndef ETS_VECTOR2_H
#define ETS_VECTOR2_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      Vector2.h - 2d vector support |
//
// This module contains the support for 2d vectors
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
// Class definition
//
//-----------------------------------------------------------------------------

#ifdef _WIN32
#pragma warning (push)
#pragma warning (disable : 4201)
#endif

class CVector2
{
    // @access Construction and destruction
    public:

        // @cmember General constructor

        inline CVector2 ()
        {
        }

        // @cmember Constructor with initial values

        inline CVector2 (float x, float y) : m_x (x), m_y (y) 
    {
    }

        // @cmember Copy constructor

        inline CVector2 (const CVector2 &src) : m_x (src .m_x), m_y (src .m_y)
    {
    }

        // @access Public inline methods
    public:

        // @cmember Compute the dot product

        float Dot (const CVector2 &rhs) const 
        {
            return m_x * rhs .m_x + m_y * rhs .m_y;
        }

        // @cmember Return the length of the vector

        float Length () const 
        {
            return (float) sqrt (m_x * m_x + m_y * m_y);
        }

        // @cmember Normalize the vector and return the length

        float Normalize () 
        {
            float fLength = Length ();
            if (fLength == 0) 
                return 0;
            m_x /= fLength;
            m_y /= fLength;
            return fLength;
        }

        // @cmember Clear the vector to (0,0)

        void Clear () 
        {
            m_x = m_y = 0;
        }

        // @cmember Return the largest component

        int LargestComponent () const
        {
            if (fabs (m_x) > fabs (m_y)) return 0;
            return 1;
        }

        // @access Public operators
    public:

        // @cmember Vector assignment

        const CVector2 &operator = (const CVector2 &rhs)
        {
            m_x = rhs .m_x;
            m_y = rhs .m_y;
            return *this;
        }

        // @cmember Vector assignment addition

        const CVector2 &operator += (const CVector2 &rhs)
        {
            m_x += rhs .m_x;
            m_y += rhs .m_y;
            return *this;
        }

        // @cmember Vector addition

        const CVector2 operator + (const CVector2 &rhs) const 
        {
            return CVector2 (m_x + rhs .m_x, m_y + rhs .m_y);
        }

        // @cmember Vector assignment subtraction

        CVector2 &operator -= (const CVector2 &rhs)
        {
            m_x -= rhs .m_x;
            m_y -= rhs .m_y;
            return *this;
        }

        // @cmember Vector subtraction

        CVector2 operator - (const CVector2 &rhs) const
        {
            return CVector2 (m_x - rhs .m_x, m_y - rhs .m_y);
        }

        // @cmember Vector assignment scaling

        const CVector2 &operator *= (float fScale) 
        {
            m_x *= fScale;
            m_y *= fScale;
            return *this;
        }

        // @cmember Vector scaling

        CVector2 operator * (float fScale) const
        {
            return CVector2 (m_x * fScale, m_y * fScale);
        }

        // @cmember Vector scaling

        CVector2 operator / (float fScale) const
        {
            return CVector2 (m_x / fScale, m_y / fScale);
        }

        // @cmember Vector negation

        CVector2 operator - () const
        {
            return CVector2 (- m_x, - m_y);
        }

        // @cmember Vector epsilon equality check

        bool AlmostEqual (const CVector2 &rhs, float fEpsilon) const 
        {
            if (fabs (m_x - rhs .m_x) > fEpsilon) return false;
            if (fabs (m_y - rhs .m_y) > fEpsilon) return false;
            return true;
        }

        // @cmember Vector equality check

        bool operator == (const CVector2 &rhs) const 
        {
            return m_x == rhs .m_x && m_y == rhs .m_y;
        }

        // @cmember Vector non-equality check

        bool operator != (const CVector2 &rhs) const 
        {
            return m_x != rhs .m_x || m_y != rhs .m_y;
        }

        // @access Public variables
    public:
        union 
        {
            struct
            {
                float m_x;
                float m_y;
            };
            float m_v [2];
        };
};

#ifdef _WIN32
#pragma warning (pop)
#endif

#endif // ETS_VECTOR2_H

/* vim: set sw=4: */
