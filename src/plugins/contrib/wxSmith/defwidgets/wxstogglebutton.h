#ifndef __WXSTOGGLEBUTTON_H
#define __WXSTOGGLEBUTTON_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsToggleButtonStyles)

wxsDWDeclareBegin(wxsToggleButton,propWidget,wxsToggleButtonId)
    wxString label;
    bool checked;
wxsDWDeclareEnd()


#endif
