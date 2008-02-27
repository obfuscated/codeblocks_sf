/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CPUREGISTERSDLG_H
#define CPUREGISTERSDLG_H

#include <wx/panel.h>

class DebuggerGDB;
class wxListCtrl;

class CPURegistersDlg : public wxPanel
{
    public:
        CPURegistersDlg(wxWindow* parent, DebuggerGDB* debugger);
        virtual ~CPURegistersDlg();

        void Clear();
        void SetRegisterValue(const wxString& reg_name, unsigned long int value);
    protected:
        int RegisterIndex(const wxString& reg_name);
        void OnRefresh(wxCommandEvent& event);

        DebuggerGDB* m_pDbg;
        wxListCtrl* m_pList;
    private:
        DECLARE_EVENT_TABLE();
};

#endif // CPUREGISTERSDLG_H
