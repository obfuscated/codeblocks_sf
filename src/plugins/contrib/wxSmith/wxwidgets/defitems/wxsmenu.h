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
        virtual bool OnIsPointer() { return false; }
        virtual bool OnCanAddToResource(wxsItemResData* Data,bool ShowMessage) { return true; }
        virtual bool OnMouseDClick(wxWindow* Preview,int PosX,int PosY);

        wxSize CalculateSize(wxArrayInt* Cols = 0); ///< \brief Getting size of this item requied by editor
        void   Draw(wxDC* DC,int BegX,int BegY);    ///< \brief Drawing preview of this menu in some DC

        enum Type                                   ///< \brief Type of menu (or it's item)
        {
            Menu,                                   ///< \brief Sub-menu (default for wxMenu class)
            Normal,                                 ///< \brief Normal menu item (can be child of wxMenu with type 'Menu' only)
            Radio,                                  ///< \brief Menu item with radio box (can be child of wxMenu with type 'Menu' only)
            Check,                                  ///< \brief Menu item with check box (can be child of wxMenu with type 'Menu' only)
            Separator,                              ///< \brief Separator (can be child of wxMenu with type 'Menu' only)
            Break                                   ///< \brief Break in menu (can be child of wxMenu with type 'Menu' only)
        };

        Type     m_Type;                            ///< \brief Type of this part of menu
        wxString m_Label;                           ///< \brief Main label
        wxString m_Accelerator;                     ///< \brief Accelerator (can be used in internal menu items only)
        wxString m_Help;                            ///< \brief Help string
        bool     m_Enabled;                         ///< \brief True to make item enabled
        bool     m_Checked;                         ///< \brief True to make this item checked

        friend class wxsMenuEditor;
};

#endif
