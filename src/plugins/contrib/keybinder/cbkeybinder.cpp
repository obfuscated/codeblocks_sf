/***************************************************************
 * Name:      cbkeybinder.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Pecan @ Mispent Intent
 * Copyright: (c) Pecan @ Mispent Intent
 * License:   GPL
 **************************************************************/
//commit 12/14/2005 9:15 AM
// The majority of this code was lifted from wxKeyBinder and
// its "minimal.cpp" sample program

// This plugin should to be loaded after all modifications to the menus
// have been made by other plugins; else it won't see the menu changes
// until a 5 second timer invokes it to scan the menus again.
// Prefixing cbKeyBinder.dll with a 'z' (eg. zcbKeyBinder.dll) is
// a simple way of loading it last.

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma implementation "cbkeybinder.h"
#endif

#include "cbkeybinder.h"
#include <licenses.h> // defines some common licenses (like the GPL)

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

// Implement the plugin's hooks
CB_IMPLEMENT_PLUGIN(cbKeyBinder);

// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(cbKeyBinder, cbPlugin)
	// add events here...

	EVT_PROJECT_CLOSE(cbKeyBinder::OnProjectClosed)
	EVT_EDITOR_OPEN(cbKeyBinder::OnEditorOpen)
	EVT_EDITOR_CLOSE(cbKeyBinder::OnEditorClose)
//    EVT_TIMER(TIMER_ID, cbKeyBinder::OnTimer) //timer
	EVT_PROJECT_OPEN(cbKeyBinder::OnProjectOpened)
	// keybinder doesnt need these (yet)
	//EVT_PROJECT_ACTIVATE(cbKeyBinder::OnProjectActivated)
	//EVT_PROJECT_FILE_ADDED(cbKeyBinder::OnProjectFileAdded)
	//EVT_PROJECT_FILE_REMOVED(cbKeyBinder::OnProjectFileRemoved)

	EVT_APP_STARTUP_DONE(cbKeyBinder::OnAppStartupDone)

END_EVENT_TABLE()
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MyDialog, wxDialog)
	EVT_BUTTON(wxID_APPLY, MyDialog::OnApply)
