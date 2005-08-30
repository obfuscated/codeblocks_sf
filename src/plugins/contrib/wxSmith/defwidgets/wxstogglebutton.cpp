#include "wxstogglebutton.h"

#include <wx/tglbtn.h>

WXS_ST_BEGIN(wxsToggleButtonStyles)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsToggleButtonStyles)

WXS_EV_BEGIN(wxsToggleButtonEvents)
    WXS_EVI(EVT_TOGGLEBUTTON,wxCommandEvent,Toggle)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsToggleButtonEvents)

wxsDWDefineBegin(wxsToggleButton,wxToggleButton,
    ThisWidget = new wxToggleButton(parent,id,label,pos,size,style);
    ThisWidget->SetValue(checked);
    )
    wxsDWDefStr(label,"Label:","Label");
    wxsDWDefBool(checked,"Checked:",false);
wxsDWDefineEnd()
