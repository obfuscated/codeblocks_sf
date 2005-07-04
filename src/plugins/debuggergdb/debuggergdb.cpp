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

#include <wx/txtstrm.h>
#include <wx/regex.h>
#include <wx/dialog.h>
#include <wx/msgdlg.h>

#include <manager.h>
#include <configmanager.h>
#include <messagemanager.h>
#include <projectmanager.h>
#include <pluginmanager.h>
#include <editormanager.h>
#include <projectbuildtarget.h>
#include <sdk_events.h>
#include <editarraystringdlg.h>
#include <compilerfactory.h>
#include <licenses.h>
#include <xtra_res.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>

#include "debuggergdb.h"
#include "debuggeroptionsdlg.h"

#ifdef __WXMSW__
    #include <winbase.h> //For GetShortPathName()...only for windows systems
#endif

#define USE_DEBUG_LOG 0 // set it to 1, to enable the debugger's debug log

#define implement_debugger_toolbar

// valid debugger command constants
#define CMD_CONTINUE    1
#define CMD_STEP        2
#define CMD_STEPIN      3
#define CMD_STOP        4
#define CMD_BACKTRACE   5
#define CMD_DISASSEMBLE 6

static const wxString g_EscapeChars = char(26);

int idMenuDebug = XRCID("idDebuggerMenuDebug");
int idMenuRunToCursor = XRCID("idDebuggerMenuRunToCursor");
int idMenuNext = XRCID("idDebuggerMenuNext");
int idMenuStep = XRCID("idDebuggerMenuStep");
int idMenuStepOut = XRCID("idDebuggerMenuStepOut");
int idMenuStop = XRCID("idDebuggerMenuStop");
int idMenuContinue = XRCID("idDebuggerMenuContinue");
int idMenuToggleBreakpoint = XRCID("idDebuggerMenuToggleBreakpoint");
int idMenuSendCommandToGDB = XRCID("idDebuggerMenuSendCommandToGDB");
int idMenuAddSymbolFile = XRCID("idDebuggerMenuAddSymbolFile");
int idMenuDisassemble = XRCID("idDebuggerMenuDisassemble");
int idMenuBacktrace = XRCID("idDebuggerMenuBacktrace");
int idMenuEditWatches = XRCID("idDebuggerMenuEditWatches");

int idGDBProcess = wxNewId();
int idTimerPollDebugger = wxNewId();
int idMenuDebuggerAddWatch = wxNewId();

cbPlugin* GetPlugin()
{
	return new DebuggerGDB;
}

BEGIN_EVENT_TABLE(DebuggerGDB, cbDebuggerPlugin)
	EVT_UPDATE_UI_RANGE(idMenuContinue, idMenuDebuggerAddWatch, DebuggerGDB::OnUpdateUI)
	// these are different because they are loaded from the XRC
	EVT_UPDATE_UI(XRCID("idDebuggerMenuDebug"), DebuggerGDB::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("idDebuggerMenuRunToCursor"), DebuggerGDB::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("idDebuggerMenuNext"), DebuggerGDB::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("idDebuggerMenuStep"), DebuggerGDB::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("idDebuggerMenuStepOut"), DebuggerGDB::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("idDebuggerMenuStop"), DebuggerGDB::OnUpdateUI)

	EVT_MENU(idMenuDebug, DebuggerGDB::OnDebug)
	EVT_MENU(idMenuContinue, DebuggerGDB::OnContinue)
	EVT_MENU(idMenuNext, DebuggerGDB::OnNext)
	EVT_MENU(idMenuStep, DebuggerGDB::OnStep)
	EVT_MENU(idMenuStepOut, DebuggerGDB::OnStepOut)
	EVT_MENU(idMenuToggleBreakpoint, DebuggerGDB::OnToggleBreakpoint)
	EVT_MENU(idMenuRunToCursor, DebuggerGDB::OnRunToCursor)
	EVT_MENU(idMenuStop, DebuggerGDB::OnStop)
	EVT_MENU(idMenuSendCommandToGDB, DebuggerGDB::OnSendCommandToGDB)
	EVT_MENU(idMenuAddSymbolFile, DebuggerGDB::OnAddSymbolFile)
	EVT_MENU(idMenuBacktrace, DebuggerGDB::OnBacktrace)
	EVT_MENU(idMenuDisassemble, DebuggerGDB::OnDisassemble)
	EVT_MENU(idMenuEditWatches, DebuggerGDB::OnEditWatches)
    EVT_MENU(idMenuDebuggerAddWatch, DebuggerGDB::OnAddWatch)
	
	EVT_EDITOR_BREAKPOINT_ADDED(DebuggerGDB::OnBreakpointAdded)
	EVT_EDITOR_BREAKPOINT_DELETED(DebuggerGDB::OnBreakpointDeleted)
	EVT_EDITOR_TOOLTIP(DebuggerGDB::OnValueTooltip)
	
	EVT_PIPEDPROCESS_STDOUT(idGDBProcess, DebuggerGDB::OnGDBOutput)
	EVT_PIPEDPROCESS_STDERR(idGDBProcess, DebuggerGDB::OnGDBError)
	EVT_PIPEDPROCESS_TERMINATED(idGDBProcess, DebuggerGDB::OnGDBTerminated)
	
	EVT_IDLE(DebuggerGDB::OnIdle)
	EVT_TIMER(idTimerPollDebugger, DebuggerGDB::OnTimer)
	
	EVT_COMMAND(-1, cbCustom_WATCHES_CHANGED, DebuggerGDB::OnWatchesChanged)
END_EVENT_TABLE()

DebuggerGDB::DebuggerGDB()
	: m_pMenu(0L),
	m_pLog(0L),
	m_pDbgLog(0L),
	m_pProcess(0L),
	m_PageIndex(-1),
	m_DbgPageIndex(-1),
	m_ProgramIsStopped(true),
	m_pCompiler(0L),
	m_LastExitCode(0),
	m_TargetIndex(-1),
	m_Pid(0),
	m_EvalWin(0L),
	m_pTree(0L),
	m_NoDebugInfo(false),
	m_BreakOnEntry(false),
	m_HaltAtLine(0),
	m_pDisassembly(0),
	m_pBacktrace(0)
{
    Manager::Get()->Loadxrc("/debugger_gdb.zip#zip:*.xrc");

	m_PluginInfo.name = "DebuggerGDB";
	m_PluginInfo.title = "GDB Debugger";
	m_PluginInfo.version = "0.1";
	m_PluginInfo.description = "Plugin that interfaces the GNU GDB debugger.";
    m_PluginInfo.author = "Yiannis An. Mandravellos";
    m_PluginInfo.authorEmail = "info@codeblocks.org";
    m_PluginInfo.authorWebsite = "www.codeblocks.org";
	m_PluginInfo.thanksTo = "";
	m_PluginInfo.license = LICENSE_GPL;
	m_PluginInfo.hasConfigure = true;
	
	m_TimerPollDebugger.SetOwner(this, idTimerPollDebugger);

	ConfigManager::AddConfiguration(m_PluginInfo.title, "/debugger_gdb");
}

