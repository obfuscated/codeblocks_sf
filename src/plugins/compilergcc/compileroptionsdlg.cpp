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

#include "compileroptionsdlg.h"
#include "compilergcc.h"
#include "advancedcompileroptionsdlg.h"
#include <wx/xrc/xmlres.h>
#include <manager.h>
#include <configmanager.h>
#include <messagemanager.h>
#include <projectmanager.h>

BEGIN_EVENT_TABLE(CompilerOptionsDlg, wxDialog)
    EVT_UPDATE_UI(			XRCID("btnEditIncludes"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnDelIncludes"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnEditLibs"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnDelLibs"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnEditVar"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnDeleteVar"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("cmbCompilerPolicy"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("cmbLinkerPolicy"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("cmbIncludesPolicy"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("cmbLibsPolicy"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnAddCompiler"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnRenameCompiler"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnDelCompiler"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("txtMasterPath"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnMasterPath"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("txtCcompiler"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnCcompiler"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("txtCPPcompiler"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnCPPcompiler"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("txtLinker"),			CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnLinker"),			CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("txtDebugger"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnDebugger"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("txtResComp"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnResComp"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("txtMake"),			CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnMake"),			CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("cmbCompiler"),		CompilerOptionsDlg::OnUpdateUI)
    //
	EVT_TREE_SEL_CHANGED(	XRCID("tcScope"),			CompilerOptionsDlg::OnTreeSelectionChange)
	EVT_TREE_SEL_CHANGING(	XRCID("tcScope"),			CompilerOptionsDlg::OnTreeSelectionChanging)
	EVT_COMBOBOX(			XRCID("cmbCategory"), 		CompilerOptionsDlg::OnCategoryChanged)
	EVT_COMBOBOX(			XRCID("cmbCompiler"), 		CompilerOptionsDlg::OnCompilerChanged)
	EVT_LISTBOX_DCLICK(		XRCID("lstVars"),			CompilerOptionsDlg::OnEditVarClick)
	EVT_BUTTON(				XRCID("btnAddCompiler"),	CompilerOptionsDlg::OnAddCompilerClick)
	EVT_BUTTON(				XRCID("btnRenameCompiler"),	CompilerOptionsDlg::OnEditCompilerClick)
	EVT_BUTTON(				XRCID("btnDelCompiler"),	CompilerOptionsDlg::OnRemoveCompilerClick)
	EVT_BUTTON(				XRCID("btnAddIncludes"),	CompilerOptionsDlg::OnAddDirClick)
	EVT_BUTTON(				XRCID("btnEditIncludes"),	CompilerOptionsDlg::OnEditDirClick)
	EVT_BUTTON(				XRCID("btnDelIncludes"),	CompilerOptionsDlg::OnRemoveDirClick)
	EVT_BUTTON(				XRCID("btnAddLibs"),		CompilerOptionsDlg::OnAddDirClick)
	EVT_BUTTON(				XRCID("btnEditLibs"),		CompilerOptionsDlg::OnEditDirClick)
	EVT_BUTTON(				XRCID("btnDelLibs"),		CompilerOptionsDlg::OnRemoveDirClick)
	EVT_BUTTON(				XRCID("btnAddVar"),			CompilerOptionsDlg::OnAddVarClick)
	EVT_BUTTON(				XRCID("btnEditVar"),		CompilerOptionsDlg::OnEditVarClick)
	EVT_BUTTON(				XRCID("btnDeleteVar"),		CompilerOptionsDlg::OnRemoveVarClick)
	EVT_BUTTON(				XRCID("btnMasterPath"),		CompilerOptionsDlg::OnMasterPathClick)
	EVT_BUTTON(				XRCID("btnCcompiler"),		CompilerOptionsDlg::OnSelectProgramClick)
	EVT_BUTTON(				XRCID("btnCPPcompiler"),	CompilerOptionsDlg::OnSelectProgramClick)
	EVT_BUTTON(				XRCID("btnLinker"),		    CompilerOptionsDlg::OnSelectProgramClick)
	EVT_BUTTON(				XRCID("btnDebugger"),		CompilerOptionsDlg::OnSelectProgramClick)
	EVT_BUTTON(				XRCID("btnResComp"),		CompilerOptionsDlg::OnSelectProgramClick)
	EVT_BUTTON(				XRCID("btnMake"),		    CompilerOptionsDlg::OnSelectProgramClick)
	EVT_BUTTON(				XRCID("btnAdvanced"),		CompilerOptionsDlg::OnAdvancedClick)
END_EVENT_TABLE()

CompilerOptionsDlg::CompilerOptionsDlg(wxWindow* parent, CompilerGCC* compiler, cbProject* project, ProjectBuildTarget* target)
	: m_Compiler(compiler),
	m_InitialCompilerIdx(0),
	m_pProject(project),
	m_BuildingTree(false)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgCompilerOptions"));
	
	int compilerIdx = m_pProject ? m_pProject->GetCompilerIndex() : m_Compiler->GetCurrentCompilerIndex();
	if (!CompilerFactory::CompilerIndexOK(compilerIdx))
        compilerIdx = 0;
    m_Options = CompilerFactory::Compilers[compilerIdx]->GetOptions();
	
	DoFillCompilerSets();
	DoFillCompilerPrograms();
	DoFillPrograms();
	DoFillOthers();
	DoFillCategories();
	DoFillTree(project, target);

    wxTreeCtrl* tree = XRCCTRL(*this, "tcScope", wxTreeCtrl);
    wxSizer* sizer = tree->GetContainingSizer();
    if (!project)
    {
        // global settings
        SetTitle(_("Global compiler options"));
        sizer->Remove(tree);
        tree->Show(false);
	}
	else
	{
        m_InitialCompilerIdx = project->GetCompilerIndex();
        // project settings
        SetTitle(_("Project's compiler options"));

        wxNotebook* nb = XRCCTRL(*this, "nbMain", wxNotebook);
        nb->DeletePage(7); // remove "Other" page
        nb->DeletePage(5); // remove "Programs" page
        
        // remove "Compiler" buttons
        wxWindow* win = XRCCTRL(*this, "btnAddCompiler", wxButton);
        wxSizer* sizer2 = win->GetContainingSizer();
        sizer2->Remove(win);
        win->Show(false);
        win = XRCCTRL(*this, "btnRenameCompiler", wxButton);
        sizer2->Remove(win);
        win->Show(false);
        win = XRCCTRL(*this, "btnDelCompiler", wxButton);
        sizer2->Remove(win);
        win->Show(false);
        sizer2->Layout();
    }
    sizer->Layout();
}

CompilerOptionsDlg::~CompilerOptionsDlg()
{
	//dtor
}

void CompilerOptionsDlg::DoFillCompilerSets()
{
    wxComboBox* cmb = XRCCTRL(*this, "cmbCompiler", wxComboBox);
    cmb->Clear();
    for (unsigned int i = 0; i < CompilerFactory::Compilers.GetCount(); ++i)
    {
        cmb->Append(CompilerFactory::Compilers[i]->GetName());
    }
	int compilerIdx = m_pProject ? m_pProject->GetCompilerIndex() : m_Compiler->GetCurrentCompilerIndex();
    cmb->SetSelection(compilerIdx);
    m_LastCompilerIdx = compilerIdx;
}

void CompilerOptionsDlg::DoFillCompilerPrograms()
{
    if (m_pProject)
        return; // no "Programs" page

    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
    /*Manager::Get()->GetMessageManager()->DebugLog("compilerIdx=%d, m_LastCompilerIdx=%d", compilerIdx, m_LastCompilerIdx);
    if (compilerIdx != m_LastCompilerIdx)
    {
        // compiler changed; check for changes and update as needed
        DoUpdateCompiler();
    }*/
    
    m_LastCompilerIdx = compilerIdx;
    const CompilerPrograms& progs = CompilerFactory::Compilers[compilerIdx]->GetPrograms();
    
    XRCCTRL(*this, "txtMasterPath", wxTextCtrl)->SetValue(CompilerFactory::Compilers[compilerIdx]->GetMasterPath());
    XRCCTRL(*this, "txtCcompiler", wxTextCtrl)->SetValue(progs.C);
    XRCCTRL(*this, "txtCPPcompiler", wxTextCtrl)->SetValue(progs.CPP);
    XRCCTRL(*this, "txtLinker", wxTextCtrl)->SetValue(progs.LD);
    XRCCTRL(*this, "txtResComp", wxTextCtrl)->SetValue(progs.WINDRES);
    XRCCTRL(*this, "txtMake", wxTextCtrl)->SetValue(progs.MAKE);
}

void CompilerOptionsDlg::DoFillPrograms()
{
	const VarsArray& vars = m_Compiler->GetCustomVars().GetVars();
	wxListBox* lst = XRCCTRL(*this, "lstVars", wxListBox);
	lst->Clear();
	//Manager::Get()->GetMessageManager()->DebugLog("[0x%8.8x] Current var count is %d (0x%8.8x)", m_Compiler, vars.GetCount(), &vars);
	for (unsigned int i = 0; i < vars.GetCount(); ++i)
	{
		Var* v = &vars[i];
		if (!v->builtin)
		{
            wxString text = v->name + " = " + v->value;
            lst->Append(text, static_cast<void*>(v));
		}
	}
}

void CompilerOptionsDlg::DoFillOthers()
{
    wxCheckBox* chk = XRCCTRL(*this, "chkSimpleBuild", wxCheckBox);
    if (chk)
        chk->SetValue(ConfigManager::Get()->Read("/compiler_gcc/simple_build", 0L));
    wxTextCtrl* txt = XRCCTRL(*this, "txtConsoleShell", wxTextCtrl);
    if (txt)
    {
        txt->SetValue(ConfigManager::Get()->Read("/compiler_gcc/console_shell", DEFAULT_CONSOLE_SHELL));
#ifdef __WXMSW__
        // under win32, this option is not needed, so disable it
        txt->Enable(false);
#endif
    }
}

void CompilerOptionsDlg::DoFillTree(cbProject* focusProject, ProjectBuildTarget* focusTarget)
{
	m_BuildingTree = true;
	wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
	tc->DeleteAllItems();
	
	wxTreeItemId root;
	ScopeTreeData* selected = 0L;
	wxTreeItemId selectedItem;

	if (!focusProject)
	{
        // global settings
        root = tc->AddRoot(_("Global options"), -1, -1);
        selectedItem = root;
    }
    else
    {
        // project settings
        ScopeTreeData* data = new ScopeTreeData(focusProject, 0L);
        root = tc->AddRoot(focusProject->GetTitle(), -1, -1, data);
        selectedItem = root;
        selected = data;
        for (int x = 0; x < focusProject->GetBuildTargetsCount(); ++x)
        {
            ProjectBuildTarget* target = focusProject->GetBuildTarget(x);
            data = new ScopeTreeData(focusProject, target);
            wxTreeItemId targetItem = tc->AppendItem(root, target->GetTitle(), -1, -1, data);
            if (target == focusTarget)
            {
                selected = data;
                selectedItem = targetItem;
            }
        }
    }
    tc->Expand(root);
    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
    DoLoadOptions(compilerIdx, selected);
    tc->SelectItem(selectedItem);
    m_BuildingTree = false;
}

void CompilerOptionsDlg::DoFillCategories()
{
	wxComboBox* cmb = XRCCTRL(*this, "cmbCategory", wxComboBox);
	cmb->Clear();
	cmb->Append(_("<All categories>"));

	for (unsigned int i = 0; i < m_Options.GetCount(); ++i)
	{
		CompOption* copt = m_Options.GetOption(i);
		if (cmb->FindString(copt->category) == -1)
			cmb->Append(copt->category);
	}
	cmb->SetSelection(0);
}

void CompilerOptionsDlg::DoFillOptions()
{
	Disconnect(XRCID("lstCompilerOptions"), -1,
			wxEVT_COMMAND_CHECKLISTBOX_TOGGLED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
			&CompilerOptionsDlg::OnOptionToggled);

	wxComboBox* cmb = XRCCTRL(*this, "cmbCategory", wxComboBox);
	wxString cat = cmb->GetValue();
	bool isAll = cmb->GetSelection() == 0;
	wxCheckListBox* list = XRCCTRL(*this, "lstCompilerOptions", wxCheckListBox);
	list->Clear();
	
	for (unsigned int i = 0; i < m_Options.GetCount(); ++i)
	{
		CompOption* copt = m_Options.GetOption(i);
		if (isAll || copt->category.Matches(cat))
		{
			list->Append(copt->name);
			list->Check(list->GetCount() - 1, copt->enabled);
//            Manager::Get()->GetMessageManager()->DebugLog("(FILL) option %s (0x%8.8x) %s", copt->option.c_str(), copt, copt->enabled ? "enabled" : "disabled");
		}
	}
	Connect(XRCID("lstCompilerOptions"), -1,
			wxEVT_COMMAND_CHECKLISTBOX_TOGGLED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
			&CompilerOptionsDlg::OnOptionToggled);
}

void CompilerOptionsDlg::TextToOptions()
{
    // disable all options
	for (unsigned int n = 0; n < m_Options.GetCount(); ++n)
	{
        CompOption* copt = m_Options.GetOption(n);
        if (copt)
            copt->enabled = false;
	}
	
	wxString rest;
	
    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
    Compiler* compiler = CompilerFactory::Compilers[compilerIdx];
    
	XRCCTRL(*this, "txtCompilerDefines", wxTextCtrl)->Clear();
	unsigned int i = 0;
	while (i < m_CompilerOptions.GetCount())
	{
		wxString opt = m_CompilerOptions.Item(i);
		opt.Trim(wxString::both);
		CompOption* copt = m_Options.GetOptionByOption(opt);
		if (copt)
		{
//            Manager::Get()->GetMessageManager()->DebugLog("Enabling option %s", copt->option.c_str());
			copt->enabled = true;
			m_CompilerOptions.Remove(i);
		}
		else if (opt.StartsWith(compiler->GetSwitches().defines, &rest))
		{
			// definition
			XRCCTRL(*this, "txtCompilerDefines", wxTextCtrl)->AppendText(rest);
			XRCCTRL(*this, "txtCompilerDefines", wxTextCtrl)->AppendText("\n");
			m_CompilerOptions.Remove(i);
		}
		else
			++i;
	}
	i = 0;
	while (i < m_LinkerOptions.GetCount())
	{
		wxString opt = m_LinkerOptions.Item(i);
		opt.Trim(wxString::both);
		CompOption* copt = m_Options.GetOptionByAdditionalLibs(opt);
		if (copt)
		{
//            Manager::Get()->GetMessageManager()->DebugLog("Enabling option %s", copt->option.c_str());
			copt->enabled = true;
			m_LinkerOptions.Remove(i);
		}
		else
			++i;
	}
}

void CompilerOptionsDlg::OptionsToText()
{
	wxArrayString array;
	DoGetCompileOptions(array, XRCCTRL(*this, "txtCompilerDefines", wxTextCtrl));
	
    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
    Compiler* compiler = CompilerFactory::Compilers[compilerIdx];

	for (unsigned int i = 0; i < array.GetCount(); ++i)
	{
		if (!array[i].IsEmpty())
		{
			if (array[i].StartsWith("-"))
			{
                if (m_CompilerOptions.Index(array[i]) == wxNOT_FOUND)
                    m_CompilerOptions.Add(array[i]);
            }
			else
			{
                if (m_CompilerOptions.Index(compiler->GetSwitches().defines + array[i]) == wxNOT_FOUND)
                    m_CompilerOptions.Add(compiler->GetSwitches().defines + array[i]);
            }
		}
	}
	for (unsigned int i = 0; i < m_Options.GetCount(); ++i)
	{
		CompOption* copt = m_Options.GetOption(i);
		if (copt->enabled)
		{
			m_CompilerOptions.Insert(copt->option, 0);
			if (!copt->additionalLibs.IsEmpty())
			{
                if (m_LinkerOptions.Index(copt->additionalLibs) == wxNOT_FOUND)
                    m_LinkerOptions.Insert(copt->additionalLibs, 0);
            }
        }
        else
        {
            // for disabled options, remove relative text option *and*
            // relative linker option
            int idx = m_CompilerOptions.Index(copt->option);
            if (idx != wxNOT_FOUND)
                m_CompilerOptions.Remove(idx);
            idx = m_LinkerOptions.Index(copt->additionalLibs);
            if (idx != wxNOT_FOUND)
                m_LinkerOptions.Remove(idx);
        }
	}
}

void CompilerOptionsDlg::DoFillCompileOptions(const wxArrayString& array, wxTextCtrl* control)
{
	control->Clear();
	int count = array.GetCount();
	for (int i = 0; i < count; ++i)
	{
        if (!array[i].IsEmpty())
        {
            control->AppendText(array[i]);
            control->AppendText('\n');
        }
    }
}

void CompilerOptionsDlg::DoFillCompileDirs(const wxArrayString& array, wxListBox* control)
{
	control->Clear();
	int count = array.GetCount();
	for (int i = 0; i < count; ++i)
	{
        if (!array[i].IsEmpty())
            control->Append(array[i]);
    }
}

void CompilerOptionsDlg::DoGetCompileOptions(wxArrayString& array, wxTextCtrl* control)
{
/* NOTE (mandrav#1#): Under Gnome2, wxTextCtrl::GetLineLength() returns always 0,
                      so wxTextCtrl::GetLineText() is always empty...
                      Now, we 're breaking up by newlines. */
    array.Clear();
#if 1
    wxString tmp = control->GetValue();
    int nl = tmp.Find('\n');
    wxString line;
    if (nl == -1)
    {
        line = tmp;
        tmp = "";
    }
    else
        line = tmp.Left(nl);
    while (nl != -1 || !line.IsEmpty())
    {
//        Manager::Get()->GetMessageManager()->DebugLog("%s text=%s", control->GetName().c_str(), line.c_str());
        if (!line.IsEmpty())
        {
            // just to make sure..
            line.Replace("\r", " ", true); // remove CRs
            line.Replace("\n", " ", true); // remove LFs
            array.Add(line.Strip(wxString::both));
        }
        tmp.Remove(0, nl + 1);
        nl = tmp.Find('\n');
        if (nl == -1)
        {
            line = tmp;
            tmp = "";
        }
        else
            line = tmp.Left(nl);
    }
#else
	int count = control->GetNumberOfLines();
	for (int i = 0; i < count; ++i)
	{
        wxString tmp = control->GetLineText(i);
        if (!tmp.IsEmpty())
        {
            tmp.Replace("\r", " ", true); // remove CRs
            tmp.Replace("\n", " ", true); // remove LFs
            array.Add(tmp.Strip(wxString::both));
        }
    }
#endif
}

void CompilerOptionsDlg::DoGetCompileDirs(wxArrayString& array, wxListBox* control)
{
    array.Clear();
	int count = control->GetCount();
	for (int i = 0; i < count; ++i)
	{
        wxString tmp = control->GetString(i);
        if (!tmp.IsEmpty())
            array.Add(tmp);
    }
}

void CompilerOptionsDlg::DoLoadOptions(int compilerIdx, ScopeTreeData* data)
{
	if (!data)
	{
		// global options
        Compiler* compiler = CompilerFactory::Compilers[compilerIdx];

		m_IncludeDirs = compiler->GetIncludeDirs();
		m_LibDirs = compiler->GetLibDirs();
		m_CompilerOptions = compiler->GetCompilerOptions();
		m_LinkerOptions = compiler->GetLinkerOptions();
		m_CommandsAfterBuild = compiler->GetCommandsAfterBuild();
		m_CommandsBeforeBuild = compiler->GetCommandsBeforeBuild();
	}
	else
	{
		if (!data->GetTarget())
		{
			// project options
			cbProject* project = data->GetProject();
			m_IncludeDirs = project->GetIncludeDirs();
			m_LibDirs = project->GetLibDirs();
			m_CompilerOptions = project->GetCompilerOptions();
			m_LinkerOptions = project->GetLinkerOptions();
			m_CommandsAfterBuild = project->GetCommandsAfterBuild();
			m_CommandsBeforeBuild = project->GetCommandsBeforeBuild();
		}
		else
		{
			// target options
			ProjectBuildTarget* target = data->GetTarget();
			m_IncludeDirs = target->GetIncludeDirs();
			m_LibDirs = target->GetLibDirs();
			m_CompilerOptions = target->GetCompilerOptions();
			m_LinkerOptions = target->GetLinkerOptions();
			m_CommandsAfterBuild = target->GetCommandsAfterBuild();
			m_CommandsBeforeBuild = target->GetCommandsBeforeBuild();
			XRCCTRL(*this, "cmbCompilerPolicy", wxComboBox)->SetSelection(target->GetOptionRelation(ortCompilerOptions));
			XRCCTRL(*this, "cmbLinkerPolicy", wxComboBox)->SetSelection(target->GetOptionRelation(ortLinkerOptions));
			XRCCTRL(*this, "cmbIncludesPolicy", wxComboBox)->SetSelection(target->GetOptionRelation(ortIncludeDirs));
			XRCCTRL(*this, "cmbLibsPolicy", wxComboBox)->SetSelection(target->GetOptionRelation(ortLibDirs));
		}
	}
	TextToOptions();

	DoFillOptions();
	DoFillCompileDirs(m_IncludeDirs, XRCCTRL(*this, "lstIncludeDirs", wxListBox));
	DoFillCompileDirs(m_LibDirs, XRCCTRL(*this, "lstLibDirs", wxListBox));
	DoFillCompileOptions(m_CompilerOptions, XRCCTRL(*this, "txtCompilerOptions", wxTextCtrl));
	DoFillCompileOptions(m_LinkerOptions, XRCCTRL(*this, "txtLinkerOptions", wxTextCtrl));
	DoFillCompileOptions(m_CommandsBeforeBuild, XRCCTRL(*this, "txtCmdBefore", wxTextCtrl));
	DoFillCompileOptions(m_CommandsAfterBuild, XRCCTRL(*this, "txtCmdAfter", wxTextCtrl));
}

void CompilerOptionsDlg::DoSaveOptions(int compilerIdx, ScopeTreeData* data)
{
	DoGetCompileDirs(m_IncludeDirs, XRCCTRL(*this, "lstIncludeDirs", wxListBox));
	DoGetCompileDirs(m_LibDirs, XRCCTRL(*this, "lstLibDirs", wxListBox));
	DoGetCompileOptions(m_CompilerOptions, XRCCTRL(*this, "txtCompilerOptions", wxTextCtrl));
	DoGetCompileOptions(m_LinkerOptions, XRCCTRL(*this, "txtLinkerOptions", wxTextCtrl));
	DoGetCompileOptions(m_CommandsBeforeBuild, XRCCTRL(*this, "txtCmdBefore", wxTextCtrl));
	DoGetCompileOptions(m_CommandsAfterBuild, XRCCTRL(*this, "txtCmdAfter", wxTextCtrl));
    OptionsToText();
    
	if (!data)
	{
		// global options
        Compiler* compiler = CompilerFactory::Compilers[compilerIdx];

		compiler->SetIncludeDirs(m_IncludeDirs);
		compiler->SetLibDirs(m_LibDirs);
		compiler->SetCompilerOptions(m_CompilerOptions);
		compiler->SetLinkerOptions(m_LinkerOptions);
		compiler->SetCommandsBeforeBuild(m_CommandsBeforeBuild);
		compiler->SetCommandsAfterBuild(m_CommandsAfterBuild);
	}
	else
	{
		if (!data->GetTarget())
		{
			// project options
			cbProject* project = data->GetProject();
			project->SetIncludeDirs(m_IncludeDirs);
			project->SetLibDirs(m_LibDirs);
			project->SetCompilerOptions(m_CompilerOptions);
			project->SetLinkerOptions(m_LinkerOptions);
			project->SetCommandsBeforeBuild(m_CommandsBeforeBuild);
			project->SetCommandsAfterBuild(m_CommandsAfterBuild);
		}
		else
		{
			// target options
			ProjectBuildTarget* target = data->GetTarget();
			target->SetIncludeDirs(m_IncludeDirs);
			target->SetLibDirs(m_LibDirs);
			target->SetCompilerOptions(m_CompilerOptions);
			target->SetLinkerOptions(m_LinkerOptions);
            target->SetOptionRelation(ortCompilerOptions, OptionsRelation(XRCCTRL(*this, "cmbCompilerPolicy", wxComboBox)->GetSelection()));
            target->SetOptionRelation(ortLinkerOptions, OptionsRelation(XRCCTRL(*this, "cmbLinkerPolicy", wxComboBox)->GetSelection()));
            target->SetOptionRelation(ortIncludeDirs, OptionsRelation(XRCCTRL(*this, "cmbIncludesPolicy", wxComboBox)->GetSelection()));
            target->SetOptionRelation(ortLibDirs, OptionsRelation(XRCCTRL(*this, "cmbLibsPolicy", wxComboBox)->GetSelection()));
			target->SetCommandsBeforeBuild(m_CommandsBeforeBuild);
			target->SetCommandsAfterBuild(m_CommandsAfterBuild);
		}
	}
}

void CompilerOptionsDlg::DoMakeRelative(wxFileName& path)
{
	wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
	ScopeTreeData* data = (ScopeTreeData*)tc->GetItemData(tc->GetSelection());
	if (data)
		path.MakeRelativeTo(data->GetProject()->GetBasePath());
}

void CompilerOptionsDlg::DoUpdateCompiler()
{
    if (!CompilerFactory::CompilerIndexOK(m_LastCompilerIdx))
        return;
    CompilerPrograms progs;
    wxString masterPath = XRCCTRL(*this, "txtMasterPath", wxTextCtrl)->GetValue();
    progs.C = XRCCTRL(*this, "txtCcompiler", wxTextCtrl)->GetValue();
    progs.CPP = XRCCTRL(*this, "txtCPPcompiler", wxTextCtrl)->GetValue();
    progs.LD = XRCCTRL(*this, "txtLinker", wxTextCtrl)->GetValue();
    progs.WINDRES = XRCCTRL(*this, "txtResComp", wxTextCtrl)->GetValue();
    progs.MAKE = XRCCTRL(*this, "txtMake", wxTextCtrl)->GetValue();
    CompilerFactory::Compilers[m_LastCompilerIdx]->SetPrograms(progs);
    CompilerFactory::Compilers[m_LastCompilerIdx]->SetMasterPath(masterPath);
    CompilerFactory::Compilers[m_LastCompilerIdx]->SetOptions(m_Options);
}

// events

void CompilerOptionsDlg::OnTreeSelectionChange(wxTreeEvent& event)
{
	if (m_BuildingTree)
		return;
	wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
	ScopeTreeData* data = (ScopeTreeData*)tc->GetItemData(event.GetItem());
    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
	DoLoadOptions(compilerIdx, data);
}

void CompilerOptionsDlg::OnTreeSelectionChanging(wxTreeEvent& event)
{
	if (m_BuildingTree)
		return;
	wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
	ScopeTreeData* data = (ScopeTreeData*)tc->GetItemData(event.GetOldItem());
	DoSaveOptions(m_LastCompilerIdx, data);
}

void CompilerOptionsDlg::OnCompilerChanged(wxCommandEvent& event)
{
	wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
	ScopeTreeData* data = (ScopeTreeData*)tc->GetItemData(tc->GetSelection());
	DoSaveOptions(m_LastCompilerIdx, data);

    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
    m_Options = CompilerFactory::Compilers[compilerIdx]->GetOptions();
    DoFillCompilerPrograms();
    DoFillCategories();
    DoFillOptions();

	if (m_BuildingTree)
		return;
	DoLoadOptions(compilerIdx, data);
}

void CompilerOptionsDlg::OnCategoryChanged(wxCommandEvent& event)
{
	DoFillOptions();
}

void CompilerOptionsDlg::OnOptionToggled(wxCommandEvent& event)
{
	wxCheckListBox* list = XRCCTRL(*this, "lstCompilerOptions", wxCheckListBox);
	int sel = event.GetInt();
	CompOption* copt = m_Options.GetOptionByName(list->GetString(sel));
	if (copt)
	{
		copt->enabled = list->IsChecked(sel);
//        Manager::Get()->GetMessageManager()->DebugLog("option %s (0x%8.8x) %s", copt->option.c_str(), copt, copt->enabled ? "enabled" : "disabled");
    }
}

void CompilerOptionsDlg::OnAddDirClick(wxCommandEvent& event)
{
    wxDirDialog dlg(this);
    dlg.SetPath(m_pProject ? m_pProject->GetBasePath() : "");
    if (dlg.ShowModal() != wxID_OK)
        return;
        
    wxFileName path(dlg.GetPath());
	DoMakeRelative(path);
   
    wxListBox* control = 0L;
    if (event.GetId() == XRCID("btnAddIncludes"))
        control = XRCCTRL(*this, "lstIncludeDirs", wxListBox);
    else
        control = XRCCTRL(*this, "lstLibDirs", wxListBox);

    control->Append(path.GetFullPath());
}

void CompilerOptionsDlg::OnEditDirClick(wxCommandEvent& event)
{
    wxListBox* control = 0L;
    if (event.GetId() == XRCID("btnEditIncludes"))
        control = XRCCTRL(*this, "lstIncludeDirs", wxListBox);
    else
        control = XRCCTRL(*this, "lstLibDirs", wxListBox);
        
    if (control->GetSelection() < 0)
        return;
        
    wxDirDialog dlg(this);
    wxFileName dir(control->GetString(control->GetSelection()) + wxFileName::GetPathSeparator());
    if (m_pProject)
        dir.Normalize(wxPATH_NORM_ALL, m_pProject->GetBasePath());
    Manager::Get()->GetMessageManager()->DebugLog(dir.GetFullPath());
    if (dir.DirExists())
        dlg.SetPath(dir.GetPath(wxPATH_GET_VOLUME));
    else if (m_pProject)
        dlg.SetPath(m_pProject->GetBasePath());
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxFileName path(dlg.GetPath());
	DoMakeRelative(path);

    control->SetString(control->GetSelection(), path.GetFullPath());
}

void CompilerOptionsDlg::OnRemoveDirClick(wxCommandEvent& event)
{
    wxListBox* control = 0L;
    if (event.GetId() == XRCID("btnDelIncludes"))
        control = XRCCTRL(*this, "lstIncludeDirs", wxListBox);
    else
        control = XRCCTRL(*this, "lstLibDirs", wxListBox);

    if (control->GetSelection() < 0)
        return;

    control->Delete(control->GetSelection());
}

void CompilerOptionsDlg::OnAddVarClick(wxCommandEvent& event)
{
	const wxString title = _("Add variable");
	wxString name = wxGetTextFromUser(_("Please enter the name for the new variable:"), title);
	if (name.IsEmpty())
		return;
	wxString value = wxGetTextFromUser(_("Please enter value for the new variable:"), title);
	if (!value.IsEmpty())
	{
		CustomVars& vars = m_Compiler->GetCustomVars();
		vars.Add(name, value);
		DoFillPrograms();
	}
}

void CompilerOptionsDlg::OnEditVarClick(wxCommandEvent& event)
{
	const wxString title = _("Edit variable");
	int sel = XRCCTRL(*this, "lstVars", wxListBox)->GetSelection();
	if (sel == -1)
		return;

	Var* var = static_cast<Var*>(XRCCTRL(*this, "lstVars", wxListBox)->GetClientData(sel));
	if (!var)
		return;
		
	wxString value = wxGetTextFromUser(_("Please edit the variable value:"), title, var->value);
	if (!value.IsEmpty())
	{
		var->value = value;
		XRCCTRL(*this, "lstVars", wxListBox)->SetString(sel, var->name + " = " + var->value);
	}
}

void CompilerOptionsDlg::OnRemoveVarClick(wxCommandEvent& event)
{
	int sel = XRCCTRL(*this, "lstVars", wxListBox)->GetSelection();
	if (sel == -1)
		return;
	if (wxMessageBox(_("Are you sure you want to delete this variable?"),
					_("Confirmation"),
					wxYES_NO | wxICON_QUESTION) == wxYES)
	{
		Var* var = static_cast<Var*>(XRCCTRL(*this, "lstVars", wxListBox)->GetClientData(sel));
		if (var)
		{
			m_Compiler->GetCustomVars().DeleteVar(var);
			DoFillPrograms();
		}
	}
}

void CompilerOptionsDlg::OnAddCompilerClick(wxCommandEvent& event)
{
    wxComboBox* cmb = XRCCTRL(*this, "cmbCompiler", wxComboBox);
    int idx = cmb->GetSelection();
	wxString value = wxGetTextFromUser(_("Please enter the new compiler's name:"), _("Add new compiler"), _("Copy of ") + CompilerFactory::Compilers[idx]->GetName());
	if (!value.IsEmpty())
	{
        // make a copy of current compiler
        int newIdx = CompilerFactory::CreateCompilerCopy(CompilerFactory::Compilers[idx]);
        Compiler* newC = CompilerFactory::Compilers[newIdx];
        // and change its name
        newC->SetName(value);

        cmb->Append(value);
        cmb->SetSelection(cmb->GetCount() - 1);
        DoFillCompilerPrograms();
        wxMessageBox(_("The new compiler has been added! Don't forget to update the \"Programs\" page..."));
    }
}

void CompilerOptionsDlg::OnEditCompilerClick(wxCommandEvent& event)
{
    wxComboBox* cmb = XRCCTRL(*this, "cmbCompiler", wxComboBox);
	wxString value = wxGetTextFromUser(_("Please edit the compiler's name:"), _("Rename compiler"), cmb->GetValue());
	if (!value.IsEmpty())
	{
        int idx = cmb->GetSelection();
        CompilerFactory::Compilers[idx]->SetName(value);
#ifndef __WXGTK__
        cmb->SetString(idx, value);
#else
	#warning wxComboBox::SetString() not implemented: CompilerOptionsDlg::OnEditCompilerClick() is not updating correctly...
#endif
        cmb->SetSelection(idx);
    }
}

void CompilerOptionsDlg::OnRemoveCompilerClick(wxCommandEvent& event)
{
	if (wxMessageBox(_("Are you sure you want to remove this compiler?"),
					_("Confirmation"),
					wxYES_NO | wxICON_QUESTION) == wxYES)
    {
        wxComboBox* cmb = XRCCTRL(*this, "cmbCompiler", wxComboBox);
        int compilerIdx = cmb->GetSelection();
        CompilerFactory::RemoveCompiler(CompilerFactory::Compilers[compilerIdx]);
        cmb->Delete(compilerIdx);
        while (compilerIdx >= cmb->GetCount())
            --compilerIdx;
        cmb->SetSelection(compilerIdx);

        DoFillCompilerPrograms();
        DoFillCategories();
        DoFillOptions();
    
        if (m_BuildingTree)
            return;
        wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
        ScopeTreeData* data = (ScopeTreeData*)tc->GetItemData(tc->GetSelection());
        DoLoadOptions(compilerIdx, data);
    }
}

void CompilerOptionsDlg::OnMasterPathClick(wxCommandEvent& event)
{
    wxDirDialog dlg(this);
    dlg.SetPath(XRCCTRL(*this, "txtMasterPath", wxTextCtrl)->GetValue());
    if (dlg.ShowModal() == wxID_OK)
    {
        XRCCTRL(*this, "txtMasterPath", wxTextCtrl)->SetValue(dlg.GetPath());
        DoUpdateCompiler();
    }
}

void CompilerOptionsDlg::OnSelectProgramClick(wxCommandEvent& event)
{
    // see who called us
    wxTextCtrl* obj = 0L;
    if (event.GetId() == XRCID("btnCcompiler"))
        obj = XRCCTRL(*this, "txtCcompiler", wxTextCtrl);
    else if (event.GetId() == XRCID("btnCPPcompiler"))
        obj = XRCCTRL(*this, "txtCPPcompiler", wxTextCtrl);
    else if (event.GetId() == XRCID("btnLinker"))
        obj = XRCCTRL(*this, "txtLinker", wxTextCtrl);
    else if (event.GetId() == XRCID("btnDebugger"))
        obj = XRCCTRL(*this, "txtDebugger", wxTextCtrl);
    else if (event.GetId() == XRCID("btnResComp"))
        obj = XRCCTRL(*this, "txtResComp", wxTextCtrl);
    else if (event.GetId() == XRCID("btnMake"))
        obj = XRCCTRL(*this, "txtMake", wxTextCtrl);

    if (!obj)
        return; // called from invalid caller

    // common part follows
    wxFileDialog* dlg = new wxFileDialog(this,
                            _("Select file"),
                            XRCCTRL(*this, "txtMasterPath", wxTextCtrl)->GetValue() + "/bin",
                            obj->GetValue(),
                            "Executable files (*.exe)|*.exe",
                            wxOPEN | wxFILE_MUST_EXIST);
    dlg->SetFilterIndex(0);

    if (dlg->ShowModal() != wxID_OK)
        return;
    wxFileName fname(dlg->GetPath());
    obj->SetValue(fname.GetFullName());
    DoUpdateCompiler();
}

void CompilerOptionsDlg::OnAdvancedClick(wxCommandEvent& event)
{
	if (wxMessageBox(_("The compiler's advanced settings, need command-line "
                        "compiler knowledge to be tweaked.\nIf you don't know "
                        "*exactly* what you 're doing, it is suggested to "
                        "NOT tamper with the advanced settings...\n\n"
                        "Are you sure you want to edit the advanced settings?"),
					_("Warning"),
					wxYES_NO | wxICON_WARNING) == wxYES)
    {
        wxComboBox* cmb = XRCCTRL(*this, "cmbCompiler", wxComboBox);
        int compilerIdx = cmb->GetSelection();
        AdvancedCompilerOptionsDlg dlg(this, compilerIdx);
        dlg.ShowModal();
    }
}

void CompilerOptionsDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    // add/edit/delete includes
    bool en = XRCCTRL(*this, "lstIncludeDirs", wxListBox)->GetSelection() >= 0;
    XRCCTRL(*this, "btnEditIncludes", wxButton)->Enable(en);
    XRCCTRL(*this, "btnDelIncludes", wxButton)->Enable(en);

    // add/edit/delete libs
    en = XRCCTRL(*this, "lstLibDirs", wxListBox)->GetSelection() >= 0;
    XRCCTRL(*this, "btnEditLibs", wxButton)->Enable(en);
    XRCCTRL(*this, "btnDelLibs", wxButton)->Enable(en);

    // add/edit/delete vars
    en = XRCCTRL(*this, "lstVars", wxListBox)->GetSelection() >= 0;
    XRCCTRL(*this, "btnEditVar", wxButton)->Enable(en);
    XRCCTRL(*this, "btnDeleteVar", wxButton)->Enable(en);

    // policies
	wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
	ScopeTreeData* data = (ScopeTreeData*)tc->GetItemData(tc->GetSelection());
	en = (data && data->GetTarget());
    XRCCTRL(*this, "cmbCompilerPolicy", wxComboBox)->Enable(en);
    XRCCTRL(*this, "cmbLinkerPolicy", wxComboBox)->Enable(en);
    XRCCTRL(*this, "cmbIncludesPolicy", wxComboBox)->Enable(en);
    XRCCTRL(*this, "cmbLibsPolicy", wxComboBox)->Enable(en);

    // compiler set buttons
    if (XRCCTRL(*this, "btnAddCompiler", wxButton)) // only if exist
    {
        en = !data; // global options selected
        int idx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
        int count = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetCount(); // compilers count
        XRCCTRL(*this, "btnAddCompiler", wxButton)->Enable(en);
        XRCCTRL(*this, "btnRenameCompiler", wxButton)->Enable(en && count);
        XRCCTRL(*this, "btnDelCompiler", wxButton)->Enable(en &&
                                                        CompilerFactory::CompilerIndexOK(idx) &&
                                                        CompilerFactory::Compilers[idx]->GetParentID() != -1);
    }
    
    // compiler programs
    if (XRCCTRL(*this, "txtMasterPath", wxTextCtrl)) // "Programs" page exists?
    {
        en = !data; // global options selected
        XRCCTRL(*this, "txtMasterPath", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnMasterPath", wxButton)->Enable(en);
        XRCCTRL(*this, "txtCcompiler", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnCcompiler", wxButton)->Enable(en);
        XRCCTRL(*this, "txtCPPcompiler", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnCPPcompiler", wxButton)->Enable(en);
        XRCCTRL(*this, "txtLinker", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnLinker", wxButton)->Enable(en);
        XRCCTRL(*this, "txtDebugger", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnDebugger", wxButton)->Enable(en);
        XRCCTRL(*this, "txtResComp", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnResComp", wxButton)->Enable(en);
        XRCCTRL(*this, "txtMake", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnMake", wxButton)->Enable(en);
        XRCCTRL(*this, "cmbCompiler", wxComboBox)->Enable(en);
    }
}

void CompilerOptionsDlg::EndModal(int retCode)
{
	wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
	ScopeTreeData* data = (ScopeTreeData*)tc->GetItemData(tc->GetSelection());
    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
	DoSaveOptions(compilerIdx, data);
	m_Compiler->GetCustomVars().Save();

    // compiler set
    int idx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
    if (m_pProject && idx != m_InitialCompilerIdx)
    {
        m_pProject->SetCompilerIndex(idx);
        wxMessageBox(_("You changed the compiler used for this project.\n"
                        "It is recommended that you fully rebuild your project, "
                        "otherwise linking errors might occur..."));
    }

    if (!m_pProject)
    {
        // only do it for global compiler options
        // why does it crash for project compiler options???
        m_LastCompilerIdx = idx;
        DoUpdateCompiler();
    }
    
	//others
    wxCheckBox* chk = XRCCTRL(*this, "chkSimpleBuild", wxCheckBox);
    if (chk)
        ConfigManager::Get()->Write("/compiler_gcc/simple_build", chk->GetValue());	
    wxTextCtrl* txt = XRCCTRL(*this, "txtConsoleShell", wxTextCtrl);
    if (txt)
        ConfigManager::Get()->Write("/compiler_gcc/console_shell", txt->GetValue());
	wxDialog::EndModal(retCode);
}
