/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CLASSWIZARDDLG_H
#define CLASSWIZARDDLG_H

#include <wx/dialog.h>
#include <wx/string.h>

class wxCommandEvent;
class wxUpdateUIEvent;

class ClassWizardDlg : public wxDialog
{
    public:
        ClassWizardDlg(wxWindow* parent);
        ~ClassWizardDlg();
        const wxString& GetHeaderFilename() const { return m_Header; }
        const wxString& GetImplementationFilename() const { return m_Implementation; }
    private:
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnNameChange(wxCommandEvent& event);
        void OnAncestorChange(wxCommandEvent& event);
        void OnCommonDirClick(wxCommandEvent& event);
        void OnLowerCaseClick(wxCommandEvent& event);
        void OnIncludeDirClick(wxCommandEvent& event);
        void OnImplDirClick(wxCommandEvent& event);
        void OnHeaderChange(wxCommandEvent& event);
        void OnOKClick(wxCommandEvent& event);
        void OnCancelClick(wxCommandEvent& event);

        void DoGuardBlock();
        void DoFileNames();

        wxString m_Header;
        wxString m_Implementation;

        DECLARE_EVENT_TABLE()
};

#endif // CLASSWIZARDDLG_H
