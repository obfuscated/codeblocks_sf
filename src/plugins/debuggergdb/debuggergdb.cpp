/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include <wx/txtstrm.h>
#include <wx/regex.h>
#include <wx/dialog.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

#include <manager.h>
#include <configmanager.h>
#include <logmanager.h>
#include <projectmanager.h>
#include <pluginmanager.h>
#include <editormanager.h>
#include <macrosmanager.h>
#include <cbeditor.h>
#include <projectbuildtarget.h>
#include <sdk_events.h>
#include <editarraystringdlg.h>
#include <compilerfactory.h>
#include <projectloader_hooks.h>
#include <xtra_res.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>

#include "annoyingdialog.h"
#include "debuggergdb.h"
#include "debuggerdriver.h"
#include "debuggeroptionsdlg.h"
#include "debuggeroptionsprjdlg.h"
#include "debuggertree.h"
#include "editbreakpointdlg.h"
#include "editwatchesdlg.h"
#include "examinememorydlg.h"
#include "threadsdlg.h"
#include "editwatchdlg.h"
#include "databreakpointdlg.h"
#include "globals.h"
#include "cbstyledtextctrl.h"


#ifdef __WXMSW__
    #include <winbase.h>
#else
    int GetShortPathName(const void*, void*, int){/* bogus */ return 0; };
#endif

#ifndef CB_PRECOMP
    #include <wx/frame.h> // GetMenuBar
    #include "cbproject.h"
#endif

#if defined(__APPLE__) && defined(__MACH__)
    #define LIBRARY_ENVVAR _T("DYLD_LIBRARY_PATH")
#elif !defined(__WXMSW__)
    #define LIBRARY_ENVVAR _T("LD_LIBRARY_PATH")
#else
    #define LIBRARY_ENVVAR _T("PATH")
#endif

#define implement_debugger_toolbar

// function pointer to DebugBreakProcess under windows (XP+)
#if (_WIN32_WINNT >= 0x0501)
typedef WINBASEAPI BOOL WINAPI (*DebugBreakProcessApiCall)(HANDLE);
DebugBreakProcessApiCall DebugBreakProcessFunc = 0;
HINSTANCE kernelLib = 0;
#endif

// valid debugger command constants
enum DebugCommandConst
{
    CMD_CONTINUE,
    CMD_STEP,
    CMD_STEPIN,
    CMD_STEPOUT,
    CMD_STEP_INSTR,
    CMD_STOP,
    CMD_BACKTRACE,
    CMD_DISASSEMBLE,
    CMD_REGISTERS,
    CMD_MEMORYDUMP,
    CMD_RUNNINGTHREADS,
};

const wxString g_EscapeChar = wxChar(26);

int idMenuDebug = XRCID("idDebuggerMenuDebug");
int idMenuRunToCursor = XRCID("idDebuggerMenuRunToCursor");
int idMenuNext = XRCID("idDebuggerMenuNext");
int idMenuStep = XRCID("idDebuggerMenuStep");
int idMenuNextInstr = XRCID("idDebuggerMenuNextInstr");
int idMenuStepOut = XRCID("idDebuggerMenuStepOut");
int idMenuStop = XRCID("idDebuggerMenuStop");
int idMenuContinue = XRCID("idDebuggerMenuContinue");
int idMenuToggleBreakpoint = XRCID("idDebuggerMenuToggleBreakpoint");
int idMenuRemoveAllBreakpoints = XRCID("idDebuggerMenuRemoveAllBreakpoints");
int idMenuAddDataBreakpoint = XRCID("idMenuAddDataBreakpoint");
int idMenuSendCommandToGDB = XRCID("idDebuggerMenuSendCommandToGDB");
int idMenuAddSymbolFile = XRCID("idDebuggerMenuAddSymbolFile");
int idMenuCPU = XRCID("idDebuggerMenuCPU");
int idMenuRegisters = XRCID("idDebuggerMenuRegisters");
int idMenuWatches = XRCID("idDebuggerMenuWatches");
int idMenuBacktrace = XRCID("idDebuggerMenuBacktrace");
int idMenuThreads = XRCID("idDebuggerMenuThreads");
int idMenuMemory = XRCID("idDebuggerMenuMemory");
int idMenuBreakpoints = XRCID("idDebuggerMenuBreakpoints");
int idMenuEditWatches = XRCID("idDebuggerMenuEditWatches");
int idMenuAttachToProcess = XRCID("idDebuggerMenuAttachToProcess");
int idMenuDetach = XRCID("idDebuggerMenuDetach");

int idDebuggerToolWindows = XRCID("idDebuggerToolWindows");

int idDebuggerToolInfo = XRCID("idDebuggerToolInfo");
int idMenuInfoFrame = XRCID("idDebuggerCurrentFrame");
int idMenuInfoDLL = XRCID("idDebuggerLoadedDLLs");
int idMenuInfoFiles = XRCID("idDebuggerFiles");
int idMenuInfoFPU = XRCID("idDebuggerFPU");
int idMenuInfoSignals = XRCID("idDebuggerSignals");

int idGDBProcess = wxNewId();
int idTimerPollDebugger = wxNewId();
int idMenuDebuggerAddWatch = wxNewId();
int idMenuSettings = wxNewId();

// this auto-registers the plugin
namespace
{
    PluginRegistrant<DebuggerGDB> reg(_T("Debugger"));
}

BEGIN_EVENT_TABLE(DebuggerGDB, cbDebuggerPlugin)
    EVT_UPDATE_UI_RANGE(idMenuContinue, idMenuDebuggerAddWatch, DebuggerGDB::OnUpdateUI)
    // these are different because they are loaded from the XRC
    EVT_UPDATE_UI(XRCID("idDebuggerMenuDebug"), DebuggerGDB::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idDebuggerMenuRunToCursor"), DebuggerGDB::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idDebuggerMenuNext"), DebuggerGDB::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idDebuggerMenuNextInstr"), DebuggerGDB::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idDebuggerMenuStep"), DebuggerGDB::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idDebuggerMenuStepOut"), DebuggerGDB::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idDebuggerMenuStop"), DebuggerGDB::OnUpdateUI)

    EVT_UPDATE_UI(XRCID("idDebuggerCurrentFrame"), DebuggerGDB::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idDebuggerLoadedDLLs"), DebuggerGDB::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idDebuggerFiles"), DebuggerGDB::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idDebuggerFPU"), DebuggerGDB::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idDebuggerSignals"), DebuggerGDB::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idDebuggerThreads"), DebuggerGDB::OnUpdateUI)

    EVT_UPDATE_UI(XRCID("idDebuggerToolInfo"), DebuggerGDB::OnUpdateUI)

    EVT_MENU(idMenuDebug, DebuggerGDB::OnDebug)
    EVT_MENU(idMenuContinue, DebuggerGDB::OnContinue)
    EVT_MENU(idMenuNext, DebuggerGDB::OnNext)
    EVT_MENU(idMenuStep, DebuggerGDB::OnStep)
    EVT_MENU(idMenuNextInstr, DebuggerGDB::OnNextInstr)
    EVT_MENU(idMenuStepOut, DebuggerGDB::OnStepOut)
    EVT_MENU(idMenuToggleBreakpoint, DebuggerGDB::OnToggleBreakpoint)
    EVT_MENU(idMenuRemoveAllBreakpoints, DebuggerGDB::OnRemoveAllBreakpoints)
    EVT_MENU(idMenuAddDataBreakpoint, DebuggerGDB::OnAddDataBreakpoint)
    EVT_MENU(idMenuRunToCursor, DebuggerGDB::OnRunToCursor)
    EVT_MENU(idMenuStop, DebuggerGDB::OnStop)
    EVT_MENU(idMenuSendCommandToGDB, DebuggerGDB::OnSendCommandToGDB)
    EVT_MENU(idMenuAddSymbolFile, DebuggerGDB::OnAddSymbolFile)
    EVT_MENU(idMenuBacktrace, DebuggerGDB::OnBacktrace)
    EVT_MENU(idMenuThreads, DebuggerGDB::OnRunningThreads)
    EVT_MENU(idMenuMemory, DebuggerGDB::OnExamineMemory)
    EVT_MENU(idMenuCPU, DebuggerGDB::OnDisassemble)
    EVT_MENU(idMenuRegisters, DebuggerGDB::OnRegisters)
    EVT_MENU(idMenuWatches, DebuggerGDB::OnViewWatches)
    EVT_MENU(idMenuBreakpoints, DebuggerGDB::OnBreakpoints)
    EVT_MENU(idMenuEditWatches, DebuggerGDB::OnEditWatches)
    EVT_MENU(idMenuDebuggerAddWatch, DebuggerGDB::OnAddWatch)
    EVT_MENU(idMenuAttachToProcess, DebuggerGDB::OnAttachToProcess)
    EVT_MENU(idMenuDetach, DebuggerGDB::OnDetach)
    EVT_MENU(idMenuSettings, DebuggerGDB::OnSettings)

    EVT_MENU(idDebuggerToolWindows, DebuggerGDB::OnDebugWindows)
    EVT_MENU(idDebuggerToolInfo, DebuggerGDB::OnToolInfo)

    EVT_MENU(idMenuInfoFrame, DebuggerGDB::OnInfoFrame)
    EVT_MENU(idMenuInfoDLL, DebuggerGDB::OnInfoDLL)
    EVT_MENU(idMenuInfoFiles, DebuggerGDB::OnInfoFiles)
    EVT_MENU(idMenuInfoFPU, DebuggerGDB::OnInfoFPU)
    EVT_MENU(idMenuInfoSignals, DebuggerGDB::OnInfoSignals)

    EVT_PIPEDPROCESS_STDOUT(idGDBProcess, DebuggerGDB::OnGDBOutput)
    EVT_PIPEDPROCESS_STDERR(idGDBProcess, DebuggerGDB::OnGDBError)
    EVT_PIPEDPROCESS_TERMINATED(idGDBProcess, DebuggerGDB::OnGDBTerminated)

    EVT_IDLE(DebuggerGDB::OnIdle)
    EVT_TIMER(idTimerPollDebugger, DebuggerGDB::OnTimer)

    EVT_COMMAND(-1, cbCustom_WATCHES_CHANGED, DebuggerGDB::OnWatchesChanged)
    EVT_COMMAND(-1, DEBUGGER_CURSOR_CHANGED, DebuggerGDB::OnCursorChanged)
END_EVENT_TABLE()

DebuggerGDB::DebuggerGDB()
    : m_State(this),
    m_pMenu(0L),
    m_pLog(0L),
    m_pDbgLog(0L),
    m_pProcess(0L),
    m_pTbar(0L),
    m_PageIndex(-1),
    m_DbgPageIndex(-1),
    m_pCompiler(0L),
    m_LastExitCode(0),
    m_Pid(0),
    m_PidToAttach(0),
    m_NoDebugInfo(false),
    m_BreakOnEntry(false),
    m_HaltAtLine(0),
    m_HasDebugLog(false),
    m_StoppedOnSignal(false),
    m_pTree(0L),
    m_pDisassembly(0),
    m_pCPURegisters(0),
    m_pBacktrace(0),
    m_pBreakpointsWindow(0),
    m_pExamineMemoryDlg(0),
    m_pThreadsDlg(0),
    m_pProject(0),
    m_WaitingCompilerToFinish(false)
{
    if(!Manager::LoadResource(_T("debugger.zip")))
    {
        NotifyMissingFile(_T("debugger.zip"));
    }

    // get a function pointer to DebugBreakProcess under windows (XP+)
    #if (_WIN32_WINNT >= 0x0501)
    kernelLib = LoadLibrary(TEXT("kernel32.dll"));
    if (kernelLib)
        DebugBreakProcessFunc = (DebugBreakProcessApiCall)GetProcAddress(kernelLib, "DebugBreakProcess");
    #endif
}

DebuggerGDB::~DebuggerGDB()
{
    #if (_WIN32_WINNT >= 0x0501)
    if (kernelLib)
        FreeLibrary(kernelLib);
    #endif
}

