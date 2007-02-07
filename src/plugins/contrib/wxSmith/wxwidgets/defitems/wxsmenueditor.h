#ifndef WXSMENUEDITOR_H
#define WXSMENUEDITOR_H

//(*Headers(wxsMenuEditor)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/treectrl.h>
//*)

#include "wxsmenu.h"

class wxsMenuBar;

class wxsMenuEditor: public wxPanel
{
	public:

        /** \brief Ctor for wxsMenuBar */
		wxsMenuEditor(wxWindow* parent,wxsMenuBar* MenuBar);

		/** \brief Ctor for wxsMenu */
		wxsMenuEditor(wxWindow* parent,wxsMenu* Menu);

		/** \brief Dctor */
		virtual ~wxsMenuEditor();

		/** \brief Function applying current edited content into real wxsMenu or wxsMenuBar object */
		void ApplyChanges();

	private:

        typedef wxsMenu::Type Type;

        /** \brief Data of menu item
         *
         * This is almost copy of wxMenu's properties, in contains additional
         * pointers to m_Child, m_Next and m_Parent items. Full functionality
         * of wxsMenu was not required here so simillar structure containing
         * data only has been provided
         */
        struct MenuItem
        {
            Type m_Type;
            wxString  m_Id;
            wxString  m_Label;
            wxString  m_Accelerator;
            wxString  m_Help;
            bool      m_Enabled;
            bool      m_Checked;
            MenuItem* m_Child;
            MenuItem* m_Next;
            MenuItem* m_Parent;

            wxTreeItemId m_TreeId;
        };

        /** \brief Helper class which connects MenuItem to wxTreeItemData
         *
         * This class is required to prevent automatic deletion of MenuItem
         * classes
         */
        class MenuItemHolder: public wxTreeItemData
        {
            public:
                MenuItemHolder(MenuItem* Item): m_Item(Item) {}
                MenuItem* m_Item;
        };

        /** \brief Function with wxSmith's code generating panel's content */
        void CreateContent(wxWindow* parent);

        /** \brief Copying data into internal structures */
        void CreateDataCopy();
        void CreateDataCopyReq(wxsMenu* Menu,MenuItem* Item);

        /** \brief Storing copy of data */
        void StoreDataCopy();
        void StoreDataCopyReq(wxsMenu* Menu,MenuItem* Item);

        /** \brief Deleting data copy */
        void DeleteDataCopy();
        void DeleteDataCopyReq(MenuItem* Item);

        /** \brief Updating content of tree and selecting first item */
        void UpdateMenuContent();
        void UpdateMenuContentReq(wxTreeItemId Id,MenuItem* Item);

        /** \brief Selecting item - this must update current item's data and switch to new one */
        void SelectItem(MenuItem* NewSelection);

        /** \brief Updating item's type and getting set of available properties */
        Type CorrectType(MenuItem* Item,bool& UseId,bool& UseLabel,bool& UseAccelerator,bool& UseHelp,bool& UseEnabled,bool& UseChecked);

        /** \brief Getting previous item in this node */
        MenuItem* GetPrevious(MenuItem* Item);

        wxsMenuBar* m_MenuBar;  ///< \brief Edited menu bar, NULL if editing menu
        wxsMenu*    m_Menu;     ///< \brief Edited menu, NULL if editing menu bar
        MenuItem    m_Data;     ///< \brief Root of menu
        MenuItem*   m_Selected; ///< \brief Currently selected item

		//(*Identifiers(wxsMenuEditor)
		static const long ID_TREECTRL1;
		static const long ID_RADIOBUTTON1;
		static const long ID_RADIOBUTTON4;
		static const long ID_RADIOBUTTON2;
		static const long ID_RADIOBUTTON5;
		static const long ID_RADIOBUTTON3;
		static const long ID_STATICLINE1;
		static const long ID_STATICTEXT6;
		static const long ID_TEXTCTRL4;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL3;
		static const long ID_STATICTEXT4;
		static const long ID_CHECKBOX1;
		static const long ID_STATICTEXT5;
		static const long ID_CHECKBOX2;
		static const long ID_STATICLINE2;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		static const long ID_BUTTON4;
		static const long ID_BUTTON5;
		static const long ID_BUTTON6;
		//*)

		//(*Handlers(wxsMenuEditor)
		void OnContentSelectionChanged(wxTreeEvent& event);
		void OnTypeChanged(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnButtonUpClick(wxCommandEvent& event);
		void OnButtonDownClick(wxCommandEvent& event);
		void OnButtonNewClick(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsMenuEditor)
		wxBoxSizer*  BoxSizer1;
		wxStaticBoxSizer*  StaticBoxSizer1;
		wxTreeCtrl*  m_Content;
		wxStaticBoxSizer*  StaticBoxSizer2;
		wxGridSizer*  GridSizer1;
		wxRadioButton*  m_TypeNormal;
		wxRadioButton*  m_TypeSeparator;
		wxRadioButton*  m_TypeCheck;
		wxRadioButton*  m_TypeBreak;
		wxRadioButton*  m_TypeRadio;
		wxStaticLine*  StaticLine1;
		wxFlexGridSizer*  FlexGridSizer1;
		wxStaticText*  StaticText6;
		wxTextCtrl*  m_Id;
		wxStaticText*  StaticText1;
		wxTextCtrl*  m_Label;
		wxStaticText*  StaticText2;
		wxTextCtrl*  m_Accelerator;
		wxStaticText*  StaticText3;
		wxTextCtrl*  m_Help;
		wxStaticText*  StaticText4;
		wxCheckBox*  m_Checked;
		wxStaticText*  StaticText5;
		wxCheckBox*  m_Enabled;
		wxStaticLine*  StaticLine2;
		wxBoxSizer*  BoxSizer2;
		wxButton*  Button1;
		wxButton*  Button2;
		wxButton*  Button3;
		wxButton*  Button4;
		wxBoxSizer*  BoxSizer3;
		wxButton*  Button5;
		wxButton*  Button6;
		//*)

};

#endif
