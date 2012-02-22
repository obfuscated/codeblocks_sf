/////////////////////////////////////////////////////////////////////////////
// Name:        chartpoints.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id: chartpoints.h 7559 2011-11-04 10:19:48Z mortenmacfly $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __CHARTPOINTS_H__ )
#define __CHARTPOINTS_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

#include "wx/charttypes.h"
#include "wx/chartpointstypes.h"

// External Classes
//-----------------
class wxChartSizes;

//+++-S-te-------------------------------------------------------------------
//  NAME:       wxDISPLAY_LABEL
//  DESC:       Specify how to display Label on Chart.
//
//----------------------------------------------------------------------E-+++
enum wxDISPLAY_LABEL
{
    NONE,           // no label
    XVALUE,         // display x value
    YVALUE,         // display y value
    XVALUE_FLOAT,   // display x value with 1 decimal number
    YVALUE_FLOAT,   // display y value with 1 decimal number
    NAME            // display custom label
};


//+++-S-cd-------------------------------------------------------------------
//	NAME:		wxChartPoints
//	DESC:		Generic chart points description
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
/* C::B begin */
class WXDLLIMPEXP_CHART wxChartPoints
/* C::B end */
{

public:
	wxChartPoints(wxChartPointsTypes t) : m_Type(t) {};
	virtual ~wxChartPoints() {};

	// Draw the series of points
	// pure virtual function
	//--------------------------
	virtual void Draw(CHART_HPAINT hp, CHART_HRECT hr) = 0;

	// Get n-th point information
	// pure virtual functions
	//---------------------------
	virtual ChartValue GetXVal(int n) const = 0;
	virtual ChartValue GetYVal(int n) const = 0;
    virtual wxString GetName(int n) const = 0;
	virtual ChartColor GetColor(int n) const = 0;

	// Get stat values
	// pure virtual functions
	//-----------------------
	virtual int GetCount() const = 0;
	virtual ChartValue GetMaxX() const = 0;
	virtual ChartValue GetMaxY() const = 0;
	virtual ChartValue GetMinX() const = 0;
	virtual ChartValue GetMinY() const = 0;

	// Get/Set zoom
	// pure virtual functions
	//-----------------------
	virtual void SetZoom(double z) = 0;
	virtual double GetZoom() = 0;

	// Set sizes for drawing
	// pure virtual functions
	//-----------------------
	virtual void SetSizes(wxChartSizes* sizes) = 0;
	virtual wxChartSizes* GetSizes() const = 0;

	// Get/Set Color
	// pure virtual functions
	//-----------------------
	virtual ChartColor GetColor() const = 0;
	virtual void SetColor(ChartColor c) = 0;

	// Get/Set Name
	// pure virtual functions
	//-----------------------
    virtual wxString GetName() const = 0;
    virtual void SetName(wxString name) = 0;

	// Add point
	//----------
    virtual void Add(wxString name, ChartValue x, ChartValue y) = 0;
    virtual void Add(wxString name, ChartValue x, ChartValue y,
				     ChartColor c) = 0;

	// Set/Get Display option
	//-----------------------
	virtual void SetDisplayTag(wxDISPLAY_LABEL d) = 0;
	virtual wxDISPLAY_LABEL GetDisplayTag() const = 0;

	// Get type
	//---------
	const wxChartPointsTypes GetType() const { return m_Type; }

private:

	// chart points type: bar, bar3d, ...
	//-----------------------------------
	const wxChartPointsTypes m_Type;

	// copy ctor & op= NOT allow
	//--------------------------
	wxChartPoints(const wxChartPoints&);
	wxChartPoints& operator=(const wxChartPoints&);

};


//+++-S-cf-------------------------------------------------------------------
//	NAME:		op==()
//	DESC:
//	PARAMETERS:	const wxChartPoints& c,
//				const wxChartPointsTypes& t
//	RETURN:		bool
//----------------------------------------------------------------------E-+++
inline bool operator==(
	const wxChartPoints& c,
	const wxChartPointsTypes& t
)
{
	return ( c.GetType() == t );
}

#endif // __CHARTPOINTS_H__

