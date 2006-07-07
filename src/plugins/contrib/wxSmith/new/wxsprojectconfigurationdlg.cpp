#include "wxsheaders.h"
#include "wxsprojectconfigurationdlg.h"

#include "wxsproject.h"
#include <wx/tokenzr.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>

BEGIN_EVENT_TABLE(wxsProjectConfigurationDlg,wxDialog)
    //(*EventTable(wxsProjectConfigurationDlg)
    EVT_BUTTON(ID_BUTTON1,wxsProjectConfigurationDlg::OnButton1Click)
    EVT_BUTTON(ID_BUTTON2,wxsProjectConfigurationDlg::OnButton2Click)
    EVT_CHECKBOX(ID_CHECKBOX2,wxsProjectConfigurationDlg::OnInitAllChange)
    EVT_BUTTON(ID_BUTTON3,wxsProjectConfigurationDlg::OnButton3Click)
    EVT_BUTTON(ID_BUTTON4,wxsProjectConfigurationDlg::OnButton4Click)
    //*)
END_EVENT_TABLE()

wxsProjectConfigurationDlg::wxsProjectConfigurationDlg(wxWindow* parent,wxsProject* _Project,wxWindowID id):
    Project(_Project)
{
	//(*Initialize(wxsProjectConfigurationDlg)
	Create(parent,id,_("Configuration of wxSmith project"),wxDefaultPosition,wxDefaultSize,wxCAPTION|wxTHICK_FRAME|wxSYSTEM_MENU|wxCLOSE_BOX);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL,this,_("Automatically loaded resources:"));
	StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Each line will be passed as file name to\nwxXmlResource::Get()->Load."),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE);
	AutoLoad = new wxTextCtrl(this,ID_TEXTCTRL1,_T(""),wxDefaultPosition,wxSize(218,102),wxTE_MULTILINE);
	if ( 0 ) AutoLoad->SetMaxLength(0);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	Button1 = new wxButton(this,ID_BUTTON1,_("Add file"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
	if (false) Button1->SetDefault();
	Button2 = new wxButton(this,ID_BUTTON2,_("Clear all"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button2->SetDefault();
	BoxSizer3->Add(Button1,1,wxALIGN_CENTER,5);
	BoxSizer3->Add(Button2,1,wxALIGN_CENTER,5);
	StaticBoxSizer1->Add(StaticText1,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer1->Add(AutoLoad,1,wxLEFT|wxRIGHT|wxTOP|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer1->Add(BoxSizer3,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer4 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL,this,_("Main resource"));
	FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	MainRes = new wxComboBox(this,ID_COMBOBOX1,_T(""),wxDefaultPosition,wxSize(201,21),0,NULL,wxCB_READONLY);
	StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("Window selected here will be created and\nshown while initializing application.\nYou should use one of frames here.\nUsing dialog as main resource is also\nacceptable but may lead to problems\nwith terminating application\nafter closing main dialog."),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE);
	FlexGridSizer1->Add(MainRes,1,wxALL|wxALIGN_CENTER,5);
	FlexGridSizer1->Add(StaticText2,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
	StaticBoxSizer2->Add(FlexGridSizer1,1,wxALIGN_CENTER,5);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("XRC system initialization"));
	BoxSizer5 = new wxBoxSizer(wxVERTICAL);
	InitAll = new wxCheckBox(this,ID_CHECKBOX2,_("Call wxXmlResource::InitAllHandlers()"),wxDefaultPosition,wxDefaultSize,0);
	InitAll->SetValue(false);
	InitAllNecessary = new wxCheckBox(this,ID_CHECKBOX1,_("Only when necessary"),wxDefaultPosition,wxDefaultSize,0);
	InitAllNecessary->SetValue(false);
	InitAllNecessary->Disable();
	BoxSizer5->Add(InitAll,0,wxLEFT|wxRIGHT|wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer5->Add(InitAllNecessary,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	StaticBoxSizer3->Add(BoxSizer5,1,wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer4->Add(StaticBoxSizer2,1,wxLEFT|wxRIGHT|wxTOP|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer4->Add(StaticBoxSizer3,0,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer2->Add(StaticBoxSizer1,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer2->Add(BoxSizer4,0,wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
	Button3 = new wxButton(this,ID_BUTTON3,_("OK"),wxDefaultPosition,wxDefaultSize,0);
	if (true) Button3->SetDefault();
	Button4 = new wxButton(this,ID_BUTTON4,_("Cancel"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button4->SetDefault();
	BoxSizer6->Add(Button3,1,wxALL|wxALIGN_CENTER,5);
	BoxSizer6->Add(Button4,1,wxALL|wxALIGN_CENTER,5);
	BoxSizer1->Add(BoxSizer2,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer1->Add(BoxSizer6,0,wxALL|wxALIGN_CENTER,5);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)

    if ( Project )
    {
        wxsProjectConfig& Config = Project->GetConfig();

        for ( size_t i=0; i<Config.LoadedResources.Count(); ++i )
        {
            AutoLoad->AppendText(Config.LoadedResources[i]);
            AutoLoad->AppendText(_T("\n"));
        }

        MainRes->Append(_("-- None --"));
        wxArrayString Resources;
        Project->EnumerateResources(Resources,true);
        MainRes->Append(Resources);
        if ( Config.MainResource.empty() )
        {
            MainRes->SetValue(_("-- None --"));
        }
        else
        {
            MainRes->SetValue(Config.MainResource);
        }

        InitAll->SetValue(Config.CallInitAll);
        InitAllNecessary->Enable(Config.CallInitAll);
        InitAllNecessary->SetValue(Config.CallInitAllNecessary);
    }
}

wxsProjectConfigurationDlg::~wxsProjectConfigurationDlg()
{
}

void wxsProjectConfigurationDlg::OnInitAllChange(wxCommandEvent& event)
{
    InitAllNecessary->Enable(InitAll->GetValue());
}

void wxsProjectConfigurationDlg::OnButton4Click(wxCommandEvent& event)
{
    EndModal(0);
}

void wxsProjectConfigurationDlg::OnButton3Click(wxCommandEvent& event)
{
    if ( Project )
    {
        wxsProjectConfig& Config = Project->GetConfig();

        wxStringTokenizer Tokens(AutoLoad->GetValue(),_T("\n"));
        Config.LoadedResources.Clear();
        while ( Tokens.HasMoreTokens() )
        {
            Config.LoadedResources.Add(Tokens.GetNextToken());
        }
        Config.MainResource = MainRes->GetValue();
        Config.CallInitAll = InitAll->GetValue();
        Config.CallInitAllNecessary = Config.CallInitAll && InitAllNecessary->GetValue();
    }
    EndModal(0);
}

void wxsProjectConfigurationDlg::OnButton1Click(wxCommandEvent& event)
{
    wxString FileName = ::wxFileSelector(
        _("Select resource file"),
        _T(""),_T(""),_T(""),
        _("XRC files (*.xrc)|*.xrc|"
          "Zipped files (*.zip)|*.zip|"
          "All files (*)|*"),
        wxOPEN|wxFILE_MUST_EXIST|wxHIDE_READONLY);

    if ( !FileName.empty() )
    {
        wxFileName FN(FileName);
        FN.MakeRelativeTo(Project->GetProjectPath());
        wxString Ext = FN.GetExt();
        FileName = FN.GetFullPath();
        if ( Ext == _T("zip") || Ext == _T("ZIP") )
        {
            FileName.Append(_T("#zip:*.xrc"));
        }
        wxString CurrentContent = AutoLoad->GetValue();
        if ( CurrentContent.Length() && ( CurrentContent[CurrentContent.Length()-1] != _T('\n') ) )
        {
            AutoLoad->AppendText(_T("\n"));
        }
        AutoLoad->AppendText(FileName);
        AutoLoad->AppendText(_T("\n"));
    }
}

void wxsProjectConfigurationDlg::OnButton2Click(wxCommandEvent& event)
{
    AutoLoad->Clear();
}
