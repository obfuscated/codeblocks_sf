/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/frame.h> // wxFrame
    #include <wx/menu.h>
    #include "cbplugin.h"
    #include "compiler.h" // GetSwitches
    #include "sdk_events.h"
    #include "manager.h"
    #include "projectbuildtarget.h"
    #include "cbproject.h"
    #include "logmanager.h"
    #include "editormanager.h"
    #include "cbeditor.h"
    #include "projectmanager.h"
    #include "infowindow.h"
    #include "macrosmanager.h"
    #include "configmanager.h"
    #include "debuggermanager.h"
#endif

#include <wx/toolbar.h>
#include "annoyingdialog.h"
#include "cbdebugger_interfaces.h"
#include "cbstyledtextctrl.h"
#include "debuggermanager.h"
#include "editor_hooks.h"
#include "loggers.h"

#ifndef __WXMSW__
    #include <errno.h>
    // needed for the kill system call
    #include <signal.h>
    #include <sys/types.h>
#endif



cbPlugin::cbPlugin()
    : m_Type(ptNone),
    m_IsAttached(false)
{
    SetEvtHandlerEnabled(false);
}

cbPlugin::~cbPlugin()
{
}

void cbPlugin::Attach()
{
    if (m_IsAttached)
        return;
    wxWindow* window = Manager::Get()->GetAppWindow();
    if (window)
    {
        // push ourself in the application's event handling chain...
        window->PushEventHandler(this);
    }
    m_IsAttached = true;
    OnAttach();
    SetEvtHandlerEnabled(true);

    CodeBlocksEvent event(cbEVT_PLUGIN_ATTACHED);
    event.SetPlugin(this);
    // post event in the host's event queue
    Manager::Get()->ProcessEvent(event);
}

void cbPlugin::Release(bool appShutDown)
{
    if (!m_IsAttached)
        return;
    m_IsAttached = false;
    SetEvtHandlerEnabled(false);
    OnRelease(appShutDown);

    CodeBlocksEvent event(cbEVT_PLUGIN_RELEASED);
    event.SetPlugin(this);
    // ask the host to process this event immediately
    // it must be done this way, because if the host references
    // us (through event.GetEventObject()), we might not be valid at that time
    // (while, now, we are...)
    Manager::Get()->ProcessEvent(event);

    if (appShutDown)
        return; // nothing more to do, if the app is shutting down

    wxWindow* window = Manager::Get()->GetAppWindow();
    if (window)
    {
        // remove ourself from the application's event handling chain...
        window->RemoveEventHandler(this);
    }
}

void cbPlugin::NotImplemented(const wxString& log) const
{
    Manager::Get()->GetLogManager()->DebugLog(log + _T(" : not implemented"));
}

/////
///// cbCompilerPlugin
/////

cbCompilerPlugin::cbCompilerPlugin()
{
    m_Type = ptCompiler;
}

/////
///// cbDebuggerPlugin
/////

cbDebuggerPlugin::cbDebuggerPlugin(const wxString &guiName, const wxString &settingsName) :
    m_toolbar(NULL),
    m_pCompiler(NULL),
    m_WaitingCompilerToFinish(false),
    m_EditorHookId(-1),
    m_StartType(StartTypeUnknown),
    m_DragInProgress(false),
    m_ActiveConfig(0),
    m_LogPageIndex(-1),
    m_lastLineWasNormal(true),
    m_guiName(guiName),
    m_settingsName(settingsName)
{
    m_Type = ptDebugger;
}