void DebuggerGDB::OnAttach()
{
    MessageManager* msgMan = Manager::Get()->GetMessageManager();
    wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
    m_pLog = new SimpleTextLog(msgMan, m_PluginInfo.title);
    m_pLog->GetTextControl()->SetFont(font);
    m_PageIndex = msgMan->AddLog(m_pLog);
    // set log image
	wxBitmap bmp;
	wxString prefix = ConfigManager::Get()->Read("data_path") + "/images/";
    bmp.LoadFile(prefix + "misc_16x16.png", wxBITMAP_TYPE_PNG);
    Manager::Get()->GetMessageManager()->SetLogImage(m_pLog, bmp);

#if USE_DEBUG_LOG
    m_pDbgLog = new SimpleTextLog(msgMan, m_PluginInfo.title + _(" (debug)"));
    m_pDbgLog->GetTextControl()->SetFont(font);
    m_DbgPageIndex = msgMan->AddLog(m_pDbgLog);
    // set log image
    bmp.LoadFile(prefix + "contents_16x16.png", wxBITMAP_TYPE_PNG);
    Manager::Get()->GetMessageManager()->SetLogImage(m_pDbgLog, bmp);
#endif

	if (!m_pTree)
		m_pTree = new DebuggerTree(this, Manager::Get()->GetNotebook());
}

void DebuggerGDB::OnRelease(bool appShutDown)
{
    if (m_pDisassembly)
        m_pDisassembly->Destroy();
    m_pDisassembly = 0;

    if (m_pBacktrace)
        m_pBacktrace->Destroy();
    m_pBacktrace = 0;

	if (m_pTree)
	{
		delete m_pTree;
		m_pTree = 0L;
	}
    
    //Close debug session when appShutDown
	CmdStop();

    if (Manager::Get()->GetMessageManager())
    {
#if USE_DEBUG_LOG
        Manager::Get()->GetMessageManager()->DeletePage(m_DbgPageIndex);
#endif
        Manager::Get()->GetMessageManager()->DeletePage(m_PageIndex);
    }
}

DebuggerGDB::~DebuggerGDB()
{
}

int DebuggerGDB::Configure()
{
	DebuggerOptionsDlg dlg(Manager::Get()->GetAppWindow());
	return dlg.ShowModal();
}

void DebuggerGDB::BuildMenu(wxMenuBar* menuBar)
{
	if (!m_IsAttached)
		return;
    m_pMenu=Manager::Get()->LoadMenu("debugger_menu",true);

	// ok, now, where do we insert?
	// three possibilities here:
	// a) locate "Compile" menu and insert after it
	// b) locate "Project" menu and insert after it
	// c) if not found (?), insert at pos 5
	int finalPos = 5;
	int projcompMenuPos = menuBar->FindMenu("Compile");
	if (projcompMenuPos != wxNOT_FOUND)
		finalPos = projcompMenuPos + 1;
	else
	{
		projcompMenuPos = menuBar->FindMenu("Project");
		if (projcompMenuPos != wxNOT_FOUND)
			finalPos = projcompMenuPos + 1;
	}
    menuBar->Insert(finalPos, m_pMenu, _("&Debug"));
}

void DebuggerGDB::BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg)
{
	if (!m_IsAttached)
		return;
    // we 're only interested in editor menus
    // we 'll add a "debug watches" entry only when the debugger is running...
    if (type != mtEditorManager || !menu) return;
    // Insert toggle breakpoint
    menu->Insert(0,idMenuToggleBreakpoint, _("Toggle breakpoint"));
	// Insert Run to Cursor
	menu->Insert(1,idMenuRunToCursor, _("Run to cursor"));
	menu->Insert(2,-1, "-");
    
    if (!m_pProcess) return;
    // has to have a word under the caret...
    wxString w = GetEditorWordAtCaret();
    if (w.IsEmpty())
        return;

    wxString s;
    s.Printf(_("Watch '%s'"), w.c_str());
	menu->Insert(2, idMenuDebuggerAddWatch,  s);
}

void DebuggerGDB::BuildToolBar(wxToolBar* toolBar)
{
    /* Loads toolbar using new Manager class functions */
#ifdef implement_debugger_toolbar
    if (!m_IsAttached)
		return;
    wxString my_16x16=Manager::isToolBar16x16(toolBar) ? "_16x16" : "";
    Manager::AddonToolBar(toolBar,wxString("debugger_toolbar")+my_16x16);
    toolBar->Realize();
#endif    
}

void DebuggerGDB::DoWatches()
{
	wxString info;
	if (m_pProcess)
	{
        if (ConfigManager::Get()->Read("debugger_gdb/watch_args", 0L))
            info << "Function Arguments = {" << GetInfoFor("info args") << "}" << '\n';
        if (ConfigManager::Get()->Read("debugger_gdb/watch_locals", 0L))
    		info << "Local variables = {" << GetInfoFor("info locals") << "}" << '\n';
		for (unsigned int i = 0; i < m_pTree->GetWatches().GetCount(); ++i)
		{
			wxString watch = m_pTree->GetWatches()[i];
			info << watch << "{" << GetInfoFor("output " + watch) << "}" << '\n';
		}
	}
	else
	{
		// since no debugging session is active, we might as well show
		// the not-evaluated user-watches, just for feedback ;)
		for (unsigned int i = 0; i < m_pTree->GetWatches().GetCount(); ++i)
		{
			info << m_pTree->GetWatches()[i] << ',';
		}
	}
	//m_pLog->AddLog(info);
	m_pTree->BuildTree(info);
}

void DebuggerGDB::SetBreakpoints()
{
	SendCommand("delete"); // clear all breakpoints
	
	cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
	if (prj)
	{
		for (int i = 0; i < prj->GetFilesCount(); ++i)
		{
			ProjectFile* pf = prj->GetFile(i);

			for (unsigned int x = 0; x < pf->breakpoints.GetCount(); ++x)
			{
				DebuggerBreakpoint* bp = pf->breakpoints[x];
				wxString filename = pf->file.GetFullName();
				wxString cmd;
				if (bp->enabled)
                {
					if (bp->func.IsEmpty())
					{
                        cmd << "break " << filename << ":" << bp->line + 1;
                        SendCommand(cmd);
                    }
                    //GDB workaround
                    //Use function name if this is C++ constructor/destructor						
					else
					{
						cmd << "break " << bp->func;
						GetInfoFor(cmd);
					}
                    //end GDB workaround					
				}
				//SendCommand(cmd);
			}
		}
	}
}

