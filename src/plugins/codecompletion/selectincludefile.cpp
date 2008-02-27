/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "selectincludefile.h"
#include <wx/xrc/xmlres.h>
#include <wx/arrstr.h>

BEGIN_EVENT_TABLE(SelectIncludeFile,wxDialog)
	//(*EventTable(SelectIncludeFile)
	EVT_LISTBOX_DCLICK(XRCID("ID_LISTBOX1"),SelectIncludeFile::OnOk)
	EVT_BUTTON(XRCID("wxID_CANCEL"),SelectIncludeFile::OnOk)
	EVT_BUTTON(XRCID("ID_BUTTON1"),SelectIncludeFile::OnCancel)
	//*)
END_EVENT_TABLE()

SelectIncludeFile::SelectIncludeFile(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(SelectIncludeFile)
	wxXmlResource::Get()->LoadDialog(this,parent,_T("SelectIncludeFile"));
	ListBox1 = XRCCTRL(*this,"ID_LISTBOX1",wxListBox);
	//*)
}

SelectIncludeFile::~SelectIncludeFile()
{
}


void SelectIncludeFile::OnOk(wxCommandEvent& event)
{
	int pos = ListBox1->GetSelection();
	if(pos != wxNOT_FOUND)
	{
		m_SelectedIncludeFile = ListBox1->GetString(pos);
	}
	else
	{
		m_SelectedIncludeFile.Clear();
	}
	EndModal(wxID_OK);
} // end of OnOk

void SelectIncludeFile::OnCancel(wxCommandEvent& event)
{
	EndModal(wxID_CANCEL);
} // end of OnCancel

void SelectIncludeFile::AddListEntries(const wxArrayString& IncludeFiles)
{
	ListBox1->InsertItems(IncludeFiles, 0);
	ListBox1->SetSelection(0);
} // end of AddListEntry
