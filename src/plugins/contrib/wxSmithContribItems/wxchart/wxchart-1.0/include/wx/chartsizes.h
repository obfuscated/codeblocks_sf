/////////////////////////////////////////////////////////////////////////////
// Name:        chartsizes.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __CHARTSIZES_H__ )
#define __CHARTSIZES_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "chartsizes.h"
#endif

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------
#include "wx/charttypes.h"

//----------------------------------------------------------------------------
// consts
//----------------------------------------------------------------------------
const int DEFAULT_BAR_WIDTH			= 20;		// pixels
const int DEFAULT_BAR3D_WIDTH		= 25;		// pixels
const int DEFAULT_GAP_WIDTH			= 20;		// pixels
const int DEFAULT_SCROLL_STEP		= 30;		// pixels
const int DEFAULT_SIZE_HEIGHT		= 40;		// pixels

const int MIN_BAR_WIDTH				= 3;		// pixels
const int MIN_BAR3D_WIDTH			= 5;		// pixels
const int MIN_GAP_WIDTH				= 3;		// pixels

//+++-S-cd-------------------------------------------------------------------
//  NAME:       wxChartSizes
//  DESC:       
//  INTERFACE:
//
//----------------------------------------------------------------------E-+++
class WXDLLIMPEXP_CHART wxChartSizes
{
private:
    int m_numBar;
    int m_numBar3d;
    int m_widthBar;
    int m_widthBar3d;
    int m_gap;
    int m_scroll;
    int m_sizeHeight;
    ChartValue m_maxY;
    ChartValue m_minY;
    ChartValue m_maxX;
    ChartValue m_minX;
    ChartValue m_xZoom;
    
public:
    
    // Inline Ctor
    //------------
    wxChartSizes() : 
        m_numBar(0), 
        m_numBar3d(0), 
        m_widthBar(DEFAULT_BAR_WIDTH), 
        m_widthBar3d(DEFAULT_BAR3D_WIDTH),
        m_gap(DEFAULT_GAP_WIDTH), 
        m_scroll(DEFAULT_SCROLL_STEP),
        m_sizeHeight(DEFAULT_SIZE_HEIGHT),
        m_maxY(0),
        m_minY(0),
        m_maxX(0),
        m_minX(0),
        m_xZoom(1)
    {}; // for singleton
    
    //------------------------------------------------------------------------
    // inlines: access properties
    //------------------------------------------------------------------------
    inline int GetNumBar();
    inline void SetNumBar(int n);
    inline int GetNumBar3d();
    inline void SetNumBar3d(int n);
    inline int GetWidthBar();
    inline void SetWidthBar(int n);
    inline int GetWidthBar3d();
    inline void SetWidthBar3d(int n);
    inline int GetGap();
    inline void SetGap(int n);
    inline int GetScroll();
    inline void SetScroll(int n);
    inline int GetSizeHeight();
    inline void SetSizeHeight(int n);
    inline ChartValue GetMaxY();
    inline void SetMaxY(ChartValue n);
    inline ChartValue GetMinY();
    inline void SetMinY(ChartValue n);
    inline ChartValue GetMaxX();
    inline void SetMaxX(ChartValue n);
    inline ChartValue GetMinX();
    inline void SetMinX(ChartValue n);
    inline ChartValue GetXZoom();
    inline void SetXZoom(ChartValue n);
    
};

//----------------------------------------------------------------------------
// Inline
//----------------------------------------------------------------------------


