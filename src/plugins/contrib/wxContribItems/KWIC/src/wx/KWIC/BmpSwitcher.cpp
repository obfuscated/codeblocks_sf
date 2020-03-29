/////////////////////////////////////////////////////////////////////////////
// Name:        BmpSwitcher.cpp
// Purpose:     wxIndustrialControls Library
// Author:      Marco Cavallini <m.cavallini AT koansoftware.com>
// Modified by:
// Copyright:   (C)2004-2006 Copyright by Koan s.a.s. - www.koansoftware.com
// Licence:     KWIC License http://www.koansoftware.com/kwic/kwic-license.htm
/////////////////////////////////////////////////////////////////////////////

//	La classe kwxBmpSwitcher riceve una o più bitmap tramite il metodo
//	AddBitmap, e permette lo scambio di esse tramite il metodo IncState o SetState.
//
//	IncState():			Passa alla bitmap successiva, se si trova all'ultima
//						bitmap riparte dalla prima inserita.
//
//	SetState(stato):	Carica direttamente la bitmap indicata dallo stato
//						passato come parametro.
//
//	Nel caso in cui sia stata inserita una sola bitmap il metodo IncState
//	non ha effetto sul controllo.
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

#include "wx/KWIC/BmpSwitcher.h"
#include <wx/event.h>

#include <wx/listimpl.cpp>

WX_DEFINE_LIST(CBmpList);

BEGIN_EVENT_TABLE(kwxBmpSwitcher,wxWindow)
	EVT_PAINT(kwxBmpSwitcher::OnPaint)
END_EVENT_TABLE()

kwxBmpSwitcher::kwxBmpSwitcher(wxWindow* parent,
		                   const wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size)
	: wxWindow(parent, id, pos, size, 0)
{
	if (parent)
		SetBackgroundColour(parent->GetBackgroundColour());
	else
		SetBackgroundColour(*wxLIGHT_GREY);

	SetSize(size.GetWidth(), size.GetHeight());

	SetAutoLayout(TRUE);
	Refresh();

	m_nState = 0 ;
	membitmap = new wxBitmap(size.GetWidth(), size.GetHeight()) ;

	// Cryogen 18/9/10 Fixed another crash caused by m_nCount not being initialised.
	m_nCount = 0;
}

kwxBmpSwitcher::~kwxBmpSwitcher()
{
	delete membitmap;

	m_bmplist.DeleteContents(true);
	m_bmplist.Clear();
	m_bmplist.DeleteContents(false);
}

void kwxBmpSwitcher::OnPaint(wxPaintEvent &WXUNUSED(event))
{
	wxPaintDC old_dc(this);

	int w,h;
	wxBitmap *pCurrent ;

	GetClientSize(&w,&h);

	/////////////////

	// Create a memory DC
	wxMemoryDC dc;
	dc.SelectObject(*membitmap);
	dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(),wxSOLID));
	dc.Clear();

	// Cryogen 16/4/10 Fixed to prevent a crash when m_nCount = 0. This is necessary for
	// wxSmithKWIC to be able to initialise the control before bitmaps are added.
	// Also moved update of m_nCount and m_nState to the appropriate functions.
	switch(m_nCount){
		case 0:
			break;
		case 1:
			pCurrent = m_bmplist.front();
			dc.DrawBitmap(*pCurrent, 0, 0, TRUE);
			break;

		default:
			pCurrent = m_bmplist.Item(m_nState)->GetData();

			dc.DrawBitmap(*pCurrent, 0, 0, TRUE);
			break;
	}

	old_dc.Blit(0, 0, w, h, &dc, 0, 0);
}

void kwxBmpSwitcher::IncState()
{
	if (m_nCount > 1 )
	{
		m_nState++ ;
		if (m_nState >= m_nCount )
			m_nState = 0 ;

		Refresh() ;
	}
}

void kwxBmpSwitcher::SetState(int state)
{
	if (m_nCount > 1 )
	{
		m_nState = state ;
		if (m_nState >= m_nCount )
			m_nState = 0 ;

		Refresh() ;
	}
}

void kwxBmpSwitcher::AddBitmap(wxBitmap *bitmap)
{
	m_bmplist.Append(bitmap);
	m_nCount = m_bmplist.GetCount() ;
}
