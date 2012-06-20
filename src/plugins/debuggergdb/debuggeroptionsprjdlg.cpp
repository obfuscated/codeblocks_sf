/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "debuggeroptionsprjdlg.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <cbproject.h>
#include <editpathdlg.h>
#include <manager.h>
#include <globals.h>

#include "debuggergdb.h"

BEGIN_EVENT_TABLE(DebuggerOptionsProjectDlg, wxPanel)
    EVT_UPDATE_UI(-1, DebuggerOptionsProjectDlg::OnUpdateUI)
    EVT_BUTTON(XRCID("btnAdd"), DebuggerOptionsProjectDlg::OnAdd)
    EVT_BUTTON(XRCID("btnEdit"), DebuggerOptionsProjectDlg::OnEdit)
    EVT_BUTTON(XRCID("btnDelete"), DebuggerOptionsProjectDlg::OnDelete)
    EVT_LISTBOX(XRCID("lstTargets"), DebuggerOptionsProjectDlg::OnTargetSel)
END_EVENT_TABLE()

DebuggerOptionsProjectDlg::DebuggerOptionsProjectDlg(wxWindow* parent, DebuggerGDB* debugger, cbProject* project)
    : m_pDBG(debugger),
    m_pProject(project),
    m_LastTargetSel(-1)
{
    if (!wxXmlResource::Get()->LoadPanel(this, parent, _T("pnlDebuggerProjectOptions")))
        return;

    m_OldPaths = m_pDBG->GetSearchDirs(project);
    m_CurrentRemoteDebugging = m_pDBG->GetRemoteDebuggingMap(project);

    wxListBox* control = XRCCTRL(*this, "lstSearchDirs", wxListBox);
    control->Clear();
    for (size_t i = 0; i < m_OldPaths.GetCount(); ++i)
    {
        control->Append(m_OldPaths[i]);
    }

    control = XRCCTRL(*this, "lstTargets", wxListBox);
    control->Clear();
    control->Append(_("<Project>"));
    for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
    {
        control->Append(project->GetBuildTarget(i)->GetTitle());
    }
    control->SetSelection(-1);

    LoadCurrentRemoteDebuggingRecord();
    Manager::Get()->RegisterEventSink(cbEVT_BUILDTARGET_REMOVED, new cbEventFunctor<DebuggerOptionsProjectDlg, CodeBlocksEvent>(this, &DebuggerOptionsProjectDlg::OnBuildTargetRemoved));
    Manager::Get()->RegisterEventSink(cbEVT_BUILDTARGET_ADDED, new cbEventFunctor<DebuggerOptionsProjectDlg, CodeBlocksEvent>(this, &DebuggerOptionsProjectDlg::OnBuildTargetAdded));
    Manager::Get()->RegisterEventSink(cbEVT_BUILDTARGET_RENAMED, new cbEventFunctor<DebuggerOptionsProjectDlg, CodeBlocksEvent>(this, &DebuggerOptionsProjectDlg::OnBuildTargetRenamed));
}

DebuggerOptionsProjectDlg::~DebuggerOptionsProjectDlg()
{
    Manager::Get()->RemoveAllEventSinksFor(this);
}

void DebuggerOptionsProjectDlg::OnBuildTargetRemoved(CodeBlocksEvent& event)
{
    cbProject* project = event.GetProject();
    if(project != m_pProject)
    {
        return;
    }
    wxString theTarget = event.GetBuildTargetName();
    for (RemoteDebuggingMap::iterator it = m_CurrentRemoteDebugging.begin(); it != m_CurrentRemoteDebugging.end(); ++it)
    {
        // find our target
        if ( !it->first || it->first->GetTitle() != theTarget)
            continue;

        m_CurrentRemoteDebugging.erase(it);
        // if we erased it, just break, there can only be one map per target
        break;
    }
    wxListBox* lstBox = XRCCTRL(*this, "lstTargets", wxListBox);
    int idx = lstBox->FindString(theTarget);
    if (idx > 0)
    {
        lstBox->Delete(idx);
    }
    if((size_t)idx >= lstBox->GetCount())
    {
        idx--;
    }
    lstBox->SetSelection(idx);
    LoadCurrentRemoteDebuggingRecord();
}

void DebuggerOptionsProjectDlg::OnBuildTargetAdded(CodeBlocksEvent& event)
{
    cbProject* project = event.GetProject();
    if(project != m_pProject)
    {
        return;
    }
    wxString newTarget = event.GetBuildTargetName();
    wxString oldTarget = event.GetOldBuildTargetName();
    if(!oldTarget.IsEmpty())
    {
        for (RemoteDebuggingMap::iterator it = m_CurrentRemoteDebugging.begin(); it != m_CurrentRemoteDebugging.end(); ++it)
        {
            // find our target
            if ( !it->first || it->first->GetTitle() != oldTarget)
                continue;
            ProjectBuildTarget* bt = m_pProject->GetBuildTarget(newTarget);
            if(bt)
                m_CurrentRemoteDebugging.insert(m_CurrentRemoteDebugging.end(), std::make_pair(bt, it->second));
            // if we inserted it, just break, there can only be one map per target
            break;
        }
    }
    wxListBox* lstBox = XRCCTRL(*this, "lstTargets", wxListBox);
    int idx = lstBox->FindString(newTarget);
    if (idx == wxNOT_FOUND)
    {
        idx = lstBox->Append(newTarget);
    }
    lstBox->SetSelection(idx);
    LoadCurrentRemoteDebuggingRecord();
}

