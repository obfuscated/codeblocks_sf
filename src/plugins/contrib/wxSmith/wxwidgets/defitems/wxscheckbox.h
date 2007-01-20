#ifndef WXSCHECKBOX_H
#define WXSCHECKBOX_H

#include "../wxswidget.h"

/** \brief Class for wxsCheckBox widget */
class wxsCheckBox: public wxsWidget
{
    public:

        wxsCheckBox(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        wxString Label;
        bool IsChecked;
};

#endif
