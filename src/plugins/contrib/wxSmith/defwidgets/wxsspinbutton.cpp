#include "../wxsheaders.h"
#include "wxsspinbutton.h"

#include <wx/spinbutt.h>

WXS_ST_BEGIN(wxsSpinButtonStyles)
    WXS_ST_CATEGORY("wxSpinButton")
    WXS_ST_MASK(wxSP_HORIZONTAL,wxsSFAll,wxsSFGTK,true)
    WXS_ST(wxSP_VERTICAL)
    WXS_ST(wxSP_ARROW_KEYS)
    WXS_ST(wxSP_WRAP)
 //   WXS_ST_DEFAULTS()
WXS_ST_END(wxsSpinButtonStyles)

WXS_EV_BEGIN(wxsSpinButtonEvents)
    WXS_EVI(EVT_SPIN,wxSpinEvent,Change)
    WXS_EVI(EVT_SPIN_UP,wxSpinEvent,ChangeUp)
    WXS_EVI(EVT_SPIN_DOWN,wxSpinEvent,ChangeDown)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsSpinButtonEvents)


//wxSpinButton(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSP_HORIZONTAL, const wxString& name = "spinButton")

wxsDWDefineBegin(wxsSpinButton,wxSpinButton,
        WXS_THIS = new wxSpinButton(WXS_PARENT,WXS_ID,WXS_POS,WXS_SIZE,WXS_STYLE,WXS_NAME);
        WXS_THIS->SetRange(min,max);
        WXS_THIS->SetValue(value);
    )
    wxsDWDefInt(value,"Default:",0);
    wxsDWDefInt(min,"Min:",0)
    wxsDWDefInt(max,"Max:",100)

wxsDWDefineEnd()
