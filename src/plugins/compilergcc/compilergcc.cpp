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
#include <projectmanager.h>
#include <editormanager.h>
#include "makefilegenerator.h"
#include "compileroptionsdlg.h"
#include "compilerMINGW.h"
#include "compilerMSVC.h"
#include "compilerBCC.h"
#include "directcommands.h"
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>

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
int idTimerPollCompiler = wxNewId();
int idMenuCompile = wxNewId();
int idMenuCompileTarget = wxNewId();
int idMenuCompileFromProjectManager = wxNewId();
int idMenuProjectCompilerOptions = wxNewId();
int idMenuTargetCompilerOptions = wxNewId();
int idMenuTargetCompilerOptionsSub = wxNewId();
int idMenuCompileTargetFromProjectManager = wxNewId();
int idMenuCompileFile = wxNewId();
int idMenuCompileFileFromProjectManager = wxNewId();
int idMenuRebuild = wxNewId();
int idMenuRebuildTarget = wxNewId();
int idMenuRebuildFromProjectManager = wxNewId();
int idMenuRebuildTargetFromProjectManager = wxNewId();
int idMenuCompileAll = wxNewId();
int idMenuRebuildAll = wxNewId();
int idMenuClean = wxNewId();
int idMenuDistClean = wxNewId();
int idMenuCleanTarget = wxNewId();
int idMenuDistCleanTarget = wxNewId();
int idMenuCleanFromProjectManager = wxNewId();
int idMenuDistCleanFromProjectManager = wxNewId();
int idMenuCleanTargetFromProjectManager = wxNewId();
int idMenuDistCleanTargetFromProjectManager = wxNewId();
int idMenuCompileAndRun = wxNewId();
int idMenuRun = wxNewId();
int idMenuKillProcess = wxNewId();
int idMenuSelectTarget = wxNewId();
int idMenuSelectTargetAll = wxNewId();
int idMenuSelectTargetOther[MAX_TARGETS]; // initialized in ctor
int idMenuNextError = wxNewId();
int idMenuPreviousError = wxNewId();
int idMenuClearErrors = wxNewId();
int idMenuCreateDist = wxNewId();
int idMenuExportMakefile = wxNewId();
int idToolTarget = wxNewId();
int idToolTargetLabel = wxNewId();

int idGCCProcess = wxNewId();

BEGIN_EVENT_TABLE(CompilerGCC, cbCompilerPlugin)
    EVT_UPDATE_UI_RANGE(idMenuCompile, idToolTargetLabel, CompilerGCC::OnUpdateUI)
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
	m_Vars(this),
	m_QueueIndex(0),
	m_DeleteTempMakefile(true)
{
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::Get()->Read("data_path", wxEmptyString);
    wxXmlResource::Get()->Load(resPath + "/compiler_gcc.zip#zip:*.xrc");

    m_Type = ptCompiler;
    m_PluginInfo.name = "Compiler";
    m_PluginInfo.title = "Compiler";
    m_PluginInfo.version = "1.0";
    m_PluginInfo.description = "This plugin is an interface to various compilers:\n\n"
                               "\tGNU GCC compiler\n"
                               "\tMicrosoft Visual C++ Free Toolkit 2003\n"
                               "\tBorland C++ Compiler 5.5";
    m_PluginInfo.author = "Yiannis An. Mandravellos";
    m_PluginInfo.authorEmail = "info@codeblocks.org";
    m_PluginInfo.authorWebsite = "www.codeblocks.org";
    m_PluginInfo.thanksTo = "All the free (and not) compilers out there";
	m_PluginInfo.hasConfigure = true;

    m_timerIdleWakeUp.SetOwner(this, idTimerPollCompiler);
	
	for (int i = 0; i < MAX_TARGETS; ++i)
		idMenuSelectTargetOther[i] = wxNewId();
#ifndef __WXMSW__
	m_ConsoleShell = ConfigManager::Get()->Read("/compiler_gcc/console_shell", DEFAULT_CONSOLE_SHELL);
#endif
	
	// register built-in compilers
	CompilerFactory::RegisterCompiler(new CompilerMINGW);
#ifdef __WXMSW__
	CompilerFactory::RegisterCompiler(new CompilerMSVC);
	CompilerFactory::RegisterCompiler(new CompilerBCC);
#endif
	// register (if any) user-copies of built-in compilers
	CompilerFactory::RegisterUserCompilers();

	ConfigManager::AddConfiguration(m_PluginInfo.title, "/compiler_gcc");
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
	wxString prefix = ConfigManager::Get()->Read("data_path") + "/images/";
    bmp.LoadFile(prefix + "misc_16x16.png", wxBITMAP_TYPE_PNG);
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
    bmp.LoadFile(prefix + "flag_16x16.png", wxBITMAP_TYPE_PNG);
    Manager::Get()->GetMessageManager()->SetLogImage(m_pListLog, bmp);

    // set default compiler for new projects
    CompilerFactory::SetDefaultCompilerIndex(ConfigManager::Get()->Read("/compiler_gcc/default_compiler", (long int)0));
	LoadOptions();
	SetupEnvironment();
}

