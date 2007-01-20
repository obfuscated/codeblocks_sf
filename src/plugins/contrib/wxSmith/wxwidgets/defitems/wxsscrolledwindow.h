#ifndef WXSSCROLLEDWINDOW_H
#define WXSSCROLLEDWINDOW_H

#include "../wxscontainer.h"

class wxsScrolledWindow : public wxsContainer
{
    public:

        wxsScrolledWindow(wxsItemResData* Data);

    protected:

        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumContainerProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);
};

#endif
