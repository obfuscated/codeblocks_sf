/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
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
#include <messagemanager.h>
#include <projectmanager.h>
#include <pluginmanager.h>
#include <editormanager.h>
#include <macrosmanager.h>
#include <cbeditor.h>
#include <projectbuildtarget.h>
#include <sdk_events.h>
#include <editarraystringdlg.h>
#include <compilerfactory.h>
#include <xtra_res.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>

#include "debuggergdb.h"
#include "debuggerdriver.h"
#include "debuggeroptionsdlg.h"
#include "debuggertree.h"
#include "editbreakpointdlg.h"
#include "editwatchesdlg.h"
#include "examinememorydlg.h"
#include "threadsdlg.h"
#include "editwatchdlg.h"
#include "globals.h"

#ifdef __WXMSW__
    #include <winbase.h> //For GetShortPathName()...only for windows systems
#endif

#ifndef CB_PRECOMP
    #include "cbproject.h"
#endif

#define implement_debugger_toolbar

// valid debugger command constants
enum DebugCommandConst
{
    CMD_CONTINUE,
    CMD_STEP,
    CMD_STEPIN,
    CMD_STEP_INSTR,
    CMD_STOP,
    CMD_BACKTRACE,
    CMD_DISASSEMBLE,
    CMD_REGISTERS,
    CMD_MEMORYDUMP,
    CMD_RUNNINGTHREADS,
};

const wxString g_EscapeChars = wxChar(26);

int idMenuDebug = XRCID("idDebuggerMenuDebug");
int idMenuRunToCursor = XRCID("idDebuggerMenuRunToCursor");
int idMenuNext = XRCID("idDebuggerMenuNext");
int idMenuStep = XRCID("idDebuggerMenuStep");
int idMenuNextInstr = XRCID("idDebuggerMenuNextInstr");
int idMenuStepOut = XRCID("idDebuggerMenuStepOut");
int idMenuStop = XRCID("idDebuggerMenuStop");
int idMenuContinue = XRCID("idDebuggerMenuContinue");
int idMenuToggleBreakpoint = XRCID("idDebuggerMenuToggleBreakpoint");
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

    EVT_EDITOR_BREAKPOINT_ADD(DebuggerGDB::OnBreakpointAdd)
    EVT_EDITOR_BREAKPOINT_EDIT(DebuggerGDB::OnBreakpointEdit)
    EVT_EDITOR_BREAKPOINT_DELETE(DebuggerGDB::OnBreakpointDelete)
    EVT_EDITOR_TOOLTIP(DebuggerGDB::OnValueTooltip)
    EVT_EDITOR_OPEN(DebuggerGDB::OnEditorOpened)

    EVT_PROJECT_ACTIVATE(DebuggerGDB::OnProjectActivated)
    EVT_PROJECT_CLOSE(DebuggerGDB::OnProjectClosed)

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
    m_pProject(0)
{
    if(!Manager::LoadResource(_T("debugger.zip")))
    {
        NotifyMissingFile(_T("debugger.zip"));
    }

    m_TimerPollDebugger.SetOwner(this, idTimerPollDebugger);
}

