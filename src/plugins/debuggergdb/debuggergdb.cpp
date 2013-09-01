/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include <algorithm> // std::remove_if

#ifndef CB_PRECOMP
    #include <wx/app.h>
    #include <wx/txtstrm.h>
    #include <wx/regex.h>
    #include <wx/msgdlg.h>
    #include <wx/frame.h> // GetMenuBar
    #include <wx/menu.h>
    #include <wx/filedlg.h>

    #include "cbproject.h"
    #include "manager.h"
    #include "configmanager.h"
    #include "projectmanager.h"
    #include "pluginmanager.h"
    #include "editormanager.h"
    #include "macrosmanager.h"
    #include "cbeditor.h"
    #include "projectbuildtarget.h"
    #include "sdk_events.h"
    #include "compilerfactory.h"
    #include "xtra_res.h"

    #include "scrollingdialog.h"
    #include "globals.h"
#endif

#include <wx/tokenzr.h>
#include "editarraystringdlg.h"
#include "projectloader_hooks.h"
#include "annoyingdialog.h"
#include "cbstyledtextctrl.h"
#include "compilercommandgenerator.h"

#include <cbdebugger_interfaces.h>
#include "editbreakpointdlg.h"

#include "databreakpointdlg.h"
#include "debuggerdriver.h"
#include "debuggergdb.h"
#include "debuggeroptionsdlg.h"
#include "debuggeroptionsprjdlg.h"
#include "editwatchdlg.h"


#define implement_debugger_toolbar

// function pointer to DebugBreakProcess under windows (XP+)
#if (_WIN32_WINNT >= 0x0501)
#include "Tlhelp32.h"
typedef BOOL WINAPI   (*DebugBreakProcessApiCall)       (HANDLE);
typedef HANDLE WINAPI (*CreateToolhelp32SnapshotApiCall)(DWORD  dwFlags,   DWORD             th32ProcessID);
typedef BOOL WINAPI   (*Process32FirstApiCall)          (HANDLE hSnapshot, LPPROCESSENTRY32W lppe);
typedef BOOL WINAPI   (*Process32NextApiCall)           (HANDLE hSnapshot, LPPROCESSENTRY32W lppe);

DebugBreakProcessApiCall        DebugBreakProcessFunc = 0;
CreateToolhelp32SnapshotApiCall CreateToolhelp32SnapshotFunc = 0;
Process32FirstApiCall           Process32FirstFunc = 0;
Process32NextApiCall            Process32NextFunc = 0;

HINSTANCE kernelLib = 0;

#endif

#ifdef __WXMSW__
// disable the CTRL_C event
inline BOOL WINAPI HandlerRoutine(cb_unused DWORD dwCtrlType)
{
    return TRUE;
}
#endif

// valid debugger command constants
enum DebugCommandConst
{
    CMD_CONTINUE,
    CMD_STEP,
    CMD_STEPIN,
    CMD_STEPOUT,
    CMD_STEP_INSTR,
    CMD_STEP_INTO_INSTR,
    CMD_STOP,
    CMD_BACKTRACE,
    CMD_DISASSEMBLE,
    CMD_REGISTERS,
    CMD_MEMORYDUMP,
    CMD_RUNNINGTHREADS
};

const wxString g_EscapeChar = wxChar(26);

namespace
{
long idMenuInfoFrame = wxNewId();
long idMenuInfoDLL = wxNewId();
long idMenuInfoFiles = wxNewId();
long idMenuInfoFPU = wxNewId();
long idMenuInfoSignals = wxNewId();

long idMenuInfoPrintElementsUnlimited = wxNewId();
long idMenuInfoPrintElements20 = wxNewId();
long idMenuInfoPrintElements50 = wxNewId();
long idMenuInfoPrintElements100 = wxNewId();

long idMenuInfoCatchThrow = wxNewId();

long idGDBProcess = wxNewId();
long idTimerPollDebugger = wxNewId();
long idMenuSettings = wxNewId();

long idMenuWatchDereference = wxNewId();

// this auto-registers the plugin
PluginRegistrant<DebuggerGDB> reg(_T("Debugger"));
}

BEGIN_EVENT_TABLE(DebuggerGDB, cbDebuggerPlugin)
    EVT_MENU(idMenuInfoFrame, DebuggerGDB::OnInfoFrame)
    EVT_MENU(idMenuInfoDLL, DebuggerGDB::OnInfoDLL)
    EVT_MENU(idMenuInfoFiles, DebuggerGDB::OnInfoFiles)
    EVT_MENU(idMenuInfoFPU, DebuggerGDB::OnInfoFPU)
    EVT_MENU(idMenuInfoSignals, DebuggerGDB::OnInfoSignals)

    EVT_MENU(idMenuWatchDereference, DebuggerGDB::OnMenuWatchDereference)

    EVT_PIPEDPROCESS_STDOUT(idGDBProcess, DebuggerGDB::OnGDBOutput)
    EVT_PIPEDPROCESS_STDERR(idGDBProcess, DebuggerGDB::OnGDBError)
    EVT_PIPEDPROCESS_TERMINATED(idGDBProcess, DebuggerGDB::OnGDBTerminated)

    EVT_IDLE(DebuggerGDB::OnIdle)
    EVT_TIMER(idTimerPollDebugger, DebuggerGDB::OnTimer)

    EVT_COMMAND(-1, DEBUGGER_CURSOR_CHANGED, DebuggerGDB::OnCursorChanged)
    EVT_COMMAND(-1, DEBUGGER_SHOW_FILE_LINE, DebuggerGDB::OnShowFile)

    EVT_UPDATE_UI(idMenuInfoPrintElementsUnlimited, DebuggerGDB::OnUpdateTools)
    EVT_UPDATE_UI(idMenuInfoPrintElements20, DebuggerGDB::OnUpdateTools)
    EVT_UPDATE_UI(idMenuInfoPrintElements50, DebuggerGDB::OnUpdateTools)
    EVT_UPDATE_UI(idMenuInfoPrintElements100, DebuggerGDB::OnUpdateTools)

    EVT_MENU(idMenuInfoPrintElementsUnlimited, DebuggerGDB::OnPrintElements)
    EVT_MENU(idMenuInfoPrintElements20, DebuggerGDB::OnPrintElements)
    EVT_MENU(idMenuInfoPrintElements50, DebuggerGDB::OnPrintElements)
    EVT_MENU(idMenuInfoPrintElements100, DebuggerGDB::OnPrintElements)

    EVT_UPDATE_UI(idMenuInfoCatchThrow, DebuggerGDB::OnUpdateCatchThrow)
    EVT_MENU(idMenuInfoCatchThrow, DebuggerGDB::OnCatchThrow)
END_EVENT_TABLE()

DebuggerGDB::DebuggerGDB() :
    cbDebuggerPlugin(wxT("GDB/CDB debugger"), wxT("gdb_debugger")),
    m_State(this),
    m_pProcess(0L),
    m_LastExitCode(0),
    m_Pid(0),
    m_PidToAttach(0),
    m_NoDebugInfo(false),
    m_StoppedOnSignal(false),
    m_pProject(0),
    m_stopDebuggerConsoleClosed(false),
    m_TemporaryBreak(false),
    m_printElements(0)
{
    if (!Manager::LoadResource(_T("debugger.zip")))
    {
        NotifyMissingFile(_T("debugger.zip"));
    }

    // get a function pointer to DebugBreakProcess under windows (XP+)
    #if (_WIN32_WINNT >= 0x0501)
    kernelLib = LoadLibrary(TEXT("kernel32.dll"));
    if (kernelLib)
    {
        DebugBreakProcessFunc = (DebugBreakProcessApiCall)GetProcAddress(kernelLib, "DebugBreakProcess");
        //Windows XP
        CreateToolhelp32SnapshotFunc = (CreateToolhelp32SnapshotApiCall)GetProcAddress(kernelLib, "CreateToolhelp32Snapshot");
        Process32FirstFunc = (Process32FirstApiCall)GetProcAddress(kernelLib, "Process32First");
        Process32NextFunc = (Process32NextApiCall)GetProcAddress(kernelLib, "Process32Next");
    }
    #endif
}

DebuggerGDB::~DebuggerGDB()
{
    #if (_WIN32_WINNT >= 0x0501)
    if (kernelLib)
        FreeLibrary(kernelLib);
    #endif
}

