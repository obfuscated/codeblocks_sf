/***************************************************************
 * Name:      ThreadSearchEvent
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

 #include "ThreadSearchEvent.h"

DEFINE_EVENT_TYPE(wxEVT_THREAD_SEARCH)
DEFINE_EVENT_TYPE(wxEVT_THREAD_SEARCH_ERROR)
IMPLEMENT_DYNAMIC_CLASS(ThreadSearchEvent, wxCommandEvent)

ThreadSearchEvent::ThreadSearchEvent(wxEventType commandType, int id)
                     :wxCommandEvent(commandType, id)
{
}


ThreadSearchEvent::ThreadSearchEvent(const ThreadSearchEvent& Event)
                  :wxCommandEvent(Event)
{
    // code copied from class CodeBlocksThreadEvent in SDK
    // make sure our string member (which uses COW, aka refcounting) is not
    // shared by other wxString instances:
    SetString(GetString().c_str());

    // clone the wxArrayString, since wxArrayString internally use wxString,
    // and wxString does not do a deep copy in copy constructor
    int count = Event.m_LineTextArray.GetCount();
    for (int i = 0; i < count; ++i)
        m_LineTextArray.Add(Event.m_LineTextArray[i].c_str());
}


ThreadSearchEvent::~ThreadSearchEvent()
{
}
