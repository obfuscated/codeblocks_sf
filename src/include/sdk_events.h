/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SDK_EVENTS_H
#define SDK_EVENTS_H

#include <wx/event.h>
#include <wx/intl.h>
#include "settings.h"
#include "prep.h"

class cbProject;
class EditorBase;
class cbPlugin;
class Logger;

/** A generic Code::Blocks event. */
class EVTIMPORT CodeBlocksEvent : public wxCommandEvent
{
	public:
		CodeBlocksEvent(wxEventType commandType = wxEVT_NULL, int id = 0, cbProject* project = nullptr, EditorBase* editor = nullptr, cbPlugin* plugin = nullptr, EditorBase* old_editor = nullptr)
			: wxCommandEvent(commandType, id),
			m_pProject(project),
			m_pEditor(editor),
			m_pOldEditor(old_editor),
			m_pPlugin(plugin),
			m_X(0),
			m_Y(0) {}
		CodeBlocksEvent(const CodeBlocksEvent& event)
			: wxCommandEvent(event),
			m_pProject(event.m_pProject),
			m_pEditor(event.m_pEditor),
			m_pOldEditor(event.m_pOldEditor),
			m_pPlugin(event.m_pPlugin),
			m_X(event.m_X),
			m_Y(event.m_Y) {}
		virtual wxEvent *Clone() const { return new CodeBlocksEvent(*this); }

		cbProject* GetProject() const             { return m_pProject;    }
		void       SetProject(cbProject* project) { m_pProject = project; }

		EditorBase* GetEditor() const             { return m_pEditor;   }
		void        SetEditor(EditorBase* editor) { m_pEditor = editor; }

		EditorBase* GetOldEditor() const             { return m_pOldEditor;   }
		void        SetOldEditor(EditorBase* editor) { m_pOldEditor = editor; }

		cbPlugin* GetPlugin() const           { return m_pPlugin;   }
		void      SetPlugin(cbPlugin* plugin) { m_pPlugin = plugin; }

		int  GetX() const { return m_X; }
		void SetX(int x)  { m_X = x;    }

		int  GetY() const { return m_Y; }
		void SetY(int y)  { m_Y = y;    }

		const wxString& GetBuildTargetName() const                 { return m_TargetName;   }
		void            SetBuildTargetName(const wxString& target) { m_TargetName = target; }

		// the following two functions are only valid for EVT_BUILDTARGET_RENAMED
		// and EVT_BUILDTARGET_SELECTED events
		const wxString& GetOldBuildTargetName() const                 { return m_OldTargetName;   }
		void            SetOldBuildTargetName(const wxString& target) { m_OldTargetName = target; }
	protected:
		cbProject*  m_pProject;
		EditorBase* m_pEditor;
		EditorBase* m_pOldEditor;
		cbPlugin*   m_pPlugin;
		// for some editor events
		int m_X;
		int m_Y;

		wxString m_TargetName;
		wxString m_OldTargetName;
	private:
		DECLARE_DYNAMIC_CLASS(CodeBlocksEvent)
};
typedef void (wxEvtHandler::*CodeBlocksEventFunction)(CodeBlocksEvent&);

/** Event used to request from the main app to add a window to the docking system. */
class EVTIMPORT CodeBlocksDockEvent : public wxEvent
{
    public:
        enum DockSide
        {
            dsLeft = 0,
            dsRight,
            dsTop,
            dsBottom,
            dsFloating,
            dsUndefined
        };

        CodeBlocksDockEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
            : wxEvent(id, commandType),
            title(_("Untitled")),
            pWindow(nullptr),
            desiredSize(100, 100),
            floatingSize(150, 150),
            minimumSize(40, 40),
            floatingPos(200, 150),
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
            : wxEvent(rhs),
            name(rhs.name),
            title(rhs.title),
            pWindow(rhs.pWindow),
            desiredSize(rhs.desiredSize),
            floatingSize(rhs.floatingSize),
            minimumSize(rhs.minimumSize),
            floatingPos(rhs.floatingPos),
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
        wxPoint floatingPos;///< The desired floating position.
        DockSide dockSide;  ///< The side to dock it.
        int row;            ///< The row to dock it.
        int column;         ///< The column to dock it.
        bool shown;         ///< If true, initially shown.
        bool stretch;       ///< If true, the dock will stretch to fill the @c dockSide
        bool hideable;      ///< If true, the dock will be allowed to be closed by the user.
        bool asTab;         ///< Add this window as a tab of an existing docked window (NOT IMPLEMENTED).
        wxString bitmap;    ///< The bitmap to use.