void DebuggerGDB::OnAttach()
{
    m_TimerPollDebugger.SetOwner(this, idTimerPollDebugger);

    LogManager* msgMan = Manager::Get()->GetLogManager();
    m_pLog = new TextCtrlLogger(true);
    m_PageIndex = msgMan->SetLog(m_pLog);
    msgMan->Slot(m_PageIndex).title = _("Debugger");
    // set log image
    wxString prefix = ConfigManager::GetDataFolder() + _T("/images/");
    wxBitmap* bmp = new wxBitmap(cbLoadBitmap(prefix + _T("misc_16x16.png"), wxBITMAP_TYPE_PNG));
    msgMan->Slot(m_PageIndex).icon = bmp;

    CodeBlocksLogEvent evtAdd(cbEVT_ADD_LOG_WINDOW, m_pLog, msgMan->Slot(m_PageIndex).title, msgMan->Slot(m_PageIndex).icon);
    Manager::Get()->ProcessEvent(evtAdd);

    m_HasDebugLog = Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("debug_log"), false);
    if (m_HasDebugLog)
    {
        m_pDbgLog = new TextCtrlLogger(true);
        m_DbgPageIndex = msgMan->SetLog(m_pDbgLog);
        msgMan->Slot(m_DbgPageIndex).title = _("Debugger (debug)");
        // set log image
        bmp = new wxBitmap(cbLoadBitmap(prefix + _T("contents_16x16.png"), wxBITMAP_TYPE_PNG));
        msgMan->Slot(m_DbgPageIndex).icon = bmp;

        CodeBlocksLogEvent evtAdd(cbEVT_ADD_LOG_WINDOW, m_pDbgLog, msgMan->Slot(m_DbgPageIndex).title, msgMan->Slot(m_DbgPageIndex).icon);
        Manager::Get()->ProcessEvent(evtAdd);
    }

    m_pTree = new DebuggerTree(Manager::Get()->GetAppWindow(), this);
    m_pDisassembly = new DisassemblyDlg(Manager::Get()->GetAppWindow(), this);
    m_pCPURegisters = new CPURegistersDlg(Manager::Get()->GetAppWindow(), this);
    m_pBacktrace = new BacktraceDlg(Manager::Get()->GetAppWindow(), this);
    m_pBreakpointsWindow = new BreakpointsDlg(m_State);
    m_pExamineMemoryDlg = new ExamineMemoryDlg(Manager::Get()->GetAppWindow(), this);
    m_pThreadsDlg = new ThreadsDlg(Manager::Get()->GetAppWindow(), this);

    CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);

    evt.name = _T("DisassemblyPane");
    evt.title = _("Disassembly");
    evt.pWindow = m_pDisassembly;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(350, 250);
    evt.floatingSize.Set(350, 250);
    evt.minimumSize.Set(150, 150);
    Manager::Get()->ProcessEvent(evt);

    evt.name = _T("CPURegistersPane");
    evt.title = _("CPU Registers");
    evt.pWindow = m_pCPURegisters;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(350, 250);
    evt.floatingSize.Set(350, 250);
    evt.minimumSize.Set(150, 150);
    Manager::Get()->ProcessEvent(evt);

    evt.name = _T("CallStackPane");
    evt.title = _("Call stack");
    evt.pWindow = m_pBacktrace;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(150, 150);
    evt.floatingSize.Set(450, 150);
    evt.minimumSize.Set(150, 150);
    Manager::Get()->ProcessEvent(evt);

    evt.name = _T("WatchesPane");
    evt.title = _("Watches");
    evt.pWindow = m_pTree;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(150, 250);
    evt.floatingSize.Set(150, 250);
    evt.minimumSize.Set(150, 150);
    Manager::Get()->ProcessEvent(evt);

    evt.name = _T("BreakpointsPane");
    evt.title = _("Breakpoints");
    evt.pWindow = m_pBreakpointsWindow;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(350, 250);
    evt.floatingSize.Set(350, 250);
    evt.minimumSize.Set(150, 150);
    Manager::Get()->ProcessEvent(evt);

    evt.name = _T("ExamineMemoryPane");
    evt.title = _("Memory");
    evt.pWindow = m_pExamineMemoryDlg;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(450, 250);
    evt.floatingSize.Set(450, 250);
    evt.minimumSize.Set(350, 150);
    Manager::Get()->ProcessEvent(evt);

    evt.name = _T("ThreadsPane");
    evt.title = _("Running threads");
    evt.pWindow = m_pThreadsDlg;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(350, 75);
    evt.floatingSize.Set(450, 75);
    evt.minimumSize.Set(250, 75);
    Manager::Get()->ProcessEvent(evt);

    // hook to project loading procedure
    ProjectLoaderHooks::HookFunctorBase* myhook = new ProjectLoaderHooks::HookFunctor<DebuggerGDB>(this, &DebuggerGDB::OnProjectLoadingHook);
    m_HookId = ProjectLoaderHooks::RegisterHook(myhook);

    // register event sink
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_BREAKPOINT_ADD, new cbEventFunctor<DebuggerGDB, CodeBlocksEvent>(this, &DebuggerGDB::OnBreakpointAdd));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_BREAKPOINT_EDIT, new cbEventFunctor<DebuggerGDB, CodeBlocksEvent>(this, &DebuggerGDB::OnBreakpointEdit));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_BREAKPOINT_DELETE, new cbEventFunctor<DebuggerGDB, CodeBlocksEvent>(this, &DebuggerGDB::OnBreakpointDelete));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_TOOLTIP, new cbEventFunctor<DebuggerGDB, CodeBlocksEvent>(this, &DebuggerGDB::OnValueTooltip));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_OPEN, new cbEventFunctor<DebuggerGDB, CodeBlocksEvent>(this, &DebuggerGDB::OnEditorOpened));

    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE, new cbEventFunctor<DebuggerGDB, CodeBlocksEvent>(this, &DebuggerGDB::OnProjectActivated));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE, new cbEventFunctor<DebuggerGDB, CodeBlocksEvent>(this, &DebuggerGDB::OnProjectClosed));

    Manager::Get()->RegisterEventSink(cbEVT_COMPILER_STARTED, new cbEventFunctor<DebuggerGDB, CodeBlocksEvent>(this, &DebuggerGDB::OnCompilerStarted));
    Manager::Get()->RegisterEventSink(cbEVT_COMPILER_FINISHED, new cbEventFunctor<DebuggerGDB, CodeBlocksEvent>(this, &DebuggerGDB::OnCompilerFinished));

    Manager::Get()->RegisterEventSink(cbEVT_BUILDTARGET_SELECTED, new cbEventFunctor<DebuggerGDB, CodeBlocksEvent>(this, &DebuggerGDB::OnBuildTargetSelected));
}

void DebuggerGDB::OnRelease(bool appShutDown)
{
    ProjectLoaderHooks::UnregisterHook(m_HookId, true);

    if (m_State.HasDriver())
        m_State.GetDriver()->SetDebugWindows(0, 0, 0, 0, 0);

    if (m_pThreadsDlg)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pThreadsDlg;
        Manager::Get()->ProcessEvent(evt);
        m_pThreadsDlg->Destroy();
    }
    m_pThreadsDlg = 0;

    if (m_pExamineMemoryDlg)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pExamineMemoryDlg;
        Manager::Get()->ProcessEvent(evt);
        m_pExamineMemoryDlg->Destroy();
    }
    m_pExamineMemoryDlg = 0;

    if (m_pBreakpointsWindow)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pBreakpointsWindow;
        Manager::Get()->ProcessEvent(evt);
        m_pBreakpointsWindow->Destroy();
    }
    m_pBreakpointsWindow = 0;

    if (m_pDisassembly)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pDisassembly;
        Manager::Get()->ProcessEvent(evt);
        m_pDisassembly->Destroy();
    }
    m_pDisassembly = 0;

    if (m_pCPURegisters)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pCPURegisters;
        Manager::Get()->ProcessEvent(evt);
        m_pCPURegisters->Destroy();
    }
    m_pCPURegisters = 0;

    if (m_pBacktrace)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pBacktrace;
        Manager::Get()->ProcessEvent(evt);
        m_pBacktrace->Destroy();
    }
    m_pBacktrace = 0;

    if (m_pTree)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pTree;
        Manager::Get()->ProcessEvent(evt);
        m_pTree->Destroy();
    }
    m_pTree = 0L;

    //Close debug session when appShutDown
    if (m_State.HasDriver())
    {
        Stop();
        wxYieldIfNeeded();
    }

    m_State.CleanUp();

    if (Manager::Get()->GetLogManager())
    {
        if (m_HasDebugLog)
        {
            CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_pDbgLog);
            Manager::Get()->ProcessEvent(evt);
            m_pDbgLog = 0;
        }
        CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_pLog);
        Manager::Get()->ProcessEvent(evt);
        m_pLog = 0;
    }
    // vars for Linux console
    m_bIsConsole = false;
    m_nConsolePid = 0;
    m_ConsoleTty = wxEmptyString;
}

int DebuggerGDB::Configure()
{
//    DebuggerOptionsDlg dlg(Manager::Get()->GetAppWindow());
//    int ret = dlg.ShowModal();
//
//    bool needsRestart = Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("debug_log"), false) != m_HasDebugLog;
//    if (needsRestart)
//        cbMessageBox(_("Code::Blocks needs to be restarted for the changes to take effect."), _("Information"), wxICON_INFORMATION);
    return 0;
}

cbConfigurationPanel* DebuggerGDB::GetConfigurationPanel(wxWindow* parent)
{
    DebuggerOptionsDlg* dlg = new DebuggerOptionsDlg(parent, this);
    return dlg;
}

cbConfigurationPanel* DebuggerGDB::GetProjectConfigurationPanel(wxWindow* parent, cbProject* project)
{
    DebuggerOptionsProjectDlg* dlg = new DebuggerOptionsProjectDlg(parent, this, project);
    return dlg;
}

void DebuggerGDB::RefreshConfiguration()
{
    // the only thing that we need to change on the fly, is the debugger's debug log
    bool log_visible = Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("debug_log"), false);

    if (!log_visible && m_HasDebugLog)
    {
        CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_pDbgLog);
        Manager::Get()->ProcessEvent(evt);
        m_pDbgLog = 0;
    }
    else if (log_visible && !m_HasDebugLog)
    {
        m_pDbgLog = new TextCtrlLogger(true);
        m_DbgPageIndex = Manager::Get()->GetLogManager()->SetLog(m_pDbgLog);
        Manager::Get()->GetLogManager()->Slot(m_DbgPageIndex).title = _("Debugger (debug)");
        // set log image
        wxBitmap* bmp = new wxBitmap(cbLoadBitmap(ConfigManager::GetDataFolder() + _T("/images/contents_16x16.png"), wxBITMAP_TYPE_PNG));
        Manager::Get()->GetLogManager()->Slot(m_DbgPageIndex).icon = bmp;

        CodeBlocksLogEvent evtAdd(cbEVT_ADD_LOG_WINDOW, m_pDbgLog, Manager::Get()->GetLogManager()->Slot(m_DbgPageIndex).title, Manager::Get()->GetLogManager()->Slot(m_DbgPageIndex).icon);
        Manager::Get()->ProcessEvent(evtAdd);
    }
    m_HasDebugLog = log_visible;
}

void DebuggerGDB::BuildMenu(wxMenuBar* menuBar)
{
    if (!IsAttached())
        return;
    m_pMenu=Manager::Get()->LoadMenu(_T("debugger_menu"),true);

    // ok, now, where do we insert?
    // three possibilities here:
    // a) locate "Compile" menu and insert after it
    // b) locate "Project" menu and insert after it
    // c) if not found (?), insert at pos 5
    int finalPos = 5;
    int projcompMenuPos = menuBar->FindMenu(_("&Build"));
    if (projcompMenuPos == wxNOT_FOUND)
        projcompMenuPos = menuBar->FindMenu(_("&Compile"));

    if (projcompMenuPos != wxNOT_FOUND)
        finalPos = projcompMenuPos + 1;
    else
    {
        projcompMenuPos = menuBar->FindMenu(_("&Project"));
        if (projcompMenuPos != wxNOT_FOUND)
            finalPos = projcompMenuPos + 1;
    }
    menuBar->Insert(finalPos, m_pMenu, _("&Debug"));
//    // Add entry in settings menu (outside "plugins")
//    int settingsMenuPos = menuBar->FindMenu(_("&Settings"));
//    if (settingsMenuPos != wxNOT_FOUND)
//    {
//        wxMenu* settingsmenu = menuBar->GetMenu(settingsMenuPos);
//        settingsmenu->Insert(3,idMenuSettings,_("&Debugger"),_("Debugger options"));
//    }
}

