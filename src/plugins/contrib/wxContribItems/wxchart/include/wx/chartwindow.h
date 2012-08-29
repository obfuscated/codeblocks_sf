/////////////////////////////////////////////////////////////////////////////
// Name:        chartwindow.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __WXCHARTWINDOW_H__ )
#define __WXCHARTWINDOW_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "chartwindow.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/window.h>
    #include <wx/scrolwin.h>
#endif

#include "wx/chart.h"

//----------------------------------------------------------------------------
// Classes
//----------------------------------------------------------------------------
class wxChartSizes;

//+++-S-cd-------------------------------------------------------------------
//	NAME:		CWxChartWindow
//	DESC:
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
/* C::B begin */
class WXDLLIMPEXP_CHART wxChartWindow : public wxWindow
/* C::B end */
{
public:

	wxChartWindow(wxScrolledWindow *parent = NULL, bool usegrid = true);

	// allow access of CChart's public methods
	// Add() - Clear()
	//-----------------------------------------
	void Add(wxChartPoints* cp);
	void Clear();

	// allow access of CChart's public methods
	// Get statistics
	//---------------
	ChartValue GetMaxX() const;
	ChartValue GetMinX() const;
	ChartValue GetMaxY() const;
	ChartValue GetMinY() const;
	int GetNumBarPoints() const;
	int GetNumBar3DPoints() const;

    // Get/Set virtual size
    //---------------------
    ChartValue GetVirtualMaxX() const;
    ChartValue GetVirtualMinX() const;
    ChartValue GetVirtualMaxY() const;
    ChartValue GetVirtualMinY() const;
    ChartValue GetVirtualWidth() const;

    // Get/Set zoom
    //-------------
    void SetZoom(double z);
    double GetZoom() const;

    // Set sizes for drawing
    //----------------------
    void SetSizes(wxChartSizes *sizes);
    wxChartSizes *GetSizes() const;

	// Draw chart points
	//------------------
	void Draw(CHART_HPAINT hp, int x = 0, int y = 0);

private:
	wxChart m_Chart;
	wxScrolledWindow *m_WinParent;
    bool m_UseGrid;

    // Draw helper function
    //-----------------
    void DrawHLines(CHART_HPAINT hp, CHART_HRECT hr);

	// wxWindows events
	//-----------------
    void OnPaint(wxPaintEvent &event);
    void OnMouse(wxMouseEvent &event);

    DECLARE_DYNAMIC_CLASS(wxChartWindow)
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// Inlines
//----------------------------------------------------------------------------


//+++-S-cf-------------------------------------------------------------------
//	NAME:		Add()
//	DESC:
//	PARAMETERS:	CChartPoints* cp
//	RETURN:		None
//----------------------------------------------------------------------E-+++
inline void wxChartWindow::Add(
	wxChartPoints* cp
)
{
	m_Chart.Add( cp );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Clear()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
inline void wxChartWindow::Clear()
{
	m_Chart.Clear();
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMaxX()
//	DESC:		Get the max x-val calculated from all chartpoints
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
inline ChartValue wxChartWindow::GetMaxX() const
{
	return ( m_Chart.GetMaxX() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMinX()
//	DESC:		Get the min x-val calculated from all chartpoints
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
inline ChartValue wxChartWindow::GetMinX() const
{
	return ( m_Chart.GetMinX() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetYMax()
//	DESC:		Get the max y-val calculated from all chartpoints
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
inline ChartValue wxChartWindow::GetMaxY() const
{
	return ( m_Chart.GetMaxY() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetYMin()
//	DESC:		Get the min y-val calculated from all chartpoints
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
inline ChartValue wxChartWindow::GetMinY() const
{
	return ( m_Chart.GetMinY() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetNumBarPoints()
//	DESC:		Number of bar points is used to calculate the width
//				of the chart
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
inline int wxChartWindow::GetNumBarPoints() const
{
	return ( m_Chart.GetNumBarPoints() );

}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetNumBar3DPoints()
//	DESC:		Number of bar points is used to calculate the width
//				of the chart
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
inline int wxChartWindow::GetNumBar3DPoints() const
{
	return ( m_Chart.GetNumBar3DPoints() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetZoom
//	DESC:		Set zoom
//	PARAMETERS:	double
//	RETURN:		None
//----------------------------------------------------------------------E-+++
inline void wxChartWindow::SetZoom(
	double z
)
{
	m_Chart.SetZoom( z );
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       SetZoom
//  DESC:       Set zoom
//  PARAMETERS: double
//  RETURN:     None
//----------------------------------------------------------------------E-+++
inline double wxChartWindow::GetZoom() const
{
    return m_Chart.GetZoom();
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetSizes
//	DESC:		Set sizes for drawing
//	PARAMETERS:	ChartSizes sizes
//	RETURN:		None
//----------------------------------------------------------------------E-+++
inline void wxChartWindow::SetSizes(
    wxChartSizes *sizes
)
{
    m_Chart.SetSizes( sizes );
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       GetSizes
//  DESC:       Get sizes for drawing
//  PARAMETERS: None
//  RETURN:     ChartSizes sizes
//----------------------------------------------------------------------E-+++
inline wxChartSizes *wxChartWindow::GetSizes() const
{
    return m_Chart.GetSizes();
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       GetVirtualMaxX
//  DESC:       Get virtual size for drawing
//  PARAMETERS: None
//  RETURN:     ChartValue
//----------------------------------------------------------------------E-+++
inline ChartValue wxChartWindow::GetVirtualMaxX() const
{
    return GetMaxX();
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       GetVirtualMinX
//  DESC:       Get virtual size for drawing
//  PARAMETERS: None
//  RETURN:     ChartValue
//----------------------------------------------------------------------E-+++
inline ChartValue wxChartWindow::GetVirtualMinX() const
{
    return GetMinX();
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       GetVirtualMaxY
//  DESC:       Get virtual size for drawing
//  PARAMETERS: None
//  RETURN:     ChartValue
//----------------------------------------------------------------------E-+++
inline ChartValue wxChartWindow::GetVirtualMaxY() const
{
    return GetMaxY();
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       GetVirtualMinX
//  DESC:       Get virtual size for drawing
//  PARAMETERS: None
//  RETURN:     ChartValue
//----------------------------------------------------------------------E-+++
inline ChartValue wxChartWindow::GetVirtualMinY() const
{
    return GetMinY();
}

#endif // __WXCHARTWINDOW_H__
