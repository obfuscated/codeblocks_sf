#include "wxsnewwindowdlg.h"
#include "resources/wxswindowres.h"
#include "wxsmith.h"

#include <wx/xrc/xmlres.h>
#include <projectmanager.h>
#include <messagemanager.h>
#include "wxsglobals.h"
#include "wxsproject.h"
#include "resources/wxsdialogres.h"
#include "resources/wxsframeres.h"
#include "resources/wxspanelres.h"

BEGIN_EVENT_TABLE(wxsNewWindowDlg,wxDialog)
    //(*EventTable(wxsNewWindowDlg)
    EVT_TEXT(ID_TEXTCTRL1,wxsNewWindowDlg::OnClassChanged)
    EVT_TEXT(ID_TEXTCTRL2,wxsNewWindowDlg::OnHeaderChanged)
    EVT_TEXT(ID_TEXTCTRL3,wxsNewWindowDlg::OnSourceChanged)
    EVT_CHECKBOX(ID_CHECKBOX1,wxsNewWindowDlg::OnUseXrcChange)
    EVT_TEXT(ID_TEXTCTRL4,wxsNewWindowDlg::OnXrcChanged)
    EVT_BUTTON(ID_BUTTON1,wxsNewWindowDlg::OnCancel)
    EVT_BUTTON(ID_BUTTON2,wxsNewWindowDlg::OnCreate)
    //*)
END_EVENT_TABLE()

wxsNewWindowDlg::wxsNewWindowDlg(wxWindow* parent,const wxString& ResType):
    SourceNotTouched(true),
    HeaderNotTouched(true),
    XrcNotTouched(true),
    BlockText(false),
    Type(ResType)
{
    wxWindowID id = wxID_ANY;

    //(*Initialize(wxsNewWindowDlg)
    wxStaticText* StaticText1;
    wxStaticText* StaticText2;
    wxStaticText* StaticText3;
    wxStaticText* StaticText4;
    wxButton* Button1;
    wxButton* Button2;

    Create(parent,id,_T(""),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE);
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL,this,_("Options"));
    FlexGridSizer1 = new wxFlexGridSizer(0,2,5,5);
    FlexGridSizer1->AddGrowableCol(1);
    StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Class Name:"),wxDefaultPosition,wxDefaultSize,0);
    Class = new wxTextCtrl(this,ID_TEXTCTRL1,_T(""),wxDefaultPosition,wxSize(80,-1),0);
    if ( 0 ) Class->SetMaxLength(0);
    StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("Header file:"),wxDefaultPosition,wxDefaultSize,0);
    Header = new wxTextCtrl(this,ID_TEXTCTRL2,_T(""),wxDefaultPosition,wxSize(80,-1),0);
    if ( 0 ) Header->SetMaxLength(0);
    StaticText3 = new wxStaticText(this,ID_STATICTEXT3,_("Source file:"),wxDefaultPosition,wxDefaultSize,0);
    Source = new wxTextCtrl(this,ID_TEXTCTRL3,_T(""),wxDefaultPosition,wxSize(80,-1),0);
    if ( 0 ) Source->SetMaxLength(0);
    UseXrc = new wxCheckBox(this,ID_CHECKBOX1,_("Xrc File:"),wxDefaultPosition,wxDefaultSize,0);
    UseXrc->SetValue(false);
    Xrc = new wxTextCtrl(this,ID_TEXTCTRL4,_T(""),wxDefaultPosition,wxSize(80,-1),0);
    if ( 0 ) Xrc->SetMaxLength(0);
    FlexGridSizer1->Add(StaticText1,0,wxALIGN_CENTER,5);
    FlexGridSizer1->Add(Class,0,wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
    FlexGridSizer1->Add(StaticText2,0,wxALIGN_CENTER,5);
    FlexGridSizer1->Add(Header,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND,5);
    FlexGridSizer1->Add(StaticText3,0,wxALIGN_CENTER,5);
    FlexGridSizer1->Add(Source,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND,5);
    FlexGridSizer1->Add(UseXrc,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND,5);
    FlexGridSizer1->Add(Xrc,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND,5);
    StaticText4 = new wxStaticText(this,ID_STATICTEXT4,_("Warning:\nWhen adding resource in Xrc mode,\nthis resource must be manually loaded.\nFor details see wxXmlResource::Load()\nand wxXmlResource::Get()."),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE);
    StaticBoxSizer1->Add(FlexGridSizer1,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND,5);
    StaticBoxSizer1->Add(StaticText4,1,wxALL|wxALIGN_CENTER,5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Button1 = new wxButton(this,ID_BUTTON1,_("Cancel"),wxDefaultPosition,wxDefaultSize,0);
    if (false) Button1->SetDefault();
    Button2 = new wxButton(this,ID_BUTTON2,_("Create"),wxDefaultPosition,wxDefaultSize,0);
    if (true) Button2->SetDefault();
    BoxSizer2->Add(Button1,0,wxALL|wxALIGN_CENTER,5);
    BoxSizer2->Add(Button2,0,wxALL|wxALIGN_CENTER,5);
    BoxSizer1->Add(StaticBoxSizer1,0,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND,5);
    BoxSizer1->Add(BoxSizer2,0,wxALL|wxALIGN_CENTER,5);
    this->SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);
    //*)

    if ( !Class || !Source || !Header )
    {
        Close();
    }
    else
    {
    	BlockText = true;
    	wxString ResName = wxString::Format(_("New%s"),ResType.c_str());
        Class->SetValue(ResName);
        Source->SetValue(ResName.Lower()+_T(".cpp"));
        Header->SetValue(ResName.Lower()+_T(".h"));
        Xrc->SetValue(ResName.Lower()+_T(".xrc"));
        Xrc->Disable();
        SetTitle(wxString::Format(_("New %s resource"),ResType.c_str()));
        BlockText = false;
    }
}

