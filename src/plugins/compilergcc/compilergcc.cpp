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
* $Id$
* $Date$
*/

#include "compilergcc.h"
#include <manager.h>
#include <sdk_events.h>
#include <pipedprocess.h>
#include <configmanager.h>
#include <messagemanager.h>
#include <macrosmanager.h>
#include <projectmanager.h>
#include <editormanager.h>
#include <customvars.h>
#include "makefilegenerator.h"
#include "compileroptionsdlg.h"
#include "compilerMINGW.h"
#include "compilerMSVC.h"
#include "compilerBCC.h"
#include "compilerDMC.h"
#include "compilerOW.h"
#include "compilerSDCC.h"
#include "directcommands.h"
#include <wx/xrc/xmlres.h>

#define COLOUR_MAROON wxColour(0xa0, 0x00, 0x00)
#define COLOUR_NAVY   wxColour(0x00, 0x00, 0xa0)

cbPlugin* GetPlugin()
{
    return new CompilerGCC;
}

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
int idMenuDistClean = XRCID("idCompilerMenuDistClean");
int idMenuCleanTarget = XRCID("idCompilerMenuCleanTarget");
int idMenuDistCleanTarget = XRCID("idCompilerMenuDistCleanTarget");
int idMenuCleanFromProjectManager = XRCID("idCompilerMenuCleanFromProjectManager");
int idMenuDistCleanFromProjectManager = XRCID("idCompilerMenuDistCleanFromProjectManager");
int idMenuCleanTargetFromProjectManager = XRCID("idCompilerMenuCleanTargetFromProjectManager");
int idMenuDistCleanTargetFromProjectManager = XRCID("idCompilerMenuDistCleanTargetFromProjectManager");
int idMenuCompileAndRun = XRCID("idCompilerMenuCompileAndRun");
int idMenuRun = XRCID("idCompilerMenuRun");
int idMenuKillProcess = XRCID("idCompilerMenuKillProcess");
int idMenuSelectTarget = XRCID("idCompilerMenuSelectTarget");
int idMenuSelectTargetAll = XRCID("idCompilerMenuSelectTargetAll");
int idMenuSelectTargetOther[MAX_TARGETS]; // initialized in ctor
int idMenuNextError = XRCID("idCompilerMenuNextError");
int idMenuPreviousError = XRCID("idCompilerMenuPreviousError");
int idMenuClearErrors = XRCID("idCompilerMenuClearErrors");
int idMenuCreateDist = XRCID("idCompilerMenuCreateDist");
int idMenuExportMakefile = XRCID("idCompilerMenuExportMakefile");
int idMenuSettings = XRCID("idCompilerMenuSettings");

int idToolTarget = XRCID("idToolTarget");
int idToolTargetLabel = XRCID("idToolTargetLabel");

int idGCCProcess = wxNewId();

BEGIN_EVENT_TABLE(CompilerGCC, cbCompilerPlugin)
    EVT_UPDATE_UI_RANGE(idMenuCompile, idToolTargetLabel, CompilerGCC::OnUpdateUI)

    // these are loaded from the XRC
    EVT_UPDATE_UI(XRCID("idCompilerMenuCompile"), CompilerGCC::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idCompilerMenuRebuild"), CompilerGCC::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idCompilerMenuCompileAndRun"), CompilerGCC::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("idCompilerMenuRun"), CompilerGCC::OnUpdateUI)

    EVT_IDLE(										CompilerGCC::OnIdle)
	EVT_TIMER(idTimerPollCompiler,                  CompilerGCC::OnTimer)

    EVT_MENU(idMenuRun,                             CompilerGCC::OnRun)
    EVT_MENU(idMenuCompileAndRun,                   CompilerGCC::OnCompileAndRun)
    EVT_MENU(idMenuCompile,                         CompilerGCC::OnCompile)
    EVT_MENU(idMenuCompileFromProjectManager,       CompilerGCC::OnCompile)
    EVT_MENU(idMenuCompileTargetFromProjectManager, CompilerGCC::OnCompile)
    EVT_MENU(idMenuCompileFile,                     CompilerGCC::OnCompileFile)
    EVT_MENU(idMenuCompileFileFromProjectManager,   CompilerGCC::OnCompileFile)
    EVT_MENU(idMenuRebuild,                         CompilerGCC::OnRebuild)
    EVT_MENU(idMenuRebuildFromProjectManager,       CompilerGCC::OnRebuild)
    EVT_MENU(idMenuRebuildTargetFromProjectManager, CompilerGCC::OnRebuild)
    EVT_MENU(idMenuCompileAll,                      CompilerGCC::OnCompileAll)
    EVT_MENU(idMenuRebuildAll,                      CompilerGCC::OnRebuildAll)
	EVT_MENU(idMenuProjectCompilerOptions,			CompilerGCC::OnProjectCompilerOptions)
	EVT_MENU(idMenuTargetCompilerOptions,			CompilerGCC::OnTargetCompilerOptions)
    EVT_MENU(idMenuClean,                           CompilerGCC::OnClean)
    EVT_MENU(idMenuDistClean,                       CompilerGCC::OnDistClean)
    EVT_MENU(idMenuCleanFromProjectManager,         CompilerGCC::OnClean)
    EVT_MENU(idMenuDistCleanFromProjectManager,     CompilerGCC::OnDistClean)
    EVT_MENU(idMenuCleanTargetFromProjectManager,   CompilerGCC::OnClean)
    EVT_MENU(idMenuDistCleanTargetFromProjectManager, CompilerGCC::OnDistClean)
    EVT_MENU(idMenuKillProcess,                     CompilerGCC::OnKillProcess)
	EVT_MENU(idMenuSelectTargetAll,					CompilerGCC::OnSelectTarget)
	EVT_MENU(idMenuNextError,						CompilerGCC::OnNextError)
	EVT_MENU(idMenuPreviousError,					CompilerGCC::OnPreviousError)
	EVT_MENU(idMenuClearErrors,						CompilerGCC::OnClearErrors)
    EVT_MENU(idMenuCreateDist,                      CompilerGCC::OnCreateDist)
    EVT_MENU(idMenuExportMakefile,                  CompilerGCC::OnExportMakefile)
    EVT_MENU(idMenuSettings,                        CompilerGCC::OnConfig)

	EVT_COMBOBOX(idToolTarget,						CompilerGCC::OnSelectTarget)

	EVT_PROJECT_ACTIVATE(CompilerGCC::OnProjectActivated)
	//EVT_PROJECT_POPUP_MENU(CompilerGCC::OnProjectPopupMenu)

	EVT_PIPEDPROCESS_STDOUT(idGCCProcess, CompilerGCC::OnGCCOutput)
	EVT_PIPEDPROCESS_STDERR(idGCCProcess, CompilerGCC::OnGCCError)
	EVT_PIPEDPROCESS_TERMINATED(idGCCProcess, CompilerGCC::OnGCCTerminated)
