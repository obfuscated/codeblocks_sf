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

#include <cbeditor.h>
#include <customvars.h>
#include <annoyingdialog.h>
#include <wx/xrc/xmlres.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include "makefilegenerator.h"
#include "compileroptionsdlg.h"
#include "directcommands.h"

#include "compilerMINGW.h"
// TODO (mandrav#1#): Find out which compilers exist for linux and adapt this
#ifdef __WXMSW__
    #include "compilerMSVC.h"
    #include "compilerBCC.h"
    #include "compilerDMC.h"
    #include "compilerOW.h"
#endif
#include "compilerICC.h"
#include "compilerSDCC.h"

#define COLOUR_MAROON wxColour(0xa0, 0x00, 0x00)
#define COLOUR_NAVY   wxColour(0x00, 0x00, 0xa0)

CB_IMPLEMENT_PLUGIN(CompilerGCC);

// A simple dialog that wraps compiler's cbConfigurationPanel*
// Used for project build options...
class ProjectConfigureDlg : public wxDialog
{
	public:
		ProjectConfigureDlg(CompilerGCC* compiler, wxWindow* parent, cbProject* project, ProjectBuildTarget* target)
            : wxDialog(parent, wxID_ANY, _("Project build options"), wxDefaultPosition, wxDefaultSize)
		{
		    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
            m_pPanel = new CompilerOptionsDlg(this, compiler, project, target);
            bs->Add(m_pPanel, 1, wxGROW | wxRIGHT | wxTOP | wxBOTTOM, 8);

            wxStaticLine* line = new wxStaticLine(this);
            bs->Add(line, 0, wxGROW | wxLEFT | wxRIGHT, 8);

            m_pOK = new wxButton(this, wxID_OK, _("&OK"));
            m_pOK->SetDefault();
            m_pCancel = new wxButton(this, wxID_CANCEL, _("&Cancel"));
            wxStdDialogButtonSizer* but = new wxStdDialogButtonSizer;
            but->AddButton(m_pOK);
            but->AddButton(m_pCancel);
            but->Realize();
            bs->Add(but, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 8);

            SetSizer(bs);
            bs->SetSizeHints(this);
            CenterOnParent();
		}
		~ProjectConfigureDlg(){}

		void EndModal(int retCode)
		{
		    if (retCode == wxID_OK)
                m_pPanel->OnApply();
            else
                m_pPanel->OnCancel();
            wxDialog::EndModal(retCode);
		}
	protected:
        cbConfigurationPanel* m_pPanel;
        wxButton* m_pOK;
        wxButton* m_pCancel;
	private:

};

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
    EVT_MENU(idMenuCleanAll,                        CompilerGCC::OnCleanAll)
    EVT_MENU(idMenuCleanFromProjectManager,         CompilerGCC::OnClean)
    EVT_MENU(idMenuCleanTargetFromProjectManager,   CompilerGCC::OnClean)
    EVT_MENU(idMenuKillProcess,                     CompilerGCC::OnKillProcess)
	EVT_MENU(idMenuSelectTargetAll,					CompilerGCC::OnSelectTarget)
	EVT_MENU(idMenuNextError,						CompilerGCC::OnNextError)
	EVT_MENU(idMenuPreviousError,					CompilerGCC::OnPreviousError)
	EVT_MENU(idMenuClearErrors,						CompilerGCC::OnClearErrors)
    EVT_MENU(idMenuExportMakefile,                  CompilerGCC::OnExportMakefile)
    EVT_MENU(idMenuSettings,                        CompilerGCC::OnConfig)

	EVT_COMBOBOX(idToolTarget,						CompilerGCC::OnSelectTarget)

	EVT_PROJECT_ACTIVATE(CompilerGCC::OnProjectActivated)
	EVT_PROJECT_OPEN(CompilerGCC::OnProjectLoaded)
	//EVT_PROJECT_POPUP_MENU(CompilerGCC::OnProjectPopupMenu)
	EVT_PROJECT_TARGETS_MODIFIED(CompilerGCC::OnProjectActivated)

	EVT_PIPEDPROCESS_STDOUT(idGCCProcess, CompilerGCC::OnGCCOutput)
	EVT_PIPEDPROCESS_STDERR(idGCCProcess, CompilerGCC::OnGCCError)
	EVT_PIPEDPROCESS_TERMINATED(idGCCProcess, CompilerGCC::OnGCCTerminated)
