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
#include "cbexception.h"
#include "annoyingdialog.h"
#include "compileroptionsdlg.h"
#include "compilergcc.h"
#include "advancedcompileroptionsdlg.h"
#include <wx/xrc/xmlres.h>
#include <manager.h>
#include <configmanager.h>
#include <messagemanager.h>
#include <projectmanager.h>
#include <macrosmanager.h>
#include "editpathdlg.h"
#include "editpairdlg.h"
#include <globals.h>
#include <wx/choice.h>

BEGIN_EVENT_TABLE(CompilerOptionsDlg, wxPanel)
    EVT_UPDATE_UI(			XRCID("btnEditDir"),	    CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnDelDir"),	        CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnClearDir"),	        CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("spnDirs"),	        CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnEditVar"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnDeleteVar"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnClearVar"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("cmbCompilerPolicy"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("cmbLinkerPolicy"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("cmbIncludesPolicy"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("cmbLibDirsPolicy"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnSetDefaultCompiler"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnAddCompiler"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnRenameCompiler"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnDelCompiler"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnResetCompiler"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnAddLib"),	        CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnEditLib"),	    CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnDelLib"),	        CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnClearLib"),	        CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("spnLibs"),	        CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("txtMasterPath"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnMasterPath"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnExtraAdd"),	    CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnExtraEdit"),	    CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnExtraDelete"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnExtraClear"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("txtCcompiler"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnCcompiler"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("txtCPPcompiler"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnCPPcompiler"),	CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("txtLinker"),			CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnLinker"),			CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("txtLibLinker"),		CompilerOptionsDlg::OnUpdateUI)
    EVT_UPDATE_UI(			XRCID("btnLibLinker"),		CompilerOptionsDlg::OnUpdateUI)
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
//	EVT_COMBOBOX(			XRCID("cmbCategory"), 		CompilerOptionsDlg::OnCategoryChanged)
	EVT_CHOICE(			XRCID("cmbCategory"), 		CompilerOptionsDlg::OnCategoryChanged)
	EVT_COMBOBOX(			XRCID("cmbCompiler"), 		CompilerOptionsDlg::OnCompilerChanged)
	EVT_LISTBOX_DCLICK(		XRCID("lstVars"),			CompilerOptionsDlg::OnEditVarClick)
	EVT_BUTTON(				XRCID("btnSetDefaultCompiler"),	CompilerOptionsDlg::OnSetDefaultCompilerClick)
	EVT_BUTTON(				XRCID("btnAddCompiler"),	CompilerOptionsDlg::OnAddCompilerClick)
	EVT_BUTTON(				XRCID("btnRenameCompiler"),	CompilerOptionsDlg::OnEditCompilerClick)
	EVT_BUTTON(				XRCID("btnDelCompiler"),	CompilerOptionsDlg::OnRemoveCompilerClick)
	EVT_BUTTON(				XRCID("btnResetCompiler"),	CompilerOptionsDlg::OnResetCompilerClick)
	EVT_BUTTON(				XRCID("btnAddDir"),	        CompilerOptionsDlg::OnAddDirClick)
	EVT_BUTTON(				XRCID("btnEditDir"),	    CompilerOptionsDlg::OnEditDirClick)
	EVT_LISTBOX_DCLICK(		XRCID("lstIncludeDirs"),    CompilerOptionsDlg::OnEditDirClick)
	EVT_LISTBOX_DCLICK(		XRCID("lstLibDirs"),        CompilerOptionsDlg::OnEditDirClick)
	EVT_LISTBOX_DCLICK(		XRCID("lstResDirs"),        CompilerOptionsDlg::OnEditDirClick)
	EVT_BUTTON(				XRCID("btnDelDir"),	        CompilerOptionsDlg::OnRemoveDirClick)
	EVT_BUTTON(				XRCID("btnClearDir"),	        CompilerOptionsDlg::OnClearDirClick)
    EVT_BUTTON(			    XRCID("btnAddLib"),	        CompilerOptionsDlg::OnAddLibClick)
    EVT_BUTTON(			    XRCID("btnEditLib"),	    CompilerOptionsDlg::OnEditLibClick)
	EVT_LISTBOX_DCLICK(		XRCID("lstLibs"),   	    CompilerOptionsDlg::OnEditLibClick)
    EVT_BUTTON(			    XRCID("btnDelLib"),	        CompilerOptionsDlg::OnRemoveLibClick)
    EVT_BUTTON(			    XRCID("btnClearLib"),	        CompilerOptionsDlg::OnClearLibClick)
    EVT_LISTBOX_DCLICK(XRCID("lstExtraPaths"), CompilerOptionsDlg::OnEditExtraPathClick)
    EVT_BUTTON(			    XRCID("btnExtraAdd"),	    CompilerOptionsDlg::OnAddExtraPathClick)
    EVT_BUTTON(			    XRCID("btnExtraEdit"),	    CompilerOptionsDlg::OnEditExtraPathClick)
    EVT_BUTTON(			    XRCID("btnExtraDelete"),	CompilerOptionsDlg::OnRemoveExtraPathClick)
    EVT_BUTTON(			    XRCID("btnExtraClear"),	CompilerOptionsDlg::OnClearExtraPathClick)
    EVT_SPIN_UP(			XRCID("spnLibs"),	        CompilerOptionsDlg::OnMoveLibUpClick)
    EVT_SPIN_DOWN(			XRCID("spnLibs"),	        CompilerOptionsDlg::OnMoveLibDownClick)
    EVT_SPIN_UP(			XRCID("spnDirs"),	        CompilerOptionsDlg::OnMoveDirUpClick)
    EVT_SPIN_DOWN(			XRCID("spnDirs"),	        CompilerOptionsDlg::OnMoveDirDownClick)
	EVT_BUTTON(				XRCID("btnAddVar"),			CompilerOptionsDlg::OnAddVarClick)
	EVT_BUTTON(				XRCID("btnEditVar"),		CompilerOptionsDlg::OnEditVarClick)
	EVT_BUTTON(				XRCID("btnDeleteVar"),		CompilerOptionsDlg::OnRemoveVarClick)
	EVT_BUTTON(				XRCID("btnClearVar"),		CompilerOptionsDlg::OnClearVarClick)
	EVT_BUTTON(				XRCID("btnMasterPath"),		CompilerOptionsDlg::OnMasterPathClick)
	EVT_BUTTON(				XRCID("btnAutoDetect"),		CompilerOptionsDlg::OnAutoDetectClick)
	EVT_BUTTON(				XRCID("btnCcompiler"),		CompilerOptionsDlg::OnSelectProgramClick)
	EVT_BUTTON(				XRCID("btnCPPcompiler"),	CompilerOptionsDlg::OnSelectProgramClick)
	EVT_BUTTON(				XRCID("btnLinker"),		    CompilerOptionsDlg::OnSelectProgramClick)
	EVT_BUTTON(				XRCID("btnLibLinker"),		CompilerOptionsDlg::OnSelectProgramClick)
	EVT_BUTTON(				XRCID("btnDebugger"),		CompilerOptionsDlg::OnSelectProgramClick)
	EVT_BUTTON(				XRCID("btnResComp"),		CompilerOptionsDlg::OnSelectProgramClick)
	EVT_BUTTON(				XRCID("btnMake"),		    CompilerOptionsDlg::OnSelectProgramClick)
	EVT_BUTTON(				XRCID("btnAdvanced"),		CompilerOptionsDlg::OnAdvancedClick)
	EVT_CHAR_HOOK(CompilerOptionsDlg::OnMyCharHook)
END_EVENT_TABLE()

