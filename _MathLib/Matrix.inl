#ifndef ETS_MATRIX_INL
#define ETS_MATRIX_INL

#include <memory.h>
#include <assert.h>

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module	Matrix.inl - Matrix support |
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

template <class _Tp>
inline void matrix_swap (_Tp& __a, _Tp& __b) 
{
  _Tp __tmp = __a;
  __a = __b;
  __b = __tmp;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CMatrix> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline CMatrix::CMatrix () 
{
	//
	// Set the matrix to the identity matrix
	//

	MakeIdentity ();
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CMatrix> copy constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline CMatrix::CMatrix (const CMatrix &src) 
{
	//
	// Copy the contents of the matrix
	//

	*this = src .m_m;
}

//-----------------------------------------------------------------------------
//
// @mfunc Make the matrix into an identity matrix
//
//		Reset the matrix to an indentity matrix.
//
// @parm void | None. |
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::MakeIdentity () 
{
	m_m [0] [0] = m_m [1] [1] = m_m [2] [2] = m_m [3] [3] = 1;
	m_m [0] [1] = m_m [0] [2] = m_m [0] [3] = 0;
	m_m [1] [0] = m_m [1] [2] = m_m [1] [3] = 0;
	m_m [2] [0] = m_m [2] [1] = m_m [2] [3] = 0;
	m_m [3] [0] = m_m [3] [1] = m_m [3] [2] = 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the rotation part of the matrix
//
//		Only the rotation part of the matrix is set by this routine.  The
//		previous setting for the rotation part of the matrix is lost.
//
// @parm const CVector3 & | vecHPR | A vector containing the heading,
//		pitch, and roll of the rotation.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::SetHPR (const CVector3 &vecHPR)
{
	SetHPR (vecHPR .m_x, vecHPR .m_y, vecHPR .m_z);
}

//-----------------------------------------------------------------------------
//
// @mfunc Make the matrix a rotation matrix
//
//		Reset the matrix to an indentity matrix and then apply a rotation 
//		to it.
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

inline void CMatrix::MakeHPR (float fH, float fP, float fR)
{
	MakeIdentity ();
	SetHPR (fH, fP, fR);
}

//-----------------------------------------------------------------------------
//
// @mfunc Make the matrix a rotation matrix
//
//		Reset the matrix to an indentity matrix and then apply a rotation 
//		to it.
//
// @parm const CVector3 & | vecHPR | A vector containing the heading,
//		pitch, and roll of the rotation.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::MakeHPR (const CVector3 &vecHPR)
{
	MakeHPR (vecHPR .m_x, vecHPR .m_y, vecHPR .m_z);
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the rotation part of the matrix
//
//		Only the rotation part of the matrix is set by this routine.  The
//		previous setting for the rotation part of the matrix is lost.
//
// @parm float | fAngle | The angle of the rotation in degrees
//
// @parm const CVector3 & | vecXYZ | Normal that specifies the axis 
//		of rotation.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::SetRotation (float fAngle, const CVector3 &vecXYZ)
{
	//
	// Invoke the helper routine
	//

	SetRotation (fAngle, vecXYZ .m_x, vecXYZ .m_y, vecXYZ .m_z);
}

//-----------------------------------------------------------------------------
//
// @mfunc Make the matrix a rotation matrix
//
//		Reset the matrix to an indentity matrix and then apply a rotation 
//		to it.
//
// @parm float | fAngle | The angle of the rotation in degrees
//
// @parm float | fX | The x coordinate of the normal that specifies
//		the axis of the rotation.
//
// @parm float | fY | The y coordinate of the normal that specifies
//		the axis of the rotation.
//
// @parm float | fZ | The z coordinate of the normal that specifies
//		the axis of the rotation.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::MakeRotation (float fAngle, float fX, float fY, float fZ)
{
	//
	// Make the matrix an identity matrix
	//

	MakeIdentity ();

	//
	// Set the rotation elements
	//

	SetRotation (fAngle, fX, fY, fZ);
}

//-----------------------------------------------------------------------------
//
// @mfunc Make the matrix a rotation matrix
//
//		Reset the matrix to an indentity matrix and then apply a rotation 
//		to it.
//
// @parm float | fAngle | The angle of the rotation in degrees
//
// @parm const CVector3 & | vecXYZ | Normal that specifies the axis 
//		of rotation.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::MakeRotation (float fAngle, const CVector3 &vecXYZ)
{
	//
	// Invoke the helper routine
	//

	MakeRotation (fAngle, vecXYZ .m_x, vecXYZ .m_y, vecXYZ .m_z);
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the translation portion of the matrix
//
//		Only the translation part of the matrix is set by this routine.  
//		The previous setting for the translation part of the matrix is lost.
//
// @parm float | fX | The X coordinate of the translation
//
// @parm float | fY | The Y coordinate of the translation
//
// @parm float | fZ | The Z coordinate of the translation
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::SetTranslate (float fX, float fY, float fZ)
{
	//
	//	Set the matrix
	//

	m_m [3] [0] = fX;
	m_m [3] [1] = fY;
	m_m [3] [2] = fZ;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the translation part of the matrix
//
//		Given a vector, set the translation of the matrix.  All other
//		elements of the matrix are preserved, but the current translation
//		values will be lost.
//
// @parm const CVector3 & | vecXYZ | Translation vector
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::SetTranslate (const CVector3 &vecXYZ)
{
	//
	// Invoke the helper routine
	//

	SetTranslate (vecXYZ .m_x, vecXYZ .m_y, vecXYZ .m_z);
}

//-----------------------------------------------------------------------------
//
// @mfunc Make the matrix a translation matrix
//
//		Given a vector, make the matrix a translation matrix.  All current
//		values in the matrix will be replaced with an identity matrix.
//
// @parm float | fX | The X coordinate of the translation vector
//
// @parm float | fY | The Y coordinate of the translation vector
//
// @parm float | fZ | The Z coordinate of the translation vector
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::MakeTranslate (float fX, float fY, float fZ)
{
	//
	// Make the matrix an identity matrix
	//

	MakeIdentity ();

	//
	// Set the translation
	//

	SetTranslate (fX, fY, fZ);
}

//-----------------------------------------------------------------------------
//
// @mfunc Make the matrix a translation matrix
//
//		Given a vector, make the matrix a translation matrix.  All current
//		values in the matrix will be replaced with an identity matrix.
//
// @parm const CVector3 & | vecXYZ | Translation vector
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::MakeTranslate (const CVector3 &vecXYZ)
{
	//
	// Invoke the helper routine
	//

	MakeTranslate (vecXYZ .m_x, vecXYZ .m_y, vecXYZ .m_z);
}

//-----------------------------------------------------------------------------
//
// @mfunc Swap the contents of two rows
//
//		Given two row numbers, swap all elements in the rows.
//
// @parm int | nRow1 | First row to swap (0-3)
//
// @parm int | nRow2 | Second row to swap (0-3)
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::SwapRows (int nRow1, int nRow2) 
{
	//
	// In debug, validate the range of the rows
	//

	assert (nRow1 >= 0 && nRow1 <= 3);
	assert (nRow2 >= 0 && nRow2 <= 3);

	//
	// Loop through the elements of the rows, swaping them
	//

	for (int i = 0; i < 4; i++) 
	{
		matrix_swap (m_m [i] [nRow1], m_m [i] [nRow2]);
	}
}

//-----------------------------------------------------------------------------
//
// @mfunc Apply a translation to the matrix
//
//		Given the translation vector, create a translation matrix and then
//		multiply this matrix and the translation matrix storing the results
//		in this matrix.
//
// @parm const CVector3 & | vecXYZ | The translation to be applied.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::Translate (const CVector3 &vecXYZ) 
{
	//
	// Make the translation matrix
	//

	CMatrix mt;
	mt .MakeTranslate (vecXYZ);

	//
	// Apply the translation
	//

	(*this) *= mt;
}

//-----------------------------------------------------------------------------
//
// @mfunc Flip the elements of the matrix along the identity diagonal
//
//		Flip the elements of the matrix along the identity diagonal		
//
// @parm void | None. |
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::Flip ()
{
	//
	// Flip the elements
	//

	matrix_swap (m_m [1] [0], m_m [0] [1]);
	matrix_swap (m_m [2] [0], m_m [0] [2]);
	matrix_swap (m_m [3] [0], m_m [0] [3]);
	matrix_swap (m_m [2] [1], m_m [1] [2]);
	matrix_swap (m_m [3] [1], m_m [1] [3]);
	matrix_swap (m_m [3] [2], m_m [2] [3]);
}

//-----------------------------------------------------------------------------
//
// @mfunc Multiply two matricies, return the results.
//
//		Given a matrix, multiply that matrix by this matrix and return the
//		results.
//		
//
// @parm const CMatrix & | rhs | Matrix to multiply
//
// @rdesc The results of the multiplication
//
//-----------------------------------------------------------------------------

inline CMatrix CMatrix::operator * (const CMatrix &rhs) const 
{
	//
	// Local definitions
	//

	CMatrix m;

	//
	// Invoke the main routine
	//

	Multiply4 (rhs, m);

	//
	// Return the results
	//

	return m;
}

//-----------------------------------------------------------------------------
//
// @mfunc Multiply two matricies, store result in this matrix
//
//		Given a matrix, multiply that matrix by this matrix.  Store the
//		results into this matrix.
//		
//
// @parm const CMatrix & | rhs | Matrix to multiply
//
// @rdesc The results of the multiplication
//
//-----------------------------------------------------------------------------

inline const CMatrix &CMatrix::operator *= (const CMatrix &rhs) 
{
	//
	// Multiply the matricies and store the results in this
	//

	*this = *this * rhs;

	//
	// Return this
	//

	return *this;
}

//-----------------------------------------------------------------------------
//
// @mfunc Test for equality of two matricies
//
//		Given a matrix, test to see if the given matrix is equal
//		to this matrix.
//		
//
// @parm const CMatrix & | rhs | Matrix to compare
//
// @rdesc The results of the comparison.
//
//		@flag true | The matricies where equal
//
//		@flag false | The matricies were not equal
//
//-----------------------------------------------------------------------------

inline bool CMatrix::operator == (const CMatrix &rhs) const
{
	//
	// Loop through the elements of the matricies.  If any elements does
	// not match, then return a false.
	//

	for (int i = 0; i < 4; i++) 
	{
		for (int j = 0; j < 4; j++) 
		{
			if (rhs .m_m [i] [j] != m_m [i] [j]) return false;
		}
	}

	//
	// If we get to this point, then all elements matched, return true
	//

	return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Test for inequality of two matricies
//
//		Given a matrix, test to see if the given matrix is not equal
//		to this matrix.
//		
//
// @parm const CMatrix & | rhs | Matrix to compare
//
// @rdesc The results of the comparison.
//
//		@flag true | The matricies where not equal
//
//		@flag false | The matricies were equal
//
//-----------------------------------------------------------------------------

inline bool CMatrix::operator != (const CMatrix &rhs) const
{
	//
	// Return the not of the equality comparison
	//

	return !(*this == rhs);
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the matrix using a 4x4 matrix
//
//		Set the matrix using a 4x4 matrix.  All previous values of the
//		matrix are lost.
//		
//
// @parm const float [4] [4] | matrix | New values for the matrix
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::operator = (const float matrix [4] [4])
{
	//
	// Memmove the values of the matrix
	//

	memcpy (m_m, matrix, sizeof (m_m));
}

//-----------------------------------------------------------------------------
//
// @mfunc Get a column of the given matrix
//
//		Given a column number return the values of the column in the vector
//		
//
// @parm int | nCol | Column number (0-3)
//
// @parm CVector3 & | v | Value of the column
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::GetCol (int nCol, CVector3 &v) const
{
	//
	// In debug, validate the range of the column
	//

	assert (nCol >= 0 && nCol <= 3);

	//
	// Get the column
	//

	v .m_x = m_m [0] [nCol];
	v .m_y = m_m [1] [nCol];
	v .m_z = m_m [2] [nCol];
}

//-----------------------------------------------------------------------------
//
// @mfunc Set a column of the given matrix with the given vector
//
//		Given a column number and a vector, set that column.  The 4th element
//		of the column is unchanged.
//		
//
// @parm int | nCol | Column number (0-3)
//
// @parm const CVector3 & | v | New value for the column
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::SetCol (int nCol, const CVector3 &v) 
{
	//
	// In debug, validate the range of the column
	//

	assert (nCol >= 0 && nCol <= 3);

	//
	// Set the column
	//

	m_m [0] [nCol] = v .m_x;
	m_m [1] [nCol] = v .m_y;
	m_m [2] [nCol] = v .m_z;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get a row of the given matrix
//
//		Given a row number return the elements of that row to a vector.
//		
//
// @parm int | nRow | Row number (0-3)
//
// @parm CVector3 & | v | The values of the row
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::GetRow (int nRow, CVector3 &v) const
{
	//
	// In debug, validate the range of the row
	//

	assert (nRow >= 0 && nRow <= 3);

	//
	// Get the row
	//

	v .m_x = m_m [nRow] [0];
	v .m_y = m_m [nRow] [1];
	v .m_z = m_m [nRow] [2];
}

//-----------------------------------------------------------------------------
//
// @mfunc Set a row of the given matrix with the given vector
//
//		Given a row number and a vector, set that row.  The 4th element
//		of the row is unchanged.
//		
//
// @parm int | nRow | Row number (0-3)
//
// @parm const CVector3 & | v | New value for the row
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::SetRow (int nRow, const CVector3 &v) 
{
	//
	// In debug, validate the range of the row
	//

	assert (nRow >= 0 && nRow <= 3);

	//
	// Set the row
	//

	m_m [nRow] [0] = v .m_x;
	m_m [nRow] [1] = v .m_y;
	m_m [nRow] [2] = v .m_z;
}

//-----------------------------------------------------------------------------
//
// @func Multiply a vector by a matrix
//
//		The routine will return the results of a vM where v is the vector
//		and M is the matrix.
//
// @parm const CVector3 & | lhs | The vector being multiplied
//
// @parm const CMatrix & | rhs | The matrix being multiplied
//
// @rdesc The result of the multiplication.
//
//-----------------------------------------------------------------------------

inline CVector3 operator * (const CVector3 &lhs, const CMatrix &rhs)
{
	CVector3 vout;
	rhs .PreTranslate (lhs, vout);
	return vout;
}

//-----------------------------------------------------------------------------
//
// @mfunc Make the matrix from a quaternion
//
// @parm const CQuaternion & | v | Source quaternion
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void CMatrix::MakeQuaternion (const CQuaternion &v)
{

	//
	// Initialize the matrix then set rotation matrix
	//

	MakeIdentity ();
	SetQuaternion (v);
}

#endif // ETS_MATRIX_INL
