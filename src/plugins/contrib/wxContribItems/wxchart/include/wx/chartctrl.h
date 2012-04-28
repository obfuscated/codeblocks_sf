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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "chartctrl.h"
#endif

#include <wx/scrolwin.h>

#include "wx/chartwindow.h"
#include "wx/legendwindow.h"
#include "wx/xaxiswindow.h"
#include "wx/yaxiswindow.h"

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

// ChartCtrl styles
//-----------------

enum wxChartStyle
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

// chart image type
//-----------------

enum wxChartImageType
{
    wxCHART_BMP = wxBITMAP_TYPE_BMP,
    wxCHART_GIF = wxBITMAP_TYPE_GIF,
    wxCHART_PNG = wxBITMAP_TYPE_PNG,
    wxCHART_JPEG = wxBITMAP_TYPE_JPEG
};

// Extern classes
//---------------

class wxChartSizes;

//+++-S-cd-------------------------------------------------------------------
//	NAME:		wxChartCtrl
//	DESC:
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
/* C::B begin */
class WXDLLIMPEXP_CHART wxChartCtrl : public wxScrolledWindow
/* C::B end */
{
public:

	wxChartCtrl() {};	// for IMPLEMENT_DYNAMIC_CLASS
    wxChartCtrl(wxWindow *parent, wxWindowID id,
				 wxChartStyle style = DEFAULT_STYLE,
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
    void Fit();

	// 3D depth utility
	//-----------------
	void DepthIn();
	void DepthOut();

	// Write chart to file
	//--------------------
	void WriteToFile(wxString file, wxChartImageType type);

private:
	double m_xZoom;
	wxChartStyle m_Style;
	wxChartWindow *m_ChartWin;
	wxLegendWindow *m_LegendWin;
	wxXAxisWindow *m_XAxisWin;
	wxYAxisWindow *m_YAxisWin;
    wxChartSizes *m_Sizes;

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

    // Set Image Type
    void LoadImageHandler(wxChartImageType type);

	// scroll bar utility
	//-------------------
	void ResetScrollbar();
	void RedrawXAxis();
	void RedrawYAxis();
    void RedrawLegend();
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
