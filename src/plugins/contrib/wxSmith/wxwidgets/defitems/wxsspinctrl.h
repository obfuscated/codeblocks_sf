#ifndef WXSSPINCTRL_H
#define WXSSPINCTRL_H

#include "../wxswidget.h"

/** \brief Class for wxsSpinCtrl widget */
class wxsSpinCtrl: public wxsWidget
{
    public:

        wxsSpinCtrl(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        wxString Value;
        int Min;
        int Max;

};

#endif
