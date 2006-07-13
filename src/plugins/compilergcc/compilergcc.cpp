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
#include "compilergcc.h"
#include <manager.h>
#include <sdk_events.h>
#include <pipedprocess.h>
#include <configmanager.h>
#include <messagemanager.h>
#include <macrosmanager.h>
#include <projectmanager.h>
#include <editormanager.h>
#include <scriptingmanager.h>
#include <configurationpanel.h>
#include <pluginmanager.h>
#include <cbeditor.h>
#include <annoyingdialog.h>
#include <filefilters.h>
#include <wx/xrc/xmlres.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include "makefilegenerator.h"
#include "compileroptionsdlg.h"
#include "directcommands.h"
#include "globals.h"

#include "compilerMINGW.h"
// TODO (mandrav#1#): Find out which compilers exist for linux and adapt this
#ifdef __WXMSW__
    #include "compilerMSVC.h"
    #include "compilerMSVC8.h"
    #include "compilerBCC.h"
    #include "compilerDMC.h"
    #include "compilerOW.h"
    #include "compilerGDC.h"
    #include "compilerDMD.h"
    #include "compilerGNUARM.h"
#endif
#ifdef __WXMAC__
    #include "compilerGDC.h"
#endif
#include "compilerICC.h"
#include "compilerSDCC.h"

#include <scripting/bindings/sc_base_types.h>

namespace ScriptBindings
{
    static int gBuildLogId = -1;

    // global funcs
    void gBuildLog(const wxString& msg)
    {
        Manager::Get()->GetMessageManager()->Log(gBuildLogId, msg);
    }
};

#define COLOUR_MAROON wxColour(0xa0, 0x00, 0x00)
#define COLOUR_NAVY   wxColour(0x00, 0x00, 0xa0)

CB_IMPLEMENT_PLUGIN(CompilerGCC, "Compiler");

// menu IDS
// just because we don't know other plugins' used identifiers,
// we use wxNewId() to generate a guaranteed unique ID ;), instead of enum
// (don't forget that, especially in a plugin)
int idTimerPollCompiler = XRCID("idTimerPollCompiler");
int idMenuCompile = XRCID("idCompilerMenuCompile");
int idMenuCompileTarget = XRCID("idCompilerMenuCompileTarget");
int idMenuCompileFromProjectManager = XRCID("idCompilerMenuCompileFromProjectManager");
int idMenuProjectCompilerOptions = XRCID("idCompilerMenuProjectCompilerOptions");
int idMenuTargetCompilerOptions = XRCID("idCompilerMenuTargetCompilerOptions");
int idMenuTargetCompilerOptionsSub = XRCID("idCompilerMenuTargetCompilerOptionsSub");
int idMenuCompileTargetFromProjectManager = XRCID("idCompilerMenuCompileTargetFromProjectManager");
int idMenuCompileFile = XRCID("idCompilerMenuCompileFile");
int idMenuCompileFileFromProjectManager = XRCID("idCompilerMenuCompileFileFromProjectManager");
int idMenuRebuild = XRCID("idCompilerMenuRebuild");
int idMenuRebuildTarget = XRCID("idCompilerMenuRebuildTarget");
int idMenuRebuildFromProjectManager = XRCID("idCompilerMenuRebuildFromProjectManager");
int idMenuRebuildTargetFromProjectManager = XRCID("idCompilerMenuRebuildTargetFromProjectManager");
int idMenuCompileAll = XRCID("idCompilerMenuCompileAll");
int idMenuRebuildAll = XRCID("idCompilerMenuRebuildAll");
int idMenuClean = XRCID("idCompilerMenuClean");
int idMenuCleanAll = XRCID("idCompilerMenuCleanAll");
int idMenuCleanTarget = XRCID("idCompilerMenuCleanTarget");
int idMenuCleanFromProjectManager = XRCID("idCompilerMenuCleanFromProjectManager");
int idMenuCleanTargetFromProjectManager = XRCID("idCompilerMenuCleanTargetFromProjectManager");
int idMenuCompileAndRun = XRCID("idCompilerMenuCompileAndRun");
int idMenuRun = XRCID("idCompilerMenuRun");
int idMenuKillProcess = XRCID("idCompilerMenuKillProcess");
int idMenuSelectTarget = XRCID("idCompilerMenuSelectTarget");
int idMenuSelectTargetAll = XRCID("idCompilerMenuSelectTargetAll");
int idMenuSelectTargetOther[MAX_TARGETS]; // initialized in ctor
int idMenuNextError = XRCID("idCompilerMenuNextError");
int idMenuPreviousError = XRCID("idCompilerMenuPreviousError");
int idMenuClearErrors = XRCID("idCompilerMenuClearErrors");
int idMenuExportMakefile = XRCID("idCompilerMenuExportMakefile");
int idMenuSettings = XRCID("idCompilerMenuSettings");

int idToolTarget = XRCID("idToolTarget");
int idToolTargetLabel = XRCID("idToolTargetLabel");

int idGCCProcess1 = wxNewId();
int idGCCProcess2 = wxNewId();
int idGCCProcess3 = wxNewId();
int idGCCProcess4 = wxNewId();
int idGCCProcess5 = wxNewId();
int idGCCProcess6 = wxNewId();
int idGCCProcess7 = wxNewId();
int idGCCProcess8 = wxNewId();
int idGCCProcess9 = wxNewId();
int idGCCProcess10 = wxNewId();
int idGCCProcess11 = wxNewId();
int idGCCProcess12 = wxNewId();
int idGCCProcess13 = wxNewId();
int idGCCProcess14 = wxNewId();
int idGCCProcess15 = wxNewId();
int idGCCProcess16 = wxNewId();

BEGIN_EVENT_TABLE(CompilerGCC, cbCompilerPlugin)
    EVT_UPDATE_UI_RANGE(idMenuCompile, idToolTargetLabel, CompilerGCC::OnUpdateUI)

    // these are loaded from the XRC
    EVT_UPDATE_UI(XRCID("idCompilerMenuCompile"), CompilerGCC::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idCompilerMenuRebuild"), CompilerGCC::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idCompilerMenuCompileAndRun"), CompilerGCC::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idCompilerMenuRun"), CompilerGCC::OnUpdateUI)

    EVT_IDLE(                                        CompilerGCC::OnIdle)
    EVT_TIMER(idTimerPollCompiler,                  CompilerGCC::OnTimer)

    EVT_MENU(idMenuRun,                             CompilerGCC::Dispatcher)
    EVT_MENU(idMenuCompileAndRun,                   CompilerGCC::Dispatcher)
    EVT_MENU(idMenuCompile,                         CompilerGCC::Dispatcher)
    EVT_MENU(idMenuCompileFromProjectManager,       CompilerGCC::Dispatcher)
    EVT_MENU(idMenuCompileTargetFromProjectManager, CompilerGCC::Dispatcher)
    EVT_MENU(idMenuCompileFile,                     CompilerGCC::Dispatcher)
    EVT_MENU(idMenuCompileFileFromProjectManager,   CompilerGCC::Dispatcher)
    EVT_MENU(idMenuRebuild,                         CompilerGCC::Dispatcher)
    EVT_MENU(idMenuRebuildFromProjectManager,       CompilerGCC::Dispatcher)
    EVT_MENU(idMenuRebuildTargetFromProjectManager, CompilerGCC::Dispatcher)
    EVT_MENU(idMenuCompileAll,                      CompilerGCC::Dispatcher)
    EVT_MENU(idMenuRebuildAll,                      CompilerGCC::Dispatcher)
    EVT_MENU(idMenuProjectCompilerOptions,          CompilerGCC::Dispatcher)
    EVT_MENU(idMenuTargetCompilerOptions,           CompilerGCC::Dispatcher)
    EVT_MENU(idMenuClean,                           CompilerGCC::Dispatcher)
    EVT_MENU(idMenuCleanAll,                        CompilerGCC::Dispatcher)
    EVT_MENU(idMenuCleanFromProjectManager,         CompilerGCC::Dispatcher)
    EVT_MENU(idMenuCleanTargetFromProjectManager,   CompilerGCC::Dispatcher)
    EVT_MENU(idMenuKillProcess,                     CompilerGCC::Dispatcher)
    EVT_MENU(idMenuSelectTargetAll,                 CompilerGCC::Dispatcher)
    EVT_MENU(idMenuNextError,                       CompilerGCC::Dispatcher)
    EVT_MENU(idMenuPreviousError,                   CompilerGCC::Dispatcher)
    EVT_MENU(idMenuClearErrors,                     CompilerGCC::Dispatcher)
    EVT_MENU(idMenuExportMakefile,                  CompilerGCC::Dispatcher)
    EVT_MENU(idMenuSettings,                        CompilerGCC::Dispatcher)

    EVT_COMBOBOX(idToolTarget,                        CompilerGCC::OnSelectTarget)

    EVT_PROJECT_ACTIVATE(CompilerGCC::OnProjectActivated)
    EVT_PROJECT_OPEN(CompilerGCC::OnProjectLoaded)
    //EVT_PROJECT_POPUP_MENU(CompilerGCC::OnProjectPopupMenu)
    EVT_PROJECT_TARGETS_MODIFIED(CompilerGCC::OnProjectActivated)

    EVT_PIPEDPROCESS_STDOUT_RANGE(idGCCProcess1, idGCCProcess16, CompilerGCC::OnGCCOutput)
    EVT_PIPEDPROCESS_STDERR_RANGE(idGCCProcess1, idGCCProcess16, CompilerGCC::OnGCCError)
    EVT_PIPEDPROCESS_TERMINATED_RANGE(idGCCProcess1, idGCCProcess16, CompilerGCC::OnGCCTerminated)
END_EVENT_TABLE()

CompilerGCC::CompilerGCC()
    : m_BuildingWorkspace(false),
    m_PageIndex(-1),
    m_ListPageIndex(-1),
    m_Menu(0L),
    m_TargetMenu(0L),
    m_pToolbar(0L),
    m_TargetIndex(-1),
    m_ErrorsMenu(0L),
    m_Project(0L),
    m_Processes(0),
    m_ParallelProcessCount(1),
    m_pTbar(0L),
    m_Pid(0),
    m_Log(0L),
    m_pListLog(0L),
    m_ToolTarget(0L),
    m_ToolTargetLabel(0L),
    m_RunAfterCompile(false),
    m_LastExitCode(0),
    m_HasTargetAll(false),
    m_NotifiedMaxErrors(false),
    m_pBuildingProject(0),
    m_BuildingProjectIdx(0),
    m_BuildingTargetIdx(0),
    m_BuildJob(bjIdle),
    m_NextBuildState(bsNone),
    m_BuildStateTargetIsAll(false),
    m_pLastBuildingProject(0),
    m_pLastBuildingTarget(0),
    m_BuildDepsIndex(0),
    m_RunTargetPostBuild(false),
    m_RunProjectPostBuild(false),
    m_DeleteTempMakefile(true)
{
    if(!Manager::LoadResource(_T("compiler_gcc.zip")))
    {
        NotifyMissingFile(_T("compiler_gcc.zip"));
    }

    m_Type = ptCompiler;
    m_PluginInfo.name = _T("Compiler");
    m_PluginInfo.title = _T("Compiler");
    m_PluginInfo.version = _T("1.0");
    m_PluginInfo.description = _("This plugin is an interface to various compilers:\n\n"
                               "\tGNU GCC compiler\n"
                               "\tMicrosoft Visual C++ Free Toolkit 2003\n"
                               "\tBorland C++ Compiler 5.5");
    m_PluginInfo.author = _T("Yiannis An. Mandravellos");
    m_PluginInfo.authorEmail = _T("info@codeblocks.org");
    m_PluginInfo.authorWebsite = _T("www.codeblocks.org");
    m_PluginInfo.thanksTo = _("All the free (and not) compilers out there");

    m_timerIdleWakeUp.SetOwner(this, idTimerPollCompiler);

    for (int i = 0; i < MAX_TARGETS; ++i)
        idMenuSelectTargetOther[i] = wxNewId();
    // register built-in compilers
    CompilerFactory::RegisterCompiler(new CompilerMINGW);
#ifdef __WXMSW__
    CompilerFactory::RegisterCompiler(new CompilerMSVC);
    CompilerFactory::RegisterCompiler(new CompilerMSVC8);
    CompilerFactory::RegisterCompiler(new CompilerBCC);
    CompilerFactory::RegisterCompiler(new CompilerDMC);
    CompilerFactory::RegisterCompiler(new CompilerOW);
#endif
    CompilerFactory::RegisterCompiler(new CompilerICC);
    CompilerFactory::RegisterCompiler(new CompilerSDCC);
#ifdef __WXMSW__
    CompilerFactory::RegisterCompiler(new CompilerGDC);
    CompilerFactory::RegisterCompiler(new CompilerDMD);
    CompilerFactory::RegisterCompiler(new CompilerGNUARM);
#endif
#ifdef __WXMAC__
    CompilerFactory::RegisterCompiler(new CompilerGDC);
#endif

    // register (if any) user-copies of built-in compilers
    CompilerFactory::RegisterUserCompilers();

    AllocProcesses();
}

CompilerGCC::~CompilerGCC()
{
    FreeProcesses();

    DoDeleteTempMakefile();
    if (m_ToolTarget)
        delete m_ToolTarget;
    CompilerFactory::UnregisterCompilers();
}

