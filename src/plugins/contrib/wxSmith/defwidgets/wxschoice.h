#ifndef __WXSCHOICE_H
#define __WXSCHOICE_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsChoiceStyles)
WXS_EV_DECLARE(wxsChoiceEvents)

wxsDWDeclareBegin(wxsChoiceBase,wxsChoiceId)
    wxArrayString arrayChoices;
    int defaultChoice;
wxsDWDeclareEnd()

class wxsChoice: public wxsChoiceBase
{
    public:
        wxsChoice(wxsWidgetManager* Man,wxsWindowRes* Res):
            wxsChoiceBase(Man,Res)
        {}

    protected:
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        virtual wxString GetProducingCode(const wxsCodeParams& Params);
};

#endif
