/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/
// RCS-ID: $Id$


#ifndef GLOBAL_H
#define GLOBAL_H

#include <wx/frame.h>
#include <wx/fileconf.h>

#include "snippetsimages.h"

    class CodeSnippetsConfig;
    class CodeSnippetsTreeCtrl;
    class CodeSnippetsWindow;
    class EditorManager;
    class ThreadSearch;
    class cbDragScroll;

    //-extern CodeSnippetsConfig* g_pConfig;
    void SetConfig(CodeSnippetsConfig*);
    CodeSnippetsConfig* GetConfig();

    extern int g_activeMenuId;
    int     GetActiveMenuId();
    void    SetActiveMenuId(int menuid);

    wxString csC2U(const char* str);
    const wxWX2MBbuf csU2C(const wxString& str);

// hashmap for fast searches: SnippetItemId, Filename
WX_DECLARE_STRING_HASH_MAP( long, FileLinksMapArray );
// hash for frame pointer containing an editor manager
WX_DECLARE_HASH_MAP( wxFrame*, EditorManager*, wxPointerHash, wxPointerEqual, EdManagerMapArray );
// ----------------------------------------------------------------------------
class CodeSnippetsConfig
// ----------------------------------------------------------------------------
{
    public:
        CodeSnippetsConfig();
        ~CodeSnippetsConfig();

    void     SettingsLoad();
    void     SettingsSave();
    void     SettingsSaveWinPosition();
    void     SettingsSaveString(const wxString settingName, const wxString settingValue );
    wxString SettingsReadString(const wxString settingName );
    wxString GetSettingsWindowState();
    void     SetSettingsWindowState(const wxString windowState);

    wxWindow*       GetMainFrame(){return pMainFrame;}
    wxMenuBar*      GetMenuBar(){return m_pMenuBar;}
    SnipImages*     GetSnipImages(){return pSnipImages;}

    CodeSnippetsWindow*     GetSnippetsWindow(){return pSnippetsWindow;}
    wxTextCtrl*             GetSnippetsSearchCtrl(){return pSnippetsSearchCtrl;}
    CodeSnippetsTreeCtrl*   GetSnippetsTreeCtrl(){return pSnippetsTreeCtrl;}
    void                    SetSnippetsTreeCtrl(CodeSnippetsTreeCtrl* p){ pSnippetsTreeCtrl=p;return;}
    wxString                GetVersion(){return m_VersionStr;}
    wxString                GetTempDir();

    FileLinksMapArray&      GetFileLinksMapArray(){ return m_fileLinksMapArray;}
    void                    ClearFileLinksMapArray(){ m_fileLinksMapArray.clear();}

//-    wxWindow*               GetThreadSearchFrame(){ return m_pThreadSearchFrame;}
//-    void                    SetThreadSearchFrame(wxWindow* p){m_pThreadSearchFrame = p;}

    ThreadSearch*           GetThreadSearchPlugin(){ return m_pThreadSearchPlugin;}
    void                    SetThreadSearchPlugin(ThreadSearch* p){m_pThreadSearchPlugin = p;}

    cbDragScroll*           GetDragScrollPlugin(){ return (cbDragScroll*)GetDragScrollEvtHandler();}
    wxEvtHandler*           GetDragScrollEvtHandler();
    void                    SetDragScrollPlugin(cbDragScroll* p){m_pDragScrollPlugin = p;}

    EditorManager*         GetEditorManager(wxFrame* pFrame);
    EditorManager*         GetEditorManager(wxWindow* frame);
    int                     GetEditorManagerCount();
    wxFrame*                GetEditorManagerFrame(const int index);
    EditorManager*         GetEditorManager(const int index);

    void                    RegisterEditorManager(wxFrame* pFrame, EditorManager* pEdMgr);
    void                    RemoveEditorManager(wxFrame* pFrame);

    wxWindow*               GetOpenFilesList(){return m_pOpenFilesList;}
    void                    SetOpenFilesList(wxWindow* p){m_pOpenFilesList = p;}

    long                    GetKeepAlivePid(){return g_lKeepAlivePid;}
    void                    SetKeepAlivePid(long pid){g_lKeepAlivePid = pid;}

    wxString                GetAppParent() {return m_AppParent;}
    void                    SetAppParent(const wxString& appParent) {m_AppParent = appParent;}

    bool                    GetEditorsStayOnTop(){return SettingsEditorsStayOnTop;}
    void                    SetEditorsStayOnTop(bool tf){SettingsEditorsStayOnTop = tf;}

    bool                    GetToolTipsOption(){return SettingsToolTipsOption;}
    void                    SetToolTipsOption(bool tf){SettingsToolTipsOption = tf;}

    bool                    IsExternalPersistentOpen(){ return m_IsExternalPersistentOpen;}
    void                    SetExternalPersistentOpen(const bool trueFalse ){ m_IsExternalPersistentOpen = trueFalse;}

    void CenterChildOnParent( wxWindow* child, wxWindow* parent=0);

    bool IsPlugin(){return m_bIsPlugin;}
    bool IsApplication(){return not m_bIsPlugin;}

    bool  IsDockedWindow(wxWindow** pw = 0, wxPoint* pcoord = 0, wxSize* psize = 0);
    bool  IsFloatingWindow(wxWindow** pw = 0, wxPoint* pcoord = 0, wxSize* psize = 0);
    bool  IsExternalWindow();

    // Snippet Tree Search options
    enum SearchScope
    {
        SCOPE_SNIPPETS,		// Searches only snippets
        SCOPE_CATEGORIES,	// Searches only categories
        SCOPE_BOTH			// Searches both snippets and categories
    };
    struct SearchConfiguration
    {
        bool caseSensitive;
        SearchScope scope;

        SearchConfiguration()
        {
            // Default settings
            caseSensitive = true;
            scope = SCOPE_BOTH;
        }
    };

    bool         m_appIsShutdown;
    bool         m_appIsDisabled;
    wxString     AppName;
    wxString     m_AppParent;
    wxWindow*    pMainFrame;
    wxMenuBar*   m_pMenuBar;

    CodeSnippetsWindow*     pSnippetsWindow;
    CodeSnippetsTreeCtrl*   pSnippetsTreeCtrl;
    wxTextCtrl*             pSnippetsSearchCtrl;

	wxString     SettingsExternalEditor;
	wxString     SettingsSnippetsXmlPath;
	wxString     SettingsSnippetsCfgPath;
	wxString     SettingsSnippetsFolder;
	wxString     SettingsCBConfigPath;

    // the key file name as {%HOME%}\codesnippets.ini
    wxString m_ConfigFolder;
    // the programs executable folder
    wxString m_ExecuteFolder;

	bool         SettingsSearchBox;
	bool         SettingsEditorsStayOnTop;
	bool         SettingsToolTipsOption;
    SnipImages*  pSnipImages;
    int          nEditDlgXpos;
    int          nEditDlgYpos;
    int          nEditDlgWidth;
    int          nEditDlgHeight;
    bool         bEditDlgMaximized;
    int          windowXpos;
    int          windowYpos;
    int          windowWidth;
    int          windowHeight;
    wxString     m_VersionStr;
    //-wxString     m_sWindowHandle;
    SearchConfiguration
                  m_SearchConfig;
    wxString      m_SettingsWindowState;
    bool          m_bWindowStateChanged;
    bool          m_bIsPlugin;
    wxWindow*     m_pOpenFilesList;
    // If launched from CB, we'll have its pid
    long          g_lKeepAlivePid;
    bool          m_IsExternalPersistentOpen;

//-    wxWindow*           m_pThreadSearchFrame;
    ThreadSearch*       m_pThreadSearchPlugin;

    // If DragScroll is loaded, contains address of DragScroll plugin
    // else contains address of CodeSnippets plugin to allow ProcessEvent()
    // and AddPendingEvent() to punt without crashing.
    cbDragScroll*       m_pDragScrollPlugin;

    // filename and SnippetDataitem ID
    FileLinksMapArray m_fileLinksMapArray;
    // map frames to an editormanager
    EdManagerMapArray m_EdManagerMapArray;

};

#endif // GLOBAL_H
