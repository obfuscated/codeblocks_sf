/////////////////////////////////////////////////////////////////////////////
// Name:        charttypes.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __CHARTTYPES_H__ )
#define __CHARTTYPES_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/string.h>
    #include <wx/dc.h>
#endif

#include "wx/chartdef.h"


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

//----------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------

// Area to be drawn
typedef struct
{
	int x;
	int xscroll;
	int y;
	int yscroll;
	int w;
	int h;
} CHART_RECT, *CHART_HRECT;

// Device Context
typedef wxDC* CHART_HPAINT;

// Chart value and Color
typedef double ChartValue;
typedef unsigned long ChartColor;

// Keep track of the size of the charts
struct ChartSizes
{
	ChartSizes() : nbar(0), nbar3d(0),
				   wbar(DEFAULT_BAR_WIDTH), wbar3d(DEFAULT_BAR3D_WIDTH),
				   gap(DEFAULT_GAP_WIDTH), scroll(DEFAULT_SCROLL_STEP),
				   s_height(DEFAULT_SIZE_HEIGHT) {};
	int nbar;
	int nbar3d;
	int wbar;
	int wbar3d;
	int gap;
	int scroll;
	int s_height;
};

#endif // __CHARTTYPES_H__
