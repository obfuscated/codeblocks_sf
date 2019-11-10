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

/// Store target UI data for every target in the list box control in the SelectTargetDlg.
/// This makes it possible to edit all targets in one go. It also make implementing the
/// Cancel button possible.
class TargetListData : public wxClientData
{
    public:
        TargetListData(const wxString& ExecParameters, const wxString& HostApp, bool RunHostAppInTerminal,
                       bool DefaultExecTarget) :
            m_HostApp(HostApp),
            m_ExecParameters(ExecParameters),
            m_RunHostAppInTerminal(RunHostAppInTerminal),
            m_DefaultExecTarget(DefaultExecTarget)
        {
        }

        void SetExecParameters(const wxString& ExecParameters)
        {
            m_ExecParameters = ExecParameters;
            // Replace all '\n' by ' ' in the program-parameters .
            // This is necessary because a multiline text control may add
            // '\n' to the text but these characters must not be part of the
            // parameters when executing the program.
            m_ExecParameters.Replace(_T("\n"), _T(" "));
        }
        void SetHostApp(const wxString &hostApp) { m_HostApp = hostApp; }
        void SetRunHostAppInTerminal(bool runAppInTerminal) { m_RunHostAppInTerminal = runAppInTerminal; }
        void SetDefaultExecTarget(bool defaultTarget) { m_DefaultExecTarget = defaultTarget; }

        const wxString& GetHostApp() const { return m_HostApp; }
        const wxString& GetExecParameters() const { return m_ExecParameters; }
        bool GetRunHostAppInTerminal() const { return m_RunHostAppInTerminal; }
        bool GetDefaultExecTarget() const { return m_DefaultExecTarget; }

    private:
        wxString m_HostApp;
        wxString m_ExecParameters;
        bool m_RunHostAppInTerminal;
        bool m_DefaultExecTarget;
};

BEGIN_EVENT_TABLE(SelectTargetDlg, wxScrollingDialog)
    EVT_LISTBOX(XRCID("lstItems"), SelectTargetDlg::OnListboxSelection)
    EVT_LISTBOX_DCLICK(XRCID("lstItems"), SelectTargetDlg::OnListboxDClick)
    EVT_BUTTON(XRCID("btnHostApplication"),    SelectTargetDlg::OnHostApplicationButtonClick)
END_EVENT_TABLE()

SelectTargetDlg::SelectTargetDlg(wxWindow* parent, cbProject* project, int selected) :
    m_pProject(project),
    m_Selected(selected),
    m_LastSelected(wxNOT_FOUND)
{
    //ctor
    cbAssert(m_pProject);

    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgSelectTarget"), _T("wxScrollingDialog"));

    m_List = XRCCTRL(*this, "lstItems", wxListBox);
    m_List->Clear();
    for (int i = 0; i < m_pProject->GetBuildTargetsCount(); ++i)
    {
        ProjectBuildTarget* target = m_pProject->GetBuildTarget(i);
        auto targetData = new TargetListData(target->GetExecutionParameters(), target->GetHostApplication(),
                                             target->GetRunHostApplicationInTerminal(),
                                             (m_pProject->GetDefaultExecuteTarget() == target->GetTitle()));
        m_List->Append(target->GetTitle(), targetData);
    }
    if (selected != -1)
        m_List->SetSelection(selected);
    else
    {
        int item = m_List->FindString(m_pProject->GetActiveBuildTarget());
        if (item == wxNOT_FOUND)
            item = m_List->FindString(m_pProject->GetDefaultExecuteTarget());
        m_List->SetSelection(item);
    }

    UpdateSelected();
    wxButton *okButton = XRCCTRL(*this, "wxID_OK", wxButton);
    okButton->MoveBeforeInTabOrder (XRCCTRL(*this, "lstItems", wxListBox));
    okButton->SetDefault();
}

SelectTargetDlg::~SelectTargetDlg()
{
    //dtor
}

