
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

#include "wxSpeedButton.h"


// ==========================================================================
// wxSpeedButton
// ==========================================================================
IMPLEMENT_DYNAMIC_CLASS( wxSpeedButton, wxControl )

BEGIN_EVENT_TABLE(wxSpeedButton,wxControl)
    EVT_MOUSE_EVENTS ( wxSpeedButton::OnMouseEvents )
    EVT_PAINT        ( wxSpeedButton::OnPaint )
    EVT_SIZE         ( wxSpeedButton::OnSize )
    EVT_SET_FOCUS    ( wxSpeedButton::OnSetFocus )
    EVT_KILL_FOCUS   ( wxSpeedButton::OnKillFocus )
    EVT_CHAR         ( wxSpeedButton::OnKey )
END_EVENT_TABLE()


//------------------------------------------------------------------------------
// book-keeping data for a series of speed-buttons

static  int             sbgCount    = 0;        // number of speed-buttons created
static  wxArrayPtrVoid  sbgArray;               // list of defined buttones

//------------------------------------------------------------------------------

wxSpeedButton::~wxSpeedButton() {
int         n;

// release the mouse

//    if (HasCapture()) ReleaseMouse();

// erase entry in master array

    n = sbgArray.Index((void *) this);
    if (n != wxNOT_FOUND) sbgArray.RemoveAt(n);

}

//------------------------------------------------------------------------------
// the basic contructor

wxSpeedButton::wxSpeedButton( wxWindow     *inParent,           // parent window
                            wxWindowID      inID,               // id of this button
                            const wxString &inLabel,            // button text
                            const wxBitmap &inGlyph,            // bitmaps displayed on button
                            int             inGlyphCount,       // number of images in inGlyph
                            int             inMargin,           // area around image and tex
                            int             inGroupIndex,       // ident of a group of buttons
                            bool            inAllowAllUp,       // allow all buttons up
                            const wxPoint  &inPos,              // button position
                            const wxSize   &inSize,             // button size
                            long            inStyle,            // border styles
                            const wxValidator &inVal,           // validator
                            const wxString &inName) {           // name of button

    Create( inParent, inID,
            inLabel,
            inGlyph, inGlyphCount, inMargin,
            inGroupIndex, inAllowAllUp,
            inPos, inSize, inStyle, inVal, inName);
}

//------------------------------------------------------------------------------
// a constructor that creates a bitmap from a filename

wxSpeedButton::wxSpeedButton( wxWindow     *inParent,           // parent window
                            wxWindowID      inID,               // id of this button
                            const wxString &inLabel,            // button text
                            const wxString &inGlyphFile,        // bitmaps displayed on button
                            int             inGlyphCount,       // number of images in inGlyph
                            int             inMargin,           // area around image and tex
                            int             inGroupIndex,       // ident of a group of buttons
                            bool            inAllowAllUp,       // allow all buttons up
                            const wxPoint  &inPos,              // button position
                            const wxSize   &inSize,             // button size
                            long            inStyle,            // border styles
                            const wxValidator &inVal,           // validator
                            const wxString &inName) {           // name of button
wxBitmap    bmp(inGlyphFile, wxBITMAP_TYPE_ANY );

    Create( inParent, inID,
            inLabel,
            bmp, inGlyphCount, inMargin,
            inGroupIndex, inAllowAllUp,
            inPos, inSize, inStyle, inVal, inName);
}



//------------------------------------------------------------------------------