void DebuggerGDB::OnAttach()
{
    MessageManager* msgMan = Manager::Get()->GetMessageManager();
    m_pLog = new SimpleTextLog(true);
    m_PageIndex = msgMan->AddLog(m_pLog, _("Debugger"));
    // set log image
    wxBitmap bmp;
    wxString prefix = ConfigManager::GetDataFolder() + _T("/images/");
    bmp = cbLoadBitmap(prefix + _T("misc_16x16.png"), wxBITMAP_TYPE_PNG);
    Manager::Get()->GetMessageManager()->SetLogImage(m_pLog, bmp);

    m_HasDebugLog = Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("debug_log"), false);
    if (m_HasDebugLog)
    {
        m_pDbgLog = new SimpleTextLog(true);
        m_DbgPageIndex = msgMan->AddLog(m_pDbgLog, _("Debugger (debug)"));
        // set log image
        bmp = cbLoadBitmap(prefix + _T("contents_16x16.png"), wxBITMAP_TYPE_PNG);
        Manager::Get()->GetMessageManager()->SetLogImage(m_pDbgLog, bmp);
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
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    evt.name = _T("CPURegistersPane");
    evt.title = _("CPU Registers");
    evt.pWindow = m_pCPURegisters;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(350, 250);
    evt.floatingSize.Set(350, 250);
    evt.minimumSize.Set(150, 150);
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    evt.name = _T("CallStackPane");
    evt.title = _("Call stack");
    evt.pWindow = m_pBacktrace;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(150, 150);
    evt.floatingSize.Set(450, 150);
    evt.minimumSize.Set(150, 150);
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    evt.name = _T("WatchesPane");
    evt.title = _("Watches");
    evt.pWindow = m_pTree;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(150, 250);
    evt.floatingSize.Set(150, 250);
    evt.minimumSize.Set(150, 150);
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    evt.name = _T("BreakpointsPane");
    evt.title = _("Breakpoints");
    evt.pWindow = m_pBreakpointsWindow;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(350, 250);
    evt.floatingSize.Set(350, 250);
    evt.minimumSize.Set(150, 150);
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    evt.name = _T("ExamineMemoryPane");
    evt.title = _("Memory");
    evt.pWindow = m_pExamineMemoryDlg;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(450, 250);
    evt.floatingSize.Set(450, 250);
    evt.minimumSize.Set(350, 150);
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    evt.name = _T("ThreadsPane");
    evt.title = _("Running threads");
    evt.pWindow = m_pThreadsDlg;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(350, 75);
    evt.floatingSize.Set(450, 75);
    evt.minimumSize.Set(250, 75);
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);
}

void DebuggerGDB::OnRelease(bool appShutDown)
{
    if (m_State.GetDriver())
        m_State.GetDriver()->SetDebugWindows(0, 0, 0, 0, 0);

    if (m_pThreadsDlg)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pThreadsDlg;
        Manager::Get()->GetAppWindow()->ProcessEvent(evt);
        m_pThreadsDlg->Destroy();
    }
    m_pThreadsDlg = 0;

    if (m_pExamineMemoryDlg)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pExamineMemoryDlg;
        Manager::Get()->GetAppWindow()->ProcessEvent(evt);
        m_pExamineMemoryDlg->Destroy();
    }
    m_pExamineMemoryDlg = 0;

    if (m_pBreakpointsWindow)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pBreakpointsWindow;
        Manager::Get()->GetAppWindow()->ProcessEvent(evt);
        m_pBreakpointsWindow->Destroy();
    }
    m_pBreakpointsWindow = 0;

    if (m_pDisassembly)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pDisassembly;
        Manager::Get()->GetAppWindow()->ProcessEvent(evt);
        m_pDisassembly->Destroy();
    }
    m_pDisassembly = 0;

    if (m_pCPURegisters)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pCPURegisters;
        Manager::Get()->GetAppWindow()->ProcessEvent(evt);
        m_pCPURegisters->Destroy();
    }
    m_pCPURegisters = 0;

    if (m_pBacktrace)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pBacktrace;
        Manager::Get()->GetAppWindow()->ProcessEvent(evt);
        m_pBacktrace->Destroy();
    }
    m_pBacktrace = 0;

    if (m_pTree)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pTree;
        Manager::Get()->GetAppWindow()->ProcessEvent(evt);
        m_pTree->Destroy();
    }
    m_pTree = 0L;

    //Close debug session when appShutDown
    Stop();

    m_State.CleanUp();

    if (Manager::Get()->GetMessageManager())
    {
        if (m_HasDebugLog)
        {
            Manager::Get()->GetMessageManager()->RemoveLog(m_pDbgLog);
            m_pDbgLog->Destroy();
        }
        Manager::Get()->GetMessageManager()->RemoveLog(m_pLog);
        m_pLog->Destroy();
    }
}

