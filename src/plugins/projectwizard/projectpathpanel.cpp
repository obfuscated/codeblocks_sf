#include "projectpathpanel.h"

BEGIN_EVENT_TABLE(ProjectPathPanel,wxPanel)
	//(*EventTable(ProjectPathPanel)
	EVT_TEXT(ID_TEXTCTRL1,ProjectPathPanel::OnFullPathChanged)
	EVT_TEXT(ID_TEXTCTRL2,ProjectPathPanel::OnFullPathChanged)
	//*)
END_EVENT_TABLE()

ProjectPathPanel::ProjectPathPanel(wxWindow* parent,wxWindowID id)
    : txtPrjPath(0),
    txtPrjName(0),
    lblFinalDir(0)
{
	//(*Initialize(ProjectPathPanel)
	wxStaticText* StaticText1;
	wxStaticText* StaticText2;
	wxStaticText* StaticText3;
	wxStaticText* StaticText5;
	
	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Please select the folder where you want the new project\nto be created."),wxDefaultPosition,wxDefaultSize,0);
	StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("Folder to create project under:"),wxDefaultPosition,wxDefaultSize,0);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	txtPrjPath = new wxTextCtrl(this,ID_TEXTCTRL1,_("Text"),wxDefaultPosition,wxDefaultSize,0);
	if ( 0 ) txtPrjPath->SetMaxLength(0);
	btnPrjPathBrowse = new wxButton(this,ID_BUTTON1,_("..."),wxDefaultPosition,wxSize(22,22),0);
	if (false) btnPrjPathBrowse->SetDefault();
	BoxSizer2->Add(txtPrjPath,1,wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer2->Add(btnPrjPathBrowse,0,wxALIGN_CENTER,5);
	StaticText3 = new wxStaticText(this,ID_STATICTEXT3,_("Project name:"),wxDefaultPosition,wxDefaultSize,0);
	txtPrjName = new wxTextCtrl(this,ID_TEXTCTRL2,_T(""),wxDefaultPosition,wxDefaultSize,0);
	if ( 0 ) txtPrjName->SetMaxLength(0);
	StaticText5 = new wxStaticText(this,ID_STATICTEXT5,_("Destination project path:"),wxDefaultPosition,wxDefaultSize,0);
	lblFinalDir = new wxStaticText(this,ID_STATICTEXT4,_("Label"),wxDefaultPosition,wxDefaultSize,0);
	BoxSizer1->Add(StaticText1,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(StaticText2,0,wxLEFT|wxRIGHT|wxTOP|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(BoxSizer2,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(StaticText3,0,wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(txtPrjName,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(StaticText5,0,wxLEFT|wxRIGHT|wxTOP|wxALIGN_LEFT|wxALIGN_TOP,8);
	BoxSizer1->Add(lblFinalDir,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

ProjectPathPanel::~ProjectPathPanel()
{
}

void ProjectPathPanel::Update()
{
    if (!txtPrjPath || !txtPrjName || !lblFinalDir)
        return; // not ready yet
        
    wxString final = txtPrjPath->GetValue();
    if (!final.IsEmpty())
    {
        if (final.Last() != _T('/') && final.Last() != _T('\\'))
            final << wxFILE_SEP_PATH;
        final << txtPrjName->GetValue();
    }
    if (final.IsEmpty())
        final = _("<invalid path>");
    lblFinalDir->SetLabel(final);
}

void ProjectPathPanel::OnFullPathChanged(wxCommandEvent& event)
{
    Update();
}
