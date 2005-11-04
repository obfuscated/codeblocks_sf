#ifndef __WXSSPINCTRL_H
#define __WXSSPINCTRL_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsSpinCtrlStyles)
WXS_EV_DECLARE(wxsSpinCtrlEvents)

wxsDWDeclareBegin(wxsSpinCtrl,wxsSpinCtrlId)
    wxString value;
    int min;
    int max;
wxsDWDeclareEnd()


#endif
