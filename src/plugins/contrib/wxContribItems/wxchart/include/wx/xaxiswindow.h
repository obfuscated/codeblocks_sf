/////////////////////////////////////////////////////////////////////////////
// Name:        xaxiswindow.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __WXXAXISWINDOW_H__ )
#define __WXXAXISWINDOW_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "xaxiswindow.h"
#endif

#include <wx/window.h>

#include "wx/xaxis.h"

// External Classes
//-----------------
class wxChartSizes;

//----------------------------------------------------------------------------
// Consts
//----------------------------------------------------------------------------
const int XAXIS_WIDTH	= 30;	// width in pixels
const int XAXIS_HEIGHT	= 60;	// height in pixels

//+++-S-cd-------------------------------------------------------------------
//	NAME:		wxXAxisWindow
//	DESC:
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
/* C::B begin */
class WXDLLIMPEXP_CHART wxXAxisWindow : public wxWindow
/* C::B end */
{
public:
	wxXAxisWindow() {};	// for IMPLEMENT_DYNAMIC_CLASS
	wxXAxisWindow(wxScrolledWindow *parent);

	// access CWxXAxis's Get/Set virtual size
	//---------------------------------------
	void SetVirtualMax(ChartValue v);
	void SetVirtualMin(ChartValue v);
	ChartValue GetVirtualMax() const;
	ChartValue GetVirtualMin() const;

	// access CWxXAxis's Get/Set zoom
	//---------------------------------------
	void SetZoom(double z);
	double GetZoom();
	void SetSizes(wxChartSizes *sizes);
	const wxChartSizes* GetSizes() const;

	// Draw y-axis window
	//-------------------
	void Draw(CHART_HPAINT hp, int x = 0, int y = 0);

private:
	wxXAxis m_XAxis;
	wxScrolledWindow *m_WinParent;

	// events
	//-------
    void OnPaint(wxPaintEvent &event);
    void OnMouse(wxMouseEvent &event);

    DECLARE_CLASS(wxXAxisWindow)
    DECLARE_EVENT_TABLE()
};

#endif // __WXXAXISWINDOW_H__
