#include "wxsnewdialogdlg.h"
#include "resources/wxsdialogres.h"
#include "wxsmith.h"

#include <wx/xrc/xmlres.h>
#include <projectmanager.h>

wxsNewDialogDlg::wxsNewDialogDlg(wxWindow* parent):
    SourceNotTouched(true),
    HeaderNotTouched(true),
    BlockText(false)
{
    wxXmlResource::Get()->LoadDialog(this, parent, "wxsNewDialogDlg");
    
    Class  = XRCCTRL(*this, "ClassName", wxTextCtrl);
    Source = XRCCTRL(*this, "SourceFile", wxTextCtrl);
    Header = XRCCTRL(*this, "HeaderFile", wxTextCtrl);
    
    if ( !Class || !Source || !Header )
    {
        Close();
    }
    else
    {
        Class->SetValue(wxT("MyDialog"));
        Source->SetValue(wxT("mydialog.cpp"));
        Header->SetValue(wxT("mydialog.h"));
    }
}

wxsNewDialogDlg::~wxsNewDialogDlg()
{
}

void wxsNewDialogDlg::OnCancel(wxCommandEvent& event)
{
    Close();
}

void wxsNewDialogDlg::OnCreate(wxCommandEvent& event)
{
    // Need to do some checks
    
    // First - validating name
    const char* ClassName = Class->GetValue().c_str();

    bool NameValid = true;

    if ( !isalpha(*ClassName) && *ClassName!='_'  )
    {
        NameValid = false;
    }
    else
    {
        while ( *++ClassName )
        {
            if ( !isalnum(*ClassName) && *ClassName!='_' )
            {
                NameValid = false;
                break;
            }
        }
    }
    
    if ( !NameValid )
    { 
        wxMessageBox(wxT("Invalid class name"));
        return;
    }
    
    // Second - checking if there's given resoure in current project
    
    wxsProject* Proj = wxSmith::Get()->GetSmithProject(Manager::Get()->GetProjectManager()->GetActiveProject());
    if ( !Proj ) { return; }
    
    if ( Proj->FindResource(Class->GetValue()) )
    {
        wxMessageBox(
            wxString(wxT("Resource '")) + Class->GetValue() +
            wxString(wxT("' already exists")));
        return;
    }
    
    // Third - checking if file already exist

    if ( wxFileName::FileExists(Proj->GetProjectFileName(Header->GetValue())) )
    {
        if ( wxMessageBox(
              wxString(wxT("File '")) + Header->GetValue() +
              wxString(wxT("' already exist. It will be overwritten.\nContinue ?")),
              wxT("File exists"),wxYES_NO|wxICON_ERROR) != wxYES )
        {
            return;
        }
    }
    
    if ( wxFileName::FileExists(Proj->GetProjectFileName(Source->GetValue())) )
    {
        if ( wxMessageBox(
              wxString(wxT("File '")) + Source->GetValue() +
              wxString(wxT("' already exist. It will be overwritten.\nContinue ?")),
              wxT("File exists"),wxYES_NO|wxICON_ERROR) != wxYES )
        {
            return;
        }
    }

    // Creating dialog

    wxString XrcFile = Class->GetValue() + wxT(".xrc");
    wxsDialogRes* NewDialog = new wxsDialogRes(Proj,Class->GetValue(),XrcFile,Source->GetValue(),Header->GetValue());
    
    if ( !NewDialog->GenerateEmptySources() )
    {
        wxMessageBox(wxT("Couldn't generate sources"),wxT("Error"),wxOK|wxICON_ERROR);
        delete NewDialog;
        return;
    }
    
    cbProject* cbProj = Manager::Get()->GetProjectManager()->GetActiveProject();
    
    // Selecting target
    
    /*
    int TargetIndex = 0;
    if ( cbProj->GetBuildTargetsCount() > 1 )
    {
        wxArrayString Arr;
        for ( int i=0; i<cbProj->GetBuildTargetsCount(); i++ )
            Arr.Add(cbProj->GetBuildTarget(i)->GetTitle());
        
        TargetIndex = ::wxGetSingleChoiceIndex(
            wxT("Please select target"),
            wxT("Target selection"),
            Arr);
    }
    
    if ( TargetIndex != -1 )
    {
        cbProj->AddFile(TargetIndex,Proj->GetProjectFileName(Proj->GetProjectFileName(Header->GetValue())));
        cbProj->AddFile(TargetIndex,Proj->GetProjectFileName(Proj->GetProjectFileName(Source->GetValue())));
    }
    */
    
    wxArrayInt targets;
    Manager::Get()->GetProjectManager()->AddFileToProject(Header->GetValue(), cbProj, targets);
    if (targets.GetCount() != 0)
        Manager::Get()->GetProjectManager()->AddFileToProject(Source->GetValue(), cbProj, targets);
    Manager::Get()->GetProjectManager()->RebuildTree();
    
    
    // Adding dialog to project and opening editor for it
    
    Proj->AddDialog(NewDialog);    
    wxsEvent SelectEvent(wxEVT_SELECT_RES,0,NewDialog,NULL);
    wxPostEvent(wxSmith::Get(),SelectEvent);
}

void wxsNewDialogDlg::OnClassChanged(wxCommandEvent& event)
{
    if ( BlockText ) return;
    BlockText = true;
    if ( HeaderNotTouched ) Header->SetValue((Class->GetValue() + wxT(".h")).MakeLower());
    if ( SourceNotTouched ) Source->SetValue((Class->GetValue() + wxT(".cpp")).MakeLower());
    BlockText = false;
}

void wxsNewDialogDlg::OnSourceChanged(wxCommandEvent& event)
{
    if ( BlockText ) return;
    BlockText = true;
    SourceNotTouched = false;
    BlockText = false;
}

void wxsNewDialogDlg::OnHeaderChanged(wxCommandEvent& event)
{
    if ( BlockText ) return;
    BlockText = true;
    if ( SourceNotTouched )
    {
        wxFileName FN(Header->GetValue());
        FN.SetExt(wxT(".cpp"));
        Source->SetValue(FN.GetFullPath());
    }
    HeaderNotTouched = false;
    BlockText = false;
}
    
BEGIN_EVENT_TABLE(wxsNewDialogDlg,wxDialog)
    EVT_BUTTON(XRCID("Cancel"),wxsNewDialogDlg::OnCancel)
    EVT_BUTTON(XRCID("Create"),wxsNewDialogDlg::OnCreate)
    EVT_TEXT(XRCID("ClassName"),wxsNewDialogDlg::OnClassChanged)
    EVT_TEXT(XRCID("SourceFile"),wxsNewDialogDlg::OnSourceChanged)
    EVT_TEXT(XRCID("HeaderFile"),wxsNewDialogDlg::OnHeaderChanged)
END_EVENT_TABLE()