int DebuggerGDB::Debug()
{
	if (m_pProcess)
		return 1;
    m_NoDebugInfo = false;

	ProjectManager* prjMan = Manager::Get()->GetProjectManager();
	cbProject* project = prjMan->GetActiveProject();
	if (!project)
		return 2;

    MessageManager* msgMan = Manager::Get()->GetMessageManager();
    msgMan->SwitchTo(m_PageIndex);
	m_pLog->GetTextControl()->Clear();


    m_TargetIndex = project->GetActiveBuildTarget();
    msgMan->SwitchTo(m_PageIndex);
	msgMan->AppendLog(m_PageIndex, _("Selecting target: "));
	if (m_TargetIndex == -1)
	{
        m_TargetIndex = project->SelectTarget(m_TargetIndex);
        if (m_TargetIndex == -1)
        {
            msgMan->Log(m_PageIndex, _("canceled"));
            return 3;
        }
	}
	ProjectBuildTarget* target = project->GetBuildTarget(m_TargetIndex);
    if (target->GetTargetType() == ttCommandsOnly)
    {
        wxMessageBox(_("The selected target is only running pre/post build step commands\n"
                    "Can't debug such a target..."), _("Information"), wxICON_INFORMATION);
        msgMan->Log(m_PageIndex, _("aborted"));
        return 3;
    }
	msgMan->Log(m_PageIndex, target->GetTitle());

	Compiler* actualCompiler = CompilerFactory::Compilers[target ? target->GetCompilerIndex() : project->GetCompilerIndex()];
	if (!actualCompiler)
	{
		wxString msg;
		msg.Printf(_("This %s is configured to use an invalid debugger.\nThe operation failed..."), target ? _("target") : _("project"));
		wxMessageBox(msg, _("Error"), wxICON_ERROR);
		return 9;
	}

	if (actualCompiler->GetPrograms().DBG.IsEmpty() ||
        !wxFileExists(actualCompiler->GetMasterPath() + wxFileName::GetPathSeparator() + "bin" + wxFileName::GetPathSeparator() + actualCompiler->GetPrograms().DBG))
	{
        wxMessageBox(_("The debugger executable is not set.\n"
                    "To set it, go to \"Settings/Configure plugins/Compiler\", switch to the \"Programs\" tab\n"
                    "and select the debugger program."), _("Error"), wxICON_ERROR);
        msgMan->Log(m_PageIndex, _("Aborted"));
        return 4;
	}

	PluginsArray plugins = Manager::Get()->GetPluginManager()->GetCompilerOffers();
	if (plugins.GetCount())
		m_pCompiler = (cbCompilerPlugin*)plugins[0];
	if (m_pCompiler)
	{
		msgMan->AppendLog(m_PageIndex, _("Compiling: "));
		m_pCompiler->Compile(target);
		while (m_pCompiler->IsRunning())
			wxYield();
        msgMan->SwitchTo(m_PageIndex);
		if (m_pCompiler->GetExitCode() != 0)
		{
			msgMan->Log(m_PageIndex, _("failed"));
			msgMan->Log(m_PageIndex, _("Aborting debugging session"));
			return -1;
		}
		msgMan->Log(m_PageIndex, _("done"));
	}

	wxString cmdexe;
	cmdexe = actualCompiler->GetPrograms().DBG;
	cmdexe.Trim();
	cmdexe.Trim(true);
	if(cmdexe.IsEmpty())
    {
        msgMan->AppendLog(m_PageIndex,_("ERROR: You need to specify a debugger program in the compiler's settings."));
        #ifdef __WXMSW__
        msgMan->Log(m_PageIndex,_("\n(For MINGW compilers, it's 'gdb.exe' (without the quotes))"));
        #else
        msgMan->Log(m_PageIndex,_("\n(For GCC compilers, it's 'gdb' (without the quotes))"));
        #endif
        return -1;
    }

	wxString cmd;
	wxString sep = wxFileName::GetPathSeparator();
	cmd << actualCompiler->GetMasterPath() << sep << "bin" << sep << cmdexe << " -nw -annotate=2 -silent";
	
	wxLogNull ln; // we perform our own error handling and logging
    m_pProcess = new PipedProcess((void**)&m_pProcess, this, idGDBProcess, true, project->GetBasePath());
	msgMan->AppendLog(m_PageIndex, _("Starting debugger: "));
//    msgMan->AppendLog(m_PageIndex, _(cmd));
    m_Pid = wxExecute(cmd, wxEXEC_ASYNC, m_pProcess);
//    m_Pid = m_pProcess->Launch(cmd);

    if (!m_Pid)
    {
		delete m_pProcess;
		m_pProcess = 0;
		msgMan->Log(m_PageIndex, _("failed"));
        return -1;
    }
    else if (!m_pProcess->GetOutputStream())
    {
		delete m_pProcess;
		m_pProcess = 0;
		msgMan->Log(m_PageIndex, _("failed (to get debugger's stdin)"));
        return -2;
    }
    else if (!m_pProcess->GetInputStream())
    {
		delete m_pProcess;
		m_pProcess = 0;
		msgMan->Log(m_PageIndex, _("failed (to get debugger's stdout)"));
        return -2;
    }
    else if (!m_pProcess->GetErrorStream())
    {
		delete m_pProcess;
		m_pProcess = 0;
		msgMan->Log(m_PageIndex, _("failed (to get debugger's stderr)"));
        return -2;
    }
	else
		msgMan->Log(m_PageIndex, _("done"));
	
	wxString out;
	m_TimerPollDebugger.Start(100);

	// add as include dirs all open project base dirs
	ProjectsArray* projects = prjMan->GetProjects();
	for (unsigned int i = 0; i < projects->GetCount(); ++i)
	{
        cbProject* it = projects->Item(i);
//        if (it == project)
//            continue;
        wxString filename = it->GetBasePath();
        msgMan->Log(m_PageIndex, _("Adding source dir: %s"), filename.c_str());
        ConvertToGDBDirectory(filename, "", false);//project->GetBasePath(), true);
        SendCommand("directory " + filename);
	}
//    msgMan->Log(m_PageIndex, cmd);

	cmd.Clear();
	switch (target->GetTargetType())
	{
		case ttExecutable:
		case ttConsoleOnly:
			// "-async" option is not really supported, at least under Win32, as far as I know
			out = UnixFilename(target->GetOutputFilename());
			msgMan->Log(m_PageIndex, _("Adding file: %s"), out.c_str());
            ConvertToGDBDirectory(out);
			cmd << "file " << out;
			// dll debugging steps:
			// gdb <hostapp>
			// (gdb) add-symbol-file <dllname> (and any other dlls the same way)
			SendCommand(cmd);
			break;

		case ttStaticLib:
		case ttDynamicLib:
			// check for hostapp
			if (target->GetHostApplication().IsEmpty())
			{
				wxMessageBox(_("You must select a host application to \"run\" a library..."));
				CmdStop();
				return 4;
			}
			out = UnixFilename(target->GetHostApplication());
			msgMan->Log(m_PageIndex, _("Adding file: %s"), out.c_str());
			ConvertToGDBDirectory(out);
			cmd << "file " << out;
			SendCommand(cmd);
			if (target->GetTargetType() == ttDynamicLib)
			{
				wxString symbols;
				out = UnixFilename(target->GetOutputFilename());
				msgMan->Log(m_PageIndex, _("Adding symbol file: %s"), out.c_str());
                ConvertToGDBDirectory(out);
				symbols << "add-symbol-file " << out;
				SendCommand(symbols);
			}
			break;

        default: break;
	}

	if (!target->GetExecutionParameters().IsEmpty())
		SendCommand("set args " + target->GetExecutionParameters());

    // switch to output dir
	// wxFileName dir(target->GetOutputFilename());
	// wxString path = UnixFilename(dir.GetPath(wxPATH_GET_VOLUME));
    wxString path = UnixFilename(target->GetWorkingDir());
    if (!path.IsEmpty())
    {
        cmd.Clear();
        ConvertToGDBDirectory(path);
        if (path != _(".")) // avoid silly message "changing to ."
        {
            msgMan->Log(m_PageIndex, _("Changing directory to: %s"), path.c_str());
            cmd << "cd " << path;
            SendCommand(cmd);
        }
    }

	SetBreakpoints();
	if (!m_Tbreak.IsEmpty())
	{
		SendCommand(m_Tbreak);
		m_Tbreak.Clear();
	}

    // send built-in init commands
	SendCommand("set confirm off");
	if (target->GetTargetType() == ttConsoleOnly)
        SendCommand("set new-console on");
#ifndef __WXMSW__
    SendCommand("set disassembly-flavor att");
#else
    SendCommand("set disassembly-flavor intel");
#endif

    // pass user init-commands
    wxString init = ConfigManager::Get()->Read("debugger_gdb/init_commands", "");
    wxArrayString initCmds = GetArrayFromString(init, '\n');
    for (unsigned int i = 0; i < initCmds.GetCount(); ++i)
    {
        SendCommand(initCmds[i]);
    }

    // finally, run the process
    if (m_BreakOnEntry)
    {
    	m_BreakOnEntry = false;
    	SendCommand("start");
	}
	else
        SendCommand("run");
	return 0;
}

