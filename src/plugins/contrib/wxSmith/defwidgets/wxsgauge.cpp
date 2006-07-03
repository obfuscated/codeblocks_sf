#include "../wxsheaders.h"
#include "wxsgauge.h"

WXS_ST_BEGIN(wxsGaugeStyles)
    WXS_ST(wxGA_HORIZONTAL)
    WXS_ST(wxGA_VERTICAL)
// NOTE (cyberkoa#1#): according to Help file , wxGA_SMOOTH is not support by all platform but does not mention which platform is not support.
    WXS_ST(wxGA_SMOOTH)
// NOTE (cyberkoa##): wxGA_PROGRESSBAR not in HELP file but in XRC code
//    WXS_ST(wxGA_PROGRESSBAR)
WXS_ST_END(wxsGaugeStyles)

WXS_EV_BEGIN(wxsGaugeEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsGaugeEvents)

wxsDWDefineBegin(wxsGauge,wxGauge,
    WXS_THIS = new wxGauge(WXS_PARENT,WXS_ID,range,WXS_POS,WXS_SIZE,WXS_STYLE,wxDefaultValidator,WXS_NAME);
    if ( value ) WXS_THIS->SetValue(value);
    if ( shadow ) WXS_THIS->SetShadowWidth(shadow);
    if ( bezel ) WXS_THIS->SetBezelFace(bezel);
    )
    wxsDWDefInt(range,"Range:",100);
    wxsDWDefInt(value,"Value:",0);
    wxsDWDefInt(shadow,"Shadow width:",0);
    wxsDWDefInt(bezel,"Bezel face width:",0);
wxsDWDefineEnd()
