/***************************************************************
 * Name:      ThreadSearchLoggerTree
 * Purpose:   ThreadSearchLoggerTree implements the
 *            ThreadSearchLoggerBase interface with a wxTreeCtrl.
 * Author:    Jerome ANTOINE
 * Created:   2007-07-28
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
	// Required extra includes
#endif

#include "ThreadSearchLoggerTree.h"
#include "ThreadSearchEvent.h"
#include "ThreadSearchView.h"

ThreadSearchLoggerTree::ThreadSearchLoggerTree(ThreadSearchView& threadSearchView,
											   ThreadSearch& threadSearchPlugin,
											   InsertIndexManager::eFileSorting fileSorting,
											   wxPanel* pParent,
											   long id)
					   : ThreadSearchLoggerBase(threadSearchView, threadSearchPlugin, fileSorting)
					   , m_pTreeLog(NULL)
					   , m_FirstItemProcessed(false)
{
    m_pTreeLog = new wxTreeCtrl(pParent, id, wxDefaultPosition, wxSize(1,1), wxTR_HAS_BUTTONS|wxTR_LINES_AT_ROOT|wxTR_FULL_ROW_HIGHLIGHT|wxTR_HIDE_ROOT|wxTR_DEFAULT_STYLE|wxSUNKEN_BORDER);
    m_pTreeLog->SetMinSize(wxSize(100, 100));
    m_pTreeLog->AddRoot(wxEmptyString);

	// Events are managed dynamically to be able to stop/start management when required.
	ConnectEvents(pParent);
}


ThreadSearchLoggerTree::~ThreadSearchLoggerTree()
{
	wxWindow* pParent = m_pTreeLog->GetParent();
	if ( pParent != NULL )
	{
		// Events are managed dynamically to be able to stop/start management when required.
		DisconnectEvents(pParent);
	}
	m_pTreeLog->Destroy();
	m_pTreeLog = NULL;
}


wxWindow* ThreadSearchLoggerTree::GetWindow()
{
	return m_pTreeLog;
}


void ThreadSearchLoggerTree::SetFocus()
{
	m_pTreeLog->SetFocus();
}


void ThreadSearchLoggerTree::ConnectEvents(wxEvtHandler* pEvtHandler)
{
	// Dynamic event connections.
	int id = m_pTreeLog->GetId();
	pEvtHandler->Connect(id, wxEVT_COMMAND_TREE_SEL_CHANGED,
			(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
			&ThreadSearchLoggerTree::OnLoggerTreeClick, NULL, this);

	pEvtHandler->Connect(id, wxEVT_COMMAND_TREE_ITEM_ACTIVATED,
			(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
			&ThreadSearchLoggerTree::OnLoggerTreeDoubleClick, NULL, this);
}


void ThreadSearchLoggerTree::DisconnectEvents(wxEvtHandler* pEvtHandler)
{
	// Dynamic event disconnections.
	int id = m_pTreeLog->GetId();
	pEvtHandler->Disconnect(id, wxEVT_COMMAND_TREE_SEL_CHANGED,
			(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
			&ThreadSearchLoggerTree::OnLoggerTreeClick, NULL, this);

	pEvtHandler->Disconnect(id, wxEVT_COMMAND_TREE_ITEM_ACTIVATED,
			(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
			&ThreadSearchLoggerTree::OnLoggerTreeDoubleClick, NULL, this);
}


void ThreadSearchLoggerTree::OnLoggerTreeClick(wxTreeEvent& event)
{
	// Manages list log left double click
    // Gets file path and line from list control
    wxString filepath(wxEmptyString);
    long line;
    if ( GetFileLineFromTreeEvent(event, filepath, line) == false )
    {
		cbMessageBox(wxT("Failed to retrieve file path and line number"), wxT("Error"), wxICON_ERROR);
    	return;
    }
    m_ThreadSearchView.OnLoggerClick(filepath, line);
    event.Skip();
}


void ThreadSearchLoggerTree::OnLoggerTreeDoubleClick(wxTreeEvent& event)
{
	// Manages list log left double click
    // Gets file path and line from list control
    wxString filepath(wxEmptyString);
    long line;
    if ( GetFileLineFromTreeEvent(event, filepath, line) == false )
    {
		cbMessageBox(wxT("Failed to retrieve file path and line number"), wxT("Error"), wxICON_ERROR);
    	return;
    }
    m_ThreadSearchView.OnLoggerDoubleClick(filepath, line);

    event.Skip();
}


bool ThreadSearchLoggerTree::GetFileLineFromTreeEvent(wxTreeEvent& event, wxString& filepath, long &line)
{
	wxTreeItemId lineItemId;
	wxTreeItemId fileItemId;
	wxTreeItemId rootItemId = m_pTreeLog->GetRootItem();
	wxTreeItemId eventItemId = event.GetItem();
	bool success(false);

	filepath = wxEmptyString;
	line = 0;

	do
	{
		// We test if we are on a file item or a line item.
		// As root is hidden, we have only file and line items on tree.
		if ( m_pTreeLog->GetItemParent(eventItemId) == rootItemId )
		{
			fileItemId = eventItemId;
			wxTreeItemIdValue cookie;
			lineItemId = m_pTreeLog->GetFirstChild(eventItemId, cookie);
			if ( lineItemId.IsOk() == false )
				break;
		}
		else
		{
			lineItemId = eventItemId;
			fileItemId = m_pTreeLog->GetItemParent(lineItemId);
		}

		// We extract line and file path values
		// Look at ThreadSearchLoggerTree::OnThreadSearchEvent to have format

		// Line:
		const wxString& lineText = m_pTreeLog->GetItemText(lineItemId);
		int columnPos = lineText.Find(':');
		if ( columnPos == wxNOT_FOUND )
			break;

		if ( lineText.Left(columnPos).ToLong(&line) == false )
			break;

		// File:
		const wxString& fileText = m_pTreeLog->GetItemText(fileItemId);
		int fileLength = fileText.Find(wxT(" ("));
		if ( fileLength == wxNOT_FOUND )
			break;

		int dirBegin   = fileLength + 2; // 2 for " ("
		int dirLength  = fileText.Length() - dirBegin - 1;
		if ( dirLength <= 0 )
			break;

		wxFileName filename(fileText.Mid(dirBegin, dirLength), fileText.Left(fileLength));
		filepath = filename.GetFullPath();

		success = true;
	} while ( false );

	return success;
}


void ThreadSearchLoggerTree::OnThreadSearchEvent(const ThreadSearchEvent& event)
{
	// A search event has been sent by the worker thread.
	// Tree log upddate
	const wxArrayString& words  = event.GetLineTextArray();
	const wxFileName&    filename(event.GetString());
	bool                 setFocus(false);
	wxTreeItemId         rootItemId(m_pTreeLog->GetRootItem());
	wxTreeItemId         fileItemId;
	long                 index    = m_IndexManager.GetInsertionIndex(filename.GetFullPath());
	long                 nb_items = m_pTreeLog->GetChildrenCount(rootItemId, false);

	wxASSERT(index != wxNOT_FOUND);
	wxASSERT((words.GetCount() % 2) == 0);

	// Use of Freeze Thaw to enhance speed and limit blink effect
	m_pTreeLog->Freeze();
	wxTreeItemId lineItemId;

	if ( index == nb_items )
	{
		fileItemId = m_pTreeLog->AppendItem(rootItemId,
													 wxString::Format(wxT("%s (%s)"),
																	  filename.GetFullName().c_str(),
																	  filename.GetPath().c_str()));
	}
	else
	{
		fileItemId = m_pTreeLog->InsertItem(rootItemId, index,
											wxString::Format(wxT("%s (%s)"),
															 filename.GetFullName().c_str(),
															 filename.GetPath().c_str()));
	}

	for (size_t i = 0; i < words.GetCount(); i += 2)
	{
		lineItemId = m_pTreeLog->AppendItem(fileItemId, wxString::Format(wxT("%s: %s"),
																		 words[i].c_str(),     // Line index starting from 1
																		 words[i+1].c_str())); // File line matching search expression

		// We update preview log for first list item
		if ( (m_FirstItemProcessed == false)                        &&
			 (m_pTreeLog->GetChildrenCount(fileItemId, false) == 1) &&
			 (m_pTreeLog->GetChildrenCount(rootItemId, false) == 1) )
		{
			// Expand first file item
			m_pTreeLog->Expand(fileItemId);

			// Select first item (generates selection event to update ThreadSearchView code preview)
			m_pTreeLog->SelectItem(lineItemId);

			// to avoid lots of preview that freezes app
			m_FirstItemProcessed = true;

			// It is useful to give focus to tree to navigate in results
			// just after running a search
			setFocus = true;
		}
	}
	m_pTreeLog->Thaw();

	if ( setFocus == true )
	{
		// On Linux, executing SetFocus just after SelectItem
		// does not work. Probbly because of Thaw...
		m_pTreeLog->SetFocus();
	}
}


void ThreadSearchLoggerTree::Clear()
{
	// As wxTreeCtrl::DeleteChildren sends lots of selected events
	// and because we don't want any wxTreeCtrl event to be processed
	// during delete, we disable events management during tree cleaning
	// (DisconnectEvents then ConnectEvents).
	wxWindow* pParent = m_pTreeLog->GetParent();
	if ( pParent == NULL ) return;

	DisconnectEvents(pParent);

    m_pTreeLog->DeleteChildren(m_pTreeLog->GetRootItem());
    m_FirstItemProcessed = false;

    m_IndexManager.Reset();

    ConnectEvents(pParent);
}