void cbDebuggerPlugin::OnAttach()
{
    Manager::Get()->GetDebuggerManager()->RegisterDebugger(this);

    OnAttachReal();

    typedef cbEventFunctor<cbDebuggerPlugin, CodeBlocksEvent> Event;

    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_OPEN, new Event(this, &cbDebuggerPlugin::OnEditorOpened));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE, new Event(this, &cbDebuggerPlugin::OnProjectActivated));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE, new Event(this, &cbDebuggerPlugin::OnProjectClosed));

    Manager::Get()->RegisterEventSink(cbEVT_COMPILER_FINISHED, new Event(this, &cbDebuggerPlugin::OnCompilerFinished));

    EditorHooks::HookFunctorBase *editor_hook;
    editor_hook = new EditorHooks::HookFunctor<cbDebuggerPlugin>(this, &cbDebuggerPlugin::OnEditorHook);
    m_EditorHookId = EditorHooks::RegisterHook(editor_hook);
    m_StartType = StartTypeUnknown;

    if (SupportsFeature(cbDebuggerFeature::ValueTooltips))
        RegisterValueTooltip();
}

void cbDebuggerPlugin::OnRelease(bool appShutDown)
{
    EditorHooks::UnregisterHook(m_EditorHookId, true);
    Manager::Get()->RemoveAllEventSinksFor(this);

    OnReleaseReal(appShutDown);

    Manager::Get()->GetDebuggerManager()->UnregisterDebugger(this);
}

void cbDebuggerPlugin::BuildMenu(wxMenuBar* menuBar)
{
    if (!IsAttached())
        return;
    Manager::Get()->GetDebuggerManager()->GetMenu();
}

wxString cbDebuggerPlugin::GetEditorWordAtCaret(const wxPoint *mousePosition)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return wxEmptyString;
    cbStyledTextCtrl* control = ed->GetControl();
    if (!control)
        return wxEmptyString;

    wxString selected_text = control->GetSelectedText();
    if (selected_text != wxEmptyString)
    {
        selected_text.Trim(true);
        selected_text.Trim(false);

        wxString::size_type pos = selected_text.find(wxT('\n'));
        if (pos != wxString::npos)
        {
            selected_text.Remove(pos, selected_text.length() - pos);
            selected_text.Trim(true);
            selected_text.Trim(false);
        }
        // check if the mouse is over the selected text
        if (mousePosition)
        {
            int startPos = control->GetSelectionStart();
            int endPos = control->GetSelectionEnd();
            wxPoint startPoint = control->PointFromPosition(startPos);
            wxPoint endPoint = control->PointFromPosition(endPos);
            int endLine = control->LineFromPosition(endPos);
            int textHeight = control->TextHeight(endLine);
            endPoint.y+=textHeight;
            if (wxRect(startPoint, endPoint).Contains(*mousePosition))
                return selected_text;
            else
                return wxEmptyString;
        }
        else
            return selected_text;
    }

    if (mousePosition)
    {
        int pos = control->PositionFromPoint(*mousePosition);
        int start = control->WordStartPosition(pos, true);
        int end = control->WordEndPosition(pos, true);
        selected_text = control->GetTextRange(start, end);
    }
    else
    {
        int start = control->WordStartPosition(control->GetCurrentPos(), true);
        int end = control->WordEndPosition(control->GetCurrentPos(), true);
        selected_text = control->GetTextRange(start, end);
    }
    return selected_text;
}

void cbDebuggerPlugin::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    cbDebuggerPlugin *active_plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    if (active_plugin != this)
        return;

    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (!prj)
        return;
    if (!IsAttached())
        return;
    // we 're only interested in editor menus
    // we 'll add a "debug watches" entry only when the debugger is running...
    if (type != mtEditorManager || !menu)
        return;

    wxString word;
    if (IsRunning())
    {
        // has to have a word under the caret...
        word = GetEditorWordAtCaret();
    }
    Manager::Get()->GetDebuggerManager()->BuildContextMenu(*menu, word, IsRunning());
}

bool cbDebuggerPlugin::BuildToolBar(wxToolBar* toolBar)
{
    return false;
}

bool cbDebuggerPlugin::ToolMenuEnabled() const
{
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();

    bool en = (prj && !prj->GetCurrentlyCompilingTarget()) || IsAttachedToProcess();
    return IsRunning() && en;
}