void DebuggerGDB::OnAttachReal()
{
    m_TimerPollDebugger.SetOwner(this, idTimerPollDebugger);

    // hook to project loading procedure
    ProjectLoaderHooks::HookFunctorBase* myhook = new ProjectLoaderHooks::HookFunctor<DebuggerGDB>(this, &DebuggerGDB::OnProjectLoadingHook);
    m_HookId = ProjectLoaderHooks::RegisterHook(myhook);

    // register event sink
    Manager::Get()->RegisterEventSink(cbEVT_BUILDTARGET_SELECTED, new cbEventFunctor<DebuggerGDB, CodeBlocksEvent>(this, &DebuggerGDB::OnBuildTargetSelected));
}

void DebuggerGDB::OnReleaseReal(cb_unused bool appShutDown)
{
    ProjectLoaderHooks::UnregisterHook(m_HookId, true);

    //Close debug session when appShutDown
    if (m_State.HasDriver())
    {
        Stop();
        wxYieldIfNeeded();
    }

    m_State.CleanUp();
    KillConsole();
}

bool DebuggerGDB::SupportsFeature(cbDebuggerFeature::Flags flag)
{
    DebuggerConfiguration &config = GetActiveConfigEx();

    if (config.IsGDB())
    {
        switch (flag)
        {
        case cbDebuggerFeature::Breakpoints:
        case cbDebuggerFeature::Callstack:
        case cbDebuggerFeature::CPURegisters:
        case cbDebuggerFeature::Disassembly:
        case cbDebuggerFeature::Watches:
        case cbDebuggerFeature::ValueTooltips:
        case cbDebuggerFeature::ExamineMemory:
        case cbDebuggerFeature::Threads:
        case cbDebuggerFeature::RunToCursor:
        case cbDebuggerFeature::SetNextStatement:
            return true;
        default:
            return false;
        }
    }
    else
    {
        switch (flag)
        {
        case cbDebuggerFeature::Breakpoints:
        case cbDebuggerFeature::Callstack:
        case cbDebuggerFeature::CPURegisters:
        case cbDebuggerFeature::Disassembly:
        case cbDebuggerFeature::Watches:
        case cbDebuggerFeature::ValueTooltips:
            return true;
        case cbDebuggerFeature::ExamineMemory:
        case cbDebuggerFeature::Threads:
        case cbDebuggerFeature::RunToCursor:
        case cbDebuggerFeature::SetNextStatement:
        default:
            return false;
        }
    }

    return false;
}

cbDebuggerConfiguration* DebuggerGDB::LoadConfig(const ConfigManagerWrapper &config)
{
    return new DebuggerConfiguration(config);
}

DebuggerConfiguration& DebuggerGDB::GetActiveConfigEx()
{
    return static_cast<DebuggerConfiguration&>(GetActiveConfig());
}

cbConfigurationPanel* DebuggerGDB::GetProjectConfigurationPanel(wxWindow* parent, cbProject* project)
{
    DebuggerOptionsProjectDlg* dlg = new DebuggerOptionsProjectDlg(parent, this, project);
    return dlg;
}

void DebuggerGDB::OnConfigurationChange(cb_unused bool isActive)
{
    DebuggerConfiguration &config = GetActiveConfigEx();
    bool locals = config.GetFlag(DebuggerConfiguration::WatchLocals);
    bool funcArgs = config.GetFlag(DebuggerConfiguration::WatchFuncArgs);

    cbWatchesDlg *watchesDialog = Manager::Get()->GetDebuggerManager()->GetWatchesDialog();

    if (!locals && m_localsWatch)
    {
        watchesDialog->RemoveWatch(m_localsWatch);
        m_localsWatch = cb::shared_ptr<GDBWatch>();
    }
    if (!funcArgs && m_funcArgsWatch)
    {
        watchesDialog->RemoveWatch(m_funcArgsWatch);
        m_funcArgsWatch = cb::shared_ptr<GDBWatch>();
    }
}

wxArrayString& DebuggerGDB::GetSearchDirs(cbProject* prj)
{
    SearchDirsMap::iterator it = m_SearchDirs.find(prj);
    if (it == m_SearchDirs.end()) // create an empty set for this project
        it = m_SearchDirs.insert(m_SearchDirs.begin(), std::make_pair(prj, wxArrayString()));

    return it->second;
}

RemoteDebuggingMap& DebuggerGDB::GetRemoteDebuggingMap(cbProject* project)
{
    if (!project)
        project = m_pProject;

    ProjectRemoteDebuggingMap::iterator it = m_RemoteDebugging.find(project);
    if (it == m_RemoteDebugging.end()) // create an empty set for this project
        it = m_RemoteDebugging.insert(m_RemoteDebugging.begin(), std::make_pair(project, RemoteDebuggingMap()));

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

                if (rdOpt)
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
                    if (rdOpt->Attribute("extended_remote"))
                        rd.extendedRemote = cbC2U(rdOpt->Attribute("extended_remote")) != _T("0");
                    if (rdOpt->Attribute("additional_shell_cmds_after"))
                        rd.additionalShellCmdsAfter = cbC2U(rdOpt->Attribute("additional_shell_cmds_after"));
                    if (rdOpt->Attribute("additional_shell_cmds_before"))
                        rd.additionalShellCmdsBefore = cbC2U(rdOpt->Attribute("additional_shell_cmds_before"));

                    rdprj.insert(rdprj.end(), std::make_pair(bt, rd));
                }

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
//                // valid targets only
//                if (!it->first)
//                    continue;

                RemoteDebugging& rd = it->second;

                // if no different than defaults, skip it
                if (rd.serialPort.IsEmpty() && rd.ip.IsEmpty() &&
                    rd.additionalCmds.IsEmpty() && rd.additionalCmdsBefore.IsEmpty() &&
                    !rd.skipLDpath && !rd.extendedRemote)
                {
                    continue;
                }

                TiXmlElement* rdnode = node->InsertEndChild(TiXmlElement("remote_debugging"))->ToElement();
                if (it->first)
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
                if (rd.extendedRemote)
                    tgtnode->SetAttribute("extended_remote", "1");
                if (!rd.additionalShellCmdsAfter.IsEmpty())
                    tgtnode->SetAttribute("additional_shell_cmds_after", cbU2C(rd.additionalShellCmdsAfter));
                if (!rd.additionalShellCmdsBefore.IsEmpty())
                    tgtnode->SetAttribute("additional_shell_cmds_before", cbU2C(rd.additionalShellCmdsBefore));
            }
        }
    }
}

void DebuggerGDB::DoWatches()
{
    if (!m_pProcess)
        return;

    DebuggerConfiguration &config = GetActiveConfigEx();

    bool locals = config.GetFlag(DebuggerConfiguration::WatchLocals);
    bool funcArgs = config.GetFlag(DebuggerConfiguration::WatchFuncArgs);


    if (locals)
    {
        if (m_localsWatch == nullptr)
        {
            m_localsWatch = cb::shared_ptr<GDBWatch>(new GDBWatch(wxT("Locals")));
            m_localsWatch->Expand(true);
            m_localsWatch->MarkAsChanged(false);
            cbWatchesDlg *watchesDialog = Manager::Get()->GetDebuggerManager()->GetWatchesDialog();
            watchesDialog->AddSpecialWatch(m_localsWatch, true);
        }
    }

    if (funcArgs)
    {
        if (m_funcArgsWatch == nullptr)
        {
            m_funcArgsWatch = cb::shared_ptr<GDBWatch>(new GDBWatch(wxT("Function arguments")));
            m_funcArgsWatch->Expand(true);
            m_funcArgsWatch->MarkAsChanged(false);
            cbWatchesDlg *watchesDialog = Manager::Get()->GetDebuggerManager()->GetWatchesDialog();
            watchesDialog->AddSpecialWatch(m_funcArgsWatch, true);
        }
    }

    m_State.GetDriver()->UpdateWatches(m_localsWatch, m_funcArgsWatch, m_watches);
}

int DebuggerGDB::LaunchProcess(const wxString& cmd, const wxString& cwd)
{
    if (m_pProcess)
        return -1;

    // start the gdb process
    m_pProcess = new PipedProcess(&m_pProcess, this, idGDBProcess, true, cwd);
    Log(_("Starting debugger: ") + cmd);
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
        DebugLog(wxString::Format( _("Executing: %s"), psCmd.wx_str()) );
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
            DebugLog(wxString::Format( _("PS Error:%s"), psErrors.Item(i).wx_str()) );
    }
