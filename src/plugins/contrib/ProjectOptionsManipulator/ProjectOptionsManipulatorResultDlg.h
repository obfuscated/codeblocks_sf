#ifndef PROJECTOPTIONSMANIPULATORRESULTDLG_H
#define PROJECTOPTIONSMANIPULATORRESULTDLG_H

//(*Headers(ProjectOptionsManipulatorResultDlg)
#include <wx/dialog.h>
class wxStdDialogButtonSizer;
class wxTextCtrl;
class wxStaticText;
class wxBoxSizer;
//*)

class ProjectOptionsManipulatorResultDlg: public wxDialog
{
	public:

		ProjectOptionsManipulatorResultDlg(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~ProjectOptionsManipulatorResultDlg();

		//(*Declarations(ProjectOptionsManipulatorResultDlg)
		wxTextCtrl* m_TxtResult;
		//*)

    void ApplyResult(const wxArrayString& result);

	protected:

		//(*Identifiers(ProjectOptionsManipulatorResultDlg)
		static const long ID_TXT_RESULT;
		//*)

	private:

		//(*Handlers(ProjectOptionsManipulatorResultDlg)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
