/////////////////////////////////////////////////////////////////////////////
// Name:        legend.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id: legend.h,v 1.1 2006/06/13 12:51:50 pgava Exp $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __LEGEND_H__ )
#define __LEGEND_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "legend.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/dynarray.h>
#endif

#include "wx/charttypes.h"

//----------------------------------------------------------------------------
// Conts
//----------------------------------------------------------------------------
#define    ARROW_UP     0
#define    ARROW_DOWN   1
#define    ROWS_PAGE    3
#define    ROW_SIZE     20 // pixels

//----------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------

// Legend descriptor
//------------------
struct DescLegend
{
    wxString m_lbl;
    ChartColor m_col;
    DescLegend(wxString lbl, ChartColor col) : m_lbl(lbl), m_col(col) {};
};

// Arrow Up/Down descriptor
//-------------------------
struct ArrowDescriptor
{
    int m_x;
    int m_y;
    int m_d;
    bool m_sel;       
    ArrowDescriptor() : m_x(0), m_y(0), m_d(0), m_sel(false) {};
};

//----------------------------------------------------------------------------
// Declare Array/List of Legend Descriptor
//----------------------------------------------------------------------------
WX_DECLARE_OBJARRAY(DescLegend, ListLegendDesc);

//+++-S-cd-------------------------------------------------------------------
//	NAME:		wxLegend
//	DESC:		
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
WXDLLIMPEXP_CHART class wxLegend
{
public:
    
    // ctor
    //-----
    wxLegend();
	~wxLegend();

	// Draw legend area
	//-----------------
	void Draw(CHART_HPAINT hp, CHART_HRECT hr);

    // Redraw arraw whne mouse over
    //-----------------------------
    void DrawArrow(CHART_HPAINT hp, int pos, bool over);
    
    // Dec/Inc page
    //-------------
    void IncPage();
    void DecPage();

    // Mouse utilities
    bool IsInArrowUp(int x, int y);
    bool IsInArrowDown(int x, int y);

	// List descriptos utilities
	//--------------------------
    void Add(const wxString &lbl, const ChartColor &col);
	void Clear();
	int GetCount() const;
	ChartColor GetColor(int n) const;
    wxString GetLabel(int n) const;

private:
	ListLegendDesc m_lDescs;
    int m_Page;
    ArrowDescriptor m_ArrowUp;
    ArrowDescriptor m_ArrowDown;

    // Drawing Utilities
    //------------------
    void DrawArrow(CHART_HPAINT hp, int x, int y, int size, 
                   int pos, bool over);
    void WriteLabel(CHART_HPAINT hp, int x, int y, int page);

    // Get number of pages
    //--------------------
    int NumPages() const;
   
};

#endif // __LEGEND_H__