wxsNewWindowDlg::~wxsNewWindowDlg()
{
}

void wxsNewWindowDlg::OnCancel(wxCommandEvent& event)
{
    Close();
}

void wxsNewWindowDlg::OnCreate(wxCommandEvent& event)
{
	bool CreateXrc = UseXrc->GetValue();

    // Need to do some checks
    // First - validating name
    if ( !wxsValidateIdentifier(Class->GetValue()) )
    {
        wxMessageBox(_("Invalid class name"));
        return;
    }

    // Second - checking if there's given resoure in current project

    wxsProject* Proj = wxsPLUGIN()->GetSmithProject(Manager::Get()->GetProjectManager()->GetActiveProject());
    if ( !Proj ) { return; }

    if ( Proj->FindResource(Class->GetValue()) )
    {
        wxMessageBox(wxString::Format(_("Resource '%s' already exists"),Class->GetValue().c_str()));
        return;
    }

    // Third - checking if files already exist

    if ( wxFileName::FileExists(Proj->GetProjectFileName(Header->GetValue())) )
    {
        if ( wxMessageBox(
              wxString::Format(_("File '%s' already exists. It will be overwritten.\nContinue ?"),Header->GetValue().c_str()),
              _("File exists"),wxYES_NO|wxICON_ERROR) != wxYES )
        {
            return;
        }
    }

    if ( wxFileName::FileExists(Proj->GetProjectFileName(Source->GetValue())) )
    {
        if ( wxMessageBox(
              wxString::Format(_("File '%s' already exists. It will be overwritten.\nContinue ?"),Source->GetValue().c_str()),
              _("File exists"),wxYES_NO|wxICON_ERROR) != wxYES )
        {
            return;
        }
    }

    if ( CreateXrc && wxFileName::FileExists(Proj->GetProjectFileName(Xrc->GetValue())) )
    {
        if ( wxMessageBox(
              wxString::Format(_("File '%s' already exists. It will be overwritten.\nContinue ?"),Source->GetValue().c_str()),
              _("File exists"),wxYES_NO|wxICON_ERROR) != wxYES )
        {
            return;
        }
    }

    // Creating new resource

    wxString WxsFile = Class->GetValue() + _T(".wxs");
    wxsWindowRes* NewWindow = NULL;

    // Configuration wil be passed through xml node
    TiXmlElement Config("tmp");
    Config.SetAttribute("class",       cbU2C(Class->GetValue()));
    Config.SetAttribute("wxs_file",    cbU2C(WxsFile));
    Config.SetAttribute("src_file",    cbU2C(Source->GetValue()));
    Config.SetAttribute("header_file", cbU2C(Header->GetValue()));
    Config.SetAttribute("xrc_file",    cbU2C(Xrc->GetValue()));
    Config.SetAttribute("edit_mode",   UseXrc->GetValue()?"Xrc":"Source");

    if ( Type == _T("Dialog") )
    {
        NewWindow = new wxsDialogRes(Proj);
    }
    else if ( Type == _T("Frame") )
    {
        NewWindow = new wxsFrameRes(Proj);
    }
    else if ( Type == _T("Panel") )
    {
        NewWindow = new wxsPanelRes(Proj);
    }
    else
    {
        DBGLOG(_T("wxSmith: Internal error: unknown type when creating resource"));
        Close();
        return;
    }

    if ( !NewWindow->CreateNewResource(&Config) )
    {
        delete NewWindow;
        DBGLOG(_T("wxSmith: Couldn't generate new resource"));
        Close();
        return;
    }

    // Updating content of resource
    if ( !PrepareResource(NewWindow) )
    {
        delete NewWindow;
        Close();
        return;
    }
    NewWindow->SetModified(true);
    NewWindow->SaveResource();

    if ( !Proj->AddResource(NewWindow) )
    {
        DBGLOG(_T("Couldn't add new resource to project"));
        delete NewWindow;
        Close();
        return;
    }

    // Adding new files to project
    wxArrayInt targets;
    Manager::Get()->GetProjectManager()->AddFileToProject(Header->GetValue(), Proj->GetCBProject(), targets);
    if (targets.GetCount() != 0)
    {
        Manager::Get()->GetProjectManager()->AddFileToProject(Source->GetValue(), Proj->GetCBProject(), targets);
    }
    Manager::Get()->GetProjectManager()->RebuildTree();

    // Opening editor for this resource
    NewWindow->EditOpen();

    Close();
}