END_EVENT_TABLE()

CompilerGCC::CompilerGCC()
    : m_CompilerIdx(-1),
    m_PageIndex(-1),
	m_ListPageIndex(-1),
    m_Menu(0L),
    m_TargetMenu(0L),
	m_pToolbar(0L),
	m_TargetIndex(-1),
	m_ErrorsMenu(0L),
    m_Project(0L),
    m_Process(0L),
    m_pTbar(0L),
    m_Pid(0),
    m_Log(0L),
	m_pListLog(0L),
	m_ToolTarget(0L),
	m_ToolTargetLabel(0L),
	m_IsRun(false),
	m_RunAfterCompile(false),
	m_DoAllProjects(mpjNone),
	m_BackupActiveProject(0L),
	m_ProjectIndex(0),
	m_LastExitCode(0),
	m_HasTargetAll(false),
	m_QueueIndex(0),
	m_DeleteTempMakefile(true)
{
    Manager::Get()->Loadxrc(_T("/compiler_gcc.zip#zip:*.xrc"));

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
	m_PluginInfo.hasConfigure = false;

    m_timerIdleWakeUp.SetOwner(this, idTimerPollCompiler);

	for (int i = 0; i < MAX_TARGETS; ++i)
		idMenuSelectTargetOther[i] = wxNewId();
#ifndef __WXMSW__
	m_ConsoleShell = ConfigManager::Get()->Read(_T("/compiler_gcc/console_shell"), DEFAULT_CONSOLE_SHELL);
#endif

	// register built-in compilers
	CompilerFactory::RegisterCompiler(new CompilerMINGW);
#ifdef __WXMSW__
	CompilerFactory::RegisterCompiler(new CompilerMSVC);
	CompilerFactory::RegisterCompiler(new CompilerBCC);
	CompilerFactory::RegisterCompiler(new CompilerDMC);
	CompilerFactory::RegisterCompiler(new CompilerOW);
#endif
	CompilerFactory::RegisterCompiler(new CompilerSDCC);

	// register (if any) user-copies of built-in compilers
	CompilerFactory::RegisterUserCompilers();

	ConfigManager::AddConfiguration(m_PluginInfo.title, _T("/compiler_gcc"));
}

CompilerGCC::~CompilerGCC()
{
    DoDeleteTempMakefile();
	if (m_ToolTarget)
		delete m_ToolTarget;
	CompilerFactory::UnregisterCompilers();
}

void CompilerGCC::OnAttach()
{
    wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
    MessageManager* msgMan = Manager::Get()->GetMessageManager();

	// create compiler's log
    m_Log = new SimpleTextLog(msgMan, m_PluginInfo.title);
    m_Log->GetTextControl()->SetFont(font);
    m_PageIndex = msgMan->AddLog(m_Log);

    // set log image
	wxBitmap bmp;
	wxString prefix = ConfigManager::Get()->Read(_T("data_path")) + _T("/images/");
    bmp.LoadFile(prefix + _T("misc_16x16.png"), wxBITMAP_TYPE_PNG);
    Manager::Get()->GetMessageManager()->SetLogImage(m_Log, bmp);

	// create warnings/errors log
	wxArrayString titles;
	int widths[3] = {128, 48, 640};
	titles.Add(_("File"));
	titles.Add(_("Line"));
	titles.Add(_("Message"));

	m_pListLog = new CompilerMessages(msgMan, m_PluginInfo.title + _(" messages"), 3, widths, titles);
	m_pListLog->SetCompilerErrors(&m_Errors);
    m_pListLog->GetListControl()->SetFont(font);
	m_ListPageIndex = msgMan->AddLog(m_pListLog);

    // set log image
    bmp.LoadFile(prefix + _T("flag_16x16.png"), wxBITMAP_TYPE_PNG);
    Manager::Get()->GetMessageManager()->SetLogImage(m_pListLog, bmp);

    // set default compiler for new projects
    CompilerFactory::SetDefaultCompilerIndex(ConfigManager::Get()->Read(_T("/compiler_gcc/default_compiler"), (long int)0));
	LoadOptions();
	SetupEnvironment();
}

void CompilerGCC::OnRelease(bool appShutDown)
{
    DoDeleteTempMakefile();
	SaveOptions();
    ConfigManager::Get()->Write(_T("/compiler_gcc/default_compiler"), CompilerFactory::GetDefaultCompilerIndex());
	if (Manager::Get()->GetMessageManager())
	{
        Manager::Get()->GetMessageManager()->DeletePage(m_ListPageIndex);
        Manager::Get()->GetMessageManager()->DeletePage(m_PageIndex);
    }

	if (appShutDown)
		return; // no need to continue if app is shutting down

	DoClearTargetMenu();

//	if (m_Menu)
//	{
//		wxMenuBar* mBar = Manager::Get()->GetAppWindow()->GetMenuBar();
//		int pos = mBar->FindMenu(_("&Compile"));
//		if (pos != wxNOT_FOUND)
//			mBar->Remove(pos);
//		delete m_Menu;
//		m_Menu = 0L;
//	}

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
    CompilerOptionsDlg dlg(Manager::Get()->GetAppWindow(), this, project, target);
    if(dlg.ShowModal()==wxID_OK)
    {
      m_ConsoleShell = ConfigManager::Get()->Read(_T("/compiler_gcc/console_shell"), DEFAULT_CONSOLE_SHELL);
      SaveOptions();
      SetupEnvironment();
    }
    return 0;
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
    menuBar->Insert(finalPos, m_Menu, _("&Compile"));

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
    // Add entry in settings menu (outside "plugins")
    int settingsMenuPos = menuBar->FindMenu(_("&Settings"));
    if (settingsMenuPos != wxNOT_FOUND)
    {
        wxMenu* settingsmenu = menuBar->GetMenu(settingsMenuPos);
        settingsmenu->Insert(2,idMenuSettings,_("&Compiler"),_("Global Compiler Options"));
    }
}