CompilerOptionsDlg::CompilerOptionsDlg(wxWindow* parent, CompilerGCC* compiler, cbProject* project, ProjectBuildTarget* target)
	: m_Compiler(compiler),
	m_InitialCompilerIdx(0),
	m_pProject(project),
	m_pTarget(target),
	m_BuildingTree(false)
{
	wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgCompilerOptions"));

	if (project)
	{
        bool hasBuildScripts = project->GetBuildScripts().GetCount() != 0;
        if (!hasBuildScripts)
        {
            // look in targets
            for (int x = 0; x < project->GetBuildTargetsCount(); ++x)
            {
                ProjectBuildTarget* target = project->GetBuildTarget(x);
                hasBuildScripts = target->GetBuildScripts().GetCount() != 0;
                if (hasBuildScripts)
                    break;
            }
        }

        XRCCTRL(*this, "lblBuildScriptsNote", wxStaticText)->Show(hasBuildScripts);
    }

	DoFillCompilerSets();
	DoFillCompilerPrograms();
	DoFillOthers();
	DoFillCategories();
	DoFillTree(project, target);
	DoFillVars();

    wxComboBox* cmb = XRCCTRL(*this, "cmbBuildMethod", wxComboBox);
    if (cmb)
    {
        // build method is always "direct" now
        cmb->SetSelection(1);
        cmb->Enable(false);
    }

    wxTreeCtrl* tree = XRCCTRL(*this, "tcScope", wxTreeCtrl);
    wxSizer* sizer = tree->GetContainingSizer();
    if (!project)
    {
        // global settings
        SetTitle(_("Compiler Settings"));
        sizer->Show(tree,false);
        sizer->Remove(tree);
        wxNotebook* nb = XRCCTRL(*this, "nbMain", wxNotebook);
        nb->DeletePage(6); // remove "Make" page
        nb->DeletePage(3); // remove "Commands" page
	}
	else
	{
        m_InitialCompilerIdx = CompilerFactory::GetCompilerIndex(project->GetCompilerID());
        // project settings

        wxNotebook* nb = XRCCTRL(*this, "nbMain", wxNotebook);
        nb->DeletePage(7); // remove "Other" page
        nb->DeletePage(4); // remove "Programs" page

        // remove "Compiler" buttons
        wxWindow* win = XRCCTRL(*this, "btnAddCompiler", wxButton);
        wxSizer* sizer2 = win->GetContainingSizer();
        sizer2->Clear(true);
        sizer2->RecalcSizes();
        sizer2->Layout();

        // disable "Make" elements, if project is not using custom makefile
        bool en = project->IsMakefileCustom();
        XRCCTRL(*this, "txtMakeCmd_Build", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "txtMakeCmd_Compile", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "txtMakeCmd_Clean", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "txtMakeCmd_DistClean", wxTextCtrl)->Enable(en);

        // disable "Commands" elements, if project is using custom makefile
        XRCCTRL(*this, "txtCmdBefore", wxTextCtrl)->Enable(!en);
        XRCCTRL(*this, "txtCmdAfter", wxTextCtrl)->Enable(!en);
        XRCCTRL(*this, "chkAlwaysRunPost", wxCheckBox)->Enable(!en);
    }
    sizer->Layout();
    Layout();
    GetSizer()->Layout();
    GetSizer()->SetSizeHints(this);
    this->SetSize(-1, -1, 0, 0);
    this->CentreOnScreen();
}

CompilerOptionsDlg::~CompilerOptionsDlg()
{
	//dtor
}

void CompilerOptionsDlg::DoFillCompilerSets()
{
    wxComboBox* cmb = XRCCTRL(*this, "cmbCompiler", wxComboBox);
    cmb->Clear();
    for (unsigned int i = 0; i < CompilerFactory::GetCompilersCount(); ++i)
    {
        cmb->Append(CompilerFactory::GetCompiler(i)->GetName());
    }

	int compilerIdx = CompilerFactory::GetCompilerIndex(CompilerFactory::GetDefaultCompilerID());
    if (m_pTarget)
        compilerIdx = CompilerFactory::GetCompilerIndex(m_pTarget->GetCompilerID());
    else if (m_pProject)
        compilerIdx = CompilerFactory::GetCompilerIndex(m_pProject->GetCompilerID());

	if (!CompilerFactory::GetCompiler(compilerIdx))
        compilerIdx = 0;
    m_Options = CompilerFactory::GetCompiler(compilerIdx)->GetOptions();
    cmb->SetSelection(compilerIdx);
    m_LastCompilerIdx = compilerIdx;
}

void CompilerOptionsDlg::DoFillCompilerPrograms()
{
    if (m_pProject)
        return; // no "Programs" page

    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
/*
//    Manager::Get()->GetMessageManager()->DebugLog("compilerIdx=%d, m_LastCompilerIdx=%d", compilerIdx, m_LastCompilerIdx);
    if (compilerIdx != m_LastCompilerIdx)
    {
        // compiler changed; check for changes and update as needed
        DoSaveCompilerPrograms();
    }
    m_LastCompilerIdx = compilerIdx;
*/
    Compiler* compiler = CompilerFactory::GetCompiler(compilerIdx);

    const CompilerPrograms& progs = compiler->GetPrograms();

    XRCCTRL(*this, "txtMasterPath", wxTextCtrl)->SetValue(compiler->GetMasterPath());
    XRCCTRL(*this, "txtCcompiler", wxTextCtrl)->SetValue(progs.C);
    XRCCTRL(*this, "txtCPPcompiler", wxTextCtrl)->SetValue(progs.CPP);
    XRCCTRL(*this, "txtLinker", wxTextCtrl)->SetValue(progs.LD);
    XRCCTRL(*this, "txtLibLinker", wxTextCtrl)->SetValue(progs.LIB);
    XRCCTRL(*this, "txtDebugger", wxTextCtrl)->SetValue(progs.DBG);
    XRCCTRL(*this, "txtResComp", wxTextCtrl)->SetValue(progs.WINDRES);
    XRCCTRL(*this, "txtMake", wxTextCtrl)->SetValue(progs.MAKE);

    XRCCTRL(*this, "lstExtraPaths", wxListBox)->Clear();
    const wxArrayString& extraPaths = compiler->GetExtraPaths();
    for (unsigned int i = 0; i < extraPaths.GetCount(); ++i)
    {
        XRCCTRL(*this, "lstExtraPaths", wxListBox)->Append(extraPaths[i]);
    }
}

void CompilerOptionsDlg::DoFillVars(const StringHash* vars)
{
	wxListBox* lst = XRCCTRL(*this, "lstVars", wxListBox);
	if (!lst)
        return;
	lst->Clear();
	if (!vars)
	{
	    CompileOptionsBase* base = GetVarsOwner();
	    if (base)
            vars = &base->GetAllVars();
	}
    if (!vars)
        return;
    for (StringHash::const_iterator it = vars->begin(); it != vars->end(); ++it)
    {
        wxString text = it->first + _T(" = ") + it->second;
        lst->Append(text);
	}
}

void CompilerOptionsDlg::DoFillOthers()
{
    wxCheckBox* chk = XRCCTRL(*this, "chkIncludeFileCwd", wxCheckBox);
    if (chk)
        chk->SetValue(Manager::Get()->GetConfigManager(_T("compiler"))->ReadBool(_T("/include_file_cwd"), false));

    chk = XRCCTRL(*this, "chkIncludePrjCwd", wxCheckBox);
    if (chk)
        chk->SetValue(Manager::Get()->GetConfigManager(_T("compiler"))->ReadBool(_T("/include_prj_cwd"), false));

    wxSpinCtrl* spn = XRCCTRL(*this, "spnParallelProcesses", wxSpinCtrl);
    if (spn)
    {
        spn->SetRange(1, 16);
        spn->SetValue(Manager::Get()->GetConfigManager(_T("compiler"))->ReadInt(_T("/parallel_processes"), 1));
    }

    spn = XRCCTRL(*this, "spnMaxErrors", wxSpinCtrl);
    if (spn)
    {
        spn->SetRange(0, 1000);
        spn->SetValue(Manager::Get()->GetConfigManager(_T("compiler"))->ReadInt(_T("/max_reported_errors"), 50));
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
//	wxComboBox* cmb = XRCCTRL(*this, "cmbCategory", wxComboBox);
	wxChoice* cmb = XRCCTRL(*this, "cmbCategory", wxChoice);
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

//	wxComboBox* cmb = XRCCTRL(*this, "cmbCategory", wxComboBox);
	wxChoice* cmb = XRCCTRL(*this, "cmbCategory", wxChoice);
//	wxString cat = cmb->GetValue();
    wxString cat = cmb->GetStringSelection();
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
    Compiler* compiler = CompilerFactory::GetCompiler(compilerIdx);

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
			m_CompilerOptions.RemoveAt(i, 1);
		}
		else if (opt.StartsWith(compiler->GetSwitches().defines, &rest))
		{
			// definition
			XRCCTRL(*this, "txtCompilerDefines", wxTextCtrl)->AppendText(rest);
			XRCCTRL(*this, "txtCompilerDefines", wxTextCtrl)->AppendText(_T("\n"));
			m_CompilerOptions.RemoveAt(i, 1);
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
			m_LinkerOptions.RemoveAt(i, 1);
		}
		else
            ++i;
	}

	XRCCTRL(*this, "lstLibs", wxListBox)->Clear();
	for (i = 0; i < m_LinkLibs.GetCount(); ++i)
	{
        XRCCTRL(*this, "lstLibs", wxListBox)->Append(m_LinkLibs[i]);
	}
	m_LinkLibs.Clear();
}

