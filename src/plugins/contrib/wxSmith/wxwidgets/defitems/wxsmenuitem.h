#ifndef WXSMENUITEM_H
#define WXSMENUITEM_H

#include "../wxstool.h"

/** \brief Generic class for any items inside menu
 */
class wxsMenuItem: public wxsTool
{
    public:

        /** \brief Ctor */
        wxsMenuItem(wxsItemResData* Data,bool BreakOrSeparator);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumToolProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language) {}
        virtual bool OnIsPointer() { return true; }
        virtual bool OnCanAddToResource(wxsItemResData* Data,bool ShowMessage) { return false; }
        virtual bool OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        virtual bool OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        virtual bool OnCanAddToParent(wxsParent* Parent,bool ShowMessage);
        virtual bool OnXmlReadChild(TiXmlElement* Elem,bool IsXRC,bool IsExtra);
        virtual void OnBuildDeclarationCode(wxString& Code,wxsCodingLang Language);
        virtual wxString OnGetTreeLabel(int& Image);

        enum Type                                   ///< \brief Type of menu (or it's item)
        {
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