bool wxSpeedButton::Create( wxWindow       *inParent,           // parent window
                            wxWindowID      inID,               // id of this button
                            const wxString &inLabel,            // button text
                            const wxBitmap &inGlyph,            // bitmaps displayed on button
                            int             inGlyphCount,       // number of images in inGlyph
                            int             inMargin,           // area around image and tex
                            int             inGroupIndex,       // ident of a group of buttons
                            bool            inAllowAllUp,       // allow all buttons up
                            const wxPoint  &inPos,              // button position
                            const wxSize   &inSize,             // button size
                            long            inStyle,            // border styles
                            const wxValidator &inVal,           // validator
                            const wxString &inName) {           // name of button

int         n;
wxString    name;
wxPoint     pos;
wxSize      size;
wxString    s;

// make sure we can load images

    wxInitAllImageHandlers();

// one more button

    sbgCount += 1;

// make a default name

    name = inName;
    name.Trim(true);
    name.Trim(false);
    if (name.Len() == 0) name.Printf(_T("SpeedButton-%d"), sbgCount);

// the position

    pos = inPos;
    if (pos.x < 0) pos.x = 0;
    if (pos.y < 0) pos.y = 0;

// the size - default size is 72 x 24

    size = inSize;
    size.SetDefaults(wxSize(72, 24));

// fix the alignment -- default to BU_LEFT
// clear border styles and make sure we clip children

    n = inStyle;
    n = n & (~ wxBORDER_MASK);
    n = n | wxBORDER_NONE;
    n = n | wxCLIP_CHILDREN;

    if (((n & wxBU_LEFT)   == 0) &&
        ((n & wxBU_TOP)    == 0) &&
        ((n & wxBU_RIGHT)  == 0) &&
        ((n & wxBU_BOTTOM) == 0))
            n = n | wxBU_LEFT;

// make the control, make sure we clip children

    if (! wxControl::Create(inParent, inID, pos, size, n, inVal, name)) return false;

// basic stuff for any control

    wxControl::SetLabel(inLabel);
    wxControl::SetBackgroundColour(inParent->GetBackgroundColour());
    wxControl::SetForegroundColour(inParent->GetForegroundColour());
    wxControl::SetFont(inParent->GetFont());

// extract bitmaps

    SplitGlyphs(inGlyph, inGlyphCount);

// the blank space around images and text

    mMargin = inMargin;
    if (mMargin < 0) mMargin = 0;

// ID for a group of buttons

    mGroupIndex = inGroupIndex;
    mAllowAllUp = inAllowAllUp;

// no button down yet

    mMouseDown     = false;
    mMouseOver     = false;
    mButtonDown    = false;
    mButtonFocused = false;

// the is a small chance that CalcLayout could be called recursively

    mCalcBusy = false;

// keep track of our parent, and the top-most parent

    mParent = GetParent();
    mTopParent = mParent;
    while ((mTopParent != NULL) && (! mTopParent->IsKindOf(CLASSINFO(wxTopLevelWindow))))
        mTopParent = mTopParent->GetParent();

// no user data yet

    mUserData = 0;

// add this button to the master list

    sbgArray.Add((void *) this);

// draw it

    Refresh(false);

// done

    return true;
}

//------------------------------------------------------------------------------
// given an input bitmap, make 3 glyphs from it:
// the input bitmap may be empty (no glyphs),
// with 1 image, make UP glyph, DOWN glyph, DISABLED glyph
// with 2 images, the first image is UP and DOWN glyphs, the 2nd image is DISABLED glyph
// with 3 images, each image is a glyph

