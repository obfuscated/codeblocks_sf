#ifndef WXSBOXSIZER_H
#define WXSBOXSIZER_H

#include "../wxssizer.h"

/** \brief Class for wxBoxSizer item */
class wxsBoxSizer: public wxsSizer
{
    public:

        static wxsItemInfo Info;

        wxsBoxSizer(wxsWindowRes* Resource): wxsSizer(Resource), Orient(wxHORIZONTAL) {}

        virtual const wxsItemInfo& GetInfo() { return Info; }

    protected:

        virtual wxSizer* BuildSizerPreview(wxWindow* Parent);
        virtual void BuildSizerCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void EnumItemProperties(long Flags);
        virtual void EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

    private:

        long Orient;
};

#endif
