/////////////////////////////////////////////////////////////////////////////
// Name:        bar3dcharpoints.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __BAR3DCHARTPOINTS_H__ )
#define __BAR3DCHARTPOINTS_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "bar3dchartpoints.h"
#endif

#include "wx/points.h"
#include "wx/chartcolors.h"
#include "wx/chartpoints.h"

// External Classes
//-----------------
class wxChartSizes;

//+++-S-cd-------------------------------------------------------------------
//  NAME:       wxBarChartPoints
//  DESC:       Definition of XY for a bar chart
//  INTERFACE:
//
//----------------------------------------------------------------------E-+++
/* C::B begin */
class WXDLLIMPEXP_CHART wxBar3DChartPoints : public wxChartPoints
/* C::B end */
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
    static wxBar3DChartPoints* CreateWxBar3DChartPoints(wxString name,
        ChartColor c = wxCHART_NOCOLOR, bool showlabel = false);

    virtual ~wxBar3DChartPoints() {};

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
    void SetSizes(wxChartSizes *sizes);
    wxChartSizes* GetSizes() const;

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
    wxChartSizes *m_Sizes;

    // ctor
    // has to be created on the heap!
    //-------------------------------
    wxBar3DChartPoints(wxString name, ChartColor c, bool showlabel);

    // copy ctor & op= NOT allow
    //--------------------------
    wxBar3DChartPoints(const wxBar3DChartPoints&);
    wxBar3DChartPoints& operator=(const wxBar3DChartPoints&);

};


#endif // __BAR3DCHARTPOINTS_H__

