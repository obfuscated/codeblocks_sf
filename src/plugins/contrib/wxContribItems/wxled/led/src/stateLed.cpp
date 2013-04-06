#include "wx/stateLed.h"

#include <string.h>

BEGIN_EVENT_TABLE (wxStateLed , wxWindow)
    EVT_PAINT (wxStateLed ::OnPaint)
END_EVENT_TABLE ()

wxStateLed ::wxStateLed (wxWindow * parent, wxWindowID id, wxColour disabledColor, const wxPoint & pos, const wxSize & size)
{
    Create(parent, id, disabledColor, pos, size);
}

wxStateLed ::wxStateLed  ()
{
}

wxStateLed ::~wxStateLed  ()
{
	delete m_bitmap ;
}

bool wxStateLed ::Create(wxWindow * parent, wxWindowID id, wxColour disabledColor, const wxPoint & pos, const wxSize & size)
{
    if(!wxWindow::Create(parent, id, wxDefaultPosition, wxDefaultSize))
		return false;

    m_bitmap    = NULL;
	m_isEnabled = true ;
	m_Disable   = disabledColor;
	m_state     = 0;
	Enable();
	return true;
}

void wxStateLed ::Enable (void)
{
    if( !m_registeredState.empty())
    {
        m_isEnable = true;
        SetBitmap (m_registeredState[m_state].GetAsString( wxC2S_HTML_SYNTAX)) ;
    }
    else
        SetBitmap( m_Disable.GetAsString( wxC2S_HTML_SYNTAX));
}

void wxStateLed ::Disable( void)
{
    m_isEnable= false;
    SetBitmap( m_Disable.GetAsString( wxC2S_HTML_SYNTAX));
}

void wxStateLed::RegisterState(int state, wxColour colour)
{
    m_registeredState[state] = colour;
}

void wxStateLed::SetState(int state)
{
    m_state = state;
    if(m_isEnable)
        SetBitmap (m_registeredState[m_state].GetAsString( wxC2S_HTML_SYNTAX)) ;
}

int wxStateLed::GetState()
{
    return m_state;
}

void wxStateLed ::SetDisableColor(wxColour rgb)
{
	m_Disable = rgb;
    if (!m_isEnabled)
        SetBitmap (m_Disable.GetAsString( wxC2S_HTML_SYNTAX)) ;
}

bool wxStateLed ::IsEnabled( void)
{
    return m_isEnabled;
}

void wxStateLed ::OnPaint (wxPaintEvent & WXUNUSED (event))
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

void wxStateLed ::SetBitmap (wxString color)
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
