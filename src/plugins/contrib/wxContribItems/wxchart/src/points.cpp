/////////////////////////////////////////////////////////////////////////////
// Name:        points.cpp
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
// wx
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "points.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/points.h"

//----------------------------------------------------------------------------
// Define Array/List of Points
//----------------------------------------------------------------------------
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ListPoints);

//+++-S-cf-------------------------------------------------------------------
//	NAME:		ctor
//	DESC:		
//	PARAMETERS:	wxString name, 
//				ChartValue x, 
//				ChartValue y,
//				ChartColor c
//	RETURN:		None
//----------------------------------------------------------------------E-+++
Point::Point(
    wxString name, 
	ChartValue x, 
	ChartValue y,
	ChartColor c
):	m_name(name), 
	m_xval(x), 
	m_yval(y),
	m_col(c)
{
}


//+++-S-cf-------------------------------------------------------------------
//	NAME:		ctor
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxPoints::wxPoints()
{
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		dtor
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxPoints::~wxPoints()
{
	Clear();
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Add()
//	DESC:		Add Point to vector (sort by xval)
//	PARAMETERS:	const Point &p
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxPoints::Add(
	const Point &p
)
{
	m_vPoints.Insert( p, GetInsertPosition(p) );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Add()
//	DESC:		Add Point to vector (sort by xval)
//	PARAMETERS:	const wxString s, 
//				const ChartValue x, 
//				const ChartValue y
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxPoints::Add(
	const wxString s, 
	const ChartValue x, 
	const ChartValue y,
	const ChartColor c
)
{
	Point p(s, x, y, c);
	
	Add( p );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Clear()
//	DESC:		remove points from vector
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxPoints::Clear()
{
	m_vPoints.Clear();
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetPoint()
//	DESC:		Get point n-th from vector
//	PARAMETERS:	int n	- point position
//	RETURN:		None
//----------------------------------------------------------------------E-+++
Point wxPoints::GetPoint(
	size_t n
) const
{

    size_t num = m_vPoints.GetCount();
    
    if ( num > n )
        return m_vPoints.Item( n );

    return Point( wxEmptyString, 0, 0 );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetName()
//	DESC:		Get name for point n-th
//	PARAMETERS:	int n
//	RETURN:		string - name
//----------------------------------------------------------------------E-+++
wxString wxPoints::GetName(
	size_t n
) const
{
    size_t num = m_vPoints.GetCount();
    
    if ( num > n )
        return m_vPoints.Item( n ).m_name;
	
    return ( wxEmptyString );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetXVal()
//	DESC:		Get xval for point n-th
//	PARAMETERS:	int n
//	RETURN:		ChartValue - xval
//----------------------------------------------------------------------E-+++
ChartValue wxPoints::GetXVal(
	size_t n
) const
{
    size_t num = m_vPoints.GetCount();
    
    if ( num > n )
        return m_vPoints.Item( n ).m_xval;

	return ( 0 );

}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetYVal()
//	DESC:		Get yval for point n-th
//	PARAMETERS:	int n
//	RETURN:		ChartValue - yval
//----------------------------------------------------------------------E-+++
ChartValue wxPoints::GetYVal(
	size_t n
) const
{
    size_t num = m_vPoints.GetCount();
    
    if ( num > n )
        return m_vPoints.Item( n ).m_yval;

	return ( 0 );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetColor()
//	DESC:		Get color for point n-th
//	PARAMETERS:	int n
//	RETURN:		ChartColor
//----------------------------------------------------------------------E-+++
ChartColor wxPoints::GetColor(
	size_t n
) const
{
    size_t num = m_vPoints.GetCount();
    
    if ( num > n )
        return m_vPoints.Item( n ).m_col;

	return ( 0 );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetCount()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		int
//----------------------------------------------------------------------E-+++
size_t wxPoints::GetCount() const
{
	return m_vPoints.GetCount();
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMaxX()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxPoints::GetMaxX() const
{
	int n = GetCount();

	if ( n > 0 )
		return m_vPoints.Item( n - 1 ).m_xval;

	return 0;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMinX()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxPoints::GetMinX() const
{
	int n = GetCount();

    if ( n > 0 )
        return m_vPoints.Item( 0 ).m_xval;

	return 0;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMaxY()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxPoints::GetMaxY() const
{
    ChartValue iRes = 0;

	for ( size_t iLoop = 0;
		  iLoop < GetCount();
		  ++iLoop )
	{
		if ( GetYVal(iLoop) > iRes )
			iRes = GetYVal( iLoop );
	}

	return ( iRes );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMinY()
//	DESC:		
//	PARAMETERS:	None
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxPoints::GetMinY() const
{
    ChartValue iRes = 0;

	for ( size_t iLoop = 0;
		  iLoop < GetCount();
		  ++iLoop )
	{
		if ( iLoop == 0 )
			iRes = GetYVal(iLoop);
		else
			if ( GetYVal(iLoop) < iRes )
				iRes = GetYVal( iLoop );
	}

	return ( iRes );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetInsertPosition()
//	DESC:		Find the position in the list whre to insert the new point
//				The elements in the list are insert in sort order. based by
//				xval.
//	PARAMETERS:	const Point& p
//	RETURN:		itListPoints&
//----------------------------------------------------------------------E-+++
size_t wxPoints::GetInsertPosition(
	const Point& p
)
{

    size_t num = m_vPoints.GetCount();
    size_t loop;
    
    for ( loop = 0; 
          loop < num; 
          loop++ ) 
	{
		if ( m_vPoints.Item( loop ).m_xval > p.m_xval )
			break;
	}

	return ( loop );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetInsertPosition()
//	DESC:		Find the position in the list whre to insert the new point
//				The elements in the list are insert in sort order. based by
//				xval.
//	PARAMETERS:	const Point& p
//	RETURN:		itListPoints&
//----------------------------------------------------------------------E-+++
// wxPoints::itListPoints wxPoints::GetPosition(
// 	int n
// )
// {
// 	itListPoints itlp;
// 
// 	for ( itlp = m_vPoints.begin();
// 		  itlp != m_vPoints.end() && n > 0;
// 		  ++itlp, --n )
// 		;
// 
// 	return ( itlp );
// }


//+++-S-cf-------------------------------------------------------------------
//	NAME:		Dump()
//	DESC:		Test only
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
// #include <fstream>
// 
// void wxPoints::Dump()
// {
// 	itListPoints itlp;
// 
// 	std::ofstream outf;
// 
// 	outf.open( "dump.txt" );
// 
// 	for ( itlp = m_vPoints.begin();
// 		  itlp != m_vPoints.end();
// 		  ++itlp )
// 	{
// 		outf << "( ";
// 		outf << itlp->m_name.c_str();
// 		outf << ", ";
// 		outf << itlp->m_xval;
// 		outf << ", ";
// 		outf << itlp->m_yval;
// 		outf << " )";
// 		outf << std::endl;
// 	}
// 
// 	outf.close();
// }

