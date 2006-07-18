/***************************************************************
 * Name:      cbkeybinder.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Pecan@users.berlios.de
 * Copyright: (c) Pecan
 * License:   GPL
 **************************************************************/
//
// The majority of this code was lifted from wxKeyBinder and
// its "minimal.cpp" sample program

#include <sdk.h>

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma implementation "cbkeybinder.h"
#endif

#include "cbkeybinder.h"
#include <licenses.h> // defines some common licenses (like the GPL)

#ifndef CB_PRECOMP
    #include "sdk_events.h"
#endif

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

// Implement the plugin's hooks
CB_IMPLEMENT_PLUGIN(cbKeyBinder, "Keyboard shortcuts configuration");

// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(cbKeyBinder, cbPlugin)
	// add events here...
    // Removed dependency on Codeblocks editor events in order to handle
    // split windows.
	//EVT_PROJECT_CLOSE(cbKeyBinder::OnProjectClosed)
	//EVT_EDITOR_OPEN(cbKeyBinder::OnEditorOpen)
	//EVT_EDITOR_CLOSE(cbKeyBinder::OnEditorClose)
	//EVT_PROJECT_OPEN(cbKeyBinder::OnProjectOpened)
	EVT_APP_STARTUP_DONE(cbKeyBinder::OnAppStartupDone)

