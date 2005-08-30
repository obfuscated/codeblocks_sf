#ifndef __WXSCHECKBOX_H
#define __WXSCHECKBOX_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsCheckBoxStyles)
WXS_EV_DECLARE(wxsCheckBoxEvents)

wxsDWDeclareBegin(wxsCheckBox,propWidget,wxsCheckBoxId)
    wxString label;
    bool checked;
wxsDWDeclareEnd()



#endif