//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetNumBar()
//	DESC:		
//	PARAMETERS:	none
//	RETURN:		int
//----------------------------------------------------------------------E-+++
int wxChartSizes::GetNumBar()
{
    return m_numBar;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetNumBar()
//	DESC:		
//	PARAMETERS:	n : new value
//	RETURN:		none
//----------------------------------------------------------------------E-+++
void wxChartSizes::SetNumBar(int n)
{
    m_numBar = n;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetNumBar3d()
//	DESC:		
//	PARAMETERS:	none
//	RETURN:		int
//----------------------------------------------------------------------E-+++
int wxChartSizes::GetNumBar3d()
{
    return m_numBar3d;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetNumBar3d()
//	DESC:		
//	PARAMETERS:	n : new value
//	RETURN:		none
//----------------------------------------------------------------------E-+++
void wxChartSizes::SetNumBar3d(int n)
{
    m_numBar3d = n;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetWidthBar()
//	DESC:		
//	PARAMETERS:	none
//	RETURN:		int
//----------------------------------------------------------------------E-+++
int wxChartSizes::GetWidthBar()
{
    return m_widthBar;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetWidthBar()
//	DESC:		
//	PARAMETERS:	n : new value
//	RETURN:		none
//----------------------------------------------------------------------E-+++
void wxChartSizes::SetWidthBar(int n)
{
    m_widthBar = n;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetWidthBar3d()
//	DESC:		
//	PARAMETERS:	none
//	RETURN:		int
//----------------------------------------------------------------------E-+++
int wxChartSizes::GetWidthBar3d()
{
    return m_widthBar3d;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetWidthBar3d()
//	DESC:		
//	PARAMETERS:	n : new value
//	RETURN:		none
//----------------------------------------------------------------------E-+++
void wxChartSizes::SetWidthBar3d(int n)
{
    m_widthBar3d = n;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetGap()
//	DESC:		
//	PARAMETERS:	none
//	RETURN:		int
//----------------------------------------------------------------------E-+++
int wxChartSizes::GetGap()
{
    return m_gap;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetGap()
//	DESC:		
//	PARAMETERS:	n : new value
//	RETURN:		none
//----------------------------------------------------------------------E-+++
void wxChartSizes::SetGap(int n)
{
    m_gap = n;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetScroll()
//	DESC:		
//	PARAMETERS:	none
//	RETURN:		int
//----------------------------------------------------------------------E-+++
int wxChartSizes::GetScroll()
{
    return m_scroll;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetScroll()
//	DESC:		
//	PARAMETERS:	n : new value
//	RETURN:		none
//----------------------------------------------------------------------E-+++
void wxChartSizes::SetScroll(int n)
{
    m_scroll = n;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetSizeHeight()
//	DESC:		
//	PARAMETERS:	none
//	RETURN:		int
//----------------------------------------------------------------------E-+++
int wxChartSizes::GetSizeHeight()
{
    return m_sizeHeight;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetSizeHeight()
//	DESC:		
//	PARAMETERS:	n : new value
//	RETURN:		none
//----------------------------------------------------------------------E-+++
void wxChartSizes::SetSizeHeight(int n)
{
    m_sizeHeight = n;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMaxY()
//	DESC:		
//	PARAMETERS:	none
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxChartSizes::GetMaxY()
{
    return m_maxY;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetMaxY()
//	DESC:		
//	PARAMETERS:	n : new value
//	RETURN:		none
//----------------------------------------------------------------------E-+++
void wxChartSizes::SetMaxY(ChartValue n)
{
    m_maxY = n;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMinY()
//	DESC:		
//	PARAMETERS:	none
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxChartSizes::GetMinY()
{
    return m_minY;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetMinY()
//	DESC:		
//	PARAMETERS:	n : new value
//	RETURN:		none
//----------------------------------------------------------------------E-+++
void wxChartSizes::SetMinY(ChartValue n)
{
    m_minY = n;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMaxX()
//	DESC:		
//	PARAMETERS:	none
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxChartSizes::GetMaxX()
{
    return m_maxX;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetMaxX()
//	DESC:		
//	PARAMETERS:	n : new value
//	RETURN:		none
//----------------------------------------------------------------------E-+++
void wxChartSizes::SetMaxX(ChartValue n)
{
    m_maxX = n;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetMinX()
//	DESC:		
//	PARAMETERS:	none
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxChartSizes::GetMinX()
{
    return m_minX;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetMinX()
//	DESC:		
//	PARAMETERS:	n : new value
//	RETURN:		none
//----------------------------------------------------------------------E-+++
void wxChartSizes::SetMinX(ChartValue n)
{
    m_minX = n;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetXZoom()
//	DESC:		
//	PARAMETERS:	none
//	RETURN:		ChartValue
//----------------------------------------------------------------------E-+++
ChartValue wxChartSizes::GetXZoom()
{
    return m_xZoom;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetXZoom()
//	DESC:		
//	PARAMETERS:	n : new value
//	RETURN:		none
//----------------------------------------------------------------------E-+++
void wxChartSizes::SetXZoom(ChartValue n)
{
    m_xZoom = n;
}

#endif // __CHARTSIZES_H__
