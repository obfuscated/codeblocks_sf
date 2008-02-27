/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef INCREMENTALSELECTLISTDLG_H
#define INCREMENTALSELECTLISTDLG_H

#include "settings.h"
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>

class myHandler; // forward decl

class DLLIMPORT IncrementalSelectListDlg : public wxDialog
{
	public:
		IncrementalSelectListDlg(wxWindow* parent, const wxArrayString& items, const wxString& caption = wxEmptyString, const wxString& message = wxEmptyString);
		virtual ~IncrementalSelectListDlg();
		wxString GetStringSelection();
		int GetSelection();
	protected:
		void FillList();
		void OnSearch(wxCommandEvent& event);
		void OnSelect(wxCommandEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		myHandler* m_pMyEvtHandler;
		wxListBox* m_List;
		wxTextCtrl* m_Text;
		const wxArrayString& m_Items;
	private:
		DECLARE_EVENT_TABLE();
};

class myHandler : public wxEvtHandler
{
	public:
		myHandler(wxDialog* parent, wxTextCtrl* text, wxListBox* list)
			: m_pParent(parent),
			m_pList(list),
			m_pText(text)
		{
			Connect( text->GetId(),  -1, wxEVT_KEY_DOWN,
						  (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)
						  &myHandler::OnKeyDown );
			Connect( list->GetId(),  -1, wxEVT_KEY_DOWN,
						  (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)
						  &myHandler::OnKeyDown );
		}
		void OnKeyDown(wxKeyEvent& event);
	private:
		wxDialog* m_pParent;
		wxListBox* m_pList;
		wxTextCtrl* m_pText;
		DECLARE_EVENT_TABLE();
};

#endif // INCREMENTALSELECTLISTDLG_H

