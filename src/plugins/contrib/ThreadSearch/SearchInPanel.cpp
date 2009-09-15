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
	#include <wx/checkbox.h>
	#include <wx/sizer.h>
#endif

#include "SearchInPanel.h"
#include "ThreadSearchControlIds.h"


SearchInPanel::SearchInPanel(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long WXUNUSED(style)):
    wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
    //{ Getting the imagesize for the buttons (16x16 or 22x22) and the appropriate path
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    int toolbar_size = cfg->ReadBool(_T("/environment/toolbar_size"),true)?1:0;
    wxString prefix = ConfigManager::GetDataFolder() + _T("/images/ThreadSearch/") + (toolbar_size==1?_T("16x16/"):_T("22x22/"));
    //}
    //{ The images are stored in triples in a wxImageList to simplify and accelerate the assignment.
    // The order is: normal - disabled - selected
    //
    m_pButtonImages = new wxImageList(toolbar_size == 1?16:22, toolbar_size == 1?16:22);
    m_pButtonImages->Add(wxBitmap(prefix + wxT("openfiles.png"), wxBITMAP_TYPE_PNG));
    m_pButtonImages->Add(wxBitmap(prefix + wxT("openfilesdisabled.png"), wxBITMAP_TYPE_PNG));
    m_pButtonImages->Add(wxBitmap(prefix + wxT("openfilesselected.png"), wxBITMAP_TYPE_PNG));

    m_pButtonImages->Add(wxBitmap(prefix + wxT("target.png"), wxBITMAP_TYPE_PNG));
    m_pButtonImages->Add(wxBitmap(prefix + wxT("targetdisabled.png"), wxBITMAP_TYPE_PNG));
    m_pButtonImages->Add(wxBitmap(prefix + wxT("targetselected.png"), wxBITMAP_TYPE_PNG));

    m_pButtonImages->Add(wxBitmap(prefix + wxT("project.png"), wxBITMAP_TYPE_PNG));
    m_pButtonImages->Add(wxBitmap(prefix + wxT("projectdisabled.png"), wxBITMAP_TYPE_PNG));
    m_pButtonImages->Add(wxBitmap(prefix + wxT("projectselected.png"), wxBITMAP_TYPE_PNG));

    m_pButtonImages->Add(wxBitmap(prefix + wxT("workspace.png"), wxBITMAP_TYPE_PNG));
    m_pButtonImages->Add(wxBitmap(prefix + wxT("workspacedisabled.png"), wxBITMAP_TYPE_PNG));
    m_pButtonImages->Add(wxBitmap(prefix + wxT("workspaceselected.png"), wxBITMAP_TYPE_PNG));

    m_pButtonImages->Add(wxBitmap(prefix + wxT("folder.png"), wxBITMAP_TYPE_PNG));
    m_pButtonImages->Add(wxBitmap(prefix + wxT("folderdisabled.png"), wxBITMAP_TYPE_PNG));
    m_pButtonImages->Add(wxBitmap(prefix + wxT("folderselected.png"), wxBITMAP_TYPE_PNG));
    //}

    // begin wxGlade: SearchInPanel::SearchInPanel
    m_pBtnSearchOpenFiles = new wxBitmapButton(this, idBtnSearchOpenFiles, m_pButtonImages->GetBitmap(0));
    m_pBtnSearchTargetFiles = new wxBitmapButton(this, idBtnSearchTargetFiles, m_pButtonImages->GetBitmap(3));
    m_pBtnSearchProjectFiles = new wxBitmapButton(this, idBtnSearchProjectFiles, m_pButtonImages->GetBitmap(6));
    m_pBtnSearchWorkspaceFiles = new wxBitmapButton(this, idBtnSearchWorkspaceFiles, m_pButtonImages->GetBitmap(9));
    m_pBtnSearchDir = new wxBitmapButton(this, idBtnSearchDirectoryFiles, m_pButtonImages->GetBitmap(12));

    set_properties();
    do_layout();
    // end wxGlade
}


BEGIN_EVENT_TABLE(SearchInPanel, wxPanel)
    // begin wxGlade: SearchInPanel::event_table
    EVT_BUTTON(idBtnSearchOpenFiles, SearchInPanel::OnBtnClickEvent)
    EVT_BUTTON(idBtnSearchTargetFiles, SearchInPanel::OnBtnSearchTargetFilesClick)
    EVT_BUTTON(idBtnSearchProjectFiles, SearchInPanel::OnBtnSearchProjectFilesClick)
    EVT_BUTTON(idBtnSearchWorkspaceFiles, SearchInPanel::OnBtnSearchWorkspaceFilesClick)
    EVT_BUTTON(idBtnSearchDirectoryFiles, SearchInPanel::OnBtnClickEvent)
    // end wxGlade
END_EVENT_TABLE();


void SearchInPanel::OnBtnClickEvent(wxCommandEvent &event)
{
    switch ( event.GetId() )
    {
        case idBtnSearchOpenFiles:
            SetSearchInOpenFiles(!m_StateSearchOpenFiles);
            break;
        case idBtnSearchDirectoryFiles:
            SetSearchInDirectory(!m_StateSearchDirectoryFiles);
            break;
    }
    event.Skip();
}