void DebuggerGDB::StripQuotes(wxString& str)
{
	if (str.GetChar(0) == '\"' && str.GetChar(str.Length() - 1) == '\"') 
			str = str.Mid(1, str.Length() - 2);
}

void DebuggerGDB::ConvertToGDBFriendly(wxString& str)
{
    if (str.IsEmpty())
        return;

    str = UnixFilename(str);
    while (str.Replace("\\", "/"))
        ;
    while (str.Replace("//", "/"))
        ;
//    str.Replace("/", "//");
    if (str.Find(' ') != -1 && str.GetChar(0) != '"')
        str = "\"" + str + "\"";
}

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
		int ColonLocation = str.Find(':');
		char buf[255];
		if(ColonLocation != -1)
		{
			//If can, get 8.3 name for path (Windows only)
			GetShortPathName(str.c_str(), buf, 255);
			str = buf;
		} 
		else if(!base.IsEmpty() && str.GetChar(0) != '/') 
		{
			if(base.GetChar(base.Length()) == '/') base = base.Mid(0, base.Length() - 2);
			while(!str.IsEmpty())
			{
				base += "/" + str.BeforeFirst('/');
				if(str.Find('/') != -1) str = str.AfterFirst('/');
				else str.Clear();
			}
			GetShortPathName(base.c_str(), buf, 255);
			str = buf;
		}
		
		if(ColonLocation == -1 || base.IsEmpty())
			relative = false;		//Can't do it
	#else
		if((str.GetChar(0) != '/' && str.GetChar(0) != '~') || base.IsEmpty())
			relative = false;	
	#endif
	
	if(relative)
	{
		#ifdef __WXMSW__
			if(str.Find(':') != -1) str = str.Mid(str.Find(':') + 2, str.Length());
			if(base.Find(':') != -1) base = base.Mid(base.Find(':') + 2, base.Length());
		#else
			if(str.GetChar(0) == '/') str = str.Mid(1, str.Length());
			else if(str.GetChar(0) == '~') str = str.Mid(2, str.Length());
			if(base.GetChar(0) == '/') base = base.Mid(1, base.Length());
			else if(base.GetChar(0) == '~') base = base.Mid(2, base.Length());
		#endif
		
		while(!base.IsEmpty() && !str.IsEmpty())
		{
			if(str.BeforeFirst('/') == base.BeforeFirst('/'))
			{
				if(str.Find('/') == -1) str.Clear();
				else str = str.AfterFirst('/');
				
				if(base.Find('/') == -1) base.Clear();
				else base = base.AfterFirst('/');
			}
			else break;
		}
		while (!base.IsEmpty())
		{
			str = "../" + str;
			if(base.Find('/') == -1) base.Clear();
			else base = base.AfterFirst('/');
		}
	}
	ConvertToGDBFriendly(str);
}

void DebuggerGDB::SendCommand(const wxString& cmd)
{
    if (!m_pProcess || !m_ProgramIsStopped)
        return;
#if USE_DEBUG_LOG
	Manager::Get()->GetMessageManager()->Log(m_DbgPageIndex, "> " + cmd);
#endif
	m_pProcess->SendString(cmd);
}

wxString DebuggerGDB::GetNextOutputLine(bool useStdErr)
{
	if (!m_pProcess)
		return wxEmptyString;

	wxString bufferOut;

	wxInputStream* m_pOut = useStdErr ? m_pProcess->GetErrorStream() : m_pProcess->GetInputStream();
	while (useStdErr ? true : m_pProcess->IsInputOpened() &&
			useStdErr ? m_pProcess->IsErrorAvailable() : m_pProcess->IsInputAvailable() &&
			!m_pOut->Eof())
	{
		char ch = m_pOut->GetC();
		if (ch == '\n' || ch == '\r')
		{
			while (useStdErr ? m_pProcess->IsErrorAvailable() : m_pProcess->IsInputAvailable() &&
					!m_pOut->Eof() &&
					(m_pOut->Peek() == '\n' || m_pOut->Peek() == '\r')
				)
				ch = m_pOut->GetC();
			break;
		}
		else
			bufferOut << ch;
	}

#if USE_DEBUG_LOG
	if (!bufferOut.IsEmpty())
		m_pDbgLog->AddLog(bufferOut);
#endif
	return bufferOut;
}

wxString DebuggerGDB::GetNextOutputLineClean(bool useStdErr)
{
	wxString line = GetNextOutputLine(useStdErr);
	while (line.IsEmpty() || line.StartsWith(g_EscapeChars))
		line = GetNextOutputLine(useStdErr);

	return line;
}

