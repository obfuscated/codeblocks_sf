/***************************************************************
 * Name:      cbkeybinder.h
 * Purpose:   Code::Blocks plugin
 * Author:    Pecan Heber
 * Copyright: (c) Pecan Heber
 * License:   GPL
 **************************************************************/
// RCS-ID:      $Id$

#ifndef CBKEYBINDER_H
#define CBKEYBINDER_H

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma interface "cbkeybinder.h"
#endif

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include <wx/string.h>
#include <wx/timer.h>

#include "cbplugin.h" // the base class we 're inheriting
#include "configurationpanel.h"
#include "keybinder.h"

// --Version-Rlease-Feature-Fix-------
#define VERSION "1.0.50 2012/09/1"
// -----------------------------------
class MyDialog;
class wxKeyConfigPanel;
class wxKeyProfileArray;
class wxWindow;
class wxEvent;
class wxMenuBar;
class wxLogWindow;
class wxArrayPtrVoid;

// ----------------------------------------------------------------------------
//  cbKeyBinder class declaration
// ----------------------------------------------------------------------------
class cbKeyBinder : public cbPlugin
{
    friend class MyDialog;
	public:
		cbKeyBinder();
		~cbKeyBinder();
		int GetConfigurationGroup() const { return cgEditor; }
        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
		void BuildMenu(wxMenuBar* menuBar);
		void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
		bool BuildToolBar(wxToolBar* toolBar);
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application

      #ifdef LOGGING
        // allocate wxLogWindow in the header
        wxLogWindow* pMyLog;
      #endif

        // the array of key profiles used by this sample app
        wxKeyProfileArray* m_pKeyProfArr;

        //memorized menubar from BuildMenu(...)
        wxMenuBar* m_pMenuBar;

        // possible locations of config file
        wxString m_sConfigFolder;
        wxString m_sExecuteFolder;
        wxString m_sDataFolder;

        // Users Key file name eg. %HOME%\cbKeybinder.ini
        wxString m_sKeyFilename;
        wxString m_sKeyFilePath;
        wxString m_OldKeyFilename;

        // Switch to reload keybinding
        bool m_bBound;

        // utility function to update the menu key labels
        // and re-attach the window event handlers after key changes
        void UpdateArr(wxKeyProfileArray &r);
        void Rebind(bool update=true);
        // key definition configuration dialog
        cbConfigurationPanel* OnKeyConfig(wxWindow* parent);
        void OnKeyConfigDialogDone(MyDialog* dlg);

        // save/load key definitions
        void OnSave(bool backitup = false);
        void OnLoad();
        // Enable/Disable Merge
        int EnableMerge(bool allow);
        int IsEnabledMerge() const {return m_mergeEnabled;}
        bool IsMerging() const {return m_bMerging;}
        void IsMerging(bool state){m_bMerging = state;}
        bool VerifyKeyBind(const wxString& strKeyCode, const int numShortcuts);
        int RemoveKeyBindingsFor(const wxString& strKeyCode, wxKeyProfile* pkp);

    protected:
        wxADD_KEYBINDER_SUPPORT();