DebuggerGDB::~DebuggerGDB()
{
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

void DebuggerGDB::RefreshConfiguration()
{
    // the only thing that we need to change on the fly, is the debugger's debug log
    bool log_visible = Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("debug_log"), false);

    if (!log_visible && m_HasDebugLog)
    {
        Manager::Get()->GetMessageManager()->RemoveLog(m_pDbgLog);
        m_pDbgLog->Destroy();
        m_pDbgLog = 0;
    }
    else if (log_visible && !m_HasDebugLog)
    {
        m_pDbgLog = new SimpleTextLog();
        m_pDbgLog->GetTextControl()->SetFont(m_pLog->GetTextControl()->GetFont());
        m_DbgPageIndex = Manager::Get()->GetMessageManager()->AddLog(m_pDbgLog, _("Debugger (debug)"));
        // set log image
        wxBitmap bmp;
        bmp = cbLoadBitmap(ConfigManager::GetDataFolder() + _T("/images/contents_16x16.png"), wxBITMAP_TYPE_PNG);
        Manager::Get()->GetMessageManager()->SetLogImage(m_pDbgLog, bmp);
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

    wxString s;
    s.Printf(_("Watch '%s'"), w.c_str());
    menu->Insert(2, idMenuDebuggerAddWatch,  s);
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
    toolBar->SetBestFittingSize();
    return true;
#else
    return false;
#endif
}

void DebuggerGDB::Log(const wxString& msg)
{
    if (IsAttached())
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, msg);
}

void DebuggerGDB::DebugLog(const wxString& msg)
{
    // gdb debug messages
    if (IsAttached() && m_HasDebugLog)
        Manager::Get()->GetMessageManager()->Log(m_DbgPageIndex, msg);
}

void DebuggerGDB::DoSwitchLayout(const wxString& config_key)
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("debugger"));
    wxString layout = cfg->Read(config_key, wxEmptyString);
    if (!layout.IsEmpty())
    {
        Manager::Get()->GetMessageManager()->DebugLog(_T("Switching layout to \"%s\""), layout.c_str());
        CodeBlocksLayoutEvent evt(cbEVT_SWITCH_VIEW_LAYOUT, layout);
        Manager::Get()->GetAppWindow()->ProcessEvent(evt);
    }
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
    if (binPath.IsEmpty() || !pathList.Member(wxPathOnly(binPath)))
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

    #ifndef __WXMSW__
        // setup dynamic linker path
        wxSetEnv(_T("LD_LIBRARY_PATH"), _T(".:$LD_LIBRARY_PATH"));
    #endif

    // start the gdb process
    wxLogNull ln; // we perform our own error handling and logging
    m_pProcess = new PipedProcess((void**)&m_pProcess, this, idGDBProcess, true, cwd);
    Manager::Get()->GetMessageManager()->AppendLog(m_PageIndex, _("Starting debugger: "));
    m_Pid = wxExecute(cmd, wxEXEC_ASYNC, m_pProcess);

    if (!m_Pid)
    {
        delete m_pProcess;
        m_pProcess = 0;
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("failed"));
        return -1;
    }
    else if (!m_pProcess->GetOutputStream())
    {
        delete m_pProcess;
        m_pProcess = 0;
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("failed (to get debugger's stdin)"));
        return -2;
    }
    else if (!m_pProcess->GetInputStream())
    {
        delete m_pProcess;
        m_pProcess = 0;
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("failed (to get debugger's stdout)"));
        return -2;
    }
    else if (!m_pProcess->GetErrorStream())
    {
        delete m_pProcess;
        m_pProcess = 0;
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("failed (to get debugger's stderr)"));
        return -2;
    }
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("done"));
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
            Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Adding file: %s"), out.c_str());
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
            Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Adding file: %s"), out.c_str());
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
    return !m_State.GetDriver() || m_State.GetDriver()->IsStopped();
}

