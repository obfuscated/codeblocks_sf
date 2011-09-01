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

#if defined(CB_PRECOMP)
#include "sdk.h"
#endif

#ifndef CB_PRECOMP
	#include <wx/checkbox.h>
	#include <wx/sizer.h>
#endif

#include "SearchInPanel.h"
#include "ThreadSearchControlIds.h"


SearchInPanel::SearchInPanel(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long WXUNUSED(style)):
    wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
    // begin wxGlade: SearchInPanel::SearchInPanel
    m_pChkSearchOpenFiles = new wxCheckBox(this, idChkSearchOpenFiles, wxT("Open files"));
    //-m_pChkSearchProjectFiles = new wxCheckBox(this, idChkSearchProjectFiles, wxT("Project files"));
    m_pChkSearchSnippetFiles = new wxCheckBox(this, idChkSearchSnippetFiles, wxT("Snippets"));
    //-m_pChkSearchWorkspaceFiles = new wxCheckBox(this, idChkSearchWorkspaceFiles, wxT("Workspace files"));
    m_pChkSearchDir = new wxCheckBox(this, idChkSearchDirectoryFiles, wxT("Directory"));

    set_properties();
    do_layout();
    // end wxGlade
}


BEGIN_EVENT_TABLE(SearchInPanel, wxPanel)
    // begin wxGlade: SearchInPanel::event_table
    EVT_CHECKBOX(idChkSearchOpenFiles, SearchInPanel::OnChkClickEvent)
    EVT_CHECKBOX(idChkSearchSnippetFiles, SearchInPanel::OnChkSearchSnippetFilesClick)
    //-EVT_CHECKBOX(idChkSearchWorkspaceFiles, SearchInPanel::OnChkSearchWorkspaceFilesClick)
    EVT_CHECKBOX(idChkSearchDirectoryFiles, SearchInPanel::OnChkClickEvent)
    // end wxGlade
END_EVENT_TABLE();


void SearchInPanel::OnChkClickEvent(wxCommandEvent &event)
{
    event.Skip();
}


void SearchInPanel::OnChkSearchSnippetFilesClick(wxCommandEvent &event)
{
	// If project scope checkbox becomes checked, we uncheck if necessary workspace
	// checkbox because project is included in workspace.
    ////if ( (event.IsChecked() == true) && (m_pChkSearchWorkspaceFiles->IsChecked() == true) )
    ////{
    ////    m_pChkSearchWorkspaceFiles->SetValue(false);
    ////    wxCommandEvent ChkEvent(wxEVT_COMMAND_CHECKBOX_CLICKED, idChkSearchWorkspaceFiles);
    ////    ChkEvent.SetInt(0);
    ////    ProcessEvent(ChkEvent);
    ////}
	event.Skip();
}


void SearchInPanel::OnChkSearchWorkspaceFilesClick(wxCommandEvent &event)
{
	// If worspace scope checkbox becomes checked, we uncheck if necessary project
	// checkbox because project is included in workspace.
	if ( (event.IsChecked()) == true && (m_pChkSearchSnippetFiles->IsChecked() == true) )
	{
		m_pChkSearchSnippetFiles->SetValue(false);
		wxCommandEvent ChkEvent(wxEVT_COMMAND_CHECKBOX_CLICKED, idChkSearchSnippetFiles);
		ChkEvent.SetInt(0);
		ProcessEvent(ChkEvent);
	}
	event.Skip();
}


// wxGlade: add SearchInPanel event handlers


void SearchInPanel::set_properties()
{
    // begin wxGlade: SearchInPanel::set_properties
    m_pChkSearchOpenFiles->SetToolTip(wxT("Search in open files"));
    m_pChkSearchOpenFiles->SetValue(1);
    //-m_pChkSearchProjectFiles->SetToolTip(wxT("Search in project files"));
    m_pChkSearchSnippetFiles->SetToolTip(wxT("Search in Snippets Tree"));
    m_pChkSearchSnippetFiles->SetValue(1);
    //-m_pChkSearchWorkspaceFiles->SetToolTip(wxT("Search in workspace files"));
    m_pChkSearchDir->SetToolTip(wxT("Search in directory files"));
    // end wxGlade
}


void SearchInPanel::do_layout()
{
    // begin wxGlade: SearchInPanel::do_layout
    wxBoxSizer* SizerTop = new wxBoxSizer(wxHORIZONTAL);
    SizerTop->Add(m_pChkSearchOpenFiles, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pChkSearchSnippetFiles, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    //-SizerTop->Add(m_pChkSearchWorkspaceFiles, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pChkSearchDir, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SetAutoLayout(true);
    SetSizer(SizerTop);
    SizerTop->Fit(this);
    SizerTop->SetSizeHints(this);
    // end wxGlade
}


// Getters
bool SearchInPanel::GetSearchInOpenFiles()      const                       {return m_pChkSearchOpenFiles->IsChecked();}
bool SearchInPanel::GetSearchInSnippetFiles()   const                       {return m_pChkSearchSnippetFiles->IsChecked();}
//-bool SearchInPanel::GetSearchInWorkspaceFiles() const                       {return m_pChkSearchWorkspaceFiles->IsChecked();}
bool SearchInPanel::GetSearchInDirectory()      const                       {return m_pChkSearchDir->IsChecked();}

// Setters
void SearchInPanel::SetSearchInOpenFiles     (bool bSearchInOpenFiles)      {m_pChkSearchOpenFiles->SetValue(bSearchInOpenFiles);}
void SearchInPanel::SetSearchInSnippetFiles  (bool bSearchInSnippetFiles)   {m_pChkSearchSnippetFiles->SetValue(bSearchInSnippetFiles);}
//-void SearchInPanel::SetSearchInWorkspaceFiles(bool bSearchInWorkspaceFiles) {m_pChkSearchWorkspaceFiles->SetValue(bSearchInWorkspaceFiles);}
void SearchInPanel::SetSearchInDirectory     (bool bSearchInDirectory)      {m_pChkSearchDir->SetValue(bSearchInDirectory);}