void DebuggerGDB::RunCommand(int cmd)
{
    switch (cmd)
    {
        case CMD_CONTINUE:
            ClearActiveMarkFromAllEditors();
            Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Continuing..."));
            SendCommand("cont");
            break;

        case CMD_STEP:
            ClearActiveMarkFromAllEditors();
            SendCommand("next");
            break;

        case CMD_STEPIN:
            ClearActiveMarkFromAllEditors();
            SendCommand("step");
            break;

        case CMD_STOP:
            ClearActiveMarkFromAllEditors();
            SendCommand("quit");
            break;

        case CMD_BACKTRACE:
//            Manager::Get()->GetMessageManager()->Log(m_PageIndex, "Running back-trace...");
            SendCommand("bt");
            break;

        case CMD_DISASSEMBLE:
//            Manager::Get()->GetMessageManager()->Log(m_PageIndex, "Disassembling...");
            SendCommand("disassemble");
            break;

        default: break;
    }
}

void DebuggerGDB::CmdDisassemble()
{
    if (!m_pDisassembly)
        m_pDisassembly = new DisassemblyDlg(Manager::Get()->GetAppWindow(), this);
    m_pDisassembly->Show();
    RunCommand(CMD_DISASSEMBLE);
}

void DebuggerGDB::CmdBacktrace()
{
    if (!m_pBacktrace)
        m_pBacktrace = new BacktraceDlg(Manager::Get()->GetAppWindow(), this);
    m_pBacktrace->Clear();
    m_pBacktrace->Show();
    RunCommand(CMD_BACKTRACE);
}

void DebuggerGDB::CmdContinue()
{
	SetBreakpoints();
	if (!m_Tbreak.IsEmpty())
	{
		SendCommand(m_Tbreak);
		m_Tbreak.Clear();
	}
    RunCommand(CMD_CONTINUE);
}

void DebuggerGDB::CmdNext()
{
    RunCommand(CMD_STEP);
}

void DebuggerGDB::CmdStep()
{
    RunCommand(CMD_STEPIN);
}

bool DebuggerGDB::Validate(const wxString& line, const char cb)
{
	bool bResult = false;
	
	int bep = line.Find(cb)+1;
	int scs = line.Find('\'')+1; 
	int sce = line.Find('\'',true)+1;
	int dcs = line.Find('"')+1;  
	int dce = line.Find('"',true)+1;
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

void DebuggerGDB::CmdStepOut()
{
	cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	if (!ed) return;
	ProjectFile* pf = ed->GetProjectFile();
	if (!pf) return;
	wxString filename = pf->file.GetFullName(), lineBuf, cmd;
	cbStyledTextCtrl* stc = ed->GetControl();
	int line = m_HaltAtLine;
	lineBuf = stc->GetLine(line);
	
	unsigned int nLevel = 1;
	while(nLevel){
		 if ((lineBuf.Find('{')+1) && Validate(lineBuf, '{') &&
			 (line > m_HaltAtLine)) nLevel++;
		 if ((lineBuf.Find('}')+1) && Validate(lineBuf, '}')) nLevel--;
		 if (nLevel) lineBuf = stc->GetLine(++line);
	}
	if (line == stc->GetCurrentLine())
		CmdNext();
	else {
		cmd << "tbreak " << filename << ":" << line+1;
		m_Tbreak = cmd;
		CmdContinue();
	}
}

void DebuggerGDB::CmdRunToCursor()
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	if (!ed)
		return;
	
	ProjectFile* pf = ed->GetProjectFile();
	if (!pf)
		return;
	wxString cmd, filename = pf->file.GetFullName();
	cmd << "tbreak " << filename << ":" << ed->GetControl()->GetCurrentLine()+1;
	m_Tbreak = cmd;
	if (m_pProcess)
	{
		CmdContinue();
	}
	else
	{
		Debug();
	}
}

void DebuggerGDB::CmdToggleBreakpoint()
{
	ClearActiveMarkFromAllEditors();
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	if (!ed)
		return;
	ed->MarkerToggle(BREAKPOINT_MARKER);
//	SetBreakpoints();
}

void DebuggerGDB::CmdStop()
{
	if (m_pProcess && m_Pid){ 
		m_pProcess->CloseOutput();
		if (m_ProgramIsStopped)	RunCommand(CMD_STOP);
		else {
			wxKillError err = m_pProcess->Kill(m_Pid, wxSIGKILL);
			if (err == wxKILL_OK){
/*				
				wxMessageBox(_("Debug session terminated!"),
					_("Debug"), wxOK | wxICON_EXCLAMATION);
*/					
			}
			m_ProgramIsStopped = true;
		}
	}
}