    private:
        void OnProjectOpened(CodeBlocksEvent& event);
        void OnProjectActivated(CodeBlocksEvent& event);
        void OnProjectClosed(CodeBlocksEvent& event);
        void OnProjectFileAdded(CodeBlocksEvent& event);
        void OnProjectFileRemoved(CodeBlocksEvent& event);
        void OnEditorOpen(CodeBlocksEvent& event);
        void OnEditorClose(CodeBlocksEvent& event);
        void OnSplit(CodeBlocksEvent& event);
        void OnUnsplit(CodeBlocksEvent& event);
        void OnMergeTimer(wxTimerEvent& event);
        void OnAppStartupDone(CodeBlocksEvent& event);
        void AttachEditor(wxWindow* pEditor);
        void DetachEditor(wxWindow* pWindow, bool deleteEvtHandler = true);
        void MergeDynamicMenus();
        void OnAppStartShutdown(CodeBlocksEvent& event);
        void OnIdle(wxIdleEvent& event);
        void OnTimerAlarm(wxTimerEvent& event);
        void OnMenuBarModify(CodeBlocksEvent& event);
        wxString FindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName);
        void MergeAcceleratorTable(const bool mergeAccelTable = false);
        int  RemoveCopyPasteBindings(wxKeyProfile* pkp);


        wxWindow*       pcbWindow;              //main app window
        wxArrayPtrVoid  m_EditorPtrs;           //attached editor windows
        bool            bKeyFileErrMsgShown;
        int             m_MenuModifiedByMerge;  //menu dynamically modified
        int             m_mergeEnabled;         //merging menus allowed
        bool            m_bMerging;             //merging menus active
        bool            m_menuPreviouslyBuilt;

    private:
        wxTimer         m_Timer;
        void    StartMergeTimer(int secs){ m_Timer.Start( secs*1000, wxTIMER_ONE_SHOT); }
        void    StopMergeTimer(){ m_Timer.Stop();}
        bool    m_bTimerAlarm;
        bool    m_bAppShutDown;
        bool    m_bConfigBusy;

		DECLARE_EVENT_TABLE()

};//class cbKeyBinder
// ----------------------------------------------------------------------------
//  MyDialog class declaration
// ----------------------------------------------------------------------------
class MyDialog : public cbConfigurationPanel
{
public:
	wxKeyConfigPanel *m_p;

public:
    // ctor(s)
    MyDialog(cbKeyBinder* binder, wxKeyProfileArray &arr, wxWindow *parent, const wxString& title, int);
	~MyDialog();