int DebuggerGDB::Debug()
{
    // if already running, return
    if (m_pProcess)
        return 1;

    m_pProject = 0;
    m_NoDebugInfo = false;

    // clear the debug log
    if (m_HasDebugLog)
        m_pDbgLog->GetTextControl()->Clear();

    m_pTree->GetTree()->DeleteAllItems();

    // switch to the debugging log and clear it
    MessageManager* msgMan = Manager::Get()->GetMessageManager();
    msgMan->SwitchTo(m_PageIndex);
    m_pLog->GetTextControl()->Clear();

    // can only debug projects or attach to processes
    ProjectManager* prjMan = Manager::Get()->GetProjectManager();
    cbProject* project = prjMan->GetActiveProject();
    if (!project && m_PidToAttach == 0)
        return 2;

    m_pProject = project;

    // compile project/target (if not attaching to a PID)
    if (m_PidToAttach == 0)
    {
        // make sure the target is compiled
        PluginsArray plugins = Manager::Get()->GetPluginManager()->GetCompilerOffers();
        if (plugins.GetCount())
            m_pCompiler = (cbCompilerPlugin*)plugins[0];
        if (m_pCompiler)
        {
            // is the compiler already running?
            if (m_pCompiler->IsRunning())
            {
                msgMan->Log(m_PageIndex, _("Compiler in use..."));
                msgMan->Log(m_PageIndex, _("Aborting debugging session"));
                cbMessageBox(_("The compiler is currently in use. Aborting debugging session..."), _("Compiler running"), wxICON_WARNING);
                return -1;
            }

            msgMan->Log(m_PageIndex, _("Building to ensure sources are up-to-date"));
            m_pCompiler->Build();
            while (m_pCompiler->IsRunning())
            {
                wxMilliSleep(10);
                Manager::Yield();
            }
            msgMan->SwitchTo(m_PageIndex);
            if (m_pCompiler->GetExitCode() != 0)
            {
                msgMan->Log(m_PageIndex, _("Build failed..."));
                msgMan->Log(m_PageIndex, _("Aborting debugging session"));
                cbMessageBox(_("Build failed. Aborting debugging session..."), _("Build failed"), wxICON_WARNING);
                return -1;
            }
            msgMan->Log(m_PageIndex, _("Build succeeded"));
        }
    }

    // select the build target to debug
    ProjectBuildTarget* target = 0;
    Compiler* actualCompiler = 0;
    if (m_PidToAttach == 0)
    {
        wxString tgt = project->GetActiveBuildTarget();
        msgMan->SwitchTo(m_PageIndex);
        msgMan->AppendLog(m_PageIndex, _("Selecting target: "));
        if (!project->BuildTargetValid(tgt, false))
        {
            int tgtIdx = project->SelectTarget(tgtIdx);
            if (tgtIdx == -1)
            {
                msgMan->Log(m_PageIndex, _("canceled"));
                return 3;
            }
            target = project->GetBuildTarget(tgtIdx);
        }
        else
            target = project->GetBuildTarget(tgt);

        // make sure it's not a commands-only target
        if (target->GetTargetType() == ttCommandsOnly)
        {
            cbMessageBox(_("The selected target is only running pre/post build step commands\n"
                        "Can't debug such a target..."), _("Information"), wxICON_INFORMATION);
            msgMan->Log(m_PageIndex, _("aborted"));
            return 3;
        }
        msgMan->Log(m_PageIndex, target->GetTitle());

        // find the target's compiler (to see which debugger to use)
        actualCompiler = CompilerFactory::GetCompiler(target ? target->GetCompilerID() : project->GetCompilerID());
    }
    else
        actualCompiler = CompilerFactory::GetDefaultCompiler();

    if (!actualCompiler)
    {
        wxString msg;
        msg.Printf(_("This %s is configured to use an invalid debugger.\nThe operation failed..."), target ? _("target") : _("project"));
        cbMessageBox(msg, _("Error"), wxICON_ERROR);
        return 9;
    }

    // is gdb accessible, i.e. can we find it?
    wxString cmdexe;
    cmdexe = actualCompiler->GetPrograms().DBG;
    cmdexe.Trim();
    cmdexe.Trim(true);
    if(cmdexe.IsEmpty())
    {
        msgMan->AppendLog(m_PageIndex,_("ERROR: You need to specify a debugger program in the compiler's settings."));
        #ifdef __WXMSW__
        msgMan->Log(m_PageIndex,_("\n(For MinGW compilers, it's 'gdb.exe' (without the quotes))"));
        msgMan->Log(m_PageIndex,_("\n(For MSVC compilers, it's 'cdb.exe' (without the quotes))"));
        #else
        msgMan->Log(m_PageIndex,_("\n(For GCC compilers, it's 'gdb' (without the quotes))"));
        #endif
        return -1;
    }

    // access the gdb executable name
    cmdexe = FindDebuggerExecutable(actualCompiler);
    if (cmdexe.IsEmpty())
    {
        cbMessageBox(_("The debugger executable is not set.\n"
                       "To set it, go to \"Settings/Compiler and debugger\", switch to the \"Programs\" tab,\n"
                       "and select the debugger program."), _("Error"), wxICON_ERROR);
        msgMan->Log(m_PageIndex, _("Aborted"));
        return 4;
    }

    // switch to the user-defined layout for debugging
    DoSwitchLayout(_T("layout_start"));

    // start debugger driver based on target compiler, or default compiler if no target
    if (!m_State.StartDriver(target))
    {
        cbMessageBox(_T("Could not decide which debugger to use!"), _T("Error"), wxICON_ERROR);
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
                if (it == project)
                    continue;
                AddSourceDir(it->GetBasePath());
                AddSourceDir(it->GetCommonTopLevelPath());
            }
        }
        // lastly, add THE project as source dir
        AddSourceDir(project->GetBasePath());
        AddSourceDir(project->GetCommonTopLevelPath());

        // switch to output dir
        wxString path = UnixFilename(target->GetWorkingDir());
        if (!path.IsEmpty())
        {
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(path); // apply env vars
            cmd.Clear();
            ConvertToGDBDirectory(path);
            if (path != _T(".")) // avoid silly message "changing to ."
            {
                msgMan->Log(m_PageIndex, _("Changing directory to: %s"), path.c_str());
                m_State.GetDriver()->SetWorkingDirectory(path);
            }
        }

        if (target && !target->GetExecutionParameters().IsEmpty())
            m_State.GetDriver()->SetArguments(target->GetExecutionParameters());

        // set the file to debug
        // (depends on the target type)
        wxString debuggee = GetDebuggee(target);
        if (debuggee.IsEmpty())
            return -3;
        cmdline = m_State.GetDriver()->GetCommandLine(cmdexe, debuggee);
    }
    else // m_PidToAttach != 0
        cmdline = m_State.GetDriver()->GetCommandLine(cmdexe, m_PidToAttach);

    // start the gdb process
    wxString wdir = project ? project->GetBasePath() : _T(".");
    DebugLog(_T("Command-line: ") + cmdline);
    DebugLog(_T("Working dir : ") + wdir);
    int ret = LaunchProcess(cmdline, wdir);
    if (ret != 0)
        return ret;

    wxString out;
    // start polling gdb's output
    m_TimerPollDebugger.Start(20);

    m_State.GetDriver()->Prepare(target && target->GetTargetType() == ttConsoleOnly);
    m_State.ApplyBreakpoints();

    // Don't issue 'run' if attaching to a process (Bug #1391904)
    if (m_PidToAttach == 0)
        m_State.GetDriver()->Start(false);

    return 0;
}

