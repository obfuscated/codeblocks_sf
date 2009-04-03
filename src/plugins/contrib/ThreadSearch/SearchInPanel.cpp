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
    // begin wxGlade: SearchInPanel::SearchInPanel
    m_pChkSearchOpenFiles = new wxCheckBox(this, idChkSearchOpenFiles, _("Open"));
    m_pChkSearchTargetFiles = new wxCheckBox(this, idChkSearchTargetFiles, _("Target"));
    m_pChkSearchProjectFiles = new wxCheckBox(this, idChkSearchProjectFiles, _("Project"));
    m_pChkSearchWorkspaceFiles = new wxCheckBox(this, idChkSearchWorkspaceFiles, _("Workspace"));
    m_pChkSearchDir = new wxCheckBox(this, idChkSearchDirectoryFiles, _("Directory"));

    set_properties();
    do_layout();
    // end wxGlade
}


BEGIN_EVENT_TABLE(SearchInPanel, wxPanel)
    // begin wxGlade: SearchInPanel::event_table
    EVT_CHECKBOX(idChkSearchOpenFiles, SearchInPanel::OnChkClickEvent)
    EVT_CHECKBOX(idChkSearchTargetFiles, SearchInPanel::OnChkSearchTargetFilesClick)
    EVT_CHECKBOX(idChkSearchProjectFiles, SearchInPanel::OnChkSearchProjectFilesClick)
    EVT_CHECKBOX(idChkSearchWorkspaceFiles, SearchInPanel::OnChkSearchWorkspaceFilesClick)
    EVT_CHECKBOX(idChkSearchDirectoryFiles, SearchInPanel::OnChkClickEvent)
    // end wxGlade
END_EVENT_TABLE();


void SearchInPanel::OnChkClickEvent(wxCommandEvent &event)
{
    event.Skip();
}


void SearchInPanel::OnChkSearchTargetFilesClick(wxCommandEvent &event)
{
	// If target scope checkbox becomes checked, we uncheck if necessary project
	// and workspace checkboxes.
	if ( event.IsChecked() == true )
	{
		if ( m_pChkSearchProjectFiles->IsChecked() == true )
		{
			m_pChkSearchProjectFiles->SetValue(false);
			wxCommandEvent ChkEvent(wxEVT_COMMAND_CHECKBOX_CLICKED, idChkSearchProjectFiles);
			ChkEvent.SetInt(0);
			ProcessEvent(ChkEvent);
		}

		if ( m_pChkSearchWorkspaceFiles->IsChecked() == true )
		{
			m_pChkSearchWorkspaceFiles->SetValue(false);
			wxCommandEvent ChkEvent(wxEVT_COMMAND_CHECKBOX_CLICKED, idChkSearchWorkspaceFiles);
			ChkEvent.SetInt(0);
			ProcessEvent(ChkEvent);
		}
	}
    event.Skip();
}


void SearchInPanel::OnChkSearchProjectFilesClick(wxCommandEvent &event)
{
	// If project scope checkbox becomes checked, we uncheck if necessary target
	// and workspace checkboxes.
	if ( event.IsChecked() == true )
	{
		if ( m_pChkSearchTargetFiles->IsChecked() == true )
		{
			m_pChkSearchTargetFiles->SetValue(false);
			wxCommandEvent ChkEvent(wxEVT_COMMAND_CHECKBOX_CLICKED, idChkSearchTargetFiles);
			ChkEvent.SetInt(0);
			ProcessEvent(ChkEvent);
		}

		if ( m_pChkSearchWorkspaceFiles->IsChecked() == true )
		{
			m_pChkSearchWorkspaceFiles->SetValue(false);
			wxCommandEvent ChkEvent(wxEVT_COMMAND_CHECKBOX_CLICKED, idChkSearchWorkspaceFiles);
			ChkEvent.SetInt(0);
			ProcessEvent(ChkEvent);
		}
	}
	event.Skip();
}