#endif

    if (!m_Pid)
    {
        delete m_pProcess;
        m_pProcess = 0;
        Log(_("failed"), Logger::error);
        return -1;
    }
    else if (!m_pProcess->GetOutputStream())
    {
        delete m_pProcess;
        m_pProcess = 0;
        Log(_("failed (to get debugger's stdin)"), Logger::error);
        return -2;
    }
    else if (!m_pProcess->GetInputStream())
    {
        delete m_pProcess;
        m_pProcess = 0;
        Log(_("failed (to get debugger's stdout)"), Logger::error);
        return -2;
    }
    else if (!m_pProcess->GetErrorStream())
    {
        delete m_pProcess;
        m_pProcess = 0;
        Log(_("failed (to get debugger's stderr)"), Logger::error);
        return -2;
    }
    Log(_("done"));
    return 0;
}

bool DebuggerGDB::IsStopped() const
{
    return !m_State.HasDriver() || m_State.GetDriver()->IsProgramStopped();
}

bool DebuggerGDB::IsBusy() const
{
    return m_State.HasDriver() && m_State.GetDriver()->IsQueueBusy();
}


bool DebuggerGDB::Debug(bool breakOnEntry)
{
    // if already running, return
    if (m_pProcess || WaitingCompilerToFinish())
        return false;

    m_pProject = 0;
    m_NoDebugInfo = false;

    // clear the debug log
    ClearLog();

    // can only debug projects or attach to processes
    ProjectManager* prjMan = Manager::Get()->GetProjectManager();
    cbProject* project = prjMan->GetActiveProject();
    if (!project && m_PidToAttach == 0)
        return false;

    m_pProject = project;
    if (m_pProject && m_ActiveBuildTarget.IsEmpty())
        m_ActiveBuildTarget = m_pProject->GetActiveBuildTarget();

    m_Canceled = false;
    if (!EnsureBuildUpToDate(breakOnEntry ? StartTypeStepInto : StartTypeRun))
        return false;

    // if not waiting for the compiler, start debugging now
    // but first check if the driver has already been started:
    // if the build process was ultra-fast (i.e. nothing to be done),
    // it may have already called DoDebug() and m_WaitingCompilerToFinish
    // would already be set to false
    // by checking the driver availability, we avoid calling DoDebug
    // a second consecutive time...
    // the same applies for m_Canceled: it is true if DoDebug() was launched but
    // returned an error
    if (!WaitingCompilerToFinish() && !m_State.HasDriver() && !m_Canceled)
    {
        return DoDebug(breakOnEntry) == 0;
    }

    return true;
}

