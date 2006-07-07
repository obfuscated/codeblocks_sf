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

#include "sdk_precomp.h"


#ifndef CB_PRECOMP
    #include <wx/xrc/xmlres.h>
    #include <wx/intl.h>
    #include <wx/checkbox.h>
    #include <wx/button.h>
    #include <wx/textctrl.h>
    #include <wx/listbox.h>

    #include "projectbuildtarget.h"
    #include "cbproject.h"
#endif

#include "selecttargetdlg.h"

BEGIN_EVENT_TABLE(SelectTargetDlg, wxDialog)
	EVT_CHECKBOX(XRCID("chkSetAsDefaultExec"), SelectTargetDlg::OnCheckboxSelection)
	EVT_LISTBOX(XRCID("lstItems"), SelectTargetDlg::OnListboxSelection)
	EVT_LISTBOX_DCLICK(XRCID("lstItems"), SelectTargetDlg::OnOK)
END_EVENT_TABLE()

SelectTargetDlg::SelectTargetDlg(wxWindow* parent, cbProject* project, int selected)
	: m_pProject(project),
	m_Selected(selected)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgSelectTarget"));

	wxListBox* list = XRCCTRL(*this, "lstItems", wxListBox);
	list->Clear();
	for (int i = 0; i < m_pProject->GetBuildTargetsCount(); ++i)
	{
		ProjectBuildTarget* target = m_pProject->GetBuildTarget(i);
		list->Append(target->GetTitle());
	}
	list->SetSelection(m_pProject->GetDefaultExecuteTargetIndex());

	UpdateSelected();
	XRCCTRL(*this, "wxID_OK", wxButton)->MoveBeforeInTabOrder (XRCCTRL(*this, "lstItems", wxListBox));
}

SelectTargetDlg::~SelectTargetDlg()
{
	//dtor
}

void SelectTargetDlg::UpdateSelected()
{
    int idx = XRCCTRL(*this, "lstItems", wxListBox)->GetSelection();
	ProjectBuildTarget* target = m_pProject->GetBuildTarget(idx);
	if (target)
	{
        XRCCTRL(*this, "chkSetAsDefaultExec", wxCheckBox)->SetValue(idx == m_pProject->GetDefaultExecuteTargetIndex());
		XRCCTRL(*this, "txtParams", wxTextCtrl)->SetValue(target->GetExecutionParameters());
		XRCCTRL(*this, "txtHostApp", wxTextCtrl)->SetValue(target->GetHostApplication());
#if 0
		bool en = target->GetTargetType() == ttExecutable ||
				target->GetTargetType() == ttConsoleOnly ||
				((target->GetTargetType() == ttDynamicLib ||
					target->GetTargetType() == ttStaticLib) &&
					!target->GetHostApplication().IsEmpty());
		XRCCTRL(*this, "btnOK", wxButton)->Enable(en);
#endif
	}
#if 0
	else
		XRCCTRL(*this, "btnOK", wxButton)->Enable(false);
#endif
}

ProjectBuildTarget* SelectTargetDlg::GetSelectionTarget()
{
    return m_pProject->GetBuildTarget(m_Selected);
}

// events

void SelectTargetDlg::OnListboxSelection(wxCommandEvent& event)
{
	UpdateSelected();
}

void SelectTargetDlg::OnCheckboxSelection(wxCommandEvent& event)
{
    if (XRCCTRL(*this, "chkSetAsDefaultExec", wxCheckBox)->GetValue())
    {
        int idx = XRCCTRL(*this, "lstItems", wxListBox)->GetSelection();
        m_pProject->SetDefaultExecuteTargetIndex(idx);
    }
}

void SelectTargetDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        m_Selected = XRCCTRL(*this, "lstItems", wxListBox)->GetSelection();
        ProjectBuildTarget* target = m_pProject->GetBuildTarget(m_Selected);
        if (target)
        {
            // Search all '\n' in the program-parameters and replace them by
            // ' '. This is necessary because a multiline text control may add
            // '\n' to the text but these characters must not be part of the
            // parameters when executing the program.
            wxString execution_parameters = XRCCTRL(*this, "txtParams", wxTextCtrl)->GetValue();
            wxString::size_type pos = 0;

            while ((pos = execution_parameters.find('\n', pos)) != wxString::npos)
            {
                execution_parameters[pos] = ' ';
            }
            target->SetExecutionParameters(execution_parameters);
            target->SetHostApplication(XRCCTRL(*this, "txtHostApp", wxTextCtrl)->GetValue());
        }
    }
	wxDialog::EndModal(retCode);
} // end of EndModal