void CompilerGCC::BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg)
{
	if (!m_IsAttached)
		return;
    // we 're only interested in project manager's menus
    if (type != mtProjectManager || !menu || m_Process)
        return;

	if (!CheckProject())
		return;

    FileType ft = FileTypeOf(arg);

    if (arg.IsEmpty())
    {
        // popup menu in empty space in ProjectManager
        menu->Append(idMenuCompileAll, _("Compile all projects"));
        menu->Append(idMenuRebuildAll, _("Rebuild all projects"));
    }
    else
    {
        // see if the arg is a project name
        bool found = false;
        ProjectsArray* array = Manager::Get()->GetProjectManager()->GetProjects();
        if (array)
        {
            for (size_t i = 0; i < array->GetCount(); ++i)
            {
                cbProject* cur = array->Item(i);
                if (cur && cur->GetTitle() == arg)
                {
                    found = true;
                    break;
                }
            }
        }

        if (found)
        {
            // popup menu on a project
            menu->AppendSeparator();
            menu->Append(idMenuCompileFromProjectManager, _("&Compile\tCtrl-F9"));
            menu->Append(idMenuRebuildFromProjectManager, _("Re&build\tCtrl-F11"));
            menu->Append(idMenuCleanFromProjectManager, _("C&lean"));
            menu->Append(idMenuDistCleanFromProjectManager, _("Di&st clean"));
            wxMenu* subMenu = new wxMenu();
            subMenu->Append(idMenuCompileTargetFromProjectManager, _("Compile"));
            subMenu->Append(idMenuRebuildTargetFromProjectManager, _("Rebuild"));
            subMenu->Append(idMenuCleanTargetFromProjectManager, _("Clean"));
            subMenu->Append(idMenuDistCleanTargetFromProjectManager, _("Dist clean"));
            subMenu->AppendSeparator();
            subMenu->Append(idMenuTargetCompilerOptions, _("Build options"));
            menu->Append(idMenuTargetCompilerOptionsSub, _("Specific build target..."), subMenu);
            menu->AppendSeparator();
            menu->Append(idMenuProjectCompilerOptions, _("Build options"));
        }
        else if (ft == ftSource || ft == ftHeader)
        {
            // popup menu on a compilable file
            menu->AppendSeparator();
            menu->Append(idMenuCompileFileFromProjectManager, _("Compile file"));
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
    DoRecreateTargetMenu(); // make sure the tool target combo is up-to-date
    return true;
}

void CompilerGCC::SetupEnvironment()
{
    if (!CompilerFactory::CompilerIndexOK(m_CompilerIdx))
        return;

    m_EnvironmentMsg.Clear();

	wxString path;
//	Manager::Get()->GetMessageManager()->DebugLog(_("Setting up compiler environment..."));
	Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Setting up compiler environment..."));

    // reset PATH to original value
    if (!m_OriginalPath.IsEmpty())
        wxSetEnv(_T("PATH"), m_OriginalPath);

    // look for valid compiler in path
	if (wxGetEnv(_T("PATH"), &path))
	{
        if (m_OriginalPath.IsEmpty())
            m_OriginalPath = path;

        wxArrayInt compilers;
        if(m_Project)
        {
            for (int x = 0; x < m_Project->GetBuildTargetsCount(); ++x)
            {
                ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
                int idx = target->GetCompilerIndex();

                // one time per compiler
                if (compilers.Index(idx) != wxNOT_FOUND || !CompilerFactory::CompilerIndexOK(idx))
                    continue;
                compilers.Add(idx);
                SetEnvironmentForCompilerIndex(idx, path);
            }
        }
        else
            SetEnvironmentForCompilerIndex(CompilerFactory::GetDefaultCompilerIndex(), path);
	}
	else
		m_EnvironmentMsg = _("Could not read the PATH environment variable!\n"
					"This can't be good. There may be problems running "
					"system commands and the application might not behave "
					"the way it was designed to...");
//    wxGetEnv("PATH", &path);
//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, "PATH set to: %s", path.c_str());
}

void CompilerGCC::SetEnvironmentForCompilerIndex(int idx, wxString& envPath)
{
    if (!CompilerFactory::CompilerIndexOK(idx))
        return;

    Compiler* compiler = CompilerFactory::Compilers[idx];
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
        Manager::Get()->GetMessageManager()->DebugLog(_("Can't find compiler executable in your search path (%s)..."), compiler->GetName().c_str());
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

int CompilerGCC::GetCurrentCompilerIndex()
{
    return CompilerFactory::CompilerIndexOK(m_CompilerIdx) ? m_CompilerIdx : 0;
}

void CompilerGCC::SwitchCompiler(int compilerIdx)
{
    if (!CompilerFactory::CompilerIndexOK(compilerIdx))
        return;
    m_CompilerIdx = compilerIdx;
    SetupEnvironment();
}

void CompilerGCC::AskForActiveProject()
{
    m_Project = Manager::Get()->GetProjectManager()->GetActiveProject();
}

bool CompilerGCC::CheckProject()
{
    AskForActiveProject();
    if (m_Project && m_Project->GetCompilerIndex() != m_CompilerIdx)
        SwitchCompiler(m_Project->GetCompilerIndex());
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
    FileTreeData* newFtd = new FileTreeData(ftd->GetProject(), ftd->GetFileIndex());
    Manager::Get()->GetProjectManager()->SetProject(ftd->GetProject(), false);
    AskForActiveProject();

    return newFtd;
}

int CompilerGCC::DoRunQueue()
{
    wxLogNull ln;

	// leave if already running
	if (m_Process)
		return -2;

    MessageManager* msgMan = Manager::Get()->GetMessageManager();
    msgMan->SwitchTo(m_PageIndex);

	// leave if no active project
    AskForActiveProject();

    // make sure all project files are saved
    if (m_Project && !m_Project->SaveAllFiles())
        msgMan->Log(_("Could not save all files..."));

    if (m_Queue.GetCount() == 0)
	{
        m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLUE, *wxWHITE));
        msgMan->Log(m_PageIndex, _("Nothing to be done."));
        m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLACK, *wxWHITE));
        m_LastExitCode = 0;
        OnJobEnd();
        return 0;
	}

	// leave if no commands in queue
    if (m_QueueIndex >= m_Queue.GetCount())
	{
        msgMan->DebugLog(_("Queue has been emptied! (count=%d, index=%d)"), m_Queue.GetCount(), m_QueueIndex);
        return -3;
	}

	m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLACK, *wxWHITE));
    wxString dir;// = m_Project->GetBasePath();
    wxString cmd;

    // loop added for compiler log when not working with Makefiles
    wxString mySimpleLog = wxString(COMPILER_SIMPLE_LOG);
    wxString myTargetChange = wxString(COMPILER_TARGET_CHANGE);
    while (true)
    {
        cmd = m_Queue[m_QueueIndex];
//	    msgMan->Log(m_PageIndex, "cmd='%s' in '%s'", cmd.c_str(), m_CdRun.c_str());

        // logging
        if (cmd.StartsWith(mySimpleLog))
        {
            cmd.Remove(0, mySimpleLog.Length());
            msgMan->Log(m_PageIndex, cmd);
        }
        // compiler change
        else if (cmd.StartsWith(myTargetChange))
        {
            cmd.Remove(0, myTargetChange.Length());
            // using other compiler now: find it and set it
            ProjectBuildTarget* bt = m_Project->GetBuildTarget(cmd);
            if (bt)
            {
                SwitchCompiler(bt->GetCompilerIndex());
                // re-apply the env vars for this target
                if (CompilerFactory::CompilerIndexOK(m_CompilerIdx))
                    CompilerFactory::Compilers[m_CompilerIdx]->GetCustomVars().ApplyVarsToEnvironment();
                m_Project->GetCustomVars().ApplyVarsToEnvironment();
                bt->GetCustomVars().ApplyVarsToEnvironment();
            }
            else
                msgMan->Log(m_PageIndex, _("Can't locate target '%s'!"), cmd.c_str());
        }
        else
        {
        	// compile command; apply custom vars
        	Manager::Get()->GetMacrosManager()->ReplaceEnvVars(cmd);
            break;
        }

        ++m_QueueIndex;
        if (m_QueueIndex >= m_Queue.GetCount())
        {
            msgMan->Log(m_PageIndex, _("Nothing to be done."));
            m_LastExitCode = 0;
            OnJobEnd();
            return 0;
        }
    }

    // if message manager is auto-hiding, this will lock it open
    Manager::Get()->GetMessageManager()->LockOpen();

	bool pipe = true;
	int flags = wxEXEC_ASYNC;
	if (m_RunAfterCompile && m_IsRun && m_QueueIndex == m_Queue.GetCount() - 1)
	{
		pipe = false; // no need to pipe output channels...
		flags |= wxEXEC_NOHIDE;
		m_IsRun = false;
		dir = m_CdRun;
#ifndef __WXMSW__
		wxSetEnv(_T("LD_LIBRARY_PATH"), _T("."));
#endif
	}

    m_Process = new PipedProcess((void**)&m_Process, this, idGCCProcess, pipe, dir);
    m_Pid = wxExecute(cmd, flags, m_Process);
    if ( !m_Pid )
    {
        m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxRED, *wxWHITE));
        msgMan->Log(m_PageIndex, _("Execution of '%s' in '%s' failed."), m_Queue[m_QueueIndex].c_str(), wxGetCwd().c_str());
		m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLACK, *wxWHITE));
        delete m_Process;
        m_Process = NULL;
        m_Queue.Clear();
    }
    else
        m_timerIdleWakeUp.Start(100);

    return 0;
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
//		wxMenuItemList& items = m_TargetMenu->GetMenuItems();
//		bool olddelete=items.GetDeleteContents();
//		items.DeleteContents(true);
//		items.Clear();
//		items.DeleteContents(olddelete);
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
        m_TargetMenu->AppendCheckItem(idMenuSelectTargetAll, _("All"), _("Compile target 'all' in current project"));
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
		caption.Printf(_("Compile target '%s' in current project"), target->GetTitle().c_str());
		m_TargetMenu->AppendCheckItem(idMenuSelectTargetOther[x], target->GetTitle(), caption);
		if (m_ToolTarget)
			m_ToolTarget->Append(target->GetTitle());
	}
	Connect( idMenuSelectTargetOther[0],  idMenuSelectTargetOther[MAX_TARGETS - 1],
			wxEVT_COMMAND_MENU_SELECTED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
			&CompilerGCC::OnSelectTarget );
	DoUpdateTargetMenu();

    SwitchCompiler(m_Project->GetCompilerIndex());
}

