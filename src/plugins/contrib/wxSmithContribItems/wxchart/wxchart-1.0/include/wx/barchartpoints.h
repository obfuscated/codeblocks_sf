/////////////////////////////////////////////////////////////////////////////
// Name:        barcharpoints.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __BARCHARTPOINTS_H__ )
#define __BARCHARTPOINTS_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "barcharpoints.h"
#endif

#include "wx/points.h"
#include "wx/chartcolors.h"
#include "wx/chartpoints.h"

//+++-S-cd-------------------------------------------------------------------
//	NAME:		wxBarChartPoints
//	DESC:		Definition of XY for a bar chart
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
WXDLLIMPEXP_CHART class wxBarChartPoints : public wxChartPoints
{
public:

    // Create object. This class has to be created on the heap
    // because CChart needs its base pointer to create a list
    // example:
    // {
    //   CBarChartPoints bcp;
    //   CChart c;
    //   c.Add( &bcp );
    // }
    // If this is allow after the bcp is out of scope the list has
    // a pointer which has been deallocated!
    //------------------------------------------------------------
    static wxBarChartPoints* CreateWxBarChartPoints(wxString name, 
        ChartColor c = wxCHART_NOCOLOR, bool showlabel = false); 

	virtual ~wxBarChartPoints() {};

	// Draw the series of points
	//--------------------------
	void Draw(CHART_HPAINT hp, CHART_HRECT hr);
    
	// Get n-th point information
	//---------------------------
	ChartValue GetXVal(int n) const;
	ChartValue GetYVal(int n) const;
    wxString GetName(int n) const;
	ChartColor GetColor(int n) const;

	// Get stat values
	//----------------
	int GetCount() const;
	ChartValue GetMaxX() const;
	ChartValue GetMaxY() const;
	ChartValue GetMinX() const;
	ChartValue GetMinY() const;

	// Get/Set zoom
	//-------------
	void SetZoom(double z);
	double GetZoom();

	// Set sizes for drawing
	//----------------------
	void SetSizes(ChartSizes sizes);
	const ChartSizes& GetSizes() const;

	// Get/Set Color
	//--------------
	ChartColor GetColor() const;
	void SetColor(ChartColor c);

	// Get/Set Name
	//--------------
    wxString GetName() const;
    void SetName(wxString name);

	// Add point
	//----------
    void Add(wxString name, ChartValue x, ChartValue y);
    void Add(wxString name, ChartValue x, ChartValue y,
			 ChartColor c);

	// Set/Get Display option
	//-----------------------
	void SetDisplayTag(wxDISPLAY_LABEL d);
	wxDISPLAY_LABEL GetDisplayTag() const;

private:
	wxPoints m_Points;
    wxString m_Name;
	ChartColor m_Color;
	double m_Zoom;
	wxDISPLAY_LABEL m_BarTag;
    bool m_ShowLabel;   
	ChartSizes m_Sizes;

    // ctor
    // has to be created on the heap!
    //-------------------------------
    wxBarChartPoints(wxString name, ChartColor c, bool showlabel);
	
    // copy ctor & op= NOT allow
	//--------------------------
	wxBarChartPoints(const wxBarChartPoints&);
	wxBarChartPoints& operator=(const wxBarChartPoints&);
};


#endif // __BARCHARTPOINTS_H__
