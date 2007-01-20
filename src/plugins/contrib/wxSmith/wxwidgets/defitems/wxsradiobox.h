#ifndef WXSRADIOBOX_H
#define WXSRADIOBOX_H

#include "../wxswidget.h"

/** \brief Class for wxsRadioBox widget */
class wxsRadioBox: public wxsWidget
{
    public:

        wxsRadioBox(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        wxString Label;
        wxArrayString ArrayChoices;
        int DefaultSelection;
        int Dimension;
};

#endif
