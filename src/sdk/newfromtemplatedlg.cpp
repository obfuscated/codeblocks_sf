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
    #include <wx/button.h>
    #include <wx/choice.h>
    #include <wx/imaglist.h>
    #include <wx/intl.h>
    #include <wx/listbox.h>
    #include <wx/listctrl.h>
    #include <wx/menu.h>
    #include <wx/string.h>
    #include <wx/settings.h>
    #include <wx/xrc/xmlres.h>

    #include "cbeditor.h"
    #include "configmanager.h"
    #include "editormanager.h"
    #include "globals.h"
    #include "manager.h"
#endif
#include "cbstyledtextctrl.h"

#include <wx/listbook.h>
#include <wx/radiobox.h>

#include "newfromtemplatedlg.h"
#include "projecttemplateloader.h"

struct ListItemData
{
    ListItemData(ProjectTemplateLoader* t = nullptr, cbWizardPlugin* p = nullptr, int i = 0) : pt(t), plugin(p), wizPluginIndex(i) {}
    ListItemData(const ListItemData& rhs) : pt(rhs.pt), plugin(rhs.plugin) {}
    ProjectTemplateLoader* pt;
    cbWizardPlugin* plugin;
    int wizPluginIndex;
};

static int idEditWizardScript = wxNewId();
static int idDiscardWizardScript = wxNewId();
static int idInfoWizardScript = wxNewId();
static int idEditGlobalWizardScript = wxNewId();

BEGIN_EVENT_TABLE(NewFromTemplateDlg, wxScrollingDialog)
    EVT_UPDATE_UI(-1, NewFromTemplateDlg::OnUpdateUI)

    // projects
    EVT_LIST_ITEM_RIGHT_CLICK(XRCID("listProjects"), NewFromTemplateDlg::OnListRightClick)
    EVT_LIST_ITEM_ACTIVATED(XRCID("listProjects"), NewFromTemplateDlg::OnListActivate)
    EVT_CHOICE(XRCID("cmbProjectCategories"), NewFromTemplateDlg::OnCategoryChanged)

    // targets
    EVT_LIST_ITEM_RIGHT_CLICK(XRCID("listTargets"), NewFromTemplateDlg::OnListRightClick)
    EVT_LIST_ITEM_ACTIVATED(XRCID("listTargets"), NewFromTemplateDlg::OnListActivate)
    EVT_CHOICE(XRCID("cmbTargetCategories"), NewFromTemplateDlg::OnCategoryChanged)

    // files
    EVT_LIST_ITEM_RIGHT_CLICK(XRCID("listFiles"), NewFromTemplateDlg::OnListRightClick)
    EVT_LIST_ITEM_ACTIVATED(XRCID("listFiles"), NewFromTemplateDlg::OnListActivate)
    EVT_CHOICE(XRCID("cmbFileCategories"), NewFromTemplateDlg::OnCategoryChanged)

    // workspaces
    EVT_LIST_ITEM_RIGHT_CLICK(XRCID("listCustoms"), NewFromTemplateDlg::OnListRightClick)
    EVT_LIST_ITEM_ACTIVATED(XRCID("listCustoms"), NewFromTemplateDlg::OnListActivate)
    EVT_CHOICE(XRCID("cmbCustomCategories"), NewFromTemplateDlg::OnCategoryChanged)

    // context menu for wizard scripts
    EVT_MENU(idEditWizardScript, NewFromTemplateDlg::OnEditScript)
    EVT_MENU(idDiscardWizardScript, NewFromTemplateDlg::OnDiscardScript)
    EVT_MENU(idEditGlobalWizardScript, NewFromTemplateDlg::OnEditGlobalScript)

    EVT_RADIOBOX(XRCID("rbView"), NewFromTemplateDlg::OnViewChange)
    EVT_MENU(idInfoWizardScript, NewFromTemplateDlg::OnHelp)
END_EVENT_TABLE()

