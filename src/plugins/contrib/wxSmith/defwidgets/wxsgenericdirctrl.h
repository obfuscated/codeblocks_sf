#ifndef __WXSGENERICDIRCTRL_H
#define __WXSGENERICDIRCTRL_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsGenericDirCtrlStyles)
WXS_EV_DECLARE(wxsGenericDirCtrlEvents)

wxsDWDeclareBegin(wxsGenericDirCtrl,wxsGenericDirCtrlId)
    wxString defaultfolder;
    wxString filter;
    int defaultfilter;
wxsDWDeclareEnd()

#endif
