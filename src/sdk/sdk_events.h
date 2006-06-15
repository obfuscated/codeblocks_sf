#ifndef SDK_EVENTS_H
#define SDK_EVENTS_H

#include <wx/event.h>
#include "settings.h"
#include "cbproject.h"
#include "editorbase.h"
#include "cbplugin.h"

/** A generic Code::Blocks event. */
class EVTIMPORT CodeBlocksEvent : public wxCommandEvent, public BlockAllocated<CodeBlocksEvent, 75>
{
	public:
		CodeBlocksEvent(wxEventType commandType = wxEVT_NULL, int id = 0, cbProject* project = 0L, EditorBase* editor = 0L, cbPlugin* plugin = 0L)
			: wxCommandEvent(commandType, id),
			m_pProject(project),
			m_pEditor(editor),
			m_pPlugin(plugin),
			m_X(0),
			m_Y(0) {}
		CodeBlocksEvent(const CodeBlocksEvent& event)
			: wxCommandEvent(event),
			m_pProject(event.m_pProject),
			m_pEditor(event.m_pEditor),
			m_pPlugin(event.m_pPlugin),
			m_X(event.m_X),
			m_Y(event.m_Y) {}
		virtual wxEvent *Clone() const { return new CodeBlocksEvent(*this); }

		cbProject* GetProject() const { return m_pProject; }
		void SetProject(cbProject* project){ m_pProject = project; }
		EditorBase* GetEditor() const { return m_pEditor; }
		void SetEditor(EditorBase* editor){ m_pEditor = editor; }
		cbPlugin* GetPlugin() const { return m_pPlugin; }
		void SetPlugin(cbPlugin* plugin){ m_pPlugin = plugin; }
		int GetX(){ return m_X; }
		void SetX(int x){ m_X = x; }
		int GetY(){ return m_Y; }
		void SetY(int y){ m_Y = y; }
	protected:
		cbProject* m_pProject;
		EditorBase* m_pEditor;
		cbPlugin* m_pPlugin;
		// for some editor events
		int m_X;
		int m_Y;
	private:
		DECLARE_DYNAMIC_CLASS(CodeBlocksEvent)
};
typedef void (wxEvtHandler::*CodeBlocksEventFunction)(CodeBlocksEvent&);

/** Event used to request from the main app to add a window to the docking system. */
class EVTIMPORT CodeBlocksDockEvent : public wxEvent, public BlockAllocated<CodeBlocksDockEvent, 75>
{
    public:
        enum DockSide
        {
            dsLeft = 0,
            dsRight,
            dsTop,
            dsBottom,
            dsFloating,
            dsUndefined,
        };

        CodeBlocksDockEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
            : wxEvent(id, commandType),
            title(_("Untitled")),
            pWindow(0),
            desiredSize(100, 100),
            floatingSize(150, 150),
            minimumSize(40, 40),
            dockSide(dsUndefined),
            row(-1),
            column(-1),
            shown(false),
            stretch(false),
            hideable(true),
            asTab(false)

        {
        }
        CodeBlocksDockEvent(const CodeBlocksDockEvent& rhs)
            : name(rhs.name),
            title(rhs.title),
            pWindow(rhs.pWindow),
            desiredSize(rhs.desiredSize),
            floatingSize(rhs.minimumSize),
            minimumSize(rhs.minimumSize),
            dockSide(rhs.dockSide),
            row(rhs.row),
            column(rhs.column),
            shown(rhs.shown),
            stretch(rhs.stretch),
            hideable(rhs.hideable),
            asTab(rhs.asTab),
            bitmap(rhs.bitmap)
        {
        }
		virtual wxEvent *Clone() const { return new CodeBlocksDockEvent(*this); }

        wxString name;      ///< Dock's name. Must be unique. If empty, a unique name will be assigned.
        wxString title;     ///< Dock's title.
        wxWindow* pWindow;  ///< The window to dock.
        wxSize desiredSize; ///< The desired size.
        wxSize floatingSize;///< The desired floating size.
        wxSize minimumSize; ///< The minimum allowed size.
        DockSide dockSide;  ///< The side to dock it.
        int row;            ///< The row to dock it.
        int column;         ///< The column to dock it.
        bool shown;         ///< If true, initially shown.
        bool stretch;       ///< If true, the dock will stretch to fill the @c dockSide
        bool hideable;      ///< If true, the dock will be allowed to be closed by the user.
        bool asTab;         ///< Add this window as a tab of an existing docked window (NOT IMPLEMENTED).
        wxString bitmap;    ///< The bitmap to use.

        char unused[64];    ///< Unused space in this class for later enhancements.
	private:
		DECLARE_DYNAMIC_CLASS(cbAddDockWindowEvent)
};
typedef void (wxEvtHandler::*CodeBlocksDockEventFunction)(CodeBlocksDockEvent&);

/** Event used to request from the main app to manage the view layouts. */
class EVTIMPORT CodeBlocksLayoutEvent : public wxEvent, public BlockAllocated<CodeBlocksLayoutEvent, 75>
{
    public:
        CodeBlocksLayoutEvent(wxEventType commandType = wxEVT_NULL, const wxString& layout_name = wxEmptyString)
            : wxEvent(wxID_ANY, commandType),
            layout(layout_name)
        {}
        CodeBlocksLayoutEvent(const CodeBlocksLayoutEvent& rhs)
            : layout(rhs.layout)
        {}
		virtual wxEvent *Clone() const { return new CodeBlocksLayoutEvent(*this); }

        wxString layout;      ///< Layout's name.
};
typedef void (wxEvtHandler::*CodeBlocksLayoutEventFunction)(CodeBlocksLayoutEvent&);