void CompilerGCC::DoUpdateTargetMenu()
{
	if (!m_TargetMenu)
		return;

    if (!m_HasTargetAll && m_TargetIndex == -1)
        m_TargetIndex = 0;

    if (m_Project)
        m_Project->SetActiveBuildTarget(m_TargetIndex);

	m_TargetMenu->Check(idMenuSelectTargetAll, m_TargetIndex == -1);
	for (int i = 0; i < MAX_TARGETS; ++i)
	{
		m_TargetMenu->Check(idMenuSelectTargetOther[i], i == m_TargetIndex);
	}
	if (m_ToolTarget)
		m_ToolTarget->SetSelection(m_TargetIndex + (m_HasTargetAll ? 1 : 0));
}

bool CompilerGCC::DoPrepareMultiProjectCommand(MultiProjectJob job)
{
    ProjectManager* prjMan = Manager::Get()->GetProjectManager();
    ProjectsArray* projects = prjMan->GetProjects();

    if (projects->GetCount() <= 1)
        return false;

    m_Queue.Clear();
	AskForActiveProject();
	m_BackupActiveProject = m_Project;
	m_ProjectIndex = 0;
	m_DoAllProjects = job;
	prjMan->SetProject(projects->Item(0), false);
	AskForActiveProject();

	return true;
}

void CompilerGCC::DoPrepareQueue()
{
	if (m_LastTempMakefile.IsEmpty() || m_Queue.GetCount() == 0)
	{
		m_QueueIndex = 0;
		if (m_DoAllProjects == mpjNone)
		{
            ClearLog();
            DoClearErrors();
        }
		DoCreateMakefile();
		wxStartTimer();
	}
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
    int idx = m_CompilerIdx;
    if (target)
        idx = target->GetCompilerIndex();
    else if (m_Project)
        idx = m_Project->GetCompilerIndex();
    if (CompilerFactory::CompilerIndexOK(idx))
        return CompilerFactory::Compilers[idx]->GetSwitches().buildMethod == cbmUseMake;
    return false;
}

bool CompilerGCC::CompilerValid(ProjectBuildTarget* target)
{
	int idx = target ? target->GetCompilerIndex() : (m_Project ? m_Project->GetCompilerIndex() : CompilerFactory::GetDefaultCompilerIndex());
	bool ret = CompilerFactory::CompilerIndexOK(idx);
	if (!ret)
	{
		wxString msg;
		msg.Printf(_("This %s is configured to use an invalid compiler.\nThe operation failed..."), target ? _("target") : _("project"));
		wxMessageBox(msg, _("Error"), wxICON_ERROR);
	}
	return ret;
}

bool CompilerGCC::DoCreateMakefile(bool temporary, const wxString& makefile)
{
    DoDeleteTempMakefile();

    // display error about incorrect compile environment
	if (!m_EnvironmentMsg.IsEmpty())
	{
		wxMessageBox(m_EnvironmentMsg, _("Error"), wxICON_ERROR);
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

void CompilerGCC::PrintBanner()
{
	if (!CompilerValid())
		return;
    if (!m_Project)
        return;
    Manager::Get()->GetMessageManager()->SwitchTo(m_PageIndex);
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Project   : %s"), m_Project->GetTitle().c_str());
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Compiler  : %s (%s)"), CompilerFactory::Compilers[m_Project->GetCompilerIndex()]->GetName().c_str(),
                                                                                    CompilerFactory::Compilers[m_Project->GetCompilerIndex()]->GetSwitches().buildMethod == cbmUseMake ? _("using GNU \"make\"") : _("called directly"));
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Directory : %s"), m_Project->GetBasePath().c_str());
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("--------------------------------------------------------------------------------"));
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
}

