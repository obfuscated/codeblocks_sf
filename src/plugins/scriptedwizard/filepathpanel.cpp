/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#ifndef CB_PRECOMP
    #include <wx/filedlg.h>
    #include <wx/intl.h>
    #include <cbproject.h>
    #include <projectbuildtarget.h>
    #include <projectmanager.h>
    #include <manager.h>
#endif
#include <wx/filefn.h>
#include "filepathpanel.h"

//(*IdInit(FilePathPanel)
const long FilePathPanel::ID_STATICTEXT1 = wxNewId();
const long FilePathPanel::ID_STATICTEXT2 = wxNewId();
const long FilePathPanel::ID_TEXTCTRL1 = wxNewId();
const long FilePathPanel::ID_BUTTON1 = wxNewId();
const long FilePathPanel::ID_STATICTEXT3 = wxNewId();
const long FilePathPanel::ID_TEXTCTRL2 = wxNewId();
const long FilePathPanel::ID_CHECKBOX1 = wxNewId();
const long FilePathPanel::ID_STATICTEXT4 = wxNewId();
const long FilePathPanel::ID_CHECKLISTBOX2 = wxNewId();
const long FilePathPanel::ID_BUTTON2 = wxNewId();
const long FilePathPanel::ID_BUTTON3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(FilePathPanel,wxPanel)
    //(*EventTable(FilePathPanel)
    //*)
END_EVENT_TABLE()

