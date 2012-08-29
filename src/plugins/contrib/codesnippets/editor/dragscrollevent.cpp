/***************************************************************
 * Name:      sDragScrollEvent
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

 #include "dragscrollevent.h"
 #include "dragscroll.h"
 #include "version.h"

IMPLEMENT_DYNAMIC_CLASS(sDragScrollEvent, wxCommandEvent)

const wxEventType wxEVT_S_DRAGSCROLL_EVENT = wxNewEventType();

// ----------------------------------------------------------------------------
sDragScrollEvent::sDragScrollEvent(wxEventType commandType, int id)
// ----------------------------------------------------------------------------
					 :wxCommandEvent(commandType, id)
{
    //ctor
    //-m_propagationLevel = wxEVENT_PROPAGATE_MAX ;
    //-m_WindowID = id;
    //-m_pWindow = 0;
    m_EventTypeLabel = _T("UNKOWN");
    if ( idDragScrollAddWindow == id)
        m_EventTypeLabel = _T("EVT_S_DRAGSCROLL_ADD_WINDOW");
    if ( idDragScrollRemoveWindow == id)
        m_EventTypeLabel = _T("EVT_S_DRAGSCROLL_REMOVE_WINDOW");
    if ( idDragScrollRescan == id)
        m_EventTypeLabel = _T("EVT_S_DRAGSCROLL_RESCAN");
    if ( idDragScrollReadConfig == id)
        m_EventTypeLabel = _T("EVT_S_DRAGSCROLL_READ_CONFIG");
    if ( idDragScrollInvokeConfig == id)
        m_EventTypeLabel = _T("EVT_S_DRAGSCROLL_INVOKE_CONFIG");
}
// -- clone -------------------------------------------------------------------
sDragScrollEvent::sDragScrollEvent( const sDragScrollEvent& Event)
// ----------------------------------------------------------------------------
				  :wxCommandEvent(Event)
				  //-,m_WindowID(0)
				  //-,m_pWindow(0)
{
    // This is invoked by sDragScrollEvent::Clone() from AddPendingEvent etc
	//-m_WindowID = Event.GetWindowID();
	//-m_pWindow = Event.GetWindow();
	m_EventTypeLabel = Event.GetEventTypeLabel();
}
// ----------------------------------------------------------------------------
sDragScrollEvent::~sDragScrollEvent()
// ----------------------------------------------------------------------------
{
    //dtor
}
// ----------------------------------------------------------------------------
bool sDragScrollEvent::PostDragScrollEvent(const cbPlugin* targetWin)
// ----------------------------------------------------------------------------
{
    // Propagate sDragScrollEvent to ThreadSearchFrame

    #if defined(LOGGING)
        //-int windowID = GetWindowID();
        //-wxWindow* scrollWindow = GetWindow();
        wxString eventTypeLabel = GetEventTypeLabel();
        //-LOGIT( _T("PostDragScrollEvent type[%s]id[%d]str[%s]"), eventTypeLabel.c_str(), windowID, scrollWindow->GetName().c_str());
        LOGIT( _T("PostDragScrollEvent type[%s]"), eventTypeLabel.c_str());
    #endif

    cbPlugin* pPlgn = (cbPlugin*)targetWin;
    if ( not targetWin )
        pPlgn = Manager::Get()->GetPluginManager()->FindPluginByName(_T("cbDragScroll"));
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
bool sDragScrollEvent::ProcessDragScrollEvent(const cbPlugin* targetWin)
// ----------------------------------------------------------------------------
{
    // Propagate sDragScrollEvent

    #if defined(LOGGING)
        //-int windowID = GetWindowID();
        //-wxWindow* scrollWindow = GetWindow();
        wxString eventTypeLabel = GetEventTypeLabel();
        //-LOGIT( _T("ProcessDragScrollEvent type[%s]id[%d]str[%s]"), eventTypeLabel.c_str(), windowID, scrollWindow->GetName().c_str());
        LOGIT( _T("ProcessDragScrollEvent type[%s]"), eventTypeLabel.c_str());
    #endif

    cbPlugin* pPlgn = (cbPlugin*)targetWin;
    if ( not targetWin )
        pPlgn = Manager::Get()->GetPluginManager()->FindPluginByName(_T("cbDragScroll"));

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
