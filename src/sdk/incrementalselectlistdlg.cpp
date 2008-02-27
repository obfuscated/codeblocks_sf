/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/xrc/xmlres.h>
    #include <wx/intl.h>
    #include <wx/textctrl.h>
    #include <wx/stattext.h>
    #include "manager.h"
    #include <wx/listbox.h>
#endif

#include "incrementalselectlistdlg.h"


BEGIN_EVENT_TABLE(myHandler, wxEvtHandler)
END_EVENT_TABLE()

void myHandler::OnKeyDown(wxKeyEvent& event)
{
	//Manager::Get()->GetLogManager()->Log(mltDevDebug, "OnKeyDown");
	switch (event.GetKeyCode())
	{
		case WXK_RETURN:
			m_pParent->EndModal(wxID_OK);
			break;

		case WXK_ESCAPE:
			m_pParent->EndModal(wxID_CANCEL);
			break;

		case WXK_UP:
			m_pList->SetSelection(m_pList->GetSelection() - 1);
			break;

		case WXK_DOWN:
			m_pList->SetSelection(m_pList->GetSelection() + 1);
			break;

		default:
			event.Skip();
			break;
	}
}

BEGIN_EVENT_TABLE(IncrementalSelectListDlg, wxDialog)
	EVT_TEXT(XRCID("txtSearch"), IncrementalSelectListDlg::OnSearch)
	EVT_LISTBOX_DCLICK(XRCID("lstItems"), IncrementalSelectListDlg::OnSelect)
END_EVENT_TABLE()

IncrementalSelectListDlg::IncrementalSelectListDlg(wxWindow* parent, const wxArrayString& items, const wxString& caption, const wxString& message)
	: m_pMyEvtHandler(0L),
	m_List(0L),
	m_Text(0L),
	m_Items(items)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgIncrementalSelectList"));
	if (!caption.IsEmpty())
		SetTitle(caption);
	if (!message.IsEmpty())
		XRCCTRL(*this, "lblMessage", wxStaticText)->SetLabel(message);

	m_Text = XRCCTRL(*this, "txtSearch", wxTextCtrl);
	m_List = XRCCTRL(*this, "lstItems", wxListBox);

	myHandler* m_pMyEvtHandler = new myHandler(this, m_Text, m_List);
	m_Text->SetNextHandler(m_pMyEvtHandler);
	m_List->SetNextHandler(m_pMyEvtHandler);

	FillList();
}

IncrementalSelectListDlg::~IncrementalSelectListDlg()
{
	m_Text->SetNextHandler(0L);
	m_List->SetNextHandler(0L);

	delete m_pMyEvtHandler;
}

wxString IncrementalSelectListDlg::GetStringSelection()
{
	return m_List->GetStringSelection();
}

int IncrementalSelectListDlg::GetSelection()
{
	return m_List->GetSelection();
}

void IncrementalSelectListDlg::FillList()
{
    Freeze();

    // We put a star before and after pattern to find search expression everywhere in path
	wxString search(wxT("*") + m_Text->GetValue().Lower() + wxT("*"));

	wxArrayString result;
	//Manager::Get()->GetLogManager()->Log(mltDevDebug, "FillList(): '%s'", search.c_str());
	m_List->Clear();
	for (unsigned int i = 0; i < m_Items.GetCount(); ++i)
	{
		// 2 for before and after stars =~ empty string
		if ((search.Length()==2) || m_Items[i].Lower().Matches(search.c_str()))
			result.Add(m_Items[i]);
//			m_List->Append(m_Items[i]);
	}
	m_List->Set(result);
	if (m_List->GetCount())
		m_List->SetSelection(0);
    Thaw();
}

// events

void IncrementalSelectListDlg::OnSearch(wxCommandEvent& event)
{
	FillList();
}

void IncrementalSelectListDlg::OnSelect(wxCommandEvent& event)
{
	EndModal(wxID_OK);
}
