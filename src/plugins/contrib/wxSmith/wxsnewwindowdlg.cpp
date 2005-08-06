#include "wxsnewwindowdlg.h"
#include "resources/wxswindowres.h"
#include "wxsmith.h"

#include <wx/xrc/xmlres.h>
#include <projectmanager.h>

wxsNewWindowDlg::wxsNewWindowDlg(wxWindow* parent,wxsWindowRes::WindowResType _Type):
    SourceNotTouched(true),
    HeaderNotTouched(true),
    BlockText(false),
    Type(_Type)
{
    wxXmlResource::Get()->LoadDialog(this, parent, _T("wxsNewWindowDlg"));
    
    Class  = XRCCTRL(*this, "ClassName", wxTextCtrl);
    Source = XRCCTRL(*this, "SourceFile", wxTextCtrl);
    Header = XRCCTRL(*this, "HeaderFile", wxTextCtrl);
    
    if ( !Class || !Source || !Header )
    {
        Close();
    }
    else
    {
    	BlockText = true;
    	switch ( Type )
    	{
    		case wxsWindowRes::Dialog:
                Class->SetValue(_("MyDialog"));
                Source->SetValue(_("mydialog.cpp"));
                Header->SetValue(_("mydialog.h"));
                SetTitle(_("New Dialog resource"));
                break;
                
            case wxsWindowRes::Frame:
                Class->SetValue(_("MyFrame"));
                Source->SetValue(_("myframe.cpp"));
                Header->SetValue(_("myframe.h"));
                SetTitle(_("New Frame resource"));
                break;
            
            case wxsWindowRes::Panel:
                Class->SetValue(_("MyPanel"));
                Source->SetValue(_("mypanel.cpp"));
                Header->SetValue(_("mypanel.h"));
                SetTitle(_("New Panel resource"));
                break;
    	}
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
    // Need to do some checks
    
    // First - validating name
    const wxChar* ClassName = Class->GetValue().c_str();

    bool NameValid = true;

    if ( ( *ClassName < _T('a') || *ClassName > _T('z') ) &&
         ( *ClassName < _T('A') || *ClassName > _T('Z') ) &&
         ( *ClassName != _T('_') ) )
    {
        NameValid = false;
    }
    else
    {
        while ( *++ClassName )
        {
            if ( ( *ClassName < _T('a') || *ClassName > _T('z') ) &&
                 ( *ClassName < _T('A') || *ClassName > _T('Z') ) &&
                 ( *ClassName < _T('0') || *ClassName > _T('9') ) &&
                 ( *ClassName != _T('_') ) )
            {
                NameValid = false;
                break;
            }
        }
    }
    
    if ( !NameValid )
    { 
        wxMessageBox(_("Invalid class name"));
        return;
    }
    
    // Second - checking if there's given resoure in current project
    
    wxsProject* Proj = wxSmith::Get()->GetSmithProject(Manager::Get()->GetProjectManager()->GetActiveProject());
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
              wxString::Format(_("File '%s' already exist. It will be overwritten.\nContinue ?"),Header->GetValue().c_str()),
              _("File exists"),wxYES_NO|wxICON_ERROR) != wxYES )
        {
            return;
        }
    }
    
    if ( wxFileName::FileExists(Proj->GetProjectFileName(Source->GetValue())) )
    {
        if ( wxMessageBox(
              wxString::Format(_("File '' already exist. It will be overwritten.\nContinue ?"),Source->GetValue().c_str()),
              _("File exists"),wxYES_NO|wxICON_ERROR) != wxYES )
        {
            return;
        }
    }

    // Creating dialog

    wxString XrcFile = Class->GetValue() + _T(".xrc");
    wxsWindowRes* NewWindow = NULL;
    
    switch ( Type )
    {
        case wxsWindowRes::Dialog:
            NewWindow = new wxsDialogRes(Proj,Class->GetValue(),XrcFile,Source->GetValue(),Header->GetValue());
            break;
            
        case wxsWindowRes::Frame:
            NewWindow = new wxsFrameRes(Proj,Class->GetValue(),XrcFile,Source->GetValue(),Header->GetValue());
            break;
        
        case wxsWindowRes::Panel:
            NewWindow = new wxsPanelRes(Proj,Class->GetValue(),XrcFile,Source->GetValue(),Header->GetValue());
            break;
    }
    
    if ( !NewWindow->GenerateEmptySources() )
    {
        wxMessageBox(_("Couldn't generate sources"),_("Error"),wxOK|wxICON_ERROR);
        delete NewWindow;
        return;
    }
    
    cbProject* cbProj = Manager::Get()->GetProjectManager()->GetActiveProject();
    
    // Selecting target
    
    wxArrayInt targets;
    Manager::Get()->GetProjectManager()->AddFileToProject(Header->GetValue(), cbProj, targets);
    if (targets.GetCount() != 0)
        Manager::Get()->GetProjectManager()->AddFileToProject(Source->GetValue(), cbProj, targets);
    Manager::Get()->GetProjectManager()->RebuildTree();
    
    
    // Adding dialog to project and opening editor for it

    switch ( Type )
    {
        case wxsWindowRes::Dialog:
            Proj->AddDialog((wxsDialogRes*)NewWindow);
            break;
            
        case wxsWindowRes::Frame:
            Proj->AddFrame((wxsFrameRes*)NewWindow);
            break;
        
        case wxsWindowRes::Panel:
            Proj->AddPanel((wxsPanelRes*)NewWindow);
            break;
    }
    
    wxsSelectRes(NewWindow);
    Close();
}

void wxsNewWindowDlg::OnClassChanged(wxCommandEvent& event)
{
    if ( BlockText ) return;
    BlockText = true;
    if ( HeaderNotTouched ) Header->SetValue((Class->GetValue() + _T(".h")).MakeLower());
    if ( SourceNotTouched ) Source->SetValue((Class->GetValue() + _T(".cpp")).MakeLower());
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
    if ( SourceNotTouched )
    {
        wxFileName FN(Header->GetValue());
        FN.SetExt(_T(".cpp"));
        Source->SetValue(FN.GetFullPath());
    }
    HeaderNotTouched = false;
    BlockText = false;
}
    
BEGIN_EVENT_TABLE(wxsNewWindowDlg,wxDialog)
    EVT_BUTTON(XRCID("Cancel"),wxsNewWindowDlg::OnCancel)
    EVT_BUTTON(XRCID("Create"),wxsNewWindowDlg::OnCreate)
    EVT_TEXT(XRCID("ClassName"),wxsNewWindowDlg::OnClassChanged)
    EVT_TEXT(XRCID("SourceFile"),wxsNewWindowDlg::OnSourceChanged)
    EVT_TEXT(XRCID("HeaderFile"),wxsNewWindowDlg::OnHeaderChanged)
END_EVENT_TABLE()
