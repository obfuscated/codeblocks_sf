/***************************************************************
 * Name:      CodeSnippetsEvent
 *
 * Purpose:   This class implements the events sent by/for a
 *            CodeSnippets request to the
 *            CodeSnippets Manager to request services such as
 *            Tree item focus and/or editing services.
 *            wxCommandEvent m_id contains a code snippets id.
 *
 * Created:   2008/4/01
 * Copyright: Pecan
 * License:   GPL
 **************************************************************/

 #include "codesnippetsevent.h"
 #include "version.h"
 #include "csutils.h"
 #include "snippetsconfig.h"

DEFINE_EVENT_TYPE(wxEVT_CODESNIPPETS_SELECT)
DEFINE_EVENT_TYPE(wxEVT_CODESNIPPETS_EDIT)
DEFINE_EVENT_TYPE(wxEVT_CODESNIPPETS_NEW_INDEX)
DEFINE_EVENT_TYPE(wxEVT_CODESNIPPETS_GETFILELINKS)
IMPLEMENT_DYNAMIC_CLASS(CodeSnippetsEvent, wxCommandEvent)

// ----------------------------------------------------------------------------
CodeSnippetsEvent::CodeSnippetsEvent(wxEventType commandType, int id)
// ----------------------------------------------------------------------------
					 :wxCommandEvent(commandType, id)
{
    //ctor
    //-m_propagationLevel = wxEVENT_PROPAGATE_MAX ;
    m_SnippetID = id;
    m_SnippetString = wxEmptyString;
    m_EventTypeLabel = _T("UNKOWN");
    if ( wxEVT_CODESNIPPETS_SELECT == commandType)
        m_EventTypeLabel = _T("wxEVT_CODESNIPPETS_SELECT");
    if ( wxEVT_CODESNIPPETS_EDIT == commandType)
        m_EventTypeLabel = _T("wxEVT_CODESNIPPETS_EDIT");
    if ( wxEVT_CODESNIPPETS_NEW_INDEX == commandType)
        m_EventTypeLabel = _T("wxEVT_CODESNIPPETS_NEW_INDEX");
    if ( wxEVT_CODESNIPPETS_GETFILELINKS == commandType)
        m_EventTypeLabel = _T("wxEVT_CODESNIPPETS_GETFILELINKS");
}
// -- clone -------------------------------------------------------------------
CodeSnippetsEvent::CodeSnippetsEvent( const CodeSnippetsEvent& Event)
// ----------------------------------------------------------------------------
				  :wxCommandEvent(Event)
				  ,m_SnippetID(0)
				  ,m_SnippetString(wxEmptyString)
{
    // This is invoked by CodeSnippetsEvent::Clone() from AddPendingEvent etc
	m_SnippetID = Event.GetSnippetID();
	m_SnippetString = Event.GetSnippetString();
	m_EventTypeLabel = Event.GetEventTypeLabel();
}
// ----------------------------------------------------------------------------
CodeSnippetsEvent::~CodeSnippetsEvent()
// ----------------------------------------------------------------------------
{
    //dtor
}
// ----------------------------------------------------------------------------
bool CodeSnippetsEvent::PostCodeSnippetsEvent(const CodeSnippetsEvent& event)
// ----------------------------------------------------------------------------
{
    // Propagate CodeSnippetsEvent to ThreadSearchFrame
    // They wont get there otherwise.

    // Here we append the events to the lowest window that
    // might conceivable want the CodeSnippetsEvents (niz., "Flat Notebook")
    // "Flat Notebook" belongs to both CodeSnippets() *and* ThreadSearch() frames
    // when ThreadSearch() re-parented ThreadSearchView() and cbEditor wxNotebook
    // into ThreadSearchFrame's wxSplitterWindow.
    // That way, the events propagate back up both frame chains for all to see.

    Utils utils;

    ////#if defined(LOGGING)
    ////    int snippetID = event.GetSnippetID();
    ////    wxString snippetString = event.GetSnippetString();
    ////    wxString eventTypeLabel = event.GetEventTypeLabel();
    ////    LOGIT( _T("PostCodeSnippetsEvent type[%s]id[%d]str[%s]"), eventTypeLabel.c_str(), snippetID, snippetString.c_str());
    ////#endif

    wxWindow* pCodeSnippetsTreeCtrl = (wxWindow*)GetConfig()->GetSnippetsTreeCtrl();
    wxWindow* pSearchPath = utils.FindWindowRecursively( (wxWindow*)GetConfig()->GetThreadSearchFrame(), _T("SCIwindow") );

    #if defined(LOGGING)
    if ( pCodeSnippetsTreeCtrl)
        LOGIT( _T("PostCodeSnippetsEvent TreeCtrl[%p][%s]"), pCodeSnippetsTreeCtrl, pCodeSnippetsTreeCtrl->GetName().c_str());
    if ( pSearchPath )
        LOGIT( _T("PostCodeSnippetsEvent SrchPath[%p][%s]"), pSearchPath, pSearchPath->GetName().c_str());
    #endif
    // Propagate the event to ThreadSearchFrame
    if ( pSearchPath && pCodeSnippetsTreeCtrl)
    {
        #if wxCHECK_VERSION(3, 0, 0)
        pSearchPath->GetEventHandler()->AddPendingEvent( (wxEvent&)event );
        pCodeSnippetsTreeCtrl->GetEventHandler()->AddPendingEvent( (wxEvent&)event );
        #else
        pSearchPath->AddPendingEvent( (wxEvent&)event );
        pCodeSnippetsTreeCtrl->AddPendingEvent( (wxEvent&)event );
        #endif
    }
    else
    {
        #if defined(LOGGING)
        LOGIT( _T("PostCodeSnippetsEvent[%s]"), _T("Failed"));
        #endif
        return false;
    }

    return true;

}//PostCodeSnippetsEvent
// ----------------------------------------------------------------------------
bool CodeSnippetsEvent::ProcessCodeSnippetsEvent(const CodeSnippetsEvent& event)
// ----------------------------------------------------------------------------
{
    // Propagate CodeSnippetsEvent to ThreadSearchFrame
    // They wont get there otherwise.

    // Here we append the events to the lowest window that
    // might conceivable want the CodeSnippetsEvents (niz., "Flat Notebook")
    // "Flat Notebook" belongs to both CodeSnippets() *and* ThreadSearch() frames
    // when ThreadSearch() re-parented ThreadSearchView() and cbEditor wxNotebook
    // into ThreadSearchFrame's wxSplitterWindow.
    // That way, the events propagate back up both frame chains for all to see.

    Utils utils;

    ////#if defined(LOGGING)
    ////    int snippetID = event.GetSnippetID();
    ////    wxString snippetString = event.GetSnippetString();
    ////    wxString eventTypeLabel = event.GetEventTypeLabel();
    ////    LOGIT( _T("PostCodeSnippetsEvent type[%s]id[%d]str[%s]"), eventTypeLabel.c_str(), snippetID, snippetString.c_str());
    ////#endif

    wxWindow* pCodeSnippetsTreeCtrl = (wxWindow*)GetConfig()->GetSnippetsTreeCtrl();
    wxWindow* pSearchPath = utils.FindWindowRecursively( GetConfig()->GetThreadSearchFrame(), _T("SCIwindow") );
    #if defined(LOGGING)
    if ( pCodeSnippetsTreeCtrl)
        LOGIT( _T("PostCodeSnippetsEvent TreeCtrl[%p][%s]"), pCodeSnippetsTreeCtrl, pCodeSnippetsTreeCtrl->GetName().c_str());
    if ( pSearchPath )
        LOGIT( _T("PostCodeSnippetsEvent SrchPath[%p][%s]"), pSearchPath, pSearchPath->GetName().c_str());
    #endif
    // Propagate the event to ThreadSearchFrame
    if ( pSearchPath && pCodeSnippetsTreeCtrl)
    {
        #if wxCHECK_VERSION(3, 0, 0)
        pSearchPath->GetEventHandler()->ProcessEvent( (wxEvent&)event );
        pCodeSnippetsTreeCtrl->GetEventHandler()->ProcessEvent( (wxEvent&)event );
        #else
        pSearchPath->ProcessEvent( (wxEvent&)event );
        pCodeSnippetsTreeCtrl->ProcessEvent( (wxEvent&)event );
        #endif
    }
    else
    {
        #if defined(LOGGING)
        LOGIT( _T("PostCodeSnippetsEvent[%s]"), _T("Failed"));
        #endif
        return false;
    }

    return true;

}//PostCodeSnippetsEvent
