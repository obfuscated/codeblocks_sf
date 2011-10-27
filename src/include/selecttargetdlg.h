/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SELECTTARGETDLG_H
#define SELECTTARGETDLG_H

#include "scrollingdialog.h"

class cbProject;
class ProjectBuildTarget;

class SelectTargetDlg : public wxScrollingDialog
{
    public:
        SelectTargetDlg(wxWindow* parent, cbProject* project, int selected = 0);
        ~SelectTargetDlg();

        void EndModal(int retCode);
        int GetSelection() const { return m_Selected; }
        ProjectBuildTarget* GetSelectionTarget();
    private:
        void OnListboxSelection(wxCommandEvent& event);
        void OnListboxDClick(wxCommandEvent& event);
        void OnCheckboxSelection(wxCommandEvent& event);
        void OnHostApplicationButtonClick(wxCommandEvent& event);
        void UpdateSelected();
        cbProject* m_pProject;
        int m_Selected;

        DECLARE_EVENT_TABLE()
};

#endif // SELECTTARGETDLG_H

