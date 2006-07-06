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
    #include <wx/choice.h>
    #include <wx/imaglist.h>
    #include <wx/intl.h>
    #include <wx/listbox.h>
    #include <wx/listctrl.h>
    #include <wx/menu.h>
    #include <wx/string.h>
    #include <wx/xrc/xmlres.h>
    #include "cbeditor.h"
    #include "configmanager.h"
    #include "editormanager.h"
    #include "globals.h"
    #include "manager.h"
#endif

#include <wx/listbook.h>
#include "newfromtemplatedlg.h"
#include "projecttemplateloader.h"

struct ListItemData
{
	ListItemData(ProjectTemplateLoader* t = 0, cbWizardPlugin* p = 0, int i = 0) : pt(t), plugin(p), wizPluginIndex(i) {}
	ListItemData(const ListItemData& rhs) : pt(rhs.pt), plugin(rhs.plugin) {}
	ProjectTemplateLoader* pt;
	cbWizardPlugin* plugin;
	int wizPluginIndex;
};

static int idEditWizardScript = wxNewId();
static int idEditGlobalWizardScript = wxNewId();

BEGIN_EVENT_TABLE(NewFromTemplateDlg, wxDialog)
    EVT_UPDATE_UI(-1, NewFromTemplateDlg::OnUpdateUI)

    // projects
	EVT_LIST_ITEM_RIGHT_CLICK(XRCID("listProjects"), NewFromTemplateDlg::OnListRightClick)
	EVT_CHOICE(XRCID("cmbProjectCategories"), NewFromTemplateDlg::OnCategoryChanged)

    // targets
	EVT_LIST_ITEM_RIGHT_CLICK(XRCID("listTargets"), NewFromTemplateDlg::OnListRightClick)
	EVT_CHOICE(XRCID("cmbTargetCategories"), NewFromTemplateDlg::OnCategoryChanged)

    // files
	EVT_LIST_ITEM_RIGHT_CLICK(XRCID("listFiles"), NewFromTemplateDlg::OnListRightClick)
	EVT_CHOICE(XRCID("cmbFileCategories"), NewFromTemplateDlg::OnCategoryChanged)

    // workspaces
	EVT_LIST_ITEM_RIGHT_CLICK(XRCID("listCustoms"), NewFromTemplateDlg::OnListRightClick)
	EVT_CHOICE(XRCID("cmbCustomCategories"), NewFromTemplateDlg::OnCategoryChanged)

    // context menu for wizard scripts
	EVT_MENU(idEditWizardScript, NewFromTemplateDlg::OnEditScript)
	EVT_MENU(idEditGlobalWizardScript, NewFromTemplateDlg::OnEditGlobalScript)
END_EVENT_TABLE()

NewFromTemplateDlg::NewFromTemplateDlg(TemplateOutputType initial, const wxArrayString& user_templates)
	: m_Template(0L),
	m_pWizard(0L),
	m_WizardIndex(-1)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, 0L, _T("dlgNewFromTemplate"));
	m_Wizards = Manager::Get()->GetPluginManager()->GetOffersFor(ptWizard);

    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    SetSettingsIconsStyle(lb->GetListView(), sisNoIcons);

    // create image lists
    XRCCTRL(*this, "listProjects", wxListCtrl)->SetImageList(new wxImageList(32, 32), wxIMAGE_LIST_NORMAL);
    XRCCTRL(*this, "listTargets", wxListCtrl)->SetImageList(new wxImageList(32, 32), wxIMAGE_LIST_NORMAL);
    XRCCTRL(*this, "listFiles", wxListCtrl)->SetImageList(new wxImageList(32, 32), wxIMAGE_LIST_NORMAL);
    XRCCTRL(*this, "listCustoms", wxListCtrl)->SetImageList(new wxImageList(32, 32), wxIMAGE_LIST_NORMAL);

	BuildCategories();
	BuildList();

    // fill user templates list
    XRCCTRL(*this, "lstUser", wxListBox)->Clear();
    for (unsigned int i = 0; i < user_templates.GetCount(); ++i)
    {
        XRCCTRL(*this, "lstUser", wxListBox)->Append(user_templates[i]);
    }

    lb->SetSelection((int)initial);
}

NewFromTemplateDlg::~NewFromTemplateDlg()
{
	//dtor
    delete XRCCTRL(*this, "listProjects", wxListCtrl)->GetImageList(wxIMAGE_LIST_NORMAL);
    delete XRCCTRL(*this, "listTargets", wxListCtrl)->GetImageList(wxIMAGE_LIST_NORMAL);
    delete XRCCTRL(*this, "listFiles", wxListCtrl)->GetImageList(wxIMAGE_LIST_NORMAL);
    delete XRCCTRL(*this, "listCustoms", wxListCtrl)->GetImageList(wxIMAGE_LIST_NORMAL);

    XRCCTRL(*this, "listProjects", wxListCtrl)->SetImageList(0, wxIMAGE_LIST_NORMAL);
    XRCCTRL(*this, "listTargets", wxListCtrl)->SetImageList(0, wxIMAGE_LIST_NORMAL);
    XRCCTRL(*this, "listFiles", wxListCtrl)->SetImageList(0, wxIMAGE_LIST_NORMAL);
    XRCCTRL(*this, "listCustoms", wxListCtrl)->SetImageList(0, wxIMAGE_LIST_NORMAL);

	ClearList();
}