void DebuggerGDB::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (!IsAttached())
        return;
    // we 're only interested in editor menus
    // we 'll add a "debug watches" entry only when the debugger is running...
    if (type != mtEditorManager || !menu) return;
    if (!prj) return;
    // Insert toggle breakpoint
    menu->Insert(0,idMenuToggleBreakpoint, _("Toggle breakpoint"));
    // Insert Run to Cursor
    menu->Insert(1,idMenuRunToCursor, _("Run to cursor"));
    menu->Insert(2,wxID_SEPARATOR, _T("-"));

    if (!m_pProcess) return;
    // has to have a word under the caret...
    wxString w = GetEditorWordAtCaret();
    if (w.IsEmpty())
        return;


    // data breakpoint
    menu->Insert(2,idMenuAddDataBreakpoint, wxString::Format(_("Add data breakpoint for '%s'"), w.c_str()));

    wxString s;
    s.Printf(_("Watch '%s'"), w.c_str());
    menu->Insert(3, idMenuDebuggerAddWatch,  s);
}

bool DebuggerGDB::BuildToolBar(wxToolBar* toolBar)
{
    m_pTbar = toolBar;
    /* Loads toolbar using new Manager class functions */
#ifdef implement_debugger_toolbar
    if (!IsAttached() || !toolBar)
        return false;
    wxString my_16x16=Manager::isToolBar16x16(toolBar) ? _T("_16x16") : _T("");
    Manager::AddonToolBar(toolBar,wxString(_T("debugger_toolbar"))+my_16x16);
    toolBar->Realize();
    toolBar->SetInitialSize();
    return true;
#else
    return false;
#endif
}

void DebuggerGDB::Log(const wxString& msg)
{
    if (IsAttached())
        Manager::Get()->GetLogManager()->Log(msg, m_PageIndex);
}

void DebuggerGDB::DebugLog(const wxString& msg)
{
    // gdb debug messages
    if (IsAttached() && m_HasDebugLog)
        Manager::Get()->GetLogManager()->Log(msg, m_DbgPageIndex);
}

wxArrayString& DebuggerGDB::GetSearchDirs(cbProject* prj)
{
    SearchDirsMap::iterator it = m_SearchDirs.find(prj);
    if (it == m_SearchDirs.end())
    {
        // create an empty set for this project
        it = m_SearchDirs.insert(m_SearchDirs.begin(), std::make_pair(prj, wxArrayString()));
    }

    return it->second;
}

RemoteDebuggingMap& DebuggerGDB::GetRemoteDebuggingMap(cbProject* project)
{
    if (!project)
        project = m_pProject;
    ProjectRemoteDebuggingMap::iterator it = m_RemoteDebugging.find(project);
    if (it == m_RemoteDebugging.end())
    {
        // create an empty set for this project
        it = m_RemoteDebugging.insert(m_RemoteDebugging.begin(), std::make_pair(project, RemoteDebuggingMap()));
    }
    return it->second;
}


void DebuggerGDB::OnProjectLoadingHook(cbProject* project, TiXmlElement* elem, bool loading)
{
    wxArrayString& pdirs = GetSearchDirs(project);
    RemoteDebuggingMap& rdprj = GetRemoteDebuggingMap(project);

    if (loading)
    {
        rdprj.clear();

        // Hook called when loading project file.
        TiXmlElement* conf = elem->FirstChildElement("debugger");
        if (conf)
        {
            TiXmlElement* pathsElem = conf->FirstChildElement("search_path");
            while (pathsElem)
            {
                if (pathsElem->Attribute("add"))
                {
                    wxString dir = cbC2U(pathsElem->Attribute("add"));
                    if (pdirs.Index(dir) == wxNOT_FOUND)
                        pdirs.Add(dir);
                }

                pathsElem = pathsElem->NextSiblingElement("search_path");
            }

            TiXmlElement* rdElem = conf->FirstChildElement("remote_debugging");
            while (rdElem)
            {
                wxString targetName = cbC2U(rdElem->Attribute("target"));
                ProjectBuildTarget* bt = project->GetBuildTarget(targetName);

                TiXmlElement* rdOpt = rdElem->FirstChildElement("options");

                if (bt && rdOpt)
                {
                    RemoteDebugging rd;

                    if (rdOpt->Attribute("conn_type"))
                        rd.connType = (RemoteDebugging::ConnectionType)atol(rdOpt->Attribute("conn_type"));
                    if (rdOpt->Attribute("serial_port"))
                        rd.serialPort = cbC2U(rdOpt->Attribute("serial_port"));
                    if (rdOpt->Attribute("serial_baud"))
                        rd.serialBaud = cbC2U(rdOpt->Attribute("serial_baud"));
                    if (rdOpt->Attribute("ip_address"))
                        rd.ip = cbC2U(rdOpt->Attribute("ip_address"));
                    if (rdOpt->Attribute("ip_port"))
                        rd.ipPort = cbC2U(rdOpt->Attribute("ip_port"));
                    if (rdOpt->Attribute("additional_cmds"))
                        rd.additionalCmds = cbC2U(rdOpt->Attribute("additional_cmds"));
                    if (rdOpt->Attribute("additional_cmds_before"))
                        rd.additionalCmdsBefore = cbC2U(rdOpt->Attribute("additional_cmds_before"));
                    if (rdOpt->Attribute("skip_ld_path"))
                        rd.skipLDpath = cbC2U(rdOpt->Attribute("skip_ld_path")) != _T("0");

                    rdprj.insert(rdprj.end(), std::make_pair(bt, rd));
                }
                else
                    Manager::Get()->GetLogManager()->Log(_T("Unknown target in remote_debugging: ") + targetName, m_PageIndex, Logger::warning);

                rdElem = rdElem->NextSiblingElement("remote_debugging");
            }
        }
    }
    else
    {
        // Hook called when saving project file.

        // since rev4332, the project keeps a copy of the <Extensions> element
        // and re-uses it when saving the project (so to avoid losing entries in it
        // if plugins that use that element are not loaded atm).
        // so, instead of blindly inserting the element, we must first check it's
        // not already there (and if it is, clear its contents)
        TiXmlElement* node = elem->FirstChildElement("debugger");
        if (!node)
            node = elem->InsertEndChild(TiXmlElement("debugger"))->ToElement();
        node->Clear();

        if (pdirs.GetCount() > 0)
        {
            for (size_t i = 0; i < pdirs.GetCount(); ++i)
            {
                TiXmlElement* path = node->InsertEndChild(TiXmlElement("search_path"))->ToElement();
                path->SetAttribute("add", cbU2C(pdirs[i]));
            }
        }

        if (rdprj.size())
        {
            for (RemoteDebuggingMap::iterator it = rdprj.begin(); it != rdprj.end(); ++it)
            {
                // valid targets only
                if (!it->first)
                    continue;

                RemoteDebugging& rd = it->second;

                // if no different than defaults, skip it
                if (rd.serialPort.IsEmpty() && rd.ip.IsEmpty() &&
                    rd.additionalCmds.IsEmpty() && rd.additionalCmdsBefore.IsEmpty() &&
                    !rd.skipLDpath)
                {
                    continue;
                }

                TiXmlElement* rdnode = node->InsertEndChild(TiXmlElement("remote_debugging"))->ToElement();
                rdnode->SetAttribute("target", cbU2C(it->first->GetTitle()));

                TiXmlElement* tgtnode = rdnode->InsertEndChild(TiXmlElement("options"))->ToElement();
                tgtnode->SetAttribute("conn_type", (int)rd.connType);
                if (!rd.serialPort.IsEmpty())
                    tgtnode->SetAttribute("serial_port", cbU2C(rd.serialPort));
                if (!rd.serialBaud.IsEmpty())
                    tgtnode->SetAttribute("serial_baud", cbU2C(rd.serialBaud));
                if (!rd.ip.IsEmpty())
                    tgtnode->SetAttribute("ip_address", cbU2C(rd.ip));
                if (!rd.ipPort.IsEmpty())
                    tgtnode->SetAttribute("ip_port", cbU2C(rd.ipPort));
                if (!rd.additionalCmds.IsEmpty())
                    tgtnode->SetAttribute("additional_cmds", cbU2C(rd.additionalCmds));
                if (!rd.additionalCmdsBefore.IsEmpty())
                    tgtnode->SetAttribute("additional_cmds_before", cbU2C(rd.additionalCmdsBefore));
                if (rd.skipLDpath)
                    tgtnode->SetAttribute("skip_ld_path", "1");
            }
        }
    }
}

void DebuggerGDB::DoSwitchToDebuggingLayout()
{
    CodeBlocksLayoutEvent queryEvent(cbEVT_QUERY_VIEW_LAYOUT);
    CodeBlocksLayoutEvent switchEvent(cbEVT_SWITCH_VIEW_LAYOUT, _("Debugging"));

    Manager::Get()->GetLogManager()->DebugLog(F(_("Switching layout to \"%s\""), switchEvent.layout.c_str()));

    // query the current layout
    Manager::Get()->ProcessEvent(queryEvent);
    m_PreviousLayout = queryEvent.layout;

    // switch to debugging layout
    Manager::Get()->ProcessEvent(switchEvent);
}

void DebuggerGDB::DoSwitchToPreviousLayout()
{
    CodeBlocksLayoutEvent switchEvent(cbEVT_SWITCH_VIEW_LAYOUT, m_PreviousLayout);

    Manager::Get()->GetLogManager()->DebugLog(F(_("Switching layout to \"%s\""), !switchEvent.layout.IsEmpty() ? switchEvent.layout.c_str() : wxString(_("Code::Blocks default")).c_str()));

    // switch to previous layout
    Manager::Get()->ProcessEvent(switchEvent);
}

void DebuggerGDB::DoWatches()
{
    if (!m_pProcess)
        return;
    m_State.GetDriver()->UpdateWatches(Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("watch_locals"), true),
                            Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("watch_args"), true),
                            m_pTree);
}

wxString DebuggerGDB::FindDebuggerExecutable(Compiler* compiler)
{
    if (compiler->GetPrograms().DBG.IsEmpty())
        return wxEmptyString;
//    if (!wxFileExists(compiler->GetMasterPath() + wxFILE_SEP_PATH + _T("bin") + wxFILE_SEP_PATH + compiler->GetPrograms().DBG))
//        return wxEmptyString;

    wxString masterPath = compiler->GetMasterPath();
    while (masterPath.Last() == '\\' || masterPath.Last() == '/')
        masterPath.RemoveLast();
    wxString gdb = compiler->GetPrograms().DBG;
    const wxArrayString& extraPaths = compiler->GetExtraPaths();

    wxPathList pathList;
    pathList.Add(masterPath + wxFILE_SEP_PATH + _T("bin"));
    for (unsigned int i = 0; i < extraPaths.GetCount(); ++i)
    {
        if (!extraPaths[i].IsEmpty())
            pathList.Add(extraPaths[i]);
    }
    pathList.AddEnvList(_T("PATH"));
    wxString binPath = pathList.FindAbsoluteValidPath(gdb);
    // it seems, under Win32, the above command doesn't search in paths with spaces...
    // look directly for the file in question in masterPath
    if (binPath.IsEmpty() || !(pathList.Index(wxPathOnly(binPath)) != wxNOT_FOUND))
    {
        if (wxFileExists(masterPath + wxFILE_SEP_PATH + _T("bin") + wxFILE_SEP_PATH + gdb))
            binPath = masterPath + wxFILE_SEP_PATH + _T("bin");
        else if (wxFileExists(masterPath + wxFILE_SEP_PATH + gdb))
            binPath = masterPath;
        else
        {
            for (unsigned int i = 0; i < extraPaths.GetCount(); ++i)
            {
                if (!extraPaths[i].IsEmpty())
                {
                    if (wxFileExists(extraPaths[i] + wxFILE_SEP_PATH + gdb))
                    {
                        binPath = extraPaths[i];
                        break;
                    }
                }
            }
        }
    }

    return binPath;
}

