#ifndef WXSARRAYSTRINGCHECKEDITORDLG_H
#define WXSARRAYSTRINGCHECKEDITORDLG_H

#include "wxsarraystringcheckproperty.h"

//(*Headers(wxsArrayStringCheckEditorDlg)
#include <wx/button.h>
#include <wx/checklst.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/settings.h>
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
		enum Identifiers
		{
		    ID_TEXTCTRL1 = 0x1000,
		    ID_BUTTON1,
		    ID_STATICLINE1,
		    ID_CHECKLISTBOX1,
		    ID_BUTTON2,
		    ID_BUTTON4,
		    ID_BUTTON3,
		    ID_BUTTON5,
		    ID_STATICLINE2,
		    ID_BUTTON6,
		    ID_BUTTON7
		};
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
