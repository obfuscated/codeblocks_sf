#ifndef WXSUSERITEMEDITOR_H
#define WXSUSERITEMEDITOR_H

//(*Headers(wxsUserItemEditor)
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

/** \brief class for dialog allowing to edit user-defined items */
class wxsUserItemEditor: public wxDialog
{
	public:

        /** \brief Ctor */
		wxsUserItemEditor(wxWindow* parent,wxWindowID id = -1);

		/** \brief Dctor */
		virtual ~wxsUserItemEditor();

	private:

        /** \brief Internal structure used to hold user item's data */
        class ItemDataCopy: public wxClientData
        {
            public:

                int m_OriginalIndex;        ///< \brief Index of original item inside user items manager
                wxString m_Name;            ///< \brief Item's name
                wxBitmap m_Icon16;          ///< \brief 16x16 bitmap
                wxBitmap m_Icon32;          ///< \brief 32x32 bitmap
                wxBitmap m_PreviewBitmap;   ///< \brief Bitmap used inside preview
        };

        /** \brief Initializing items list with data from user item manager */
        void FillItemsList();

        /** \brief Setting new selection */
        void SelectItem(ItemDataCopy* Item);

        /** \brief Storing data from dialog into current selection */
        void StoreCurrentItem();

        /** \brief Currently selected item */
        ItemDataCopy* m_Selected;

		//(*Identifiers(wxsUserItemEditor)
		static const long ID_LISTBOX1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON5;
		static const long ID_BUTTON2;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT7;
		static const long ID_BITMAPBUTTON1;
		static const long ID_BITMAPBUTTON2;
		static const long ID_STATICTEXT4;
		static const long ID_BITMAPBUTTON3;
		static const long ID_STATICTEXT5;
		static const long ID_BUTTON3;
		static const long ID_STATICTEXT6;
		static const long ID_BUTTON4;
		//*)

		//(*Handlers(wxsUserItemEditor)
		void OnItemListSelect(wxCommandEvent& event);
		void OnOk(wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsUserItemEditor)
		wxBoxSizer* BoxSizer1;
		wxBoxSizer* BoxSizer2;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxListBox* m_ItemList;
		wxBoxSizer* BoxSizer3;
		wxButton* m_Add;
		wxButton* m_Duplicate;
		wxButton* m_Delete;
		wxStaticBoxSizer* StaticBoxSizer2;
		wxFlexGridSizer* FlexGridSizer1;
		wxStaticText* StaticText1;
		wxTextCtrl* m_Name;
		wxStaticText* StaticText2;
		wxFlexGridSizer* FlexGridSizer2;
		wxStaticText* StaticText3;
		wxStaticText* StaticText7;
		wxBitmapButton* m_Icon16;
		wxBitmapButton* m_Icon32;
		wxStaticText* StaticText4;
		wxBitmapButton* m_PreviewBitmap;
		wxStaticText* StaticText5;
		wxButton* m_PropertiesEdit;
		wxStaticText* StaticText6;
		wxButton* m_SourceCodeEdit;
		wxStdDialogButtonSizer* StdDialogButtonSizer1;
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
