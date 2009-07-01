#ifndef ETS_NWNMODEL_H
#define ETS_NWNMODEL_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnModel.h - Nwn model (MFDL) file support |
//
// This module contains the definition of the NwnModel.
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
// Based on information from:
//
// http://nwn-j3d.sf.net
//
// @end
//
// $History: CnfMainWnd.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "NwnMemoryFile.h"
#include "NwnMdlGeometry.h"
#include "NwnMdlNodes.h"

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

template <class TClass>
class CNwnModelImpl
{
    public:

        typedef bool (__cdecl FNNWNMODELENUM) (TClass *pFile, void *lParam, 
                                               void *pData, bool fIsGeometryHeader, UINT32 ulFlags, int nOffset);

        enum EnumFlags
        {
            EnumFlag_Geometry                   = 0x00000001,
            EnumFlag_Animation                  = 0x00000002,
        };

        struct Header
        {
            UINT32                                      ulZero;                                                 // 0x0000
            UINT32                                      ulRawDataOffset;                                // 0x0004
            UINT32                                      ulRawDataSize;                                  // 0x0008
        };

        // @access Public methods
    public:

        // @cmember Does this offset point to a geometry header

        bool IsGeometryHeader (int nOffset);

        // @access Public inline methods
    public:

        // @cmember Get the header

        Header *GetHeader () 
        {
            TClass *pT = static_cast <TClass *> (this);
            return (Header *) pT ->GetData (0);
        }

        // @cmember Get a pointer in the model data

        unsigned char *GetModelData (int nOffset = 0) 
        {
            TClass *pT = static_cast <TClass *> (this);
            Header *pHeader = GetHeader ();
            return pT ->GetData (sizeof (Header) + nOffset);
        }

        // @cmember Get a pointer in the raw data

        unsigned char *GetRawData (int nOffset = 0) 
        {
            TClass *pT = static_cast <TClass *> (this);
            Header *pHeader = GetHeader ();
            return pT ->GetData (sizeof (Header) + 
                                 pHeader ->ulRawDataOffset + nOffset);
        }

        // @cmember Get the raw offset

        int GetRawDataOffset () 
        {
            Header *pHeader = GetHeader ();
            return pHeader ->ulRawDataOffset;
        }

        // @cmember Get the raw data size

        int GetRawDataSize () 
        {
            Header *pHeader = GetHeader ();
            return pHeader ->ulRawDataSize;
        }

        // @cmember Enumerate the model

        bool EnumModel (FNNWNMODELENUM *pEnumFunc, void *lParam, 
                        UINT32 ulFlags = EnumFlag_Geometry | EnumFlag_Animation)
        {
            return HandleModelHeader (pEnumFunc, lParam, ulFlags, 0);
        }

        // @access Protected methods
    protected:

        // @cmember Handle the enumeration of a node

        bool HandleNodeHeader (FNNWNMODELENUM *pEnumFunc, 
                               void *lParam, UINT32 ulFlags, int nOffset);

        // @cmember Handle the animation header

        bool HandleAnimationHeader (FNNWNMODELENUM *pEnumFunc, 
                                    void *lParam, UINT32 ulFlags, int nOffset);

        // @cmember Handle the model header

        bool HandleModelHeader (FNNWNMODELENUM *pEnumFunc, 
                                void *lParam, UINT32 ulFlags, int nOffset);
};

//-----------------------------------------------------------------------------
//
// @mfunc Handle the enumeration of a node
//
// @parm FNNWNMODELENUM * | pEnumFunc | Caller supplied callback
//
// @parm void * | lParam | Caller supplied parameter
//
// @parm UINT32 | ulFlags | Flags
//
// @parm int | nOffset | Offset to the node.
//
// @rdesc If true, continue enumeration.  If false, cancel.
//
//-----------------------------------------------------------------------------

    template <class TClass>
