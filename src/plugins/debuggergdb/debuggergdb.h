#ifndef DEBUGGERGDB_H
#define DEBUGGERGDB_H

#include <settings.h> // much of the SDK is here
#include <sdk_events.h>
#include <cbplugin.h>
#include <simpletextlog.h>
#include <pipedprocess.h>
#include <wx/regex.h>
#include <wx/tipwin.h>

#include "debuggertree.h"

class DebuggerGDB : public cbDebuggerPlugin
{
	public:
		DebuggerGDB();
		~DebuggerGDB();
		int Configure();
		void BuildMenu(wxMenuBar* menuBar);
		void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg);
		void BuildToolBar(wxToolBar* toolBar);
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
	protected:
        void ConvertToGDBFriendly(wxString& str);
		int Debug();
		void CmdContinue();
		void CmdNext();
		void CmdStep();
		void CmdRunToCursor();
		void CmdToggleBreakpoint();
		void CmdStop();
		bool IsRunning(){ return m_pProcess; }
		int GetExitCode(){ return m_LastExitCode; }
	private:
		void ParseOutput(const wxString& output);
		void SyncEditor(const wxString& filename, int line);
		void BringAppToFront();
		void ClearActiveMarkFromAllEditors();
		void SetBreakpoints();
		wxString GetInfoFor(const wxString& dbgCmd);
		wxString GetNextOutputLine(bool useStdErr = false);
		wxString GetNextOutputLineClean(bool useStdErr = false);
		void DoWatches();
        wxString GetEditorWordAtCaret();

		void SendCommand(const wxString& cmd);
		void OnUpdateUI(wxUpdateUIEvent& event);
		void OnDebug(wxCommandEvent& event);
		void OnStop(wxCommandEvent& event);
		void OnSendCommandToGDB(wxCommandEvent& event);
		void OnAddSymbolFile(wxCommandEvent& event);
		void OnEditWatches(wxCommandEvent& event);
		void OnContinue(wxCommandEvent& event);
		void OnNext(wxCommandEvent& event);
		void OnStep(wxCommandEvent& event);
		void OnToggleBreakpoint(wxCommandEvent& event);
		void OnRunToCursor(wxCommandEvent& event);
		void OnBreakpointAdded(CodeBlocksEvent& event);
		void OnBreakpointDeleted(CodeBlocksEvent& event);
		void OnValueTooltip(CodeBlocksEvent& event);
        void OnGDBOutput(wxCommandEvent& event);
        void OnGDBError(wxCommandEvent& event);
        void OnGDBTerminated(wxCommandEvent& event);
        void OnIdle(wxIdleEvent& event);
		void OnTimer(wxTimerEvent& event);
		void OnWatchesChanged(wxCommandEvent& event);
        void OnAddWatch(wxCommandEvent& event);

		wxMenu* m_pMenu;
        SimpleTextLog* m_pLog;
        SimpleTextLog* m_pDbgLog;
		PipedProcess* m_pProcess;
        int m_PageIndex;
        int m_DbgPageIndex;
		wxRegEx reSource;
		bool m_ProgramIsStopped;
		wxString m_LastCmd;
		wxString m_LastFrame;
		wxString m_Variable;
		cbCompilerPlugin* m_pCompiler;
		bool m_LastExitCode;
		int m_TargetIndex;
		int m_Pid;
		wxString m_Tbreak;
		wxTipWindow* m_EvalWin;
		wxString m_LastEval;
		wxRect m_EvalRect;
		wxTimer m_TimerPollDebugger;
		DebuggerTree* m_pTree;
		bool m_NoDebugInfo;
		
		DECLARE_EVENT_TABLE()
};

extern "C"
{
	PLUGIN_EXPORT cbPlugin* GetPlugin();
};

#endif // DEBUGGERGDB_H

