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
* $Id$
* $Date$
*/

#include "sdk_precomp.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/notebook.h>

#include "newfromtemplatedlg.h"
#include "manager.h"
#include "configmanager.h"

struct ListItemData
{
	ListItemData(ProjectTemplateLoader* t = 0, cbProjectWizardPlugin* p = 0) : pt(t), plugin(p) {}
	ListItemData(const ListItemData& rhs) : pt(rhs.pt), plugin(rhs.plugin) {}
	ProjectTemplateLoader* pt;
	cbProjectWizardPlugin* plugin;
};

BEGIN_EVENT_TABLE(NewFromTemplateDlg, wxDialog)
    EVT_UPDATE_UI(-1, NewFromTemplateDlg::OnUpdateUI)
	EVT_LIST_ITEM_SELECTED(XRCID("listTemplates"), NewFromTemplateDlg::OnListSelection)
	EVT_COMBOBOX(XRCID("cmbCategories"), NewFromTemplateDlg::OnCategoryChanged)
END_EVENT_TABLE()

NewFromTemplateDlg::NewFromTemplateDlg(const ProjectTemplateArray& templates, const wxArrayString& user_templates)
	: m_Template(0L),
	m_pWizard(0L),
	m_ImageList(32, 32),
	m_Templates(templates)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, 0L, _T("dlgNewFromTemplate"));
	m_Wizards = Manager::Get()->GetPluginManager()->GetOffersFor(ptProjectWizard);
	BuildCategories();
	BuildList();

    // fill user templates list
    XRCCTRL(*this, "lstUser", wxListBox)->Clear();
    for (unsigned int i = 0; i < user_templates.GetCount(); ++i)
    {
        XRCCTRL(*this, "lstUser", wxListBox)->Append(user_templates[i]);
    }
}

NewFromTemplateDlg::~NewFromTemplateDlg()
{
	//dtor
	ClearList();
}

void NewFromTemplateDlg::ClearList()
{
	wxListCtrl* list = XRCCTRL(*this, "listTemplates", wxListCtrl);
	for (int i = 0; i < list->GetItemCount(); ++i)
	{
		ListItemData* data = (ListItemData*)list->GetItemData(i);
		delete data;
	}
	list->ClearAll();
}

int NewFromTemplateDlg::GetOptionIndex()
{
	return XRCCTRL(*this, "cmbOptions", wxComboBox)->GetSelection();
}

int NewFromTemplateDlg::GetFileSetIndex()
{
	return XRCCTRL(*this, "cmbFileSets", wxComboBox)->GetSelection();
}

void NewFromTemplateDlg::BuildCategories()
{
	wxComboBox* cat = XRCCTRL(*this, "cmbCategories", wxComboBox);
	cat->Clear();
	cat->Append(_("<All categories>"));
	for (unsigned int i = 0; i < m_Templates.GetCount(); ++i)
	{
		ProjectTemplateLoader* pt = m_Templates[i];
		if (cat->FindString(pt->m_Category) == wxNOT_FOUND)
			cat->Append(pt->m_Category);
	}
	for (unsigned int i = 0; i < m_Wizards.GetCount(); ++i)
	{
		cbProjectWizardPlugin* plugin = (cbProjectWizardPlugin*)m_Wizards[i];
		if (cat->FindString(plugin->GetCategory()) == wxNOT_FOUND)
			cat->Append(plugin->GetCategory());
	}
	cat->SetSelection(0);
}