int DebuggerGDB::LaunchProcess(const wxString& cmd, const wxString& cwd)
{
    if (m_pProcess)
        return -1;

    // start the gdb process
    m_pProcess = new PipedProcess((void**)&m_pProcess, this, idGDBProcess, true, cwd);
    Manager::Get()->GetLogManager()->Log(_("Starting debugger: "), m_PageIndex);
    m_Pid = wxExecute(cmd, wxEXEC_ASYNC, m_pProcess);

#ifdef __WXMAC__
    if (m_Pid == -1)
    {
        // Great! We got a fake PID. Time to Go Fish with our "ps" rod:

        m_Pid = 0;
        pid_t mypid = getpid();
        wxString mypidStr;
        mypidStr << mypid;

        long pspid = 0;
        wxString psCmd;
        wxArrayString psOutput;
        wxArrayString psErrors;

        psCmd << wxT("/bin/ps -o ppid,pid,command");
        DebugLog(wxString::Format( _("Executing: %s"), psCmd.c_str()) );
        int result = wxExecute(psCmd, psOutput, psErrors, wxEXEC_SYNC);

        mypidStr << wxT(" ");

        for (int i = 0; i < psOutput.GetCount(); ++i)
        { //  PPID   PID COMMAND
           wxString psLine = psOutput.Item(i);
           if (psLine.StartsWith(mypidStr) && psLine.Contains(wxT("gdb")))
           {
               wxString pidStr = psLine.Mid(mypidStr.Length());
               pidStr = pidStr.BeforeFirst(' ');
               if (pidStr.ToLong(&pspid))
               {
                   m_Pid = pspid;
                   break;
               }
           }
         }

        for (int i = 0; i < psErrors.GetCount(); ++i)
            DebugLog(wxString::Format( _("PS Error:%s"), psErrors.Item(i).c_str()) );
    }
#endif

    if (!m_Pid)
    {
        delete m_pProcess;
        m_pProcess = 0;
        Manager::Get()->GetLogManager()->Log(_("failed"), m_PageIndex);
        return -1;
    }
    else if (!m_pProcess->GetOutputStream())
    {
        delete m_pProcess;
        m_pProcess = 0;
        Manager::Get()->GetLogManager()->Log(_("failed (to get debugger's stdin)"), m_PageIndex);
        return -2;
    }
    else if (!m_pProcess->GetInputStream())
    {
        delete m_pProcess;
        m_pProcess = 0;
        Manager::Get()->GetLogManager()->Log(_("failed (to get debugger's stdout)"), m_PageIndex);
        return -2;
    }
    else if (!m_pProcess->GetErrorStream())
    {
        delete m_pProcess;
        m_pProcess = 0;
        Manager::Get()->GetLogManager()->Log(_("failed (to get debugger's stderr)"), m_PageIndex);
        return -2;
    }
    Manager::Get()->GetLogManager()->Log(_("done"), m_PageIndex);
    return 0;
}

wxString DebuggerGDB::GetDebuggee(ProjectBuildTarget* target)
{
    if (!target)
        return wxEmptyString;

    wxString out;
    switch (target->GetTargetType())
    {
        case ttExecutable:
        case ttConsoleOnly:
            out = UnixFilename(target->GetOutputFilename());
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out); // apply env vars
            Manager::Get()->GetLogManager()->Log(_("Adding file: ") + out, m_PageIndex);
            ConvertToGDBDirectory(out);
            break;

        case ttStaticLib:
        case ttDynamicLib:
            // check for hostapp
            if (target->GetHostApplication().IsEmpty())
            {
                cbMessageBox(_("You must select a host application to \"run\" a library..."));
                return wxEmptyString;
            }
            out = UnixFilename(target->GetHostApplication());
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out); // apply env vars
            Manager::Get()->GetLogManager()->Log(_("Adding file: ") + out, m_PageIndex);
            ConvertToGDBDirectory(out);
            break;
//            // for DLLs, add the DLL's symbols
//            if (target->GetTargetType() == ttDynamicLib)
//            {
//                wxString symbols;
//                out = UnixFilename(target->GetOutputFilename());
//                Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out); // apply env vars
//                msgMan->Log(m_PageIndex, _("Adding symbol file: %s"), out.c_str());
//                ConvertToGDBDirectory(out);
//                QueueCommand(new DbgCmd_AddSymbolFile(this, out));
//            }
//            break;

        default: break;
    }
    return out;
}

bool DebuggerGDB::IsStopped()
{
    return !m_State.HasDriver() || m_State.GetDriver()->IsStopped();
}

bool DebuggerGDB::EnsureBuildUpToDate()
{
    m_WaitingCompilerToFinish = false;

    // compile project/target (if not attaching to a PID)
    if (m_PidToAttach == 0)
    {
        LogManager* msgMan = Manager::Get()->GetLogManager();

        // make sure the target is compiled
        PluginsArray plugins = Manager::Get()->GetPluginManager()->GetCompilerOffers();
        if (plugins.GetCount())
            m_pCompiler = (cbCompilerPlugin*)plugins[0];
        else
            m_pCompiler = 0;
        if (m_pCompiler)
        {
            // is the compiler already running?
            if (m_pCompiler->IsRunning())
            {
                msgMan->Log(_("Compiler in use..."), m_PageIndex);
                msgMan->Log(_("Aborting debugging session"), m_PageIndex);
                cbMessageBox(_("The compiler is currently in use. Aborting debugging session..."), _("Compiler running"), wxICON_WARNING);
                return false;
            }

            msgMan->Log(_("Building to ensure sources are up-to-date"), m_PageIndex);
            m_WaitingCompilerToFinish = true;
            m_pCompiler->Build();
            // now, when the build is finished, DoDebug will be launched in OnCompilerFinished()
        }
    }
    return true;
}

int DebuggerGDB::Debug()
{
    // if already running, return
    if (m_pProcess || m_WaitingCompilerToFinish)
        return 1;

    m_pProject = 0;
    m_NoDebugInfo = false;

    // clear the debug log
    if (m_HasDebugLog)
        m_pDbgLog->Clear();

    m_pTree->GetTree()->DeleteAllItems();

    // switch to the debugging log and clear it
    CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_pLog);
    CodeBlocksLogEvent evtShow(cbEVT_SHOW_LOG_MANAGER);
    Manager::Get()->ProcessEvent(evtSwitch);
    Manager::Get()->ProcessEvent(evtShow);
    m_pLog->Clear();

    // can only debug projects or attach to processes
    ProjectManager* prjMan = Manager::Get()->GetProjectManager();
    cbProject* project = prjMan->GetActiveProject();
    if (!project && m_PidToAttach == 0)
        return 2;

    m_pProject = project;

    // should we build to make sure project is up-to-date?
    if (Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("auto_build"), true))
    {
        // compile project/target (if not attaching to a PID)
        // this will wait for the compiler to finish and then call DoDebug
        if (!EnsureBuildUpToDate())
            return -1;
    }
    else
    {
        m_pCompiler = 0;
        m_WaitingCompilerToFinish = false;
        m_Canceled = false;
    }

    // if not waiting for the compiler, start debugging now
    // but first check if the driver has already been started:
    // if the build process was ultra-fast (i.e. nothing to be done),
    // it may have already called DoDebug() and m_WaitingCompilerToFinish
    // would already be set to false
    // by checking the driver availability, we avoid calling DoDebug
    // a second consecutive time...
    // the same applies for m_Canceled: it is true if DoDebug() was launched but
    // returned an error
    if (!m_WaitingCompilerToFinish && !m_State.HasDriver() && !m_Canceled)
        return DoDebug();

    return 0;
}

int DebuggerGDB::DoDebug()
{
    // set this to true before every error exit point in this function
    m_Canceled = false;

    LogManager* msgMan = Manager::Get()->GetLogManager();
    ProjectManager* prjMan = Manager::Get()->GetProjectManager();

    // this is always called after EnsureBuildUpToDate() so we should display the build result
    CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_pLog);
    CodeBlocksLogEvent evtShow(cbEVT_SHOW_LOG_MANAGER);
    Manager::Get()->ProcessEvent(evtSwitch);
    Manager::Get()->ProcessEvent(evtShow);

    if (m_pCompiler)
    {
        if (m_pCompiler->GetExitCode() != 0)
        {
            msgMan->Log(_("Build failed..."), m_PageIndex);
            msgMan->Log(_("Aborting debugging session"), m_PageIndex);
            cbMessageBox(_("Build failed. Aborting debugging session..."), _("Build failed"), wxICON_WARNING);
            m_Canceled = true;
            return 1;
        }
        msgMan->Log(_("Build succeeded"), m_PageIndex);
    }

    // select the build target to debug
    ProjectBuildTarget* target = 0;
    Compiler* actualCompiler = 0;
    if (m_PidToAttach == 0)
    {
        CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_pLog);
        CodeBlocksLogEvent evtShow(cbEVT_SHOW_LOG_MANAGER);
        Manager::Get()->ProcessEvent(evtSwitch);
        Manager::Get()->ProcessEvent(evtShow);

        msgMan->Log(_("Selecting target: "), m_PageIndex);
        if (!m_pProject->BuildTargetValid(m_ActiveBuildTarget, false))
        {
            int tgtIdx = m_pProject->SelectTarget();
            if (tgtIdx == -1)
            {
                msgMan->Log(_("canceled"), m_PageIndex);
                m_Canceled = true;
                return 3;
            }
            target = m_pProject->GetBuildTarget(tgtIdx);
            m_ActiveBuildTarget = target->GetTitle();
        }
        else
            target = m_pProject->GetBuildTarget(m_ActiveBuildTarget);

        // make sure it's not a commands-only target
        if (target->GetTargetType() == ttCommandsOnly)
        {
            cbMessageBox(_("The selected target is only running pre/post build step commands\n"
                        "Can't debug such a target..."), _("Information"), wxICON_INFORMATION);
            msgMan->Log(_("aborted"), m_PageIndex);
            return 3;
        }
        msgMan->Log(target->GetTitle(), m_PageIndex);

        // find the target's compiler (to see which debugger to use)
        actualCompiler = CompilerFactory::GetCompiler(target ? target->GetCompilerID() : m_pProject->GetCompilerID());
    }
    else
        actualCompiler = CompilerFactory::GetDefaultCompiler();

    if (!actualCompiler)
    {
        wxString msg;
        msg.Printf(_("This %s is configured to use an invalid debugger.\nThe operation failed..."), target ? _("target") : _("project"));
        cbMessageBox(msg, _("Error"), wxICON_ERROR);
        m_Canceled = true;
        return 9;
    }

    // is gdb accessible, i.e. can we find it?
    wxString cmdexe;
    cmdexe = actualCompiler->GetPrograms().DBG;
    cmdexe.Trim();
    cmdexe.Trim(true);
    if(cmdexe.IsEmpty())
    {
        msgMan->Log(_("ERROR: You need to specify a debugger program in the compiler's settings."), m_PageIndex);

        if(platform::windows)
        {
            msgMan->Log(_("(For MinGW compilers, it's 'gdb.exe' (without the quotes))"), m_PageIndex);
            msgMan->Log(_("(For MSVC compilers, it's 'cdb.exe' (without the quotes))"), m_PageIndex);
        }
        else
        {
            msgMan->Log(_("(For GCC compilers, it's 'gdb' (without the quotes))"), m_PageIndex);
        }

        m_Canceled = true;
        return -1;
    }

    // access the gdb executable name
    cmdexe = FindDebuggerExecutable(actualCompiler);
    if (cmdexe.IsEmpty())
    {
        cbMessageBox(_("The debugger executable is not set.\n"
                       "To set it, go to \"Settings/Compiler and debugger\", switch to the \"Programs\" tab,\n"
                       "and select the debugger program."), _("Error"), wxICON_ERROR);
        msgMan->Log(_("Aborted"), m_PageIndex);
        m_Canceled = true;
        return 4;
    }

    // start debugger driver based on target compiler, or default compiler if no target
    if (!m_State.StartDriver(target))
    {
        cbMessageBox(_T("Could not decide which debugger to use!"), _T("Error"), wxICON_ERROR);
        m_Canceled = true;
        return -1;
    }
    m_State.GetDriver()->SetDebugWindows(m_pBacktrace,
                                        m_pDisassembly,
                                        m_pCPURegisters,
                                        m_pExamineMemoryDlg,
                                        m_pThreadsDlg);


    // create gdb launch command
    wxString cmd;

    // prepare the driver
    wxString cmdline;
    if (m_PidToAttach == 0)
    {
        m_State.GetDriver()->ClearDirectories();
        // add other open projects dirs as search dirs (only if option is enabled)
        if (Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("add_other_search_dirs"), false))
        {
            // add as include dirs all open project base dirs
            ProjectsArray* projects = prjMan->GetProjects();
            for (unsigned int i = 0; i < projects->GetCount(); ++i)
            {
                cbProject* it = projects->Item(i);
                // skip if it's THE project (added last)
                if (it == m_pProject)
                    continue;
                AddSourceDir(it->GetBasePath());
                AddSourceDir(it->GetCommonTopLevelPath());
            }
        }
        // now add all per-project user-set search dirs
        wxArrayString& pdirs = GetSearchDirs(m_pProject);
        for (size_t i = 0; i < pdirs.GetCount(); ++i)
        {
            AddSourceDir(pdirs[i]);
        }
        // lastly, add THE project as source dir
        AddSourceDir(m_pProject->GetBasePath());
        AddSourceDir(m_pProject->GetCommonTopLevelPath());

        // switch to output dir
        wxString path = UnixFilename(target->GetWorkingDir());
        if (!path.IsEmpty())
        {
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(path); // apply env vars
            cmd.Clear();
            ConvertToGDBDirectory(path);
            if (path != _T(".")) // avoid silly message "changing to ."
            {
                msgMan->Log(_("Changing directory to: ") + path, m_PageIndex);
                m_State.GetDriver()->SetWorkingDirectory(path);
            }
        }

        if (target && !target->GetExecutionParameters().IsEmpty())
            m_State.GetDriver()->SetArguments(target->GetExecutionParameters());

        // set the file to debug
        // (depends on the target type)
        wxString debuggee = GetDebuggee(target);
        if (debuggee.IsEmpty())
        {
            m_Canceled = true;
            return -3;
        }
        cmdline = m_State.GetDriver()->GetCommandLine(cmdexe, debuggee);
    }
    else // m_PidToAttach != 0
        cmdline = m_State.GetDriver()->GetCommandLine(cmdexe, m_PidToAttach);

    RemoteDebugging* rd = 0;
    RemoteDebuggingMap& rdprj = GetRemoteDebuggingMap();
    RemoteDebuggingMap::iterator it = rdprj.find(target);
    if (it != rdprj.end())
        rd = &it->second;

    wxString oldLibPath; // keep old PATH/LD_LIBRARY_PATH contents
    if (!rd || !rd->skipLDpath)
    {
        wxGetEnv(LIBRARY_ENVVAR, &oldLibPath);

        // setup dynamic linker path
        if (actualCompiler && target)
        {
            wxString newLibPath;
            const wxString libPathSep = platform::windows ? _T(";") : _T(":");
            newLibPath << _T(".") << libPathSep;
            newLibPath << GetStringFromArray(actualCompiler->GetLinkerSearchDirs(target), libPathSep);
            if (newLibPath.SubString(newLibPath.Length() - 1, 1) != libPathSep)
                newLibPath << libPathSep;
            newLibPath << oldLibPath;
            wxSetEnv(LIBRARY_ENVVAR, newLibPath);
            DebugLog(LIBRARY_ENVVAR _T("=") + newLibPath);
        }
    }

    // start the gdb process
    wxString wdir = m_pProject ? m_pProject->GetBasePath() : _T(".");
    DebugLog(_T("Command-line: ") + cmdline);
    DebugLog(_T("Working dir : ") + wdir);
    int ret = LaunchProcess(cmdline, wdir);

    if (!rd || !rd->skipLDpath)
    {
        // restore dynamic linker path
        wxSetEnv(LIBRARY_ENVVAR, oldLibPath);
    }

    if (ret != 0)
    {
        m_Canceled = true;
        return ret;
    }

    wxString out;
    // start polling gdb's output
    m_TimerPollDebugger.Start(20);

    // although I don't really like these do-nothing loops, we must wait a small amount of time
    // for gdb to see if it really started: it may fail to load shared libs or whatever
    // the reason this is added is because I had a case where gdb would error and bail out
    // *while* the driver->Prepare() call was running below and hell broke loose...
    int i = 50;
    while (i)
    {
        wxMilliSleep(1);
        Manager::Yield();
        --i;
    }
    if (!m_State.HasDriver())
        return -1;

    m_State.GetDriver()->Prepare(target, target && target->GetTargetType() == ttConsoleOnly);
    m_State.ApplyBreakpoints();

   #ifdef __WXGTK__
    // create xterm and issue tty "/dev/pts/#" to GDB where
    // # is the tty for the newly created xterm
    m_bIsConsole = (target && target->GetTargetType() == ttConsoleOnly);
    if (m_bIsConsole)
    {
        if (RunNixConsole() > 0 )
        {   wxString gdbTtyCmd;
            gdbTtyCmd << wxT("tty ") << m_ConsoleTty;
            m_State.GetDriver()->QueueCommand(new DebuggerCmd(m_State.GetDriver(), gdbTtyCmd, true));
            DebugLog(wxString::Format( _("Queued:[%s]"), gdbTtyCmd.c_str()) );
        }
    }//if
   #endif//def __WXGTK__

    // Don't issue 'run' if attaching to a process (Bug #1391904)
    if (m_PidToAttach == 0)
        m_State.GetDriver()->Start(m_BreakOnEntry);

    // switch to the user-defined layout for debugging
    if (m_pProcess)
        DoSwitchToDebuggingLayout();

    return 0;
} // Debug