void NewFromTemplateDlg::ClearList()
{
	ClearListFor(XRCCTRL(*this, "listProjects", wxListCtrl));
	ClearListFor(XRCCTRL(*this, "listTargets", wxListCtrl));
	ClearListFor(XRCCTRL(*this, "listFiles", wxListCtrl));
	ClearListFor(XRCCTRL(*this, "listCustoms", wxListCtrl));
}

void NewFromTemplateDlg::ClearListFor(wxListCtrl* list)
{
    if (!list)
        return;
	for (int i = 0; i < list->GetItemCount(); ++i)
	{
		ListItemData* data = (ListItemData*)list->GetItemData(i);
		delete data;
	}
	list->ClearAll();
}

void NewFromTemplateDlg::BuildCategories()
{
	BuildCategoriesFor(totProject, XRCCTRL(*this, "cmbProjectCategories", wxChoice));
	BuildCategoriesFor(totTarget, XRCCTRL(*this, "cmbTargetCategories", wxChoice));
	BuildCategoriesFor(totFiles, XRCCTRL(*this, "cmbFileCategories", wxChoice));
	BuildCategoriesFor(totCustom, XRCCTRL(*this, "cmbCustomCategories", wxChoice));
}

void NewFromTemplateDlg::BuildCategoriesFor(TemplateOutputType otype, wxChoice* cat)
{
    if (!cat)
        return;
	cat->Clear();
	cat->Append(_("<All categories>"));

    // wizards
    for (unsigned int i = 0; i < m_Wizards.GetCount(); ++i)
    {
        cbWizardPlugin* plugin = (cbWizardPlugin*)m_Wizards[i];
        for (int w = 0; w < plugin->GetCount(); ++w)
        {
            if (plugin->GetOutputType(w) != otype)
                continue;

            if (cat->FindString(plugin->GetCategory(w)) == wxNOT_FOUND)
                cat->Append(plugin->GetCategory(w));
        }
    }
	cat->SetSelection(0);
}

int wxCALLBACK SortTemplates(long item1, long item2, long sortData)
{
    ListItemData* data1 = reinterpret_cast<ListItemData*>(item1);
    ListItemData* data2 = reinterpret_cast<ListItemData*>(item2);

    if (!data1 && data2)
        return 1;
    if (data1 && !data2)
        return -1;
    if (data1 && data2)
    {
        wxString name1 = data1->pt ? data1->pt->m_Title : data1->plugin->GetTitle(data1->wizPluginIndex);
        wxString name2 = data2->pt ? data2->pt->m_Title : data2->plugin->GetTitle(data2->wizPluginIndex);

        return name1.CompareTo(name2);
    }
    return 0;
}

void NewFromTemplateDlg::BuildList()
{
	BuildListFor(totProject, XRCCTRL(*this, "listProjects", wxListCtrl), XRCCTRL(*this, "cmbProjectCategories", wxChoice));
	BuildListFor(totTarget, XRCCTRL(*this, "listTargets", wxListCtrl), XRCCTRL(*this, "cmbTargetCategories", wxChoice));
	BuildListFor(totFiles, XRCCTRL(*this, "listFiles", wxListCtrl), XRCCTRL(*this, "cmbFileCategories", wxChoice));
	BuildListFor(totCustom, XRCCTRL(*this, "listCustoms", wxListCtrl), XRCCTRL(*this, "cmbCustomCategories", wxChoice));
}

void NewFromTemplateDlg::BuildListFor(TemplateOutputType otype, wxListCtrl* list, const wxChoice* cat)
{
    if (!list || !cat)
        return;
	ClearListFor(list);
	list->GetImageList(wxIMAGE_LIST_NORMAL)->RemoveAll();

    wxBitmap bmp;
    bool all = cat->GetSelection() == 0;

    // wizards
    for (unsigned int i = 0; i < m_Wizards.GetCount(); ++i)
    {
        cbWizardPlugin* plugin = (cbWizardPlugin*)m_Wizards[i];
        for (int w = 0; w < plugin->GetCount(); ++w)
        {
            if (plugin->GetOutputType(w) != otype)
                continue;

            if (all || plugin->GetCategory(w).Matches(cat->GetStringSelection()))
            {
                int idx = plugin->GetBitmap(w).Ok() ? list->GetImageList(wxIMAGE_LIST_NORMAL)->Add(plugin->GetBitmap(w)) : -2;
                int index = list->InsertItem(0, plugin->GetTitle(w), idx);
                if (index != -1)
                    list->SetItemData(index, (long)(new ListItemData(0, plugin, w)));
            }
        }
    }

    list->SortItems(SortTemplates, 0);
}

