/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef EDITSNIPPETDLG_H
#define EDITSNIPPETDLG_H

#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/button.h>

class Edit;

class EditSnippetDlg : public wxDialog
{
    friend class EditSnippetDropTarget;

	public:
		EditSnippetDlg(const wxString& snippetName, const wxString& snippetText,
                            wxSemaphore* pWaitSem, wxString fileName=wxEmptyString);
		~EditSnippetDlg();

		wxString GetName();
		wxString GetText();
	private:
		void InitDlg();
		void EndSnippetDlg(int wxID_OKorCANCEL);


		void OnOK(wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);
		void OnHelp(wxCommandEvent& event);

		wxStaticText* m_NameLbl;
		wxTextCtrl* m_SnippetNameCtrl;
		wxStaticText* m_SnippetLbl;
		//wxTextCtrl* m_SnippetTextCtrl;
		Edit* m_SnippetEditCtrl;
		wxButton* m_OKBtn;
		wxButton* m_CancelBtn;
		wxButton* m_HelpBtn;
		wxSemaphore* pWaitingSemaphore;
		wxString     m_EditFileName;

		DECLARE_EVENT_TABLE()
};

#endif // EDITSNIPPETDLG_H
