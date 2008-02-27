/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef EDITBREAKPOINT_H
#define EDITBREAKPOINT_H

#include <wx/dialog.h>
#include "debugger_defs.h"

class EditBreakpointDlg : public wxDialog
{
    public:
        EditBreakpointDlg(DebuggerBreakpoint* bp, wxWindow* parent = 0);
        virtual ~EditBreakpointDlg();
    protected:
        void OnUpdateUI(wxUpdateUIEvent& event);
        void EndModal(int retCode);

        DebuggerBreakpoint* m_Bp;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // EDITBREAKPOINT_H