END_EVENT_TABLE()
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
cbKeyBinder::cbKeyBinder()
// ---------------------------------------------------------------------------
{
	//ctor
	m_PluginInfo.name = _T("cbKeyBinder");
	m_PluginInfo.title = _("Keyboard shortcuts configuration");
	m_PluginInfo.version = _T("0.4");
	m_PluginInfo.description = _("CodeBlocks KeyBinder\nCommit 12/14/2005 9AM");
	m_PluginInfo.author = _T("Pecan && Mispent Intent");
	m_PluginInfo.authorEmail = _T("");
	m_PluginInfo.authorWebsite = _T("");
	m_PluginInfo.thanksTo = _("wxKeyBinder & CodeBlocks Developers");
	m_PluginInfo.license = LICENSE_GPL;
	m_PluginInfo.hasConfigure = true;

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
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...


    // Only stable windows can be attached for codeblocks; currently
    // "SCIwindow" & "cbMainWindow". See wxKeyBinder::Attach() in keybinder.cpp
    // Change name of codeblocks main window so it gets attached by keybinder

    // Have to have at least one window to attach to else secondary keys wont work
    // and "notebook" windows work just fine. Dont need AppWindow attach

    //get window for log when debugging
    pcbWindow = Manager::Get()->GetAppWindow();
    //pcbWindow->SetName("cbAppWindow");

    #if LOGGING
        // allocate wxLogWindow in the header
        //  wxLogWindow* pMyLog;
        // #define LOGIT wxLogMessage
        /* wxLogWindow* */
        pMyLog = new wxLogWindow(pcbWindow,m_PluginInfo.name);
        wxLog::SetActiveTarget(pMyLog);
        LOGIT("log message window open");
        pMyLog->Flush();
    #endif

    // Allocate array but do actual key bindings after all menuitems have
    // been implemented by other plugins
	m_pKeyProfArr = new wxKeyProfileArray;
	m_bBound = FALSE;   //say keys are unbound to menus
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
	// m_IsAttached will be FALSE...
}
// ----------------------------------------------------------------------------
//  cbKeyBinder Configure
// ----------------------------------------------------------------------------
int cbKeyBinder::Configure()
{
	//create and display the configuration dialog for your plugin
	//NotImplemented(_T("cbKeyBinder::Configure()"));
    if(!m_IsAttached) {	return -1;}

    // This calls the key definition dialog. This routine could be invoked
    // via a menuitem if desired, but its just as simple
    // to assign a key to the plugin via the plugin dialog itself.
    // Then, when saved to a config file, the key will be enabled at
    // C::B plugin initialization.

    //is main window attached?
    OnKeybindings();
    return 0;
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

	if(!m_IsAttached) {	return;	 }

	//memorize incomming menubar
    m_pMenuBar = menuBar;

    // init the keybinder
    //memorize the key file name as {%HOME%}\cbKeyBinder+{ver}.ini
    m_sKeyFilename = ConfigManager::GetConfigFolder();
    // remove the dots from version string
    wxString sPluginVersion=m_PluginInfo.version;
    sPluginVersion.Replace(_T("."),_T(""));
    m_sKeyFilename = m_sKeyFilename<<wxFILE_SEP_PATH<<m_PluginInfo.name<<sPluginVersion
        <<_T("v")<<PLUGIN_SDK_VERSION_MAJOR<<PLUGIN_SDK_VERSION_MINOR<<_T(".ini");

    #if LOGGING
     LOGIT("cbKB:BuildMenu()");
    #endif
    // wait to show error messages after CodeBlocks is running
    bKeyFileErrMsgShown = TRUE;
    //load the key binding definition file
    //-OnLoad(); unecessary since EVT_APP_STARTUP_DONE(cbKeyBinder::OnAppStartupDone)
    bKeyFileErrMsgShown = FALSE;

    // override OnLoad()s setting of 'm_bBound' so timer will reload
    // key file after other plugins change menus
    m_bBound = false;

    // set timer to recheck key bindings later
//    m_timer.Start(5000);    // 5 second interval

    return ;

}//BuildMenu
// ----------------------------------------------------------------------------
void cbKeyBinder::BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg)
// ----------------------------------------------------------------------------
{
	//Some library module is ready to display a pop-up menu.
	//Check the parameter "type" and see which module it is
	//and append any items you need in the menu...
	//TIP: for consistency, add a separator as the first item...
	//-v0.1--NotImplemented(_T("cbKeyBinder::OfferModuleMenuSpace()"));
	if(!m_IsAttached) {	return;	 }


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
	if(!m_IsAttached) {	return false; }
	return false;
}
// ----------------------------------------------------------------------------
void cbKeyBinder::Rebind()
// ----------------------------------------------------------------------------
{

 	wxKeyProfile *pPrimary;//-, *pSecondary;

	pPrimary = new wxKeyProfile(wxT("Primary"), wxT("Our primary keyprofile"));
	pPrimary->ImportMenuBarCmd(m_pMenuBar);

	#if LOGGING
        LOGIT("cbKB:ReBind:Imported");
    #endif

	//pSecondary = new wxKeyProfile(*pPrimary);
	//-pSecondary = new wxKeyProfile(wxT("Secondary"), wxT("Our Secondary keyprofile"));

	//pSecondary->SetName(wxT("Secondary"));
	//pSecondary->SetDesc(wxT("Our secondary keyprofile"));

	// re-import the current menu items
    //-pSecondary->ImportMenuBarCmd(m_pMenuBar);

        // The following lines from the original wxKeyBinder demo
        //-	// just to show some features....
        //-	pPrimary->AddShortcut(Minimal_Shortcut1, wxKeyBind(wxT("CTRL+SHIFT+E")));
        //-
        //-	// and to make some differences between the two key profiles
        //-	pSecondary->GetCmd(Minimal_Shortcut1)->GetShortcut(0)->Set(wxKeyBind(wxT("ALT+F10")));
        //-	pSecondary->GetCmd(Minimal_Shortcut2)->GetShortcut(0)->Set(wxKeyBind(wxT("ALT+F11")));
        //-	pSecondary->GetCmd(Minimal_Shortcut3)->GetShortcut(0)->Set(wxKeyBind(wxT("ALT+F12")));

	// remove keyprofiles from our array
    for (int i=0; i<m_pKeyProfArr->GetCount();i++)
      m_pKeyProfArr->Remove(m_pKeyProfArr->Item(i));

	// clear out old array
    m_pKeyProfArr->DetachAll();
	m_pKeyProfArr->Cleanup();

	wxMenuCmd::Register(m_pMenuBar);
	m_pKeyProfArr->Add(pPrimary);
	//-pArr->Add(pSecondary);

	// attach to this window the default primary keybinder
	m_pKeyProfArr->SetSelProfile(0);

	//bind keys to menu items
	UpdateArr(*m_pKeyProfArr);

 	#ifdef LOGGING
        wxLogDebug("cbKeyBinder Rebind\n");
    #endif

	return;
}//Rebind
// ----------------------------------------------------------------------------
//  cbKeyBinder UpdateArr
// ----------------------------------------------------------------------------
void cbKeyBinder::UpdateArr(wxKeyProfileArray &r)
{
    //=====================================================================//
    // The following warnings don't apply anymore because this
    // keybinder.h::wxKeyBinder::Attach() has been caged.
    // It can only attach to SCIwindows and the C::B main window
    //=====================================================================//

    //=================================================================//
    //***do not use this routine until the dialog has been dismissed***//
    //=================================================================//

    #if LOGGING
      LOGIT("UpdateArr::Begin");
    #endif

	// detach all windows bound to keys
	r.DetachAll();

	// enable & attach to this window only one profile
	r.GetSelProfile()->Enable(TRUE);

        // VERY IMPORTANT: we should not use this function when we
        //                 have temporary children... they would be
        //                 added to the binder and when
        //                 deleted, the binder will reference invalid memory...

        // either of the following two lines work
        //-r.GetSelProfile()->AttachRecursively(Manager::Get()->GetAppWindow());

        r.GetSelProfile()->AttachRecursively(Manager::Get()->GetAppWindow());
        //r.UpdateAllCmd();		// not necessary

    #if LOGGING
      LOGIT("UpdateArr::End");
    #endif

}//cbKeyBinder::UpdateArr
// ----------------------------------------------------------------------------
void cbKeyBinder::OnKeybindings()
// ----------------------------------------------------------------------------
{
    #ifdef LOGGING
     LOGIT("cbKB:OnKeybindings()");
    #endif

    //wait for a good key file load()
    if (!m_bBound) return;

    // Sets options and invoke the dialog
    // The commented lines are from the original wxKeyBinder
    // They may be useful later

	//bool btree = GetMenuBar()->IsChecked(Minimal_UseTreeCtrl);
	bool btree = true;
	//bool baddprofile = GetMenuBar()->IsChecked(Minimal_ShowAddRemoveProfile);
	bool baddprofile = false;
	//bool bprofiles = GetMenuBar()->IsChecked(Minimal_ShowKeyProfiles);
	bool bprofiles = false;
	//bool bprofileedit = GetMenuBar()->IsChecked(Minimal_EnableProfileEdit);
	bool bprofileedit = false;

	// setup build flags
	int mode = btree ? wxKEYBINDER_USE_TREECTRL : wxKEYBINDER_USE_LISTBOX;
	if (baddprofile) mode |= wxKEYBINDER_SHOW_ADDREMOVE_PROFILE;
	if (bprofileedit) mode |= wxKEYBINDER_ENABLE_PROFILE_EDITING;

	int exitcode, sel;

    //=====================================================================//
    // The following warnings don't apply anymore because this
    // keybinder.h::wxKeyBinder::Attach() has been caged.
    // It can only attach to SCIwindows and the C::B main window
    //=====================================================================//
	{ //<-------- Block declaration to enclose the dlg allocation -----------

        // we need to destroy MyDialog *before* the call to UpdateArr:()
		// otherwise the call to wxKeyBinder::AttachRecursively() which
		// is done inside UpdateArr() would attach to the binder all
		// MyDialog subwindows which are children of the main frame.
		// then, when the dialog is destroyed, wxKeyBinder holds
		// invalid pointers which will provoke a crash !!

		MyDialog* pdlg= new MyDialog(*m_pKeyProfArr, Manager::Get()->GetAppWindow(),
		    wxT("Keybindings"), mode | wxKEYBINDER_SHOW_APPLYBUTTON);

        //following had no effect on crash problem
        //guanantee keyBinder won't attach dialog window
        pdlg->SetExtraStyle(wxWS_EX_TRANSIENT);

		// enable|disable keyprofiles combo box
		pdlg->m_p->EnableKeyProfiles(bprofiles);

		if ((exitcode = pdlg->ShowModal()) == wxID_OK)
		 {
			// update our array (we gave a copy of it to MyDialog)
			*m_pKeyProfArr = pdlg->m_p->GetProfiles();
		 }

		 // Make sure we dont bind to the dialog window
		 // wxWdigets wxWindow Documentation states:
         // The default close event handler for wxDialog simulates a Cancel command,
         // generating a wxID_CANCEL event. Since the handler for this cancel event
         // might itself call Close, there is a check for infinite looping. The default
         // handler for wxID_CANCEL hides the dialog (if modeless) or calls EndModal(wxID_CANCEL)
         // (if modal). In other words, by default, the DIALOG IS NOT DESTROYED (it might have
         // been created on the stack, so the assumption of dynamic creation cannot be made).

		 pdlg->Destroy();

	}//**here, dlg is destroyed**<------- can now use UpdateArr(...)
     //============================================================//
	if (exitcode == wxID_OK)
	 {
        //update Windows/EventHanders from changed wxKeyProfile
        UpdateArr(*m_pKeyProfArr) ;
		//Save the key profiles to file
        OnSave();
	    // select the right keyprofile
		sel = m_pKeyProfArr->GetSelProfileIdx();

		#if LOGGING
            wxLogDebug(wxString::Format(wxT("Selected the #%d profile (named '%s')."),
                sel+1, m_pKeyProfArr->Item(sel)->GetName().c_str()),
                wxT("Profile selected"));
        #endif

	 }//if
}//OnKeybindings
// ----------------------------------------------------------------------------
void cbKeyBinder::OnLoad()
// ----------------------------------------------------------------------------
{
    // Load key binding definitions from a file %HOME%\cbKeyBinder{ver}.ini

	// before loading we must register in wxCmd arrays the various types
	// of commands we want wxCmd::Load to be able to recognize...

	LOGIT("--------------");
	LOGIT("OnLoad()Begin");

    // tell other routines that binding has taken place
    m_bBound = TRUE;

	// clear our old array
	// could be some orphaned wxMenuItem ptrs left in it
    m_pKeyProfArr->DetachAll();
	m_pKeyProfArr->Cleanup();


	// before loading we must register in wxCmd arrays the various types
	// of commands we want wxCmd::Load to be able to recognize...
	wxMenuCmd::Register(m_pMenuBar);

	wxString strLoadFilename = m_sKeyFilename;
	LOGIT("cbKB:Loading File %s", strLoadFilename.GetData());

    wxFileConfig cfg(wxEmptyString, // appname
                    wxEmptyString, // vendor
                    //-ConfigManager::GetConfigFolder() + _T("/keys.conf"), // local file
                    strLoadFilename,
                    wxEmptyString, // global file
                    wxCONFIG_USE_LOCAL_FILE);

	if (m_pKeyProfArr->Load(&cfg, wxEmptyString))
	 {
        // get the cmd count
		int total = 0;
		for (int i=0; i<m_pKeyProfArr->GetCount(); i++)
			total += m_pKeyProfArr->Item(i)->GetCmdCount();

		if (total == 0)
		 {
			wxMessageBox(wxString::Format(
					wxT("No keyprofiles have been found...\n")
					wxT("A default keyprofile will be set.")));
			wxKeyProfile *p = new wxKeyProfile(wxT("Default"));
			p->ImportMenuBarCmd(m_pMenuBar);
			#if LOGGING
			  LOGIT("cbKB:OnLoad:Menu Imported");
			#endif
			m_pKeyProfArr->Add(p);

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
             LOGIT("cbKeyBinder Matched %d MenuItems", total);
            #endif
		 }//endelse

		// reattach frames to the loaded keybinder
		UpdateArr(*m_pKeyProfArr);

	} else {
        #ifdef LOGGING
	     LOGIT("cbKeyBinder:Error loading key file.\nCreating Defaults"); //doing nothing for now
	    #endif
//	    wxString strErrMsg = "Error loading Key Bindings file:\n"+m_sKeyFilename;
//	    if ( ! bKeyFileErrMsgShown)
//	      wxMessageBox(strErrMsg);
//	    bKeyFileErrMsgShown = TRUE; //say message has been shown
	    Rebind();
	}

    #ifdef LOGGING
	 LOGIT("OnLoad()End\n");
	#endif

	return;

}//OnLoad
// ----------------------------------------------------------------------------
void cbKeyBinder::OnSave()
// ----------------------------------------------------------------------------
{
    // Save the key profile(s) to a file

    // delete the file
    // Removes file, returning true if successful.
    bool done = ::wxRemoveFile(m_sKeyFilename);
     if (done)
      {
        #if LOGGING
          { LOGIT("cbKB:File %s deleted.",m_sKeyFilename.GetData()); }
        #endif
      }//if (done..

	wxString path = m_sKeyFilename;
	path.Replace(_T(".ini"),_T(""));
	wxFileConfig *cfg = new wxFileConfig(path);

	if (m_pKeyProfArr->Save(cfg, wxEmptyString, TRUE))
	 {
		// get the cmd count
		int total = 0;
		for (int i=0; i<m_pKeyProfArr->GetCount(); i++)
			total += m_pKeyProfArr->Item(i)->GetCmdCount();

		wxMessageBox(wxString::Format(wxT("All the [%d] keyprofiles ([%d] commands ")
			wxT("in total) have been saved in \n\"")+path+wxT(".ini\""),
            m_pKeyProfArr->GetCount(), total),
			wxT("Save"));

	 } else {

		wxMessageBox(wxT("Something wrong while saving !"), wxT("Save Error"),
			wxOK | wxICON_ERROR);
	 }

	delete cfg;
}//OnSave
// ----------------------------------------------------------------------------
//                          D I A L O G
// ----------------------------------------------------------------------------
//     keybindings dialog: a super-simple wrapper for wxKeyConfigPanel
// ----------------------------------------------------------------------------
MyDialog::MyDialog(wxKeyProfileArray &prof,
				   wxWindow *parent, const wxString &title, int mode)
