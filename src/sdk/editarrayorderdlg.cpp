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
    #include <wx/button.h>
    #include <wx/listbox.h>
#endif

#include "editarrayorderdlg.h" // class's header file

BEGIN_EVENT_TABLE(EditArrayOrderDlg, wxDialog)
    EVT_UPDATE_UI( -1, EditArrayOrderDlg::OnUpdateUI)
	EVT_BUTTON(XRCID("btnMoveUp"), EditArrayOrderDlg::OnMoveUp)
	EVT_BUTTON(XRCID("btnMoveDown"), EditArrayOrderDlg::OnMoveDown)
END_EVENT_TABLE()

// class constructor
EditArrayOrderDlg::EditArrayOrderDlg(wxWindow* parent, const wxArrayString& array)
    : m_Array(array)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgEditArrayOrder"));
	DoFillList();
}

// class destructor
EditArrayOrderDlg::~EditArrayOrderDlg()
{
}

void EditArrayOrderDlg::DoFillList()
{
    wxListBox* list = XRCCTRL(*this, "lstItems", wxListBox);
    list->Clear();
	for (unsigned int i = 0; i < m_Array.GetCount(); ++i)
        list->Append(m_Array[i]);
}

void EditArrayOrderDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    wxListBox* list = XRCCTRL(*this, "lstItems", wxListBox);

    XRCCTRL(*this, "btnMoveUp", wxButton)->Enable(list->GetSelection() > 0);
    XRCCTRL(*this, "btnMoveDown", wxButton)->Enable(list->GetSelection() >= 0 && list->GetSelection() < (int)list->GetCount() - 1);
}

void EditArrayOrderDlg::OnMoveUp(wxCommandEvent& event)
{
    wxListBox* list = XRCCTRL(*this, "lstItems", wxListBox);
    int sel = list->GetSelection();

    if (sel > 0)
    {
        wxString tmp = list->GetString(sel);
        list->Delete(sel);
        list->InsertItems(1, &tmp, sel - 1);
		list->SetSelection(sel - 1);
    }
}

void EditArrayOrderDlg::OnMoveDown(wxCommandEvent& event)
{
    wxListBox* list = XRCCTRL(*this, "lstItems", wxListBox);
    int sel = list->GetSelection();

    if (sel < (int)list->GetCount() - 1)
    {
        wxString tmp = list->GetString(sel);
        list->Delete(sel);
        list->InsertItems(1, &tmp, sel + 1);
		list->SetSelection(sel + 1);
    }
}

void EditArrayOrderDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        wxListBox* list = XRCCTRL(*this, "lstItems", wxListBox);

        m_Array.Clear();
        for (int i = 0; i < (int)list->GetCount(); ++i)
            m_Array.Add(list->GetString(i));
    }

    wxDialog::EndModal(retCode);
}