END_EVENT_TABLE()

CompilerGCC::CompilerGCC()
    : m_BuildingWorkspace(false),
    m_CompilerIdx(-1),
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
	m_RunAfterCompile(false),
	m_LastExitCode(0),
	m_HasTargetAll(false),
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

    m_timerIdleWakeUp.SetOwner(this, idTimerPollCompiler);

	for (int i = 0; i < MAX_TARGETS; ++i)
		idMenuSelectTargetOther[i] = wxNewId();
#ifndef __WXMSW__
	m_ConsoleTerm = Manager::Get()->GetConfigManager(_T("compiler"))->Read(_T("/console_terminal"), DEFAULT_CONSOLE_TERM);
	m_ConsoleShell = Manager::Get()->GetConfigManager(_T("compiler"))->Read(_T("/console_shell"), DEFAULT_CONSOLE_SHELL);
    // because in previous versions the value for terminal
    // used to be "console_shell" (incorrectly), double-check that
    // the word "term" or "onsol" doesn't appear in "shell"
    if (m_ConsoleShell.Contains(_T("term")) || m_ConsoleShell.Contains(_T("onsol")))
        m_ConsoleShell = DEFAULT_CONSOLE_SHELL;
#endif

	// register built-in compilers
	CompilerFactory::RegisterCompiler(new CompilerMINGW);
#ifdef __WXMSW__
	CompilerFactory::RegisterCompiler(new CompilerMSVC);
	CompilerFactory::RegisterCompiler(new CompilerBCC);
	CompilerFactory::RegisterCompiler(new CompilerDMC);
	CompilerFactory::RegisterCompiler(new CompilerOW);
#endif
	CompilerFactory::RegisterCompiler(new CompilerICC);
	CompilerFactory::RegisterCompiler(new CompilerSDCC);

	// register (if any) user-copies of built-in compilers
	CompilerFactory::RegisterUserCompilers();
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
    m_Log = new SimpleTextLog();
    m_Log->GetTextControl()->SetFont(font);
    m_PageIndex = msgMan->AddLog(m_Log, _("Build log"));

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
    m_pListLog->GetListControl()->SetFont(font);
	m_ListPageIndex = msgMan->AddLog(m_pListLog, _("Build messages"));

    // set log image
    bmp.LoadFile(prefix + _T("flag_16x16.png"), wxBITMAP_TYPE_PNG);
    Manager::Get()->GetMessageManager()->SetLogImage(m_pListLog, bmp);

    // set default compiler for new projects
    CompilerFactory::SetDefaultCompilerIndex(Manager::Get()->GetConfigManager(_T("compiler"))->ReadInt(_T("/default_compiler"), 0));
	LoadOptions();
	SetupEnvironment();
}

void CompilerGCC::OnRelease(bool appShutDown)
{
    DoDeleteTempMakefile();
	SaveOptions();
    Manager::Get()->GetConfigManager(_T("compiler"))->Write(_T("/default_compiler"), CompilerFactory::GetDefaultCompilerIndex());
	if (Manager::Get()->GetMessageManager())
	{
        Manager::Get()->GetMessageManager()->RemoveLog(m_Log);
        Manager::Get()->GetMessageManager()->RemoveLog(m_pListLog);
    }

	if (appShutDown)
		return; // no need to continue if app is shutting down

	DoClearTargetMenu();

//	if (m_Menu)
//	{
//		wxMenuBar* mBar = Manager::Get()->GetAppWindow()->GetMenuBar();
//		int pos = mBar->FindMenu(_("&Build"));
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
    ProjectConfigureDlg dlg(this, Manager::Get()->GetAppWindow(), project, target);
    if(dlg.ShowModal() == wxID_OK)
    {
        m_ConsoleTerm = Manager::Get()->GetConfigManager(_T("compiler"))->Read(_T("/console_terminal"), DEFAULT_CONSOLE_TERM);
        m_ConsoleShell = Manager::Get()->GetConfigManager(_T("compiler"))->Read(_T("/console_shell"), DEFAULT_CONSOLE_SHELL);
        SaveOptions();
        SetupEnvironment();
        Manager::Get()->GetMacrosManager()->Reset();
    }
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
    DoRecreateTargetMenu(); // make sure the tool target combo is up-to-date
    return true;
}

