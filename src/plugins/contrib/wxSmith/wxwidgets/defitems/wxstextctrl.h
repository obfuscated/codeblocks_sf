#ifndef WXSTEXTCTRL_H
#define WXSTEXTCTRL_H

#include "../wxswidget.h"

/** \brief Class for wxsTextCtrl widget */
class wxsTextCtrl: public wxsWidget
{
    public:

        wxsTextCtrl(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        wxString Text;
        int MaxLength;
};

#endif