void CompilerGCC::Dispatcher(wxCommandEvent& event)
{
    // Memorize the currently focused window

    wxWindow* focused = wxWindow::FindFocus();

    int eventId = event.GetId();

//    Manager::Get()->GetMessageManager()->Log(wxT("Dispatcher"));

    if (eventId == idMenuRun)
        OnRun(event);

    if (eventId == idMenuCompileAndRun)
        OnCompileAndRun(event);

    if (eventId == idMenuCompile)
        OnCompile(event);

    if (eventId == idMenuCompileFromProjectManager)
        OnCompile(event);

    if (eventId == idMenuCompileTargetFromProjectManager)
        OnCompile(event);

    if (eventId == idMenuCompileFile)
        OnCompileFile(event);

    if (eventId == idMenuCompileFileFromProjectManager)
        OnCompileFile(event);

    if (eventId == idMenuRebuild)
        OnRebuild(event);

    if (eventId == idMenuRebuildFromProjectManager)
        OnRebuild(event);

    if (eventId == idMenuRebuildTargetFromProjectManager)
        OnRebuild(event);

    if (eventId == idMenuCompileAll)
        OnCompileAll(event);

    if (eventId == idMenuRebuildAll)
        OnRebuildAll(event);

    if (eventId == idMenuProjectCompilerOptions)
        OnProjectCompilerOptions(event);

    if (eventId == idMenuTargetCompilerOptions)
        OnTargetCompilerOptions(event);

    if (eventId == idMenuClean)
        OnClean(event);

    if (eventId == idMenuCleanAll)
        OnCleanAll(event);

    if (eventId == idMenuCleanFromProjectManager)
        OnClean(event);

    if (eventId == idMenuCleanTargetFromProjectManager)
        OnClean(event);

    if (eventId == idMenuKillProcess)
        OnKillProcess(event);

    if (eventId == idMenuSelectTargetAll)
        OnSelectTarget(event);

    if (eventId == idMenuNextError)
        OnNextError(event);

    if (eventId == idMenuPreviousError)
        OnPreviousError(event);

    if (eventId == idMenuClearErrors)
        OnClearErrors(event);

    if (eventId == idMenuExportMakefile)
        OnExportMakefile(event);

    if (eventId == idMenuSettings)
        OnConfig(event);

    // Return focus to previously focused window

    if (focused)
        focused->SetFocus();
}

void CompilerGCC::AllocProcesses()
{
    // create the parallel processes array
    m_ParallelProcessCount = Manager::Get()->GetConfigManager(_T("compiler"))->ReadInt(_T("/parallel_processes"), 1);
    m_Processes = (wxProcess**)malloc(m_ParallelProcessCount * sizeof(wxProcess*));
    m_Pid = new long int[m_ParallelProcessCount];
    for (size_t i = 0; i < m_ParallelProcessCount; ++i)
    {
        m_Processes[i] = 0;
        m_Pid[i] = 0;
    }
}

void CompilerGCC::FreeProcesses()
{
    // free the parallel processes array
    free(m_Processes);
    m_Processes = 0;
    delete[] m_Pid;
    m_Pid = 0;
}

bool CompilerGCC::ReAllocProcesses()
{
    FreeProcesses();
    AllocProcesses();
    return true;
}

void CompilerGCC::OnAttach()
{
    MessageManager* msgMan = Manager::Get()->GetMessageManager();

    // create compiler's log
    m_Log = new SimpleTextLog(true);
    m_PageIndex = msgMan->AddLog(m_Log, _("Build log"));
    msgMan->SetBatchBuildLog(m_PageIndex);

    // set log image
    wxBitmap bmp;
    wxString prefix = ConfigManager::GetDataFolder() + _T("/images/");
    bmp.LoadFile(prefix + _T("misc_16x16.png"), wxBITMAP_TYPE_PNG);
    Manager::Get()->GetMessageManager()->SetLogImage(m_Log, bmp);

    // create warnings/errors log
    wxArrayString titles;
    int widths[3] = {128, 48, 640};
    titles.Add(_("File"));
    titles.Add(_("Line"));
    titles.Add(_("Message"));

    m_pListLog = new CompilerMessages(3, widths, titles);
    m_pListLog->SetCompilerErrors(&m_Errors);
    m_ListPageIndex = msgMan->AddLog(m_pListLog, _("Build messages"));

    // set log image
    bmp.LoadFile(prefix + _T("flag_16x16.png"), wxBITMAP_TYPE_PNG);
    Manager::Get()->GetMessageManager()->SetLogImage(m_pListLog, bmp);

    // set default compiler for new projects
    CompilerFactory::SetDefaultCompiler(Manager::Get()->GetConfigManager(_T("compiler"))->Read(_T("/default_compiler"), _T("gcc")));
    LoadOptions();
//    SetupEnvironment();

    // register compiler's script functions
    // make sure the VM is initialized
    Manager::Get()->GetScriptingManager();
    if (SquirrelVM::GetVMPtr())
    {
        ScriptBindings::gBuildLogId = m_PageIndex;
        SqPlus::RegisterGlobal(ScriptBindings::gBuildLog, "LogBuild");
    }
    else
        ScriptBindings::gBuildLogId = -1;
}

void CompilerGCC::OnRelease(bool appShutDown)
{
    // disable script functions
    ScriptBindings::gBuildLogId = -1;

    DoDeleteTempMakefile();
    SaveOptions();
    Manager::Get()->GetConfigManager(_T("compiler"))->Write(_T("/default_compiler"), CompilerFactory::GetDefaultCompilerID());
    if (Manager::Get()->GetMessageManager())
    {
        Manager::Get()->GetMessageManager()->RemoveLog(m_Log);
        Manager::Get()->GetMessageManager()->RemoveLog(m_pListLog);
    }

    if (appShutDown)
        return; // no need to continue if app is shutting down

    DoClearTargetMenu();

//    if (m_Menu)
//    {
//        wxMenuBar* mBar = Manager::Get()->GetAppWindow()->GetMenuBar();
//        int pos = mBar->FindMenu(_("&Build"));
//        if (pos != wxNOT_FOUND)
//            mBar->Remove(pos);
//        delete m_Menu;
//        m_Menu = 0L;
//    }

    if (m_pToolbar)
    {
        m_pToolbar->DeleteTool(idMenuCompile);
        m_pToolbar->DeleteTool(idMenuRun);
        m_pToolbar->DeleteTool(idMenuCompileAndRun);
        m_pToolbar->DeleteTool(idMenuRebuild);

        m_pToolbar->DeleteTool(idToolTarget);
        delete m_ToolTarget;
        m_ToolTarget = 0L;

        m_pToolbar->DeleteTool(idToolTargetLabel);
        delete m_ToolTargetLabel;
        m_ToolTargetLabel = 0L;
    }
}

int CompilerGCC::Configure(cbProject* project, ProjectBuildTarget* target)
{
    cbConfigurationDialog dlg(Manager::Get()->GetAppWindow(), wxID_ANY, _("Project build options"));
    cbConfigurationPanel* panel = new CompilerOptionsDlg(&dlg, this, project, target);
    dlg.AttachConfigurationPanel(panel);
    PlaceWindow(&dlg);
    if(dlg.ShowModal() == wxID_OK)
    {
        SaveOptions();
//        SetupEnvironment();
        Manager::Get()->GetMacrosManager()->Reset();
    }
//    delete panel;
    return 0;
}

cbConfigurationPanel* CompilerGCC::GetConfigurationPanel(wxWindow* parent)
{
    CompilerOptionsDlg* dlg = new CompilerOptionsDlg(parent, this, 0, 0);
    return dlg;
}

void CompilerGCC::OnConfig(wxCommandEvent& event)
{
    Configure(NULL);
}

void CompilerGCC::BuildMenu(wxMenuBar* menuBar)
{
    if (!m_IsAttached)
        return;
    if (m_Menu)
        return;

    m_Menu=Manager::Get()->LoadMenu(_T("compiler_menu"),true);

    // target selection menu
    wxMenuItem *tmpitem=m_Menu->FindItem(idMenuSelectTarget,NULL);
    m_TargetMenu = tmpitem ? tmpitem->GetSubMenu() : new wxMenu(_T(""));
    DoRecreateTargetMenu();
    //m_Menu->Append(idMenuSelectTarget, _("Select target..."), m_TargetMenu);

    // ok, now, where do we insert?
    // three possibilities here:
    // a) locate "Debug" menu and insert before it
    // b) locate "Project" menu and insert after it
    // c) if not found (?), insert at pos 5
    int finalPos = 5;
    int projMenuPos = menuBar->FindMenu(_("&Debug"));
    if (projMenuPos != wxNOT_FOUND)
        finalPos = projMenuPos;
    else
    {
        projMenuPos = menuBar->FindMenu(_("&Project"));
        if (projMenuPos != wxNOT_FOUND)
            finalPos = projMenuPos + 1;
    }
    menuBar->Insert(finalPos, m_Menu, _("&Build"));

    // now add some entries in Project menu
    projMenuPos = menuBar->FindMenu(_("&Project"));
    if (projMenuPos != wxNOT_FOUND)
    {
        wxMenu* prj = menuBar->GetMenu(projMenuPos);
        // look if we have a "Properties" item. If yes, we 'll insert
        // before it, else we 'll just append...
        size_t propsPos = prj->GetMenuItemCount(); // append
        int propsID = prj->FindItem(_("Properties"));
        if (propsID != wxNOT_FOUND)
            prj->FindChildItem(propsID, &propsPos);
        prj->Insert(propsPos, idMenuProjectCompilerOptions, _("Build options"), _("Set the project's build options"));
        prj->InsertSeparator(propsPos);
    }
//    // Add entry in settings menu (outside "plugins")
//    int settingsMenuPos = menuBar->FindMenu(_("&Settings"));
//    if (settingsMenuPos != wxNOT_FOUND)
//    {
//        wxMenu* settingsmenu = menuBar->GetMenu(settingsMenuPos);
//        settingsmenu->Insert(2,idMenuSettings,_("&Compiler"),_("Global Compiler Options"));
//    }
}

void CompilerGCC::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    if (!m_IsAttached)
        return;
    // we 're only interested in project manager's menus
    if (type != mtProjectManager || !menu || IsRunning())
        return;

    if (!CheckProject())
        return;

    if (!data || data->GetKind() == FileTreeData::ftdkUndefined)
    {
        // popup menu in empty space in ProjectManager
        menu->Append(idMenuCompileAll, _("Build workspace"));
        menu->Append(idMenuRebuildAll, _("Rebuild workspace"));
    }
    else if (data && data->GetKind() == FileTreeData::ftdkProject)
    {
        // popup menu on a project
        menu->AppendSeparator();
        menu->Append(idMenuCompileFromProjectManager, _("Build\tCtrl-F9"));
        menu->Append(idMenuRebuildFromProjectManager, _("Rebuild\tCtrl-F11"));
        menu->Append(idMenuCleanFromProjectManager, _("Clean"));
        wxMenu* subMenu = new wxMenu();
        subMenu->Append(idMenuCompileTargetFromProjectManager, _("Build"));
        subMenu->Append(idMenuRebuildTargetFromProjectManager, _("Rebuild"));
        subMenu->Append(idMenuCleanTargetFromProjectManager, _("Clean"));
        subMenu->AppendSeparator();
        subMenu->Append(idMenuTargetCompilerOptions, _("Build options"));
        menu->Append(idMenuTargetCompilerOptionsSub, _("Specific build target..."), subMenu);
        menu->AppendSeparator();
        menu->Append(idMenuProjectCompilerOptions, _("Build options"));
    }
    else if (data && data->GetKind() == FileTreeData::ftdkFile)
    {
        FileType ft = FileTypeOf(data->GetProjectFile()->relativeFilename);
        if (ft == ftSource || ft == ftHeader)
        {
            // popup menu on a compilable file
            menu->AppendSeparator();
            menu->Append(idMenuCompileFileFromProjectManager, _("Build file"));
        }
    }
}

bool CompilerGCC::BuildToolBar(wxToolBar* toolBar)
{
    if (!m_IsAttached || !toolBar)
        return false;
    m_pTbar = toolBar;
    wxString my_16x16=Manager::isToolBar16x16(toolBar) ? _T("_16x16") : _T("");
    Manager::Get()->AddonToolBar(toolBar,_T("compiler_toolbar")+my_16x16);

    // neither the generic nor Motif native toolbars really support this
    #if (wxUSE_TOOLBAR_NATIVE && !USE_GENERIC_TBAR) && !defined(__WXMOTIF__) && !defined(__WXX11__) && !defined(__WXMAC__)
    m_ToolTarget = XRCCTRL(*toolBar, "idToolTarget", wxComboBox);
    #endif
    toolBar->Realize();
    toolBar->SetBestFittingSize();
    DoRecreateTargetMenu(); // make sure the tool target combo is up-to-date
    return true;
}

