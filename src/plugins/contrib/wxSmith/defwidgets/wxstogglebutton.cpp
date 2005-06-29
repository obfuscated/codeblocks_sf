#include "wxstogglebutton.h"

#include <wx/tglbtn.h>

WXS_ST_BEGIN(wxsToggleButtonStyles)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsToggleButtonStyles)

wxsDWDefineBeginExt(wxsToggleButton,wxToggleButton,
    ThisWidget = new wxToggleButton(parent,id,label,pos,size,style);
    ThisWidget->SetValue(checked);
    ,true)
    wxsDWDefStr(label,"Label:","");
    wxsDWDefBool(checked,"Checked:",false);
wxsDWDefineEnd()
