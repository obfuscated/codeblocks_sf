/***************************************************************
 * Name:      sDragScrollEvent
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
class sDragScrollEvent : public wxCommandEvent
// ----------------------------------------------------------------------------
{
public:
	/** Constructor. */
	sDragScrollEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

	/** Copy constructor. */
	sDragScrollEvent( const sDragScrollEvent& event);

	/** Destructor. */
	~sDragScrollEvent();


	virtual wxEvent* Clone() const { return new sDragScrollEvent(*this);}


	//-int       GetWindowID() const {return m_WindowID;}
	//-void      SetWindowID( const int itemid ) {m_WindowID = itemid;}
	//-wxWindow* GetWindow() const {return m_pWindow;}
	//-void      SetWindow(  wxWindow* p ) {m_pWindow = p;}
	wxString  GetEventTypeLabel() const {return m_EventTypeLabel;}

    bool      PostDragScrollEvent(const cbPlugin* targetWin);
    bool      ProcessDragScrollEvent(const cbPlugin* targetWin);

private:
	//-int        m_WindowID;
	//-wxWindow*  m_pWindow;
	wxString   m_EventTypeLabel;

	DECLARE_DYNAMIC_CLASS(sDragScrollEvent);
};

typedef void (wxEvtHandler::*sDragScrollEventFunction)(sDragScrollEvent&);

extern const wxEventType wxEVT_S_DRAGSCROLL_EVENT;
#define EVT_S_DRAGSCROLL_EVENT(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_S_DRAGSCROLL_EVENT, id, -1, \
	(wxObjectEventFunction)(wxEventFunction) (sDragScrollEventFunction) & fn,(wxObject *) NULL ),

#endif // DRAGSCROLL_EVENT_H