    wxString GetTitle() const { return _("Keyboard shortcuts"); }
    wxString GetBitmapBaseName() const { return _T("onekeytobindthem"); }
	void OnApply();
	void OnCancel(){}

private:
    cbKeyBinder* m_pBinder;
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

#endif // CBKEYBINDER_H
// 11/9/2005
// C::B KeyBinder
// 2005 Nov 17
// ----------------------------------------------------------------------------
//cbKeyBinder is an adaptation of wxKeyBinder as a Code::Blocks plugin.
//
//It provides the C::B user an ability to bind custom key combinations to
//the C::B menu items.
//
//Some key combinations cannot be overridden by cbKeyBinder.
//For example, Ctrl+B executes Toggle Bookmark even when
//assigned to another menu item. The same is true for Ctrl+Shift+N
//(New Project). Some keys belong to wxScintilla and others have mutilple
// definitions and the second menu item is not overridden by keybinder.
//
// ----------------------------------------------------------------------------
//== bugs ==
// -done- Second hotkey doesnt trigger
// -done- Adding hotkey to third level menus doesnt trigger
//
// -done- getting periodic crashes defining key in dlg, Detatch??
//      happens when a new project is opened. Keys are lost and
//      invoking cbKeyBinder/Apply crashes C::B
//      A: wxCmdItem.GetLabel() unable to handle menu items with
//       filenames. the '\'s gave it a pain in the asm
//
// -done- bindings are being lost periodically also
//          A: same as above
// -done- Save/Load hotkeys
//          A: taken care of by dialog plus Onsave/OnLoad

//== Needs ==
// put messageboxs' 'ok' box under the mouse cursor
// -done Avoid running multiple copies or multiple versions
//      mutex would be nice
//      using version in file name
// Avoid running after C::B has own wxKeyBinder
// Add macro keys ability eg. Ctrl+Enter = {END}{ENTER}
//  yeah, Right!

//RC3 2005 Dec 10
//+v0.4
//  -done- Change file name to RC3 location + codeblocks version
//  -done- Dont memorize "filename" menu items
//  -done- Find correct eventHandler to pop (not just first)
//
//  -done-mandrav- Use the .conf way to save keybindings file ala mandrav 12/12/2005
//
//  -done-?what is
//  -done-    DetachAll - detaching from all my [0] targets message when
//  -done-    invoking dlg
//  -done-    A: caused by Rebind() doing DetachAll()/cleanup() before UpdateArr()
//  -done-      Ok. No harm done.
//
//  -done- Any keys refuse to be bound? 12/12/2005
//  -done- Re-Allow Secondary/Additional profiles v0.4.2
//  -done-mandrav=Find better way to load keys after CB initialization 12/12/2005
//  -done- After a plugin Menu change a plugin Shortcut key is assigned to wrong menu itme
//         A: verified key from file by Menu name NOT menu id
//  -done- After re-assignment and re-saving(the above), File description is wrong on disk
//  -done-  checkout MainWin vs cbMainWindow and remove code
//          A: attach()ing to "notebook" instead

// v0.4.1 12/30/2005
//  added event.Skip() to cbKeyBinder::OnAppStartupDone(CodeBlocksEvent& event)
//  enabled multiple profiles

// v0.4.2 1/2/2006 6PM
// keybinder attaching some windows named 's'
// so verify full name of window not just substring
// attach to text windows allowing keybinder invocation when no editor

// v0.4.4 1/7/2006 1
// additions to keybinder::Update() for use of bitmapped menuitems
// made keybinder "usableWindow" filter efficient (cf. Attach())
// add recursive "winExist" check to stop disappearing panels crash
// ---------------------------------------------------------------------------
// Commit 12/14/2005 9:15 AM
// ---------------------------------------------------------------------------
//   closed  Project tree Ctrl+Shift+{ or } gets following log msgs:
//              15:01:24: wxMenuCmd::Update - setting the new text to [Activate prior project	Alt+F5]
//              15:01:24: wxKeyBind::KeyCodeToString - unknown key: [123]
//              15:01:24: wxMenuCmd::Update - setting the new text to [Move project up	Ctrl+Shift+]
//              15:01:24: wxKeyBind::KeyCodeToString - unknown key: [125]
//           A: checked keys as "isPrintable" instead of "isAlNum"

//   closed 12/15/2005 3:38 PM
//          Ctrl+Alt+Up/Down Ctrl+Shift+Up/Down dont record at all
//          A:wxWidgets2.6.2 returns false on modifier keys for Ctrl+Alt+UP/DOWN combination.
//          It returns Ctrl+Alt+PRIOR/NEXT instead of UP/DOWN and shows false for ctrl & alt.
//          Same is true for Ctrl+Shift+UP/Down.
//          Alt+Shift+Up/Down work ok.
//

//   closed 12/15/2005 5:37 PM
//          Shift-UP recorded as shift+{ and shift+down=shift+}
//          Any UP is getting recorded as {
//          Looks like all the UP/DOWN/LEFT/RIGHT are being turned to alpha
//          AND, they dont work
//          A: keys not defined in StringToKeyCode()
//

//  closed  12/16/2005 8:52 PM opened    12/15/2005 10:29 PM
//          wxMenuCmd::Update() is verifying by id, not text
//          could be causing the missing hotkey label above
// ---------------------------------------------------------------------------
// commit 1/7/2006
// ---------------------------------------------------------------------------
//Commited Keybinder ver 0.4.4 1/7/2006 10:56 PM
//
//Re: keybinder clobbers menuitems with bitmaps.
//
//It's been a real educational tracing through WX262 menu
//code this past week. It appears to me that the menu bitmap code
//is a paste and forget job. Very disappointing.
//
//The XRC code "owner draws" the bitmap onto the menu, then
//the menu code willy nilly turns off the "m_bOwnerDrawn" flag.
//Bitmapped menuitems are not supported in WX, menu code, only OwnerDrawn.
//
//Reason: Looks like a Win98 thingie. But...
//
//The next time the menuitem is updated, ownerdrawn attributes, bitmaps
//etc are clobbered because, of course, the owner drawn code is never called.
//
//If the modifying coder turns owner drawn back on, the width of the
//menuitem is ignored, the accelerator is drawn "right aligned", thus
//clobbering (drawing over) the menuitem text.
//
//Hack: SetText without OwnerDrawn to guarantee the menuitem width, then do it again
//with OwnerDrawn true to redraw/reinstate the bitmap. Jeeezzh!!!
//
//[code]
//		// set "un-ownerdrawn" text to preserve menu width
//        m_pItem->SetText(newtext);
//        //now redraw the menuitem if bitmapped
//		if (m_pItem->GetBitmap().GetWidth())
//        {   m_pItem->SetOwnerDrawn();
//            m_pItem->SetText(newtext);
//        }
//        //-m_pItem->GetMenu()->UpdateAccel(m_pItem); //<--does nothing previous SetTExt() didnt
//[/code]
//
//The results are not pretty. The margins between the bitmap and the text is changed,
//and the accelerator is vertically misaligned with its siblings  because it's
//drawn "right aligned" into the menuitem. Yuk!
//
//So: I'm commiting the "fixed" keybinder, and will now find a way to draw
//it as pretty as XRC does it. I think this is important, because we're all
//gonna have to mangle a menu someday.
//
//thanks
//pecan
//  closed  1/7/2006 8:42 PM
//          adding/removing/loading keys clobbered menu bitmaps
//          loading keys clobbered menu nmemonics
//          attaching to unwanted windows
//          crashing when "panel" disappeared
// ---------------------------------------------------------------------------
//commit 1/8/2006 9:47 AM v0.4.5
// ---------------------------------------------------------------------------
//  closed  new windows code in Update() was clobbersing GTK code
//          surrounded "OwnerDrawn" code with # if/endif(s)
//
// ---------------------------------------------------------------------------
//commit 1/9/2006 9 AM v0.4.6
// ---------------------------------------------------------------------------
//  closed  1/9/2006 9 PM opened    1/7/2006 8:47 PM
//          modifying menuitem modifies bitmap spacing/margins?
//          menus items dont align vertically after modification
//          A: rebuild (Destroy/Insert) items with bitmaps
//
// ---------------------------------------------------------------------------
//  open    1/7/2006 8:44 PM
//          Detach() may be leaking when winExist==false
//          verify eventhandler is off the event chain
//
// ----------------------------------------------------------------------------
//commit 1/10/2006 5:56 PM v0.4.8
// ----------------------------------------------------------------------------
//  closed  1/10/2006 4:14 PM opened 12/15/2005 5:42 PM
//          Adding hotkey to Plugins works, but doesnt show
//           A: havent been able to reproduce since above changes
//          On occation, hotkeys don't show on the menus but they still work.
//          Had been adding/removing a lot of keys.
//          Especially noticed in plugin menu items.
//          Removed a key, then they appeared.
//          See the -disappeared- item above
//          A: ini file had duplicate menuitems, one from settings
//          and one from help. The Help entry would reset the "settings"
//          menuitem because it had no accerator and we searched by
//          text. Fixed by searching first by id+text in keybinder::CreateNew()
// closed   1/10/2006 5:57 PM
//          found potential crash situation when event handler tried to
//          free itself but the window had already been closed.
//          A: if NOT winExists, RemoveEvtHandler() using main window ptr
//          in wxKeybinder::DetachAll() and wxBinderEventHander::~dtor
//
// ----------------------------------------------------------------------------
//commit 1/11/2006 1:22 PM v0.4.9
// ----------------------------------------------------------------------------
// Modify   1/11/2006 10:50 AM
//          changed direct stow of wxBinderEventHander::m_pTarget to SetInvalid()
//          from keybinder::DetachAll() to avoid crash on closed window pointer;
//
//  closed  1/11/2006 1:20 PM opened 1/11/2006 1:20 PM
//          Re: wxKeyBinder problems/solutions discussion
//          « Reply #7 on: Today at 01:06:03 PM »
//      	Reply with quote
//          Quote from: Pecan on January 10, 2006, 06:40:49 PM
//          ...Have commited KeyBinder v0.4.8 (svn 1708)...
//          you've used wxMenuItem::SetFont & ::GetFont
//          these are windows-only ! your plugin doesn't build under linux
//          Code:
//          menuutils.cpp: In member function 'void wxMenuCmd::RebuildMenuitem()':
//          menuutils.cpp:162: error: 'class wxMenuItem' has no member named 'SetFont'
//          menuutils.cpp:162: error: 'class wxMenuItem' has no member named 'GetFont'
// ----------------------------------------------------------------------------
//commit 1/11/2006 3 PM v0.4.10
// ----------------------------------------------------------------------------
//	closed  open	1/11/2006 2:28 PM
//		    GTK mnemonics incorrectly updated;
//		    completely seperated out the GTK Update() routine
//
// ----------------------------------------------------------------------------
//commit 1/17/2006 v0.4.11
// ----------------------------------------------------------------------------
//	closed  1/17/2006 9:04 PM open	]1/17/2006 11:07 AM
//		    GTK Update() menuitems crashing on disappearing & numeric menuitems
//          GetConfigFolder() returning double "//"s like "/home/pecan//.codeblocks" ?
//  closed  1/17/2006 9:05 PM open    1/17/2006 5:50 PM
//          GTK - keybinder file is not being saved.
//  mod     1/17/2006 5:51 PM
//          moved UpdateAllCmd() to cbKeybinder.cpp Updatearr instead of
//          keybinder::Attach() becaue it was being invoked for every attach()
//          made to an object. Very slow on GTK
//
// ----------------------------------------------------------------------------
// commit 1/23/2006 v0.4.13
// ----------------------------------------------------------------------------
//  mod     1/22/2006 5:33 PM
//          m_PluginInfo.hasConfigure removed by SDK change
//  closed  1/23/2006 V0.4.13
//          SDK settings scheme broke keybinder updates
//
// ----------------------------------------------------------------------------
// commit   2/9/2006 v0.4.15
// ----------------------------------------------------------------------------
//  mod     1/25/2006 12:00 PM v0.4.14
//          removed delete dlg from cbKeyBinder.cpp since it's handed
//          off to CodeBlocks cbConfigureationDialog
//  mod     Removed full class names from cbKeyBinder.H event table.
//          as requested by bug report
//          http://sourceforge.net/tracker/index.php?func=detail&aid=1411997&group_id=126998&atid=707418
//          fixed {PRIOR/NEXT} keys unrecognized by wxGetAccelFromString()
// ----------------------------------------------------------------------------
//  commit  2006/03/25 v 0.4.15
// ----------------------------------------------------------------------------
//  fixed   possible array overflow and unitialized integer
// ----------------------------------------------------------------------------
//  closed  2006/04/23  open    2006/04/10 v0.4.16
//          Ctrl-Shift-N cannot be overridden unless its the first accelerator set
//          on a menu item.
//          There are two NewProject menu items/accerators with the same id
//          on the menu and keybinder never recorded the second.
//          Removing the second definition of Ctrl-Shift-N from main_menu.xrc would
//          solve the problem.
// -----------------------------------------------------------------------------
//  fixed   2006/04/22 v0.4.17
//          Appease linux gcc 4.0.2 compiler by putting extra ()'s
//          around keybinder.cpp 2219 if(assignment statement)
//          Removed RC2 code and references
//          Fixed duplicate menu items not being updated by using the menu bar
//          as the source to search the keybinder array rather than the reverse.
// -----------------------------------------------------------------------------
//  commit  2006/04/23 v0.4.17
// -----------------------------------------------------------------------------
//  fixed   2006/06/12 v0.4.19
//          Added EVT_CREATE & EVT_DESTROY to catch wxSplitterWindow activity.
//          CodeBlocks provided no events for split/unsplit windows
// -----------------------------------------------------------------------------
//  closed  2006/06/12 opened    2006/06/12
//          Duplicate key problem again. On restart, the second duplicate menu
//          item takes on its default value. eg., File/New Project=Ctrl+Alt+5,
//          but Project/New Project=Ctrl+Shift+N. I suspect because there is no
//          entry in the key file for the duplicate menu item.
//          A: Caused by misspelled "flatnotebook". We need at least one attach
//          to take place for UpdateAllCmd(pMenuBar) to update the menu items.
// -----------------------------------------------------------------------------
//  commit  2006/06/14 v0.4.19
// -----------------------------------------------------------------------------
//  fixed   Getting compile error on linux. Missing editormanager.h
// -----------------------------------------------------------------------------
//  commit  2006/06/15 v0.4.20
// -----------------------------------------------------------------------------
//  closed  2006/07/12 open    2006/07/11 reverted
//          Remove dependency on CodeBlocks editor open/close events
//          in order to avoid leaks on splitWindows.
//          Solution: removed pushed evenHandlers and used Connect()/Disconnect()
// -----------------------------------------------------------------------------
//  closed  2006/07/12 open 2006/07/12 reverted
//          Secondary profiles are not being recorded. Getting
//          "DialogDone: NO key changes" message, the Primary is then set
//          Resolution: cbKeyBinder::OnKeyConfigDialogDone compare function not
//          taking into account multiple keyprofiles in the keyBinderProfileArray.
// -----------------------------------------------------------------------------
//  closed  2006/07/15 open    2006/07/15 reverted
//          keybinder not clearing previous profile keys when loading other
//          profiles.
//          Resolution: Code not updating deep menu items. Walk down to deepest menu
//          items in wxKeyBinder::UpdateAllCmd via new UpdateSubMenu recursion.
// -----------------------------------------------------------------------------
//  commit  v0.4.21 2006/07/17 reverted
// -----------------------------------------------------------------------------
//  closed  open    2006/07/19 v0.4.23a reverted
//          Allowing secondary key definitions on "Quit" causes CB to crash
//          during termination. A hack ignores these keys in wxKeyBinder::OnChar.
//          But this needs to be fixed.
//          Tried: Sending the quit keys to the main application window does not
//          solve the crashes.
//  closed  2006/08/11 open    2006/07/19
//          I experienced again that the menus did not show key redefinitions that
//          actually were working and showed correctly in the dialog. As if updateAll
//          was screwed up again.
// -----------------------------------------------------------------------------
//  commit  2006/07/19 v0.22 reverted
//          Temporary hack to ignore "Quit" menu redefinitions to avoid
//          crashes during CB termination.
// -----------------------------------------------------------------------------
//  open    2006/07/19 reverted
//          Secondary keys do not work from the main html window.
//          Even hooking all windows does not allow seconary keys to work at htmlWindow.
//          Primary keys work ok.
//          The following does *not* solve the problem
//          if ( pcbWindow->GetName() eq _T("frame") )
//              pcbWindow->SetName(_T("Code::Blocks"));
// -----------------------------------------------------------------------------
//  commit  2006/07/29 v0.4.23
//          reverted  2006/07/29 to v0.4.20 CB 2761
//          crashes during CB exit
// -----------------------------------------------------------------------------
//  fixed   2006/08/11 v0.4.24
//          Reapplied recursive UpdateAll() to update sub menus and catch
//          duplicate key definitions. Had been removed by reverting to v0.4.20
// -----------------------------------------------------------------------------
//  fixed   2006/08/11 v0.4.24
//          Reapplied fix for multiple key profiles to compare arrays and update
//          key definitions. Had been removed by reverting to v0.4.20
// -----------------------------------------------------------------------------
//  commit  2006/08/13 v0.4.24
// -----------------------------------------------------------------------------
//  opened  2006/08/20
//          Not recognizing dynamically assigned menu keys after initialization.
//          Re-setting F1 back to first menu item when plugin had set another.
//  fixed   2006/08/31
//          Added MergeDynamicMenuItems() to update key profile array
//          Added dynamic scan and save() at wxIdleEvent every 15 seconds.
// -----------------------------------------------------------------------------
//  opened  2006/08/22
//          Corrupted ini file msg needs to say where file is located.
//  fixed   2006/08/31
//          Added global pointer pKeyFilename to get filename to include with msg.
//          Added save to...ini.bak file before writing new .ini file.
// -----------------------------------------------------------------------------
//  note    2006/09/2
//          wxWidgets 2.6.3 fixed the bitmapped menu icon being clobbered when
//          SetText() was issued.
//          When, in wxKeyBinder::MergeSubMenu(), a RemoveShortcut() or
//          AddShortCut() is issued, Update() is called to update the app menu item.
//          Update() Destroy()ed and New'ed the app menuitem. This caused MergeSubMenu
//          to crash because the app menu chain was changed within this recursive
//          routine.
//          Because 2.6.3 was fixed, I removed the RebuildMenuItem() routine to
//          avoid altering the menuitem chain and to avoid the crash.
//          But this means that the bitmaps will be clobbered by SetText()
//          under 2.6.2 or earlier.
//  note    2006/09/2
//          Was testing on wxGTK 2.6.2. wxMenuItem->GetText() did not return the shortcut
//          key with the string. Had to use wxAcceleratorEntry routines to get the keys.
//          This works also for wxMSW. Had to trim all Labels and Texts from wxGTK
//          menu items. GTK resets all shortcut keys with +'s and keyBinder appends a space
//          to the SetText() menu items to avoid a GTK optimization.
//
//          GTK onIdle() is entered at much longer intervals than MSW, so dynamic
//          shortcut changes are recorded only on "idle" entries, but no shorter than
//          15 second intervals.
// -----------------------------------------------------------------------------
//  commit   v0.4.25d 2006/09/20
//          - recording dynamically changed menu items
//          - Get menu shortcuts via wxAcceratorEntry
//          - Add file name to corrupted file message
//          - Non Destructive update of menu items
//          - backup of .ini file before delete/save
// -----------------------------------------------------------------------------
//  Commit  v0.4.26 2006/09/23
//          - minor #defines for WXMAC
//          - stop OnProfileSelected() from saving blank keyProfile
// -----------------------------------------------------------------------------
//  Commit  1.0.1 2006/10/27
//          Change OnIdle to OnTimer as per Denk advice
//          On new version, copy old key defs file if compatible
// -----------------------------------------------------------------------------
//  closed  2006/12/12 open    2006/12/12
//          Update/Merge the current array of keybindings via WalkMenuStructure
//          ala Rebind() but without clobbering other arrayss
//          A: Forget it. It'll wipe out the user secondary command settings.
//  closed  2006/12/12
//          When merging, verify menu id and delete from array if absent.
//  closed  when menuItems are changed without an attached() window, they arnt saved
//          A: no window was being attached at OnLoad() & UpdateArr(), so UpdateAllCmd()
//          was not being run. "flatnotebook" named had changed to "flat notebook"
//  closed  The OnKey() fix now doesnt allow F1-F9 etc
//  closed  2006/12/29
//          If a key is defined in the secondary profile, then user changes to primary
//          profile, key will be "saved" there also. Next load will have key in wrong
//          profile. EG. define secondary About=Alt-Shift-H. Then change to primary.
//          The definition will be saved in primary also.
//          All defs need to be cleared when a different profile is loaded. This might
//          cause havoc with dynamic keys
//          A: fixed, see next two items.
//  closed  The only way to clear a def thats in both the primary and secondary is:
//          Remove one def, do OK, reload, remove the secondary def. This is probably a
//          side effect of dynamic merging.
//          A: caused by merging before invoking UpdateArr() in OnLoad().
//  closed  Deleteing a secondary profile fails when no keys were changed.
//          A: caused by profile compare code in OnKeyConfigDialogDone() reporting
//             no changes even when a secondary profile was deleted.
//  Todo    2007/01/29
//          Save() .ini file to .ini.bak, then rename it to .ini
// ----------------------------------------------------------------------------
//  Commit  1.0.8 2006/12/14
//          2) Added code to remove stale dynamic menu items
//          3) Added MergeDynamicMenu() & OnSave() at plugin termination
//          4) In IsValidKeyComb() changed '+' to '-'
//          5) Disallowed single key cmdStrings in wxKeyMonitorTextCtrl::OnKey()
//          6) Reduced size of .ini fileName
//          7) Fixed broken OnKey() to accept function keys w/o Ctrl- Alt- or Shift-
//  Commit  1.0.9 2006/12/19
//          8) Removed dependency on event EVT_APP_STARTUP_DONE to avoid "uninitialized" crashes
//          9) Set all EOL to dos ala TimS instructions & {svn propset eol-style native" *.h and *.cpp
// ----------------------------------------------------------------------------
//  Commit  1.0.12 2006/12/29
//          10) redefine WXK_PRIOR and WXK_NEXT for wx2.8.0 allowing expected code behavior
//          11) remove case stmts for wx2.8.0
//          12) Fixed secondary profile keys leaking into primary profile
//              Fixed inability to delete secondary profiles
//  Commit  1.0.13 2006/12/30
//          13) Re-instated 2.6.2 version of RebuildMenuItem() with TimS additions
//              to solve missing menu icon problem (wx2.6.2 dejavu).
//              Completely rebuilds menuItems with changed shortcut and a bitmapped icon.
//              Works with wx2.6.3 w/fixes and wx2.8.0 according to TimS.
// ----------------------------------------------------------------------------
//  Commit  1.0.14 2007/01/13
//          14) Applied patch 1821 by DerMeister
//              This patch fixes a crash in the *nix keybinder plugin.
//              See http://forums.codeblocks.org/index.php?topic=4927.0 for more information.
//              https://developer.berlios.de/patch/index.php?func=detailpatch&patch_id=1821&group_id=5358
// ----------------------------------------------------------------------------
//  Commit  1.0.15 2007/01/18
//          15) Empty profile written when CB re-enables KeyBinder and no events occur.
// ----------------------------------------------------------------------------
//  Commit  1.0.16 2007/01/19
//          16) Fixed for missing *nix menu mnemonics
// ----------------------------------------------------------------------------
//  Commit  1.0.19 2007/01/23
//          17) Fixed: CB Plugins->ManagePlugins->disable plugin is completely
//              re-building the menuBar at another address causing Keybinder to crash.
//          18) Fixed: CB Plugins->ManagePlugins->disable plugin is re-entering
//              KeyBinders BuildMenu() routine causing crashes.
//          19) Re-initialize KeyBinder on next OnEditorOpen() when re-enabled by Plugin Manager.
// ----------------------------------------------------------------------------
//  Commit  1.0.21 2007/02/23
//          20) Allow override of predefined menu accelerators set in Main.cpp
//          21) Use cbKeyBinder##.ini file when in executable folder
// ----------------------------------------------------------------------------
//  Commit  1.0.22 2007/03/9
//          22) [ Patch #1913 ] Patch Key Binder plugin for wxW28 with disable_compat26
// ----------------------------------------------------------------------------
//  Commit  1.0.24 2007/03/31
//          23) Remove dynamic merge from OnRelease() to avoid recording CB core menu deletions
//          24) Reinstate Menu UpdateByLabel when UpdateByID fails
// ----------------------------------------------------------------------------
//  Commit  1.0.30 2007/05/2
//          25) Create backup keys .ini.bak only on successful user definition save
//          26) Set guards against merging when possible app shutdown
//          27) Add ConfigBusy guard to OnIdle() merging
//          28) Added EVT_MENUBAR_CREATE_BEGIN/END, but they're never called. Even if
//              they were, they're not called during Tools menu changes.
//          29) Corrected for warning msg in wx2.8.3
//          30) Removed key assignment via text search when id search fails.(wxKeyBinder::UpdateSubMenu).
//               Causes too much grief.
// ----------------------------------------------------------------------------
//  Commit  1.0.32 2007/05/10
//          31) Removed OnIdle() dynamic merging. Set Merge just before user definitions.
//          32) Fixed disappearing "duplicate" menu items. If mismatched id, just leave 'em alone.
// ----------------------------------------------------------------------------
//  Commit  1.0.33 2007/05/11
//          33) Fixes for use of non-precompiled headers
// ----------------------------------------------------------------------------
//  Commit  1.0.34 2007/05/31
//          34) Re-enabled dynamic merge eliminating consideration of duplicates
// ----------------------------------------------------------------------------
//  Commit  1.0.35 2007/06/7
//          35) Prepend --personality arg to cbKeybinder.ini filename
// ----------------------------------------------------------------------------
//  Commit  1.0.38 2007/06/15
//          36) Use full menu path as key value in external storage
//          37) Resolve unmatched menu id's with full menu path
//          38) Add Tool menu shortcut preservation via cbEVT_MENUBAR_CREATE_BEGIN/END events
// ----------------------------------------------------------------------------
//  Commit  1.0.39
//          39) mandrav changes for sdk RegisterEventSink
// ----------------------------------------------------------------------------
//  Commit  1.0.41 2007/07/30
//          40) check for numeric menu items that start with "_" wxGTK 2.8.4
//          41) Fixed disappearing/reappearing command keys on wxGTK 2.8.4 by
//              trimming all menu items and conf strings before comparing
// ----------------------------------------------------------------------------
//  Commit  1.0.42 2007/08/12
//          42) Change AddPendingEvent() to ProcessEvent() in wxMenuCmd::Exec()
//              to avoid crashes when secondary command key would close editors wx284
// ----------------------------------------------------------------------------
//  commit  1.0.44 2007/12/2
//          43) add non-pch logmanager headers
//          44) fix skipping of previously opened editors
// ----------------------------------------------------------------------------
//  commit  1.0.46 2008/02/12
//          45) Killerbot changes to header includes 2008/12/28
//          46) Fixed: [ Bug #12823 ] keybinder breaks layout submenu on linux
// ----------------------------------------------------------------------------
//  Commit  1.0.48 2009/06/8
//          47) If tree item is a sub-menu don't allow key assignment
//          48) Change KeyBinder menu access from GetLabel() to GetLabel().Trim() (wx2.8.9/10)
// ----------------------------------------------------------------------------
//  Commit  1.0.49 2010/03/3
//          49) Apply patch 2885 by techy
// ----------------------------------------------------------------------------
//  Commit  1.0.50 2012/09/1
//          50) Merge global accelerators into keybinder, leave the rest alone
//              Remove bindings for Ctrl-C/P/X, allowing native behavior
// ----------------------------------------------------------------------------
