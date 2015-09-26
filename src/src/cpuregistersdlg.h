/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CPUREGISTERSDLG_H
#define CPUREGISTERSDLG_H

#include <wx/panel.h>
#include <cbdebugger_interfaces.h>

class wxListCtrl;

class CPURegistersDlg : public wxPanel, public cbCPURegistersDlg
{
    public:
        CPURegistersDlg(wxWindow* parent);

        wxWindow* GetWindow() { return this; }

        void Clear();
        void SetRegisterValue(const wxString& reg_name, const wxString& hexValue, const wxString& interpreted);
        void EnableWindow(bool enable);
    protected:
        int RegisterIndex(const wxString& reg_name);
        void OnRefresh(wxCommandEvent& event);

    private:
        DECLARE_EVENT_TABLE();
    private:
        wxListCtrl* m_pList;
};

#endif // CPUREGISTERSDLG_H