NewFromTemplateDlg::NewFromTemplateDlg(TemplateOutputType initial, const wxArrayString& user_templates)
    : m_Template(nullptr),
    m_pWizard(nullptr),
    m_WizardIndex(-1)
{
    //ctor
    wxXmlResource::Get()->LoadObject(this, nullptr, _T("dlgNewFromTemplate"),_T("wxScrollingDialog"));
    m_Wizards = Manager::Get()->GetPluginManager()->GetOffersFor(ptWizard);

    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    SetSettingsIconsStyle(lb->GetListView(), sisNoIcons);

    // create image lists
    XRCCTRL(*this, "listProjects", wxListCtrl)->SetImageList(new wxImageList(32, 32), wxIMAGE_LIST_NORMAL);
    XRCCTRL(*this, "listProjects", wxListCtrl)->SetImageList(new wxImageList(32, 32), wxIMAGE_LIST_SMALL);
    XRCCTRL(*this, "listTargets", wxListCtrl)->SetImageList(new wxImageList(32, 32), wxIMAGE_LIST_NORMAL);
    XRCCTRL(*this, "listTargets", wxListCtrl)->SetImageList(new wxImageList(32, 32), wxIMAGE_LIST_SMALL);
    XRCCTRL(*this, "listFiles", wxListCtrl)->SetImageList(new wxImageList(32, 32), wxIMAGE_LIST_NORMAL);
    XRCCTRL(*this, "listFiles", wxListCtrl)->SetImageList(new wxImageList(32, 32), wxIMAGE_LIST_SMALL);
    XRCCTRL(*this, "listCustoms", wxListCtrl)->SetImageList(new wxImageList(32, 32), wxIMAGE_LIST_NORMAL);
    XRCCTRL(*this, "listCustoms", wxListCtrl)->SetImageList(new wxImageList(32, 32), wxIMAGE_LIST_SMALL);

    // load view prefs
    XRCCTRL(*this, "rbView", wxRadioBox)->SetSelection(Manager::Get()->GetConfigManager(_T("new_from_template"))->ReadInt(_T("/view"), 0));
    ChangeView();

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
    delete XRCCTRL(*this, "listProjects", wxListCtrl)->GetImageList(wxIMAGE_LIST_SMALL);
    delete XRCCTRL(*this, "listTargets", wxListCtrl)->GetImageList(wxIMAGE_LIST_NORMAL);
    delete XRCCTRL(*this, "listTargets", wxListCtrl)->GetImageList(wxIMAGE_LIST_SMALL);
    delete XRCCTRL(*this, "listFiles", wxListCtrl)->GetImageList(wxIMAGE_LIST_NORMAL);
    delete XRCCTRL(*this, "listFiles", wxListCtrl)->GetImageList(wxIMAGE_LIST_SMALL);
    delete XRCCTRL(*this, "listCustoms", wxListCtrl)->GetImageList(wxIMAGE_LIST_NORMAL);
    delete XRCCTRL(*this, "listCustoms", wxListCtrl)->GetImageList(wxIMAGE_LIST_SMALL);

    XRCCTRL(*this, "listProjects", wxListCtrl)->SetImageList(nullptr, wxIMAGE_LIST_NORMAL);
    XRCCTRL(*this, "listProjects", wxListCtrl)->SetImageList(nullptr, wxIMAGE_LIST_SMALL);
    XRCCTRL(*this, "listTargets",  wxListCtrl)->SetImageList(nullptr, wxIMAGE_LIST_NORMAL);
    XRCCTRL(*this, "listTargets",  wxListCtrl)->SetImageList(nullptr, wxIMAGE_LIST_SMALL);
    XRCCTRL(*this, "listFiles",    wxListCtrl)->SetImageList(nullptr, wxIMAGE_LIST_NORMAL);
    XRCCTRL(*this, "listFiles",    wxListCtrl)->SetImageList(nullptr, wxIMAGE_LIST_SMALL);
    XRCCTRL(*this, "listCustoms",  wxListCtrl)->SetImageList(nullptr, wxIMAGE_LIST_NORMAL);
    XRCCTRL(*this, "listCustoms",  wxListCtrl)->SetImageList(nullptr, wxIMAGE_LIST_SMALL);

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

#if wxCHECK_VERSION(3, 0, 0)
inline int wxCALLBACK SortTemplates(wxIntPtr item1, wxIntPtr item2, cb_unused wxIntPtr sortData)
#else
inline int wxCALLBACK SortTemplates(long item1, long item2, cb_unused long sortData)
#endif
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

        return name1.CompareTo(name2.wx_str());
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
    list->GetImageList(wxIMAGE_LIST_SMALL)->RemoveAll();

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
                int iconIndex;
                if (plugin->GetBitmap(w).Ok())
                {
                    iconIndex = list->GetImageList(wxIMAGE_LIST_NORMAL)->Add(plugin->GetBitmap(w));
                    list->GetImageList(wxIMAGE_LIST_SMALL)->Add(plugin->GetBitmap(w));
                }
                else
                    iconIndex = -2;
                int index = list->InsertItem(0, plugin->GetTitle(w), iconIndex);
                if (index != -1)
                {
                    list->SetItemData(index, (wxIntPtr)(new ListItemData(nullptr, plugin, w)));
                    // if the script exists in the user's configuration, mark that it's been customized
                    wxString script = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/") + plugin->GetScriptFilename(w);
                    if (wxFileExists(script))
                    {
                        list->SetItemTextColour(index, *wxRED);
                    }
                }
            }
        }
    }

    list->SortItems(SortTemplates, (wxIntPtr)0);
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
        default: return nullptr;
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
        default: return nullptr;
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
    m_pWizard = nullptr;
    m_WizardIndex = -1;
    wxListCtrl* list = GetVisibleListCtrl();
    if (!list)
        return nullptr;
    long index = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (index == -1)
        return nullptr;
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

