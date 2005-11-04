#ifndef __WXSSCROLLBAR_H
#define __WXSSCROLLBAR_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsScrollBarStyles)
WXS_EV_DECLARE(wxsScrollBarEvents)

wxsDWDeclareBegin(wxsScrollBar,wxsScrollBarId)
    int value;
    int thumbsz;
    int range;
    int pagesz;
wxsDWDeclareEnd()


#endif