        char padding[64];    ///< Unused space in this class for later enhancements.
	private:
		DECLARE_DYNAMIC_CLASS(CodeBlocksDockEvent)
};
typedef void (wxEvtHandler::*CodeBlocksDockEventFunction)(CodeBlocksDockEvent&);

/** Event used to request from the main app to manage the view layouts. */
class EVTIMPORT CodeBlocksLayoutEvent : public wxEvent
{
    public:
        CodeBlocksLayoutEvent(wxEventType commandType = wxEVT_NULL, const wxString& layout_name = wxEmptyString)
            : wxEvent(wxID_ANY, commandType),
            layout(layout_name)
        {}
        CodeBlocksLayoutEvent(const CodeBlocksLayoutEvent& rhs)
            : wxEvent(rhs), layout(rhs.layout)
        {}
		virtual wxEvent *Clone() const { return new CodeBlocksLayoutEvent(*this); }

        wxString layout;      ///< Layout's name.
	private:
		DECLARE_DYNAMIC_CLASS(CodeBlocksLayoutEvent)
};
typedef void (wxEvtHandler::*CodeBlocksLayoutEventFunction)(CodeBlocksLayoutEvent&);

/** Event used to request from the main app to add a log.
  *
  * @note By adding a Logger*, the app takes ownership of it in cbEVT_ADD_LOG_WINDOW.
  * By adding a wxWindow*, the ownership is not touched and you should delete the window after calling
  * cbEVT_REMOVE_LOG_WINDOW for it.
  */
class EVTIMPORT CodeBlocksLogEvent : public wxEvent
{
    public:
        CodeBlocksLogEvent(wxEventType commandType = wxEVT_NULL, Logger* logger = nullptr, const wxString& title = wxEmptyString, wxBitmap *icon = nullptr);
        CodeBlocksLogEvent(wxEventType commandType, wxWindow* window, const wxString& title = wxEmptyString, wxBitmap *icon = nullptr);
        CodeBlocksLogEvent(wxEventType commandType, int logIndex, const wxString& title = wxEmptyString, wxBitmap *icon = nullptr);
        CodeBlocksLogEvent(const CodeBlocksLogEvent& rhs);

		virtual wxEvent *Clone() const { return new CodeBlocksLogEvent(*this); }

        Logger* logger; ///< The logger.
        int logIndex; ///< The logger's index.
		wxBitmap *icon; ///< The logger's icon. Valid only for cbEVT_ADD_LOG_WINDOW.
		wxString title; ///< The logger's title. Valid only for cbEVT_ADD_LOG_WINDOW.
		wxWindow* window; ///< A non-logger window. Needed at least by cbEVT_REMOVE_LOG_WINDOW.
	private:
		DECLARE_DYNAMIC_CLASS(CodeBlocksLogEvent)
};
typedef void (wxEvtHandler::*CodeBlocksLogEventFunction)(CodeBlocksLogEvent&);


// Thread event, this is basically a derived wxCommandEvent but enforce a deep copy of its
// m_cmdString member. wxEVT_COMMAND_MENU_SELECTED is reused and event handlers are matched by
// ids. This is just to conserve the old code, an alternative is use some
// new event type like: cbEVT_THREAD_LOG_MESSAGE, cbEVT_THREAD_LOGDEBUG_MESSAGE
// cbEVT_THREAD_SYSTEM_HEADER_UPDATE.

class EVTIMPORT CodeBlocksThreadEvent : public wxCommandEvent
{
public:
    CodeBlocksThreadEvent(wxEventType eventType = wxEVT_NULL, int id = wxID_ANY)
        : wxCommandEvent(eventType,id)
        { }

    CodeBlocksThreadEvent(const CodeBlocksThreadEvent& event)
        : wxCommandEvent(event)
    {
        // make sure our string member (which uses COW, aka refcounting) is not
        // shared by other wxString instances:
        SetString(GetString().c_str());
    }

    virtual wxEvent *Clone() const
    {
        return new CodeBlocksThreadEvent(*this);
    }


private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(CodeBlocksThreadEvent)
};

typedef void ( wxEvtHandler::*CodeblocksThreadEventFunction) ( CodeBlocksThreadEvent&);

