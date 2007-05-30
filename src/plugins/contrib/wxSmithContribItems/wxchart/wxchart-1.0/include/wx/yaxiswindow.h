/////////////////////////////////////////////////////////////////////////////
// Name:        yaxiswindow.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __WXYAXISWINDOW_H__ )
#define __WXYAXISWINDOW_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/window.h>
#endif


#include "wx/yaxis.h"

//----------------------------------------------------------------------------
// Consts
//----------------------------------------------------------------------------
const int YAXIS_WIDTH	= 60;	// width in pixels
const int YAXIS_HEIGHT	= 30;	// height in pixels

//+++-S-cd-------------------------------------------------------------------
//	NAME:		wxYAxisWindow
//	DESC:
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
WXDLLIMPEXP_CHART class wxYAxisWindow : public wxWindow
{
public:
	wxYAxisWindow() {};	// for IMPLEMENT_DYNAMIC_CLASS
	wxYAxisWindow(wxScrolledWindow *parent,
                   ChartValue max = 0, ChartValue min = 0);

	// access CWxYAxis's Get/Set virtual size
	//---------------------------------------
	void SetVirtualMax(ChartValue v);
	void SetVirtualMin(ChartValue v);
	ChartValue GetVirtualMax() const;
	ChartValue GetVirtualMin() const;
	void SetSizes(ChartSizes sizes);
	const ChartSizes& GetSizes() const;

	// access CWxYAxis's Get/Set zoom
	//---------------------------------------
	void SetZoom(double z);
	double GetZoom();

	// Draw y-axis window
	//-------------------
	void Draw(CHART_HPAINT hp, int x = 0, int y = 0);

private:
	wxScrolledWindow *m_WinParent;
    wxYAxis m_YAxis;

	// events
	//-------
    void OnPaint(wxPaintEvent &event);
    void OnMouse(wxMouseEvent &event);

    DECLARE_CLASS(wxYAxisWindow)
    DECLARE_EVENT_TABLE()
};

#endif // __WXYAXISWINDOW_H__
