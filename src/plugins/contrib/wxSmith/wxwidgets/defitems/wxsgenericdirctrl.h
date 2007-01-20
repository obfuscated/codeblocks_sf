#ifndef WXSGENERICDIRCTRL_H
#define WXSGENERICDIRCTRL_H

#include "../wxswidget.h"

/** \brief Class for wxsGenericDirCtrl widget */
class wxsGenericDirCtrl: public wxsWidget
{
    public:

        wxsGenericDirCtrl(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        wxString DefaultFolder;
    	wxString Filter;
    	int DefaultFilter;
};

#endif
