/////////////////////////////////////////////////////////////////////////////
// Name:        xaxiswindow.cpp
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
#pragma implementation "xaxiswindow.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/xaxiswindow.h"
#include "wx/chartsizes.h"

IMPLEMENT_DYNAMIC_CLASS(wxXAxisWindow, wxWindow)

BEGIN_EVENT_TABLE(wxXAxisWindow, wxWindow)
  EVT_PAINT(        wxXAxisWindow::OnPaint)
  EVT_LEFT_DOWN(    wxXAxisWindow::OnMouse)
  EVT_LEFT_DCLICK(  wxXAxisWindow::OnMouse)
END_EVENT_TABLE()


//+++-S-cf-------------------------------------------------------------------
//	NAME:		ctor
//	DESC:		
//	PARAMETERS:	wxScrolledWindow* parent
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxXAxisWindow::wxXAxisWindow(
	 wxScrolledWindow *parent
):  wxWindow(parent, -1, wxDefaultPosition, 
			 wxSize(XAXIS_WIDTH, XAXIS_HEIGHT)/*, wxSIMPLE_BORDER*/),
	m_WinParent(parent)
{
	SetBackgroundColour( *wxWHITE );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetVirtualMax
//	DESC:		
//	PARAMETERS:	ChartValue v
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxXAxisWindow::SetVirtualMax(
	ChartValue v
)
{
	m_XAxis.SetVirtualMax( v );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetVirtualMin
//	DESC:		
//	PARAMETERS:	ChartValue v
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxXAxisWindow::SetVirtualMin(
	ChartValue v
)
{
	m_XAxis.SetVirtualMin( v );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetVirtualMax
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxXAxisWindow::GetVirtualMax() const
{
	return ( m_XAxis.GetVirtualMax() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetVirtualMin
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxXAxisWindow::GetVirtualMin() const
{
	return ( m_XAxis.GetVirtualMin() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetZoom
//	DESC:		Set zoom
//	PARAMETERS:	double
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxXAxisWindow::SetZoom(
	double z
)
{
	m_XAxis.SetZoom( z );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetZoom
//	DESC:		Get zoom
//	PARAMETERS:	double
//	RETURN:		None
//----------------------------------------------------------------------E-+++
double wxXAxisWindow::GetZoom()
{
	return ( m_XAxis.GetZoom() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetSizes
//	DESC:		Set sizes for drawing
//	PARAMETERS:	ChartSizes sizes
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxXAxisWindow::SetSizes(
	wxChartSizes *sizes
)
{
	m_XAxis.SetSizes( sizes );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetSizes
//	DESC:		Set sizes for drawing
//	PARAMETERS:	ChartSizes sizes
//	RETURN:		None
//----------------------------------------------------------------------E-+++
const wxChartSizes* wxXAxisWindow::GetSizes() const
{
	return ( m_XAxis.GetSizes() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Draw()
//	DESC:		Draw x-axis window
//	PARAMETERS:	CHART_HPAINT hp,
//				int x,
//				int y
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxXAxisWindow::Draw(
	CHART_HPAINT hp,
	int x,
	int y
)
{
	//-----------------------------------------------------------------------
	// Get window information
	//-----------------------------------------------------------------------
	CHART_RECT r;
    m_WinParent->GetViewStart( &r.xscroll, &r.yscroll );
    r.x = x; r.y = y;
	GetClientSize( &r.w, &r.h );

	//-----------------------------------------------------------------------
	// Draw x-axis 
	//-----------------------------------------------------------------------
	m_XAxis.Draw( hp, &r );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		OnPaint()
//	DESC:		
//	PARAMETERS:	wxPaintEvent &event
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxXAxisWindow::OnPaint(
	wxPaintEvent &WXUNUSED(event)
)
{
    wxPaintDC dc( this );

	//-----------------------------------------------------------------------
	// Draw x-axis window
	//-----------------------------------------------------------------------
	Draw( &dc );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		OnMouse()
//	DESC:		
//	PARAMETERS:	wxMouseEvent &event
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxXAxisWindow::OnMouse(
	wxMouseEvent &WXUNUSED(event)
)
{
}