void CompilerGCC::SetupEnvironment()
{
    if (!CompilerFactory::GetCompiler(m_CompilerId))
        return;

    m_EnvironmentMsg.Clear();

    wxString path;
//    Manager::Get()->GetMessageManager()->DebugLog(_T("Setting up compiler environment..."));

    // reset PATH to original value
    if (!m_OriginalPath.IsEmpty())
        wxSetEnv(_T("PATH"), m_OriginalPath);

    // look for valid compiler in path
    if (wxGetEnv(_T("PATH"), &path))
    {
        if (m_OriginalPath.IsEmpty())
            m_OriginalPath = path;

        SetEnvironmentForCompiler(m_CompilerId, path);
    }
    else
        m_EnvironmentMsg = _("Could not read the PATH environment variable!\n"
                    "This can't be good. There may be problems running "
                    "system commands and the application might not behave "
                    "the way it was designed to...");
//    wxGetEnv("PATH", &path);
//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, "PATH set to: %s", path.c_str());
}

void CompilerGCC::SetEnvironmentForCompiler(const wxString& id, wxString& envPath)
{
    if (!CompilerFactory::GetCompiler(id))
        return;

    Compiler* compiler = CompilerFactory::GetCompiler(id);
    wxString sep = wxFileName::GetPathSeparator();

    wxString masterPath = compiler->GetMasterPath();
    while (masterPath.Last() == '\\' || masterPath.Last() == '/')
        masterPath.RemoveLast();
    wxString gcc = compiler->GetPrograms().C;
    const wxArrayString& extraPaths = compiler->GetExtraPaths();

    wxPathList pathList;
    pathList.Add(masterPath + sep + _T("bin"));
    for (unsigned int i = 0; i < extraPaths.GetCount(); ++i)
    {
        if (!extraPaths[i].IsEmpty())
            pathList.Add(extraPaths[i]);
    }
    pathList.AddEnvList(_T("PATH"));
    wxString binPath = pathList.FindAbsoluteValidPath(gcc);
    // it seems, under Win32, the above command doesn't search in paths with spaces...
    // look directly for the file in question in masterPath
    if (binPath.IsEmpty() || !pathList.Member(wxPathOnly(binPath)))
    {
        if (wxFileExists(masterPath + sep + _T("bin") + sep + gcc))
            binPath = masterPath + sep + _T("bin");
        else if (wxFileExists(masterPath + sep + gcc))
            binPath = masterPath;
        else
        {
            for (unsigned int i = 0; i < extraPaths.GetCount(); ++i)
            {
                if (!extraPaths[i].IsEmpty())
                {
                    if (wxFileExists(extraPaths[i] + sep + gcc))
                    {
                        binPath = extraPaths[i];
                        break;
                    }
                }
            }
        }
    }

    if (binPath.IsEmpty() || !pathList.Member(wxPathOnly(binPath)))
    {
        m_EnvironmentMsg << _("Can't find compiler executable in your search path for ") << compiler->GetName() << _T('\n');
        Manager::Get()->GetMessageManager()->DebugLog(_T("Can't find compiler executable in your search path (%s)..."), compiler->GetName().c_str());
    }
    else
    {
        m_EnvironmentMsg.Clear();
#ifdef __WXMSW__
#define PATH_SEP _T(";")
#else
#define PATH_SEP _T(":")
#endif
        // add extra compiler paths in PATH
        wxString oldpath = envPath;
        envPath.Clear();
        for (unsigned int i = 0; i < extraPaths.GetCount(); ++i)
        {
            if (!extraPaths[i].IsEmpty())
            {
                envPath += extraPaths[i] + PATH_SEP;
            }
        }
        envPath = envPath + oldpath;

        // add bin path to PATH env. var.
        if (wxFileExists(masterPath + sep + _T("bin") + sep + gcc))
            envPath = masterPath + sep + _T("bin") + PATH_SEP + envPath;
        else if (wxFileExists(masterPath + sep + gcc))
            envPath = masterPath + PATH_SEP + envPath;
        wxSetEnv(_T("PATH"), envPath);
#undef PATH_SEP
    }
}

bool CompilerGCC::CheckDebuggerIsRunning()
{
    PluginsArray plugins = Manager::Get()->GetPluginManager()->GetDebuggerOffers();
    if (plugins.GetCount())
    {
        cbDebuggerPlugin* dbg = (cbDebuggerPlugin*)plugins[0];
        if (dbg)
        {
            // is the debugger running?
            if (dbg->IsRunning())
            {
                m_Log->GetTextControl()->Clear();
                Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Debugger is active"));
                Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Aborting build"));
                cbMessageBox(_("The debugger is currently active. Aborting build..."), _("Debugger active"), wxICON_WARNING);
                return true;
            }
        }
    }
    return false;
}

void CompilerGCC::SaveOptions()
{
    // save compiler sets
    CompilerFactory::SaveSettings();
}

void CompilerGCC::LoadOptions()
{
    // load compiler sets
    CompilerFactory::LoadSettings();
}

const wxString& CompilerGCC::GetCurrentCompilerID()
{
    static wxString def = _T("gcc");
    return CompilerFactory::GetCompiler(m_CompilerId) ? m_CompilerId : def;
}

void CompilerGCC::SwitchCompiler(const wxString& id)
{
    if (!CompilerFactory::GetCompiler(id))
        return;
    m_CompilerId = id;
    SetupEnvironment();
}

void CompilerGCC::AskForActiveProject()
{
    m_Project = m_pBuildingProject
                ? m_pBuildingProject
                : Manager::Get()->GetProjectManager()->GetActiveProject();
}

bool CompilerGCC::CheckProject()
{
    AskForActiveProject();
    // switch compiler for the project (if needed)
    if (m_Project && m_Project->GetCompilerID() != m_CompilerId)
        SwitchCompiler(m_Project->GetCompilerID());
    // switch compiler for single file (if needed)
    else if (!m_Project && m_CompilerId != CompilerFactory::GetDefaultCompilerID())
        SwitchCompiler(CompilerFactory::GetDefaultCompilerID());

    return m_Project;
}

wxString CompilerGCC::ProjectMakefile()
{
    AskForActiveProject();
    if (!m_Project)
        return wxEmptyString;

    return m_Project->GetMakefile();
}

void CompilerGCC::ClearLog()
{
    Manager::Get()->GetMessageManager()->SwitchTo(m_PageIndex);
    if (m_Log)
        m_Log->GetTextControl()->Clear();
}

FileTreeData* CompilerGCC::DoSwitchProjectTemporarily()
{
    wxTreeCtrl* tree = Manager::Get()->GetProjectManager()->GetTree();
    wxTreeItemId sel = tree->GetSelection();
    FileTreeData* ftd = (FileTreeData*)tree->GetItemData(sel);
    if (!ftd)
        return 0L;
    // copy ftd to a new instance, because after the SetProject() call
    // that follows, ftd will no longer be valid...
    FileTreeData* newFtd = new FileTreeData(*ftd);
    Manager::Get()->GetProjectManager()->SetProject(ftd->GetProject(), false);
    AskForActiveProject();

    return newFtd;
}

void CompilerGCC::AddToCommandQueue(const wxArrayString& commands)
{
    // loop added for compiler log when not working with Makefiles
    wxString mySimpleLog = wxString(COMPILER_SIMPLE_LOG);
    wxString myTargetChange = wxString(COMPILER_TARGET_CHANGE);
    wxString myWait = wxString(COMPILER_WAIT);
//    wxString myWaitEnd = wxString(COMPILER_WAIT_END);
//    ProjectBuildTarget* lastTarget = 0;
    ProjectBuildTarget* bt = m_pBuildingProject ? m_pBuildingProject->GetBuildTarget(m_BuildingTargetIdx) : 0;
    bool isLink = false;
    size_t count = commands.GetCount();
    for (size_t i = 0; i < count; ++i)
    {
        wxString cmd = commands[i];

        // logging
        if (cmd.StartsWith(mySimpleLog))
        {
            cmd.Remove(0, mySimpleLog.Length());
            m_CommandQueue.Add(new CompilerCommand(wxEmptyString, cmd, m_pBuildingProject, bt));
        }
        // compiler change
        else if (cmd.StartsWith(myTargetChange))
        {
//            cmd.Remove(0, myTargetChange.Length());
            // using other compiler now: find it and set it
//            lastTarget = m_Project->GetBuildTarget(cmd);

//            if (bt)
//            {
//                m_Project->SetCurrentlyCompilingTarget(bt);
//                SwitchCompiler(bt->GetCompilerID());
//                // re-apply the env vars for this target
//                if (CompilerFactory::CompilerIndexOK(m_CompilerId))
//                    CompilerFactory::GetCompiler(m_CompilerId)->GetCustomVars().ApplyVarsToEnvironment();
//                m_Project->GetCustomVars().ApplyVarsToEnvironment();
//                bt->GetCustomVars().ApplyVarsToEnvironment();
//
//            }
//            else
//                msgMan->Log(m_PageIndex, _("Can't locate target '%s'!"), cmd.c_str());
        }
        else if (cmd.StartsWith(myWait))
        {
//            cmd.Remove(0, myLinkStep.Length());
            isLink = true;
        }
//        else if (cmd.StartsWith(myWaitEnd))
//        {
////            cmd.Remove(0, myLinkStep.Length());
//            isLink = false;
//        }
        else
        {
            // compiler command
            CompilerCommand* p = new CompilerCommand(cmd, wxEmptyString, m_pBuildingProject, bt);
            p->mustWait = isLink;
            m_CommandQueue.Add(p);
//            Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("ADD: %s %s"), cmd.c_str(), isLink ? _T("(wait)") : _T(""));
            isLink = false;
        }
    }
}

bool CompilerGCC::IsProcessRunning(int idx) const
{
    // invalid process index
    if (idx >= (int)m_ParallelProcessCount)
        return false;
    // specific process
    if (idx >= 0)
        return m_Processes[idx] != 0;
    // any process (-1)
    for (size_t i = 0; i < m_ParallelProcessCount; ++i)
    {
        if (m_Processes[i] != 0)
            return true;
    }
    return false;
}

int CompilerGCC::GetNextAvailableProcessIndex() const
{
    for (size_t i = 0; i < m_ParallelProcessCount; ++i)
    {
        if (m_Processes[i] == 0 && m_Pid[i] == 0)
            return i;
    }
    return -1;
}

int CompilerGCC::GetActiveProcessCount() const
{
    size_t count = 0;
    for (size_t i = 0; i < m_ParallelProcessCount; ++i)
    {
        if (m_Processes[i] != 0)
            ++count;
    }
    return count;
}

int CompilerGCC::DoRunQueue()
{
    wxLogNull ln;

    MessageManager* msgMan = Manager::Get()->GetMessageManager();
//    msgMan->SwitchTo(m_PageIndex);

    // leave if already running
    int procIndex = GetNextAvailableProcessIndex();
    if (procIndex == -1)
    {
//        msgMan->Log(m_PageIndex, _("(all processes running)"));
        return -2;
    }

    // if next command is linking and compilation is still in progress, abort
    if (IsProcessRunning())
    {
        CompilerCommand* cmd = m_CommandQueue.Peek();
        if (cmd && cmd->mustWait)
        {
//            msgMan->Log(m_PageIndex, _("Waiting for compile to finish before linking..."));
            return -3;
        }
    }

    CompilerCommand* cmd = m_CommandQueue.Next();
    if (!cmd)
    {
        if (IsProcessRunning())
        {
            return 0;
        }

        while (1)
        {
            // keep switching build states until we have commands to run or reach end of states
            BuildStateManagement();
            cmd = m_CommandQueue.Next();
            if (!cmd && m_BuildState == bsNone && m_NextBuildState == bsNone)
            {
                NotifyJobDone(true);
                ResetBuildState();
                if (m_RunAfterCompile)
                {
                    m_RunAfterCompile = false;
                    if (Run() == 0)
                        DoRunQueue();
                }
                return 0;
            }

            if (cmd)
                break;
        }
    }

    m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
    wxString dir = cmd->dir;

    // if message manager is auto-hiding, this will lock it open
    msgMan->LockOpen();

    if (!cmd->message.IsEmpty())
    {
//        msgMan->Log(m_PageIndex, _T("[%u] %s"), procIndex, cmd->message.c_str());
        msgMan->Log(m_PageIndex, cmd->message);
        msgMan->LogToStdOut(cmd->message + _T('\n'));
    }

    if (cmd->command.IsEmpty())
    {
        int ret = DoRunQueue();
        delete cmd;
        return ret;
    }

    bool pipe = true;
    int flags = wxEXEC_ASYNC;
    if (cmd->isRun)
    {
        pipe = false; // no need to pipe output channels...
        flags |= wxEXEC_NOHIDE;
        dir = m_CdRun;
    #ifndef __WXMSW__
        // setup dynamic linker path
        wxSetEnv(_T("LD_LIBRARY_PATH"), _T(".:$LD_LIBRARY_PATH"));
    #endif
    }

    // special shell used only for build commands
    if (!cmd->isRun)
    {
    #ifndef __WXMSW__
        // run the command in a shell, so backtick'd expressions can be evaluated
        wxString shell = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/console_shell"), DEFAULT_CONSOLE_SHELL);
        cmd->command = shell + _T(" '") + cmd->command + _T("'");
//    #else
//    // TODO (mandrav#1#): Check windows version and substitute cmd.exe with command.com if needed.
//        cmd->command = _T("cmd /c ") + cmd->command;
    #endif
    }

    // create a new process
    m_Processes[procIndex] = new PipedProcess((void**)&m_Processes[procIndex], this, idGCCProcess1 + procIndex, pipe, dir);
    m_Pid[procIndex] = wxExecute(cmd->command, flags, m_Processes[procIndex]);
    if ( !m_Pid[procIndex] )
    {
        m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxRED, *wxWHITE));
        wxString err = wxString::Format(_("Execution of '%s' in '%s' failed."), cmd->command.c_str(), wxGetCwd().c_str());
        msgMan->Log(m_PageIndex, err);
        msgMan->LogToStdOut(err + _T('\n'));
        m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
        delete m_Processes[procIndex];
        m_Processes[procIndex] = 0;
        m_CommandQueue.Clear();
        ResetBuildState();
    }
    else
        m_timerIdleWakeUp.Start(100);

    delete cmd;
    return DoRunQueue();
}