void SelectTargetDlg::UpdateSelected()
{
    wxCheckBox* chkSetAsDefaultExec = XRCCTRL(*this, "chkSetAsDefaultExec", wxCheckBox);
    wxTextCtrl* txtParams = XRCCTRL(*this, "txtParams", wxTextCtrl);
    wxTextCtrl* txtHostApp = XRCCTRL(*this, "txtHostApp", wxTextCtrl);
    wxCheckBox* chkHostInTerminal = XRCCTRL(*this, "chkHostInTerminal", wxCheckBox);

    if (m_LastSelected != wxNOT_FOUND)
    {
        auto lastData = static_cast<TargetListData*>(m_List->GetClientObject(m_LastSelected));
        lastData->SetExecParameters(txtParams->GetValue());
        lastData->SetHostApp(txtHostApp->GetValue());
        lastData->SetRunHostAppInTerminal(chkHostInTerminal->GetValue());

        if (chkSetAsDefaultExec->GetValue())
        {
            for (size_t i = 0; i < m_List->GetCount(); i++)
            {
                auto data = static_cast<TargetListData*>(m_List->GetClientObject(i));
                data->SetDefaultExecTarget(false);
            }
        }
        lastData->SetDefaultExecTarget(chkSetAsDefaultExec->GetValue());
    }

    if (m_List->GetSelection() != wxNOT_FOUND)
    {
        auto selectedData = static_cast<TargetListData*>(m_List->GetClientObject(m_List->GetSelection()));
        chkSetAsDefaultExec->SetValue(selectedData->GetDefaultExecTarget());
        txtParams->SetValue(selectedData->GetExecParameters());
        txtHostApp->SetValue(selectedData->GetHostApp());
        chkHostInTerminal->SetValue(selectedData->GetRunHostAppInTerminal());
    }
    m_LastSelected = m_List->GetSelection();
} // end of UpdateSelected

ProjectBuildTarget* SelectTargetDlg::GetSelectionTarget()
{
    return m_pProject->GetBuildTarget(m_Selected);
}

// events

void SelectTargetDlg::OnListboxSelection(cb_unused wxCommandEvent& event)
{
    UpdateSelected();
} // end of OnListboxSelection

void SelectTargetDlg::OnListboxDClick(cb_unused wxCommandEvent& event)
{
    EndModal(wxID_OK);
} // end of OnListboxDClick

void SelectTargetDlg::OnHostApplicationButtonClick(cb_unused wxCommandEvent& event)
{
    if(wxTextCtrl* obj = XRCCTRL(*this, "txtHostApp", wxTextCtrl))
    {
        wxFileDialog dlg(this,
                         _("Select host application"),
                         _T(""),
                         obj->GetValue(),
#ifdef __WXMSW__
                         _("Executable files (*.exe)|*.exe"),
#else
                         _("All files (*)|*"),
#endif
                         wxFD_OPEN | wxFD_FILE_MUST_EXIST | compatibility::wxHideReadonly);
        dlg.SetFilterIndex(0);
        PlaceWindow(&dlg);
        if (dlg.ShowModal() != wxID_OK)
            return;
        wxFileName fname(dlg.GetPath());
        if (fname.GetFullPath().StartsWith(m_pProject->GetCommonTopLevelPath()))
        {
            fname.MakeRelativeTo(m_pProject->GetCommonTopLevelPath());
            obj->SetValue(fname.GetFullPath());
        }
        else
            obj->SetValue(fname.GetFullPath());
    }
} // end of OnHostApplicationButtonClick

void SelectTargetDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        UpdateSelected();
        m_Selected = m_List->GetSelection();
        wxString defaultExecTarget;

        for (size_t i = 0; i < m_List->GetCount(); i++)
        {
            auto data = static_cast<TargetListData*>(m_List->GetClientObject(i));
            ProjectBuildTarget *target = m_pProject->GetBuildTarget(i);

            target->SetExecutionParameters(data->GetExecParameters());
            target->SetHostApplication(data->GetHostApp());
            target->SetRunHostApplicationInTerminal(data->GetRunHostAppInTerminal());

            if (data->GetDefaultExecTarget())
                defaultExecTarget = m_List->GetString(i);
        }

        if (defaultExecTarget != m_pProject->GetDefaultExecuteTarget())
            m_pProject->SetDefaultExecuteTarget(defaultExecTarget);
    }
    wxScrollingDialog::EndModal(retCode);
} // end of EndModal
