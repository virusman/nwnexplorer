#ifndef ETS_MATRIX_H
#define ETS_MATRIX_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      Matrix.h - Matrix support |
//
// This module contains the support matricies.
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
#include "Vector4.h"
#include "Quaternion.h"

//-----------------------------------------------------------------------------
//
// @class CMatrix | The class provides support for matries
//
//-----------------------------------------------------------------------------

class CMatrix
{
    // @access Constructors and Destructors
    public:

        // @cmember Constructor.

        CMatrix ();

        // @cmember Copy constructor.

        CMatrix (const CMatrix &src);

        // @access Public Methods
    public:

        // @cmember Multiply a vector by the matrix.

        void PreTranslate (const CVector3 &vec, CVector3 &vecOut) const;

        // @cmember Multiply a vector by the matrix.

        void PreTranslate (const CVector3 &vec, CVector4 &vecOut) const;

        // @cmember Multiply a normal by the matrix.

        void PreRotate (const CVector3 &vec, CVector3 &vecOut) const;

        // @cmember Multiply a matrix by the matrix.

        void PostRotate (const CVector3 &vec, CVector3 &vecOut) const;

        // @cmember Set the rotation part of the matrix

        void SetRotation (float fAngle, const CVector3 &vecXYZ);

        // @cmember Set the rotation part of the matrix

        void SetRotation (float fAngle, float fX, float fY, float fZ);

        // @cmember Make the matrix a rotation matrix

        void MakeRotation (float fAngle, float fX, float fY, float fZ);

        // @cmember Make the matrix a rotation matrix

        void MakeRotation (float fAngle, const CVector3 &vecXYZ);

        // @cmember Set the rotation part of the matrix

        void SetHPR (const CVector3 &vecHPR);

        // @cmember Set the rotation part of the matrix

        void SetHPR (float fH, float fP, float fR);

        // @cmember Make the matrix a rotation matrix

        void MakeHPR (float fH, float fP, float fR);

        // @cmember Make the matrix a rotation matrix

        void MakeHPR (const CVector3 &vecHPR);

        // @cmember Set the translation part of the matrix

        void SetTranslate (const CVector3 &vecXYZ);

        // @cmember Set the translation part of the matrix

        void SetTranslate (float fX, float fY, float fZ);

        // @cmember Make the matrix a translation matrix

        void MakeTranslate (float fX, float fY, float fZ);

        // @cmember Make the matrix a translation matrix

        void MakeTranslate (const CVector3 &vecXYZ);

        // @cmember Apply a translation to the matrix

        void Translate (const CVector3 &vecXYZ);

        // @cmember Set the matrix to look at a given point

        void MakeLookAt (const CVector3 &vEye, const CVector3 &vCenter, const CVector3 &vUp);

        // @cmember Make the matrix an identity matrix

        void MakeIdentity ();

        // @cmember Set the rotation based on the given unit vector

        void SetRotation (const CVector3 &s, const CVector3 &t);

        // @cmember Swap two rows in the matrix

        void SwapRows (int nRow1, int nRow2);

        // @cmember Flip the matrix

        void Flip ();

        // @cmember Get a column of the matrix

        void GetCol (int nCol, CVector3 &v) const;

        // @cmember Set the column of the matrix

        void SetCol (int nCol, const CVector3 &v); 

        // @cmember Get the row of the matrix

        void GetRow (int nRow, CVector3 &v) const;

        // @cmember Set the row of the matrix

        void SetRow (int nRow, const CVector3 &v); 

        // @cmember Set from a quaternion

        void SetQuaternion (const CQuaternion &v);

        // @cmember Make the matrix a quaternion matrix

        void MakeQuaternion (const CQuaternion &v);

        // @cmember Multiply two matricies

        void Multiply4 (const CMatrix &rhs, CMatrix &out) const;

        // @cmember Multiply two matricies

        void Multiply3 (const CMatrix &rhs, CMatrix &out) const;

        // @access Public operators
    public:

        // @cmember Multiply two matricies, return the results

        CMatrix operator * (const CMatrix &rhs) const;

        // @cmember Multiple a matrix by another matrix and store result in matrix

        const CMatrix &operator *= (const CMatrix &rhs);

        // @cmember Test for equality of two matricies

        bool operator == (const CMatrix &rhs) const;

        // @cmember Test for inequality of two matricies

        bool operator != (const CMatrix &rhs) const;

        // @cmember Set the contents of the matrix using a 4x4 floating point matrix

        void operator = (const float matrix [4] [4]);

        // @access Public static methods
    public:

        // @cmember Get the sine and cosine of an angle

        static void GetCosSin (float fAngle, float &fCos, float &fSin);

        // @access Public variables
    public:

        // @cmember The values of the matrix in row/col form

        float m_m [4] [4]; // column major form (row/col)
};

//-----------------------------------------------------------------------------
//
// Include the inline methods
//
//-----------------------------------------------------------------------------

#include "Matrix.inl"

#endif // ETS_MATRIX_H

/* vim: set sw=4: */
