#include "wxsspinctrl.h"

#include "wx/spinctrl.h"

WXS_ST_BEGIN(wxsSpinCtrlStyles)
    WXS_ST_CATEGORY("wxSpinCtrl")
    WXS_ST(wxSP_ARROW_KEYS)    
    WXS_ST(wxSP_WRAP)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsSpinCtrlStyles)

//wxSpinCtrl(wxWindow* parent, wxWindowID id = -1, const wxString& value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSP_ARROW_KEYS, int min = 0, int max = 100, int initial = 0, const wxString& name = _T("wxSpinCtrl"))


wxsDWDefineBegin(wxsSpinCtrl,wxSpinCtrl,
        ThisWidget = new wxSpinCtrl(parent,id,value,pos,size,style, min ,max);
    )
    wxsDWDefStr(value,"Default:",_T("0"));
    wxsDWDefInt(min,"Min:",0)
    wxsDWDefInt(max,"Max:",100)
wxsDWDefineEnd()