FilePathPanel::FilePathPanel(wxWindow* parent,wxWindowID id) :
    m_Selection(-1)
{
	//(*Initialize(FilePathPanel)
	wxStaticText* StaticText2;
	wxStaticText* StaticText1;
	wxBoxSizer* BoxSizer1;
	wxStaticText* StaticText4;

	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Please enter the file\'s location and name and\nwhether to add it to the active project."), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer1->Add(StaticText1, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 8);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Filename with full path:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer1->Add(StaticText2, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 8);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	txtFilename = new wxTextCtrl(this, ID_TEXTCTRL1, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	BoxSizer2->Add(txtFilename, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
	btnBrowse = new wxButton(this, ID_BUTTON1, _("..."), wxDefaultPosition, wxSize(22,22), 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(btnBrowse, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
	BoxSizer1->Add(BoxSizer2, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 8);
	lblGuard = new wxStaticText(this, ID_STATICTEXT3, _("Header guard word:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	BoxSizer1->Add(lblGuard, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 8);
	txtGuard = new wxTextCtrl(this, ID_TEXTCTRL2, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	BoxSizer1->Add(txtGuard, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 8);
	BoxSizer1->Add(-1,-1,0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	chkAddToProject = new wxCheckBox(this, ID_CHECKBOX1, _("Add file to active project"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	chkAddToProject->SetValue(false);
	BoxSizer1->Add(chkAddToProject, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 8);
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer1->AddGrowableCol(1);
	FlexGridSizer1->AddGrowableRow(1);
	FlexGridSizer1->Add(16,16,0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("In build target(s):"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(StaticText4, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	FlexGridSizer1->Add(16,16,0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
	clbTargets = new wxCheckListBox(this, ID_CHECKLISTBOX2, wxDefaultPosition, wxSize(222,111), 0, 0, wxLB_SINGLE|wxLB_HSCROLL, wxDefaultValidator, _T("ID_CHECKLISTBOX2"));
	clbTargets->Disable();
	FlexGridSizer1->Add(clbTargets, 1, wxALL|wxEXPAND|wxFIXED_MINSIZE|wxALIGN_LEFT|wxALIGN_TOP, 0);
	FlexGridSizer1->Add(16,16,0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
	BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
	btnAll = new wxButton(this, ID_BUTTON2, _("All"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	btnAll->Disable();
	BoxSizer6->Add(btnAll, 0, wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 4);
	btnNone = new wxButton(this, ID_BUTTON3, _("None"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	btnNone->Disable();
	BoxSizer6->Add(btnNone, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer1->Add(BoxSizer6, 0, wxALL|wxALIGN_RIGHT|wxALIGN_TOP, 0);
	BoxSizer1->Add(FlexGridSizer1, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 8);
	BoxSizer3 = new wxBoxSizer(wxVERTICAL);
	BoxSizer1->Add(BoxSizer3, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 8);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&FilePathPanel::OntxtFilenameText);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&FilePathPanel::OnbtnBrowseClick);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&FilePathPanel::OnchkAddToProjectChange);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&FilePathPanel::OnbtnAllClick);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&FilePathPanel::OnbtnNoneClick);
	//*)

    txtFilename->SetValue(wxEmptyString);
    txtGuard->SetValue(wxEmptyString);

    // fill targets list
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (prj)
    {
        clbTargets->Clear();
        for (int i = 0; i < prj->GetBuildTargetsCount(); ++i)
        {
            ProjectBuildTarget* bt = prj->GetBuildTarget(i);
            if (bt)
                clbTargets->Append(bt->GetTitle());
        }
        clbTargets->SetSelection(clbTargets->FindString(prj->GetActiveBuildTarget()));
        chkAddToProject->SetValue(true);
        ToggleVisibility(true);
    }
    else
    {
        chkAddToProject->SetValue(false);
        ToggleVisibility(false);
    }
}

FilePathPanel::~FilePathPanel()
{
}

void FilePathPanel::SetAddToProject(bool add)
{
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    add = add && prj;
    chkAddToProject->SetValue(add);

    ToggleVisibility(add);
}

// note : design not so nice because the function is not const !!!
int FilePathPanel::GetTargetIndex()
{
    int selection = -1;

    if (m_Selection > static_cast<int>(clbTargets->GetCount()) - 2) // (m_Selection >= clbTargets->GetCount()-1)
    {
        m_Selection = -1; // Reset -> notify about "last item" with -1
        return m_Selection;
    }

    // start with next (x+1) or first (-1+1 = 0) item
    for (selection = m_Selection + 1; selection < static_cast<int>(clbTargets->GetCount()); ++selection)
    {
        if (clbTargets->IsChecked(selection))
            break; // selection found. break for-loop to save item's index
    }

    m_Selection = selection; // return current selected items index (-1 if nothing selected)

    return m_Selection;
}

void FilePathPanel::OntxtFilenameText(wxCommandEvent& event)
{
    if (!txtFilename || txtFilename->GetValue().IsEmpty())
        return;
    wxString name = wxFileNameFromPath(txtFilename->GetValue());
    while (name.Replace(_T(" "), _T("_")))
        ;
    while (name.Replace(_T("\t"), _T("_")))
        ;
    while (name.Replace(_T("."), _T("_")))
        ;
    name.MakeUpper();
    name << _T("_INCLUDED");
    txtGuard->SetValue(name);
}

void FilePathPanel::OnbtnBrowseClick(wxCommandEvent& event)
{
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    wxFileDialog dlg(this,
                     _("Select filename"),
                     prj ? prj->GetBasePath() : _T(""),
                     txtFilename->GetValue(),
                     m_ExtFilter,
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
        txtFilename->SetValue(dlg.GetPath());
}

void FilePathPanel::OnchkAddToProjectChange(wxCommandEvent& event)
{
    SetAddToProject(event.IsChecked());
}

void FilePathPanel::OnbtnAllClick(wxCommandEvent& event)
{
    for (int i = 0; i < static_cast<int>(clbTargets->GetCount()); ++i)
    {
        clbTargets->Check(i, true);
    }
} // end of OnbtnAllClick

void FilePathPanel::OnbtnNoneClick(wxCommandEvent& event)
{
    for (int i = 0; i < static_cast<int>(clbTargets->GetCount()); ++i)
    {
        clbTargets->Check(i, false);
    }
} // end of OnbtnNoneClick

void FilePathPanel::ToggleVisibility(bool on)
{
    clbTargets->Enable(on);
    btnAll->Enable(on);
    btnNone->Enable(on);
}
