/***************************************************************
 * Name:      ThreadSearchLoggerTree
 * Purpose:   ThreadSearchLoggerTree implements the
 *            ThreadSearchLoggerBase interface with a wxTreeCtrl.
 * Author:    Jerome ANTOINE
 * Created:   2007-07-28
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef THREAD_SEARCH_LOGGER_TREE_H
#define THREAD_SEARCH_LOGGER_TREE_H

#include <wx/event.h>

#include "ThreadSearchLoggerBase.h"

class wxString;
class wxWindow;
class wxPanel;
class wxTreeCtrl;
class wxTreeEvent;

class ThreadSearch;
class ThreadSearchView;
class ThreadSearchEvent;


// MessageLog inheritance is necessary to be able to
// add logger to Messages notebook.
class ThreadSearchLoggerTree : public wxEvtHandler, public ThreadSearchLoggerBase
{
public:
	/** Constructor. */
	ThreadSearchLoggerTree(ThreadSearchView& threadSearchView, ThreadSearch& threadSearchPlugin,
							InsertIndexManager::eFileSorting fileSorting, wxPanel* pParent, long id);

	/** Destructor. */
	virtual ~ThreadSearchLoggerTree();

	/** Getter */
	virtual eLoggerTypes GetLoggerType() {return TypeTree;}

	/** Called by ThreadSearchView to process a ThreadSearchEvent
	  * sent by worker thread.
	  */
	virtual void OnThreadSearchEvent(const ThreadSearchEvent& event);

	/** Removes all items from logger. */
	virtual void Clear();

	/** Returns the logger window. */
	virtual wxWindow* GetWindow();

	/** Sets focus on list window. */
	virtual void SetFocus();

	/** Single click event handler */
    void OnLoggerTreeClick(wxTreeEvent& event);

	/** Double click event handler */
    void OnLoggerTreeDoubleClick(wxTreeEvent& event);

protected:
	/** GetFileLineFromTreeEvent
	  * Return the file path at index from the list control using dir and file columns.
	  * @param event    : list control event
	  * @param filepath : reference that will receive the path
	  * @param line     : reference that will receive the line index
	  * @return true if successful.
	  */
	bool GetFileLineFromTreeEvent(wxTreeEvent& event, wxString& filepath, long &line);

	/** Dynamic events connection. */
	virtual void ConnectEvents(wxEvtHandler* pEvtHandler);

	/** Dynamic events disconnection. */
	virtual void DisconnectEvents(wxEvtHandler* pEvtHandler);

	wxTreeCtrl* m_pTreeLog;
	bool        m_FirstItemProcessed; // Used to filter wxTree events and process useful ones only.
};

#endif // THREAD_SEARCH_LOGGER_TREE_H
