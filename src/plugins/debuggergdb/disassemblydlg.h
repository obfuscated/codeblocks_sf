/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef DISASSEMBLYDLG_H
#define DISASSEMBLYDLG_H

#include <wx/panel.h>

class DebuggerGDB;
class wxScintilla;
struct StackFrame;

class DisassemblyDlg : public wxPanel
{
    public:
        DisassemblyDlg(wxWindow* parent, DebuggerGDB* debugger);
        virtual ~DisassemblyDlg();

        void Clear(const StackFrame& frame);
        void AddAssemblerLine(unsigned long int addr, const wxString& line);
        void SetActiveAddress(unsigned long int addr);
    protected:
        void OnSave(wxCommandEvent& event);
        void OnRefresh(wxCommandEvent& event);

        DebuggerGDB* m_pDbg;
        wxScintilla* m_pCode;
        unsigned long int m_LastActiveAddr;
        bool m_HasActiveAddr;
        wxString m_FrameFunction;
        wxString m_FrameAddress;
        bool m_ClearFlag;
    private:
        DECLARE_EVENT_TABLE();
};

#endif // DISASSEMBLYDLG_H
