#ifndef __WXSRADIOBUTTON_H
#define __WXSRADIOBUTTON_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsRadioButtonStyles)
WXS_EV_DECLARE(wxsRadioButtonEvents)

wxsDWDeclareBegin(wxsRadioButton,wxsRadioButtonId)
    wxString label;
    bool selected;
wxsDWDeclareEnd()


#endif