cbDebuggerConfiguration& cbDebuggerPlugin::GetActiveConfig()
{
    DebuggerManager::RegisteredPlugins &allPlugins = Manager::Get()->GetDebuggerManager()->GetAllDebuggers();

    DebuggerManager::RegisteredPlugins::iterator it = allPlugins.find(this);
    if (it == allPlugins.end())
        cbAssert(false);
    cbDebuggerConfiguration *config = it->second.GetConfiguration(m_ActiveConfig);
    if (!config)
        return *it->second.GetConfigurations().front();
    else
        return *config;
}

void cbDebuggerPlugin::SetActiveConfig(int index)
{
    m_ActiveConfig = index;
}

int cbDebuggerPlugin::GetIndexOfActiveConfig() const
{
    return m_ActiveConfig;
}

void cbDebuggerPlugin::ClearActiveMarkFromAllEditors()
{
    EditorManager* edMan = Manager::Get()->GetEditorManager();
    for (int i = 0; i < edMan->GetEditorsCount(); ++i)
    {
        cbEditor* ed = edMan->GetBuiltinEditor(i);
        if (ed)
            ed->SetDebugLine(-1);
    }
}

cbDebuggerPlugin::SyncEditorResult cbDebuggerPlugin::SyncEditor(const wxString& filename, int line, bool setMarker)
{
    if (setMarker)
    {
        EditorManager* edMan = Manager::Get()->GetEditorManager();
        for (int i = 0; i < edMan->GetEditorsCount(); ++i)
        {
            cbEditor* ed = edMan->GetBuiltinEditor(i);
            if (ed)
                ed->SetDebugLine(-1);
        }
    }
    FileType ft = FileTypeOf(filename);
    if (ft != ftSource && ft != ftHeader && ft != ftResource)
    {
        // if the line is >= 0 and ft == ftOther assume, that we are in header without extension
        if (line < 0 || ft != ftOther)
        {
            ShowLog(false);
            Log(_("Unknown file: ") + filename, Logger::error);
            InfoWindow::Display(_("Unknown file"), _("File: ") + filename, 5000);

            return SyncFileUnknown; // don't try to open unknown files
        }
    }

    cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    ProjectFile* f = project ? project->GetFileByFilename(filename, false, true) : 0;

    wxString unixfilename = UnixFilename(filename);
    wxFileName fname(unixfilename);

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
        return SyncOk;
    }
    else
    {
        ShowLog(false);
        Log(_("Cannot open file: ") + filename, Logger::error);
        InfoWindow::Display(_("Cannot open file"), _("File: ") + filename, 5000);

        return SyncFileNotFound;
    }
}

bool HasBreakpoint(cbDebuggerPlugin &plugin, wxString const &filename, int line)
{
    int count = plugin.GetBreakpointsCount();
    for (int ii = 0; ii < count; ++ii)
    {
        const cbBreakpoint::Pointer &b = plugin.GetBreakpoint(ii);

        if (b->GetLocation() == filename && b->GetLine() == line)
            return true;
    }
    return false;
}

