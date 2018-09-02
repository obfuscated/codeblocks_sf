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
const long ManageGlobsDlg::ID_LISTCTRL1 = wxNewId();
const long ManageGlobsDlg::ID_BUTTON1 = wxNewId();
const long ManageGlobsDlg::ID_BUTTON2 = wxNewId();
const long ManageGlobsDlg::ID_BUTTON3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ManageGlobsDlg,wxDialog)
	//(*EventTable(ManageGlobsDlg)
	//*)
END_EVENT_TABLE()

ManageGlobsDlg::ManageGlobsDlg(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(ManageGlobsDlg)
	wxGridBagSizer* GridBagSizer1;
	wxGridBagSizer* GridBagSizer2;
	wxStaticLine* StaticLine1;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	GridBagSizer1 = new wxGridBagSizer(0, 0);
	lstGlobsList = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT, wxDefaultValidator, _T("ID_LISTCTRL1"));
	GridBagSizer1->Add(lstGlobsList, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxEXPAND, 5);
	GridBagSizer2 = new wxGridBagSizer(0, 0);
	btnAdd = new wxButton(this, ID_BUTTON1, _("Add"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	GridBagSizer2->Add(btnAdd, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	btnDelete = new wxButton(this, ID_BUTTON2, _("Delete"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	GridBagSizer2->Add(btnDelete, wxGBPosition(1, 0), wxDefaultSpan, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	btnEdit = new wxButton(this, ID_BUTTON3, _("Edit"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
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

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ManageGlobsDlg::OnAddClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ManageGlobsDlg::OnDeleteClick);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ManageGlobsDlg::OnEditClick);
	Connect(wxID_OK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ManageGlobsDlg::OnOkClick);
	//*)

    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if(prj != nullptr)
    {
        m_GlobList = prj->GetGlobs();
        wxString title = prj->GetTitle();
        this->SetTitle(wxString::Format(_("Edit globs of %s"), title.wx_str()));
    }

    lstGlobsList->InsertColumn(0, _("Path"));
    lstGlobsList->InsertColumn(1, _("Recursive"), wxLIST_FORMAT_CENTRE);
    lstGlobsList->InsertColumn(2, _("Wildcard"));

    PopulateList();

}

ManageGlobsDlg::~ManageGlobsDlg()
{
	//(*Destroy(ManageGlobsDlg)
	//*)
}

void ManageGlobsDlg::PopulateList()
{
    lstGlobsList->DeleteAllItems();
    for (size_t i = 0; i < m_GlobList.size(); ++i)
    {
        lstGlobsList->InsertItem(i, m_GlobList[i].m_Path);
        wxString rec = wxString::Format(wxT("%i"), m_GlobList[i].m_Recursive);
        lstGlobsList->SetItem(i, 1, rec);
        lstGlobsList->SetItem(i, 2, m_GlobList[i].m_WildCard);
    }
}

void ManageGlobsDlg::OnAddClick(wxCommandEvent& event)
{
    cbProject::Glob tmpGlob(wxT(""), wxT("*.*"), false);
    EditProjectGlobsDlg dlg(tmpGlob, nullptr);

    if (dlg.ShowModal() == wxID_OK)
    {
        tmpGlob = dlg.GetGlob();
        m_GlobList.push_back(tmpGlob);
        PopulateList();
    }
}

void ManageGlobsDlg::OnDeleteClick(wxCommandEvent& event)
{
    int item = -1;
    std::vector<int> items;
    for ( ;; )
    {
        item = lstGlobsList->GetNextItem(item,
                                         wxLIST_NEXT_ALL,
                                         wxLIST_STATE_SELECTED);
        if ( item == -1 )
            break;
        items.push_back(item);
    }

    // Make sure, that the list is in ascending order
    std::sort(items.begin(), items.end());

    for (int i = items.size()-1; i >= 0; --i)
    {
        m_GlobList.erase(m_GlobList.begin() + items[i]);
    }
    PopulateList();
}

void ManageGlobsDlg::OnEditClick(wxCommandEvent& event)
{
    int item = lstGlobsList->GetNextItem(-1,
                                         wxLIST_NEXT_ALL,
                                         wxLIST_STATE_SELECTED);
    if ( item == -1 )
        return;

    EditProjectGlobsDlg dlg(m_GlobList[item], this);
    if(dlg.ShowModal() == wxID_OK)
    {
        m_GlobList[item] = dlg.GetGlob();
        PopulateList();
    }
}

void ManageGlobsDlg::OnOkClick(wxCommandEvent& event)
{
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if(prj != nullptr && GlobsChanged())
    {
        prj->SetGlobs(m_GlobList);
        prj->SetModified(true);
        if (cbMessageBox(_("Globs have been changed. The project has to be saved and reloaded for the options to take place.\nDo it now?"),
                        _("Globs have been changed"),
                        wxICON_QUESTION | wxYES_NO, this) == wxID_YES)
        {
            if(prj->Save() == false)
            {
                cbMessageBox(_("Project could not be saved."),
                             _("Error saving project!"),
                             wxICON_ERROR | wxOK, this);
                this->EndModal(wxID_CANCEL);
                return;
            }
            Manager::Get()->GetProjectManager()->ReloadProject(prj);
        }
    }
    this->EndModal(wxID_OK);
}

bool ManageGlobsDlg::GlobsChanged()
{
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if(prj != nullptr)
    {

        if(prj->GetGlobs().size() != m_GlobList.size())
            return true;

        std::vector<cbProject::Glob> prjGlobs = prj->GetGlobs();
        for (size_t i = 0; i < m_GlobList.size(); ++i)
        {
           if(m_GlobList[i].m_Path      != prjGlobs[i].m_Path     ||
              m_GlobList[i].m_WildCard  != prjGlobs[i].m_WildCard ||
              m_GlobList[i].m_Recursive != prjGlobs[i].m_Recursive  )
            return true;
        }
        return false;
    }
    return true; // in doubt return true
}
