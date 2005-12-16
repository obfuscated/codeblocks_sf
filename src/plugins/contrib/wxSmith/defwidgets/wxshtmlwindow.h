#ifndef __WXSHTMLWINDOW_H
#define __WXSHTMLWINDOW_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsHtmlWindowStyles)
WXS_EV_DECLARE(wxsHtmlWindowEvents)

wxsDWDeclareBegin(wxsHtmlWindowBase,wxsHtmlWindowId)
    int borders;
    wxString url;
    wxString htmlcode;
wxsDWDeclareEnd()

class WXSCLASS wxsHtmlWindow: public wxsHtmlWindowBase
{
    public:
        wxsHtmlWindow(wxsWidgetManager* Man,wxsWindowRes* Res):
            wxsHtmlWindowBase(Man,Res)
        {}

    protected:
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        virtual wxString GetProducingCode(wxsCodeParams& Params);
};

#endif