void DebuggerGDB::ParseOutput(const wxString& output)
{
	wxString buffer = output;
	if (buffer.StartsWith(g_EscapeChars)) // ->->
	{
		buffer.Remove(0, 2); // remove ->->
#if USE_DEBUG_LOG
		m_pDbgLog->AddLog(buffer); // write it in the full debugger log
#endif		
		// Is the program running?
		if (buffer.Matches("starting"))
			m_ProgramIsStopped = false;
		
		// Is the program stopped?
		else if (buffer.Matches("stopped"))
		{
			bool already = m_ProgramIsStopped;
			m_ProgramIsStopped = true;
			if (!already)
				DoWatches();
		}

		// Is the program exited?
		else if (buffer.StartsWith("exited "))
		{
			m_ProgramIsStopped = true;
			Manager::Get()->GetMessageManager()->Log(m_PageIndex, buffer);
			CmdStop();
		}

		// error
		else if (buffer.Matches("error"))
		{
			Manager::Get()->GetMessageManager()->Log(m_PageIndex, buffer);
			//CmdStop();
		}
		else if (buffer.StartsWith("error-begin"))
		{
            wxString error = GetNextOutputLineClean(true);
			Manager::Get()->GetMessageManager()->Log(m_PageIndex, error);
			if (error.StartsWith("No symbol table is loaded."))
                m_NoDebugInfo = true;
			//CmdStop();
		}

		// signal
		else if (buffer.Matches("signal-name"))
		{
			BringAppToFront();
			wxString sig = GetNextOutputLineClean();
			Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Program received signal (%s)"), sig.c_str());
		}
		else if (buffer.Matches("signal-string"))
		{
			wxString sig = GetNextOutputLineClean();
			Manager::Get()->GetMessageManager()->Log(m_PageIndex, sig);
		}

		// Stack-frame info
		else if (buffer.Matches("frames-invalid"))
            m_CurrentFrame.Clear();
		else if (buffer.StartsWith("frame-begin "))
		{
            m_CurrentFrame.Clear();
            sscanf(buffer.c_str(), "frame-begin %d %x", &m_CurrentFrame.number, &m_CurrentFrame.address);
            m_CurrentFrame.valid = true;
        }
		else if (buffer.Matches("frame-function-name"))
		{
            m_CurrentFrame.function = GetNextOutputLineClean();
//			Manager::Get()->GetMessageManager()->Log(m_PageIndex, "m_FrameFunction=%s", m_FrameFunction.c_str());
        }
		else if (buffer.Matches("frame-source-file"))
			m_CurrentFrame.file = GetNextOutputLineClean();
		else if (buffer.Matches("frame-source-line"))
			m_CurrentFrame.line = GetNextOutputLineClean();
		else if (buffer.Matches("frame-end") && m_CurrentFrame.valid)
		{
            if (m_pBacktrace)
                m_pBacktrace->AddFrame(m_CurrentFrame);
//			Manager::Get()->GetMessageManager()->Log(m_PageIndex,
//                                                    _("Frame #%-2d [0x%8.8x]: %s (%s:%s)"),
//                                                    m_CurrentFrame.number,
//                                                    m_CurrentFrame.address,
//                                                    m_CurrentFrame.valid ? m_CurrentFrame.function.c_str() : "??",
//                                                    m_CurrentFrame.valid && !m_CurrentFrame.file.IsEmpty() ? m_CurrentFrame.file.c_str() : "??",
//                                                    m_CurrentFrame.valid && !m_CurrentFrame.line.IsEmpty() ? m_CurrentFrame.line.c_str() : "??");
		}

		// variable info
		/*else if (buffer.StartsWith("field-begin "))
			m_Variable << "    " << GetNextOutputLineClean();
		else if (buffer.Matches("field-name-end") ||
				buffer.Matches("field-value"))
			m_Variable << GetNextOutputLineClean();
		else if (buffer.Matches("value-begin -"))
		{
			wxString tmp = GetNextOutputLineClean();
			m_Variable << tmp;
			if (tmp.Contains('{'))
				m_Variable << '\n';
		}
		else if (buffer.Matches("field-end"))
		{
			m_Variable << '\n';
			wxString tmp = GetNextOutputLineClean();
			if (!tmp.StartsWith(","))
				m_Variable << tmp;
			m_pLog->AddLog(m_Variable);
			m_Variable.Clear();
		}
		else if (!m_Variable.IsEmpty() && buffer.Matches("value-end"))
		{
			cbEditor* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
			if (ed)
			{
				if (m_EvalWin)
					m_EvalWin->Destroy();
				wxString tip;
				tip.Printf(_("Value of '%s':\n%s"), m_LastEval.c_str(), m_Variable.c_str());
				m_EvalWin = new wxTipWindow(ed->GetControl(), tip, 640, &m_EvalWin, &m_EvalRect);
			}
			else
				Manager::Get()->GetMessageManager()->Log(m_PageIndex, m_Variable);
			m_Variable.Clear();
		}*/
		
		// source d:/wx2.4/samples/exec/exec.cpp:753:22811:beg:0x403e39
		else if (buffer.StartsWith("source "))
		{
			Manager::Get()->GetMessageManager()->DebugLog(buffer);
			buffer.Remove(0, 7); // remove "source "

			if (!reSource.IsValid())
			#ifdef __WXMSW__
				reSource.Compile("([A-Za-z]:)([ A-Za-z0-9_/\\.~-]*):([0-9]*)");
			#else
				reSource.Compile("([ A-Za-z0-9_/\\.~-]*):([0-9]*)");
			#endif
			if ( reSource.Matches(buffer) )
			{
			#ifdef __WXMSW__
				wxString file = reSource.GetMatch(buffer, 1) + reSource.GetMatch(buffer, 2);
				wxString lineStr = reSource.GetMatch(buffer, 3);
            #else
				wxString file = reSource.GetMatch(buffer, 1);
				wxString lineStr = reSource.GetMatch(buffer, 2);
            #endif
				long int line;
				lineStr.ToLong(&line);
//				Manager::Get()->GetMessageManager()->DebugLog("file %s, line %ld", file.c_str(), line);
				SyncEditor(file, line);
				m_HaltAtLine = line-1;
				BringAppToFront();
			}
		}
	}
	else
	{
        if (buffer.StartsWith("Dump of assembler code"))
        {
//            Manager::Get()->GetMessageManager()->Log(m_PageIndex,
//                                                    "Starting disassembly of %s (starting address: 0x%8.8x)",
//                                                    m_CurrentFrame.valid ? m_CurrentFrame.function.c_str() : "??",
//                                                    m_CurrentFrame.valid ? m_CurrentFrame.address : 0);
            if (m_pDisassembly)
                m_pDisassembly->Clear(m_CurrentFrame);
            //0x00403977 <_ZN7MyFrame11OnLocalTestER14wxCommandEvent+521>:	ret
            wxRegEx re("(0x[0-9A-Za-z]+)[ \t]+<.*>:[ \t]+(.*)");
            wxString tmp;
            do
            {
                tmp = GetNextOutputLine();
                if (tmp.Matches("End of assembler dump."))
                {
//                    Manager::Get()->GetMessageManager()->Log(m_PageIndex, "Disassembly end");
                    break;
                }
                if (re.Matches(tmp))
                    m_pDisassembly->AddAssemblerLine(re.GetMatch(tmp, 1) + ": " + re.GetMatch(tmp, 2));
//                    Manager::Get()->GetMessageManager()->Log(m_PageIndex, "%s: %s", re.GetMatch(tmp, 1).c_str(), re.GetMatch(tmp, 2).c_str());
            }
            while (!tmp.IsEmpty());
            if (m_pDisassembly)
                m_pDisassembly->Show(true);
        }
	}
//        Manager::Get()->GetMessageManager()->Log(m_PageIndex, buffer);
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
	if (!edMan)
        return;
	for (int i = 0; i < edMan->GetEditorsCount(); ++i)
	{
        cbEditor* ed = edMan->GetBuiltinEditor(i);
        if (ed)
            ed->MarkLine(ACTIVE_LINE, -1);
	}
}

void DebuggerGDB::SyncEditor(const wxString& filename, int line)
{
	ClearActiveMarkFromAllEditors();
	cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
	if (project)
	{
        wxFileName fname(filename);
	    ProjectFile* f = project->GetFileByFilename(fname.GetFullPath(), false, true);
    	if (f)
        {
        	cbEditor* ed = Manager::Get()->GetEditorManager()->Open(f->file.GetFullPath());
            if (ed)
			{
				ed->SetProjectFile(f);
            	ed->Show(true);
				ed->GetControl()->GotoLine(line - 10); // make sure we can see some context...
				ed->GetControl()->GotoLine(line - 1);
				ed->MarkLine(ACTIVE_LINE, line - 1);
			}
        }
        else
        {
            // no such file in project; maybe in another open project?
        	cbEditor* ed = Manager::Get()->GetEditorManager()->Open(fname.GetFullPath());
            if (ed)
			{
            	ed->Show(true);
				ed->GetControl()->GotoLine(line - 10); // make sure we can see some context...
				ed->GetControl()->GotoLine(line - 1);
				ed->MarkLine(ACTIVE_LINE, line - 1);
			}
        }
	}
}

