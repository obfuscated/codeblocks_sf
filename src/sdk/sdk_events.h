#ifndef SDK_EVENTS_H
#define SDK_EVENTS_H

#include <wx/event.h>
#include "settings.h"
#include "cbproject.h"
#include "cbeditor.h"
#include "cbplugin.h"

class EVTIMPORT CodeBlocksEvent : public wxCommandEvent
{
	public:
		CodeBlocksEvent(wxEventType commandType = wxEVT_NULL, int id = 0, cbProject* project = 0L, cbEditor* editor = 0L, cbPlugin* plugin = 0L)
			: wxCommandEvent(commandType, id),
			m_pProject(project),
			m_pEditor(editor),
			m_pPlugin(plugin) {}
		CodeBlocksEvent(const CodeBlocksEvent& event)
			: wxCommandEvent(event),
			m_pProject(event.m_pProject),
			m_pEditor(event.m_pEditor),
			m_pPlugin(event.m_pPlugin) {}
		virtual wxEvent *Clone() const { return new CodeBlocksEvent(*this); }

		cbProject* GetProject() const { return m_pProject; }
		void SetProject(cbProject* project){ m_pProject = project; }
		cbEditor* GetEditor() const { return m_pEditor; }
		void SetEditor(cbEditor* editor){ m_pEditor = editor; }
		cbPlugin* GetPlugin() const { return m_pPlugin; }
		void SetPlugin(cbPlugin* plugin){ m_pPlugin = plugin; }
		int GetX(){ return m_X; }
		void SetX(int x){ m_X = x; }
		int GetY(){ return m_Y; }
		void SetY(int y){ m_Y = y; }
	protected:
		cbProject* m_pProject;
		cbEditor* m_pEditor;
		cbPlugin* m_pPlugin;
		// for some editor events
		int m_X;
		int m_Y;
	private:
		DECLARE_DYNAMIC_CLASS(CodeBlocksEvent)
};

typedef void (wxEvtHandler::*CodeBlocksEventFunction)(CodeBlocksEvent&);

// plugin events
DECLARE_CB_EVENT_TYPE(cbEVT_PLUGIN_ATTACHED)
#define EVT_PLUGIN_ATTACHED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PLUGIN_ATTACHED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_PLUGIN_RELEASED)
#define EVT_PLUGIN_RELEASED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PLUGIN_RELEASED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

// editor events
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_CLOSE)
#define EVT_EDITOR_CLOSE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_CLOSE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_OPEN)
#define EVT_EDITOR_OPEN(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_OPEN, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_SAVE)
#define EVT_EDITOR_SAVE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_SAVE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_AUTOCOMPLETE)
#define EVT_EDITOR_AUTOCOMPLETE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_AUTOCOMPLETE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_CALLTIP)
#define EVT_EDITOR_CALLTIP(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_CALLTIP, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_TOOLTIP)
#define EVT_EDITOR_TOOLTIP(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_TOOLTIP, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_TOOLTIP_CANCEL)
#define EVT_EDITOR_TOOLTIP_CANCEL(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_TOOLTIP_CANCEL, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_USERLIST_SELECTION)
#define EVT_EDITOR_USERLIST_SELECTION(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_USERLIST_SELECTION, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_BREAKPOINT_ADDED)
#define EVT_EDITOR_BREAKPOINT_ADDED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_BREAKPOINT_ADDED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_BREAKPOINT_DELETED)
#define EVT_EDITOR_BREAKPOINT_DELETED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_BREAKPOINT_DELETED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_UPDATE_UI)
#define EVT_EDITOR_UPDATE_UI(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_UPDATE_UI, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

// project events
DECLARE_CB_EVENT_TYPE(cbEVT_PROJECT_CLOSE)
#define EVT_PROJECT_CLOSE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_CLOSE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_PROJECT_OPEN)
#define EVT_PROJECT_OPEN(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_OPEN, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_PROJECT_SAVE)
#define EVT_PROJECT_SAVE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_SAVE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_PROJECT_ACTIVATE)
#define EVT_PROJECT_ACTIVATE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_ACTIVATE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_PROJECT_FILE_ADDED)
#define EVT_PROJECT_FILE_ADDED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_FILE_ADDED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_PROJECT_FILE_REMOVED)
#define EVT_PROJECT_FILE_REMOVED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_FILE_REMOVED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_PROJECT_POPUP_MENU)
#define EVT_PROJECT_POPUP_MENU(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_POPUP_MENU, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

// pipedprocess events
DECLARE_CB_EVENT_TYPE(cbEVT_PIPEDPROCESS_STDOUT)
#define EVT_PIPEDPROCESS_STDOUT(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_STDOUT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_PIPEDPROCESS_STDERR)
#define EVT_PIPEDPROCESS_STDERR(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_STDERR, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_PIPEDPROCESS_TERMINATED)
#define EVT_PIPEDPROCESS_TERMINATED(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_TERMINATED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),

#endif // SDK_EVENTS_H
