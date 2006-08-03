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
    EVT_TEXT_ENTER(XRCID("txtAddress"), ExamineMemoryDlg::OnGo)
END_EVENT_TABLE()

ExamineMemoryDlg::ExamineMemoryDlg(wxWindow* parent, DebuggerGDB* debugger)
    : m_pDbg(debugger),
    m_LastRowStartingAddress(0)
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
    m_LastRowStartingAddress = 0;
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

    if (m_LastRowStartingAddress == 0)
    {
        // because we 'll be appending each row *after* we have consumed it
        // and then "addr" will point to the next row's starting address,
        // we 'll keep the current row's starting address in "m_LastRowStartingAddress".

        // if it's zero (i.e this is the first row), keep "addr" as starting address for this row.
        // m_LastRowStartingAddress will be set again when we 've consumed this row...
        addr.ToLong(&m_LastRowStartingAddress, 16);
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

    // flush every 16 bytes
    if (m_ByteCounter != 0 && m_ByteCounter % 16 == 0)
    {
        // filled 16 bytes window; append text and reset accumulator array
        if (m_ByteCounter != 16) // after the first line,
            m_pText->AppendText(_T('\n')); // prepend a newline
        m_LineText[23] = _T('|'); // put a "separator" in the middle (just to ease reading a bit)

        long a;
        addr.ToLong(&a, 16);
        m_pText->AppendText(wxString::Format(_T("0x%x: %s"), m_LastRowStartingAddress, m_LineText));
        for (int i = 0; i < 67; ++i)
            m_LineText[i] = _T(' ');
        // update starting address for next row
        // add 8 bytes: addr is the start address of the second 8-byte chunk of this line, so next line is +8
        m_LastRowStartingAddress = a + 8;
    }
}

void ExamineMemoryDlg::OnGo(wxCommandEvent& event)
{
    m_pDbg->MemoryDump();
}
