#ifndef WXSGAUGE_H
#define WXSGAUGE_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsGaugeStyles)

wxsDWDeclareBegin(wxsGauge,propWidget,wxsGaugeId)
    int range;
    int value;
    int shadow;
    int bezel;
wxsDWDeclareEnd()


#endif // WXSGAUGE_H
