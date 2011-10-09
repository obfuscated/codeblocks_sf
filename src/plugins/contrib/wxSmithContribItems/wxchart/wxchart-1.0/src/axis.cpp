/////////////////////////////////////////////////////////////////////////////
// Name:        axis.cpp
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
#include <wx/axis.h>
#include <wx/chartsizes.h>

//+++-S-cf-------------------------------------------------------------------
//	NAME:		ctor
//	DESC:
//	PARAMETERS:	ChartValue max,
//				ChartValue min
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxAxis::wxAxis(
	ChartValue max,
	ChartValue min
) : m_vMax(max),
	m_vMin(min),
	m_Zoom(1)
{
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetVirtualMax
//	DESC:
//	PARAMETERS:	ChartValue v
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxAxis::SetVirtualMax(
	ChartValue v
)
{
	m_vMax = v;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetVirtualMin
//	DESC:
//	PARAMETERS:	ChartValue v
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxAxis::SetVirtualMin(
	ChartValue v
)
{
	m_vMin = v;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetVirtualMax
//	DESC:
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxAxis::GetVirtualMax() const
{
	return ( m_vMax );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetVirtualMin
//	DESC:
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxAxis::GetVirtualMin() const
{
	return ( m_vMin );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetZoom
//	DESC:		Set zoom
//	PARAMETERS:	double
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxAxis::SetZoom(
	double z
)
{
	m_Zoom = z;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetZoom
//	DESC:		Get zoom
//	PARAMETERS:	double
//	RETURN:		None
//----------------------------------------------------------------------E-+++
double wxAxis::GetZoom()
{
    return ( m_Sizes->GetXZoom() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetSizes
//	DESC:		Set sizes for drawing
//	PARAMETERS:	ChartSizes sizes
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxAxis::SetSizes(
	wxChartSizes *sizes
)
{
    m_Sizes = sizes;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetSizes
//	DESC:		Get sizes for drawing
//	PARAMETERS:	None
//	RETURN:		ChartSizes sizes
//----------------------------------------------------------------------E-+++
wxChartSizes* wxAxis::GetSizes() const
{
	return ( m_Sizes );
}
