#ifndef WXSDIALOG_H
#define WXSDIALOG_H

#include "../wxscontainer.h"

class wxsDialog : public wxsContainer
{
    public:

        /** \brief Widget's info, in public scope because it will be accessed
         *         from widget manager
         */
        static wxsItemInfo Info;

        /** \brief Ctor */
        wxsDialog(wxsWindowRes* Resource);

        /** \brief Function building creating code */
        virtual void BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);

    protected:

        /** \brief Function building preview */
        virtual wxObject* DoBuildPreview(wxWindow* Parent,bool Exact);

        /** \brief Function enumerating properties specific for this item  */
        virtual void EnumContainerProperties(long Flags);

        /** \brief Enumerating header files */
        virtual void EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language){};
        virtual long GetPropertiesFlags();

    private:

        wxString Title;
        bool Centered;
};

#endif