void DebuggerGDB::AddSourceDir(const wxString& dir)
{
    if (!m_State.HasDriver() || dir.IsEmpty())
        return;
    wxString filename = dir;
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(filename); // apply env vars
    Manager::Get()->GetLogManager()->Log(_("Adding source dir: ") + filename, m_PageIndex);
    ConvertToGDBDirectory(filename, _T(""), false);
    m_State.GetDriver()->AddDirectory(filename);
}

// static
void DebuggerGDB::StripQuotes(wxString& str)
{
    if (str.GetChar(0) == _T('\"') && str.GetChar(str.Length() - 1) == _T('\"'))
            str = str.Mid(1, str.Length() - 2);
}

// static
void DebuggerGDB::ConvertToGDBFriendly(wxString& str)
{
    if (str.IsEmpty())
        return;

    str = UnixFilename(str);
    while (str.Replace(_T("\\"), _T("/")))
        ;
    while (str.Replace(_T("//"), _T("/")))
        ;
//    str.Replace("/", "//");
    if (str.Find(_T(' ')) != -1 && str.GetChar(0) != _T('"'))
        str = _T("\"") + str + _T("\"");
}

// static
void DebuggerGDB::ConvertToGDBFile(wxString& str)
{
    wxFileName fname = str;
    str = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    DebuggerGDB::ConvertToGDBDirectory(str);
    str << fname.GetFullName();
}

// static
//if relative == false, try to leave as an absolute path
void DebuggerGDB::ConvertToGDBDirectory(wxString& str, wxString base, bool relative)
{
    if (str.IsEmpty())
        return;

    ConvertToGDBFriendly(str);
    ConvertToGDBFriendly(base);
    StripQuotes(str);
    StripQuotes(base);

    if(platform::windows)
    {
        int ColonLocation = str.Find(_T(':'));
        wxChar buf[255];
        if(ColonLocation != -1)
        {
            //If can, get 8.3 name for path (Windows only)
            if (str.Contains(_T(' '))) // only if has spaces
            {
                GetShortPathName(str.c_str(), buf, 255);
                str = buf;
            }
        }
        else if(!base.IsEmpty() && str.GetChar(0) != _T('/'))
        {
            if(base.GetChar(base.Length()) == _T('/')) base = base.Mid(0, base.Length() - 2);
            while(!str.IsEmpty())
            {
                base += _T("/") + str.BeforeFirst(_T('/'));
                if(str.Find(_T('/')) != -1) str = str.AfterFirst(_T('/'));
                else str.Clear();
            }
            if (base.Contains(_T(' '))) // only if has spaces
            {
                GetShortPathName(base.c_str(), buf, 255);
                str = buf;
            }
        }

        if(ColonLocation == -1 || base.IsEmpty())
            relative = false;        //Can't do it
    }
    else
    {
        if((str.GetChar(0) != _T('/') && str.GetChar(0) != _T('~')) || base.IsEmpty())
            relative = false;
    }

    if(relative)
    {
        if(platform::windows)
        {
            if(str.Find(_T(':')) != -1) str = str.Mid(str.Find(_T(':')) + 2, str.Length());
            if(base.Find(_T(':')) != -1) base = base.Mid(base.Find(_T(':')) + 2, base.Length());
        }
        else
        {
            if(str.GetChar(0) == _T('/')) str = str.Mid(1, str.Length());
            else if(str.GetChar(0) == _T('~')) str = str.Mid(2, str.Length());
            if(base.GetChar(0) == _T('/')) base = base.Mid(1, base.Length());
            else if(base.GetChar(0) == _T('~')) base = base.Mid(2, base.Length());
        }

        while(!base.IsEmpty() && !str.IsEmpty())
        {
            if(str.BeforeFirst(_T('/')) == base.BeforeFirst(_T('/')))
            {
                if(str.Find(_T('/')) == -1) str.Clear();
                else str = str.AfterFirst(_T('/'));

                if(base.Find(_T('/')) == -1) base.Clear();
                else base = base.AfterFirst(_T('/'));
            }
            else break;
        }
        while (!base.IsEmpty())
        {
            str = _T("../") + str;
            if(base.Find(_T('/')) == -1) base.Clear();
            else base = base.AfterFirst(_T('/'));
        }
    }
    ConvertToGDBFriendly(str);
}

void DebuggerGDB::SendCommand(const wxString& cmd)
{
//    Log(cmd);
    if (!m_pProcess || !IsStopped())
        return;
    if (m_HasDebugLog)
        Manager::Get()->GetLogManager()->Log(_T("> ") + cmd, m_DbgPageIndex);
//    m_QueueBusy = true;
    m_pProcess->SendString(cmd);
}

void DebuggerGDB::RunCommand(int cmd)
{
    // just check for the process
    if (!m_pProcess)
        return;

    switch (cmd)
    {
        case CMD_CONTINUE:
            ClearActiveMarkFromAllEditors();
            if (m_State.HasDriver())
            {
                Manager::Get()->GetLogManager()->Log(_("Continuing..."), m_PageIndex);
                m_State.GetDriver()->Continue();
            }
//            QueueCommand(new DebuggerCmd(this, _T("cont")));
            break;

        case CMD_STEP:
            ClearActiveMarkFromAllEditors();
            if (m_State.HasDriver())
                m_State.GetDriver()->Step();
//            QueueCommand(new DebuggerCmd(this, _T("next")));
            break;

        case CMD_STEP_INSTR:
            ClearActiveMarkFromAllEditors();
            if (!IsWindowReallyShown(m_pDisassembly))
            {
                // first time users should have some help from us ;)
                Disassemble();
            }
            if (m_State.HasDriver())
                m_State.GetDriver()->StepInstruction();
//            QueueCommand(new DebuggerCmd(this, _T("nexti")));
            break;

        case CMD_STEPIN:
            ClearActiveMarkFromAllEditors();
            if (m_State.HasDriver())
                m_State.GetDriver()->StepIn();
//            QueueCommand(new DebuggerCmd(this, _T("step")));
            break;

        case CMD_STEPOUT:
            ClearActiveMarkFromAllEditors();
            if (m_State.HasDriver())
                m_State.GetDriver()->StepOut();
//            QueueCommand(new DebuggerCmd(this, _T("finish")));
            break;

        case CMD_STOP:
            ClearActiveMarkFromAllEditors();
            if (m_State.HasDriver())
                m_State.GetDriver()->Stop();
//            QueueCommand(new DebuggerCmd(this, _T("quit")));
            break;

        case CMD_BACKTRACE:
//            Manager::Get()->GetLogManager()->Log(m_PageIndex, "Running back-trace...");
            if (m_State.HasDriver())
                m_State.GetDriver()->Backtrace();
            break;

        case CMD_DISASSEMBLE:
        {
//            Manager::Get()->GetLogManager()->Log(m_PageIndex, "Disassembling...");
            if (m_State.HasDriver())
                m_State.GetDriver()->Disassemble();
            break;
        }

        case CMD_REGISTERS:
        {
//            Manager::Get()->GetLogManager()->Log(m_PageIndex, "Displaying registers...");
            if (m_State.HasDriver())
                m_State.GetDriver()->CPURegisters();
            break;
        }

        case CMD_MEMORYDUMP:
        {
            if (m_State.HasDriver())
                m_State.GetDriver()->MemoryDump();
        }

        case CMD_RUNNINGTHREADS:
        {
            if (m_State.HasDriver())
                m_State.GetDriver()->RunningThreads();
        }

        default: break;
    }
}

bool DebuggerGDB::AddBreakpoint(const wxString& file, int line)
{
    if (!IsStopped())
        return false;
    m_State.AddBreakpoint(file, line, false);
    if (m_pBreakpointsWindow)
        m_pBreakpointsWindow->Refresh();
    return true;
}

bool DebuggerGDB::AddBreakpoint(const wxString& functionSignature)
{
    if (!IsStopped())
        return false;
    m_State.AddBreakpoint(wxEmptyString, -1, false, functionSignature);
    if (m_pBreakpointsWindow)
        m_pBreakpointsWindow->Refresh();
    return true;
}

bool DebuggerGDB::RemoveBreakpoint(const wxString& file, int line)
{
    if (!IsStopped())
        return false;
    m_State.RemoveBreakpoint(file, line);
    if (m_pBreakpointsWindow)
        m_pBreakpointsWindow->Refresh();
    return true;
}

