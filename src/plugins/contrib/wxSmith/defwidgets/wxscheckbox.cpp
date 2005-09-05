#include "wxscheckbox.h"

WXS_ST_BEGIN(wxsCheckBoxStyles)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsCheckBoxStyles)

WXS_EV_BEGIN(wxsCheckBoxEvents)
    WXS_EVI(EVT_CHECKBOX,wxCommandEvent,Change)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsCheckBoxEvents)

wxsDWDefineBegin(wxsCheckBox,wxCheckBox,

    ThisWidget = new wxCheckBox(parent,id,label,pos,size,style);
    ThisWidget->SetValue(checked);
    
    )

    wxsDWDefLongStr(label,"Label:","Label");
    wxsDWDefBool(checked,"Checked:",false);

wxsDWDefineEnd()
