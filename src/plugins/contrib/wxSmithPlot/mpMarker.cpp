
#include "mpMarker.h"


//-----------------------------------------------------------------------------
// mpText - provided by Val Greene
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(mpMarker, mpLayer)


//-----------------------------------------------------------------------------

/** @param name text to be displayed
@param offsetx x position in percentage (0-100)
@param offsetx y position in percentage (0-100)
*/
mpMarker::mpMarker( wxString name, double atX, double atY ) {
    SetName(name);
    mX = atX;
    mY = atY;
}

//-----------------------------------------------------------------------------

void mpMarker::Plot(wxDC & dc, mpWindow & w) {
wxCoord     cx, cy, tw, th;
wxColour    cc;
wxString    ss;

// setup

    dc.SetPen(m_pen);
    dc.SetFont(m_font);

// part of setup is setting the text color

    cc = m_pen.GetColour();
    dc.SetTextForeground(cc);

// what to draw

    ss = GetName();

// where to draw

    dc.GetTextExtent(ss, &tw, &th);
    cx = (wxCoord) ((mX - w.GetPosX()) * w.GetScaleX()) - (tw / 2);
    cy = (wxCoord) ((w.GetPosY() - mY) * w.GetScaleY()) - (th / 2);

// do it

    dc.DrawText( ss, cx, cy);
}
