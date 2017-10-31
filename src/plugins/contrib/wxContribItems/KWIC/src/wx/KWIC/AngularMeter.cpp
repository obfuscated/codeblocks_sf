/////////////////////////////////////////////////////////////////////////////
// Name:        AngularMeter.cpp
// Purpose:     wxIndustrialControls Library
// Author:      Marco Cavallini <m.cavallini AT koansoftware.com>
// Modified by:
// Copyright:   (C)2004-2006 Copyright by Koan s.a.s. - www.koansoftware.com
// Licence:     KWIC License http://www.koansoftware.com/kwic/kwic-license.htm
/////////////////////////////////////////////////////////////////////////////
//
//	Cleaned up and modified by Gary Harris for wxSmithKWIC, 2010.
//
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include <wx/image.h>
#include <math.h>
#include <wx/log.h>

#include "wx/KWIC/AngularMeter.h"
#include <wx/event.h>


//IMPLEMENT_DYNAMIC_CLASS(kwxAngularMeter, wxWindow)

BEGIN_EVENT_TABLE(kwxAngularMeter,wxWindow)
	EVT_PAINT(kwxAngularMeter::OnPaint)
	EVT_ERASE_BACKGROUND(kwxAngularMeter::OnEraseBackGround)
END_EVENT_TABLE()

kwxAngularMeter::kwxAngularMeter(wxWindow* parent, const wxWindowID id, const wxPoint& pos, const wxSize& size)
	: wxWindow(parent, id, pos, size, 0)
{

  if (parent)
    SetBackgroundColour(parent->GetBackgroundColour());
  else
    SetBackgroundColour(*wxLIGHT_GREY);

    //SetSize(size);
    SetAutoLayout(TRUE);
	Refresh();

	m_id = id;

	//Default values

	m_nScaledVal = 0;		//degrees
	m_nRealVal = 0;
	m_nTick = 0;	//number of notches
	m_nSec = 1;		//default number of sectors
	m_nRangeStart = 0;
	m_nRangeEnd = 220;
	m_nAngleStart = -20;
	m_nAngleEnd = 200;
	m_aSectorColor[0] = *wxWHITE;
//	m_cBackColour = *wxLIGHT_GREY;
	m_cBackColour = GetBackgroundColour() ;		//default background application
	m_cNeedleColour = *wxRED;	//indicator
	m_cBorderColour = GetBackgroundColour() ;
	m_dPI = 4.0 * atan(1.0);
	m_Font = *wxSWISS_FONT;	//font
	m_bDrawCurrent = true ;

	membitmap = new wxBitmap(size.GetWidth(), size.GetHeight()) ;

    m_BackgroundDc.SelectObject(*membitmap);
    m_BackgroundDc.SetBackground(parent->GetBackgroundColour());
    m_BackgroundDc.Clear();
    m_BackgroundDc.SetPen(*wxRED_PEN);
    //m_BackgroundDc.SetBrush(*wxTRANSPARENT_BRUSH);
    m_BackgroundDc.SetBrush(*wxTheBrushList->FindOrCreateBrush(*wxRED,wxSOLID));

    int w,h ;
    GetClientSize(&w,&h);

    ///////////////////

	//Rectangle

	//m_BackgroundDc.SetPen(*wxThePenList->FindOrCreatePen(m_cBorderColour, 1, wxSOLID));
	//m_BackgroundDc.DrawRectangle(0,0,w,h);

	//Fields
	DrawSectors(m_BackgroundDc) ;

	//Ticks
	if (m_nTick > 0)
		DrawTicks(m_BackgroundDc);



    //m_BackgroundDc.DrawRectangle(wxRect(10, 10, 100, 100));

	/////////////// TODO : Test for BMP image loading /////////////////
/*

	m_pPreviewBmp = NULL ;

    wxBitmap bitmap( 300, 300 );
    wxImage image = bitmap.ConvertToImage();

    image.Destroy();

    if ( !image.LoadFile( "thumbnail.bmp", wxBITMAP_TYPE_BMP ) )
        wxLogError(wxT("Can't load BMP image"));
    else
        m_pPreviewBmp = new wxBitmap( image );
*/
    m_bNeedRedrawBackground = false;

}

kwxAngularMeter::~kwxAngularMeter()
{
	delete membitmap;
}

void kwxAngularMeter::SetValue(int val)
{
	int deltarange = m_nRangeEnd - m_nRangeStart;
//	int rangezero = deltarange - m_nRangeStart;
	int deltaangle = m_nAngleEnd - m_nAngleStart;
	double coeff = (double)deltaangle / (double)deltarange;

	m_nScaledVal = (int)((double)(val - m_nRangeStart) * coeff);
	m_nRealVal = val;
	m_bNeedRedrawBackground = true;
	Refresh();
}