void SearchInPanel::OnChkSearchWorkspaceFilesClick(wxCommandEvent &event)
{
	// If workspace scope checkbox becomes checked, we uncheck if necessary target
	// and project checkboxes.
	if ( event.IsChecked() == true )
	{
		if ( m_pChkSearchTargetFiles->IsChecked() == true )
		{
			m_pChkSearchTargetFiles->SetValue(false);
			wxCommandEvent ChkEvent(wxEVT_COMMAND_CHECKBOX_CLICKED, idChkSearchTargetFiles);
			ChkEvent.SetInt(0);
			ProcessEvent(ChkEvent);
		}

		if ( m_pChkSearchProjectFiles->IsChecked() == true )
		{
			m_pChkSearchProjectFiles->SetValue(false);
			wxCommandEvent ChkEvent(wxEVT_COMMAND_CHECKBOX_CLICKED, idChkSearchProjectFiles);
			ChkEvent.SetInt(0);
			ProcessEvent(ChkEvent);
		}
	}
    event.Skip();
}


// wxGlade: add SearchInPanel event handlers


void SearchInPanel::set_properties()
{
    // begin wxGlade: SearchInPanel::set_properties
    m_pChkSearchOpenFiles->SetToolTip(_("Search in open files"));
    m_pChkSearchOpenFiles->SetValue(1);
    m_pChkSearchTargetFiles->SetToolTip(_("Search in target files"));
    m_pChkSearchProjectFiles->SetToolTip(_("Search in project files"));
    m_pChkSearchProjectFiles->SetValue(1);
    m_pChkSearchWorkspaceFiles->SetToolTip(_("Search in workspace files"));
    m_pChkSearchDir->SetToolTip(_("Search in directory files"));
    // end wxGlade
}


void SearchInPanel::do_layout()
{
    // begin wxGlade: SearchInPanel::do_layout
    wxBoxSizer* SizerTop = new wxBoxSizer(wxHORIZONTAL);
    SizerTop->Add(m_pChkSearchOpenFiles, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pChkSearchTargetFiles, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pChkSearchProjectFiles, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pChkSearchWorkspaceFiles, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pChkSearchDir, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SetAutoLayout(true);
    SetSizer(SizerTop);
    SizerTop->Fit(this);
    SizerTop->SetSizeHints(this);
    // end wxGlade
}


// Getters
bool SearchInPanel::GetSearchInOpenFiles()      const                       {return m_pChkSearchOpenFiles->IsChecked();}
bool SearchInPanel::GetSearchInTargetFiles()    const                       {return m_pChkSearchTargetFiles->IsChecked();}
bool SearchInPanel::GetSearchInProjectFiles()   const                       {return m_pChkSearchProjectFiles->IsChecked();}
bool SearchInPanel::GetSearchInWorkspaceFiles() const                       {return m_pChkSearchWorkspaceFiles->IsChecked();}
bool SearchInPanel::GetSearchInDirectory()      const                       {return m_pChkSearchDir->IsChecked();}

// Setters
void SearchInPanel::SetSearchInOpenFiles     (bool bSearchInOpenFiles)      {m_pChkSearchOpenFiles->SetValue(bSearchInOpenFiles);}
void SearchInPanel::SetSearchInTargetFiles   (bool bSearchInTargetFiles)    {m_pChkSearchTargetFiles->SetValue(bSearchInTargetFiles);}
void SearchInPanel::SetSearchInProjectFiles  (bool bSearchInProjectFiles)   {m_pChkSearchProjectFiles->SetValue(bSearchInProjectFiles);}
void SearchInPanel::SetSearchInWorkspaceFiles(bool bSearchInWorkspaceFiles) {m_pChkSearchWorkspaceFiles->SetValue(bSearchInWorkspaceFiles);}
void SearchInPanel::SetSearchInDirectory     (bool bSearchInDirectory)      {m_pChkSearchDir->SetValue(bSearchInDirectory);}
