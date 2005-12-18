#ifndef MANAGER_H
#define MANAGER_H

#include <wx/event.h>
#include "settings.h"
#include "sdk_events.h"

// forward decls
class wxFrame;
class wxNotebook;
class ProjectManager;
class EditorManager;
class MessageManager;
class PluginManager;
class ToolsManager;
class MacrosManager;
class PersonalityManager;
class wxMenu;
class wxMenuBar;
class wxToolBar;
class UserVariableManager;
class ScriptingManager;
class ConfigManager;

/*
 * No description
 */
class DLLIMPORT Manager
{
	public:
        static Manager* Get(wxFrame* appWindow = 0L, wxNotebook* prjNB = 0L, wxWindow* clientWin = 0L);
		static void Free();
		wxFrame* GetAppWindow();
		wxNotebook* GetNotebook();
		wxWindow* GetClientWindow();
		bool ProcessEvent(CodeBlocksEvent& event);
		ProjectManager* GetProjectManager();
		EditorManager* GetEditorManager();
		MessageManager* GetMessageManager();
		PluginManager* GetPluginManager();
		ToolsManager* GetToolsManager();
		MacrosManager* GetMacrosManager();
		PersonalityManager* GetPersonalityManager();
		UserVariableManager* GetUserVariableManager();
		ScriptingManager* GetScriptingManager();
		ConfigManager* Manager::GetConfigManager(const wxString& name_space);

		static bool isappShutingDown();
		// stupid typo ;-P
		static bool isappShuttingDown();

        // Gets a notebook panel with the specified name
        wxWindow* GetNotebookPage(const wxString &name, long style =  wxTAB_TRAVERSAL | wxCLIP_CHILDREN,bool issplit=false);

        /////// XML Resource functions ///////

		// Inits XML Resource system
		static void Initxrc(bool force=false);
		// Loads XRC file(s) using data_path
		static void Loadxrc(wxString relpath);
		// Loads Menubar from XRC
		static wxMenuBar* LoadMenuBar(wxString resid,bool createonfailure=false);
		// Loads Menu from XRC
		static wxMenu* LoadMenu(wxString menu_id,bool createonfailure=false);
		// Loads ToolBar from XRC
		static wxToolBar *LoadToolBar(wxFrame *parent,wxString resid,bool defaultsmall=true);
		// Loads ToolBarAddOn from XRC into existing Toolbar

        static void AddonToolBar(wxToolBar* toolBar,wxString resid);
        static bool isToolBar16x16(wxToolBar* toolBar);

    private:
		Manager(wxFrame* appWindow, wxNotebook* notebook, wxWindow* clientWin = 0L);
		~Manager();

        bool SendEventTo(wxEvtHandler* handler, CodeBlocksEvent& event);
		void OnMenu(wxCommandEvent& event);
        wxFrame* m_pAppWindow;
        wxNotebook* m_pNotebook;
        wxWindow* m_pClientWin;
};

#endif // MANAGER_H