void kwxAngularMeter::OnPaint(wxPaintEvent &WXUNUSED(event))
{
	wxPaintDC old_dc(this);

	int w,h ;

	GetClientSize(&w,&h);

	/////////////////

	if (m_bNeedRedrawBackground)
    {
        ConstructBackground();
        m_bNeedRedrawBackground = false;
    }


	// Create a memory DC
    wxMemoryDC dc;
    wxBitmap a(w,h);
    dc.SelectObject(a);
//
//dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(m_cBackColour,wxSOLID));
//dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(m_cBackColour,wxSOLID));
//	dc.Clear();


/*
    if (m_pPreviewBmp && m_pPreviewBmp->Ok())
		dc.DrawBitmap( *m_pPreviewBmp, 1, 1 );
*/

    dc.Blit(0, 0, w, h, &m_BackgroundDc, 0, 0);


	//Indicator needle

	DrawNeedle(dc);


	//Text value
	if (m_bDrawCurrent)
	{
		wxString valuetext;
		valuetext.Printf(wxT("%d"),m_nRealVal);
		dc.SetFont(m_Font);
		dc.DrawText(valuetext, (w / 2) - 10, (h / 2) + 10);
	}

	// We can now draw into the memory DC...
	// Copy from this DC to another DC.
	old_dc.Blit(0, 0, w, h, &dc, 0, 0);
}


void kwxAngularMeter::DrawNeedle(wxDC &dc)
{
	//Triangular
	double dxi,dyi, val;
	wxPoint ppoint[6];
	int w, h ;

	GetClientSize(&w,&h);

	dc.SetPen(*wxThePenList->FindOrCreatePen(m_cNeedleColour, 1,wxSOLID));

	val = (m_nScaledVal + m_nAngleStart) * m_dPI / 180; //radians angle parameter

	dyi = sin(val - 90) * 2; //coordinates of the left base
	dxi = cos(val - 90) * 2;

	ppoint[0].x = (w / 2) - dxi;	//left base
	ppoint[0].y = (h / 2) - dyi;

	dxi = cos(val) * ((h / 2) - 4); //coordinates of the tip marker
	dyi = sin(val) * ((h / 2) - 4);

	ppoint[2].x = (w / 2) - dxi;	//points
	ppoint[2].y = (h / 2) - dyi;

	dxi = cos(val + 90) * 2; //coordinates basic right
	dyi = sin(val + 90) * 2;

	ppoint[4].x = (w / 2) - dxi;	//right based
	ppoint[4].y = (h / 2) - dyi;

	ppoint[5].x = ppoint[0].x;	//return the left base
	ppoint[5].y = ppoint[0].y;

//////////////////////////
	val = (m_nScaledVal + m_nAngleStart + 1) * m_dPI / 180;

	dxi = cos(val) * ((h / 2) - 10); //coordinates of the right middle
	dyi = sin(val) * ((h / 2) - 10);

	ppoint[3].x = (w / 2) - dxi;	//average basis right
	ppoint[3].y = (h / 2) - dyi;

	val = (m_nScaledVal + m_nAngleStart - 1) * m_dPI / 180;

	dxi = cos(val) * ((h / 2) - 10); //coordinates of the left middle
	dyi = sin(val) * ((h / 2) - 10);

	ppoint[1].x = (w / 2) - dxi;	//average basis the left
	ppoint[1].y = (h / 2) - dyi;

/////////////////////////


	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(m_cNeedleColour,wxSOLID));

	dc.DrawPolygon(6, ppoint, 0, 0, wxODDEVEN_RULE);

	//Circle indicator
	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(*wxWHITE,wxSOLID));
	dc.DrawCircle(w / 2, h / 2, 4);
}



void kwxAngularMeter::DrawSectors(wxDC &dc)
{
	double starc,endarc;
	int secount,dx,dy;
	int w,h ;

	double val;

	GetClientSize(&w,&h);

	wxRect innerRect;
	innerRect.x = w*2/35;
	innerRect.y = h*2/35;
	innerRect.width = w-2*innerRect.x;
	innerRect.height = h-2*innerRect.y;

	//Arch -> sectors
	//dc.SetPen(*wxThePenList->FindOrCreatePen(*wxBLACK, 1, wxSOLID));
	dc.SetPen(*wxThePenList->FindOrCreatePen(*wxBLACK, 1, wxTRANSPARENT));

	starc = m_nAngleStart;
	endarc = starc + ((m_nAngleEnd - m_nAngleStart) / (double)m_nSec);
	//dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(*wxRED,wxSOLID));
	for(secount=0;secount<m_nSec;secount++)
	{
		dc.SetPen(*wxThePenList->FindOrCreatePen(*wxBLACK, 1, wxTRANSPARENT));
		dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(m_aSectorColor[secount],wxSOLID));
		dc.DrawEllipticArc(0,0,w,h,180 - endarc,180 - starc);

		dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(m_cBackColour,wxSOLID));
		dc.DrawEllipticArc(innerRect.x,innerRect.y,innerRect.width,innerRect.height,180 - endarc,180 - starc);

		starc = endarc;
		endarc += ((m_nAngleEnd - m_nAngleStart) / (double)m_nSec);
	}



	val = (m_nAngleStart * m_dPI) / 180.0;
	dx = cos(val) * h / 2.0;
	dy = sin(val) * h / 2.0;

	dc.DrawLine(w / 2, h / 2, (w / 2) - dx, (h / 2) - dy);	//left line

	val = (m_nAngleEnd * m_dPI) / 180.0;
	dx = cos(val) * h / 2.0;
	dy = sin(val) * h / 2.0;

	dc.DrawLine(w / 2, h / 2, (w / 2) - dx, (h / 2) - dy);	//right line

}

