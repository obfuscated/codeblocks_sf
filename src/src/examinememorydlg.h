/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 * Modified MicroSourceCode https://github.com/MicroSourceCode
 */

#ifndef EXAMINEMEMORYDLG_H
#define EXAMINEMEMORYDLG_H

#include <wx/panel.h>
#include <cbdebugger_interfaces.h>

class wxTextCtrl;

class ExamineMemoryDlg : public wxPanel, public cbExamineMemoryDlg
{
    public:
        ExamineMemoryDlg(wxWindow* parent);

        wxWindow* GetWindow() { return this; }

        // used for Freeze()/Thaw() calls
        void Begin();
        void End();

        void Clear();
        wxString GetBaseAddress();
        void SetBaseAddress(const wxString &addr);
        int GetBytes();
        int GetCols();
        void AddError(const wxString& err);
        void AddHexByte(const wxString& addr, const wxString& hexbyte);
        void EnableWindow(bool enable);
    protected:
        void OnGo(wxCommandEvent& event);

    private:
        wxTextCtrl* m_pText;
        size_t m_ByteCounter;
        unsigned m_ColumnsCtrl;
        unsigned m_PartLength;
        wxChar m_LineText[128];
        uint64_t m_LastRowStartingAddress;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // EXAMINEMEMORYDLG_H
