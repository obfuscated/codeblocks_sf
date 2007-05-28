#include "projectpathpanel.h"
#include <filefilters.h>
#include <wx/filename.h>

BEGIN_EVENT_TABLE(ProjectPathPanel,wxPanel)
	//(*EventTable(ProjectPathPanel)
	EVT_TEXT(ID_TEXTCTRL3,ProjectPathPanel::OntxtPrjTitleText)
	EVT_TEXT(ID_TEXTCTRL1,ProjectPathPanel::OnFullPathChanged)
	EVT_TEXT(ID_TEXTCTRL2,ProjectPathPanel::OnFullPathChanged)
	EVT_TEXT(ID_TEXTCTRL4,ProjectPathPanel::OntxtFinalDirText)
	//*)
END_EVENT_TABLE()

ProjectPathPanel::ProjectPathPanel(wxWindow* parent,wxWindowID id)
    : m_LockUpdates(false),
    txtPrjPath(0),
    txtPrjName(0),
    txtFinalDir(0)
{
	//(*Initialize(ProjectPathPanel)
	wxStaticText* StaticText1;
	wxStaticText* StaticText4;
	wxStaticText* StaticText2;
	wxStaticText* StaticText3;
	wxStaticText* StaticText5;

	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Please select the folder where you want the new project\nto be created as well as its title."),wxDefaultPosition,wxDefaultSize,wxST_NO_AUTORESIZE);
	StaticText4 = new wxStaticText(this,ID_STATICTEXT4,_("Project title:"),wxDefaultPosition,wxDefaultSize,0);
	txtPrjTitle = new wxTextCtrl(this,ID_TEXTCTRL3,_T(""),wxDefaultPosition,wxDefaultSize,0);
	if ( 0 ) txtPrjTitle->SetMaxLength(0);
	StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("Folder to create project in:"),wxDefaultPosition,wxDefaultSize,0);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	txtPrjPath = new wxTextCtrl(this,ID_TEXTCTRL1,_("Text"),wxDefaultPosition,wxDefaultSize,0);
	if ( 0 ) txtPrjPath->SetMaxLength(0);
	btnPrjPathBrowse = new wxButton(this,ID_BUTTON1,_("..."),wxDefaultPosition,wxSize(22,22),0);
	if (false) btnPrjPathBrowse->SetDefault();
	BoxSizer2->Add(txtPrjPath,1,wxALIGN_CENTER,5);
	BoxSizer2->Add(btnPrjPathBrowse,0,wxALIGN_CENTER,5);
	StaticText3 = new wxStaticText(this,ID_STATICTEXT3,_("Project filename:"),wxDefaultPosition,wxDefaultSize,0);
	txtPrjName = new wxTextCtrl(this,ID_TEXTCTRL2,_T(""),wxDefaultPosition,wxDefaultSize,0);
	if ( 0 ) txtPrjName->SetMaxLength(0);
	StaticText5 = new wxStaticText(this,ID_STATICTEXT5,_("Resulting filename:"),wxDefaultPosition,wxDefaultSize,0);
	txtFinalDir = new wxTextCtrl(this,ID_TEXTCTRL4,_T(""),wxDefaultPosition,wxDefaultSize,0);
	if ( 0 ) txtFinalDir->SetMaxLength(0);
	BoxSizer1->Add(StaticText1,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(StaticText4,0,wxLEFT|wxRIGHT|wxTOP|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(txtPrjTitle,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(StaticText2,0,wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(BoxSizer2,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(StaticText3,0,wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(txtPrjName,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(StaticText5,0,wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_TOP,8);
	BoxSizer1->Add(txtFinalDir,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

ProjectPathPanel::~ProjectPathPanel()
{
}

void ProjectPathPanel::UpdateFromResulting()
{
    if (m_LockUpdates || !txtPrjPath || !txtPrjName || !txtFinalDir || txtFinalDir->GetValue().IsEmpty())
        return; // not ready yet
    m_LockUpdates = true;
    wxFileName fn = txtFinalDir->GetValue();
    txtPrjPath->SetValue(fn.GetPath(wxPATH_GET_VOLUME));
    txtPrjName->SetValue(fn.GetName());
    m_LockUpdates = false;
}

void ProjectPathPanel::Update()
{
    if (m_LockUpdates || !txtPrjPath || !txtPrjName || !txtFinalDir)
        return; // not ready yet

    wxString final = txtPrjPath->GetValue();
    if (!final.IsEmpty())
    {
        wxFileName fname(txtPrjName->GetValue());
        fname.MakeAbsolute(final);
        final = fname.GetFullPath();
    }
    if (final.IsEmpty() || txtPrjName->GetValue().IsEmpty())
        final = _("<invalid path>");
    m_LockUpdates = true;
    txtFinalDir->SetValue(final);
    m_LockUpdates = false;
}

void ProjectPathPanel::OnFullPathChanged(wxCommandEvent& event)
{
    Update();
}

void ProjectPathPanel::OntxtFinalDirText(wxCommandEvent& event)
{
    if (!txtPrjName->GetValue().StartsWith(_("<invalid path>")))
        UpdateFromResulting();
}

void ProjectPathPanel::OntxtPrjTitleText(wxCommandEvent& event)
{
    wxString prjtitle = txtPrjTitle->GetValue();
    // Make a check if the project title has any extension or not
    if (!prjtitle.IsEmpty() &&
        !prjtitle.Right(4).IsSameAs(FileFilters::CODEBLOCKS_DOT_EXT))
        prjtitle = prjtitle + FileFilters::CODEBLOCKS_DOT_EXT;
    txtPrjName->SetValue(prjtitle);
// FIXME (Biplab#1#): In Linux, text update event is not thrown
    Update();
}