#define DECLARE_CB_EVENT_TYPE(type) \
	extern EVTIMPORT const wxEventType type;

// app events
DECLARE_CB_EVENT_TYPE(cbEVT_APP_STARTUP_DONE)
#define EVT_APP_STARTUP_DONE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_APP_STARTUP_DONE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_APP_START_SHUTDOWN)
#define EVT_APP_START_SHUTDOWN(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_APP_START_SHUTDOWN, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_APP_UPDATE_TITLE)
#define EVT_APP_UPDATE_TITLE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_APP_UPDATE_TITLE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

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
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_ACTIVATED)
#define EVT_EDITOR_ACTIVATED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_ACTIVATED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_DEACTIVATED)
#define EVT_EDITOR_DEACTIVATED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_DEACTIVATED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_SAVE)
#define EVT_EDITOR_SAVE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_SAVE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_MODIFIED)
#define EVT_EDITOR_MODIFIED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_MODIFIED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_TOOLTIP)
#define EVT_EDITOR_TOOLTIP(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_TOOLTIP, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_TOOLTIP_CANCEL)
#define EVT_EDITOR_USERLIST_SELECTION(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_USERLIST_SELECTION, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_BREAKPOINT_ADD)
#define EVT_EDITOR_BREAKPOINT_ADD(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_BREAKPOINT_ADD, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_BREAKPOINT_EDIT)
#define EVT_EDITOR_BREAKPOINT_EDIT(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_BREAKPOINT_EDIT, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_EDITOR_BREAKPOINT_DELETE)
#define EVT_EDITOR_BREAKPOINT_DELETE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_BREAKPOINT_DELETE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
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
DECLARE_CB_EVENT_TYPE(cbEVT_PROJECT_TARGETS_MODIFIED)
#define EVT_PROJECT_TARGETS_MODIFIED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_TARGETS_MODIFIED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

// pipedprocess events
DECLARE_CB_EVENT_TYPE(cbEVT_PIPEDPROCESS_STDOUT)
#define EVT_PIPEDPROCESS_STDOUT(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_STDOUT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_PIPEDPROCESS_STDOUT_RANGE(id, id1, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_STDOUT, id, id1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_PIPEDPROCESS_STDERR)
#define EVT_PIPEDPROCESS_STDERR(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_STDERR, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_PIPEDPROCESS_STDERR_RANGE(id, id1, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_STDERR, id, id1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_PIPEDPROCESS_TERMINATED)
//#define EVT_PIPEDPROCESS_TERMINATED(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_TERMINATED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_PIPEDPROCESS_TERMINATED(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_TERMINATED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
#define EVT_PIPEDPROCESS_TERMINATED_RANGE(id, id1, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_TERMINATED, id, id1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),

// thread-pool events
DECLARE_CB_EVENT_TYPE(cbEVT_THREADTASK_STARTED)
#define EVT_THREADTASK_STARTED(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_THREADTASK_STARTED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_THREADTASK_ENDED)
#define EVT_THREADTASK_ENDED(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_THREADTASK_ENDED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_THREADTASK_ALLDONE)
#define EVT_THREADTASK_ALLDONE(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_THREADTASK_ALLDONE, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),

// request app to add and manage a docked window
DECLARE_CB_EVENT_TYPE(cbEVT_ADD_DOCK_WINDOW)
#define EVT_ADD_DOCK_WINDOW(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_ADD_DOCK_WINDOW, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksDockEventFunction) & fn, (wxObject *) NULL ),
// request app to stop managing a docked window
DECLARE_CB_EVENT_TYPE(cbEVT_REMOVE_DOCK_WINDOW)
#define EVT_REMOVE_DOCK_WINDOW(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_REMOVE_DOCK_WINDOW, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksDockEventFunction) & fn, (wxObject *) NULL ),
// request app to show a docked window
DECLARE_CB_EVENT_TYPE(cbEVT_SHOW_DOCK_WINDOW)
#define EVT_SHOW_DOCK_WINDOW(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_SHOW_DOCK_WINDOW, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksDockEventFunction) & fn, (wxObject *) NULL ),
// request app to hide a docked window
DECLARE_CB_EVENT_TYPE(cbEVT_HIDE_DOCK_WINDOW)
#define EVT_HIDE_DOCK_WINDOW(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_HIDE_DOCK_WINDOW, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksDockEventFunction) & fn, (wxObject *) NULL ),
// request app to switch view layout
DECLARE_CB_EVENT_TYPE(cbEVT_SWITCH_VIEW_LAYOUT)
#define EVT_SWITCH_VIEW_LAYOUT(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_SWITCH_VIEW_LAYOUT, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksLayoutEventFunction) & fn, (wxObject *) NULL ),

// compiler-related events
DECLARE_CB_EVENT_TYPE(cbEVT_COMPILER_STARTED)
#define EVT_COMPILER_STARTED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_COMPILER_STARTED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_COMPILER_FINISHED)
#define EVT_COMPILER_FINISHED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_COMPILER_FINISHED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

// debugger-related events
DECLARE_CB_EVENT_TYPE(cbEVT_DEBUGGER_STARTED)
#define EVT_DEBUGGER_STARTED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_DEBUGGER_STARTED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_DEBUGGER_PAUSED)
#define EVT_DEBUGGER_PAUSED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_DEBUGGER_PAUSED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
DECLARE_CB_EVENT_TYPE(cbEVT_DEBUGGER_FINISHED)
#define EVT_DEBUGGER_FINISHED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_DEBUGGER_FINISHED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

#endif // SDK_EVENTS_H
