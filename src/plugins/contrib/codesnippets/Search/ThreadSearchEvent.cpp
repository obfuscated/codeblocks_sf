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

DEFINE_EVENT_TYPE(wxEVT_S_THREAD_SEARCH)
DEFINE_EVENT_TYPE(wxEVT_S_THREAD_SEARCH_ERROR)
DEFINE_EVENT_TYPE(wxEVT_S_THREAD_SEARCH_SHOWMSG)
IMPLEMENT_DYNAMIC_CLASS(sThreadSearchEvent, wxCommandEvent)

sThreadSearchEvent::sThreadSearchEvent(wxEventType commandType, int id)
					 :wxCommandEvent(commandType, id)
{
}


sThreadSearchEvent::sThreadSearchEvent(const sThreadSearchEvent& Event)
				  :wxCommandEvent(Event)
{
	m_LineTextArray = Event.GetLineTextArray();
}


sThreadSearchEvent::~sThreadSearchEvent()
{
}