void CompilerGCC::OnRelease(bool appShutDown)
{
    DoDeleteTempMakefile();
	SaveOptions();
    ConfigManager::Get()->Write("/compiler_gcc/default_compiler", CompilerFactory::GetDefaultCompilerIndex());
	if (Manager::Get()->GetMessageManager())
	{
        Manager::Get()->GetMessageManager()->DeletePage(m_ListPageIndex);
        Manager::Get()->GetMessageManager()->DeletePage(m_PageIndex);
    }
    
	if (appShutDown)
		return; // no need to continue if app is shutting down

	DoClearTargetMenu();

	if (m_Menu)
	{
		wxMenuBar* mBar = Manager::Get()->GetAppWindow()->GetMenuBar();
		int pos = mBar->FindMenu(_("&Compile"));
		if (pos != wxNOT_FOUND)
			mBar->Remove(pos);
		delete m_Menu;
		m_Menu = 0L;
	}

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
	dlg.ShowModal();
	m_ConsoleShell = ConfigManager::Get()->Read("/compiler_gcc/console_shell", DEFAULT_CONSOLE_SHELL);
	SaveOptions();
	return 0;
}

void CompilerGCC::BuildMenu(wxMenuBar* menuBar)
{
	if (!m_IsAttached)
		return;
//	if (m_Menu)
//		return;
	m_Menu = new wxMenu("");
    m_Menu->Append(idMenuRun, _("&Run\tCtrl-F10"), _("Run current project"));
    m_Menu->Append(idMenuCompileAndRun, _("Com&pile && run\tF9"), _("Compile and run current project"));
    m_Menu->AppendSeparator();
    m_Menu->Append(idMenuCompile, _("&Compile\tCtrl-F9"), _("Compile current project"));
    m_Menu->Append(idMenuCompileFile, _("Compile current &file\tCtrl-Shift-F9"), _("Compile current file"));
    m_Menu->Append(idMenuRebuild, _("Re&build\tCtrl-F11"), _("Rebuild current project"));
    m_Menu->Append(idMenuClean, _("C&lean"), _("Clean current project"));
    m_Menu->Append(idMenuDistClean, _("Di&st clean"), _("Clean current project (dependencies too)"));
    m_Menu->AppendSeparator();
    m_Menu->Append(idMenuCompileAll, _("Compile all pro&jects"), _("Compile all projects"));
    m_Menu->Append(idMenuRebuildAll, _("Reb&uild all projects"), _("Rebuild all projects"));
    m_Menu->AppendSeparator();
    m_Menu->Append(idMenuKillProcess, _("&Kill process"), _("Kill the running process"));
    m_Menu->AppendSeparator();

	m_ErrorsMenu = new wxMenu(""); // target selection menu
	m_ErrorsMenu->Append(idMenuPreviousError, _("&Previous error\tAlt-F1"), _("Go to previous compilation error"));
	m_ErrorsMenu->Append(idMenuNextError, _("&Next error\tAlt-F2"), _("Go to next compilation error"));
    m_ErrorsMenu->AppendSeparator();
	m_ErrorsMenu->Append(idMenuClearErrors, _("&Clear all errors"), _("Clear all compilation errors"));
	m_Menu->Append(idMenuSelectTarget, _("&Errors"), m_ErrorsMenu);

	m_TargetMenu = new wxMenu(""); // target selection menu
	DoRecreateTargetMenu();
	m_Menu->Append(idMenuSelectTarget, _("Select target..."), m_TargetMenu);

    m_Menu->AppendSeparator();
    m_Menu->Append(idMenuCreateDist, _("Create &distribution"), _("Create a source distribution package"));
    m_Menu->Append(idMenuExportMakefile, _("E&xport Makefile"), _("Export Makefile so that you can build the program from the command line"));

	// ok, now, where do we insert?
	// three possibilities here:
	// a) locate "Debug" menu and insert before it
	// b) locate "Project" menu and insert after it
	// c) if not found (?), insert at pos 5
	int finalPos = 5;
	int projMenuPos = menuBar->FindMenu("Debug");
	if (projMenuPos != wxNOT_FOUND)
		finalPos = projMenuPos;
	else
	{
		projMenuPos = menuBar->FindMenu("Project");
		if (projMenuPos != wxNOT_FOUND)
			finalPos = projMenuPos + 1;
	}
    menuBar->Insert(finalPos, m_Menu, _("&Compile"));
    
    // now add some entries in Project menu
	projMenuPos = menuBar->FindMenu("Project");
	if (projMenuPos != wxNOT_FOUND)
	{
        wxMenu* prj = menuBar->GetMenu(projMenuPos);
        // look if we have a "Properties" item. If yes, we 'll insert
        // before it, else we 'll just append...
        size_t propsPos = prj->GetMenuItemCount(); // append
        int propsID = prj->FindItem("Properties");
        if (propsID != wxNOT_FOUND)
            prj->FindChildItem(propsID, &propsPos);
        prj->Insert(propsPos, idMenuProjectCompilerOptions, _("Build options"), _("Set the project's build options"));
    }
}