void DebuggerGDB::AddSourceDir(const wxString& dir)
{
    if (!m_State.GetDriver() || dir.IsEmpty())
        return;
    wxString filename = dir;
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(filename); // apply env vars
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Adding source dir: %s"), filename.c_str());
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

    #ifdef __WXMSW__
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
    #else
        if((str.GetChar(0) != _T('/') && str.GetChar(0) != _T('~')) || base.IsEmpty())
            relative = false;
    #endif

    if(relative)
    {
        #ifdef __WXMSW__
            if(str.Find(_T(':')) != -1) str = str.Mid(str.Find(_T(':')) + 2, str.Length());
            if(base.Find(_T(':')) != -1) base = base.Mid(base.Find(_T(':')) + 2, base.Length());
        #else
            if(str.GetChar(0) == _T('/')) str = str.Mid(1, str.Length());
            else if(str.GetChar(0) == _T('~')) str = str.Mid(2, str.Length());
            if(base.GetChar(0) == _T('/')) base = base.Mid(1, base.Length());
            else if(base.GetChar(0) == _T('~')) base = base.Mid(2, base.Length());
        #endif

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
        Manager::Get()->GetMessageManager()->Log(m_DbgPageIndex, _T("> ") + cmd);
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
            if (m_State.GetDriver())
            {
                Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Continuing..."));
                m_State.GetDriver()->Continue();
            }
//            QueueCommand(new DebuggerCmd(this, _T("cont")));
            break;

        case CMD_STEP:
            ClearActiveMarkFromAllEditors();
            if (m_State.GetDriver())
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
            if (m_State.GetDriver())
                m_State.GetDriver()->StepInstruction();
//            QueueCommand(new DebuggerCmd(this, _T("nexti")));
            break;

        case CMD_STEPIN:
            ClearActiveMarkFromAllEditors();
            if (m_State.GetDriver())
                m_State.GetDriver()->StepIn();
//            QueueCommand(new DebuggerCmd(this, _T("step")));
            break;

        case CMD_STOP:
            ClearActiveMarkFromAllEditors();
            if (m_State.GetDriver())
                m_State.GetDriver()->Stop();
//            QueueCommand(new DebuggerCmd(this, _T("quit")));
            break;

        case CMD_BACKTRACE:
//            Manager::Get()->GetMessageManager()->Log(m_PageIndex, "Running back-trace...");
            if (m_State.GetDriver())
                m_State.GetDriver()->Backtrace();
            break;

        case CMD_DISASSEMBLE:
        {
//            Manager::Get()->GetMessageManager()->Log(m_PageIndex, "Disassemblying...");
            if (m_State.GetDriver())
                m_State.GetDriver()->Disassemble();
            break;
        }

        case CMD_REGISTERS:
        {
//            Manager::Get()->GetMessageManager()->Log(m_PageIndex, "Displaying registers...");
            if (m_State.GetDriver())
                m_State.GetDriver()->CPURegisters();
            break;
        }

        case CMD_MEMORYDUMP:
        {
            if (m_State.GetDriver())
                m_State.GetDriver()->MemoryDump();
        }

        case CMD_RUNNINGTHREADS:
        {
            if (m_State.GetDriver())
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
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    RunCommand(CMD_REGISTERS);
}

void DebuggerGDB::Disassemble()
{
    // show it
    CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
    evt.pWindow = m_pDisassembly;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    RunCommand(CMD_DISASSEMBLE);
}

void DebuggerGDB::Backtrace()
{
    m_pBacktrace->Clear();

    // show it
    CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
    evt.pWindow = m_pBacktrace;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    RunCommand(CMD_BACKTRACE);
}

void DebuggerGDB::MemoryDump()
{
    m_pExamineMemoryDlg->Clear();

    // show it
    CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
    evt.pWindow = m_pExamineMemoryDlg;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    RunCommand(CMD_MEMORYDUMP);
}

void DebuggerGDB::RunningThreads()
{
    m_pThreadsDlg->Clear();

    // show it
    CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
    evt.pWindow = m_pThreadsDlg;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

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
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed) return;
    wxString filename = UnixFilename(ed->GetFilename()), lineBuf, cmd;
    cbStyledTextCtrl* stc = ed->GetControl();
    int line = m_HaltAtLine;
    lineBuf = stc->GetLine(line);
    int maxline = stc->GetLineCount();

    unsigned int nLevel = 1;
    while(nLevel && line <= maxline)
    {
        if ((lineBuf.Find(_T('{'))+1) &&
            Validate(lineBuf, _T('{')) &&
            (line > m_HaltAtLine))
        {
            nLevel++;
        }
        if ((lineBuf.Find(_T('}'))+1) && Validate(lineBuf, _T('}')))
            nLevel--;
        if (nLevel)
            lineBuf = stc->GetLine(++line);
    }
    if (line == stc->GetCurrentLine())
        Next();
    else
    {
        ConvertToGDBFile(filename);
        m_State.AddBreakpoint(filename, line, true);
        Continue();
    }
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

void DebuggerGDB::Stop()
{
    if (m_pProcess && m_Pid)
    {
        if (IsStopped())
        {
            RunCommand(CMD_STOP);
            m_pProcess->CloseOutput();
        }
        else
        {
        #ifndef __WXMSW__
            // non-windows gdb can interrupt the running process. yay!
            unsigned long pid = m_State.GetDriver()->GetChildPID();
            wxKill(pid, wxSIGINT);
        #else
            m_pProcess->CloseOutput();
            wxKillError err = m_pProcess->Kill(m_Pid, wxSIGKILL);
            if (err == wxKILL_OK){
/*
                cbMessageBox(_("Debug session terminated!"),
                    _("Debug"), wxOK | wxICON_EXCLAMATION);
*/
            }
        #endif
        }
    }
}

void DebuggerGDB::ParseOutput(const wxString& output)
{
    if (m_State.GetDriver())
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
    bool en = prj || m_PidToAttach != 0;
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    wxMenuBar* mbar = Manager::Get()->GetAppWindow()->GetMenuBar();
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
    }
    else Step();
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

void DebuggerGDB::OnStop(wxCommandEvent& event)
{
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
                                    wxOPEN | wxFILE_MUST_EXIST);
    if (file.IsEmpty())
        return;
//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Adding symbol file: %s"), file.c_str());
    ConvertToGDBDirectory(file);
//    QueueCommand(new DbgCmd_AddSymbolFile(this, file));
}

void DebuggerGDB::OnBacktrace(wxCommandEvent& event)
{
    // show it
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pBacktrace;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    if (event.IsChecked())
        Backtrace();
}

void DebuggerGDB::OnDisassemble(wxCommandEvent& event)
{
    // show it
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pDisassembly;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    if (event.IsChecked())
        Disassemble();
}

void DebuggerGDB::OnRegisters(wxCommandEvent& event)
{
    // show it
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pCPURegisters;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    if (event.IsChecked())
        Registers();
}

void DebuggerGDB::OnViewWatches(wxCommandEvent& event)
{
    // show it
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pTree;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    if (event.IsChecked())
        DoWatches();
}

void DebuggerGDB::OnBreakpoints(wxCommandEvent& event)
{
    // show it
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pBreakpointsWindow;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);
}

