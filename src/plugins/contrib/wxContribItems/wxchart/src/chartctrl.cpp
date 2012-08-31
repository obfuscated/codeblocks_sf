/////////////////////////////////////////////////////////////////////////////
// Name:        chartctrl.cpp
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

// c++
#include <cmath>

// wx
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "chartctrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include <wx/image.h>

#include "wx/chartctrl.h"
#include "wx/chartsizes.h"

// ----------------------------------------------------------------------------
// XPMs
// ----------------------------------------------------------------------------

#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "wx/chartart/chart_zin.xpm"
    #include "wx/chartart/chart_zot.xpm"
#endif

// ----------------------------------------------------------------------------
// Consts
// ----------------------------------------------------------------------------
const int ID_ZOOM_IN   =    1500;
const int ID_ZOOM_OUT  =    1501;

// ----------------------------------------------------------------------------
// wxChartCtrl event table
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxChartCtrl, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxChartCtrl, wxScrolledWindow)
  EVT_BUTTON(ID_ZOOM_IN, wxChartCtrl::OnZoomIn)
  EVT_BUTTON(ID_ZOOM_OUT, wxChartCtrl::OnZoomOut)
  EVT_SCROLLWIN(wxChartCtrl::OnScroll)
  EVT_SIZE(wxChartCtrl::OnSize)
END_EVENT_TABLE()

