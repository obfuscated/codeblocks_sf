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
#include <licenses.h>
#include "xtra_res.h"
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>

#include "debuggergdb.h"
#include "debuggeroptionsdlg.h"

static const wxString g_EscapeChars = char(26);

int idMenuDebug = XRCID("idDebuggerMenuDebug");
int idMenuRunToCursor = XRCID("idDebuggerMenuRunToCursor");
int idMenuNext = XRCID("idDebuggerMenuNext");
int idMenuStep = XRCID("idDebuggerMenuStep");
int idMenuStop = XRCID("idDebuggerMenuStop");
int idMenuContinue = wxNewId();
int idMenuToggleBreakpoint = wxNewId();
int idMenuSendCommandToGDB = wxNewId();
int idMenuAddSymbolFile = wxNewId();
int idMenuEditWatches = wxNewId();

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
	EVT_UPDATE_UI(XRCID("idDebuggerMenuStop"), DebuggerGDB::OnUpdateUI)

	EVT_MENU(idMenuDebug, DebuggerGDB::OnDebug)
	EVT_MENU(idMenuContinue, DebuggerGDB::OnContinue)
	EVT_MENU(idMenuNext, DebuggerGDB::OnNext)
	EVT_MENU(idMenuStep, DebuggerGDB::OnStep)
	EVT_MENU(idMenuToggleBreakpoint, DebuggerGDB::OnToggleBreakpoint)
	EVT_MENU(idMenuRunToCursor, DebuggerGDB::OnRunToCursor)
	EVT_MENU(idMenuStop, DebuggerGDB::OnStop)
	EVT_MENU(idMenuSendCommandToGDB, DebuggerGDB::OnSendCommandToGDB)
	EVT_MENU(idMenuAddSymbolFile, DebuggerGDB::OnAddSymbolFile)
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
	m_NoDebugInfo(false)
{
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxXmlResource::Get()->InsertHandler(new wxToolBarAddOnXmlHandler);
    wxString resPath = ConfigManager::Get()->Read("data_path", wxEmptyString);
    wxXmlResource::Get()->Load(resPath + "/debugger_gdb.zip#zip:*.xrc");

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
    m_pLog = new SimpleTextLog(msgMan, m_PluginInfo.title);
    m_pDbgLog = new SimpleTextLog(msgMan, m_PluginInfo.title + _(" (debug)"));
    wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);

    m_pLog->GetTextControl()->SetFont(font);
    m_pDbgLog->GetTextControl()->SetFont(font);
    m_PageIndex = msgMan->AddLog(m_pLog);
    m_DbgPageIndex = msgMan->AddLog(m_pDbgLog);

    // set log image
	wxBitmap bmp;
	wxString prefix = ConfigManager::Get()->Read("data_path") + "/images/";
    bmp.LoadFile(prefix + "misc_16x16.png", wxBITMAP_TYPE_PNG);
    Manager::Get()->GetMessageManager()->SetLogImage(m_pLog, bmp);
    bmp.LoadFile(prefix + "contents_16x16.png", wxBITMAP_TYPE_PNG);
    Manager::Get()->GetMessageManager()->SetLogImage(m_pDbgLog, bmp);

	if (!m_pTree)
		m_pTree = new DebuggerTree(this, Manager::Get()->GetNotebook());
}

