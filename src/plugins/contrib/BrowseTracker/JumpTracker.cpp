// ----------------------------------------------------------------------------
//  JumpTracker.cpp
// ----------------------------------------------------------------------------
#include <sdk.h> // Code::Blocks SDK
#include <configurationpanel.h>
#include <cbstyledtextctrl.h>
#include <editormanager.h>
#include <cbeditor.h>

#include "Version.h"
#include "JumpTracker.h"
#include "JumpData.h"

// now that we have ArrayOfJumpData declaration in scope we may finish the
// definition -- note that this expands into some C++
// code and so should only be compiled once (i.e., don't put this in the
// header, but into a source file or you will get linking errors)
#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(ArrayOfJumpData);

// ----------------------------------------------------------------------------
//  namespace
// ----------------------------------------------------------------------------
// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    //-PluginRegistrant<JumpTracker> reg(_T("JumpTracker"));
    int idMenuJumpView = wxNewId();
    int idMenuJumpBack = wxNewId();
    int idMenuJumpNext = wxNewId();
    int idMenuJumpClear = wxNewId();
    int idMenuJumpDump = wxNewId();

    int idToolJumpPrev = XRCID("idJumpPrev");
    int idToolJumpNext = XRCID("idJumpNext");
}

// ----------------------------------------------------------------------------
//  Events Table
// ----------------------------------------------------------------------------
// events handling
BEGIN_EVENT_TABLE(JumpTracker, cbPlugin)
    // add any events you want to handle here
//    Now doing Connect() in BuildMenu()
//    EVT_MENU( idMenuJumpBack,  JumpTracker::OnMenuJumpBack)
//    EVT_MENU( idMenuJumpNext,  JumpTracker::OnMenuJumpNext)
//    EVT_MENU( idMenuJumpClear, JumpTracker::OnMenuJumpClear)
//    EVT_MENU( idMenuJumpDump,  JumpTracker::OnMenuJumpDump)

END_EVENT_TABLE()

