/////////////////////////////////////////////////////////////////////////////
// Name:        pie3dchartpoints.cpp
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id: pie3dchartpoints.cpp,v 1.1 2006/06/13 12:51:50 pgava Exp $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

// c++
#include <cmath>

// wx
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "pie3dchartpoints.h"
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
#include "wx/pie3dchartpoints.h"


//---------------------------------------------------------------------------
// Consts
//---------------------------------------------------------------------------
const int SHADOW_DEEP = 20;
const double ELLISSE_W = 2;
const double ELLISSE_H = 0.75;


//+++-S-cf-------------------------------------------------------------------
//	NAME:		DegToRad
//	DESC:		Convert Degree -> Radiant
//	PARAMETERS:	double deg
//	RETURN:		Radiant
//----------------------------------------------------------------------E-+++
static inline double DegToRad(
	double deg
) 
{ 
	return (deg * M_PI) / 180.0; 
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       DegToRad
//  DESC:       Convert Degree -> Radiant
//  PARAMETERS: 
//    int w
//    int h
//    int x0
//    int y0
//    double rad
//  RETURN:     Radiant
//----------------------------------------------------------------------E-+++
static inline wxPoint EllipsePoint(
    int w,
    int h,
    int x0,
    int y0,
    double rad
) 
{ 
    int halfW = static_cast<int>( floor((double)w/2) );
    int halfH = static_cast<int>( floor((double)h/2) );
    
    int x = x0 + halfW + static_cast<int>( floor(halfW * cos(rad)) );
    int y = y0 + halfH - static_cast<int>( floor(halfH * sin(rad)) );
    
    return wxPoint( x, y ); 
}


//+++-S-cf-------------------------------------------------------------------
//	NAME:		ctor
//	DESC:		has to be created on the heap!
//	PARAMETERS:	wxString name, 
//				ChartColor c
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxPie3DChartPoints::wxPie3DChartPoints(
	wxString name,
	ChartColor c,
    bool showlabel   
) : wxChartPoints(wxChartPointsTypes::Pie3D()),
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
ChartValue wxPie3DChartPoints::GetXVal(
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
ChartValue wxPie3DChartPoints::GetYVal(
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
wxString wxPie3DChartPoints::GetName(
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
ChartColor wxPie3DChartPoints::GetColor(int n) const
{
	return ( m_Points.GetColor(n) );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetCount()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		int
//----------------------------------------------------------------------E-+++
int wxPie3DChartPoints::GetCount() const
{
	return ( m_Points.GetCount() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMaxX()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		int
//----------------------------------------------------------------------E-+++
ChartValue wxPie3DChartPoints::GetMaxX() const
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
ChartValue wxPie3DChartPoints::GetMaxY() const
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
ChartValue wxPie3DChartPoints::GetMinX() const
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
ChartValue wxPie3DChartPoints::GetMinY() const
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
void wxPie3DChartPoints::SetZoom(
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
double wxPie3DChartPoints::GetZoom()
{
	return ( m_Zoom );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetSizes
//	DESC:		Set sizes for drawing
//	PARAMETERS:	ChartSizes sizes
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxPie3DChartPoints::SetSizes(
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
const ChartSizes& wxPie3DChartPoints::GetSizes() const
{
	return ( m_Sizes );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetColor()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		unsigned long
//----------------------------------------------------------------------E-+++
ChartColor wxPie3DChartPoints::GetColor() const
{
	return ( m_Color );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetColor()
//	DESC:		
//	PARAMETERS:	ChartColor c
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxPie3DChartPoints::SetColor(
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
wxString wxPie3DChartPoints::GetName() const
{
	return ( m_Name );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetName()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxPie3DChartPoints::SetName(
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
void wxPie3DChartPoints::Add(
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
void wxPie3DChartPoints::Add(
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
void wxPie3DChartPoints::SetDisplayTag(
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
wxDISPLAY_LABEL wxPie3DChartPoints::GetDisplayTag() const
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
void wxPie3DChartPoints::Draw(
    CHART_HPAINT hp, 
    CHART_HRECT hr
)
{
    
    //-----------------------------------------------------------------------
    // Get sizes
    //-----------------------------------------------------------------------
    ChartSizes sizes = GetSizes();

    //-----------------------------------------------------------------------
    // Fit Ellisse in window
    //-----------------------------------------------------------------------
    int r = (int)wxMin( (int)hr->w/2, (int)(hr->h - 2*sizes.s_height*ELLISSE_H)/2 );
         
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

            hp->SetPen( *wxBLACK_PEN );

            double percent;
            double grad, grad1;
            double rad;
            int deep;
            int x, y, w, h;

			// Calc Size of Rectangle which hold Ellisse
            w = (int)floor(r * ELLISSE_W);
			h = (int)floor(r * ELLISSE_H);

			// Top corner left hand side
            x = hr->x + hr->w/2 - w/2;
            y = hr->y + hr->h/2 - h;
                        
            // Shadow Deep
            deep = (int)floor( SHADOW_DEEP * GetZoom() );
                        
			//---------------------------------------------------------------
			// Draw shadow part of chart
			//---------------------------------------------------------------      

			hp->DrawEllipticArc( 
				x, 
				y + deep, // Shadow Deep
				w, 
				h, 
				175, // Draw half Ellisse
				360);
			hp->DrawEllipticArc( 
				x, 
				y + deep, // Shadow Deep
				w, 
				h, 
				0, // Draw half Ellisse
				5);

			// left hand side line
			rad = DegToRad( 180 );
			
			hp->DrawLine( 
                EllipsePoint( w, h, x, y, rad ).x,
                EllipsePoint( w, h, x, y, rad ).y,
                EllipsePoint( w, h, x, y, rad ).x,
                EllipsePoint( w, h, x, y + deep, rad ).y + 1
            );

			// right hand side line
			rad = DegToRad( 360 );

            hp->DrawLine( 
                EllipsePoint( w, h, x, y, rad ).x,
                EllipsePoint( w, h, x, y, rad ).y,
                EllipsePoint( w, h, x, y, rad ).x,
                EllipsePoint( w - 180, h, x, y + deep, rad ).y
            );

			grad = 0;
			//int count = 0;
            for ( iData = 0; iData < iDatas; ++ iData )
            {
                hp->SetPen( *wxBLACK_PEN );
                hp->SetBrush( wxBrush(GetColor(iData), wxSOLID) );

                // Calc radiants
                percent = (double)(GetYVal(iData) * 100) / (double)ValTot;
                grad1    = grad + (double)(percent * 360) / (double)100;
                rad     = DegToRad( grad );
                
                hp->DrawEllipticArc( x, y, w, h, grad, grad1);

                //-----------------------------------------------------------
                // Fill the shadow with right color
                //-----------------------------------------------------------
                if ( grad1 > 180 )
                {
					//if (++count > 3)
					//	return;

                    // set colors to draw
                    hp->SetPen( 
                        wxPen(wxChartColors::GetDarkColor(
                            GetColor(iData), 15)) 
                    );
                    hp->SetBrush( 
                        wxBrush(wxChartColors::GetDarkColor(
                            GetColor(iData), 15), 
                        wxSOLID) 
                    );
                    
                    // Avoid redraw line
                    if ( grad1 < 360 )
                    {
                        hp->DrawLine( 
                            EllipsePoint( w, h, x, y, DegToRad( grad1 ) ).x,
                            EllipsePoint( w, h, x, y, DegToRad( grad1 ) ).y - 1,
                            EllipsePoint( w, h, x, y, DegToRad( grad1 ) ).x,
                            EllipsePoint( w, h, x, y + deep, 
                                DegToRad( grad1 ) ).y + 1
                        );
                    }
                    hp->FloodFill(
                        EllipsePoint( w, h, x, y, 
                            DegToRad( grad1 ) ).x - 3, // just inside
                        (int)floor(EllipsePoint( w, h, x, y, 
                            DegToRad( grad1 ) ).y + (double)deep/2), // middle
                        *wxWHITE
                    );
				}
                                
                //-----------------------------------------------------------
                // Only draw Label if user wants it
                //-----------------------------------------------------------
                if (!m_ShowLabel)
                    continue;

#if 1
                wxString lbl; 
                wxLabel wxLbl;
                
                LABEL_POSITION p;
                if ( grad < 90 ||
                    grad > 270 )
                    p = RIGHT;
                else
                    p = LEFT;
                if ( grad  > 180 )
                    p = (LABEL_POSITION)( p | DOWN );
                else
                    p = (LABEL_POSITION)( p | UP );

                switch ( GetDisplayTag() )
                {
                case XVALUE:
                    lbl.Printf( wxT("%d"), static_cast<int>(GetXVal(iData)) );
                    wxLbl.Draw( hp, 
                        EllipsePoint( w, h, x, y, DegToRad( grad ) ).x, 
                        EllipsePoint( w, h, x, y, DegToRad( grad ) ).y, 
                        GetColor(iData), lbl, p );
                    break;
                case YVALUE:
                    lbl.Printf( wxT("%d"), static_cast<int>(GetYVal(iData)) );
                    wxLbl.Draw( hp, 
                        EllipsePoint( w, h, x, y, DegToRad( grad ) ).x, 
                        EllipsePoint( w, h, x, y, DegToRad( grad ) ).y, 
                        GetColor(iData), lbl, p );
                    break;
                case NAME:
                    lbl = GetName(iData).c_str();
                    wxLbl.Draw( hp, 
                        EllipsePoint( w, h, x, y, DegToRad( grad ) ).x, 
                        EllipsePoint( w, h, x, y, DegToRad( grad ) ).y, 
                        GetColor(iData), lbl, p );
                    break;
                default:
                    break;            
                }
#endif
                grad = grad1;

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
wxPie3DChartPoints* wxPie3DChartPoints::CreateWxPie3DChartPoints(
    wxString name,
    ChartColor c,
    bool showlabel
)
{
    if ( c == wxCHART_NOCOLOR )
        c = wxChartColors::GetColor();

    return new wxPie3DChartPoints( name, c, showlabel );
}