#define CodeBlocksThreadEventHandler(func)  \
	(wxObjectEventFunction)(wxEventFunction) \
	wxStaticCastEvent(CodeblocksThreadEventFunction, &func)


#define EVT_CODEBLOCKS_THREAD(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MENU_SELECTED, id, wxID_ANY, \
    (wxObjectEventFunction) (wxEventFunction) (CodeblocksThreadEventFunction) \
    wxStaticCastEvent( ThreadEventFunction, & fn ), (wxObject *) NULL ),

//
// if you add more event types, remember to add event sinks in Manager...
//

// app events
extern EVTIMPORT const wxEventType cbEVT_APP_STARTUP_DONE;
#define EVT_APP_STARTUP_DONE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_APP_STARTUP_DONE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_APP_START_SHUTDOWN;
#define EVT_APP_START_SHUTDOWN(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_APP_START_SHUTDOWN, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_APP_ACTIVATED;
#define EVT_APP_ACTIVATED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_APP_ACTIVATED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_APP_DEACTIVATED;
#define EVT_APP_DEACTIVATED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_APP_DEACTIVATED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_APP_CMDLINE;
#define EVT_APP_CMDLINE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_APP_CMDLINE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),


// plugin events
extern EVTIMPORT const wxEventType cbEVT_PLUGIN_ATTACHED;
#define EVT_PLUGIN_ATTACHED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PLUGIN_ATTACHED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PLUGIN_RELEASED;
#define EVT_PLUGIN_RELEASED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PLUGIN_RELEASED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PLUGIN_INSTALLED;
#define EVT_PLUGIN_INSTALLED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PLUGIN_INSTALLED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PLUGIN_UNINSTALLED;
#define EVT_PLUGIN_UNINSTALLED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PLUGIN_UNINSTALLED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PLUGIN_LOADING_COMPLETE;
#define EVT_PLUGIN_LOADING_COMPLETE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PLUGIN_LOADING_COMPLETE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

// editor events
extern EVTIMPORT const wxEventType cbEVT_EDITOR_CLOSE;
#define EVT_EDITOR_CLOSE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_CLOSE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_EDITOR_OPEN;
#define EVT_EDITOR_OPEN(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_OPEN, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_EDITOR_SWITCHED;
#define EVT_EDITOR_SWITCHED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_SWITCHED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_EDITOR_ACTIVATED;
#define EVT_EDITOR_ACTIVATED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_ACTIVATED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_EDITOR_DEACTIVATED;
#define EVT_EDITOR_DEACTIVATED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_DEACTIVATED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_EDITOR_BEFORE_SAVE;
#define EVT_EDITOR_BEFORE_SAVE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_BEFORE_SAVE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_EDITOR_SAVE;
#define EVT_EDITOR_SAVE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_SAVE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_EDITOR_MODIFIED;
#define EVT_EDITOR_MODIFIED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_MODIFIED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_EDITOR_TOOLTIP;
#define EVT_EDITOR_TOOLTIP(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_TOOLTIP, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_EDITOR_TOOLTIP_CANCEL;
#define EVT_EDITOR_TOOLTIP_CANCEL(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_TOOLTIP_CANCEL, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_EDITOR_SPLIT;
#define EVT_EDITOR_SPLIT(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_SPLIT, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_EDITOR_UNSPLIT;
#define EVT_EDITOR_UNSPLIT(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_UNSPLIT, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_EDITOR_UPDATE_UI;
#define EVT_EDITOR_UPDATE_UI(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_UPDATE_UI, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

extern EVTIMPORT const wxEventType cbEVT_EDITOR_CC_DONE;
#define EVT_EDITOR_CC_DONE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_EDITOR_CC_DONE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

