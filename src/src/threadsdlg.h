/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef THREADSDLG_H
#define THREADSDLG_H

#include <wx/panel.h>
#include <cbdebugger_interfaces.h>

class wxCommandEvent;
class wxListCtrl;
class wxListEvent;

class ThreadsDlg : public wxPanel, public cbThreadsDlg
{
    public:
        ThreadsDlg(wxWindow* parent);

        wxWindow* GetWindow() { return this; }

        void Reload();
        void EnableWindow(bool enable);
    protected:
        void OnListRightClick(wxListEvent& event);
        void OnListDoubleClick(wxListEvent& event);
        void OnSwitchThread(wxCommandEvent& event);

    private:
        DECLARE_EVENT_TABLE();
    private:
        wxListCtrl *m_list;
};

#endif // THREADSDLG_H
