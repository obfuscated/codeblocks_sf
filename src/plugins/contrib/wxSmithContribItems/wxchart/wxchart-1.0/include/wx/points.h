/////////////////////////////////////////////////////////////////////////////
// Name:        points.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __POINTS_H__ )
#define __POINTS_H__

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
    #include <wx/dynarray.h>
#endif

#include "wx/charttypes.h"

//----------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------

// Point description
//------------------
struct Point
{
    wxString m_name;
    ChartValue m_xval;
    ChartValue m_yval;
    ChartColor m_col;
    Point(wxString name=wxEmptyString, ChartValue x=0, ChartValue y=0,
            ChartColor c = 0);
};

//----------------------------------------------------------------------------
// Declare Array/List of Points
//----------------------------------------------------------------------------
WX_DECLARE_OBJARRAY(Point, ListPoints);

//+++-S-cd-------------------------------------------------------------------
//	NAME:		CPoints
//	DESC:		points implementation with list
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
WXDLLIMPEXP_CHART class wxPoints
{
public:
	wxPoints();
	~wxPoints();

	// Add Point to vector (sort by xval)
	//-----------------------------------
	void Add(const Point &p);
    void Add(const wxString s, const ChartValue x, const ChartValue y,
			 const ChartColor c = 0);

	// remove points from vector
	//--------------------------
	void Clear();

	// Get points (vals) from vector
	//------------------------------
	Point GetPoint(size_t n) const;
    wxString GetName(size_t n) const;
	ChartValue GetXVal(size_t n) const;
	ChartValue GetYVal(size_t n) const;
	ChartColor GetColor(size_t n) const;

	// Get stat values
	//------------------------------
	size_t GetCount() const;
	ChartValue GetMaxX() const;
	ChartValue GetMaxY() const;
	ChartValue GetMinX() const;
	ChartValue GetMinY() const;

	// Test
	//-----
	//void Dump();

private:

	ListPoints m_vPoints;

	// Utility list manipolation
	//--------------------------
	size_t GetInsertPosition(const Point& p);
	//size_t GetPosition(int n);

	// copy ctor & op= NOT allow
	//--------------------------
	wxPoints(const wxPoints&);
	wxPoints& operator=(const wxPoints&);
};


#endif // __POINTS_H__