void NewFromTemplateDlg::EditScript(const wxString& filename)
{
    if (filename.IsEmpty())
        return;

    // edited before?
    bool first_time = false;
    wxString script = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/") + filename;
    if (!wxFileExists(script))
    {
        first_time = true;
        script = ConfigManager::GetFolder(sdDataGlobal) + _T("/templates/wizard/") + filename;
    }

    cbEditor* ed = Manager::Get()->GetEditorManager()->Open(script);
    if (ed)
    {
        ed->GetControl()->SetFocus();
        if (first_time)
        {
            // first time editing this script; change the filename to point to the user's dir
            ed->SetFilename(ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/") + filename);
            ed->SetModified(true);
            // also make sure the destination directory exists
            CreateDirRecursively(ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/") + filename);
        }
        EndModal(wxID_CANCEL);
        return;
    }
    cbMessageBox(_("Couldn't open script:\n") + script, _("Error"), wxICON_ERROR, this);
}

void NewFromTemplateDlg::ChangeView()
{
    int sel = XRCCTRL(*this, "rbView", wxRadioBox)->GetSelection();
    int style = sel == 0 ? wxLC_ICON : wxLC_LIST;

    XRCCTRL(*this, "listProjects", wxListCtrl)->SetSingleStyle(style);
    XRCCTRL(*this, "listTargets",  wxListCtrl)->SetSingleStyle(style);
    XRCCTRL(*this, "listFiles",    wxListCtrl)->SetSingleStyle(style);
    XRCCTRL(*this, "listCustoms",  wxListCtrl)->SetSingleStyle(style);

    // in non-windows platforms, the above clears the control contents too
    // so rebuild them
#ifndef __WXMSW__
    BuildListFor(totProject, XRCCTRL(*this, "listProjects", wxListCtrl), XRCCTRL(*this, "cmbProjectCategories", wxChoice));
    BuildListFor(totTarget,  XRCCTRL(*this, "listTargets",  wxListCtrl), XRCCTRL(*this, "cmbTargetCategories",  wxChoice));
    BuildListFor(totFiles,   XRCCTRL(*this, "listFiles",    wxListCtrl), XRCCTRL(*this, "cmbFileCategories",    wxChoice));
    BuildListFor(totCustom,  XRCCTRL(*this, "listCustoms",  wxListCtrl), XRCCTRL(*this, "cmbCustomCategories",  wxChoice));
#endif
}

void NewFromTemplateDlg::OnListRightClick(wxListEvent& event)
{
    wxListCtrl* list = GetVisibleListCtrl();
    if (!list)
        return;
    wxMenu* menu = new wxMenu;
    ListItemData* data = reinterpret_cast<ListItemData*>(event.GetData());

    if (data && data->plugin)
    {
        menu->Append(idEditWizardScript, _("Edit this script"));

        // if the script exists in the user's configuration, it has been customized
        wxString script = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/") + data->plugin->GetScriptFilename(data->wizPluginIndex);
        if (wxFileExists(script))
        {
            menu->Append(idDiscardWizardScript, _("Discard modifications of this script"));
            menu->Append(idInfoWizardScript, _("Why is this script marked red?"));
            menu->AppendSeparator();
        }
    }
    menu->Append(idEditGlobalWizardScript, _("Edit global registration script"));
    list->PopupMenu(menu);
    delete menu;
}

void NewFromTemplateDlg::OnListActivate(cb_unused wxListEvent& event)
{
    EndModal(wxID_OK);
}

void NewFromTemplateDlg::OnCategoryChanged(cb_unused wxCommandEvent& event)
{
    BuildListFor(GetVisibleOutputType(), GetVisibleListCtrl(), GetVisibleCategory());
}

void NewFromTemplateDlg::OnEditScript(cb_unused wxCommandEvent& event)
{
    wxListCtrl* list = GetVisibleListCtrl();
    if (!list)
        return;
    long index = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (index == -1)
        return;
    ListItemData* data = (ListItemData*)list->GetItemData(index);
    cbWizardPlugin* wiz = data->plugin;
    EditScript(wiz->GetScriptFilename(data->wizPluginIndex));
}

void NewFromTemplateDlg::OnDiscardScript(cb_unused wxCommandEvent& event)
{
    wxListCtrl* list = GetVisibleListCtrl();
    if (!list)
        return;
    long index = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (index == -1)
        return;
    ListItemData* data = (ListItemData*)list->GetItemData(index);
    if (!data)
        return;

    wxString script = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/") + data->plugin->GetScriptFilename(data->wizPluginIndex);
    if (wxFileExists(script))
    {
        if (cbMessageBox(_("Are you sure you want to discard all local modifications to this script?"),
                        _("Confirmation"), wxICON_QUESTION | wxYES_NO, this) == wxID_YES)
        {
            if (wxRemoveFile(script))
                list->SetItemTextColour(index, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        }
    }
}

void NewFromTemplateDlg::OnEditGlobalScript(cb_unused wxCommandEvent& event)
{
    cbMessageBox(_("Any changes you make to the global wizard registration script will "
                    "take effect after you restart Code::Blocks."),
                    _("Information"), wxICON_INFORMATION, this);
    EditScript(_T("config.script"));
}

void NewFromTemplateDlg::OnViewChange(cb_unused wxCommandEvent& event)
{
    ChangeView();
}

void NewFromTemplateDlg::OnHelp(cb_unused wxCommandEvent& event)
{
    cbMessageBox(_("When you edit a wizard's script, you actually edit a copy of it which "
                    "is automatically placed inside your user configuration directory.\n"
                    "This means that if a new version of the script is released, Code::Blocks "
                    "will still use your customized script, not the globally installed version.\n\n"
                    "These customized wizard scripts are coloured red just to remind you "
                    "that they are exactly that: customized scripts.\n"
                    "So, if you update your Code::Blocks copy and find that an updated wizard's "
                    "behaviour doesn't change, check if you have customized it. If you have, "
                    "the only way to re-enable the globally installed script is to remove "
                    "the customized one.\n\n"
                    "On this computer, the customized scripts are located under:\n") +
                    ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/"),
                    _("Help"),
                    wxICON_INFORMATION, this);
}

void NewFromTemplateDlg::OnUpdateUI(cb_unused wxUpdateUIEvent& event)
{
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    size_t page = lb->GetSelection();

    XRCCTRL(*this, "wxID_OK", wxButton)->Enable((page != lb->GetPageCount() - 1 && GetSelectedTemplate()) || SelectedUserTemplate());
}

void NewFromTemplateDlg::EndModal(int retCode)
{
    // save view prefs
    int sel = XRCCTRL(*this, "rbView", wxRadioBox)->GetSelection();
    Manager::Get()->GetConfigManager(_T("new_from_template"))->Write(_T("/view"), (int)sel);

    GetSelectedTemplate();
    wxScrollingDialog::EndModal(retCode);
}
