/////////////////////////////////////////////////////////////////////////////
// Name:        legendwindow.cpp
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
#pragma implementation "legendwindow.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/legendwindow.h"

IMPLEMENT_DYNAMIC_CLASS(wxLegendWindow, wxWindow)

BEGIN_EVENT_TABLE(wxLegendWindow, wxWindow)
  EVT_PAINT(        wxLegendWindow::OnPaint)
  EVT_LEFT_DOWN(    wxLegendWindow::OnMouse)
  EVT_LEFT_DCLICK(  wxLegendWindow::OnMouse)
  EVT_MOTION(       wxLegendWindow::OnMouseMove)
END_EVENT_TABLE()


//+++-S-cf-------------------------------------------------------------------
//	NAME:		ctor
//	DESC:		
//	PARAMETERS:	wxWindow* parent
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxLegendWindow::wxLegendWindow(
	 wxWindow *parent
):  wxWindow(parent, -1, wxDefaultPosition, 
			 wxSize(LEGEND_WIDTH, LEGEND_HEIGHT)/*, wxSIMPLE_BORDER*/),
	m_WinParent(parent)
{
	SetBackgroundColour( *wxWHITE );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Add()
//	DESC:		List descriptos utilities
//	PARAMETERS:	const wxString &lbl, 
//				const ChartColor &col
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxLegendWindow::Add(
	const wxString &lbl, 
	const ChartColor &col
) 
{ 
	m_Legend.Add( lbl, col ); 
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Clear()
//	DESC:		List descriptos utilities
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxLegendWindow::Clear() 
{ 
	m_Legend.Clear(); 
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetCount()
//	DESC:		List descriptos utilities
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
int wxLegendWindow::GetCount() const
{
	return ( m_Legend.GetCount() ); 
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetColor()
//	DESC:		List descriptos utilities
//	PARAMETERS:	int n
//	RETURN:		None
//----------------------------------------------------------------------E-+++
ChartColor wxLegendWindow::GetColor(
	int n
) const
{
	return ( m_Legend.GetColor(n) );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetLabel()
//	DESC:		List descriptos utilities
//	PARAMETERS:	int n
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxString wxLegendWindow::GetLabel(
	int n
) const
{
	return ( m_Legend.GetLabel(n) );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Draw()
//	DESC:		Draw legend window
//	PARAMETERS:	CHART_HPAINT hp,
//				int x,
//				int y
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxLegendWindow::Draw(
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
	// Draw legend
	//-----------------------------------------------------------------------
	m_Legend.Draw( hp, &r );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		OnPaint()
//	DESC:		
//	PARAMETERS:	wxPaintEvent &event
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxLegendWindow::OnPaint(
	wxPaintEvent &WXUNUSED(event)
)
{	
	wxPaintDC dc( this );

	//-----------------------------------------------------------------------
	// Draw legend window
	//-----------------------------------------------------------------------
	Draw( &dc );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		OnMouse()
//	DESC:		
//	PARAMETERS:	wxMouseEvent &event
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxLegendWindow::OnMouse(
	wxMouseEvent &event
)
{
	wxPoint p = event.GetPosition();
	
	if ( m_Legend.IsInArrowDown(p.x, p.y) )
	{
		m_Legend.DecPage();
		Refresh();
	}
	else if ( m_Legend.IsInArrowUp(p.x, p.y) )
	{
		m_Legend.IncPage();
		Refresh();
	}
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       OnMouseMove()
//  DESC:       
//  PARAMETERS: wxMouseEvent &event
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void wxLegendWindow::OnMouseMove(
    wxMouseEvent &event
)
{
    wxPoint p = event.GetPosition();
    
    wxClientDC dc(this);
    
    if ( m_Legend.IsInArrowDown(p.x, p.y) )
    {
        m_Legend.DrawArrow(&dc, ARROW_DOWN, true);
    }
    else if ( m_Legend.IsInArrowUp(p.x, p.y) )
    {
        m_Legend.DrawArrow(&dc, ARROW_UP, true);
    }
    else
    {
        m_Legend.DrawArrow(&dc, ARROW_DOWN, false);
        m_Legend.DrawArrow(&dc, ARROW_UP, false);
    }
}
