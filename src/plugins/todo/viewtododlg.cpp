#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/file.h>
#include <manager.h>
#include <configmanager.h>
#include <editormanager.h>
#include <projectmanager.h>
#include <projectbuildtarget.h>
#include <cbproject.h>
#include "viewtododlg.h"
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ToDoItems);

BEGIN_EVENT_TABLE(ViewTodoDlg, wxDialog)
	EVT_COMBOBOX(-1, ViewTodoDlg::OnComboChange)
	EVT_LIST_ITEM_ACTIVATED(XRCID("lcItems"), ViewTodoDlg::OnListSelected)
	EVT_UPDATE_UI(XRCID("wxID_OK"), ViewTodoDlg::OnUpdateUI)
END_EVENT_TABLE()

ViewTodoDlg::ViewTodoDlg(wxWindow* parent)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, "dlgViewTodo");
	m_pItems = XRCCTRL(*this, "lcItems", wxListCtrl);
	m_pSource = XRCCTRL(*this, "cmbSource", wxComboBox);
	m_pUser = XRCCTRL(*this, "cmbUser", wxComboBox);

	Parse();
	FillList();
}

ViewTodoDlg::~ViewTodoDlg()
{
	//dtor
	m_Items.Clear();
}

wxString ViewTodoDlg::GetFilename()
{
	int idx = m_pItems->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (idx == -1)
		return wxEmptyString;
	return m_Items[idx].filename;
}

int ViewTodoDlg::GetLine()
{
	int idx = m_pItems->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (idx == -1)
		return -1;
	return m_Items[idx].line;
}

void ViewTodoDlg::LoadUsers()
{
	wxString oldStr = m_pUser->GetStringSelection();
	m_pUser->Clear();
	m_pUser->Append(_("<All users>"));

	// loop through all todos and add distinct users
	for (unsigned int i = 0; i < m_Items.GetCount(); ++i)
	{
		wxString user = m_Items[i].user;
		if (!user.IsEmpty())
		{
			if (m_pUser->FindString(user) == wxNOT_FOUND)
				m_pUser->Append(user);
		}
	}
	int old = m_pUser->FindString(oldStr);
	if (old != wxNOT_FOUND)
		m_pUser->SetSelection(old);
	else
		m_pUser->SetSelection(0); // all users
}

void ViewTodoDlg::InitList()
{
	m_pItems->ClearAll();

	m_pItems->InsertColumn(0, _("Type"), wxLIST_FORMAT_LEFT, 64);
	m_pItems->InsertColumn(1, _("Text"), wxLIST_FORMAT_LEFT, 240);
	m_pItems->InsertColumn(2, _("User"), wxLIST_FORMAT_LEFT, 64);
	m_pItems->InsertColumn(3, _("Priority"), wxLIST_FORMAT_RIGHT, 32);
	m_pItems->InsertColumn(4, _("File"), wxLIST_FORMAT_LEFT, 128);
	m_pItems->InsertColumn(5, _("Line"), wxLIST_FORMAT_RIGHT, 48);
}

void ViewTodoDlg::FillList()
{
	LoadUsers();
	m_pItems->Freeze();
	InitList();

	for (unsigned int i = 0; i < m_Items.GetCount(); ++i)
	{
		const ToDoItem& item = m_Items[i];
		if (m_pUser->GetSelection() == 0 || // all users
			m_pUser->GetStringSelection().Matches(item.user)) // or matches user
		{
			int idx = m_pItems->InsertItem(m_pItems->GetItemCount(), item.type);
			m_pItems->SetItem(idx, 1, item.text);
			m_pItems->SetItem(idx, 2, item.user);
			m_pItems->SetItem(idx, 3, item.priorityStr);
			m_pItems->SetItem(idx, 4, item.filename);
			m_pItems->SetItem(idx, 5, item.lineStr);
		}
	}

	m_pItems->Thaw();

}

void ViewTodoDlg::Parse()
{
	// based on user prefs, parse files for todo items
	m_Items.Clear();	
	
	switch (m_pSource->GetSelection())
	{
		case 0: // current file only
		{
			// this is the easiest selection ;)
			cbEditor* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
			ParseEditor(ed);
			break;
		}
		case 1: // open files
		{
			// easy too; parse all open editor files...
			for (int i = 0; i < Manager::Get()->GetEditorManager()->GetEditorsCount(); ++i)
			{
				cbEditor* ed = Manager::Get()->GetEditorManager()->GetEditor(i);
				ParseEditor(ed);
			}
			break;
		}
		case 2: // all project files
		{
			// loop all project files
			// but be aware: if a file is opened, use the open file because
			// it might not be the same on the disk...
			cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
			if (!prj)
				return;
			for (int i = 0; i < prj->GetFilesCount(); ++i)
			{
				ProjectFile* pf = prj->GetFile(i);
				wxString filename = pf->file.GetFullPath();
				cbEditor* ed = Manager::Get()->GetEditorManager()->IsOpen(filename);
				if (ed)
					ParseEditor(ed);
				else
					ParseFile(filename);
			}
			break;
		}
	}
}

void ViewTodoDlg::ParseEditor(cbEditor* pEditor)
{
	if (pEditor)
		ParseBuffer(pEditor->GetControl()->GetText(), pEditor->GetFilename());
}

void ViewTodoDlg::ParseFile(const wxString& filename)
{
	if (!wxFileExists(filename))
		return;
				
	// open file
	wxString st;
	wxFile file(filename);
	if (!file.IsOpened())
		return;
	char* buff = st.GetWriteBuf(file.Length());
	file.Read(buff, file.Length());
	st.UngetWriteBuf();
	ParseBuffer(st, filename);
}

