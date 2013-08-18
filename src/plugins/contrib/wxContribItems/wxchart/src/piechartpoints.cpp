/////////////////////////////////////////////////////////////////////////////
// Name:        piechartpoints.cpp
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

// c++
#include <cmath>

// wx
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "piechartpoints.h"
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
#include "wx/piechartpoints.h"
#include "wx/chartsizes.h"

//+++-S-cf-------------------------------------------------------------------
//	NAME:		ctor
//	DESC:		has to be created on the heap!
//	PARAMETERS:	wxString name,
//				ChartColor c
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxPieChartPoints::wxPieChartPoints(
	wxString name,
	ChartColor c,
    bool showlabel
) : wxChartPoints(wxChartPointsTypes::Pie()),
	m_Name(name),
	m_Color(c),
	m_Zoom(1),
	m_PieTag(NONE),
    m_ShowLabel(showlabel)
{
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetXVal()
//	DESC:
//	PARAMETERS:	int n
//	RETURN:		int
//----------------------------------------------------------------------E-+++
ChartValue wxPieChartPoints::GetXVal(
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
ChartValue wxPieChartPoints::GetYVal(
	int n
) const
{
	return ( m_Points.GetYVal(n) );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetName()
//	DESC:
//	PARAMETERS:	int n
//	RETURN:		wxString
//----------------------------------------------------------------------E-+++
wxString wxPieChartPoints::GetName(
	int n
) const
{
	return ( m_Points.GetName(n) );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetColor()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		unsigned long
//----------------------------------------------------------------------E-+++
ChartColor wxPieChartPoints::GetColor(int n) const
{
	return ( m_Points.GetColor(n) );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetCount()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		int
//----------------------------------------------------------------------E-+++
int wxPieChartPoints::GetCount() const
{
	return ( m_Points.GetCount() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMaxX()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		int
//----------------------------------------------------------------------E-+++
ChartValue wxPieChartPoints::GetMaxX() const
{
	//return ( m_Points.GetMaxX() );
	return ( 0 );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMaxY()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		int
//----------------------------------------------------------------------E-+++
ChartValue wxPieChartPoints::GetMaxY() const
{
//	return ( m_Points.GetMaxY() );
	return ( 0 );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMinX()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		int
//----------------------------------------------------------------------E-+++
ChartValue wxPieChartPoints::GetMinX() const
{
//	return ( m_Points.GetMinX() );
	return ( 0 );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMinY()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		int
//----------------------------------------------------------------------E-+++
ChartValue wxPieChartPoints::GetMinY() const
{
//	return ( m_Points.GetMinY() );
	return ( 0 );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetZoom
//	DESC:		Set zoom
//	PARAMETERS:	double
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxPieChartPoints::SetZoom(
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
double wxPieChartPoints::GetZoom()
{
	return ( m_Zoom );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetSizes
//	DESC:		Set sizes for drawing
//	PARAMETERS:	ChartSizes sizes
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxPieChartPoints::SetSizes(
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
wxChartSizes* wxPieChartPoints::GetSizes() const
{
	return ( m_Sizes );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetColor()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		unsigned long
//----------------------------------------------------------------------E-+++
ChartColor wxPieChartPoints::GetColor() const
{
	return ( m_Color );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetColor()
//	DESC:
//	PARAMETERS:	ChartColor c
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxPieChartPoints::SetColor(
	ChartColor c
)
{
	m_Color = c;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetName()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		ChartColor
//----------------------------------------------------------------------E-+++
wxString wxPieChartPoints::GetName() const
{
	return ( m_Name );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetName()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxPieChartPoints::SetName(
	wxString name
)
{
	m_Name =  name;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Add()
//	DESC:		Add point
//	PARAMETERS:	wxString name,
//				ChartValue x,
//				ChartValue y
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxPieChartPoints::Add(
	wxString name,
	ChartValue x,
	ChartValue y
)
{
	Add( name, x, y, wxCHART_NOCOLOR );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Add()
//	DESC:		Add point
//	PARAMETERS:	wxString name,
//				ChartValue x,
//				ChartValue y,
//				ChartColor c
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxPieChartPoints::Add(
	wxString name,
	ChartValue x,
	ChartValue y,
	ChartColor c
)
{
	if ( c == wxCHART_NOCOLOR )
		c = wxChartColors::GetColor();
	m_Points.Add( name, x, y, c );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetDisplay()
//	DESC:		Set Display option
//	PARAMETERS:	wxDISPLAY_LABEL d
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxPieChartPoints::SetDisplayTag(
 	wxDISPLAY_LABEL d
)
{
	m_PieTag = d;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetDisplay()
//	DESC:		Get Display option
//	PARAMETERS:	None
//	RETURN:		wxDISPLAY_LABEL
//----------------------------------------------------------------------E-+++
wxDISPLAY_LABEL wxPieChartPoints::GetDisplayTag() const
{
	return ( m_PieTag );
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       Draw()
//  DESC:       Draw Bar chart
//  PARAMETERS: CHART_HPAINT hp,
//              CHART_HRECT hr
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void wxPieChartPoints::Draw(
    CHART_HPAINT hp,
    CHART_HRECT hr
)
{

    //-----------------------------------------------------------------------
    // Get sizes
    //-----------------------------------------------------------------------
    wxChartSizes *sizes = GetSizes();

    int r = (int)wxMin( (int)hr->w/2,
        (int)(hr->h - 2*sizes->GetSizeHeight())/2 );

    if ( r > 0 )
    {
        int iNodes = GetCount();

        if ( iNodes > 0 )
        {
            int iData;
            int ValTot;
            int iDatas = GetCount();
            for ( iData = 0, ValTot = 0; iData < iDatas; ++ iData )
                ValTot += static_cast<int>(GetYVal( iData ));

            double percent;
            double grad;
            double rad;
            int x1, y1, x2, y2, xc, yc;
            xc = hr->x + hr->w/2;
            yc = hr->y + hr->h/2;
            x1 = xc + r;
            y1 = yc;

            hp->SetPen( *wxBLACK_PEN );

            for ( iData = 0, rad = 0; iData < iDatas; ++ iData )
            {
                hp->SetBrush( wxBrush(GetColor(iData), wxSOLID) );

                // Calc radiants
                percent = (double)(GetYVal(iData) * 100) / (double)ValTot;
                grad    = (double)(percent * 360) / (double)100;
                rad     += (double)(grad * 3.1415) / (double)180;

                x2 = (int)(xc + r * cos( rad ));
                y2 = (int)(yc - r * sin( rad ));
                hp->DrawArc( x1, y1, x2, y2, xc, yc );
                x1 = x2;
                y1 = y2;

                //-----------------------------------------------------------
                // Only draw Label if user wants it
                //-----------------------------------------------------------
                if (!m_ShowLabel)
                    continue;

                wxString lbl;
                wxLabel wxLbl;

                LABEL_POSITION p;
                if ( x2 > xc )
                    p = RIGHT;
                else
                    p = LEFT;
                if ( y2 > yc )
                    p = (LABEL_POSITION)( p | DOWN );
                else
                    p = (LABEL_POSITION)( p | UP );

                switch ( GetDisplayTag() )
                {
                case XVALUE:
                    lbl.Printf( wxT("%d"), static_cast<int>(GetXVal(iData)) );
                    wxLbl.Draw( hp, x2, y2, GetColor(iData), lbl, p );
                    break;
                case YVALUE:
                    lbl.Printf( wxT("%d"), static_cast<int>(GetYVal(iData)) );
                    wxLbl.Draw( hp, x2, y2, GetColor(iData), lbl, p );
                    break;
                case XVALUE_FLOAT:
                    lbl.Printf( wxT("%4.1f"), GetXVal(iData) );
                    wxLbl.Draw( hp, x2, y2, GetColor(iData), lbl, p );
                    break;
                case YVALUE_FLOAT:
                    lbl.Printf( wxT("%4.1f"), GetYVal(iData) );
                    wxLbl.Draw( hp, x2, y2, GetColor(iData), lbl, p );
                    break;
                    case NAME:
                    lbl = GetName(iData).c_str();
                    wxLbl.Draw( hp, x2, y2, GetColor(iData), lbl, p );
                    break;
                default:
                    break;
                }
            }
        }
    }

}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       CreateWxPieChartPoints()
//  DESC:
//  PARAMETERS: wxString name,
//              ChartColor c
//              bool showlabel
//  RETURN:     CBarChartPoints*
//----------------------------------------------------------------------E-+++
wxPieChartPoints* wxPieChartPoints::CreateWxPieChartPoints(
    wxString name,
    ChartColor c,
    bool showlabel
)
{
    if ( c == wxCHART_NOCOLOR )
        c = wxChartColors::GetColor();

    return new wxPieChartPoints( name, c, showlabel );
}