bool DebuggerGDB::RemoveBreakpoint(const wxString& functionSignature)
{
//    if (!IsStopped())
        return false;
//    m_State.RemoveBreakpoint(wxEmptyString, event.GetInt());
//    if (m_pBreakpointsWindow)
//        m_pBreakpointsWindow->Refresh();
//    return true;
}

bool DebuggerGDB::RemoveAllBreakpoints(const wxString& file)
{
    if (!IsStopped())
        return false;
    m_State.RemoveAllBreakpoints(file);
    if (m_pBreakpointsWindow)
        m_pBreakpointsWindow->Refresh();
    return true;
}

void DebuggerGDB::EditorLinesAddedOrRemoved(cbEditor* editor, int startline, int lines)
{
    // here we keep the breakpoints in sync with the editors
    // (whenever lines are added or removed)

    if (!editor || lines == 0)
        return;

    if (lines < 0)
    {
        // removed lines
        // make "lines" positive, for easier reading below
        lines = -lines;

        int endline = startline + lines - 1;
        // remove file's breakpoints in deleted range
        m_State.RemoveBreakpointsRange(editor->GetFilename(), startline, endline);

        // shift the rest of file's breakpoints up by "lines"
        m_State.ShiftBreakpoints(editor->GetFilename(), endline + 1, -lines);

        // special case:
        // when deleting a block of lines, if these lines contain at least one marker,
        // one marker is retained at the cursor position.
        // In our case here, this means that all breakpoints will be deleted in the range
        // but one "orphan" breakpoint (i.e. editor mark only, no actual breakpoint behind it)
        // will be visible on the line with the cursor.
        //
        // If we really have an "orphan", we remove it.
        bool is_orphan = m_State.HasBreakpoint(editor->GetFilename(), endline - lines + 1) == -1;
        if (is_orphan)
            editor->RemoveBreakpoint(endline - lines + 1, false);
    }
    else
    {
        // mimic scintilla's behaviour regarding moving a marker (starts from the next line)
        startline += 1;
        // just shift file's breakpoints down by "lines"
        m_State.ShiftBreakpoints(editor->GetFilename(), startline, lines);
    }

    if (m_pBreakpointsWindow)
        m_pBreakpointsWindow->Refresh();
}

void DebuggerGDB::Registers()
{
    // show it
    CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
    evt.pWindow = m_pCPURegisters;
    Manager::Get()->ProcessEvent(evt);

    RunCommand(CMD_REGISTERS);
}

void DebuggerGDB::Disassemble()
{
    // show it
    CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
    evt.pWindow = m_pDisassembly;
    Manager::Get()->ProcessEvent(evt);

    RunCommand(CMD_DISASSEMBLE);
}

void DebuggerGDB::Backtrace()
{
    m_pBacktrace->Clear();

    // show it
    CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
    evt.pWindow = m_pBacktrace;
    Manager::Get()->ProcessEvent(evt);

    RunCommand(CMD_BACKTRACE);
}

void DebuggerGDB::MemoryDump()
{
    m_pExamineMemoryDlg->Clear();

    // show it
    CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
    evt.pWindow = m_pExamineMemoryDlg;
    Manager::Get()->ProcessEvent(evt);

    RunCommand(CMD_MEMORYDUMP);
}

void DebuggerGDB::RunningThreads()
{
    m_pThreadsDlg->Clear();

    // show it
    CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
    evt.pWindow = m_pThreadsDlg;
    Manager::Get()->ProcessEvent(evt);

    RunCommand(CMD_RUNNINGTHREADS);
}

void DebuggerGDB::Continue()
{
    RunCommand(CMD_CONTINUE);
}

void DebuggerGDB::Next()
{
    RunCommand(CMD_STEP);
}

void DebuggerGDB::NextInstr()
{
    RunCommand(CMD_STEP_INSTR);
}

void DebuggerGDB::Step()
{
    RunCommand(CMD_STEPIN);
}

bool DebuggerGDB::Validate(const wxString& line, const char cb)
{
    bool bResult = false;

    int bep = line.Find(cb)+1;
    int scs = line.Find(_T('\''))+1;
    int sce = line.Find(_T('\''),true)+1;
    int dcs = line.Find(_T('"'))+1;
    int dce = line.Find(_T('"'),true)+1;
    //No single and double quote
    if(!scs && !sce && !dcs && !dce) bResult = true;
    //No single/double quote in pair
    if(!(sce-scs) && !(dce-dcs)) bResult = true;
    //Outside of single quote
    if((sce-scs) && ((bep < scs)||(bep >sce))) bResult = true;
    //Outside of double quote
    if((dce-dcs) && ((bep < dcs)||(bep >dce))) bResult = true;

    return bResult;
}

void DebuggerGDB::StepOut()
{
    RunCommand(CMD_STEPOUT);
}

void DebuggerGDB::RunToCursor()
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;
    wxString lb = ed->GetControl()->GetLine(ed->GetControl()->GetCurrentLine());
    m_State.AddBreakpoint(ed->GetFilename(), ed->GetControl()->GetCurrentLine(), true, lb);

    if (m_pProcess)
        Continue();
    else
        Debug();
}

void DebuggerGDB::ToggleBreakpoint()
{
//    ClearActiveMarkFromAllEditors();
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;
    ed->ToggleBreakpoint();
}

void DebuggerGDB::AddDataBreakpoint()
{
    DataBreakpointDlg dlg(0, -1);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        int sel = dlg.GetSelection();
        m_State.AddBreakpoint(GetEditorWordAtCaret(), sel != 1, sel != 0);
        if (m_pBreakpointsWindow)
            m_pBreakpointsWindow->Refresh();
    }
}

void DebuggerGDB::Break()
{
    // m_Process is PipedProcess I/O; m_Pid is debugger pid
    if (m_pProcess && m_Pid && !IsStopped())
    {
        long pid = m_State.GetDriver()->GetChildPID();
        if (pid <= 0)
            pid = m_Pid; // try poking gdb directly
    #ifndef __WXMSW__
        // non-windows gdb can interrupt the running process. yay!
        if (pid <= 0) // look out for the "fake" PIDs (killall)
            cbMessageBox(_("Unable to stop the debug process!"), _("Error"), wxOK | wxICON_WARNING);
        else
            wxKill(pid, wxSIGINT);
    #else
        // windows gdb can interrupt the running process too. yay!
        bool done = false;
        if (DebugBreakProcessFunc && pid > 0)
        {
            Log(_("Trying to pause the running process..."));
            HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
            if (proc)
            {
                DebugBreakProcessFunc(proc); // yay!
                CloseHandle(proc);
                done = true;
            }
            else
                Log(_("Failed."));
        }
    #endif
    }
}

void DebuggerGDB::Stop()
{
    // m_Process is PipedProcess I/O; m_Pid is debugger pid
    if (m_pProcess && m_Pid)
    {
        if (!IsStopped())
        {
            long pid = m_State.GetDriver()->GetChildPID();
            if (pid <= 0) // look out for the "fake" PIDs (killall)
            {
                cbMessageBox(_("Unable to stop the debug process!"), _("Error"), wxOK | wxICON_WARNING);
                return;
            }
            else
            {
                m_pProcess->CloseOutput();
                m_pProcess->Kill(pid, wxSIGKILL);
            }
        }
        RunCommand(CMD_STOP);
        m_pProcess->CloseOutput();
    }
}

void DebuggerGDB::ParseOutput(const wxString& output)
{
    if (!output.IsEmpty() && m_State.HasDriver())
    {
        m_State.GetDriver()->ParseOutput(output);
    }
}

void DebuggerGDB::BringAppToFront()
{
    wxWindow* app = Manager::Get()->GetAppWindow();
    if (app)
        app->Raise();
}

void DebuggerGDB::ClearActiveMarkFromAllEditors()
{
    EditorManager* edMan = Manager::Get()->GetEditorManager();
//  Plugins are destroyed prior to EditorManager, so this is guaranteed to be valid at all times
//    if (!edMan)
//        return;
    for (int i = 0; i < edMan->GetEditorsCount(); ++i)
    {
        cbEditor* ed = edMan->GetBuiltinEditor(i);
        if (ed)
            ed->SetDebugLine(-1);
    }
}

void DebuggerGDB::SyncEditor(const wxString& filename, int line, bool setMarker)
{
    if (setMarker)
        ClearActiveMarkFromAllEditors();
    FileType ft = FileTypeOf(filename);
    if (ft != ftSource && ft != ftHeader && ft != ftResource)
        return; // don't try to open unknown files
    cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    ProjectFile* f = project ? project->GetFileByFilename(filename, false, true) : 0;
    wxFileName fname(filename);
    if (project && fname.IsRelative())
        fname.MakeAbsolute(project->GetBasePath());
    // gdb can't work with spaces in filenames, so we have passed it the shorthand form (C:\MYDOCU~1 etc)
    // revert this change now so the file can be located and opened...
    // we do this by calling GetLongPath()
    cbEditor* ed = Manager::Get()->GetEditorManager()->Open(fname.GetLongPath());
    if (ed)
    {
        ed->Show(true);
        if (f && !ed->GetProjectFile())
            ed->SetProjectFile(f);
        ed->GotoLine(line - 1, false);
        if (setMarker)
            ed->SetDebugLine(line - 1);
    }
    else
        Log(_("Cannot open file: ") + fname.GetLongPath());
}

wxString DebuggerGDB::GetEditorWordAtCaret()
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return _T("");
    int start = ed->GetControl()->WordStartPosition(ed->GetControl()->GetCurrentPos(), true);
    int end = ed->GetControl()->WordEndPosition(ed->GetControl()->GetCurrentPos(), true);
    return ed->GetControl()->GetTextRange(start, end);
}

// events

void DebuggerGDB::OnUpdateUI(wxUpdateUIEvent& event)
{
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    bool en = (prj && !prj->GetCurrentlyCompilingTarget()) || m_PidToAttach != 0;
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    wxMenuBar* mbar = Manager::Get()->GetAppFrame()->GetMenuBar();
    bool stopped = IsStopped();
    if (mbar)
    {
        mbar->Enable(idMenuDebug, !m_pProcess && en);
        mbar->Enable(idMenuContinue, m_pProcess && en && stopped);
        mbar->Enable(idMenuNext, m_pProcess && en && stopped);
        mbar->Enable(idMenuNextInstr, m_pProcess && en && stopped);
        mbar->Enable(idMenuStep, en && stopped);
        mbar->Enable(idMenuStepOut, m_pProcess && en && stopped);
        mbar->Enable(idMenuRunToCursor, en && ed && stopped);
        mbar->Enable(idMenuToggleBreakpoint, en && ed && stopped);
        mbar->Enable(idMenuRemoveAllBreakpoints, en && ed && stopped);
        mbar->Enable(idMenuSendCommandToGDB, m_pProcess && stopped);
        mbar->Enable(idMenuAddSymbolFile, m_pProcess && stopped);
        mbar->Enable(idMenuStop, m_pProcess && en);
        mbar->Enable(idMenuAttachToProcess, !m_pProcess);
        mbar->Enable(idMenuDetach, m_pProcess && m_PidToAttach != 0);

        mbar->Enable(idMenuInfoFrame, m_pProcess && stopped);
        mbar->Enable(idMenuInfoDLL, m_pProcess && stopped);
        mbar->Enable(idMenuInfoFiles, m_pProcess && stopped);
        mbar->Enable(idMenuInfoFPU, m_pProcess && stopped);
        mbar->Enable(idMenuInfoSignals, m_pProcess && stopped);

        mbar->Check(idMenuThreads, IsWindowReallyShown(m_pThreadsDlg));
        mbar->Check(idMenuMemory, IsWindowReallyShown(m_pExamineMemoryDlg));
        mbar->Check(idMenuBacktrace, IsWindowReallyShown(m_pBacktrace));
        mbar->Check(idMenuCPU, IsWindowReallyShown(m_pDisassembly));
        mbar->Check(idMenuWatches, IsWindowReallyShown(m_pTree));
        mbar->Check(idMenuRegisters, IsWindowReallyShown(m_pCPURegisters));
        mbar->Check(idMenuBreakpoints, IsWindowReallyShown(m_pBreakpointsWindow));
    }

    #ifdef implement_debugger_toolbar
    wxToolBar* tbar = m_pTbar;//Manager::Get()->GetAppWindow()->GetToolBar();
    tbar->EnableTool(idMenuDebug, (!m_pProcess || stopped) && en);
    tbar->EnableTool(idMenuRunToCursor, en && ed && stopped);
    tbar->EnableTool(idMenuNext, m_pProcess && en && stopped);
    tbar->EnableTool(idMenuNextInstr, m_pProcess && en && stopped);
    tbar->EnableTool(idMenuStep, en && stopped);
    tbar->EnableTool(idMenuStepOut, m_pProcess && en && stopped);
    tbar->EnableTool(idMenuStop, m_pProcess && en);
    tbar->EnableTool(idDebuggerToolInfo, m_pProcess && en);
    #endif

    // allow other UpdateUI handlers to process this event
    // *very* important! don't forget it...
    event.Skip();
}