wxListCtrl* NewFromTemplateDlg::GetVisibleListCtrl()
{
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    size_t page = lb->GetSelection();

    switch (page)
    {
        case 0: return XRCCTRL(*this, "listProjects", wxListCtrl); // projects
        case 1: return XRCCTRL(*this, "listTargets", wxListCtrl); // targets
        case 2: return XRCCTRL(*this, "listFiles", wxListCtrl); // files
        case 3: return XRCCTRL(*this, "listCustoms", wxListCtrl); // workspaces
        default: return 0;
    }
}

wxChoice* NewFromTemplateDlg::GetVisibleCategory()
{
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    size_t page = lb->GetSelection();

    switch (page)
    {
        case 0: return XRCCTRL(*this, "cmbProjectCategories", wxChoice); // projects
        case 1: return XRCCTRL(*this, "cmbTargetCategories", wxChoice); // targets
        case 2: return XRCCTRL(*this, "cmbFileCategories", wxChoice); // files
        case 3: return XRCCTRL(*this, "cmbCustomCategories", wxChoice); // workspaces
        default: return 0;
    }
}

TemplateOutputType NewFromTemplateDlg::GetVisibleOutputType() const
{
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    size_t page = lb->GetSelection();

    switch (page)
    {
        case 0: return totProject;
        case 1: return totTarget;
        case 2: return totFiles;
        case 3: return totCustom;
        default: return totProject;
    }
}

cbWizardPlugin* NewFromTemplateDlg::GetSelectedTemplate()
{
    m_pWizard = 0;
    m_WizardIndex = -1;
    wxListCtrl* list = GetVisibleListCtrl();
    if (!list)
        return 0;
    long index = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (index == -1)
        return 0;
	ListItemData* data = (ListItemData*)list->GetItemData(index);
    m_pWizard = data->plugin;
    m_WizardIndex = data->wizPluginIndex;
    return m_pWizard;
}

bool NewFromTemplateDlg::SelectedUserTemplate() const
{
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    size_t page = lb->GetSelection();
    return  page == lb->GetPageCount() - 1 &&
            XRCCTRL(*this, "lstUser", wxListBox)->GetSelection() != -1;
}

wxString NewFromTemplateDlg::GetSelectedUserTemplate() const
{
    int sel = XRCCTRL(*this, "lstUser", wxListBox)->GetSelection();
    return sel != -1 ? XRCCTRL(*this, "lstUser", wxListBox)->GetString(sel) : _T("");
}

void NewFromTemplateDlg::EditScript(const wxString& relativeFilename)
{
    if (relativeFilename.IsEmpty())
        return;
    wxString script = ConfigManager::GetDataFolder() + _T("/templates/wizard/");
    script += relativeFilename;
    cbEditor* ed = Manager::Get()->GetEditorManager()->Open(script);
    if (ed)
    {
        ed->GetControl()->SetFocus();
        EndModal(wxID_CANCEL);
        return;
    }
    cbMessageBox(_("Couldn't open script:\n") + script, _("Error"), wxICON_ERROR);
}

void NewFromTemplateDlg::OnListRightClick(wxListEvent& event)
{
    wxListCtrl* list = GetVisibleListCtrl();
    if (!list)
        return;
    wxMenu* menu = new wxMenu;
	ListItemData* data = (ListItemData*)event.GetData();

	if (data && data->plugin)
	{
	    menu->Append(idEditWizardScript, _("Edit this wizard's script"));
	}
    menu->Append(idEditGlobalWizardScript, _("Edit global registration script"));
	list->PopupMenu(menu);
	delete menu;
}

void NewFromTemplateDlg::OnCategoryChanged(wxCommandEvent& event)
{
    BuildListFor(GetVisibleOutputType(), GetVisibleListCtrl(), GetVisibleCategory());
}

void NewFromTemplateDlg::OnEditScript(wxCommandEvent& event)
{
    wxListCtrl* list = GetVisibleListCtrl();
    if (!list)
        return;
    long index = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (index == -1)
        return;
	ListItemData* data = (ListItemData*)list->GetItemData(index);
    cbWizardPlugin* wiz = data->plugin;
    EditScript(wiz->GetScriptFilename(data->wizPluginIndex)); // script's filename is always relative to the above path
}

void NewFromTemplateDlg::OnEditGlobalScript(wxCommandEvent& event)
{
    cbMessageBox(_("Any changes you make to the global wizard registration script will "
                    "take effect after you restart Code::Blocks."),
                    _("Information"), wxICON_INFORMATION);
    EditScript(_T("config.script"));
}

void NewFromTemplateDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    size_t page = lb->GetSelection();

	XRCCTRL(*this, "wxID_OK", wxButton)->Enable((page != lb->GetPageCount() - 1 && GetSelectedTemplate()) || SelectedUserTemplate());
}

void NewFromTemplateDlg::EndModal(int retCode)
{
    GetSelectedTemplate();
    wxDialog::EndModal(retCode);
}
