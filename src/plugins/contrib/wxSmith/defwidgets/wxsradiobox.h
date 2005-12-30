#ifndef __WXSRADIOBOX_H
#define __WXSRADIOBOX_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsRadioBoxStyles)
WXS_EV_DECLARE(wxsRadioBoxEvents)

wxsDWDeclareBegin(wxsRadioBoxBase,wxsRadioBoxId)
    wxString label;
    wxArrayString arrayChoices;
    int defaultChoice;
    int dimension;
wxsDWDeclareEnd()

class wxsRadioBox: public wxsRadioBoxBase
{
	public:
        wxsRadioBox(wxsWidgetManager* Man,wxsWindowRes* Res):
            wxsRadioBoxBase(Man,Res)
        {}

    protected:

        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        virtual wxString GetProducingCode(wxsCodeParams& Params);
};

#endif
