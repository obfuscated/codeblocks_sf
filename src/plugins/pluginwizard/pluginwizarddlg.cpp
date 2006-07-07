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

#ifdef CB_PRECOMP
#include "sdk.h"
#else
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>
#include "globals.h"
#include "manager.h"
#endif
#include "enterinfodlg.h"
#include "pluginwizarddlg.h"

BEGIN_EVENT_TABLE(PluginWizardDlg, wxDialog)
	EVT_UPDATE_UI(-1, PluginWizardDlg::OnUpdateUI)
	EVT_BUTTON(XRCID("btnInfo"), PluginWizardDlg::OnEditInfoClick)
	EVT_BUTTON(XRCID("btnOK"), PluginWizardDlg::OnOKClick)
	EVT_TEXT(XRCID("txtName"), PluginWizardDlg::OnNameChange)
END_EVENT_TABLE()

PluginWizardDlg::PluginWizardDlg()
{
	wxXmlResource::Get()->LoadDialog(this, (wxWindow*)Manager::Get()->GetAppWindow(), _T("dlgNewPlugin"));
}

PluginWizardDlg::~PluginWizardDlg()
{
}

void PluginWizardDlg::DoGuardBlock()
{
	m_Header = XRCCTRL(*this, "txtHeader", wxTextCtrl)->GetValue();
	wxFileName headerFname(m_Header);
	wxString GuardWord = headerFname.GetFullName();
	GuardWord.MakeUpper();
	GuardWord.Replace(_T("."), _T("_"));
	GuardWord.Replace(_T(" "), _T("_"));
	XRCCTRL(*this, "txtGuardBlock", wxTextCtrl)->SetValue(GuardWord);
}


void PluginWizardDlg::DoAddHeaderOption(wxString& buffer, bool has, const wxString& retVal)
{
	if (has)
		buffer << _T(";") << _T('\n');
	else
		buffer << _T("{ return ") << retVal << _T("; }") << _T('\n');
}

void PluginWizardDlg::DoAddHeaderTool(wxString& buffer)
{
	buffer << _T('\t') << _T('\t') << _T("int Execute();") << _T('\n');
}

void PluginWizardDlg::DoAddHeaderCompiler(wxString& buffer)
{
	buffer << _T('\t') << _T('\t') << _T("int Run(ProjectBuildTarget* target = 0L);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int Run(const wxString& target);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int Clean(ProjectBuildTarget* target = 0L);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int Clean(const wxString& target);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int Build(ProjectBuildTarget* target = 0L);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int Build(const wxString& target);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int Rebuild(ProjectBuildTarget* target = 0L);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int Rebuild(const wxString& target);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int BuildWorkspace(const wxString& target = wxEmptyString);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int RebuildWorkspace(const wxString& target = wxEmptyString);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int CleanWorkspace(const wxString& target = wxEmptyString);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int CompileFile(const wxString& file);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int KillProcess();") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("bool IsRunning() const;") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int GetExitCode() const;") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int Configure(cbProject* project, ProjectBuildTarget* target = 0L);") << _T('\n');
}