void CompilerGCC::DoClearTargetMenu()
{
    if (m_TargetMenu)
    {
        wxMenuItemList& items = m_TargetMenu->GetMenuItems();
        while (wxMenuItemList::Node* node = items.GetFirst())
        {
            if (node->GetData())
                m_TargetMenu->Delete(node->GetData());
        }
// mandrav: The following lines DO NOT clear the menu!
//        wxMenuItemList& items = m_TargetMenu->GetMenuItems();
//        bool olddelete=items.GetDeleteContents();
//        items.DeleteContents(true);
//        items.Clear();
//        items.DeleteContents(olddelete);
    }
    m_HasTargetAll = false;
}

void CompilerGCC::DoRecreateTargetMenu()
{
    if (!m_IsAttached)
        return;

    DoClearTargetMenu();
    if (m_ToolTarget)
        m_ToolTarget->Clear();
    if (!CheckProject())
        return;

    if (m_Project->GetBuildTargetsCount() == 0)
        return;

    // find out if at least one target is included in "all"
    // (if not, no need to add "all" in menus and target combo)
    bool atLeastOneBuildableTarget = false;
    for (int i = 0; i < m_Project->GetBuildTargetsCount(); ++i)
    {
        ProjectBuildTarget* bt = m_Project->GetBuildTarget(i);
        if (bt->GetIncludeInTargetAll())
        {
            atLeastOneBuildableTarget = true;
            break;
        }
    }

    m_TargetIndex = m_Project->GetActiveBuildTarget();
    if (atLeastOneBuildableTarget)
    {
        if (m_TargetMenu)
            m_TargetMenu->AppendCheckItem(idMenuSelectTargetAll, _("All"), _("Build target 'all' in current project"));
        if (m_ToolTarget)
            m_ToolTarget->Append(_("All"));
    }
    else
    {
        if (m_TargetIndex == -1)
            m_TargetIndex = 0;
    }
    m_HasTargetAll = atLeastOneBuildableTarget;

    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

        wxString caption;
        caption.Printf(_("Build target '%s' in current project"), target->GetTitle().c_str());
        if (m_TargetMenu)
            m_TargetMenu->AppendCheckItem(idMenuSelectTargetOther[x], target->GetTitle(), caption);
        if (m_ToolTarget)
            m_ToolTarget->Append(target->GetTitle());
    }
    Connect( idMenuSelectTargetOther[0],  idMenuSelectTargetOther[MAX_TARGETS - 1],
            wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &CompilerGCC::OnSelectTarget );
    DoUpdateTargetMenu();

    SwitchCompiler(m_Project->GetCompilerID());
}

void CompilerGCC::DoUpdateTargetMenu()
{
    // keep the last selected target name for BuildWorkspace() and friends
    m_LastTargetName.Clear();
    if (m_Project && m_TargetIndex != -1)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(m_TargetIndex);
        if (target)
            m_LastTargetName = target->GetTitle();
    }

    if (!m_TargetMenu)
        return;

    if (!m_HasTargetAll && m_TargetIndex == -1)
        m_TargetIndex = 0;

    if (m_Project)
        m_Project->SetActiveBuildTarget(m_TargetIndex);

    if (m_HasTargetAll)
        m_TargetMenu->Check(idMenuSelectTargetAll, m_TargetIndex == -1);
    for (int i = 0; i < MAX_TARGETS; ++i)
    {
        wxMenuItem* item = m_TargetMenu->FindItem(idMenuSelectTargetOther[i]);
        if (!item || !item->IsCheckable())
            continue;
        item->Check(i == m_TargetIndex);
    }
    if (m_ToolTarget)
        m_ToolTarget->SetSelection(m_TargetIndex + (m_HasTargetAll ? 1 : 0));
}

void CompilerGCC::DoPrepareQueue()
{
    if (m_CommandQueue.GetCount() == 0)
    {
        ClearLog();
        DoClearErrors();
        wxStartTimer();
    }
    Manager::Yield();
}

ProjectBuildTarget* CompilerGCC::DoAskForTarget()
{
    if (!CheckProject())
        return 0L;

    return m_Project->GetBuildTarget(m_TargetIndex);
}

int CompilerGCC::DoGUIAskForTarget()
{
    if (!CheckProject())
        return -1;

    return m_Project->SelectTarget(m_TargetIndex);
}

void CompilerGCC::DoDeleteTempMakefile()
{
    // delete temp Makefile
    if (m_DeleteTempMakefile && !m_LastTempMakefile.IsEmpty())
        wxRemoveFile(m_LastTempMakefile);
    m_LastTempMakefile = _T("");
}

bool CompilerGCC::UseMake(ProjectBuildTarget* target)
{
    if (!m_Project)
        return false;
    wxString idx = m_Project->GetCompilerID();
    if (CompilerFactory::GetCompiler(idx))
    {
        if (m_Project->IsMakefileCustom())
            return true;
        else
        {
            if (CompilerFactory::GetCompiler(idx)->GetSwitches().buildMethod == cbmUseMake)
            {
                // since November 28 2005, "make" is no more a valid build method
                // except if the project is set to use a custom Makefile
                // (and is selected automatically, in this case).
                // just notify the user once about this change
                if (!Manager::Get()->GetConfigManager(_T("compiler"))->Exists(_T("/notify_no_make")))
                {
                    Manager::Get()->GetConfigManager(_T("compiler"))->Set(_T("/notify_no_make"));
                    cbMessageBox(_("Code::Blocks no longer supports the \"GNU make\" build method, "
                                    "except for projects which\nare using a custom Makefile "
                                    "(in which case, this build method is selected automatically).\n"
                                    "This poses no problem on the overall build process and this "
                                    "notification is displayed\n"
                                    "just to alert you about this change.\n\n"
                                    "No further action is required on your part."),
                                    _("Information"),
                                    wxICON_INFORMATION);
                }
            }
        }
    }
    return false;
}

bool CompilerGCC::CompilerValid(ProjectBuildTarget* target)
{
    wxString idx = target ? target->GetCompilerID() : (m_Project ? m_Project->GetCompilerID() : CompilerFactory::GetDefaultCompilerID());
    bool ret = CompilerFactory::GetCompiler(idx);
    if (!ret)
    {
        wxString msg;
        msg.Printf(_("This %s is configured to use an invalid compiler.\nThe operation failed..."), target ? _("target") : _("project"));
        cbMessageBox(msg, _("Error"), wxICON_ERROR);
    }
    return ret;
}

bool CompilerGCC::DoCreateMakefile(bool temporary, const wxString& makefile)
{
    DoDeleteTempMakefile();

    // display error about incorrect compile environment
    if (!m_EnvironmentMsg.IsEmpty())
    {
        cbMessageBox(m_EnvironmentMsg, _("Error"), wxICON_ERROR);
        m_EnvironmentMsg.Clear(); // once is enough, per session...
    }

    // verify current project
    AskForActiveProject();
    if (!m_Project)
        return false;

    if (UseMake())
    {
        // if the project has a custom makefile, use that (i.e. don't create makefile)
        if (temporary && m_Project->IsMakefileCustom())
        {
            m_LastTempMakefile = m_Project->GetMakefile();
            m_DeleteTempMakefile = false;
            return true;
        }

        // invoke Makefile generation
        if (temporary)
            m_LastTempMakefile = wxFileName::CreateTempFileName(_T("cbmk"), 0L);
        else
        {
            m_LastTempMakefile = makefile;
            if (m_LastTempMakefile.IsEmpty())
            {
                m_LastTempMakefile = ProjectMakefile();
                if (m_LastTempMakefile.IsEmpty())
                    m_LastTempMakefile = _T("Makefile");
            }
        }
    }

    PrintBanner();
    wxSetWorkingDirectory(m_Project->GetBasePath());

    if (UseMake())
    {
        MakefileGenerator generator(this, m_Project, m_LastTempMakefile, m_PageIndex);
        bool ret = generator.CreateMakefile();

        // if exporting Makefile, reset variable so that it's not deleted on
        // next Makefile generation :)
        if (!temporary)
            m_LastTempMakefile = _T("");
        m_DeleteTempMakefile = temporary;

        return ret;
    }
    return true;
}

void CompilerGCC::PrintBanner(cbProject* prj, ProjectBuildTarget* target)
{
    if (!CompilerValid(target))
        return;
    Manager::Get()->GetMessageManager()->Open();

    if (!prj)
        prj = m_Project;

    m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
    wxString banner;
    banner.Printf(_("-------------- Build: %s in %s ---------------"),
                    target
                        ? target->GetTitle().c_str()
                        : _("\"no target\""),
                    prj
                        ? prj->GetTitle().c_str()
                        : _("\"no project\"")
                );
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, banner);
    Manager::Get()->GetMessageManager()->LogToStdOut(banner + _T('\n'));
}

void CompilerGCC::DoGotoNextError()
{
    m_Errors.Next();
    m_pListLog->FocusError(m_Errors.GetFocusedError());
}

void CompilerGCC::DoGotoPreviousError()
{
    m_Errors.Previous();
    m_pListLog->FocusError(m_Errors.GetFocusedError());
}

void CompilerGCC::DoClearErrors()
{
    m_Errors.Clear();
    m_pListLog->Clear();
    m_NotifiedMaxErrors = false;
}

int CompilerGCC::RunSingleFile(const wxString& filename)
{
    wxFileName fname(filename);

    if (fname.GetExt() == _T("script"))
    {
        Manager::Get()->GetScriptingManager()->LoadScript(filename);
        return 0;
    }

    m_CdRun = fname.GetPath();
    fname.SetExt(FileFilters::EXECUTABLE_EXT);
    wxString exe_filename = fname.GetFullPath();
    wxString cmd;

#ifndef __WXMSW__
    // for non-win platforms, use m_ConsoleTerm to run the console app
    wxString term = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/console_terminal"), DEFAULT_CONSOLE_TERM);
    term.Replace(_T("$TITLE"), _T("'") + exe_filename + _T("'"));
    cmd << term << _T(" ");
#endif
    wxString baseDir = ConfigManager::GetExecutableFolder();
#ifdef __WXMSW__
    #define CONSOLE_RUNNER "cb_console_runner.exe"
#else
    #define CONSOLE_RUNNER "cb_console_runner"
#endif
    if (wxFileExists(baseDir + wxT("/" CONSOLE_RUNNER)))
        cmd << baseDir << wxT("/" CONSOLE_RUNNER " ");

    cmd << _T("\"");
    cmd << exe_filename;
    cmd << _T("\"");

    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Checking for existence: %s"), exe_filename.c_str());
    if (!wxFileExists(exe_filename))
    {
        int ret = cbMessageBox(_("It seems that this file has not been built yet.\n"
                                "Do you want to build it now?"),
                                _("Information"),
                                wxYES_NO | wxCANCEL | wxICON_QUESTION);
        switch (ret)
        {
            case wxID_YES:
            {
                m_RunAfterCompile = true;
                Build(wxEmptyString);
                return -1;
            }
            case wxID_NO:
                break;
            default:
                return -1;
        }
    }

    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(m_CdRun);
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Executing: %s (in %s)"), cmd.c_str(), m_CdRun.c_str());
    m_CommandQueue.Add(new CompilerCommand(cmd, wxEmptyString, 0, 0, true));
    return 0;
}

int CompilerGCC::Run(const wxString& target)
{
    if (!CheckProject())
        return -1;
    return Run(m_Project->GetBuildTarget(target.IsEmpty() ? m_LastTargetName : target));
}

