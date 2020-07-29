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
#include <vector>

class ThreadSearchEvent : public wxCommandEvent
{
public:
    /** Constructor. */
    ThreadSearchEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

    /** Copy constructor. */
    ThreadSearchEvent(const ThreadSearchEvent& event);

    /** Destructor. */
    ~ThreadSearchEvent() override;

    wxEvent *Clone() const override { return new ThreadSearchEvent(*this); }

    DECLARE_DYNAMIC_CLASS(ThreadSearchEvent);

    // Contains a series of string containing line index (starting from 1), matching line
    const wxArrayString& GetLineTextArray() const { return m_LineTextArray; }
    void SetLineTextArray(const wxArrayString& ArrayString) { m_LineTextArray = ArrayString; }

    void SetMatchedPositions(std::vector<int> &matchedPositions)
    {
        m_MatchedPositions.swap(matchedPositions);
    }
    const std::vector<int>& GetMatchedPositions() const {
        return m_MatchedPositions;
    }

    size_t GetNumberOfMatches() const { return m_LineTextArray.GetCount() / 2; }

private:
    wxArrayString m_LineTextArray;
    /// Stores info for the positions of the matches.
    /// The format of the matches for a single line is as follows:
    /// ||count of matches|start match 0|length match0|...|start match N|length match N||
    /// The vector would contain a single such record for every matching line.
    /// Storing a count of 0 for a matching line is a valid behaviour for a searcher.
    std::vector<int> m_MatchedPositions;
};

typedef void (wxEvtHandler::*ThreadSearchEventFunction)(ThreadSearchEvent&);

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXEXPORT, wxEVT_THREAD_SEARCH, wxID_ANY)
    DECLARE_EXPORTED_EVENT_TYPE(WXEXPORT, wxEVT_THREAD_SEARCH_ERROR, wxID_ANY)
END_DECLARE_EVENT_TYPES()

#define EVT_THREAD_SEARCH(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY(wxEVT_THREAD_SEARCH, id, -1, \
    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) (ThreadSearchEventFunction) & fn,(wxObject *) NULL ),

#define EVT_THREAD_SEARCH_ERROR(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY(wxEVT_THREAD_SEARCH_ERROR, id, -1, \
    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) (ThreadSearchEventFunction) & fn,(wxObject *) NULL ),

#endif // THREAD_SEARCH_EVENT_H


