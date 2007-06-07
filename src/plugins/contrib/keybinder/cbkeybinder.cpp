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

#ifdef CB_PRECOMP
    #include <sdk.h>
#else
    #include "sdk_common.h"
    #include "sdk_events.h"
#endif

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma implementation "cbkeybinder.h"
#endif

#include "wxscintilla/include/wx/wxscintilla.h"
#include "cbkeybinder.h"
#include <licenses.h> // defines some common licenses (like the GPL)

#include "manager.h"
#include "messagemanager.h"

//#include <wx/datetime.h>

// ----------------------------------------------------------------------------
wxString* pKeyFilename = 0;     // used by keybinder key definition dialog
// ----------------------------------------------------------------------------

// Register the plugin
namespace
{
    PluginRegistrant<cbKeyBinder> reg(_T("cbKeyBinder"));
};

// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(cbKeyBinder, cbPlugin)
	// add events here...

	EVT_PROJECT_CLOSE   (cbKeyBinder::OnProjectClosed)
	EVT_EDITOR_OPEN     (cbKeyBinder::OnEditorOpen)
	EVT_EDITOR_CLOSE    (cbKeyBinder::OnEditorClose)
	EVT_PROJECT_OPEN    (cbKeyBinder::OnProjectOpened)
	EVT_APP_STARTUP_DONE(cbKeyBinder::OnAppStartupDone)
    EVT_APP_START_SHUTDOWN(cbKeyBinder::OnAppStartShutdown)
    EVT_MENUBAR_CREATE_BEGIN(cbKeyBinder::OnMenuBarModify) //never invoked
    EVT_MENUBAR_CREATE_END  (cbKeyBinder::OnMenuBarModify) //never invoked
    EVT_IDLE            (cbKeyBinder::OnIdle)
    EVT_TIMER           (-1, cbKeyBinder::OnTimerAlarm)
END_EVENT_TABLE()
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MyDialog, wxPanel)
END_EVENT_TABLE()
// ----------------------------------------------------------------------------
cbKeyBinder::cbKeyBinder()
// ---------------------------------------------------------------------------
    : m_Timer(this,0)
{
	//ctor
	m_menuPreviouslyBuilt = false;
}
// ----------------------------------------------------------------------------
cbKeyBinder::~cbKeyBinder()
// ----------------------------------------------------------------------------
{
	//dtor
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


    // Only stable windows can be attached for codeblocks; currently
    // "SCIwindow" & "notebook". See wxKeyBinder::Attach() in keybinder.cpp

    // Have to have at least one window to attach to else secondary keys wont work
    // and "notebook" windows work just fine. Dont need AppWindow attach
    m_menuPreviouslyBuilt = false;
    //get window for log when debugging
    pcbWindow = Manager::Get()->GetAppWindow();

    #if LOGGING
        // allocate wxLogWindow in the header
        //  wxLogWindow* pMyLog;
        // #define LOGIT wxLogMessage
        /* wxLogWindow* */
        pMyLog = new wxLogWindow(pcbWindow,wxT("KeyBinder"),true,false);
        wxLog::SetActiveTarget(pMyLog);
        LOGIT(_T("keybinder log open"));
        pMyLog->Flush();
        pMyLog->GetFrame()->Move(20,20);
    #endif

    // Allocate array but do actual key bindings after all menuitems have
    // been implemented by other plugins
	m_pKeyProfArr = new wxKeyProfileArray;
	m_bBound = FALSE;   //say keys are unbound to menus
	m_MenuModifiedByMerge = 0;
	m_bTimerAlarm = false;
	m_bAppShutDown = false;
	m_bConfigBusy = false;
	IsMerging(false);

	// Add window names to which keybinder may attach
	// a "*" allows attaching to ALL windows for debugging
   #if LOGGING
    //wxKeyBinder::usableWindows.Add(_T("*"));                 //+v0.4.4
   #endif
   // -----------------------------------------------
   // At least one window must be attached for the menus
   // to get update at startup. Thus "flat notebook"
   // -----------------------------------------------
    wxKeyBinder::usableWindows.Add(_T("sciwindow"));           //+v0.4.4
    wxKeyBinder::usableWindows.Add(_T("flat notebook"));        //+v0.4.4

    // CB window name is "frame". Not good for us.
    // LOGIT(wxT("CodeBlocks AppName[%s]"), pcbWindow->GetName().c_str());

    //wxKeyBinder::usableWindows.Add(_T("panel"));             //+v0.4.4
    //wxKeyBinder::usableWindows.Add(_T("list"));              //+v0.4.4
    //wxKeyBinder::usableWindows.Add(_T("listctrl"));          //+v0.4.4
    //wxKeyBinder::usableWindows.Add(_T("treectrl"));          //+v0.4.4

    //block any dynamic update attempts
	m_mergeEnabled = 0;
	// remove any predefined command keys set in main.cpp accelerator table
    Manager::Get()->GetAppWindow()->SetAcceleratorTable(wxNullAcceleratorTable);

    // Set current plugin version
	PluginInfo* pInfo = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(this));
	pInfo->version = wxT(VERSION);

    // if old key definitions file is valid for new keybinder release
    //  set it here.
	m_OldKeyFilename = wxEmptyString;
	m_OldKeyFilename = wxT("cbKeyBinder10v111.ini");

	return;

}//OnAttach

