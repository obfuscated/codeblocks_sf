#include "wxstogglebutton.h"

#include <wx/tglbtn.h>

WXS_ST_BEGIN(wxsToggleButtonStyles)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsToggleButtonStyles)

wxsDWDefineBegin(wxsToggleButton,wxToggleButton,
    ThisWidget = new wxToggleButton(parent,id,label,pos,size,style);
    ThisWidget->SetValue(checked);
    )
    wxsDWDefStr(label,"Label:","Label");
    wxsDWDefBool(checked,"Checked:",false);
wxsDWDefineEnd()
