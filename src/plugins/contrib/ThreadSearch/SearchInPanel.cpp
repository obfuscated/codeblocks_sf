/***************************************************************
 * Name:      SearchInPanel
 *
 * Purpose:   This class is a panel that allows the user to
 *            define the search scope : open files, project,
 *            worspace or directory.
 *            It is used in the ThreadSearchView and the
 *            ThreadSearchConfPanel.
 *            It does nothing but forwarding events to the
 *            parent window.
 *
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/bitmap.h>
    #include <wx/bmpbuttn.h>
    #include <wx/checkbox.h>
    #include <wx/sizer.h>
    #include "configmanager.h"
#endif

#include "wx/things/toggle.h"
#include "wx/tglbtn.h"

#include "SearchInPanel.h"
#include "ThreadSearchControlIds.h"


SearchInPanel::SearchInPanel(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long WXUNUSED(style)):
    wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
    //{ Getting the imagesize for the buttons (16x16 or 22x22) and the appropriate path
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    bool toolbar_size = cfg->ReadBool(_T("/environment/toolbar_size"),true);
    wxString prefix = ConfigManager::GetDataFolder() + _T("/images/ThreadSearch/") + (toolbar_size?_T("16x16/"):_T("22x22/"));

    // create a dummy button to get the standard button-size,
    // wxCustomButton does not do that properly
    // we have to force this size as MinSize to make it work
    wxBitmapButton dummyBtn(this, wxID_ANY, wxBitmap(prefix + wxT("openfiles.png"), wxBITMAP_TYPE_PNG));
    wxSize dummySize = dummyBtn.GetSize();

    m_pBtnSearchOpenFiles = new wxCustomButton(this, idBtnSearchOpenFiles, wxBitmap(prefix + wxT("openfiles.png"), wxBITMAP_TYPE_PNG), wxDefaultPosition, dummySize);
    m_pBtnSearchOpenFiles->SetBitmapDisabled(wxBitmap(prefix + wxT("openfilesdisabled.png"), wxBITMAP_TYPE_PNG));
    m_pBtnSearchOpenFiles->SetBitmapSelected(wxBitmap(prefix + wxT("openfilesselected.png"), wxBITMAP_TYPE_PNG));
    m_pBtnSearchOpenFiles->SetMinSize(dummySize);
    m_pBtnSearchTargetFiles = new wxCustomButton(this, idBtnSearchTargetFiles, wxBitmap(prefix + wxT("target.png"), wxBITMAP_TYPE_PNG), wxDefaultPosition, dummySize);
    m_pBtnSearchTargetFiles->SetBitmapDisabled(wxBitmap(prefix + wxT("targetdisabled.png"), wxBITMAP_TYPE_PNG));
    m_pBtnSearchTargetFiles->SetBitmapSelected(wxBitmap(prefix + wxT("targetselected.png"), wxBITMAP_TYPE_PNG));
    m_pBtnSearchTargetFiles->SetMinSize(dummySize);
    m_pBtnSearchProjectFiles = new wxCustomButton(this, idBtnSearchProjectFiles, wxBitmap(prefix + wxT("project.png"), wxBITMAP_TYPE_PNG), wxDefaultPosition, dummySize);
    m_pBtnSearchProjectFiles->SetBitmapDisabled(wxBitmap(prefix + wxT("projectdisabled.png"), wxBITMAP_TYPE_PNG));
    m_pBtnSearchProjectFiles->SetBitmapSelected(wxBitmap(prefix + wxT("projectselected.png"), wxBITMAP_TYPE_PNG));
    m_pBtnSearchProjectFiles->SetMinSize(dummySize);
    m_pBtnSearchWorkspaceFiles = new wxCustomButton(this, idBtnSearchWorkspaceFiles, wxBitmap(prefix + wxT("workspace.png"), wxBITMAP_TYPE_PNG), wxDefaultPosition, dummySize);
    m_pBtnSearchWorkspaceFiles->SetBitmapDisabled(wxBitmap(prefix + wxT("workspacedisabled.png"), wxBITMAP_TYPE_PNG));
    m_pBtnSearchWorkspaceFiles->SetBitmapSelected(wxBitmap(prefix + wxT("workspaceselected.png"), wxBITMAP_TYPE_PNG));
    m_pBtnSearchWorkspaceFiles->SetMinSize(dummySize);
    m_pBtnSearchDir = new wxCustomButton(this, idBtnSearchDirectoryFiles, wxBitmap(prefix + wxT("folder.png"), wxBITMAP_TYPE_PNG), wxDefaultPosition, dummySize);
    m_pBtnSearchDir->SetBitmapDisabled(wxBitmap(prefix + wxT("folderdisabled.png"), wxBITMAP_TYPE_PNG));
    m_pBtnSearchDir->SetBitmapSelected(wxBitmap(prefix + wxT("folderselected.png"), wxBITMAP_TYPE_PNG));
    m_pBtnSearchDir->SetMinSize(dummySize);

    set_properties();
    do_layout();
    // end wxGlade
}


BEGIN_EVENT_TABLE(SearchInPanel, wxPanel)
    // begin wxGlade: SearchInPanel::event_table
    EVT_TOGGLEBUTTON(idBtnSearchOpenFiles, SearchInPanel::OnBtnClickEvent)
    EVT_TOGGLEBUTTON(idBtnSearchTargetFiles, SearchInPanel::OnBtnSearchTargetFilesClick)
    EVT_TOGGLEBUTTON(idBtnSearchProjectFiles, SearchInPanel::OnBtnSearchProjectFilesClick)
    EVT_TOGGLEBUTTON(idBtnSearchWorkspaceFiles, SearchInPanel::OnBtnSearchWorkspaceFilesClick)
    EVT_TOGGLEBUTTON(idBtnSearchDirectoryFiles, SearchInPanel::OnBtnClickEvent)
    // end wxGlade
END_EVENT_TABLE();


void SearchInPanel::OnBtnClickEvent(wxCommandEvent &event)
{
    event.Skip();
}


void SearchInPanel::OnBtnSearchTargetFilesClick(wxCommandEvent &event)
{
    // If target scope becomes checked, we uncheck if necessary project
    // and workspace.
    if(event.GetInt())
    {
        SetSearchInProjectFiles(false);
        SetSearchInWorkspaceFiles(false);
    }
    event.Skip();
}


void SearchInPanel::OnBtnSearchProjectFilesClick(wxCommandEvent &event)
{
    // If project scope becomes checked, we uncheck if necessary target
    // and workspace.
    if (event.GetInt())
    {
        SetSearchInTargetFiles(false);
        SetSearchInWorkspaceFiles(false);
    }
    event.Skip();
}


void SearchInPanel::OnBtnSearchWorkspaceFilesClick(wxCommandEvent &event)
{
    // If workspace scope becomes checked, we uncheck if necessary target
    // and project.
    if (event.GetInt())
    {
        SetSearchInTargetFiles(false);
        SetSearchInProjectFiles(false);
    }
    event.Skip();
}


// wxGlade: add SearchInPanel event handlers


void SearchInPanel::set_properties()
{
    // begin wxGlade: SearchInPanel::set_properties
    m_pBtnSearchOpenFiles->SetToolTip(_("Search in open files"));
    m_pBtnSearchTargetFiles->SetToolTip(_("Search in target files"));
    m_pBtnSearchProjectFiles->SetToolTip(_("Search in project files"));
    m_pBtnSearchWorkspaceFiles->SetToolTip(_("Search in workspace files"));
    m_pBtnSearchDir->SetToolTip(_("Search in directory files"));
    // end wxGlade
}


void SearchInPanel::do_layout()
{
#if wxCHECK_VERSION(2, 9, 0)
    #define wxADJUST_MINSIZE 0
#endif
    // begin wxGlade: SearchInPanel::do_layout
    wxBoxSizer* SizerTop = new wxBoxSizer(wxHORIZONTAL);
    SizerTop->Add(m_pBtnSearchOpenFiles, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pBtnSearchTargetFiles, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pBtnSearchProjectFiles, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pBtnSearchWorkspaceFiles, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pBtnSearchDir, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SetAutoLayout(true);
    SetSizer(SizerTop);
    SizerTop->Fit(this);
    SizerTop->SetSizeHints(this);
    // end wxGlade
}


//{ Getters
bool SearchInPanel::GetSearchInOpenFiles()      const {return m_pBtnSearchOpenFiles->GetValue();}
bool SearchInPanel::GetSearchInTargetFiles()    const {return m_pBtnSearchTargetFiles->GetValue();}
bool SearchInPanel::GetSearchInProjectFiles()   const {return m_pBtnSearchProjectFiles->GetValue();}
bool SearchInPanel::GetSearchInWorkspaceFiles() const {return m_pBtnSearchWorkspaceFiles->GetValue();}
bool SearchInPanel::GetSearchInDirectory()      const {return m_pBtnSearchDir->GetValue();}
//}

//{ Setters
void SearchInPanel::SetSearchInOpenFiles(bool bSearchInOpenFiles)
{
    m_pBtnSearchOpenFiles->SetValue(bSearchInOpenFiles);
}
void SearchInPanel::SetSearchInTargetFiles(bool bSearchInTargetFiles)
{
    m_pBtnSearchTargetFiles->SetValue(bSearchInTargetFiles);
}
void SearchInPanel::SetSearchInProjectFiles(bool bSearchInProjectFiles)
{
    m_pBtnSearchProjectFiles->SetValue(bSearchInProjectFiles);
}
void SearchInPanel::SetSearchInWorkspaceFiles(bool bSearchInWorkspaceFiles)
{
    m_pBtnSearchWorkspaceFiles->SetValue(bSearchInWorkspaceFiles);
}
void SearchInPanel::SetSearchInDirectory(bool bSearchInDirectoryFiles)
{
    m_pBtnSearchDir->SetValue(bSearchInDirectoryFiles);
}
//}