// constructor
// ----------------------------------------------------------------------------
JumpTracker::JumpTracker()
// ----------------------------------------------------------------------------
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
//    if(!Manager::LoadResource(_T("JumpTracker.zip")))
//    {
//        NotifyMissingFile(_T("JumpTracker.zip"));
//    }

    m_bShuttingDown = false;
    m_FilenameLast = wxEmptyString;
    m_PosnLast = 0;
    m_cursor = 0;
    m_insertNext = maxJumpEntries;
    m_ArrayOfJumpData.Clear();
    m_bProjectClosing = false;
    m_IsAttached = false;
    m_bJumpInProgress = false;
    m_bWrapJumpEntries = false;
    m_pToolBar = 0;
}
// ----------------------------------------------------------------------------
JumpTracker::~JumpTracker()
// ----------------------------------------------------------------------------
{
    // destructor
}
// ----------------------------------------------------------------------------
void JumpTracker::OnAttach()
// ----------------------------------------------------------------------------
{
    // do whatever initialization you need for your plugin
    // :NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...

    // --------------------------------------------------
    // Initialize a debugging log/version control
    // --------------------------------------------------
    //    PlgnVersion plgnVersion;
    //    #if LOGGING
    //     wxLog::EnableLogging(true);
    //     m_pPlgnLog = new wxLogWindow( Manager::Get()->GetAppWindow(), _T(" JumpTracker"),true,false);
    //     wxLog::SetActiveTarget( m_pPlgnLog);
    //     m_pPlgnLog->GetFrame()->SetSize(20,30,600,300);
    //     LOGIT( _T("JT JumpTracker Logging Started[%s]"),plgnVersion.GetVersion().c_str());
    //     m_pPlgnLog->Flush();
    //    #endif

    //    // Set current plugin version
    //    PluginInfo* pInfo = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(this));
    //    pInfo->version = plgnVersion.GetVersion();

    m_bJumpInProgress = false;

    wxWindow* appWin = Manager::Get()->GetAppWindow();
    appWin->PushEventHandler(this);
    appWin->Connect(idMenuJumpBack, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpBack), 0, this);
    appWin->Connect(idMenuJumpNext, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpNext), 0, this);
    appWin->Connect(idMenuJumpClear, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpClear), 0, this);
    appWin->Connect(idMenuJumpDump, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpDump), 0, this);

    appWin->Connect(idToolJumpPrev, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(JumpTracker::OnMenuJumpBack), 0, this);
    appWin->Connect(idToolJumpNext, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(JumpTracker::OnMenuJumpNext), 0, this);
    appWin->Connect(idToolJumpPrev, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(JumpTracker::OnUpdateUI), 0, this);
    appWin->Connect(idToolJumpNext, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(JumpTracker::OnUpdateUI), 0, this);


    // Codeblocks Events registration
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_UPDATE_UI   , new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnEditorUpdateEvent));
   //-Manager::Get()->RegisterEventSink(cbEVT_EDITOR_ACTIVATED, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnEditorActivated));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_DEACTIVATED, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnEditorDeactivated));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_CLOSE, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnEditorClosed));

    Manager::Get()->RegisterEventSink(cbEVT_APP_START_SHUTDOWN, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnStartShutdown));
    // -- Project events
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnProjectActivatedEvent));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnProjectClosing));

}
// ----------------------------------------------------------------------------
void JumpTracker::OnRelease(bool /*appShutDown*/)
// ----------------------------------------------------------------------------
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...

    // Free JumpData memory
    wxCommandEvent evt;
    OnMenuJumpClear(evt);

    wxWindow* appWin = Manager::Get()->GetAppWindow();
    appWin->Disconnect(idMenuJumpBack, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpBack), 0, this);
    appWin->Disconnect(idMenuJumpNext, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpNext), 0, this);
    appWin->Disconnect(idMenuJumpClear, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpClear), 0, this);
    appWin->Disconnect(idMenuJumpDump, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpDump), 0, this);

    appWin->Disconnect(idToolJumpPrev, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(JumpTracker::OnMenuJumpBack), 0, this);
    appWin->Disconnect(idToolJumpNext, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(JumpTracker::OnMenuJumpNext), 0, this);
    appWin->Disconnect(idToolJumpPrev, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(JumpTracker::OnUpdateUI), 0, this);
    appWin->Disconnect(idToolJumpNext, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(JumpTracker::OnUpdateUI), 0, this);

}
// ----------------------------------------------------------------------------
void JumpTracker::BuildMenu(wxMenuBar* menuBar)
// ----------------------------------------------------------------------------
{
    //The application is offering its menubar for your plugin,
    //to add any menu items you want...
    //Append any items you need in the menu...
    //NOTE: Be careful in here... The application's menubar is at your disposal.
    //-NotImplemented(_T("JumpTracker::BuildMenu()"));

     // insert menu items
    wxMenu* jump_submenu = new wxMenu;
    jump_submenu->Append(idMenuJumpBack,  _("Jump Back"),   _("Jump back to previous ed position"));
    jump_submenu->Append(idMenuJumpNext,  _("Jump Frwd"),   _("Jump to next ed position"));
    jump_submenu->Append(idMenuJumpClear, _("Jump Clear"),  _("Jump Clear History"));
    #if defined(LOGGING)
    jump_submenu->Append(idMenuJumpDump,  _("Jump Dump"),   _("Jump Dump Array"));
    #endif

    int viewPos = menuBar->FindMenu(_("&View"));

    if ( viewPos == wxNOT_FOUND )
        return;

    // Place the menu inside the View Menu
    wxMenu* pViewMenu = menuBar->GetMenu(viewPos);
    pViewMenu->Append(idMenuJumpView, _("Jump"), jump_submenu, _("Jump"));

}
// ----------------------------------------------------------------------------
void JumpTracker::BuildModuleMenu(const ModuleType /*type*/, wxMenu* /*menu*/, const FileTreeData* /*data*/)
// ----------------------------------------------------------------------------
{
    //Some library module is ready to display a pop-up menu.
    //Check the parameter \"type\" and see which module it is
    //and append any items you need in the menu...
    //TIP: for consistency, add a separator as the first item...
    //-NotImplemented(_T("JumpTracker::BuildModuleMenu()"));
}
// ----------------------------------------------------------------------------
bool JumpTracker::BuildToolBar(wxToolBar* toolBar)
// ----------------------------------------------------------------------------
{
    m_pToolBar = toolBar;

    //The application is offering its toolbar for your plugin,
    //to add any toolbar items you want...
    //Append any items you need on the toolbar...
    //-NotImplemented(_T("JumpTracker::BuildToolBar()"));

    // return true if you add toolbar items
    return false;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnEditorUpdateEvent(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    event.Skip();

    if ( m_bShuttingDown )
        return;
    if (m_bJumpInProgress)
        return;

    cbEditor* ed =  Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if(not ed)
        return;

    wxString edFilename = ed->GetFilename();
    cbStyledTextCtrl* edstc = ed->GetControl();
    if(edstc->GetCurrentLine() == wxSCI_INVALID_POSITION)
        return;

    long edLine = edstc->GetCurrentLine();
    long edPosn = edstc->GetCurrentPos();

    long topLine = edstc->GetFirstVisibleLine();
    long scnSize = edstc->LinesOnScreen();
    long botLine = (topLine+scnSize)-1; // In keeping with top line == 0 origin
    botLine = (botLine < 0) ? 0 : botLine;
    botLine = (botLine > edstc->GetLineCount()) ? edstc->GetLineCount() : botLine;

    #if defined(LOGGING)
    //LOGIT( _T("JT OnEditorUpdateEvent Filename[%s] line[%ld] pos[%ld] "), edFilename.c_str(), edLine, edPosn);
    //LOGIT( _T("JT \ttopLine[%ld] botLine[%ld] OnScrn[%ld] "), topLine, botLine, edstc->LinesOnScreen());
    #endif

    // New editor activated?
    if (m_FilenameLast not_eq edFilename)
    {
        m_PosnLast = edPosn;
        m_FilenameLast = edFilename;
        //if ( m_cursor not_eq JumpDataContains(edFilename, edPosn) )
            JumpDataAdd(edFilename, edPosn, edLine);
    }

    // If new line within half screen of old line, don't record current line
    //-long lineLast = edstc->LineFromPosition(m_PosnLast);

    //-bool bIsVis = ( edstc->GetLineVisible(lineLast) ); doesnt work
    //-if ( (lineLast >= topLine) &&                      doesnt work
    //-    (lineLast < botLine) )
    //-int halfPageSize = edstc->LinesOnScreen()>>1;
    //-if ( halfPageSize > abs(edLine - lineLast))
    //-    ;//return;

    // if user has not moved cursor, ignore this update
    if (m_PosnLast == edPosn)
        return;

    // record new posn
    m_PosnLast = edPosn;
    m_FilenameLast = edFilename;
    //if ( m_cursor not_eq JumpDataContains(edFilename, edPosn) )
        JumpDataAdd(edFilename, edPosn, edLine);

    return;
}//OnEditorUpdateEvent
// ----------------------------------------------------------------------------
void JumpTracker::OnEditorActivated(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // Record this activation event and place activation in history
    event.Skip();

    if (m_bShuttingDown) return;
    if (not IsAttached()) return;

    // Don't record closing editor activations
    if (m_bProjectClosing)
        return;

    EditorBase* eb = event.GetEditor();
    wxString edFilename = eb->GetFilename();
    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);

    if (not cbed)
    {
        // Since wxAuiNotebook added, there's no cbEditor associated during
        // an initial cbEVT_EDITOR_ACTIVATED event. So we ignore the inital
        // call and get OnEditorOpened() to re-issue OnEditorActivated() when
        // it does have a cbEditor, but no cbProject associated;
        #if defined(LOGGING)
        LOGIT( _T("JT [OnEditorActivated ignored:no cbEditor[%s]"), edFilename.c_str());
        #endif
        return;
    }

    #if defined(LOGGING)
    LOGIT( _T("JT Editor Activated[%s]"), eb->GetShortName().c_str() );
    #endif

    cbStyledTextCtrl* edstc = cbed->GetControl();
    if(edstc->GetCurrentLine() == wxSCI_INVALID_POSITION)
        return;

    long edPosn = edstc->GetCurrentPos();
    //if ( m_cursor not_eq JumpDataContains(edFilename, edPosn) )
        JumpDataAdd(edFilename, edPosn, edstc->GetCurrentLine());
    return;
}//OnEditorActivated
// ----------------------------------------------------------------------------
void JumpTracker::OnEditorDeactivated(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // Record this deactivation event and place deactivation location in history
    event.Skip();

    if (m_bShuttingDown) return;
    if (not IsAttached()) return;

    // Don't record closing editor deactivations
    if (m_bProjectClosing)
        return;

    if ( Manager::Get()->GetProjectManager()->IsLoading() )
        return;

    EditorBase* eb = event.GetEditor();
    wxString edFilename = eb->GetFilename();
    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);

    if (not cbed)
    {
        // We ignore events with no associated editor
        #if defined(LOGGING)
        LOGIT( _T("JT [OnEditorDeactivated ignored:no cbEditor[%s]"), edFilename.c_str());
        #endif
        return;
    }

    #if defined(LOGGING)
    LOGIT( _T("JT Editor DeActivated[%s]"), eb->GetShortName().c_str() );
    #endif

    cbStyledTextCtrl* edstc = cbed->GetControl();
    if(edstc->GetCurrentLine() == wxSCI_INVALID_POSITION)
        return;

    long edPosn = edstc->GetCurrentPos();
    //if ( m_cursor not_eq JumpDataContains(edFilename, edPosn) )
        JumpDataAdd(edFilename, edPosn, edstc->GetCurrentLine());
    return;
}//OnEditorDeactivated
// ----------------------------------------------------------------------------
void JumpTracker::OnEditorClosed(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // clear this editor out of our arrays and pointers

    //_NOTE: using Manager::Get->GetEditorManager()->GetEditor... etc will
    //      fail in this codeblocks event.
    //      The cbEditors are nolonger available

    event.Skip();

    if (not IsAttached())
        return;

    wxString filePath = event.GetString();

    #if defined(LOGGING)
        EditorBase* eb = event.GetEditor();
        LOGIT( _T("JT OnEditorClosed Eb[%p][%s]"), eb, eb->GetShortName().c_str() );
    #endif


    for (int ii=m_ArrayOfJumpData.GetCount()-1; ii > -1; --ii)
    {
        if (m_ArrayOfJumpData[ii].GetFilename() == filePath)
            m_ArrayOfJumpData.RemoveAt(ii);
        if (((int)m_ArrayOfJumpData.GetCount()-1) < m_cursor)
            m_cursor = GetPreviousIndex(m_cursor);
        if (((int)m_ArrayOfJumpData.GetCount()-1) < m_insertNext)
            m_insertNext = GetPreviousIndex(m_insertNext);
    }
    #if defined(LOGGING)
    wxCommandEvent evt;
    OnMenuJumpDump(evt);
    #endif

}//OnEditorClosed

