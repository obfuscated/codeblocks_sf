#include "wxsgauge.h"

WXS_ST_BEGIN(wxsGaugeStyles)
    WXS_ST(wxGA_HORIZONTAL)
    WXS_ST(wxGA_VERTICAL)
    WXS_ST(wxGA_SMOOTH)
WXS_ST_END(wxsGaugeStyles)

wxsDWDefineBegin(wxsGauge,wxGauge,
    ThisWidget = new wxGauge(parent,id,range,pos,size,style);
    if ( value ) ThisWidget->SetValue(value);
    if ( shadow ) ThisWidget->SetValue(shadow);
    if ( bezel ) ThisWidget->SetValue(bezel);
    )
    wxsDWDefInt(range,"Range:",100);
    wxsDWDefInt(value,"Value:",0);
    wxsDWDefInt(shadow,"Shadow width:",0);
    wxsDWDefInt(bezel,"Bezel face width:",0);
wxsDWDefineEnd()