void CompilerGCC::RemoveMenu(wxMenuBar* menuBar)
{
    int idx = menuBar->FindMenu(_("&Compile"));
    if (idx != wxNOT_FOUND)
    {
        m_Menu = menuBar->Remove(idx);
        delete m_Menu;
        m_Menu = 0;
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
		
    if (arg.IsEmpty())
    {
        // popup menu in empty space in ProjectManager
        menu->Append(idMenuCompileAll, _("Compile all projects"));
        menu->Append(idMenuRebuildAll, _("Rebuild all projects"));
    }
    else if (FileTypeOf(arg) != ftSource)
    {
        // popup menu on a tree item (not a file)
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
    else
    {
        // popup menu on a tree item (a file)
        menu->AppendSeparator();
        menu->Append(idMenuCompileFileFromProjectManager, _("Compile file"));
    }
}

void CompilerGCC::BuildToolBar(wxToolBar* toolBar)
{
	if (!m_IsAttached)
		return;
	if (toolBar)
	{
		m_pToolbar = toolBar;
		wxString res = ConfigManager::Get()->Read("/data_path") + "/images/";
		
		toolBar->AddSeparator();
		
		toolBar->AddTool(idMenuCompile, _("Compile"), wxBitmap(res + "compile.png", wxBITMAP_TYPE_PNG));
		toolBar->SetToolShortHelp(idMenuCompile, _("Compile"));
		toolBar->SetToolLongHelp(idMenuCompile, _("Compile the active project"));

		toolBar->AddTool(idMenuRun, _("Run"), wxBitmap(res + "run.png", wxBITMAP_TYPE_PNG));
		toolBar->SetToolShortHelp(idMenuRun, _("Run"));
		toolBar->SetToolLongHelp(idMenuRun, _("Run the active project"));

		toolBar->AddTool(idMenuCompileAndRun, _("Compile and run"), wxBitmap(res + "compilerun.png", wxBITMAP_TYPE_PNG));
		toolBar->SetToolShortHelp(idMenuCompileAndRun, _("Compile and run"));
		toolBar->SetToolLongHelp(idMenuCompileAndRun, _("Compile and run the active project"));

		toolBar->AddTool(idMenuRebuild, _("Rebuild"), wxBitmap(res + "rebuild.png", wxBITMAP_TYPE_PNG));
		toolBar->SetToolShortHelp(idMenuRebuild, _("Rebuild"));
		toolBar->SetToolLongHelp(idMenuRebuild, _("Rebuild the active project"));

		toolBar->AddSeparator();
		
// neither the generic nor Motif native toolbars really support this
#if (wxUSE_TOOLBAR_NATIVE && !USE_GENERIC_TBAR) && !defined(__WXMOTIF__) && !defined(__WXX11__) && !defined(__WXMAC__)
		m_ToolTargetLabel = new wxStaticText(toolBar, idToolTargetLabel, _("Build target: "), wxDefaultPosition);
		toolBar->AddControl(m_ToolTargetLabel);
		m_ToolTarget = new wxComboBox(toolBar, idToolTarget, "Compiler Target Selection", wxDefaultPosition, wxDefaultSize, 0, 0L, wxCB_DROPDOWN | wxCB_READONLY);
		toolBar->AddControl(m_ToolTarget);
		m_ToolTarget->Enable(false); // to force OnUpdateUI() to call tbar->Refresh() the first time...
#endif
		toolBar->Realize();
	}
}

void CompilerGCC::RemoveToolBar(wxToolBar* toolBar)
{
    toolBar->DeleteTool(idMenuCompile);
    toolBar->DeleteTool(idMenuRun);
    toolBar->DeleteTool(idMenuCompileAndRun);
    toolBar->DeleteTool(idMenuRebuild);
    toolBar->DeleteTool(idToolTargetLabel);
    toolBar->DeleteTool(idToolTarget);
    
    m_ToolTargetLabel = 0;
    m_ToolTarget = 0;
    m_pToolbar = 0;
}

void CompilerGCC::SetupEnvironment()
{
    if (!CompilerFactory::CompilerIndexOK(m_CompilerIdx))
        return;

    wxString sep = wxFileName::GetPathSeparator();
    
	wxPathList pathList;
	wxString path;
	Manager::Get()->GetMessageManager()->DebugLog(_("Setting up compiler environment..."));
    wxString masterPath = CompilerFactory::Compilers[m_CompilerIdx]->GetMasterPath();
    wxString gcc = CompilerFactory::Compilers[m_CompilerIdx]->GetPrograms().C;
	//Manager::Get()->GetMessageManager()->DebugLog("Checking in " + masterPath + sep + "bin for " + gcc);

    // reset PATH to original value
    if (!m_OriginalPath.IsEmpty())
        wxSetEnv("PATH", m_OriginalPath);

    // look for valid compiler in path
	if (wxGetEnv("PATH", &path))
	{
        if (m_OriginalPath.IsEmpty())
            m_OriginalPath = path;
        pathList.Add(masterPath + sep + "bin");
		pathList.AddEnvList("PATH");
		wxString binPath = pathList.FindAbsoluteValidPath(gcc);
        // it seems, under Win32, the above command doesn't search in paths with spaces...
        // look directly for the file in question in masterPath
		if (binPath.IsEmpty() || !pathList.Member(wxPathOnly(binPath)))
		{
            if (wxFileExists(masterPath + sep + "bin" + gcc))
                binPath = masterPath + sep + "bin";
		}
        
		if (binPath.IsEmpty() || !pathList.Member(wxPathOnly(binPath)))
		{
			m_EnvironmentMsg = _("Can't find compiler executable in your search path. "
								"Most probably, you will not be able to compile anything...");
			Manager::Get()->GetMessageManager()->DebugLog(_("Can't find compiler executable in your search path..."));
		}
		else
		{
#ifdef __WXMSW__
	#define PATH_SEP ";"
#else
	#define PATH_SEP ":"
#endif
			// add bin path to PATH env. var.
			wxSetEnv("PATH", masterPath + sep + "bin" + PATH_SEP + path);
			wxGetEnv("PATH", &path);
            //Manager::Get()->GetMessageManager()->DebugLog("$PATH=" + path);
#undef PATH_SEP
		}
	}
	else
		m_EnvironmentMsg = _("Could not read the PATH environment variable!\n"
					"This can't be good. There may be problems running "
					"system commands and the application might not behave "
					"the way it was designed to...");
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
    Manager::Get()->GetProjectManager()->SetProject(ftd->GetProject());
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
    if (!CheckProject())
        return -1;
        
    // make sure all project files are saved
    if (!Manager::Get()->GetProjectManager()->GetActiveProject()->SaveAllFiles())
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
        msgMan->DebugLog("Queue has been emptied! (count=%d, index=%d)", m_Queue.GetCount(), m_QueueIndex);
        return -3;
	}

	DoClearErrors();
	m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLACK, *wxWHITE));
    wxString dir;// = m_Project->GetBasePath();
    wxString cmd;

    // loop added for compiler log when not working with Makefiles
    while (true)
    {
        cmd = m_Queue[m_QueueIndex];
//	    msgMan->Log(m_PageIndex, "cmd='%s' in '%s'", cmd.c_str(), m_CdRun.c_str());
    
        if (cmd.StartsWith(COMPILER_SIMPLE_LOG))
        {
            cmd.Remove(0, strlen(COMPILER_SIMPLE_LOG));
            msgMan->Log(m_PageIndex, cmd);
        }
        else
            break;

        ++m_QueueIndex;
        if (m_QueueIndex >= m_Queue.GetCount())
        {
            msgMan->Log(m_PageIndex, _("Nothing to be done."));
            m_LastExitCode = 0;
            OnJobEnd();
            return 0;
        }
    }

	bool pipe = true;
	int flags = wxEXEC_ASYNC;
	if (m_RunAfterCompile && m_IsRun && m_QueueIndex == m_Queue.GetCount() - 1)
	{
		pipe = false; // no need to pipe output channels...
		flags |= wxEXEC_NOHIDE;
		m_IsRun = false;
		dir = m_CdRun;
#ifndef __WXMSW__
		wxSetEnv("LD_LIBRARY_PATH", ".");
#endif
	}

    m_Process = new PipedProcess((void**)&m_Process, this, idGCCProcess, pipe, dir);
    m_Pid = wxExecute(cmd, flags, m_Process);
    if ( !m_Pid )
    {
        m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxWHITE, *wxRED));
        msgMan->Log(m_PageIndex, _("Command execution failed..."));
        msgMan->DebugLog(_("Execution of '%s' in '%s' failed."), m_Queue[m_QueueIndex].c_str(), wxGetCwd().c_str());
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
		for (wxMenuItemList::Node* node = items.GetFirst(); node; node = node->GetNext())
		{
			// Note: I added the second check to prevent segmentation faults, when
			// node exists, yet the data is NULL
			if (node && node->GetData())
				m_TargetMenu->Delete(node->GetData());
			else
				break;
		}
	}
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

    m_TargetMenu->AppendCheckItem(idMenuSelectTargetAll, _("All"), _("Compile target 'all' in current project"));
	if (m_ToolTarget)
		m_ToolTarget->Append(_("All"));
		
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
	
	m_TargetMenu->Check(idMenuSelectTargetAll, m_TargetIndex == -1);
	for (int i = 0; i < MAX_TARGETS; ++i)
	{
		m_TargetMenu->Check(idMenuSelectTargetOther[i], i == m_TargetIndex);
	}
	if (m_ToolTarget)
		m_ToolTarget->SetSelection(m_TargetIndex + 1);
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
	prjMan->SetProject(projects->Item(0));
	AskForActiveProject();
	
	return true;
}

