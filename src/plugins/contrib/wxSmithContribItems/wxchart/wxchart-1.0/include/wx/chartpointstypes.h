/////////////////////////////////////////////////////////////////////////////
// Name:        chartpointstypes.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __CHARTPOINTSTYPES_H__ )
#define __CHARTPOINTSTYPES_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------
#define UNUSED_ARG(arg)

//+++-S-cd-------------------------------------------------------------------
//	NAME:		wxChartPointsTypes
//	DESC:
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
class WXDLLIMPEXP_CHART wxChartPointsTypes
{
private:
	enum CHART_POINTS_TYPES
	{
		BAR = 0,
		BAR3D,
		PIE,
		PIE3D,
		POINTS,
		POINTS3D,
		LINE,
		LINE3D,
		AREA,
		AREA3D
	};

public:

	// Type ctors
	//-----------
	static const wxChartPointsTypes Bar() { return (BAR); }
	static const wxChartPointsTypes Bar3D() { return (BAR3D); }
	static const wxChartPointsTypes Pie() { return (PIE); }
	static const wxChartPointsTypes Pie3D() { return (PIE3D); }
	static const wxChartPointsTypes Points() { return (POINTS); }
	static const wxChartPointsTypes Poiints3D() { return (POINTS3D); }
	static const wxChartPointsTypes Line() { return (LINE); }
	static const wxChartPointsTypes Line3D() { return (LINE3D); }
	static const wxChartPointsTypes Area() { return (AREA); }
	static const wxChartPointsTypes Area3D() { return (AREA3D); }

	// conversion to int
	//------------------
	int GetIntType() const { return (int)m_Type; }

private:
	const CHART_POINTS_TYPES m_Type;

	wxChartPointsTypes(CHART_POINTS_TYPES t) : m_Type(t) {};

	// Wrong. Only for MSW compiler
    wxChartPointsTypes& operator=(const wxChartPointsTypes& UNUSED_ARG(rhs))
        { return *this;}
};

//+++-S-cf-------------------------------------------------------------------
//	NAME:		op==()
//	DESC:
//	PARAMETERS:	const CChartPointsTypes& lhs,
//				const CChartPointsTypes& rhs
//	RETURN:		bool
//----------------------------------------------------------------------E-+++
inline bool operator==(
	const wxChartPointsTypes& lhs,
	const wxChartPointsTypes& rhs
)
{
	return ( lhs.GetIntType() == rhs.GetIntType() );
}

#endif // __CHARTPOINTSTYPES_H__
