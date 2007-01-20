#ifndef WXSDATEPICKERCTRL_H
#define WXSDATEPICKERCTRL_H

#include "../wxswidget.h"

/** \brief Class for wxsDatePickerCtrl widget */
class wxsDatePickerCtrl: public wxsWidget
{
    public:

        wxsDatePickerCtrl(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

};

#endif
