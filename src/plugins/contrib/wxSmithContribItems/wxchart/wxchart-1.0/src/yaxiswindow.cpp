/////////////////////////////////////////////////////////////////////////////
// Name:        yaxiswindow.cpp
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

#include "wx/yaxiswindow.h"

IMPLEMENT_DYNAMIC_CLASS(wxYAxisWindow, wxWindow)

BEGIN_EVENT_TABLE(wxYAxisWindow, wxWindow)
  EVT_PAINT(        wxYAxisWindow::OnPaint)
  EVT_LEFT_DOWN(    wxYAxisWindow::OnMouse)
  EVT_LEFT_DCLICK(  wxYAxisWindow::OnMouse)
END_EVENT_TABLE()


//+++-S-cf-------------------------------------------------------------------
//	NAME:		ctor
//	DESC:
//	PARAMETERS:	wxWindow* parent,
//				ChartValue max,
//				ChartValue min,
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxYAxisWindow::wxYAxisWindow(
	 wxScrolledWindow *parent,
	 ChartValue max,
	 ChartValue min
):  wxWindow(parent, -1, wxDefaultPosition,
			 wxSize(YAXIS_WIDTH, YAXIS_HEIGHT)/*, wxSIMPLE_BORDER*/),
	m_WinParent(parent),
	m_YAxis(max, min)
{
	SetBackgroundColour( *wxWHITE );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetVirtualMax
//	DESC:
//	PARAMETERS:	ChartValue v
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxYAxisWindow::SetVirtualMax(
	ChartValue v
)
{
	m_YAxis.SetVirtualMax( v );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetVirtualMin
//	DESC:
//	PARAMETERS:	ChartValue v
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxYAxisWindow::SetVirtualMin(
	ChartValue v
)
{
	m_YAxis.SetVirtualMin( v );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetVirtualMax
//	DESC:
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxYAxisWindow::GetVirtualMax() const
{
	return ( m_YAxis.GetVirtualMax() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetVirtualMin
//	DESC:
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxYAxisWindow::GetVirtualMin() const
{
	return ( m_YAxis.GetVirtualMin() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetZoom
//	DESC:		Set zoom
//	PARAMETERS:	double
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxYAxisWindow::SetZoom(
	double z
)
{
	m_YAxis.SetZoom( z );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetZoom
//	DESC:		Get zoom
//	PARAMETERS:	double
//	RETURN:		None
//----------------------------------------------------------------------E-+++
double wxYAxisWindow::GetZoom()
{
	return ( m_YAxis.GetZoom() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetSizes
//	DESC:		Set sizes for drawing
//	PARAMETERS:	ChartSizes sizes
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxYAxisWindow::SetSizes(
	ChartSizes sizes
)
{
	m_YAxis.SetSizes( sizes );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetSizes
//	DESC:		Set sizes for drawing
//	PARAMETERS:	ChartSizes sizes
//	RETURN:		None
//----------------------------------------------------------------------E-+++
const ChartSizes& wxYAxisWindow::GetSizes() const
{
	return ( m_YAxis.GetSizes() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Draw()
//	DESC:		Draw y-axis window
//	PARAMETERS:	CHART_HPAINT hp,
//				int x,
//				int y
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxYAxisWindow::Draw(
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
	// Draw y-axis
	//-----------------------------------------------------------------------
	m_YAxis.Draw( hp, &r );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		OnPaint()
//	DESC:
//	PARAMETERS:	wxPaintEvent &event
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxYAxisWindow::OnPaint(
	wxPaintEvent &WXUNUSED(event)
)
{
    wxPaintDC dc( this );

	//-----------------------------------------------------------------------
	// Draw y-axis window
	//-----------------------------------------------------------------------
	Draw( &dc );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		OnMouse()
//	DESC:
//	PARAMETERS:	wxMouseEvent &event
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxYAxisWindow::OnMouse(
	wxMouseEvent &WXUNUSED(event)
)
{
}
