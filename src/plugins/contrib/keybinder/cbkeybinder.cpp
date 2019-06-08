/***************************************************************
 * Name:      cbkeybinder.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Pecan
 * Copyright: (c) Pecan Heber etal.
 * License:   GPL
 **************************************************************/
// RCS-ID:      $Id$

// The majority of this code was lifted from wxKeyBinder and
// its "minimal.cpp" sample program

// Modified CodeBlocks KeyBnder v2.0 2019/04/8

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation "cbkeybinder.h"
#endif

#include <vector>

#include <sdk.h>
#ifndef CB_PRECOMP
    #include <wx/dynarray.h>
    #include <wx/intl.h>
    #include <wx/log.h>
    #include <wx/sizer.h>
    #include <wx/stdpaths.h>

    #include "cbeditor.h"
    #include "configmanager.h"
    #include "editormanager.h"
    #include "manager.h"
    #include "pluginmanager.h"
    #include "personalitymanager.h"
#endif

#include <wx/xrc/xmlres.h>
#include <wx/fileconf.h>
#include <wx/event.h>
#if defined(__WXMSW__) && wxCHECK_VERSION(3, 0, 0)
    #include <wx/msw/private/keyboard.h>
#endif
#include <wx/listbook.h>

#include "logmanager.h"
#if defined(LOGGING)
    #include "debugging.h"
#endif
#include "menuutils.h"
#include "cbkeybinder.h"
#include "clKeyboardManager.h"
#include "cbKeyConfigPanel.h"


// ----------------------------------------------------------------------------
// Register the plugin
// ----------------------------------------------------------------------------
namespace
{
    PluginRegistrant<cbKeyBinder> reg(_T("cbKeyBinder"));

    #if wxVERSION_NUMBER < 3000
    int wxEVT_LISTBOOK_PAGE_CHANGED = wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED; //wx2.8
    #endif // wxVERSION_NUMBER
    int idKeyBinderRefresh = XRCID("idKeyBinderRefresh"); //(pecan 2019/04/26)
    wxString sep = wxFileName::GetPathSeparator();
    inline bool wxFound(int result){return result != wxNOT_FOUND;}
};
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(cbKeyBinder, cbPlugin)
    // add events here...

    EVT_MENU (idKeyBinderRefresh, cbKeyBinder::OnKeyBinderRefreshRequested)

