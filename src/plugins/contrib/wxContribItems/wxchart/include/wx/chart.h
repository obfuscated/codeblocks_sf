/////////////////////////////////////////////////////////////////////////////
// Name:        chart.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id: chart.h 7559 2011-11-04 10:19:48Z mortenmacfly $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __CHART_H__ )
#define __CHART_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "chart.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/dynarray.h>
#endif

#include "wx/chartpoints.h"

// External Classes
//-----------------
class wxChartSizes;

//----------------------------------------------------------------------------
// Declare Array/List of Points
//----------------------------------------------------------------------------
WX_DECLARE_OBJARRAY(wxChartPoints*, ListChartPoints);

//+++-S-cd-------------------------------------------------------------------
//	NAME:		wxChart
//	DESC:
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
/* C::B begin */
class WXDLLIMPEXP_CHART wxChart
/* C::B end */
{
public:
	wxChart();
	virtual ~wxChart();

	// ChartPoints utility
	//--------------------
	void Add(wxChartPoints* cp);
	void Clear();

	// Get statistics
	//---------------
	ChartValue GetMaxX() const;
	ChartValue GetMinX() const;
	ChartValue GetMaxY() const;
	ChartValue GetMinY() const;
	int GetNumBarPoints() const;
	int GetNumBar3DPoints() const;

	// Set zoom
	//-------------
	void SetZoom(double z);
    double GetZoom() const;

	// Set sizes for drawing
	//----------------------
	void SetSizes(wxChartSizes *sizes);
    wxChartSizes* GetSizes() const;

	// Draw chart
	//-----------
	virtual void Draw(CHART_HPAINT hp, CHART_HRECT hr);

private:
	ListChartPoints m_LCP;
};

#endif // __CHART_H__