// ----------------------------------------------------------------------------
void cbKeyBinder::OnRelease(bool appShutDown)
// ----------------------------------------------------------------------------
{
	// do de-initialization for your plugin
	// if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
	// which means you must not use any of the SDK Managers
	// NOTE: after this function, the inherited member variable
	// IsAttached() will be FALSE...

    // -------------------------------------------
    // Saving disabled when no editors attached because
    // when CB/Plugins/ManagePlugins menu "re-enables" the plugin,
    // then if the user closes down without opening a file,
    // an empty .ini file is written. The profiles are never built because no
    // opens took place, no attaches took place, no events handed to KeyBinder.
    // -------------------------------------------
	// stop the merge timer, do a final merge to catch any changes
    StopMergeTimer();
	if (m_bBound)
    {   // m_bBound is false when KB re-enabled and no events occured
        // Do *not* do a merge here. CB code has already remove some menu items
        //-*MergeDynamicMenus();*//
        EnableMerge(false);
        // Do *not* do a save, here. CB core has destroyed some menu items before
        // we have a chance to finish the write.
        //-OnSave();
    }
    // remove keyboard and window close event //+v0.4.7
	m_pKeyProfArr->DetachAll();
}
// ----------------------------------------------------------------------------
//  cbKeyBinder GetConfigurationPanel
// ----------------------------------------------------------------------------
cbConfigurationPanel* cbKeyBinder::GetConfigurationPanel(wxWindow* parent)
{
	//create and display the configuration dialog for your plugin
	//NotImplemented(_T("cbKeyBinder::Configure()"));
    if(!IsAttached()) {	return 0;}

    //call configuation dialogue
    return OnKeyConfig(parent);
}
// ----------------------------------------------------------------------------
//  cbKeyBinder BuildMenu
// ----------------------------------------------------------------------------
void cbKeyBinder::BuildMenu(wxMenuBar* menuBar)
{
	//The application is offering its menubar for your plugin,
	//to add any menu items you want...
	//Append any items you need in the menu...
	//NOTE: Be careful in here... The application's menubar is at your disposal.
	//-NotImplemented(_T("cbKeyBinder::OfferMenuSpace()"));

	if(!IsAttached()) {	return;	 }

	// This routine may be called when another plugin modifies the menu.
	// or codeblocks disables another plugin and rebuilds the menu bar
    if ( m_menuPreviouslyBuilt )
    {
         LOGIT( _T("KeyBinder re-entered at BuildMenu") );
         LOGIT( _T("OldMenuBar[%p] NewMenuBar[%p]"), m_pMenuBar, menuBar );
        m_pMenuBar = menuBar;
       	wxMenuCmd::Register(m_pMenuBar);
       	EnableMerge(false);
       	for (int i=0;i<5;i++)
        {
            if ( IsMerging() ) wxSleep(1);
            else break;
        }
       	OnLoad();
        return;
    }
    m_menuPreviouslyBuilt = true;

    // init the keybinder
	// memorize incomming menubar
    m_pMenuBar = menuBar;

    // Create filename like cbKeyBinder{pluginversion}.ini

    //memorize the key file name as {%HOME%}\cbKeyBinder+{ver}.ini
    //m_sKeyFilename = ConfigManager::GetConfigFolder();
    m_sConfigFolder = ConfigManager::GetConfigFolder();
    m_sExecuteFolder = FindAppPath(wxTheApp->argv[0], ::wxGetCwd(), wxEmptyString);
    m_sDataFolder = ConfigManager::GetDataFolder();

    //*bug* GTK GetConfigFolder is returning double "//?, eg, "/home/pecan//.codeblocks"
    LOGIT(_T("GetConfigFolder() is returning [%s]"), m_sConfigFolder.GetData());
    LOGIT(_T("GetExecutableFolder() is returning [%s]"), m_sExecuteFolder.GetData());
    //LOGIT(_T("GetDataFolder() is returning [%s]"), m_sDataFolder.GetData());

    // remove the double //s from filename //+v0.4.11
    m_sConfigFolder.Replace(_T("//"),_T("/"));
    m_sExecuteFolder.Replace(_T("//"),_T("/"));

    // get version number from keybinder plugin
    const PluginInfo* info = Manager::Get()->GetPluginManager()->GetPluginInfo(this);
    wxString sPluginVersion = info->version.BeforeLast('.'); //+v0.4.1

    // remove the dots from version string (using first 3 chars)
    sPluginVersion.Replace(_T("."),_T(""));

    // get the CodeBlocks "personality" argument
    wxString m_Personality = Manager::Get()->GetPersonalityManager()->GetPersonality();
	if (m_Personality == wxT("default")) m_Personality = wxEmptyString;
    LOGIT( _T("Personality is[%s]"), m_Personality.GetData() );

    // if cbKeyBinder##.ini is in the executable folder, use it
    // else use the default config folder
    m_sKeyFilePath = m_sExecuteFolder;
    m_sKeyFilename = m_sKeyFilePath + wxFILE_SEP_PATH;
    if (not m_Personality.IsEmpty()) m_sKeyFilename << m_Personality + wxT(".") ;
    m_sKeyFilename << info->name<< sPluginVersion <<_T(".ini");
    if (::wxFileExists(m_sKeyFilename)) {;/*OK Use exe path*/}
    else //use the default.conf folder
    {
        m_sKeyFilePath = m_sConfigFolder;
        m_sKeyFilename = m_sKeyFilePath + wxFILE_SEP_PATH;
        if (not m_Personality.IsEmpty()) m_sKeyFilename << m_Personality + wxT(".") ;
        m_sKeyFilename << info->name<<sPluginVersion << _T(".ini");
    }

    #if LOGGING
     LOGIT(_T("cbKB:BuildMenu()File:%s"),m_sKeyFilename.GetData());
    #endif

    // initialize static pointer to filename
    pKeyFilename = &m_sKeyFilename;

    //say keys have not yet been loaded/bound to menus
    m_bBound = false;

    return ;

}//BuildMenu
// ----------------------------------------------------------------------------
void cbKeyBinder::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
// ----------------------------------------------------------------------------
{
	//Some library module is ready to display a pop-up menu.
	//Check the parameter "type" and see which module it is
	//and append any items you need in the menu...
	//TIP: for consistency, add a separator as the first item...
	//-v0.1--NotImplemented(_T("cbKeyBinder::OfferModuleMenuSpace()"));
	if(!IsAttached()) {	return;	 }
	return;
}
// ----------------------------------------------------------------------------
bool cbKeyBinder::BuildToolBar(wxToolBar* toolBar)
// ----------------------------------------------------------------------------
{
	//The application is offering its toolbar for your plugin,
	//to add any toolbar items you want...
	//Append any items you need on the toolbar...
	//NotImplemented(_T("cbKeyBinder::BuildToolBar()"));
	// return true if you add toolbar items
	if(!IsAttached()) {	return false; }
	return false;
}
// ----------------------------------------------------------------------------
void cbKeyBinder::Rebind()
// ----------------------------------------------------------------------------
{
    // called when we can't do anything else. Makes a virgin key profile
    // array from the C::B menu items.

 	wxKeyProfile *pPrimary;

	pPrimary = new wxKeyProfile(wxT("Primary"), wxT("Our primary keyprofile"));
	pPrimary->ImportMenuBarCmd(m_pMenuBar);

	#if LOGGING
        LOGIT(_T("cbKB:ReBind:Imported MenuBar"));
    #endif

	// remove keyprofiles from our array
    for (int i=0; i<m_pKeyProfArr->GetCount();i++)
      m_pKeyProfArr->Remove(m_pKeyProfArr->Item(i));

	// clear out old array
    m_pKeyProfArr->DetachAll();
	m_pKeyProfArr->Cleanup();

	wxMenuCmd::Register(m_pMenuBar);
	m_pKeyProfArr->Add(pPrimary);

	// attach to this window the default primary keybinder
	m_pKeyProfArr->SetSelProfile(0);

	//bind keys to menu items
	UpdateArr(*m_pKeyProfArr);

 	#ifdef LOGGING
        wxLogDebug(_T("cbKeyBinder Rebind\n"));
    #endif

	return;
}//Rebind
// ----------------------------------------------------------------------------
//  cbKeyBinder UpdateArr
// ----------------------------------------------------------------------------
void cbKeyBinder::UpdateArr(wxKeyProfileArray &r)
{
    #if LOGGING
      LOGIT(_T("UpdateArr::Begin"));
    #endif

	// detach all windows bound to keys
	r.DetachAll();

	// enable & attach to this window only one profile
	r.GetSelProfile()->Enable(TRUE);

        // VERY IMPORTANT: we should not use this function when we
        //                 have temporary children... they would be
        //                 added to the binder

        r.GetSelProfile()->AttachRecursively(Manager::Get()->GetAppWindow());
        //r.UpdateAllCmd(m_pMenuBar); //+v0.4.17
        r.GetSelProfile()->UpdateAllCmd(m_pMenuBar);                  //+v0.4.25

    #if LOGGING
      LOGIT(_T("UpdateArr::End"));
    #endif

}//cbKeyBinder::UpdateArr
// ----------------------------------------------------------------------------
int cbKeyBinder::EnableMerge(bool allow)
// ----------------------------------------------------------------------------
{
    // Enable/Disable Merge

    // disable Merge
    if (not allow)    // release lock
    {
        m_mergeEnabled=0;
        StopMergeTimer();
        return m_mergeEnabled;
    }
    // enable Merge
    m_mergeEnabled  = (m_mergeEnabled < 0 ? 1 : ++m_mergeEnabled );
    // StartMergetTimer removed for testing 2007/05/10
    //  re-enables 2007/05/31 to record plugin menu key changes
    StartMergeTimer( 15 );
    return m_mergeEnabled;
}//LockMerge
// ----------------------------------------------------------------------------// ----------------------------------------------------------------------------
void cbKeyBinder::MergeDynamicMenus()
// ----------------------------------------------------------------------------
{
    //v0.4.25
    // Add or adjust any dynamic menu/key assignments made by plugins etal.

    // Caller must have previously enabled merging
    int n;
    if (not (n = IsEnabledMerge()))
    {
        #ifdef LOGGING
         LOGIT( _T("MergeDynamicMenus entered when disabled n:%d."),n );
        #endif //LOGGING
        return;
    }

    // dont allow re-entry from other calls (eg. OnTimer() )
    EnableMerge(false);

    //#ifdef LOGGING
    // if ( n != 1)
    // LOGIT( _T("MergeDynamicMenus: EnabelMerge Out of synch. Count[%d]"), n );
    //#endif //LOGGING

    IsMerging(true);
    m_MenuModifiedByMerge +=
            m_pKeyProfArr->GetSelProfile()->MergeDynamicMenuItems(m_pMenuBar);

    if (m_MenuModifiedByMerge)
    {
        #ifdef LOGGING
         LOGIT( _T("KeyBindings were changed dynamically") );
        #endif //LOGGING;
    }

    IsMerging(false);
    // enable re-entry to this routine
    EnableMerge(true);
    return;

}//MergeDynamicMenus
// ----------------------------------------------------------------------------
cbConfigurationPanel* cbKeyBinder::OnKeyConfig(wxWindow* parent)
// ----------------------------------------------------------------------------
{
    // user key configuration/definition dialog

    #ifdef LOGGING
     LOGIT(_T("cbKB:OnKeyConfig()"));
    #endif

    //wait for a good key file load()
    if (not m_bBound) return 0;

    // Add or adjust any dynamic menu/key assignments made by plugins etc.
    EnableMerge(true);
    MergeDynamicMenus();                                               //v0.4.25
    // Disable menu merging while in settings dialog
    // MyDialog dtor will re-enable menu merging
    m_bConfigBusy = true;
    EnableMerge(false);

    // Set options and invoke the configuation dialog
    // The commented lines are from the original wxKeyBinder
    // They may be useful later

	//bool btree = GetMenuBar()->IsChecked(Minimal_UseTreeCtrl);
	bool btree = true;
	//bool baddprofile = GetMenuBar()->IsChecked(Minimal_ShowAddRemoveProfile);
	bool baddprofile = true;
	//bool bprofiles = GetMenuBar()->IsChecked(Minimal_ShowKeyProfiles);
	bool bprofiles = true;
	//bool bprofileedit = GetMenuBar()->IsChecked(Minimal_EnableProfileEdit);
	bool bprofileedit = true;

	// setup build flags
	int mode = btree ? wxKEYBINDER_USE_TREECTRL : wxKEYBINDER_USE_LISTBOX;
	if (baddprofile) mode |= wxKEYBINDER_SHOW_ADDREMOVE_PROFILE;
	if (bprofileedit) mode |= wxKEYBINDER_ENABLE_PROFILE_EDITING;


    MyDialog* dlg = new MyDialog(this, *m_pKeyProfArr, parent,
        wxT("Keybindings"), mode);

    // enable|disable keyprofiles combo box
    dlg->m_p->EnableKeyProfiles(bprofiles);

    // when the configuration panel is closed with OK, OnKeyConfigDialogDone() will be called
    return dlg;

}//OnKeyConfig
// ----------------------------------------------------------------------------
void cbKeyBinder::OnKeyConfigDialogDone(MyDialog* dlg)
// ----------------------------------------------------------------------------
{
    // The configuration panel has run its OnApply() function.
    // So here it's as if we were using ShowModal() and it just returned wxID_OK.

    // Apply user keybinder changes to key profile array as if the
    //  old EVT_BUTTON(wxID_APPLY, wxKeyConfigPanel::OnApplyChanges)
    //  in keybinder.cpp had been hit

    // stop dynamic menu merges
    EnableMerge(false);

    bool modified = false;
    dlg->m_p->ApplyChanges();

        modified = true;
        *m_pKeyProfArr = dlg->m_p->GetProfiles();
    // don't delete "dlg" variable; CodeBlocks will destroy it

    //update Windows/EventHanders from changed wxKeyProfile
    // update attaches and menu items
    UpdateArr(*m_pKeyProfArr) ;
    //Save the key profiles to external storage
    OnSave(true);
    m_MenuModifiedByMerge = 0;

    // select the right keyprofile
    #if LOGGING
        int sel =
    #endif
    m_pKeyProfArr->GetSelProfileIdx();

    #if LOGGING
        wxLogDebug(wxString::Format(wxT("Selected the #%d profile (named '%s')."),
            sel+1, m_pKeyProfArr->Item(sel)->GetName().c_str()),
            wxT("Profile selected"));
    #endif

}//OnKeyConfigDialogDone
// ----------------------------------------------------------------------------
void cbKeyBinder::OnLoad()
// ----------------------------------------------------------------------------
{
	// stop any dynamic update attempts
    EnableMerge(false);

    // if compatible, copy old key defs file to new key definitions
    if (not m_OldKeyFilename.IsEmpty() )
    {    wxString oldKeyFile = m_sKeyFilePath+wxFILE_SEP_PATH+m_OldKeyFilename;
        if (not ::wxFileExists(m_sKeyFilename) )
            if (::wxFileExists( oldKeyFile ) )
                ::wxCopyFile( oldKeyFile, m_sKeyFilename);
    }
    // Load key binding definitions from a file %HOME%\cbKeyBinder{ver}.ini

	// before loading we must register in wxCmd arrays the various types
	// of commands we want wxCmd::Load to be able to recognize...
    #if LOGGING
	 LOGIT(_T("--------------"));
	 LOGIT(_T("OnLoad()Begin"));
	#endif

    // tell other routines that binding has taken place
    m_bBound = TRUE;

	// clear our old array
	// could be some orphaned wxMenuItem ptrs left in it
    m_pKeyProfArr->DetachAll();
	m_pKeyProfArr->Cleanup();


	// before loading we must register in wxCmd arrays the various types
	// of commands we want wxCmd::Load to be able to recognize...
	wxMenuCmd::Register(m_pMenuBar);
     LOGIT( _T("OnLoad Register MenuBar[%p]"),m_pMenuBar );
	wxString strLoadFilename = m_sKeyFilename;
	#if LOGGING
	 LOGIT(_T("cbKB:Loading File %s"), strLoadFilename.GetData());
	#endif

    wxFileConfig cfg(wxEmptyString,         // appname
                    wxEmptyString,          // vendor
                    strLoadFilename,        // loacal file
                    wxEmptyString,          // global file
                    wxCONFIG_USE_LOCAL_FILE);

	if (m_pKeyProfArr->Load(&cfg, wxEmptyString))
    {
        // get the cmd count
		int total = 0;
		for (int i=0; i<m_pKeyProfArr->GetCount(); i++)
			total += m_pKeyProfArr->Item(i)->GetCmdCount();

		if (total == 0)
        {
            wxString msg;
            msg  	<< wxT("KeyBinder: No keyprofiles have been found...\n")
					<< strLoadFilename.c_str()
					<< wxT("\nmay be corrupted.\n")
					<< wxT("A default keyprofile will be set.");
			wxMessageBox(msg,wxT("KeyBinder"));
            Rebind();
		}//endif
        else
        { //all is loaded
            ;
		    #ifdef LOGGING
			 wxLogMessage(wxString::Format(
					wxT("All the [%d] keyprofiles have been correctly loaded ")
					wxT("(%d commands in total).\n")
					wxT("The #%d loaded profile ('%s') will be applied."),
					m_pKeyProfArr->GetCount(),
                    total, m_pKeyProfArr->GetSelProfileIdx()+1,
					m_pKeyProfArr->GetSelProfile()->GetName().c_str()),
						wxT("Load Successful"));
             LOGIT(_T("cbKeyBinder Matched %d MenuItems"), total);
            #endif

        }//endelse

		// reattach frames to the loaded keybinder
		// which updates the menu items to current profile
		UpdateArr(*m_pKeyProfArr);

	} else {
        #ifdef LOGGING
	     LOGIT(_T("cbKeyBinder:Error loading key file.\nCreating Defaults")); //doing nothing for now
	    #endif
//	    wxString strErrMsg = "Error loading Key Bindings file:\n"+m_sKeyFilename;
//	    if ( ! bKeyFileErrMsgShown)
//	      wxMessageBox(strErrMsg);
//	    bKeyFileErrMsgShown = TRUE; //say message has been shown

        // The last resort, create a virgin key profile array
	    Rebind();
	}

    #ifdef LOGGING
	 LOGIT(_T("OnLoad()Ended, EnableMerge[%d]"), IsEnabledMerge());
	#endif
    if (not IsEnabledMerge()) EnableMerge(true);
	return;

}//OnLoad
// ----------------------------------------------------------------------------
void cbKeyBinder::OnSave(bool backitup)
// ----------------------------------------------------------------------------
{
    // Save the key profile(s) to a file

    // delete the key definition file (removes invalid menuitem id's)
    bool done = false;
    done = ::wxRemoveFile(m_sKeyFilename);
    #if LOGGING
      if (done)
       LOGIT(_T("cbKB:File %s deleted."),m_sKeyFilename.GetData());
    #endif

	wxString path = m_sKeyFilename;
	#ifdef LOGGING
	  LOGIT( _T("cbKB:SavePath %s"), path.GetData() );
	#endif //LOGGING
	wxFileConfig *cfg = new wxFileConfig(wxEmptyString,wxEmptyString,path); //v04.11

	if (m_pKeyProfArr->Save(cfg, wxEmptyString, TRUE))
	 {
		// get the cmd count
		int total = 0;
		for (int i=0; i<m_pKeyProfArr->GetCount(); i++)
			total += m_pKeyProfArr->Item(i)->GetCmdCount();
        cfg->Flush();
		LOGIT(wxString::Format(wxT("All the [%d] keyprofiles ([%d] commands ")
			wxT("in total) have been saved in \n\"")+path, //+wxT(".ini\""),
              m_pKeyProfArr->GetCount(), total) );

        // copy the .ini file to a .ini.bak file
        done = false;
        if ( backitup && ::wxFileExists(m_sKeyFilename) )
        {
            done = ::wxCopyFile(m_sKeyFilename, m_sKeyFilename+_T(".bak"));
            #if LOGGING
              if ( done )
                LOGIT(_T("cbKB:File %s copied to .bak"),m_sKeyFilename.GetData());
            #endif
    }//if (done..


	 } else {

		wxMessageBox(wxT("Keybinder:Error saving key file!"), wxT("Save Error"),
			wxOK | wxICON_ERROR);
	 }

	delete cfg;
}//OnSave
// ----------------------------------------------------------------------------
//                          D I A L O G
// ----------------------------------------------------------------------------
//     keybindings dialog: a super-simple wrapper for wxKeyConfigPanel
// ----------------------------------------------------------------------------
MyDialog::MyDialog(cbKeyBinder* binder, wxKeyProfileArray &prof,
				   wxWindow *parent, const wxString &title, int mode)
