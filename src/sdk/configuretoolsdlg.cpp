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
    #include <wx/button.h>
    #include <wx/intl.h>
    #include <wx/listbox.h>
    #include <wx/string.h>
    #include <wx/xrc/xmlres.h>

    #include "manager.h"
    #include "cbtool.h"
    #include "toolsmanager.h"
    #include "globals.h"
#endif

#include "configuretoolsdlg.h"
#include "edittooldlg.h"

BEGIN_EVENT_TABLE(ConfigureToolsDlg, wxDialog)
	EVT_BUTTON(XRCID("btnAdd"), 	ConfigureToolsDlg::OnAdd)
	EVT_BUTTON(XRCID("btnEdit"), 	ConfigureToolsDlg::OnEdit)
	EVT_BUTTON(XRCID("btnRemove"), 	ConfigureToolsDlg::OnRemove)
	EVT_BUTTON(XRCID("btnAddSeparator"), 	ConfigureToolsDlg::OnAddSeparator)
	EVT_BUTTON(XRCID("btnUp"), 		ConfigureToolsDlg::OnUp)
	EVT_BUTTON(XRCID("btnDown"), 	ConfigureToolsDlg::OnDown)
	EVT_UPDATE_UI(-1,				ConfigureToolsDlg::OnUpdateUI)
END_EVENT_TABLE()

ConfigureToolsDlg::ConfigureToolsDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgConfigureTools"));
	DoFillList();
} // end of constructor

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
		if(const cbTool* tool = toolMan->GetToolByIndex(i))
		{
			list->Append(tool->GetName());
		}
	}
} // end of DoFillList

bool ConfigureToolsDlg::DoEditTool(cbTool* tool)
{
	if (!tool)
		return false;
	EditToolDlg dlg(this, tool);
    PlaceWindow(&dlg);
	return dlg.ShowModal() == wxID_OK;
} // end of DoEditTool

// events

void ConfigureToolsDlg::OnUpdateUI(wxUpdateUIEvent& /*event*/)
{
	const wxListBox* list = XRCCTRL(*this, "lstTools", wxListBox);
	bool hasSel = list->GetSelection() != -1;
	bool notFirst = list->GetSelection() > 0;
	bool notLast = (list->GetSelection() < list->GetCount() -1) && hasSel;
	bool notSeparator = true;

	if(hasSel)
        notSeparator = Manager::Get()->GetToolsManager()->GetToolByIndex(list->GetSelection())->GetName() != CB_TOOLS_SEPARATOR;

	XRCCTRL(*this, "btnEdit", wxButton)->Enable(hasSel && notSeparator);
	XRCCTRL(*this, "btnRemove", wxButton)->Enable(hasSel);
	XRCCTRL(*this, "btnUp", wxButton)->Enable(notFirst);
	XRCCTRL(*this, "btnDown", wxButton)->Enable(notLast);
} // end of OnUpdateUI

void ConfigureToolsDlg::OnAdd(wxCommandEvent& /*event*/)
{
	cbTool tool;
	if (DoEditTool(&tool))
	{
		Manager::Get()->GetToolsManager()->AddTool(&tool);
		DoFillList();
	}
} // end of OnAdd

void ConfigureToolsDlg::OnEdit(wxCommandEvent& /*event*/)
{
	const wxListBox* list = XRCCTRL(*this, "lstTools", wxListBox);
	cbTool* tool = Manager::Get()->GetToolsManager()->GetToolByIndex(list->GetSelection());
	DoEditTool(tool);
	DoFillList();
} // end of OnEdit

void ConfigureToolsDlg::OnRemove(wxCommandEvent& /*event*/)
{
	const wxListBox* list = XRCCTRL(*this, "lstTools", wxListBox);
	int sel = list->GetSelection();
	if (cbMessageBox(_("Are you sure you want to remove this tool?"),
					_("Remove tool?"),
					wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) == wxID_YES)
	{
		Manager::Get()->GetToolsManager()->RemoveToolByIndex(sel);
		DoFillList();
	}
} // end of OnRemove

void ConfigureToolsDlg::OnAddSeparator(wxCommandEvent& /*event*/)
{
    cbTool tool;
    tool.SetName(CB_TOOLS_SEPARATOR);
    tool.SetCommand(CB_TOOLS_SEPARATOR);
    Manager::Get()->GetToolsManager()->AddTool(&tool);
    DoFillList();
} // end of OnAddSeparator

void ConfigureToolsDlg::OnUp(wxCommandEvent& /*event*/)
{
	wxListBox* list = XRCCTRL(*this, "lstTools", wxListBox);
	int sel = list->GetSelection();

	cbTool tool(*(Manager::Get()->GetToolsManager()->GetToolByIndex(sel)));
	Manager::Get()->GetToolsManager()->RemoveToolByIndex(sel);
	Manager::Get()->GetToolsManager()->InsertTool(sel - 1, &tool);
	DoFillList();
	list->SetSelection(sel - 1);
} // end of OnUp

void ConfigureToolsDlg::OnDown(wxCommandEvent& /*event*/)
{
	wxListBox* list = XRCCTRL(*this, "lstTools", wxListBox);
	int sel = list->GetSelection();

	cbTool tool(*(Manager::Get()->GetToolsManager()->GetToolByIndex(sel)));
	Manager::Get()->GetToolsManager()->RemoveToolByIndex(sel);
	Manager::Get()->GetToolsManager()->InsertTool(sel + 1, &tool);
	DoFillList();
	list->SetSelection(sel + 1);
}// end of OnDown
