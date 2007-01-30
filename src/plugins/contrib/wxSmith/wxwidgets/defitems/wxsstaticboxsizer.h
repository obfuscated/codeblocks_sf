#ifndef WXSSTATICBOXSIZER_H
#define WXSSTATICBOXSIZER_H

#include "../wxssizer.h"

class wxsStaticBoxSizer: public wxsSizer
{
    public:

        wxsStaticBoxSizer(wxsItemResData* Data);

    private:

        virtual wxSizer* OnBuildSizerPreview(wxWindow* Parent);
        virtual void OnBuildSizerCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumSizerProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);
        virtual bool OnIsPointer() { return true; }

        long Orient;
        wxString Label;
};

#endif