int CompilerGCC::Run(ProjectBuildTarget* target)
{
    if (!CheckProject())
    {
        if (Manager::Get()->GetEditorManager()->GetActiveEditor())
            return RunSingleFile(Manager::Get()->GetEditorManager()->GetActiveEditor()->GetFilename());
        return -1;
    }
    DoPrepareQueue();
    if (!CompilerValid(target))
        return -1;

    if (!target)
    {
        if (m_TargetIndex == -1) // only ask for target if target 'all' is selected
        {
            int idx = -1;
            int bak = m_TargetIndex;
            if (m_Project->GetBuildTargetsCount() == 1)
                idx = 0;
            else
                idx = DoGUIAskForTarget();

            m_TargetIndex = idx;
            target = DoAskForTarget();
            m_TargetIndex = bak;
        }
        else
            target = DoAskForTarget();
    }

    if (!target)
        return -1;

    m_Project->SetCurrentlyCompilingTarget(target); // help macros manager

    wxString out = UnixFilename(target->GetOutputFilename());
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out);

    wxString cmd;
    wxFileName f(out);
    f.MakeAbsolute(m_Project->GetBasePath());

    m_CdRun = target->GetWorkingDir();
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(m_CdRun);
    wxFileName cd(m_CdRun);
    if (cd.IsRelative())
        cd.MakeAbsolute(m_Project->GetBasePath());
    m_CdRun = cd.GetFullPath();

    // for console projects, use helper app to wait for a key after
    // execution ends...
    if (target->GetTargetType() == ttConsoleOnly)
    {
#ifndef __WXMSW__
        // for non-win platforms, use m_ConsoleTerm to run the console app
        wxString term = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/console_terminal"), DEFAULT_CONSOLE_TERM);
        term.Replace(_T("$TITLE"), _T("'") + m_Project->GetTitle() + _T("'"));
        cmd << term << _T(" 'LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH ");
#endif
        // should console runner be used?
        if (target->GetUseConsoleRunner())
        {
            wxString baseDir = ConfigManager::GetExecutableFolder();
#ifdef __WXMSW__
    #define CONSOLE_RUNNER "cb_console_runner.exe"
#else
    #define CONSOLE_RUNNER "cb_console_runner"
#endif
            if (wxFileExists(baseDir + wxT("/" CONSOLE_RUNNER)))
                cmd << baseDir << wxT("/" CONSOLE_RUNNER " ");
        }
    }

    if (target->GetTargetType() == ttDynamicLib ||
        target->GetTargetType() == ttStaticLib)
    {
        // check for hostapp
        if (target->GetHostApplication().IsEmpty())
        {
            cbMessageBox(_("You must select a host application to \"run\" a library..."));
            m_Project->SetCurrentlyCompilingTarget(0);
            return -1;
        }
        wxString tmp = target->GetHostApplication();
        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(tmp);
        cmd << _T("\"") << tmp << _T("\" ") << target->GetExecutionParameters();
    }
    else if (target->GetTargetType() != ttCommandsOnly)
    {
        cmd << _T("\"");
        cmd << f.GetFullPath();
        cmd << _T("\" ");
        cmd << target->GetExecutionParameters();
#ifndef __WXMSW__
        // closing single-quote for xterm command line
        if (target->GetTargetType() == ttConsoleOnly)
            cmd << _T("'");
#endif
    }
    else
    {
        // commands-only target?
        cbMessageBox(_("You can't \"run\" a commands-only target..."));
        m_Project->SetCurrentlyCompilingTarget(0);
        return -1;
    }

    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Checking for existence: %s"), f.GetFullPath().c_str());
    if (!wxFileExists(f.GetFullPath()))
    {
        int ret = cbMessageBox(_("It seems that this project has not been built yet.\n"
                                "Do you want to build it now?"),
                                _("Information"),
                                wxYES_NO | wxCANCEL | wxICON_QUESTION);
        switch (ret)
        {
            case wxID_YES:
            {
                m_Project->SetCurrentlyCompilingTarget(0);
                m_RunAfterCompile = true;
                Build(target);
                return -1;
            }
            case wxID_NO:
                break;
            default:
                m_Project->SetCurrentlyCompilingTarget(0);
                return -1;
        }
    }

    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Executing: %s (in %s)"), cmd.c_str(), m_CdRun.c_str());
    m_CommandQueue.Add(new CompilerCommand(cmd, wxEmptyString, m_Project, target, true));

    m_Project->SetCurrentlyCompilingTarget(0);
    return 0;
}

wxString CompilerGCC::GetMakeCommandFor(MakeCommand cmd, ProjectBuildTarget* target)
{
    if (!m_Project)
        return wxEmptyString;
    wxString command = target ? target->GetMakeCommandFor(cmd) : m_Project->GetMakeCommandFor(cmd);
    command.Replace(_T("$makefile"), m_Project->GetMakefile());
    command.Replace(_T("$make"), CompilerFactory::GetCompiler(m_CompilerId)->GetPrograms().MAKE);
    command.Replace(_T("$target"), target ? target->GetTitle() : _T(""));
//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("Make: %s"), command.c_str());
    return command;
}

int CompilerGCC::Clean(const wxString& target)
{
    if (!CheckProject())
        return -1;
    return Clean(m_Project->GetBuildTarget(target.IsEmpty() ? m_LastTargetName : target));
}

int CompilerGCC::Clean(ProjectBuildTarget* target)
{
    // make sure all project files are saved
    if (m_Project && !m_Project->SaveAllFiles())
        Manager::Get()->GetMessageManager()->Log(_("Could not save all files..."));

    DoPrepareQueue();
    if (!CompilerValid(target))
        return -1;

//    Manager::Get()->GetMacrosManager()->Reset();

    if (m_Project)
        wxSetWorkingDirectory(m_Project->GetBasePath());
    CompilerFactory::GetCompiler(m_CompilerId)->Init(m_Project);

    if (UseMake(target))
    {
        wxString cmd = GetMakeCommandFor(mcClean, target);
        m_CommandQueue.Add(new CompilerCommand(cmd, wxEmptyString, m_Project, target));
        return DoRunQueue();
    }
    else
    {
        wxArrayString clean;
        if (m_Project)
        {
            DirectCommands dc(this, CompilerFactory::GetCompiler(m_CompilerId), m_Project, m_PageIndex);
            clean = dc.GetCleanCommands(target, true);
            Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Cleaning %s..."), target ? target->GetTitle().c_str() : m_Project->GetTitle().c_str());
        }
        else
        {
            DirectCommands dc(this, CompilerFactory::GetDefaultCompiler(), 0, m_PageIndex);
            clean = dc.GetCleanSingleFileCommand(Manager::Get()->GetEditorManager()->GetActiveEditor()->GetFilename());
            Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Cleaning object and output files..."));
        }
        for (unsigned int i = 0; i < clean.GetCount(); ++i)
        {
            wxRemoveFile(clean[i]);
        }
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Done."));
        Manager::Get()->GetMessageManager()->Close();
    }
    return 0;
}

int CompilerGCC::DistClean(const wxString& target)
{
    if (!CheckProject())
        return -1;
    return DistClean(m_Project->GetBuildTarget(target.IsEmpty() ? m_LastTargetName : target));
}

int CompilerGCC::DistClean(ProjectBuildTarget* target)
{
    // make sure all project files are saved
    if (m_Project && !m_Project->SaveAllFiles())
        Manager::Get()->GetMessageManager()->Log(_("Could not save all files..."));

    DoPrepareQueue();
    if (!CompilerValid(target))
        return -1;

//    Manager::Get()->GetMacrosManager()->Reset();

    if (m_Project)
        wxSetWorkingDirectory(m_Project->GetBasePath());
    CompilerFactory::GetCompiler(m_CompilerId)->Init(m_Project);

    if (UseMake(target))
    {
        wxString cmd = GetMakeCommandFor(mcDistClean, target);
        m_CommandQueue.Add(new CompilerCommand(cmd, wxEmptyString, m_Project, target));
        return DoRunQueue();
    }
    else
    {
        NotImplemented(_T("CompilerGCC::Distclean() without a custom Makefile"));
        return -1;
    }
    return 0;
}

void CompilerGCC::OnExportMakefile(wxCommandEvent& event)
{
    cbMessageBox(_("This functionality has been temporarily removed from Code::Blocks.\n"
                    "With all the updates from the last couple of months, the makefile exporter "
                    "is not anymore in-sync with the rest of the build process.\n"
                    "We are sorry for the inconvenience..."),
                    _T("Warning"),
                    wxICON_WARNING);
    return;

    if (!CompilerValid())
        return;
    wxString makefile = wxGetTextFromUser(_("Please enter the \"Makefile\" name:"), _("Export Makefile"), ProjectMakefile());
    if (makefile.IsEmpty())
        return;

    wxSetWorkingDirectory(m_Project->GetBasePath());
    if (UseMake())
    {
        DoCreateMakefile(false, makefile);
    }
    else
    {
        MakefileGenerator generator(this, m_Project, makefile, m_PageIndex);
        generator.CreateMakefile();
    }
    wxString msg;
    msg.Printf(_("\"%s\" has been exported in the same directory as the project file."), makefile.c_str());
    cbMessageBox(msg);
}

void CompilerGCC::InitBuildState(BuildJob job, const wxString& target)
{
    wxString ftgt = target.IsEmpty() ? m_LastTargetName : target;
    m_BuildJob = job;
    m_BuildStateTargetIsAll = ftgt.IsEmpty() || ftgt.Lower() == _("all");
    m_BuildState = bsNone;
    m_NextBuildState = bsProjectPreBuild;
    m_pBuildingProject = Manager::Get()->GetProjectManager()->GetActiveProject();
    m_pLastBuildingProject = 0;
    m_pLastBuildingTarget = 0;
    m_BuildingTargetName = ftgt;
    m_BuildDepsIndex = 0;
    m_BuildDeps.Clear();
    m_CommandQueue.Clear();
}

void CompilerGCC::ResetBuildState()
{
    if (m_pBuildingProject)
        m_pBuildingProject->SetCurrentlyCompilingTarget(0);
    else if (m_Project)
        m_Project->SetCurrentlyCompilingTarget(0);

    // reset state
    m_BuildJob = bjIdle;
    m_BuildState = bsNone;
    m_NextBuildState = bsNone;
    m_pBuildingProject = 0;
    m_BuildingProjectIdx = 0;
    m_BuildingTargetIdx = -1;
    m_BuildingTargetName.Clear();

    m_pLastBuildingProject = 0;
    m_pLastBuildingTarget = 0;

    m_BuildDeps.Clear();
    m_BuildDepsIndex = 0;

    m_CommandQueue.Clear();

    // Clear the Active Project's currently compiling target
    // NOTE (rickg22#1#): This way we can prevent Codeblocks from shutting down
    // when a project is being compiled.
    // NOTE (mandrav#1#): Make sure no open project is marked as compiling
    ProjectsArray* arr = Manager::Get()->GetProjectManager()->GetProjects();
    for (size_t i = 0; i < arr->GetCount(); ++i)
    {
        arr->Item(i)->SetCurrentlyCompilingTarget(0);
    }
}

wxString StateToString(BuildState bs)
{
    switch (bs)
    {
        case bsNone: return _T("bsNone");
        case bsProjectPreBuild: return _T("bsProjectPreBuild");
        case bsTargetPreBuild: return _T("bsTargetPreBuild");
        case bsTargetBuild: return _T("bsTargetBuild");
        case bsTargetPostBuild: return _T("bsTargetPostBuild");
        case bsProjectPostBuild: return _T("bsProjectPostBuild");
        case bsProjectDone: return _T("bsProjectDone");
    }
    return _T("Huh!?!");
}

BuildState CompilerGCC::GetNextStateBasedOnJob()
{
    switch (m_BuildState)
    {
        case bsProjectPreBuild: return bsTargetPreBuild;
        case bsTargetPreBuild: return bsTargetBuild;
        case bsTargetBuild: return bsTargetPostBuild;

        // advance target in the project
        case bsTargetPostBuild:
        {
            if (m_BuildJob != bjTarget)
            {
                // switch to next target in project
                if (m_BuildStateTargetIsAll)
                {
                    if (m_BuildingTargetIdx < m_pBuildingProject->GetBuildTargetsCount() - 1)
                    {
                        ++m_BuildingTargetIdx;
                        return bsTargetPreBuild;
                    }
                }
                return bsProjectPostBuild;
            }
            m_pBuildingProject->SetCurrentlyCompilingTarget(0);
            break;
        }

        case bsProjectPostBuild: return bsProjectDone;

        case bsProjectDone:
        {
            // switch to next project in workspace
            if (m_pBuildingProject)
                m_pBuildingProject->SetCurrentlyCompilingTarget(0);
            if (m_BuildJob == bjWorkspace)
            {
                ++m_BuildDepsIndex;
                if (m_BuildDepsIndex < (int)m_BuildDeps.GetCount())
                {
                    m_BuildingProjectIdx = m_BuildDeps[m_BuildDepsIndex];
                    if (m_BuildingProjectIdx < (int)Manager::Get()->GetProjectManager()->GetProjects()->GetCount())
                    {
                        m_pBuildingProject = Manager::Get()->GetProjectManager()->GetProjects()->Item(m_BuildingProjectIdx);
                        m_NextBuildState = bsProjectPreBuild;
                        DoBuild(m_pBuildingProject);
                        return bsProjectPreBuild;
                    }
                }
            }
            break;
        }

        default:
            break;
    }
    return bsNone;
}