int DebuggerGDB::DoDebug(bool breakOnEntry)
{
    // set this to true before every error exit point in this function
    m_Canceled = false;
    ProjectManager* prjMan = Manager::Get()->GetProjectManager();

    // select the build target to debug
    ProjectBuildTarget* target = 0;
    Compiler* actualCompiler = 0;
    if ( (m_PidToAttach == 0) && m_pProject)
    {
        Log(_("Selecting target: "));
        if (!m_pProject->BuildTargetValid(m_ActiveBuildTarget, false))
        {
            int tgtIdx = m_pProject->SelectTarget();
            if (tgtIdx == -1)
            {
                Log(_("canceled"));
                m_Canceled = true;
                return 3;
            }
            target = m_pProject->GetBuildTarget(tgtIdx);
            m_ActiveBuildTarget = (target ? target->GetTitle() : wxString(wxEmptyString));
        }
        else
            target = m_pProject->GetBuildTarget(m_ActiveBuildTarget);

        // make sure it's not a commands-only target
        if (target && target->GetTargetType() == ttCommandsOnly)
        {
            cbMessageBox(_("The selected target is only running pre/post build step commands\n"
                           "Can't debug such a target..."), _("Information"), wxICON_INFORMATION);
            Log(_("aborted"));
            return 3;
        }
        if (target) Log(target->GetTitle());

        // find the target's compiler (to see which debugger to use)
        actualCompiler = CompilerFactory::GetCompiler(target ? target->GetCompilerID()
                                                             : m_pProject->GetCompilerID());
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
    cmdexe = GetActiveConfigEx().GetDebuggerExecutable();
    cmdexe.Trim();
    cmdexe.Trim(true);
    if (cmdexe.IsEmpty())
    {
        Log(_("ERROR: You need to specify a debugger program in the debuggers's settings."), Logger::error);

        if (platform::windows)
        {
            Log(_("(For MinGW compilers, it's 'gdb.exe' (without the quotes))"), Logger::error);
            Log(_("(For MSVC compilers, it's 'cdb.exe' (without the quotes))"), Logger::error);
        }
        else
        {
            Log(_("(For GCC compilers, it's 'gdb' (without the quotes))"), Logger::error);
        }

        m_Canceled = true;
        return -1;
    }

    // start debugger driver based on target compiler, or default compiler if no target
    if (!m_State.StartDriver(target))
    {
        cbMessageBox(_T("Could not decide which debugger to use!"), _T("Error"), wxICON_ERROR);
        m_Canceled = true;
        return -1;
    }

    // Notify debugger plugins so they could start a GDB server process
    PluginManager *plm = Manager::Get()->GetPluginManager();
    CodeBlocksEvent evt(cbEVT_DEBUGGER_STARTED);
    plm->NotifyPlugins(evt);
    int nRet = evt.GetInt();
    if (nRet < 0)
    {
        cbMessageBox(_T("A plugin interrupted the debug process."));
        Log(_("Aborted by plugin"));
        m_Canceled = true;
        return -1;
    }
    // Continue

    // create gdb launch command
    wxString cmd;

    // prepare the driver
    wxString cmdline;
    if (m_PidToAttach == 0)
    {
        m_State.GetDriver()->ClearDirectories();
        // add other open projects dirs as search dirs (only if option is enabled)
        if (GetActiveConfigEx().GetFlag(DebuggerConfiguration::AddOtherProjectDirs))
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
            AddSourceDir(pdirs[i]);

        // lastly, add THE project as source dir
        if (m_pProject)
        {
            AddSourceDir(m_pProject->GetBasePath());
            AddSourceDir(m_pProject->GetCommonTopLevelPath());
        }

        // set the file to debug (depends on the target type)
        wxString debuggee, path;
        if ( !GetDebuggee(debuggee, path, target) )
        {
            m_Canceled = true;
            return -3;
        }

        if (!path.empty())
        {
            ConvertToGDBDirectory(path);
            if (path != _T(".")) // avoid silly message "changing to ."
            {
                Log(_("Changing directory to: ") + path);
                m_State.GetDriver()->SetWorkingDirectory(path);
            }
        }

        if (target && !target->GetExecutionParameters().IsEmpty())
            m_State.GetDriver()->SetArguments(target->GetExecutionParameters());

        cmdline = m_State.GetDriver()->GetCommandLine(cmdexe, debuggee, GetActiveConfigEx().GetUserArguments());
    }
    else // m_PidToAttach != 0
        cmdline = m_State.GetDriver()->GetCommandLine(cmdexe, m_PidToAttach, GetActiveConfigEx().GetUserArguments());

    RemoteDebuggingMap& rdprj = GetRemoteDebuggingMap();
    RemoteDebugging rd = rdprj[0]; // project settings
    RemoteDebuggingMap::iterator it = rdprj.find(target); // target settings
    if (it != rdprj.end())
        rd.MergeWith(it->second);
//////////////////killerbot : most probably here : execute the shell commands (we could access the per target debugger settings)
    wxString oldLibPath; // keep old PATH/LD_LIBRARY_PATH contents
    if (!rd.skipLDpath)
    {
        wxGetEnv(CB_LIBRARY_ENVVAR, &oldLibPath);

        // setup dynamic linker path
        if (actualCompiler && target)
        {
            wxString newLibPath;
            const wxString libPathSep = platform::windows ? _T(";") : _T(":");
            newLibPath << _T(".") << libPathSep;

            CompilerCommandGenerator *generator = actualCompiler->GetCommandGenerator(m_pProject);
            newLibPath << GetStringFromArray(generator->GetLinkerSearchDirs(target), libPathSep);
            delete generator;

            if (newLibPath.Mid(newLibPath.Length() - 1, 1) != libPathSep)
                newLibPath << libPathSep;
            newLibPath << oldLibPath;
            wxSetEnv(CB_LIBRARY_ENVVAR, newLibPath);
            Log(wxString(_("Set variable: ")) + CB_LIBRARY_ENVVAR wxT("=") + newLibPath);
        }
    }

    #ifdef __WXMSW__
    if (!m_State.GetDriver()->UseDebugBreakProcess())
    {
        AllocConsole();
        SetConsoleTitleA("Codeblocks debug console - DO NOT CLOSE!");
        SetConsoleCtrlHandler(HandlerRoutine, TRUE);
        m_bIsConsole = true;

        HWND windowHandle = GetConsoleWindow();
        if (windowHandle)
            ShowWindow(windowHandle, SW_HIDE);
    }
    #endif
    // start the gdb process
    wxString wdir = m_State.GetDriver()->GetDebuggersWorkingDirectory();
    if (wdir.empty())
        wdir = m_pProject ? m_pProject->GetBasePath() : _T(".");
    DebugLog(_T("Command-line: ") + cmdline);
    DebugLog(_T("Working dir : ") + wdir);
    int ret = LaunchProcess(cmdline, wdir);

    if (!rd.skipLDpath)
    {
        // restore dynamic linker path
        wxSetEnv(CB_LIBRARY_ENVVAR, oldLibPath);
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

    bool isConsole = (target && target->GetTargetType() == ttConsoleOnly);
    m_State.GetDriver()->Prepare(isConsole, m_printElements);
    m_State.ApplyBreakpoints();

   #ifndef __WXMSW__
    // create xterm and issue tty "/dev/pts/#" to GDB where
    // # is the tty for the newly created xterm
    m_bIsConsole = target && target->GetUseConsoleRunner();
    if (m_bIsConsole)
    {
        wxString consoleTty;
        m_nConsolePid = RunNixConsole(consoleTty);
        if (m_nConsolePid > 0)
        {
            m_stopDebuggerConsoleClosed = true;
            wxString gdbTtyCmd;
            gdbTtyCmd << wxT("tty ") << consoleTty;
            m_State.GetDriver()->QueueCommand(new DebuggerCmd(m_State.GetDriver(), gdbTtyCmd, true));
            DebugLog(wxString::Format( _("Queued:[%s]"), gdbTtyCmd.wx_str()) );
        }
    }//if
   #endif//ndef __WXMSW__

    // Don't issue 'run' if attaching to a process (Bug #1391904)
    if (m_PidToAttach == 0)
        m_State.GetDriver()->Start(breakOnEntry);
    else
        m_State.GetDriver()->Attach(m_PidToAttach);

    // switch to the user-defined layout for debugging
    if (m_pProcess)
        SwitchToDebuggingLayout();

    return 0;
} // Debug

void DebuggerGDB::AddSourceDir(const wxString& dir)
{
    if (!m_State.HasDriver() || dir.IsEmpty())
        return;
    wxString filename = dir;
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(filename); // apply env vars
    Log(_("Adding source dir: ") + filename);
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

void DebuggerGDB::ConvertDirectory(wxString& str, wxString base, bool relative)
{
    ConvertToGDBDirectory(str, base, relative);
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

    if (platform::windows)
    {
        int  ColonLocation   = str.Find(_T(':'));
        bool convert_path_83 = false;
        if (ColonLocation != wxNOT_FOUND)
            convert_path_83 = true;
        else if (!base.IsEmpty() && str.GetChar(0) != _T('/'))
        {
            if (base.GetChar(base.Length()) == _T('/'))
                base = base.Mid(0, base.Length() - 2);

            while (!str.IsEmpty())
            {
                base += _T("/") + str.BeforeFirst(_T('/'));
                if (str.Find(_T('/')) != wxNOT_FOUND) str = str.AfterFirst(_T('/'));
                else                                  str.Clear();
            }
            convert_path_83 = true;
        }

        // If can, get 8.3 name for path (Windows only)
        if (convert_path_83 && str.Contains(_T(' '))) // only if has spaces
        {
            wxFileName fn(str); // might contain a file name, too
            wxString path_83 = fn.GetShortPath();
            if (!path_83.IsEmpty())
                str = path_83; // construct filename again
        }

        if (ColonLocation == wxNOT_FOUND || base.IsEmpty())
            relative = false; // Can't do it
    }
    else
    {
        if ((str.GetChar(0) != _T('/') && str.GetChar(0) != _T('~')) || base.IsEmpty())
            relative = false;
    }

    if (relative)
    {
        if (platform::windows)
        {
            if (str.Find(_T(':')) != wxNOT_FOUND)
                str = str.Mid(str.Find(_T(':')) + 2, str.Length());
            if (base.Find(_T(':')) != wxNOT_FOUND)
                base = base.Mid(base.Find(_T(':')) + 2, base.Length());
        }
        else
        {
            if      (str.GetChar(0) == _T('/'))  str  = str.Mid(1, str.Length());
            else if (str.GetChar(0) == _T('~'))  str  = str.Mid(2, str.Length());

            if      (base.GetChar(0) == _T('/')) base = base.Mid(1, base.Length());
            else if (base.GetChar(0) == _T('~')) base = base.Mid(2, base.Length());
        }

        while (!base.IsEmpty() && !str.IsEmpty())
        {
            if (str.BeforeFirst(_T('/')) == base.BeforeFirst(_T('/')))
            {
                if (str.Find(_T('/')) == wxNOT_FOUND) str.Clear();
                else                                  str = str.AfterFirst(_T('/'));

                if (base.Find(_T('/')) == wxNOT_FOUND) base.Clear();
                else                                   base = base.AfterFirst(_T('/'));
            }
            else break;
        }
        while (!base.IsEmpty())
        {
            str = _T("../") + str;
            if (base.Find(_T('/')) == wxNOT_FOUND) base.Clear();
            else                                   base = base.AfterFirst(_T('/'));
        }
    }
    ConvertToGDBFriendly(str);
}

void DebuggerGDB::SendCommand(const wxString& cmd, bool debugLog)
{
    const wxString &cleandCmd = CleanStringValue(cmd);
    if (!debugLog)
        Log(_T("> ") + cleandCmd);

    if (debugLog)
        DoSendCommand(cleandCmd);
    else if (m_State.HasDriver())
        m_State.GetDriver()->QueueCommand(new DebuggerCmd(m_State.GetDriver(), cleandCmd, true));
}

void DebuggerGDB::DoSendCommand(const wxString& cmd)
{
    if (!m_pProcess || !IsStopped())
        return;

    if (HasDebugLog())
        DebugLog(wxT("> ") + cmd);

    m_pProcess->SendString(cmd);
}

void DebuggerGDB::RequestUpdate(DebugWindows window)
{
    switch (window)
    {
        case Backtrace:
            RunCommand(CMD_BACKTRACE);
            break;
        case CPURegisters:
            RunCommand(CMD_REGISTERS);
            break;
        case Disassembly:
            RunCommand(CMD_DISASSEMBLE);
            break;
        case ExamineMemory:
            RunCommand(CMD_MEMORYDUMP);
            break;
        case Threads:
            RunCommand(CMD_RUNNINGTHREADS);
            break;
        case Watches:
            if (IsWindowReallyShown(Manager::Get()->GetDebuggerManager()->GetWatchesDialog()->GetWindow()))
                DoWatches();
            break;
        default:
            break;
    }
}

void DebuggerGDB::RunCommand(int cmd)
{
    // just check for the process
    if (!m_pProcess)
        return;

    switch (cmd)
    {
        case CMD_CONTINUE:
        {
            ClearActiveMarkFromAllEditors();
            if (m_State.HasDriver())
            {
                Log(_("Continuing..."));
                m_State.GetDriver()->Continue();
                m_State.GetDriver()->ResetCurrentFrame();
            }
            break;
        }

        case CMD_STEP:
        {
            ClearActiveMarkFromAllEditors();
            if (m_State.HasDriver())
            {
                m_State.GetDriver()->Step();
                m_State.GetDriver()->ResetCurrentFrame();
            }
            break;
        }

        case CMD_STEP_INSTR:
        {
            ClearActiveMarkFromAllEditors();
            if (!Manager::Get()->GetDebuggerManager()->UpdateDisassembly())
            {
                // first time users should have some help from us ;)
                RunCommand(CMD_DISASSEMBLE);
            }
            if (m_State.HasDriver())
            {
                m_State.GetDriver()->StepInstruction();
                m_State.GetDriver()->ResetCurrentFrame();
                m_State.GetDriver()->NotifyCursorChanged();
            }
            break;
        }

        case CMD_STEP_INTO_INSTR:
        {
            ClearActiveMarkFromAllEditors();
            if (!Manager::Get()->GetDebuggerManager()->UpdateDisassembly())
            {
                // first time users should have some help from us ;)
                RunCommand(CMD_DISASSEMBLE);
            }
            if (m_State.HasDriver())
            {
                m_State.GetDriver()->StepIntoInstruction();
                m_State.GetDriver()->ResetCurrentFrame();
                m_State.GetDriver()->NotifyCursorChanged();
            }
            break;
        }

        case CMD_STEPIN:
        {
            ClearActiveMarkFromAllEditors();
            if (m_State.HasDriver())
            {
                m_State.GetDriver()->StepIn();
                m_State.GetDriver()->ResetCurrentFrame();
            }
            break;
        }

        case CMD_STEPOUT:
        {
            ClearActiveMarkFromAllEditors();
            if (m_State.HasDriver())
            {
                m_State.GetDriver()->StepOut();
                m_State.GetDriver()->ResetCurrentFrame();
            }
            break;
        }

        case CMD_STOP:
        {
            ClearActiveMarkFromAllEditors();
            if (m_State.HasDriver())
            {
                m_State.GetDriver()->Stop();
                m_State.GetDriver()->ResetCurrentFrame();
                MarkAsStopped();
            }
            break;
        }

        case CMD_BACKTRACE:
        {
            if (m_State.HasDriver())
                m_State.GetDriver()->Backtrace();
            break;
        }

        case CMD_DISASSEMBLE:
        {
            if (m_State.HasDriver())
                m_State.GetDriver()->Disassemble();
            break;
        }

        case CMD_REGISTERS:
        {
            if (m_State.HasDriver())
                m_State.GetDriver()->CPURegisters();
            break;
        }

        case CMD_MEMORYDUMP:
        {
            if (m_State.HasDriver())
                m_State.GetDriver()->MemoryDump();
            break;
        }

        case CMD_RUNNINGTHREADS:
        {
            if (m_State.HasDriver())
                m_State.GetDriver()->RunningThreads();
            break;
        }

        default: break;
    }
}

int DebuggerGDB::GetStackFrameCount() const
{
    return m_State.GetDriver()->GetStackFrames().size();
}

cb::shared_ptr<const cbStackFrame> DebuggerGDB::GetStackFrame(int index) const
{
    return m_State.GetDriver()->GetStackFrames()[index];
}

void DebuggerGDB::SwitchToFrame(int number)
{
    if (m_State.HasDriver())
    {
        m_State.GetDriver()->SetCurrentFrame(number, true);
        m_State.GetDriver()->SwitchToFrame(number);

        if (Manager::Get()->GetDebuggerManager()->UpdateBacktrace())
           Manager::Get()->GetDebuggerManager()->GetBacktraceDialog()->Reload();
    }
}

int DebuggerGDB::GetActiveStackFrame() const
{
    return m_State.HasDriver() ? m_State.GetDriver()->GetCurrentFrame() : 0;
}

int DebuggerGDB::GetThreadsCount() const
{
    if (!m_State.HasDriver())
        return 0;
    else
        return m_State.GetDriver()->GetThreads().size();
}

cb::shared_ptr<const cbThread> DebuggerGDB::GetThread(int index) const
{
    return m_State.GetDriver()->GetThreads()[index];
}

bool DebuggerGDB::SwitchToThread(int thread_number)
{
    if (!m_State.HasDriver())
        return false;
    DebuggerDriver *driver = m_State.GetDriver();
    DebuggerDriver::ThreadsContainer const &threads = driver->GetThreads();

    for (DebuggerDriver::ThreadsContainer::const_iterator it = threads.begin(); it != threads.end(); ++it)
    {
        if ((*it)->GetNumber() == thread_number)
        {
            if (!(*it)->IsActive())
                driver->SwitchThread(thread_number);
            return true;
        }
    }
    return false;
}

cb::shared_ptr<cbBreakpoint> DebuggerGDB::AddBreakpoint(const wxString& filename, int line)
{
    bool debuggerIsRunning = !IsStopped();
    if (debuggerIsRunning)
        DoBreak(true);

    cb::shared_ptr<DebuggerBreakpoint> bp = m_State.AddBreakpoint(filename, line, false);

    if (debuggerIsRunning)
        Continue();

    return bp;
}

cb::shared_ptr<cbBreakpoint> DebuggerGDB::AddDataBreakpoint(const wxString& dataExpression)
{
    DataBreakpointDlg dlg(Manager::Get()->GetAppWindow(), dataExpression, true, 1);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        const wxString& newDataExpression = dlg.GetDataExpression();
        int sel = dlg.GetSelection();
        cb::shared_ptr<DebuggerBreakpoint> bp = m_State.AddBreakpoint(newDataExpression, sel != 1, sel != 0);
        return bp;
    }
    else
        return cb::shared_ptr<cbBreakpoint>();
}

int DebuggerGDB::GetBreakpointsCount() const
{
    return m_State.GetBreakpoints().size();
}

cb::shared_ptr<cbBreakpoint> DebuggerGDB::GetBreakpoint(int index)
{
    BreakpointsList::const_iterator it = m_State.GetBreakpoints().begin();
    std::advance(it, index);
    cbAssert(it != m_State.GetBreakpoints().end());
    return *it;
}

cb::shared_ptr<const cbBreakpoint> DebuggerGDB::GetBreakpoint(int index) const
{
    BreakpointsList::const_iterator it = m_State.GetBreakpoints().begin();
    std::advance(it, index);
    cbAssert(it != m_State.GetBreakpoints().end());
    return *it;
}

void DebuggerGDB::UpdateBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint)
{
    const BreakpointsList &breakpoints = m_State.GetBreakpoints();
    BreakpointsList::const_iterator it = std::find(breakpoints.begin(), breakpoints.end(), breakpoint);
    if (it == breakpoints.end())
        return;
    cb::shared_ptr<DebuggerBreakpoint> bp = cb::static_pointer_cast<DebuggerBreakpoint>(breakpoint);
    bool reset = false;
    switch (bp->type)
    {
        case DebuggerBreakpoint::bptCode:
        {
            EditBreakpointDlg dlg(*bp, Manager::Get()->GetAppWindow());
            PlaceWindow(&dlg);
            if (dlg.ShowModal() == wxID_OK)
            {
                *bp = dlg.GetBreakpoint();
                reset = true;
            }
            break;
        }
        case DebuggerBreakpoint::bptData:
        {
            int old_sel = 0;
            if (bp->breakOnRead && bp->breakOnWrite)
                old_sel = 2;
            else if (!bp->breakOnRead && bp->breakOnWrite)
                old_sel = 1;
            DataBreakpointDlg dlg(Manager::Get()->GetAppWindow(), bp->breakAddress, bp->enabled, old_sel);
            PlaceWindow(&dlg);
            if (dlg.ShowModal() == wxID_OK)
            {
                bp->enabled = dlg.IsEnabled();
                bp->breakOnRead = dlg.GetSelection() != 1;
                bp->breakOnWrite = dlg.GetSelection() != 0;
                bp->breakAddress = dlg.GetDataExpression();
                reset = true;
            }
            break;
        }
        case DebuggerBreakpoint::bptFunction:
        default:
            return;
    }

    if (reset)
    {
        bool debuggerIsRunning = !IsStopped();
        if (debuggerIsRunning)
            DoBreak(true);

        m_State.ResetBreakpoint(bp);

        if (debuggerIsRunning)
            Continue();
    }
}

void DebuggerGDB::DeleteBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint)
{
    bool debuggerIsRunning = !IsStopped();
    if (debuggerIsRunning)
        DoBreak(true);

    m_State.RemoveBreakpoint(cb::static_pointer_cast<DebuggerBreakpoint>(breakpoint));

    if (debuggerIsRunning)
        Continue();
}

