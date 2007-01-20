#ifndef WXSRADIOBUTTON_H
#define WXSRADIOBUTTON_H

#include "../wxswidget.h"

/** \brief Class for wxsRadioButton widget */
class wxsRadioButton: public wxsWidget
{
    public:

        wxsRadioButton(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        wxString Label;
        bool IsSelected;
};

#endif
