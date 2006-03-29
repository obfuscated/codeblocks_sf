#ifndef WXSSPACER_H
#define WXSSPACER_H

#include "../wxsitem.h"

class wxsSpacer: public wxsItem
{
    public:

        static wxsItemInfo Info;

        wxsSpacer(wxsWindowRes* Res): wxsItem(Res) {}

        /** \brief Size may be accessed publically */
        wxsSizeData Size;

        /** \brief Getting size */
        inline wxSize GetSize(wxWindow* Parent) { return Size.GetSize(Parent); }

        /** \brief Getting size code */
        wxString GetSizeCode(const wxString& WindowParent,wxsCodingLang Language);

    private:

        virtual const wxsItemInfo& GetInfo() { return Info; }

        virtual void AddItemQPP(wxsAdvQPP* QPP) {}

        virtual void BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language) {}

        virtual void BuildDeclarationCode(wxString& Code,wxsCodingLang Language) {}

        virtual long GetPropertiesFlags();

        wxObject* DoBuildPreview(wxWindow* Parent,bool Exact);

};

#endif