int CompilerGCC::Run(ProjectBuildTarget* target)
{
    if (!CheckProject())
        return -1;
	DoPrepareQueue();
	if (!CompilerValid(target))
		return -1;

    Manager::Get()->GetMessageManager()->Open();

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

    wxString out = UnixFilename(target->GetOutputFilename());
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out);

    wxString cmd;
    wxFileName f(out);
    f.MakeAbsolute(m_Project->GetBasePath());
//    m_CdRun = f.GetPath(wxPATH_GET_VOLUME);
    m_CdRun = target->GetWorkingDir();

    // for console projects, use helper app to wait for a key after
    // execution ends...
	if (target->GetTargetType() == ttConsoleOnly)
	{
#ifndef __WXMSW__
        // for non-win platforms, use m_ConsoleShell to run the console app
        wxString shell = m_ConsoleShell;
        shell.Replace(_T("$TITLE"), _T("'") + m_Project->GetTitle() + _T("'"));
        cmd << shell << _T(" ");
#endif
        // should console runner be used?
        if (target->GetUseConsoleRunner())
        {
            wxString baseDir = ConfigManager::Get()->Read(_T("/app_path"));
            if (wxFileExists(baseDir + _T("/console_runner.exe")))
                cmd << baseDir << _T("/console_runner.exe ");
        }
    }

	if (target->GetTargetType() == ttDynamicLib ||
		target->GetTargetType() == ttStaticLib)
	{
		// check for hostapp
		if (target->GetHostApplication().IsEmpty())
		{
			wxMessageBox(_("You must select a host application to \"run\" a library..."));
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
    }
    else
    {
        // commands-only target?
        wxMessageBox(_("You can't \"run\" a commands-only target..."));
        return -1;
    }

    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Checking for existence: %s"), out.c_str());
    if (!wxFileExists(out))
    {
    	int ret = wxMessageBox(_("It seems that this project has not been built yet.\n"
                                "Do you want to build it now?"),
                                _("Information"),
                                wxYES | wxNO | wxCANCEL | wxICON_QUESTION);
        switch (ret)
        {
        	case wxYES:
        	{
        		Compile(target);
        		return -1;
        	}
            case wxNO:
                break;
            default:
                return -1;
        }
    }

    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(m_CdRun);
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Executing: %s (in %s)"), cmd.c_str(), m_CdRun.c_str());
	m_Queue.Add(cmd);

	m_IsRun = true;

	return 0;
}

int CompilerGCC::Clean(ProjectBuildTarget* target)
{
	DoPrepareQueue();
	if (!CompilerValid(target))
		return -1;

    if (CompilerFactory::CompilerIndexOK(m_CompilerIdx))
        CompilerFactory::Compilers[m_CompilerIdx]->GetCustomVars().ApplyVarsToEnvironment();
    m_Project->GetCustomVars().ApplyVarsToEnvironment();

    Manager::Get()->GetMessageManager()->Open();

    wxSetWorkingDirectory(m_Project->GetBasePath());
    if (UseMake(target))
    {
        wxString cmd;
        wxString make = CompilerFactory::Compilers[m_CompilerIdx]->GetPrograms().MAKE;
        if (target)
            cmd << make << _T(" -f ") << m_LastTempMakefile << _T(" clean_") << target->GetTitle();
        else
            cmd << make << _T(" -f ") << m_LastTempMakefile << _T(" clean");
        m_Queue.Add(cmd);
        return DoRunQueue();
    }
    else
    {
        DirectCommands dc(this, CompilerFactory::Compilers[m_CompilerIdx], m_Project, m_PageIndex);
        wxArrayString clean = dc.GetCleanCommands(target, false);
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Cleaning %s..."), target ? target->GetTitle().c_str() : m_Project->GetTitle().c_str());
        for (unsigned int i = 0; i < clean.GetCount(); ++i)
        {
            wxRemoveFile(clean[i]);
        }
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Done."));
        Manager::Get()->GetMessageManager()->Close();
    }
    return 0;
}

int CompilerGCC::DistClean(ProjectBuildTarget* target)
{
	DoPrepareQueue();
	if (!CompilerValid(target))
		return -1;

    if (CompilerFactory::CompilerIndexOK(m_CompilerIdx))
        CompilerFactory::Compilers[m_CompilerIdx]->GetCustomVars().ApplyVarsToEnvironment();
    m_Project->GetCustomVars().ApplyVarsToEnvironment();

    Manager::Get()->GetMessageManager()->Open();

    wxSetWorkingDirectory(m_Project->GetBasePath());
    if (UseMake(target))
    {
        wxString cmd;
        wxString make = CompilerFactory::Compilers[m_CompilerIdx]->GetPrograms().MAKE;
        if (target)
            cmd << make << _T(" -f ") << m_LastTempMakefile << _T(" distclean_") << target->GetTitle();
        else
            cmd << make << _T(" -f ") << m_LastTempMakefile << _T(" distclean");
        m_Queue.Add(cmd);
        return DoRunQueue();
    }
    else
    {
        DirectCommands dc(this, CompilerFactory::Compilers[m_CompilerIdx], m_Project, m_PageIndex);
        wxArrayString clean = dc.GetCleanCommands(target, true);
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Dist-cleaning %s..."), target ? target->GetTitle().c_str() : m_Project->GetTitle().c_str());
        for (unsigned int i = 0; i < clean.GetCount(); ++i)
        {
            wxRemoveFile(clean[i]);
        }
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Done."));
        Manager::Get()->GetMessageManager()->Close();
    }
    return 0;
}

int CompilerGCC::CreateDist()
{
	DoPrepareQueue();
	if (!CompilerValid())
		return -1;

    Manager::Get()->GetMessageManager()->Open();

    wxString cmd;
    if (UseMake())
    {
        wxString make = CompilerFactory::Compilers[m_CompilerIdx]->GetPrograms().MAKE;
        cmd << make << _T(" -f ") << m_LastTempMakefile << _T(" dist");
        m_Queue.Add(cmd);
        return DoRunQueue();
    }
    else
        wxMessageBox(_("\"Create distribution\" is only valid when using GNU \"make\"..."));

    return -1;
}

void CompilerGCC::OnExportMakefile(wxCommandEvent& event)
{
	if (!CompilerValid())
		return;
	wxString makefile = wxGetTextFromUser(_("Please enter the \"Makefile\" name:"), _("Export Makefile"), ProjectMakefile());
	if (makefile.IsEmpty())
		return;

    Manager::Get()->GetMessageManager()->Open();

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
    wxMessageBox(msg);
}