void CompilerGCC::BuildStateManagement()
{
//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("BuildStateManagement"));
    if (IsProcessRunning())
    {
        return;
    }

    Manager::Yield();
    if (!m_pBuildingProject)
    {
        ResetBuildState();
        return;
    }

    ProjectBuildTarget* bt = m_pBuildingProject->GetBuildTarget(m_BuildingTargetIdx);
    if (!bt)
    {
        ResetBuildState();
        return;
    }

    if (m_pBuildingProject != m_pLastBuildingProject || bt != m_pLastBuildingTarget)
    {
        Manager::Get()->GetMacrosManager()->RecalcVars(m_pBuildingProject, Manager::Get()->GetEditorManager()->GetActiveEditor(), bt);
        if (bt)
            SwitchCompiler(bt->GetCompilerID());

        bool hasLogged = m_Log->GetTextControl()->GetInsertionPoint() != 0;
//        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("CHANGE *****> m_BuildState=%s, m_NextBuildState=%s, m_pBuildingProject=%p, bt=%p (%p)"), StateToString(m_BuildState).c_str(), StateToString(m_NextBuildState).c_str(), m_pBuildingProject, bt, m_pLastBuildingTarget);
        if (!hasLogged || (m_pBuildingProject == m_pLastBuildingProject && m_NextBuildState == bsTargetPreBuild) || m_NextBuildState == bsProjectPreBuild)
        {
            if (hasLogged)
                Manager::Get()->GetMessageManager()->Log(m_PageIndex, wxEmptyString);
            PrintBanner(m_pBuildingProject, bt);
        }

        if (m_pBuildingProject != m_pLastBuildingProject)
        {
            m_pLastBuildingProject = m_pBuildingProject;
            wxSetWorkingDirectory(m_pBuildingProject->GetBasePath());
            CompilerFactory::GetCompiler(m_CompilerId)->Init(m_pBuildingProject);
        }
        if (bt != m_pLastBuildingTarget)
        {
            // check if we 're switching compilers, now that we 're switching targets
            // if so, we must Init() the target's compiler...
            Compiler* last = m_pLastBuildingTarget ? CompilerFactory::GetCompiler(m_pLastBuildingTarget->GetCompilerID()) : 0;
            Compiler* curr = bt ? CompilerFactory::GetCompiler(bt->GetCompilerID()) : 0;
            if (curr && last != curr)
                curr->Init(m_pBuildingProject);
            m_pLastBuildingTarget = bt;
        }
    }

    m_pBuildingProject->SetCurrentlyCompilingTarget(bt);
    DirectCommands dc(this, CompilerFactory::GetCompiler(m_CompilerId), m_pBuildingProject, m_PageIndex);
    dc.m_doYield = true;

//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("NOCHANGE *****> m_BuildState=%s, m_NextBuildState=%s, m_pBuildingProject=%p, bt=%p"), StateToString(m_BuildState).c_str(), StateToString(m_NextBuildState).c_str(), m_pBuildingProject, bt);

    m_BuildState = m_NextBuildState;
    wxArrayString cmds;
    switch (m_NextBuildState)
    {
        case bsProjectPreBuild:
        {
            // run project pre-build steps
            cmds = dc.GetPreBuildCommands(0);
            break;
        }

        case bsTargetPreBuild:
        {
            // check if it should build with "All"
            if (m_BuildStateTargetIsAll && !bt->GetIncludeInTargetAll())
                break;
            // run target pre-build steps
            cmds = dc.GetPreBuildCommands(bt);
            break;
        }

        case bsTargetBuild:
        {
            if (m_BuildStateTargetIsAll && !bt->GetIncludeInTargetAll())
                break;
            // run target build
            cmds = dc.GetCompileCommands(bt);
            bool hasCommands = cmds.GetCount();
            m_RunTargetPostBuild = hasCommands;
            m_RunProjectPostBuild = hasCommands;
            if (!hasCommands)
            {
                Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Target is up to date."));
                Manager::Get()->GetMessageManager()->LogToStdOut(_("Target is up to date.\n"));
            }
            break;
        }

        case bsTargetPostBuild:
        {
            // check if it should build with "All"
            if (!m_BuildStateTargetIsAll || bt->GetIncludeInTargetAll())
            {
                // run target post-build steps
                if (m_RunTargetPostBuild || bt->GetAlwaysRunPostBuildSteps())
                    cmds = dc.GetPostBuildCommands(bt);
            }
            // reset
            m_RunTargetPostBuild = false;
            break;
        }

        case bsProjectPostBuild:
        {
            m_pLastBuildingTarget = 0;
            // run project post-build steps
            if (m_RunProjectPostBuild || m_pBuildingProject->GetAlwaysRunPostBuildSteps())
                cmds = dc.GetPostBuildCommands(0);
            // reset
            m_RunProjectPostBuild = false;
            break;
        }

        case bsProjectDone:
        {
            m_pLastBuildingProject = 0;
            break;
        }

        default:
            break;
    }
    m_NextBuildState = GetNextStateBasedOnJob();
    AddToCommandQueue(cmds);
    Manager::Yield();
}

int CompilerGCC::DoBuild(cbProject* prj)
{
    if (!prj)
        return -2;

    // make sure all project files are saved
    if (prj && !prj->SaveAllFiles())
        Manager::Get()->GetMessageManager()->Log(_("Could not save all files..."));

    m_pBuildingProject = prj;

    // we 'll locate the target by searching, 'cause we need its index
    m_BuildingTargetIdx = m_BuildingTargetName.IsEmpty() ? -1 : -2;
    ProjectBuildTarget* bt = 0;
    for (int i = 0; i < prj->GetBuildTargetsCount(); ++i)
    {
        ProjectBuildTarget* bt_search =  prj->GetBuildTarget(i);
        if (bt_search->GetTitle() == m_BuildingTargetName)
        {
            bt = bt_search;
            m_BuildingTargetIdx = i;
            break;
        }
    }
    if (m_BuildingTargetIdx == -2)
    {
        if (m_Log->GetTextControl()->GetInsertionPoint() != 0)
            Manager::Get()->GetMessageManager()->Log(m_PageIndex, wxEmptyString);
        PrintBanner(m_pBuildingProject, 0);
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("%s doesn't have a target named \"%s\"..."), prj->GetTitle().c_str(), m_BuildingTargetName.c_str());
        m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(COLOUR_MAROON));
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Build aborted."));
        m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)));
        return -2;
    }
    if (m_BuildingTargetIdx == -1)
    {
        if (m_HasTargetAll)
        {
            m_BuildingTargetIdx = 0;
            bt =  prj->GetBuildTarget(0);
        }
        else
        {
            m_BuildingTargetIdx = prj->SelectTarget();
            bt = prj->GetBuildTarget(m_BuildingTargetIdx);
        }
    }

    if (!bt || !CompilerValid(bt))
        return -2;

//    Manager::Get()->GetMacrosManager()->Reset();

    wxString cmd;
    if (UseMake(bt))
    {
        wxString cmd = GetMakeCommandFor(mcBuild, bt);
        m_CommandQueue.Add(new CompilerCommand(cmd, wxEmptyString, prj, bt));
    }
    else
    {
        BuildStateManagement();
    }
    return 0;
}

void CompilerGCC::CalculateWorkspaceDependencies()
{
    m_BuildDeps.Clear();
    ProjectsArray* arr = Manager::Get()->GetProjectManager()->GetProjects();
    for (size_t i = 0; i < arr->GetCount(); ++i)
    {
        CalculateProjectDependencies(arr->Item(i));
    }
}

void CompilerGCC::CalculateProjectDependencies(cbProject* prj)
{
    int prjidx = Manager::Get()->GetProjectManager()->GetProjects()->Index(prj);
    const ProjectsArray* arr = Manager::Get()->GetProjectManager()->GetDependenciesForProject(prj);
    if (!arr || !arr->GetCount())
    {
        // no dependencies; add the project in question and exit
        if (m_BuildDeps.Index(prjidx) == wxNOT_FOUND)
        {
//            Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("Adding dependency: %s"), prj->GetTitle().c_str());
            m_BuildDeps.Add(prjidx);
        }
        return;
    }

    for (size_t i = 0; i < arr->GetCount(); ++i)
    {
        cbProject* thisprj = arr->Item(i);
        if (!Manager::Get()->GetProjectManager()->CausesCircularDependency(prj, thisprj))
        {
            // recursively check dependencies
            CalculateProjectDependencies(thisprj);

            // find out project's index in full (open) projects array
            ProjectsArray* parr = Manager::Get()->GetProjectManager()->GetProjects();
            int idx = parr->Index(thisprj);
            if (idx != wxNOT_FOUND)
            {
                // avoid duplicates
                if (m_BuildDeps.Index(idx) == wxNOT_FOUND)
                {
//                    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("Adding dependency: %s"), thisprj->GetTitle().c_str());
                    m_BuildDeps.Add(idx);
                }
            }
        }
        else
        {
            m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(COLOUR_MAROON));
            Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Circular dependency detected between \"%s\" and \"%s\". Skipping..."), prj->GetTitle().c_str(), thisprj->GetTitle().c_str());
            m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)));
        }
    }

    // always add the project in question
    if (m_BuildDeps.Index(prjidx) == wxNOT_FOUND)
    {
//        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("Adding dependency: %s"), prj->GetTitle().c_str());
        m_BuildDeps.Add(prjidx);
    }
}

int CompilerGCC::Build(const wxString& target)
{
    if (CheckDebuggerIsRunning())
        return -1;

    if (!CheckProject())
    {
        // no active project
        if (Manager::Get()->GetEditorManager()->GetActiveEditor())
            return CompileFile(Manager::Get()->GetEditorManager()->GetActiveEditor()->GetFilename());
        return -1;
    }
    DoClearErrors();
    DoPrepareQueue();

    ProjectBuildTarget* bt = m_Project->GetBuildTarget(target);
    if (UseMake(bt))
    {
        // make sure all project files are saved
        if (m_Project && !m_Project->SaveAllFiles())
            Manager::Get()->GetMessageManager()->Log(_("Could not save all files..."));
        wxString cmd = GetMakeCommandFor(mcBuild, bt);
        m_CommandQueue.Add(new CompilerCommand(cmd, wxEmptyString, m_Project, bt));
    }
    else
    {
        InitBuildState(bjProject, target);
        CalculateProjectDependencies(m_Project);
        m_BuildingProjectIdx = m_BuildDeps[m_BuildDepsIndex];
        m_pBuildingProject = Manager::Get()->GetProjectManager()->GetProjects()->Item(m_BuildingProjectIdx);
        if (m_BuildDeps.GetCount() > 1)
            m_BuildJob = bjWorkspace;
        if (DoBuild(m_pBuildingProject))
            return -2;
    }
    return DoRunQueue();
}

int CompilerGCC::Build(ProjectBuildTarget* target)
{
    return Build(target ? target->GetTitle() : _T(""));
}

int CompilerGCC::Rebuild(const wxString& target)
{
    if (!CheckProject())
        return -1;
    return Rebuild(m_Project->GetBuildTarget(target.IsEmpty() ? m_LastTargetName : target));
}

int CompilerGCC::Rebuild(ProjectBuildTarget* target)
{
    if (CheckDebuggerIsRunning())
        return -1;

    // make sure all project files are saved
    if (m_Project && !m_Project->SaveAllFiles())
        Manager::Get()->GetMessageManager()->Log(_("Could not save all files..."));

    DoPrepareQueue();
    if (!CompilerValid(target))
        return -1;

//    Manager::Get()->GetMacrosManager()->Reset();

    CompilerFactory::GetCompiler(m_CompilerId)->Init(m_Project);

    if (UseMake(target))
    {
        wxString cmd = GetMakeCommandFor(mcClean, target);
        m_CommandQueue.Add(new CompilerCommand(cmd, wxEmptyString, m_Project, target));
        cmd = GetMakeCommandFor(mcBuild, target);
        m_CommandQueue.Add(new CompilerCommand(cmd, wxEmptyString, m_Project, target));
    }
    else
    {
        Clean(target);
        Build(target);
    }
    return DoRunQueue();
}

int CompilerGCC::BuildWorkspace(const wxString& target)
{
    if (CheckDebuggerIsRunning())
        return -1;

    DoPrepareQueue();
    ClearLog();

    // save files from all projects as they might require each other...
    ProjectsArray* arr = Manager::Get()->GetProjectManager()->GetProjects();
    if (arr)
    {
        for (size_t i = 0; i < arr->GetCount(); ++i)
        {
            cbProject* prj = arr->Item(i);
            if (prj && !prj->SaveAllFiles())
                Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Could not save all files of %s..."), prj->GetTitle().c_str());
        }
    }

    InitBuildState(bjWorkspace, target);
    CalculateWorkspaceDependencies();
    if (m_BuildDepsIndex >= (int)m_BuildDeps.GetCount())
        return -1;
    m_BuildingProjectIdx = m_BuildDeps[m_BuildDepsIndex];
    m_pBuildingProject = Manager::Get()->GetProjectManager()->GetProjects()->Item(m_BuildingProjectIdx);
    if (!m_pBuildingProject)
        return -1;
    DoBuild(m_pBuildingProject);
    return DoRunQueue();
}

int CompilerGCC::RebuildWorkspace(const wxString& target)
{
    if (CheckDebuggerIsRunning())
        return -1;

    int ret = CleanWorkspace(target);
    if (ret != 0)
        return ret;
    ResetBuildState();
    return BuildWorkspace(target);
}

int CompilerGCC::CleanWorkspace(const wxString& target)
{
    if (CheckDebuggerIsRunning())
        return -1;

    DoPrepareQueue();
    ClearLog();

    ResetBuildState();
    cbProject* bak = m_Project;
    ProjectsArray* arr = Manager::Get()->GetProjectManager()->GetProjects();
    for (size_t i = 0; i < arr->GetCount(); ++i)
    {
        m_pBuildingProject = arr->Item(i);
        Clean(target);
    }
    ResetBuildState();
    m_Project = bak;
    return 0;
}

