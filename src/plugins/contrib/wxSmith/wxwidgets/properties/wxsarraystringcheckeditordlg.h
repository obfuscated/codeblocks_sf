#ifndef WXSARRAYSTRINGCHECKEDITORDLG_H
#define WXSARRAYSTRINGCHECKEDITORDLG_H

#include "wxsarraystringcheckproperty.h"

//(*Headers(wxsArrayStringCheckEditorDlg)
#include <wx/button.h>
#include <wx/checklst.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
//*)


class wxsArrayStringCheckEditorDlg: public wxDialog
{
	public:

		wxsArrayStringCheckEditorDlg(wxWindow* parent,wxArrayString& Strings,wxArrayBool& Bools,wxWindowID id = -1);
		virtual ~wxsArrayStringCheckEditorDlg();

		//(*Identifiers(wxsArrayStringCheckEditorDlg)
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_STATICLINE1;
		static const long ID_CHECKLISTBOX1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON4;
		static const long ID_BUTTON3;
		static const long ID_BUTTON5;
		static const long ID_STATICLINE2;
		static const long ID_BUTTON6;
		static const long ID_BUTTON7;
		//*)

	protected:

		//(*Handlers(wxsArrayStringCheckEditorDlg)
		void OnButton1Click(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnButton4Click(wxCommandEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		void OnButton5Click(wxCommandEvent& event);
		void OnButton6Click(wxCommandEvent& event);
		void OnButton7Click(wxCommandEvent& event);
		void OnStringListToggled(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsArrayStringCheckEditorDlg)
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

        wxArrayString& Strings;
        wxArrayBool& Bools;

		DECLARE_EVENT_TABLE()
};

#endif
