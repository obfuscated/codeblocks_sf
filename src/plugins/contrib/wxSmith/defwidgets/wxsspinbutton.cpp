#include "wxsspinbutton.h"

#include <wx/spinbutt.h>

WXS_ST_BEGIN(wxsSpinButtonStyles)
    WXS_ST_CATEGORY("wxSpinButton")
#ifndef __WXGTK__    
    WXS_ST(wxSP_HORIZONTAL)    
#endif    
    WXS_ST(wxSP_VERTICAL)
    WXS_ST(wxSP_ARROW_KEYS)    
    WXS_ST(wxSP_WRAP)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsSpinButtonStyles)


//wxSpinButton(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSP_HORIZONTAL, const wxString& name = "spinButton")

wxsDWDefineBegin(wxsSpinButton,wxSpinButton,
        ThisWidget = new wxSpinButton(parent,id,pos,size,style);
        ThisWidget->SetRange(min,max);
        ThisWidget->SetValue(value);
    )
    wxsDWDefInt(value,"Default:",0);
    wxsDWDefInt(min,"Min:",0)
    wxsDWDefInt(max,"Max:",100)

wxsDWDefineEnd()