void wxSpeedButton::SplitGlyphs(const wxBitmap &inBitmap, int inCount) {
int         n;
int         bw,bh;
int         sw,sh;
wxRect      rr;
wxImage     img;
wxBitmap    *bmp;

// no images yet

    mGlyphUp       = wxNullBitmap;
    mGlyphDown     = wxNullBitmap;
    mGlyphDisabled = wxNullBitmap;

// if no bitmap, then we are done

    if (! inBitmap.Ok()) return;

// size of the bitmap

    bw = inBitmap.GetWidth();
    bh = inBitmap.GetHeight();
    if ((bw <= 0) || (bh <= 0)) return;

// determine the number of images in the source bitmap
// if inCount > 0, then that is the count specified by the user
// else, count number of square sub-images

    if      (inCount > 0) n = inCount;
    else if (bw >= bh)    n = (int) bw / bh;
    else                  n = (int) bh / bw;

// extract sub-images, either vertically or horizontally

    if (n == 1) {
        mGlyphUp   = inBitmap;
        mGlyphDown = inBitmap;

        img = inBitmap.ConvertToImage();
        img = img.ConvertToGreyscale();
        bmp = new wxBitmap(img);
        mGlyphDisabled = *bmp;
    }
    else if ((n == 2) && (bw >= bh)) {
        sw = (int) bw / n;
        sh = bh;

        rr.SetX(0);
        rr.SetY(0);
        rr.SetWidth(sw);
        rr.SetHeight(sh);
        mGlyphUp = inBitmap.GetSubBitmap(rr);
        mGlyphDown = inBitmap.GetSubBitmap(rr);
        rr.SetX(sw);
        mGlyphDisabled = inBitmap.GetSubBitmap(rr);
    }
    else if ((n == 2) && (bh > bw)) {
        sw = bw;
        sh = (int) bh / n;

        rr.SetX(0);
        rr.SetY(0);
        rr.SetWidth(sw);
        rr.SetHeight(sh);
        mGlyphUp = inBitmap.GetSubBitmap(rr);
        mGlyphDown = inBitmap.GetSubBitmap(rr);
        rr.SetY(sh);
        mGlyphDisabled = inBitmap.GetSubBitmap(rr);
    }
    else if ((n >= 3) && (bw >= bh)) {
        sw = (int) bw / n;
        sh = bh;

        rr.SetX(0);
        rr.SetY(0);
        rr.SetWidth(sw);
        rr.SetHeight(sh);
        mGlyphUp = inBitmap.GetSubBitmap(rr);
        rr.SetX(sw);
        mGlyphDown = inBitmap.GetSubBitmap(rr);
        rr.SetX(sw+sw);
        mGlyphDisabled = inBitmap.GetSubBitmap(rr);
    }
    else { // (n >= 3) && (bh > bw)
        sw = bw;
        sh = (int) bh / n;

        rr.SetX(0);
        rr.SetY(0);
        rr.SetWidth(sw);
        rr.SetHeight(sh);
        mGlyphUp = inBitmap.GetSubBitmap(rr);
        rr.SetY(sh);
        mGlyphDown = inBitmap.GetSubBitmap(rr);
        rr.SetY(sh+sh);;
        mGlyphDisabled = inBitmap.GetSubBitmap(rr);
    };

// make them all transparent

    MakeTransparent(mGlyphUp);
    MakeTransparent(mGlyphDown);
    MakeTransparent(mGlyphDisabled);
}

//------------------------------------------------------------------------------
// make sure a bitmap has a transparency mask

void wxSpeedButton::MakeTransparent(wxBitmap &inBitmap) {
int         h;
int         r,g,b;
wxImage     img;
wxBitmap    *bmp;

// not a good image?

    if (! inBitmap.IsOk()) return;

// already have a mask?

    img = inBitmap.ConvertToImage();
    if (img.HasMask()) return;

// get the colors of the lower-left corner of the image

    h = img.GetHeight();
    r = img.GetRed(0, h-1);
    b = img.GetBlue(0, h-1);
    g = img.GetGreen(0, h-1);

// make a mask from those colors

    img.SetMaskColour(r, g, b);

// store it back in the bitmap

    bmp = new wxBitmap(img);
    inBitmap = *bmp;
}

//------------------------------------------------------------------------------

void wxSpeedButton::GetGlyphSize(wxBitmap &inGlyph, int &outWidth, int &outHeight) {

    if (inGlyph.Ok()) {
        outWidth  = inGlyph.GetWidth();
        outHeight = inGlyph.GetHeight();
    }
    else {
        outWidth  = 0;
        outHeight = 0;
    };

}

//------------------------------------------------------------------------------
// calc best size for button,
// fills in mGlyphSize, mLabelSize, mBestSize

