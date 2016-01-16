/***************************************************************
 * Name:      DragScrollEvent
 *
 * Purpose:   This class implements the events sent by/for a
 *            DragScroll request to the
 *            DragScroll plugin to request services such as
 *            start/end scroll monitoring of an open window.
 *            wxCommandEvent m_id contains a window id.
 *
 * Author:    Pecan
 * Created:   2008/4/01
 * Copyright: Pecan
 * License:   GPL
 **************************************************************/
#ifndef DRAGSCROLL_EVENT_H
#define DRAGSCROLL_EVENT_H

#include <wx/event.h>
class cbPlugin;

	enum {
	    idDragScrollAddWindow = 1,
	    idDragScrollRemoveWindow,
	    idDragScrollRescan,
	    idDragScrollReadConfig,
	    idDragScrollInvokeConfig
    };
// ----------------------------------------------------------------------------
class DragScrollEvent : public wxCommandEvent
// ----------------------------------------------------------------------------
{
public:
	/** Constructor. */
	DragScrollEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

	/** Copy constructor. */
	DragScrollEvent( const DragScrollEvent& event);

	/** Destructor. */
	~DragScrollEvent();

	virtual wxEvent* Clone() const { return new DragScrollEvent(*this);}

	DECLARE_DYNAMIC_CLASS(DragScrollEvent);

	wxString  GetEventTypeLabel() const {return m_EventTypeLabel;}

    bool      PostDragScrollEvent(const cbPlugin* targetWin);
    bool      ProcessDragScrollEvent(const cbPlugin* targetWin);

private:
	//-int        m_WindowID;
	//-wxWindow*  m_pWindow;
	wxString   m_EventTypeLabel;
};

typedef void (wxEvtHandler::*DragScrollEventFunction)(DragScrollEvent&);


extern const wxEventType wxEVT_DRAGSCROLL_EVENT;
#define EVT_DRAGSCROLL_EVENT(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_DRAGSCROLL_EVENT, id, -1, \
	(wxObjectEventFunction)(wxEventFunction) (DragScrollEventFunction) & fn,(wxObject *) NULL ),

#endif // DRAGSCROLL_EVENT_H


