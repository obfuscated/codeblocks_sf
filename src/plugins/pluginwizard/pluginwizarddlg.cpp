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

#include "pluginwizarddlg.h"
#include "enterinfodlg.h"
#include <wx/intl.h>
#include <wx/mdi.h>
#include <wx/filename.h>
#include <wx/xrc/xmlres.h>
#include <manager.h>
#include <wx/file.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>

BEGIN_EVENT_TABLE(PluginWizardDlg, wxDialog)
	EVT_UPDATE_UI(-1, PluginWizardDlg::OnUpdateUI)
	EVT_BUTTON(XRCID("btnInfo"), PluginWizardDlg::OnEditInfoClick)
	EVT_BUTTON(XRCID("btnOK"), PluginWizardDlg::OnOKClick)
	EVT_TEXT(XRCID("txtName"), PluginWizardDlg::OnNameChange)
END_EVENT_TABLE()

PluginWizardDlg::PluginWizardDlg()
{
	wxXmlResource::Get()->LoadDialog(this, Manager::Get()->GetAppWindow(), _("dlgNewPlugin"));
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
	GuardWord.Replace(".", "_");
	XRCCTRL(*this, "txtGuardBlock", wxTextCtrl)->SetValue(GuardWord);
}


void PluginWizardDlg::DoAddHeaderOption(wxString& buffer, bool has, const wxString& retVal)
{
	if (has)
		buffer << ";" << '\n';
	else
		buffer << "{ return" << retVal << "; }" << '\n';
}

void PluginWizardDlg::DoAddHeaderTool(wxString& buffer)
{
	buffer << '\t' << '\t' << "int Execute();" << '\n';
}

void PluginWizardDlg::DoAddHeaderCompiler(wxString& buffer)
{
	buffer << '\t' << '\t' << "int Run(ProjectBuildTarget* target = 0L);" << '\n';
	buffer << '\t' << '\t' << "int Clean(ProjectBuildTarget* target = 0L);" << '\n';
	buffer << '\t' << '\t' << "int Compile(ProjectBuildTarget* target = 0L);" << '\n';
	buffer << '\t' << '\t' << "int CompileAll();" << '\n';
	buffer << '\t' << '\t' << "int RebuildAll();" << '\n';
	buffer << '\t' << '\t' << "int Rebuild(ProjectBuildTarget* target = 0L);" << '\n';
	buffer << '\t' << '\t' << "int CompileFile(const wxString& file);" << '\n';
	buffer << '\t' << '\t' << "int KillProcess();" << '\n';
	buffer << '\t' << '\t' << "bool IsRunning();" << '\n';
	buffer << '\t' << '\t' << "int GetExitCode();" << '\n';
}

void PluginWizardDlg::DoAddHeaderDebugger(wxString& buffer)
{
	buffer << '\t' << '\t' << "int Debug();" << '\n';
	buffer << '\t' << '\t' << "void CmdContinue();" << '\n';
	buffer << '\t' << '\t' << "void CmdNext();" << '\n';
	buffer << '\t' << '\t' << "void CmdStep();" << '\n';
	buffer << '\t' << '\t' << "void CmdToggleBreakpoint();" << '\n';
	buffer << '\t' << '\t' << "void CmdStop();" << '\n';
	buffer << '\t' << '\t' << "bool IsRunning();" << '\n';
	buffer << '\t' << '\t' << "int GetExitCode();" << '\n';
}

void PluginWizardDlg::DoAddHeaderCodeCompletion(wxString& buffer)
{
	buffer << '\t' << '\t' << "wxArrayString GetCallTipsFor(const wxString& command);" << '\n';
	buffer << '\t' << '\t' << "int CodeComplete();" << '\n';
	buffer << '\t' << '\t' << "void ShowCallTip();" << '\n';
}

void PluginWizardDlg::DoAddSourceTool(const wxString& classname, wxString& buffer)
{
	buffer << "int " << m_Info.name << "::Execute()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//do your magic ;)" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::Execute()" << "\");" << '\n';
	buffer << '\t' << "return -1;" << '\n';
	buffer << "}" << '\n';
	buffer << '\n';
}

