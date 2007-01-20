#ifndef WXSTOGGLEBUTTON_H
#define WXSTOGGLEBUTTON_H

#include "../wxswidget.h"

/** \brief Class for wxsToggleButton widget */
class wxsToggleButton: public wxsWidget
{
    public:

        wxsToggleButton(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        wxString Label;
	    bool IsChecked;

};

#endif
