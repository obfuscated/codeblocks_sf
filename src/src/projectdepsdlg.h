/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef PROJECTDEPSDLG_H
#define PROJECTDEPSDLG_H

#include "scrollingdialog.h"

class cbProject;

class ProjectDepsDlg : public wxScrollingDialog
{
    public:
        ProjectDepsDlg(wxWindow* parent, cbProject* sel = nullptr);
        ~ProjectDepsDlg() override;
        void EndModal(int retCode) override;
    protected:
        void FillList();
        bool SaveList();
        void OnProjectChange(wxCommandEvent& event);
        int m_LastSel;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // PROJECTDEPSDLG_H