void CompilerGCC::SetupEnvironment()
{
    if (!CompilerFactory::CompilerIndexOK(m_CompilerIdx))
        return;

    m_EnvironmentMsg.Clear();

	wxString path;
	Manager::Get()->GetMessageManager()->DebugLog(_("Setting up compiler environment..."));

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
    m_Project = m_pBuildingProject
                ? m_pBuildingProject
                : Manager::Get()->GetProjectManager()->GetActiveProject();
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
//    ProjectBuildTarget* lastTarget = 0;
    ProjectBuildTarget* bt = m_pBuildingProject ? m_pBuildingProject->GetBuildTarget(m_BuildingTargetIdx) : 0;
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
            cmd.Remove(0, myTargetChange.Length());
            // using other compiler now: find it and set it
//            lastTarget = m_Project->GetBuildTarget(cmd);

//            if (bt)
//            {
//                m_Project->SetCurrentlyCompilingTarget(bt);
//                SwitchCompiler(bt->GetCompilerIndex());
//                // re-apply the env vars for this target
//                if (CompilerFactory::CompilerIndexOK(m_CompilerIdx))
//                    CompilerFactory::Compilers[m_CompilerIdx]->GetCustomVars().ApplyVarsToEnvironment();
//                m_Project->GetCustomVars().ApplyVarsToEnvironment();
//                bt->GetCustomVars().ApplyVarsToEnvironment();
//
//            }
//            else
//                msgMan->Log(m_PageIndex, _("Can't locate target '%s'!"), cmd.c_str());
        }
        else
        {
            // compiler command
            m_CommandQueue.Add(new CompilerCommand(cmd, wxEmptyString, m_pBuildingProject, bt));
        }
    }
}