wxString DebuggerGDB::GetEditorWordAtCaret()
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return "";
    int start = ed->GetControl()->WordStartPosition(ed->GetControl()->GetCurrentPos(), true);
    int end = ed->GetControl()->WordEndPosition(ed->GetControl()->GetCurrentPos(), true);
    return ed->GetControl()->GetTextRange(start, end);
}

// events

void DebuggerGDB::OnUpdateUI(wxUpdateUIEvent& event)
{
    static bool init_flag=false;
    static bool toolflags[5];
    bool tmpflags[5];

	cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    wxMenuBar* mbar = Manager::Get()->GetAppWindow()->GetMenuBar();
    if (mbar)
    {
        mbar->Enable(idMenuDebug, !m_pProcess && prj);
        mbar->Enable(idMenuContinue, m_pProcess && prj && m_ProgramIsStopped);
        mbar->Enable(idMenuNext, m_pProcess && prj && m_ProgramIsStopped);
        mbar->Enable(idMenuStep, prj && m_ProgramIsStopped);
        mbar->Enable(idMenuStepOut, m_pProcess && prj && m_ProgramIsStopped);
 		mbar->Enable(idMenuRunToCursor, prj && ed && m_ProgramIsStopped);
		mbar->Enable(idMenuToggleBreakpoint, ed && m_ProgramIsStopped);
		mbar->Enable(idMenuSendCommandToGDB, m_pProcess && m_ProgramIsStopped);
 		mbar->Enable(idMenuAddSymbolFile, m_pProcess && m_ProgramIsStopped);
 		mbar->Enable(idMenuBacktrace, m_pProcess && m_ProgramIsStopped);
 		mbar->Enable(idMenuDisassemble, m_pProcess && m_ProgramIsStopped);
 		mbar->Enable(idMenuEditWatches, prj && m_ProgramIsStopped);
        mbar->Enable(idMenuStop, m_pProcess && prj);
	}
/*  NOTE (Rick#1#): The disappearing combobox bug happens due to interference 
    between the different UpdateUI handlers in plugins. Apparently after 
    every ToolBar->Enable() call, the combobox disappears and needs to be drawn 
    again.

    This could be solved by calling ToolBar->Refresh(), 
    but surprise! This triggers another UpdateUI event.
    
    To solve this, we simply check whether the status of the tools has been
    changed. If it hasn't, then there's no need to refresh.
    
*/    
    #ifdef implement_debugger_toolbar
	wxToolBar* tbar = Manager::Get()->GetAppWindow()->GetToolBar();
	if (tbar)
	{
        tmpflags[0]=((!m_pProcess || m_ProgramIsStopped) && prj);
        tmpflags[1]=(prj && ed && m_ProgramIsStopped);
        tmpflags[2]=(m_pProcess && prj && m_ProgramIsStopped);
        tmpflags[3]=(prj && m_ProgramIsStopped);
        tmpflags[4]=(m_pProcess && prj);
        if(!init_flag ||
           toolflags[0]!=tmpflags[0] ||
           toolflags[1]!=tmpflags[1] ||
           toolflags[2]!=tmpflags[2] ||
           toolflags[3]!=tmpflags[3] ||
           toolflags[4]!=tmpflags[4] )
        {
            if(!init_flag) init_flag=true;
            toolflags[0]=tmpflags[0];
            toolflags[1]=tmpflags[1];
            toolflags[2]=tmpflags[2];
            toolflags[3]=tmpflags[3];
            toolflags[4]=tmpflags[4];

            tbar->EnableTool(idMenuDebug,toolflags[0]);
            tbar->EnableTool(idMenuRunToCursor,toolflags[1]);
            tbar->EnableTool(idMenuNext,toolflags[2]);
            tbar->EnableTool(idMenuStep,toolflags[3]);
            tbar->EnableTool(idMenuStepOut,toolflags[2]);
            tbar->EnableTool(idMenuStop,toolflags[4]);
            // This creates a recursive call but since we're checking the flags
            // it only happens once.
            tbar->Refresh(); 
        }
	}
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
        if (m_ProgramIsStopped)
            CmdContinue();
    }
}

void DebuggerGDB::OnContinue(wxCommandEvent& event)
{
	CmdContinue();
}

void DebuggerGDB::OnNext(wxCommandEvent& event)
{
	CmdNext();
}

void DebuggerGDB::OnStep(wxCommandEvent& event)
{
	if (!m_pProcess)
	{
		m_BreakOnEntry = true;
		Debug();
	}
	else CmdStep();
}

void DebuggerGDB::OnStepOut(wxCommandEvent& event)
{
	CmdStepOut();
}

void DebuggerGDB::OnRunToCursor(wxCommandEvent& event)
{
	CmdRunToCursor();
}

void DebuggerGDB::OnToggleBreakpoint(wxCommandEvent& event)
{
	CmdToggleBreakpoint();
}

void DebuggerGDB::OnStop(wxCommandEvent& event)
{
	CmdStop();
}

void DebuggerGDB::OnSendCommandToGDB(wxCommandEvent& event)
{
	wxString cmd = wxGetTextFromUser(_("Enter command for GDB:"), _("Send command to GDB:"), m_LastCmd);
	if (cmd.IsEmpty())
		return;
	m_LastCmd = cmd;
//	Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Sending command \"%s\" to gdb:"), m_LastCmd.c_str());
//	SendCommand("set annotate 0");
	SendCommand(m_LastCmd);
//	SendCommand("set annotate 2");
}

wxString DebuggerGDB::GetInfoFor(const wxString& dbgCmd)
{
	if (!m_pProcess)
		return wxEmptyString;
	m_TimerPollDebugger.Stop();
	wxSafeYield();
	SendCommand(dbgCmd);
	wxString buf = GetNextOutputLine();
	wxString output;
	
	// delay until gdb starts sending output
	// (with 0.5 second timeout - approximately)
	int i = 0;
	while (buf.IsEmpty() && i < 500)
	{
		buf = GetNextOutputLine();
		#if wxVERSION_NUMBER < 2500
            wxUsleep(5);
        #else
            wxMilliSleep(5);
        #endif
		i += 5;
	}
	
	if (buf.IsEmpty())
		m_pLog->AddLog(_("Timeout expired waiting for info..."));

	// let's do business
	while (!buf.IsEmpty())
	{
		if (buf.StartsWith(g_EscapeChars))
		{
			// buffer holds an escaped output
			buf.Remove(0, 2); // clear it
			if (buf.Matches("post-prompt"))
			{
				// our command has been parsed by gdb, so we start receiving data
				buf = GetNextOutputLine();
				while (1)
				{
					if (buf.StartsWith(g_EscapeChars))
					{
						// escaped output
						buf.Remove(0, 2);
						if (buf.Matches("pre-prompt")) // if we encounter this, we 're done
							break;
					}
					else if (!buf.IsEmpty())
					{
						// clean output
						output << buf;
						// append new-line when needed, to separate vars
						if (!buf.Matches(" = ") && buf.Find('=') != -1 && buf.GetChar(buf.Length() - 2) != ',')
							output << '\n';
					}
					buf = GetNextOutputLine();
				}
				break;
			}
		}
		buf = GetNextOutputLine();
	}
	m_TimerPollDebugger.Start(100);

	// loop until gdb finishes sending output
	// (with 0.5 second timeout - approximately)
	i = 0;
	while (!buf.IsEmpty() && i < 500)
	{
		buf = GetNextOutputLine();
        //GDB workaround
        //If overloaded C++ constructor/destructor, break on all.
		if (buf.StartsWith(g_EscapeChars))
		{
			buf.Remove(0,2);
			if (buf.Matches("overload-choice"))
			{
				SendCommand("1");
			}
		}
        //end GDB workaround		
		#if wxVERSION_NUMBER < 2500
            wxUsleep(5);
        #else
            wxMilliSleep(5);
        #endif
		i += 5;
	}

	return output;
}

