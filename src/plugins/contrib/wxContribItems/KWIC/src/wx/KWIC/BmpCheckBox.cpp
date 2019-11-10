/////////////////////////////////////////////////////////////////////////////
// Name:        BmpCheckBox.cpp
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
//#include "kprec.h"		//#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include <wx/image.h>
#include <wx/log.h>

#include "wx/KWIC/BmpCheckBox.h"
#include <wx/event.h>



IMPLEMENT_DYNAMIC_CLASS(kwxBmpCheckBox, wxControl)


BEGIN_EVENT_TABLE(kwxBmpCheckBox,wxControl)
  EVT_PAINT(kwxBmpCheckBox::OnPaint)
  EVT_MOUSE_EVENTS(kwxBmpCheckBox::OnMouse)
END_EVENT_TABLE()

kwxBmpCheckBox::~kwxBmpCheckBox()
{
	if (membitmap)
		delete membitmap ;
}

kwxBmpCheckBox::kwxBmpCheckBox(wxWindow* parent,
		                   const wxWindowID id,
                           wxBitmap& OnBitmap,
							wxBitmap& OffBitmap,
							wxBitmap& OnSelBitmap,
							wxBitmap& OffSelBitmap,
                           const wxPoint& pos,
                           const wxSize& size,
		                   const long int style)
	: wxControl(parent, id, pos, size, style)
{

  if (parent)
    SetBackgroundColour(parent->GetBackgroundColour());
  else
    SetBackgroundColour(*wxLIGHT_GREY);

	mOnBitmap  = &OnBitmap;
	mOffBitmap = &OffBitmap;
	mOnSelBitmap = &OnSelBitmap;
	mOffSelBitmap = &OffSelBitmap;
	m_id = id;

    int total_width = 0, total_height = 0 ;
	total_width = size.GetWidth() ;
	total_height = size.GetHeight();

	SetSize(total_width, total_height);
    SetAutoLayout(TRUE);
	Refresh();
	m_stato = 0;
	m_oldstato = 0;
	m_bPress = false ;
	m_bBord = true ;
	m_nStyle = wxDOT;

	membitmap = new wxBitmap(total_width, total_height) ;
}

void kwxBmpCheckBox::SetLabel(wxString label)
{
	mLabelStr = label;
}

void kwxBmpCheckBox::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxPaintDC old_dc(this);

	int w,h;
	int bdraw = 0 ;

	GetClientSize(&w,&h);

	/////////////////

	// Create a memory DC
	wxMemoryDC dc;
	dc.SelectObject(*membitmap);


	dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(),wxSOLID));
	dc.Clear();

	///////////////////


	// se impostato n bitmap lo disegno
    //if (mOffBitmap)
	if(m_stato == 0)
	{
		if(m_bPress)
			dc.DrawBitmap(*mOnBitmap, 0, 0, TRUE);
		else
			dc.DrawBitmap(*mOffBitmap, 0, 0, TRUE);
		bdraw = 0 ;
	}
	else if(m_stato == 1)
	{
		if(m_bPress)
			dc.DrawBitmap(*mOnSelBitmap, 0, 0, TRUE);
		else
			dc.DrawBitmap(*mOffSelBitmap, 0, 0, TRUE);
		bdraw = 1 ;
	}
	else if(m_stato == 2)
	{
		if(m_bPress)
			dc.DrawBitmap(*mOffSelBitmap, 0, 0, TRUE);
		else
			dc.DrawBitmap(*mOnSelBitmap, 0, 0, TRUE);
		bdraw = 1 ;
	}

	if(m_bBord)
	{
		if(bdraw)
		{
			// Cornice intorno
			dc.SetPen(*wxThePenList->FindOrCreatePen(*wxRED, 1, m_nStyle ));
			dc.DrawLine(0, 0, 0, h - 1);
			dc.DrawLine(0, 0, w, 0);
			dc.DrawLine(0, h - 1, w, h - 1);
			dc.DrawLine(w - 1, 0, w - 1, h - 1);
		}
	}

	// We can now draw into the memory DC...
	// Copy from this DC to another DC.
	old_dc.Blit(0, 0, w, h, &dc, 0, 0);
}

void kwxBmpCheckBox::OnMouse(wxMouseEvent& event)
{
	if (m_stato == 0 && event.Entering())
	{
		m_stato = 1;	// mouse sul bottone
		wxCommandEvent ev(event.GetEventType(),GetId());
		ev.SetEventType(wxEVT_ENTER_WINDOW);
		event.SetEventObject(this);
		GetEventHandler()->ProcessEvent(ev);
	}
	else if (m_stato == 1 && event.LeftDown())
		m_stato = 2;	// uscita click sul bottone
	else if (m_stato >= 1 && event.Leaving())
	{
		m_stato = 0;	// uscita mouse dal bottone
		wxCommandEvent ev(event.GetEventType(),GetId());
		ev.SetEventType(wxEVT_LEAVE_WINDOW);
		event.SetEventObject(this);
		GetEventHandler()->ProcessEvent(ev);
	}
	else if (m_stato == 2 && event.LeftUp())
	{
		m_bPress = !m_bPress ;
		Click();		// rilascio sul bottone genera evento
		m_stato = 1;

	}


	if(m_oldstato != m_stato)
		Refresh();
	m_oldstato=m_stato;

	event.Skip();
}

void kwxBmpCheckBox::Click()
{
	wxCommandEvent event(kwxEVT_BITBUTTON_CLICK, GetId());
	event.SetEventObject(this);
//	ProcessCommand(event);
    GetEventHandler()->ProcessEvent(event);
}

void kwxBmpCheckBox::SetState(bool newstate)
{
	m_bPress = newstate ;
	Refresh();
}