wxSize wxSpeedButton::DoGetBestSize(void) {
int         i,n;
int         w,h;
int         bn;
int         bw, bh;
int         gw, gh;
int         lw, lh;

// max size of the bitmaps

    bw = 0;
    bh = 0;
    GetGlyphSize(mGlyphUp, gw, gh);
        if (gw > bw) bw = gw;
        if (gh > bh) bh = gh;
    GetGlyphSize(mGlyphDown, gw, gh);
        if (gw > bw) bw = gw;
        if (gh > bh) bh = gh;
    GetGlyphSize(mGlyphDisabled, gw, gh);
        if (gw > bw) bw = gw;
        if (gh > bh) bh = gh;

    mGlyphSize.Set(bw, bh);

// max size of the t  n = inStyle | wxCLIP_CHILDREN;ext label

    lw = 0;
    lh = 0;
    if (! GetLabel().IsEmpty()) GetTextExtent(GetLabel(), &lw, &lh);

    mLabelSize.Set(lw, lh);

// border size is a magiv number, imperically determined using the
// wxNativeRenderer to draw the button

    bn = 2;

// best size depends upon the alignment

    i = GetWindowStyleFlag();

    if (((i & wxBU_LEFT) != 0) || ((i & wxBU_RIGHT) != 0)) {
        w = bn + mMargin + bw + mMargin + lw + mMargin + bn;
        n = (bh > lh) ? bh : lh;
        h = bn + mMargin + n + mMargin + bn;
    }
    else if (((i & wxBU_TOP) != 0) || ((i & wxBU_BOTTOM) != 0)) {
        n = (bw > lw) ? bw : lw;
        w = bn + mMargin + n + mMargin + bn;
        h = bn + mMargin + bh + mMargin + lh + mMargin + bn;
    }
    else { // assume BU_LEFT
        w = bn + mMargin + bw + mMargin + lw + mMargin + bn;
        n = (bh > lh) ? bh : lh;
        h = bn + mMargin + n + mMargin + bn;
    };

    mBestSize.Set(w, h);

// done

    return mBestSize;
}

//------------------------------------------------------------------------------
// calculate the position of the bitmap and the label
// fills in mGlyphPos, mLabelPos, mCurrentSize

void wxSpeedButton::CalcLayout(bool inRefresh) {
int         i;
int         bn;
int         w, h;
bool        gz, lz;
int         gx, gy;
int         lx, ly;

// no recursive calls

    if (mCalcBusy) return;
    mCalcBusy = true;

// current size of the button

    GetSize(&w,&h);
    mCurrentSize.Set(w, h);

// get sizes of bitmaps and labels, alignment and margin

    DoGetBestSize();

// glyph or label not used?

    gz = (mGlyphSize.GetWidth() == 0) || (mGlyphSize.GetHeight() == 0);
    lz = (mLabelSize.GetWidth() == 0) || (mLabelSize.GetHeight() == 0);

// border size is a magiv number, imperically determined using the
// wxNativeRenderer to draw the button

    bn = 2;

// position depends on alignment

    i = GetWindowStyleFlag();

    if (gz && lz) {                 // no glyph, no label
        gx = 0;
        gy = 0;
        lx = 0;
        ly = 0;
    }
    else if (gz) {                  // no glyph, only label
        gx = 0;
        gy = 0;
        lx = (mCurrentSize.GetWidth()  - mLabelSize.GetWidth())  / 2;
        ly = (mCurrentSize.GetHeight() - mLabelSize.GetHeight()) / 2;
    }
    else if (lz) {                  // no label, glyph only
        gx = (mCurrentSize.GetWidth()  - mGlyphSize.GetWidth())  / 2;
        gy = (mCurrentSize.GetHeight() - mGlyphSize.GetHeight()) / 2;
        lx = 0;
        ly = 0;
    }
    else if ((i & wxBU_LEFT) != 0) {
        gx = bn + mMargin;
        lx = gx + mGlyphSize.GetWidth() + mMargin;
        gy = (mCurrentSize.GetHeight() - mGlyphSize.GetHeight()) / 2;
        ly = (mCurrentSize.GetHeight() - mLabelSize.GetHeight()) / 2;
    }
    else if ((i & wxBU_RIGHT) != 0) {
        gx = mCurrentSize.GetWidth() - (mGlyphSize.GetWidth() + mMargin + bn);
        lx = gx - (mLabelSize.GetWidth() + mMargin);
        gy = (mCurrentSize.GetHeight() - mGlyphSize.GetHeight()) / 2;
        ly = (mCurrentSize.GetHeight() - mLabelSize.GetHeight()) / 2;
    }
    else if ((i & wxBU_TOP) != 0) {
        gx = (mCurrentSize.GetWidth()  - mGlyphSize.GetWidth())  / 2;
        lx = (mCurrentSize.GetWidth()  - mLabelSize.GetWidth())  / 2;
        gy = bn + mMargin;
        ly = gy + mMargin + mGlyphSize.GetHeight();
    }
    else if ((i & wxBU_BOTTOM) != 0) {
        gx = (mCurrentSize.GetWidth()  - mGlyphSize.GetWidth())  / 2;
        lx = (mCurrentSize.GetWidth()  - mLabelSize.GetWidth())  / 2;
        gy = mCurrentSize.GetHeight() - (bn + mMargin + mGlyphSize.GetHeight());
        ly = gy - (mLabelSize.GetHeight() + mMargin);
    }
    else { // unknown
        gx = 0;
        gy = 0;
        lx = 0;
        ly = 0;
    };

// save the positions

    mGlyphPos.x = gx;
    mGlyphPos.y = gy;
    mLabelPos.x = lx;
    mLabelPos.y = ly;

// redraw button?

    if (inRefresh) Refresh(false);

// allow another call

    mCalcBusy = false;
}





