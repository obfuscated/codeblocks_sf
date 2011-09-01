/***************************************************************
 * Name:      MouseSap.h
 * Purpose:   Code::Blocks plugin
 * Author:    Pecan<>
 * Copyright: (c) Pecan
 * License:   GPL
 **************************************************************/
// This plugin emulates Linux GPM functions within the editors in linux and msWindows.
// If selected text, paste selected text at current cursor position
// If selected text, and user middle-clicks inside selection, copy to clipboard
// If no selected text, paste clipboard data at cursor position
// If selected text, and user switches editors, copy selection to clipboard

#ifndef MouseSap_H
#define MouseSap_H

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma interface "MouseSap.h"
#endif

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include <wx/arrstr.h>
#include <wx/dynarray.h>
#include <wx/event.h> // wxEvtHandler
#include <wx/gdicmn.h> //wxPoint
#include <wx/log.h>
#include <cbplugin.h> // the base class we 're inheriting

//#include "GtkClipbrd.h"
// ---------------------------------------------------------------------------
//  Logging / debugging
// ---------------------------------------------------------------------------

//----------------------------------------
#define VERSION "1.1.7 2009/11/8"
//----------------------------------------

#undef LOGGING
#define LOGIT wxLogDebug
#if defined(dsLOGGING)
 #define LOGGING 1
 #undef LOGIT
 #define LOGIT wxLogMessage
 #define TRAP asm("int3")
#endif

// anchor to one and only MouseSap object
class MMSapEvents;
class MouseSapCfg;
class wxLogWindow;
class wxObject;
class wxScintillaEvent;
class cbStyledTextCtrl;

// ----------------------------------------------------------------------------
//  MouseSap class declaration
// ----------------------------------------------------------------------------
class MouseSap : public cbPlugin
{
	public:
		MouseSap();
		~MouseSap();
        int  GetConfigurationGroup() const { return -1; }
		void BuildMenu(wxMenuBar* /*menuBar*/){ return; }
        void BuildModuleMenu(const ModuleType /*type*/, wxMenu* /*menu*/, const FileTreeData* /*data*/){ return; }
        bool BuildToolBar(wxToolBar* /*toolBar*/){ return false; }
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
     virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);

    static MouseSap* pMouseSap;
	protected:
        cbConfigurationPanel* CreatecbCfgPanel(wxWindow* parent);

    public:
        //void OnDialogDone(MouseSapCfg* pdlg);

        bool GetMouseSapEnabled() const { return m_bMouseSapEnabled; }
        bool IsAttachedTo(wxWindow* p);

        wxWindow* m_pMS_Window;

	private:
        void OnAppStartupDone(CodeBlocksEvent& event);
        void OnAppStartupDoneInit();
        void OnDoConfigRequests(wxUpdateUIEvent& event);

        void AttachRecursively(wxWindow *p);
        void Detach(wxWindow* thisEditor);
        void DetachAll();
        void Attach(wxWindow *p);
        void DisconnectEvtHandler(MMSapEvents* thisEvtHandler);

        wxWindow* winExists(wxWindow *parent);
        wxWindow* FindWindowRecursively(const wxWindow* parent, const wxWindow* handle);
        wxString  FindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName);
        void      OnWindowOpen(wxEvent& event);
        void      OnWindowClose(wxEvent& event);
        void      OnEditorEventHook(cbEditor* pcbEditor, wxScintillaEvent& event);


        wxString        m_ConfigFolder;
        wxString        m_ExecuteFolder;
        wxString        m_DataFolder;
        wxString        m_CfgFilenameStr;

        wxArrayString   m_UsableWindows;
        wxArrayPtrVoid  m_EditorPtrs;
        wxLogWindow*    m_pMyLog;
        bool            m_bEditorsAttached;

        bool            m_bMouseSapEnabled ;  //Enable/Disable plugin
        MMSapEvents*    m_pMMSapEvents;

    private:
		DECLARE_EVENT_TABLE()

};//MouseSap
////// ----------------------------------------------------------------------------
////class MMClipboard : public wxClipboard
////// ----------------------------------------------------------------------------
////{
////    public:
////        MMClipboard(){}
////        virtual ~MMClipboard(){};
////        // wxWidgets clears wxTheClipboard and the system's clipboard if possible
////        // we need to add items to the PRIMARY without clearing the regular
////        // gtk clipboard.
////        virtual void Clear()
////        {
////                #if defined(LOGGING)
////                LOGIT(wxT("MMClipboard::Clear() averted."));
////                #endif
////        };
////};

// ----------------------------------------------------------------------------
class MMSapEvents : public wxEvtHandler
// ----------------------------------------------------------------------------
{

    public:
        MMSapEvents(wxWindow * /*window*/){ }
        ~MMSapEvents();

        void OnMouseEvent(wxMouseEvent& event);
        void OnMiddleMouseDown(wxMouseEvent& event, cbStyledTextCtrl* ed);
        void PasteFromClipboard( wxMouseEvent& event, cbStyledTextCtrl* ed, bool shiftKeyState );
        void OnKillFocusEvent( wxFocusEvent& event );
        void DumpClipboard();

    private:
        //-MMClipboard OurClipBoard;


    DECLARE_EVENT_TABLE()
};
// ----------------------------------------------------------------------------
//  Modification/ToDo History
// ----------------------------------------------------------------------------
//  Commit  0.1.5 2008/02/18
//          00) First version adapted from DragScroll
//          01) Paste from clipboard if no current text selection 2008/02/14
//          02) OnKillFocusEvent copy selected text to clipboard 2008/02/15
//          03) If user clicks middleMouse in selected text, copy to clipboard 2008/02/15
//          04) Removed unnecessary event handler mangling
//          05) wxTheClipboard->UsePrimarySelection(false); in PastFromClipboard()
// ----------------------------------------------------------------------------
//  Commit  1.1.7 2009/11/8
//          06) Shift-MiddleMouse will paste over marked area if cursor also in marked
//              else insert pasted data.
//          07) Change Paste() to SetTarget() for Linux
//  ToDo
// ----------------------------------------------------------------------------
#endif // MouseSap_H
