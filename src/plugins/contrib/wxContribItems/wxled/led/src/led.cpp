/////////////////////////////////////////////////////////////////////////////
// Name:        led.cpp
// Purpose:     wxLed implementation
// Author:      Thomas Monjalon
// Created:     09/06/2005
// Revision:    09/06/2005
// Licence:     wxWidgets
/////////////////////////////////////////////////////////////////////////////

#include "wx/led.h"

#include <string.h>

BEGIN_EVENT_TABLE (wxLed, wxWindow)
    EVT_PAINT (wxLed::OnPaint)
END_EVENT_TABLE ()

wxLed::wxLed (wxWindow * parent, wxWindowID id, wxColour disabledColor, wxColour enableColour, const wxPoint & pos, const wxSize & size)
:
	wxWindow (parent, id, pos, size),
	m_bitmap (NULL)
{
	this->m_isEnabled = true ;
	m_Disable = disabledColor;
	m_Enable = enableColour;
	Enable();
}


wxLed::~wxLed ()
{
	delete this->m_bitmap ;
}

void wxLed::Enable (void)
{
	wxWindow::Enable (true) ;
	this->SetBitmap (this->m_Enable.GetAsString( wxC2S_HTML_SYNTAX)) ;
}

void wxLed::Disable( void)
{
    wxWindow::Enable( false);
    this->SetBitmap( this->m_Disable.GetAsString( wxC2S_HTML_SYNTAX));
}

void wxLed::SetColor (wxColour rgb)
{
	m_Enable = rgb;
    if (this->m_isEnabled)
        this->SetBitmap (this->m_Enable.GetAsString( wxC2S_HTML_SYNTAX)) ;
}

bool wxLed::IsEnabled( void)
{
    return this->m_isEnabled;
}

void wxLed::OnPaint (wxPaintEvent & WXUNUSED (event))
{
	wxPaintDC dc (this) ;
	this->m_mutex.Lock () ;
	dc.DrawBitmap (* this->m_bitmap, 0, 0, true) ;
	this->m_mutex.Unlock () ;
}

#define WX_LED_WIDTH    17
#define WX_LED_HEIGHT   17
#define WX_LED_COLORS    5
#define WX_LED_XPM_COLS    (WX_LED_WIDTH + 1)
#define WX_LED_XPM_LINES   (1 + WX_LED_COLORS + WX_LED_HEIGHT)

void wxLed::SetBitmap (wxString color)
{
    char ** xpm = NULL ;
    char * xpmData = NULL ;
    xpm = new char * [WX_LED_XPM_LINES] ;
    if (xpm == NULL)
        goto end ;
    xpmData = new char [WX_LED_XPM_LINES * WX_LED_XPM_COLS] ;
    if (xpmData == NULL)
        goto end ;
    for (int i = 0 ; i < WX_LED_XPM_LINES ; i ++)
        xpm [i] = xpmData + i * WX_LED_XPM_COLS ;
    // width height num_colors chars_per_pixel
    sprintf (xpm [0], "%d %d %d 1", WX_LED_WIDTH, WX_LED_HEIGHT, WX_LED_COLORS) ;
    // colors
    strncpy (xpm [1], "  c None", WX_LED_XPM_COLS) ;
    strncpy (xpm [2], "- c #C0C0C0", WX_LED_XPM_COLS) ;
    strncpy (xpm [3], "_ c #F8F8F8", WX_LED_XPM_COLS) ;
    strncpy (xpm [4], "* c #FFFFFF", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS], "X c ", WX_LED_XPM_COLS) ;
    strncpy ((xpm [WX_LED_COLORS]) + 4, color.char_str(), 8) ;
    // pixels
    strncpy (xpm [WX_LED_COLORS +  1], "      -----      ", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS +  2], "    ---------    ", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS +  3], "   -----------   ", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS +  4], "  -----XXX----_  ", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS +  5], " ----XX**XXX-___ ", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS +  6], " ---X***XXXXX___ ", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS +  7], "----X**XXXXXX____", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS +  8], "---X**XXXXXXXX___", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS +  9], "---XXXXXXXXXXX___", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS + 10], "---XXXXXXXXXXX___", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS + 11], "----XXXXXXXXX____", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS + 12], " ---XXXXXXXXX___ ", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS + 13], " ---_XXXXXXX____ ", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS + 14], "  _____XXX_____  ", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS + 15], "   ___________   ", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS + 16], "    _________    ", WX_LED_XPM_COLS) ;
    strncpy (xpm [WX_LED_COLORS + 17], "      _____      ", WX_LED_XPM_COLS) ;
    this->m_mutex.Lock () ;
    delete this->m_bitmap ;
    this->m_bitmap = new wxBitmap (xpm) ;
    if (this->m_bitmap == NULL)
    {
        this->m_mutex.Unlock () ;
        goto end ;
    }
    this->SetSize (wxSize (this->m_bitmap->GetWidth (), this->m_bitmap->GetHeight ())) ;
    this->m_mutex.Unlock () ;
    this->Refresh () ;
end :
    delete [] xpm ;
    delete [] xpmData ;
}
