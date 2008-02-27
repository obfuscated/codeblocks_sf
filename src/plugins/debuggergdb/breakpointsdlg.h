/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef BREAKPOINTSDLG_H
#define BREAKPOINTSDLG_H

#include <wx/panel.h>
#include "debugger_defs.h"

class wxCommandEvent;
class wxListCtrl;
class wxListEvent;
class BreakpointsList;
class DebuggerState;

class BreakpointsDlg : public wxPanel
{
    public:
        BreakpointsDlg(DebuggerState& state);
        virtual ~BreakpointsDlg();

        void Refresh();
    protected:
        void FillBreakpoints();
        void RemoveBreakpoint(int sel);

        void OnRemove(wxCommandEvent& event);
        void OnRemoveAll(wxCommandEvent& event);
        void OnProperties(wxCommandEvent& event);
        void OnOpen(wxCommandEvent& event);
        void OnRightClick(wxListEvent& event);
        void OnDoubleClick(wxListEvent& event);

        DebuggerState& m_State;
        BreakpointsList& m_BreakpointsList;
        wxListCtrl* m_pList;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // BREAKPOINTSDLG_H