// ----------------------------------------------------------------------------
void JumpTracker::OnStartShutdown(CodeBlocksEvent& /*event*/)
// ----------------------------------------------------------------------------
{
    m_bShuttingDown = true;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnProjectClosing(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    event.Skip();

    // This hook occurs before the editors are closed. That allows
    // us to reference CB project and editor related data before CB
    // deletes it all.

    if (m_bShuttingDown) return;
    if (not IsAttached()) return;

    //cbProject* pProject = event.GetProject();
    //if (not pProject) return; //It happens!

    m_bProjectClosing = true;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnProjectActivatedEvent(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    event.Skip();

    // NB: EVT_PROJECT_ACTIVATE is occuring before EVT_PROJECT_OPEN
    // NB: EVT_EDITOR_ACTIVATE event occur before EVT_PROJECT_ACTIVATE or EVT_PROJECT_OPEN

    event.Skip();
    if (m_bShuttingDown) return;
    if (not IsAttached()) return;

    // Previous project was closing
    if (m_bProjectClosing)
        m_bProjectClosing = false;

}//OnProjectActivatedEvent
// ----------------------------------------------------------------------------
void JumpTracker::OnUpdateUI(wxUpdateUIEvent& event)
// ----------------------------------------------------------------------------
{
    int count = m_ArrayOfJumpData.GetCount();
    //-m_pToolBar->EnableTool(idToolJumpNext, count > 0 /*&& m_cursor != m_insertNext - 1*/);
    //-m_pToolBar->EnableTool(idToolJumpPrev, count > 0 /*&& m_cursor != m_insertNext*/);

    // If not  wrapping && we're about to advance into the insert index, diable
    bool enableNext = (count > 0);
    if (not m_bWrapJumpEntries)
        if (m_cursor == m_insertNext)
            enableNext = false;

    bool enablePrev = count > 0;
    // If not wrapping && we're about to backup into the insert index, disable
    if (not m_bWrapJumpEntries)
        if (GetPreviousIndex(m_cursor) == m_insertNext)
            enablePrev = false;

    m_pToolBar->EnableTool( idToolJumpNext, enableNext);
    m_pToolBar->EnableTool( idToolJumpPrev, enablePrev);

    event.Skip();
}
// ----------------------------------------------------------------------------
void JumpTracker::JumpDataAdd(const wxString& filename, const long posn, const long lineNum)
// ----------------------------------------------------------------------------
{
    // Do not record old jump locations when a jump is in progress
    // Caused by activating an editor inside the jump routines
    if (m_bJumpInProgress)
        return;

    // Dont record position if line number is < 1 since a newly loaded
    // file always reports an event for line 0
     if (lineNum < 1)       // user requested feature 2010/06/1
        return;

    // if current entry is identical edit line, just update position
    if ( JumpDataContains(m_cursor,filename, posn))
    {
        m_ArrayOfJumpData[m_cursor].SetPosition(posn);
        return;
    }
    if ( JumpDataContains(GetPreviousIndex(m_insertNext), filename, posn))
    {
        m_ArrayOfJumpData[GetPreviousIndex(m_insertNext)].SetPosition(posn);
        return;
    }

    //
    // record new jump entry
    //

    size_t kount = m_ArrayOfJumpData.GetCount();
    if ( m_insertNext > maxJumpEntries-1 )
            m_insertNext = 0;

    #if defined(LOGGING)
    LOGIT( _T("JT JumpDataAdd[%s][%ld][%d]"), filename.c_str(), posn, m_insertNext);
    #endif

    if ( kount == maxJumpEntries )
    {   //remove last item in the array
        m_ArrayOfJumpData.RemoveAt(maxJumpEntries-1);
    }

   //initialize new item
    m_insertNext = GetNextIndex(m_insertNext);
    m_ArrayOfJumpData.Insert(new JumpData(filename, posn), m_insertNext);
    m_cursor = m_insertNext;

    #if defined(LOGGING)
    wxCommandEvent evt;
    OnMenuJumpDump(evt);
    #endif
    return;
}
// ----------------------------------------------------------------------------
int JumpTracker::FindJumpDataContaining(const wxString& filename, const long posn)
// ----------------------------------------------------------------------------
{
    size_t kount = m_ArrayOfJumpData.GetCount();
    if (not kount) return wxNOT_FOUND;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(filename);
    if (not ed) return wxNOT_FOUND;

    cbStyledTextCtrl* pstc = ed->GetControl();
    if (not pstc) return wxNOT_FOUND;

    int halfPageSize = pstc->LinesOnScreen()>>1;

    // search from array insertion point so we check last entered entry first
    size_t j = m_insertNext;
    for (size_t i=0; i<kount; ++i, ++j)
    {
        j = GetPreviousIndex(j);
        JumpData& jumpData = m_ArrayOfJumpData.Item(j);
        if ( jumpData.GetFilename() not_eq filename )
            continue;
        long jumpLine = pstc->LineFromPosition(jumpData.GetPosition());
        long currLine = pstc->LineFromPosition(posn);
        if ( halfPageSize > abs(jumpLine - currLine))
           return j;
    }

    return wxNOT_FOUND;
}
// ----------------------------------------------------------------------------
bool JumpTracker::JumpDataContains(const int indx, const wxString& filename, const long posn)
// ----------------------------------------------------------------------------
{
    size_t kount = m_ArrayOfJumpData.GetCount();
    if (not kount) return false;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(filename);
    if (not ed) return false;

    cbStyledTextCtrl* pstc = ed->GetControl();
    if (not pstc) return false;

    int halfPageSize = pstc->LinesOnScreen()>>1;

    JumpData& jumpData = m_ArrayOfJumpData.Item(indx);
    if ( jumpData.GetFilename() not_eq filename )
        return false;
    long jumpLine = pstc->LineFromPosition(jumpData.GetPosition());
    long currLine = pstc->LineFromPosition(posn);
    if ( halfPageSize > abs(jumpLine - currLine))
       return true;


    return false;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnMenuJumpBack(wxCommandEvent &/*event*/)
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    LOGIT( _T("JT [%s]"), _T("OnMenuJumpBack"));
    #endif

    int knt = m_ArrayOfJumpData.GetCount();
    if (0 == knt)
        return;

    // If not wrapping && we're about to backup into the insert index, return
    if (not m_bWrapJumpEntries)
        if (GetPreviousIndex(m_cursor) == m_insertNext)
            return;

    m_bJumpInProgress = true;

    EditorManager* edmgr = Manager::Get()->GetEditorManager();
    EditorBase* eb = edmgr->GetActiveEditor();
    cbEditor* cbed = edmgr->GetBuiltinEditor(eb);
    //-long activeEdLine = 0;
    long activeEdPosn = 0;
    wxString activeEdFilename = wxEmptyString;
    if (cbed)
    {
        //-activeEdLine = cbed->GetControl()->GetCurrentLine();
        activeEdPosn = cbed->GetControl()->GetCurrentPos();
        activeEdFilename = cbed->GetFilename();
    }

    // if active editor line == m_cursor, back up the m_cursor
    // until finding an entry that does not match this m_cursor entry.
    // else
    // find the previous m_insertNext entry that does not match this editor or line number

    do {    // find the previous appropriate jump position

        // if current cursor position is also current editor and line,
        // return the previous m_cursor entry
        if ( JumpDataContains(m_cursor, cbed->GetFilename(), activeEdPosn))
        {
            // FIXME (ph#): a dead editor can be returned here
            m_cursor = GetPreviousIndex(m_cursor);
        }
        else    //find an entry (backward) from the insertion point
        {
            int idx = m_insertNext;

            for (int ii=0; ii<knt; ++ii)
            {
                idx = GetPreviousIndex(idx);
                if ( idx == wxNOT_FOUND)
                    break;
                JumpData& jumpdata = m_ArrayOfJumpData[idx];
                if (not edmgr->IsOpen(jumpdata.GetFilename()))
                    continue;
                // skip entry when same as current position
                if ( JumpDataContains(idx, activeEdFilename, activeEdPosn))
                    continue;
                else
                {
                    m_cursor = idx;
                    break; //for
                }
            }//for
        }//else

        JumpData& jumpData = m_ArrayOfJumpData[m_cursor];
        wxString edFilename = jumpData.GetFilename();
        long edPosn = jumpData.GetPosition();
        #if defined(LOGGING)
        LOGIT( _T("JT OnMenuJumpBack [%s][%ld]curs[%d]"), edFilename.wx_str(), edPosn, m_cursor);
        #endif

        // activate editor
        EditorBase* eb = edmgr->GetEditor(edFilename);
        if (not eb) break;

        edmgr->SetActiveEditor(eb); // cause a cbEVT_EditorActivated event
        // position to editor line
        cbEditor* cbed = edmgr->GetBuiltinEditor(eb);
        if (not cbed) break;

        cbed->GotoLine(cbed->GetControl()->LineFromPosition(edPosn)); //center on scrn
        cbed->GetControl()->GotoPos(edPosn);

    }while(0);

    #if defined(LOGGING)
    LOGIT( _T("JT [%s]"), _T("END OnMenuJumpBack"));
    wxCommandEvent evt;
    OnMenuJumpDump(evt);
    #endif

    m_bJumpInProgress = false;
    return;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnMenuJumpNext(wxCommandEvent &/*event*/)
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    LOGIT( _T("JT [%s]"), _T("OnMenuJumpNext"));
    #endif

    int knt = m_ArrayOfJumpData.GetCount();
    if (0 == knt)
        return;

    // If not  wrapping && we're about to advance into the insert index, return
    //-if (GetNextIndex(m_cursor) == m_insertNext)
    if (not m_bWrapJumpEntries)
        if (m_cursor == m_insertNext)
            return;

    m_bJumpInProgress = true;

    EditorManager* edmgr = Manager::Get()->GetEditorManager();
    EditorBase* eb = edmgr->GetActiveEditor();
    cbEditor* cbed = edmgr->GetBuiltinEditor(eb);
    //-long activeEdLine = 0;
    long activeEdPosn = 0;
    wxString activeEdFilename = wxEmptyString;
    if (cbed)
    {
        //-activeEdLine = cbed->GetControl()->GetCurrentLine();
        activeEdPosn = cbed->GetControl()->GetCurrentPos();
        activeEdFilename = cbed->GetFilename();
    }

    // if active editor line == m_cursor, advance up the m_cursor
    // until finding an entry that does not match this m_cursor entry.
    // else
    // find the next m_insertNext entry that does not match this editor or line number

    do {    // find the next appropriate jump position

        // if current cursor position is also current editor and line,
        // return the next m_cursor entry
        if ( JumpDataContains(m_cursor, cbed->GetFilename(), activeEdPosn))
        {
            m_cursor = GetNextIndex(m_cursor);
        }
        else    //find an entry (forward) from the insertion point
        {
            int idx = m_insertNext;

            for (int ii=0; ii<knt; ++ii)
            {
                idx = GetNextIndex(idx);
                if ( idx == wxNOT_FOUND)
                    break;
                JumpData& jumpdata = m_ArrayOfJumpData[idx];
                if (not edmgr->IsOpen(jumpdata.GetFilename()))
                    continue;

                if ( not JumpDataContains(idx, activeEdFilename, activeEdPosn))
                {
                    m_cursor = idx;
                    break; //for
                }
            }//for
        }

        JumpData& jumpData = m_ArrayOfJumpData[m_cursor];
        wxString edFilename = jumpData.GetFilename();
        long edPosn = jumpData.GetPosition();
        #if defined(LOGGING)
        LOGIT( _T("JT OnMenuJumpBack [%s][%ld]curs[%d]"), edFilename.wx_str(), edPosn, m_cursor);
        #endif

        // activate editor
        EditorBase* eb = edmgr->GetEditor(edFilename);
        if (not eb) break;

        edmgr->SetActiveEditor(eb); // cause a cbEVT_EditorActivated event
        // position to editor line
        cbEditor* cbed = edmgr->GetBuiltinEditor(eb);
        if (not cbed) break;

        cbed->GotoLine(cbed->GetControl()->LineFromPosition(edPosn)); //center on scrn
        cbed->GetControl()->GotoPos(edPosn);

    }while(0);

    #if defined(LOGGING)
    LOGIT( _T("JT [%s]"), _T("END OnMenuJumpBack"));
    wxCommandEvent evt;
    OnMenuJumpDump(evt);
    #endif

    m_bJumpInProgress = false;
    return;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnMenuJumpClear(wxCommandEvent &/*event*/)
// ----------------------------------------------------------------------------
{
    m_insertNext = m_cursor = maxJumpEntries;
    m_ArrayOfJumpData.Clear();
}
// ----------------------------------------------------------------------------
void JumpTracker::OnMenuJumpDump(wxCommandEvent &/*event*/)
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    if (not m_ArrayOfJumpData.GetCount())
        LOGIT( _T("JumpDump Empty"));

    for (size_t count = 0; count < m_ArrayOfJumpData.GetCount(); ++count)
    {
        JumpData& jumpData = m_ArrayOfJumpData.Item(count);
        wxString edFilename = jumpData.GetFilename();
        long edLine = wxNOT_FOUND;
        long edPosn = jumpData.GetPosition();
        cbStyledTextCtrl* pstc = 0;
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(edFilename);
        if ( ed )
        {
            pstc = ed->GetControl();
            if (not pstc) pstc = 0;
            if (pstc)
            edLine = pstc->LineFromPosition(jumpData.GetPosition());
            edLine +=1; //editors are 0 origin
        }

        wxString msg = wxString::Format(_T("[%d][%s][%ld][%ld]"), count, edFilename.c_str(), edPosn, edLine);
        if (count == (size_t)m_cursor)
            msg.Append(_T("<--c"));
        if (count == (size_t)m_insertNext)
            msg.Append(_T("<--i"));
        LOGIT( msg );
    }

    #endif
}
// ----------------------------------------------------------------------------
void JumpTracker::SetWrapJumpEntries(const bool tf)
// ----------------------------------------------------------------------------
{
    m_bWrapJumpEntries = tf;
}
// ----------------------------------------------------------------------------
int JumpTracker::GetPreviousIndex(const int idx)
// ----------------------------------------------------------------------------
{
    int prev = idx;
    int knt = m_ArrayOfJumpData.GetCount();
    prev--;
    if ( prev < 0 )
        prev = knt-1;
    if ( prev < 0 )
        prev = 0;
    return prev;
}
// ----------------------------------------------------------------------------
int JumpTracker::GetNextIndex(const int idx)
// ----------------------------------------------------------------------------
{
    int next = idx;
    int knt = m_ArrayOfJumpData.GetCount();
    next++;
    if ( next > (knt-1) )
        next = 0;
    return next;
}
