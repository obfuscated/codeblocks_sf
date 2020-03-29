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

 #include "dragscrollevent.h"
 #include "dragscroll.h"

IMPLEMENT_DYNAMIC_CLASS(DragScrollEvent, wxCommandEvent)

const wxEventType wxEVT_DRAGSCROLL_EVENT = wxNewEventType();

// ----------------------------------------------------------------------------
DragScrollEvent::DragScrollEvent(wxEventType commandType, int id)
// ----------------------------------------------------------------------------
					 :wxCommandEvent(commandType, id)
{
    //ctor
    //-m_propagationLevel = wxEVENT_PROPAGATE_MAX ;
    m_EventTypeLabel = _T("UNKOWN");
    if ( idDragScrollAddWindow == id)
        m_EventTypeLabel = _T("EVT_DRAGSCROLL_ADD_WINDOW");
    if ( idDragScrollRemoveWindow == id)
        m_EventTypeLabel = _T("EVT_DRAGSCROLL_REMOVE_WINDOW");
    if ( idDragScrollRescan == id)
        m_EventTypeLabel = _T("EVT_DRAGSCROLL_RESCAN");
    if ( idDragScrollReadConfig == id)
        m_EventTypeLabel = _T("EVT_DRAGSCROLL_READ_CONFIG");
    if ( idDragScrollInvokeConfig == id)
        m_EventTypeLabel = _T("EVT_DRAGSCROLL_INVOKE_CONFIG");
}
// -- clone -------------------------------------------------------------------
DragScrollEvent::DragScrollEvent( const DragScrollEvent& Event)
// ----------------------------------------------------------------------------
				  :wxCommandEvent(Event)
				  //-,m_WindowID(0)
				  //-,m_pWindow(0)
{
    // This is invoked by DragScrollEvent::Clone() from AddPendingEvent etc
    // Copy any fields necessary to the cloned event, eg.
	m_EventTypeLabel = Event.GetEventTypeLabel();
}
// ----------------------------------------------------------------------------
DragScrollEvent::~DragScrollEvent()
// ----------------------------------------------------------------------------
{
    //dtor
}
// ----------------------------------------------------------------------------
bool DragScrollEvent::PostDragScrollEvent(const cbPlugin* targetWin)
// ----------------------------------------------------------------------------
{
    // Propagate DragScrollEvent to ThreadSearchFrame

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
bool DragScrollEvent::ProcessDragScrollEvent(const cbPlugin* targetWin)
// ----------------------------------------------------------------------------
{
    // Propagate DragScrollEvent


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
