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
    private:
		Manager(wxMDIParentFrame* appWindow, wxNotebook* notebook);
		~Manager();

		void OnMenu(wxCommandEvent& event);
        wxMDIParentFrame* m_pAppWindow;
        wxNotebook* m_pNotebook;
};

#endif // MANAGER_H