void NewFromTemplateDlg::BuildList()
{
	wxComboBox* cat = XRCCTRL(*this, "cmbCategories", wxComboBox);
	wxListCtrl* list = XRCCTRL(*this, "listTemplates", wxListCtrl);
	ClearList();
	m_ImageList.RemoveAll();
	list->SetImageList(&m_ImageList, wxIMAGE_LIST_NORMAL);

	// file-based templates
	wxBitmap bmp;
	bool all = cat->GetSelection() == 0;
	wxString baseDir = ConfigManager::Get()->Read(_T("/data_path"));
	baseDir << _T("/templates/");
	for (unsigned int x = 0; x < m_Templates.GetCount(); ++x)
	{
		ProjectTemplateLoader* pt = m_Templates[x];
		if (all || pt->m_Category.Matches(cat->GetStringSelection()))
		{
			int idx = -2;
			if (wxFileExists(baseDir + pt->m_Bitmap))
			{
				bmp.LoadFile(baseDir + pt->m_Bitmap, wxBITMAP_TYPE_PNG);
				idx = m_ImageList.Add(bmp);
			}
			int index = list->InsertItem(0, pt->m_Title, idx);
			if (index != -1)
				list->SetItemData(index, (long)(new ListItemData(pt)));
		}
	}

	// wizards
	for (unsigned int i = 0; i < m_Wizards.GetCount(); ++i)
	{
		cbProjectWizardPlugin* plugin = (cbProjectWizardPlugin*)m_Wizards[i];
		if (all || plugin->GetCategory().Matches(cat->GetStringSelection()))
		{
			int idx = plugin->GetBitmap().Ok() ? m_ImageList.Add(plugin->GetBitmap()) : -2;
			int index = list->InsertItem(0, plugin->GetTitle(), idx);
			if (index != -1)
				list->SetItemData(index, (long)(new ListItemData(0, plugin)));
		}
	}

	XRCCTRL(*this, "cmbOptions", wxComboBox)->Enable(false);
	XRCCTRL(*this, "cmbFileSets", wxComboBox)->Enable(false);
}

void NewFromTemplateDlg::FillTemplate(ProjectTemplateLoader* pt)
{
	m_Template = pt;
	XRCCTRL(*this, "cmbOptions", wxComboBox)->Clear();
	XRCCTRL(*this, "cmbOptions", wxComboBox)->Enable(false);
	XRCCTRL(*this, "cmbFileSets", wxComboBox)->Enable(false);
	if (!pt)
		return;

	for (unsigned int i = 0; i < pt->m_TemplateOptions.GetCount(); ++i)
	{
		TemplateOption& opt = pt->m_TemplateOptions[i];
		XRCCTRL(*this, "cmbOptions", wxComboBox)->Append(opt.name);
	}
	XRCCTRL(*this, "cmbFileSets", wxComboBox)->Clear();
	for (unsigned int i = 0; i < pt->m_FileSets.GetCount(); ++i)
	{
		FileSet& fs = pt->m_FileSets[i];
		XRCCTRL(*this, "cmbFileSets", wxComboBox)->Append(fs.title);
	}
	XRCCTRL(*this, "cmbOptions", wxComboBox)->Enable(pt->m_TemplateOptions.GetCount());
	XRCCTRL(*this, "cmbOptions", wxComboBox)->SetSelection(0);
	XRCCTRL(*this, "cmbFileSets", wxComboBox)->Enable(pt->m_FileSets.GetCount());
	XRCCTRL(*this, "cmbFileSets", wxComboBox)->SetSelection(0);
}

bool NewFromTemplateDlg::DoNotCreateFiles()
{
    return XRCCTRL(*this, "chkDoNotCreateFiles", wxCheckBox)->IsChecked();
}

bool NewFromTemplateDlg::SelectedTemplate()
{
    return  XRCCTRL(*this, "nbMain", wxNotebook)->GetSelection() == 0 &&
            XRCCTRL(*this, "listTemplates", wxListCtrl)->GetSelectedItemCount() != 0;
}

bool NewFromTemplateDlg::SelectedUserTemplate()
{
    return  XRCCTRL(*this, "nbMain", wxNotebook)->GetSelection() == 1 &&
            XRCCTRL(*this, "lstUser", wxListBox)->GetSelection() != -1;
}

wxString NewFromTemplateDlg::GetSelectedUserTemplate()
{
    int sel = XRCCTRL(*this, "lstUser", wxListBox)->GetSelection();
    return sel != -1 ? XRCCTRL(*this, "lstUser", wxListBox)->GetString(sel) : _T("");
}

void NewFromTemplateDlg::OnListSelection(wxListEvent& event)
{
	ListItemData* data = (ListItemData*)event.GetData();
	XRCCTRL(*this, "cmbOptions", wxComboBox)->Enable(event.GetIndex() != -1 && data->pt);
	XRCCTRL(*this, "cmbFileSets", wxComboBox)->Enable(event.GetIndex() != -1 && data->pt);

	m_pWizard = data->plugin;
	FillTemplate(data->pt);
}

void NewFromTemplateDlg::OnCategoryChanged(wxCommandEvent& event)
{
	BuildList();
}

void NewFromTemplateDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
	XRCCTRL(*this, "wxID_OK", wxButton)->Enable(SelectedTemplate() || SelectedUserTemplate());
}
