#ifndef WXSSLIDER_H
#define WXSSLIDER_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsSliderStyles)
WXS_EV_DECLARE(wxsSliderEvents)

wxsDWDeclareBegin(wxsSlider,wxsSliderId)
    int value;
    int min;
    int max;
    int tickfreq;
    int pagesize;
    int linesize;
    int thumb;
    int tick;
    int selmin;
    int selmax;
wxsDWDeclareEnd()


#endif