int CompilerGCC::Compile(ProjectBuildTarget* target)
{
    DoClearErrors();
	DoPrepareQueue();
	if (!m_Project || !CompilerValid(target))
        return -2;

    if (CompilerFactory::CompilerIndexOK(m_CompilerIdx))
        CompilerFactory::Compilers[m_CompilerIdx]->GetCustomVars().ApplyVarsToEnvironment();
    m_Project->GetCustomVars().ApplyVarsToEnvironment();

    Manager::Get()->GetMessageManager()->Open();

    wxString cmd;
    wxSetWorkingDirectory(m_Project->GetBasePath());
    if (UseMake(target))
    {
        wxString make = CompilerFactory::Compilers[m_CompilerIdx]->GetPrograms().MAKE;
        if (target)
            cmd << make << _T(" -f ") << m_LastTempMakefile << _T(" ") << target->GetTitle();
        else
            cmd << make << _T(" -f ") << m_LastTempMakefile;
        m_Queue.Add(cmd);
    }
    else
    {
        DirectCommands dc(this, CompilerFactory::Compilers[m_CompilerIdx], m_Project, m_PageIndex);
        wxArrayString compile = dc.GetCompileCommands(target);
        dc.AppendArray(compile, m_Queue);
    }
    return DoRunQueue();
}

int CompilerGCC::Rebuild(ProjectBuildTarget* target)
{
	DoPrepareQueue();
	if (!CompilerValid(target))
		return -1;

    if (CompilerFactory::CompilerIndexOK(m_CompilerIdx))
        CompilerFactory::Compilers[m_CompilerIdx]->GetCustomVars().ApplyVarsToEnvironment();
    m_Project->GetCustomVars().ApplyVarsToEnvironment();

    Manager::Get()->GetMessageManager()->Open();

    if (UseMake(target))
    {
        wxString cmd;
        wxString make = CompilerFactory::Compilers[m_CompilerIdx]->GetPrograms().MAKE;
        if (target)
        {
            cmd << make << _T(" -f ") << m_LastTempMakefile << _T(" clean_") << target->GetTitle();
            m_Queue.Add(cmd);
            cmd.Clear();
            cmd << make << _T(" -f ") << m_LastTempMakefile << _T(" ") << target->GetTitle();
            m_Queue.Add(cmd);
        }
        else
        {
            cmd << make << _T(" -f ") << m_LastTempMakefile << _T(" clean");
            m_Queue.Add(cmd);
            cmd.Clear();
            cmd << make << _T(" -f ") << m_LastTempMakefile;
            m_Queue.Add(cmd);
        }
    }
    else
    {
        Clean(target);
        Compile(target);
    }
    return DoRunQueue();
}

int CompilerGCC::CompileAll()
{
    Manager::Get()->GetMessageManager()->Open();
    DoPrepareMultiProjectCommand(mpjCompile);
    DoPrepareQueue();
    ClearLog();
    ProjectBuildTarget* target = DoAskForTarget();
    return Compile(target);
}

int CompilerGCC::RebuildAll()
{
    Manager::Get()->GetMessageManager()->Open();
    DoPrepareMultiProjectCommand(mpjRebuild);
    DoPrepareQueue();
    ClearLog();
    ProjectBuildTarget* target = DoAskForTarget();
    return Rebuild(target);
}

int CompilerGCC::KillProcess()
{
    if (!m_Process || !m_Pid)
        return -1;
    wxKillError ret;
    bool isdirect=(!UseMake());

    m_Queue.Clear();

    // Close input pipe
    m_Process->CloseOutput();
    ret = wxProcess::Kill(m_Pid, wxSIGTERM);
    if(isdirect && ret!=wxKILL_OK)
    {
        // No need to tell the user about the errors - just keep him waiting.
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Aborting..."));
    }
    else switch (ret)
    {
        case wxKILL_ACCESS_DENIED: wxMessageBox(_("Access denied")); break;
        case wxKILL_NO_PROCESS: wxMessageBox(_("No process")); break;
        case wxKILL_BAD_SIGNAL: wxMessageBox(_("Bad signal")); break;
        case wxKILL_ERROR: wxMessageBox(_("Unspecified error")); break;

        case wxKILL_OK:
        default: Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Process killed..."));
    }
    return ret;
}

