/***************************************************************
 * Name:      cbkeybinder.h
 * Purpose:   Code::Blocks plugin
 * Author:    Pecan<>
 * Copyright: (c) Pecan
 * License:   GPL
 **************************************************************/
//commit 12/14/2005 9:16 AM
//commit 12/16/2005 8:54 PM

#ifndef CBKEYBINDER_H
#define CBKEYBINDER_H

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma interface "cbkeybinder.h"
#endif
// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <cbplugin.h> // the base class we 're inheriting
#include <settings.h> // needed to use the Code::Blocks SDK
// ----------------------------------------------------------------------------
//  additional includeds for cbKeyBinder
// ----------------------------------------------------------------------------
#include <manager.h>
#include <messagemanager.h>
#include <configmanager.h>
#include <cbeditor.h>
#include "keybinder.h"
#include "menuutils.h"
#include "wx/config.h"
#include "wx/fileconf.h"
#include "wx/app.h"
#include "wx/utils.h"


// ----------------------------------------------------------------------------
#include "debugging.h"
#define RC3 1
// ----------------------------------------------------------------------------
//  cbKeyBinder class declaration
// ----------------------------------------------------------------------------
class cbKeyBinder : public cbPlugin
{
	public:
		cbKeyBinder();
		~cbKeyBinder();
		int Configure();
		void BuildMenu(wxMenuBar* menuBar);
		void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg);
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

        // Users Key file name eg. %HOME%\cbKeybinder.ini
        wxString m_sKeyFilename;

        // Switch to reload keybinding
        bool m_bBound;

        // utility function to update the menu key labels
        // and re-attach the window event handlers after key changes
        void UpdateArr(wxKeyProfileArray &r);
        void cbKeyBinder::Rebind();
        // key definition configuration dialog
        void cbKeyBinder::OnKeybindings();

        // save/load key definitions
        void cbKeyBinder::OnSave();
        void cbKeyBinder::OnLoad();

    protected:
        wxADD_KEYBINDER_SUPPORT();

    private:
        void cbKeyBinder::OnProjectOpened(CodeBlocksEvent& event);
        void cbKeyBinder::OnProjectActivated(CodeBlocksEvent& event);
        void cbKeyBinder::OnProjectClosed(CodeBlocksEvent& event);
        void cbKeyBinder::OnProjectFileAdded(CodeBlocksEvent& event);
        void cbKeyBinder::OnProjectFileRemoved(CodeBlocksEvent& event);
        void cbKeyBinder::OnEditorOpen(CodeBlocksEvent& event);
        void cbKeyBinder::OnEditorClose(CodeBlocksEvent& event);
        //void cbKeyBinder::OnSetFocus(wxFocusEvent& event);
        void cbKeyBinder::OnAppStartupDone(CodeBlocksEvent& event);

        wxWindow* pcbWindow;            //main app window
        wxArrayPtrVoid m_EditorPtrs;    //attached editor windows
        bool bKeyFileErrMsgShown;

    private:
		DECLARE_EVENT_TABLE()
};//class cbKeyBinder
// ----------------------------------------------------------------------------
//  MyDialog class declaration
// ----------------------------------------------------------------------------
class MyDialog : public wxDialog
{
public:
	wxKeyConfigPanel *m_p;

public:
    // ctor(s)
    MyDialog(wxKeyProfileArray &arr, wxWindow *parent, const wxString& title, int);
	~MyDialog();

	void OnApply( wxCommandEvent &ev );

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// Declare the plugin's hooks
CB_DECLARE_PLUGIN();

#endif // CBKEYBINDER_H
