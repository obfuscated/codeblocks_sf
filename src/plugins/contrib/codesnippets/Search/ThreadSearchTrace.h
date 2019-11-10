/***************************************************************
 * Name:      ThreadSearchTrace
 *
 * Purpose:   This class implements the events sent by the
 *            worker search thread (ThreadSearchThread) to the
 *            ThreadSearchView to update the file/line/line
 *            list control.
 *            wxCommandEvent m_commandString contains file path.
 *            m_LineTextArray contains Line/FoundText series of items
 *
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/
#ifndef THREAD_SEARCH_TRACE_H
#define THREAD_SEARCH_TRACE_H

#include <wx/string.h>
#include <wx/file.h>
#include <wx/thread.h>

class ThreadSearchTrace : protected wxFile, protected wxMutex
{
public:
	/** Destructor. */
	~ThreadSearchTrace() {};

	static bool Init(const wxString& path);
	static void Uninit();
	static bool Trace(const wxString& str);

private:
	/** Constructor. */
	ThreadSearchTrace() {};

	static ThreadSearchTrace* ms_Tracer;
};


class TraceBeginEndOfMethod
{
public:
	TraceBeginEndOfMethod(const wxString& method);

	~TraceBeginEndOfMethod();

private:
	wxString m_Method;
};

#endif // THREAD_SEARCH_TRACE_H