// project events
extern EVTIMPORT const wxEventType cbEVT_PROJECT_NEW;
#define EVT_PROJECT_NEW(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_NEW, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_CLOSE;
#define EVT_PROJECT_CLOSE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_CLOSE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_OPEN;
#define EVT_PROJECT_OPEN(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_OPEN, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_SAVE;
#define EVT_PROJECT_SAVE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_SAVE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_ACTIVATE;
#define EVT_PROJECT_ACTIVATE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_ACTIVATE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_BEGIN_ADD_FILES;
#define EVT_PROJECT_BEGIN_ADD_FILES(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_BEGIN_ADD_FILES, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_END_ADD_FILES;
#define EVT_PROJECT_END_ADD_FILES(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_END_ADD_FILES, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_BEGIN_REMOVE_FILES;
#define EVT_PROJECT_BEGIN_REMOVE_FILES(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_BEGIN_REMOVE_FILES, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_END_REMOVE_FILES;
#define EVT_PROJECT_END_REMOVE_FILES(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_END_REMOVE_FILES, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_FILE_ADDED;
#define EVT_PROJECT_FILE_ADDED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_FILE_ADDED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_FILE_REMOVED;
#define EVT_PROJECT_FILE_REMOVED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_FILE_REMOVED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_FILE_CHANGED;
#define EVT_PROJECT_FILE_CHANGED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_FILE_CHANGED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_POPUP_MENU;
#define EVT_PROJECT_POPUP_MENU(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_POPUP_MENU, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_TARGETS_MODIFIED;
#define EVT_PROJECT_TARGETS_MODIFIED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_TARGETS_MODIFIED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_RENAMED;
#define EVT_PROJECT_RENAMED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_RENAMED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PROJECT_OPTIONS_CHANGED;
#define EVT_PROJECT_OPTIONS_CHANGED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PROJECT_OPTIONS_CHANGED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_WORKSPACE_CHANGED;
#define EVT_WORKSPACE_CHANGED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_WORKSPACE_CHANGED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_WORKSPACE_LOADING_COMPLETE;
#define EVT_WORKSPACE_LOADING_COMPLETE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_WORKSPACE_LOADING_COMPLETE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_WORKSPACE_CLOSING_BEGIN;
#define EVT_WORKSPACE_CLOSING_BEGIN(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_WORKSPACE_CLOSE_BEGIN, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_WORKSPACE_CLOSING_COMPLETE;
#define EVT_WORKSPACE_CLOSING_COMPLETE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_WORKSPACE_CLOSE_COMPLETE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

// build targets events
extern EVTIMPORT const wxEventType cbEVT_BUILDTARGET_ADDED;
#define EVT_BUILDTARGET_ADDED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_BUILDTARGET_ADDED, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksEventFunction) & fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_BUILDTARGET_REMOVED;
#define EVT_BUILDTARGET_REMOVED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_BUILDTARGET_REMOVED, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksEventFunction) & fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_BUILDTARGET_RENAMED;
#define EVT_BUILDTARGET_RENAMED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_BUILDTARGET_RENAMED, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksEventFunction) & fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_BUILDTARGET_SELECTED;
#define EVT_BUILDTARGET_SELECTED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_BUILDTARGET_SELECTED, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksEventFunction) & fn, (wxObject *) NULL ),

// pipedprocess events
extern EVTIMPORT const wxEventType cbEVT_PIPEDPROCESS_STDOUT;
#define EVT_PIPEDPROCESS_STDOUT(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_STDOUT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_PIPEDPROCESS_STDOUT_RANGE(id, id1, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_STDOUT, id, id1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PIPEDPROCESS_STDERR;
#define EVT_PIPEDPROCESS_STDERR(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_STDERR, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_PIPEDPROCESS_STDERR_RANGE(id, id1, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_STDERR, id, id1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_PIPEDPROCESS_TERMINATED;
#define EVT_PIPEDPROCESS_TERMINATED(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_TERMINATED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
#define EVT_PIPEDPROCESS_TERMINATED_RANGE(id, id1, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_PIPEDPROCESS_TERMINATED, id, id1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),

// thread-pool events
extern EVTIMPORT const wxEventType cbEVT_THREADTASK_STARTED;
#define EVT_THREADTASK_STARTED(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_THREADTASK_STARTED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_THREADTASK_ENDED;
#define EVT_THREADTASK_ENDED(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_THREADTASK_ENDED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_THREADTASK_ALLDONE;
#define EVT_THREADTASK_ALLDONE(id, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_THREADTASK_ALLDONE, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),

