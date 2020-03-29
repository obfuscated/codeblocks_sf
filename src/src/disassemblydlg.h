/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef DISASSEMBLYDLG_H
#define DISASSEMBLYDLG_H

#include <vector>

#include <wx/panel.h>
#include <wx/checkbox.h>
#include <cbdebugger_interfaces.h>

class wxScintilla;
class cbStackFrame;

class DisassemblyDlg : public wxPanel, public cbDisassemblyDlg
{
    public:
        DisassemblyDlg(wxWindow* parent);

        wxWindow* GetWindow() { return this; }

        void Clear(const cbStackFrame& frame);
        void AddAssemblerLine(uint64_t addr, const wxString& line);
        void AddSourceLine(int lineno, const wxString& line);
        bool SetActiveAddress(uint64_t addr);
        void CenterLine(int lineno);
        void CenterCurrentLine();
        bool HasActiveAddr() { return m_HasActiveAddr; }
        void EnableWindow(bool enable);

    protected:
        void OnSave(wxCommandEvent& event);
        void OnRefresh(wxCommandEvent& event);
        void OnMixedModeCB(wxCommandEvent &event);
        void OnAdjustLine(wxCommandEvent &event);

        wxScintilla* m_pCode;
        uint64_t m_LastActiveAddr;
        bool m_HasActiveAddr;
        wxString m_FrameFunction;
        wxString m_FrameAddress;
        bool m_ClearFlag;
        std::vector<char> m_LineTypes; //'S'ource, 'D'isassembly

    private:
        wxCheckBox *m_MixedModeCB;
        DECLARE_EVENT_TABLE();
};

#endif // DISASSEMBLYDLG_H
