/////////////////////////////////////////////////////////////////////////////
// Name:        label.cpp
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

// wx
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "label.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wx/chartcolors.h"
#include "wx/label.h"

//----------------------------------------------------------------------------
// Consts
//----------------------------------------------------------------------------
const ChartColor LBL_BACKGROUND_COL = wxCHART_LYELOW;
const ChartColor LBL_LINE_COL       = wxCHART_LCYAN;
const int LBL_GAP = 25; // pixels

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Draw()
//	DESC:		Draw Label Area
//	PARAMETERS:	wxDC* dc,
//				int x,
//				int y,
//				ChartString lbl,
//				POSITION pos = UP
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxLabel::Draw(
    CHART_HPAINT hp,
	int x,
	int y,
    ChartColor c,
	wxString& lbl,
	LABEL_POSITION pos
)
{
	//-----------------------------------------------------------------------
	// Get actual configuration
	//-----------------------------------------------------------------------
    wxFont oldFont = hp->GetFont();
    wxBrush oldBrush = hp->GetBrush();
    wxPen oldPen = hp->GetPen();

	//-----------------------------------------------------------------------
	// Set new values
	//-----------------------------------------------------------------------
	wxFont font( 8, wxROMAN, wxNORMAL, wxNORMAL );
    hp->SetFont( font );
    hp->SetBrush( wxBrush(c, wxSOLID) );
    hp->SetPen( wxPen(LBL_LINE_COL, 1, wxSOLID) );

	//-----------------------------------------------------------------------
	// Get the size of the label for the specify font
	//-----------------------------------------------------------------------
	int w, h;
    hp->GetTextExtent( lbl, &w, &h );

	//-----------------------------------------------------------------------
	// Add boarder
	//-----------------------------------------------------------------------
	w += 5;
	h += 5;

	//-----------------------------------------------------------------------
	// Get the left-top rectangle point
	//-----------------------------------------------------------------------
	int xr = x, yr = y;
	if ( pos & UP )
		yr -= LBL_GAP;
	if ( pos & DOWN )
		yr += LBL_GAP;
	if ( pos & LEFT )
		xr -= LBL_GAP;
	if ( pos & RIGHT )
		xr += LBL_GAP;

	//-----------------------------------------------------------------------
	// Draw all
	//-----------------------------------------------------------------------
    hp->DrawRectangle( xr, yr, w, h );
	if ( pos & DOWN )
        hp->DrawLine( x, y, xr + w/2, yr );
	else
        hp->DrawLine( x, y, xr + w/2, yr + h);
    hp->DrawText( lbl, xr+2, yr );


	//-----------------------------------------------------------------------
	// Set old configuration
	//-----------------------------------------------------------------------
    hp->SetFont( oldFont );
    hp->SetBrush( oldBrush );
    hp->SetPen( oldPen );
}
