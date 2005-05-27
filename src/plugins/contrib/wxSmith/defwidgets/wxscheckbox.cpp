#include "wxscheckbox.h"

WXS_ST_BEGIN(wxsCheckBoxStyles)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsCheckBoxStyles)

wxsDWDefineBegin(wxsCheckBox,wxCheckBox,

    ThisWidget = new wxCheckBox(parent,id,label,pos,size,style);
    ThisWidget->SetValue(checked);
    
    )

    wxsDWDefStrP(label,"Label:","");
    wxsDWDefBoolP(checked,"Checked:",false);

wxsDWDefineEnd()
