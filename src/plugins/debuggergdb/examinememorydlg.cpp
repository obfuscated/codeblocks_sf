#include <sdk.h>
#include "examinememorydlg.h"
#include "debuggergdb.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/button.h>

BEGIN_EVENT_TABLE(ExamineMemoryDlg, wxPanel)
    EVT_BUTTON(XRCID("btnGo"), ExamineMemoryDlg::OnGo)
    EVT_COMBOBOX(XRCID("cmbBytes"), ExamineMemoryDlg::OnGo)
END_EVENT_TABLE()

ExamineMemoryDlg::ExamineMemoryDlg(wxWindow* parent, DebuggerGDB* debugger)
    : m_pDbg(debugger)
{
	//ctor
	wxXmlResource::Get()->LoadPanel(this, parent, _T("MemoryDumpPanel"));
	m_pText = XRCCTRL(*this, "txtDump", wxTextCtrl);

	wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
    m_pText->SetFont(font);

    Clear();
}

ExamineMemoryDlg::~ExamineMemoryDlg()
{
	//dtor
}

void ExamineMemoryDlg::Begin()
{
    m_pText->Freeze();
}

void ExamineMemoryDlg::End()
{
    m_pText->Thaw();
}

void ExamineMemoryDlg::Clear()
{
    m_pText->Clear();
    m_ByteCounter = 0;
    for (int i = 0; i < 67; ++i)
        m_LineText[i] = _T(' ');
}

wxString ExamineMemoryDlg::GetBaseAddress()
{
    return XRCCTRL(*this, "txtAddress", wxTextCtrl)->GetValue();
}

int ExamineMemoryDlg::GetBytes()
{
    long a;
    XRCCTRL(*this, "cmbBytes", wxComboBox)->GetValue().ToLong(&a);
    return a;
}

void ExamineMemoryDlg::AddError(const wxString& err)
{
    m_pText->AppendText(err + _T('\n'));
}

void ExamineMemoryDlg::AddHexByte(const wxString& addr, const wxString& hexbyte)
{
//    m_pDbg->Log(_T("AddHexByte(") + addr + _T(", ") + hexbyte + _T(')'));
    int bcmod = m_ByteCounter % 16;

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

    // flush every 16 bytes
    if (m_ByteCounter != 0 && m_ByteCounter % 16 == 0)
    {
        // filled 16 bytes window; append text and reset accumulator array
        if (m_ByteCounter != 16) // after the first line,
            m_pText->AppendText(_T('\n')); // prepend a newline
        long a;
        addr.ToLong(&a, 16);
        a -= 8; // actually addr holds the start-address of the 2nd 8-bytes sequence,
                // but we want the start-address of the previous 8-bytes sequence displayed
        m_pText->AppendText(wxString::Format(_T("0x%x: %s"), a, m_LineText));
        for (int i = 0; i < 67; ++i)
            m_LineText[i] = _T(' ');
    }
}

void ExamineMemoryDlg::OnGo(wxCommandEvent& event)
{
    m_pDbg->MemoryDump();
}