void cbDebuggerPlugin::EditorLinesAddedOrRemoved(cbEditor* editor, int startline, int lines)
{
    // here we keep the breakpoints in sync with the editors
    // (whenever lines are added or removed)
    if (!editor || lines == 0)
        return;

    const wxString& filename = editor->GetFilename();

    std::vector<int> breakpoints_for_file;
    int count = GetBreakpointsCount();
    for (int ii = 0; ii < count; ++ii)
    {
        const cbBreakpoint::Pointer &b = GetBreakpoint(ii);

        if (b->GetLocation() == filename)
        {
            breakpoints_for_file.push_back(ii);
        }
    }

    if (lines < 0)
    {
        // removed lines
        // make "lines" positive, for easier reading below
        lines = -lines;
        int endline = startline + lines - 1;

        std::vector<cbBreakpoint::Pointer> to_remove;

        for (std::vector<int>::iterator it = breakpoints_for_file.begin(); it != breakpoints_for_file.end(); ++it)
        {
            const cbBreakpoint::Pointer &b = GetBreakpoint(*it);
            if (b->GetLine() > endline)
                ShiftBreakpoint(*it, -lines);
            else if (b->GetLine() >= startline && b->GetLine() <= endline)
                to_remove.push_back(b);
        }

        for (std::vector<cbBreakpoint::Pointer>::iterator it = to_remove.begin(); it != to_remove.end(); ++it)
            DeleteBreakpoint(*it);

        // special case:
        // when deleting a block of lines, if these lines contain at least one marker,
        // one marker is retained at the cursor position.
        // In our case here, this means that all breakpoints will be deleted in the range
        // but one "orphan" breakpoint (i.e. editor mark only, no actual breakpoint behind it)
        // will be visible on the line with the cursor.
        //
        // If we really have an "orphan", we remove it.
        bool is_orphan = !HasBreakpoint(*this, editor->GetFilename(), endline - lines + 1);
        if (is_orphan)
            editor->RemoveBreakpoint(endline - lines, false);
    }
    else
    {
        for (std::vector<int>::iterator it = breakpoints_for_file.begin(); it != breakpoints_for_file.end(); ++it)
        {
            const cbBreakpoint::Pointer &b = GetBreakpoint(*it);
            if (b->GetLine() > startline)
                ShiftBreakpoint(*it, lines);
        }
    }
    cbBreakpointsDlg *dlg = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
    dlg->Reload();
}

void cbDebuggerPlugin::OnEditorOpened(CodeBlocksEvent& event)
{
    // when an editor opens, look if we have breakpoints for it
    // and notify it...
    EditorBase* ed = event.GetEditor();
    if (ed)
    {
        ed->RefreshBreakpointMarkers();

        if (IsRunning())
        {
            wxString filename;
            int line;
            GetCurrentPosition(filename, line);

            wxFileName edFileName(ed->GetFilename());
            edFileName.Normalize();

            wxFileName dbgFileName(filename);
            dbgFileName.Normalize();
            if (dbgFileName.GetFullPath().IsSameAs(edFileName.GetFullPath()) && line != -1)
            {
                ed->SetDebugLine(line - 1);
            }
        }
    }
    event.Skip(); // must do
}

void cbDebuggerPlugin::OnProjectActivated(CodeBlocksEvent& event)
{
    // allow others to catch this
    event.Skip();

    if(this != Manager::Get()->GetDebuggerManager()->GetActiveDebugger())
        return;
    // when a project is activated and it's not the actively debugged project,
    // ask the user to end debugging or re-activate the debugged project.

    if (!IsRunning())
        return;

    if (event.GetProject() != GetProject() && GetProject())
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
            Manager::Get()->GetProjectManager()->SetProject(GetProject());
        }
    }
}

void cbDebuggerPlugin::OnProjectClosed(CodeBlocksEvent& event)
{
    // allow others to catch this
    event.Skip();

    if(this != Manager::Get()->GetDebuggerManager()->GetActiveDebugger())
        return;
    CleanupWhenProjectClosed(event.GetProject());

    // when a project closes, make sure it's not the actively debugged project.
    // if so, end debugging immediately!
    if (!IsRunning())
        return;

    if (event.GetProject() == GetProject())
    {
        AnnoyingDialog dlg(_("Project closed while debugging message"),
                           _("The project you were debugging has closed.\n"
                             "(The application most likely just finished.)\n"
                             "The debugging session will terminate immediately."),
                            wxART_WARNING, AnnoyingDialog::OK, wxID_OK);
        dlg.ShowModal();
        Stop();
        ResetProject();
    }
}

void cbDebuggerPlugin::OnEditorHook(cbEditor* editor, wxScintillaEvent& event)
{
    if (event.GetEventType() == wxEVT_SCI_START_DRAG)
        m_DragInProgress = true;
    else if (event.GetEventType() == wxEVT_SCI_FINISHED_DRAG)
        m_DragInProgress = false;
}

