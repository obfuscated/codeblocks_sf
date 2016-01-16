/***************************************************************
 * Name:      ThreadSearchLoggerList
 * Purpose:   ThreadSearchLoggerTree implements the
 *            ThreadSearchLoggerBase interface with a wxListCtrl.
 * Author:    Jerome ANTOINE
 * Created:   2007-07-28
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#if defined(CB_PRECOMP)
#include "sdk.h"
#endif
//#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
	// Required extra includes
	#include "configmanager.h"
#endif

#include <wx/listctrl.h>
#include <wx/dynarray.h>

#include "ThreadSearch.h"
#include "ThreadSearchView.h"
#include "ThreadSearchEvent.h"
#include "ThreadSearchLoggerList.h"
#include "ThreadSearchControlIds.h"
#include "snippetsconfig.h"


ThreadSearchLoggerList::ThreadSearchLoggerList(ThreadSearchView& threadSearchView,
											   ThreadSearch& threadSearchPlugin,
											   InsertIndexManager::eFileSorting fileSorting,
											   wxPanel* pParent,
											   long id)
					   : ThreadSearchLoggerBase(threadSearchView, threadSearchPlugin, fileSorting)
{
	m_pListLog = new wxListCtrl(pParent, id, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER);
	m_pListLog->SetMinSize(wxSize(100,100));

	SetListColumns();

	// Events are managed dynamically to be able to stop/start management when required.
	ConnectEvents(pParent);
}


// ----------------------------------------------------------------------------
ThreadSearchLoggerList::~ThreadSearchLoggerList()
// ----------------------------------------------------------------------------
{
	wxWindow* pParent = m_pListLog->GetParent();
	if ( pParent != NULL )
	{
		// Events are managed dynamically to be able to stop/start management when required.
		DisconnectEvents(pParent);
	}

	// Memorize colume widths for next usage //(pecan 2008/5/22)
    ConfigManager* pCfg = Manager::Get()->GetConfigManager(_T("SnippetsSearch"));
    pCfg->Write(wxT("/LogColSizeDir"),  m_pListLog->GetColumnWidth(0));
    pCfg->Write(wxT("/LogColSizeFile"), m_pListLog->GetColumnWidth(1));
    pCfg->Write(wxT("/LogColSizeLine"), m_pListLog->GetColumnWidth(2));
    pCfg->Write(wxT("/LogColSizeText"), m_pListLog->GetColumnWidth(3));

	m_pListLog->Destroy();
}


void ThreadSearchLoggerList::Update()
{
	ThreadSearchLoggerBase::Update();

	// Adds/Removes listview header and adds/removes vertical rules
	m_pListLog->SetSingleStyle(wxLC_NO_HEADER, !m_ThreadSearchPlugin.GetDisplayLogHeaders());
	m_pListLog->SetSingleStyle(wxLC_VRULES,     m_ThreadSearchPlugin.GetDrawLogLines());
	SetListColumns();
}
// ----------------------------------------------------------------------------
void ThreadSearchLoggerList::SetListColumns()
// ----------------------------------------------------------------------------
{
	m_pListLog->ClearAll();

    // Size columes set by last user usage //(pecan 2008/5/22)
    ConfigManager* pCfg = Manager::Get()->GetConfigManager(_T("SnippetsSearch"));
    int directoryColSize  = pCfg->ReadInt(wxT("/LogColSizeDir"), 100);
    int fileColSize  = pCfg->ReadInt(wxT("/LogColSizeFile"), 100);
    int lineColSize  = pCfg->ReadInt(wxT("/LogColSizeLine"),  50);
    int textColSize  = pCfg->ReadInt(wxT("/LogColSizeText"), 500);

	m_pListLog->InsertColumn(0, wxT("Directory"), wxLIST_FORMAT_LEFT, directoryColSize);
	m_pListLog->InsertColumn(1, wxT("File"),      wxLIST_FORMAT_LEFT, fileColSize);
	m_pListLog->InsertColumn(2, wxT("Line"),      wxLIST_FORMAT_RIGHT, lineColSize);
	m_pListLog->InsertColumn(3, wxT("Text"),      wxLIST_FORMAT_LEFT, textColSize);
}
// ----------------------------------------------------------------------------
void ThreadSearchLoggerList::OnMouseWheelEvent(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    // Ctrl-MouseWheel rotation changes treeCtrl font
    //-wxWindow* pParent = m_pListLog->GetParent();
    //wxWindow* pParent = (wxWindow*)event.GetEventObject();
    wxWindow* pParent = m_pListLog;
	if ( not pParent ) return;

    bool mouseCtrlKeyDown = event.ControlDown();
    #ifdef LOGGING
     //LOGIT(wxT("treeCtrl:OnMouseWheel[%s]"), m_MouseCtrlKeyDown?wxT("Down"):wxT("UP") );
    #endif
    if (not mouseCtrlKeyDown) {event.Skip(); return;}

    int nRotation = event.GetWheelRotation();
    wxFont ctrlFont = pParent->GetFont();

    if ( nRotation > 0)
        ctrlFont.SetPointSize( ctrlFont.GetPointSize()-1);
    else
        ctrlFont.SetPointSize( ctrlFont.GetPointSize()+1);

    pParent->SetFont(ctrlFont);
    pParent->Refresh();
    pParent->Update();
    return;
}
// ----------------------------------------------------------------------------
void ThreadSearchLoggerList::OnLoggerListClick(wxListEvent& event)
// ----------------------------------------------------------------------------
{
	// Manages list log left (single) click
    // Gets file path and line from list control

    //Ignore the right mouse key //(pecan 2008/4/25)
#if wxCHECK_VERSION(2,9,0)
    if ( ::wxGetMouseState().RightIsDown() )
#else
    if ( ::wxGetMouseState().RightDown() )
#endif
    {
        // Ignore RightMouseSelect, put back last LeftMouseSelect
        wxListCtrl* pListCtrl = (wxListCtrl*)event.GetEventObject();
        pListCtrl->SetItemState(m_LastLeftMouseClickIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        event.Skip(); return;
    }

	wxString filepath(wxEmptyString);
	long line;
	if ( GetFileLineFromListEvent(event, filepath, line) == false )
	{
		cbMessageBox(wxT("Failed to retrieve file path and line number"), wxT("Error"), wxICON_ERROR);
		return;
	}
	m_ThreadSearchView.OnLoggerClick(filepath, line);
	m_LastLeftMouseClickIndex = event.GetIndex();
	event.Skip();
}
// ----------------------------------------------------------------------------
void ThreadSearchLoggerList::OnLoggerListDoubleClick(wxListEvent& event)
// ----------------------------------------------------------------------------
{
	// Manages list log left double click
    // Gets file path and line from list control
    wxString filepath(wxEmptyString);
    long line;
    if ( GetFileLineFromListEvent(event, filepath, line) == false )
    {
		cbMessageBox(wxT("Failed to retrieve file path and line number"), wxT("Error"), wxICON_ERROR);
    	return;
    }
    m_ThreadSearchView.OnLoggerDoubleClick(filepath, line);
    event.Skip();
}


bool ThreadSearchLoggerList::GetFileLineFromListEvent(wxListEvent& /*event*/, wxString& filepath, long &line)
{
	bool success = false;
	wxListItem listItem;

	do {
		// Finds selected item index
		long index = m_pListLog->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
		if ( index == -1 ) break;

		// First, gets file dir
		wxString filedir;
		listItem.m_itemId = index;
		listItem.m_col    = 0;
		listItem.m_mask   = wxLIST_MASK_TEXT;

		if ( m_pListLog->GetItem(listItem) == false ) break;

		filedir = listItem.GetText();

		// Then build complete filename from dir and file
		listItem.m_col = 1;
		if ( m_pListLog->GetItem(listItem) == false ) break;

		wxFileName sFile(filedir, listItem.GetText());
		filepath = sFile.GetFullPath();

		// Then gets line number
		listItem.m_col = 2;
		if ( m_pListLog->GetItem(listItem) == false ) break;

		if ( listItem.GetText().ToLong(&line) == true )
		{
			success = true;
		}
	} while ( 0 );

	return success;
}
// ----------------------------------------------------------------------------
void ThreadSearchLoggerList::OnThreadSearchEvent(const sThreadSearchEvent& event)
// ----------------------------------------------------------------------------
{
	// A search event has been sent by the worker thread.
	// List log upddate
	const wxArrayString& words  = event.GetLineTextArray();
	const wxFileName&    filename(event.GetString());
	bool                 setFocus(false);
	long                 setFocusItem;
	wxString             setFocusDir;
	wxString             setFocusFilename;

	wxASSERT((words.GetCount() % 2) == 0);

	// Use of Freeze Thaw to enhance speed and limit blink effect
	m_pListLog->Freeze();
	long index = m_IndexManager.GetInsertionIndex(filename.GetFullPath(), words.GetCount()/2);
	for (size_t i = 0; i < words.GetCount(); i += 2)
	{
		m_pListLog->InsertItem(index, filename.GetPath());     // Directory
		m_pListLog->SetItem(index, 1, filename.GetFullName()); // File name
		m_pListLog->SetItem(index, 2, words[i]);               // Line index starting from 1
		m_pListLog->SetItem(index, 3, words[i+1]);             // File line matching search expression

		//(pecan 2008/7/31) Dont do the following here or Ctrl-MouseWheel cannot change the font size
		//-m_pListLog->SetItemFont(index, m_ThreadSearchPlugin.m_Conf_font);          //(pecan 2008/3/06)

		// We update preview log for first list item
		if ( m_pListLog->GetItemCount() == 1 )
		{
			// Gets line index
			long line = 0;
			if ( words[i].ToLong(&line) == false )
			{
				cbMessageBox(wxT("Failed to convert line number from %s") + words[i], wxT("Error"), wxICON_ERROR);
			}
			else
			{
				m_ThreadSearchView.UpdatePreview(filename.GetFullPath(), line);

				// It is useful to give focus to list to navigate in results
				// just after running a search
				setFocus = true;
				setFocusItem = index;
				setFocusDir = filename.GetPath();
				setFocusFilename = filename.GetFullName();
			}
		}
		index++;
	}
	m_pListLog->Thaw();
	// Note: (pecan 2008/4/27)
	// The wxListCtrl is not redrawn at this point. It's blank.
	// This routine is driven from a timer interrupt. So the listCtrl
	// will problably not update until events are exhausted.
	// Doing a Yield() just causes crashes in event Processing.
	//Manager::Yield(); <-- causes crash in queued event processing.
	//See: SyncLoggerToPreview() called from OnTmrListCtrlUpdate

	if ( setFocus == true )
	{
        // Select item so update view can find it later              //(pecan 2008/4/27)
        m_pListLog->SetItemState(setFocusItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

		// On Linux, executing SetFocus just after UpdatePreview(0)
		// does not work. Probbly because of Thaw...
		m_pListLog->SetFocus();
	}
}


wxWindow* ThreadSearchLoggerList::GetWindow()
{
	return m_pListLog;
}


// ----------------------------------------------------------------------------
void ThreadSearchLoggerList::SetFocus()
// ----------------------------------------------------------------------------
{
	m_pListLog->SetFocus();
	SyncLoggerToPreview();
}
// ----------------------------------------------------------------------------
void ThreadSearchLoggerList::ConnectEvents(wxEvtHandler* pEvtHandler)
// ----------------------------------------------------------------------------
{
	// Dynamic event connections.
	int id = m_pListLog->GetId();
	pEvtHandler->Connect(id, wxEVT_COMMAND_LIST_ITEM_SELECTED,
						(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
						&ThreadSearchLoggerList::OnLoggerListClick, NULL, static_cast<wxEvtHandler*>(this));

	pEvtHandler->Connect(id, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
						(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
						&ThreadSearchLoggerList::OnLoggerListDoubleClick, NULL, static_cast<wxEvtHandler*>(this));

	m_pListLog->Connect(id, wxEVT_MOUSEWHEEL, //(pecan 2008/7/31)
						(wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
						&ThreadSearchLoggerList::OnMouseWheelEvent, NULL, static_cast<wxEvtHandler*>(this));
}


void ThreadSearchLoggerList::DisconnectEvents(wxEvtHandler* pEvtHandler)
{
	// Dynamic event Disconnections.
	int id = m_pListLog->GetId();
    pEvtHandler->Disconnect(id, wxEVT_COMMAND_LIST_ITEM_SELECTED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &ThreadSearchLoggerList::OnLoggerListClick, NULL, static_cast<wxEvtHandler*>(this));

    pEvtHandler->Disconnect(id, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &ThreadSearchLoggerList::OnLoggerListDoubleClick, NULL, static_cast<wxEvtHandler*>(this));

	m_pListLog->Disconnect(id, wxEVT_MOUSEWHEEL, //(pecan 2008/7/31)
						(wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
						&ThreadSearchLoggerList::OnMouseWheelEvent, NULL, static_cast<wxEvtHandler*>(this));

}


void ThreadSearchLoggerList::Clear()
{
	m_pListLog->DeleteAllItems();
    m_IndexManager.Reset();
}
// ----------------------------------------------------------------------------
void ThreadSearchLoggerList::SyncLoggerToPreview()
// ----------------------------------------------------------------------------
{   //(pecan 2008/4/27)
    // We need to position the Logger window to the selection
    // in Preview cbEditor control.
    // This routine must be called *after* the thread events have been
    // exhausted (in the timer event) and the window has already been redrawn.

    wxListCtrl* pListCtrl = (wxListCtrl*)GetWindow();
    long setFocusItem = pListCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (setFocusItem == -1)    //no selecction
        return;

    m_LastLeftMouseClickIndex = setFocusItem;
    pListCtrl->EnsureVisible(setFocusItem);
    pListCtrl->SetFocus();
}
