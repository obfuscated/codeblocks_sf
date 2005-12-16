#ifndef __WXSSTATICBOX_H
#define __WXSSTATICBOX_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsStaticBoxStyles)
WXS_EV_DECLARE(wxsStaticBoxEvents)

wxsDWDeclareBegin(wxsStaticBox,wxsStaticBoxId)
    wxString label;
wxsDWDeclareEnd()

#endif
