#ifndef WXSCHECKLISTBOX_H
#define WXSCHECKLISTBOX_H

#include "../wxsdefwidget.h"
#include "../properties/wxsstringlistcheckproperty.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsCheckListBoxStyles)
WXS_EV_DECLARE(wxsCheckListBoxEvents)

wxsDWDeclareBegin(wxsCheckListBoxBase,wxsCheckListBoxId)
    wxArrayString arrayChoices;
    wxsArrayBool arrayChecks;
wxsDWDeclareEnd()

class wxsCheckListBox: public wxsCheckListBoxBase
{
    public:
    
        wxsCheckListBox(wxsWidgetManager* Man,wxsWindowRes* Res):
           wxsCheckListBoxBase(Man,Res)
        {}
        
        virtual wxString GetProducingCode(const wxsCodeParams& Params);
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);

    protected:
    
        virtual bool MyXmlLoad();
        virtual bool MyXmlSave();
        virtual void MyCreateProperties();
};

#endif