void SearchInPanel::OnBtnSearchTargetFilesClick(wxCommandEvent &event)
{
    // If target scope becomes checked, we uncheck if necessary project
    // and workspace.
    SetSearchInTargetFiles(!m_StateSearchTargetFiles);
    if (m_StateSearchTargetFiles)
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
    SetSearchInProjectFiles(!m_StateSearchProjectFiles);
    if (m_StateSearchProjectFiles)
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
    SetSearchInWorkspaceFiles(!m_StateSearchWorkspaceFiles);
    if (m_StateSearchWorkspaceFiles)
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
    m_pBtnSearchOpenFiles->SetBitmapDisabled(m_pButtonImages->GetBitmap(1));
    m_pBtnSearchOpenFiles->SetSize(m_pBtnSearchOpenFiles->GetBestSize());
    m_pBtnSearchTargetFiles->SetToolTip(_("Search in target files"));
    m_pBtnSearchTargetFiles->SetBitmapDisabled(m_pButtonImages->GetBitmap(4));
    m_pBtnSearchTargetFiles->SetSize(m_pBtnSearchTargetFiles->GetBestSize());
    m_pBtnSearchProjectFiles->SetToolTip(_("Search in project files"));
    m_pBtnSearchProjectFiles->SetBitmapDisabled(m_pButtonImages->GetBitmap(7));
    m_pBtnSearchProjectFiles->SetSize(m_pBtnSearchProjectFiles->GetBestSize());
    m_pBtnSearchWorkspaceFiles->SetToolTip(_("Search in workspace files"));
    m_pBtnSearchWorkspaceFiles->SetBitmapDisabled(m_pButtonImages->GetBitmap(10));
    m_pBtnSearchWorkspaceFiles->SetSize(m_pBtnSearchWorkspaceFiles->GetBestSize());
    m_pBtnSearchDir->SetToolTip(_("Search in directory files"));
    m_pBtnSearchDir->SetBitmapDisabled(m_pButtonImages->GetBitmap(13));
    m_pBtnSearchDir->SetSize(m_pBtnSearchDir->GetBestSize());
    // end wxGlade
}


void SearchInPanel::do_layout()
{
    // begin wxGlade: SearchInPanel::do_layout
    wxBoxSizer* SizerTop = new wxBoxSizer(wxHORIZONTAL);
    SizerTop->Add(m_pBtnSearchOpenFiles, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pBtnSearchTargetFiles, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pBtnSearchProjectFiles, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pBtnSearchWorkspaceFiles, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pBtnSearchDir, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SetAutoLayout(true);
    SetSizer(SizerTop);
    SizerTop->Fit(this);
    SizerTop->SetSizeHints(this);
    // end wxGlade
}


//{ Getters
bool SearchInPanel::GetSearchInOpenFiles()      const {return m_StateSearchOpenFiles;}
bool SearchInPanel::GetSearchInTargetFiles()    const {return m_StateSearchTargetFiles;}
bool SearchInPanel::GetSearchInProjectFiles()   const {return m_StateSearchProjectFiles;}
bool SearchInPanel::GetSearchInWorkspaceFiles() const {return m_StateSearchWorkspaceFiles;}
bool SearchInPanel::GetSearchInDirectory()      const {return m_StateSearchDirectoryFiles;}
//}

//{ Setters
void SearchInPanel::SetSearchInOpenFiles(bool bSearchInOpenFiles)
{
    m_StateSearchOpenFiles=bSearchInOpenFiles;
    m_pBtnSearchOpenFiles->SetBitmapLabel(m_pButtonImages->GetBitmap(bSearchInOpenFiles?2:0));
}
void SearchInPanel::SetSearchInTargetFiles(bool bSearchInTargetFiles)
{
    m_StateSearchTargetFiles=bSearchInTargetFiles;
    m_pBtnSearchTargetFiles->SetBitmapLabel(m_pButtonImages->GetBitmap(bSearchInTargetFiles?5:3));
}
void SearchInPanel::SetSearchInProjectFiles(bool bSearchInProjectFiles)
{
    m_StateSearchProjectFiles=bSearchInProjectFiles;
    m_pBtnSearchProjectFiles->SetBitmapLabel(m_pButtonImages->GetBitmap(bSearchInProjectFiles?8:6));
}
void SearchInPanel::SetSearchInWorkspaceFiles(bool bSearchInWorkspaceFiles)
{
    m_StateSearchWorkspaceFiles=bSearchInWorkspaceFiles;
    m_pBtnSearchWorkspaceFiles->SetBitmapLabel(m_pButtonImages->GetBitmap(bSearchInWorkspaceFiles?11:9));
}
void SearchInPanel::SetSearchInDirectory(bool bSearchInDirectoryFiles)
{
    m_StateSearchDirectoryFiles=bSearchInDirectoryFiles;
    m_pBtnSearchDir->SetBitmapLabel(m_pButtonImages->GetBitmap(bSearchInDirectoryFiles?14:12));
}
//}
