/////////////////////////////////////////////////////////////////////////////
// Name:        led.h
// Purpose:     wxLed class
// Author:      Thomas Monjalon
// Created:     09/06/2005
// Revision:    09/06/2005
// Licence:     wxWidgets
// mod   by:    Jonas Zinn
// mod date:    24/03/2012
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LED_H_
#define _WX_LED_H_

#include <wx/window.h>
#include <wx/bitmap.h>
#include <wx/dcclient.h>
#include <wx/thread.h>

/// Class to display a Led on the used dialogs page
class wxLed : public wxWindow
{
	public :
        /** Constructor
          * @param parent The window parent
          * @param id If u want to specify a ID
          * @param disableColor If the window is disabled, which color it should have
          * @param onColor If the window is enabled and turned on, how it should glow
          * @param offColor If the window is enabled and turned off, how it should look
        */
		wxLed (wxWindow * parent, wxWindowID id = wxID_ANY, wxColour disableColour = wxColour( 128,128,128), wxColour onColour = wxColour( 00, 255, 00), wxColour offColour = wxColour( 255, 00, 00),const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxDefaultSize);
		wxLed();

        /// Destructor
		~wxLed () ;

		bool Create (wxWindow * parent, wxWindowID id = wxID_ANY, wxColour disableColour = wxColour( 128,128,128), wxColour onColour = wxColour( 00, 255, 00), wxColour offColour = wxColour( 255, 00, 00));

        /// Enable the Led
		void Enable (void) ;

		/// Disable the Led
		void Disable( void);

		/// If the Led is Enabled and on, switch the Led off else on.
		void Switch(void);

		/// If is Enabled turn the Led on
		void SwitchOn(void);

		/// If is Enabled turn the Led off
		void SwitchOff(void);

		/** Change the on Color
          * @param rgb The Color how the Led should glow in ON Mode
		*/
		void SetOnColour (wxColour rgb);

		/** Change the off Color
		* @param rgb The Color how the Led should glow in OFF Mode
		*/
		void SetOffColour (wxColour rgb);

		/** Change the disable Color
		* @param rgb The Color how the Led should glow if Disabled
		*/
		void SetDisableColour (wxColour rgb);

        /**

        */
		void SetOnOrOff( bool on);

		/** Test if the Led is Enabled or Disabled
		  * @return function returns true, if the Led is enabled, false otherwise
		*/
		bool IsEnabled( void);

		/** Test if the Led is ON or OFF
		  * @return function returns true, if the Led is ON, false otherwise
		*/
		bool IsOn(void);

	protected :

		wxColour m_On;          /// contains the Color for the ON state
		wxColour m_Off;         /// contains the Color for the OFF state
		wxColour m_Disable;     /// contains the Color for the Disable state
		wxBitmap * m_bitmap;    /// contains the Led as a Bitmap
		wxMutex m_mutex ;
		bool m_isEnable;        /// is the Led enabled?
		bool m_isOn;            /// is the Led in ON state?

        /** Function to paint the LED at the place in the dialog
          * @param event Normal wxWidgets Event
        */
		void OnPaint (wxPaintEvent & event);

        /** Function to create the Bitmap, which is paint on the dialog
          * @param color The used color for the LED
        */
		virtual void SetBitmap (wxString color);

	private :

		DECLARE_EVENT_TABLE ()
} ;

#endif // _WX_LED_H_
