/////////////////////////////////////////////////////////////////////////////
// Name:        axis.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __AXIS_H__ )
#define __AXIS_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------
#include "wx/charttypes.h"

// External Classes
//-----------------
class wxChartSizes;

//+++-S-cd-------------------------------------------------------------------
//	NAME:		wxAxis
//	DESC:
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
/* C::B begin */
class WXDLLIMPEXP_CHART wxAxis
/* C::B end */
{
public:
	wxAxis(ChartValue max = 0, ChartValue min = 0);
	virtual ~wxAxis() {};

	// Draw xaxis area
	//-----------------
	virtual void Draw(CHART_HPAINT hp, CHART_HRECT hr) = 0;

	// Get/Set virtual size
	//---------------------
	virtual void SetVirtualMax(ChartValue v);
	virtual void SetVirtualMin(ChartValue v);
	virtual ChartValue GetVirtualMax() const;
	virtual ChartValue GetVirtualMin() const;

	// Get/Set zoom
	//-------------
	virtual void SetZoom(double z);
	virtual double GetZoom();

	// Set sizes for drawing
	//----------------------
	virtual void SetSizes(wxChartSizes *sizes);
	virtual wxChartSizes* GetSizes() const;

private:
	ChartValue m_vMax;
	ChartValue m_vMin;
	double m_Zoom;
	wxChartSizes *m_Sizes;
};

#endif // __XAXIS_H__