void CompilerGCC::DoPrepareQueue()
{
	if (m_LastTempMakefile.IsEmpty() || m_Queue.GetCount() == 0)
	{
		m_QueueIndex = 0;
		if (m_DoAllProjects == mpjNone)
            ClearLog();
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
    m_LastTempMakefile = "";
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

    if (CompilerFactory::Compilers[m_CompilerIdx]->GetSwitches().buildMethod == cbmUseMake)
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
            m_LastTempMakefile = wxFileName::CreateTempFileName("cbmk", 0L);
        else
        {
            m_LastTempMakefile = makefile;
            if (m_LastTempMakefile.IsEmpty())
            {
                m_LastTempMakefile = ProjectMakefile();
                if (m_LastTempMakefile.IsEmpty())
                    m_LastTempMakefile = "Makefile";
            }
        }
    }

    PrintBanner();
	wxSetWorkingDirectory(m_Project->GetBasePath());

    if (CompilerFactory::Compilers[m_CompilerIdx]->GetSwitches().buildMethod == cbmUseMake)
    {
        MakefileGenerator generator(this, m_Project, m_LastTempMakefile, m_PageIndex);
        bool ret = generator.CreateMakefile();
    
        // if exporting Makefile, reset variable so that it's not deleted on
        // next Makefile generation :)
        if (!temporary)
            m_LastTempMakefile = "";
        m_DeleteTempMakefile = temporary;

        return ret;
    }
    return true;
}

