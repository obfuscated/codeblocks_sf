/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef THREADSDLG_H
#define THREADSDLG_H

#include <wx/panel.h>
#include <wx/listctrl.h>

class DebuggerGDB;

class ThreadsDlg : public wxPanel
{
    public:
        ThreadsDlg(wxWindow* parent, DebuggerGDB* debugger);
        virtual ~ThreadsDlg();

        void Clear();
        void AddThread(const wxString& active_mark, const wxString& thread_num, const wxString& thread_info);
    protected:
        void OnListRightClick(wxListEvent& event);
        void OnSwitchThread(wxCommandEvent& event);

        DebuggerGDB* m_pDbg;
    private:
        DECLARE_EVENT_TABLE();
};

#endif // THREADSDLG_H
