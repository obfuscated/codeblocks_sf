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

#include "debuggergdb.h"

static const wxString g_EscapeChars = char(26);

int idMenuDebug = wxNewId();
int idMenuRunToCursor = wxNewId();
int idMenuContinue = wxNewId();
int idMenuNext = wxNewId();
int idMenuStep = wxNewId();
int idMenuToggleBreakpoint = wxNewId();
int idMenuStop = wxNewId();
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
	EVT_UPDATE_UI_RANGE(idMenuDebug, idMenuDebuggerAddWatch, DebuggerGDB::OnUpdateUI)

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
	//EVT_EDITOR_TOOLTIP(DebuggerGDB::OnValueTooltip)
	
	EVT_PIPEDPROCESS_STDOUT(idGDBProcess, DebuggerGDB::OnGDBOutput)
	EVT_PIPEDPROCESS_STDERR(idGDBProcess, DebuggerGDB::OnGDBError)
	EVT_PIPEDPROCESS_TERMINATED(idGDBProcess, DebuggerGDB::OnGDBTerminated)
	
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
	m_pTree(0L)
{
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
}

void DebuggerGDB::OnAttach()
{
    MessageManager* msgMan = Manager::Get()->GetMessageManager();
    m_pLog = new SimpleTextLog(msgMan, m_PluginInfo.title);
    m_pDbgLog = new SimpleTextLog(msgMan, m_PluginInfo.title + _(" (debug)"));
#ifdef __WXMSW__
    #define DEFAULT_SIZE 9
#else
    #define DEFAULT_SIZE 12
#endif // __WXMSW__
    wxFont font(DEFAULT_SIZE, wxMODERN, wxNORMAL, wxNORMAL);

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

void DebuggerGDB::OnRelease()
{
	if (m_pTree)
	{
		delete m_pTree;
		m_pTree = 0L;
	}

    Manager::Get()->GetMessageManager()->DeletePage(m_DbgPageIndex);
    Manager::Get()->GetMessageManager()->DeletePage(m_PageIndex);
}

DebuggerGDB::~DebuggerGDB()
{
}

int DebuggerGDB::Configure()
{
	NotImplemented("DebuggerGDB::Configure()");
	return -1;
}

void DebuggerGDB::BuildMenu(wxMenuBar* menuBar)
{
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
	if (toolBar)
	{
		wxString res = ConfigManager::Get()->Read("data_path") + "/images/";
		
		toolBar->AddSeparator();
		
		toolBar->AddTool(idMenuDebug, _("Debug"), wxBitmap(res + "dbgrun.png", wxBITMAP_TYPE_PNG));
		toolBar->SetToolShortHelp(idMenuDebug, _("Debug"));
		toolBar->SetToolLongHelp(idMenuDebug, _("Debug program"));
		toolBar->AddTool(idMenuRunToCursor, _("Run to cursor"), wxBitmap(res + "dbgrunto.png", wxBITMAP_TYPE_PNG));
		toolBar->SetToolShortHelp(idMenuRunToCursor, _("Run to cursor"));
		toolBar->SetToolLongHelp(idMenuRunToCursor, _("Run program until it reaches the current line"));
		toolBar->AddTool(idMenuNext, _("Next"), wxBitmap(res + "dbgnext.png", wxBITMAP_TYPE_PNG));
		toolBar->SetToolShortHelp(idMenuNext, _("Next line"));
		toolBar->SetToolLongHelp(idMenuNext, _("Execute the next line of code"));
		toolBar->AddTool(idMenuStep, _("Step"), wxBitmap(res + "dbgstep.png", wxBITMAP_TYPE_PNG));
		toolBar->SetToolShortHelp(idMenuStep, _("Next instruction"));
		toolBar->SetToolLongHelp(idMenuStep, _("Execute the next instruction, stepping into functions"));
		toolBar->AddTool(idMenuStop, _("Stop"), wxBitmap(res + "stop.png", wxBITMAP_TYPE_PNG));
		toolBar->SetToolShortHelp(idMenuStop, _("Stop debugger"));
		toolBar->SetToolLongHelp(idMenuStop, _("Stop current debugging session"));
		toolBar->Realize();
	}
}

void DebuggerGDB::DoWatches()
{
	wxString info;
	if (m_pProcess)
	{
		// TODO: add configuration for the following...
		//info << GetInfoFor("info args") << '\n';
		//info << GetInfoFor("info locals") << '\n';
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
				wxString cmd;
				if (bp->enabled)
					cmd << "break " << UnixFilename(pf->relativeFilename) << ":" << bp->line + 1;
			
				SendCommand(cmd);
			}
		}
	}
}