void CompilerOptionsDlg::OptionsToText()
{
	wxArrayString array;
	DoGetCompileOptions(array, XRCCTRL(*this, "txtCompilerDefines", wxTextCtrl));

    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
    Compiler* compiler = CompilerFactory::GetCompiler(compilerIdx);

	for (unsigned int i = 0; i < array.GetCount(); ++i)
	{
		if (!array[i].IsEmpty())
		{
			if (array[i].StartsWith(_T("-")))
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
                m_CompilerOptions.RemoveAt(idx, 1);
            idx = m_LinkerOptions.Index(copt->additionalLibs);
            if (idx != wxNOT_FOUND)
                m_LinkerOptions.RemoveAt(idx, 1);
        }
	}

	// linker options and libs
	wxListBox* lstLibs = XRCCTRL(*this, "lstLibs", wxListBox);
	for (int i = 0; i < lstLibs->GetCount(); ++i)
        m_LinkLibs.Add(lstLibs->GetString(i));
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
            control->AppendText(_T('\n'));
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
                      Now, we 're breaking up by newlines. */    array.Clear();
#if 1
    wxString tmp = control->GetValue();
    int nl = tmp.Find(_T('\n'));
    wxString line;
    if (nl == -1)
    {
        line = tmp;
        tmp = _T("");
    }
    else
        line = tmp.Left(nl);
    while (nl != -1 || !line.IsEmpty())
    {
//        Manager::Get()->GetMessageManager()->DebugLog("%s text=%s", control->GetName().c_str(), line.c_str());
        if (!line.IsEmpty())
        {
            // just to make sure..
            line.Replace(_T("\r"), _T(" "), true); // remove CRs
            line.Replace(_T("\n"), _T(" "), true); // remove LFs
            array.Add(line.Strip(wxString::both));
        }
        tmp.Remove(0, nl + 1);
        nl = tmp.Find(_T('\n'));
        if (nl == -1)
        {
            line = tmp;
            tmp = _T("");
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
            tmp.Replace(_T("\r"), _T(" "), true); // remove CRs
            tmp.Replace(_T("\n"), _T(" "), true); // remove LFs
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
        Compiler* compiler = CompilerFactory::GetCompiler(compilerIdx);

		m_IncludeDirs = compiler->GetIncludeDirs();
		m_ResDirs = compiler->GetResourceIncludeDirs();
		m_LibDirs = compiler->GetLibDirs();
		m_CompilerOptions = compiler->GetCompilerOptions();
		m_LinkerOptions = compiler->GetLinkerOptions();
		m_LinkLibs = compiler->GetLinkLibs();
		m_CommandsAfterBuild = compiler->GetCommandsAfterBuild();
		m_CommandsBeforeBuild = compiler->GetCommandsBeforeBuild();
		m_AlwaysUsePost = compiler->GetAlwaysRunPostBuildSteps();

        wxComboBox* cmb = XRCCTRL(*this, "cmbLogging", wxComboBox);
        if (cmb)
            cmb->SetSelection((int)compiler->GetSwitches().logging);
//        cmb = XRCCTRL(*this, "cmbBuildMethod", wxComboBox);
//        if (cmb)
//            cmb->SetSelection((int)compiler->GetSwitches().buildMethod);
	}
	else
	{
		if (!data->GetTarget())
		{
			// project options
			SetTitle(_("Project build options"));
			m_pTarget = 0;
			cbProject* project = data->GetProject();
			m_IncludeDirs = project->GetIncludeDirs();
			m_ResDirs = project->GetResourceIncludeDirs();
			m_LibDirs = project->GetLibDirs();
			m_CompilerOptions = project->GetCompilerOptions();
			m_LinkerOptions = project->GetLinkerOptions();
			m_LinkLibs = project->GetLinkLibs();
			m_CommandsAfterBuild = project->GetCommandsAfterBuild();
			m_CommandsBeforeBuild = project->GetCommandsBeforeBuild();
			m_AlwaysUsePost = project->GetAlwaysRunPostBuildSteps();

            XRCCTRL(*this, "txtMakeCmd_Build", wxTextCtrl)->SetValue(project->GetMakeCommandFor(mcBuild));
            XRCCTRL(*this, "txtMakeCmd_Compile", wxTextCtrl)->SetValue(project->GetMakeCommandFor(mcCompileFile));
            XRCCTRL(*this, "txtMakeCmd_Clean", wxTextCtrl)->SetValue(project->GetMakeCommandFor(mcClean));
            XRCCTRL(*this, "txtMakeCmd_DistClean", wxTextCtrl)->SetValue(project->GetMakeCommandFor(mcDistClean));
        }
		else
		{
			// target options
			ProjectBuildTarget* target = data->GetTarget();
			SetTitle(_("Target build options: ") + target->GetTitle());
			m_pTarget = target;
			m_IncludeDirs = target->GetIncludeDirs();
			m_ResDirs = target->GetResourceIncludeDirs();
			m_LibDirs = target->GetLibDirs();
			m_CompilerOptions = target->GetCompilerOptions();
			m_LinkerOptions = target->GetLinkerOptions();
			m_LinkLibs = target->GetLinkLibs();
			m_CommandsAfterBuild = target->GetCommandsAfterBuild();
			m_CommandsBeforeBuild = target->GetCommandsBeforeBuild();
			m_AlwaysUsePost = target->GetAlwaysRunPostBuildSteps();
			XRCCTRL(*this, "cmbCompilerPolicy", wxComboBox)->SetSelection(target->GetOptionRelation(ortCompilerOptions));
			XRCCTRL(*this, "cmbLinkerPolicy", wxComboBox)->SetSelection(target->GetOptionRelation(ortLinkerOptions));
			XRCCTRL(*this, "cmbIncludesPolicy", wxComboBox)->SetSelection(target->GetOptionRelation(ortIncludeDirs));
			XRCCTRL(*this, "cmbLibDirsPolicy", wxComboBox)->SetSelection(target->GetOptionRelation(ortLibDirs));
			XRCCTRL(*this, "cmbResDirsPolicy", wxComboBox)->SetSelection(target->GetOptionRelation(ortResDirs));

            XRCCTRL(*this, "txtMakeCmd_Build", wxTextCtrl)->SetValue(target->GetMakeCommandFor(mcBuild));
            XRCCTRL(*this, "txtMakeCmd_Compile", wxTextCtrl)->SetValue(target->GetMakeCommandFor(mcCompileFile));
            XRCCTRL(*this, "txtMakeCmd_Clean", wxTextCtrl)->SetValue(target->GetMakeCommandFor(mcClean));
            XRCCTRL(*this, "txtMakeCmd_DistClean", wxTextCtrl)->SetValue(target->GetMakeCommandFor(mcDistClean));
		}
	}
	TextToOptions();

    DoFillVars();
	DoFillOptions();
	DoFillCompileDirs(m_IncludeDirs, XRCCTRL(*this, "lstIncludeDirs", wxListBox));
	DoFillCompileDirs(m_LibDirs, XRCCTRL(*this, "lstLibDirs", wxListBox));
	DoFillCompileDirs(m_ResDirs, XRCCTRL(*this, "lstResDirs", wxListBox));
	DoFillCompileOptions(m_CompilerOptions, XRCCTRL(*this, "txtCompilerOptions", wxTextCtrl));
	DoFillCompileOptions(m_LinkerOptions, XRCCTRL(*this, "txtLinkerOptions", wxTextCtrl));

    // only if "Commands" page exists
	if (XRCCTRL(*this, "txtCmdBefore", wxTextCtrl))
	{
        DoFillCompileOptions(m_CommandsBeforeBuild, XRCCTRL(*this, "txtCmdBefore", wxTextCtrl));
        DoFillCompileOptions(m_CommandsAfterBuild, XRCCTRL(*this, "txtCmdAfter", wxTextCtrl));
        XRCCTRL(*this, "chkAlwaysRunPost", wxCheckBox)->SetValue(m_AlwaysUsePost);
    }
}

void CompilerOptionsDlg::DoSaveOptions(int compilerIdx, ScopeTreeData* data)
{
    // only if "Commands" page exists
	if (XRCCTRL(*this, "txtCmdBefore", wxTextCtrl))
	{
        m_AlwaysUsePost = XRCCTRL(*this, "chkAlwaysRunPost", wxCheckBox)->GetValue();
        DoGetCompileOptions(m_CommandsBeforeBuild, XRCCTRL(*this, "txtCmdBefore", wxTextCtrl));
        DoGetCompileOptions(m_CommandsAfterBuild, XRCCTRL(*this, "txtCmdAfter", wxTextCtrl));
	}
	DoGetCompileDirs(m_IncludeDirs, XRCCTRL(*this, "lstIncludeDirs", wxListBox));
	DoGetCompileDirs(m_LibDirs, XRCCTRL(*this, "lstLibDirs", wxListBox));
	DoGetCompileDirs(m_ResDirs, XRCCTRL(*this, "lstResDirs", wxListBox));
	DoGetCompileOptions(m_CompilerOptions, XRCCTRL(*this, "txtCompilerOptions", wxTextCtrl));
	DoGetCompileOptions(m_LinkerOptions, XRCCTRL(*this, "txtLinkerOptions", wxTextCtrl));
    OptionsToText();

	if (!data)
	{
		// global options
        Compiler* compiler = CompilerFactory::GetCompiler(compilerIdx);

		compiler->SetIncludeDirs(m_IncludeDirs);
		compiler->SetLibDirs(m_LibDirs);
		compiler->SetResourceIncludeDirs(m_ResDirs);
		compiler->SetCompilerOptions(m_CompilerOptions);
		compiler->SetLinkerOptions(m_LinkerOptions);
		compiler->SetLinkLibs(m_LinkLibs);
		compiler->SetCommandsBeforeBuild(m_CommandsBeforeBuild);
		compiler->SetCommandsAfterBuild(m_CommandsAfterBuild);
		compiler->SetAlwaysRunPostBuildSteps(m_AlwaysUsePost);

        wxComboBox* cmb = XRCCTRL(*this, "cmbLogging", wxComboBox);
        if (cmb)
        {
            CompilerSwitches switches = compiler->GetSwitches();
            switches.logging = (CompilerLoggingType)cmb->GetSelection();
            compiler->SetSwitches(switches);
        }
//        cmb = XRCCTRL(*this, "cmbBuildMethod", wxComboBox);
//        if (cmb)
//        {
//            CompilerSwitches switches = compiler->GetSwitches();
//            switches.buildMethod = (CompilerBuildMethod)cmb->GetSelection();
//            compiler->SetSwitches(switches);
//        }
	}
	else
	{
		if (!data->GetTarget())
		{
			// project options
			cbProject* project = data->GetProject();
			project->SetIncludeDirs(m_IncludeDirs);
			project->SetResourceIncludeDirs(m_ResDirs);
			project->SetLibDirs(m_LibDirs);
			project->SetCompilerOptions(m_CompilerOptions);
			project->SetLinkerOptions(m_LinkerOptions);
			project->SetLinkLibs(m_LinkLibs);
			project->SetCommandsBeforeBuild(m_CommandsBeforeBuild);
			project->SetCommandsAfterBuild(m_CommandsAfterBuild);
            project->SetAlwaysRunPostBuildSteps(m_AlwaysUsePost);

            project->SetMakeCommandFor(mcBuild, XRCCTRL(*this, "txtMakeCmd_Build", wxTextCtrl)->GetValue());
            project->SetMakeCommandFor(mcCompileFile, XRCCTRL(*this, "txtMakeCmd_Compile", wxTextCtrl)->GetValue());
            project->SetMakeCommandFor(mcClean, XRCCTRL(*this, "txtMakeCmd_Clean", wxTextCtrl)->GetValue());
            project->SetMakeCommandFor(mcDistClean, XRCCTRL(*this, "txtMakeCmd_DistClean", wxTextCtrl)->GetValue());
		}
		else
		{
			// target options
			ProjectBuildTarget* target = data->GetTarget();
			target->SetIncludeDirs(m_IncludeDirs);
			target->SetResourceIncludeDirs(m_ResDirs);
			target->SetLibDirs(m_LibDirs);
			target->SetCompilerOptions(m_CompilerOptions);
			target->SetLinkerOptions(m_LinkerOptions);
			target->SetLinkLibs(m_LinkLibs);
            target->SetOptionRelation(ortCompilerOptions, OptionsRelation(XRCCTRL(*this, "cmbCompilerPolicy", wxComboBox)->GetSelection()));
            target->SetOptionRelation(ortLinkerOptions, OptionsRelation(XRCCTRL(*this, "cmbLinkerPolicy", wxComboBox)->GetSelection()));
            target->SetOptionRelation(ortIncludeDirs, OptionsRelation(XRCCTRL(*this, "cmbIncludesPolicy", wxComboBox)->GetSelection()));
            target->SetOptionRelation(ortLibDirs, OptionsRelation(XRCCTRL(*this, "cmbLibDirsPolicy", wxComboBox)->GetSelection()));
            target->SetOptionRelation(ortResDirs, OptionsRelation(XRCCTRL(*this, "cmbResDirsPolicy", wxComboBox)->GetSelection()));
			target->SetCommandsBeforeBuild(m_CommandsBeforeBuild);
			target->SetCommandsAfterBuild(m_CommandsAfterBuild);
            target->SetAlwaysRunPostBuildSteps(m_AlwaysUsePost);

            target->SetMakeCommandFor(mcBuild, XRCCTRL(*this, "txtMakeCmd_Build", wxTextCtrl)->GetValue());
            target->SetMakeCommandFor(mcCompileFile, XRCCTRL(*this, "txtMakeCmd_Compile", wxTextCtrl)->GetValue());
            target->SetMakeCommandFor(mcClean, XRCCTRL(*this, "txtMakeCmd_Clean", wxTextCtrl)->GetValue());
            target->SetMakeCommandFor(mcDistClean, XRCCTRL(*this, "txtMakeCmd_DistClean", wxTextCtrl)->GetValue());
		}
	}
}

void CompilerOptionsDlg::DoMakeRelative(wxFileName& path)
{
    // NOTE: this function is not currently used

	wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
	ScopeTreeData* data = (ScopeTreeData*)tc->GetItemData(tc->GetSelection());
	if (data && data->GetProject())
	{
        path.MakeRelativeTo(data->GetProject()->GetBasePath());
    }
}

void CompilerOptionsDlg::DoSaveCompilerPrograms(int compilerIdx)
{
    Compiler* compiler = CompilerFactory::GetCompiler(compilerIdx);
    if (m_pProject || !compiler) // no "Programs" page or no compiler
    {
        return;
    }
    CompilerPrograms progs;
    wxString masterPath = XRCCTRL(*this, "txtMasterPath", wxTextCtrl)->GetValue();
    progs.C = XRCCTRL(*this, "txtCcompiler", wxTextCtrl)->GetValue();
    progs.CPP = XRCCTRL(*this, "txtCPPcompiler", wxTextCtrl)->GetValue();
    progs.LD = XRCCTRL(*this, "txtLinker", wxTextCtrl)->GetValue();
    progs.LIB = XRCCTRL(*this, "txtLibLinker", wxTextCtrl)->GetValue();
    progs.WINDRES = XRCCTRL(*this, "txtResComp", wxTextCtrl)->GetValue();
    progs.MAKE = XRCCTRL(*this, "txtMake", wxTextCtrl)->GetValue();
    progs.DBG = XRCCTRL(*this, "txtDebugger", wxTextCtrl)->GetValue();
    compiler->SetPrograms(progs);
    compiler->SetMasterPath(masterPath);
    compiler->SetOptions(m_Options);
}

// events

void CompilerOptionsDlg::OnTreeSelectionChange(wxTreeEvent& event)
{
	if (m_BuildingTree)
		return;
	wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
	ScopeTreeData* data = (ScopeTreeData*)tc->GetItemData(event.GetItem());
	if (!data)
        return;
    int compilerIdx = data->GetTarget() ? CompilerFactory::GetCompilerIndex(data->GetTarget()->GetCompilerID()) :
                        (data->GetProject() ? CompilerFactory::GetCompilerIndex(data->GetProject()->GetCompilerID()) :
                        XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection());
    XRCCTRL(*this, "cmbCompiler", wxComboBox)->SetSelection(compilerIdx);
    CompilerChanged(data);
    m_pTarget = data->GetTarget();
//	DoLoadOptions(compilerIdx, data);
}

void CompilerOptionsDlg::OnTreeSelectionChanging(wxTreeEvent& event)
{
	if (m_BuildingTree)
		return;
	wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
	ScopeTreeData* data = (ScopeTreeData*)tc->GetItemData(event.GetOldItem());
	if (!data)
        return;
    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
	DoSaveOptions(compilerIdx, data);
}

void CompilerOptionsDlg::OnCompilerChanged(wxCommandEvent& event)
{
	wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
	ScopeTreeData* data = tc ? (ScopeTreeData*)tc->GetItemData(tc->GetSelection()) : 0;
	DoSaveCompilerPrograms(m_LastCompilerIdx);
	DoSaveOptions(m_LastCompilerIdx, data);
    CompilerChanged(data);
}

void CompilerOptionsDlg::CompilerChanged(ScopeTreeData* data)
{
    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
    wxString id = CompilerFactory::GetCompiler(compilerIdx)->GetID();
    if (data)
    {
        if (data->GetTarget())
             data->GetTarget()->SetCompilerID(id);
        else if (data->GetProject())
            data->GetProject()->SetCompilerID(id);
    }
    else if (m_pProject)
        m_pProject->SetCompilerID(id);

    m_Options = CompilerFactory::GetCompiler(compilerIdx)->GetOptions();
    DoFillCompilerPrograms();
    DoFillCategories();
    DoFillOptions();

	if (m_BuildingTree)
		return;
	DoLoadOptions(compilerIdx, data);
	m_LastCompilerIdx = compilerIdx;

	// this relies on GetCustomVars(), which relies on m_LastCompilerIdx
	DoFillVars();
}

void CompilerOptionsDlg::UpdateCompilerForTargets(int compilerIdx)
{
    int ret = cbMessageBox(_("You have changed the compiler used for the project.\n"
                            "Do you want to use the same compiler for all the project's build targets too?"),
                            _("Question"),
                            wxICON_QUESTION | wxYES_NO);
    if (ret == wxID_YES)
    {
        for (int i = 0; i < m_pProject->GetBuildTargetsCount(); ++i)
        {
            ProjectBuildTarget* target = m_pProject->GetBuildTarget(i);
            target->SetCompilerID(CompilerFactory::GetCompiler(compilerIdx)->GetID());
        }
    }
}

void CompilerOptionsDlg::AutoDetectCompiler()
{
    wxComboBox* cmb = XRCCTRL(*this, "cmbCompiler", wxComboBox);
    int compilerIdx = cmb->GetSelection();
    Compiler* compiler = CompilerFactory::GetCompiler(compilerIdx);
    wxString backup = XRCCTRL(*this, "txtMasterPath", wxTextCtrl)->GetValue();

    wxArrayString empty;
    compiler->SetExtraPaths(empty);

    switch (compiler->AutoDetectInstallationDir())
    {
        case adrDetected:
        {
            wxString msg;
            msg.Printf(_("Auto-detected installation path of \"%s\"\nin \"%s\""), compiler->GetName().c_str(), compiler->GetMasterPath().c_str());
            cbMessageBox(msg);
        }
        break;

        case adrGuessed:
        {
            wxString msg;
            msg.Printf(_("Could not auto-detect installation path of \"%s\"...\n"
                        "Do you want to use this compiler's default installation directory?"),
                        compiler->GetName().c_str());
            if (cbMessageBox(msg, _("Confirmation"), wxICON_QUESTION | wxYES_NO) == wxID_NO)
                compiler->SetMasterPath(backup);
        }
        break;
    }
    XRCCTRL(*this, "txtMasterPath", wxTextCtrl)->SetValue(compiler->GetMasterPath());
    XRCCTRL(*this, "lstExtraPaths", wxListBox)->Clear();
    const wxArrayString& extraPaths = CompilerFactory::GetCompiler(compilerIdx)->GetExtraPaths();
    for (unsigned int i = 0; i < extraPaths.GetCount(); ++i)
    {
        XRCCTRL(*this, "lstExtraPaths", wxListBox)->Append(extraPaths[i]);
    }
}

wxListBox* CompilerOptionsDlg::GetDirsListBox()
{
    wxNotebook* nb = XRCCTRL(*this, "nbDirs", wxNotebook);
    if (!nb)
        return 0;
    switch (nb->GetSelection())
    {
        case 0: // compiler dirs
            return XRCCTRL(*this, "lstIncludeDirs", wxListBox);
        case 1: // linker dirs
            return XRCCTRL(*this, "lstLibDirs", wxListBox);
        case 2: // resource compiler dirs
            return XRCCTRL(*this, "lstResDirs", wxListBox);
        default: break;
    }
    return 0;
}

CompileOptionsBase* CompilerOptionsDlg::GetVarsOwner()
{
	wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
    ScopeTreeData* data = tc ? (ScopeTreeData*)tc->GetItemData(tc->GetSelection()) : 0;
    if (!data)
    {
        Compiler* compiler = CompilerFactory::GetCompiler(m_LastCompilerIdx);
        return compiler;
    }
    if (data->GetTarget())
        return data->GetTarget();
    return m_pProject;
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
    EditPathDlg dlg(this,
            m_pProject ? m_pProject->GetBasePath() : _T(""),
            m_pProject ? m_pProject->GetBasePath() : _T(""),
            _("Add directory"));

    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxString path = dlg.GetPath();

        wxListBox* control = GetDirsListBox();
        if (control)
            control->Append(path);
    }
}