int CompilerGCC::KillProcess()
{
    ResetBuildState();
    m_RunAfterCompile = false;
    if (!IsProcessRunning())
        return 0;
    wxKillError ret = wxKILL_OK;

    m_CommandQueue.Clear();

    for (size_t i = 0; i < m_ParallelProcessCount; ++i)
    {
        if (!m_Processes[i])
            continue;

        // Close input pipe
        m_Processes[i]->CloseOutput();

        ret = wxProcess::Kill(m_Pid[i], wxSIGTERM);
        if(ret != wxKILL_OK)
        {
            // No need to tell the user about the errors - just keep him waiting.
            Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Aborting process %d..."), i);
        }
        else switch (ret)
        {
//            case wxKILL_ACCESS_DENIED: cbMessageBox(_("Access denied")); break;
//            case wxKILL_NO_PROCESS: cbMessageBox(_("No process")); break;
//            case wxKILL_BAD_SIGNAL: cbMessageBox(_("Bad signal")); break;
//            case wxKILL_ERROR: cbMessageBox(_("Unspecified error")); break;

            case wxKILL_OK:
            default: break;//Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Process killed..."));
        }
    }
    return ret;
}

bool CompilerGCC::IsRunning() const
{
    return m_BuildJob != bjIdle || IsProcessRunning() || m_CommandQueue.GetCount();
}

ProjectBuildTarget* CompilerGCC::GetBuildTargetForFile(ProjectFile* pf)
{
    if (!pf)
        return 0;

    if (!pf->buildTargets.GetCount())
    {
        cbMessageBox(_("That file isn't assigned to any target."),
                    _("Information"), wxICON_INFORMATION);
        return 0;
    }
    else if (pf->buildTargets.GetCount() == 1)
        return m_Project->GetBuildTarget(pf->buildTargets[0]);
    // belongs to two or more build targets
    ProjectBuildTarget* bt = 0;
    // if "All" is selected, ask for build target
    if (m_HasTargetAll && m_TargetIndex == -1)
    {
        int idx = DoGUIAskForTarget();
        if (idx == -1)
            return 0;
        bt = m_Project->GetBuildTarget(idx);
    }
    else // use the currently selected build target
        bt = m_Project->GetBuildTarget(m_TargetIndex); // pick the selected target

    return bt;
}

ProjectBuildTarget* CompilerGCC::GetBuildTargetForFile(const wxString& file)
{
    ProjectFile* pf = m_Project ? m_Project->GetFileByFilename(file, true, false) : 0;
    return GetBuildTargetForFile(pf);
}

int CompilerGCC::CompileFile(const wxString& file)
{
    DoPrepareQueue();
    if (!CompilerValid())
        return -1;

    if (m_Project)
        wxSetWorkingDirectory(m_Project->GetBasePath());

    ProjectFile* pf = m_Project ? m_Project->GetFileByFilename(file, true, false) : 0;
    ProjectBuildTarget* bt = GetBuildTargetForFile(pf);
    bool useMake = UseMake(bt);

    if (!pf)
    {
        // compile single file not belonging to a project
        Manager::Get()->GetEditorManager()->Save(file);

        // switch to the default compiler
        SwitchCompiler(CompilerFactory::GetDefaultCompilerID());
//        Manager::Get()->GetMessageManager()->DebugLog("-----CompileFile [if(!pf)]-----");
        Manager::Get()->GetMacrosManager()->Reset();
        CompilerFactory::GetCompiler(m_CompilerId)->Init(0);

        if (useMake)
        {
            cbMessageBox(_("That file doesn't belong to a project.\n"
                            "If you want to compile it as stand-alone, please use the \"Invoke compiler directly\" build method\n"
                            "(Settings->Compiler->Other->Build method)"),
                        _("Information"), wxICON_INFORMATION);
        }
        else
        {
            // get compile commands for file (always linked as console-executable)
            DirectCommands dc(this, CompilerFactory::GetDefaultCompiler(), 0, m_PageIndex);
            wxArrayString compile = dc.GetCompileSingleFileCommand(file);
            AddToCommandQueue(compile);
        }
        return DoRunQueue();
    }

    if (!bt)
        return -2;
    if (useMake)
    {
        wxFileName tmp = pf->GetObjName();
        wxFileName o_file(bt->GetObjectOutput() + wxFILE_SEP_PATH + tmp.GetFullPath());
        wxString fname = UnixFilename(o_file.GetFullPath());
        MakefileGenerator mg(this, 0, _T(""), 0);
        mg.ConvertToMakefileFriendly(fname, true);

        Manager::Get()->GetMacrosManager()->Reset();

        wxString cmd = GetMakeCommandFor(mcCompileFile, bt);
        cmd.Replace(_T("$file"), fname);
        m_CommandQueue.Add(new CompilerCommand(cmd, wxEmptyString, m_Project, bt));
    }
    else
    {
        CompilerFactory::GetCompiler(m_CompilerId)->Init(m_Project);

        DirectCommands dc(this, CompilerFactory::GetCompiler(bt->GetCompilerID()), m_Project, m_PageIndex);
        wxArrayString compile = dc.CompileFile(bt, pf);
        AddToCommandQueue(compile);
    }
    return DoRunQueue();
}

// events

void CompilerGCC::OnIdle(wxIdleEvent& event)
{
    if (IsProcessRunning())
    {
        for (size_t i = 0; i < m_ParallelProcessCount; ++i)
        {
            if (m_Processes[i] != 0 && ((PipedProcess*)m_Processes[i])->HasInput())
            {
                event.RequestMore();
                break;
            }
        }
    }
    else
        event.Skip();
}

void CompilerGCC::OnTimer(wxTimerEvent& event)
{
    wxWakeUpIdle();
}

void CompilerGCC::OnRun(wxCommandEvent& event)
{
    if (Run() == 0)
        DoRunQueue();
}

void CompilerGCC::OnCompileAndRun(wxCommandEvent& event)
{
    ProjectBuildTarget* target = DoAskForTarget();
    m_RunAfterCompile = true;
    Build(target);
//    if (m_CommandQueue.GetCount()) // if we have build commands, use the flag to run
//    else // else make it a "Run" command
//        OnRun(event);
}

void CompilerGCC::OnCompile(wxCommandEvent& event)
{
    int bak = m_TargetIndex;
    if (event.GetId() == idMenuCompileTargetFromProjectManager)
    {
        // we 're called from a menu in ProjectManager
        int idx = DoGUIAskForTarget();
        if (idx == -1)
            return;
        else
            m_TargetIndex = idx;

        // let's check the selected project...
        DoSwitchProjectTemporarily();
    }
    else if (event.GetId() == idMenuCompileFromProjectManager)
    {
        // we 're called from a menu in ProjectManager
        // let's check the selected project...
        DoSwitchProjectTemporarily();
    }
    ProjectBuildTarget* target = DoAskForTarget();
    Build(target);
    m_TargetIndex = bak;
}

void CompilerGCC::OnCompileFile(wxCommandEvent& event)
{
     wxFileName file;
    if (event.GetId() == idMenuCompileFileFromProjectManager)
    {
        // we 're called from a menu in ProjectManager
        // let's check the selected project...
        FileTreeData* ftd = DoSwitchProjectTemporarily();
        ProjectFile* pf = m_Project->GetFile(ftd->GetFileIndex());
        if (!pf)
        {
//            wxLogError("File index=%d", ftd->GetFileIndex());
            return;
        }
        file = pf->file;
    }
    else
    {
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (ed)
        {
            // make sure it is saved
            if (ed->Save())
                file.Assign(ed->GetFilename());
        }
    }

    if (m_Project)
    {
        if(!m_Project->SaveAllFiles())
        {
            Manager::Get()->GetMessageManager()->Log(_("Could not save all files..."));
        }
        file.MakeRelativeTo(m_Project->GetBasePath());
    }
#ifdef ALWAYS_USE_MAKEFILE
    file.SetExt(OBJECT_EXT);
#endif
    wxString fname = file.GetFullPath();
    if (!fname.IsEmpty())
        CompileFile(UnixFilename(fname));
}

void CompilerGCC::OnRebuild(wxCommandEvent& event)
{
    CheckProject();
    AnnoyingDialog dlg(_("Rebuild project"),
                        _("Rebuilding the project will cause the deletion of all "
                        "object files and building it from scratch.\nThis action "
                        "might take a while, especially if your project contains "
                        "more than a few files.\nAnother factor is your CPU "
                        "and the available system memory.\n\n"
                        "Are you sure you want to rebuild the entire project?"),
                    wxART_QUESTION,
                    AnnoyingDialog::YES_NO,
                    wxID_YES);
    if (m_Project && dlg.ShowModal() == wxID_NO)
    {
        return;
    }

    int bak = m_TargetIndex;
    if (event.GetId() == idMenuRebuildTargetFromProjectManager)
    {
        // we 're called from a menu in ProjectManager
        int idx = DoGUIAskForTarget();
        if (idx == -1)
            return;
        else
            m_TargetIndex = idx;

        // let's check the selected project...
        DoSwitchProjectTemporarily();
    }
    else if (event.GetId() == idMenuRebuildFromProjectManager)
    {
        // we 're called from a menu in ProjectManager
        // let's check the selected project...
        DoSwitchProjectTemporarily();
    }
    ProjectBuildTarget* target = DoAskForTarget();
    Rebuild(target);
    m_TargetIndex = bak;
}

void CompilerGCC::OnCompileAll(wxCommandEvent& event)
{
    BuildWorkspace();
}

void CompilerGCC::OnRebuildAll(wxCommandEvent& event)
{
    AnnoyingDialog dlg(_("Rebuild workspace"),
                        _("Rebuilding ALL the open projects will cause the deletion of all "
                        "object files and building them from scratch.\nThis action "
                        "might take a while, especially if your projects contain "
                        "more than a few files.\nAnother factor is your CPU "
                        "and the available system memory.\n\n"
                        "Are you sure you want to rebuild ALL the projects?"),
                    wxART_QUESTION,
                    AnnoyingDialog::YES_NO,
                    wxID_YES);
    if (dlg.ShowModal() == wxID_NO)
    {
        return;
    }
    RebuildWorkspace();
}

void CompilerGCC::OnCleanAll(wxCommandEvent& event)
{
    AnnoyingDialog dlg(_("Clean project"),
                        _("Cleaning ALL the open projects will cause the deletion "
                        "of all relevant object files.\nThis means that you will "
                        "have to build ALL your projects from scratch next time you "
                        "'ll want to build them.\nThat action "
                        "might take a while, especially if your projects contain "
                        "more than a few files.\nAnother factor is your CPU "
                        "and the available system memory.\n\n"
                        "Are you sure you want to proceed to cleaning?"),
                    wxART_QUESTION,
                    AnnoyingDialog::YES_NO,
                    wxID_YES);
    if (dlg.ShowModal() == wxID_NO)
    {
        return;
    }
    CleanWorkspace();
}

void CompilerGCC::OnClean(wxCommandEvent& event)
{
    CheckProject();
    AnnoyingDialog dlg(_("Clean project"),
                        _("Cleaning the target or project will cause the deletion "
                        "of all relevant object files.\nThis means that you will "
                        "have to build your project from scratch next time you "
                        "'ll want to build it.\nThat action "
                        "might take a while, especially if your project contains "
                        "more than a few files.\nAnother factor is your CPU "
                        "and the available system memory.\n\n"
                        "Are you sure you want to proceed to cleaning?"),
                    wxART_QUESTION,
                    AnnoyingDialog::YES_NO,
                    wxID_YES);
    if (m_Project && dlg.ShowModal() == wxID_NO)
    {
        return;
    }

    int bak = m_TargetIndex;
    if (event.GetId() == idMenuCleanTargetFromProjectManager)
    {
        // we 're called from a menu in ProjectManager
        int idx = DoGUIAskForTarget();
        if (idx == -1)
            return;
        else
            m_TargetIndex = idx;

        // let's check the selected project...
        DoSwitchProjectTemporarily();
    }
    else if (event.GetId() == idMenuCleanFromProjectManager)
    {
        // we 're called from a menu in ProjectManager
        // let's check the selected project...
        DoSwitchProjectTemporarily();
    }
    ProjectBuildTarget* target = DoAskForTarget();
    Clean(target);
    m_TargetIndex = bak;
}

void CompilerGCC::OnProjectCompilerOptions(wxCommandEvent& event)
{
    wxTreeCtrl* tree = Manager::Get()->GetProjectManager()->GetTree();
    wxTreeItemId sel = tree->GetSelection();
    FileTreeData* ftd = (FileTreeData*)tree->GetItemData(sel);
    if (ftd)
    {
        // 'configure' selected target, if other than 'All'
        ProjectBuildTarget* target = 0;
        if (ftd->GetProject() == m_Project)
        {
            if (!m_HasTargetAll || m_TargetIndex != -1)
                target = m_Project->GetBuildTarget(m_TargetIndex);
        }
        Configure(ftd->GetProject(), target);
    }
    else
    {
        cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
        if (prj)
            Configure(prj);
    }
}

