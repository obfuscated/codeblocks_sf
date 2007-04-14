/////////////////////////////////////////////////////////////////////////////
// Name:        barchartpoints.cpp
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id: barchartpoints.cpp,v 1.1 2006/06/13 12:51:50 pgava Exp $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

// c++
#include <cmath>

// wx
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "barchartpoints.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/label.h"
#include "wx/barchartpoints.h"

//+++-S-cf-------------------------------------------------------------------
//	NAME:		ctor
//	DESC:		has to be created on the heap!
//	PARAMETERS:	std::string name, 
//				ChartColor c
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxBarChartPoints::wxBarChartPoints(
    wxString name,
	ChartColor c,
    bool showlabel   
) : wxChartPoints(wxChartPointsTypes::Bar()),
	m_Name(name), 
	m_Color(c),
	m_Zoom(1),
	m_BarTag(NONE),
    m_ShowLabel( showlabel)  
{
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetXVal()
//	DESC:		
//	PARAMETERS:	int n
//	RETURN:		int
//----------------------------------------------------------------------E-+++
ChartValue wxBarChartPoints::GetXVal(
	int n
) const
{
	return ( m_Points.GetXVal(n) );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetYVal()
//	DESC:		
//	PARAMETERS:	int n
//	RETURN:		int
//----------------------------------------------------------------------E-+++
ChartValue wxBarChartPoints::GetYVal(
	int n
) const
{
	return ( m_Points.GetYVal(n) );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetName()
//	DESC:		
//	PARAMETERS:	int n
//	RETURN:		std::string
//----------------------------------------------------------------------E-+++
wxString wxBarChartPoints::GetName(
	int n
) const
{
	return ( m_Points.GetName(n) );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetCount()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		int
//----------------------------------------------------------------------E-+++
int wxBarChartPoints::GetCount() const
{
	return ( m_Points.GetCount() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetColor()
//	DESC:		
//	PARAMETERS:	int n
//	RETURN:		ChartColor
//----------------------------------------------------------------------E-+++
ChartColor wxBarChartPoints::GetColor(
	int WXUNUSED(n)
) const
{
	return ( 0 );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMaxX()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		int
//----------------------------------------------------------------------E-+++
ChartValue wxBarChartPoints::GetMaxX() const
{
	return ( m_Points.GetMaxX() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMaxY()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		int
//----------------------------------------------------------------------E-+++
ChartValue wxBarChartPoints::GetMaxY() const
{
	return ( m_Points.GetMaxY() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMinX()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		int
//----------------------------------------------------------------------E-+++
ChartValue wxBarChartPoints::GetMinX() const
{
	return ( m_Points.GetMinX() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMinY()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		int
//----------------------------------------------------------------------E-+++
ChartValue wxBarChartPoints::GetMinY() const
{
	return ( m_Points.GetMinY() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetZoom
//	DESC:		Set zoom
//	PARAMETERS:	double
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxBarChartPoints::SetZoom(
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
double wxBarChartPoints::GetZoom()
{
	return ( m_Zoom );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetSizes
//	DESC:		Set sizes for drawing
//	PARAMETERS:	ChartSizes sizes
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxBarChartPoints::SetSizes(
	ChartSizes sizes
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
const ChartSizes& wxBarChartPoints::GetSizes() const
{
	return ( m_Sizes );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetColor()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		unsigned long
//----------------------------------------------------------------------E-+++
ChartColor wxBarChartPoints::GetColor() const
{
	return ( m_Color );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetColor()
//	DESC:		
//	PARAMETERS:	ChartColor c
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxBarChartPoints::SetColor(
	ChartColor c
)
{
	m_Color =  c;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetName()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		ChartColor
//----------------------------------------------------------------------E-+++
wxString wxBarChartPoints::GetName() const
{
	return ( m_Name );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetName()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxBarChartPoints::SetName(
    wxString name
)
{
	m_Name =  name;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Add()
//	DESC:		Add point
//	PARAMETERS:	std::string name, 
//				ChartValue x, 
//				ChartValue y
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxBarChartPoints::Add(
    wxString name, 
	ChartValue x, 
	ChartValue y
)
{
	m_Points.Add( name, x, y );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Add()
//	DESC:		Add point
//	PARAMETERS:	std::string name, 
//				ChartValue x, 
//				ChartValue y,
//				ChartColor c
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxBarChartPoints::Add(
    wxString name, 
	ChartValue x, 
	ChartValue y,
	ChartColor WXUNUSED(c)
)
{
	//-----------------------------------------------------------------------
	// discard color
	//-----------------------------------------------------------------------
	Add( name, x, y );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetDisplay()
//	DESC:		Set Display option
//	PARAMETERS:	wxDISPLAY_LABEL d
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxBarChartPoints::SetDisplayTag(
	wxDISPLAY_LABEL d
)
{
	m_BarTag = d;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetDisplay()
//	DESC:		Get Display option
//	PARAMETERS:	None
//	RETURN:		wxDISPLAY_LABEL
//----------------------------------------------------------------------E-+++
wxDISPLAY_LABEL wxBarChartPoints::GetDisplayTag() const
{
	return ( m_BarTag );
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       Draw()
//  DESC:       Draw Bar chart
//  PARAMETERS: CHART_HPAINT hp, 
//              CHART_HRECT hr
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void wxBarChartPoints::Draw(
    CHART_HPAINT hp, 
    CHART_HRECT hr
)
{
    //-----------------------------------------------------------------------
    // get number of bars
    //-----------------------------------------------------------------------
    double iNodes = ceil( static_cast<double>(GetCount()) );

    //-----------------------------------------------------------------------
    // get max height
    //-----------------------------------------------------------------------
    double ValMax = ceil( GetMaxY() );
    if ( ValMax == 0 ) 
        ValMax = 1;

    //-----------------------------------------------------------------------
    // Get sizes
    //-----------------------------------------------------------------------
    ChartSizes sizes = GetSizes();

    hp->SetBrush( wxBrush(GetColor(), wxSOLID) );
    hp->SetPen( *wxTRANSPARENT_PEN );

    double x, y;
    for ( int iNode = 0; iNode < iNodes; ++ iNode )
    {
        //-------------------------------------------------------------------
        // Get x-position for iNode bar
        //-------------------------------------------------------------------
        double xVal  = ceil( GetXVal(iNode) );
        x = hr->x + GetZoom() * xVal * ( sizes.wbar * sizes.nbar + 
                                         sizes.wbar3d * sizes.nbar3d + 
                                         sizes.gap);

        //-------------------------------------------------------------------
        // Get y-position for iNode bar
        //-------------------------------------------------------------------
        y = hr->y + ( (hr->h - sizes.s_height)* GetYVal(iNode) ) / ValMax ;

        hp->DrawRectangle( static_cast<int>(ceil(x)),
                           static_cast<int>(ceil(hr->h - y)),
                           static_cast<int>(sizes.wbar * GetZoom()), 
                           static_cast<int>(ceil(y)) );
        
        //-------------------------------------------------------------------
        // Only draw Label if user wants it
        //-------------------------------------------------------------------
        if (!m_ShowLabel)
            continue;

        wxString lbl;
        wxLabel wxLbl;
        switch ( GetDisplayTag() )
        {
        case XVALUE:
            lbl.Printf( wxT("%d"), static_cast<int>(xVal));
            wxLbl.Draw( hp, static_cast<int>(ceil(x)), 
                        static_cast<int>(ceil(hr->h - y)), 
                        GetColor(),
                        lbl,
                        UP);
            break;
        case YVALUE:
            lbl.Printf( wxT("%d"), static_cast<int>(GetYVal(iNode)));
            wxLbl.Draw( hp, static_cast<int>(ceil(x)), 
                        static_cast<int>(ceil(hr->h - y)), 
                        GetColor(),
                        lbl,
                        UP );
            break;
        case NAME:
            lbl = GetName(iNode).c_str();
            wxLbl.Draw( hp, static_cast<int>(ceil(x)), 
                        static_cast<int>(ceil(hr->h - y)), 
                        GetColor(),
                        lbl,
                        UP );
            break;
        default:
            break;      
        }
    }

    hp->SetPen( *wxBLACK_PEN );
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       CreateWxBarChartPoints()
//  DESC:       
//  PARAMETERS: wxString name
//              ChartColor c
//              bool showlabel
//  RETURN:     CBarChartPoints*
//----------------------------------------------------------------------E-+++
wxBarChartPoints* wxBarChartPoints::CreateWxBarChartPoints(
    wxString name,
    ChartColor c,
    bool showlabel
)
{
    if ( c == wxCHART_NOCOLOR )
        c = wxChartColors::GetColor();

    return new wxBarChartPoints( name, c, showlabel );
}
