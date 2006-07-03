#include "../wxsheaders.h"
#include "wxsspinctrl.h"

#include "wx/spinctrl.h"

WXS_ST_BEGIN(wxsSpinCtrlStyles)
    WXS_ST_CATEGORY("wxSpinCtrl")

// NOTE (cyberkoa##): wxSP_HORIZONTAL, wxSP_VERTICAL are not found in HELP but in wxMSW's XRC. Assume same as spinbutton
    WXS_ST_MASK(wxSP_HORIZONTAL,wxsSFAll,wxsSFGTK,true)
    WXS_ST(wxSP_VERTICAL)

    WXS_ST(wxSP_ARROW_KEYS)
    WXS_ST(wxSP_WRAP)
//    WXS_ST_DEFAULTS()
WXS_ST_END(wxsSpinCtrlStyles)

WXS_EV_BEGIN(wxsSpinCtrlEvents)
    WXS_EVI(EVT_SPINCTRL,wxSpinEvent,Change)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsSpinCtrlEvents)

//wxSpinCtrl(wxWindow* parent, wxWindowID id = -1, const wxString& value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSP_ARROW_KEYS, int min = 0, int max = 100, int initial = 0, const wxString& name = _T("wxSpinCtrl"))


wxsDWDefineBegin(wxsSpinCtrl,wxSpinCtrl,
        WXS_THIS = new wxSpinCtrl(WXS_PARENT,WXS_ID,value,WXS_POS,WXS_SIZE,WXS_STYLE,min,max,0,WXS_NAME);
    )
    wxsDWDefStr(value,"Default:","0");
    wxsDWDefInt(min,"Min:",0)
    wxsDWDefInt(max,"Max:",100)
wxsDWDefineEnd()
