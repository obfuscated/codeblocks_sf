#ifndef WXSBOXSIZER_H
#define WXSBOXSIZER_H

#include "../wxssizer.h"

/** \brief Class for wxBoxSizer item */
class wxsBoxSizer: public wxsSizer
{
    public:

        wxsBoxSizer(wxsItemResData* Data);

    private:

        virtual wxSizer* OnBuildSizerPreview(wxWindow* Parent);
        virtual void OnBuildSizerCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumSizerProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        long Orient;
};

#endif