int CompilerGCC::DoRunQueue()
{
    wxLogNull ln;

	// leave if already running
	if (m_Process)
		return -2;

    MessageManager* msgMan = Manager::Get()->GetMessageManager();
//    msgMan->SwitchTo(m_PageIndex);

    CompilerCommand* cmd = m_CommandQueue.Next();
    if (!cmd)
	{
	    while (1)
	    {
	        // keep switching build states until we have commands to run or reach end of states
            BuildStateManagement();
            cmd = m_CommandQueue.Next();
            if (!cmd && m_BuildState == bsNone && m_NextBuildState == bsNone)
            {
                m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLUE, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
                msgMan->Log(m_PageIndex, _("Nothing to be done."));
                msgMan->LogToStdOut(_("Nothing to be done.\n"));
                m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
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
		wxSetEnv(_T("LD_LIBRARY_PATH"), dir + _T(":$LD_LIBRARY_PATH"));
    #endif
	}

    // special shell used only for build commands
    if (!cmd->isRun)
    {
    #ifndef __WXMSW__
        // run the command in a shell, so backtick'd expressions can be evaluated
        cmd->command = GetConsoleShell() + _T(" '") + cmd->command + _T("'");
//    #else
//    // TODO (mandrav#1#): Check windows version and substitute cmd.exe with command.com if needed.
//        cmd->command = _T("cmd /c ") + cmd->command;
    #endif
    }

    m_Process = new PipedProcess((void**)&m_Process, this, idGCCProcess, pipe, dir);
    m_Pid = wxExecute(cmd->command, flags, m_Process);
    if ( !m_Pid )
    {
        m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxRED, *wxWHITE));
        wxString err = wxString::Format(_("Execution of '%s' in '%s' failed."), cmd->command.c_str(), wxGetCwd().c_str());
        msgMan->Log(m_PageIndex, err);
        msgMan->LogToStdOut(err + _T('\n'));
		m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
        delete m_Process;
        m_Process = NULL;
        m_CommandQueue.Clear();
        ResetBuildState();
    }
    else
        m_timerIdleWakeUp.Start(100);

    delete cmd;
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

    SwitchCompiler(m_Project->GetCompilerIndex());
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
    int idx = m_Project->GetCompilerIndex();
    if (CompilerFactory::CompilerIndexOK(idx))
    {
        if (m_Project->IsMakefileCustom())
            return true;
        else
        {
            if (CompilerFactory::Compilers[idx]->GetSwitches().buildMethod == cbmUseMake)
            {
                // since November 28 2005, "make" is no more a valid build method
                // except if the project is set to use a custom Makefile
                // (and is selected automatically, in this case).
                // just notify the user once about this change
                if (!Manager::Get()->GetConfigManager(_T("compiler"))->Exists(_T("/notify_no_make")))
                {
                    Manager::Get()->GetConfigManager(_T("compiler"))->Set(_T("/notify_no_make"));
                    wxMessageBox(_("Code::Blocks no longer supports the \"GNU make\" build method, "
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
}

int CompilerGCC::RunSingleFile(const wxString& filename)
{
    wxFileName fname(filename);
    m_CdRun = fname.GetPath();
    fname.SetExt(EXECUTABLE_EXT);
    wxString exe_filename = fname.GetFullPath();
    wxString cmd;

#ifndef __WXMSW__
    // for non-win platforms, use m_ConsoleTerm to run the console app
    wxString shell = m_ConsoleTerm;
    shell.Replace(_T("$TITLE"), _T("'") + exe_filename + _T("'"));
    cmd << shell << _T(" ");
#endif
    wxString baseDir = ConfigManager::GetExecutableFolder();
#ifdef __WXMSW__
	#define CONSOLE_RUNNER "console_runner.exe"
#else
	#define CONSOLE_RUNNER "console_runner"
#endif
    if (wxFileExists(baseDir + wxT("/" CONSOLE_RUNNER)))
        cmd << baseDir << wxT("/" CONSOLE_RUNNER " ");

    cmd << _T("\"");
    cmd << exe_filename;
    cmd << _T("\"");

    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Checking for existence: %s"), exe_filename.c_str());
    if (!wxFileExists(exe_filename))
    {
    	int ret = wxMessageBox(_("It seems that this file has not been built yet.\n"
                                "Do you want to build it now?"),
                                _("Information"),
                                wxYES | wxNO | wxCANCEL | wxICON_QUESTION);
        switch (ret)
        {
        	case wxYES:
        	{
        	    m_RunAfterCompile = true;
        		Build(wxEmptyString);
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

    wxString out = UnixFilename(target->GetOutputFilename());
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out);

    wxString cmd;
    wxFileName f(out);
    f.MakeAbsolute(m_Project->GetBasePath());
//    m_CdRun = f.GetPath(wxPATH_GET_VOLUME);
    wxFileName cd(target->GetWorkingDir());
    if (cd.IsRelative())
        cd.MakeAbsolute(m_Project->GetBasePath());
    m_CdRun = cd.GetFullPath();

    // for console projects, use helper app to wait for a key after
    // execution ends...
	if (target->GetTargetType() == ttConsoleOnly)
	{
#ifndef __WXMSW__
        // for non-win platforms, use m_ConsoleTerm to run the console app
        wxString shell = m_ConsoleTerm;
        shell.Replace(_T("$TITLE"), _T("'") + m_Project->GetTitle() + _T("'"));
        cmd << shell << _T(" ");
#endif
        // should console runner be used?
        if (target->GetUseConsoleRunner())
        {
            wxString baseDir = ConfigManager::GetExecutableFolder();
#ifdef __WXMSW__
	#define CONSOLE_RUNNER "console_runner.exe"
#else
	#define CONSOLE_RUNNER "console_runner"
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

    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Checking for existence: %s"), f.GetFullPath().c_str());
    if (!wxFileExists(f.GetFullPath()))
    {
    	int ret = wxMessageBox(_("It seems that this project has not been built yet.\n"
                                "Do you want to build it now?"),
                                _("Information"),
                                wxYES | wxNO | wxCANCEL | wxICON_QUESTION);
        switch (ret)
        {
        	case wxYES:
        	{
        	    m_RunAfterCompile = true;
        		Build(target);
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
    m_CommandQueue.Add(new CompilerCommand(cmd, wxEmptyString, m_Project, target, true));
//    m_RunCmd = cmd;
//    m_RunAfterCompile = true;
    return 0;
}

wxString CompilerGCC::GetMakeCommandFor(MakeCommand cmd, ProjectBuildTarget* target)
{
    if (!m_Project)
        return wxEmptyString;
    wxString command = target ? target->GetMakeCommandFor(cmd) : m_Project->GetMakeCommandFor(cmd);
    command.Replace(_T("$makefile"), m_Project->GetMakefile());
    command.Replace(_T("$make"), CompilerFactory::Compilers[m_CompilerIdx]->GetPrograms().MAKE);
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

//	Manager::Get()->GetMacrosManager()->Reset();

    if (m_Project)
        wxSetWorkingDirectory(m_Project->GetBasePath());
    m_Generator.Init(m_Project);

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
            DirectCommands dc(this, &m_Generator, CompilerFactory::Compilers[m_CompilerIdx], m_Project, m_PageIndex);
            clean = dc.GetCleanCommands(target, true);
            Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Cleaning %s..."), target ? target->GetTitle().c_str() : m_Project->GetTitle().c_str());
        }
        else
        {
            DirectCommands dc(this, &m_Generator, CompilerFactory::GetDefaultCompiler(), 0, m_PageIndex);
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

void CompilerGCC::OnExportMakefile(wxCommandEvent& event)
{
    wxMessageBox(_("This functionality has been temporarily removed from Code::Blocks.\n"
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
    wxMessageBox(msg);
}

void CompilerGCC::InitBuildState(BuildJob job, const wxString& target)
{
    wxString ftgt = target.IsEmpty() ? m_LastTargetName : target;
    m_BuildJob = job;
    m_BuildStateTargetIsAll = ftgt.IsEmpty() || ftgt.Lower() == _("all");
    m_BuildState = bsNone;
    m_NextBuildState = !m_BuildStateTargetIsAll ? bsTargetPreBuild : bsProjectPreBuild;
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
        //                    BuildStateManagement();
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
            SwitchCompiler(bt->GetCompilerIndex());

        bool hasLogged = m_Log->GetTextControl()->GetInsertionPoint() != 0;
//        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("*****> m_BuildState=%s, m_NextBuildState=%s, m_pBuildingProject=%p, bt=%p (%p)"), StateToString(m_BuildState).c_str(), StateToString(m_NextBuildState).c_str(), m_pBuildingProject, bt, m_pLastBuildingTarget);
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
            m_Generator.Init(m_pBuildingProject);
        }
        if (bt != m_pLastBuildingTarget)
            m_pLastBuildingTarget = bt;
    }

    m_pBuildingProject->SetCurrentlyCompilingTarget(bt);
    DirectCommands dc(this, &m_Generator, CompilerFactory::Compilers[m_CompilerIdx], m_pBuildingProject, m_PageIndex);
    dc.m_doYield = true;

//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("*****> m_BuildState=%s, m_NextBuildState=%s, m_pBuildingProject=%p, bt=%p"), StateToString(m_BuildState).c_str(), StateToString(m_NextBuildState).c_str(), m_pBuildingProject, bt);
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
    if (m_BuildingTargetIdx == -1 && m_HasTargetAll)
    {
        m_BuildingTargetIdx = 0;
        bt =  prj->GetBuildTarget(0);
    }

    if (!bt || !CompilerValid(bt))
        return -2;

//	Manager::Get()->GetMacrosManager()->Reset();

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
    if (!CheckProject())
    {
        // no active project
        if (Manager::Get()->GetEditorManager()->GetActiveEditor())
            return CompileFile(Manager::Get()->GetEditorManager()->GetActiveEditor()->GetFilename());
        return -1;
    }
    DoClearErrors();
	DoPrepareQueue();

    InitBuildState(bjProject, target);
    CalculateProjectDependencies(m_Project);
    m_BuildingProjectIdx = m_BuildDeps[m_BuildDepsIndex];
    m_pBuildingProject = Manager::Get()->GetProjectManager()->GetProjects()->Item(m_BuildingProjectIdx);
    if (m_BuildDeps.GetCount() > 1)
        m_BuildJob = bjWorkspace;
    if (DoBuild(m_pBuildingProject))
        return -2;
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
    // make sure all project files are saved
    if (m_Project && !m_Project->SaveAllFiles())
        Manager::Get()->GetMessageManager()->Log(_("Could not save all files..."));

	DoPrepareQueue();
	if (!CompilerValid(target))
		return -1;

//	Manager::Get()->GetMacrosManager()->Reset();

    m_Generator.Init(m_Project);

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
    DoPrepareQueue();
    ClearLog();

    InitBuildState(bjWorkspace, target);
    CalculateWorkspaceDependencies();
    m_BuildingProjectIdx = m_BuildDeps[m_BuildDepsIndex];
    m_pBuildingProject = Manager::Get()->GetProjectManager()->GetProjects()->Item(m_BuildingProjectIdx);
    if (!m_pBuildingProject)
        return -1;
    DoBuild(m_pBuildingProject);
    return DoRunQueue();
}

int CompilerGCC::RebuildWorkspace(const wxString& target)
{
    int ret = CleanWorkspace(target);
    if (ret != 0)
        return ret;
    ResetBuildState();
    return BuildWorkspace(target);
}

int CompilerGCC::CleanWorkspace(const wxString& target)
{
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
    if (!m_Process || !m_Pid)
        return -1;
    wxKillError ret;
    bool isdirect=(!UseMake());

    m_CommandQueue.Clear();

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

bool CompilerGCC::IsRunning() const
{
    return m_BuildJob != bjIdle || m_Process || m_CommandQueue.GetCount();
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
        SwitchCompiler(CompilerFactory::GetDefaultCompilerIndex());
//        Manager::Get()->GetMessageManager()->DebugLog("-----CompileFile [if(!pf)]-----");
		Manager::Get()->GetMacrosManager()->Reset();
        m_Generator.Init(0);

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
            DirectCommands dc(this, &m_Generator, CompilerFactory::GetDefaultCompiler(), 0, m_PageIndex);
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
//        if (CompilerFactory::CompilerIndexOK(m_CompilerIdx))
//            CompilerFactory::Compilers[m_CompilerIdx]->GetCustomVars().ApplyVarsToEnvironment();
//        m_Project->GetCustomVars().ApplyVarsToEnvironment();
        m_Generator.Init(m_Project);

        DirectCommands dc(this, &m_Generator, CompilerFactory::Compilers[bt->GetCompilerIndex()], m_Project, m_PageIndex);
        wxArrayString compile = dc.CompileFile(bt, pf);
        AddToCommandQueue(compile);
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
    AnnoyingDialog dlg(_("Clean workspace"),
                        _("Cleaning ALL the open projects will cause the deletion of all "
                        "object files and building them from scratch.\nThis action "
                        "might take a while, especially if your projects contain "
                        "more than a few files.\nAnother factor is your CPU "
                        "and the available system memory.\n\n"
                        "Are you sure you want to clean ALL the projects?"),
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

void CompilerGCC::AddOutputLine(const wxString& output, bool forceErrorColor)
{
    Manager::Get()->GetMessageManager()->LogToStdOut(output + _T('\n'));

    size_t maxErrors = Manager::Get()->GetConfigManager(_T("compiler"))->ReadInt(_T("/max_reported_errors"), 50);
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
                m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)));
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

    if (m_CommandQueue.GetCount() != 0 && m_LastExitCode == 0)
    {
        // continue running commands while last exit code was 0.
        DoRunQueue();
    }
    else
    {
        if (m_LastExitCode == 0)
        {
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
        m_Log->GetTextControl()->SetDefaultStyle(m_LastExitCode == 0 ? wxTextAttr(*wxBLUE) : wxTextAttr(*wxRED));
        wxString msg = wxString::Format(_("Process terminated with status %d (%d minutes, %d seconds)"), m_LastExitCode, mins, secs);
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, msg);
        Manager::Get()->GetMessageManager()->LogToStdOut(_T('\n') + msg + _T('\n'));
        if (!m_CommandQueue.LastCommandWasRun())
        {
            m_Log->GetTextControl()->SetDefaultStyle(wxTextAttr(COLOUR_NAVY));
            wxString msg = wxString::Format(_("%d errors, %d warnings"), m_Errors.GetErrorsCount(), m_Errors.GetWarningsCount());
            Manager::Get()->GetMessageManager()->Log(m_PageIndex, msg);
            Manager::Get()->GetMessageManager()->LogToStdOut(msg + _T('\n'));
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

        if (m_Errors.GetErrorsCount())
        {
            Manager::Get()->GetMessageManager()->Open();
            Manager::Get()->GetMessageManager()->SwitchTo(m_ListPageIndex);
            DoGotoNextError();
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
                // if message manager is auto-hiding, this will unlock it
                Manager::Get()->GetMessageManager()->Close(true);
            }
        }
        m_RunAfterCompile = false;
    }
}