void DebuggerGDB::DeleteAllBreakpoints()
{
    bool debuggerIsRunning = !IsStopped();
    if (debuggerIsRunning)
        DoBreak(true);
    m_State.RemoveAllBreakpoints();

    if (debuggerIsRunning)
        Continue();
}

void DebuggerGDB::ShiftBreakpoint(int index, int lines_to_shift)
{
    BreakpointsList breakpoints = m_State.GetBreakpoints();
    BreakpointsList::iterator it = breakpoints.begin();
    std::advance(it, index);
    if (it != breakpoints.end())
        m_State.ShiftBreakpoint(*it, lines_to_shift);
}

void DebuggerGDB::EnableBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint, bool enable)
{
    bool debuggerIsRunning = !IsStopped();
    DebugLog(wxString::Format(wxT("DebuggerGDB::EnableBreakpoint(running=%d);"), debuggerIsRunning?1:0));
    if (debuggerIsRunning)
        DoBreak(true);

    cb::shared_ptr<DebuggerBreakpoint> bp = cb::static_pointer_cast<DebuggerBreakpoint>(breakpoint);
    bp->enabled = enable;
    m_State.ResetBreakpoint(bp);

    if (debuggerIsRunning)
        Continue();
}

void DebuggerGDB::DeleteAllProjectBreakpoints(cbProject* project)
{
    m_State.RemoveAllProjectBreakpoints(project);
}

