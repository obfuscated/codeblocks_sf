/***************************************************************
 * Name:      wxledpanel.h
 * Purpose:   Defines Class wxLEDPanel
 * Author:    Christian Gräfe (info@mcs-soft.de)
 * Created:   2007-02-28
 * Copyright: Christian Gräfe (www.mcs-soft.de)
 * License:	  wxWindows licence
 **************************************************************/
#ifndef WXLEDPANEL_H
#define WXLEDPANEL_H

#include "wx/advancedmatrixobject.h"
#include "wx/wxledfont.h"
//#include "wx/wxledpaneldef.h"
#include <wx/wx.h>
#include <wx/animate.h>

enum wxLEDColour
{
	wxLED_COLOUR_RED = 1,
	wxLED_COLOUR_GREEN = 2,
	wxLED_COLOUR_BLUE = 3,
	wxLED_COLOUR_YELLOW = 4,
	wxLED_COLOUR_MAGENTA = 5,
	wxLED_COLOUR_CYAN = 6,
	wxLED_COLOUR_GREY = 7
};

class wxLEDPanel : public wxControl
{
	public:
		// Ctor
		wxLEDPanel();
		wxLEDPanel(wxWindow* parent, wxWindowID id,
					const wxSize& ledsize, const wxSize& fieldsize, int padding=0,
					const wxPoint& pos = wxDefaultPosition,
					long style = wxNO_BORDER,
					const wxValidator& validator = wxDefaultValidator);

        // Dtor
		virtual ~wxLEDPanel();

		// Create the Element
		bool Create(wxWindow* parent, wxWindowID id,
					const wxSize& ledsize, const wxSize& fieldsize, int padding=0,
					const wxPoint& pos = wxDefaultPosition,
					long style = wxNO_BORDER,
					const wxValidator& validator = wxDefaultValidator);

		// Size of the element
		wxSize DoGetBestSize() const;

		// Clear the Context
		void Clear();

		// Reset the Context
		void Reset();

		// Größenangaben
		wxSize GetFieldsize() const;
		wxSize GetLEDSize() const;

		// Set the Colour of the LEDs
		void SetLEDColour(wxLEDColour colourID);
		const wxColour& GetLEDColour() const;

		// Set the Colour of the Background
		virtual bool SetBackgroundColour(const wxColour& colour);

		// ScrollSpeed
		void SetScrollSpeed(int speed);
		int GetScrollSpeed() const;

		// Scrolldirection
		void SetScrollDirection(wxDirection d);
		wxDirection GetScrollDirection() const;

		// Draw Invertet (default behavior is false)
		void ShowInvertet(bool invert=true);

		// Show inactiv LEDs? (default behavior is true)
		void ShowInactivLEDs(bool show_inactivs=true);

		// Text Alignment if wxLED_SCROLL_NONE
		void SetContentAlign(int a);	// a -> wxAlignment e.g. wxALIGN_TOP|wxALIGN_RIGHT
		int GetContentAlign() const;

		// Text to show
		void SetText(const wxString& text, int align=-1);	// align e.g. "wxLEFT|wxTOP", align=-1 -> Use corrent align
		wxString GetText() const;

		// Bitmap to show
		void SetImage(const wxImage img);
		wxImage GetContentAsImage() const;

		// Animation
		void SetAnimation(const wxAnimation ani);
		const wxAnimation GetAnimation() const;

		// TextPadding (only used when wxALIGN_LEFT or wxALIGN_RIGHT)
		void SetContentPaddingLeft(int padLeft);
		void SetContentPaddingRight(int padRight);
		int GetContentPaddingLeft() const;
		int GetContentPaddingRight() const;

		// Space Between the Letters
		void SetLetterSpace(int letterSpace);
		int GetLetterSpace() const;

		// FontType
		void SetFontType(wxLEDFontType t);
		wxLEDFontType GetFontType() const;

    protected:
		// Drawing
		void DrawField(wxDC& dc, bool backgroundMode=false);
		void OnEraseBackground(wxEraseEvent& event);
		void OnPaint(wxPaintEvent &event);

		// Move the Content
		void ShiftLeft();
		void ShiftRight();
		void ShiftUp();
		void ShiftDown();

		// the data for the LED-field
		AdvancedMatrixObject m_field;

		// Control-Properties
		wxSize m_ledsize;
        int m_padding;
        int m_align;
        int m_padLeft;
        int m_padRight;
        wxLEDColour m_activ_colour_id;
        bool m_invert;
        bool m_show_inactivs;

        // Scroll-Properties
        int m_scrollspeed;
		wxDirection m_scrolldirection;
        wxTimer m_scrollTimer;

        // Scroll-function
        void OnScrollTimer(wxTimerEvent& event);

		// MemoryDCs with "LED on" and "LED off" and no LED
        wxMemoryDC m_mdc_led_on;
        wxMemoryDC m_mdc_led_off;
        wxMemoryDC m_mdc_led_none;

        wxMemoryDC m_mdc_background;

        // the text
        wxString m_text;
        MatrixObject m_content_mo;
        wxPoint m_pos;
        wxLEDFont m_font;
        wxAnimation m_ani;
        int m_aniFrameNr;

        void ResetPos();
        void PrepareBackground();

        // Colours
        static const wxColour s_colour[7];
        static const wxColour s_colour_dark[7];
        static const wxColour s_colour_verydark[7];
        static const wxColour s_colour_light[7];

        // The Event Table
        DECLARE_EVENT_TABLE()
};



#endif // WXLEDPANEL_H
