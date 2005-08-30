#ifndef __WXSSTATICTEXT_H
#define __WXSSTATICTEXT_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsStaticTextStyles)
WXS_EV_DECLARE(wxsStaticTextEvents)

wxsDWDeclareBegin(wxsStaticText,propWidget,wxsStaticTextId)
    wxString label;
wxsDWDeclareEnd()


#endif
