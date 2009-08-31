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

	int       GetWindowID() const {return m_WindowID;}
	void      SetWindowID( const int itemid ) {m_WindowID = itemid;}
	wxWindow* GetWindow() const {return m_pWindow;}
	void      SetWindow(  wxWindow* p ) {m_pWindow = p;}
	wxString  GetEventTypeLabel() const {return m_EventTypeLabel;}

    bool      PostDragScrollEvent(const wxEvtHandler* targetWin);
    bool      ProcessDragScrollEvent(const wxEvtHandler* targetWin);

private:
	int        m_WindowID;
	wxWindow*  m_pWindow;
	wxString   m_EventTypeLabel;
};

typedef void (wxEvtHandler::*DragScrollEventFunction)(DragScrollEvent&);

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EXPORTED_EVENT_TYPE(WXEXPORT, wxEVT_DRAGSCROLL_ADD_WINDOW, wxID_ANY)
	DECLARE_EXPORTED_EVENT_TYPE(WXEXPORT, wxEVT_DRAGSCROLL_REMOVE_WINDOW, wxID_ANY)
	DECLARE_EXPORTED_EVENT_TYPE(WXEXPORT, wxEVT_DRAGSCROLL_RESCAN, wxID_ANY)
END_DECLARE_EVENT_TYPES()

#define EVT_DRAGSCROLL_ADD_WINDOW(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_DRAGSCROLL_ADD_WINDOW, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) (DragScrollEventFunction) & fn,(wxObject *) NULL ),

#define EVT_DRAGSCROLL_REMOVE_WINDOW(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_DRAGSCROLL_REMOVE_WINDOW, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) (DragScrollEventFunction) & fn,(wxObject *) NULL ),

#define EVT_DRAGSCROLL_RESCAN(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_DRAGSCROLL_RESCAN, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) (DragScrollEventFunction) & fn,(wxObject *) NULL ),

#endif // DRAGSCROLL_EVENT_H


