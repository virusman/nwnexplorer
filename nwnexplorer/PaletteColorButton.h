#ifndef ETS_PALETTECOLORBUTTON_H
#define ETS_PALETTECOLORBUTTON_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      PaletteColorButton.h - Color button |
//
// This module contains the definition of the color button control.
//
// Based on work by Chris Maunder, Alexander Bischofberger and James White.
// (See following original copyright statement)
//
// http://www.codetools.com/miscctrl/PaletteColorButton.asp
// http://www.codetools.com/miscctrl/colour_picker.asp
//
// Copyright (c) 2000-2002 - Descartes Systems Sciences, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met:
// 
// 1. Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
// 2. Neither the name of Descartes Systems Sciences, Inc nor the names of 
//    its contributors may be used to endorse or promote products derived 
//    from this software without specific prior written permission.
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
// --- ORIGINAL COPYRIGHT STATEMENT ---
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
//
// Updated 30 May 1998 to allow any number of colours, and to
//                     make the appearance closer to Office 97. 
//                     Also added "Default" text area.         (CJM)
//
//         13 June 1998 Fixed change of focus bug (CJM)
//         30 June 1998 Fixed bug caused by focus bug fix (D'oh!!)
//                      Solution suggested by Paul Wilkerson.
//
// ColourPopup is a helper class for the colour picker control
// CColourPicker. Check out the header file or the accompanying 
// HTML doc file for details.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 
//
// @end
//
// $History: PaletteColorButton.h $
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Test for themes
//
//-----------------------------------------------------------------------------

#if !defined (PALETTECOLORBUTTON_NOTHEMES) && !defined (__ATLTHEME_H__)
#define PALETTECOLORBUTTON_NOTHEMES
#endif

//-----------------------------------------------------------------------------
//
// Notification messages
//
//-----------------------------------------------------------------------------

#define CPN_SELCHANGE        0x8000     /* Colour Picker Selection change */
#define CPN_DROPDOWN         0x8001     /* Colour Picker drop down */
#define CPN_CLOSEUP          0x8002     /* Colour Picker close up */
#define CPN_SELENDOK         0x8003     /* Colour Picker end OK */
#define CPN_SELENDCANCEL     0x8004     /* Colour Picker end (cancelled) */

struct NMPALETTECOLORBUTTON
{
    NMHDR               hdr;
    BOOL                fColorValid;
    int                 nIndex;
};

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CPaletteColorButton :
    public CWindowImpl <CPaletteColorButton>
#if !defined (PALETTECOLORBUTTON_NOTHEMES)
    , public CThemeImpl <CPaletteColorButton>
