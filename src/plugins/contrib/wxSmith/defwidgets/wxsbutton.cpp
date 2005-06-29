#include "wxsbutton.h"

WXS_ST_BEGIN(wxsButtonStyles)
    WXS_ST_CATEGORY("wxButton")
    WXS_ST(wxBU_LEFT)
    WXS_ST(wxBU_TOP)
    WXS_ST(wxBU_RIGHT)
    WXS_ST(wxBU_BOTTOM)
    WXS_ST(wxBU_EXACTFIT)
    WXS_ST(wxNO_BORDER)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsButtonStyles)

wxsDWDefineBeginExt(wxsButton,wxButton,

    ThisWidget = new wxButton(parent,id,label,pos,size,style);
    if (def) ThisWidget->SetDefault();
    
    ,true)

    wxsDWDefStr(label,"Label:","");
    wxsDWDefBoolX(def,"default","Default:",false);

wxsDWDefineEnd()
