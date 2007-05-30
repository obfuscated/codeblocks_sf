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

#include "wx/points.h"
#include "wx/chartcolors.h"
#include "wx/chartpoints.h"

//+++-S-cd-------------------------------------------------------------------
//  NAME:       wxBarChartPoints
//  DESC:       Definition of XY for a bar chart
//  INTERFACE:
//
//----------------------------------------------------------------------E-+++
WXDLLIMPEXP_CHART class wxBar3DChartPoints : public wxChartPoints
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
    virtual void Draw(CHART_HPAINT hp, CHART_HRECT hr);

    // Get n-th point information
    //---------------------------
    virtual ChartValue GetXVal(int n) const;
    virtual ChartValue GetYVal(int n) const;
    virtual wxString GetName(int n) const;
    virtual ChartColor GetColor(int n) const;

    // Get stat values
    //----------------
    virtual int GetCount() const;
    virtual ChartValue GetMaxX() const;
    virtual ChartValue GetMaxY() const;
    virtual ChartValue GetMinX() const;
    virtual ChartValue GetMinY() const;

    // Get/Set zoom
    //-------------
    virtual void SetZoom(double z);
    virtual double GetZoom();

    // Set sizes for drawing
    //----------------------
    virtual void SetSizes(ChartSizes sizes);
    virtual const ChartSizes& GetSizes() const;

    // Get/Set Color
    //--------------
    virtual ChartColor GetColor() const;
    virtual void SetColor(ChartColor c);

    // Get/Set Name
    //--------------
    virtual wxString GetName() const;
    virtual void SetName(wxString name);

    // Add point
    //----------
    virtual void Add(wxString name, ChartValue x, ChartValue y);
    virtual void Add(wxString name, ChartValue x, ChartValue y,
             ChartColor c);

    // Set/Get Display option
    //-----------------------
    virtual void SetDisplayTag(wxDISPLAY_LABEL d);
    virtual wxDISPLAY_LABEL GetDisplayTag() const;

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
    wxBar3DChartPoints(wxString name, ChartColor c, bool showlabel);

    // copy ctor & op= NOT allow
    //--------------------------
    wxBar3DChartPoints(const wxBar3DChartPoints&);
    wxBar3DChartPoints& operator=(const wxBar3DChartPoints&);

};


#endif // __BAR3DCHARTPOINTS_H__