void wxsNewWindowDlg::OnClassChanged(wxCommandEvent& event)
{
    if ( BlockText ) return;
    BlockText = true;
    if ( HeaderNotTouched ) Header->SetValue((Class->GetValue() + _T(".h")).MakeLower());
    if ( SourceNotTouched ) Source->SetValue((Class->GetValue() + _T(".cpp")).MakeLower());
    if ( XrcNotTouched ) Xrc->SetValue((Class->GetValue() + _T(".xrc")).MakeLower());
    BlockText = false;
}

void wxsNewWindowDlg::OnSourceChanged(wxCommandEvent& event)
{
    if ( BlockText ) return;
    BlockText = true;
    SourceNotTouched = false;
    BlockText = false;
}

void wxsNewWindowDlg::OnHeaderChanged(wxCommandEvent& event)
{
    if ( BlockText ) return;
    BlockText = true;
    wxFileName FN(Header->GetValue());
    FN.SetExt(_T("cpp"));
    if ( SourceNotTouched )
    {
        Source->SetValue(FN.GetFullPath());
    }
    FN.SetExt(_T("xrc"));
    if ( XrcNotTouched )
    {
        Xrc->SetValue(FN.GetFullPath());
    }
    HeaderNotTouched = false;
    BlockText = false;
}

void wxsNewWindowDlg::OnUseXrcChange(wxCommandEvent& event)
{
    Xrc->Enable(UseXrc->GetValue());
}

void wxsNewWindowDlg::OnXrcChanged(wxCommandEvent& event)
{
    if ( BlockText ) return;
    BlockText = true;
    XrcNotTouched = false;
    BlockText = false;
}
