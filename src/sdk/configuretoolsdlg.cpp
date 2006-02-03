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
    #include <wx/intl.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/button.h>
    #include <wx/msgdlg.h>
    #include <wx/listbox.h>

    #include "manager.h"
    #include "toolsmanager.h"
#endif


#include "configuretoolsdlg.h"
#include "edittooldlg.h"

BEGIN_EVENT_TABLE(ConfigureToolsDlg, wxDialog)
	EVT_BUTTON(XRCID("btnAdd"), 	ConfigureToolsDlg::OnAdd)
	EVT_BUTTON(XRCID("btnEdit"), 	ConfigureToolsDlg::OnEdit)
	EVT_BUTTON(XRCID("btnRemove"), 	ConfigureToolsDlg::OnRemove)
	EVT_BUTTON(XRCID("btnUp"), 		ConfigureToolsDlg::OnUp)
	EVT_BUTTON(XRCID("btnDown"), 	ConfigureToolsDlg::OnDown)
	EVT_UPDATE_UI(-1,				ConfigureToolsDlg::OnUpdateUI)
END_EVENT_TABLE()

ConfigureToolsDlg::ConfigureToolsDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgConfigureTools"));
	DoFillList();
}

ConfigureToolsDlg::~ConfigureToolsDlg()
{
}

void ConfigureToolsDlg::DoFillList()
{
	wxListBox* list = XRCCTRL(*this, "lstTools", wxListBox);
	list->Clear();
	ToolsManager* toolMan = Manager::Get()->GetToolsManager();
	int count = toolMan->GetToolsCount();
	for (int i = 0; i < count; ++i)
	{
		Tool* tool = toolMan->GetToolByIndex(i);
		if (tool)
			list->Append(tool->name);
	}
}

bool ConfigureToolsDlg::DoEditTool(Tool* tool)
{
	if (!tool)
		return false;
	EditToolDlg dlg(this, tool);
	DoFillList();
	return dlg.ShowModal() == wxID_OK;
}

// events

void ConfigureToolsDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
	wxListBox* list = XRCCTRL(*this, "lstTools", wxListBox);
	bool hasSel = list->GetSelection() != -1;
	bool notFirst = list->GetSelection() > 0;
	bool notLast = (list->GetSelection() < list->GetCount() -1) && hasSel;

	XRCCTRL(*this, "btnEdit", wxButton)->Enable(hasSel);
	XRCCTRL(*this, "btnRemove", wxButton)->Enable(hasSel);
	XRCCTRL(*this, "btnUp", wxButton)->Enable(notFirst);
	XRCCTRL(*this, "btnDown", wxButton)->Enable(notLast);
}

void ConfigureToolsDlg::OnAdd(wxCommandEvent& event)
{
	Tool tool;
	if (DoEditTool(&tool))
	{
		Manager::Get()->GetToolsManager()->AddTool(&tool);
		DoFillList();
	}
}

void ConfigureToolsDlg::OnEdit(wxCommandEvent& event)
{
	wxListBox* list = XRCCTRL(*this, "lstTools", wxListBox);
	Tool* tool = Manager::Get()->GetToolsManager()->GetToolByIndex(list->GetSelection());
	DoEditTool(tool);
	DoFillList();
}

void ConfigureToolsDlg::OnRemove(wxCommandEvent& event)
{
	wxListBox* list = XRCCTRL(*this, "lstTools", wxListBox);
	int sel = list->GetSelection();
	if (wxMessageBox(_("Are you sure you want to remove this tool?"),
					_("Remove tool?"),
					wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) == wxYES)
	{
		Manager::Get()->GetToolsManager()->RemoveToolByIndex(sel);
		DoFillList();
	}
}

void ConfigureToolsDlg::OnUp(wxCommandEvent& event)
{
	wxListBox* list = XRCCTRL(*this, "lstTools", wxListBox);
	int sel = list->GetSelection();

	Tool tool(*(Manager::Get()->GetToolsManager()->GetToolByIndex(sel)));
	Manager::Get()->GetToolsManager()->RemoveToolByIndex(sel);
	Manager::Get()->GetToolsManager()->InsertTool(sel - 1, &tool);
	DoFillList();
	list->SetSelection(sel - 1);
}

void ConfigureToolsDlg::OnDown(wxCommandEvent& event)
{
	wxListBox* list = XRCCTRL(*this, "lstTools", wxListBox);
	int sel = list->GetSelection();

	Tool tool(*(Manager::Get()->GetToolsManager()->GetToolByIndex(sel)));
	Manager::Get()->GetToolsManager()->RemoveToolByIndex(sel);
	Manager::Get()->GetToolsManager()->InsertTool(sel + 1, &tool);
	DoFillList();
	list->SetSelection(sel + 1);
}