void CompilerOptionsDlg::OnEditDirClick(wxCommandEvent& event)
{
    wxListBox* control = GetDirsListBox();
    if (!control || control->GetSelection() < 0)
        return;

    EditPathDlg dlg(this,
            control->GetString(control->GetSelection()),
            m_pProject ? m_pProject->GetBasePath() : _T(""),
            _("Edit directory"));

    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxString path = dlg.GetPath();

        control->SetString(control->GetSelection(), path);
    }
}

void CompilerOptionsDlg::OnRemoveDirClick(wxCommandEvent& event)
{
    wxListBox* control = GetDirsListBox();
    if (!control || control->GetSelection() < 0)
        return;
	if (cbMessageBox(_("Remove '")+control->GetStringSelection()+_("' from the list?"),
					_("Confirmation"),
					wxOK | wxCANCEL | wxICON_QUESTION) == wxID_OK)
	{
        control->Delete(control->GetSelection());
    }
}

void CompilerOptionsDlg::OnClearDirClick(wxCommandEvent& event)
{
    wxListBox* control = GetDirsListBox();
    if (!control || control->GetCount() == 0)
        return;
	if (cbMessageBox(_("Remove all directories from the list?"),
					_("Confirmation"),
					wxOK | wxCANCEL | wxICON_QUESTION) == wxID_OK)
	{
        control->Clear();
    }
}

