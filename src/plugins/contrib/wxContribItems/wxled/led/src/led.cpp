/////////////////////////////////////////////////////////////////////////////
// Name:        led.cpp
// Purpose:     wxLed implementation
// Author:      Thomas Monjalon
// Created:     09/06/2005
// Revision:    09/06/2005
// Licence:     wxWidgets
// mod   by:    Jonas Zinn
// mod date:    24/03/2012
/////////////////////////////////////////////////////////////////////////////

#include "wx/led.h"

#include <string.h>

BEGIN_EVENT_TABLE (wxLed, wxWindow)
    EVT_PAINT (wxLed::OnPaint)
END_EVENT_TABLE ()

wxLed::wxLed(wxWindow * parent, wxWindowID id, wxColour disableColour, wxColour onColour, wxColour offColour, const wxPoint & pos, const wxSize & size)
{
    Create(parent, id, disableColour, onColour, offColour, pos, size);
}

wxLed::wxLed ()
{

}

wxLed::~wxLed ()
{
	delete m_bitmap ;
}

bool wxLed::Create(wxWindow * parent, wxWindowID id, wxColour disableColour, wxColour onColour, wxColour offColour, const wxPoint & pos, const wxSize & size)
{
    if(!wxWindow::Create(parent, id, wxDefaultPosition, wxDefaultSize))
		return false;

    m_bitmap    = NULL;
	m_isEnabled = true ;
	m_isOn      = false;
	m_Disable   = disableColour;
	m_On        = onColour;
	m_Off       = offColour;
	Enable();

	return true;
}

void wxLed::Enable (void)
{
    m_isEnable = true;
	if( m_isOn)
        SetBitmap (m_On.GetAsString( wxC2S_HTML_SYNTAX)) ;
    else
        SetBitmap (m_Off.GetAsString( wxC2S_HTML_SYNTAX)) ;
}

void wxLed::Disable( void)
{
    m_isEnable= false;
    SetBitmap( m_Disable.GetAsString( wxC2S_HTML_SYNTAX));
}

void wxLed::Switch(void)
{
    if( m_isEnable)
    {
        m_isOn = !m_isOn;
        if(m_isOn)
            SetBitmap (m_On.GetAsString( wxC2S_HTML_SYNTAX)) ;
        else
            SetBitmap (m_Off.GetAsString( wxC2S_HTML_SYNTAX)) ;
    }
}

void wxLed::SwitchOn(void)
{
    if( m_isEnable)
    {
        m_isOn = true;
        SetBitmap (m_On.GetAsString( wxC2S_HTML_SYNTAX)) ;
    }
}

void wxLed::SwitchOff( void)
{
    if( m_isEnable)
    {
        m_isOn = false;
        SetBitmap (m_Off.GetAsString( wxC2S_HTML_SYNTAX)) ;
    }
}

void wxLed::SetOnColour (wxColour rgb)
{
	m_On = rgb;
    if (m_isEnabled && m_isOn)
        SetBitmap (m_On.GetAsString( wxC2S_HTML_SYNTAX)) ;

}

void wxLed::SetOffColour(wxColour rgb)
{
	m_Off = rgb;
    if (m_isEnabled && !m_isOn)
        SetBitmap (m_Off.GetAsString( wxC2S_HTML_SYNTAX)) ;
}

void wxLed::SetDisableColour(wxColour rgb)
{
	m_Disable = rgb;
    if (!m_isEnabled)
        SetBitmap (m_Disable.GetAsString( wxC2S_HTML_SYNTAX)) ;
}

void wxLed::SetOnOrOff( bool on)
{

    m_isOn = on;
    if(m_isEnable)
    {
        if( m_isOn)
            SetBitmap (m_On.GetAsString( wxC2S_HTML_SYNTAX)) ;
        else
            SetBitmap (m_Off.GetAsString( wxC2S_HTML_SYNTAX)) ;
    }
}

bool wxLed::IsEnabled( void)
{
    return m_isEnabled;
}

bool wxLed::IsOn(void)
{
    return m_isOn;
}

void wxLed::OnPaint (wxPaintEvent & WXUNUSED (event))
{
	wxPaintDC dc (this) ;
	m_mutex.Lock () ;
	dc.DrawBitmap (* m_bitmap, 0, 0, true) ;
	m_mutex.Unlock () ;
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
    m_mutex.Lock () ;
    delete m_bitmap ;
    m_bitmap = new wxBitmap (xpm) ;
    if (m_bitmap == NULL)
    {
        m_mutex.Unlock () ;
        goto end ;
    }
    SetSize (wxSize (m_bitmap->GetWidth (), m_bitmap->GetHeight ())) ;
    m_mutex.Unlock () ;
    Refresh () ;
end :
    delete [] xpm ;
    delete [] xpmData ;
}
