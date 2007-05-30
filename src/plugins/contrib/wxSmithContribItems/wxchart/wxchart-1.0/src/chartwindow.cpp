/////////////////////////////////////////////////////////////////////////////
// Name:        chartwindow.cpp
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

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/chartwindow.h"

IMPLEMENT_DYNAMIC_CLASS(wxChartWindow, wxWindow)

BEGIN_EVENT_TABLE(wxChartWindow, wxWindow)
  EVT_PAINT(        wxChartWindow::OnPaint)
  EVT_LEFT_DOWN(    wxChartWindow::OnMouse)
  EVT_LEFT_DCLICK(  wxChartWindow::OnMouse)
END_EVENT_TABLE()


//+++-S-cf-------------------------------------------------------------------
//	NAME:		ctor
//	DESC:
//	PARAMETERS:	wxWindow* parent
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxChartWindow::wxChartWindow(
	wxScrolledWindow *parent,
    bool usegrid
):  wxWindow(parent, -1, wxDefaultPosition, wxDefaultSize/*, wxSIMPLE_BORDER*/),
	m_WinParent(parent),
    m_UseGrid(usegrid)
{
	SetBackgroundColour( *wxWHITE );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Draw()
//	DESC:		Draw chart points
//	PARAMETERS:	CHART_HPAINT hp,
//				int x,
//				int y
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartWindow::Draw(
	CHART_HPAINT hp,
	int x,
	int y
)
{
	//-----------------------------------------------------------------------
	// Get window information
	//-----------------------------------------------------------------------
	CHART_RECT r;
    r.x = x; r.y = y;
	r.xscroll = 0; r.yscroll = 0;
	GetClientSize( &r.w, &r.h );

    //-----------------------------------------------------------------------
    // Set Background
    //-----------------------------------------------------------------------

#if 0
    hp->SetBrush( wxBrush(0xecf1f1, wxSOLID) ); //fcfdd8, *wxLIGHT_GREY_BRUSH
    hp->SetPen( *wxTRANSPARENT_PEN );
    hp->DrawRectangle(
        r.x + 2,
        r.y + 5,
        static_cast<int>(GetVirtualWidth()),
        r.h - 5
    );
#endif

    //-----------------------------------------------------------------------
    // Draw horizontal lines
    //-----------------------------------------------------------------------
    if ( m_UseGrid )
        DrawHLines( hp, &r );

	//-----------------------------------------------------------------------
	// Draw all charts
	//-----------------------------------------------------------------------
	m_Chart.Draw( hp, &r );
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       DrawHLines()
//  DESC:       Draw horizontal lines
//  PARAMETERS: CHART_HPAINT hp,
//              CHART_HRECT hr
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void wxChartWindow::DrawHLines(
    CHART_HPAINT hp,
    CHART_HRECT hr
)
{
    if ( GetVirtualMaxY() > 0 )
    {
        double range = GetVirtualMaxY();
        double start = 0;
        double end = range;

        int int_log_range = (int)floor( log10( range ) );
        double step = 1.0;
        if (int_log_range > 0)
        {
            for (int i = 0; i < int_log_range; i++)
                step *= 10;
        }
        if (int_log_range < 0)
        {
            for (int i = 0; i < -int_log_range; i++)
                step /= 10;
        }
        double lower = ceil(start / step) * step;
        double upper = floor(end / step) * step;

        // if too few values, shrink size
        if ((range/step) < 4)
        {
            step /= 2;
            if (lower-step > start) lower -= step;
            if (upper+step < end) upper += step;
        }

        // if still too few, again
        if ((range/step) < 4)
        {
            step /= 2;
            if (lower-step > start) lower -= step;
            if (upper+step < end) upper += step;
        }

        ChartSizes sizes = GetSizes();

        hp->SetPen( *wxBLACK_DASHED_PEN );

        double current = lower;
        while (current < upper+(step/2))
        {
            int y = (int)( (GetVirtualMaxY()-current) /
                range * ((double)hr->h - sizes.s_height)) - 1;
            if ((y > 10) && (y < hr->h - 7 - sizes.s_height))
            {
                hp->DrawLine( hr->x,
                    y + sizes.s_height + hr->y,
                    hr->x + static_cast<int>(GetVirtualWidth()),
                    y + sizes.s_height + hr->y );
            }

            current += step;
        }
    }
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       GetVirtualWidth()
//  DESC:
//  PARAMETERS: NOne
//  RETURN:     None
//----------------------------------------------------------------------E-+++
ChartValue wxChartWindow::GetVirtualWidth() const
{
    int iNodes = static_cast<int>(ceil( GetVirtualMaxX() ));
    ChartSizes sizes = GetSizes();

    ChartValue x = 0;

    for ( int iNode = 0; iNode <= iNodes; ++ iNode )
    {
        x +=  GetZoom() * ( sizes.wbar * sizes.nbar +
                            sizes.wbar3d * sizes.nbar3d +
                            sizes.gap );
    }

    return ( x );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		OnPaint()
//	DESC:
//	PARAMETERS:	wxPaintEvent &event
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartWindow::OnPaint(
	wxPaintEvent &WXUNUSED(event)
)
{
    wxPaintDC dc( this );
    m_WinParent->PrepareDC( dc );

	//-----------------------------------------------------------------------
	// Draw all charts window
	//-----------------------------------------------------------------------
	Draw( &dc );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		OnMouse()
//	DESC:
//	PARAMETERS:	wxMouseEvent &event
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartWindow::OnMouse(
	wxMouseEvent &WXUNUSED(event)
)
{
}