//+++-S-cf-------------------------------------------------------------------
//	NAME:		ctor
//	DESC:
//	PARAMETERS:	wxWindow *parent,
//				wxWindowID id,
//				wxChartStyle style,
//				const wxPoint &pos,
//				const wxSize &size,
//				int flags
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxChartCtrl::wxChartCtrl(
	wxWindow *parent,
	wxWindowID id,
	wxChartStyle style,
	const wxPoint &pos,
	const wxSize &size,
	int flags
) : wxScrolledWindow(parent, id, pos, size, flags),
	m_xZoom(1),
	m_Style(style),
	m_ChartWin(0),
	m_LegendWin(0),
	m_XAxisWin(0),
	m_YAxisWin(0)
{

	//-----------------------------------------------------------------------
	// Create window chart, the only always present
	//-----------------------------------------------------------------------
	m_ChartWin = new wxChartWindow( this, (m_Style & USE_GRID) != 0 );

	//-----------------------------------------------------------------------
	// Create main sizer and set min size
	//-----------------------------------------------------------------------
	wxBoxSizer *MainSizer = new wxBoxSizer( wxHORIZONTAL );
	MainSizer->SetMinSize( size.GetWidth(), size.GetHeight() );

	//-----------------------------------------------------------------------
	// Create all the other sizers.
	//-----------------------------------------------------------------------
	wxBoxSizer *VerSizer = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer *Hor1Sizer = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer *Hor2Sizer = new wxBoxSizer( wxHORIZONTAL );

	//-----------------------------------------------------------------------
	// Add y-axis & chart window
	//-----------------------------------------------------------------------
	if ( m_Style & USE_AXIS_Y )
	{
		m_YAxisWin = new wxYAxisWindow( this );
		Hor1Sizer->Add( m_YAxisWin, 0, wxEXPAND );
	}
	Hor1Sizer->Add( m_ChartWin, 1, wxEXPAND );

	//-----------------------------------------------------------------------
	// Add previuos to vertical sizer and add x-axis
	//-----------------------------------------------------------------------
	VerSizer->Add( Hor1Sizer, 1, wxEXPAND );
	if ( m_Style & USE_AXIS_X )
	{
		wxBoxSizer *Hor3Sizer = new wxBoxSizer( wxHORIZONTAL );
		if ( m_YAxisWin )
			Hor3Sizer->Add( YAXIS_WIDTH, XAXIS_HEIGHT );

		m_XAxisWin = new wxXAxisWindow( this );
		Hor3Sizer->Add( m_XAxisWin, 1, wxEXPAND );
		VerSizer->Add( Hor3Sizer, 0, wxEXPAND );
	}

	//-----------------------------------------------------------------------
	// Now add buttons to horizontal sizer
	//-----------------------------------------------------------------------
	if ( m_Style & (USE_ZOOM_BUT | USE_DEPTH_BUT) )
	{
        wxBoxSizer *buttonlist = new wxBoxSizer( wxVERTICAL );

		if ( m_Style & USE_ZOOM_BUT )
		{
			buttonlist->Add( new wxBitmapButton( this,
												 ID_ZOOM_IN,
												 GetZoomInBitmap() ),
												 0, wxEXPAND|wxALL, 2 );
			buttonlist->Add( new wxBitmapButton( this,
												 ID_ZOOM_OUT,
												 GetZoomOutBitmap() ),
												 0, wxEXPAND|wxALL, 2 );
		}
#if 0
		if ( m_Style & USE_DEPTH_BUT )
		{
			buttonlist->Add( new wxBitmapButton( this,
												 ID_ZOOM_IN,
												 *GetZoomInBitmap() ),
							 0, wxEXPAND|wxALL, 2 );
			buttonlist->Add( new wxBitmapButton( this,
												 ID_ZOOM_OUT,
												 *GetZoomOutBitmap() ),
							 0, wxEXPAND|wxALL, 2 );
		}

#endif
		Hor2Sizer->Add( buttonlist, 0, wxEXPAND|wxALL, 4 );
	}

	//-----------------------------------------------------------------------
	// ...and all the other things plus legend window
	//-----------------------------------------------------------------------
	Hor2Sizer->Add( VerSizer, 1, wxEXPAND );
	if ( m_Style & USE_LEGEND )
	{
		m_LegendWin = new wxLegendWindow( this );
		Hor2Sizer->Add( m_LegendWin, 0, wxEXPAND );
	}

	//-----------------------------------------------------------------------
	// finally MainSizer hold all the windows
	//-----------------------------------------------------------------------
	MainSizer->Add( Hor2Sizer, 1, wxEXPAND );

   	//-----------------------------------------------------------------------
	// Instantiate the size object.
    // We have to make sure that we have one object per Control
	//-----------------------------------------------------------------------
    m_Sizes = new wxChartSizes();

	//-----------------------------------------------------------------------
	// use the sizer for layout
	//-----------------------------------------------------------------------
    SetAutoLayout( TRUE );
	SetSizer( MainSizer );
    SetScrollRate( m_Sizes->GetScroll(), m_Sizes->GetScroll() );

	MainSizer->Fit( this );

	//-----------------------------------------------------------------------
	// set size hints to honour minimum size
	//-----------------------------------------------------------------------
	MainSizer->SetSizeHints( this );

	//-----------------------------------------------------------------------
	// set target win to scroll
	//-----------------------------------------------------------------------
    SetTargetWindow( m_ChartWin );

	//-----------------------------------------------------------------------
	// set color
	//-----------------------------------------------------------------------
	SetBackgroundColour( *wxWHITE );

}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		dtor
//	DESC:
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxChartCtrl::~wxChartCtrl()
{
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       Add()
//  DESC:
//  PARAMETERS: CChartPoints* cp
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::Add(
    wxChartPoints* cp
)
{
    wxASSERT( m_ChartWin != NULL );

    m_ChartWin->Add( cp );
    SetSizes();
    m_ChartWin->SetSizes( m_Sizes );
    ResetScrollbar();

    //-----------------------------------------------------------------------
    // Recalculate virtual axis size
    //-----------------------------------------------------------------------
    if ( m_YAxisWin )
    {
        m_YAxisWin->SetVirtualMax( m_ChartWin->GetMaxY() );
        m_YAxisWin->SetVirtualMin( m_ChartWin->GetMinY() );
        m_YAxisWin->SetSizes( m_Sizes );
    }

    if ( m_XAxisWin )
    {
        m_XAxisWin->SetVirtualMax( m_ChartWin->GetMaxX() );
        m_XAxisWin->SetVirtualMin( m_ChartWin->GetMinX() );
        m_XAxisWin->SetSizes( m_Sizes );
    }

    if ( m_LegendWin )
    {
        m_LegendWin->Add( cp->GetName(), cp->GetColor() );
    }
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       Clear()
//  DESC:
//  PARAMETERS: None
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::Clear()
{
    wxASSERT( m_ChartWin != NULL );

    m_ChartWin->Clear();
    SetSizes();

    //-----------------------------------------------------------------------
    // Recalculate virtual axis size
    //-----------------------------------------------------------------------
    if ( m_YAxisWin )
    {
        m_YAxisWin->SetVirtualMax( 0 );
        m_YAxisWin->SetVirtualMin( 0 );
        m_YAxisWin->SetSizes( m_Sizes );
    }

    if ( m_XAxisWin )
    {
        m_XAxisWin->SetVirtualMax( 0 );
        m_XAxisWin->SetVirtualMin( 0 );
        m_XAxisWin->SetSizes( m_Sizes );
    }

    if ( m_LegendWin )
    {
        m_LegendWin->Clear();
    }
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		ZoomIn()
//	DESC:		Zoom utility
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::ZoomIn()
{
	SetZoom( m_xZoom * 1.5 );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		ZoomOut()
//	DESC:		Zoom utility
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::ZoomOut()
{
	SetZoom( m_xZoom * 0.6666 );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Resize()
//	DESC:		Resize chart to actual window size
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::Resize()
{
	wxASSERT( m_ChartWin != NULL );

	int h, w;
	GetClientSize( &w, &h );

    int iMax = static_cast<int>(ceil( m_ChartWin->GetMaxX() ));

	//-----------------------------------------------------------------------
	// Try to fit all the charts to the actual window size.
	// Firstly reduce the size by 8/10 and if still not enough repeat the
	// same procedure
	//-----------------------------------------------------------------------

    m_Sizes->SetWidthBar( static_cast<int>(floor(
        m_Sizes->GetWidthBar() * m_xZoom)) );
    m_Sizes->SetWidthBar3d( static_cast<int>(floor(
        m_Sizes->GetWidthBar3d() * m_xZoom)) );
    m_Sizes->SetGap( static_cast<int>(floor(
        m_Sizes->GetGap() * m_xZoom)) );

    m_xZoom = 1;
    do
	{
        if ( m_Sizes->GetWidthBar() < MIN_BAR_WIDTH ||
             m_Sizes->GetWidthBar3d() < MIN_BAR3D_WIDTH ||
             m_Sizes->GetGap() < MIN_GAP_WIDTH )
			break;

        int iWidth = CalWidth( iMax+1, m_Sizes->GetNumBar(),
                               m_Sizes->GetNumBar3d(), m_Sizes->GetWidthBar(),
                               m_Sizes->GetWidthBar3d(), m_Sizes->GetGap() );
		if ( iWidth > w )
		{
            m_Sizes->SetWidthBar( static_cast<int>(
                floor(m_Sizes->GetWidthBar() * static_cast<double>(8)/10)) );
            m_Sizes->SetWidthBar3d( static_cast<int>(
                floor(m_Sizes->GetWidthBar() * static_cast<double>(8)/10)) );
            m_Sizes->SetGap( static_cast<int>(
                    floor(m_Sizes->GetGap() * static_cast<double>(8)/10)) );
		}
		else
			break;
	}
	while( true );

	//-----------------------------------------------------------------------
	// resize scroll bars
	//-----------------------------------------------------------------------
	SetZoom( 1 );
	ResetScrollbar();

	//-----------------------------------------------------------------------
	// Set the new sizes to all windows
	//-----------------------------------------------------------------------
	m_ChartWin->SetSizes( m_Sizes );
	if ( m_YAxisWin )
	{
		m_YAxisWin->SetSizes( m_Sizes );
	}

	if ( m_XAxisWin )
	{
		m_XAxisWin->SetSizes( m_Sizes );
	}

	if ( m_LegendWin )
	{
		// nothing to do
	}

}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Fit()
//	DESC:		Fit chart inside window
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::Fit()
{
    wxASSERT( m_ChartWin != NULL );

    int h, w;
    GetClientSize( &w, &h );

    int iMax = static_cast<int>(ceil( m_ChartWin->GetMaxX() ));

	//-----------------------------------------------------------------------
	// Try to fit all the charts to the actual window size.
	// Firstly reduce the size by 8/10 and if still not enough repeat the
	// same procedure
	//-----------------------------------------------------------------------

    m_xZoom = 1;

    m_Sizes->SetWidthBar( DEFAULT_BAR_WIDTH );
    m_Sizes->SetWidthBar3d( DEFAULT_BAR3D_WIDTH );
    m_Sizes->SetGap( DEFAULT_GAP_WIDTH );

    bool wasSmaller = false;
    do
    {
        int iWidth = CalWidth( iMax+1, m_Sizes->GetNumBar(),
                               m_Sizes->GetNumBar3d(), m_Sizes->GetWidthBar(),
                               m_Sizes->GetWidthBar3d(), m_Sizes->GetGap() );

        if ( iWidth < w &&
             !wasSmaller )
        {
            m_Sizes->SetWidthBar( static_cast<int>(
                    floor(m_Sizes->GetWidthBar() * static_cast<double>(12)/10)) );
            m_Sizes->SetWidthBar3d( static_cast<int>(
                    floor(m_Sizes->GetWidthBar() * static_cast<double>(12)/10)) );
            m_Sizes->SetGap( static_cast<int>(
                    floor(m_Sizes->GetGap() * static_cast<double>(12)/10)) );

            continue;
        }

        if ( iWidth > w )
        {
            wasSmaller = true;
            m_Sizes->SetWidthBar( static_cast<int>(
                    floor(m_Sizes->GetWidthBar() * static_cast<double>(8)/10)) );
            m_Sizes->SetWidthBar3d( static_cast<int>(
                    floor(m_Sizes->GetWidthBar() * static_cast<double>(8)/10)) );
            m_Sizes->SetGap( static_cast<int>(
                    floor(m_Sizes->GetGap() * static_cast<double>(8)/10)) );
        }
        else
            break;
    }
    while( true );

	//-----------------------------------------------------------------------
	// resize scroll bars
	//-----------------------------------------------------------------------
    SetZoom( 1 );
    ResetScrollbar();

	//-----------------------------------------------------------------------
	// Set the new sizes to all windows
	//-----------------------------------------------------------------------
    m_ChartWin->SetSizes( m_Sizes );
    if ( m_YAxisWin )
    {
        m_YAxisWin->SetSizes( m_Sizes );
    }

    if ( m_XAxisWin )
    {
        m_XAxisWin->SetSizes( m_Sizes );
    }

    if ( m_LegendWin )
    {
		// nothing to do
    }

}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       LoadImageHandler()
//  DESC:       Load Image Handler
//  PARAMETERS: ChartImageType type
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::LoadImageHandler(
    wxChartImageType type
)
{
    if ( !wxImage::FindHandler(static_cast<wxBitmapType>(type)) )
    {
        switch ( type )
        {
        case wxCHART_PNG:
#if wxUSE_LIBPNG
            wxImage::AddHandler( new wxPNGHandler );
#endif
            break;
        case wxCHART_GIF:
#if wxUSE_GIF
            wxImage::AddHandler( new wxGIFHandler );
#endif
            break;
        case wxCHART_JPEG:
#if wxUSE_LIBJPEG
            wxImage::AddHandler( new wxJPEGHandler );
#endif
            break;
        case wxCHART_BMP:
            // nothing todo
            break;
        }
    }
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		WriteToFile()
//	DESC:		Write chart to file
//	PARAMETERS:	wxString file
//              wxChartImageType type
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::WriteToFile(
	wxString file,
    wxChartImageType type
)
{
	wxASSERT( m_ChartWin != NULL );

	wxBitmap	*memChart;
	wxMemoryDC	memDC;

    //-----------------------------------------------------------------------
    // Load handler for image type if not loaded yet
    //-----------------------------------------------------------------------
    LoadImageHandler( type );

	//-----------------------------------------------------------------------
	// Get the size of the chart
	//-----------------------------------------------------------------------
    int iMax = static_cast<int>(ceil( m_ChartWin->GetMaxX() ));
	int h, w, w1;
	GetClientSize( &w1, &h );

	w = 0;
	if ( iMax > 0 )
        w = CalWidth( iMax+1, m_Sizes->GetNumBar(), m_Sizes->GetNumBar3d(),
            m_Sizes->GetWidthBar(), m_Sizes->GetWidthBar(), m_Sizes->GetGap() );

    //-----------------------------------------------------------------------
    // Make sure the size of the bitmap is created big enough.
    // w1 is the size on screen w is the relative size
    //-----------------------------------------------------------------------
    w = w1 > w ? w1 : w;

	//-----------------------------------------------------------------------
	// add extra size, so legend window can be a bit far from the graph
	//-----------------------------------------------------------------------
	const int ENLARGE_WIDTH = 30;
	if ( iMax > 0 )
    {
        if ( m_YAxisWin )
            w += YAXIS_WIDTH;

        if ( m_LegendWin )
            w += LEGEND_WIDTH;

		w += ENLARGE_WIDTH;
    }

	//-----------------------------------------------------------------------
	// Create the bitmap to hold the chart
	//-----------------------------------------------------------------------
	memChart = new wxBitmap( w, h );

	memDC.SelectObject( *memChart );
	memDC.Clear();

	//-----------------------------------------------------------------------
	// Draw all
	//-----------------------------------------------------------------------
	//m_ChartWin->Draw( &memDC, YAXIS_WIDTH, 0 );
	if ( m_YAxisWin )
	{
        m_ChartWin->Draw( &memDC, YAXIS_WIDTH, 0 );
		m_YAxisWin->Draw( &memDC, 0, 0 );

        if ( m_XAxisWin )
        {
            m_XAxisWin->Draw( &memDC, YAXIS_WIDTH, h - XAXIS_HEIGHT );
        }
    }
    else
    {
        m_ChartWin->Draw( &memDC, 0, 0 );

        if ( m_XAxisWin )
        {
            m_XAxisWin->Draw( &memDC, 0, h - XAXIS_HEIGHT );
        }

    }

	if ( m_LegendWin )
	{
		m_LegendWin->Draw( &memDC, w - LEGEND_WIDTH, 0 );
	}

	memDC.SelectObject( wxNullBitmap );

	memChart->SaveFile( file, wxBITMAP_TYPE_PNG, (wxPalette*)NULL );
	//memChart->SaveFile( file, wxBITMAP_TYPE_BMP, (wxPalette*)NULL );

	delete memChart;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		CalWidth()
//	DESC:		Calculate the size of the chart
//	PARAMETERS:	int n,
//				int nbar,
//				int nbar3d,
//				int wbar,
//				int wbar3d,
//				int gap
//	RETURN:		None
//----------------------------------------------------------------------E-+++
int wxChartCtrl::CalWidth(
	int n,
	int nbar,
	int nbar3d,
	int wbar,
	int wbar3d,
	int gap
)
{
	//-----------------------------------------------------------------------
	// formula to calcolate the width
	//-----------------------------------------------------------------------
    int iWidth = static_cast<int>(n * m_xZoom *
                                  ( nbar  * wbar + nbar3d * wbar3d + gap ));
	return ( iWidth );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetZoom()
//	DESC:
//	PARAMETERS:	double zoom
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::SetZoom(
	double zoom
)
{
	wxASSERT( m_ChartWin != NULL );

	//-----------------------------------------------------------------------
	// set the zoom only if respect min constrains
	//-----------------------------------------------------------------------
    if ( zoom * m_Sizes->GetWidthBar() >= MIN_BAR_WIDTH &&
         zoom * m_Sizes->GetWidthBar3d() >= MIN_BAR3D_WIDTH &&
         zoom * m_Sizes->GetGap() >= MIN_GAP_WIDTH )
		m_xZoom = zoom;

	//-----------------------------------------------------------------------
	// resize scroll bar
	//-----------------------------------------------------------------------
	ResetScrollbar();

	//-----------------------------------------------------------------------
	// Set the new zoom to all windows
	//-----------------------------------------------------------------------
    m_Sizes->SetXZoom( m_xZoom );

	m_ChartWin->SetZoom( m_xZoom );

	if ( m_YAxisWin )
	{
		m_YAxisWin->SetZoom( m_xZoom );
	}

	if ( m_XAxisWin )
	{
		m_XAxisWin->SetZoom( m_xZoom );
	}

	if ( m_LegendWin )
	{
		// nothing to do
	}

	RedrawEverything();
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		SetSizes()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::SetSizes()
{
	wxASSERT( m_ChartWin != NULL );

	//-----------------------------------------------------------------------
	// update sizes descriptor
	//-----------------------------------------------------------------------
    m_Sizes->SetNumBar( m_ChartWin->GetNumBarPoints() );
    m_Sizes->SetNumBar3d( m_ChartWin->GetNumBar3DPoints() );
    m_Sizes->SetMaxY( m_ChartWin->GetMaxY() );
    m_Sizes->SetMinY( m_ChartWin->GetMinY() );
    m_Sizes->SetMaxX( m_ChartWin->GetMaxX() );
    m_Sizes->SetMinX( m_ChartWin->GetMinX() );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		ResetScrollbar()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::ResetScrollbar()
{

	wxASSERT( m_ChartWin != NULL );

    int iMax = static_cast<int>(ceil( m_ChartWin->GetMaxX() ));

	if ( iMax > 0 )
	{
        int iWidth = CalWidth( iMax+1, m_Sizes->GetNumBar(),
                               m_Sizes->GetNumBar3d(), m_Sizes->GetWidthBar(),
                               m_Sizes->GetWidthBar3d(), m_Sizes->GetGap() );
		//int x, y;
		//GetClientSize( &x, &y );

		m_ChartWin->SetVirtualSize( iWidth, -1 );
		FitInside();
	}

}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		RedrawXAxis()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::RedrawXAxis()
{
	if ( m_XAxisWin )
        m_XAxisWin->Refresh( TRUE );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		RedrawYAxis()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::RedrawYAxis()
{
	if ( m_YAxisWin )
       m_YAxisWin->Refresh( TRUE );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		RedrawLegend()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::RedrawLegend()
{
    if ( m_LegendWin )
        m_LegendWin->Refresh( TRUE );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		RedrawEverything()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::RedrawEverything()
{
	wxASSERT( m_ChartWin != NULL );

	RedrawXAxis();
	RedrawYAxis();
    RedrawLegend();

	m_ChartWin->Refresh( TRUE );
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		OnZoomIn()
//	DESC:
//	PARAMETERS:	wxCommandEvent& event
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::OnZoomIn(
	wxCommandEvent &WXUNUSED(event)
)
{
	ZoomIn();
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		OnZoomOut()
//	DESC:
//	PARAMETERS:	wxCommandEvent& event
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::OnZoomOut(
	wxCommandEvent &WXUNUSED(event)
)
{
	ZoomOut();
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		OnScroll()
//	DESC:
//	PARAMETERS:	wxScrollWinEvent& event
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::OnScroll(
	wxScrollWinEvent& event
)
{
    //    if (event.GetEventType() != wxEVT_SCROLLWIN_THUMBTRACK)
    {
#if wxCHECK_VERSION(2, 9, 0)
        wxScrolledWindow::HandleOnScroll( event );
#else
        wxScrolledWindow::OnScroll( event );
#endif
        RedrawXAxis();
    }

}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       OnSize()
//  DESC:
//  PARAMETERS: wxSizeEvent& event
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void wxChartCtrl::OnSize(
    wxSizeEvent &WXUNUSED(event)
)
{
    Resize();
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetZoomInBitmap()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		wxBitmap *
//----------------------------------------------------------------------E-+++
wxBitmap wxChartCtrl::GetZoomInBitmap()
{
//    static wxBitmap* s_bitmap = (wxBitmap *) NULL;
//    static bool s_loaded = FALSE;

//    if ( !s_loaded )
    {
//        s_loaded = TRUE; // set it to TRUE anyhow, we won't try again

        #if defined(__WXMSW__) || defined(__WXPM__)
            return wxBitmap(wxT("chart_zin_bmp"), wxBITMAP_TYPE_RESOURCE);
        #else
            return wxBitmap( chart_zin_xpm );
        #endif
    }

//    return s_bitmap;
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		GetZoomOutBitmap()
//	DESC:
//	PARAMETERS:	None
//	RETURN:		wxBitmap *
//----------------------------------------------------------------------E-+++
wxBitmap wxChartCtrl::GetZoomOutBitmap()
{
//    static wxBitmap* s_bitmap = (wxBitmap *) NULL;
//    static bool s_loaded = FALSE;

//	if ( !s_loaded )
    {
//        s_loaded = TRUE; // set it to TRUE anyhow, we won't try again

        #if defined(__WXMSW__) || defined(__WXPM__)
            return wxBitmap(wxT("chart_zot_bmp"), wxBITMAP_TYPE_RESOURCE);
        #else
            return wxBitmap( chart_zot_xpm );
        #endif
    }

//    return s_bitmap;
}
