#include "wxsheaders.h"
#include "wxsprojectconfigurationdlg.h"

#include "wxsproject.h"
#include <wx/tokenzr.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>

BEGIN_EVENT_TABLE(wxsProjectConfigurationDlg,wxDialog)
    //(*EventTable(wxsProjectConfigurationDlg)
    EVT_INIT_DIALOG(wxsProjectConfigurationDlg::OnInit)
    EVT_BUTTON(XRCID("ID_BUTTON1"),wxsProjectConfigurationDlg::OnButton1Click)
    EVT_BUTTON(XRCID("ID_BUTTON2"),wxsProjectConfigurationDlg::OnButton2Click)
    EVT_CHECKBOX(XRCID("ID_CHECKBOX2"),wxsProjectConfigurationDlg::OnInitAllChange)
    //*)
    EVT_BUTTON(wxID_OK,wxsProjectConfigurationDlg::OnButton3Click)
    EVT_BUTTON(wxID_CANCEL,wxsProjectConfigurationDlg::OnButton4Click)
END_EVENT_TABLE()

wxsProjectConfigurationDlg::wxsProjectConfigurationDlg(wxWindow* parent,wxsProject* _Project,wxWindowID id):
    Project(_Project)
{
	//(*Initialize(wxsProjectConfigurationDlg)
	wxXmlResource::Get()->LoadDialog(this,parent,_T("wxsProjectConfigurationDlg"));
	StaticText1 = XRCCTRL(*this,"ID_STATICTEXT1",wxStaticText);
	AutoLoad = XRCCTRL(*this,"ID_TEXTCTRL1",wxTextCtrl);
	Button1 = XRCCTRL(*this,"ID_BUTTON1",wxButton);
	Button2 = XRCCTRL(*this,"ID_BUTTON2",wxButton);
	MainRes = XRCCTRL(*this,"ID_COMBOBOX1",wxComboBox);
	StaticText2 = XRCCTRL(*this,"ID_STATICTEXT2",wxStaticText);
	InitAll = XRCCTRL(*this,"ID_CHECKBOX2",wxCheckBox);
	InitAllNecessary = XRCCTRL(*this,"ID_CHECKBOX1",wxCheckBox);
	//*)
}

wxsProjectConfigurationDlg::~wxsProjectConfigurationDlg()
{
}

void wxsProjectConfigurationDlg::OnInit(wxInitDialogEvent& event)
{
    if ( !Project ) return;

    while ( !Project->IsAppManaged() )
    {
        int Ret = ::wxMessageBox(
            _("WARNING:\n"
              "\n"
              "In this project, wxSmith does not manage source code for\n"
              "wxApp-derived class, there are no project options.\n"
              "\n"
              "If You want wxSmith to manage Your application class,\n"
              "it must be declared in following form:\n"
              "\n"
              "\t//(*AppHeaders\n"
              "\t  /*... wxSmith will put necesarry header includes here ...*/\n"
              "\t//*)\n"
              "\n"
              "\tclass MyApp : public wxApp\n"
              "\t{\n"
              "\t\tpublic:\n"
              "\t\t\tvirtual bool OnInit();\n"
              "\t};\n"
              "\tIMPLEMENT_APP(MyApp);\n"
              "\n"
              "\tbool MyApp::OnInit()\n"
              "\t{\n"
              "\t\t//(*AppInitialize\n"
              "\t\t  /*... wxSmith will put application's initialization code here ...*/\n"
              "\t\t//*)\n"
              "\t\t  /*... wxsOK will be true if initialized correctly ... */\n"
              "\t\treturn wxsOK;\n"
              "\t}\n"
              "\n"
              "If You have already changed Your application to this form,\n"
              "click Yes and choose source file with application class.\n"
              "\n"
              "If You don't want wxSmith to manage Your application\n"
              "(f.ex. You have Your own initialization system), click No.\n"),
            _("Application not manager in wxSmith"),
            wxYES_NO | wxICON_INFORMATION );

        if ( Ret != wxYES )
        {
            EndModal(1);
            return;
        }

        wxString NewFileName;
        for(;;)
        {
            NewFileName = ::wxFileSelector(
                _("Choose source file with Your application"),
                _T(""), _T("main.cpp"), _T("cpp"),
                _("C++ source files (*.cpp)|*.cpp|"
                  "All files (*)|*"),
                wxOPEN|wxFILE_MUST_EXIST
#if (WXWIN_COMPATIBILITY_2_4)
                | wxHIDE_READONLY
#endif
                );
            if ( NewFileName.empty() )
            {
                EndModal(1);
                return;
            }
            wxFileName FN(NewFileName);
            FN.MakeRelativeTo(Project->GetProjectPath());
            NewFileName = FN.GetFullPath();
            if ( Project->CheckProjFileExists(NewFileName) ) break;
            Ret = ::wxMessageBox(
                _("Selected file is not included in this project.\n"
                  "Continue ?"),
                _("Selected external file"),
                wxYES_NO | wxICON_QUESTION );
            if ( Ret == wxYES ) break;
        }

        Project->SetAppSourceFile(NewFileName);
    }

    const wxArrayString& AutoLoadArr = Project->GetAutoLoadedResources();
    for ( size_t i=0; i<AutoLoadArr.Count(); ++i )
    {
        AutoLoad->AppendText(AutoLoadArr[i]);
        AutoLoad->AppendText(_T("\n"));
    }

    wxArrayString AllResources;
    AllResources.Add(_("-- None --"));
    Project->EnumerateResources(AllResources,true);
    MainRes->Append(AllResources);
    if ( !Project->GetMainResource().Length() )
    {
        MainRes->SetValue(_("-- None --"));
    }
    else
    {
        MainRes->SetValue(Project->GetMainResource());
    }

    InitAll->SetValue(Project->GetCallInitAll());
    InitAllNecessary->Enable(Project->GetCallInitAll());
    InitAllNecessary->SetValue(Project->GetCallInitAllNecessary());
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
        wxStringTokenizer Tokens(AutoLoad->GetValue(),_T("\n"));
        wxArrayString Array;
        while ( Tokens.HasMoreTokens() )
        {
            Array.Add(Tokens.GetNextToken());
        }
        Project->SetAutoLoadedResources(Array);
        Project->SetMainResource(MainRes->GetValue());
        Project->SetCallInitAll(InitAll->GetValue(),InitAllNecessary->GetValue());
        Project->SetModified(true);
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
        wxOPEN|wxFILE_MUST_EXIST
#if (WXWIN_COMPATIBILITY_2_4)
        | wxHIDE_READONLY
#endif
        );

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

