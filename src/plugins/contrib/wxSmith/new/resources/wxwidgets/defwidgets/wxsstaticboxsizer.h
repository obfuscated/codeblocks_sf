#ifndef WXSSTATICBOXSIZER_H
#define WXSSTATICBOXSIZER_H

#include "../wxssizer.h"

class wxsStaticBoxSizer: public wxsSizer
{
    public:

        static wxsItemInfo Info;

        wxsStaticBoxSizer(wxsWindowRes* Resource): wxsSizer(Resource), Orient(wxHORIZONTAL), Label(_("Label")) {}

        virtual const wxsItemInfo& GetInfo() { return Info; }

    protected:

        virtual wxSizer* BuildSizerPreview(wxWindow* Parent);
        virtual void BuildSizerCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void EnumItemProperties(long Flags);
        virtual void EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

    private:

        long Orient;
        wxString Label;
};

#endif