void CompilerGCC::PrintBanner()
{
    if (!m_Project)
        return;
    Manager::Get()->GetMessageManager()->SwitchTo(m_PageIndex);
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Project   : %s"), m_Project->GetTitle().c_str());
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Compiler  : %s (%s)"), CompilerFactory::Compilers[m_Project->GetCompilerIndex()]->GetName().c_str(),
                                                                                    CompilerFactory::Compilers[m_Project->GetCompilerIndex()]->GetSwitches().buildMethod == cbmUseMake ? _("using GNU \"make\"") : _("called directly"));
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Directory : %s"), m_Project->GetBasePath().c_str());
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, "--------------------------------------------------------------------------------");
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


    wxString cmd;
    wxFileName f(UnixFilename(target->GetOutputFilename()));
    f.MakeAbsolute(m_Project->GetBasePath());
    m_CdRun = f.GetPath(wxPATH_GET_VOLUME);

    // for console projects, use helper app to wait for a key after
    // execution ends...
	if (target->GetTargetType() == ttConsoleOnly)
	{
#ifndef __WXMSW__
        // for non-win platforms, use m_ConsoleShell to run the console app
        wxString shell = m_ConsoleShell;
        shell.Replace("$TITLE", "'" + m_Project->GetTitle() + "'");
        cmd << shell << " ";
#endif
		wxString baseDir = ConfigManager::Get()->Read("/app_path");
		if (wxFileExists(baseDir + "/console_runner.exe"))
            cmd << baseDir << "/console_runner.exe ";
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
		cmd << "\"" << target->GetHostApplication() << "\" " << target->GetExecutionParameters();
	}
	else
    {
        cmd << "\"";
#ifndef __WXMSW__
        cmd << "./";
#endif
		cmd << f.GetFullName();
		cmd << "\" ";
		cmd << target->GetExecutionParameters();
    }