bool cbDebuggerPlugin::DragInProgress() const
{
    return m_DragInProgress;
}

void cbDebuggerPlugin::ShowLog(bool clear)
{
    TextCtrlLogger *log = Manager::Get()->GetDebuggerManager()->GetLogger();
    if (log)
    {
        // switch to the debugger log
        CodeBlocksLogEvent eventSwitchLog(cbEVT_SWITCH_TO_LOG_WINDOW, log);
        Manager::Get()->ProcessEvent(eventSwitchLog);
        CodeBlocksLogEvent eventShowLog(cbEVT_SHOW_LOG_MANAGER);
        Manager::Get()->ProcessEvent(eventShowLog);

        if (clear)
            log->Clear();
    }
}

void cbDebuggerPlugin::Log(const wxString& msg, Logger::level level)
{
    if (IsAttached())
    {
        Manager::Get()->GetLogManager()->Log((m_lastLineWasNormal ? wxEmptyString : wxT("\n")) + msg, m_LogPageIndex,
                                             level);
        m_lastLineWasNormal = true;
    }
}

void cbDebuggerPlugin::DebugLog(const wxString& msg, Logger::level level)
{
    // gdb debug messages
    if (IsAttached() && HasDebugLog())
    {
        Manager::Get()->GetLogManager()->Log((!m_lastLineWasNormal ? wxT("[debug]") : wxT("\n[debug]")) + msg,
                                             m_LogPageIndex, level);
        m_lastLineWasNormal = false;
    }
}

bool cbDebuggerPlugin::HasDebugLog() const
{
    return cbDebuggerCommonConfig::GetFlag(cbDebuggerCommonConfig::ShowDebuggersLog);
}

void cbDebuggerPlugin::ClearLog()
{
    Manager::Get()->GetDebuggerManager()->GetLogger()->Clear();
}

void cbDebuggerPlugin::SetupLog(int normalIndex)
{
    m_LogPageIndex = normalIndex;
}

void cbDebuggerPlugin::SwitchToDebuggingLayout()
{
    CodeBlocksLayoutEvent queryEvent(cbEVT_QUERY_VIEW_LAYOUT);

    const cbDebuggerConfiguration &config = GetActiveConfig();

    wxString perspectiveName;
    switch (cbDebuggerCommonConfig::GetPerspective())
    {
    case cbDebuggerCommonConfig::OnePerDebugger:
        perspectiveName = GetGUIName();
        break;
    case cbDebuggerCommonConfig::OnePerDebuggerConfig:
        perspectiveName = GetGUIName() + wxT(":") + config.GetName();
        break;
    case cbDebuggerCommonConfig::OnlyOne:
    default:
        perspectiveName = _("Debugging");
    }

    CodeBlocksLayoutEvent switchEvent(cbEVT_SWITCH_VIEW_LAYOUT, perspectiveName);

    #if wxCHECK_VERSION(2, 9, 0)
    Manager::Get()->GetLogManager()->DebugLog(F(_("Switching layout to \"%s\""), switchEvent.layout.wx_str()));
    #else
    Manager::Get()->GetLogManager()->DebugLog(F(_("Switching layout to \"%s\""), switchEvent.layout.c_str()));
    #endif

    // query the current layout
    Manager::Get()->ProcessEvent(queryEvent);
    m_PreviousLayout = queryEvent.layout;

    // switch to debugging layout
    Manager::Get()->ProcessEvent(switchEvent);

    ShowLog(false);
}