// request app to add and manage a docked window
extern EVTIMPORT const wxEventType cbEVT_ADD_DOCK_WINDOW;
#define EVT_ADD_DOCK_WINDOW(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_ADD_DOCK_WINDOW, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksDockEventFunction) & fn, (wxObject *) NULL ),
// request app to stop managing a docked window
extern EVTIMPORT const wxEventType cbEVT_REMOVE_DOCK_WINDOW;
#define EVT_REMOVE_DOCK_WINDOW(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_REMOVE_DOCK_WINDOW, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksDockEventFunction) & fn, (wxObject *) NULL ),
// request app to show a docked window
extern EVTIMPORT const wxEventType cbEVT_SHOW_DOCK_WINDOW;
#define EVT_SHOW_DOCK_WINDOW(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_SHOW_DOCK_WINDOW, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksDockEventFunction) & fn, (wxObject *) NULL ),
// request app to hide a docked window
extern EVTIMPORT const wxEventType cbEVT_HIDE_DOCK_WINDOW;
#define EVT_HIDE_DOCK_WINDOW(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_HIDE_DOCK_WINDOW, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksDockEventFunction) & fn, (wxObject *) NULL ),
// app notifies that a docked window has been hidden/shown
// to actually find out its state use IsWindowReallyShown(event.pWindow);
extern EVTIMPORT const wxEventType cbEVT_DOCK_WINDOW_VISIBILITY;
#define EVT_DOCK_WINDOW_VISIBILITY(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_DOCK_WINDOW_VISIBILITY, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksDockEventFunction) & fn, (wxObject *) NULL ),
// force update current view layout
extern EVTIMPORT const wxEventType cbEVT_UPDATE_VIEW_LAYOUT;
#define EVT_UPDATE_VIEW_LAYOUT(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_UPDATE_VIEW_LAYOUT, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksLayoutEventFunction) & fn, (wxObject *) NULL ),
// ask which is the current view layout
extern EVTIMPORT const wxEventType cbEVT_QUERY_VIEW_LAYOUT;
#define EVT_QUERY_VIEW_LAYOUT(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_QUERY_VIEW_LAYOUT, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksLayoutEventFunction) & fn, (wxObject *) NULL ),
// request app to switch view layout
extern EVTIMPORT const wxEventType cbEVT_SWITCH_VIEW_LAYOUT;
#define EVT_SWITCH_VIEW_LAYOUT(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_SWITCH_VIEW_LAYOUT, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksLayoutEventFunction) & fn, (wxObject *) NULL ),
// app notifies that a new layout has been applied
extern EVTIMPORT const wxEventType cbEVT_SWITCHED_VIEW_LAYOUT;
#define EVT_SWITCHED_VIEW_LAYOUT(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_SWITCHED_VIEW_LAYOUT, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksLayoutEventFunction) & fn, (wxObject *) NULL ),
// app notifies that the menubar is started being (re)created
extern EVTIMPORT const wxEventType cbEVT_MENUBAR_CREATE_BEGIN;
#define EVT_MENUBAR_CREATE_BEGIN(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_MENUBAR_CREATE_BEGIN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksEventFunction) & fn, (wxObject *) NULL ),
// app notifies that the menubar (re)creation ended
extern EVTIMPORT const wxEventType cbEVT_MENUBAR_CREATE_END;
#define EVT_MENUBAR_CREATE_END(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_MENUBAR_CREATE_END, -1, -1, (wxObjectEventFunction) (wxEventFunction) (CodeBlocksEventFunction) & fn, (wxObject *) NULL ),

// compiler-related events (compiler plugins usually fire them)
extern EVTIMPORT const wxEventType cbEVT_COMPILER_STARTED;
#define EVT_COMPILER_STARTED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_COMPILER_STARTED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_COMPILER_FINISHED;
#define EVT_COMPILER_FINISHED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_COMPILER_FINISHED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_COMPILER_SET_BUILD_OPTIONS;
#define EVT_COMPILER_SET_BUILD_OPTIONS(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_COMPILER_SET_BUILD_OPTIONS, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_CLEAN_PROJECT_STARTED;
#define EVT_CLEAN_PROJECT_STARTED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_CLEAN_PROJECT_STARTED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_CLEAN_WORKSPACE_STARTED;
#define EVT_CLEAN_WORKSPACE_STARTED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_CLEAN_WORKSPACE_STARTED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_COMPILER_SETTINGS_CHANGED;
#define EVT_COMPILER_SETTINGS_CHANGED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_COMPILER_SETTINGS_CHANGED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

// request app to compile a single file
extern EVTIMPORT const wxEventType cbEVT_COMPILE_FILE_REQUEST;
#define EVT_COMPILE_FILE_REQUEST(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_COMPILE_FILE_REQUEST, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

// debugger-related events (debugger plugins must fire them)
extern EVTIMPORT const wxEventType cbEVT_DEBUGGER_STARTED;
#define EVT_DEBUGGER_STARTED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_DEBUGGER_STARTED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_DEBUGGER_PAUSED;
#define EVT_DEBUGGER_PAUSED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_DEBUGGER_PAUSED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_DEBUGGER_FINISHED;
#define EVT_DEBUGGER_FINISHED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_DEBUGGER_FINISHED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

