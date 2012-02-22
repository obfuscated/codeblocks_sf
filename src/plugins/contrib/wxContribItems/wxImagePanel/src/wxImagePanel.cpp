/*
* This file is part of wxSmithContribItems plugin for Code::Blocks Studio
* Copyright (C) 2010 Ron Collins
*
* wxSmithContribItems is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmithContribItems is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmithContribItems. If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "wx/wxImagePanel.h"


IMPLEMENT_CLASS(wxImagePanel, wxPanel)


//-----------------------------------------------------------------------------
// create a new panel

wxImagePanel::wxImagePanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name):
            wxPanel(parent, id, pos, size, style, name) {

    mBitmap  = wxNullBitmap;
    mStretch = false;

    Connect(wxEVT_PAINT, (wxObjectEventFunction)&wxImagePanel::DoPaint, 0, this);

};


//-----------------------------------------------------------------------------

void    wxImagePanel::SetBitmap(wxBitmap &inBitmap) {

    mBitmap = inBitmap;
    Refresh();
}

wxBitmap wxImagePanel::GetBitmap(void) {

    return mBitmap;
}

//-----------------------------------------------------------------------------

void    wxImagePanel::SetStretch(bool inStretch) {

    mStretch = inStretch;
    Refresh();
}

bool    wxImagePanel::GetStretch(void) {

    return mStretch;
}



//-----------------------------------------------------------------------------

void    wxImagePanel::DoPaint(wxPaintEvent& event) {
int         ww, wh;
int         bw, bh;
double      sx, sy;
wxColour    cc;
wxBrush     brush;
wxPaintDC   dc(this);

// fill in the background color

/**
    cc = GetBackgroundColour();
    if (cc.IsOk()) {

    };
**/

    dc.Clear();

// no valid picture?

    if (! mBitmap.IsOk()) return;

// need to stretch it?

    if (mStretch) {
        GetSize(&ww, &wh);
        bw = mBitmap.GetWidth();
        bh = mBitmap.GetHeight();
        if ((bw > 0) && (bh > 0)) {
            sx = ((double) ww) / ((double) bw);
            sy = ((double) wh) / ((double) bh);
            dc.SetUserScale(sx, sy);
        };
    };

// draw it

    dc.DrawBitmap(mBitmap, 0, 0, false);

// reset the scale

    dc.SetUserScale(1.0, 1.0);
}