void cbDebuggerPlugin::SwitchToPreviousLayout()
{
    CodeBlocksLayoutEvent switchEvent(cbEVT_SWITCH_VIEW_LAYOUT, m_PreviousLayout);

    wxString const &name = !switchEvent.layout.IsEmpty() ? switchEvent.layout : wxString(_("Code::Blocks default"));

    #if wxCHECK_VERSION(2, 9, 0)
    Manager::Get()->GetLogManager()->DebugLog(F(_("Switching layout to \"%s\""), name.wx_str()));
    #else
    Manager::Get()->GetLogManager()->DebugLog(F(_("Switching layout to \"%s\""), name.c_str()));
    #endif

    // switch to previous layout
    Manager::Get()->ProcessEvent(switchEvent);
}

bool cbDebuggerPlugin::GetDebuggee(wxString &pathToDebuggee, wxString &workingDirectory, ProjectBuildTarget* target)
{
    if (!target)
        return wxEmptyString;

    wxString out;
    switch (target->GetTargetType())
    {
        case ttExecutable:
        case ttConsoleOnly:
        case ttNative:
            {
                out = UnixFilename(target->GetOutputFilename());
                Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out); // apply env vars
                wxFileName f(out);
                f.MakeAbsolute(target->GetParentProject()->GetBasePath());
                out = f.GetFullPath();
                Log(_("Adding file: ") + out);
                ConvertDirectory(out);
            }
            break;

        case ttStaticLib:
        case ttDynamicLib:
            // check for hostapp
            if (target->GetHostApplication().IsEmpty())
            {
                cbMessageBox(_("You must select a host application to \"run\" a library..."));
                return false;
            }
            out = UnixFilename(target->GetHostApplication());
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out); // apply env vars
            Log(_("Adding file: ") + out);
            ConvertDirectory(out);
            break;

        default:
            Log(_("Unsupported target type (Project -> Properties -> Build Targets -> Type)"), Logger::error);
            return false;
    }
    if (out.empty())
    {
        Log(_("Couldn't find the path to the debuggee!"), Logger::error);
        return false;
    }

    workingDirectory = target->GetWorkingDir();
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(workingDirectory);
    wxFileName cd(workingDirectory);
    if (cd.IsRelative())
        cd.MakeAbsolute(target->GetParentProject()->GetBasePath());
    workingDirectory = cd.GetFullPath();

    pathToDebuggee = out;
    return true;
}

bool cbDebuggerPlugin::EnsureBuildUpToDate(StartType startType)
{
    m_StartType = startType;
    m_WaitingCompilerToFinish = false;

    // compile project/target (if not attaching to a PID)
    if (!IsAttachedToProcess())
    {
        // should we build to make sure project is up-to-date?
        if (!cbDebuggerCommonConfig::GetFlag(cbDebuggerCommonConfig::AutoBuild))
        {
            m_WaitingCompilerToFinish = false;
            m_pCompiler = NULL;
            return true;
        }

        // make sure the target is compiled
        PluginsArray plugins = Manager::Get()->GetPluginManager()->GetCompilerOffers();
        if (plugins.GetCount())
            m_pCompiler = (cbCompilerPlugin*)plugins[0];
        else
            m_pCompiler = NULL;
        if (m_pCompiler)
        {
            // is the compiler already running?
            if (m_pCompiler->IsRunning())
            {
                Log(_("Compiler in use..."));
                Log(_("Aborting debugging session"));
                cbMessageBox(_("The compiler is currently in use. Aborting debugging session..."),
                             _("Compiler running"), wxICON_WARNING);
                return false;
            }

            Log(_("Building to ensure sources are up-to-date"));
            m_WaitingCompilerToFinish = true;
            m_pCompiler->Build();
            // now, when the build is finished, DoDebug will be launched in OnCompilerFinished()
        }
    }
    return true;
}