void DebuggerGDB::OnExamineMemory(wxCommandEvent& event)
{
    // show it
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pExamineMemoryDlg;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    if (event.IsChecked())
        MemoryDump();
}

void DebuggerGDB::OnRunningThreads(wxCommandEvent& event)
{
    // show it
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pThreadsDlg;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

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
    if (m_State.GetDriver())
    {
        m_State.GetDriver()->InfoFrame();
    }
}

void DebuggerGDB::OnInfoDLL(wxCommandEvent& event)
{
    if (m_State.GetDriver())
    {
        m_State.GetDriver()->InfoDLL();
    }
}

void DebuggerGDB::OnInfoFiles(wxCommandEvent& event)
{
    if (m_State.GetDriver())
    {
        m_State.GetDriver()->InfoFiles();
    }
}

void DebuggerGDB::OnInfoFPU(wxCommandEvent& event)
{
    if (m_State.GetDriver())
    {
        m_State.GetDriver()->InfoFPU();
    }
}

void DebuggerGDB::OnInfoSignals(wxCommandEvent& event)
{
    if (m_State.GetDriver())
    {
        m_State.GetDriver()->InfoSignals();
    }
}

void DebuggerGDB::OnGDBOutput(wxCommandEvent& event)
{
    wxString msg = event.GetString();
    if (!msg.IsEmpty())
    {
//        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("O>>> %s"), msg.c_str());
        ParseOutput(msg);
    }
}