END_EVENT_TABLE()
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
cbKeyBinder::cbKeyBinder()
// ---------------------------------------------------------------------------
{
    //ctor

    m_UserPersonality       = wxEmptyString;
    m_menuPreviouslyBuilt   = false;
    m_pUsrConfigPanel       = nullptr;
    m_AppStartupDone        = false;
    m_KeyBinderRefreshRequested = false;
}
// ----------------------------------------------------------------------------
cbKeyBinder::~cbKeyBinder()
// ----------------------------------------------------------------------------
{
    //dtor
    //-if (m_pUsrConfigPanel)
    //-    wxDELETE(m_pUsrConfigPanel); looks like it's already been deleted
    //          when EditorSettingDlg ends in CB main.cpp?
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnAttach()
// ----------------------------------------------------------------------------
{
    // do whatever initialization you need for your plugin
    // :NOTE: after this function, the inherited member variable
    // IsAttached() will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* ""
    // (see: does not need) this plugin...


    //get window for log when debugging
    pcbWindow = Manager::Get()->GetAppWindow();
    m_pKBMgr  = clKeyboardManager::Get();


    #if LOGGING
        // allocate wxLogWindow in the header
        //  wxLogWindow* pMyLog;
        // #define LOGIT wxLogMessage
        /* wxLogWindow* */
        wxLog::EnableLogging(true);
        m_pMyLog = new wxLogWindow(pcbWindow,wxT("KeyBinder"),true,false);
        wxLog::SetActiveTarget(m_pMyLog);
        wxRect logRect = m_pMyLog->GetFrame()->GetSize();
        logRect.width = logRect.width << 1;
        m_pMyLog->GetFrame()->SetSize(logRect);
        LOGIT(_T("keybinder v2.0 log open"));
        m_pMyLog->Flush();
        m_pMyLog->GetFrame()->Move(20,20);
    #endif

    m_bAppShutDown = false;

    // Set current plugin version
    PluginInfo* pInfo = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(this));
    pInfo->version = wxT(VERSION);

    // Get CodeBlocks executable last modification time
    wxFileName fnExecFile(wxStandardPaths::Get().GetExecutablePath());
    wxDateTime cbExeTimeStamp = fnExecFile.GetModificationTime();
    m_cbExeTimeStampstr = cbExeTimeStamp.Format(wxT("%y%m%d_%H%M%S"), wxDateTime::UTC );

    // register event sink
    Manager::Get()->RegisterEventSink(cbEVT_APP_STARTUP_DONE, new cbEventFunctor<cbKeyBinder, CodeBlocksEvent>(this, &cbKeyBinder::OnAppStartupDone));
    //-Manager::Get()->RegisterEventSink(cbEVT_APP_START_SHUTDOWN, new cbEventFunctor<cbKeyBinder, CodeBlocksEvent>(this, &cbKeyBinder::OnAppStartShutdown));
    return;

}//OnAttach
// ----------------------------------------------------------------------------
void cbKeyBinder::OnRelease(bool /*appShutDown*/)
// ----------------------------------------------------------------------------
{
    // Do de-initialization for your plugin
    // if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NB: after this function, the inherited member variable
    // IsAttached() will be FALSE...
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnConfigListbookClose(wxEvent& event)
// ----------------------------------------------------------------------------
{
    // The Editor configuration dialog is being destroyed.
    // Remove this routines connected events to that dialog.


    wxWindow* pWindow = (wxWindow*)(event.GetEventObject());
    if (pWindow == m_pConfigListbook)
    {
        pWindow->GetEventHandler()->Disconnect(XRCID("nbMain"), wxEVT_LISTBOOK_PAGE_CHANGED, wxListbookEventHandler( cbKeyBinder::OnConfigListbookEvent), NULL, this);
        pWindow->GetEventHandler()->Disconnect(XRCID("nbMain"), wxEVT_DESTROY, wxEventHandler( cbKeyBinder::OnConfigListbookClose), NULL, this);
        // Dont event.Skip(). causes crash
        return;
    }

    event.Skip();

}//OnWindowClose
// ----------------------------------------------------------------------------
void cbKeyBinder::OnConfigListbookEvent(wxListbookEvent& event)
// ----------------------------------------------------------------------------
{
    // This event occurs when the user clicks on MainMenu/Settings/Editor/Keyboard shortccuts .
    // This routine will call phaseII to complete the setting dialog
    // by scanning the menu structure and merging the user defined shortcuts.
    // It will then display a menu tree and allow the user to modify the menu and global key shortcuts.

    event.Skip();

    if ( (event.GetEventType() == wxEVT_LISTBOOK_PAGE_CHANGED))
    {
        int sel = event.GetSelection();
        wxListbook* plb = (wxListbook*)event.GetEventObject();
        wxString label = plb->GetPageText(sel);
        if (label == _("Keyboard shortcuts") )
        {
            wxMenuBar* pMenuBar = Manager::Get()->GetAppFrame()->GetMenuBar();
            m_pUsrConfigPanel->GetKeyConfigPanelPhaseII(pMenuBar, m_pUsrConfigPanel, m_mode);
        }
    }

    return;
}
// ----------------------------------------------------------------------------
//  cbKeyBinder GetConfigurationPanel()  //phaseI
// ----------------------------------------------------------------------------
cbConfigurationPanel* cbKeyBinder::GetConfigurationPanel(wxWindow* parent)
{
    // This routine will create a minimal configuration panel for
    // the Editor configuration 'Keyboard shortcuts' dialog. The actual work
    // for this panel will done in GetConfigurationPanePhaseII() if the user
    // clicks on MainMenu\Settings\Editor\Keyboard shortcuts.
    // See OnConfigListbookEvent().

    //create and display the configuration dialog for your plugin
    if(not IsAttached()) { return nullptr;}

    // Note : parent == wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    // cf., editorconfiguration.cpp
    m_pConfigListbook = (wxListbook*)parent;
    parent->GetEventHandler()->Connect(XRCID("nbMain"), wxEVT_LISTBOOK_PAGE_CHANGED, wxListbookEventHandler( cbKeyBinder::OnConfigListbookEvent), NULL, this);
    parent->GetEventHandler()->Connect(XRCID("nbMain"), wxEVT_DESTROY, wxEventHandler( cbKeyBinder::OnConfigListbookClose), NULL, this);

    // Create a Configurtion panel and return it to CodeBlocks
    // The commented lines below are from the original wxKeyBinder
    // They may be useful later

    //bool btree = GetMenuBar()->IsChecked(Minimal_UseTreeCtrl);
    bool btree = true;
    //bool baddprofile = GetMenuBar()->IsChecked(Minimal_ShowAddRemoveProfile);
    bool baddprofile = false;
    //bool bprofiles = GetMenuBar()->IsChecked(Minimal_ShowKeyProfiles);
    //-bool bprofiles = false;
    //bool bprofileedit = GetMenuBar()->IsChecked(Minimal_EnableProfileEdit);
    bool bprofileedit = false;
    // setup build flags
    int mode = btree ? wxKEYBINDER_USE_TREECTRL : wxKEYBINDER_USE_LISTBOX;
    if (baddprofile) mode |= wxKEYBINDER_SHOW_ADDREMOVE_PROFILE;
    if (bprofileedit) mode |= wxKEYBINDER_ENABLE_PROFILE_EDITING;
    //-mode |= wxKEYBINDER_SHOW_APPLYBUTTON; no need for this in CodeBlocks
    m_mode = mode;

    m_pUsrConfigPanel = new UsrConfigPanel(parent, wxT("Keybindings"), mode);

    return m_pUsrConfigPanel;
}
// ----------------------------------------------------------------------------
//  cbKeyBinder BuildMenu
// ----------------------------------------------------------------------------
void cbKeyBinder::BuildMenu(wxMenuBar* menuBar)
{
    //The application is offering its menubar for your plugin,
    //to add any menu items you want...
    //Append any items you need in the menu...
    //NB: Be careful in here... The application's menubar is at your disposal.
    //-NotImplemented(_T("cbKeyBinder::OfferMenuSpace()"));

    if(not IsAttached()) { return; }

    // This routine may be called when another plugin modifies the menu.
    // or codeblocks disables another plugin and rebuilds the menu bar
    if ( m_menuPreviouslyBuilt )
    {
        #if defined(LOGGING)
         LOGIT( _T("KeyBinder re-entered at BuildMenu") );
         LOGIT( _T("OldMenuBar[%p] NewMenuBar[%p]"), m_pMenuBar, menuBar );
        #endif
    }
    m_menuPreviouslyBuilt = true;

    // init the keybinder
    // memorize incomming menubar
    m_pMenuBar = menuBar;

    // get version number from keybinder plugin
    const PluginInfo* info = Manager::Get()->GetPluginManager()->GetPluginInfo(this);
    wxString pluginVersion = info->version.BeforeLast('.');

    // remove the dots from version string (using first 3 chars)
    pluginVersion.Replace(_T("."),_T(""));

    // get the CodeBlocks "personality" argument
    m_UserPersonality = Manager::Get()->GetPersonalityManager()->GetPersonality();
    #if defined(LOGGING)
        LOGIT( _T("m_UserPersonality is[%s]"), m_UserPersonality.GetData() );
    #endif

    m_OldKeyBinderFullFilePath = wxStandardPaths::Get().GetUserDataDir();
    m_OldKeyBinderFullFilePath = m_OldKeyBinderFullFilePath + wxFILE_SEP_PATH;
    m_OldKeyBinderFullFilePath << m_UserPersonality + wxT(".cbKeyBinder10.ini") ;
    if (not wxFileExists(m_OldKeyBinderFullFilePath)) m_OldKeyBinderFullFilePath = wxEmptyString;

    #if LOGGING
        if (m_OldKeyBinderFullFilePath.Len())
            LOGIT(_T("cbKB:BuildMenu:OldKeyBinderFile:%s"),m_OldKeyBinderFullFilePath.GetData());
    #endif

    return ;

}//BuildMenu
// ----------------------------------------------------------------------------
void cbKeyBinder::BuildModuleMenu(const ModuleType /*type*/, wxMenu* /*menu*/, const FileTreeData* /*data*/)
// ----------------------------------------------------------------------------
{
    //Some library module is ready to display a pop-up menu.
    //Check the parameter "type" and see which module it is
    //and append any items you need in the menu...
    //TIP: for consistency, add a separator as the first item...
    //-v0.1--NotImplemented(_T("cbKeyBinder::OfferModuleMenuSpace()"));
    if(not IsAttached()) { return; }
    return;
}
// ----------------------------------------------------------------------------
bool cbKeyBinder::BuildToolBar(wxToolBar* /*toolBar*/)
// ----------------------------------------------------------------------------
{
    //The application is offering its toolbar for your plugin,
    //to add any toolbar items you want...
    //Append any items you need on the toolbar...
    //NotImplemented(_T("cbKeyBinder::BuildToolBar()"));
    // return true if you add toolbar items
    if(not IsAttached()) { return false; }
    return false;
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnAppStartupDone(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // KeyBinder initialization is being run here in order to catch any new or deleted
    // accelerators inserted by other plugin initialization.

    // ----------------------------------------------------------------------------
    // If no cbKeyBinder??.conf, create it from the menu structure
    // ----------------------------------------------------------------------------
    // Create %appdata%\<appName>\<profile>.cbKeyBinder??.conf if it does not exist.
    // keyMnuAccels.conf contains the default accerators created by walking the menu structure.
    // %appdata%\<personality>.cbKeyBinder??.ini contains the key bindings from the previous version of this plugin.

    wxFileName fnKeyBindings(wxStandardPaths::Get().GetUserDataDir(), _T("cbKeyBinder20.conf"));
    fnKeyBindings.SetName(GetUserPersonality() +_T(".") + fnKeyBindings.GetName());

    bool isRefreshRequest = (event.GetId() == idKeyBinderRefresh);
    if (not fnKeyBindings.FileExists() )
        isRefreshRequest = false;

    // Create default keybindings file keyMnuAccels.conf by scanning the app menus.
    bool created = CreateKeyBindDefaultFile(isRefreshRequest);
    wxUnusedVar(created);

    // Load the menu structure accerators + the user defined accelerators.
    // if no existing cbKeyBinder??.conf, ignore the refresh request to do full keybindings.conf creation.
    clKeyboardManager::Get()->Initialize(isRefreshRequest);

    m_AppStartupDone = true;
    m_KeyBinderRefreshRequested = false;

    return;
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnAppStartShutdown(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // Currently this is defined in the sdk, but not implemented
    // and never called. Another sdk gotcha! And another reason to avoid it.

    wxUnusedVar(event);
    #if defined(LOGGING)
     LOGIT( _T("OnAppStartShutdown") );
    #endif
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnKeyBinderRefreshRequested(wxCommandEvent& event)    //(pecan 2019/04/26)
// ----------------------------------------------------------------------------
{
    // a process has issued: wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("idKeyBinderRefresh"))

    if (m_KeyBinderRefreshRequested)
        return; //already doing the job

    m_KeyBinderRefreshRequested = true;

    if (m_AppStartupDone)
    {
        CodeBlocksEvent cbevt(event.GetEventType(), event.GetId());
        OnAppStartupDone(cbevt);
        m_KeyBinderRefreshRequested = false;
    }
}
// ----------------------------------------------------------------------------
wxString cbKeyBinder::GetPluginVersion()
// ----------------------------------------------------------------------------
{
    // get version number of keybinder plugin
    const PluginInfo* info = Manager::Get()->GetPluginManager()->GetPluginInfo(this);
    wxString pluginVersion = info->version.BeforeLast('.');

    // remove the dots from version string (using first 3 chars only)
    pluginVersion.Replace(_T("."),_T(""));

    return pluginVersion;
}
// ----------------------------------------------------------------------------
bool cbKeyBinder::CreateKeyBindDefaultFile(bool refresh)
// ----------------------------------------------------------------------------
{
    // Create %temp%\<profile>.keyMnuAccels.conf default accelerators by:
    // 1) Scan the menu structure to create keyOldFmtMnuScan.ini as a comparison base.
    // 2) if no %appdata%\<personality>.cbKeyBinder20.conf, try to merge old plugins' cbKeyBinder10.ini file
    // 3) if cbKeyBinder10.ini gets errors, use keyOldFmtMnuScan.ini to create keyMnuAccels.conf
    // 4) convert the results from above to %temp%\<personality>.keyMnuAccels.conf
    // The keyMnuAccels.conf file will, later, be merged with cbkeybinder20.conf to catch added/deleted menu items
    //      cf: clKeyboardManager::Initialize() .

    // pseudo code
    // create keyOldFmtMnuScan.ini by scanning the menu structure
    // if (exists old plugin version of cbKeyBinder10.ini) {
    //     copy keyOldFmtMnuScan.ini to keyMnuAccels.conf
    //     merge cbKeyBinder10.ini into keyMnuAccels.conf.
    //     if ok, return
    // }
    // else {
    //     convert keyOldFmtMnuScan into keyMnuAccels.conf
    //     return
    // }


    // GetDataDir() returns the directory where the executable file is located
    // GetUserDataDir() returns ...\%appdata%\<thisAppName>

    // userPersonality comes from the CodeBlocks /p argument or else set to 'default'
    wxFileName fnTempKeyMnuAccels(wxStandardPaths::Get().GetTempDir(), _T("keyMnuAccels.conf"));
    fnTempKeyMnuAccels.SetName(GetUserPersonality() + _T(".") + fnTempKeyMnuAccels.GetName());

    // cbKeyBinder20.conf == defaults key bindings + user key bindings (filename is prefixed with userPersonality.)
    wxFileName fnNewcbKeyBinderConf(wxStandardPaths::Get().GetUserDataDir(), _T("cbKeyBinder20.conf"));
    fnNewcbKeyBinderConf.SetName(GetUserPersonality() + _T(".") +fnNewcbKeyBinderConf.GetName());

    #if defined(LOGGING)
        LOGIT( _T("fnTempKeyMnuAccels[%s]"),   fnTempKeyMnuAccels.GetFullPath().wx_str());
        LOGIT( _T("fnNewcbKeyBinderConf[%s]"), fnNewcbKeyBinderConf.GetFullPath().wx_str());
    #endif

    #if defined(LOGGING) //debugging
        wxString look = fnTempKeyMnuAccels.GetFullPath();
        LOGIT( _T("CreateKeyBindDefaultFile()[%s]"), look.wx_str());
    #endif

    bool ok = false;

    // Remove any stale %temp%\keyOldFmtMnuScan.ini or %temp%\keyMnuAccels.conf
    wxFileName fnTempOldFmtMnuScan(wxStandardPaths::Get().GetTempDir(), _T("keyOldFmtMnuScan.ini"));
    if (fnTempOldFmtMnuScan.FileExists())
        wxRemoveFile(fnTempOldFmtMnuScan.GetFullPath());
    if (fnTempKeyMnuAccels.FileExists())
        wxRemoveFile(fnTempKeyMnuAccels.GetFullPath());

    // ----------------------------------------------------------------------------
    // Use KeyBinder menu walker to create old format .ini file of menu structure
    // ----------------------------------------------------------------------------
    // old format ...\tempDir\keyOldFmtMnuScan.ini will be converted to new codelite format keyMnuAccels.conf file
    // we need this older format to compare against cbKeyBinder10.ini file to convert old user key bindings
    if (not fnTempOldFmtMnuScan.FileExists())
    {
        // start the menu walker process
        wxKeyProfileArray* pKeyProfileArray = new wxKeyProfileArray();
        wxKeyProfile* pPrimary = new wxKeyProfile(_("Primary"), _("Primary keyprofile"));

        // remove keyprofiles from our array
        for (int i=0; i<pKeyProfileArray->GetCount();i++)
          pKeyProfileArray->Remove(pKeyProfileArray->Item(i));
        // clear out old array
        pKeyProfileArray->Cleanup();

        wxMenuCmd::Register(m_pMenuBar);
        pKeyProfileArray->Add(pPrimary);

        // Set profile 0 as the default primary keybinder
        pKeyProfileArray->SetSelProfile(0);
        // Import menu items by walking the menu structure
        pPrimary->ImportMenuBarCmd(m_pMenuBar);
        // Save the resulting keys definitions to old format keybinder20.conf
        ok = OnSaveKbOldFormatCfgFile(pKeyProfileArray, fnTempOldFmtMnuScan.GetFullPath(), false);

        if (pPrimary)
        {
            pKeyProfileArray->Remove(pPrimary);
            wxDELETE( pPrimary);
        }
        if (pKeyProfileArray)
            wxDELETE( pKeyProfileArray);
    }

    // If %appdata%\<personality>.cbKeybinder??.conf does not exist
    // try to convert the previous keybinder plugin versions' keybindings file if available,
    // currently (as of 2019/05/22) at %appdata%\<appName>\userPersonality.cbKeyBinder10.ini
    // The 'refresh' parm refers to a request from external processes to capture new key bindings
    int missingMenuItems = 0;
    if ( (not fnNewcbKeyBinderConf.FileExists()) and (not refresh) )
    {
        wxString plgnVersionString = GetPluginVersion();
        long plgnVersionNum; plgnVersionString.ToLong(&plgnVersionNum);
        int oldPlgnVersionNum = plgnVersionNum - 10;

        wxString oldVersionFile = wxString::Format(_T("cbKeyBinder%d.ini"), oldPlgnVersionNum);
        wxFileName fnOldVersionKeyBindings(wxStandardPaths::Get().GetUserDataDir(), _T("cbKeyBinder10.ini"));
        fnOldVersionKeyBindings.SetName(GetUserPersonality() +_T(".") + fnOldVersionKeyBindings.GetName());

        ok = fnOldVersionKeyBindings.FileExists();
        if (ok)
        {   //convert a copy of cbKeybinder??.ini to temp\<personality>.keyMnuAccels.conf
            ok = wxCopyFile(fnTempOldFmtMnuScan.GetFullPath(), fnTempKeyMnuAccels.GetFullPath());
            if (ok)
            {
                missingMenuItems = //convert eg., old cbKeyBinder10.ini to temp\keyMnuAccels.conf
                    ConvertOldKeybinderIniToAcceratorsConf(fnOldVersionKeyBindings.GetFullPath(), fnTempKeyMnuAccels.GetFullPath());
                wxRemoveFile(fnTempOldFmtMnuScan.GetFullPath());
                return (missingMenuItems != -1);
            }
        }
    }//endif no keybindings.conf

    // Too many errors or missing menu items, old cbKeyBinder10.ini not a good match for current menu structure
    if ( (not ok) or (missingMenuItems != 0) )
    {
        wxString msg = _("Preserving older key bindings failed.\nWill create key bindings from current menu structure.");
        #if defined(LOGGING)
            LOGIT( msg );
        #endif
        Manager::Get()->GetLogManager()->LogWarning(msg);
    }
    // ----------------------------------------------------
    // Convert %temp%\keyOldFmtMnuScan.ini to codelite format %temp%\<personality>.keyMnuAccels.conf
    // ----------------------------------------------------
    if (fnTempOldFmtMnuScan.FileExists())
    {
        //Convert menu structure scan into codelite format file keyMnuAccers.conf
        ConvertMenuScanToKeyMnuAcceratorsConf(fnTempOldFmtMnuScan.GetFullPath(), fnTempKeyMnuAccels.GetFullPath());
    }

    return true;
}
// ----------------------------------------------------------------------------
bool cbKeyBinder::OnSaveKbOldFormatCfgFile(wxKeyProfileArray* pKeyProfArr, wxString oldFmtMnuScanFilePath, bool backitup)
// ----------------------------------------------------------------------------
{
    // Save the key profile(s) to a file
    // GetDataDir() returns the directory where the executable file is located
    // GetUserDataDir() returns the %appdata% directory
    // GetTempDir() returns system temporary directory

    wxUnusedVar(backitup);
    wxFileName fnKeyBinderCfg(oldFmtMnuScanFilePath);

    wxString m_KeyBinderCfgFilename = fnKeyBinderCfg.GetFullPath();
    // Remove any old temp key definition file
    if (fnKeyBinderCfg.FileExists())
        ::wxRemoveFile(m_KeyBinderCfgFilename);

    // signature of wxConfigBase:
    //wxConfigBase(const wxString& appName = wxEmptyString, const wxString& vendorName = wxEmptyString,
    //                const wxString& localFilename = wxEmptyString, const wxString& globalFilename = wxEmptyString,
    //                 long style = 0, wxMBConv& conv = wxConvUTF8)

    wxFileConfig* cfg = new wxFileConfig(wxEmptyString, wxEmptyString,
                             m_KeyBinderCfgFilename, m_KeyBinderCfgFilename
                             );
    bool ok = false;
    if ((ok = pKeyProfArr->Save(cfg, wxEmptyString, TRUE)))
     {
        // get the cmd count
        int total = 0;
        for (int i=0; i<pKeyProfArr->GetCount(); i++)
            total += pKeyProfArr->Item(i)->GetCmdCount();
        cfg->Flush();
        #if defined(LOGGING)
            LOGIT(wxString::Format(wxT("All the [%d] keyprofiles ([%d] commands ")
            wxT("in total) have been saved in \n") + fnKeyBinderCfg.GetFullPath(),
              pKeyProfArr->GetCount(), total) );
        #endif

    }//endif Save
    else
    {
        ok = false;
        wxString msg = wxString::Format(_("Keybinder:Error saving menu scan key file %s"), m_KeyBinderCfgFilename.wx_str());
        Manager::Get()->GetLogManager()->Log(msg);
        wxMessageBox(msg, _("KeyBinder Save Error"), wxOK | wxICON_ERROR);
    }
    if (cfg)
        wxDELETE( cfg);

    return ok;

}//OnSaveKbOldFormatCfgFile
// ----------------------------------------------------------------------------
int cbKeyBinder::ConvertMenuScanToKeyMnuAcceratorsConf(wxString keybinderFile, wxString acceleratorFile)
// ----------------------------------------------------------------------------
{
    // Convert menu walker keyOldFmtMnuScan.ini to CodeLite format accerators.conf (keyMnuAccels.conf)
    // keyMnuAccels.conf will later be used to create/modify ...\%appdata%\<personality>.cbKeyBinder??.conf .
    // arg acceratorFile must be full Path
    // return -1 on open error
    // returns number of non-matching menu items between menu structure and keybinder file

    wxFileName fncbkb(keybinderFile);       // usually ...\%TEMP%\keyOldFmtMnuScan.ini
    wxFileName fnclacc(acceleratorFile);    // usually ...\%TEMP%\<personality>.keyMnuAccel.conf

    if (not fncbkb.FileExists())
    {
        wxASSERT_MSG(0, _("ConvertMenuScanToKeyMnuAcceratorsConf() called, but file does not exist."));
        return false;
    }
    // remove old KeyMnuAccels.conf
    if (fnclacc.FileExists())
        wxRemoveFile(fnclacc.GetFullPath());

    wxTextFile txtkb(fncbkb.GetFullPath());
    txtkb.Open();

    wxTextFile txtacc(fnclacc.GetFullPath());
    if (not txtacc.Create() )
    {
        wxASSERT_MSG(0, _("ConvertMenuScanToKeyMnuAcceratorsConf() failed to create "+acceleratorFile) );
    }
    if (not txtacc.Open())
    {
        wxASSERT_MSG(0, _("ConvertMenuScanToKeyMnuAcceratorsConf() failed to open "+acceleratorFile) );
        return -1;
    }

    size_t lineKnt = txtkb.GetLineCount();
    if (not lineKnt) return false;
    wxArrayString lineItems;
    size_t missingMenuItems = 0;
    for (size_t lineNo = 0; lineNo < lineKnt; ++lineNo)
    {
        wxString lineTxt = txtkb.GetLine(lineNo);
        if (not lineTxt.StartsWith(_T("bind"))) continue;
        lineTxt = lineTxt.Mid(4); //remove "bind"
        lineTxt.Replace(_T("-type4660="), _T("|")); //remove type
        lineTxt.Replace(_T("\\"), _T(":"));
        lineItems.Empty();
        lineItems = GetArrayFromStrings(lineTxt, _T("|"), true);
        #if defined(LOGGING)
            wxString look = GetStringsFromArray(lineItems, _T("|"), false);
            LOGIT( _T("ConvertkbToAcc[%s]"), look.wx_str());
        #endif
        // Dont pass subMenu place holders to the KeyMnuAccels.conf file.
        // Dont pass menu items that no longer exist.
        if (lineItems[0].IsNumber() )
        {
            long itemID; lineItems[0].ToLong(&itemID);
            wxMenuItem* pMnuItem = m_pMenuBar->FindItem(itemID);
            if (pMnuItem and pMnuItem->GetSubMenu())
                continue;
            if (not pMnuItem)
            {
                ++missingMenuItems;
                #if defined(LOGGING)
                LOGIT( _T("MissingOldMenuItem[%s]"), lineTxt.wx_str());
                #endif
                continue;
            }
            txtacc.AddLine(GetStringsFromArray(lineItems,_T("|"), true));
        }
    }

    if (txtkb.IsOpened())
        txtkb.Close();

    if (txtacc.IsOpened())
    {
        txtacc.Write();
        txtacc.Close();
    }

    if (missingMenuItems)
    {
        wxString msg = wxString::Format(_("Convert found %u unmatched menu items."), (unsigned)missingMenuItems);
        #if defined(LOGGING)
            //cbMessageBox(msg, _("Converter"), wxOK, Manager::Get()->GetAppWindow());
        #endif
        return missingMenuItems;
    }

    // Merge the CodeBlocks accerator table into the keyMnuAccels file.
    MergeAcceleratorTable(txtacc );

    return 0;

}//ConvertMenuScanToKeyMnuAcceratorsConf
// ----------------------------------------------------------------------------
int cbKeyBinder::ConvertOldKeybinderIniToAcceratorsConf(wxString oldKeybinderFile, wxString newAcceleratorFile)
// ----------------------------------------------------------------------------
{
    // Convert cbKeyBinder10.ini to CodeLite format accerators.conf (keyMnuAccels.conf)
    // arg acceratorFile must be full Path
    // return -1 on errors
    // returns number of non-matching menu items between menu structure and keybinder file

    // eg., older cbKeyBinder10.ini file to ...\temp\<personality>.keyMnuAccels.conf
    wxFileName fncbkb(oldKeybinderFile);
    if (not fncbkb.FileExists())
    {
        wxASSERT_MSG(0, wxString::Format(_("ConvertKeybinderIni called, but %s file does not exist."), fncbkb.GetFullPath().wx_str()) );
        return -1;
    }
    wxTextFile txtkb(fncbkb.GetFullPath());
    if (not txtkb.Open() )
    {
        wxASSERT_MSG(0, _("ConvertKeybinderIni failed to open " + oldKeybinderFile) );
        return -1;
    }

    // Verify .conf which is usually at tempDir\<personality>.keyMnuAccels.conf
    wxFileName fnclacc(newAcceleratorFile);
    if (not fnclacc.FileExists())
    {
        wxASSERT_MSG(0, wxString::Format(_("ConvertKeybinderIni called, but %s file does not exist."), fnclacc.GetFullPath().wx_str()) );
        return -1;
    }
    wxTextFile txtacc(fnclacc.GetFullPath());
    if (not txtacc.Open())
    {
        wxASSERT_MSG(0, _("ConvertKeybinderIni failed to open " + newAcceleratorFile) );
        return -1;
    }

    #if defined(LOGGING)
    {
        size_t lineKnt = txtkb.GetLineCount();
        LOGIT( _T("ConvertKeybinderIni Open input txtKB[%s]Lines[%u]"), fncbkb.GetFullPath().wx_str(), (unsigned)lineKnt);
        lineKnt = txtacc.GetLineCount();
        LOGIT( _T("ConvertKeybinderIni Open output txtACC[%s] Lines[%u]"), fnclacc.GetFullPath().wx_str(), (unsigned)lineKnt);
    }
    #endif

    size_t lineKnt = txtkb.GetLineCount();
    if (not lineKnt) return false;
    wxArrayString lineItems;
    size_t missingMenuItems = 0;
    for (size_t lineNo = 0; lineNo < lineKnt; ++lineNo)
    {
        wxString kbLineTxt = txtkb.GetLine(lineNo);
        if (not kbLineTxt.StartsWith(_T("bind"))) continue;
        kbLineTxt = kbLineTxt.Mid(4); //remove "bind"
        kbLineTxt.Replace(_T("-type4660="), _T("|")); //remove type
        lineItems.Empty();
        lineItems = GetArrayFromStrings(kbLineTxt, _T("|"), true);
        #if defined(LOGGING)
            wxString look = GetStringsFromArray(lineItems, _T("|"), false);
            LOGIT( _T("ConvertOldKBToAcc[%s]"), look.wx_str());
        #endif
        // Dont pass subMenu place holders to the KeyMnuAccels.conf file
        // Dont pass menu items that no longer exist
        if (lineItems[0].IsNumber() )
        {
            long itemID; lineItems[0].ToLong(&itemID);
            size_t parts = lineItems.GetCount();
            wxString kbParentMenu =  (parts > 1) ? lineItems[1] : _T("");
            wxString kbDescription = (parts > 2) ? lineItems[2] : _T("");
            wxString kbShortcut1 =   (parts > 3) ? lineItems[3] : _T("");
            wxString kbShortcut2 =   (parts > 4) ? lineItems[4] : _T("");
            // Find line in new menu file containing old parent menu
            // and replace its shortcuts with the users old version shortcuts
            int  accMnuLineNum = FindLineInFileContaining(txtacc, kbParentMenu);
            if (accMnuLineNum == wxNOT_FOUND)
            {   ++missingMenuItems;
                #if defined(LOGGING)
                    LOGIT( _T("MissingOldMenuItem[%s]"), kbLineTxt.wx_str());
                #endif
                continue;
            }
            // Skip oldKB MnuLines which represent submenu anchors or no longer exist
            wxString accMnuLine = txtacc.GetLine(accMnuLineNum);
            size_t nPos  = accMnuLine.Find(_T("-type"));
            if (not wxFound(nPos)) continue;
            wxString accMnuIDstr = accMnuLine.Mid(0,nPos); //get 'bind'+menuID
            accMnuIDstr = accMnuIDstr.Mid(4);              //remove 'bind'
            long accMnuID; accMnuIDstr.ToLong(&accMnuID);
            wxMenuItem* pMnuItem = m_pMenuBar->FindItem(accMnuID);
            if (not pMnuItem) continue;
            if (pMnuItem->GetSubMenu()) continue;

            // Set the default menu accelerator to the oldKB .ini file accelerator
            nPos = accMnuLine.find(kbParentMenu);
            nPos += (kbParentMenu.Length() + kbDescription.Length() + 1);
            if (kbShortcut1.Length())
            {
                accMnuLine = accMnuLine.Mid(0, nPos);   //whack off the accelerators
                accMnuLine += _T("|") + (kbShortcut1 + _T("|") );
                txtacc[accMnuLineNum] = accMnuLine;
                if (parts > 4) //Global accerator follows menu accelerator in oldKB line
                {
                    // create eg., -31844||<global>Close file|Ctrl-F4
                    wxString global = accMnuLine;
                    // menu id is between "bind" and "-type" strings
                    global = global.Mid(4);
                    global = global.BeforeFirst(_T('t'));
                    global.RemoveLast();
                    global  << _T("||<global>") + kbParentMenu + _T("|") +kbShortcut2;
                    global.Replace(_T("\\"), _T(":"));
                    txtacc.AddLine(global);
                }
            }
            else //clear accelerator since old keybinder accelerator was empty
            {
                accMnuLine = accMnuLine.Mid(0, nPos);
                accMnuLine += _T("|");
                txtacc[accMnuLineNum] = accMnuLine;
            }
        }//end if line has a menu id
    }//end for line num

    // Remove text strings not needed in new menu file format
    lineKnt = txtacc.GetLineCount();
    for (int lineNo = 0; lineNo < (int)lineKnt; ++lineNo)
    {
        wxString lineTxt = txtacc.GetLine(lineNo);
        if (not lineTxt.StartsWith(_T("bind")))
        {
            // skip global accelerators
            if (lineTxt.Contains(_T("||<global>")) )
                continue;
            txtacc.RemoveLine(lineNo);
            lineNo -= 1;
            lineKnt -= 1;
            continue;
        }
        lineTxt = lineTxt.Mid(4); //remove "bind"
        lineTxt.Replace(_T("-type4660="), _T("|")); //remove old type id
        lineTxt.Replace(_T("\\"), _T(":"));
        txtacc[lineNo] = lineTxt;
    }

    #if defined(LOGGING)
    lineKnt = txtkb.GetLineCount();
    LOGIT( _T("Closing txtKB Lines[%u]"), (unsigned)lineKnt);
    lineKnt = txtacc.GetLineCount();
    LOGIT( _T("Closing txtACC Lines[%u]"), (unsigned)lineKnt);
    #endif

    if (txtkb.IsOpened())
        txtkb.Close();

    if (txtacc.IsOpened())
    {
        txtacc.Write();
        txtacc.Close();
    }

    if (missingMenuItems)
    {
        wxString msg = wxString::Format(_T("KeyBinder Convert found %u unmatched menu items."), (unsigned)missingMenuItems);
        #if defined(LOGGING)
            LOGIT( _T("[%s]"), msg.wx_str());
        #endif
        //-return missingMenuItems; deprecated; ok if some items missing
    }

    // Merge CodeBlocks global AcceratorTable into the new keyMnuAccels.conf file.
    MergeAcceleratorTable(txtacc );

    return 0;

}//ConvertOldKeybinderIniToAcceratorsConf
// ----------------------------------------------------------------------------
bool cbKeyBinder::MergeAcceleratorTable(wxTextFile& textOutFile)
// ----------------------------------------------------------------------------
{
    // Get Codeblocks main.cpp Accelerator entries.

    // Verify output file exists.
    if (not textOutFile.IsOpened())
        if (not textOutFile.Open())
            return false;

    std::vector<wxAcceleratorEntry> globalAccels; //To be filled by main.cpp

    int idGetGlobalAccels   = XRCID("idGetGlobalAccels");
    wxCommandEvent getGlobalAccels(wxEVT_COMMAND_MENU_SELECTED, idGetGlobalAccels);
    getGlobalAccels.SetClientData(&globalAccels);
    wxTheApp->GetTopWindow()->GetEventHandler()->ProcessEvent(getGlobalAccels);
    int accelCount = globalAccels.size();

    #if defined(LOGGING)
    LOGIT( _T("MergeAcceleratorTable() found %d global accelerators."), accelCount);
    #endif

    if (not accelCount) return false;

    // Format each global accelerator to be acceptable by clKeyboardManager.
    for (int ii = 0; ii<accelCount; ++ii)
    {
        #if defined(LOGGING)
        wxString vFlags;
        if (globalAccels[ii].GetFlags() & wxACCEL_CTRL)
            vFlags += _T("Ctrl-");
        if (globalAccels[ii].GetFlags() & wxACCEL_ALT)
            vFlags += _T("Alt-");
        if (globalAccels[ii].GetFlags() & wxACCEL_SHIFT)
            vFlags += _T("Shift-");
        LOGIT( _T("accelEntry[%d]flags[%s]code[%s],id[%d]"),
                    ii,
                    vFlags.wx_str(),
                    m_pKBMgr->KeyCodeToString(globalAccels[ii].GetKeyCode()).wx_str(),
                    globalAccels[ii].GetCommand()
                  );
        #endif

        // Add an accelerator table entry to KeyMnuAccels.conf file
        wxMenuItem* pMenuItem = m_pMenuBar->FindItem(globalAccels[ii].GetCommand());
        wxString outputLine = wxString::Format(_T("%d"), globalAccels[ii].GetCommand());
        outputLine.Append(_T("||")); //empty parent means global accelerator
        outputLine += _T("<global>");
        if (pMenuItem)
            outputLine += pMenuItem->GetItemLabelText();
        else outputLine += _T("<unbound>");
        outputLine += _T("|");
        if (globalAccels[ii].GetFlags() & wxACCEL_SHIFT)
            outputLine.Append(_T("Shift-"));
        if (globalAccels[ii].GetFlags() & wxACCEL_CTRL)
            outputLine.Append(_T("Ctrl-"));
        if (globalAccels[ii].GetFlags() & wxACCEL_ALT)
            outputLine.Append(_T("Alt-"));
        outputLine += m_pKBMgr->KeyCodeToString(globalAccels[ii].GetKeyCode());

        // keep accelerators that also have no matching menu item
        textOutFile.AddLine(outputLine);

    }
    if (textOutFile.IsOpened())
    {
        textOutFile.Write();
        textOutFile.Close();
    }

    return true;
}
// ----------------------------------------------------------------------------
int cbKeyBinder::FindLineInFileContaining(wxTextFile& txtFile, wxString& pattern)
// ----------------------------------------------------------------------------
{
    size_t knt = txtFile.GetLineCount();
    for (size_t ii=0; ii<knt; ++ii)
    {
        if (txtFile.GetLine(ii).Contains(pattern) )
            return ii;
    }
    return wxNOT_FOUND;
}
// ----------------------------------------------------------------------------
int cbKeyBinder::RemoveKeyBindingsFor(const wxString& strKeyCode, wxKeyProfile* pKeyprofile)
// ----------------------------------------------------------------------------
{
    int knt = 0;
    wxCmd* p_wxCmd = 0;
    do{
        p_wxCmd = pKeyprofile->GetCmdBindTo(strKeyCode);
        if (p_wxCmd)
        {
            ++knt;
            pKeyprofile->RemoveCmd(p_wxCmd);
        }
    }while(p_wxCmd);
    return knt;
}
// ----------------------------------------------------------------------------
int cbKeyBinder::RemoveCopyPasteBindings(wxKeyProfile* pKeyProfile)
// ----------------------------------------------------------------------------
{
    // revert Copy/Paste/Cut Ctrl-C, Ctrl-V Ctrl-X to their native actions

    wxCmd* p_wxCmd = 0;
    int knt = 0;

    p_wxCmd = pKeyProfile->GetCmdBindTo(_T("Ctrl-C"));
    if (p_wxCmd && (p_wxCmd->GetName() == _T("Copy")))
        knt +=  RemoveKeyBindingsFor(_T("Ctrl-C"), pKeyProfile);

    p_wxCmd = pKeyProfile->GetCmdBindTo(_T("Ctrl-V"));
    if (p_wxCmd && (p_wxCmd->GetName() == _T("Paste")))
        knt +=  RemoveKeyBindingsFor(_T("Ctrl-V"),pKeyProfile);

    p_wxCmd = pKeyProfile->GetCmdBindTo(_T("Ctrl-X"));
    if (p_wxCmd && (p_wxCmd->GetName() == _T("Cut")))
        knt +=  RemoveKeyBindingsFor(_T("Ctrl-X"),pKeyProfile);

    return knt;
}
//// ----------------------------------------------------------------------------
//bool cbKeyBinder::VerifyKeyBind(const wxString& strKeyCode, const int numShortcuts)
//// ----------------------------------------------------------------------------
//{
//    // Verify There exists a wxCmd containing this shortcut
//
//    wxKeyProfile* pKeyProfile = m_pKeyProfArr->GetSelProfile();
//
//    //wxKeyBind keybind(entries[ii].GetFlags(), entries[ii].GetKeyCode());
//    bool kbOk = true;
//
//    wxCmd* pcmd = pKeyProfile->GetCmdBindTo(strKeyCode);
//    if (pcmd)
//    {
//        int shortcutsCount = pcmd->GetShortcutCount();
//        wxString desc = pcmd->GetDescription();
//        wxString name = pcmd->GetName();
//
//        #if defined(LOGGING)
//            int id = pcmd->GetId();
//            int type = pcmd->GetType();
//            LOGIT( _T("shortcut count for[%s]is[%d]type[%d]"),
//                  strKeyCode.wx_str(), shortcutsCount, type);
//        #endif
//
//        if (shortcutsCount not_eq numShortcuts)
//            kbOk = false;
//        for (int kk = 0; kk < shortcutsCount; ++kk )
//        {
//            wxKeyBind* pkbind = pcmd->GetShortcut(kk);
//            if (pkbind)
//            {   wxString strKeyCodeLoc = wxKeyBind::KeyCodeToString(pkbind->GetKeyCode());
//                if (pkbind->GetModifiers() & wxACCEL_SHIFT)
//                    strKeyCodeLoc.Prepend(_T("Shift-"));
//                if (pkbind->GetModifiers() & wxACCEL_CTRL)
//                    strKeyCodeLoc.Prepend(_T("Ctrl-"));
//                if (pkbind->GetModifiers() & wxACCEL_ALT)
//                    strKeyCodeLoc.Prepend(_T("Alt-"));
//                #if defined(LOGGING)
//                LOGIT( _T("keybind[%d.%d] for [%s] is [%s]"),
//                        id, kk, name.wx_str(), strKeyCodeLoc.wx_str());
//                #endif
//            }
//            else kbOk = false;
//        }//for kk
//    }//if
//    else kbOk = false;
//
//    return kbOk;
//}//VerifyKeyBind
// ----------------------------------------------------------------------------
wxString cbKeyBinder::FindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName)
// ----------------------------------------------------------------------------
{
    // Find the absolute path from where this application has been run.
    // argv0 is wxTheApp->argv[0]
    // cwd is the current working directory (at startup)
    // appVariableName is the name of a variable containing the directory for this app, e.g.
    // MYAPPDIR. This is checked first.

    wxString str;

    // Try appVariableName
    if (!appVariableName.IsEmpty())
    {
        str = wxGetenv(appVariableName);
        if (!str.IsEmpty())
            return str;
    }

#if defined(__WXMAC__) && !defined(__DARWIN__)
    // On Mac, the current directory is the relevant one when
    // the application starts.
    return cwd;
#endif

    wxString argv0Str = argv0;
    #if defined(__WXMSW__)
        do{
            if (argv0Str.Contains(wxT(".exe")) ) break;
            if (argv0Str.Contains(wxT(".bat")) ) break;
            if (argv0Str.Contains(wxT(".cmd")) ) break;
            argv0Str.Append(wxT(".exe"));
        }while(0);
    #endif

    if (wxIsAbsolutePath(argv0Str))
    {
        #if defined(LOGGING)
        LOGIT( _T("FindAppPath: AbsolutePath[%s]"), wxPathOnly(argv0Str).GetData() );
        #endif
        return wxPathOnly(argv0Str);
    }
    else
    {
        // Is it a relative path?
        wxString currentDir(cwd);
        if (currentDir.Last() != wxFILE_SEP_PATH)
            currentDir += wxFILE_SEP_PATH;

        str = currentDir + argv0Str;
        if (wxFileExists(str))
        {
            #if defined(LOGGING)
            LOGIT( _T("FindAppPath: RelativePath[%s]"), wxPathOnly(str).GetData() );
            #endif
            return wxPathOnly(str);
        }
    }

    // OK, it's neither an absolute path nor a relative path.
    // Search PATH.

    wxPathList pathList;
    pathList.AddEnvList(_("PATH"));
    str = pathList.FindAbsoluteValidPath(argv0Str);
    if (!str.IsEmpty())
    {
        #if defined(LOGGING)
        LOGIT( _T("FindAppPath: SearchPath[%s]"), wxPathOnly(str).GetData() );
        #endif
        return wxPathOnly(str);
    }

    // Failed
     #if defined(LOGGING)
     LOGIT(  _T("FindAppPath: Failed, returning cwd") );
     #endif
    return wxEmptyString;
    //return cwd;
}
// ----------------------------------------------------------------------------
wxString cbKeyBinder::GetStringsFromArray(const wxArrayString& array, const wxString& separator, bool SeparatorAtEnd)
// ----------------------------------------------------------------------------
{
    wxString out;
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        out << array[i];
        if (i < array.GetCount() - 1 || SeparatorAtEnd)
            out << separator;
    }
    return out;
}

// ----------------------------------------------------------------------------
wxArrayString cbKeyBinder::GetArrayFromStrings(const wxString& text, const wxString& separator, bool trimSpaces)
// ----------------------------------------------------------------------------
{
    wxArrayString out;
    wxString search = text;
    int seplen = separator.Length();
    while (true)
    {
        int idx = search.Find(separator);
        if (idx == -1)
        {
            if (trimSpaces)
            {
                search.Trim(false);
                search.Trim(true);
            }
            if (!search.IsEmpty())
                out.Add(search);
            break;
        }
        wxString part = search.Left(idx);
        search.Remove(0, idx + seplen);
        if (trimSpaces)
        {
            part.Trim(false);
            part.Trim(true);
        }
        //-if (!part.IsEmpty()) keybinder needs the empty strings
            out.Add(part);
    }
    return out;
}
