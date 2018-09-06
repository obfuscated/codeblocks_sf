#include "sdk.h"

#ifndef CB_PRECOMP
    #include "cbproject.h"
    #include "configmanager.h"
    #include "editorcolourset.h"
    #include "editormanager.h"
    #include "globals.h"
    #include "manager.h"
    #include "projectmanager.h"
#endif
#include "manageglobsdlg.h"
#include "editprojectglobsdlg.h"

#include <algorithm>    // std::sort

//(*InternalHeaders(ManageGlobsDlg)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(ManageGlobsDlg)
const long ManageGlobsDlg::ID_LISTCTRL = wxNewId();
const long ManageGlobsDlg::ID_BUTTON_ADD = wxNewId();
const long ManageGlobsDlg::ID_BUTTON_DELETE = wxNewId();
const long ManageGlobsDlg::ID_BUTTON_EDIT = wxNewId();
//*)

BEGIN_EVENT_TABLE(ManageGlobsDlg,wxDialog)
    //(*EventTable(ManageGlobsDlg)
    //*)
END_EVENT_TABLE()

ManageGlobsDlg::ManageGlobsDlg(wxWindow* parent, wxWindowID id)
{
    //(*Initialize(ManageGlobsDlg)
    wxButton* btnAdd;
    wxButton* btnCancel;
    wxButton* btnDelete;
    wxButton* btnEdit;
    wxButton* btnOk;
    wxGridBagSizer* GridBagSizer1;
    wxGridBagSizer* GridBagSizer2;
    wxStaticLine* StaticLine1;

    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("id"));
    SetClientSize(wxDefaultSize);
    Move(wxDefaultPosition);
    GridBagSizer1 = new wxGridBagSizer(0, 0);
    m_ListGlobs = new wxListCtrl(this, ID_LISTCTRL, wxDefaultPosition, wxDefaultSize, wxLC_REPORT, wxDefaultValidator, _T("ID_LISTCTRL"));
    GridBagSizer1->Add(m_ListGlobs, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxEXPAND, 5);
    GridBagSizer2 = new wxGridBagSizer(0, 0);
    btnAdd = new wxButton(this, ID_BUTTON_ADD, _("Add"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_ADD"));
    GridBagSizer2->Add(btnAdd, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    btnDelete = new wxButton(this, ID_BUTTON_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_DELETE"));
    GridBagSizer2->Add(btnDelete, wxGBPosition(1, 0), wxDefaultSpan, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    btnEdit = new wxButton(this, ID_BUTTON_EDIT, _("Edit"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_EDIT"));
    GridBagSizer2->Add(btnEdit, wxGBPosition(2, 0), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticLine1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("wxID_ANY"));
    GridBagSizer2->Add(StaticLine1, wxGBPosition(3, 0), wxDefaultSpan, wxALL|wxEXPAND, 5);
    btnCancel = new wxButton(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_CANCEL"));
    GridBagSizer2->Add(btnCancel, wxGBPosition(4, 0), wxDefaultSpan, wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    btnOk = new wxButton(this, wxID_OK, _("Ok"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_OK"));
    btnOk->SetDefault();
    GridBagSizer2->Add(btnOk, wxGBPosition(5, 0), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    GridBagSizer1->Add(GridBagSizer2, wxGBPosition(0, 1), wxDefaultSpan, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    GridBagSizer1->AddGrowableCol(0);
    GridBagSizer1->AddGrowableRow(0);
    SetSizer(GridBagSizer1);
    GridBagSizer1->Fit(this);
    GridBagSizer1->SetSizeHints(this);

    Connect(ID_BUTTON_ADD,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ManageGlobsDlg::OnAddClick);
    Connect(ID_BUTTON_DELETE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ManageGlobsDlg::OnDeleteClick);
    Connect(ID_BUTTON_EDIT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ManageGlobsDlg::OnEditClick);
    Connect(wxID_OK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ManageGlobsDlg::OnOkClick);
    //*)

    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (prj != nullptr)
    {
        m_GlobList = prj->GetGlobs();
        wxString title = prj->GetTitle();
        SetTitle(wxString::Format(_("Edit globs of %s"), title.wx_str()));
    }

    m_ListGlobs->InsertColumn(0, _("Path"));
    m_ListGlobs->InsertColumn(1, _("Recursive"), wxLIST_FORMAT_CENTRE);
    m_ListGlobs->InsertColumn(2, _("Wildcard"));

    PopulateList();

}

ManageGlobsDlg::~ManageGlobsDlg()
{
    //(*Destroy(ManageGlobsDlg)
    //*)
}

void ManageGlobsDlg::PopulateList()
{
    m_ListGlobs->DeleteAllItems();
    for (const cbProject::Glob &glob : m_GlobList)
    {
        const long item = m_ListGlobs->InsertItem(m_ListGlobs->GetItemCount(), glob.m_Path);
        m_ListGlobs->SetItem(item, 1, glob.m_Recursive ? _("Yes") : _("No"));
        m_ListGlobs->SetItem(item, 2, glob.m_WildCard);
    }

    if (m_ListGlobs->GetItemCount() > 0)
    {
        for (int column = 0; column < m_ListGlobs->GetColumnCount(); ++column)
            m_ListGlobs->SetColumnWidth(column, wxLIST_AUTOSIZE_USEHEADER);
    }
}

void ManageGlobsDlg::OnAddClick(cb_unused wxCommandEvent& event)
{
    cbProject::Glob tmpGlob(wxString(), wxT("*.*"), false);
    EditProjectGlobsDlg dlg(tmpGlob, nullptr);

    if (dlg.ShowModal() == wxID_OK)
    {
        tmpGlob = dlg.GetGlob();
        m_GlobList.push_back(tmpGlob);
        PopulateList();
    }
}

void ManageGlobsDlg::OnDeleteClick(cb_unused wxCommandEvent& event)
{
    int item = -1;
    std::vector<int> items;
    for (;;)
    {
        item = m_ListGlobs->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (item == -1 )
            break;
        items.push_back(item);
    }

    // Make sure, that the list is in ascending order
    std::sort(items.begin(), items.end());

    // Start at the end and delete all selected entries from top to bottom, so that the distance to
    // m_GlobList.begin() is always correct.
    for (std::vector<int>::reverse_iterator it = items.rbegin(); it != items.rend(); ++it)
    {
        m_GlobList.erase(m_GlobList.begin() + *it);
    }

    PopulateList();
}

void ManageGlobsDlg::OnEditClick(cb_unused wxCommandEvent& event)
{
    const int item = m_ListGlobs->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item == -1)
        return;

    EditProjectGlobsDlg dlg(m_GlobList[item], this);
    if (dlg.ShowModal() == wxID_OK)
    {
        m_GlobList[item] = dlg.GetGlob();
        PopulateList();
    }
}

void ManageGlobsDlg::OnOkClick(cb_unused wxCommandEvent& event)
{
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (prj != nullptr && GlobsChanged())
    {
        prj->SetGlobs(m_GlobList);
        prj->SetModified(true);
        if (cbMessageBox(_("Globs have been changed. The project has to be saved and reloaded for the options to take place.\nDo it now?"),
                        _("Globs have been changed"),
                        wxICON_QUESTION | wxYES_NO, this) == wxID_YES)
        {
            if (prj->Save() == false)
            {
                cbMessageBox(_("Project could not be saved."), _("Error saving project!"),
                             wxICON_ERROR | wxOK, this);
                EndModal(wxID_CANCEL);
                return;
            }
            Manager::Get()->GetProjectManager()->ReloadProject(prj);
        }
    }
    EndModal(wxID_OK);
}

bool ManageGlobsDlg::GlobsChanged()
{
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (!prj)
        return true;

    const std::vector<cbProject::Glob> &projectGlobs = prj->GetGlobs();
    if (projectGlobs.size() != m_GlobList.size())
        return true;

    for (size_t i = 0; i < m_GlobList.size(); ++i)
    {
        if (m_GlobList[i] != projectGlobs[i])
            return true;
    }
    return false;
}