void cbDebuggerPlugin::OnCompilerFinished(CodeBlocksEvent& event)
{
    if (m_WaitingCompilerToFinish)
    {
        m_WaitingCompilerToFinish = false;
        bool compilerFailed = false;
        // only proceed if build succeeeded
        if (m_pCompiler && m_pCompiler->GetExitCode() != 0)
        {
            AnnoyingDialog dlg(_("Debug anyway?"), _("Build failed, do you want to debug the program?"),
                               wxART_QUESTION, AnnoyingDialog::YES_NO, wxID_NO);
            if (dlg.ShowModal() != wxID_YES)
            {
                ProjectManager *manager = Manager::Get()->GetProjectManager();
                if (manager->GetIsRunning() && manager->GetIsRunning() == this)
                    manager->SetIsRunning(NULL);
                compilerFailed = true;
            }
        }
        ShowLog(false);
        if (!CompilerFinished(compilerFailed, m_StartType))
        {
            ProjectManager *manager = Manager::Get()->GetProjectManager();
            if (manager->GetIsRunning() && manager->GetIsRunning() == this)
                manager->SetIsRunning(NULL);
        }
    }
}

#ifndef __WXMSW__
namespace
{
wxString MakeSleepCommand()
{
    return wxString::Format(wxT("sleep %d"), 80000000 + ::wxGetProcessId());
}
}
#endif

int cbDebuggerPlugin::RunNixConsole(wxString &consoleTty)
{
    consoleTty = wxEmptyString;
#ifndef __WXMSW__

    // start the xterm and put the shell to sleep with -e sleep 80000
    // fetch the xterm tty so we can issue to gdb a "tty /dev/pts/#"
    // redirecting program stdin/stdout/stderr to the xterm console.

    wxString cmd;
    wxString title = wxT("Program Console");
    int consolePid = 0;
    // for non-win platforms, use m_ConsoleTerm to run the console app
    wxString term = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/console_terminal"), DEFAULT_CONSOLE_TERM);
    term.Replace(_T("$TITLE"), _T("'") + title + _T("'"));
    cmd << term << _T(" ");
    cmd << MakeSleepCommand();

    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(cmd);
//    DebugLog(wxString::Format( _("Executing: %s"), cmd.c_str()) );
    //start xterm -e sleep {some unique # of seconds}
    consolePid = wxExecute(cmd, wxEXEC_ASYNC);
    if (consolePid <= 0) return -1;

    // Issue the PS command to get the /dev/tty device name
    // First, wait for the xterm to settle down, else PS won't see the sleep task
    for (int ii = 0; ii < 100; ++ii)
    {
        Manager::Yield();
        ::wxMilliSleep(200);

        // For some reason wxExecute returns PID>0, when the command cannot be launched.
        // Here we check if the process is alive and the PID is really a valid one.
        if (kill(consolePid, 0) == -1 && errno == ESRCH) {
            Log(wxString::Format(_("Can't launch console (%s)"), cmd.c_str()), Logger::error);
            break;
        }

        int localConsolePid = consolePid;
        consoleTty = GetConsoleTty(localConsolePid);
        if (!consoleTty.IsEmpty() )
        {
            // show what we found as tty
            return localConsolePid;
        }
    }
    // failed to find the console tty
    if (consolePid != 0)
        ::wxKill(consolePid);
    consolePid = 0;
#endif // !__WWXMSW__
    return -1;
}

void cbDebuggerPlugin::MarkAsStopped()
{
    Manager::Get()->GetProjectManager()->SetIsRunning(NULL);
}

wxString cbDebuggerPlugin::GetConsoleTty(int ConsolePid)
{
#ifndef __WXMSW__

    // execute the ps x -o command  and read PS output to get the /dev/tty field

    unsigned long ConsPid = ConsolePid;
    wxString psCmd;
    wxArrayString psOutput;
    wxArrayString psErrors;

    psCmd << wxT("ps x -o tty,pid,command");
//    DebugLog(wxString::Format( _("Executing: %s"), psCmd.c_str()) );
    int result = wxExecute(psCmd, psOutput, psErrors, wxEXEC_SYNC);
    psCmd.Clear();
    if (result != 0)
    {
        psCmd << wxT("Result of ps x:") << result;
//        DebugLog(wxString::Format( _("Execution Error:"), psCmd.c_str()) );
        return wxEmptyString;
    }

    wxString ConsTtyStr;
    wxString ConsPidStr;
    ConsPidStr << ConsPid;
    //find task with our unique sleep time
    const wxString &uniqueSleepTimeStr = MakeSleepCommand();
    // search the output of "ps pid" command
    int knt = psOutput.GetCount();
    for (int i=knt-1; i>-1; --i)
    {
        psCmd = psOutput.Item(i);
//        DebugLog(wxString::Format( _("PS result: %s"), psCmd.c_str()) );
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
//            DebugLog(wxString::Format( _("TTY is[%s]"), ConsTtyStr.c_str()) );
            return ConsTtyStr;
        } while(0);//if do
    }//for

    knt = psErrors.GetCount();
