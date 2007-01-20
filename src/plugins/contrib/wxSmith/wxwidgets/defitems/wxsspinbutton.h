#ifndef WXSSPINBUTTONL_H
#define WXSSPINBUTTON_H

#include "../wxswidget.h"

/** \brief Class for wxsSpinButton widget */
class wxsSpinButton: public wxsWidget
{
    public:

        wxsSpinButton(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        int Value;
        int Min;
        int Max;
};

#endif