//------------------------------------------------------------------------------

void wxSpeedButton::SetLabel(wxString &inLabel) {
    wxControl::SetLabel(inLabel);
    Refresh(false);
}

//------------------------------------------------------------------------------

void    wxSpeedButton::SetGlyphUp(wxBitmap &inBitmap) {

    mGlyphUp = inBitmap;
    Refresh(false);
}

wxBitmap    &wxSpeedButton::GetGlyphUp(void) {

    return mGlyphUp;
}

//------------------------------------------------------------------------------

void    wxSpeedButton::SetGlyphDown(wxBitmap &inBitmap) {

    mGlyphDown = inBitmap;
    Refresh(false);
}

wxBitmap    &wxSpeedButton::GetGlyphDown(void) {

    return mGlyphDown;
}

//------------------------------------------------------------------------------

void    wxSpeedButton::SetGlyphDisabled(wxBitmap &inBitmap) {

    mGlyphDisabled = inBitmap;
    Refresh(false);
}

wxBitmap    &wxSpeedButton::GetGlyphDisabled(void) {

    return mGlyphDisabled;
}




//------------------------------------------------------------------------------

void    wxSpeedButton::SetAlign(int inAlign) {
int     i,n;

// make sure a valid alignment

    n = inAlign;
    if ((n != wxBU_LEFT) && (n != wxBU_TOP) &&(n != wxBU_RIGHT) &&(n != wxBU_BOTTOM)) n = wxBU_LEFT;

// get current style

    i = GetWindowStyleFlag();

// remove old alignment, and border info

    i = i & (~ wxBORDER_MASK);
    i = i & (~ wxBU_ALIGN_MASK);

// put in alignment and no-border

    i = i | wxBORDER_NONE;
    i = i | n;
    i = i | wxCLIP_CHILDREN;

// save new style

    SetWindowStyleFlag(i);
    Refresh(false);
}

int     wxSpeedButton::GetAlign(void) {
int     i;

    i = GetWindowStyleFlag();
    i = i & wxBU_ALIGN_MASK;

    return i;
}

//------------------------------------------------------------------------------

void    wxSpeedButton::SetMargin(int inMargin) {

    mMargin = inMargin;
    Refresh(false);
}

int     wxSpeedButton::GetMargin(void) {

    return mMargin;
}

//------------------------------------------------------------------------------

void    wxSpeedButton::SetGroupIndex(bool inIndex) {

    mGroupIndex = inIndex;
    Refresh(false);
}

int     wxSpeedButton::GetGroupIndex(void) {

    return mGroupIndex;
}



//------------------------------------------------------------------------------

