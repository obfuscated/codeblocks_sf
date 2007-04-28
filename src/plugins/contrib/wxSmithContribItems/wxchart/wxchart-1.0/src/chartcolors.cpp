/////////////////////////////////////////////////////////////////////////////
// Name:        chartcolors.cpp
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
#include "wx/chartcolors.h"

ChartColor wxChartColors::vColors[] = {
		wxCHART_RED,
		wxCHART_GREEN,
		wxCHART_BLUE,
        wxCHART_YELOW, 
        wxCHART_CYAN,
		wxCHART_LRED,
		wxCHART_LGREEN,
		wxCHART_LBLUE,
		wxCHART_LYELOW,
		wxCHART_LCYAN,
        wxCHART_ALICEBLUE,
        wxCHART_ANTIQUEWHITE,
        wxCHART_AQUAMARINE,
        wxCHART_BLUEVIOLET,
        wxCHART_BROWN,
        wxCHART_TURQUOISE,
        wxCHART_SEAGREEN,
        wxCHART_GOLD,
        wxCHART_SIENNA,
        wxCHART_GOLDENROD,
        wxCHART_ROYALBLUE,
        wxCHART_GRAY,
        wxCHART_SLATEGRAY
};

size_t wxChartColors::colPos = 0;

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetColor()
//	DESC:		Get a color from the list
//	PARAMETERS:	None
//	RETURN:		ChartColor
//----------------------------------------------------------------------E-+++
ChartColor wxChartColors::GetColor()
{
	if ( colPos >= sizeof(vColors)/sizeof(ChartColor) )
		colPos = 0;

	return static_cast<ChartColor>( vColors[colPos++] );
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       GetDarkColor()
//  DESC:       
//  PARAMETERS: 
//      int step
//      ChartColor c
//  RETURN:     unsigned long
//----------------------------------------------------------------------E-+++
ChartColor wxChartColors::GetDarkColor(
    ChartColor c,
    int step
)
{
    int r =  c & 0x0000FF;
    int g =  (c & 0x00FF00) >> 8;
    int b =  (c & 0xFF0000) >> 16;
    
    //-----------------------------------------------------------------------
    // Decrease RGB by step%
    //-----------------------------------------------------------------------
    
    r -= r * step/100;
    if ( r < 0 )
        r = 0;
        
    b -= b * step/100;
    if ( b < 0 )
        b = 0;
        
    g -= g * step/100;
    if ( g < 0 )
        g = 0;
    
    return ( r | (g << 8) | (b << 16) );
}

