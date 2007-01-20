#ifndef WXSSPACER_H
#define WXSSPACER_H

#include "../wxsitem.h"

class wxsSpacer: public wxsItem
{
    public:

        wxsSpacer(wxsItemResData* Data);

    private:

        virtual void OnEnumItemProperties(long Flags);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnBuildDeclarationCode(wxString& Code,wxsCodingLang Language) {}
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language) {}
};

#endif