void    wxSpeedButton::SetDown(bool inDown) {

// a simple button never goes DOWN, it always reports as UP

    if (mGroupIndex == 0) {
        mButtonDown = false;
    }

// a sime toggle button goes UP and DOWN

    else if (mGroupIndex == -1) {
        mButtonDown = inDown;
    }

// a group toggle button, grouped by immediate parent

    else if (mGroupIndex == -2) {
        SetAllUp(this);
        if ((! inDown) && (mAllowAllUp)) mButtonDown = false;
        else                             mButtonDown = true;
    }

// all else is a group toggle button, grouped by index and top-level parent

    else {
        SetAllUp(this);
        if ((! inDown) && (mAllowAllUp)) mButtonDown = false;
        else                             mButtonDown = true;
    };

// done

    Refresh(false);
}

bool    wxSpeedButton::GetDown(void) {

    return mButtonDown;
}

void    wxSpeedButton::SetAllUp(wxSpeedButton *inButton) {
int             i,n;
wxSpeedButton   *b;

// no button?

    if (inButton == NULL) return;

// simple button

    if (inButton->mGroupIndex == 0) {
        inButton->mButtonDown = false;
        inButton->Refresh(false);
    }

// toggle button

    else if (inButton->mGroupIndex == -1) {
        inButton->mButtonDown = false;
        inButton->Refresh(false);
    }

// group button, grouped by immediate parent

    else if (inButton->mGroupIndex == -2) {
        n = sbgArray.GetCount();
        for(i=0; i<n; i++) {
            b = (wxSpeedButton *) sbgArray.Item(i);
            if (b->mParent == inButton->mParent) {
                b->mButtonDown = false;
                b->Refresh(false);
            };
        };
    }

// all else is a group toggle button, grouped by index and top-level parent

    else {
        n = sbgArray.GetCount();
        for(i=0; i<n; i++) {
            b = (wxSpeedButton *) sbgArray.Item(i);
            if ((b->mGroupIndex == inButton->mGroupIndex) && (b->mTopParent == inButton->mTopParent)) {
                b->mButtonDown = false;
                b->Refresh(false);
            };
        };
    };


}

void    wxSpeedButton::SetValue(bool inDown) {

    SetDown(inDown);
}

bool    wxSpeedButton::GetValue(void) {

    return GetDown();
}

//------------------------------------------------------------------------------

void    wxSpeedButton::SetAllowAllUp(bool inAllUp) {

    mAllowAllUp = inAllUp;
    Refresh(false);
}

bool    wxSpeedButton::GetAllowAllUp(void) {

    return mAllowAllUp;
}

//------------------------------------------------------------------------------

void    wxSpeedButton::SetUserData(long inData) {

    mUserData = inData;
}

long    wxSpeedButton::GetUserData(void) {

    return mUserData;
}






//------------------------------------------------------------------------------

// sequence of events in GTK is up, dclick, up.

void wxSpeedButton::OnMouseEvents(wxMouseEvent& event) {
wxWindow    *win;

// our underlying window

    win = (wxWindow *) this;

// any mouse button down

    if (event.LeftDown() || event.RightDown()) {
       if (!HasCapture()) CaptureMouse();
        mMouseDown = true;
        mButtonFocused = true;
        win->SetFocus();
        Redraw();
    }

// any mouse button up
// this is where we send a click event

    else if (event.LeftUp() || event.RightUp()) {
        if (HasCapture()) ReleaseMouse();
        mMouseDown = false;
        mButtonFocused = true;
        win->SetFocus();
        SetDown(! mButtonDown);
        SendEvent(event.LeftUp());
        Redraw();
    }

// mouse over

    else if (event.Entering()) {
        mMouseOver = true;
        Redraw();
    }
    else if (event.Leaving()) {
        mMouseOver = false;
        Redraw();
    };
}

//------------------------------------------------------------------------------
// cake a mouse click

void wxSpeedButton::DoClick(bool inLeft) {

    SetDown(! mButtonDown);
    SendEvent(inLeft);
    Refresh(false);
}


//------------------------------------------------------------------------------
// send a command click-event for the left or right buttons