ProjectBuildTarget* CompilerGCC::GetBuildTargetForFile(ProjectFile* pf)
{
    if (!pf)
        return 0;

    if (!pf->buildTargets.GetCount())
    {
        wxMessageBox(_("That file isn't assigned to any target."),
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

    Manager::Get()->GetMessageManager()->Open();

    if (m_Project)
        wxSetWorkingDirectory(m_Project->GetBasePath());

    ProjectFile* pf = m_Project ? m_Project->GetFileByFilename(file, true, false) : 0;
    ProjectBuildTarget* bt = GetBuildTargetForFile(pf);
    bool useMake = UseMake(bt);

    if (!pf)
    {
        // compile single file not belonging to a project

        // switch to the default compiler
        SwitchCompiler(CompilerFactory::GetDefaultCompilerIndex());

        if (useMake)
        {
            wxMessageBox(_("That file doesn't belong to a project.\n"
                            "If you want to compile it as stand-alone, please use the \"Invoke compiler directly\" build method\n"
                            "(Settings->Compiler->Other->Build method)"),
                        _("Information"), wxICON_INFORMATION);
        }
        else
        {
            // get compile commands for file (always linked as console-executable)
            DirectCommands dc(this, CompilerFactory::GetDefaultCompiler(), 0, m_PageIndex);
            wxArrayString compile = dc.GetCompileSingleFileCommand(file);
            dc.AppendArray(compile, m_Queue);

            // apply global custom vars
            CompilerFactory::GetDefaultCompiler()->GetCustomVars().ApplyVarsToEnvironment();
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

        // apply global custom vars
        CompilerFactory::Compilers[bt->GetCompilerIndex()]->GetCustomVars().ApplyVarsToEnvironment();

        wxString make = CompilerFactory::Compilers[m_CompilerIdx]->GetPrograms().MAKE;
        m_Queue.Add(make + _T(" -f ") + m_LastTempMakefile + _T(" depend_") + bt->GetTitle() + _T("_DIRS")); // make the output dir
        m_Queue.Add(make + _T(" -f ") + m_LastTempMakefile + _T(" ") + fname);
    }
    else
    {
        DirectCommands dc(this, CompilerFactory::Compilers[bt->GetCompilerIndex()], m_Project, m_PageIndex);
        wxArrayString compile = dc.CompileFile(bt, pf);
        dc.AppendArray(compile, m_Queue);
    }
    return DoRunQueue();
}

// events

void CompilerGCC::OnIdle(wxIdleEvent& event)
{
    if (m_Process && ((PipedProcess*)m_Process)->HasInput())
		event.RequestMore();
	else
		event.Skip();
}

void CompilerGCC::OnTimer(wxTimerEvent& event)
{
	wxWakeUpIdle();
}

void CompilerGCC::OnRun(wxCommandEvent& event)
{
	m_RunAfterCompile = true;
	if (Run() == 0)
		DoRunQueue();
	m_RunAfterCompile = false;
}

void CompilerGCC::OnCompileAndRun(wxCommandEvent& event)
{
	m_RunAfterCompile = true;
    Compile(DoAskForTarget());
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
    Compile(target);
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
        file.MakeRelativeTo(m_Project->GetBasePath());
#ifdef ALWAYS_USE_MAKEFILE
	file.SetExt(OBJECT_EXT);
#endif
    wxString fname = file.GetFullPath();
    if (!fname.IsEmpty())
        CompileFile(UnixFilename(fname));
}

void CompilerGCC::OnRebuild(wxCommandEvent& event)
{
	if (wxMessageBox(_("Rebuilding the project will cause the deletion of all "
                        "object files and building it from scratch.\nThis action "
                        "might take a while, especially if your project contains "
                        "more than a few files.\nAnother factor is your CPU "
                        "and the available system memory.\n\n"
                        "Are you sure you want to rebuild the entire project?"),
					_("Rebuild project"),
					wxYES_NO | wxICON_QUESTION) == wxNO)
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
    CompileAll();
}

void CompilerGCC::OnRebuildAll(wxCommandEvent& event)
{
	if (wxMessageBox(_("Rebuilding ALL the open projects will cause the deletion of all "
                        "object files and building them from scratch.\nThis action "
                        "might take a while, especially if your projects contain "
                        "more than a few files.\nAnother factor is your CPU "
                        "and the available system memory.\n\n"
                        "Are you sure you want to rebuild ALL the projects?"),
					_("Rebuild projects"),
					wxYES_NO | wxICON_QUESTION) == wxNO)
    {
        return;
    }
    RebuildAll();
}

void CompilerGCC::OnClean(wxCommandEvent& event)
{
	if (wxMessageBox(_("Cleaning the target or project will cause the deletion "
                        "of all relevant object files.\nThis means that you will "
                        "have to build your project from scratch next time you "
                        "'ll want to build it.\nThat action "
                        "might take a while, especially if your project contains "
                        "more than a few files.\nAnother factor is your CPU "
                        "and the available system memory.\n\n"
                        "Are you sure you want to proceed to cleaning?"),
					_("Clean target/project"),
					wxYES_NO | wxICON_QUESTION) == wxNO)
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

void CompilerGCC::OnDistClean(wxCommandEvent& event)
{
	if (wxMessageBox(_("Dist-cleaning the target or project will cause the deletion "
                        "of all relevant object files.\nThis means that you will "
                        "have to build your project from scratch next time you "
                        "'ll want to build it.\nThat action "
                        "might take a while, especially if your project contains "
                        "more than a few files.\nAnother factor is your CPU "
                        "and the available system memory.\n\n"
                        "Are you sure you want to proceed to dist-cleaning?"),
					_("Dist-clean target/project"),
					wxYES_NO | wxICON_QUESTION) == wxNO)
    {
        return;
    }

	int bak = m_TargetIndex;
    if (event.GetId() == idMenuDistCleanTargetFromProjectManager)
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
    else if (event.GetId() == idMenuDistCleanFromProjectManager)
    {
    	// we 're called from a menu in ProjectManager
    	// let's check the selected project...
    	DoSwitchProjectTemporarily();
    }
    ProjectBuildTarget* target = DoAskForTarget();
    DistClean(target);
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
		m_TargetIndex = m_ToolTarget->GetSelection() - (m_HasTargetAll ? 1 : 0);
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

void CompilerGCC::OnCreateDist(wxCommandEvent& event)
{
    CreateDist();
}

void CompilerGCC::OnUpdateUI(wxUpdateUIEvent& event)
{
	cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    wxMenuBar* mbar = Manager::Get()->GetAppWindow()->GetMenuBar();
    if (mbar)
    {
        mbar->Enable(idMenuCompile, !m_Process && prj);
        mbar->Enable(idMenuCompileAll, !m_Process && prj);
        mbar->Enable(idMenuCompileFromProjectManager, !m_Process && prj);
        mbar->Enable(idMenuCompileTargetFromProjectManager, !m_Process && prj);
        mbar->Enable(idMenuCompileFile, !m_Process && ed);
        mbar->Enable(idMenuCompileFileFromProjectManager, !m_Process && prj);
        mbar->Enable(idMenuRebuild, !m_Process && prj);
        mbar->Enable(idMenuRebuildAll, !m_Process && prj);
        mbar->Enable(idMenuRebuildFromProjectManager, !m_Process && prj);
        mbar->Enable(idMenuRebuildTargetFromProjectManager, !m_Process && prj);
        mbar->Enable(idMenuClean, !m_Process && prj);
        mbar->Enable(idMenuDistClean, !m_Process && prj);
        mbar->Enable(idMenuCleanFromProjectManager, !m_Process && prj);
        mbar->Enable(idMenuDistCleanFromProjectManager, !m_Process && prj);
        mbar->Enable(idMenuCleanTargetFromProjectManager, !m_Process && prj);
        mbar->Enable(idMenuDistCleanTargetFromProjectManager, !m_Process && prj);
        mbar->Enable(idMenuCompileAndRun, !m_Process && prj);
        mbar->Enable(idMenuRun, !m_Process && prj);
        mbar->Enable(idMenuKillProcess, m_Process);
        mbar->Enable(idMenuSelectTarget, !m_Process && prj);

        mbar->Enable(idMenuNextError, !m_Process && prj && m_Errors.HasNextError());
        mbar->Enable(idMenuPreviousError, !m_Process && prj && m_Errors.HasPreviousError());
//        mbar->Enable(idMenuClearErrors, cnt);

        mbar->Enable(idMenuCreateDist, !m_Process && prj);
        mbar->Enable(idMenuExportMakefile, !m_Process && prj);

        // Project menu
        mbar->Enable(idMenuProjectCompilerOptions, !m_Process && prj);
    }

	// enable/disable compiler toolbar buttons
	wxToolBar* tbar = m_pTbar;//Manager::Get()->GetAppWindow()->GetToolBar();
	if (tbar)
	{
        tbar->EnableTool(idMenuCompile, !m_Process && prj);
        tbar->EnableTool(idMenuRun, !m_Process && prj);
        tbar->EnableTool(idMenuCompileAndRun, !m_Process && prj);
        tbar->EnableTool(idMenuRebuild, !m_Process && prj);
        tbar->EnableTool(idMenuKillProcess, m_Process && prj);

        m_ToolTarget = XRCCTRL(*tbar, "idToolTarget", wxComboBox);
        if (m_ToolTarget)
            m_ToolTarget->Enable(!m_Process && prj);
    }

    // allow other UpdateUI handlers to process this event
    // *very* important! don't forget it...
    event.Skip();
}

void CompilerGCC::OnProjectActivated(CodeBlocksEvent& event)
{
    //Manager::Get()->GetMessageManager()->Log(mltDevDebug, "OnProjectActivated()");
	DoRecreateTargetMenu();
	event.Skip(); // *very* important! don't forget it...
}

/*void CompilerGCC::OnProjectPopupMenu(wxNotifyEvent& event)
{
	BuildModuleMenu(mtProjectManager, (wxMenu*)event.GetClientData(), event.GetString());
	event.Skip();
}*/

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

void CompilerGCC::AddOutputLine(const wxString& output, bool forceErrorColor)
{
    size_t maxErrors = ConfigManager::Get()->Read(_T("/compiler_gcc/max_reported_errors"), 50);
    if (maxErrors > 0)
    {
        if (m_Errors.GetErrorsCount() > maxErrors)
            return;
        else if (m_Errors.GetErrorsCount() == maxErrors)
        {
            // if we reached the max errors count, notify about it
            m_Errors.AddError(_T(""), 0, _("More errors follow but not being shown."), false);
            m_Errors.AddError(_T(""), 0, _("Edit the max errors limit in compiler options..."), false);
            return;
        }
    }

	Compiler* compiler = CompilerFactory::Compilers[m_CompilerIdx];
	CompilerLineType clt = compiler->CheckForWarningsAndErrors(output);

	switch (clt)
	{
        case cltWarning:
			m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(COLOUR_NAVY));
			break;

        case cltError:
			m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxRED));
			break;

        default:
            if (forceErrorColor)
                m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(COLOUR_MAROON));
            else
                m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLACK));
			break;
	}

	if (clt != cltNormal)
	{
        wxArrayString errors;
        errors.Add(compiler->GetLastErrorFilename());
        errors.Add(compiler->GetLastErrorLine());
        errors.Add(compiler->GetLastError());
        m_pListLog->AddLog(errors);
        m_pListLog->GetListControl()->SetColumnWidth(2, wxLIST_AUTOSIZE);

        // colorize the list output
/* NOTE (mandrav#1#): For this to work under win32, one must use -D_WIN32_IE=0x300 when building wxWidgets
                      and probably edit wx/msw/treectrl.cpp and wx/listctrl.cpp (grep for _WIN32_IE) */
        m_pListLog->GetListControl()->SetItemTextColour(m_pListLog->GetListControl()->GetItemCount() - 1,
                                                        clt == cltWarning ? COLOUR_NAVY : *wxRED);

        m_Errors.AddError(compiler->GetLastErrorFilename(),
                          !compiler->GetLastErrorLine().IsEmpty() ? atoi(compiler->GetLastErrorLine().mb_str()) : 0,
                          compiler->GetLastError(),
                          clt == cltWarning);
    }

	if (!output.IsEmpty())
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, output.c_str());
}