//		cmd << "\"" << target->GetOutputFilename() << "\" " << target->GetExecutionParameters();
//    wxMessageBox("cdrun=" + m_CdRun + "\nWill run: " + cmd);
	m_Queue.Add(cmd);

	m_IsRun = true;
	
	return 0;
}

int CompilerGCC::Clean(ProjectBuildTarget* target)
{
	DoPrepareQueue();

    if (CompilerFactory::Compilers[m_CompilerIdx]->GetSwitches().buildMethod == cbmUseMake)
    {
        wxString cmd;
        wxString make = CompilerFactory::Compilers[m_CompilerIdx]->GetPrograms().MAKE;
        if (target)
            cmd << make << " -f " << m_LastTempMakefile << " clean_" << target->GetTitle();
        else
            cmd << make << " -f " << m_LastTempMakefile << " clean";
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
    }
    return 0;
}

int CompilerGCC::DistClean(ProjectBuildTarget* target)
{
	DoPrepareQueue();

    if (CompilerFactory::Compilers[m_CompilerIdx]->GetSwitches().buildMethod == cbmUseMake)
    {
        wxString cmd;
        wxString make = CompilerFactory::Compilers[m_CompilerIdx]->GetPrograms().MAKE;
        if (target)
            cmd << make << " -f " << m_LastTempMakefile << " distclean_" << target->GetTitle();
        else
            cmd << make << " -f " << m_LastTempMakefile << " distclean";
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
    }
    return 0;
}

int CompilerGCC::CreateDist()
{
	DoPrepareQueue();

    wxString cmd;
    wxString make = CompilerFactory::Compilers[m_CompilerIdx]->GetPrograms().MAKE;
    cmd << make << " -f " << m_LastTempMakefile << " dist";
    m_Queue.Add(cmd);

    return DoRunQueue();
}

void CompilerGCC::OnExportMakefile(wxCommandEvent& event)
{
	wxString makefile = wxGetTextFromUser(_("Please enter the \"Makefile\" name:"), _("Export Makefile"), ProjectMakefile());
	if (makefile.IsEmpty())
		return;
    
    if (CompilerFactory::Compilers[m_CompilerIdx]->GetSwitches().buildMethod == cbmUseMake)
    {
        DoCreateMakefile(false, makefile);
    }
    else
    {
        wxSetWorkingDirectory(m_Project->GetBasePath());
        MakefileGenerator generator(this, m_Project, makefile, m_PageIndex);
        generator.CreateMakefile();
    }
    wxString msg;
    msg.Printf(_("\"%s\" has been exported in the same directory as the project file."), makefile.c_str());
    wxMessageBox(msg);
}

