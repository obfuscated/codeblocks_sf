#ifndef _WX_SPEEDBUTTON_H_
#define _WX_SPEEDBUTTON_H_


/*
* This file is part of a wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith and this file is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
* 
* Ron Collins
* rcoll@theriver.com
* 4-Feb-2010
* 
*/

#include "wx/defs.h"
#include "wx/wxprec.h"
#include "wx/control.h"
#include "wx/settings.h"
#include "wx/bitmap.h"
#include "wx/gdicmn.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/tglbtn.h"
#include "wx/image.h"
#include "wx/brush.h"
#include "wx/pen.h"
#include "wx/bitmap.h"
#include "wx/button.h"
#include "wx/renderer.h"
#include "wx/toplevel.h"
#include "wx/event.h"
#include "wx/datetime.h"
#include "wx/dynarray.h"





//-----------------------------------------------------------------------------
// wxSpeedButton
//-----------------------------------------------------------------------------

class wxSpeedButton : public wxControl
{
public:

// default contructor

            wxSpeedButton() { Create(); }

// destructor

virtual    ~wxSpeedButton();

// basic constructor

            wxSpeedButton(  wxWindow       *inParent,                               // parent window
                            wxWindowID      inID,                                   // id of this button
                            const wxString &inLabel         = _(""),                // button text
                            const wxBitmap &inGlyph         = wxNullBitmap,         // bitmaps displayed on button
                            int             inGlyphCount    = 0,                    // number of images in inGlyph
                            int             inMargin        = 2,                    // area around image and text
                            int             inGroupIndex    = 0,                    // ident of a group of buttons
                            bool            inAllowAllUp    = true,                 // allow all buttons up
                            const wxPoint  &inPos           = wxDefaultPosition,    // button position
                            const wxSize   &inSize          = wxDefaultSize,        // button size
                            long            inStyle         = 0,                    // border styles
                            const wxValidator &inVal        = wxDefaultValidator,   // validator
                            const wxString &inName          = _("SpeedButton"));    // name of button

// a constructor that creates a bitmap from a filename

            wxSpeedButton( wxWindow        *inParent,                               // parent window
                            wxWindowID      inID,                                   // id of this button
                            const wxString &inLabel         = _(""),                // button text
                            const wxString &inGlyphFile     = _(""),                // bitmaps displayed on button
                            int             inGlyphCount    = 0,                    // number of images in inGlyph
                            int             inMargin        = 2,                    // area around image and tex
                            int             inGroupIndex    = 0,                    // ident of a group of buttons
                            bool            inAllowAllUp    = true,                 // allow all buttons up
                            const wxPoint  &inPos           = wxDefaultPosition,    // button position
                            const wxSize   &inSize          = wxDefaultSize,        // button size
                            long            inStyle         = 0,                    // border styles
                            const wxValidator &inVal        = wxDefaultValidator,   // validator
                            const wxString &inName          = _("SpeedButton"));    // name of button



// create the basic speed-button object

            bool Create( wxWindow          *inParent        = NULL,                 // parent window
                            wxWindowID      inID            = wxNewId(),            // id of this button
                            const wxString &inLabel         = _(""),                // button text
                            const wxBitmap &inGlyph         = wxNullBitmap,         // bitmaps displayed on button
                            int             inGlyphCount    = 0,                    // number of images in inGlyph
                            int             inMargin        = 1,                    // area around image and text
                            int             inGroupIndex    = 0,                    // ident of a group of buttons
                            bool            inAllowAllUp    = true,                 // allow all buttons up
                            const wxPoint  &inPos           = wxDefaultPosition,    // button position
                            const wxSize   &inSize          = wxDefaultSize,        // button size
                            long            inStyle         = 0,                    // border styles
                            const wxValidator &inVal        = wxDefaultValidator,   // validator
                            const wxString &inName          = _("SpeedButton"));    // name of button





            void        SetLabel(wxString &inLabel);
            void        SetGlyphUp(wxBitmap &inBitmap);
            wxBitmap   &GetGlyphUp(void);
            void        SetGlyphDown(wxBitmap &inBitmap);
            wxBitmap   &GetGlyphDown(void);
            void        SetGlyphDisabled(wxBitmap &inBitmap);
            wxBitmap   &GetGlyphDisabled(void);

            void        SetAlign(int inAlign);
            int         GetAlign(void);
            void        SetMargin(int inMargin);
            int         GetMargin(void);
            void        SetGroupIndex(bool inIndex);
            int         GetGroupIndex(void);
            void        SetDown(bool inDown);
            bool        GetDown(void);
            void        SetValue(bool inDown);
            bool        GetValue(void);
            void        SetAllowAllUp(bool inAllUp);
            bool        GetAllowAllUp(void);
            void        SetUserData(long inData);
            long        GetUserData(void);

            void        DoClick(bool inLeft);


protected:

            void        SplitGlyphs(const wxBitmap &inBitmap, int inCount);
            void        MakeTransparent(wxBitmap &inBitmap);
            void        GetGlyphSize(wxBitmap &inGlyph, int &outWidth, int &outHeight);
virtual     wxSize      DoGetBestSize(void);            // calculate best button size given the current state
virtual     void        CalcLayout(bool inRefresh);     // calculate position for bitmap and label
            void        OnSetFocus(wxFocusEvent& event);
            void        OnKillFocus(wxFocusEvent& event);
            void        OnPaint(wxPaintEvent &event);
            void        Redraw();
virtual     void        Paint( wxDC &dc );
virtual     void        SendEvent(bool inLeft);
            void        OnMouseEvents(wxMouseEvent &event);
            void        OnSize( wxSizeEvent &event );
            void        SetAllUp(wxSpeedButton *inButton);
            void        OnKey(wxKeyEvent& event);



// button data

    wxBitmap                mGlyphUp;               // image when button is UP
    wxBitmap                mGlyphDown;             // image when button is DOWN
    wxBitmap                mGlyphDisabled;         // image when button is disabled
    int                     mMargin;                // area around image and text
    wxSize                  mGlyphSize;             // max area used by bitmaps
    wxSize                  mLabelSize;             // max area used by text label
    wxSize                  mBestSize;              // calculated best size of button
    wxPoint                 mGlyphPos;              // position of the bitmap
    wxPoint                 mLabelPos;              // position of the label
    wxSize                  mCurrentSize;           // current size of the button
    bool                    mCalcBusy;              // avoid recursive calls to CalcLayout
    int                     mGroupIndex;            // ID for a group of buttons
    bool                    mAllowAllUp;            // all in a group can be UP?
    bool                    mMouseDown;             // mouse button is down?
    bool                    mMouseOver;             // mouse of over the button?
    bool                    mButtonDown;            // button is clicked DOWN?
    bool                    mButtonFocused;         // does the button have input focus?
    wxWindow                *mParent;               // keep track of parent
    wxWindow                *mTopParent;            // and the top-most parent
    long                    mUserData;              // anything you want

private:
    DECLARE_DYNAMIC_CLASS(wxSpeedButton)
    DECLARE_EVENT_TABLE()
};

#endif  // _WX_SPEEDBUTTON_H_