void DebuggerGDB::OnGDBError(wxCommandEvent& event)
{
    wxString msg = event.GetString();
    if (!msg.IsEmpty())
    {
//        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("E>>> %s"), msg.c_str());
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
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Debugger finished with status %d"), m_LastExitCode);

    if (m_NoDebugInfo)
    {
        cbMessageBox(_("This project/target has no debugging info."
                        "Please change this in the project's build options and retry..."),
                        _("Error"),
                        wxICON_STOP);
    }

    // switch to the user-defined layout when finished debugging
    DoSwitchLayout(_T("layout_end"));
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
    if (ed)
    {
        for (unsigned int i = 0; i < m_State.GetBreakpoints().GetCount(); ++i)
        {
            DebuggerBreakpoint* bp = m_State.GetBreakpoints()[i];
            if (bp->filename.Matches(ed->GetFilename()))
                ed->ToggleBreakpoint(bp->line, false);
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

    if (!m_State.GetDriver() || !m_pProject)
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

    // remove all breakpoints belonging to the closed project
    m_State.RemoveAllProjectBreakpoints(event.GetProject());
    if (m_pBreakpointsWindow)
        m_pBreakpointsWindow->Refresh();

    // when a project closes, make sure it's not the actively debugged project.
    // if so, end debugging immediately!

    if (!m_State.GetDriver() || !m_pProject)
        return;

    if (event.GetProject() == m_pProject)
    {
        cbMessageBox(_("The project you were debugging has closed.\n"
                        "The debugging session will terminate immediately."),
                    _("Warning"),
                    wxICON_WARNING);
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
    wxWakeUpIdle();
}

void DebuggerGDB::OnWatchesChanged(wxCommandEvent& event)
{
    DoWatches();
}

void DebuggerGDB::OnCursorChanged(wxCommandEvent& event)
{
    if (m_State.GetDriver())
    {
        const Cursor& cursor = m_State.GetDriver()->GetCursor();
        if (m_State.GetDriver()->IsStopped() && cursor.changed)
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
                long int addrL;
                cursor.address.ToLong(&addrL, 16);
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
