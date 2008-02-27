/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"
#ifndef CB_PRECOMP
    #include <wx/button.h>
    #include <wx/checkbox.h>
    #include <wx/filename.h>
    #include <wx/intl.h>
    #include <wx/listbox.h>
    #include <wx/textctrl.h>
    #include <wx/xrc/xmlres.h>

    #include "projectbuildtarget.h"
    #include "cbproject.h"
#endif
#include <wx/filedlg.h>
#include "selecttargetdlg.h"

BEGIN_EVENT_TABLE(SelectTargetDlg, wxDialog)
	EVT_CHECKBOX(XRCID("chkSetAsDefaultExec"), SelectTargetDlg::OnCheckboxSelection)
	EVT_LISTBOX(XRCID("lstItems"), SelectTargetDlg::OnListboxSelection)
	EVT_LISTBOX_DCLICK(XRCID("lstItems"), SelectTargetDlg::OnListboxDClick)
	EVT_BUTTON(XRCID("btnHostApplication"),	SelectTargetDlg::OnHostApplicationButtonClick)
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
	if (selected != -1)
		list->SetSelection(selected);
	else
		list->SetSelection(list->FindString(m_pProject->GetDefaultExecuteTarget()));

	UpdateSelected();
	XRCCTRL(*this, "wxID_OK", wxButton)->MoveBeforeInTabOrder (XRCCTRL(*this, "lstItems", wxListBox));
}

SelectTargetDlg::~SelectTargetDlg()
{
	//dtor
}

void SelectTargetDlg::UpdateSelected()
{
    wxString name = XRCCTRL(*this, "lstItems", wxListBox)->GetStringSelection();
	ProjectBuildTarget* target = m_pProject->GetBuildTarget(name);
	if (target)
	{
        XRCCTRL(*this, "chkSetAsDefaultExec", wxCheckBox)->SetValue(name == m_pProject->GetDefaultExecuteTarget());
		XRCCTRL(*this, "txtParams", wxTextCtrl)->SetValue(target->GetExecutionParameters());
		XRCCTRL(*this, "txtHostApp", wxTextCtrl)->SetValue(target->GetHostApplication());
	}
	XRCCTRL(*this, "wxID_OK", wxButton)->Enable(target);
} // end of UpdateSelected

ProjectBuildTarget* SelectTargetDlg::GetSelectionTarget()
{
    return m_pProject->GetBuildTarget(m_Selected);
}

// events

void SelectTargetDlg::OnListboxSelection(wxCommandEvent& /*event*/)
{
	UpdateSelected();
} // end of OnListboxSelection

void SelectTargetDlg::OnListboxDClick(wxCommandEvent& /*event*/)
{
    UpdateSelected();
    EndModal(wxID_OK);
} // end of OnListboxDClick

void SelectTargetDlg::OnCheckboxSelection(wxCommandEvent& /*event*/)
{
    if (XRCCTRL(*this, "chkSetAsDefaultExec", wxCheckBox)->GetValue())
    {
        wxString name = XRCCTRL(*this, "lstItems", wxListBox)->GetStringSelection();
        m_pProject->SetDefaultExecuteTarget(name);
    }
} // end of OnCheckboxSelection

void SelectTargetDlg::OnHostApplicationButtonClick(wxCommandEvent& /*event*/)
{
    if(wxTextCtrl* obj = XRCCTRL(*this, "txtHostApp", wxTextCtrl))
    {
        wxFileDialog* dlg = new wxFileDialog(this,
                            _("Select host application"),
                            _T(""),
                            obj->GetValue(),
                            #ifdef __WXMSW__
                            _("Executable files (*.exe)|*.exe"),
                            #else
                            _("All files (*)|*"),
                            #endif
                            wxOPEN | wxFILE_MUST_EXIST | compatibility::wxHideReadonly);
        dlg->SetFilterIndex(0);
        PlaceWindow(dlg);
        if (dlg->ShowModal() != wxID_OK)
            return;
        wxFileName fname(dlg->GetPath());
        obj->SetValue(fname.GetFullName());
    }
} // end of OnHostApplicationButtonClick

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
