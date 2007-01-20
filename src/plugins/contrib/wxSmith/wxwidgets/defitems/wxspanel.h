#ifndef WXSPANEL_H
#define WXSPANEL_H

#include "../wxscontainer.h"

class wxsPanel : public wxsContainer
{
    public:

        wxsPanel(wxsItemResData* Data);

    protected:

        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumContainerProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);
};

#endif