void DebuggerOptionsProjectDlg::OnBuildTargetRenamed(CodeBlocksEvent& event)
{
    cbProject* project = event.GetProject();
    if(project != m_pProject)
    {
        return;
    }
    wxString newTarget = event.GetBuildTargetName();
    wxString oldTarget = event.GetOldBuildTargetName();
    for (RemoteDebuggingMap::iterator it = m_CurrentRemoteDebugging.begin(); it != m_CurrentRemoteDebugging.end(); ++it)
    {
        // find our target
        if ( !it->first || it->first->GetTitle() != oldTarget)
            continue;
        it->first->SetTitle(newTarget);
        // if we renamed it, just break, there can only be one map per target
        break;
    }

    wxListBox* lstBox = XRCCTRL(*this, "lstTargets", wxListBox);
    int idx = lstBox->FindString(oldTarget);
    if (idx == wxNOT_FOUND)
    {
        return;
    }
    lstBox->SetString(idx, newTarget);
    lstBox->SetSelection(idx);
    LoadCurrentRemoteDebuggingRecord();
}

void DebuggerOptionsProjectDlg::LoadCurrentRemoteDebuggingRecord()
{
    // -1 because entry 0 is "<Project>"
    m_LastTargetSel = XRCCTRL(*this, "lstTargets", wxListBox)->GetSelection() - 1;

    ProjectBuildTarget* bt = m_pProject->GetBuildTarget(m_LastTargetSel);
    if (m_CurrentRemoteDebugging.find(bt) != m_CurrentRemoteDebugging.end())
    {
        RemoteDebugging& rd = m_CurrentRemoteDebugging[bt];
        XRCCTRL(*this, "cmbConnType", wxChoice)->SetSelection((int)rd.connType);
        XRCCTRL(*this, "txtSerial", wxTextCtrl)->SetValue(rd.serialPort);
        XRCCTRL(*this, "cmbBaud", wxChoice)->SetStringSelection(rd.serialBaud);
        XRCCTRL(*this, "txtIP", wxTextCtrl)->SetValue(rd.ip);
        XRCCTRL(*this, "txtPort", wxTextCtrl)->SetValue(rd.ipPort);
        XRCCTRL(*this, "txtCmds", wxTextCtrl)->SetValue(rd.additionalCmds);
        XRCCTRL(*this, "txtCmdsBefore", wxTextCtrl)->SetValue(rd.additionalCmdsBefore);
        XRCCTRL(*this, "chkSkipLDpath", wxCheckBox)->SetValue(rd.skipLDpath);
        XRCCTRL(*this, "chkExtendedRemote", wxCheckBox)->SetValue(rd.extendedRemote);
        XRCCTRL(*this, "txtShellCmdsAfter", wxTextCtrl)->SetValue(rd.additionalShellCmdsAfter);
        XRCCTRL(*this, "txtShellCmdsBefore", wxTextCtrl)->SetValue(rd.additionalShellCmdsBefore);
    }
    else
    {
        XRCCTRL(*this, "cmbConnType", wxChoice)->SetSelection(0);
        XRCCTRL(*this, "txtSerial", wxTextCtrl)->SetValue(wxEmptyString);
        XRCCTRL(*this, "cmbBaud", wxChoice)->SetSelection(0);
        XRCCTRL(*this, "txtIP", wxTextCtrl)->SetValue(wxEmptyString);
        XRCCTRL(*this, "txtPort", wxTextCtrl)->SetValue(wxEmptyString);
        XRCCTRL(*this, "txtCmds", wxTextCtrl)->SetValue(wxEmptyString);
        XRCCTRL(*this, "txtCmdsBefore", wxTextCtrl)->SetValue(wxEmptyString);
        XRCCTRL(*this, "chkSkipLDpath", wxCheckBox)->SetValue(false);
        XRCCTRL(*this, "chkExtendedRemote", wxCheckBox)->SetValue(false);
        XRCCTRL(*this, "txtShellCmdsAfter", wxTextCtrl)->SetValue(wxEmptyString);
        XRCCTRL(*this, "txtShellCmdsBefore", wxTextCtrl)->SetValue(wxEmptyString);
    }
}

