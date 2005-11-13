#include "../wxsheaders.h"
#include "wxsbutton.h"

WXS_ST_BEGIN(wxsButtonStyles)
    WXS_ST_CATEGORY("wxButton")
    WXS_ST(wxBU_LEFT)
    WXS_ST(wxBU_TOP)
    WXS_ST(wxBU_RIGHT)
    WXS_ST(wxBU_BOTTOM)
    WXS_ST(wxBU_EXACTFIT)
// cyberkoa : No in the XRC but in the help file, MSW and GTK only
#ifdef __WXMSW__
    WXS_ST(wxNO_BORDER)
#endif
#ifdef __WXGTK__
    WXS_ST(wxNO_BORDER)
#endif

    WXS_ST_DEFAULTS()
WXS_ST_END(wxsButtonStyles)

WXS_EV_BEGIN(wxsButtonEvents)
    WXS_EVI(EVT_BUTTON,wxCommandEvent,Click)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsButtonEvents)


wxsDWDefineBegin(wxsButton,wxButton,

    ThisWidget = new wxButton(parent,id,label,pos,size,style);
    if (def) ThisWidget->SetDefault();

    )

    wxsDWDefLongStr(label,"Label:","Label");
    wxsDWDefBoolX(def,"default","Default:",false);

wxsDWDefineEnd()
