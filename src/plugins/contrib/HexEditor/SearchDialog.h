#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

//(*Headers(SearchDialog)
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/radiobut.h>
#include <wx/radiobox.h>
//*)

#include "FileContentBase.h"

class SearchDialog: public wxDialog
{
	public:

		SearchDialog( wxWindow* parent, FileContentBase* content, FileContentBase::OffsetT current );
		virtual ~SearchDialog();
		inline FileContentBase::OffsetT GetOffset() { return m_Offset; }

	private:

		void BuildContent(wxWindow* parent);

		//(*Identifiers(SearchDialog)
		static const long ID_COMBOBOX1;
		static const long ID_RADIOBUTTON1;
		static const long ID_RADIOBUTTON2;
		static const long ID_RADIOBUTTON3;
		static const long ID_BUTTON1;
		static const long ID_RADIOBOX2;
		//*)

		//(*Declarations(SearchDialog)
		wxRadioButton* m_SearchTypeString;
		wxComboBox* m_SearchValue;
		wxRadioButton* m_SearchTypeExpression;
		wxButton* Button1;
		wxRadioBox* m_StartFrom;
		wxRadioButton* m_SearchTypeHex;
		//*)

		//(*Handlers(SearchDialog)
		void OnOk( wxCommandEvent& event );
		void OnButton1Click(wxCommandEvent& event);
		//*)

        void SearchAscii( const char* text );
        void SearchHex( const wxChar* text );
		void SearchBuffer( const unsigned char* data, size_t length );
		void SearchExpression( const wxString& expression );

		void NotFound();
		void FoundAt( FileContentBase::OffsetT pos );
		void ReadError();
		void Cancel();

		static int BlockCompare( const unsigned char* searchIn, size_t inLength, const unsigned char* searchFor, size_t forLength );

		FileContentBase*         m_Content;
		FileContentBase::OffsetT m_Offset;
};

#endif