void wxSpeedButton::SendEvent(bool inLeft) {
int                 n;
long                now;
wxCommandEvent      event;

// set command type and window ID of caller

    if (inLeft) event.SetEventType(wxEVT_COMMAND_LEFT_CLICK);
    else        event.SetEventType(wxEVT_COMMAND_RIGHT_CLICK);

    event.SetId(GetId());

// save pointer to this object

    event.SetEventObject(this);

// if LEFT, the int is 1, else 0

    n = inLeft ? 1 : 0;
    event.SetInt(n);

// and the time

    now = (long) wxDateTime::GetTimeNow();
    event.SetTimestamp(now);

// send it

    GetEventHandler()->ProcessEvent(event);
}


//------------------------------------------------------------------------------

void wxSpeedButton::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    Paint(dc);
    event.Skip();
}

//------------------------------------------------------------------------------

void wxSpeedButton::Redraw()
{
    wxClientDC dc(this);
    Paint(dc);
}

//------------------------------------------------------------------------------
// drawing up, down, flat
// optional hot, transparent

void    wxSpeedButton::Paint( wxDC &dc ) {
int         n;
int         w,h;
wxColour    cf;                     // foreground color
wxColour    cb;                     // background color
wxColour    cg;                     // gray text
wxColour    cy;                     // yellow
wxBrush     bb;                     // background brush
wxPen       pp;                     // line-drawing pen
wxBitmap    bmp;
wxString    s;
wxRect      rr;

// get size and layout

    if (! mCalcBusy) CalcLayout(false);

    w = mCurrentSize.GetWidth();
    h = mCurrentSize.GetHeight();

// get colors

    cf = GetForegroundColour();
    cb = GetBackgroundColour();
    cg = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT );
    cy = wxTheColourDatabase->Find(_("YELLOW"));

// use wxWidgets to draw the basic button

    rr.SetX(0);
    rr.SetY(0);
    rr.SetWidth(w);
    rr.SetHeight(h);

    n = 0;
    if (mMouseDown || mButtonDown) n = n | wxCONTROL_PRESSED;
    if (mButtonFocused) n = n | wxCONTROL_ISDEFAULT;

    wxRendererNative::Get().DrawPushButton(this, dc, rr, n);

// track mouse movements with mMouseOver using a dotted yellow line

    pp = *wxBLACK_PEN;
    pp.SetColour(cy);
//  pp.SetStyle(wxDOT);
    dc.SetPen(pp);
    if (mMouseOver) {
        n = 2;
        dc.DrawLine(  n,   n, w-n,   n);
        dc.DrawLine(w-n,   n, w-n, h-n);
        dc.DrawLine(w-n, h-n,   n, h-n);
        dc.DrawLine(  n, h-n,   n,   n);
    };

// select the bitmap to draw

    if      (! IsEnabled())             bmp = mGlyphDisabled;
    else if (mMouseDown || mButtonDown) bmp = mGlyphDown;
    else                                bmp = mGlyphUp;

    if (bmp.IsOk()) dc.DrawBitmap(bmp, mGlyphPos.x, mGlyphPos.y, true );

// the text label

    s = GetLabelText();
    if (! s.IsEmpty()) {
        dc.SetFont(GetFont());
        dc.SetBackgroundMode(wxTRANSPARENT);
        if (! IsEnabled()) dc.SetTextForeground(cg);
        else               dc.SetTextForeground(cf);
        dc.DrawText(s, mLabelPos.x, mLabelPos.y);
    };
}

//------------------------------------------------------------------------------

void wxSpeedButton::OnSize( wxSizeEvent &event )
{
    Refresh(false);
    event.Skip();
}



//------------------------------------------------------------------------------

void wxSpeedButton::OnSetFocus(wxFocusEvent& event) {

    mButtonFocused = true;
    Redraw();
    event.Skip();
}

//------------------------------------------------------------------------------

void wxSpeedButton::OnKillFocus(wxFocusEvent& event) {

    mButtonFocused = false;
    Redraw();
    event.Skip();
}

//------------------------------------------------------------------------------

void wxSpeedButton::OnKey(wxKeyEvent& event) {
int         n;
wxString    s;

    n = event.GetKeyCode();
    if ((n == '\n') || ( n == '\r') || (n == ' ')) {
        mButtonFocused = true;
        SetDown(! mButtonDown);
        SendEvent(true);
        Redraw();
//Refresh(false);
    };

    event.Skip();
}








