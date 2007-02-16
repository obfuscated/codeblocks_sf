#ifndef WXSMENU_H
#define WXSMENU_H

#include "../wxstool.h"

/** \brief handler for menus */
class wxsMenu: public wxsTool
{
    public:

        wxsMenu(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumToolProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);
        virtual bool OnIsPointer();
        virtual bool OnCanAddToResource(wxsItemResData* Data,bool ShowMessage) { return true; }
        virtual bool OnMouseDClick(wxWindow* Preview,int PosX,int PosY);
        virtual bool OnCanAddChild(wxsItem* Item,bool ShowMessage);
        virtual bool OnCanAddToParent(wxsParent* Item,bool ShowMessage);
        virtual bool OnXmlReadChild(TiXmlElement* Elem,bool IsXRC,bool IsExtra);
        virtual wxString OnGetTreeLabel();

//        wxSize CalculateSize(wxArrayInt* Cols = 0); ///< \brief Getting size of this item requied by editor
//        void   Draw(wxDC* DC,int BegX,int BegY);    ///< \brief Drawing preview of this menu in some DC

        wxString m_Label;                           ///< \brief Main label

        friend class wxsMenuEditor;
};

#endif