void CompilerOptionsDlg::OnAddVarClick(wxCommandEvent& event)
{
    CompileOptionsBase* base = GetVarsOwner();
    if (!base)
        return;

    wxString key;
    wxString value;
    EditPairDlg dlg(this, key, value, _("Add new variable"), EditPairDlg::bmBrowseForDirectory);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        key.Trim(true).Trim(false);
        value.Trim(true).Trim(false);
        ::QuoteStringIfNeeded(value);
        base->SetVar(key, value);
        XRCCTRL(*this, "lstVars", wxListBox)->Append(key + _T(" = ") + value);
    }
}

void CompilerOptionsDlg::OnEditVarClick(wxCommandEvent& event)
{
	int sel = XRCCTRL(*this, "lstVars", wxListBox)->GetSelection();
	if (sel == -1)
		return;

    CompileOptionsBase* base = GetVarsOwner();
    if (!base)
        return;

    wxString key = XRCCTRL(*this, "lstVars", wxListBox)->GetStringSelection().BeforeFirst(_T('=')).Trim(true).Trim(false);
	if (key.IsEmpty())
		return;
    wxString old_key = key;
    wxString value = XRCCTRL(*this, "lstVars", wxListBox)->GetStringSelection().AfterFirst(_T('=')).Trim(true).Trim(false);
    wxString old_value = value;

    EditPairDlg dlg(this, key, value, _("Edit variable"), EditPairDlg::bmBrowseForDirectory);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        key.Trim(true).Trim(false);
        value.Trim(true).Trim(false);
        ::QuoteStringIfNeeded(value);

        if (value != old_value)
        {
            if (key != old_key)
                base->UnsetVar(key);
            base->SetVar(key, value);
            XRCCTRL(*this, "lstVars", wxListBox)->SetString(sel, key + _T(" = ") + value);
        }
	}
}

void CompilerOptionsDlg::OnRemoveVarClick(wxCommandEvent& event)
{
	int sel = XRCCTRL(*this, "lstVars", wxListBox)->GetSelection();
	if (sel == -1)
		return;

    CompileOptionsBase* base = GetVarsOwner();
    if (!base)
        return;

    wxString key = XRCCTRL(*this, "lstVars", wxListBox)->GetStringSelection().BeforeFirst(_T('=')).Trim(true);
	if (key.IsEmpty())
		return;

	if (cbMessageBox(_("Are you sure you want to delete this variable?"),
					_("Confirmation"),
					wxYES_NO | wxICON_QUESTION) == wxID_YES)
	{
	    base->UnsetVar(key);
        XRCCTRL(*this, "lstVars", wxListBox)->Delete(sel);
	}
}