#endif
{
    // @access Types and enumerations
    public:

        struct ColorTableEntry
        {
            COLORREF    clrColor;
            LPCTSTR             pszName;
        };

        // @access Construction and destruction
    public:

        // @cmember General constructor

        CPaletteColorButton ();

        // @cmember General destructor

        ~CPaletteColorButton ();

        // @access Public inline methods
    public:

        // @cmember Subclass the window

        BOOL SubclassWindow (HWND hWnd);

        // @cmember Get the current color index

        int GetColorIndex (void) const
        {
            return m_nCurrent;
        }

        // @cmember Set the current color index

        void SetColorIndex (int nCurrent)
        {
            m_nCurrent = nCurrent;
            if (IsWindow ())
                InvalidateRect (NULL);
        }

        // @cmember Set the bitmap

        void SetBitmap (UINT nBitmapID)
        {
            m_nBitmapID = nBitmapID;
            m_bitmap .LoadBitmap (nBitmapID);
        }

        // @cmember Get the tracking flag

        BOOL GetTrackSelection (void) const
        {
            return m_fTrackSelection;
        }

        // @cmember Set the tracking flag

        void SetTrackSelection (BOOL fTrack)
        {
            m_fTrackSelection = fTrack;
        }

        // @access ATL window support
    public:

        BEGIN_MSG_MAP (CPaletteColorButton)
#if !defined (PALETTECOLORBUTTON_NOTHEMES)
            CHAIN_MSG_MAP (CThemeImpl <CPaletteColorButton>)    // should be here, not at bottom
#endif

            MESSAGE_HANDLER (WM_MOUSEMOVE, OnMouseMove);
        MESSAGE_HANDLER (WM_MOUSELEAVE, OnMouseLeave);

        MESSAGE_HANDLER (OCM__BASE + WM_DRAWITEM, OnDrawItem)

            REFLECTED_COMMAND_CODE_HANDLER (BN_CLICKED, OnClicked)

            ALT_MSG_MAP (1)

            MESSAGE_HANDLER (WM_PAINT, OnPickerPaint);
        MESSAGE_HANDLER (WM_QUERYNEWPALETTE, OnPickerQueryNewPalette);
        MESSAGE_HANDLER (WM_PALETTECHANGED, OnPickerPaletteChanged);
        END_MSG_MAP ()

            // @access ATL Message handlers
    protected:

            // @cmember Handle draw item

            LRESULT OnDrawItem (UINT uMsg, 
                                WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle mouse move

            LRESULT OnMouseMove (UINT uMsg, 
                                 WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle mouse leave

            LRESULT OnMouseLeave (UINT uMsg, 
                                  WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle key down for picker

            LRESULT OnPickerKeyDown (UINT uMsg, 
                                     WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle button up event for picker

            LRESULT OnPickerLButtonUp (UINT uMsg, 
                                       WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle mouse move for picker

            LRESULT OnPickerMouseMove (UINT uMsg, 
                                       WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle paint for picker

            LRESULT OnPickerPaint (UINT uMsg, 
                                   WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle palette query for picker

            LRESULT OnPickerQueryNewPalette (UINT uMsg, 
                                             WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle palette change for picker

            LRESULT OnPickerPaletteChanged (UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @access Notification handlers
    public:

            // @cmember Handle on click

            LRESULT OnClicked (WORD wNotifyCode, WORD wID, 
                               HWND hWndCtl, BOOL& bHandled);

            // @access Protected methods
    protected:

            // @cmember Display the picker popup

            BOOL Picker ();

            // @cmember Set the window size of the picker control

            void SetPickerWindowSize ();

            // @cmember Get the rect of a given cell

            BOOL GetPickerCellRect (int nIndex, RECT *pRect) const;

            // @cmember Set a new selection

            void ChangePickerSelection (int nIndex);

            // @cmember End the picker selection process

            void EndPickerSelection (BOOL fOked);

            // @cmember Send notification message

            void SendNotification (UINT nCode, int nIndex, BOOL fColorValid);

            // @cmember Do a hit test

            int PickerHitTest (const POINT &pt);

            // @cmember Draw a highlight

            void DrawHighlight (CDC &dc, CBrush &br, CRect &rect, int nWidth); 


            // @access Protected static methods
    protected:

            // @cmember Draw an arrow

            static void DrawArrow (CDC &dc, const RECT &rect,
                                   int iDirection = 0, COLORREF clrArrow = RGB (0, 0, 0));

            // @access Protected members
    protected:

            //
            // THE FOLLOWING variables control the actual button
            //

            // @cmember Current color index

            int                                         m_nCurrent;

            // @cmember True if popup active override

            BOOL                                        m_fPopupActive;

            // @cmember True if tracking selection

            BOOL                                        m_fTrackSelection;

            // @cmember True if the mouse is over

            BOOL                                        m_fMouseOver;

            // @cmember Bitmap ID

            UINT                                        m_nBitmapID;

            // @cmember Picker bitmap

            CBitmap                                     m_bitmap;

            //
            // THE FOLLOWING variables control the popup
            //

            // @cmember The contained picker control

            CContainedWindow            m_wndPicker;

            // @cmember Current picker color index

            int                                         m_nPicker;

            // @cmember Margins for the picker

            CRect                                       m_rectMargins;

            // @cmember Rectangle of the boxes

            CRect                                       m_rectBoxes;

            // @cmember If true, menu is flat

            BOOL                                        m_fPickerFlat;

            // @cmember Size of the color boxes 

            CSize                                       m_sizeBox;

            // @cmember Picker current selection

            int                                         m_nCurrentSel;

            // @cmember The original user selection

            int                                         m_nChosenColorSel;

            // @cmember If true, the picker was OK, and no canceled out

            BOOL                                        m_fOked;

            // @cmember Color used to draw background

            COLORREF                            m_clrBackground;

            // @cmember Color used for highlight border

            COLORREF                            m_clrHiLightBorder;

            // @cmember Color used for highlight 

            COLORREF                            m_clrHiLight;

            // @cmember Color used for low-light

            COLORREF                            m_clrLoLight;

            // @cmember Color used for highlight text

            COLORREF                            m_clrHiLightText;

            // @cmember Color used for normal text

            COLORREF                            m_clrText;
};

#endif // ETS_PALETTECOLORBUTTON_H

/* vim: set sw=4: */