// ----------------------------------------------------------------------------
    :wxDialog(parent, -1, title, wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	// we can do our task in two ways:
	// 1) we can use wxKeyConfigPanel::ImportMenuBarCmd which gives
	//    better appearances (for me, at least, :-))
	// 2) we can use wxKeyConfigPanel::ImportKeyBinderCmd

	// STEP #1: create a simple wxKeyConfigPanel
	m_p = new wxKeyConfigPanel(this, mode);

	// STEP #2: add a profile array to the wxKeyConfigPanel
	m_p->AddProfiles(prof);

	// STEP #3: populate the wxTreeCtrl widget of the panel
	m_p->ImportMenuBarCmd(((wxFrame*)parent)->GetMenuBar());

	// and embed it in a little sizer
	wxBoxSizer *main = new wxBoxSizer(wxVERTICAL);
	main->Add(m_p, 1, wxGROW);
	SetSizer(main);
	main->SetSizeHints(this);

	// this is a little modification to make dlg look nicer
	wxSize sz(GetSizer()->GetMinSize());
	SetSize(-1, -1, (int)(sz.GetWidth()*1.3), (int)(sz.GetHeight()*1.1));
	CenterOnScreen();
}

// ----------------------------------------------------------------------------
MyDialog::~MyDialog() {}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void MyDialog::OnApply(wxCommandEvent &)
// ----------------------------------------------------------------------------
{
     EndModal(wxID_OK);
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnProjectOpened(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (m_IsAttached)
     {
        #if LOGGING
          LOGIT("cbKB:ProjectOpened");
        #endif
     }
    event.Skip();
}

// ----------------------------------------------------------------------------
void cbKeyBinder::OnProjectActivated(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (m_IsAttached)
     {
        #if LOGGING
          LOGIT("cbKB:ProjectActivated");
        #endif
     }
    event.Skip();
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnProjectClosed(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (m_IsAttached)
     {
        #if LOGGING
          LOGIT("cbKB:ProjectClosed");
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
    if (m_IsAttached)
     {
        #if LOGGING
          LOGIT("cbKB:ProjectFileAdded");
        #endif
     }
    event.Skip();
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnProjectFileRemoved(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (m_IsAttached)
     {
       #if LOGGING
        LOGIT("cbKB:ProjectFileRemoved");
       #endif
     }
    event.Skip();
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnEditorOpen(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (m_IsAttached)
     {
         LOGIT("cbKB:OnEditorOpen()");
        if (!m_bBound)
         {
            OnLoad(); event.Skip(); return;
         }

        //already bound, just add the editor window
        #ifdef RC2
         wxFrame* thisEditor = (wxFrame*)event.GetEditor()->GetControl();
        #endif

        #ifdef RC3
         wxWindow* thisWindow = event.GetEditor();
         wxWindow* thisEditor = thisWindow->FindWindowByName("SCIwindow",thisWindow);

         // find editor window the Code::Blocks way
         // find the cbStyledTextCtrl wxScintilla "SCIwindow" to this EditorBase
         cbEditor* ed = 0;
         EditorBase* eb = event.GetEditor();
         if (eb && eb->IsBuiltinEditor())
          {  ed = static_cast<cbEditor*>(eb);
             thisEditor = ed->GetControl();
          }
        #endif

        //skip editors that we already have
        if ( thisEditor && (wxNOT_FOUND == m_EditorPtrs.Index(thisEditor)) )
         {
            //add editor to our array and push a keyBinder event handler
            m_EditorPtrs.Add(thisEditor);
            //Rebind keys to newly opened windows
            m_pKeyProfArr->GetSelProfile()->Attach(thisEditor);
            #if LOGGING
             LOGIT("cbKB:OnEditorOpen/Attach %s %p", thisEditor->GetTitle().c_str(), thisEditor);
            #endif
         }
     }
     event.Skip();
}
// ----------------------------------------------------------------------------
void cbKeyBinder::OnEditorClose(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (m_IsAttached)
     {

        #ifdef RC2
         wxFrame* thisEditor = (wxFrame*)event.GetEditor()->GetControl();
        #endif

        #ifdef RC3
         wxWindow* thisWindow = event.GetEditor();

         // Cannot use GetBuiltinActiveEditor() because the active Editor is NOT the
         // one being closed!!
         // wxWindow* thisEditor
         //  = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor()->GetControl();

         //find the cbStyledTextCtrl wxScintilla window
         wxWindow*
           thisEditor = thisWindow->FindWindowByName("SCIwindow", thisWindow);

         // find editor window the Code::Blocks way
         // find the cbStyledTextCtrl wxScintilla "SCIwindow" to this EditorBase
         cbEditor* ed = 0;
         EditorBase* eb = event.GetEditor();
         if (eb && eb->IsBuiltinEditor())
          {  ed = static_cast<cbEditor*>(eb);
             thisEditor = ed->GetControl();
          }
         #endif



        if ( thisEditor && (m_EditorPtrs.Index(thisEditor) != wxNOT_FOUND) )
         {
            m_pKeyProfArr->GetSelProfile()->Detach(thisEditor);
            m_EditorPtrs.Remove(thisEditor);
            #if LOGGING
             LOGIT("cbKB:OnEditorClose/Detach %s %p", thisEditor->GetTitle().c_str(), thisEditor);
            #endif
         }//if
     }
    event.Skip();
}//OnEditorClose
// ----------------------------------------------------------------------------
void cbKeyBinder::OnAppStartupDone(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // get around the un-binding problem in BuildMenu
    // without this, no key bindings take place until
    // a project or file is opened

    // if keys still unbound, do it here.

    // load key binding from file
    if (!m_bBound)
     {
        #if LOGGING
         LOGIT("cbKeyBinder:Begin initial Key Load");
        #endif
        m_bBound=TRUE;
        OnLoad();
        #if LOGGING
         LOGIT("cbKeyBinder:End initial Key Load");
        #endif
     }
    return;
}//OnAppStartupDone
// ----------------------------------------------------------------------------