void ViewTodoDlg::ParseBuffer(const wxString& buffer, const wxString& filename)
{
	// this is the actual workhorse...
	
	// ok, we look for two basic kinds of todo entries in the text
	// our version...
    // TODO (mandrav#0#): Implement code to do this and the other...
	// and a generic version...
    // TODO: Implement code to do this and the other...
	
	// we 'll use an array of wxString to look for TODO, FIXME and NOTE
	wxString todoBase[3] = {"TODO", "FIXME", "NOTE"};
	
	for (int i = 0; i < 3; ++i)
	{
//Manager::Get()->GetMessageManager()->DebugLog("Looking for %s", todoBase[i].c_str());
		int pos = buffer.find(todoBase[i], 0);
		
		while (pos > 0)
		{
			// ok, start parsing now...
			// keep a temp copy of pos to work with
			int idx = pos;
			bool isValid = false; // found it in a comment?
			bool isC = false; // C or C++ style comment?
			
#warning TODO (mandrav#1#): Make viewtododlg understand and display todo notes that are compiler warnings/errors...

			// first check what type of comment we have
			wxString allowedChars = " \t/*";
			wxChar lastChar = '\0';
			while (idx >= 0)
			{
				wxChar c = buffer.GetChar(--idx);
				if ((int)allowedChars.Index(c) != wxNOT_FOUND)
				{
					if (c == '/' && (lastChar == '/' || lastChar == '*'))
					{
						isValid = true;
						isC = lastChar == '*';
						break;
					}
				}
				else
					break;
				lastChar = c;
			}
			
//Manager::Get()->GetMessageManager()->DebugLog("Found %s %s style %s at %d", isValid ? "valid" : "invalid", isC ? "C" : "C++", todoBase[i].c_str(), pos);
			if (isValid)
			{
				ToDoItem item;
				item.type = todoBase[i];
				item.filename = filename;
				
				idx = pos + todoBase[i].Length();
				wxChar c = '\0';
				
//Manager::Get()->GetMessageManager()->DebugLog("1");
				// skip to next non-blank char
				while (idx < (int)buffer.Length())
				{
					c = buffer.GetChar(idx);
					if (c != ' ' && c != '\t')
						break;
					++idx;
				}
//Manager::Get()->GetMessageManager()->DebugLog("2");
				// is it ours or generic todo?
				if (c == '(')
				{
					// it's ours, find user and/or priority
					++idx; // skip (
					while (idx < (int)buffer.Length())
					{
						wxChar c1 = buffer.GetChar(idx);
						if (c1 != '#' && c1 != ')')
						{
							// a little logic doesn't hurt ;)
							
							if (c1 == ' ' || c1 == '\t' || c1 == '\r' || c1 == '\n')
							{
								// allow one consecutive space
								if (item.user.Last() != ' ')
									item.user << ' ';
							}
							else
								item.user << c1;
						}
						else if (c1 == '#')
						{
							// look for priority
							c1 = buffer.GetChar(++idx);
							allowedChars = "0123456789";
							if ((int)allowedChars.Index(c1) != wxNOT_FOUND)
								item.priorityStr << c1;
							// skip to start of text
							while (idx < (int)buffer.Length())
							{
								wxChar c2 = buffer.GetChar(idx++);
								if (c2 == ')' || c2 == '\r' || c2 == '\n')
									break;
							}
							break;
						}
						else
							break;
						++idx;
					}
				}
//Manager::Get()->GetMessageManager()->DebugLog("3");
				// ok, we 've reached the actual todo text :)
				// take everything up to the end of line or end of comment (if isC)
				wxChar lastChar = '\0';
				if (buffer.GetChar(idx) == ':')
					++idx;
				while (idx < (int)buffer.Length())
				{
					wxChar c1 = buffer.GetChar(idx++);
					if (c1 == '\r' || c1 == '\n')
						break;
					if (isC && c1 == '/' && lastChar == '*')
						break;
					if (c1 == ' ' || c1 == '\t')
					{
						// allow one consecutive space
						if (item.text.Last() != ' ')
							item.text << ' ';
					}
					else
						item.text << c1;
					lastChar = c1;
				}
//Manager::Get()->GetMessageManager()->DebugLog("4");
				// do some clean-up
				item.text.Trim();
				item.text.Trim(false);
				item.user.Trim();
				item.user.Trim(false);
				item.line = CalculateLineNumber(buffer, pos);
				item.lineStr << item.line + 1; // 1-based line number for list
				m_Items.Add(item);
			}
			else
				break; // invalid style...

			pos = buffer.find(todoBase[i], idx);
		}
//		Manager::Get()->GetMessageManager()->DebugLog("Found it at %d", pos);
	}
}

int ViewTodoDlg::CalculateLineNumber(const wxString& buffer, int upTo)
{
	int line = 0;
	for (int i = 0; i < upTo; ++i)
	{
		if (buffer.GetChar(i) == '\n')
			++line;
	}
	return line;
}

// events

void ViewTodoDlg::OnComboChange(wxCommandEvent& event)
{
	Parse();
	FillList();
}

void ViewTodoDlg::OnListSelected(wxListEvent& event)
{
	EndModal(wxID_OK);
}

void ViewTodoDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
	XRCCTRL(*this, "wxID_OK", wxButton)->Enable(m_pItems->GetSelectedItemCount() > 0);
}
