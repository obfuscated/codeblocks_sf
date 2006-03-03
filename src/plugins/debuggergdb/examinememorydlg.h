#ifndef EXAMINEMEMORYDLG_H
#define EXAMINEMEMORYDLG_H

#include <wx/panel.h>

class DebuggerGDB;
class wxTextCtrl;

class ExamineMemoryDlg : public wxPanel
{
	public:
		ExamineMemoryDlg(wxWindow* parent, DebuggerGDB* debugger);
		virtual ~ExamineMemoryDlg();

        void Clear();
		void AddHexByte(const wxString& addr, const wxString& hexbyte);
	protected:
        DebuggerGDB* m_pDbg;
        wxTextCtrl* m_pText;
        size_t m_ByteCounter;
        wxChar m_LineText[67]; // 16*3 "7F " + 3 "   " + 16 "."
	private:
};

#endif // EXAMINEMEMORYDLG_H