void DebuggerOptionsProjectDlg::SaveCurrentRemoteDebuggingRecord()
{
//  if (m_LastTargetSel == -1)
//      return;

    ProjectBuildTarget* bt = m_pProject->GetBuildTarget(m_LastTargetSel);
//  if (!bt)
//      return;

    RemoteDebuggingMap::iterator it = m_CurrentRemoteDebugging.find(bt);
    if (it == m_CurrentRemoteDebugging.end())
        it = m_CurrentRemoteDebugging.insert(m_CurrentRemoteDebugging.end(), std::make_pair(bt, RemoteDebugging()));

    RemoteDebugging& rd = it->second;

    rd.connType = (RemoteDebugging::ConnectionType)XRCCTRL(*this, "cmbConnType", wxChoice)->GetSelection();
    rd.serialPort = XRCCTRL(*this, "txtSerial", wxTextCtrl)->GetValue();
    rd.serialBaud = XRCCTRL(*this, "cmbBaud", wxChoice)->GetStringSelection();
    rd.ip = XRCCTRL(*this, "txtIP", wxTextCtrl)->GetValue();
    rd.ipPort = XRCCTRL(*this, "txtPort", wxTextCtrl)->GetValue();
    rd.additionalCmds = XRCCTRL(*this, "txtCmds", wxTextCtrl)->GetValue();
    rd.additionalCmdsBefore = XRCCTRL(*this, "txtCmdsBefore", wxTextCtrl)->GetValue();
    rd.skipLDpath = XRCCTRL(*this, "chkSkipLDpath", wxCheckBox)->GetValue();
    rd.extendedRemote = XRCCTRL(*this, "chkExtendedRemote", wxCheckBox)->GetValue();
    rd.additionalShellCmdsAfter = XRCCTRL(*this, "txtShellCmdsAfter", wxTextCtrl)->GetValue();
    rd.additionalShellCmdsBefore = XRCCTRL(*this, "txtShellCmdsBefore", wxTextCtrl)->GetValue();
}

void DebuggerOptionsProjectDlg::OnTargetSel(wxCommandEvent& WXUNUSED(event))
{
    // update remote debugging controls
    SaveCurrentRemoteDebuggingRecord();
    LoadCurrentRemoteDebuggingRecord();
}

void DebuggerOptionsProjectDlg::OnAdd(wxCommandEvent& WXUNUSED(event))
{
    wxListBox* control = XRCCTRL(*this, "lstSearchDirs", wxListBox);

    EditPathDlg dlg(this,
            m_pProject ? m_pProject->GetBasePath() : _T(""),
            m_pProject ? m_pProject->GetBasePath() : _T(""),
            _("Add directory"));

    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxString path = dlg.GetPath();
        control->Append(path);
    }
}

void DebuggerOptionsProjectDlg::OnEdit(wxCommandEvent& WXUNUSED(event))
{
    wxListBox* control = XRCCTRL(*this, "lstSearchDirs", wxListBox);
    int sel = control->GetSelection();
    if (sel < 0)
        return;

    EditPathDlg dlg(this,
            control->GetString(sel),
            m_pProject ? m_pProject->GetBasePath() : _T(""),
            _("Edit directory"));

    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxString path = dlg.GetPath();
        control->SetString(sel, path);
    }
}

void DebuggerOptionsProjectDlg::OnDelete(wxCommandEvent& WXUNUSED(event))
{
    wxListBox* control = XRCCTRL(*this, "lstSearchDirs", wxListBox);
    int sel = control->GetSelection();
    if (sel < 0)
        return;

    control->Delete(sel);
}

void DebuggerOptionsProjectDlg::OnUpdateUI(wxUpdateUIEvent& WXUNUSED(event))
{
    wxListBox* control = XRCCTRL(*this, "lstSearchDirs", wxListBox);
    bool en = control->GetSelection() >= 0;

    XRCCTRL(*this, "btnEdit", wxButton)->Enable(en);
    XRCCTRL(*this, "btnDelete", wxButton)->Enable(en);

    en = XRCCTRL(*this, "lstTargets", wxListBox)->GetSelection() != wxNOT_FOUND;

    XRCCTRL(*this, "cmbConnType", wxChoice)->Enable(en);
    XRCCTRL(*this, "txtSerial", wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "cmbBaud", wxChoice)->Enable(en);
    XRCCTRL(*this, "txtIP", wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "txtPort", wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "txtCmds", wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "txtCmdsBefore", wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "chkSkipLDpath", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkExtendedRemote", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "txtShellCmdsAfter", wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "txtShellCmdsBefore", wxTextCtrl)->Enable(en);
}

void DebuggerOptionsProjectDlg::OnApply()
{
    wxListBox* control = XRCCTRL(*this, "lstSearchDirs", wxListBox);

    m_OldPaths.Clear();
    for (int i = 0; i < (int)control->GetCount(); ++i)
    {
        m_OldPaths.Add(control->GetString(i));
    }

    SaveCurrentRemoteDebuggingRecord();

    m_pDBG->GetSearchDirs(m_pProject) = m_OldPaths;
    m_pDBG->GetRemoteDebuggingMap(m_pProject) = m_CurrentRemoteDebugging;
}
