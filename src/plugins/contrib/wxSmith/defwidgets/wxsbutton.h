#ifndef __WXSBUTTON_H
#define __WXSBUTTON_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsButtonStyles)
WXS_EV_DECLARE(wxsButtonEvents)

wxsDWDeclareBegin(wxsButton,wxsButtonId)
    wxString label;
    bool def;
wxsDWDeclareEnd()


#endif
