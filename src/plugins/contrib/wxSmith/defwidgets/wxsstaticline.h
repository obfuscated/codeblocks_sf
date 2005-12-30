#ifndef __WXSSTATICLINE_H
#define __WXSSTATICLINE_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsStaticLineStyles)
WXS_EV_DECLARE(wxsStaticLineEvents)

wxsDWDeclareBegin(wxsStaticLineBase,wxsStaticLineId)
wxsDWDeclareEnd()

class wxsStaticLine: public wxsStaticLineBase
{
    public:
        wxsStaticLine(wxsWidgetManager* Man,wxsWindowRes* Res);

    protected:
        virtual bool PropertiesUpdated(bool Validate,bool Correct);
};


#endif