void PluginWizardDlg::DoAddHeaderDebugger(wxString& buffer)
{
	buffer << _T('\t') << _T('\t') << _T("bool AddBreakpoint(const wxString& file, int line);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("bool AddBreakpoint(const wxString& functionSignature);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("bool RemoveBreakpoint(const wxString& file, int line);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("bool RemoveBreakpoint(const wxString& functionSignature);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("bool RemoveAllBreakpoints(const wxString& file = wxEmptyString);") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int Debug();") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("void Continue();") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("void Next();") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("void Step();") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("void Stop();") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("bool IsRunning() const;") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int GetExitCode() const;") << _T('\n');
}

void PluginWizardDlg::DoAddHeaderCodeCompletion(wxString& buffer)
{
	buffer << _T('\t') << _T('\t') << _T("wxArrayString GetCallTips();") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int CodeComplete();") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("void ShowCallTip();") << _T('\n');
}

void PluginWizardDlg::DoAddHeaderMime(wxString& buffer)
{
	buffer << _T('\t') << _T('\t') << _T("bool HandlesEverything() const;") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("bool CanHandleFile(const wxString& filename) const;") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int OpenFile(const wxString& filename);") << _T('\n');
}

void PluginWizardDlg::DoAddHeaderWizard(wxString& buffer)
{
	buffer << _T('\t') << _T('\t') << _T("int GetCount() const;") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("wxString GetTitle(int index) const;") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("wxString GetDescription(int index) const;") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("wxString GetCategory(int index) const;") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("const wxBitmap& GetBitmap(int index) const;") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("int Launch(int index);") << _T('\n');
}

void PluginWizardDlg::DoAddSourceTool(const wxString& classname, wxString& buffer)
{
	buffer << _T("int ") << m_Info.name << _T("::Execute()") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//do your magic ;)") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Execute()") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
}

void PluginWizardDlg::DoAddSourceCompiler(const wxString& classname, wxString& buffer)
{
	buffer << _T("int ") << classname << _T("::Run(ProjectBuildTarget* target)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//run the active project or specified target") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Run(ProjectBuildTarget*)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::Run(const wxString& target)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//run the active project or specified target") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Run(const wxString&)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::Clean(ProjectBuildTarget* target)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//clean the active project or specified target") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Clean(ProjectBuildTarget*)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::Clean(const wxString& target)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//clean the active project or specified target") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Clean(const wxString&)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::Build(ProjectBuildTarget* target)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//build the active project or specified target") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Build(ProjectBuildTarget*)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::Build(const wxString& target)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//build the active project or specified target") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Build(const wxString&)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::Rebuild(ProjectBuildTarget* target)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//rebuild the active project or specified target") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Rebuild(ProjectBuildTarget*)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::Rebuild(const wxString& target)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//rebuild the active project or specified target") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Rebuild(const wxString&)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::BuildWorkspace(const wxString& target)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//compile all projects") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::BuildWorkspace(const wxString&)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::RebuildWorkspace(const wxString& target)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//rebuild all projects") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::RebuildWorkspace(const wxString&)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::CleanWorkspace(const wxString& target)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//rebuild all projects") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::CleanWorkspace(const wxString&)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::CompileFile(const wxString& file)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//compile only the specified file") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::CompileFile()") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::KillProcess()") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//end compiler process") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::KillProcess()") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("bool ") << classname << _T("::IsRunning() const") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//return true if session is active") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::IsRunning()") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return false;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::GetExitCode() const") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//return last session exit code") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::GetExitCode()") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::Configure(cbProject* project, ProjectBuildTarget* target)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//show configuration dialog") << _T('\n');
	buffer << _T('\t') << _T("//all function arguments are optional") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Configure(cbProject*, ProjectBuildTarget*)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
}

void PluginWizardDlg::DoAddSourceDebugger(const wxString& classname, wxString& buffer)
{
	buffer << _T("bool ") << classname << _T("::AddBreakpoint(const wxString& file, int line)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//add a breakpoint") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::AddBreakpoint(const wxString&, int)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return false;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("bool ") << classname << _T("::AddBreakpoint(const wxString& functionSignature)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//add a breakpoint") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::AddBreakpoint(const wxString&)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return false;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("bool ") << classname << _T("::RemoveBreakpoint(const wxString& file, int line)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//remove a breakpoint") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::RemoveBreakpoint(const wxString&, int)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return false;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("bool ") << classname << _T("::RemoveBreakpoint(const wxString& functionSignature)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//remove a breakpoint") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::RemoveBreakpoint(const wxString&)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return false;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("bool ") << classname << _T("::RemoveAllBreakpoints(const wxString& file)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//remove all breakpoints") << _T('\n');
	buffer << _T('\t') << _T("//if file is not empty, remove breakpoints only from file") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::RemoveAllBreakpoints(const wxString&)") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return false;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::Debug()") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//actual debugging session starts here") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Debug()") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("void ") << classname << _T("::Continue()") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//tell debugger to continue") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Continue()") << _T("\"));") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("void ") << classname << _T("::Next()") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//tell debugger to step one line of code") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Next()") << _T("\"));") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("void ") << classname << _T("::Step()") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//tell debugger to step one instruction (following inside functions, if needed)") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Step()") << _T("\"));") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("void ") << classname << _T("::Stop()") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//tell debugger to end debugging session") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::Stop()") << _T("\"));") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("bool ") << classname << _T("::IsRunning() const") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//return true if session is active") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::IsRunning()") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return false;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T("int ") << classname << _T("::GetExitCode() const") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//return last session exit code") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::GetExitCode()") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
}