void PluginWizardDlg::DoAddSourceCompiler(const wxString& classname, wxString& buffer)
{
	buffer << "int " << classname << "::Run(ProjectBuildTarget* target = 0L)" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//run the active project or specified target" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::Run()" << "\");" << '\n';
	buffer << '\t' << "return -1;" << '\n';
	buffer << "}" << '\n';
	buffer << "int " << classname << "::Clean(ProjectBuildTarget* target = 0L)" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//clean the active project or specified target" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::Clean()" << "\");" << '\n';
	buffer << '\t' << "return -1;" << '\n';
	buffer << "}" << '\n';
	buffer << "int " << classname << "::Compile(ProjectBuildTarget* target = 0L)" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//compile the active project or specified target" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::Compile()" << "\");" << '\n';
	buffer << '\t' << "return -1;" << '\n';
	buffer << "}" << '\n';
	buffer << "int " << classname << "::CompileAll()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//compile all projects" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::CompileAll()" << "\");" << '\n';
	buffer << '\t' << "return -1;" << '\n';
	buffer << "}" << '\n';
	buffer << "int " << classname << "::RebuildAll()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//rebuild all projects" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::RebuildAll()" << "\");" << '\n';
	buffer << '\t' << "return -1;" << '\n';
	buffer << "}" << '\n';
	buffer << "int " << classname << "::Rebuild(ProjectBuildTarget* target = 0L)" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//rebuild the active project or specified target" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::Rebuild()" << "\");" << '\n';
	buffer << '\t' << "return -1;" << '\n';
	buffer << "}" << '\n';
	buffer << "int " << classname << "::CompileFile(const wxString& file)" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//compile only the specified file" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::CompileFile()" << "\");" << '\n';
	buffer << '\t' << "return -1;" << '\n';
	buffer << "}" << '\n';
	buffer << "int " << classname << "::KillProcess()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//end compiler process" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::KillProcess()" << "\");" << '\n';
	buffer << '\t' << "return -1;" << '\n';
	buffer << "}" << '\n';
	buffer << "bool " << classname << "::IsRunning()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//return true if session is active" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::IsRunning()" << "\");" << '\n';
	buffer << '\t' << "return false;" << '\n';
	buffer << "}" << '\n';
	buffer << "int " << classname << "::GetExitCode()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//return last session exit code" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::GetExitCode()" << "\");" << '\n';
	buffer << '\t' << "return -1;" << '\n';
	buffer << "}" << '\n';
}

void PluginWizardDlg::DoAddSourceDebugger(const wxString& classname, wxString& buffer)
{
	buffer << "int " << classname << "::Debug()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//actual debugging session starts here" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::Debug()" << "\");" << '\n';
	buffer << '\t' << "return -1;" << '\n';
	buffer << "}" << '\n';
	buffer << "void " << classname << "::CmdContinue()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//tell debugger to continue" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::CmdContinue()" << "\");" << '\n';
	buffer << "}" << '\n';
	buffer << "void " << classname << "::CmdNext()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//tell debugger to step one line of code" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::CmdNext()" << "\");" << '\n';
	buffer << "}" << '\n';
	buffer << "void " << classname << "::CmdStep()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//tell debugger to step one instruction (following inside functions, if needed)" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::CmdStep()" << "\");" << '\n';
	buffer << "}" << '\n';
	buffer << "void " << classname << "::CmdStop()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//tell debugger to end debugging session" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::CmdStop()" << "\");" << '\n';
	buffer << "}" << '\n';
	buffer << "bool " << classname << "::IsRunning()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//return true if session is active" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::IsRunning()" << "\");" << '\n';
	buffer << '\t' << "return false;" << '\n';
	buffer << "}" << '\n';
	buffer << "int " << classname << "::GetExitCode()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//return last session exit code" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::GetExitCode()" << "\");" << '\n';
	buffer << '\t' << "return -1;" << '\n';
	buffer << "}" << '\n';
}

void PluginWizardDlg::DoAddSourceCodeCompletion(const wxString& classname, wxString& buffer)
{
	buffer << "wxArrayString " << classname << "::GetCallTipsFor(const wxString& command)" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//well, give call tips for the string \"command\"..." << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::GetCallTipsFor()" << "\");" << '\n';
	buffer << '\t' << "wxArrayString items;" << '\n';
	buffer << '\t' << "return items;" << '\n';
	buffer << "}" << '\n';
	buffer << '\n';
	buffer << "int " << classname << "::CodeComplete()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//code-complete" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::CodeComplete()" << "\");" << '\n';
	buffer << '\t' << "return -1;" << '\n';
	buffer << "}" << '\n';
	buffer << '\n';
	buffer << "void " << classname << "::ShowCallTip()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//display a call-tip" << '\n';
	buffer << '\t' << "NotImplemented(\"" << classname << "::ShowCallTip()" << "\");" << '\n';
	buffer << "}" << '\n';
	buffer << '\n';
}

// events

