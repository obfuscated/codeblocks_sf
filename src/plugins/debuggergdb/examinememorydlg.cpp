#include <sdk.h>
#include "examinememorydlg.h"
#include "debuggergdb.h"
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>

ExamineMemoryDlg::ExamineMemoryDlg(wxWindow* parent, DebuggerGDB* debugger)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
    m_pDbg(debugger)
{
	//ctor
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	m_pText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);
	sizer->Add(m_pText, 1, wxGROW);
	SetSizer(sizer);
	Layout();

	wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
    m_pText->SetFont(font);

    Clear();
}

ExamineMemoryDlg::~ExamineMemoryDlg()
{
	//dtor
}

void ExamineMemoryDlg::Clear()
{
    m_pText->Clear();
    m_ByteCounter = 0;
    for (int i = 0; i < 67; ++i)
        m_LineText[i] = _T(' ');
}

void ExamineMemoryDlg::AddHexByte(const wxString& addr, const wxString& hexbyte)
{
//    m_pDbg->Log(_T("AddHexByte(") + addr + _T(", ") + hexbyte + _T(')'));
    int bcmod = m_ByteCounter % 16;
    if (m_ByteCounter != 0 && bcmod == 0)
    {
        // filled 16 bytes window; append text and reset accumulator array
        if (m_ByteCounter != 16) // after the first line,
            m_pText->AppendText(_T('\n')); // prepend a newline
        m_pText->AppendText(addr + _T(": ") + m_LineText);
        for (int i = 0; i < 67; ++i)
            m_LineText[i] = _T(' ');
    }

#define HEX_OFFSET(a) (a*3)
#define CHAR_OFFSET(a) (16*3 + 3 + a)

    long hb;
    hexbyte.ToLong(&hb, 16);
//    m_pDbg->Log(wxString::Format(_T("hb=%d, [0]=%c, [1]=%c"), hb, hexbyte[0], hexbyte[1]));
//    m_pDbg->Log(wxString::Format(_T("HEX_OFFSET(bcmod)=%d, CHAR_OFFSET(bcmod)=%d"), HEX_OFFSET(bcmod), CHAR_OFFSET(bcmod)));
    m_LineText[HEX_OFFSET(bcmod)] = hexbyte[0];
    m_LineText[HEX_OFFSET(bcmod) + 1] = hexbyte[1];
    m_LineText[CHAR_OFFSET(bcmod)] = hb >= 32 ? wxChar(hb) : wxChar(_T('.'));
    ++m_ByteCounter;
}
