/////////////////////////////////////////////////////////////////////////////
// Name:        led.h
// Purpose:     wxLed class
// Author:      Thomas Monjalon
// Created:     09/06/2005
// Revision:    09/06/2005
// Licence:     wxWidgets
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LED_H_
#define _WX_LED_H_

#include <wx/window.h>
#include <wx/bitmap.h>
#include <wx/dcclient.h>
#include <wx/thread.h>

class wxLed : public wxWindow
{
	public :

		wxLed (wxWindow * parent, wxWindowID id, wxColour disabledColor = wxColour( 00,00,00), wxColour enableColour = wxColour( 00, 255, 00), const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxDefaultSize) ;
		~wxLed () ;

		void Enable (void) ;
		void Disable( void);
		void SetColor (wxColour rgb);
		bool IsEnabled( void);

	protected :

		wxColour m_Enable;
		wxColour m_Disable;
		wxBitmap * m_bitmap ;
		wxMutex m_mutex ;
		bool m_IsEnable;

		void OnPaint (wxPaintEvent & event);

		virtual void SetBitmap (wxString color);

	private :

		DECLARE_EVENT_TABLE ()
} ;

#endif // _WX_LED_H_