void DebuggerGDB::Continue()
{
    RunCommand(CMD_CONTINUE);
}

void DebuggerGDB::Next()
{
    RunCommand(CMD_STEP);
}

void DebuggerGDB::NextInstruction()
{
    RunCommand(CMD_STEP_INSTR);
}

void DebuggerGDB::StepIntoInstruction()
{
    RunCommand(CMD_STEP_INTO_INSTR);
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
    if (!scs && !sce && !dcs && !dce) bResult = true;
    //No single/double quote in pair
    if (!(sce-scs) && !(dce-dcs)) bResult = true;
    //Outside of single quote
    if ((sce-scs) && ((bep < scs)||(bep >sce))) bResult = true;
    //Outside of double quote
    if ((dce-dcs) && ((bep < dcs)||(bep >dce))) bResult = true;

    return bResult;
}

void DebuggerGDB::StepOut()
{
    RunCommand(CMD_STEPOUT);
}

bool DebuggerGDB::RunToCursor(const wxString& filename, int line, const wxString& line_text)
{
    if (m_pProcess)
    {
        m_State.AddBreakpoint(filename, line, true, line_text);
        Manager::Get()->GetDebuggerManager()->GetBreakpointDialog()->Reload();
        Continue();
        return true;
    }
    else
    {
        if (!GetActiveConfigEx().GetFlag(DebuggerConfiguration::DoNotRun))
        {
            m_State.AddBreakpoint(filename, line, true, line_text);
            Manager::Get()->GetDebuggerManager()->GetBreakpointDialog()->Reload();
        }
        return Debug(false);
    }
}

void DebuggerGDB::SetNextStatement(const wxString& filename, int line)
{
    if (m_State.HasDriver() && IsStopped())
    {
        m_State.GetDriver()->SetNextStatement(filename, line);
    }
}

void DebuggerGDB::Break()
{
    DoBreak(false);
}

void DebuggerGDB::DoBreak(bool temporary)
{
    m_TemporaryBreak = temporary;

    // m_Process is PipedProcess I/O; m_Pid is debugger pid
    if (m_pProcess && m_Pid && !IsStopped())
    {
        long childPid = m_State.GetDriver()->GetChildPID();
        long pid = childPid;
    #ifndef __WXMSW__
        if (pid > 0 && !wxProcess::Exists(pid))
        {
            DebugLog(wxString::Format(_("Child process (pid:%ld) doesn't exists"), pid));
            pid = 0;
        }
        if (pid <= 0)
            pid = m_Pid; // try poking gdb directly
        // non-windows gdb can interrupt the running process. yay!
        if (pid <= 0) // look out for the "fake" PIDs (killall)
            cbMessageBox(_("Unable to stop the debug process!"), _("Error"), wxOK | wxICON_WARNING);
        else
        {
            if (!wxProcess::Exists(pid))
                DebugLog(wxString::Format(_("GDB process (pid:%ld) doesn't exists"), pid));

            DebugLog(wxString::Format(_("Code::Blocks is trying to interrupt process with pid: %ld; child pid: %ld gdb pid: %ld"),
                                      pid, childPid, static_cast<long>(m_Pid)));
            wxKillError error;
            if (wxKill(pid, wxSIGINT, &error) != 0)
                DebugLog(wxString::Format(_("Can't kill process (%ld) %d"), pid, (int)(error)));
        }
    #else
        // windows gdb can interrupt the running process too. yay!
        if (   (pid <=0)
            && (CreateToolhelp32SnapshotFunc!=NULL)
            && (Process32FirstFunc!=NULL)
            && (Process32NextFunc!=NULL) )
        {
            HANDLE snap = CreateToolhelp32SnapshotFunc(TH32CS_SNAPALL,0);
            if (snap!=INVALID_HANDLE_VALUE)
            {
                PROCESSENTRY32 lppe;
                lppe.dwSize = sizeof(PROCESSENTRY32);
                BOOL ok = Process32FirstFunc(snap, &lppe);
                while ( ok == TRUE)
                {
                    if (static_cast<int>(lppe.th32ParentProcessID) == m_Pid) // Have my Child...
                    {
                        pid = lppe.th32ProcessID;
                    }
                    lppe.dwSize = sizeof(PROCESSENTRY32);
                    ok = Process32NextFunc(snap, &lppe);
                }
                CloseHandle(snap);
            }
            else
                Log(_("No handle created. Trying to pause directly with cbd.exe..."));
        }

        if (m_State.GetDriver()->UseDebugBreakProcess())
        {
            if (!DebugBreakProcessFunc)
                Log(_("DebugBreakProcess is not supported, you need Windows XP or newer..."));
            else if (pid > 0)
            {
                Log(_("Trying to pause the running process..."));
                HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
                if (proc)
                {
                    DebugBreakProcessFunc(proc); // yay!
                    CloseHandle(proc);
                }
                else
                    Log(_("Failed."));
            }
        }
        else
        {
            if (m_Pid > 0)
            {
                if (GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0) == 0)
                {
                    Log(wxT("Interupting debugger failed :("));
                    DebugLog(wxT("GenerateConsoleCtrlEvent failed :("));
                    return;
                }
            }
        }
    #endif
        // Notify debugger plugins for end of debug session
        PluginManager *plm = Manager::Get()->GetPluginManager();
        CodeBlocksEvent evt(cbEVT_DEBUGGER_PAUSED);
        plm->NotifyPlugins(evt);
    }
}

void DebuggerGDB::Stop()
{
    // m_Process is PipedProcess I/O; m_Pid is debugger pid
    if (m_pProcess && m_Pid)
    {
        if (!IsStopped())
        {
            // TODO (obfuscated#): Check if this can be implemented on Windows
#ifdef __WXGTK__
            int childPID=m_State.GetDriver()->GetChildPID();
            if (childPID == 0)
            {
                DebugLog(_("Child pid is 0, so we will terminate GDB directly"));
                wxKill(m_Pid, wxSIGTERM);
                return;
            }
#endif
            Break();
        }
        RunCommand(CMD_STOP);
    }
}

void DebuggerGDB::ParseOutput(const wxString& output)
{
    if (!output.IsEmpty() && m_State.HasDriver())
    {
        m_State.GetDriver()->ParseOutput(output);
    }
}

void DebuggerGDB::GetCurrentPosition(wxString &filename, int &line)
{
    if (m_State.HasDriver())
    {
        const Cursor& cursor = m_State.GetDriver()->GetCursor();
        filename = cursor.file;
        line = cursor.line;
    }
    else
    {
        filename = wxEmptyString;
        line = -1;
    }
}

// TODO: should reimplement
void DebuggerGDB::OnAddSymbolFile(wxCommandEvent& WXUNUSED(event))
{
    wxString file = wxFileSelector(_("Choose file to read symbols from"),
                                    _T(""),
                                    _T(""),
                                    _T(""),
                                    _("Executables and libraries|*.exe;*.dll"),
                                    wxFD_OPEN | wxFD_FILE_MUST_EXIST | compatibility::wxHideReadonly);
    if (file.IsEmpty())
        return;
//    Manager::Get()->GetLogManager()->Log(m_PageIndex, _("Adding symbol file: %s"), file.wx_str());
    ConvertToGDBDirectory(file);
//    QueueCommand(new DbgCmd_AddSymbolFile(this, file));
}

