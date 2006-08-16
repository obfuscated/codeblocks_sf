#include <sdk.h>
#ifndef CB_PRECOMP
    #include <wx/filedlg.h>
    #include <cbproject.h>
    #include <projectbuildtarget.h>
    #include <projectmanager.h>
    #include <manager.h>
#endif
#include <wx/filefn.h>
#include "filepathpanel.h"

BEGIN_EVENT_TABLE(FilePathPanel,wxPanel)
    //(*EventTable(FilePathPanel)
    EVT_TEXT(ID_TEXTCTRL1,FilePathPanel::OntxtFilenameText)
    EVT_BUTTON(ID_BUTTON1,FilePathPanel::OnbtnBrowseClick)
    EVT_CHECKBOX(ID_CHECKBOX1,FilePathPanel::OnchkAddToProjectChange)
    EVT_BUTTON(ID_BUTTON2,FilePathPanel::OnbtnAllClick)
    EVT_BUTTON(ID_BUTTON3,FilePathPanel::OnbtnNoneClick)
    //*)
END_EVENT_TABLE()

FilePathPanel::FilePathPanel(wxWindow* parent,wxWindowID id) :
    txtFilename(0),
    btnBrowse(0),
    lblGuard(0),
    txtGuard(0),
    chkAddToProject(0),
    clbTargets(0),
		btnAll(0),
		btnNone(0),
		m_ExtFilter(_T("")),
		m_Selection(-1)
{
	//(*Initialize(FilePathPanel)
	wxBoxSizer* BoxSizer1;
	wxStaticText* StaticText1;
	wxStaticText* StaticText2;
	wxStaticText* StaticText4;

	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,_T(""));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Please enter the file\'s location and name and\nwhether to add it to the active project."),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT1"));
	StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("Filename with full path:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT2"));
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	txtFilename = new wxTextCtrl(this,ID_TEXTCTRL1,_("Text"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_TEXTCTRL1"));
	if ( 0 ) txtFilename->SetMaxLength(0);
	btnBrowse = new wxButton(this,ID_BUTTON1,_("..."),wxDefaultPosition,wxSize(22,22),0,wxDefaultValidator,_("ID_BUTTON1"));
	if (false) btnBrowse->SetDefault();
	BoxSizer2->Add(txtFilename,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	BoxSizer2->Add(btnBrowse,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	lblGuard = new wxStaticText(this,ID_STATICTEXT3,_("Header guard word:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT3"));
	txtGuard = new wxTextCtrl(this,ID_TEXTCTRL2,_("Text"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_TEXTCTRL2"));
	if ( 0 ) txtGuard->SetMaxLength(0);
	chkAddToProject = new wxCheckBox(this,ID_CHECKBOX1,_("Add file to active project"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_CHECKBOX1"));
	chkAddToProject->SetValue(false);
	BoxSizer3 = new wxBoxSizer(wxVERTICAL);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	StaticText4 = new wxStaticText(this,ID_STATICTEXT4,_("In build target(s):"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT4"));
	BoxSizer4->Add(16,16,0);
	BoxSizer4->Add(StaticText4,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	clbTargets = new wxCheckListBox(this,ID_CHECKLISTBOX2,wxDefaultPosition,wxDefaultSize,0,NULL,0,wxDefaultValidator,_("ID_CHECKLISTBOX2"));
	clbTargets->Disable();
	BoxSizer5->Add(16,16,0);
	BoxSizer5->Add(clbTargets,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
	btnAll = new wxButton(this,ID_BUTTON2,_("All"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON2"));
	if (false) btnAll->SetDefault();
	btnAll->Disable();
	btnNone = new wxButton(this,ID_BUTTON3,_("None"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON3"));
	if (false) btnNone->SetDefault();
	btnNone->Disable();
	BoxSizer6->Add(16,16,0);
	BoxSizer6->Add(btnAll,0,wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL,0);
	BoxSizer6->Add(btnNone,0,wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL,0);
	BoxSizer3->Add(BoxSizer4,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	BoxSizer3->Add(BoxSizer5,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	BoxSizer3->Add(BoxSizer6,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	BoxSizer1->Add(StaticText1,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,8);
	BoxSizer1->Add(StaticText2,0,wxLEFT|wxRIGHT|wxTOP|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(BoxSizer2,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(lblGuard,0,wxLEFT|wxRIGHT|wxTOP|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(txtGuard,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(-1,-1,1);
	BoxSizer1->Add(chkAddToProject,0,wxLEFT|wxRIGHT|wxTOP|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(BoxSizer3,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
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

int FilePathPanel::GetTargetIndex()
{
    int selection = -1;

    if (m_Selection > clbTargets->GetCount()-2) // (m_Selection >= clbTargets->GetCount()-1)
    {
        m_Selection = -1; // Reset -> notify about "last item" with -1
        return m_Selection;
    }

    // start with next (x+1) or first (-1+1 = 0) item
    for (selection = m_Selection+1; selection<clbTargets->GetCount(); ++selection)
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
    wxFileDialog* dlg = new wxFileDialog(this,
                            _("Select filename"),
                            prj ? prj->GetBasePath() : _T(""),
                            txtFilename->GetValue(),
                            m_ExtFilter,
                            wxSAVE | wxOVERWRITE_PROMPT);
    PlaceWindow(dlg);
    if (dlg->ShowModal() == wxID_OK)
        txtFilename->SetValue(dlg->GetPath());
    dlg->Destroy();
}

void FilePathPanel::OnchkAddToProjectChange(wxCommandEvent& event)
{
    SetAddToProject(event.IsChecked());
}

void FilePathPanel::OnbtnAllClick(wxCommandEvent& event)
{
    for (int i=0; i<clbTargets->GetCount(); ++i)
        clbTargets->Check(i, true);
}

void FilePathPanel::OnbtnNoneClick(wxCommandEvent& event)
{
    for (int i=0; i<clbTargets->GetCount(); ++i)
        clbTargets->Check(i, false);
}

void FilePathPanel::ToggleVisibility(bool on)
{
    clbTargets->Enable(on);
    btnAll->Enable(on);
    btnNone->Enable(on);
}
