/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

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
