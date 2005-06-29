#include "wxscheckbox.h"

WXS_ST_BEGIN(wxsCheckBoxStyles)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsCheckBoxStyles)

wxsDWDefineBeginExt(wxsCheckBox,wxCheckBox,

    ThisWidget = new wxCheckBox(parent,id,label,pos,size,style);
    ThisWidget->SetValue(checked);
    
    ,true)

    wxsDWDefStr(label,"Label:","");
    wxsDWDefBool(checked,"Checked:",false);

wxsDWDefineEnd()
