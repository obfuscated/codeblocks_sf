#ifndef __WXSSPINBUTTON_H
#define __WXSSPINBUTTON_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsSpinButtonStyles)
WXS_EV_DECLARE(wxsSpinButtonEvents)

wxsDWDeclareBegin(wxsSpinButton,propWidget,wxsSpinButtonId)
    int value;
    int min;
    int max;
wxsDWDeclareEnd()


#endif
