/////////////////////////////////////////////////////////////////////////////
// Name:        chartctrl.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __CHARTCTRL_H__ )
#define __CHARTCTRL_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

#include <wx/scrolwin.h>

#include "chartwindow.h"
#include "legendwindow.h"
#include "xaxiswindow.h"
#include "yaxiswindow.h"

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

// ChartCtrl styles
//-----------------
enum STYLE
{
    USE_NONE        = 0,
    USE_AXIS_X      = 0x01,
    USE_AXIS_Y      = 0x02,
    USE_LEGEND      = 0x04,
    USE_ZOOM_BUT    = 0x08,
    USE_DEPTH_BUT   = 0x10,
    USE_GRID        = 0x20,
    DEFAULT_STYLE   = USE_AXIS_X | USE_AXIS_Y | USE_LEGEND |
                        USE_ZOOM_BUT | USE_DEPTH_BUT | USE_GRID
};

//+++-S-cd-------------------------------------------------------------------
//	NAME:		wxChartCtrl
//	DESC:
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
class WXDLLIMPEXP_CHART wxChartCtrl : public wxScrolledWindow
{
public:

	wxChartCtrl() {};	// for IMPLEMENT_DYNAMIC_CLASS
    wxChartCtrl(wxWindow *parent, wxWindowID id,
				 STYLE style = DEFAULT_STYLE,
				 const wxPoint &pos = wxDefaultPosition,
			     const wxSize &size = wxDefaultSize,
				 int flags = wxSIMPLE_BORDER);

	~wxChartCtrl();

	// allow use of m_ChartWin's public methods
	// Add() - Clear()
	//-----------------------------------------
	void Add(wxChartPoints* cp);
	void Clear();

	// Zoom utility
	//-------------
	void ZoomIn();
	void ZoomOut();
	void Resize();

	// 3D depth utility
	//-----------------
	void DepthIn();
	void DepthOut();

	// Write chart to file
	//--------------------
	void WriteToFile(wxString file);

private:
	double m_xZoom;
	STYLE m_Style;
	ChartSizes m_Sizes;
	wxChartWindow	*m_ChartWin;
	wxLegendWindow	*m_LegendWin;
	wxXAxisWindow	*m_XAxisWin;
	wxYAxisWindow	*m_YAxisWin;

	// Calculate width
	//----------------
	int CalWidth(int n, int nbar, int nbar3d,
				 int wbar, int wbar3d, int gap);

	// Zoom utility
	//-------------
	void SetZoom(double zoom);

	// Size utilities
	//---------------
	void SetSizes();

	// scroll bar utility
	//-------------------
	void ResetScrollbar();
	void RedrawXAxis();
	void RedrawYAxis();
	void RedrawEverything();

    // events
    // ------
    void OnZoomIn(wxCommandEvent& event);
    void OnZoomOut(wxCommandEvent& event);
    void OnScroll(wxScrollWinEvent& event);
    void OnSize(wxSizeEvent& event);

	// bitmap buttons utility
	//-----------------------
	wxBitmap GetZoomInBitmap();
	wxBitmap GetZoomOutBitmap();

    DECLARE_CLASS(wxChartCtrl)
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// Inlines
//----------------------------------------------------------------------------



#endif // __CHARTCTRL_H__