END_EVENT_TABLE()
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MyDialog, wxPanel)
END_EVENT_TABLE()
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
cbKeyBinder::cbKeyBinder()
// ---------------------------------------------------------------------------
{
	//ctor
	m_PluginInfo.name = _T("cbKeyBinder");
	m_PluginInfo.title = _("Keyboard shortcuts");
	m_PluginInfo.version = _T("0.4.21 (2006/07/17)");
	m_PluginInfo.description <<_("\nCode::Blocks KeyBinder\n\n")
                            << _("Modify any menu key with up to three key\n")
                            << _("combinations. Multiple sets of key profiles\n")
                            << _("can be stored and selected. \n\n")
                            << _("NOTE: Ctrl+Alt+{UP|DOWN} is unsupported.\n\n");
	m_PluginInfo.author = _T("Pecan Heber");
	m_PluginInfo.authorEmail = _T("");
	m_PluginInfo.authorWebsite = _T("");
	m_PluginInfo.thanksTo << _("Thanks to...\n\n")
                        <<_("wxKeyBinder authors:\n")
                        <<_("Aleksandras Gluchovas,\nFrancesco Montorsi,\n\n")
                        <<_("wxWidgets, and\n")
                        <<_("The Code::Blocks team");
	m_PluginInfo.license = LICENSE_GPL;

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
	// m_IsAttached will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...


    // Only stable windows can be attached for codeblocks; currently
    // "SCIwindow" & "notebook". See wxKeyBinder::Attach() in keybinder.cpp

    // Have to have at least one window to attach to else secondary keys wont work
    // and "notebook" windows work just fine. Dont need AppWindow attach

    //get window for log when debugging
    pcbWindow = Manager::Get()->GetAppWindow();

    #if LOGGING
        // allocate wxLogWindow in the header
        //  wxLogWindow* pMyLog;
        // #define LOGIT wxLogMessage
        /* wxLogWindow* */
        pMyLog = new wxLogWindow(pcbWindow,m_PluginInfo.name,true,false);
        wxLog::SetActiveTarget(pMyLog);
        LOGIT(_T("keybinder log open"));
        pMyLog->Flush();
        pMyLog->GetFrame()->Move(20,20);
    #endif

    // Allocate array but do actual key bindings after all menuitems have
    // been implemented by other plugins
	m_pKeyProfArr = new wxKeyProfileArray;
	m_bBound = FALSE;   //say keys are unbound to menus

	// Add window names to which keybinder may attach
	// a "*" allows attaching to ALL windows for debugging
   #if LOGGING
    //wxKeyBinder::usableWindows.Add(_T("*"));                 //+v0.4.4
   #endif
    wxKeyBinder::usableWindows.Add(_T("sciwindow"));           //+v0.4.4
    wxKeyBinder::usableWindows.Add(_T("flatnotebook"));       //+v0.4.4
    //wxKeyBinder::usableWindows.Add(_T("panel"));             //+v0.4.4
    //wxKeyBinder::usableWindows.Add(_T("list"));              //+v0.4.4
    //wxKeyBinder::usableWindows.Add(_T("listctrl"));          //+v0.4.4
    //wxKeyBinder::usableWindows.Add(_T("treectrl"));          //+v0.4.4

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
    if(!m_IsAttached) {	return 0;}

    //call configuation dialogue
    return OnKeybindings(parent);
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

    // init the keybinder
	// memorize incomming menubar
    m_pMenuBar = menuBar;

    // Create filename like cbKeyBinder{pluginversion}v{sdkversion}.ini
    // +v0.4.1 Get major and minor SDK versions to use in filename
    int SDKmajor; int SDKminor; int SDKrelease;
    PluginSDKVersion( &SDKmajor, &SDKminor, &SDKrelease);
    wxString SDKverStr = wxEmptyString;
    SDKverStr.sprintf(_T("%d%d"),SDKmajor,SDKminor);

    //memorize the key file name as {%HOME%}\cbKeyBinder+{vers}.ini
    m_sKeyFilename = ConfigManager::GetConfigFolder();

    //*bug* GTK GetConfigFolder is returning double "//?, eg, "/home/pecan//.codeblocks"
    LOGIT(_T("GetConfigFolder() is returning [%s]"), m_sKeyFilename.GetData());

    // remove the double //s from filename //+v0.4.11
    m_sKeyFilename.Replace(_T("//"),_T("/"));

    // get version number from keybinder plugin
    wxString sPluginVersion = m_PluginInfo.version.BeforeLast('.'); //+v0.4.1

    // remove the dots from version string (using first 3 chars)
    sPluginVersion.Replace(_T("."),_T(""));
    m_sKeyFilename = m_sKeyFilename
         <<wxFILE_SEP_PATH
        <<m_PluginInfo.name<<sPluginVersion
        <<_T("v")<<SDKverStr<<_T(".ini"); //+v0.4.1

    #if LOGGING
     LOGIT(_T("cbKB:BuildMenu()"));
     LOGIT(_T("SDKmajor:%d SDKminor:%d SDKverStr:%s"),SDKmajor,SDKminor,SDKverStr.GetData());
     LOGIT(_T("File:%s"),m_sKeyFilename.GetData());
    #endif

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
    // called when we can't do anything else. Makes a virgin key profile
    // array from the C::B menu items.

 	wxKeyProfile *pPrimary;

	pPrimary = new wxKeyProfile(wxT("Primary"), wxT("Our primary keyprofile"));
	pPrimary->ImportMenuBarCmd(m_pMenuBar);

	#if LOGGING
        LOGIT(_T("cbKB:ReBind:Imported"));
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
        //-r.UpdateAllCmd();		// necessary //+v04.11
        r.UpdateAllCmd(m_pMenuBar); //+v0.4.17

    #if LOGGING
      LOGIT(_T("UpdateArr::End"));
    #endif

}//cbKeyBinder::UpdateArr
// ----------------------------------------------------------------------------
cbConfigurationPanel* cbKeyBinder::OnKeybindings(wxWindow* parent)
// ----------------------------------------------------------------------------
{
    #ifdef LOGGING
     LOGIT(_T("cbKB:OnKeybindings()"));
    #endif

    //wait for a good key file load()
    if (!m_bBound) return 0;

    // Sets options and invokes the configuation dialog
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

    // when the configuration panel is closed with OK, OnKeybindingsDialogDone() will be called
    return dlg;

}//OnKeybindings
// ----------------------------------------------------------------------------
void cbKeyBinder::OnKeybindingsDialogDone(MyDialog* dlg)
// ----------------------------------------------------------------------------
{
    // The configuration panel has run its OnApply() function.
    // So here it's like we were using ShowModal() and it just returned wxID_OK.

    // Apply user keybinder changes to key profile array as if the
    //  old EVT_BUTTON(wxID_APPLY, wxKeyConfigPanel::OnApplyChanges)
    //  in keybinder.cpp had been hit

    bool modified = false;
    dlg->m_p->ApplyChanges();

    // check if any key modifications //v0.4.13
    wxKeyProfileArray* pNewKBA = new wxKeyProfileArray;
    *pNewKBA = dlg->m_p->GetProfiles();
    int newSel = pNewKBA->GetSelProfileIdx();
    int oldSel = m_pKeyProfArr->GetSelProfileIdx();

    if ( ( newSel == oldSel )
        && ( *(pNewKBA->Item(newSel)) == *(m_pKeyProfArr->Item(oldSel)) ) )
    {
        LOGIT(_T("DialogDone: NO key changes NewIdx[%d] OldIdx[%d]"),
                newSel, oldSel);
    }
    else
    {   // update our array (we gave a copy of it to MyDialog)
        modified = true;
        *m_pKeyProfArr = dlg->m_p->GetProfiles();
        LOGIT(_T("DialogDone keys MODIFIED"));
    }
    delete pNewKBA;
    // don't delete dlg; CodeBlocks will destory it

    //update Windows/EventHanders from changed wxKeyProfile
    if ( modified )
    {   // update attaches and menu items
        UpdateArr(*m_pKeyProfArr) ;
        //Save the key profiles to external storage
        OnSave();
    }//fi

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
}//OnKeybindingsDialogDone
// ----------------------------------------------------------------------------
void cbKeyBinder::OnLoad()
// ----------------------------------------------------------------------------
{
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

	wxString strLoadFilename = m_sKeyFilename;
	#if LOGGING
	 LOGIT(_T("cbKB:Loading File %s"), strLoadFilename.GetData());
	#endif

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
			  LOGIT(_T("cbKB:OnLoad:Menu Imported"));
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
             LOGIT(_T("cbKeyBinder Matched %d MenuItems"), total);
            #endif
		 }//endelse

		// reattach frames to the loaded keybinder
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
	 LOGIT(_T("OnLoad()End\n"));
	#endif

	return;

}//OnLoad
// ----------------------------------------------------------------------------
void cbKeyBinder::OnSave()
// ----------------------------------------------------------------------------
{
    // Save the key profile(s) to a file

    // delete the key definition file (removes invalid menuitem id's)
    bool done = ::wxRemoveFile(m_sKeyFilename);
     if (done)
    {
        #if LOGGING
          { LOGIT(_T("cbKB:File %s deleted."),m_sKeyFilename.GetData()); }
        #endif
    }//if (done..

	wxString path = m_sKeyFilename;
	//path.Replace(_T(".ini"),_T(""));
	LOGIT( _T("cbKB:SavePath %s"), path.GetData() );
	wxFileConfig *cfg = new wxFileConfig(wxEmptyString,wxEmptyString,path); //v04.11

	if (m_pKeyProfArr->Save(cfg, wxEmptyString, TRUE))
	 {
		// get the cmd count
		int total = 0;
		for (int i=0; i<m_pKeyProfArr->GetCount(); i++)
			total += m_pKeyProfArr->Item(i)->GetCmdCount();

//		wxMessageBox(wxString::Format(wxT("All the [%d] keyprofiles ([%d] commands ")
//			wxT("in total) have been saved in \n\"")+path, //+wxT(".ini\""),
//            m_pKeyProfArr->GetCount(), total),
//			wxT("Save"));

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
}

// ----------------------------------------------------------------------------
MyDialog::~MyDialog() {}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void MyDialog::OnApply()
// ----------------------------------------------------------------------------
{
    m_pBinder->OnKeybindingsDialogDone(this);
}
// ----------------------------------------------------------------------------
//#if 0
//// ----------------------------------------------------------------------------
//void cbKeyBinder::OnProjectOpened(CodeBlocksEvent& event)
//// ----------------------------------------------------------------------------
//{
//    //////////////////////////////////////////////////////////////////////
//    //*Deprecated*Unused*Deprecated*Unused*Deprecated*Unused*Deprecated*//
//    //////////////////////////////////////////////////////////////////////
//
//    if (m_IsAttached)
//     {
//        #if LOGGING
//          LOGIT(_T("cbKB:ProjectOpened"));
//        #endif
//     }
//    event.Skip();
//}
//// ----------------------------------------------------------------------------
//void cbKeyBinder::OnProjectActivated(CodeBlocksEvent& event)
//// ----------------------------------------------------------------------------
//{
//    //////////////////////////////////////////////////////////////////////
//    //*Deprecated*Unused*Deprecated*Unused*Deprecated*Unused*Deprecated*//
//    //////////////////////////////////////////////////////////////////////
//
//    if (m_IsAttached)
//     {
//        #if LOGGING
//          LOGIT(_T("cbKB:ProjectActivated"));
//        #endif
//     }
//    event.Skip();
//}
//// ----------------------------------------------------------------------------
//void cbKeyBinder::OnProjectClosed(CodeBlocksEvent& event)
//// ----------------------------------------------------------------------------
//{
//    //////////////////////////////////////////////////////////////////////
//    //*Deprecated*Unused*Deprecated*Unused*Deprecated*Unused*Deprecated*//
//    //////////////////////////////////////////////////////////////////////
//
//    if (m_IsAttached)
//     {
//        #if LOGGING
//          LOGIT(_T("cbKB:ProjectClosed"));
//        #endif
//
//        //get rid of unused editor ptr space
//        m_EditorPtrs.Shrink();
//    }
//    event.Skip();
//}
//// ----------------------------------------------------------------------------
//void cbKeyBinder::OnProjectFileAdded(CodeBlocksEvent& event)
//// ----------------------------------------------------------------------------
//{
//    //////////////////////////////////////////////////////////////////////
//    //*Deprecated*Unused*Deprecated*Unused*Deprecated*Unused*Deprecated*//
//    //////////////////////////////////////////////////////////////////////
//
//    if (m_IsAttached)
//     {
//        #if LOGGING
//          LOGIT(_T("cbKB:ProjectFileAdded"));
//        #endif
//     }
//    event.Skip();
//}
//// ----------------------------------------------------------------------------
//void cbKeyBinder::OnProjectFileRemoved(CodeBlocksEvent& event)
//// ----------------------------------------------------------------------------
//{
//    //////////////////////////////////////////////////////////////////////
//    //*Deprecated*Unused*Deprecated*Unused*Deprecated*Unused*Deprecated*//
//    //////////////////////////////////////////////////////////////////////
//
//    if (m_IsAttached)
//     {
//       #if LOGGING
//        LOGIT(_T("cbKB:ProjectFileRemoved"));
//       #endif
//     }
//    event.Skip();
//}
//// ----------------------------------------------------------------------------
//#endif //if 0
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void cbKeyBinder::AttachEditor(wxWindow* pWindow)
// ----------------------------------------------------------------------------
{
    if (m_IsAttached)
     {
         wxWindow* thisEditor = pWindow->FindWindowByName(_T("SCIwindow"),pWindow);

         // find editor window the Code::Blocks way
         // find the cbStyledTextCtrl wxScintilla "SCIwindow" to this EditorBase
//         cbEditor* ed = 0;
//         EditorBase* eb = event.GetEditor();
//         if (eb && eb->IsBuiltinEditor())
//          {  ed = static_cast<cbEditor*>(eb);
//             thisEditor = ed->GetControl();
//          }

        //skip editors that we already have
        if ( thisEditor && (wxNOT_FOUND == m_EditorPtrs.Index(thisEditor)) )
         {
            //add editor to our array and push a keyBinder event handler
            m_EditorPtrs.Add(thisEditor);
            //Rebind keys to newly opened windows
            m_pKeyProfArr->GetSelProfile()->Attach(thisEditor);
            #if LOGGING
             LOGIT(_T("cbKB:AttachEditor %s %p"), thisEditor->GetTitle().c_str(), thisEditor);
            #endif
         }
     }
}
// ----------------------------------------------------------------------------
void cbKeyBinder::DetachEditor(wxWindow* pWindow)
// ----------------------------------------------------------------------------
{
    if (m_IsAttached)
     {

         //wxWindow* thisWindow = pWindow;

         // Cannot use GetBuiltinActiveEditor() because the active Editor is NOT the
         // one being closed!!
         // wxWindow* thisEditor
         //  = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor()->GetControl();

         //find the cbStyledTextCtrl wxScintilla window
         //wxWindow*
         //  thisEditor = thisWindow->FindWindowByName(_T("SCIwindow"), thisWindow);

         // find editor window the Code::Blocks way
         // find the cbStyledTextCtrl wxScintilla "SCIwindow" to this EditorBase
         //cbEditor* ed = 0;
         //EditorBase* eb = event.GetEditor();
         //if (eb && eb->IsBuiltinEditor())
         // {  ed = static_cast<cbEditor*>(eb);
         //    thisEditor = ed->GetControl();
         // }

        if ( pWindow && (m_EditorPtrs.Index(pWindow) != wxNOT_FOUND) )
         {
            #if LOGGING
             LOGIT(_T("cbKB:DetachEditor %s %p"), pWindow->GetTitle().c_str(), pWindow);
            #endif
            m_pKeyProfArr->GetSelProfile()->Detach(pWindow);
            m_EditorPtrs.Remove(pWindow);
         }//if
     }

}//DetachEditor
// ----------------------------------------------------------------------------
//#if 0
//// ----------------------------------------------------------------------------
//void cbKeyBinder::OnEditorOpen(CodeBlocksEvent& event)
//// ----------------------------------------------------------------------------
//{
//    //////////////////////////////////////////////////////////////////////
//    //*Deprecated*Unused*Deprecated*Unused*Deprecated*Unused*Deprecated*//
//    //////////////////////////////////////////////////////////////////////
//
//    if (m_IsAttached)
//     {
//         //LOGIT(_T("cbKB:OnEditorOpen()"));
//        if (!m_bBound)
//         {
//            OnLoad(); event.Skip(); return;
//         }
//
//        //already bound, just add the editor window
//         wxWindow* thisWindow = event.GetEditor();
//         wxWindow* thisEditor = thisWindow->FindWindowByName(_T("SCIwindow"),thisWindow);
//
//         // find editor window the Code::Blocks way
//         // find the cbStyledTextCtrl wxScintilla "SCIwindow" to this EditorBase
//         cbEditor* ed = 0;
//         EditorBase* eb = event.GetEditor();
//         if (eb && eb->IsBuiltinEditor())
//          {  ed = static_cast<cbEditor*>(eb);
//             thisEditor = ed->GetControl();
//          }
//
//        //skip editors that we already have
//        if ( thisEditor && (wxNOT_FOUND == m_EditorPtrs.Index(thisEditor)) )
//         {
//            //add editor to our array and push a keyBinder event handler
//            m_EditorPtrs.Add(thisEditor);
//            //Rebind keys to newly opened windows
//            m_pKeyProfArr->GetSelProfile()->Attach(thisEditor);
//            #if LOGGING
//             LOGIT(_T("cbKB:OnEditorOpen/Attach %s %p"), thisEditor->GetTitle().c_str(), thisEditor);
//            #endif
//         }
//     }
//     event.Skip();
//}
//// ----------------------------------------------------------------------------
//void cbKeyBinder::OnEditorClose(CodeBlocksEvent& event)
//// ----------------------------------------------------------------------------
//{
//    //////////////////////////////////////////////////////////////////////
//    //*Deprecated*Unused*Deprecated*Unused*Deprecated*Unused*Deprecated*//
//    //////////////////////////////////////////////////////////////////////
//
//    if (m_IsAttached)
//     {
//
//         wxWindow* thisWindow = event.GetEditor();
//
//         // Cannot use GetBuiltinActiveEditor() because the active Editor is NOT the
//         // one being closed!!
//         // wxWindow* thisEditor
//         //  = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor()->GetControl();
//
//         //find the cbStyledTextCtrl wxScintilla window
//         wxWindow*
//           thisEditor = thisWindow->FindWindowByName(_T("SCIwindow"), thisWindow);
//
//         // find editor window the Code::Blocks way
//         // find the cbStyledTextCtrl wxScintilla "SCIwindow" to this EditorBase
//         cbEditor* ed = 0;
//         EditorBase* eb = event.GetEditor();
//         if (eb && eb->IsBuiltinEditor())
//          {  ed = static_cast<cbEditor*>(eb);
//             thisEditor = ed->GetControl();
//          }
//
//        if ( thisEditor && (m_EditorPtrs.Index(thisEditor) != wxNOT_FOUND) )
//         {
//            m_pKeyProfArr->GetSelProfile()->Detach(thisEditor);
//            m_EditorPtrs.Remove(thisEditor);
//            #if LOGGING
//             LOGIT(_T("cbKB:OnEditorClose/Detach %s %p"), thisEditor->GetTitle().c_str(), thisEditor);
//            #endif
//         }//if
//     }
//    event.Skip();
//}//OnEditorClose
//// ----------------------------------------------------------------------------
//#endif //if 0
// ----------------------------------------------------------------------------
void cbKeyBinder::OnAppStartupDone(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // if keys still unbound, do it here.

    // load key binding from file
    if (!m_bBound)
     {
        #if LOGGING
         LOGIT(_T("cbKeyBinder:Begin initial Key Load"));
        #endif
        m_bBound=TRUE;
        OnLoad();
        #if LOGGING
         LOGIT(_T("cbKeyBinder:End initial Key Load"));
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
            if ( pWindow && (pRightSplitWin eq 0) )
            {
                //-if (pRightSplitWin eq pWindow)
                //-{    Attach(pRightSplitWin);
                if (pWindow->GetParent() eq ed)
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
    if (not m_IsAttached){event.Skip(); return; };

    wxWindow* pWindow = (wxWindow*)(event.GetEventObject());

    if ( (pWindow) && (m_EditorPtrs.Index(pWindow) != wxNOT_FOUND))
    {
        DetachEditor(pWindow);
        #ifdef LOGGING
         LOGIT( _T("OnWindowDestroyEvent Remove %p"), pWindow);
        #endif //LOGGING
    }
    event.Skip();
}//OnWindowClose
// ----------------------------------------------------------------------------