void PluginWizardDlg::OnNameChange(wxCommandEvent& event)
{
	wxString name = XRCCTRL(*this, "txtName", wxTextCtrl)->GetValue();
	name.MakeLower();
	XRCCTRL(*this, "txtHeader", wxTextCtrl)->SetValue(name + ".h");
	XRCCTRL(*this, "txtImplementation", wxTextCtrl)->SetValue(name + ".cpp");
	DoGuardBlock();
}

void PluginWizardDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
	bool isTool = XRCCTRL(*this, "cmbType", wxComboBox)->GetSelection() == 1;
	XRCCTRL(*this, "chkHasMenu", wxCheckBox)->Enable(!isTool);
	XRCCTRL(*this, "chkHasModuleMenu", wxCheckBox)->Enable(!isTool);
	XRCCTRL(*this, "chkHasToolbar", wxCheckBox)->Enable(!isTool);
}

void PluginWizardDlg::OnEditInfoClick(wxCommandEvent& event)
{
	EnterInfoDlg dlg(m_Info);
	if (dlg.ShowModal() == wxID_OK)
		m_Info = dlg.GetInfo();
}

void PluginWizardDlg::OnOKClick(wxCommandEvent& event)
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
	wxFileName headerFname(m_Header);
	wxFileName implFname(m_Implementation);
	
	wxString buffer;

	// actual creation starts here
	// let's start with the header file
	buffer << "/***************************************************************" << '\n';
	buffer << " * Name:      " << headerFname.GetFullName() << '\n';
	buffer << " * Purpose:   Code::Blocks plugin" << '\n';
	buffer << " * Author:    " << m_Info.author << "<" << m_Info.authorEmail << ">" << '\n';
	buffer << " * Created:   " << wxDateTime::Now().Format("%c", wxDateTime::Local) << '\n';
	buffer << " * Copyright: (c) " << m_Info.author <<  '\n';
	buffer << " * License:   GPL" << '\n';
	buffer << " **************************************************************/" << '\n';
	buffer << '\n';

	if (GuardBlock)
	{
		buffer << "#ifndef " << GuardWord << '\n';
		buffer << "#define " << GuardWord << '\n';
		buffer << '\n';
	}

	buffer << "#if defined(__GNUG__) && !defined(__APPLE__)" << '\n';
	buffer << "\t#pragma interface \"" << headerFname.GetFullName() << "\"" << '\n';
	buffer << "#endif" << '\n';

	buffer << "// For compilers that support precompilation, includes <wx/wx.h>" << '\n';
	buffer << "#include <wx/wxprec.h>" << '\n';
	buffer << '\n';
	
	buffer << "#ifdef __BORLANDC__" << '\n';
	buffer << "\t#pragma hdrstop" << '\n';
	buffer << "#endif" << '\n';
	buffer << '\n';
	
	buffer << "#ifndef WX_PRECOMP" << '\n';
	buffer << "\t#include <wx/wx.h>" << '\n';
	buffer << "#endif" << '\n';
	buffer << '\n';

	buffer << "#include <cbplugin.h> // the base class we 're inheriting" << '\n';
	buffer << "#include <settings.h> // needed to use the Code::Blocks SDK" << '\n';
	buffer << '\n';
	buffer << "class " << m_Info.name;
	switch (type)
	{
		case 0: buffer << " : public cbPlugin"; break;
		case 1: buffer << " : public cbToolPlugin"; break;
		case 2: buffer << " : public cbCompilerPlugin"; break;
		case 3: buffer << " : public cbDebuggerPlugin"; break;
		case 4: buffer << " : public cbCodeCompletionPlugin"; break;
	}
	buffer << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "public:" << '\n';
	buffer << '\t' << '\t' << m_Info.name << "();" << '\n';
	buffer << '\t' << '\t' << "~" << m_Info.name << "();" << '\n';
	
	buffer << '\t' << '\t' << "int Configure()";
	DoAddHeaderOption(buffer, hasConfigure);
	buffer << '\t' << '\t' << "void BuildMenu(wxMenuBar* menuBar)";
	DoAddHeaderOption(buffer, hasMenu, wxEmptyString);
	buffer << '\t' << '\t' << "void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg)";
	DoAddHeaderOption(buffer, hasModuleMenu, wxEmptyString);
	buffer << '\t' << '\t' << "void BuildToolBar(wxToolBar* toolBar)";
	DoAddHeaderOption(buffer, hasToolbar, wxEmptyString);
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
	}
	buffer << '\t' << '\t' << "void OnAttach(); // fires when the plugin is attached to the application" << '\n';
	buffer << '\t' << '\t' << "void OnRelease(bool appShutDown); // fires when the plugin is released from the application" << '\n';
	buffer << '\t' << "protected:" << '\n';
	buffer << '\t' << "private:" << '\n';
	if (hasMenu || hasModuleMenu || hasToolbar)
		buffer << '\t' << '\t' << "DECLARE_EVENT_TABLE()" << '\n';
	buffer << "};" << '\n';
	buffer << '\n';
	buffer << "extern \"C\"" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "PLUGIN_EXPORT cbPlugin* GetPlugin();" << '\n';
	buffer << "};" << '\n';

	if (GuardBlock)
	{
		buffer << '\n';
		buffer << "#endif // " << GuardWord << '\n';
	}
	buffer << '\n';
	// write buffer to disk
	wxFile hdr(m_Header, wxFile::write);
	hdr.Write(buffer, buffer.Length());
	hdr.Flush();
	// end of header file
	
	// now the implementation file
	buffer.Clear();
	buffer << "/***************************************************************" << '\n';
	buffer << " * Name:      " << implFname.GetFullName() << '\n';
	buffer << " * Purpose:   Code::Blocks plugin" << '\n';
	buffer << " * Author:    " << m_Info.author << "<" << m_Info.authorEmail << ">" << '\n';
	buffer << " * Created:   " << wxDateTime::Now().Format("%c", wxDateTime::Local) << '\n';
	buffer << " * Copyright: (c) " << m_Info.author <<  '\n';
	buffer << " * License:   GPL" << '\n';
	buffer << " **************************************************************/" << '\n';
	buffer << '\n';
	buffer << "#if defined(__GNUG__) && !defined(__APPLE__)" << '\n';
	buffer << "\t#pragma implementation \"" << headerFname.GetFullName() << "\"" << '\n';
	buffer << "#endif" << '\n';
	buffer << '\n';
	buffer << "#include \"" << headerFname.GetFullName() << "\"" << '\n';
	buffer << "#include <licenses.h> // defines some common licenses (like the GPL)" << '\n';
	buffer << '\n';
	buffer << "cbPlugin* GetPlugin()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "return new " << m_Info.name << ";" << '\n';
	buffer << "}" << '\n';
	buffer << '\n';
	if (hasMenu || hasModuleMenu || hasToolbar)
	{
		buffer << "BEGIN_EVENT_TABLE(" << m_Info.name << ", ";
		switch (type)
		{
			case 0: // generic
				buffer << "cbPlugin)" << '\n';
				break;
			case 1: // tool
				buffer << "cbToolPlugin)" << '\n';
				break;
			case 2: // compiler
				buffer << "cbCompilerPlugin)" << '\n';
				break;
			case 3: // debugger
				buffer << "cbDebuggerPlugin)" << '\n';
				break;
			case 4: // codecompl
				buffer << "cbCodeCompletionPlugin)" << '\n';
				break;
		}
		buffer << '\t' << "// add events here..." << '\n';
		buffer << "END_EVENT_TABLE()" << '\n';
		buffer << '\n';
	}
	buffer << m_Info.name << "::" << m_Info.name << "()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//ctor" << '\n';
    buffer << '\t' << "m_PluginInfo.name = \"" << m_Info.name << "\";" << '\n';
    buffer << '\t' << "m_PluginInfo.title = \"" << m_Info.title << "\";" << '\n';
    buffer << '\t' << "m_PluginInfo.version = \"" << m_Info.version << "\";" << '\n';
    buffer << '\t' << "m_PluginInfo.description = \"" << m_Info.description << "\";" << '\n';
    buffer << '\t' << "m_PluginInfo.author = \"" << m_Info.author << "\";" << '\n';
    buffer << '\t' << "m_PluginInfo.authorEmail = \"" << m_Info.authorEmail << "\";" << '\n';
    buffer << '\t' << "m_PluginInfo.authorWebsite = \"" << m_Info.authorWebsite << "\";" << '\n';
    buffer << '\t' << "m_PluginInfo.thanksTo = \"" << m_Info.thanksTo << "\";" << '\n';
    buffer << '\t' << "m_PluginInfo.license = LICENSE_GPL;" << '\n';
	buffer << '\t' << "m_PluginInfo.hasConfigure = " << (hasConfigure ? "true;" : "false;") << '\n';
	buffer << "}" << '\n';
	buffer << '\n';
	buffer << m_Info.name << "::~" << m_Info.name << "()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//dtor" << '\n';
	buffer << "}" << '\n';
	buffer << '\n';
	buffer << "void " << m_Info.name << "::OnAttach()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "// do whatever initialization you need for your plugin" << '\n';
	buffer << '\t' << "// NOTE: after this function, the inherited member variable" << '\n';
	buffer << '\t' << "// m_IsAttached will be TRUE..." << '\n';
	buffer << '\t' << "// You should check for it in other functions, because if it" << '\n';
	buffer << '\t' << "// is FALSE, it means that the application did *not* \"load\"" << '\n';
	buffer << '\t' << "// (see: does not need) this plugin..." << '\n';
	buffer << "}" << '\n';
	buffer << '\n';
	buffer << "void " << m_Info.name << "::OnRelease(bool appShutDown)" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "// do de-initialization for your plugin" << '\n';
	buffer << '\t' << "// if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down," << '\n';
	buffer << '\t' << "// which means you must not use any of the SDK Managers" << '\n';
	buffer << '\t' << "// NOTE: after this function, the inherited member variable" << '\n';
	buffer << '\t' << "// m_IsAttached will be FALSE..." << '\n';
	buffer << "}" << '\n';
	buffer << '\n';
	if (hasConfigure)
	{
		buffer << "int " << m_Info.name << "::Configure()" << '\n';
		buffer << "{" << '\n';
		buffer << '\t' << "//create and display the configuration dialog for your plugin" << '\n';
		buffer << '\t' << "NotImplemented(\"" << m_Info.name << "::Configure()" << "\");" << '\n';
		buffer << '\t' << "return -1;" << '\n';
		buffer << "}" << '\n';
		buffer << '\n';
	}
	if (hasMenu)
	{
		buffer << "void " << m_Info.name << "::BuildMenu(wxMenuBar* menuBar)" << '\n';
		buffer << "{" << '\n';
		buffer << '\t' << "//The application is offering its menubar for your plugin," << '\n';
		buffer << '\t' << "//to add any menu items you want..." << '\n';
		buffer << '\t' << "//Append any items you need in the menu..." << '\n';
		buffer << '\t' << "//NOTE: Be careful in here... The application's menubar is at your disposal." << '\n';
		buffer << '\t' << "//HINT: Keep a variable pointing to every menu item you create," << '\n';
		buffer << '\t' << "//      so that you can delete it in the OnRelease() event..." << '\n';
		buffer << '\t' << "NotImplemented(\"" << m_Info.name << "::OfferMenuSpace()" << "\");" << '\n';
		buffer << "}" << '\n';
		buffer << '\n';
	}
	if (hasModuleMenu)
	{
		buffer << "void " << m_Info.name << "::BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg)" << '\n';
		buffer << "{" << '\n';
		buffer << '\t' << "//Some library module is ready to display a pop-up menu." << '\n';
		buffer << '\t' << "//Check the parameter \"type\" and see which module it is" << '\n';
		buffer << '\t' << "//and append any items you need in the menu..." << '\n';
		buffer << '\t' << "//TIP: for consistency, add a separator as the first item..." << '\n';
		buffer << '\t' << "NotImplemented(\"" << m_Info.name << "::OfferModuleMenuSpace()" << "\");" << '\n';
		buffer << "}" << '\n';
		buffer << '\n';
	}
	if (hasToolbar)
	{
		buffer << "void " << m_Info.name << "::BuildToolBar(wxToolBar* toolBar)" << '\n';
		buffer << "{" << '\n';
		buffer << '\t' << "//The application is offering its toolbar for your plugin," << '\n';
		buffer << '\t' << "//to add any toolbar items you want..." << '\n';
		buffer << '\t' << "//Append any items you need on the toolbar..." << '\n';
		buffer << '\t' << "//" << '\n';
		buffer << '\t' << "//An example follows for a toolbar button:" << '\n';
		buffer << '\t' << "//" << '\t' << "PluginToolbarArray* m_ToolsArray;" << '\n';
		buffer << '\t' << "//" << '\t' << "PluginToolbarItem* tool;" << '\n';
		buffer << '\t' << "//" << '\n';
		buffer << '\t' << "//" << '\t' << "tool = new PluginToolbarItem();" << '\n';
		buffer << '\t' << "//" << '\t' << "tool->type = ptitButton;" << '\n';
		buffer << '\t' << "//" << '\t' << "tool->id = idSomeID;" << '\n';
		buffer << '\t' << "//" << '\t' << "tool->image << \"some_image.bmp\";" << '\n';
		buffer << '\t' << "//" << '\t' << "m_ToolsArray->Add(tool);" << '\n';
		buffer << '\t' << "NotImplemented(\"" << m_Info.name << "::BuildToolBar()" << "\");" << '\n';
		buffer << '\t' << "return;" << '\n';
		buffer << "}" << '\n';
		buffer << '\n';
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
	}
	// write buffer to disk
	wxFile impl(m_Implementation, wxFile::write);
	impl.Write(buffer, buffer.Length());
	impl.Flush();
	// end of implementation file
	
	EndModal(wxID_OK);
}