void CompilerOptionsDlg::OnClearVarClick(wxCommandEvent& event)
{
	wxListBox* lstVars = XRCCTRL(*this, "lstVars", wxListBox);
	if (lstVars->IsEmpty())
		return;

  CompileOptionsBase* base = GetVarsOwner();
  if (!base)
    return;

	if (cbMessageBox(_("Are you sure you want to clear all variables?"),
                   _("Confirmation"),
                   wxYES | wxNO | wxICON_QUESTION) == wxID_YES)
	{
        // Unset all variables of lstVars
        for (int i=0; i<lstVars->GetCount(); i++)
        {
          wxString key = lstVars->GetString(i).BeforeFirst(_T('=')).Trim(true);
          if (!key.IsEmpty())
            base->UnsetVar(key);
        }

        lstVars->Clear();
	}
}

void CompilerOptionsDlg::OnSetDefaultCompilerClick(wxCommandEvent& event)
{
    wxComboBox* cmb = XRCCTRL(*this, "cmbCompiler", wxComboBox);
    int idx = cmb->GetSelection();
    CompilerFactory::SetDefaultCompiler(idx);
    wxString msg;
    msg.Printf(_("%s is now selected as the default compiler for new projects"), CompilerFactory::GetDefaultCompiler()->GetName().c_str());
    cbMessageBox(msg);
}

void CompilerOptionsDlg::OnAddCompilerClick(wxCommandEvent& event)
{
    wxComboBox* cmb = XRCCTRL(*this, "cmbCompiler", wxComboBox);
    int idx = cmb->GetSelection();
	wxString value = wxGetTextFromUser(_("Please enter the new compiler's name:"),
                                    _("Add new compiler"),
                                    _("Copy of ") + CompilerFactory::GetCompiler(idx)->GetName());
	if (!value.IsEmpty())
	{
        // make a copy of current compiler
        Compiler* newC = 0;
        try
        {
            newC = CompilerFactory::CreateCompilerCopy(CompilerFactory::GetCompiler(idx), value);
        }
        catch (cbException& e)
        {
            // usually throws because of non-unique ID
            e.ShowErrorMessage(false);
            return;
        }

        if (!newC)
        {
            cbMessageBox(_("The new compiler could not be created.\n(maybe a compiler with the same name already exists?)"),
                        _("Error"), wxICON_ERROR);
            return;
        }

        int newIdx = CompilerFactory::GetCompilerIndex(newC);

        cmb->Append(value);
        cmb->SetSelection(cmb->GetCount() - 1);
        // refresh settings in dialog
        DoFillCompilerPrograms();
        DoFillCategories();
        DoFillOptions();
        DoLoadOptions(newIdx, 0);
        m_LastCompilerIdx = newIdx;
        cbMessageBox(_("The new compiler has been added! Don't forget to update the \"Programs\" page..."));
    }
}

void CompilerOptionsDlg::OnEditCompilerClick(wxCommandEvent& event)
{
    wxComboBox* cmb = XRCCTRL(*this, "cmbCompiler", wxComboBox);
	wxString value = wxGetTextFromUser(_("Please edit the compiler's name:"), _("Rename compiler"), cmb->GetValue());
	if (!value.IsEmpty())
	{
        int idx = cmb->GetSelection();
        CompilerFactory::GetCompiler(idx)->SetName(value);
        cmb->SetString(idx, value);
        cmb->SetSelection(idx);
    }
}

void CompilerOptionsDlg::OnRemoveCompilerClick(wxCommandEvent& event)
{
	if (cbMessageBox(_("Are you sure you want to remove this compiler?"),
					_("Confirmation"),
					wxOK | wxCANCEL | wxICON_QUESTION | wxNO_DEFAULT) == wxID_OK)
    {
        wxComboBox* cmb = XRCCTRL(*this, "cmbCompiler", wxComboBox);
        int compilerIdx = cmb->GetSelection();
        CompilerFactory::RemoveCompiler(CompilerFactory::GetCompiler(compilerIdx));
        cmb->Delete(compilerIdx);
        while (compilerIdx >= cmb->GetCount())
            --compilerIdx;
        cmb->SetSelection(compilerIdx);

        DoFillCompilerPrograms();
        DoFillCategories();
        DoFillOptions();
        m_LastCompilerIdx = compilerIdx;
        DoLoadOptions(compilerIdx, 0);
    }
}

void CompilerOptionsDlg::OnResetCompilerClick(wxCommandEvent& event)
{
	if (cbMessageBox(_("Reset this compiler's settings to the defaults?"),
					_("Confirmation"),
					wxOK | wxCANCEL | wxICON_QUESTION | wxNO_DEFAULT) == wxID_OK)
	if (cbMessageBox(_("Reset this compiler's settings to the defaults?\n"
	                   "\nAre you REALLY sure?"),
					_("Confirmation"),
					wxOK | wxCANCEL | wxICON_QUESTION | wxNO_DEFAULT) == wxID_OK)
    {
        wxComboBox* cmb = XRCCTRL(*this, "cmbCompiler", wxComboBox);
        int compilerIdx = cmb->GetSelection();
        CompilerFactory::GetCompiler(compilerIdx)->Reset();
        // run auto-detection
        AutoDetectCompiler();
        CompilerFactory::SaveSettings();
        // refresh settings in dialog
        DoFillCompilerPrograms();
        DoFillCategories();
        DoFillOptions();
        DoLoadOptions(compilerIdx, 0);
    }
}

void CompilerOptionsDlg::OnAddLibClick(wxCommandEvent& event)
{
    /*int compilerIdx = m_pTarget ? m_pTarget->GetCompilerID()
                                : (m_pProject ? m_pProject->GetCompilerID()
                                              : XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection());*/
    wxListBox* lstLibs = XRCCTRL(*this, "lstLibs", wxListBox);
    /*LinkLibDlg dlg(this, m_pProject, m_pTarget, CompilerFactory::GetCompiler(compilerIdx), "");*/

    EditPathDlg dlg(this,
            _T(""),
            m_pProject ? m_pProject->GetBasePath() : _T(""),
            _("Add library"),
            _("Choose library to link"),
            false,
            true,
            _("Library files (*.a, *.so, *.lib)|*.a;*.so;*.lib|All files (*)|*"));

    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxArrayString paths = GetArrayFromString(dlg.GetPath());
        for (unsigned int i = 0; i < paths.GetCount(); ++i)
            lstLibs->Append(paths[i]);
    }
}

void CompilerOptionsDlg::OnEditLibClick(wxCommandEvent& event)
{
    /*int compilerIdx = m_pTarget ? m_pTarget->GetCompilerID()
                                : (m_pProject ? m_pProject->GetCompilerID()
                                              : XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection());*/
    wxListBox* lstLibs = XRCCTRL(*this, "lstLibs", wxListBox);

    /*LinkLibDlg dlg(this, m_pProject, m_pTarget, CompilerFactory::GetCompiler(compilerIdx), lstLibs->GetStringSelection());*/

    EditPathDlg dlg(this,
            lstLibs->GetStringSelection(),
            m_pProject ? m_pProject->GetBasePath() : _T(""),
            _("Edit library"),
            _("Choose library to link"),
            false,
            false,
            _("Library files (*.a, *.so, *.lib)|*.a;*.so;*.lib|All files (*)|*"));

    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        /*lstLibs->SetString(lstLibs->GetSelection(), dlg.GetLib());*/
        lstLibs->SetString(lstLibs->GetSelection(), dlg.GetPath());
    }
}

void CompilerOptionsDlg::OnRemoveLibClick(wxCommandEvent& event)
{
    wxListBox* lstLibs = XRCCTRL(*this, "lstLibs", wxListBox);
    if (!lstLibs || lstLibs->GetSelection() < 0)
        return;
    if (cbMessageBox(_("Remove library '")+lstLibs->GetStringSelection()+_("' from the list?"), _("Confirmation"), wxICON_QUESTION | wxOK | wxCANCEL) == wxID_OK)
        lstLibs->Delete(lstLibs->GetSelection());
}

void CompilerOptionsDlg::OnClearLibClick(wxCommandEvent& event)
{
    wxListBox* lstLibs = XRCCTRL(*this, "lstLibs", wxListBox);
    if (!lstLibs || lstLibs->GetCount() == 0)
        return;
    if (cbMessageBox(_("Remove all libraries from the list?"), _("Confirmation"), wxICON_QUESTION | wxOK | wxCANCEL) == wxID_OK)
        lstLibs->Clear();
}

void CompilerOptionsDlg::OnAddExtraPathClick(wxCommandEvent& event)
{
    wxString path = ChooseDirectory(this,
                                    _("Select directory"),
                                    _T(""),
                                    _T(""),
                                    true,
                                    true);
    if (path.IsEmpty())
        return;

    wxListBox* control = XRCCTRL(*this, "lstExtraPaths", wxListBox);
    if (control)
    {
        int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
        Compiler* compiler = CompilerFactory::GetCompiler(compilerIdx);
        wxArrayString extraPaths = CompilerFactory::GetCompiler(compilerIdx)->GetExtraPaths();
        if (extraPaths.Index(path) != wxNOT_FOUND)
        {
            cbMessageBox(_("Path already in extra paths list!"), _("Warning"), wxICON_WARNING);
            return;
        }
        extraPaths.Add(path);
        compiler->SetExtraPaths(extraPaths);
        control->Append(path);
    }
}

