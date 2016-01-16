/////////////////////////////////////////////////////////////////////////////
// Name:        LCDClock.cpp
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

#include <wx/event.h>

#include "wx/KWIC/LCDClock.h"
#include "wx/KWIC/LCDWindow.h"
#include "wx/KWIC/TimeAlarm.h"

BEGIN_EVENT_TABLE(kwxLCDClock,kwxLCDDisplay)
	EVT_TIMER(TIMER_TIME, kwxLCDClock::OnTimer)
END_EVENT_TABLE()

kwxLCDClock::kwxLCDClock(wxWindow *parent, const wxPoint& pos, const wxSize& size )
{
	Create(parent, pos, size);
}

 bool kwxLCDClock::Create(wxWindow *parent, const wxPoint& pos, const wxSize& size )
{
	if(!kwxLCDDisplay::Create(parent, pos, size)){
		return false;
	}

	time_t reset = 0 ;

	m_LastCheck.Set(reset) ;

	wxDateTime now = wxDateTime::Now();

	alarm = new CTimeAlarm();
	alarm->SetAlarmTime(now) ;

	return true;
}

kwxLCDClock::~kwxLCDClock()
{
	if (m_timer.IsRunning())
		m_timer.Stop() ;

	delete alarm ;
}

void kwxLCDClock::OnTimer(wxTimerEvent &WXUNUSED(event))
{
	wxDateTime now = wxDateTime::Now();
	double jDate ;

	if (!now.IsEqualTo(m_LastCheck))
	{
		jDate = now.GetModifiedJulianDayNumber();

		SetValue(now.FormatISOTime()) ;	//(HH:MM:SS)
		m_LastCheck = now ;
	}
}

void kwxLCDClock::StartClock()
{
	m_timer.SetOwner(this, TIMER_TIME) ;

	m_timer.Start(500, wxTIMER_CONTINUOUS) ;
}

void kwxLCDClock::StopClock()
{
	m_timer.Stop() ;
}
