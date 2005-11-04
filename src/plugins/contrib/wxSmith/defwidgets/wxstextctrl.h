#ifndef WXSTEXTCTRL_H
#define WXSTEXTCTRL_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsTextCtrlStyles)
WXS_EV_DECLARE(wxsTextCtrlEvents)

wxsDWDeclareBegin(wxsTextCtrl,wxsTextCtrlId)
    wxString value;
    int maxlength;
wxsDWDeclareEnd()

#endif // WXSTEXTCTRL_H
