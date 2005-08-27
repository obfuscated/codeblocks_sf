#ifndef __WXSRADIOBOX_H
#define __WXSRADIOBOX_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsRadioBoxStyles)

wxsDWDeclareBegin(wxsRadioBox,propWidget,wxsRadioBoxId)
    wxString label;
    wxArrayString arrayChoices;
    int defaultChoice;    
wxsDWDeclareEnd()



#endif