void DebuggerGDB::OnDebug(wxCommandEvent& event)
{
    if (!m_pProcess)
        Debug();
    else
    {
        if (IsStopped())
            Continue();
    }
}

void DebuggerGDB::OnContinue(wxCommandEvent& event)
{
    Continue();
}

void DebuggerGDB::OnNext(wxCommandEvent& event)
{
    Next();
}

void DebuggerGDB::OnNextInstr(wxCommandEvent& event)
{
    NextInstr();
}

void DebuggerGDB::OnStep(wxCommandEvent& event)
{
    if (!m_pProcess)
    {
        m_BreakOnEntry = true;
        Debug();
        m_BreakOnEntry = false;
    }
    else
        Step();
}

void DebuggerGDB::OnStepOut(wxCommandEvent& event)
{
    StepOut();
}

void DebuggerGDB::OnRunToCursor(wxCommandEvent& event)
{
    RunToCursor();
}

void DebuggerGDB::OnToggleBreakpoint(wxCommandEvent& event)
{
    ToggleBreakpoint();
}

void DebuggerGDB::OnRemoveAllBreakpoints(wxCommandEvent& event)
{
    if (IsStopped()) // Code from BreakpointsDlg has been used
    {
        while (m_State.GetBreakpoints().GetCount())
        {
            // if not valid breakpoint, continue with the next one
            DebuggerBreakpoint* bp = m_State.GetBreakpoints()[0];
            if (!bp)
                continue;
            cbEditor* ed = Manager::Get()->GetEditorManager()->IsBuiltinOpen(bp->filenameAsPassed);
            if (ed)
                ed->RemoveBreakpoint(bp->line, false);
            m_State.RemoveBreakpoint(0);
        }
    }
}

void DebuggerGDB::OnAddDataBreakpoint(wxCommandEvent& event)
{
    AddDataBreakpoint();
}

void DebuggerGDB::OnStop(wxCommandEvent& event)
{
    if (!IsStopped())
        Break();
    else
        Stop();
}

void DebuggerGDB::OnSendCommandToGDB(wxCommandEvent& event)
{
    wxString cmd = wxGetTextFromUser(_("Enter command for GDB:"), _("Send command to GDB:"), m_LastCmd);
    if (cmd.IsEmpty())
        return;
    m_LastCmd = cmd;
    m_State.GetDriver()->QueueCommand(new DebuggerCmd(m_State.GetDriver(), cmd, true));
}

void DebuggerGDB::OnAddSymbolFile(wxCommandEvent& event)
{
    wxString file = wxFileSelector(_("Choose file to read symbols from"),
                                    _T(""),
                                    _T(""),
                                    _T(""),
                                    _("Executables and libraries|*.exe;*.dll"),
                                    wxOPEN | wxFILE_MUST_EXIST | compatibility::wxHideReadonly);
    if (file.IsEmpty())
        return;
//    Manager::Get()->GetLogManager()->Log(m_PageIndex, _("Adding symbol file: %s"), file.c_str());
    ConvertToGDBDirectory(file);
//    QueueCommand(new DbgCmd_AddSymbolFile(this, file));
}

void DebuggerGDB::OnBacktrace(wxCommandEvent& event)
{
    // show it
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pBacktrace;
    Manager::Get()->ProcessEvent(evt);

    if (event.IsChecked())
        Backtrace();
}

void DebuggerGDB::OnDisassemble(wxCommandEvent& event)
{
    // show it
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pDisassembly;
    Manager::Get()->ProcessEvent(evt);

    if (event.IsChecked())
        Disassemble();
}

void DebuggerGDB::OnRegisters(wxCommandEvent& event)
{
    // show it
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pCPURegisters;
    Manager::Get()->ProcessEvent(evt);

    if (event.IsChecked())
        Registers();
}

void DebuggerGDB::OnViewWatches(wxCommandEvent& event)
{
    // show it
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pTree;
    Manager::Get()->ProcessEvent(evt);

    if (event.IsChecked())
        DoWatches();
}

void DebuggerGDB::OnBreakpoints(wxCommandEvent& event)
{
    // show it
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pBreakpointsWindow;
    Manager::Get()->ProcessEvent(evt);
}

void DebuggerGDB::OnExamineMemory(wxCommandEvent& event)
{
    // show it
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pExamineMemoryDlg;
    Manager::Get()->ProcessEvent(evt);

    if (event.IsChecked())
        MemoryDump();
}

void DebuggerGDB::OnRunningThreads(wxCommandEvent& event)
{
    // show it
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pThreadsDlg;
    Manager::Get()->ProcessEvent(evt);

    if (event.IsChecked())
        RunningThreads();
}

void DebuggerGDB::OnEditWatches(wxCommandEvent& event)
{
    WatchesArray watches = m_pTree->GetWatches();
    EditWatchesDlg dlg(watches);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        m_pTree->SetWatches(watches);
    }
}

void DebuggerGDB::OnDebugWindows(wxCommandEvent& event)
{
    wxMenu m;

    m.AppendCheckItem(idMenuBreakpoints,    _("Breakpoints"));
    m.AppendCheckItem(idMenuBacktrace,      _("Call stack"));
    m.AppendCheckItem(idMenuRegisters,      _("CPU Registers"));
    m.AppendCheckItem(idMenuCPU,            _("Disassembly"));
    m.AppendCheckItem(idMenuMemory,         _("Memory dump"));
    m.AppendCheckItem(idMenuThreads,        _("Running threads"));
    m.AppendCheckItem(idMenuWatches,        _("Watches"));

    m.Check(idMenuBreakpoints,  IsWindowReallyShown(m_pBreakpointsWindow));
    m.Check(idMenuBacktrace,    IsWindowReallyShown(m_pBacktrace));
    m.Check(idMenuRegisters,    IsWindowReallyShown(m_pCPURegisters));
    m.Check(idMenuCPU,          IsWindowReallyShown(m_pDisassembly));
    m.Check(idMenuCPU,          IsWindowReallyShown(m_pDisassembly));
    m.Check(idMenuMemory,       IsWindowReallyShown(m_pExamineMemoryDlg));
    m.Check(idMenuThreads,      IsWindowReallyShown(m_pThreadsDlg));
    m.Check(idMenuWatches,      IsWindowReallyShown(m_pTree));

    Manager::Get()->GetAppWindow()->PopupMenu(&m);
}

void DebuggerGDB::OnToolInfo(wxCommandEvent& event)
{
    wxMenu m;
    m.Append(idMenuInfoFrame,   _("Current stack frame"));
    m.Append(idMenuInfoDLL,     _("Loaded libraries"));
    m.Append(idMenuInfoFiles,   _("Targets and files"));
    m.Append(idMenuInfoFPU,     _("FPU status"));
    m.Append(idMenuInfoSignals, _("Signal handling"));
    Manager::Get()->GetAppWindow()->PopupMenu(&m);
}

void DebuggerGDB::OnInfoFrame(wxCommandEvent& event)
{
    if (m_State.HasDriver())
    {
        m_State.GetDriver()->InfoFrame();
    }
}

void DebuggerGDB::OnInfoDLL(wxCommandEvent& event)
{
    if (m_State.HasDriver())
    {
        m_State.GetDriver()->InfoDLL();
    }
}

void DebuggerGDB::OnInfoFiles(wxCommandEvent& event)
{
    if (m_State.HasDriver())
    {
        m_State.GetDriver()->InfoFiles();
    }
}

void DebuggerGDB::OnInfoFPU(wxCommandEvent& event)
{
    if (m_State.HasDriver())
    {
        m_State.GetDriver()->InfoFPU();
    }
}

void DebuggerGDB::OnInfoSignals(wxCommandEvent& event)
{
    if (m_State.HasDriver())
    {
        m_State.GetDriver()->InfoSignals();
    }
}

void DebuggerGDB::OnGDBOutput(wxCommandEvent& event)
{
    wxString msg = event.GetString();
    if (!msg.IsEmpty())
    {
//        Manager::Get()->GetLogManager()->Log(m_PageIndex, _T("O>>> %s"), msg.c_str());
        ParseOutput(msg);
    }
}

void DebuggerGDB::OnGDBError(wxCommandEvent& event)
{
    wxString msg = event.GetString();
    if (!msg.IsEmpty())
    {
//        Manager::Get()->GetLogManager()->Log(m_PageIndex, _T("E>>> %s"), msg.c_str());
        ParseOutput(msg);
    }
}

void DebuggerGDB::OnGDBTerminated(wxCommandEvent& event)
{
    m_PidToAttach = 0;

    m_TimerPollDebugger.Stop();
    m_LastExitCode = event.GetInt();
    //the process deletes itself
//    m_pProcess = 0L;

    ClearActiveMarkFromAllEditors();
    m_State.StopDriver();
    Manager::Get()->GetLogManager()->Log(F(_("Debugger finished with status %d"), m_LastExitCode), m_PageIndex);

    if (m_NoDebugInfo)
    {
        cbMessageBox(_("This project/target has no debugging info."
                        "Please change this in the project's build options and retry..."),
                        _("Error"),
                        wxICON_STOP);
    }

    // switch to the user-defined layout when finished debugging
    DoSwitchToPreviousLayout();

    #ifdef __WXGTK__
    // kill any linux console
    if ( m_bIsConsole && (m_nConsolePid > 0) )
    {
        ::wxKill(m_nConsolePid);
        m_nConsolePid = 0;
        m_bIsConsole = false;
    }
    #endif
}

void DebuggerGDB::OnBreakpointAdd(CodeBlocksEvent& event)
{
//    EditorBase* base = event.GetEditor();
//    cbEditor* ed = base && base->IsBuiltinEditor() ? static_cast<cbEditor*>(base) : 0;
//    wxString lb;
//    if (ed)
//        lb = ed->GetControl()->GetLine(event.GetInt());
//    m_State.AddBreakpoint(event.GetString(), event.GetInt(), false, lb);
//    if (m_pBreakpointsWindow)
//        m_pBreakpointsWindow->Refresh();
}

void DebuggerGDB::OnBreakpointEdit(CodeBlocksEvent& event)
{
    int idx = m_State.HasBreakpoint(event.GetString(), event.GetInt());
    DebuggerBreakpoint* bp = m_State.GetBreakpoint(idx);
    if (!bp)
        return;
    EditBreakpointDlg dlg(bp);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        m_State.ResetBreakpoint(idx);
    }
    if (m_pBreakpointsWindow)
        m_pBreakpointsWindow->Refresh();
}

void DebuggerGDB::OnBreakpointDelete(CodeBlocksEvent& event)
{
//    m_State.RemoveBreakpoint(event.GetString(), event.GetInt());
//    if (m_pBreakpointsWindow)
//        m_pBreakpointsWindow->Refresh();
}

void DebuggerGDB::OnValueTooltip(CodeBlocksEvent& event)
{
    event.Skip();
    if (!m_pProcess || !IsStopped())
        return;
    if (!Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("eval_tooltip"), false))
        return;

    EditorBase* base = event.GetEditor();
    cbEditor* ed = base && base->IsBuiltinEditor() ? static_cast<cbEditor*>(base) : 0;
    if (!ed)
        return;

    int style = event.GetInt();
    if (style != wxSCI_C_DEFAULT && style != wxSCI_C_OPERATOR && style != wxSCI_C_IDENTIFIER)
        return;

    wxPoint pt;
    pt.x = event.GetX();
    pt.y = event.GetY();
    int pos = ed->GetControl()->PositionFromPoint(pt);
    int start = ed->GetControl()->WordStartPosition(pos, true);
    int end = ed->GetControl()->WordEndPosition(pos, true);
    wxString token;
    if (start >= ed->GetControl()->GetSelectionStart() &&
        end <= ed->GetControl()->GetSelectionEnd())
    {
        token = ed->GetControl()->GetSelectedText();
    }
    else
        token = ed->GetControl()->GetTextRange(start,end);

    if (!token.IsEmpty())
    {
        pt = ed->GetControl()->PointFromPosition(start);
        pt = ed->GetControl()->ClientToScreen(pt);
        m_EvalRect.x = pt.x;
        m_EvalRect.y = pt.y;
        pt = ed->GetControl()->PointFromPosition(end);
        pt = ed->GetControl()->ClientToScreen(pt);
        m_EvalRect.width = pt.x - m_EvalRect.x;
        m_EvalRect.height = (pt.y + ed->GetControl()->GetCharHeight()) - m_EvalRect.y;
        m_LastEval = token;
        m_State.GetDriver()->EvaluateSymbol(token, m_EvalRect);
    }
}