// logger-related events

// add a log window
extern EVTIMPORT const wxEventType cbEVT_ADD_LOG_WINDOW;
#define EVT_ADD_LOG_WINDOW(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_ADD_LOG_WINDOW, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksLogEventFunction)&fn, (wxObject *) NULL ),
// remove a log window
extern EVTIMPORT const wxEventType cbEVT_REMOVE_LOG_WINDOW;
#define EVT_REMOVE_LOG_WINDOW(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_REMOVE_LOG_WINDOW, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksLogEventFunction)&fn, (wxObject *) NULL ),
// switch to a log window (make it visible)
extern EVTIMPORT const wxEventType cbEVT_HIDE_LOG_WINDOW;
#define EVT_HIIDE_LOG_WINDOW(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_HIDE_LOG_WINDOW, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksLogEventFunction)&fn, (wxObject *) NULL ),
// switch to a log window (make it visible)
extern EVTIMPORT const wxEventType cbEVT_SWITCH_TO_LOG_WINDOW;
#define EVT_SWITCH_TO_LOG_WINDOW(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_SWITCH_TO_LOG_WINDOW, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksLogEventFunction)&fn, (wxObject *) NULL ),
// gets the active log window
extern EVTIMPORT const wxEventType cbEVT_GET_ACTIVE_LOG_WINDOW;
#define EVT_GET_ACTIVE_LOG_WINDOW(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_GET_ACTIVE_LOG_WINDOW, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksLogEventFunction)&fn, (wxObject *) NULL ),
// show log manager
extern EVTIMPORT const wxEventType cbEVT_SHOW_LOG_MANAGER;
#define EVT_SHOW_LOG_MANAGER(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_SHOW_LOG_MANAGER, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksLogEventFunction)&fn, (wxObject *) NULL ),
// hide log manager
extern EVTIMPORT const wxEventType cbEVT_HIDE_LOG_MANAGER;
#define EVT_HIDE_LOG_MANAGER(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_HIDE_LOG_MANAGER, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksLogEventFunction)&fn, (wxObject *) NULL ),
// "lock" it (used with auto-hiding functionality)
extern EVTIMPORT const wxEventType cbEVT_LOCK_LOG_MANAGER;
#define EVT_LOCK_LOG_MANAGER(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_LOCK_LOG_MANAGER, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksLogEventFunction)&fn, (wxObject *) NULL ),
// "unlock" it (used with auto-hiding functionality)
extern EVTIMPORT const wxEventType cbEVT_UNLOCK_LOG_MANAGER;
#define EVT_UNLOCK_LOG_MANAGER(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_UNLOCK_LOG_MANAGER, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksLogEventFunction)&fn, (wxObject *) NULL ),

//cbAUiNotebook related events
// left doubleclick on a tab
extern EVTIMPORT const wxEventType cbEVT_CBAUIBOOK_LEFT_DCLICK;
#define EVT_CBAUIBOOK_LEFT_DCLICK(winid, fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_CBAUIBOOK_LEFT_DCLICK, winid, -1, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)&fn, (wxObject *) NULL ),

// code-completion related events (CodeCompletion plugin usually fires them)
extern EVTIMPORT const wxEventType cbEVT_COMPLETE_CODE;
#define EVT_COMPLETE_CODE(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_COMPLETE_CODE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),
extern EVTIMPORT const wxEventType cbEVT_SHOW_CALL_TIP;
#define EVT_SHOW_CALL_TIP(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_SHOW_CALL_TIP, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

// settings events

// event.GetInt() returns value of type cbSettingsType::Type indicating which setting group was changed
struct cbSettingsType
{
    enum Type
    {
        Compiler,
        Debugger,
        Environment,
        Editor,
        Plugins
    };
};
extern EVTIMPORT const wxEventType cbEVT_SETTINGS_CHANGED;
#define EVT_SETTINGS_CHANGED(fn) DECLARE_EVENT_TABLE_ENTRY( cbEVT_SETTINGS_CHANGED, -1, -1, (wxObjectEventFunction)(wxEventFunction)(CodeBlocksEventFunction)&fn, (wxObject *) NULL ),

#endif // SDK_EVENTS_H
