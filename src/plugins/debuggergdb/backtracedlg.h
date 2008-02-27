/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef BACKTRACEDLG_H
#define BACKTRACEDLG_H

#include <wx/panel.h>
#include <wx/listctrl.h>

class DebuggerGDB;
struct StackFrame;

class BacktraceDlg : public wxPanel
{
    public:
        BacktraceDlg(wxWindow* parent, DebuggerGDB* debugger);
        virtual ~BacktraceDlg();

        void Clear();
        void AddFrame(const StackFrame& frame);
    protected:
        void OnListRightClick(wxListEvent& event);
        void OnJump(wxCommandEvent& event);
        void OnDblClick(wxListEvent& event);
        void OnSave(wxCommandEvent& event);
        void OnSwitchFrame(wxCommandEvent& event);

        DebuggerGDB* m_pDbg;
    private:
        DECLARE_EVENT_TABLE();
};

#endif // BACKTRACEDLG_H