int CompilerGCC::Compile(ProjectBuildTarget* target)
{
	DoPrepareQueue();
	if (!m_Project)
        return -2;

    wxString cmd;
    if (CompilerFactory::Compilers[m_CompilerIdx]->GetSwitches().buildMethod == cbmUseMake)
    {
        wxString make = CompilerFactory::Compilers[m_CompilerIdx]->GetPrograms().MAKE;
        if (target)
            cmd << make << " -f " << m_LastTempMakefile << " " << target->GetTitle();
        else
            cmd << make << " -f " << m_LastTempMakefile;
        m_Queue.Add(cmd);
    }
    else
    {
        wxSetWorkingDirectory(m_Project->GetBasePath());
        DirectCommands dc(this, CompilerFactory::Compilers[m_CompilerIdx], m_Project, m_PageIndex);
        wxArrayString compile = dc.GetCompileCommands(target);
        dc.AppendArray(compile, m_Queue);
    }
    return DoRunQueue();
}

int CompilerGCC::Rebuild(ProjectBuildTarget* target)
{
	DoPrepareQueue();

    if (CompilerFactory::Compilers[m_CompilerIdx]->GetSwitches().buildMethod == cbmUseMake)
    {
        wxString cmd;
        wxString make = CompilerFactory::Compilers[m_CompilerIdx]->GetPrograms().MAKE;
        if (target)
        {
            cmd << make << " -f " << m_LastTempMakefile << " clean_" << target->GetTitle();
            m_Queue.Add(cmd);
            cmd.Clear();
            cmd << make << " -f " << m_LastTempMakefile << " " << target->GetTitle();
            m_Queue.Add(cmd);
        }
        else
        {
            cmd << make << " -f " << m_LastTempMakefile << " clean";
            m_Queue.Add(cmd);
            cmd.Clear();
            cmd << make << " -f " << m_LastTempMakefile;
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
    DoPrepareMultiProjectCommand(mpjCompile);
    DoPrepareQueue();
    ClearLog();
    return Compile();
}

int CompilerGCC::RebuildAll()
{
    DoPrepareMultiProjectCommand(mpjRebuild);
    DoPrepareQueue();
    ClearLog();
    return Rebuild();
}

int CompilerGCC::KillProcess()
{
    if (!m_Process || !m_Pid)
        return -1;
        
    m_Queue.Clear();

    wxKillError ret = wxProcess::Kill(m_Pid, wxSIGTERM);
    switch (ret)
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

int CompilerGCC::CompileFile(const wxString& file)
{
	DoPrepareQueue();

    if (CompilerFactory::Compilers[m_CompilerIdx]->GetSwitches().buildMethod == cbmUseMake)
    {
        wxFileName f(file);
        wxString fname = UnixFilename(f.GetPath() + ".objs/" + f.GetFullName());
        MakefileGenerator mg(this, 0, "", 0);
        mg.ConvertToMakefileFriendly(fname);
    
        wxString make = CompilerFactory::Compilers[m_CompilerIdx]->GetPrograms().MAKE;
        m_Queue.Add(make + " -f " + m_LastTempMakefile + " " + fname);
    }
    else
    {
        DirectCommands dc(this, CompilerFactory::Compilers[m_CompilerIdx], m_Project, m_PageIndex);
        ProjectFile* pf = m_Project->GetFileByFilename(file, true, false);
        if (!pf)
            return -1;
        ProjectBuildTarget* bt = m_Project->GetBuildTarget(pf->buildTargets[0]);
        if (!bt)
            return -2;
        wxArrayString compile = dc.GetCompileFileCommand(bt, pf);
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
    Compile();
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
        file.Assign(Manager::Get()->GetEditorManager()->GetActiveEditor()->GetFilename());
    }

    file.MakeRelativeTo(m_Project->GetFilename());
#ifdef ALWAYS_USE_MAKEFILE
	file.SetExt(OBJECT_EXT);
#endif
    wxString fname = file.GetFullPath();
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
        Configure(ftd->GetProject());
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
		m_TargetIndex = m_ToolTarget->GetSelection() - 1;
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
	cbEditor* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    wxMenuBar* mbar = Manager::Get()->GetAppWindow()->GetMenuBar();
    if (mbar)
    {		
        mbar->Enable(idMenuCompile, !m_Process && prj);
        mbar->Enable(idMenuCompileAll, !m_Process && prj);
        mbar->Enable(idMenuCompileFromProjectManager, !m_Process && prj);
        mbar->Enable(idMenuCompileTargetFromProjectManager, !m_Process && prj);
        mbar->Enable(idMenuCompileFile, !m_Process && prj && ed);
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

	// enable disable target selection combobox
	bool refreshTbar = false; // decide if we should refresh the toolbar
    if (m_ToolTarget)
	{
		bool en = !m_Process && prj;
		// refresh the toolbar if we change state of the combobox...
		refreshTbar = en != m_ToolTarget->IsEnabled();
        m_ToolTarget->Enable(en);
	}

	// enable/disable compiler toolbar buttons
	wxToolBar* tbar = Manager::Get()->GetAppWindow()->GetToolBar();
	if (tbar)
	{
		tbar->EnableTool(idMenuCompile, !m_Process && prj);
        tbar->EnableTool(idMenuRun, !m_Process && prj);
        tbar->EnableTool(idMenuCompileAndRun, !m_Process && prj);
        tbar->EnableTool(idMenuRebuild, !m_Process && prj);
		
		if (refreshTbar)
			tbar->Refresh();
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
        !msg.Matches("# ??*"))  // gcc 3.4 started displaying a line like this filter
                                // when calculating dependencies. Until I check out
                                // why this happens (and if there is a switch to
                                // turn it off), I put this condition here to avoid
                                // displaying it...
	{
        AddOutputLine(msg);
	}
}

static int errcnt = 0;

void CompilerGCC::OnGCCError(CodeBlocksEvent& event)
{
	++errcnt;
	if (errcnt > 50)
		return;
	wxString msg = event.GetString();
	AddOutputLine(msg, true);
}

void CompilerGCC::AddOutputLine(const wxString& output, bool forceErrorColor)
{
	Compiler* compiler = CompilerFactory::Compilers[m_CompilerIdx];
	Compiler::CompilerLineType clt = compiler->CheckForWarningsAndErrors(output);
	
	switch (clt)
	{
        case Compiler::cltWarning:
			m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(COLOUR_NAVY));
			break;
			
        case Compiler::cltError:
			m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxRED));
			break;
			
        default:
            if (forceErrorColor)
                m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(COLOUR_MAROON));
            else
                m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLACK));
			break;
	}

	if (clt != Compiler::cltNormal)
	{
        wxArrayString errors;
        errors.Add(compiler->GetLastErrorFilename());
        errors.Add(compiler->GetLastErrorLine());
        errors.Add(compiler->GetLastError());
        m_pListLog->AddLog(errors);
        
        m_Errors.AddError(compiler->GetLastErrorFilename(),
                          !compiler->GetLastErrorLine().IsEmpty() ? atoi(compiler->GetLastErrorLine().c_str()) : 0,
                          compiler->GetLastError(),
                          clt == Compiler::cltWarning);
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
	errcnt = 0;
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
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, " "); // blank line

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
                    prjMan->SetProject(projects->Item(++m_ProjectIndex));
                    CheckProject();
                    m_QueueIndex = 0;
                    if (CompilerFactory::Compilers[m_CompilerIdx]->GetSwitches().buildMethod == cbmUseMake)
                    {
                        wxString oldMK = m_LastTempMakefile;
                        DoCreateMakefile();
                        for (unsigned int i = 0; i < m_Queue.GetCount(); ++i)
                            m_Queue[i].Replace(oldMK, m_LastTempMakefile);
                        DoRunQueue();
                    }
                    else
                    {
                        m_Queue.Clear();
                        switch (m_DoAllProjects)
                        {
                            case mpjCompile: Compile(); break;
                            case mpjRebuild: Rebuild(); break;
                            default: break;
                        }
                    }
                }
                else if (m_BackupActiveProject)
                {
                    m_DoAllProjects = mpjNone;
                    m_QueueIndex = 0;
                    m_Queue.Clear();
                    prjMan->SetProject(m_BackupActiveProject);
                    AskForActiveProject();
                    DoDeleteTempMakefile();
                }
            }
            else
            {
                m_Queue.Clear();
                m_QueueIndex = 0;
                DoDeleteTempMakefile();
            }
        }
        else
        {
            m_DoAllProjects = mpjNone;
            m_Queue.Clear();
            m_QueueIndex = 0;
            if (m_Errors.GetCount())
            {
                Manager::Get()->GetMessageManager()->SwitchTo(m_ListPageIndex);
                DoGotoNextError();
            }
        }
        m_RunAfterCompile = false;
    }
}
