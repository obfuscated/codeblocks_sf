#ifndef MANAGER_H
#define MANAGER_H

#include <wx/event.h>
#include "settings.h"

// forward decls
class wxMDIParentFrame;
class wxNotebook;
class ProjectManager;
class EditorManager;
class MessageManager;
class PluginManager;
class ToolsManager;
class MacrosManager;
class wxMenu;
class wxMenuBar;
class wxToolBar;

/*
 * No description
 */
class DLLIMPORT Manager
{
	public:
        static Manager* Get(wxMDIParentFrame* appWindow = 0L, wxNotebook* notebook = 0L);
		static void Free();
		wxMDIParentFrame* GetAppWindow();
		wxNotebook* GetNotebook();
		ProjectManager* GetProjectManager();
		EditorManager* GetEditorManager();
		MessageManager* GetMessageManager();
		PluginManager* GetPluginManager();
		ToolsManager* GetToolsManager();
		MacrosManager* GetMacrosManager();
		static bool isappShutingDown();
		// stupid typo ;-P		
		static bool isappShuttingDown();

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
		Manager(wxMDIParentFrame* appWindow, wxNotebook* notebook);
		~Manager();

		void OnMenu(wxCommandEvent& event);
        wxMDIParentFrame* m_pAppWindow;
        wxNotebook* m_pNotebook;
};

#endif // MANAGER_H

