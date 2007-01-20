#ifndef WXSDIALOG_H
#define WXSDIALOG_H

#include "../wxscontainer.h"

class wxsDialog : public wxsContainer
{
    public:

        wxsDialog(wxsItemResData* Data);

    private:

        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumContainerProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        wxString Title;
        bool Centered;
};

#endif
