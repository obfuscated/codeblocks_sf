/////////////////////////////////////////////////////////////////////////////
// Name:        chart.cpp
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id: chart.cpp,v 1.1 2006/06/13 12:51:50 pgava Exp $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------
// wx
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "chart.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/chart.h"

//----------------------------------------------------------------------------
// Define Array/List of Points
//----------------------------------------------------------------------------
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ListChartPoints);

//+++-S-cf-------------------------------------------------------------------
//	NAME:		ctor
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxChart::wxChart()
{
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		dtor
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxChart::~wxChart() 
{
	wxChartPoints* cptmp;

    size_t num = m_LCP.GetCount();
    
    for ( size_t loop = 0; 
          loop < num; 
          loop++ ) 
    {
        cptmp = m_LCP.Item(loop);
        delete cptmp;
    }

	m_LCP.Clear();
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Add()
//	DESC:		
//	PARAMETERS:	CChartPoints* cp
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChart::Add(
	wxChartPoints* cp
) 
{
	m_LCP.Add(cp); 
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Clear()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChart::Clear() 
{ 
	m_LCP.Clear();
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMaxX()
//	DESC:		Get the max x-val calculated from all chartpoints
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxChart::GetMaxX() const
{
	ChartValue valTmp, valRes = 0;

    size_t num = m_LCP.GetCount();
    
    for ( size_t loop = 0; 
          loop < num; 
          loop++ ) 
    {
        valTmp = (m_LCP.Item(loop))->GetMaxX();
        if ( valTmp > valRes )
            valRes = valTmp;
    }

    //-----------------------------------------------------------------------
    // if MaxX is 0 could be a Pie chart that doesn't have points like 
    // Bar chart so force this to be something != from 0, otherwise 
    // wont be draw
    //-----------------------------------------------------------------------
    if ( valRes == 0 )
        valRes = 10;
    
	return ( valRes );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMinX()
//	DESC:		Get the min x-val calculated from all chartpoints
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxChart::GetMinX() const
{
	ChartValue valTmp, valRes = 0;

    size_t num = m_LCP.GetCount();
    
    for ( size_t loop = 0; 
          loop < num; 
          loop++ ) 
    {            
        valTmp = (m_LCP.Item(loop))->GetMinX();
        if ( loop == 0 )
            valRes = valTmp;
        else
            if ( valTmp < valRes )
                valRes = valTmp;
    }

	return ( valRes );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetYMax()
//	DESC:		Get the max y-val calculated from all chartpoints
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxChart::GetMaxY() const
{
	ChartValue valTmp, valRes = 0;

    size_t num = m_LCP.GetCount();
    
    for ( size_t loop = 0; 
          loop < num; 
          loop++ ) 
    {                    
        valTmp = (m_LCP.Item(loop))->GetMaxY();
        if ( valTmp > valRes )
            valRes = valTmp;
    }

    //-----------------------------------------------------------------------
    // if MaxY is 0 could be a Pie chart that doesn't have points like 
    // Bar chart so force this to be something != from 0, otherwise 
    // wont be draw
    //-----------------------------------------------------------------------
    if ( valRes == 0 )
        valRes = 10;
    
    return ( valRes );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetYMin()
//	DESC:		Get the min y-val calculated from all chartpoints
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxChart::GetMinY() const
{
	ChartValue valTmp, valRes = 0;

    size_t num = m_LCP.GetCount();
    
    for ( size_t loop = 0; 
          loop < num; 
          loop++ ) 
    {                        
        valTmp = (m_LCP.Item(loop))->GetMinY();
        if ( loop == 0 )
            valRes = valTmp;
        else
            if ( valTmp < valRes )
                valRes = valTmp;
    }
	
	return ( valRes );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetNumBarPoints()
//	DESC:		Number of bar points is used to calculate the width
//				of the chart
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
int wxChart::GetNumBarPoints() const
{
	int valRes = 0;
	
    size_t num = m_LCP.GetCount();
    
    for ( size_t loop = 0; 
          loop < num; 
          loop++ ) 
    {                            
        if ( *(m_LCP.Item(loop)) == wxChartPointsTypes::Bar() )
            valRes += 1;
    }
   
	return ( valRes );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetNumBar3DPoints()
//	DESC:		Number of bar points is used to calculate the width
//				of the chart
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
int wxChart::GetNumBar3DPoints() const
{
	int valRes = 0;

    size_t num = m_LCP.GetCount();
    
    for ( size_t loop = 0; 
          loop < num; 
          loop++ ) 
    {                            
        if ( *(m_LCP.Item(loop)) == wxChartPointsTypes::Bar3D() )
            valRes += 1;
    }

	return ( valRes );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetZoom
//	DESC:		Set zoom
//	PARAMETERS:	double
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChart::SetZoom(
	double z
)
{
    size_t num = m_LCP.GetCount();
    
    for ( size_t loop = 0; 
          loop < num; 
          loop++ ) 
    {
        (m_LCP.Item(loop))->SetZoom( z );
    }
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       GetZoom
//  DESC:       Get zoom
//  PARAMETERS: Noce
//  RETURN:     double
//----------------------------------------------------------------------E-+++
double wxChart::GetZoom() const
{
    size_t num = m_LCP.GetCount();
    
    for ( size_t loop = 0; 
          loop < num; 
        ) 
    {
        return (m_LCP.Item(loop))->GetZoom();
    }
    
    return 1; // default Zoom set to 1
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetSizes
//	DESC:		Set sizes for drawing
//	PARAMETERS:	ChartSizes sizes
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChart::SetSizes(
	ChartSizes sizes
)
{
    size_t num = m_LCP.GetCount();
    
    for ( size_t loop = 0; 
          loop < num; 
          loop++ ) 
    {
        (m_LCP.Item(loop))->SetSizes( sizes );
    }
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       GetSizes
//  DESC:       Get sizes for drawing
//  PARAMETERS: None
//  RETURN:     ChartSizes sizes
//----------------------------------------------------------------------E-+++
ChartSizes wxChart::GetSizes() const
{
    size_t num = m_LCP.GetCount();
    
    for ( size_t loop = 0; 
          loop < num; 
        ) 
    {
        return (m_LCP.Item(loop))->GetSizes();
    }
    
    return ChartSizes();
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Draw()
//	DESC:		
//	PARAMETERS:	CHART_HPAINT hp, 
//				CHART_HRECT hr
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChart::Draw(
	CHART_HPAINT hp, 
	CHART_HRECT hr
) 
{ 
	int iBarCounter = 0;
	int iBar3DCounter = 0;
	int xTmp = hr->x;

    size_t num = m_LCP.GetCount();
    for ( size_t loop = 0; 
          loop < num; 
          loop++ ) 
	{
		//-------------------------------------------------------------------
		// Count the number of Bar-Bar3d charts so when more than one bar is
		// present the first one is draw at position x, the second one at 
		// position x+1*bar_width so just next to the previous one
		//-------------------------------------------------------------------
        
        ChartSizes sizes = m_LCP.Item(loop)->GetSizes();
        hr->x += ( 
            iBarCounter * 
                static_cast<int>(sizes.wbar * m_LCP.Item(loop)->GetZoom()) +
            iBar3DCounter * 
                static_cast<int>(sizes.wbar3d * m_LCP.Item(loop)->GetZoom())
        );
        
		if ( *(m_LCP.Item(loop)) == wxChartPointsTypes::Bar() )
		{			
			iBarCounter += 1;
		}
		else if ( *m_LCP.Item(loop) == wxChartPointsTypes::Bar3D() )
		{
			iBar3DCounter += 1;
		}

		//-------------------------------------------------------------------
		// draw all
		//-------------------------------------------------------------------
		m_LCP.Item(loop)->Draw( hp, hr );
		
		hr->x = xTmp;
	}
}