//    for (int i=0; i<knt; ++i)
//        DebugLog(wxString::Format( _("PS Error:%s"), psErrors.Item(i).c_str()) );
#endif // !__WXMSW__
    return wxEmptyString;
}

void cbDebuggerPlugin::BringCBToFront()
{
    wxWindow* app = Manager::Get()->GetAppWindow();
    if (app)
        app->Raise();
}

void cbDebuggerPlugin::RegisterValueTooltip()
{
    typedef cbEventFunctor<cbDebuggerPlugin, CodeBlocksEvent> Event;
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_TOOLTIP, new Event(this, &cbDebuggerPlugin::ProcessValueTooltip));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_TOOLTIP_CANCEL,
                                      new Event(this, &cbDebuggerPlugin::CancelValueTooltip));
}

bool cbDebuggerPlugin::ShowValueTooltip(int /*style*/)
{
    return false;
}

// Default implementation does nothing
void cbDebuggerPlugin::OnValueTooltip(const wxString &/*token*/, const wxRect &/*evalRect*/)
{
}

void cbDebuggerPlugin::ProcessValueTooltip(CodeBlocksEvent& event)
{
    event.Skip();
    if (cbDebuggerCommonConfig::GetFlag(cbDebuggerCommonConfig::RequireCtrlForTooltips))
    {
        if (!wxGetKeyState(WXK_CONTROL))
            return;
    }

    if (Manager::Get()->GetDebuggerManager()->GetInterfaceFactory()->IsValueTooltipShown())
        return;

    if (!ShowValueTooltip(event.GetInt()))
        return;

    EditorBase* base = event.GetEditor();
    cbEditor* ed = base && base->IsBuiltinEditor() ? static_cast<cbEditor*>(base) : 0;
    if (!ed)
        return;

    if (ed->IsContextMenuOpened())
        return;

    // get rid of other calltips (if any) [for example the code completion one, at this time we
    // want the debugger value call/tool-tip to win and be shown]
    if (ed->GetControl()->CallTipActive())
        ed->GetControl()->CallTipCancel();

    wxPoint pt;
    pt.x = event.GetX();
    pt.y = event.GetY();

    const wxString &token = GetEditorWordAtCaret(&pt);
    if (!token.empty())
    {
        pt = ed->GetControl()->ClientToScreen(pt);
        OnValueTooltip(token, wxRect(pt.x - 5, pt.y, 10, 10));
    }
}

void cbDebuggerPlugin::CancelValueTooltip(CodeBlocksEvent& event)
{
    Manager::Get()->GetDebuggerManager()->GetInterfaceFactory()->HideValueTooltip();
}
/////
///// cbToolPlugin
/////

cbToolPlugin::cbToolPlugin()
{
    m_Type = ptTool;
}

/////
///// cbMimePlugin
/////

cbMimePlugin::cbMimePlugin()
{
    m_Type = ptMime;
}

/////
///// cbCodeCompletionPlugin
/////

cbCodeCompletionPlugin::cbCodeCompletionPlugin()
{
    m_Type = ptCodeCompletion;
}

/////
///// cbWizardPlugin
/////

cbWizardPlugin::cbWizardPlugin()
{
    m_Type = ptWizard;
}