int DebuggerGDB::Debug()
{
	if (m_pProcess)
		return 1;
	
	cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
	if (!project)
		return 2;

    MessageManager* msgMan = Manager::Get()->GetMessageManager();
    msgMan->SwitchTo(m_PageIndex);
	m_pLog->GetTextControl()->Clear();

	PluginsArray plugins = Manager::Get()->GetPluginManager()->GetCompilerOffers();
	if (plugins.GetCount())
		m_pCompiler = (cbCompilerPlugin*)plugins[0];
	if (m_pCompiler)
	{
		m_pCompiler->Compile();
		msgMan->AppendLog(m_PageIndex, _("Compiling: "));
		while (m_pCompiler->IsRunning())
			wxYield();
		if (m_pCompiler->GetExitCode() != 0)
		{
			msgMan->Log(m_PageIndex, _("failed"));
			msgMan->Log(m_PageIndex, _("Aborting debugging session"));
			return -1;
		}
		msgMan->Log(m_PageIndex, _("done"));
	}

    msgMan->SwitchTo(m_PageIndex);
	msgMan->AppendLog(m_PageIndex, _("Selecting target: "));
	m_TargetIndex = project->SelectTarget(m_TargetIndex);
	if (m_TargetIndex == -1)
	{
		msgMan->Log(m_PageIndex, _("canceled"));
		return 3;
	}
	ProjectBuildTarget* target = project->GetBuildTarget(m_TargetIndex);

	msgMan->Log(m_PageIndex, target->GetTitle());
	msgMan->AppendLog(m_PageIndex, _("Starting debugger: "));
	
	wxString cmd;
	cmd << "gdb -nw -annotate=2 -silent";

    m_pProcess = new PipedProcess(this, idGDBProcess, true, project->GetBasePath());
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
	
	m_TimerPollDebugger.Start(100);
	SendCommand("set confirm off");
	cmd.Clear();
	switch (target->GetTargetType())
	{
		case ttExecutable:
		case ttConsoleOnly:
			// "-async" option is not really supported, at least under Win32, as far as I know
			cmd << "file " << UnixFilename(target->GetOutputFilename());
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
				wxLogError(_("You must select a host application to \"run\" a library..."));
				CmdStop();
				return 4;
			}
			cmd << "file " << UnixFilename(target->GetHostApplication());
			SendCommand(cmd);
			if (target->GetTargetType() == ttDynamicLib)
			{
				wxString symbols;
				symbols << "add-symbol-file " << UnixFilename(target->GetOutputFilename());
				SendCommand(symbols);
			}
			break;
	}

	if (!target->GetExecutionParameters().IsEmpty())
		SendCommand("set args " + target->GetExecutionParameters());

	SetBreakpoints();
	if (!m_Tbreak.IsEmpty())
	{
		SendCommand(m_Tbreak);
		m_Tbreak.Clear();
	}
	SendCommand("run");
	return 0;
}

void DebuggerGDB::SendCommand(const wxString& cmd)
{
    if (!m_pProcess || !m_ProgramIsStopped)
        return;
	//Manager::Get()->GetMessageManager()->Log(m_PageIndex, cmd);
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
			Manager::Get()->GetMessageManager()->Log(m_PageIndex, GetNextOutputLineClean(true));
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
				reSource.Compile("([A-Za-z]:)([A-Za-z0-9_/\\.]*):([0-9]*)");  // check for . - _ too
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
	    ProjectFile* f = project->GetFileByFilename(filename, false, true);
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
	wxToolBar* tbar = Manager::Get()->GetAppWindow()->GetToolBar();
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

	if (tbar)
	{
        tbar->EnableTool(idMenuDebug, !m_pProcess && prj);
		tbar->EnableTool(idMenuRunToCursor, !m_pProcess && prj && ed);
        tbar->EnableTool(idMenuNext, m_pProcess && prj && m_ProgramIsStopped);
        tbar->EnableTool(idMenuStep, m_pProcess && prj && m_ProgramIsStopped);
        tbar->EnableTool(idMenuStop, m_pProcess && prj && m_ProgramIsStopped);
	}

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
		Sleep(5);
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
		Sleep(5);
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
	m_pProcess = 0L;

	ClearActiveMarkFromAllEditors();
	Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Debugger finished with status %d"), m_LastExitCode);
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
		wxString cmd;
		cmd << "break " << pf->relativeFilename << ":" << event.GetInt() + 1;
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
		wxString cmd;
		cmd << "clear " << pf->relativeFilename << ":" << event.GetInt() + 1;
		SendCommand(cmd);
	}
	else
		Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("OnBreakpointDeleted(): No editor defined!"));
}

void DebuggerGDB::OnValueTooltip(CodeBlocksEvent& event)
{
	if (!m_pProcess || !m_ProgramIsStopped)
		return;

	cbEditor* ed = event.GetEditor();
	if (!ed)
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
		//Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Value of %s:"), token.c_str());
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
		tip.Printf(_("Evaluating '%s'..."), m_LastEval.c_str());
		m_EvalWin = new wxTipWindow(ed->GetControl(), tip, 640, &m_EvalWin);
		wxString cmd;
		cmd << "p " << token;
		SendCommand(cmd);
	}
}

void DebuggerGDB::OnTimer(wxTimerEvent& event)
{
    while (m_pProcess && ((PipedProcess*)m_pProcess)->HasInput())
		;
}

void DebuggerGDB::OnWatchesChanged(wxCommandEvent& event)
{
	DoWatches();
}

void DebuggerGDB::OnAddWatch(wxCommandEvent& event)
{
    m_pTree->AddWatch(GetEditorWordAtCaret());
}
