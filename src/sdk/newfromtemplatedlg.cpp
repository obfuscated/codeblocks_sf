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

#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/combobox.h>
#include <wx/button.h>

#include "newfromtemplatedlg.h"
#include "manager.h"
#include "configmanager.h"

BEGIN_EVENT_TABLE(NewFromTemplateDlg, wxDialog)
	EVT_LIST_ITEM_SELECTED(XRCID("listTemplates"), NewFromTemplateDlg::OnListSelection)
	EVT_COMBOBOX(XRCID("cmbCategories"), NewFromTemplateDlg::OnCategoryChanged)
END_EVENT_TABLE()

NewFromTemplateDlg::NewFromTemplateDlg(const ProjectTemplateArray& templates)
	: m_Template(0L),
	m_ImageList(32, 32),
	m_Templates(templates)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, 0L, _("dlgNewFromTemplate"));
	BuildCategories();
	BuildList();
}

NewFromTemplateDlg::~NewFromTemplateDlg()
{
	//dtor
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
	cat->SetSelection(0);
}

void NewFromTemplateDlg::BuildList()
{
	wxComboBox* cat = XRCCTRL(*this, "cmbCategories", wxComboBox);
	wxListCtrl* list = XRCCTRL(*this, "listTemplates", wxListCtrl);
	list->ClearAll();
	m_ImageList.RemoveAll();
	list->SetImageList(&m_ImageList, wxIMAGE_LIST_NORMAL);

	wxBitmap bmp;
	bool all = cat->GetSelection() == 0;
	wxString baseDir = ConfigManager::Get()->Read("/data_path");
	baseDir << "/templates/";
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
				list->SetItemData(index, (long)pt);
		}
	}

	XRCCTRL(*this, "cmbOptions", wxComboBox)->Enable(false);
	XRCCTRL(*this, "cmbFileSets", wxComboBox)->Enable(false);
}

void NewFromTemplateDlg::FillTemplate(ProjectTemplateLoader* pt)
{
	if (!pt)
		return;
	
	m_Template = pt;
	XRCCTRL(*this, "cmbOptions", wxComboBox)->Clear();
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

	XRCCTRL(*this, "wxID_OK", wxButton)->Enable(pt->m_TemplateOptions.GetCount() && pt->m_FileSets.GetCount());
}

void NewFromTemplateDlg::OnListSelection(wxListEvent& event)
{
	ProjectTemplateLoader* data = (ProjectTemplateLoader*)event.GetData();
	XRCCTRL(*this, "wxID_OK", wxButton)->Enable(false);
	XRCCTRL(*this, "cmbOptions", wxComboBox)->Enable(event.GetIndex() != -1 && data);
	XRCCTRL(*this, "cmbFileSets", wxComboBox)->Enable(event.GetIndex() != -1 && data);
	
	FillTemplate(data);
}

void NewFromTemplateDlg::OnCategoryChanged(wxCommandEvent& event)
{
	BuildList();
}