void CompilerGCC::OnGCCTerminated(CodeBlocksEvent& event)
{
	m_LastExitCode = event.GetInt();
	OnJobEnd();
}

void CompilerGCC::OnJobEnd()
{
    m_timerIdleWakeUp.Stop();
    m_Pid = 0;

	bool ended = true;
    if (m_Queue.GetCount() != 0 && m_QueueIndex < m_Queue.GetCount() - 1)
    {
        if (m_LastExitCode == 0)
        {
			++m_QueueIndex;
            if (DoRunQueue() != -3) // not end of queue
				ended = false;
		}
    }

//    m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLUE));
//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("%d commands in queue (at %d)"), m_Queue.GetCount(), m_QueueIndex);
//	m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLACK, *wxWHITE));

	if (ended)
    {
        long int elapsed = wxGetElapsedTime() / 1000;
        int mins = elapsed / 60;
        int secs = (elapsed % 60);
        m_Log->GetTextControl()->SetDefaultStyle(m_LastExitCode == 0 ? wxTextAttr(*wxBLUE) : wxTextAttr(*wxRED));
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Process terminated with status %d (%d minutes, %d seconds)"), m_LastExitCode, mins, secs);
        if (!m_RunAfterCompile)
        {
            m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(COLOUR_NAVY));
            Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("%d errors, %d warnings"), m_Errors.GetErrorsCount(), m_Errors.GetWarningsCount());
        }
        m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLACK, *wxWHITE));
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T(" ")); // blank line

        if (m_LastExitCode == 0)
        {
            if (m_RunAfterCompile)
            {
                m_QueueIndex = 0;
                m_Queue.Clear();
                if (Run() == 0)
                    DoRunQueue();
            }
            if (m_DoAllProjects != mpjNone)
            {
                ProjectManager* prjMan = Manager::Get()->GetProjectManager();
                ProjectsArray* projects = prjMan->GetProjects();

                if (m_ProjectIndex < projects->GetCount() - 1)
                {
                    prjMan->SetProject(projects->Item(++m_ProjectIndex), false);
                    CheckProject();
                    m_QueueIndex = 0;
                    if (UseMake())
                    {
                        wxString oldMK = m_LastTempMakefile;
                        DoCreateMakefile();
                        for (unsigned int i = 0; i < m_Queue.GetCount(); ++i)
                            m_Queue[i].Replace(oldMK, m_LastTempMakefile);
                        DoRunQueue();
                    }
                    else
                    {
                        ProjectBuildTarget* target = DoAskForTarget();
                        m_Queue.Clear();
                        switch (m_DoAllProjects)
                        {
                            case mpjCompile: Compile(target); break;
                            case mpjRebuild: Rebuild(target); break;
                            default: break;
                        }
                    }
                }
                else if (m_BackupActiveProject)
                {
                    m_DoAllProjects = mpjNone;
                    m_QueueIndex = 0;
                    m_Queue.Clear();
                    prjMan->SetProject(m_BackupActiveProject, false);
                    AskForActiveProject();
                    DoDeleteTempMakefile();
                    // if message manager is auto-hiding, this will unlock it
                    Manager::Get()->GetMessageManager()->Close(true);
                }
            }
            else
            {
                m_Queue.Clear();
                m_QueueIndex = 0;
                DoDeleteTempMakefile();

                // if message manager is auto-hiding, this will unlock it
                Manager::Get()->GetMessageManager()->Close(true);
            }
        }
        else
        {
            m_DoAllProjects = mpjNone;
            m_Queue.Clear();
            m_QueueIndex = 0;
            if (m_Errors.GetCount())
            {
                Manager::Get()->GetMessageManager()->Open();
                Manager::Get()->GetMessageManager()->SwitchTo(m_ListPageIndex);
                DoGotoNextError();
            }
        }
        m_RunAfterCompile = false;
    }
}
