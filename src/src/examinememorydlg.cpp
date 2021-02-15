/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 * Modified MicroSourceCode https://github.com/MicroSourceCode
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"

#ifndef CB_PRECOMP
    #include <wx/button.h>
    #include <wx/combobox.h>
    #include <wx/intl.h>
    #include <wx/textctrl.h>
    #include <wx/xrc/xmlres.h>

    #include "cbplugin.h"
#endif

#include "examinememorydlg.h"
#include "debuggermanager.h"

BEGIN_EVENT_TABLE(ExamineMemoryDlg, wxPanel)
    EVT_BUTTON(XRCID("btnGo"), ExamineMemoryDlg::OnGo)
    EVT_COMBOBOX(XRCID("cmbBytes"), ExamineMemoryDlg::OnGo)
    EVT_COMBOBOX(XRCID("colSelect"), ExamineMemoryDlg::OnGo)
    EVT_TEXT_ENTER(XRCID("txtAddress"), ExamineMemoryDlg::OnGo)
END_EVENT_TABLE()

ExamineMemoryDlg::ExamineMemoryDlg(wxWindow* parent) :
    m_LastRowStartingAddress(0)
{
    //ctor
    if (!wxXmlResource::Get()->LoadPanel(this, parent, _T("MemoryDumpPanel")))
        return;
    m_pText = XRCCTRL(*this, "txtDump", wxTextCtrl);

    wxFont font(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    m_pText->SetFont(font);

    ConfigManager *c = Manager::Get()->GetConfigManager(wxT("debugger_common"));
    int bytes = c->ReadInt(wxT("/common/examine_memory/size_to_show"), 32);
    int bytesCols = c->ReadInt(wxT("/common/examine_memory/columns_to_show"), 8);
    wxString strBytes, strCols;
    strBytes << bytes;
    strCols << bytesCols;

    wxComboBox *combo = XRCCTRL(*this, "cmbBytes", wxComboBox);
    if (!combo->SetStringSelection(strBytes))
        combo->SetSelection(1); // Default is 32 bytes

    wxComboBox *combo2 = XRCCTRL(*this, "colSelect", wxComboBox);
    if (!combo2->SetStringSelection(strCols))
        combo2->SetSelection(3); // Default is 8 columns

    Clear();
}

void ExamineMemoryDlg::Begin()
{
    m_pText->Freeze();
}

void ExamineMemoryDlg::End()
{
    m_pText->Thaw();
    m_pText->SetInsertionPoint(0); //Scrolling up
}

void ExamineMemoryDlg::Clear()
{
    m_ColumnsCtrl = GetCols();
    m_pText->Clear();
    m_LastRowStartingAddress = 0;
    m_ByteCounter = 0;
    //  (4 column * 2) + 4 column =  12,  for 6 is m_PartLength = 18
    m_PartLength = (m_ColumnsCtrl * 2) + m_ColumnsCtrl;

    for (int i = 0; i < 128; ++i) //128 is: 32 * 3 + 32
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

int ExamineMemoryDlg::GetCols()
{
    long a;
    XRCCTRL(*this, "colSelect", wxComboBox)->GetValue().ToLong(&a);
    return a;
}

void ExamineMemoryDlg::AddError(const wxString& err)
{
    m_pText->AppendText(err + _T('\n'));
}

void ExamineMemoryDlg::AddHexByte(const wxString& addr, const wxString& hexbyte)
{
//    m_pDbg->Log(_T("AddHexByte(") + addr + _T(", ") + hexbyte + _T(')'));
    int bcmod = m_ByteCounter % m_ColumnsCtrl;

    if (m_LastRowStartingAddress == 0)
    {
        // because we 'll be appending each row *after* we have consumed it
        // and then "addr" will point to the next row's starting address,
        // we 'll keep the current row's starting address in "m_LastRowStartingAddress".

        // if it's zero (i.e this is the first row), keep "addr" as starting address for this row.
        // m_LastRowStartingAddress will be set again when we 've consumed this row...
        m_LastRowStartingAddress = cbDebuggerStringToAddress(addr);
    }

  #define HEX_OFFSET(a) (a*3)
//#define CHAR_OFFSET(a) (16*3 + 3 + a)

    unsigned long hb;
    hexbyte.ToULong(&hb, 16);
//    m_pDbg->Log(wxString::Format(_T("hb=%d, [0]=%c, [1]=%c"), hb, hexbyte[0], hexbyte[1]));
//    m_pDbg->Log(wxString::Format(_T("HEX_OFFSET(bcmod)=%d, CHAR_OFFSET(bcmod)=%d"), HEX_OFFSET(bcmod), CHAR_OFFSET(bcmod)));
    
    m_LineText[HEX_OFFSET(bcmod)] = hexbyte[0];
    m_LineText[HEX_OFFSET(bcmod) + 1] = hexbyte[1];
    m_LineText[(m_PartLength + bcmod)] = hb >= 32 ? wxChar(hb) : wxChar(_T('.'));
    ++m_ByteCounter;

     // flush every m_ColumnsCtrl bytes
     if (m_ByteCounter != 0 && m_ByteCounter % m_ColumnsCtrl == 0)
    {
	     // filled m_ColumnsCtrl bytes window; append text and reset accumulator array
		 if (m_ByteCounter != m_ColumnsCtrl) // after the first line,
                    m_pText->AppendText(_T('\n')); // prepend a newline
       
		 m_pText->AppendText(wxString::Format(_T("0x%lx: "), m_LastRowStartingAddress));
		 for (unsigned i = 0; i < (m_PartLength + m_ColumnsCtrl); ++i)
			m_pText->AppendText(m_LineText[i]);
		
	// update starting address for next row every m_ColumnsCtrl bytes
        m_LastRowStartingAddress += m_ColumnsCtrl;
    }
}
	
void ExamineMemoryDlg::OnGo(cb_unused wxCommandEvent& event)
{
    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();

    // Save the values of the bytes combo box in the config,
    // so it is the same next time the dialog is used.
    ConfigManager *c = Manager::Get()->GetConfigManager(wxT("debugger_common"));
    c->Write(wxT("/common/examine_memory/size_to_show"), GetBytes());
    c->Write(wxT("/common/examine_memory/columns_to_show"), GetCols());

    if (plugin)
        plugin->RequestUpdate(cbDebuggerPlugin::ExamineMemory);
}

void ExamineMemoryDlg::EnableWindow(bool enable)
{
    Enable(enable);
}

void ExamineMemoryDlg::SetBaseAddress(const wxString &addr)
{
    XRCCTRL(*this, "txtAddress", wxTextCtrl)->SetValue(addr);

    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    if (plugin)
        plugin->RequestUpdate(cbDebuggerPlugin::ExamineMemory);

}
