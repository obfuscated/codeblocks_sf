#ifndef WXSPANELR_H
#define WXSPANELR_H

#include "../wxscontainer.h"

class wxsPanelr : public wxsContainer
{
    public:

        /** \brief Widget's info, in public scope because it will be accessed
         *         from widget manager
         */
        static wxsItemInfo Info;

        /** \brief Ctor */
        wxsPanelr(wxsWindowRes* Resource);

        /** \brief Function building creating code */
        virtual void BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);

    protected:

        /** \brief Function building preview */
        virtual wxObject* DoBuildPreview(wxWindow* Parent,bool Exact);

        /** \brief Function enumerating properties specific for this item  */
        virtual void EnumContainerProperties(long Flags);

        virtual void EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language) {}

        virtual long GetPropertiesFlags();

};

#endif
