#ifndef WXSCOMBOBOX_H
#define WXSCOMBOBOX_H

#include "../wxswidget.h"

/** \brief Class for wxsComboBox widget */
class wxsComboBox: public wxsWidget
{
    public:

        wxsComboBox(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);


        wxArrayString ArrayChoices;
        int DefaultSelection;
};

#endif
