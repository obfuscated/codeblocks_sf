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
    m_Cursor = maxJumpEntries;
    m_ArrayOfJumpData.Clear();
    m_bProjectClosing = false;
    m_IsAttached = false;
    m_bJumpInProgress = false;
    m_bWrapJumpEntries = false;

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
    appWin->Connect(idMenuJumpBack, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &JumpTracker::OnMenuJumpBack, 0, this);
    appWin->Connect(idMenuJumpNext, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &JumpTracker::OnMenuJumpNext, 0, this);
    appWin->Connect(idMenuJumpClear, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &JumpTracker::OnMenuJumpClear, 0, this);
    appWin->Connect(idMenuJumpDump, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &JumpTracker::OnMenuJumpDump, 0, this);

    // Codeblocks Events registration
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_UPDATE_UI   , new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnEditorUpdateEvent));
   //-Manager::Get()->RegisterEventSink(cbEVT_EDITOR_ACTIVATED, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnEditorActivated));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_DEACTIVATED, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnEditorDeactivated));
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
    appWin->Disconnect(idMenuJumpBack, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &JumpTracker::OnMenuJumpBack, 0, this);
    appWin->Disconnect(idMenuJumpNext, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &JumpTracker::OnMenuJumpNext, 0, this);
    appWin->Disconnect(idMenuJumpClear, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &JumpTracker::OnMenuJumpClear, 0, this);
    appWin->Disconnect(idMenuJumpDump, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &JumpTracker::OnMenuJumpDump, 0, this);
    appWin->RemoveEventHandler(this);

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
bool JumpTracker::BuildToolBar(wxToolBar* /*toolBar*/)
// ----------------------------------------------------------------------------
{
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
        //if ( m_Cursor not_eq JumpDataContains(edFilename, edPosn) )
            JumpDataAdd(edFilename, edPosn, edLine);
    }

    // If new line within half screen of old line, don't record current line
    long lineLast = edstc->LineFromPosition(m_PosnLast);
    //bool bIsVis = ( edstc->GetLineVisible(lineLast) ); doesnt work
    //if ( (lineLast >= topLine) &&                      doesnt work
    //    (lineLast < botLine) )
    int halfPageSize = edstc->LinesOnScreen()>>1;
    if ( halfPageSize > abs(edLine - lineLast))
        return;

    // record new posn
    m_PosnLast = edPosn;
    m_FilenameLast = edFilename;
    //if ( m_Cursor not_eq JumpDataContains(edFilename, edPosn) )
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
    //if ( m_Cursor not_eq JumpDataContains(edFilename, edPosn) )
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
    //if ( m_Cursor not_eq JumpDataContains(edFilename, edPosn) )
        JumpDataAdd(edFilename, edPosn, edstc->GetCurrentLine());
    return;
}//OnEditorDeactivated

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
     {
        return;
     }

    // if current entry is identical, return
    if (m_Cursor == JumpDataContains(filename, posn))
        return;

    // record new jump entry
    size_t count = m_ArrayOfJumpData.GetCount();
    m_Cursor += 1;
    if ( m_Cursor > maxJumpEntries-1 )
        m_Cursor = 0;

    #if defined(LOGGING)
    LOGIT( _T("JT JumpDataAdd[%s][%ld][%d]"), filename.c_str(), posn, m_Cursor);
    #endif

    do {
        if ( count <= (size_t)m_Cursor )
        {   //initialize new item
            m_ArrayOfJumpData.Add(new JumpData(filename, posn));
            //return;
            break;
        }

        JumpData& jumpData = m_ArrayOfJumpData.Item(m_Cursor);
        jumpData.SetFilename( filename );
        jumpData.SetPosition(  posn );
        break;
    }while(0);

    #if defined(LOGGING)
    wxCommandEvent evt;
    OnMenuJumpDump(evt);
    #endif
    return;
}
// ----------------------------------------------------------------------------
int JumpTracker::JumpDataContains(const wxString& filename, const long posn)
// ----------------------------------------------------------------------------
{
    size_t count = m_ArrayOfJumpData.GetCount();
    if (not count) return wxNOT_FOUND;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (not ed) return wxNOT_FOUND;

    cbStyledTextCtrl* pstc = ed->GetControl();
    if (not pstc) return wxNOT_FOUND;

    int halfPageSize = pstc->LinesOnScreen()>>1;

    // search from cursor posn so we check last entry first
    size_t j = m_Cursor;
    for (size_t i=0; i<count; ++i, ++j)
    {
        if (j > count-1) j = 0;
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
void JumpTracker::OnMenuJumpBack(wxCommandEvent &/*event*/)
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    LOGIT( _T("JT [%s]"), _T("OnMenuJumpBack"));
    #endif

    m_bJumpInProgress = true;

    do {
        int knt = m_ArrayOfJumpData.GetCount();
        if (not knt) break;

        if ( knt > 1 )
            m_Cursor -= 1;
        if (m_Cursor < 0)
        {
            if (m_bWrapJumpEntries)
                m_Cursor = maxJumpEntries-1;  //wrap code
            else
            {
                m_Cursor = 0;
                return;
            }
        }
        if (m_Cursor > (int)knt-1)
        {
            if (m_bWrapJumpEntries)
                m_Cursor = knt-1;           //wrap code
            else
            {
                m_Cursor = (int)knt-1;
                return;
            }
        }

        EditorManager* edmgr = Manager::Get()->GetEditorManager();
        int cursor = m_Cursor;
        wxString edFilename;
        long edPosn;
        bool found = false;
        for (int i = 0; i<knt; ++i, --cursor)
        {
            if (cursor < 0) cursor = knt-1;
            JumpData& jumpBack = m_ArrayOfJumpData.Item(cursor);
            edFilename = jumpBack.GetFilename();
            edPosn = jumpBack.GetPosition();
            if (not edmgr->IsOpen(edFilename))
                continue;
            found = true;
            break;
        }
        if (not found) break;

        m_Cursor = cursor;

        #if defined(LOGGING)
        LOGIT( _T("JT OnMenuJumpBack [%s][%ld]curs[%d]"), edFilename.c_str(), edPosn, m_Cursor);
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

    m_bJumpInProgress = false;
    #if defined(LOGGING)
    LOGIT( _T("JT [%s]"), _T("END OnMenuJumpBack"));
    #endif

    return;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnMenuJumpNext(wxCommandEvent &/*event*/)
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    //LOGIT( _T("JT [%s]"), _T("OnMenuJumpNext"));
    #endif

    m_bJumpInProgress = true;
    do {
        int knt = m_ArrayOfJumpData.GetCount();
        if (not knt) break;

        if ( knt > 1 )
            m_Cursor += 1;
        if (m_bWrapJumpEntries)
        {
            if (m_Cursor > (int)knt-1)  //wrap code
            m_Cursor = 0;               //wrap code
        }
        else //dont wrap
        {
            if (m_Cursor > (int)knt-1)
                m_Cursor = (int)knt-1;
        }

        EditorManager* edmgr = Manager::Get()->GetEditorManager();
        int cursor = m_Cursor;
        wxString edFilename;
        long edPosn;
        bool found = false;
        for (int i = 0; i<knt; ++i, ++cursor)
        {
            if (cursor > knt-1) cursor = 0;
            JumpData& jumpNext = m_ArrayOfJumpData.Item(cursor);
            edFilename = jumpNext.GetFilename();
            edPosn = jumpNext.GetPosition();
            if (not edmgr->IsOpen(edFilename))
                continue;
            found = true;
            break;
        }
        if (not found) break;

        m_Cursor = cursor;

        #if defined(LOGGING)
        LOGIT( _T("JT OnMenuJumpNext [%s][%ld]curs[%d]"), edFilename.c_str(), edPosn, m_Cursor);
        #endif
        // activate editor
        EditorBase* eb = edmgr->GetEditor(edFilename);
        if (not eb) break;

        edmgr->SetActiveEditor(eb);
        // position to editor line
        cbEditor* cbed = edmgr->GetBuiltinEditor(eb);
        if (not cbed) break;

        cbed->GotoLine(cbed->GetControl()->LineFromPosition(edPosn)); //center on scrn
        cbed->GetControl()->GotoPos(edPosn);
    }while(0);

    m_bJumpInProgress = false;
    return;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnMenuJumpClear(wxCommandEvent &/*event*/)
// ----------------------------------------------------------------------------
{
    m_Cursor = maxJumpEntries;
    m_ArrayOfJumpData.Clear();
}
// ----------------------------------------------------------------------------
void JumpTracker::OnMenuJumpDump(wxCommandEvent &/*event*/)
// ----------------------------------------------------------------------------
{
    //-return; //debugging
    #if defined(LOGGING)

    for (size_t count = 0; count < m_ArrayOfJumpData.GetCount(); ++count)
    {
        JumpData& jumpData = m_ArrayOfJumpData.Item(count);
        wxString edFilename = jumpData.GetFilename();
        long edPosn = jumpData.GetPosition();
        wxString msg = wxString::Format(_T("[%d][%s][%ld]"), count, edFilename.c_str(), edPosn);
        if (count == (size_t)m_Cursor)
            msg.Append(_T("<--"));
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
