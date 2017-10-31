/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef EDITBREAKPOINT_H
#define EDITBREAKPOINT_H

#include "scrollingdialog.h"
#include "debugger_defs.h"

class EditBreakpointDlg : public wxScrollingDialog
{
    public:
        EditBreakpointDlg(const DebuggerBreakpoint &breakpoint, wxWindow* parent = 0);
        virtual ~EditBreakpointDlg();

        const DebuggerBreakpoint& GetBreakpoint() const { return m_breakpoint; }
    protected:
        void OnUpdateUI(wxUpdateUIEvent& event);
        void EndModal(int retCode);

        DebuggerBreakpoint m_breakpoint;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // EDITBREAKPOINT_H
