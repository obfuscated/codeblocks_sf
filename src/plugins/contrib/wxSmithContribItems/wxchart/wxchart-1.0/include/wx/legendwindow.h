/////////////////////////////////////////////////////////////////////////////
// Name:        legendwindow.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __WXLEGENDWINDOW_H__ )
#define __WXLEGENDWINDOW_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

#include <wx/window.h>

#include "wx/legend.h"

//----------------------------------------------------------------------------
// Consts
//----------------------------------------------------------------------------
const int LEGEND_WIDTH	= 70;	// width in pixels
const int LEGEND_HEIGHT	= 60;	// height in pixels

//+++-S-cd-------------------------------------------------------------------
//	NAME:		wxLegendWindow
//	DESC:
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
class WXDLLIMPEXP_CHART wxLegendWindow : public wxWindow
{
public:
	wxLegendWindow() {};	// for IMPLEMENT_DYNAMIC_CLASS
	wxLegendWindow(wxWindow *parent);

	// List descriptos utilities
	// allow access to CWxLegend functionality
	//----------------------------------------
    void Add(const wxString &lbl, const ChartColor &col);
	void Clear();
	int GetCount() const;
	ChartColor GetColor(int n) const;
	wxString GetLabel(int n) const;

	// Draw legend window
	//-------------------
	void Draw(CHART_HPAINT hp, int x = 0, int y = 0);

private:
	wxLegend m_Legend;
	wxWindow *m_WinParent;

	// events
	//-------
    void OnPaint(wxPaintEvent &event);
    void OnMouse(wxMouseEvent &event);
    void OnMouseMove(wxMouseEvent &event);

    DECLARE_CLASS(wxLegendWindow)
    DECLARE_EVENT_TABLE()
};

#endif // __WXLEGENDWINDOW_H__
