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

#include "projectfileoptionsdlg.h"
#include "cbproject.h"
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/textctrl.h>
#include <wx/button.h>

BEGIN_EVENT_TABLE(ProjectFileOptionsDlg, wxDialog)
	EVT_UPDATE_UI(-1, ProjectFileOptionsDlg::OnUpdateUI)
	EVT_BUTTON(XRCID("btnOK"), ProjectFileOptionsDlg::OnOKClick)
END_EVENT_TABLE()

ProjectFileOptionsDlg::ProjectFileOptionsDlg(wxWindow* parent, ProjectFile* pf)
	: m_ProjectFile(pf)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgProjectFileOptions"));
	
	if (pf)
	{
		int idx = -1;
		cbProject* prj = pf->project;
		for (int i = 0; i < prj->GetBuildTargetsCount(); ++i)
		{
			wxString targetName = prj->GetBuildTarget(i)->GetTitle();
			XRCCTRL(*this, "cmbTarget", wxComboBox)->Append(targetName);
			if (idx == -1 && pf->buildTargets.Index(targetName) != -1)
				idx = i;
		}

		XRCCTRL(*this, "txtCompiler", wxTextCtrl)->SetValue(pf->compilerVar);
		XRCCTRL(*this, "cmbTarget", wxComboBox)->SetSelection(idx);
		XRCCTRL(*this, "chkCompile", wxCheckBox)->SetValue(pf->compile);
		XRCCTRL(*this, "chkLink", wxCheckBox)->SetValue(pf->link);
		XRCCTRL(*this, "sliderWeight", wxSlider)->SetValue(pf->weight);
		XRCCTRL(*this, "txtObjName", wxTextCtrl)->SetValue(pf->GetObjName());
		XRCCTRL(*this, "chkBuildStage", wxCheckBox)->SetValue(pf->useCustomBuildCommand);
		XRCCTRL(*this, "txtBuildStage", wxTextCtrl)->SetValue(pf->buildCommand);
		XRCCTRL(*this, "chkCustomDeps", wxCheckBox)->SetValue(!pf->autoDeps);
		XRCCTRL(*this, "txtCustomDeps", wxTextCtrl)->SetValue(pf->customDeps);
	}
	// FIXME: support more than one target per file
}

ProjectFileOptionsDlg::~ProjectFileOptionsDlg()
{
}

void ProjectFileOptionsDlg::OnUpdateUI(wxCommandEvent& event)
{
	if (m_ProjectFile)
	{
		bool en = XRCCTRL(*this, "chkBuildStage", wxCheckBox)->GetValue();
		XRCCTRL(*this, "txtBuildStage", wxTextCtrl)->Enable(en);
		en = XRCCTRL(*this, "chkCustomDeps", wxCheckBox)->GetValue();
		XRCCTRL(*this, "txtCustomDeps", wxTextCtrl)->Enable(en);
	}
	else
	{
		XRCCTRL(*this, "txtCompiler", wxTextCtrl)->Enable(false);
		XRCCTRL(*this, "cmbTarget", wxComboBox)->Enable(false);
		XRCCTRL(*this, "chkCompile", wxCheckBox)->Enable(false);
		XRCCTRL(*this, "chkLink", wxCheckBox)->Enable(false);
		XRCCTRL(*this, "txtObjName", wxTextCtrl)->Enable(false);;
		XRCCTRL(*this, "chkBuildStage", wxCheckBox)->Enable(false);
		XRCCTRL(*this, "txtBuildStage", wxTextCtrl)->Enable(false);
		XRCCTRL(*this, "chkCustomDeps", wxCheckBox)->Enable(false);
		XRCCTRL(*this, "txtCustomDeps", wxTextCtrl)->Enable(false);
	}
}

void ProjectFileOptionsDlg::OnOKClick(wxCommandEvent& event)
{
	m_ProjectFile->buildTargets.Clear();
	m_ProjectFile->AddBuildTarget(XRCCTRL(*this, "cmbTarget", wxComboBox)->GetValue());
	m_ProjectFile->compile = XRCCTRL(*this, "chkCompile", wxCheckBox)->GetValue();
	m_ProjectFile->link = XRCCTRL(*this, "chkLink", wxCheckBox)->GetValue();
	m_ProjectFile->weight = XRCCTRL(*this, "sliderWeight", wxSlider)->GetValue();
	m_ProjectFile->SetObjName(XRCCTRL(*this, "txtObjName", wxTextCtrl)->GetValue());
	m_ProjectFile->useCustomBuildCommand = XRCCTRL(*this, "chkBuildStage", wxCheckBox)->GetValue();
	m_ProjectFile->buildCommand = XRCCTRL(*this, "txtBuildStage", wxTextCtrl)->GetValue();
	m_ProjectFile->autoDeps = !XRCCTRL(*this, "chkCustomDeps", wxCheckBox)->GetValue();
	m_ProjectFile->customDeps = XRCCTRL(*this, "txtCustomDeps", wxTextCtrl)->GetValue();
	m_ProjectFile->compilerVar = XRCCTRL(*this, "txtCompiler", wxTextCtrl)->GetValue();

    cbProject* prj = m_ProjectFile->project;
    prj->SetModified(true);
	EndModal(wxID_OK);
}