void kwxAngularMeter::DrawTicks(wxDC &dc)
{
	double intervallo = (m_nAngleEnd - m_nAngleStart) / (m_nTick + 1.0); //angle between each ticks (deg)
	double valint = m_nAngleStart; //(deg)
	double tx, ty;
	double val;
	double dx, dy;
	int n, w, h;
	int tw, th;
	wxString s;

	GetClientSize(&w, &h);

	dc.SetPen(*wxThePenList->FindOrCreatePen(*wxBLACK, 2, wxSOLID));

	for(n = 0;n < m_nTick+2;n++)
	{
		dc.SetPen(*wxThePenList->FindOrCreatePen(*wxBLACK, 4, wxSOLID));
		val=(valint * m_dPI) / 180;
		//wxLogTrace("v: %f",valint);
		dx = cos(val) * (h/2);	//point on the circle
		dy = sin(val) * (h/2);

		tx = cos(val) * ((h / 2) - 10);	//point in the circle
		ty = sin(val) * ((h / 2) - 10);

		dc.DrawLine((w / 2) - tx, (h / 2) - ty, (w / 2) - dx, (h / 2) - dy);


		int deltarange = m_nRangeEnd - m_nRangeStart;
		int deltaangle = m_nAngleEnd - m_nAngleStart;
		double coeff = (double)deltaangle / (double)deltarange;

		int rightval = (int)(((valint - (double)m_nAngleStart)) / coeff) + m_nRangeStart;

		s.Printf(wxT("%d"), rightval); //only use integer value

		dc.GetTextExtent(s, &tw, &th);

		val = valint * m_dPI/ 180 - 0.5*tw/(0.5*h - 12.0);	//angle moved (rad)
		tx = cos(val) * ((h / 2) - 12);	//item text
		ty = sin(val) * ((h / 2) - 12);

		dc.SetFont(m_Font);

		dc.DrawRotatedText(s,(w / 2) - tx, (h / 2) - ty, 90 - valint);


        //draw SubTick (Do not draw subticks after the last Tick)
        if (n==m_nTick+1)
            continue;

        double stepSubtick  = intervallo*m_dPI/5.0/180.0;
        double beginSubtick = valint*m_dPI/180.0+stepSubtick;
		for (int i = 0; i<4; i++)
        {
            dc.SetPen(*wxThePenList->FindOrCreatePen(*wxBLACK, 1, wxSOLID));

            dx = cos(beginSubtick) * (h/2);	//point on the circle
            dy = sin(beginSubtick) * (h/2);
            tx = cos(beginSubtick) * ((h / 2) - 5);	//point in the circle
            ty = sin(beginSubtick) * ((h / 2) - 5);

            dc.DrawLine((w / 2) - tx, (h / 2) - ty, (w / 2) - dx, (h / 2) - dy);
            beginSubtick += stepSubtick;
        }

		valint = valint + intervallo;
	}

}

void kwxAngularMeter::SetSectorColor(int nSector, wxColour colour)
{
	m_aSectorColor[nSector] = colour;
	m_bNeedRedrawBackground = true;
}

void kwxAngularMeter::ConstructBackground()
{
    m_BackgroundDc.SelectObject(*membitmap);
    m_BackgroundDc.SetBackground(GetBackgroundColour());
    m_BackgroundDc.Clear();
    m_BackgroundDc.SetPen(*wxRED_PEN);
    //m_BackgroundDc.SetBrush(*wxTRANSPARENT_BRUSH);
    m_BackgroundDc.SetBrush(*wxTheBrushList->FindOrCreateBrush(*wxRED,wxSOLID));

    int w,h ;
    GetClientSize(&w,&h);

    ///////////////////

	//Rectangle

	//m_BackgroundDc.SetPen(*wxThePenList->FindOrCreatePen(m_cBorderColour, 1, wxSOLID));
	//m_BackgroundDc.DrawRectangle(0,0,w,h);

	//Fields
	DrawSectors(m_BackgroundDc) ;
	//Ticks
	if (m_nTick > 0)
		DrawTicks(m_BackgroundDc);
}
