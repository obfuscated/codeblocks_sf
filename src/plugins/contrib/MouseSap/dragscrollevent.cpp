/***************************************************************
 * Name:      DragScrollEvent
 *
 * Purpose:   This class implements the events sent by/for a
 *            DragScroll request to the
 *            DragScroll plugin for services such as
 *            add/remove scrollable windows.
 *            wxCommandEvent m_id contains a window id.
 *
 * Created:   2008/04/23
 * Copyright: Pecan
 * License:   GPL
 **************************************************************/

 #include "DragScrollEvent.h"
 #include "manager.h"
 #include "pluginmanager.h"

DEFINE_EVENT_TYPE(wxEVT_DRAGSCROLL_ADD_WINDOW)
DEFINE_EVENT_TYPE(wxEVT_DRAGSCROLL_REMOVE_WINDOW)
DEFINE_EVENT_TYPE(wxEVT_DRAGSCROLL_RESCAN)
IMPLEMENT_DYNAMIC_CLASS(DragScrollEvent, wxCommandEvent)

// ----------------------------------------------------------------------------
DragScrollEvent::DragScrollEvent(wxEventType commandType, int id)
// ----------------------------------------------------------------------------
					 :wxCommandEvent(commandType, id)
{
    //ctor
    //-m_propagationLevel = wxEVENT_PROPAGATE_MAX ;
    m_WindowID = id;
    m_pWindow = 0;
    m_EventTypeLabel = _T("UNKOWN");
    if ( wxEVT_DRAGSCROLL_ADD_WINDOW == commandType)
        m_EventTypeLabel = _T("wxEVT_DRAGSCROLL_ADD_WINDOW");
    if ( wxEVT_DRAGSCROLL_REMOVE_WINDOW == commandType)
        m_EventTypeLabel = _T("wxEVT_DRAGSCROLL_REMOVE_WINDOW");
    if ( wxEVT_DRAGSCROLL_RESCAN == commandType)
        m_EventTypeLabel = _T("wxEVT_DRAGSCROLL_RESCAN");
}
// -- clone -------------------------------------------------------------------
DragScrollEvent::DragScrollEvent( const DragScrollEvent& Event)
// ----------------------------------------------------------------------------
				  :wxCommandEvent(Event)
				  ,m_WindowID(0)
				  ,m_pWindow(0)
{
    // This is invoked by DragScrollEvent::Clone() from AddPendingEvent etc
	m_WindowID = Event.GetWindowID();
	m_pWindow = Event.GetWindow();
	m_EventTypeLabel = Event.GetEventTypeLabel();
}
// ----------------------------------------------------------------------------
DragScrollEvent::~DragScrollEvent()
// ----------------------------------------------------------------------------
{
    //dtor
}
// ----------------------------------------------------------------------------
bool DragScrollEvent::PostDragScrollEvent(const wxEvtHandler* targetWin)
// ----------------------------------------------------------------------------
{
    // Propagate DragScrollEvent to ThreadSearchFrame

    #if defined(LOGGING)
        int windowID = GetWindowID();
        wxWindow* scrollWindow = GetWindow();
        wxString eventTypeLabel = GetEventTypeLabel();
        LOGIT( _T("PostDragScrollEvent type[%s]id[%d]str[%s]"), eventTypeLabel.c_str(), windowID, scrollWindow->GetName().c_str());
    #endif

    wxEvtHandler* pPlgn = (wxEvtHandler*)targetWin;
    if ( not targetWin )
        pPlgn = (wxEvtHandler*)Manager::Get()->GetPluginManager()->FindPluginByName(_T("cbDragScroll"));
    //cbPlugin* pPlgn = GetConfig()->GetDragScrollPlugin();

    // Propagate the event to DragScroll plugin
    if ( pPlgn )
    {
        pPlgn->AddPendingEvent( (wxEvent&)*this );
    }
    else
    {
        #if defined(LOGGING)
        LOGIT( _T("PostDragScrollEvent[%s]"), _T("Failed"));
        #endif
        return false;
    }

    return true;

}//PostDragScrollEvent
// ----------------------------------------------------------------------------
bool DragScrollEvent::ProcessDragScrollEvent(const wxEvtHandler* targetWin)
// ----------------------------------------------------------------------------
{
    // Propagate DragScrollEvent

    #if defined(LOGGING)
        int windowID = GetWindowID();
        wxWindow* scrollWindow = GetWindow();
        wxString eventTypeLabel = GetEventTypeLabel();
        LOGIT( _T("ProcessDragScrollEvent type[%s]id[%d]str[%s]"), eventTypeLabel.c_str(), windowID, scrollWindow->GetName().c_str());
    #endif

    wxEvtHandler* pPlgn = (wxEvtHandler*)targetWin;
    if ( not targetWin )
        pPlgn = (wxEvtHandler*)Manager::Get()->GetPluginManager()->FindPluginByName(_T("cbDragScroll"));

    // Propagate the event to DragScroll plugin
    if ( pPlgn )
    {
        pPlgn->ProcessEvent( (wxEvent&)*this );
    }
    else
    {
        #if defined(LOGGING)
        LOGIT( _T("ProcessDragScrollEvent[%s]"), _T("Failed"));
        #endif
        return false;
    }

    return true;

}//PostDragScrollEvent
