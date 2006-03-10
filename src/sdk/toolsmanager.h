#ifndef TOOLSMANAGER_H
#define TOOLSMANAGER_H

#include <wx/event.h>
#include <wx/timer.h>
#include "settings.h"
#include "manager.h"
#include "menuitemsmanager.h"
#include "sdk_events.h"
#include "sanitycheck.h"

class Tool
{
    public:
	Tool(){ menuId = -1; }
	wxString name;
	wxString command;
	wxString params;
	wxString workingDir;
	bool createConsole;
	int menuId;
};

WX_DECLARE_LIST(Tool, ToolsList);

// forward decls
class wxMenuBar;
class wxMenu;
class wxMenuItem;
class PipedProcess;

class DLLIMPORT ToolsManager : public Mgr<ToolsManager>, public wxEvtHandler
{
	public:
        friend class Mgr<ToolsManager>;
        friend class Manager; // give Manager access to our private members
		void CreateMenu(wxMenuBar* menuBar);
		void ReleaseMenu(wxMenuBar* menuBar);
		void AddTool(const wxString& name, const wxString& command, const wxString& params, const wxString& workingDir, bool save = true);
		void AddTool(Tool* tool, bool save = true);
		void InsertTool(int position, Tool* tool, bool save = true);
		void RemoveToolByIndex(int index);
		void RemoveToolByName(const wxString& name);
		Tool* GetToolById(int id);
		Tool* GetToolByIndex(int index);
		int GetToolsCount(){ return m_Tools.GetCount(); }
		void BuildToolsMenu(wxMenu* menu);
		int Configure();
		bool Execute(Tool* tool);
		void LoadTools();
		void SaveTools();
		void OnToolClick(wxCommandEvent& event);
		void OnProcessTimer(wxTimerEvent& event);
		void OnIdle(wxIdleEvent& event);
		void OnToolStdOutput(CodeBlocksEvent& event);
		void OnToolErrOutput(CodeBlocksEvent& event);
		void OnToolTerminated(CodeBlocksEvent& event);
		void OnConfigure(wxCommandEvent& event);
	protected:
	private:
		void DoRemoveTool(ToolsList::Node* node);
		ToolsManager();
		~ToolsManager();

		ToolsList m_Tools;
		MenuItemsManager m_ItemsManager;
		wxMenu* m_Menu;
		PipedProcess* m_pProcess;
		int m_Pid;
		wxTimer m_Timer;

		DECLARE_EVENT_TABLE()
		DECLARE_SANITY_CHECK

};

#endif // TOOLSMANAGER_H
