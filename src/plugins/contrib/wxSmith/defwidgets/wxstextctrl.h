#ifndef WXSTEXTCTRL_H
#define WXSTEXTCTRL_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsTextCtrlStyles)

wxsDWDeclareBegin(wxsTextCtrl,propWidget,wxsTextCtrlId)
    wxString value;
    int maxlength;
wxsDWDeclareEnd()

#endif // WXSTEXTCTRL_H
