/***************************************************************
 * Name:      ThreadSearchLoggerList
 * Purpose:   ThreadSearchLoggerList implements the
 *            ThreadSearchLoggerBase with a wxListCtrl.
 * Author:    Jerome ANTOINE
 * Created:   2007-07-28
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef THREAD_SEARCH_LOGGER_LIST_H
#define THREAD_SEARCH_LOGGER_LIST_H

#include <wx/event.h>

#include "ThreadSearchLoggerBase.h"

class wxPanel;
class wxWindow;
class wxListEvent;
class wxListCtrl;
class wxString;

class ThreadSearch;
class ThreadSearchView;
class ThreadSearchEvent;

class ThreadSearchLoggerList : public wxEvtHandler, public ThreadSearchLoggerBase
{
public:
	/** Constructor. */
	ThreadSearchLoggerList(ThreadSearchView& threadSearchView, ThreadSearch& threadSearchPlugin, wxPanel* pParent, long id);

	/** Destructor. */
	virtual ~ThreadSearchLoggerList();

	/** Getter */
	virtual eLoggerTypes GetLoggerType() {return TypeList;}

	/** Called by ThreadSearchView when new settings are applied. */
	virtual void Update();

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
	void OnLoggerListClick(wxListEvent& event);

	/** Double click event handler */
	void OnLoggerListDoubleClick(wxListEvent& event);

protected:
	/** SetListColumns
	  * The SimpleListLog constructor does not set the provided columns on Linux.
	  * It is necessary to set columns after the ThreadSearchLoggerList constructor.
	  */
	void SetListColumns();

	/** GetFileLineFromListEvent
	  * Return the file path at index from the list control using dir and file columns.
	  * @param event    : list control event
	  * @param filepath : reference that will receive the path
	  * @param line     : reference that will receive the line index
	  * @return true if successful.
	  */
	bool GetFileLineFromListEvent(wxListEvent& event, wxString& filepath, long &line);

	/** Dynamic events connection. */
	virtual void ConnectEvents(wxEvtHandler* pEvtHandler);

	/** Dynamic events disconnection. */
	virtual void DisconnectEvents(wxEvtHandler* pEvtHandler);

	wxListCtrl* m_pListLog;
};

#endif // THREAD_SEARCH_LOGGER_LIST_H
