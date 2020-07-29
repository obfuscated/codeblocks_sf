/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef DEBUGGEROPTIONSPRJDLG_H
#define DEBUGGEROPTIONSPRJDLG_H

#include <wx/intl.h>
#include "configurationpanel.h"
#include <settings.h>

#include "remotedebugging.h"

class cbProject;
class wxListBox;
class DebuggerGDB;
class CodeBlocksEvent;

class DebuggerOptionsProjectDlg : public cbConfigurationPanel
{
    public:
        DebuggerOptionsProjectDlg(wxWindow* parent, DebuggerGDB* debugger, cbProject* project);
        ~DebuggerOptionsProjectDlg() override;

        wxString GetTitle() const override { return _("Debugger"); }
        wxString GetBitmapBaseName() const override { return _T("debugger"); }
        void OnApply() override;
        void OnCancel() override {}
    protected:
        void OnTargetSel(wxCommandEvent& event);
        void OnAdd(wxCommandEvent& event);
        void OnEdit(wxCommandEvent& event);
        void OnDelete(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
    private:
        void OnBuildTargetRemoved(CodeBlocksEvent& event);
        void OnBuildTargetAdded(CodeBlocksEvent& event);
        void OnBuildTargetRenamed(CodeBlocksEvent& event);
        void LoadCurrentRemoteDebuggingRecord();
        void SaveCurrentRemoteDebuggingRecord();

        DebuggerGDB* m_pDBG;
        cbProject* m_pProject;
        wxArrayString m_OldPaths;
        RemoteDebuggingMap m_OldRemoteDebugging;
        RemoteDebuggingMap m_CurrentRemoteDebugging;
        int m_LastTargetSel;
        DECLARE_EVENT_TABLE()
};

#endif // DEBUGGEROPTIONSPRJDLG_H