void DebuggerGDB::SetupToolsMenu(wxMenu &menu)
{
    if (!GetActiveConfigEx().IsGDB())
        return;
    menu.Append(idMenuInfoFrame,   _("Current stack frame"), _("Displays info about the current (selected) stack frame"));
    menu.Append(idMenuInfoDLL,     _("Loaded libraries"), _("List dynamically loaded libraries (DLL/SO)"));
    menu.Append(idMenuInfoFiles,   _("Targets and files"), _("Displays info on the targets and files being debugged"));
    menu.Append(idMenuInfoFPU,     _("FPU status"), _("Displays the status of the floating point unit"));
    menu.Append(idMenuInfoSignals, _("Signal handling"), _("Displays how the debugger handles various signals"));
    menu.AppendSeparator();

    wxMenu *menuPrint = new wxMenu;
    menuPrint->AppendRadioItem(idMenuInfoPrintElementsUnlimited, _("Unlimited"),
                               _("The full arrays are printed, using this should be most reliable"));
    menuPrint->AppendRadioItem(idMenuInfoPrintElements20, _("20"));
    menuPrint->AppendRadioItem(idMenuInfoPrintElements50, _("50"));
    menuPrint->AppendRadioItem(idMenuInfoPrintElements100, _("100"));
    menu.AppendSubMenu(menuPrint, _("Print Elements"), _("Set limit on string chars or array elements to print"));
    menu.AppendCheckItem(idMenuInfoCatchThrow, _("Catch throw"),
                         _("If enabled the debugger will break when an exception is thronw"));
}

void DebuggerGDB::OnUpdateTools(wxUpdateUIEvent &event)
{
    bool checked = (event.GetId() == idMenuInfoPrintElementsUnlimited && m_printElements==0) ||
                   (event.GetId() == idMenuInfoPrintElements20 && m_printElements==20) ||
                   (event.GetId() == idMenuInfoPrintElements50 && m_printElements==50) ||
                   (event.GetId() == idMenuInfoPrintElements100 && m_printElements==100);
    event.Check(checked);
    event.Enable(IsRunning() && IsStopped());
}

void DebuggerGDB::OnPrintElements(wxCommandEvent &event)
{
    if (event.GetId() == idMenuInfoPrintElementsUnlimited)
        m_printElements = 0;
    else if (event.GetId() == idMenuInfoPrintElements20)
        m_printElements = 20;
    else if (event.GetId() == idMenuInfoPrintElements50)
        m_printElements = 50;
    else if (event.GetId() == idMenuInfoPrintElements100)
        m_printElements = 100;
    else
        return;

    wxString cmd = wxString::Format(wxT("set print elements %d"), m_printElements);
    m_State.GetDriver()->QueueCommand(new DebuggerCmd(m_State.GetDriver(), cmd));
    RequestUpdate(Watches);
}

void DebuggerGDB::OnUpdateCatchThrow(wxUpdateUIEvent &event)
{
    DebuggerConfiguration &config = GetActiveConfigEx();
    event.Enable(config.IsGDB() && IsStopped());
    event.Check(config.GetFlag(DebuggerConfiguration::CatchExceptions));
}

void DebuggerGDB::OnCatchThrow(wxCommandEvent &event)
{
    bool flag = event.IsChecked();
    GetActiveConfigEx().SetFlag(DebuggerConfiguration::CatchExceptions, flag);
    m_State.GetDriver()->EnableCatchingThrow(flag);
}

void DebuggerGDB::OnInfoFrame(wxCommandEvent& WXUNUSED(event))
{
    if (m_State.HasDriver())
        m_State.GetDriver()->InfoFrame();
}

void DebuggerGDB::OnInfoDLL(wxCommandEvent& WXUNUSED(event))
{
    if (m_State.HasDriver())
    {
        m_State.GetDriver()->InfoDLL();
    }
}

void DebuggerGDB::OnInfoFiles(wxCommandEvent& WXUNUSED(event))
{
    if (m_State.HasDriver())
        m_State.GetDriver()->InfoFiles();
}

void DebuggerGDB::OnInfoFPU(wxCommandEvent& WXUNUSED(event))
{
    if (m_State.HasDriver())
        m_State.GetDriver()->InfoFPU();
}

void DebuggerGDB::OnInfoSignals(wxCommandEvent& WXUNUSED(event))
{
    if (m_State.HasDriver())
        m_State.GetDriver()->InfoSignals();
}

void DebuggerGDB::OnGDBOutput(wxCommandEvent& event)
{
    wxString msg = event.GetString();
    if (!msg.IsEmpty())
        ParseOutput(msg);
}

void DebuggerGDB::OnGDBError(wxCommandEvent& event)
{
    wxString msg = event.GetString();
    if (!msg.IsEmpty())
        ParseOutput(msg);
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
    Manager::Get()->GetDebuggerManager()->GetBreakpointDialog()->Reload();
    if (!Manager::IsAppShuttingDown())
    {
        Log(wxString::Format(_("Debugger finished with status %d"), m_LastExitCode));

        if (m_NoDebugInfo)
        {
            cbMessageBox(_("This project/target has no debugging info."
                           "Please change this in the project's build options, re-compile and retry..."),
                         _("Error"), wxICON_STOP);
        }
    }

    // Notify debugger plugins for end of debug session
    PluginManager *plm = Manager::Get()->GetPluginManager();
    CodeBlocksEvent evt(cbEVT_DEBUGGER_FINISHED);
    plm->NotifyPlugins(evt);

    // switch to the user-defined layout when finished debugging
    if (!Manager::IsAppShuttingDown())
        SwitchToPreviousLayout();
    KillConsole();
    MarkAsStopped();

    ///killerbot : run there the post shell commands ?
}

void DebuggerGDB::KillConsole()
{
#ifdef __WXMSW__
    if (m_bIsConsole)
    {
        // remove the CTRL_C handler
        SetConsoleCtrlHandler(HandlerRoutine, FALSE);
        FreeConsole();
        m_bIsConsole = false;
    }
#else
    // kill any linux console
    if ( m_bIsConsole && (m_nConsolePid > 0) )
    {
        ::wxKill(m_nConsolePid);
        m_nConsolePid = 0;
        m_bIsConsole = false;
    }
#endif
}

void DebuggerGDB::CheckIfConsoleIsClosed()
{
#ifndef __WXMSW__
    // Detect if the console is closed by the user and if it is stop the session.
    if (m_stopDebuggerConsoleClosed && m_nConsolePid > 0 && wxKill(m_nConsolePid, wxSIGNONE) != 0)
    {
        AnnoyingDialog dialog(_("Terminal/Console closed"),
                              _("Detected that the Terminal/Console has been closed. "
                                "Do you want to stop the debugging session?"),
                              wxART_QUESTION);
        if (dialog.ShowModal() == AnnoyingDialog::rtNO)
            m_stopDebuggerConsoleClosed = false;
        else
        {
            Stop();
            m_nConsolePid = 0;
        }
    }
#endif
}

bool DebuggerGDB::ShowValueTooltip(int style)
{
    if (!m_pProcess || !IsStopped())
        return false;

    if (!m_State.HasDriver() || !m_State.GetDriver()->IsDebuggingStarted())
        return false;

    if (!GetActiveConfigEx().GetFlag(DebuggerConfiguration::EvalExpression))
        return false;
    if (style != wxSCI_C_DEFAULT && style != wxSCI_C_OPERATOR && style != wxSCI_C_IDENTIFIER &&
        style != wxSCI_C_WORD2 && style != wxSCI_C_GLOBALCLASS)
    {
        return false;
    }
    return true;
}

void DebuggerGDB::OnValueTooltip(const wxString &token, const wxRect &evalRect)
{
    m_State.GetDriver()->EvaluateSymbol(token, evalRect);
}

void DebuggerGDB::CleanupWhenProjectClosed(cbProject *project)
{
    // remove all search dirs stored for this project so we don't have conflicts
    // if a newly opened project happens to use the same memory address
    GetSearchDirs(project).clear();

    // the same for remote debugging
    GetRemoteDebuggingMap(project).clear();

    // remove all breakpoints belonging to the closed project
    DeleteAllProjectBreakpoints(project);
    // FIXME (#obfuscated): Optimize this when multiple projects are closed
    //                      (during workspace close operation for exmaple).
    cbBreakpointsDlg *dlg = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
    dlg->Reload();
}

void DebuggerGDB::OnIdle(wxIdleEvent& event)
{
    if (m_pProcess && ((PipedProcess*)m_pProcess)->HasInput())
        event.RequestMore();
    else
        event.Skip();
}