// ----------------------------------------------------------------------------
    :m_pBinder(binder)
{
    cbConfigurationPanel::Create(parent, -1, wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// we can do our task in two ways:
	// 1) we can use wxKeyConfigPanel::ImportMenuBarCmd which gives
	//    better appearances (for me, at least, :-))
	// 2) we can use wxKeyConfigPanel::ImportKeyBinderCmd

	// STEP #1: create a simple wxKeyConfigPanel
	m_p = new wxKeyConfigPanel(this, mode);

	// STEP #2: add a profile array to the wxKeyConfigPanel
	m_p->AddProfiles(prof);

	// STEP #3: populate the wxTreeCtrl widget of the panel
	m_p->ImportMenuBarCmd(((wxFrame*)Manager::Get()->GetAppWindow())->GetMenuBar());

	// and embed it in a little sizer
	wxBoxSizer *main = new wxBoxSizer(wxVERTICAL);
	main->Add(m_p, 1, wxGROW);
	SetSizer(main);
	main->SetSizeHints(this);

	// this is a little modification to make dlg look nicer
	//wxSize sz(GetSizer()->GetMinSize());
	//SetSize(-1, -1, (int)(sz.GetWidth()*1.3), (int)(sz.GetHeight()*1.1));
	//CentreOnScreen();
}//MyDialog

// ----------------------------------------------------------------------------
MyDialog::~MyDialog()
{
    m_pBinder->EnableMerge(true);
    m_pBinder->m_bConfigBusy = false;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void MyDialog::OnApply()
// ----------------------------------------------------------------------------
{
    m_pBinder->OnKeyConfigDialogDone(this);
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnProjectOpened(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (IsAttached())
     {
        #if LOGGING
          LOGIT(_T("cbKB:ProjectOpened"));
        #endif
     }
    event.Skip();
}

// ----------------------------------------------------------------------------
void cbKeyBinder::OnProjectActivated(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (IsAttached())
     {
        #if LOGGING
          LOGIT(_T("cbKB:ProjectActivated"));
        #endif
     }
    event.Skip();
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnProjectClosed(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (IsAttached())
     {
        #if LOGGING
          //LOGIT(_T("cbKB:ProjectClosed"));
        #endif

        //get rid of unused editor ptr space
        m_EditorPtrs.Shrink();
    }
    event.Skip();
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnProjectFileAdded(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (IsAttached())
     {
        #if LOGGING
          LOGIT(_T("cbKB:ProjectFileAdded"));
        #endif
     }
    event.Skip();
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnProjectFileRemoved(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (IsAttached())
     {
       #if LOGGING
        LOGIT(_T("cbKB:ProjectFileRemoved"));
       #endif
     }
    event.Skip();
}
// ----------------------------------------------------------------------------
void cbKeyBinder::AttachEditor(wxWindow* pWindow)
// ----------------------------------------------------------------------------
{
    if (IsAttached())
     {
         wxWindow* thisEditor = pWindow->FindWindowByName(_T("SCIwindow"),pWindow);


        //skip editors that we already have
        if ( thisEditor && (wxNOT_FOUND == m_EditorPtrs.Index(thisEditor)) )
         {
            //add editor to our array and push a keyBinder event handler
            m_EditorPtrs.Add(thisEditor);
            //Rebind keys to newly opened windows
            m_pKeyProfArr->GetSelProfile()->Attach(thisEditor);
            #if LOGGING
             LOGIT(_T("cbKB:AttachEditor %s %p"), thisEditor->GetLabel().c_str(), thisEditor);
            #endif
         }
     }
}
// ----------------------------------------------------------------------------
void cbKeyBinder::DetachEditor(wxWindow* pWindow)
// ----------------------------------------------------------------------------
{
    if (IsAttached())
     {

         wxWindow* thisWindow = pWindow;

         // Cannot use GetBuiltinActiveEditor() because the active Editor is NOT the
         // one being closed!!
         // wxWindow* thisEditor
         //  = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor()->GetControl();

         //find the cbStyledTextCtrl wxScintilla window
         wxWindow*
           thisEditor = thisWindow->FindWindowByName(_T("SCIwindow"), thisWindow);

        if ( thisEditor && (m_EditorPtrs.Index(thisEditor) != wxNOT_FOUND) )
         {
            m_pKeyProfArr->GetSelProfile()->Detach(thisEditor);
            m_EditorPtrs.Remove(thisEditor);
            #if LOGGING
             LOGIT(_T("cbKB:DetachEditor %s %p"), thisEditor->GetLabel().c_str(), thisEditor);
            #endif
         }//if
     }

}//DetachEditor
// ----------------------------------------------------------------------------
void cbKeyBinder::OnEditorOpen(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (IsAttached())
     {
         //LOGIT(_T("cbKB:OnEditorOpen()"));
        if (not m_bBound)
         {
            //OnLoad(); event.Skip(); return;
            OnAppStartupDone(event);
         }

        //already bound, just add the editor window
         wxWindow* thisWindow = event.GetEditor();
         wxWindow* thisEditor = thisWindow->FindWindowByName(_T("SCIwindow"),thisWindow);

         // find editor window the Code::Blocks way
         // find the cbStyledTextCtrl wxScintilla "SCIwindow" to this EditorBase
         cbEditor* ed = 0;
         EditorBase* eb = event.GetEditor();
         if (eb && eb->IsBuiltinEditor())
          {  ed = static_cast<cbEditor*>(eb);
             thisEditor = ed->GetControl();
          }

        //skip editors that we already have
        if ( thisEditor && (wxNOT_FOUND == m_EditorPtrs.Index(thisEditor)) )
         {
            //add editor to our array and push a keyBinder event handler
            m_EditorPtrs.Add(thisEditor);
            //Rebind keys to newly opened windows
            m_pKeyProfArr->GetSelProfile()->Attach(thisEditor);
            #if LOGGING
             LOGIT(_T("cbKB:OnEditorOpen/Attach %s %p"), thisEditor->GetLabel().c_str(), thisEditor);
            #endif
         }
     }
     event.Skip();
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnEditorClose(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (IsAttached() && m_bBound)
     {

         wxWindow* thisWindow = event.GetEditor();

         // Cannot use GetBuiltinActiveEditor() because the active Editor is NOT the
         // one being closed!!
         // wxWindow* thisEditor
         //  = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor()->GetControl();

         //find the cbStyledTextCtrl wxScintilla window
         wxWindow*
           thisEditor = thisWindow->FindWindowByName(_T("SCIwindow"), thisWindow);

         // find editor window the Code::Blocks way
         // find the cbStyledTextCtrl wxScintilla "SCIwindow" to this EditorBase
         cbEditor* ed = 0;
         EditorBase* eb = event.GetEditor();
         if (eb && eb->IsBuiltinEditor())
          {  ed = static_cast<cbEditor*>(eb);
             thisEditor = ed->GetControl();
          }

        if ( thisEditor && (m_EditorPtrs.Index(thisEditor) != wxNOT_FOUND) )
         {
            m_pKeyProfArr->GetSelProfile()->Detach(thisEditor);
            m_EditorPtrs.Remove(thisEditor);
            #if LOGGING
             LOGIT(_T("cbKB:OnEditorClose/Detach %s %p"), thisEditor->GetLabel().c_str(), thisEditor);
            #endif
         }//if
     }
    event.Skip();
}//OnEditorClose
// ----------------------------------------------------------------------------
void cbKeyBinder::OnAppStartupDone(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{

    // if keys still unbound, do it here.
    // load key binding from file
    if (not m_bBound)
     {
        #if LOGGING
         LOGIT(_T("cbKB:OnAppStartupDone:Begin initial Key Load"));
        #endif
        // attach to at least one window so menus get updated

        OnLoad();

        #if LOGGING
         LOGIT(_T("cbKB:OnAppStartupDone:End initial Key Load"));
        #endif
     }
    // Check creation of windows that have no notification (ie., wxSplitWindows)
    Connect( wxEVT_CREATE,
        (wxObjectEventFunction) (wxEventFunction)
        (wxCommandEventFunction) &cbKeyBinder::OnWindowCreateEvent);

    // Check Destroyed windows that have no notification (ie., wxSplitWindows)
    Connect( wxEVT_DESTROY,
        (wxObjectEventFunction) (wxEventFunction)
        (wxCommandEventFunction) &cbKeyBinder::OnWindowDestroyEvent);

    event.Skip(); //+v0.4.1
    return;
}//OnAppStartupDone
// ----------------------------------------------------------------------------
void cbKeyBinder::OnWindowCreateEvent(wxEvent& event)
// ----------------------------------------------------------------------------
{
    // wxEVT_CREATE entry
    // Have to do this for split windows since CodeBlocks does not have
    // events when opening/closing split windows

    // Attach a split window (or any other window)
    if ( m_bBound )
    {
        wxWindow* pWindow = (wxWindow*)(event.GetEventObject());
        cbEditor* ed = 0;
        cbStyledTextCtrl* p_cbStyledTextCtrl = 0;
        cbStyledTextCtrl* pLeftSplitWin = 0;
        cbStyledTextCtrl* pRightSplitWin = 0;
        ed  = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (ed)
        {   p_cbStyledTextCtrl = ed->GetControl();
            pLeftSplitWin = ed->GetLeftSplitViewControl();
            pRightSplitWin = ed->GetRightSplitViewControl();
            //Has this window been split?
            //**This is a temporary hack until some cbEvents are defined**
            if ( pWindow && (pRightSplitWin == 0) )
            {
                //-if (pRightSplitWin eq pWindow)
                //-{    Attach(pRightSplitWin);
                if (pWindow->GetParent() == ed)
                {    AttachEditor(pWindow);
                    LOGIT( _T("OnWindowCreateEvent Attaching:%p"), pWindow );
                }
            }
        }
    }//if m_bBound...

    event.Skip();
}//OnWindowCreateEvent
// ----------------------------------------------------------------------------
void cbKeyBinder::OnWindowDestroyEvent(wxEvent& event)
// ----------------------------------------------------------------------------
{
    // wxEVT_DESTROY entry
    // This routine simply clears the memorized Editor pointers
    // that dont get cleared by OnEditorClose, which doesnt get
    // entered for split windows. CodeBlocks doesnt yet have events
    // when opening/closing split windows.

    if (not m_bBound){ event.Skip(); return;}

    wxWindow* pWindow = (wxWindow*)(event.GetEventObject());

    //-Detach(pWindow); causes crash
    if ( (pWindow) && (m_EditorPtrs.Index(pWindow) != wxNOT_FOUND))
    {
        m_EditorPtrs.Remove(pWindow);
        //-DetachEditor(pWindow); causes crash
        #ifdef LOGGING
         LOGIT( _T("OnWindowDestroyEven Remove %p"), pWindow);
        #endif //LOGGING
    }
    event.Skip();
}//OnWindowClose
// ----------------------------------------------------------------------------
void cbKeyBinder::OnTimerAlarm(wxTimerEvent& event)
// ----------------------------------------------------------------------------
{
    m_bTimerAlarm = true;
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnIdle(wxIdleEvent& event)
// ----------------------------------------------------------------------------
{
    // Scan the menu and enter any modified menu items into the array of
    //  key bindings

    if ( not m_bTimerAlarm) { event.Skip(); return; }
    // LOGIT( _T("OnIdle m_bTimerAlarm[%d]"), m_bTimerAlarm );

    if ( m_bAppShutDown )   { event.Skip(); return; }
    if ( not m_bBound )     { event.Skip(); return; }

    // LOGIT( _T("OnIdle IsMerging[%d]"), IsMerging() );
    if ( IsMerging() )      { event.Skip(); return; }

    // LOGIT( _T("OnIdle m_bConfigBusy[%d]"), m_bConfigBusy );
    if ( m_bConfigBusy )    { event.Skip(); return; }

    if ( IsEnabledMerge() )
    {
        // scan for dynamic menu changes
        MergeDynamicMenus();
        if (m_MenuModifiedByMerge){
            #ifdef LOGGING
              LOGIT( _T("OnIdle:Modified:%d"), m_MenuModifiedByMerge );
            #endif //LOGGING
            // write changed key profile to disk
            EnableMerge(false);
            if (not m_bAppShutDown)
                OnSave(false);
            m_MenuModifiedByMerge = 0;
        }
    }//fi IsEnableMerge...

    // re-enable the timer
    m_bTimerAlarm = false;
    if (not m_bAppShutDown) EnableMerge(true);
    event.Skip();
}//OnMergeTimer
// ----------------------------------------------------------------------------
void cbKeyBinder::OnAppStartShutdown(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // currently this is defined in the sdk, but not implemented
    // and never called. Another sdk gotcha! And another reason to avoid it.
     LOGIT( _T("OnAppStartShutdown") );
    m_bAppShutDown = true;
	// stop the merge timer
    EnableMerge(false);
    m_bTimerAlarm = false;
    // wait for any current merge to complete
    for (int i=0;i<5 ;++i )
    {   if ( IsMerging() )
            {::wxSleep(1); wxYield();}
        else break;
    }
    EnableMerge(false);

    event.Skip(); // allow others to process it too
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnMenuBarModify(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // CodeBlocks is beginning or ending menu modification
    // Bug: this routine is never invoked by CB core
    // KeyBinder was going to use it to preserve the accelerators on
    // the Tools menu. But, alas, it's never called when the tools menu is changed.

    int modType = event.GetEventType();
     LOGIT( _T("OnMenuBarModify[%d]"), modType );
    event.Skip();
}
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
        LOGIT( _T("FindAppPath: AbsolutePath[%s]"), wxPathOnly(argv0Str).GetData() );
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
            LOGIT( _T("FindAppPath: RelativePath[%s]"), wxPathOnly(str).GetData() );
            return wxPathOnly(str);
        }
    }

    // OK, it's neither an absolute path nor a relative path.
    // Search PATH.

    wxPathList pathList;
    pathList.AddEnvList(wxT("PATH"));
    str = pathList.FindAbsoluteValidPath(argv0Str);
    if (!str.IsEmpty())
    {
        LOGIT( _T("FindAppPath: SearchPath[%s]"), wxPathOnly(str).GetData() );
        return wxPathOnly(str);
    }

    // Failed
     LOGIT(  _T("FindAppPath: Failed, returning cwd") );
    return wxEmptyString;
    //return cwd;
}
// ----------------------------------------------------------------------------