void DebuggerGDB::OnEditorOpened(CodeBlocksEvent& event)
{
    // when an editor opens, look if we have breakpoints for it
    // and notify it...
    EditorBase* ed = event.GetEditor();
    wxFileName bpFileName, edFileName;
    if (ed)
    {
        for (unsigned int i = 0; i < m_State.GetBreakpoints().GetCount(); ++i)
        {
            DebuggerBreakpoint* bp = m_State.GetBreakpoints()[i];
            bpFileName.Assign(bp->filename);
            edFileName.Assign(ed->GetFilename());
            bpFileName.Normalize();
            edFileName.Normalize();
            if (bpFileName.GetFullPath().Matches(edFileName.GetFullPath()))
                ed->ToggleBreakpoint(bp->line, false);
        }
        // Now check and highlight the active line under debugging
        if (m_State.HasDriver())
        {
            const Cursor& line_cursor = m_State.GetDriver()->GetCursor();
            wxFileName dbgFileName(line_cursor.file);
            dbgFileName.Normalize();
            if (dbgFileName.GetFullPath().IsSameAs(edFileName.GetFullPath())
                && line_cursor.line != -1)
                ed->SetDebugLine(line_cursor.line - 1);
        }
    }
    event.Skip(); // must do
}

void DebuggerGDB::OnProjectActivated(CodeBlocksEvent& event)
{
    // allow others to catch this
    event.Skip();

    // when a project is activated and it's not the actively debugged project,
    // ask the user to end debugging or re-activate the debugged project.

    if (!m_State.HasDriver() || !m_pProject)
        return;

    if (event.GetProject() != m_pProject)
    {
        wxString msg = _("You can't change the active project while you 're actively debugging another.\n"
                        "Do you want to stop debugging?\n\n"
                        "Click \"Yes\" to stop debugging now or click \"No\" to re-activate the debuggee.");
        if (cbMessageBox(msg, _("Warning"), wxICON_WARNING | wxYES_NO) == wxID_YES)
        {
            Stop();
        }
        else
        {
            Manager::Get()->GetProjectManager()->SetProject(m_pProject);
        }
    }
}

void DebuggerGDB::OnProjectClosed(CodeBlocksEvent& event)
{
    // allow others to catch this
    event.Skip();

    // remove all search dirs stored for this project so we don't have conflicts
    // if a newly opened project happens to use the same memory address
    GetSearchDirs(event.GetProject()).clear();

    // the same for remote debugging
    GetRemoteDebuggingMap(event.GetProject()).clear();

    // remove all breakpoints belonging to the closed project
    m_State.RemoveAllProjectBreakpoints(event.GetProject());
    if (m_pBreakpointsWindow)
        m_pBreakpointsWindow->Refresh();

    // when a project closes, make sure it's not the actively debugged project.
    // if so, end debugging immediately!

    if (!m_State.HasDriver() || !m_pProject)
        return;

    if (event.GetProject() == m_pProject)
    {
        AnnoyingDialog dlg(_("Project closed while debugging message"),
                           _("The project you were debugging has closed.\n"
                             "(The application most liekely just finished.)"
                             "The debugging session will terminate immediately."),
                            wxART_WARNING, AnnoyingDialog::OK, wxID_OK);
        dlg.ShowModal();
        Stop();
    }
}

void DebuggerGDB::OnIdle(wxIdleEvent& event)
{
    if (m_pProcess && ((PipedProcess*)m_pProcess)->HasInput())
        event.RequestMore();
    else
        event.Skip();
}

void DebuggerGDB::OnTimer(wxTimerEvent& event)
{
    // send any buffered (previous) output
    ParseOutput(wxEmptyString);

    wxWakeUpIdle();
}

void DebuggerGDB::OnWatchesChanged(wxCommandEvent& event)
{
    DoWatches();
}

void DebuggerGDB::OnCursorChanged(wxCommandEvent& event)
{
    if (m_State.HasDriver())
    {
        const Cursor& cursor = m_State.GetDriver()->GetCursor();
        // checking if driver is stopped is redundant because it would only
        // send us this event if it was stopped anyway
        if (/*m_State.GetDriver()->IsStopped() &&*/ cursor.changed)
        {
            SyncEditor(cursor.file, cursor.line);
            m_HaltAtLine = cursor.line - 1;
            BringAppToFront();
            if (cursor.line != -1)
                Log(wxString::Format(_("At %s:%d"), cursor.file.c_str(), cursor.line));
            else
                Log(wxString::Format(_("In %s (%s)"), cursor.function.c_str(), cursor.file.c_str()));

            // update watches
            if (IsWindowReallyShown(m_pTree))
                DoWatches();

            // update CPU registers
            if (IsWindowReallyShown(m_pCPURegisters))
                RunCommand(CMD_REGISTERS);

            // update callstack
            if (IsWindowReallyShown(m_pBacktrace))
                RunCommand(CMD_BACKTRACE);

            // update disassembly
            if (IsWindowReallyShown(m_pDisassembly))
            {
                unsigned long int addrL;
                cursor.address.ToULong(&addrL, 16);
                m_pDisassembly->SetActiveAddress(addrL);
                RunCommand(CMD_DISASSEMBLE);
            }

            // update memory examiner
            if (IsWindowReallyShown(m_pExamineMemoryDlg))
                MemoryDump();

            // update running threads
            if (IsWindowReallyShown(m_pThreadsDlg))
                RunningThreads();
        }
    }
}

void DebuggerGDB::OnAddWatch(wxCommandEvent& event)
{
    m_pTree->AddWatch(GetEditorWordAtCaret());
}

void DebuggerGDB::OnAttachToProcess(wxCommandEvent& event)
{
    wxString pidStr = wxGetTextFromUser(_("PID to attach to:"));
    if (!pidStr.IsEmpty())
    {
        pidStr.ToLong((long*)&m_PidToAttach);
        Debug();
    }
}

void DebuggerGDB::OnDetach(wxCommandEvent& event)
{
    m_State.GetDriver()->Detach();
    m_State.GetDriver()->Stop();
}

void DebuggerGDB::OnSettings(wxCommandEvent& event)
{
    Configure();
}

int DebuggerGDB::RunNixConsole()
{
#ifndef __WXMSW__

    // start the xterm and put the shell to sleep with -e sleep 80000
    // fetch the xterm tty so we can issue to gdb a "tty /dev/pts/#"
    // redirecting program stdin/stdout/stderr to the xterm console.

    wxString cmd;
    wxString title = wxT("Program Console");
    m_nConsolePid = 0;
    // for non-win platforms, use m_ConsoleTerm to run the console app
    wxString term = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/console_terminal"), DEFAULT_CONSOLE_TERM);
    term.Replace(_T("$TITLE"), _T("'") + title + _T("'"));
    cmd << term << _T(" ");
    cmd << wxT("sleep ");
    cmd << wxString::Format(wxT("%d"),80000 + ::wxGetProcessId());

    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(cmd);
    DebugLog(wxString::Format( _("Executing: %s"), cmd.c_str()) );
    //start xterm -e sleep {some unique # of seconds}
    m_nConsolePid = wxExecute(cmd, wxEXEC_ASYNC);
    if (m_nConsolePid <= 0) return -1;

    // Issue the PS command to get the /dev/tty device name
    // First, wait for the xterm to settle down, else PS won't see the sleep task
    Manager::Yield();
    ::wxSleep(1);
    m_ConsoleTty = GetConsoleTty(m_nConsolePid);
    if (not m_ConsoleTty.IsEmpty() )
    {
        // show what we found as tty
        DebugLog(wxString::Format(wxT("GetConsoleTTY[%s]ConsolePid[%d]"),m_ConsoleTty.c_str(),m_nConsolePid));
        return m_nConsolePid;
    }
    // failed to find the console tty
    DebugLog( wxT("Console Execution error:failed to find console tty."));
    if (m_nConsolePid != 0)
        ::wxKill(m_nConsolePid);
    m_nConsolePid = 0;
#endif // !__WWXMSW__
    return -1;
}

wxString DebuggerGDB::GetConsoleTty(int ConsolePid)
{
#ifndef __WXMSW__

    // execute the ps x -o command  and read PS output to get the /dev/tty field

    unsigned long ConsPid = ConsolePid;
    wxString psCmd;
    wxArrayString psOutput;
    wxArrayString psErrors;

    psCmd << wxT("ps x -o tty,pid,command");
    DebugLog(wxString::Format( _("Executing: %s"), psCmd.c_str()) );
    int result = wxExecute(psCmd, psOutput, psErrors, wxEXEC_SYNC);
    psCmd.Clear();
    if (result != 0)
    {
        psCmd << wxT("Result of ps x:") << result;
        DebugLog(wxString::Format( _("Execution Error:"), psCmd.c_str()) );
        return wxEmptyString;
    }

    wxString ConsTtyStr;
    wxString ConsPidStr;
    ConsPidStr << ConsPid;
    //find task with our unique sleep time
    wxString uniqueSleepTimeStr;
    uniqueSleepTimeStr << wxT("sleep ") << wxString::Format(wxT("%d"),80000 + ::wxGetProcessId());
    // search the output of "ps pid" command
    int knt = psOutput.GetCount();
    for (int i=knt-1; i>-1; --i)
    {
        psCmd = psOutput.Item(i);
        DebugLog(wxString::Format( _("PS result: %s"), psCmd.c_str()) );
        // find the pts/# or tty/# or whatever it's called
        // by seaching the output of "ps x -o tty,pid,command" command.
        // The output of ps looks like:
        // TT       PID   COMMAND
        // pts/0    13342 /bin/sh ./run.sh
        // pts/0    13343 /home/pecanpecan/devel/trunk/src/devel/codeblocks
        // pts/0    13361 /usr/bin/gdb -nx -fullname -quiet -args ./conio
        // pts/0    13362 xterm -font -*-*-*-*-*-*-20-*-*-*-*-*-*-* -T Program Console -e sleep 93343
        // pts/2    13363 sleep 93343
        // ?        13365 /home/pecan/proj/conio/conio
        // pts/1    13370 ps x -o tty,pid,command

        if (psCmd.Contains(uniqueSleepTimeStr))
        do
        {
            // check for correct "sleep" line
            if (psCmd.Contains(wxT("-T")))
                break; //error;wrong sleep line.
            // found "sleep 93343" string, extract tty field
            ConsTtyStr = wxT("/dev/") + psCmd.BeforeFirst(' ');
            DebugLog(wxString::Format( _("TTY is[%s]"), ConsTtyStr.c_str()) );
            return ConsTtyStr;
        } while(0);//if do
    }//for

    knt = psErrors.GetCount();
    for (int i=0; i<knt; ++i)
        DebugLog(wxString::Format( _("PS Error:%s"), psErrors.Item(i).c_str()) );
#endif // !__WXMSW__
    return wxEmptyString;
}

void DebuggerGDB::OnCompilerStarted(CodeBlocksEvent& event)
{
//    Manager::Get()->GetLogManager()->DebugLog(F(_T("DebuggerGDB::OnCompilerStarted")));
}

void DebuggerGDB::OnCompilerFinished(CodeBlocksEvent& event)
{
//    Manager::Get()->GetLogManager()->DebugLog(F(_T("DebuggerGDB::OnCompilerFinished")));

    if (m_WaitingCompilerToFinish)
    {
        m_WaitingCompilerToFinish = false;
        // only proceed if build succeeeded
        if (!m_pCompiler || m_pCompiler->GetExitCode() == 0)
            DoDebug();
    }
}

void DebuggerGDB::OnBuildTargetSelected(CodeBlocksEvent& event)
{
//    Manager::Get()->GetLogManager()->DebugLog(F(_T("DebuggerGDB::OnBuildTargetSelected: target=%s"), event.GetBuildTargetName().c_str()));

    // verify that the project that sent it, is the one we 're debugging
    if (!m_pProject || event.GetProject() == m_pProject)
        m_ActiveBuildTarget = event.GetBuildTargetName();
}
