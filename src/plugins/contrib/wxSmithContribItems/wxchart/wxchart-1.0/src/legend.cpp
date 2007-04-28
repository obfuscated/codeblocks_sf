/////////////////////////////////////////////////////////////////////////////
// Name:        legend.cpp
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
#pragma implementation "legend.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wx/chartcolors.h"
#include "wx/legend.h"

//----------------------------------------------------------------------------
// Define Array/List of Points
//----------------------------------------------------------------------------
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ListLegendDesc);


//+++-S-cf-------------------------------------------------------------------
//  NAME:       dtor
//  DESC:       
//  PARAMETERS: None
//  RETURN:     None
//----------------------------------------------------------------------E-+++
wxLegend::wxLegend() 
    : m_Page(0)
{
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       dtor
//  DESC:       
//  PARAMETERS: None
//  RETURN:     None
//----------------------------------------------------------------------E-+++
wxLegend::~wxLegend() 
{
    m_lDescs.Clear();
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Add()
//	DESC:		List descriptos utilities
//	PARAMETERS:	const wxString &lbl, 
//				const ChartColor &col
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxLegend::Add(
	const wxString &lbl, 
	const ChartColor &col
) 
{ 
	m_lDescs.Add( DescLegend(lbl, col) ); 
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Clear()
//	DESC:		List descriptos utilities
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxLegend::Clear() 
{ 
	m_lDescs.Clear(); 
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetCount()
//	DESC:		List descriptos utilities
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
int wxLegend::GetCount() const
{
	return ( m_lDescs.GetCount() ); 
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetColor()
//	DESC:		List descriptos utilities
//	PARAMETERS:	int n
//	RETURN:		None
//----------------------------------------------------------------------E-+++
ChartColor wxLegend::GetColor(
	int n
) const
{
	ChartColor colRes = wxCHART_NOCOLOR;
	
    int iMax = GetCount();
	if ( iMax > n )
	{
        colRes = m_lDescs.Item( n ).m_col;
	}

	return ( colRes );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetLabel()
//	DESC:		List descriptos utilities
//	PARAMETERS:	int n
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxString wxLegend::GetLabel(
	int n
) const
{
    wxString lblRes = wxEmptyString;
	
	int iMax = GetCount();
	if ( iMax > n )
	{
        lblRes = m_lDescs.Item( n ).m_lbl;
           
	}

	return ( lblRes );
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       Draw()
//  DESC:       Draw legend
//  PARAMETERS: CHART_HPAINT hp, 
//              CHART_HRECT hr
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void wxLegend::Draw(
    CHART_HPAINT hp, 
    CHART_HRECT hr
)
{    
    int iPages = NumPages();
    int iLines = iPages > 0 ? ROWS_PAGE : GetCount();
    wxCoord h;
    //h = (ROWS_PAGE * ROW_SIZE < hr->h) ? ROWS_PAGE * ROW_SIZE : hr->h;
    h = (iLines * ROW_SIZE < hr->h) ? iLines * ROW_SIZE : hr->h;

    wxCoord x, y;
    x = (wxCoord)( 5 + hr->x );
    y = (wxCoord)( 5 + hr->y );

    //-----------------------------------------------------------------------
    // draw arrows
    //-----------------------------------------------------------------------
    if ( iPages > 0 )
    {
        hp->SetBrush( *wxGREY_BRUSH );
        hp->SetPen( *wxBLACK_PEN );
        
        DrawArrow( hp, x+hr->w/2, y, 8, ARROW_UP, false );
        hp->DrawLine( x+15, y+10, x+hr->w-15, y+10);
        DrawArrow( hp, x+hr->w/2, y + 20, 8, ARROW_DOWN, false );
    }

    //-----------------------------------------------------------------------
    // draw shadow
    //-----------------------------------------------------------------------
    y += 30;
    hp->SetBrush( *wxGREY_BRUSH );
    hp->SetPen( *wxTRANSPARENT_PEN );
    hp->DrawRectangle( x +5, y +5, hr->w - 10, h );

    //-----------------------------------------------------------------------
    // draw legend window
    //-----------------------------------------------------------------------
    hp->SetBrush( *wxWHITE_BRUSH );
    hp->SetPen( *wxBLACK_PEN );
    hp->DrawRectangle( x, y, hr->w - 10, h );

    //-----------------------------------------------------------------------
    // write labels
    //-----------------------------------------------------------------------
    WriteLabel( hp, x+3, y+3, m_Page );

}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       IncPage()
//  DESC:       Increment  page number
//  PARAMETERS: None
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void wxLegend::IncPage()
{
    int iDatas = GetCount();
    int iPages = NumPages();

    if ( iPages * ROWS_PAGE != iDatas ) 
        iPages += 1;

    if ( m_Page + 1 < iPages )
        m_Page += 1;

}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       DecPage()
//  DESC:       Decrement  page number
//  PARAMETERS: None
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void wxLegend::DecPage()
{
    if ( m_Page > 0 )
        m_Page -= 1;
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       IsInArrowUp()
//  DESC:       Mouse utilities
//  PARAMETERS: int x, 
//              int y
//  RETURN:     None
//----------------------------------------------------------------------E-+++
bool wxLegend::IsInArrowUp(
    int x, 
    int y
)
{
    bool fRes = false;
    if ( abs(x - m_ArrowUp.m_x) <= 4 &&
         abs(y - m_ArrowUp.m_d) <= 4 )
        fRes = true;

    return ( fRes );
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       IsInArrowUp()
//  DESC:       Mouse utilities
//  PARAMETERS: int x, 
//              int y
//  RETURN:     None
//----------------------------------------------------------------------E-+++
bool wxLegend::IsInArrowDown(
    int x, 
    int y
)
{
    bool fRes = false;
    if ( abs(x - m_ArrowDown.m_x) <= 4 &&
         abs(y - m_ArrowDown.m_y) <= 4 )
        fRes = true;

    return ( fRes );
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       DrawArrow()
//  DESC:       Draw arrow
//  PARAMETERS: CHART_HPAINT hp, 
//              int pos,
//              bool over
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void wxLegend::DrawArrow(
    CHART_HPAINT hp, 
    int pos,
    bool over
)
{    
    //-----------------------------------------------------------------------
    // Get actual configuration
    //-----------------------------------------------------------------------
    wxBrush oldBrush = hp->GetBrush();
    wxPen oldPen = hp->GetPen();
    
    //-----------------------------------------------------------------------
    // if mouse over use different colours
    //-----------------------------------------------------------------------
    
    if (over)
    {        
        hp->SetBrush( *wxBLACK_BRUSH );
        hp->SetPen( *wxBLACK_PEN );
    }
    else
    {
        hp->SetBrush( *wxGREY_BRUSH );
        hp->SetPen( *wxBLACK_PEN );
    }
    
    if ( pos == ARROW_DOWN )
    {
        //-------------------------------------------------------------------
        // If mouse position has change than redraw arrow
        //-------------------------------------------------------------------
        if (m_ArrowDown.m_sel != over)
        {
            DrawArrow(hp, m_ArrowDown.m_x, m_ArrowDown.m_y, 8, pos, over);
        }
    }
    if ( pos == ARROW_UP )
    {
        //-------------------------------------------------------------------
        // If mouse position has change than redraw arrow
        //-------------------------------------------------------------------
        if (m_ArrowUp.m_sel != over)
        {
            DrawArrow(hp, m_ArrowUp.m_x, m_ArrowUp.m_y, 8, pos, over);
        }
    }
        
    //-----------------------------------------------------------------------
    // Set old colours
    //-----------------------------------------------------------------------
    hp->SetBrush( oldBrush );
    hp->SetPen( oldPen );
    
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       DrawArrow()
//  DESC:       Draw arrow
//  PARAMETERS: CHART_HPAINT hp, 
//              int x, 
//              int y,
//              int size,
//              int pos,
//              bool over
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void wxLegend::DrawArrow(
    CHART_HPAINT hp, 
    int x, 
    int y,
    int size,
    int pos,
    bool over
)
{    
    if ( pos == ARROW_DOWN )
        size *= (-1);

    wxPoint points[] = { 
        wxPoint( x, y ), 
        wxPoint( x - size, y + size ), 
        wxPoint( x + size, y + size) };
    hp->DrawPolygon( 3, points );

    if ( pos == ARROW_UP )
    {
        m_ArrowUp.m_x = x;
        m_ArrowUp.m_y = y;
        m_ArrowUp.m_d = y + size/2;
        m_ArrowUp.m_sel = over;           
    }
    else
    {
        m_ArrowDown.m_x = x;
        m_ArrowDown.m_y = y;
        m_ArrowDown.m_d = y + size/2;;
        m_ArrowDown.m_sel = over;           
    }
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       WriteLabel()
//  DESC:       Write ROWS_PAGE labels starting from page 
//  PARAMETERS: CHART_HPAINT hp, 
//              int x, 
//              int y, 
//              int page
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void wxLegend::WriteLabel(
    CHART_HPAINT hp, 
    int x, 
    int y, 
    int page
)
{    
    int iDatas = GetCount();

    wxFont font(8, wxROMAN, wxNORMAL, wxNORMAL);
    hp->SetFont(font);
    hp->SetPen( *wxBLACK_PEN );

    wxString label;
    
    for ( int iData = page * ROWS_PAGE; 
          iData < iDatas && iData < (page+1) * ROWS_PAGE; 
          ++ iData )
    {
        hp->SetBrush( wxBrush(GetColor(iData), wxSOLID) );
        hp->SetPen( *wxTRANSPARENT_PEN );
        hp->DrawRectangle( x, y+2, 10, 10 );
        
        hp->SetPen( *wxBLACK_PEN );
        label = GetLabel( iData ).c_str();
        label.Truncate( 5 );
        hp->DrawText( label, x + 15, y );
        
        y += ROW_SIZE;
    }
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       NumPages()
//  DESC:       Get numbe of pages
//  PARAMETERS: None
//  RETURN:     int
//----------------------------------------------------------------------E-+++
int wxLegend::NumPages() const
{
    int iDatas = GetCount();
    if (iDatas > 0)
        iDatas -= 1;
    int iPages = iDatas / ROWS_PAGE;

    return ( iPages );
}