void PluginWizardDlg::DoAddSourceCodeCompletion(const wxString& classname, wxString& buffer)
{
	buffer << _T("wxArrayString ") << classname << _T("::GetCallTips()") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//well, give call tips...") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::GetCallTips()") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("wxArrayString items;") << _T('\n');
	buffer << _T('\t') << _T("return items;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
	buffer << _T("int ") << classname << _T("::CodeComplete()") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//code-complete") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::CodeComplete()") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
	buffer << _T("void ") << classname << _T("::ShowCallTip()") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//display a call-tip") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::ShowCallTip()") << _T("\"));") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
}

void PluginWizardDlg::DoAddSourceMime(const wxString& classname, wxString& buffer)
{
	buffer << _T("bool ") << classname << _T("::HandlesEverything() const") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//return true if this plugin can handle *any* file, false if not...") << _T('\n');
	buffer << _T('\t') << _T("return false;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
	buffer << _T("bool ") << classname << _T("::CanHandleFile(const wxString& filename) const") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//return true if this plugin can handle the file, false if not...") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::CanHandleFile()") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return false;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
	buffer << _T("int ") << classname << _T("::OpenFile(const wxString& filename)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//Open the file. You said you could in CanHandleFile(), didn't you ;)") << _T('\n');
	buffer << _T('\t') << _T("//Remember to return zero for success...") << _T('\n');
	buffer << _T('\t') << _T("NotImplemented(_T(\"") << classname << _T("::OpenFile()") << _T("\"));") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
}

void PluginWizardDlg::DoAddSourceWizard(const wxString& classname, wxString& buffer)
{
	buffer << _T("int ") << classname << _T("::GetCount() const") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//return how many wizards are contained in this plugin") << _T('\n');
	buffer << _T('\t') << _T("return 1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
	buffer << _T("wxString ") << classname << _T("::GetTitle(int index) const") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//return this wizard's title") << _T('\n');
	buffer << _T('\t') << _T("//this will appear in the new-project dialog") << _T('\n');
	buffer << _T('\t') << _T("//make sure you set this!") << _T('\n');
	buffer << _T('\t') << _T("return m_PluginInfo.title;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
	buffer << _T("wxString ") << classname << _T("::GetDescription(int index) const") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//return this wizard's description") << _T('\n');
	buffer << _T('\t') << _T("//make sure you set this!") << _T('\n');
	buffer << _T('\t') << _T("return m_PluginInfo.description;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
	buffer << _T("wxString ") << classname << _T("::GetCategory(int index) const") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//return this wizard's category") << _T('\n');
	buffer << _T('\t') << _T("//try to match an existing category") << _T('\n');
	buffer << _T('\t') << _T("//make sure you change this!") << _T('\n');
	buffer << _T('\t') << _T("return _(\"My wizard category\");") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
	buffer << _T("const wxBitmap& ") << classname << _T("::GetBitmap(int index) const") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//return this wizard's bitmap") << _T('\n');
	buffer << _T('\t') << _T("//this will appear in the new-project dialog") << _T('\n');
	buffer << _T('\t') << _T("static wxBitmap IconBitmap;") << _T('\n');
	buffer << _T('\t') << _T("//make sure the bitmap is loaded") << _T('\n');
	buffer << _T('\t') << _T("if (!IconBitmap.Ok())") << _T('\n');
	buffer << _T('\t') << _T("{") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("//make sure you change the values below (xyz_wizard and xyz.png)") << _T('\n');
    buffer << _T('\t') << _T('\t') << _T("wxString resPath = ConfigManager::GetDataFolder() + _T(\"/templates/xyz_wizard\");") << _T('\n');
    buffer << _T('\t') << _T('\t') << _T("IconBitmap.LoadFile(resPath + _T(\"/xyz.png\"), wxBITMAP_TYPE_PNG);") << _T('\n');
	buffer << _T('\t') << _T("}") << _T('\n');
	buffer << _T('\t') << _T("return IconBitmap;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
	buffer << _T("int ") << classname << _T("::Launch(int index)") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//launch your wizard :D") << _T('\n');
	buffer << _T('\t') << _T("//return 0 on success...") << _T('\n');
	buffer << _T('\t') << _T("return -1;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
}

// events

void PluginWizardDlg::OnNameChange(wxCommandEvent& event)
{
	wxString name = XRCCTRL(*this, "txtName", wxTextCtrl)->GetValue();
	name.MakeLower();
	XRCCTRL(*this, "txtHeader", wxTextCtrl)->SetValue(name + _T(".h"));
	XRCCTRL(*this, "txtImplementation", wxTextCtrl)->SetValue(name + _T(".cpp"));
	DoGuardBlock();
}

void PluginWizardDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
	bool isTool = XRCCTRL(*this, "cmbType", wxComboBox)->GetSelection() == 1;
	bool isMime = XRCCTRL(*this, "cmbType", wxComboBox)->GetSelection() == 5;
	bool isWizard = XRCCTRL(*this, "cmbType", wxComboBox)->GetSelection() == 6;
	XRCCTRL(*this, "chkHasConfigure", wxCheckBox)->Enable(!isWizard);
	XRCCTRL(*this, "chkHasMenu", wxCheckBox)->Enable(!isTool && !isMime && !isWizard);
	XRCCTRL(*this, "chkHasModuleMenu", wxCheckBox)->Enable(!isTool && !isMime && !isWizard);
	XRCCTRL(*this, "chkHasToolbar", wxCheckBox)->Enable(!isTool && !isMime && !isWizard);
}

void PluginWizardDlg::OnEditInfoClick(wxCommandEvent& event)
{
	EnterInfoDlg dlg(m_Info);
    PlaceWindow(&dlg);
	if (dlg.ShowModal() == wxID_OK)
		m_Info = dlg.GetInfo();
}

void PluginWizardDlg::OnOKClick(wxCommandEvent& event)
{
	EndModal(wxID_OK);
}

void PluginWizardDlg::CreateFiles()
{
	int type = XRCCTRL(*this, "cmbType", wxComboBox)->GetSelection();
	bool isTool = type == 1;
	// set some variable for easy reference
	m_Info.name = XRCCTRL(*this, "txtName", wxTextCtrl)->GetValue();

	bool hasConfigure = XRCCTRL(*this, "chkHasConfigure", wxCheckBox)->GetValue();
	bool hasMenu = !isTool && XRCCTRL(*this, "chkHasMenu", wxCheckBox)->GetValue();
	bool hasModuleMenu = !isTool && XRCCTRL(*this, "chkHasModuleMenu", wxCheckBox)->GetValue();
	bool hasToolbar = !isTool && XRCCTRL(*this, "chkHasToolbar", wxCheckBox)->GetValue();

	m_Header = XRCCTRL(*this, "txtHeader", wxTextCtrl)->GetValue();
	m_Implementation = XRCCTRL(*this, "txtImplementation", wxTextCtrl)->GetValue();
	bool GuardBlock = XRCCTRL(*this, "chkGuardBlock", wxCheckBox)->GetValue();
	wxString GuardWord = XRCCTRL(*this, "txtGuardBlock", wxTextCtrl)->GetValue();
	if (GuardWord.IsEmpty())
		DoGuardBlock();
	wxFileName headerFname(UnixFilename(m_Header));
	wxFileName implFname(UnixFilename(m_Implementation));

	wxString buffer;

	// actual creation starts here
	// let's start with the header file
	buffer << _T("/***************************************************************") << _T('\n');
	buffer << _T(" * Name:      ") << headerFname.GetFullName() << _T('\n');
	buffer << _T(" * Purpose:   Code::Blocks plugin") << _T('\n');
	buffer << _T(" * Author:    ") << m_Info.author << _T("<") << m_Info.authorEmail << _T(">") << _T('\n');
//	buffer << _T(" * Created:   ") << wxDateTime::Now().Format(_("%c"), wxDateTime::Local) << _T('\n');
	buffer << _T(" * Copyright: (c) ") << m_Info.author <<  _T('\n');
	buffer << _T(" * License:   GPL") << _T('\n');
	buffer << _T(" **************************************************************/") << _T('\n');
	buffer << _T('\n');

	if (GuardBlock)
	{
		buffer << _T("#ifndef ") << GuardWord << _T('\n');
		buffer << _T("#define ") << GuardWord << _T('\n');
		buffer << _T('\n');
	}

	buffer << _T("// For compilers that support precompilation, includes <wx/wx.h>") << _T('\n');
	buffer << _T("#include <wx/wxprec.h>") << _T('\n');
	buffer << _T('\n');

	buffer << _T("#ifdef __BORLANDC__") << _T('\n');
	buffer << _T("\t#pragma hdrstop") << _T('\n');
	buffer << _T("#endif") << _T('\n');
	buffer << _T('\n');

	buffer << _T("#ifndef WX_PRECOMP") << _T('\n');
	buffer << _T("\t#include <wx/wx.h>") << _T('\n');
	buffer << _T("#endif") << _T('\n');
	buffer << _T('\n');

	buffer << _T("#include <cbplugin.h> // the base class we 're inheriting") << _T('\n');
	buffer << _T("#include <settings.h> // needed to use the Code::Blocks SDK") << _T('\n');
	buffer << _T('\n');
	buffer << _T("class ") << m_Info.name;
	switch (type)
	{
		case 0: buffer << _T(" : public cbPlugin"); break;
		case 1: buffer << _T(" : public cbToolPlugin"); break;
		case 2: buffer << _T(" : public cbCompilerPlugin"); break;
		case 3: buffer << _T(" : public cbDebuggerPlugin"); break;
		case 4: buffer << _T(" : public cbCodeCompletionPlugin"); break;
		case 5: buffer << _T(" : public cbMimePlugin"); break;
		case 6: buffer << _T(" : public cbProjectWizardPlugin"); break;
	}
	buffer << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("public:") << _T('\n');
	buffer << _T('\t') << _T('\t') << m_Info.name << _T("();") << _T('\n');
	buffer << _T('\t') << _T('\t') << _T("~") << m_Info.name << _T("();") << _T('\n');

    buffer << _T('\t') << _T('\t') << _T("int Configure()");
    DoAddHeaderOption(buffer, hasConfigure, _T(" 0"));
    buffer << _T('\t') << _T('\t') << _T("int GetConfigurationPriority() const");
    DoAddHeaderOption(buffer, false, _T(" 50"));
    buffer << _T('\t') << _T('\t') << _T("int GetConfigurationGroup() const");
    DoAddHeaderOption(buffer, false, _T(" cgUnknown"));
    buffer << _T('\t') << _T('\t') << _T("cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent)");
    DoAddHeaderOption(buffer, hasConfigure, _T(" 0"));
	if (type != 1 && type != 5 && type != 6) // not cbToolPlugin, not cbMimePlugin and not cbProjectWizardPlugin
	{
        buffer << _T('\t') << _T('\t') << _T("void BuildMenu(wxMenuBar* menuBar)");
        DoAddHeaderOption(buffer, hasMenu, wxEmptyString);
        buffer << _T('\t') << _T('\t') << _T("void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0)");
        DoAddHeaderOption(buffer, hasModuleMenu, wxEmptyString);
        buffer << _T('\t') << _T('\t') << _T("bool BuildToolBar(wxToolBar* toolBar)");
        DoAddHeaderOption(buffer, hasToolbar, _T("false"));
	}
	switch (type)
	{
		case 1: // tool
			DoAddHeaderTool(buffer);
			break;
		case 2: // compiler
			DoAddHeaderCompiler(buffer);
			break;
		case 3: // debugger
			DoAddHeaderDebugger(buffer);
			break;
		case 4: // codecompl
			DoAddHeaderCodeCompletion(buffer);
			break;
		case 5: // mime
			DoAddHeaderMime(buffer);
			break;
		case 6: // wizard
			DoAddHeaderWizard(buffer);
			break;
	}
	if (type != 6) // wizard
	{
        buffer << _T('\t') << _T('\t') << _T("void OnAttach(); // fires when the plugin is attached to the application") << _T('\n');
        buffer << _T('\t') << _T('\t') << _T("void OnRelease(bool appShutDown); // fires when the plugin is released from the application") << _T('\n');
	}
	buffer << _T('\t') << _T("protected:") << _T('\n');
	buffer << _T('\t') << _T("private:") << _T('\n');
	if (hasMenu || hasModuleMenu || hasToolbar)
		buffer << _T('\t') << _T('\t') << _T("DECLARE_EVENT_TABLE()") << _T('\n');
	buffer << _T("};") << _T('\n');
	buffer << _T('\n');
	buffer << _T("// Declare the plugin's hooks") << _T('\n');
	buffer << _T("CB_DECLARE_PLUGIN();") << _T('\n');

	if (GuardBlock)
	{
		buffer << _T('\n');
		buffer << _T("#endif // ") << GuardWord << _T('\n');
	}
	buffer << _T('\n');
	// write buffer to disk
	wxFile hdr(UnixFilename(m_Header), wxFile::write);
	cbWrite(hdr,buffer);
	// end of header file

	// now the implementation file
	buffer.Clear();
	buffer << _T("/***************************************************************") << _T('\n');
	buffer << _T(" * Name:      ") << implFname.GetFullName() << _T('\n');
	buffer << _T(" * Purpose:   Code::Blocks plugin") << _T('\n');
	buffer << _T(" * Author:    ") << m_Info.author << _T("<") << m_Info.authorEmail << _T(">") << _T('\n');
//	buffer << _T(" * Created:   ") << wxDateTime::Now().Format(_("%c"), wxDateTime::Local) << _T('\n');
	buffer << _T(" * Copyright: (c) ") << m_Info.author <<  _T('\n');
	buffer << _T(" * License:   GPL") << _T('\n');
	buffer << _T(" **************************************************************/") << _T('\n');
	buffer << _T('\n');
	buffer << _T("#include <sdk.h> // PCH support") << _T('\n');
	buffer << _T("#include \"") << headerFname.GetFullName() << _T("\"") << _T('\n');
	buffer << _T("#include <licenses.h> // defines some common licenses (like the GPL)") << _T('\n');
	if (type == 6) // wizard
	{
        buffer << _T("#include <wx/bitmap.h>") << _T('\n');
        buffer << _T("#include <configmanager.h>") << _T('\n');
	}
	buffer << _T('\n');
	buffer << _T("// Implement the plugin's hooks") << _T('\n');
	buffer << _T("CB_IMPLEMENT_PLUGIN(") << m_Info.name << _T(", \"") <<
                                            (!m_Info.title.IsEmpty() ? m_Info.title : m_Info.name) <<
                                            _T("\");") << _T('\n');
	buffer << _T('\n');
	if (hasMenu || hasModuleMenu || hasToolbar)
	{
		buffer << _T("BEGIN_EVENT_TABLE(") << m_Info.name << _T(", ");
		switch (type)
		{
			case 0: // generic
				buffer << _T("cbPlugin)") << _T('\n');
				break;
			case 2: // compiler
				buffer << _T("cbCompilerPlugin)") << _T('\n');
				break;
			case 3: // debugger
				buffer << _T("cbDebuggerPlugin)") << _T('\n');
				break;
			case 4: // codecompl
				buffer << _T("cbCodeCompletionPlugin)") << _T('\n');
				break;
		}
		buffer << _T('\t') << _T("// add events here...") << _T('\n');
		buffer << _T("END_EVENT_TABLE()") << _T('\n');
		buffer << _T('\n');
	}
	buffer << m_Info.name << _T("::") << m_Info.name << _T("()") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//ctor") << _T('\n');
    buffer << _T('\t') << _T("m_PluginInfo.name = _T(\"") << m_Info.name << _T("\");") << _T('\n');
    buffer << _T('\t') << _T("m_PluginInfo.title = _(\"") << m_Info.title << _T("\");") << _T('\n');
    buffer << _T('\t') << _T("m_PluginInfo.version = _T(\"") << m_Info.version << _T("\");") << _T('\n');
    buffer << _T('\t') << _T("m_PluginInfo.description = _(\"") << m_Info.description << _T("\");") << _T('\n');
    buffer << _T('\t') << _T("m_PluginInfo.author = _T(\"") << m_Info.author << _T("\");") << _T('\n');
    buffer << _T('\t') << _T("m_PluginInfo.authorEmail = _T(\"") << m_Info.authorEmail << _T("\");") << _T('\n');
    buffer << _T('\t') << _T("m_PluginInfo.authorWebsite = _T(\"") << m_Info.authorWebsite << _T("\");") << _T('\n');
    buffer << _T('\t') << _T("m_PluginInfo.thanksTo = _(\"") << m_Info.thanksTo << _T("\");") << _T('\n');
    buffer << _T('\t') << _T("m_PluginInfo.license = LICENSE_GPL;") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
	buffer << m_Info.name << _T("::~") << m_Info.name << _T("()") << _T('\n');
	buffer << _T("{") << _T('\n');
	buffer << _T('\t') << _T("//dtor") << _T('\n');
	buffer << _T("}") << _T('\n');
	buffer << _T('\n');
	if (type != 6) // wizard
	{
        buffer << _T("void ") << m_Info.name << _T("::OnAttach()") << _T('\n');
        buffer << _T("{") << _T('\n');
        buffer << _T('\t') << _T("// do whatever initialization you need for your plugin") << _T('\n');
        buffer << _T('\t') << _T("// NOTE: after this function, the inherited member variable") << _T('\n');
        buffer << _T('\t') << _T("// m_IsAttached will be TRUE...") << _T('\n');
        buffer << _T('\t') << _T("// You should check for it in other functions, because if it") << _T('\n');
        buffer << _T('\t') << _T("// is FALSE, it means that the application did *not* \"load\"") << _T('\n');
        buffer << _T('\t') << _T("// (see: does not need) this plugin...") << _T('\n');
        buffer << _T("}") << _T('\n');
        buffer << _T('\n');
        buffer << _T("void ") << m_Info.name << _T("::OnRelease(bool appShutDown)") << _T('\n');
        buffer << _T("{") << _T('\n');
        buffer << _T('\t') << _T("// do de-initialization for your plugin") << _T('\n');
        buffer << _T('\t') << _T("// if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,") << _T('\n');
        buffer << _T('\t') << _T("// which means you must not use any of the SDK Managers") << _T('\n');
        buffer << _T('\t') << _T("// NOTE: after this function, the inherited member variable") << _T('\n');
        buffer << _T('\t') << _T("// m_IsAttached will be FALSE...") << _T('\n');
        buffer << _T("}") << _T('\n');
        buffer << _T('\n');
	}
	if (hasConfigure)
	{
		buffer << _T("int ") << m_Info.name << _T("::Configure()") << _T('\n');
		buffer << _T("{") << _T('\n');
		buffer << _T('\t') << _T("//create and display the configuration dialog for your plugin") << _T('\n');
        buffer << _T('\t') << _T("cbConfigurationDialog dlg(Manager::Get()->GetAppWindow(), wxID_ANY, _(\"Your dialog title\"));") << _T('\n');
        buffer << _T('\t') << _T("cbConfigurationPanel* panel = GetConfigurationPanel(&dlg);") << _T('\n');
        buffer << _T('\t') << _T("if (panel)") << _T('\n');
        buffer << _T('\t') << _T("{") << _T('\n');
        buffer << _T('\t') << _T('\t') << _T("dlg.AttachConfigurationPanel(panel);") << _T('\n');
        buffer << _T('\t') << _T('\t') << _T("PlaceWindow(&dlg);") << _T('\n');
        buffer << _T('\t') << _T('\t') << _T("return dlg.ShowModal() == wxID_OK ? 0 : -1;") << _T('\n');
        buffer << _T('\t') << _T("}") << _T('\n');
		buffer << _T('\t') << _T("return -1;") << _T('\n');
		buffer << _T("}") << _T('\n');
		buffer << _T('\n');
		buffer << _T("cbConfigurationPanel* ") << m_Info.name << _T("::GetConfigurationPanel(wxWindow* parent)") << _T('\n');
		buffer << _T("{") << _T('\n');
		buffer << _T('\t') << _T("//return the configuration panel for your plugin") << _T('\n');
		buffer << _T('\t') << _T("NotImplemented(_T(\"") << m_Info.name << _T("::GetConfigurationPanel()") << _T("\"));") << _T('\n');
		buffer << _T('\t') << _T("return 0;") << _T('\n');
		buffer << _T("}") << _T('\n');
		buffer << _T('\n');
	}
	if (type != 1 && type != 5) // not cbToolPlugin and not cbMimePlugin
	{
        if (hasMenu)
        {
            buffer << _T("void ") << m_Info.name << _T("::BuildMenu(wxMenuBar* menuBar)") << _T('\n');
            buffer << _T("{") << _T('\n');
            buffer << _T('\t') << _T("//The application is offering its menubar for your plugin,") << _T('\n');
            buffer << _T('\t') << _T("//to add any menu items you want...") << _T('\n');
            buffer << _T('\t') << _T("//Append any items you need in the menu...") << _T('\n');
            buffer << _T('\t') << _T("//NOTE: Be careful in here... The application's menubar is at your disposal.") << _T('\n');
            buffer << _T('\t') << _T("NotImplemented(_T(\"") << m_Info.name << _T("::BuildMenu()") << _T("\"));") << _T('\n');
            buffer << _T("}") << _T('\n');
            buffer << _T('\n');
        }
        if (hasModuleMenu)
        {
            buffer << _T("void ") << m_Info.name << _T("::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)") << _T('\n');
            buffer << _T("{") << _T('\n');
            buffer << _T('\t') << _T("//Some library module is ready to display a pop-up menu.") << _T('\n');
            buffer << _T('\t') << _T("//Check the parameter \"type\" and see which module it is") << _T('\n');
            buffer << _T('\t') << _T("//and append any items you need in the menu...") << _T('\n');
            buffer << _T('\t') << _T("//TIP: for consistency, add a separator as the first item...") << _T('\n');
            buffer << _T('\t') << _T("NotImplemented(_T(\"") << m_Info.name << _T("::BuildModuleMenu()") << _T("\"));") << _T('\n');
            buffer << _T("}") << _T('\n');
            buffer << _T('\n');
        }
        if (hasToolbar)
        {
            buffer << _T("bool ") << m_Info.name << _T("::BuildToolBar(wxToolBar* toolBar)") << _T('\n');
            buffer << _T("{") << _T('\n');
            buffer << _T('\t') << _T("//The application is offering its toolbar for your plugin,") << _T('\n');
            buffer << _T('\t') << _T("//to add any toolbar items you want...") << _T('\n');
            buffer << _T('\t') << _T("//Append any items you need on the toolbar...") << _T('\n');
            buffer << _T('\t') << _T("NotImplemented(_T(\"") << m_Info.name << _T("::BuildToolBar()") << _T("\"));") << _T('\n');
            buffer << _T('\t') << _T("// return true if you add toolbar items") << _T('\n');
            buffer << _T('\t') << _T("return false;") << _T('\n');
            buffer << _T("}") << _T('\n');
            buffer << _T('\n');
        }
	}
	switch (type)
	{
		case 1: // tool
			DoAddSourceTool(m_Info.name, buffer);
			break;
		case 2: // compiler
			DoAddSourceCompiler(m_Info.name, buffer);
			break;
		case 3: // debugger
			DoAddSourceDebugger(m_Info.name, buffer);
			break;
		case 4: // codecompl
			DoAddSourceCodeCompletion(m_Info.name, buffer);
			break;
		case 5: // mime
			DoAddSourceMime(m_Info.name, buffer);
			break;
		case 6: // wizard
			DoAddSourceWizard(m_Info.name, buffer);
			break;
	}
	// write buffer to disk
	wxFile impl(UnixFilename(m_Implementation), wxFile::write);
	cbWrite(impl,buffer);
	// end of implementation file
}
