/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
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