void DebuggerGDB::OnRelease(bool appShutDown)
{
	if (m_pTree)
	{
		delete m_pTree;
		m_pTree = 0L;
	}

    if (Manager::Get()->GetMessageManager())
    {
        Manager::Get()->GetMessageManager()->DeletePage(m_DbgPageIndex);
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

	m_pMenu = new wxMenu("");
    m_pMenu->Append(idMenuDebug, _("&Debug\tF8"), _("Run current project in debugger"));
    m_pMenu->AppendSeparator();
    m_pMenu->Append(idMenuContinue, _("Continue\tCtrl-F7"), _("Continue execution"));
    m_pMenu->Append(idMenuNext, _("Next\tF7"), _("Execute the next line of code"));
    m_pMenu->Append(idMenuStep, _("Step in\tShift-F7"), _("Execute the next instruction, stepping into functions"));
    m_pMenu->AppendSeparator();
    m_pMenu->Append(idMenuToggleBreakpoint, _("Toggle breakpoint\tF5"), _("Toggle breakpoint in current line"));
    m_pMenu->Append(idMenuRunToCursor, _("Run to cursor\tF4"), _("Run program until it reaches the current line"));
    m_pMenu->AppendSeparator();
    m_pMenu->Append(idMenuSendCommandToGDB, _("Send command to GDB"), _("Sends a user-defined command to the GDB debugger"));
    m_pMenu->Append(idMenuAddSymbolFile, _("Add symbol file"), _("Adds symbols from another file"));
    m_pMenu->Append(idMenuEditWatches, _("&Watches..."), _("Edit watches..."));
    m_pMenu->AppendSeparator();
    m_pMenu->Append(idMenuStop, _("Stop debugger"), _("Stop current debugging session"));
	
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
    if (type != mtEditorManager || !menu || !m_pProcess)
        return;
    
    // has to have a word under the caret...
    wxString w = GetEditorWordAtCaret();
    if (w.IsEmpty())
        return;

    wxString s;
    s.Printf(_("Watch '%s'"), w.c_str());
    menu->Insert(0, idMenuDebuggerAddWatch,  s);
    menu->Insert(1, -1, "-");
}

void DebuggerGDB::BuildToolBar(wxToolBar* toolBar)
{
/* NOTE (mandrav#1#): Removed debugger toolbar.
Without it, the combobox in the compiler toolbar doesn't dissappear anymore...
Until we find out why, better without a debugger toolbar. */

	/*if (!m_IsAttached)
		return;
	if (toolBar)
	{        
        wxSize mysize=toolBar->GetToolBitmapSize();
        bool is_small=(mysize.GetWidth()<=16 && mysize.GetHeight()<=16);
        wxString my_16x16=is_small ? "_16x16" : "";
        
        wxString resPath = ConfigManager::Get()->Read("data_path", wxEmptyString);
        wxXmlResource *myres = wxXmlResource::Get();
        myres->Load(resPath + "/debugger_gdb.zip#zip:*.xrc");
        
		// supported by our new wxToolBarAddOnHandler
		myres->LoadObject(toolBar,NULL,"debugger_toolbar"+my_16x16,"wxToolBarAddOn");
        toolBar->Realize();
	}*/
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
				wxString filename = pf->relativeFilename;
				ConvertToGDBFriendly(filename);
				wxString cmd;
				if (bp->enabled)
					cmd << "break " << filename << ":" << bp->line + 1;
			
				SendCommand(cmd);
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

	msgMan->Log(m_PageIndex, target->GetTitle());

	PluginsArray plugins = Manager::Get()->GetPluginManager()->GetCompilerOffers();
	if (plugins.GetCount())
		m_pCompiler = (cbCompilerPlugin*)plugins[0];
	if (m_pCompiler)
	{
		msgMan->AppendLog(m_PageIndex, _("Compiling: "));
		m_pCompiler->Compile(target);
		while (m_pCompiler->IsRunning())
			wxSafeYield();
        msgMan->SwitchTo(m_PageIndex);
		if (m_pCompiler->GetExitCode() != 0)
		{
			msgMan->Log(m_PageIndex, _("failed"));
			msgMan->Log(m_PageIndex, _("Aborting debugging session"));
			return -1;
		}
		msgMan->Log(m_PageIndex, _("done"));
	}

	msgMan->AppendLog(m_PageIndex, _("Starting debugger: "));

	wxString cmd;
	cmd << "gdb -annotate=2 -silent";
	
    m_pProcess = new PipedProcess((void**)&m_pProcess, this, idGDBProcess, true, project->GetBasePath());
    m_Pid = wxExecute(cmd, wxEXEC_ASYNC, m_pProcess);
//    m_Pid = m_pProcess->Launch(cmd);

    if (!m_Pid)
    {
		msgMan->Log(m_PageIndex, _("failed"));
        return -1;
    }
    else if (!m_pProcess->GetOutputStream())
    {
		msgMan->Log(m_PageIndex, _("failed (to get debugger's stdin)"));
        return -2;
    }
    else if (!m_pProcess->GetInputStream())
    {
		msgMan->Log(m_PageIndex, _("failed (to get debugger's stdout)"));
        return -2;
    }
    else if (!m_pProcess->GetErrorStream())
    {
		msgMan->Log(m_PageIndex, _("failed (to get debugger's stderr)"));
        return -2;
    }
	else
		msgMan->Log(m_PageIndex, _("done"));
	
	wxString out;
	m_TimerPollDebugger.Start(100);
	SendCommand("set confirm off");

	// add as include dirs all open project base dirs
	ProjectsArray* projects = prjMan->GetProjects();
	for (unsigned int i = 0; i < projects->GetCount(); ++i)
	{
        cbProject* it = projects->Item(i);
//        if (it == project)
//            continue;
        wxString filename = it->GetBasePath();
        ConvertToGDBFriendly(filename);
        msgMan->Log(m_PageIndex, _("Adding source dir: %s"), filename.c_str());
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
			ConvertToGDBFriendly(out);
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
			ConvertToGDBFriendly(out);
			cmd << "file " << out;
			SendCommand(cmd);
			if (target->GetTargetType() == ttDynamicLib)
			{
				wxString symbols;
				out = UnixFilename(target->GetOutputFilename());
                ConvertToGDBFriendly(out);
				symbols << "add-symbol-file " << out;
				SendCommand(symbols);
			}
			break;
	}

	if (!target->GetExecutionParameters().IsEmpty())
		SendCommand("set args " + target->GetExecutionParameters());

    // switch to output dir
    wxFileName dir(target->GetOutputFilename());
    wxString path = UnixFilename(dir.GetPath(wxPATH_GET_VOLUME));
    if (!path.IsEmpty())
    {
        cmd.Clear();
        ConvertToGDBFriendly(path);
        cmd << "cd " << path;
        SendCommand(cmd);
    }

	SetBreakpoints();
	if (!m_Tbreak.IsEmpty())
	{
		SendCommand(m_Tbreak);
		m_Tbreak.Clear();
	}

    // finally, run the process
	SendCommand("run");
	return 0;
}

void DebuggerGDB::ConvertToGDBFriendly(wxString& str)
{
    if (str.IsEmpty())
        return;

    str = UnixFilename(str);
    while (str.Replace("\\", "/"))
        ;
    str.Replace("/", "//");
    if (str.Find(' ') != -1 && str.GetChar(0) != '"')
        str = "\"" + str + "\"";
}

void DebuggerGDB::SendCommand(const wxString& cmd)
{
    if (!m_pProcess || !m_ProgramIsStopped)
        return;
	Manager::Get()->GetMessageManager()->Log(m_DbgPageIndex, "> " + cmd);
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

	if (!bufferOut.IsEmpty())
		m_pDbgLog->AddLog(bufferOut);
	return bufferOut;
}

wxString DebuggerGDB::GetNextOutputLineClean(bool useStdErr)
{
	wxString line = GetNextOutputLine(useStdErr);
	while (line.IsEmpty() || line.StartsWith(g_EscapeChars))
		line = GetNextOutputLine(useStdErr);

	return line;
}

void DebuggerGDB::CmdContinue()
{
	ClearActiveMarkFromAllEditors();
	Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Continuing..."));
	SendCommand("cont");
}

void DebuggerGDB::CmdNext()
{
	ClearActiveMarkFromAllEditors();
	SendCommand("next");
}

void DebuggerGDB::CmdStep()
{
	ClearActiveMarkFromAllEditors();
	SendCommand("step");
}

void DebuggerGDB::CmdRunToCursor()
{
	cbEditor* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
	if (!ed)
		return;
	
	ProjectFile* pf = ed->GetProjectFile();
	if (!pf)
		return;
	wxString cmd;
	cmd << "tbreak " << pf->relativeFilename << ":" << ed->GetControl()->GetCurrentLine() + 1;
	if (m_pProcess)
	{
		m_Tbreak.Clear();
		SendCommand(cmd);
		CmdContinue();
	}
	else
	{
		m_Tbreak = cmd;
		Debug();
	}
}

void DebuggerGDB::CmdToggleBreakpoint()
{
	ClearActiveMarkFromAllEditors();
	cbEditor* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
	if (!ed)
		return;
	ed->MarkerToggle(BREAKPOINT_MARKER);
	SetBreakpoints();
}

void DebuggerGDB::CmdStop()
{
	ClearActiveMarkFromAllEditors();
	SendCommand("quit");
}

void DebuggerGDB::ParseOutput(const wxString& output)
{
	wxString buffer = output;
	if (buffer.StartsWith(g_EscapeChars)) // ->->
	{
		buffer.Remove(0, 2); // remove ->->
		m_pDbgLog->AddLog(buffer); // write it in the full debugger log
		
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
		else if (buffer.StartsWith("frame-begin "))
			m_LastFrame << GetNextOutputLineClean();
		else if (buffer.Matches("frame-function-name"))
			m_LastFrame << GetNextOutputLineClean();
		else if (buffer.Matches("frame-source-file"))
			m_LastFrame << _(" at ") << GetNextOutputLineClean();
		else if (buffer.Matches("frame-source-line"))
			m_LastFrame << ":" << GetNextOutputLineClean();
		else if (buffer.Matches("frame-end") && !m_LastFrame.IsEmpty())
		{
			Manager::Get()->GetMessageManager()->Log(m_PageIndex, m_LastFrame);
			m_LastFrame.Clear();
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
				reSource.Compile("([A-Za-z]:)([ A-Za-z0-9_/\\.]*):([0-9]*)");  // check for . - _ too
			if ( reSource.Matches(buffer) )
			{
				wxString file = reSource.GetMatch(buffer, 1) + reSource.GetMatch(buffer, 2);
				wxString lineStr = reSource.GetMatch(buffer, 3);
				long int line;
				Manager::Get()->GetMessageManager()->DebugLog("file %s, line %ld", file.c_str(), line);
				lineStr.ToLong(&line);
				SyncEditor(file, line);
				BringAppToFront();
			}
		}
	}
//	else
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
	for (int i = 0; i < edMan->GetEditorsCount(); ++i)
	{
		cbEditor* ed = edMan->GetEditor(i);
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
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (!ed)
        return "";
    int start = ed->GetControl()->WordStartPosition(ed->GetControl()->GetCurrentPos(), true);
    int end = ed->GetControl()->WordEndPosition(ed->GetControl()->GetCurrentPos(), true);
    return ed->GetControl()->GetTextRange(start, end);
}

// events

void DebuggerGDB::OnUpdateUI(wxUpdateUIEvent& event)
{
	cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
	cbEditor* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    wxMenuBar* mbar = Manager::Get()->GetAppWindow()->GetMenuBar();
//	wxToolBar* tbar = Manager::Get()->GetAppWindow()->GetToolBar();
    if (mbar)
    {
        mbar->Enable(idMenuDebug, !m_pProcess && prj);
        mbar->Enable(idMenuContinue, m_pProcess && prj && m_ProgramIsStopped);
        mbar->Enable(idMenuNext, m_pProcess && prj && m_ProgramIsStopped);
        mbar->Enable(idMenuStep, m_pProcess && prj && m_ProgramIsStopped);
		mbar->Enable(idMenuRunToCursor, !m_pProcess && prj && ed);
		mbar->Enable(idMenuToggleBreakpoint, ed && m_ProgramIsStopped);
		mbar->Enable(idMenuSendCommandToGDB, m_pProcess && m_ProgramIsStopped);
 		mbar->Enable(idMenuAddSymbolFile, m_pProcess && m_ProgramIsStopped);
 		mbar->Enable(idMenuEditWatches, prj && m_ProgramIsStopped);
        mbar->Enable(idMenuStop, m_pProcess && prj && m_ProgramIsStopped);
	}

	/*if (tbar)
	{
        tbar->EnableTool(idMenuDebug, !m_pProcess && prj);
		tbar->EnableTool(idMenuRunToCursor, !m_pProcess && prj && ed);
        tbar->EnableTool(idMenuNext, m_pProcess && prj && m_ProgramIsStopped);
        tbar->EnableTool(idMenuStep, m_pProcess && prj && m_ProgramIsStopped);
        tbar->EnableTool(idMenuStop, m_pProcess && prj && m_ProgramIsStopped);
	}*/

    // allow other UpdateUI handlers to process this event
    // *very* important! don't forget it...
    event.Skip();
}

void DebuggerGDB::OnDebug(wxCommandEvent& event)
{
	Debug();
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
	CmdStep();
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
	wxYield();
	SendCommand(dbgCmd);
	wxString buf = GetNextOutputLine();
	wxString output;
	
	// delay until gdb starts sending output
	// (with 0.5 second timeout - approximately)
	int i = 0;
	while (buf.IsEmpty() && i < 500)
	{
		buf = GetNextOutputLine();
		wxUsleep(5);
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
		wxUsleep(5);
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
	SendCommand("add-symbol-file " + file);
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
		//m_pDbgLog->AddLog(msg); // write it in the full debugger log
		ParseOutput(msg);
	}
}

void DebuggerGDB::OnGDBError(wxCommandEvent& event)
{
	wxString msg = event.GetString();
	if (!msg.IsEmpty())
	{
		//m_pDbgLog->AddLog(msg); // write it in the full debugger log
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
		ProjectFile* pf = ed->GetProjectFile();
		if (!pf)
			return;
        wxString filename = pf->relativeFilename;
        ConvertToGDBFriendly(filename);
//		Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("file %s"), filename.c_str());
		wxString cmd;
		cmd << "break " << filename << ":" << event.GetInt() + 1;
		SendCommand(cmd);
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
		ProjectFile* pf = ed->GetProjectFile();
		if (!pf)
			return;
        wxString filename = pf->relativeFilename;
        ConvertToGDBFriendly(filename);
//		Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("file %s"), filename.c_str());
		wxString cmd;
		cmd << "clear " << filename << ":" << event.GetInt() + 1;
		SendCommand(cmd);
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
