#include "../wxsheaders.h"
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
    WXS_THIS = new wxToggleButton(WXS_PARENT,WXS_ID,label,WXS_POS,WXS_SIZE,WXS_STYLE,wxDefaultValidator,WXS_NAME);
    WXS_THIS->SetValue(checked);
    )
    wxsDWDefLongStr(label,"Label:","Label");
    wxsDWDefBool(checked,"Checked:",false);
wxsDWDefineEnd()
