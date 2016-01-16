/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef PROJECTFILEOPTIONSDLG_H
#define PROJECTFILEOPTIONSDLG_H

#include "scrollingdialog.h"
#include "settings.h"

class ProjectFile;

class DLLIMPORT ProjectFileOptionsDlg : public wxScrollingDialog
{
    public:
        ProjectFileOptionsDlg(wxWindow* parent, ProjectFile* pf);
        ProjectFileOptionsDlg(wxWindow* parent, const wxString& fileName);
        ~ProjectFileOptionsDlg();

        void OnReadOnlyCheck(wxCommandEvent& event);
        void OnCompilerCombo(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
        void EndModal(int retCode);

    private:
        void FillGeneralProperties();
        void FillCompilers();
        void UpdateBuildCommand();
        void SaveBuildCommandSelection();
        bool ToggleFileReadOnly(bool setReadOnly);

        ProjectFile* m_ProjectFile;
        wxString     m_FileNameStr;
        wxFileName   m_FileName;
        int          m_LastBuildStageCompilerSel;

        DECLARE_EVENT_TABLE()
};

#endif // PROJECTFILEOPTIONSDLG_H