void DebuggerGDB::OnAddSymbolFile(wxCommandEvent& event)
{
	wxString file = wxFileSelector(_("Choose file to read symbols from"),
									"",
									"",
									"",
									_("Executables and libraries|*.exe;*.dll"),
									wxOPEN | wxFILE_MUST_EXIST);
	if (file.IsEmpty())
		return;
    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Adding symbol file: %s"), file.c_str());
    ConvertToGDBDirectory(file);
	SendCommand("add-symbol-file " + file);
}

void DebuggerGDB::OnBacktrace(wxCommandEvent& event)
{
    CmdBacktrace();
}

void DebuggerGDB::OnDisassemble(wxCommandEvent& event)
{
    CmdDisassemble();
}

void DebuggerGDB::OnEditWatches(wxCommandEvent& event)
{
	wxArrayString watches = m_pTree->GetWatches();
	EditArrayStringDlg dlg(Manager::Get()->GetAppWindow(), watches);
	if (dlg.ShowModal() == wxID_OK)
	{
		m_pTree->SetWatches(watches);
	}
}

void DebuggerGDB::OnGDBOutput(wxCommandEvent& event)
{
	wxString msg = event.GetString();
	if (!msg.IsEmpty())
	{
#if USE_DEBUG_LOG
		//m_pDbgLog->AddLog(msg); // write it in the full debugger log
#endif
		ParseOutput(msg);
	}
}

void DebuggerGDB::OnGDBError(wxCommandEvent& event)
{
	wxString msg = event.GetString();
	if (!msg.IsEmpty())
	{
#if USE_DEBUG_LOG
		//m_pDbgLog->AddLog(msg); // write it in the full debugger log
#endif
		ParseOutput(msg);
	}
}

void DebuggerGDB::OnGDBTerminated(wxCommandEvent& event)
{
	m_TimerPollDebugger.Stop();
	m_LastExitCode = event.GetInt();
	//the process deletes itself
//	m_pProcess = 0L;

	ClearActiveMarkFromAllEditors();
	Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Debugger finished with status %d"), m_LastExitCode);
	
	if (m_NoDebugInfo)
	{
        wxMessageBox(_("This project/target has no debugging info."
                        "Please change this in the project's build options and retry..."),
                        _("Error"),
                        wxICON_STOP);
	}
}

void DebuggerGDB::OnBreakpointAdded(CodeBlocksEvent& event)
{
    if (!m_pProcess)
        return;

	cbEditor* ed = event.GetEditor();
	if (ed)
	{
		Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Breakpoint added: file %s, line %d"), ed->GetFilename().c_str(), event.GetInt() + 1);
/*		
		ProjectFile* pf = ed->GetProjectFile();
		if (!pf)
			return;
        wxString filename = pf->relativeFilename;
        ConvertToGDBFriendly(filename);
//		Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("file %s"), filename.c_str());
		wxString cmd;
		cmd << "break " << filename << ":" << event.GetInt() + 1;
		SendCommand(cmd);
*/		
	}
	else
		Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("OnBreakpointAdded(): No editor defined!"));
}

void DebuggerGDB::OnBreakpointDeleted(CodeBlocksEvent& event)
{
    if (!m_pProcess)
        return;

	cbEditor* ed = event.GetEditor();
	if (ed)
	{
		Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Breakpoint deleted: file %s, line %d"), ed->GetFilename().c_str(), event.GetInt() + 1);
/*		
		ProjectFile* pf = ed->GetProjectFile();
		if (!pf)
			return;
        wxString filename = pf->relativeFilename;
        ConvertToGDBFriendly(filename);
//		Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("file %s"), filename.c_str());
		wxString cmd;
		cmd << "clear " << filename << ":" << event.GetInt() + 1;
		SendCommand(cmd);
*/		
	}
	else
		Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("OnBreakpointDeleted(): No editor defined!"));
}

void DebuggerGDB::OnValueTooltip(CodeBlocksEvent& event)
{
	if (!m_pProcess || !m_ProgramIsStopped)
		return;
    if (!ConfigManager::Get()->Read("debugger_gdb/eval_tooltip", 0L))
        return;

	cbEditor* ed = event.GetEditor();
	if (!ed)
		return;

    int style = event.GetInt();
    if (style != wxSTC_C_DEFAULT && style != wxSTC_C_OPERATOR && style != wxSTC_C_IDENTIFIER)
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
//		Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Value of %s:"), token.c_str());
		pt = ed->GetControl()->PointFromPosition(start);
		pt = ed->GetControl()->ClientToScreen(pt);
		m_EvalRect.x = pt.x;
		m_EvalRect.y = pt.y;
		pt = ed->GetControl()->PointFromPosition(end);
		pt = ed->GetControl()->ClientToScreen(pt);
		m_EvalRect.width = pt.x - m_EvalRect.x;
		m_EvalRect.height = (pt.y + ed->GetControl()->GetCharHeight()) - m_EvalRect.y;
		m_LastEval = token;
		wxString tip;
        tip = GetInfoFor("p " + m_LastEval);
        int pos = tip.First('\n'); // tip is e.g. "$1 = \n<value>"
        if (pos != -1)
            tip.Remove(0, pos + 1); // discard first line
        tip = token + " = " + tip;
		if (m_EvalWin)
            m_EvalWin->Destroy();
		m_EvalWin = new wxTipWindow(ed->GetControl(), tip, 640, &m_EvalWin);
		// set the rect that when the cursor gets out of, the tip window closes
		// just use the tipwindow's rect, a little bit enlarged vertically
		// (because it displays below the cursor)
		wxRect r = m_EvalWin->GetRect();
		r.Inflate(0, 32);
		r.Offset(0, -16);
		m_EvalWin->SetBoundingRect(r);
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

void DebuggerGDB::OnAddWatch(wxCommandEvent& event)
{
    m_pTree->AddWatch(GetEditorWordAtCaret());
}
