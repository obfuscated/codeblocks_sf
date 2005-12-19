#ifndef WXSCHECKSTRINGSEDITOR_H
#define WXSCHECKSTRINGSEDITOR_H

#include "wxsstringlistcheckproperty.h"

//(*Headers(wxsCheckStringsEditor)
#include <wx/button.h>
#include <wx/checklst.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
//*)

class wxsCheckStringsEditor: public wxDialog
{
	public:

		wxsCheckStringsEditor(wxWindow* parent,wxWindowID id = -1);
		virtual ~wxsCheckStringsEditor();

		//(*Identifiers(wxsCheckStringsEditor)
		enum Identifiers
		{
		    ID_BUTTON1 = 0x1000,
		    ID_BUTTON2,
		    ID_BUTTON3,
		    ID_BUTTON4,
		    ID_BUTTON5,
		    ID_BUTTON6,
		    ID_BUTTON7,
		    ID_CHECKLISTBOX1,
		    ID_STATICLINE1,
		    ID_STATICLINE2,
		    ID_TEXTCTRL1
		};
		//*)
		
		wxArrayString Strings;
		wxsArrayBool Bools;

	protected:

		//(*Handlers(wxsCheckStringsEditor)
		void OnButton7Click(wxCommandEvent& event);
		void OnButton6Click(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnButton4Click(wxCommandEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		void OnButton5Click(wxCommandEvent& event);
		void OnListClick(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsCheckStringsEditor)
		wxBoxSizer* BoxSizer1;
		wxBoxSizer* BoxSizer2;
		wxTextCtrl* EditArea;
		wxButton* Button1;
		wxStaticLine* StaticLine1;
		wxBoxSizer* BoxSizer3;
		wxCheckListBox* StringList;
		wxBoxSizer* BoxSizer4;
		wxButton* Button2;
		wxButton* Button4;
		wxButton* Button3;
		wxButton* Button5;
		wxStaticLine* StaticLine2;
		wxBoxSizer* BoxSizer5;
		wxButton* Button6;
		wxButton* Button7;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
