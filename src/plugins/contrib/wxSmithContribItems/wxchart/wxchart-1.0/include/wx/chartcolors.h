/////////////////////////////////////////////////////////////////////////////
// Name:        chartcolors.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __CHARTCOLORS_H__ )
#define __CHARTCOLORS_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------
#include "wx/charttypes.h"

//----------------------------------------------------------------------------
// Consts
//----------------------------------------------------------------------------
enum {
    wxCHART_NOCOLOR      = 0x00UL,
    wxCHART_BLACK        = 0x000000UL,
    wxCHART_RED          = 0x0000FFUL,
    wxCHART_GREEN        = 0x00FF00UL,
    wxCHART_BLUE         = 0xFF0000UL,
    wxCHART_LRED         = 0x0066FFUL,
    wxCHART_LGREEN       = 0x99FF00UL,
    wxCHART_LBLUE        = 0xFFCC33UL,
    wxCHART_YELOW        = 0x00FFFFUL,
    wxCHART_LYELOW       = 0x80FFFFUL,
    wxCHART_CYAN         = 0xFFFF00UL,
    wxCHART_LCYAN        = 0xFFFFA0UL,
    wxCHART_ALICEBLUE    = 0xFFC8C0UL,
    wxCHART_ANTIQUEWHITE = 0xD7EBFFUL,
    wxCHART_AQUAMARINE   = 0xDFFF7FUL,   
    wxCHART_BLUEVIOLET   = 0xE22B8AUL,
    wxCHART_BROWN        = 0x2A2AA5UL,
    wxCHART_TURQUOISE    = 0xDBC643UL,
    wxCHART_SEAGREEN     = 0x75894EUL,
    wxCHART_GOLD         = 0x17A0D4UL,
    wxCHART_SIENNA       = 0x3174F8UL,
    wxCHART_GOLDENROD    = 0x74DAEDUL,
    wxCHART_ROYALBLUE    = 0xDE602BUL,
    wxCHART_GRAY         = 0x6E6F73UL,
    wxCHART_SLATEGRAY    = 0x837365UL   
};

//+++-S-cd-------------------------------------------------------------------
//	NAME:		wxChartColor
//	DESC:		
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
WXDLLIMPEXP_CHART class wxChartColors
{

public:
	static ChartColor GetColor();
    
    // Get shadow Color for 3D part of chart
    //--------------------------------------
    static ChartColor GetDarkColor(ChartColor c, int step);   

private:
	static ChartColor vColors[];
	static size_t colPos;
};


#endif // __CHARTCOLOR_H__
