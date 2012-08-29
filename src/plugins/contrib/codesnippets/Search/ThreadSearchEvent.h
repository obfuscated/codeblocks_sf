/***************************************************************
 * Name:      ThreadSearchEvent
 *
 * Purpose:   This class implements the events sent by the
 *            worker search thread (ThreadSearchThread) to the
 *            ThreadSearchView to update the logger window with
 *            file/line/text.
 *            wxCommandEvent m_commandString contains file path.
 *            m_LineTextArray contains Line/FoundText series of items
 *
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/
#ifndef THREAD_SEARCH_EVENT_H
#define THREAD_SEARCH_EVENT_H

#include <wx/event.h>
#include <wx/arrstr.h>

class sThreadSearchEvent : public wxCommandEvent
{
public:
	/** Constructor. */
	sThreadSearchEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

	/** Copy constructor. */
	sThreadSearchEvent(const sThreadSearchEvent& event);

	/** Destructor. */
	~sThreadSearchEvent();

	virtual wxEvent *Clone() const {return new sThreadSearchEvent(*this);}

	DECLARE_DYNAMIC_CLASS(sThreadSearchEvent);

	wxArrayString GetLineTextArray() const {return m_LineTextArray;};   // Contains a series of string containing
																		// line index (starting from 1), matching line
	void SetLineTextArray(const wxArrayString& ArrayString) {m_LineTextArray = ArrayString;};

private:
	wxArrayString m_LineTextArray;
};

typedef void (wxEvtHandler::*sThreadSearchEventFunction)(sThreadSearchEvent&);

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EXPORTED_EVENT_TYPE(WXEXPORT, wxEVT_S_THREAD_SEARCH, wxID_ANY)
	DECLARE_EXPORTED_EVENT_TYPE(WXEXPORT, wxEVT_S_THREAD_SEARCH_ERROR, wxID_ANY)
	DECLARE_EXPORTED_EVENT_TYPE(WXEXPORT, wxEVT_S_THREAD_SEARCH_SHOWMSG, wxID_ANY)
END_DECLARE_EVENT_TYPES()

#define EVT_S_THREAD_SEARCH(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_S_THREAD_SEARCH, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) (sThreadSearchEventFunction) & fn,(wxObject *) NULL ),

#define EVT_S_THREAD_SEARCH_ERROR(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_S_THREAD_SEARCH_ERROR, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) (sThreadSearchEventFunction) & fn,(wxObject *) NULL ),

#define EVT_S_THREAD_SEARCH_SHOWMSG(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_S_THREAD_SEARCH_SHOWMSG, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) (sThreadSearchEventFunction) & fn,(wxObject *) NULL ),

#endif // THREAD_SEARCH_EVENT_H