void CompilerGCC::OnTargetCompilerOptions(wxCommandEvent& event)
{
    int bak = m_TargetIndex;
    // we 're called from a menu in ProjectManager
    int idx = DoGUIAskForTarget();
    if (idx == -1)
        return;
    else
        m_TargetIndex = idx;
       // let's check the selected project...
       DoSwitchProjectTemporarily();

    ProjectBuildTarget* target = DoAskForTarget();
    m_TargetIndex = bak;
    Configure(m_Project, target);
}

void CompilerGCC::OnKillProcess(wxCommandEvent& event)
{
    KillProcess();
}

void CompilerGCC::OnSelectTarget(wxCommandEvent& event)
{
     if (event.GetId() == idMenuSelectTargetAll)
        m_TargetIndex = -1;
    else if (event.GetId() == idToolTarget)
#if wxCHECK_VERSION(2, 6, 2)
        m_TargetIndex = m_ToolTarget->GetCurrentSelection() - (m_HasTargetAll ? 1 : 0);
#else
        m_TargetIndex = m_ToolTarget->GetSelection() - (m_HasTargetAll ? 1 : 0);
#endif
    else
    {
        for (int i = 0; i < MAX_TARGETS; ++i)
        {
            if (event.GetId() == idMenuSelectTargetOther[i])
            {
                m_TargetIndex = i;
                break;
            }
        }
    }
    DoUpdateTargetMenu();
}

void CompilerGCC::OnNextError(wxCommandEvent& event)
{
    DoGotoNextError();
}

void CompilerGCC::OnPreviousError(wxCommandEvent& event)
{
    DoGotoPreviousError();
}

void CompilerGCC::OnClearErrors(wxCommandEvent& event)
{
    DoClearErrors();
}

void CompilerGCC::OnUpdateUI(wxUpdateUIEvent& event)
{
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    wxMenuBar* mbar = Manager::Get()->GetAppWindow()->GetMenuBar();
    bool running = IsRunning();
    if (mbar)
    {
        mbar->Enable(idMenuCompile, !running && (prj || ed));
        mbar->Enable(idMenuCompileAll, !running && prj);
//        mbar->Enable(idMenuCompileFromProjectManager, !running && prj);
//        mbar->Enable(idMenuCompileTargetFromProjectManager, !running && prj);
        mbar->Enable(idMenuCompileFile, !running && ed);
//        mbar->Enable(idMenuCompileFileFromProjectManager, !running && prj);
        mbar->Enable(idMenuRebuild, !running && (prj || ed));
        mbar->Enable(idMenuRebuildAll, !running && prj);
//        mbar->Enable(idMenuRebuildFromProjectManager, !running && prj);
//        mbar->Enable(idMenuRebuildTargetFromProjectManager, !running && prj);
        mbar->Enable(idMenuClean, !running && (prj || ed));
        mbar->Enable(idMenuCleanAll, !running && prj);
//        mbar->Enable(idMenuCleanFromProjectManager, !running && prj);
//        mbar->Enable(idMenuCleanTargetFromProjectManager, !running && prj);
        mbar->Enable(idMenuCompileAndRun, !running && (prj || ed));
        mbar->Enable(idMenuRun, !running && (prj || ed));
        mbar->Enable(idMenuKillProcess, running);
        mbar->Enable(idMenuSelectTarget, !running && prj);

        mbar->Enable(idMenuNextError, !running && (prj || ed) && m_Errors.HasNextError());
        mbar->Enable(idMenuPreviousError, !running && (prj || ed) && m_Errors.HasPreviousError());
//        mbar->Enable(idMenuClearErrors, cnt);

        mbar->Enable(idMenuExportMakefile, false);// !running && prj);

        // Project menu
        mbar->Enable(idMenuProjectCompilerOptions, !running && prj);
    }

    // enable/disable compiler toolbar buttons
    wxToolBar* tbar = m_pTbar;//Manager::Get()->GetAppWindow()->GetToolBar();
    if (tbar)
    {
        tbar->EnableTool(idMenuCompile, !running && (prj || ed));
        tbar->EnableTool(idMenuRun, !running && (prj || ed));
        tbar->EnableTool(idMenuCompileAndRun, !running && (prj || ed));
        tbar->EnableTool(idMenuRebuild, !running && (prj || ed));
        tbar->EnableTool(idMenuKillProcess, running && prj);

        m_ToolTarget = XRCCTRL(*tbar, "idToolTarget", wxComboBox);
        if (m_ToolTarget)
            m_ToolTarget->Enable(!running && prj);
    }

    // allow other UpdateUI handlers to process this event
    // *very* important! don't forget it...
    event.Skip();
}

void CompilerGCC::OnProjectActivated(CodeBlocksEvent& event)
{   //NOTE: this function is also called on PROJECT_TARGETS_MODIFIED events to keep the combobox in sync
    DoRecreateTargetMenu();
    event.Skip(); // *very* important! don't forget it...
}

void CompilerGCC::OnProjectLoaded(CodeBlocksEvent& event)
{
    DoRecreateTargetMenu();
    event.Skip(); // *very* important! don't forget it...
}

void CompilerGCC::OnGCCOutput(CodeBlocksEvent& event)
{
    wxString msg = event.GetString();
    if (!msg.IsEmpty() &&
        !msg.Matches(_T("# ??*")))  // gcc 3.4 started displaying a line like this filter
                                // when calculating dependencies. Until I check out
                                // why this happens (and if there is a switch to
                                // turn it off), I put this condition here to avoid
                                // displaying it...
    {
        AddOutputLine(msg);
    }
}

void CompilerGCC::OnGCCError(CodeBlocksEvent& event)
{
    wxString msg = event.GetString();
    AddOutputLine(msg);
}

void CompilerGCC::AddOutputLine(const wxString& output, bool forceErrorColour)
{
    Manager::Get()->GetMessageManager()->LogToStdOut(output + _T('\n'));

    // if max_errors reached, display a one-time message and do not log anymore
    size_t maxErrors = Manager::Get()->GetConfigManager(_T("compiler"))->ReadInt(_T("/max_reported_errors"), 50);
    if (maxErrors > 0 && m_Errors.GetCount(cltError) == maxErrors)
    {
        if (!m_NotifiedMaxErrors)
        {
            m_NotifiedMaxErrors = true;

            // if we reached the max errors count, notify about it
            LogWarningOrError(cltNormal, 0, wxEmptyString, wxEmptyString, _("More errors follow but not being shown."));
            LogWarningOrError(cltNormal, 0, wxEmptyString, wxEmptyString, _("Edit the max errors limit in compiler options..."));
        }
        return;
    }

    Compiler* compiler = CompilerFactory::GetCompiler(m_CompilerId);
    CompilerLineType clt = compiler->CheckForWarningsAndErrors(output);

    // log to build log
    switch (clt)
    {
        case cltWarning:
            m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(COLOUR_NAVY));
            break;

        case cltError:
            m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxRED));
            break;

        default:
            if (forceErrorColour)
                m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(COLOUR_MAROON));
            else
                m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)));
            break;
    }

    // log to build messages if warning/error
    if (clt != cltNormal)
    {
        // display current project/target "header" in build messages, if different since last warning/error
        static ProjectBuildTarget* last_bt = 0;
        if (last_bt != m_pLastBuildingTarget)
        {
            last_bt = m_pLastBuildingTarget;
            if (last_bt)
            {
                wxString msg;
                msg.Printf(_T("=== %s, %s ==="),
                            last_bt->GetParentProject()->GetTitle().c_str(),
                            last_bt->GetTitle().c_str());
                LogWarningOrError(cltNormal, 0, wxEmptyString, wxEmptyString, msg);
            }
        }
        // actually log message
        LogWarningOrError(clt, m_pBuildingProject, compiler->GetLastErrorFilename(), compiler->GetLastErrorLine(), compiler->GetLastError());
    }

    if (!output.IsEmpty())
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, output.c_str());
}

void CompilerGCC::LogWarningOrError(CompilerLineType lt, cbProject* prj, const wxString& filename, const wxString& line, const wxString& msg)
{
    wxArrayString errors;

    // add build message
    errors.Add(filename);
    errors.Add(line);
    errors.Add(msg);
    m_pListLog->AddLog(errors);
    m_pListLog->GetListControl()->SetColumnWidth(2, wxLIST_AUTOSIZE);

    // colourize the list output
    wxColour c;
    switch (lt)
    {
        case cltNormal: c = *wxBLACK; break;
        case cltWarning: c = COLOUR_NAVY; break;
        case cltError: c = *wxRED; break;
    }
    m_pListLog->GetListControl()->SetItemTextColour(m_pListLog->GetListControl()->GetItemCount() - 1, c);

    // add to error keeping struct
    m_Errors.AddError(lt, prj, filename, line.IsEmpty() ? 0 : atoi(line.mb_str()), msg);
}

void CompilerGCC::OnGCCTerminated(CodeBlocksEvent& event)
{
    OnJobEnd(event.GetId() - idGCCProcess1, event.GetInt());
}

void CompilerGCC::OnJobEnd(size_t procIndex, int exitCode)
{
//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("JobDone: index=%u, exitCode=%d"), procIndex, exitCode);
    m_timerIdleWakeUp.Stop();
    m_Pid[procIndex] = 0;
    m_Processes[procIndex] = 0;
    m_LastExitCode = exitCode;

    if (m_CommandQueue.GetCount() != 0 && exitCode == 0)
    {
        // continue running commands while last exit code was 0.
        DoRunQueue();
    }
    else
    {
        if (exitCode == 0)
        {
            if (IsProcessRunning())
            {
                DoRunQueue();
                return;
            }

            while (1)
            {
                BuildStateManagement();
                if (m_CommandQueue.GetCount())
                {
                    DoRunQueue();
                    return;
                }
                if (m_BuildState == bsNone && m_NextBuildState == bsNone)
                    break;
            }
        }
        m_CommandQueue.Clear();
        ResetBuildState();

        long int elapsed = wxGetElapsedTime() / 1000;
        int mins = elapsed / 60;
        int secs = (elapsed % 60);
        m_Log->GetTextControl()->SetDefaultStyle(exitCode == 0 ? wxTextAttr(*wxBLUE) : wxTextAttr(*wxRED));
        wxString msg = wxString::Format(_("Process terminated with status %d (%d minutes, %d seconds)"), exitCode, mins, secs);
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, msg);
        Manager::Get()->GetMessageManager()->LogToStdOut(_T('\n') + msg + _T('\n'));
        if (!m_CommandQueue.LastCommandWasRun())
        {
            m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(COLOUR_NAVY));
            wxString msg = wxString::Format(_("%d errors, %d warnings"), m_Errors.GetCount(cltError), m_Errors.GetCount(cltWarning));
            Manager::Get()->GetMessageManager()->Log(m_PageIndex, msg);
            Manager::Get()->GetMessageManager()->LogToStdOut(msg + _T('\n'));
            LogWarningOrError(cltNormal, 0, wxEmptyString, wxEmptyString, wxString::Format(_("=== Build finished: %s ==="), msg.c_str()));
        }
        else
        {
            // last command was "Run"
            // force exit code to zero (0) or else debugger will think build failed if last run returned non-zero...
// TODO (mandrav##): Maybe create and use GetLastRunExitCode()? Is it needed?
            m_LastExitCode = 0;
        }
        m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T(" ")); // blank line

        NotifyJobDone();

        if (m_Errors.GetCount(cltError))
        {
            if (Manager::Get()->GetConfigManager(_T("message_manager"))->ReadBool(_T("/auto_show_build_errors"), true))
                Manager::Get()->GetMessageManager()->Open();
            Manager::Get()->GetMessageManager()->SwitchTo(m_ListPageIndex);
            m_pListLog->FocusError(m_Errors.GetFirstError());
        }
        else
        {
            if (m_RunAfterCompile)
            {
                m_RunAfterCompile = false;
                if (Run() == 0)
                    DoRunQueue();
            }
            else
            {
                // don't close the message manager (if auto-hiding), if warnings are required to keep it open
                if (m_Errors.GetCount(cltWarning) &&
                    (!Manager::Get()->GetMessageManager()->IsAutoHiding() ||
                    Manager::Get()->GetConfigManager(_T("message_manager"))->ReadBool(_T("/auto_show_build_warnings"), true)))
                {
                    Manager::Get()->GetMessageManager()->Open();
                    Manager::Get()->GetMessageManager()->SwitchTo(m_ListPageIndex);
                }
                else // if message manager is auto-hiding, unlock it (i.e. close it)
                    Manager::Get()->GetMessageManager()->Close(true);
            }
        }

        m_RunAfterCompile = false;

        // no matter what happened with the build, return the focus to the active editor
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(Manager::Get()->GetEditorManager()->GetActiveEditor());
        if (ed)
            ed->GetControl()->SetFocus();
    }
}

void CompilerGCC::NotifyJobDone(bool showNothingToBeDone)
{
    m_BuildJob = bjIdle;
    if (showNothingToBeDone)
    {
        m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLUE, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Nothing to be done."));
        Manager::Get()->GetMessageManager()->LogToStdOut(_("Nothing to be done.\n"));
        m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
    }

    if (!IsProcessRunning())
    {
        CodeBlocksEvent evt(cbEVT_COMPILER_FINISHED, 0, 0, 0, this);
        evt.SetInt(m_LastExitCode);
        Manager::Get()->GetPluginManager()->NotifyPlugins(evt);
    }
}
