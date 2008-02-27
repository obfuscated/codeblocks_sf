/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef PROJECTDEPSDLG_H
#define PROJECTDEPSDLG_H

#include <wx/dialog.h>

class cbProject;

class ProjectDepsDlg : public wxDialog
{
    public:
        ProjectDepsDlg(wxWindow* parent, cbProject* sel = 0);
        virtual ~ProjectDepsDlg();
        virtual void EndModal(int retCode);
    protected:
        void FillList();
        bool SaveList();
        void OnProjectChange(wxCommandEvent& event);
        int m_LastSel;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // PROJECTDEPSDLG_H