void CompilerOptionsDlg::OnEditExtraPathClick(wxCommandEvent& event)
{
    wxListBox* control = XRCCTRL(*this, "lstExtraPaths", wxListBox);
    if (!control || control->GetSelection() < 0)
        return;

    wxFileName dir(control->GetString(control->GetSelection()) + wxFileName::GetPathSeparator());
    wxString initial = _T("");
    if (dir.DirExists())
        initial = dir.GetPath(wxPATH_GET_VOLUME);

    wxString path = ChooseDirectory(this,
                                    _("Select directory"),
                                    initial,
                                    _T(""),
                                    true,
                                    true);
    if (path.IsEmpty())
        return;

    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
    Compiler* compiler = CompilerFactory::GetCompiler(compilerIdx);
    wxArrayString extraPaths = CompilerFactory::GetCompiler(compilerIdx)->GetExtraPaths();
    if (extraPaths.Index(path) != wxNOT_FOUND)
    {
        cbMessageBox(_("Path already in extra paths list!"), _("Warning"), wxICON_WARNING);
        return;
    }
    extraPaths[control->GetSelection()] = path;
    compiler->SetExtraPaths(extraPaths);
    control->SetString(control->GetSelection(), path);
}

void CompilerOptionsDlg::OnRemoveExtraPathClick(wxCommandEvent& event)
{
    wxListBox* control = XRCCTRL(*this, "lstExtraPaths", wxListBox);
    if (!control || control->GetSelection() < 0)
        return;

    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
    Compiler* compiler = CompilerFactory::GetCompiler(compilerIdx);
    wxArrayString extraPaths = CompilerFactory::GetCompiler(compilerIdx)->GetExtraPaths();
    extraPaths.RemoveAt(control->GetSelection());
    compiler->SetExtraPaths(extraPaths);
    control->Delete(control->GetSelection());
}

void CompilerOptionsDlg::OnClearExtraPathClick(wxCommandEvent& event)
{
    wxListBox* control = XRCCTRL(*this, "lstExtraPaths", wxListBox);
    if (!control || control->IsEmpty())
        return;

    if (cbMessageBox(_("Remove all extra paths from the list?"), _("Confirmation"), wxICON_QUESTION | wxOK | wxCANCEL) == wxID_OK)
    {
        int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
        Compiler* compiler = CompilerFactory::GetCompiler(compilerIdx);
        wxArrayString empty;
        compiler->SetExtraPaths(empty);
        control->Clear();
    }
}

void CompilerOptionsDlg::OnMoveLibUpClick(wxSpinEvent& event)
{
    wxListBox* lstLibs = XRCCTRL(*this, "lstLibs", wxListBox);
    if (lstLibs->GetSelection() <= 0)
        return;
    int sel = lstLibs->GetSelection();
    wxString lib = lstLibs->GetStringSelection();
    lstLibs->Delete(sel);
    lstLibs->InsertItems(1, &lib, sel - 1);
    lstLibs->SetSelection(sel - 1);
    if (m_pProject)
        m_pProject->SetModified(true);
}

void CompilerOptionsDlg::OnMoveLibDownClick(wxSpinEvent& event)
{
    wxListBox* lstLibs = XRCCTRL(*this, "lstLibs", wxListBox);
    if (lstLibs->GetSelection() == lstLibs->GetCount() - 1)
        return;
    int sel = lstLibs->GetSelection();
    wxString lib = lstLibs->GetStringSelection();
    lstLibs->Delete(sel);
    lstLibs->InsertItems(1, &lib, sel + 1);
    lstLibs->SetSelection(sel + 1);
    if (m_pProject)
        m_pProject->SetModified(true);
}

void CompilerOptionsDlg::OnMoveDirUpClick(wxSpinEvent& event)
{
    wxListBox* lst = GetDirsListBox();
    if (!lst || lst->GetSelection() <= 0)
        return;
    int sel = lst->GetSelection();
    wxString lib = lst->GetStringSelection();
    lst->Delete(sel);
    lst->InsertItems(1, &lib, sel - 1);
    lst->SetSelection(sel - 1);
    if (m_pProject)
        m_pProject->SetModified(true);
}

void CompilerOptionsDlg::OnMoveDirDownClick(wxSpinEvent& event)
{
    wxListBox* lst = GetDirsListBox();
    if (!lst || lst->GetSelection() == lst->GetCount() - 1)
        return;
    int sel = lst->GetSelection();
    wxString lib = lst->GetStringSelection();
    lst->Delete(sel);
    lst->InsertItems(1, &lib, sel + 1);
    lst->SetSelection(sel + 1);
    if (m_pProject)
        m_pProject->SetModified(true);
}

void CompilerOptionsDlg::OnMasterPathClick(wxCommandEvent& event)
{
    wxString path = ChooseDirectory(this,
                                    _("Select directory"),
                                    XRCCTRL(*this, "txtMasterPath", wxTextCtrl)->GetValue());
    if (path.IsEmpty())
        return;
    if (!path.IsEmpty())
    {
        XRCCTRL(*this, "txtMasterPath", wxTextCtrl)->SetValue(path);
        int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
        DoSaveCompilerPrograms(compilerIdx);
    }
}

void CompilerOptionsDlg::OnAutoDetectClick(wxCommandEvent& event)
{
    AutoDetectCompiler();
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
    else if (event.GetId() == XRCID("btnLibLinker"))
        obj = XRCCTRL(*this, "txtLibLinker", wxTextCtrl);
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
                            XRCCTRL(*this, "txtMasterPath", wxTextCtrl)->GetValue() + _T("/bin"),
                            obj->GetValue(),
                            #ifdef __WXMSW__
                            _("Executable files (*.exe)|*.exe"),
                            #else
                            _("All files (*)|*"),
                            #endif
                            wxOPEN | wxFILE_MUST_EXIST | wxHIDE_READONLY);
    dlg->SetFilterIndex(0);

    PlaceWindow(dlg);
    if (dlg->ShowModal() != wxID_OK)
        return;
    wxFileName fname(dlg->GetPath());
    obj->SetValue(fname.GetFullName());
    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
    DoSaveCompilerPrograms(compilerIdx);
}

void CompilerOptionsDlg::OnAdvancedClick(wxCommandEvent& event)
{
    AnnoyingDialog dlg(_("Edit advanced compiler settings?"),
                        _("The compiler's advanced settings, need command-line "
                        "compiler knowledge to be tweaked.\nIf you don't know "
                        "*exactly* what you 're doing, it is suggested to "
                        "NOT tamper with these...\n\n"
                        "Are you sure you want to proceed?"),
					wxART_QUESTION,
					AnnoyingDialog::YES_NO,
					wxID_YES);
	if (dlg.ShowModal() == wxID_YES)
    {
        wxComboBox* cmb = XRCCTRL(*this, "cmbCompiler", wxComboBox);
        int compilerIdx = cmb->GetSelection();
        AdvancedCompilerOptionsDlg dlg(this, CompilerFactory::GetCompiler(compilerIdx)->GetID());
        PlaceWindow(&dlg);
        dlg.ShowModal();
    }
}

void CompilerOptionsDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    wxListBox* control = GetDirsListBox();
    if (control)
    {
        // add/edit/delete/clear dir
        bool en = control->GetSelection() >= 0;
        XRCCTRL(*this, "btnEditDir", wxButton)->Enable(en);
        XRCCTRL(*this, "btnDelDir", wxButton)->Enable(en);
        XRCCTRL(*this, "btnClearDir", wxButton)->Enable(control->GetCount() != 0);

        // moveup/movedown dir
        XRCCTRL(*this, "spnDirs", wxSpinButton)->Enable(en);
    }

    // add/edit/delete/moveup/movedown lib
    bool en = XRCCTRL(*this, "lstLibs", wxListBox)->GetSelection() >= 0;
    XRCCTRL(*this, "btnEditLib", wxButton)->Enable(en);
    XRCCTRL(*this, "btnDelLib", wxButton)->Enable(en);
    XRCCTRL(*this, "btnClearLib", wxButton)->Enable(XRCCTRL(*this, "lstLibs", wxListBox)->GetCount() != 0);
    XRCCTRL(*this, "spnLibs", wxSpinButton)->Enable(en);

    // add/edit/delete/clear vars
    if (XRCCTRL(*this, "lstVars", wxListBox))
    {
        en = XRCCTRL(*this, "lstVars", wxListBox)->GetSelection() >= 0;
        XRCCTRL(*this, "btnEditVar", wxButton)->Enable(en);
        XRCCTRL(*this, "btnDeleteVar", wxButton)->Enable(en);
        XRCCTRL(*this, "btnClearVar", wxButton)->Enable(XRCCTRL(*this, "lstVars", wxListBox)->GetCount() != 0);
    }

    // policies
	wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
	ScopeTreeData* data = (ScopeTreeData*)tc->GetItemData(tc->GetSelection());
	en = (data && data->GetTarget());
    XRCCTRL(*this, "cmbCompilerPolicy", wxComboBox)->Enable(en);
    XRCCTRL(*this, "cmbLinkerPolicy", wxComboBox)->Enable(en);
    XRCCTRL(*this, "cmbIncludesPolicy", wxComboBox)->Enable(en);
    XRCCTRL(*this, "cmbLibDirsPolicy", wxComboBox)->Enable(en);
    XRCCTRL(*this, "cmbResDirsPolicy", wxComboBox)->Enable(en);

    // compiler set buttons
    if (XRCCTRL(*this, "btnAddCompiler", wxButton)) // only if exist
    {
        en = !data; // global options selected
        int idx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
        int count = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetCount(); // compilers count
        Compiler* compiler = CompilerFactory::GetCompiler(idx);
        XRCCTRL(*this, "btnSetDefaultCompiler", wxButton)->Enable(CompilerFactory::GetDefaultCompilerID() != idx);
        XRCCTRL(*this, "btnAddCompiler", wxButton)->Enable(en);
        XRCCTRL(*this, "btnRenameCompiler", wxButton)->Enable(en && count);
        XRCCTRL(*this, "btnDelCompiler", wxButton)->Enable(en &&
                                                        compiler &&
                                                        !compiler->GetParentID().IsEmpty());
        XRCCTRL(*this, "btnResetCompiler", wxButton)->Enable(en &&
                                                        compiler &&
                                                        compiler->GetParentID().IsEmpty());
    }

    // compiler programs
    if (XRCCTRL(*this, "txtMasterPath", wxTextCtrl)) // "Programs" page exists?
    {
        en = !data; // global options selected
        int extraSel = XRCCTRL(*this, "lstExtraPaths", wxListBox)->GetSelection();
        XRCCTRL(*this, "txtMasterPath", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnMasterPath", wxButton)->Enable(en);
        XRCCTRL(*this, "btnExtraAdd", wxButton)->Enable(en);
        XRCCTRL(*this, "btnExtraEdit", wxButton)->Enable(en && extraSel != -1);
        XRCCTRL(*this, "btnExtraDelete", wxButton)->Enable(en && extraSel != -1);
        XRCCTRL(*this, "btnExtraClear", wxButton)->Enable(en && XRCCTRL(*this, "lstExtraPaths", wxListBox)->GetCount() != 0);
        XRCCTRL(*this, "txtCcompiler", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnCcompiler", wxButton)->Enable(en);
        XRCCTRL(*this, "txtCPPcompiler", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnCPPcompiler", wxButton)->Enable(en);
        XRCCTRL(*this, "txtLinker", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnLinker", wxButton)->Enable(en);
        XRCCTRL(*this, "txtLibLinker", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnLibLinker", wxButton)->Enable(en);
        XRCCTRL(*this, "txtDebugger", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnDebugger", wxButton)->Enable(en);
        XRCCTRL(*this, "txtResComp", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnResComp", wxButton)->Enable(en);
        XRCCTRL(*this, "txtMake", wxTextCtrl)->Enable(en);
        XRCCTRL(*this, "btnMake", wxButton)->Enable(en);
        XRCCTRL(*this, "cmbCompiler", wxComboBox)->Enable(en);
    }
}

void CompilerOptionsDlg::OnApply()
{
	wxTreeCtrl* tc = XRCCTRL(*this, "tcScope", wxTreeCtrl);
	ScopeTreeData* data = (ScopeTreeData*)tc->GetItemData(tc->GetSelection());
    int compilerIdx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
	DoSaveOptions(compilerIdx, data);
	CompilerFactory::SaveSettings();

    // compiler set
    int idx = XRCCTRL(*this, "cmbCompiler", wxComboBox)->GetSelection();
    if (m_pProject && !data->GetTarget() && idx != m_InitialCompilerIdx)
    {
        m_pProject->SetCompilerID(CompilerFactory::GetCompiler(idx)->GetID());
        UpdateCompilerForTargets(idx);
        cbMessageBox(_("You changed the compiler used for this project.\n"
                        "It is recommended that you fully rebuild your project, "
                        "otherwise linking errors might occur..."),
                        _("Notice"),
                        wxICON_EXCLAMATION);
    }

    if (!m_pProject)
    {
        // only do it for global compiler options
        // why does it crash for project compiler options???
        DoSaveCompilerPrograms(idx);
    }

	//others
    wxCheckBox* chk = XRCCTRL(*this, "chkIncludeFileCwd", wxCheckBox);
    if (chk)
        Manager::Get()->GetConfigManager(_T("compiler"))->Write(_T("/include_file_cwd"), (bool)chk->IsChecked());
    chk = XRCCTRL(*this, "chkIncludePrjCwd", wxCheckBox);
    if (chk)
        Manager::Get()->GetConfigManager(_T("compiler"))->Write(_T("/include_prj_cwd"), (bool)chk->IsChecked());
    wxSpinCtrl* spn = XRCCTRL(*this, "spnParallelProcesses", wxSpinCtrl);
    if (spn)
    {
        if (m_Compiler->IsRunning())
            cbMessageBox(_("You can't change the number of parallel processes while building!\nSetting ignored..."), _("Warning"), wxICON_WARNING);
        else
        {
            Manager::Get()->GetConfigManager(_T("compiler"))->Write(_T("/parallel_processes"), (int)spn->GetValue());
            m_Compiler->ReAllocProcesses();
        }
    }
    spn = XRCCTRL(*this, "spnMaxErrors", wxSpinCtrl);
    if (spn)
        Manager::Get()->GetConfigManager(_T("compiler"))->Write(_T("/max_reported_errors"), (int)spn->GetValue());

    m_Compiler->SaveOptions();
    m_Compiler->SetupEnvironment();
    Manager::Get()->GetMacrosManager()->Reset();
}

void CompilerOptionsDlg::OnMyCharHook(wxKeyEvent& event)
{
    wxWindow* focused = wxWindow::FindFocus();
    if(!focused)
        { event.Skip();return; }
    int keycode = event.GetKeyCode();
    int id = focused->GetId();

    int myid = 0;
    unsigned int myidx = 0;

    const wxChar* str_libs[4] = { _T("btnEditLib"),_T("btnAddLib"),_T("btnDelLib"),_T("btnClearLib") };
    const wxChar* str_dirs[4] = { _T("btnEditDir"),_T("btnAddDir"),_T("btnDelDir"),_T("btnClearDir") };
    const wxChar* str_vars[4] = { _T("btnEditVar"),_T("btnAddVar"),_T("btnDeleteVar"),_T("btnClearVar") };
    const wxChar* str_xtra[4] = { _T("btnExtraEdit"),_T("btnExtraAdd"),_T("btnExtraDelete"),_T("btnExtraClear") };

    if(keycode == WXK_RETURN || keycode == WXK_NUMPAD_ENTER)
        { myidx = 0; } // Edit
    else if(keycode == WXK_INSERT || keycode == WXK_NUMPAD_INSERT)
        { myidx = 1; } // Add
    else if(keycode == WXK_DELETE || keycode == WXK_NUMPAD_DELETE)
        { myidx = 2; } // Delete
    else
        { event.Skip();return; }

    if(     id == XRCID("lstLibs")) // Link libraries
        { myid =  wxXmlResource::GetXRCID(str_libs[myidx]); }
    else if(id == XRCID("lstIncludeDirs") || id == XRCID("lstLibDirs") || id == XRCID("lstResDirs")) // Directories
        { myid =  wxXmlResource::GetXRCID(str_dirs[myidx]); }
    else if(id == XRCID("lstVars")) // Custom Vars
        { myid =  wxXmlResource::GetXRCID(str_vars[myidx]); }
    else if(id == XRCID("lstExtraPaths")) // Extra Paths
        { myid =  wxXmlResource::GetXRCID(str_xtra[myidx]); }
    else
        myid = 0;

    // Generate the event
    if(myid == 0)
        event.Skip();
    else
    {
        wxCommandEvent newevent(wxEVT_COMMAND_BUTTON_CLICKED,myid);
        this->ProcessEvent(newevent);
    }
}
