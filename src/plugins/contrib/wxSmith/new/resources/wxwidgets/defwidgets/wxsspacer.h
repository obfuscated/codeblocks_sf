#ifndef WXSSPACER_H
#define WXSSPACER_H

#include "../wxsitem.h"

class wxsSpacer: public wxsItem
{
    public:

        static wxsItemInfo Info;

        wxsSpacer(wxsWindowRes* Res): wxsItem(Res) {}

    private:

        virtual const wxsItemInfo& GetInfo() { return Info; }
        virtual void BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void BuildDeclarationCode(wxString& Code,wxsCodingLang Language) {}
        virtual long GetPropertiesFlags();
        virtual void EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language) {}

        wxObject* DoBuildPreview(wxWindow* Parent,bool Exact);

        wxsSizeData Size;

};

#endif