void DebuggerGDB::OnTimer(cb_unused wxTimerEvent& event)
{
    // send any buffered (previous) output
    ParseOutput(wxEmptyString);

    CheckIfConsoleIsClosed();

    wxWakeUpIdle();
}

void DebuggerGDB::OnShowFile(wxCommandEvent& event)
{
    SyncEditor(event.GetString(), event.GetInt(), false);
}

void DebuggerGDB::DebuggeeContinued()
{
    m_TemporaryBreak = false;
}

void DebuggerGDB::OnCursorChanged(wxCommandEvent& WXUNUSED(event))
{
    if (m_TemporaryBreak)
        return;

    if (m_State.HasDriver())
    {
        const Cursor& cursor = m_State.GetDriver()->GetCursor();
        // checking if driver is stopped is redundant because it would only
        // send us this event if it was stopped anyway
        if (/*m_State.GetDriver()->IsStopped() &&*/ cursor.changed)
        {
            bool autoSwitch = cbDebuggerCommonConfig::GetFlag(cbDebuggerCommonConfig::AutoSwitchFrame);

            MarkAllWatchesAsUnchanged();

            // if the cursor line is invalid and the auto switch is on,
            // we don't sync the editor, because there is no line to sync to
            // and also we are going to execute a backtrace command hoping to find a valid frame.
            if (!autoSwitch || cursor.line != -1)
                SyncEditor(cursor.file, cursor.line);

            BringCBToFront();
            if (cursor.line != -1)
                Log(wxString::Format(_("At %s:%ld"), cursor.file.wx_str(), cursor.line));
            else
                Log(wxString::Format(_("In %s (%s)"), cursor.function.wx_str(), cursor.file.wx_str()));

            // update watches
            DebuggerManager *dbg_manager = Manager::Get()->GetDebuggerManager();

            if (IsWindowReallyShown(dbg_manager->GetWatchesDialog()->GetWindow()))
                DoWatches();

            // update CPU registers
            if (dbg_manager->UpdateCPURegisters())
                RunCommand(CMD_REGISTERS);

            // update callstack
            if (dbg_manager->UpdateBacktrace())
                RunCommand(CMD_BACKTRACE);
            else
            {
                if (cursor.line == -1 && autoSwitch)
                    RunCommand(CMD_BACKTRACE);
            }

            // update disassembly
            if (dbg_manager->UpdateDisassembly())
            {
                unsigned long int addrL;
                cursor.address.ToULong(&addrL, 16);
                //if zero addr, don't attempt disassembly
                if (addrL && !dbg_manager->GetDisassemblyDialog()->SetActiveAddress(addrL))
                    RunCommand(CMD_DISASSEMBLE);
            }

            // update memory examiner
            if (dbg_manager->UpdateExamineMemory())
                RunCommand(CMD_MEMORYDUMP);

            // update running threads
            if (dbg_manager->UpdateThreads())
                RunCommand(CMD_RUNNINGTHREADS);
        }
    }
}

cb::shared_ptr<cbWatch> DebuggerGDB::AddWatch(const wxString& symbol)
{
    cb::shared_ptr<GDBWatch> watch(new GDBWatch(CleanStringValue(symbol)));
    m_watches.push_back(watch);

    if (m_pProcess)
        m_State.GetDriver()->UpdateWatch(m_watches.back());

    return watch;
}

void DebuggerGDB::AddWatchNoUpdate(const cb::shared_ptr<GDBWatch> &watch)
{
    m_watches.push_back(watch);
}

void DebuggerGDB::DeleteWatch(cb::shared_ptr<cbWatch> watch)
{
    m_watches.erase(std::find(m_watches.begin(), m_watches.end(), watch));
}

bool DebuggerGDB::HasWatch(cb::shared_ptr<cbWatch> watch)
{
    WatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), watch);
    if (it != m_watches.end())
        return true;
    else
        return watch == m_localsWatch || watch == m_funcArgsWatch;
}

void DebuggerGDB::ShowWatchProperties(cb::shared_ptr<cbWatch> watch)
{
    // not supported for child nodes!
    if (watch->GetParent())
        return;

    cb::shared_ptr<GDBWatch> real_watch = cb::static_pointer_cast<GDBWatch>(watch);
    EditWatchDlg dlg(real_watch, nullptr);
    if (dlg.ShowModal() == wxID_OK)
        DoWatches();
}

bool DebuggerGDB::SetWatchValue(cb::shared_ptr<cbWatch> watch, const wxString &value)
{
    if (!HasWatch(cbGetRootWatch(watch)))
        return false;

    if (!m_State.HasDriver())
        return false;

    wxString full_symbol;
    cb::shared_ptr<cbWatch> temp_watch = watch;
    while (temp_watch)
    {
        wxString symbol;
        temp_watch->GetSymbol(symbol);
        temp_watch = temp_watch->GetParent();

        if (symbol.find(wxT('*')) != wxString::npos || symbol.find(wxT('&')) != wxString::npos)
            symbol = wxT('(') + symbol + wxT(')');

        if (full_symbol.empty())
            full_symbol = symbol;
        else
            full_symbol = symbol + wxT('.') + full_symbol;
    }

    DebuggerDriver* driver = m_State.GetDriver();
    driver->SetVarValue(full_symbol, value);
    DoWatches();
    return true;
}

void DebuggerGDB::ExpandWatch(cb_unused cb::shared_ptr<cbWatch> watch) // TODO: shouldn't this do something?
{
}

void DebuggerGDB::CollapseWatch(cb_unused cb::shared_ptr<cbWatch> watch)
{
}

void DebuggerGDB::MarkAllWatchesAsUnchanged()
{
    if (m_localsWatch)
        m_localsWatch->MarkAsChangedRecursive(false);
    if (m_funcArgsWatch)
        m_funcArgsWatch->MarkAsChangedRecursive(false);

    for (WatchesContainer::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
        (*it)->MarkAsChangedRecursive(false);
}

void DebuggerGDB::OnWatchesContextMenu(wxMenu &menu, const cbWatch &watch, wxObject *property, int &disabledMenus)
{
    wxString type, symbol;
    watch.GetType(type);
    watch.GetSymbol(symbol);

    if (IsPointerType(type))
    {
        menu.InsertSeparator(0);
        menu.Insert(0, idMenuWatchDereference, _("Dereference ") + symbol);
        m_watchToDereferenceSymbol = symbol;
        m_watchToDereferenceProperty = property;
    }

    if (watch.GetParent())
    {
        disabledMenus = WatchesDisabledMenuItems::Rename;
        disabledMenus |= WatchesDisabledMenuItems::Properties;
        disabledMenus |= WatchesDisabledMenuItems::Delete;
        disabledMenus |= WatchesDisabledMenuItems::AddDataBreak;
    }
}

void DebuggerGDB::OnMenuWatchDereference(cb_unused wxCommandEvent& event)
{
    cbWatchesDlg *watches = Manager::Get()->GetDebuggerManager()->GetWatchesDialog();
    if (!watches)
        return;

    watches->RenameWatch(m_watchToDereferenceProperty, wxT("*") + m_watchToDereferenceSymbol);
    m_watchToDereferenceProperty = NULL;
    m_watchToDereferenceSymbol = wxEmptyString;
}

void DebuggerGDB::AttachToProcess(const wxString& pid)
{
    if (!pid.IsEmpty())
    {
        pid.ToLong((long*)&m_PidToAttach);
        Debug(false);
    }
}

void DebuggerGDB::DetachFromProcess()
{
    m_State.GetDriver()->Detach();
    m_PidToAttach = 0;
    m_State.GetDriver()->Stop();
}

bool DebuggerGDB::IsAttachedToProcess() const
{
    return m_PidToAttach != 0;
}

bool DebuggerGDB::CompilerFinished(bool compilerFailed, StartType startType)
{
    if (compilerFailed || startType == StartTypeUnknown)
        return false;
    if (DoDebug(startType == StartTypeStepInto) != 0)
        return false;
    return true;
}

void DebuggerGDB::OnBuildTargetSelected(CodeBlocksEvent& event)
{
    // verify that the project that sent it, is the one we 're debugging
    // and that a project is loaded
    if (m_pProject && event.GetProject() == m_pProject)
        m_ActiveBuildTarget = event.GetBuildTargetName();
}