bool CNwnModelImpl <TClass>::HandleNodeHeader (FNNWNMODELENUM *pEnumFunc, 
                                               void *lParam, UINT32 ulFlags, int nOffset)
{

    //
    // Get the node type
    //

    CNwnMdlNodeHeader *pNodeHeader = (CNwnMdlNodeHeader *) GetModelData (nOffset);

    //
    // Invoke the enum function
    //

    if (!pEnumFunc (static_cast <TClass *> (this), lParam,
                    pNodeHeader, false, ulFlags, nOffset))
        return false;

    //
    // Handle the children
    //

    if (pNodeHeader ->m_apChildren .GetCount () > 0)
    {
        CNwnPointer <CNwnMdlNodeHeader> *pulOffsets = pNodeHeader ->
            m_apChildren .GetData (GetModelData ());
        for (int i = 0; i < pNodeHeader ->m_apChildren .GetCount (); i++)
        {
            if (!HandleNodeHeader (pEnumFunc, lParam, 
                                   ulFlags, pulOffsets [i] .GetOffset ()))
                return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the enumeration of the animation header
//
// @parm FNNWNMODELENUM * | pEnumFunc | Caller supplied callback
//
// @parm void * | lParam | Caller supplied parameter
//
// @parm UINT32 | ulFlags | Flags
//
// @parm int | nOffset | Offset to the node.
//
// @rdesc If true, continue enumeration.  If false, cancel.
//
//-----------------------------------------------------------------------------

    template <class TClass>
bool CNwnModelImpl <TClass>::HandleAnimationHeader (FNNWNMODELENUM *pEnumFunc, 
                                                    void *lParam, UINT32 ulFlags, int nOffset)
{

    //
    // Get the header
    //

    CNwnMdlAnimation *pAnimationHeader = 
        (CNwnMdlAnimation *) GetModelData (nOffset);

    //
    // Invoke the enum function
    //

    if (!pEnumFunc (static_cast <TClass *> (this), lParam, pAnimationHeader, 
                    true, EnumFlag_Animation, nOffset))
        return false;

    //
    // Enum the nodes
    //

    return HandleNodeHeader (pEnumFunc, lParam, EnumFlag_Animation, 
                             pAnimationHeader ->m_pGeometry .GetOffset ());
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the enumeration of the model header
//
// @parm FNNWNMODELENUM * | pEnumFunc | Caller supplied callback
//
// @parm void * | lParam | Caller supplied parameter
//
// @parm UINT32 | ulFlags | Flags
//
// @parm int | nOffset | Offset to the node.
//
// @rdesc If true, continue enumeration.  If false, cancel.
//
//-----------------------------------------------------------------------------

    template <class TClass>
bool CNwnModelImpl <TClass>::HandleModelHeader (FNNWNMODELENUM *pEnumFunc, 
                                                void * lParam, UINT32 ulFlags, int nOffset)
{

    //
    // Get the header
    //

    CNwnMdlModel *pModelHeader = 
        (CNwnMdlModel *) GetModelData (nOffset);

    //
    // Invoke the enum function
    //

    if (!pEnumFunc (static_cast <TClass *> (this), lParam, 
                    pModelHeader, true, 0, nOffset))
        return false;

    //
    // If we are to do geometry
    //

    if (ulFlags & EnumFlag_Geometry)
    {
        if (!HandleNodeHeader (pEnumFunc, lParam, 
                               EnumFlag_Geometry, pModelHeader ->m_pGeometry .GetOffset ()))
            return false;
    }

    //
    // Handle the animations
    //

    if (ulFlags & EnumFlag_Animation)
    {
        if (pModelHeader ->m_apAnimations .GetCount () > 0)
        {
            CNwnPointer <CNwnMdlAnimation> *pulOffsets = pModelHeader ->m_apAnimations 
                .GetData (GetModelData ());
            for (int i = 0; i < pModelHeader ->m_apAnimations .GetCount (); i++)
            {
                if (!HandleAnimationHeader (pEnumFunc, lParam, 
                                            EnumFlag_Animation, pulOffsets [i] .GetOffset ()))
                    return false;
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Test to see if the offset is a geometry header
//
// @parm int | nOffset | Offset of the data into the model
//
// @rdesc True if the header is a geometry header
//
//-----------------------------------------------------------------------------

    template <class TClass>
bool CNwnModelImpl <TClass>::IsGeometryHeader (int nOffset)
{

    //
    // If offset is zero, this must be the model header
    //

    if (nOffset == 0)
        return true;

    //
    // If not, then see if this is one of the animation header
    //

    CNwnMdlModel *pModelHeader = (CNwnMdlModel *) GetModelData (0);
    CNwnPointer <CNwnMdlAnimation> *pOffsets = pModelHeader ->
        m_apAnimations .GetData (GetModelData ());
    for (int i = 0; i < pModelHeader ->m_apAnimations .GetCount (); i++)
    {
        if (pOffsets [i] .GetOffset () == nOffset)
            return true;
    }

    //
    // Must be a normal node
    //

    return false;
}

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CNwnModel :
    public CNwnModelImpl <CNwnModel>,
    public CNwnMemoryFile
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnModel (unsigned char *pauchData = NULL, 
                   unsigned long ulSize = 0, bool fAllocated = true) :
            CNwnMemoryFile (pauchData, ulSize, fAllocated)
    {
    }
};

#endif // ETS_NWNMODEL_H

/* vim: set sw=4: */
