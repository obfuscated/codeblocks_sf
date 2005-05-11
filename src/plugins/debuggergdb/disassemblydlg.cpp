#include "disassemblydlg.h"
#include "debuggergdb.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/button.h>

BEGIN_EVENT_TABLE(DisassemblyDlg, wxDialog)
    EVT_BUTTON(XRCID("btnSave"), DisassemblyDlg::OnSave)
    EVT_BUTTON(XRCID("btnRefresh"), DisassemblyDlg::OnRefresh)
END_EVENT_TABLE()

DisassemblyDlg::DisassemblyDlg(wxWindow* parent, DebuggerGDB* debugger)
    : m_pDbg(debugger)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgDisassembly"));
	SetWindowStyle(GetWindowStyle() | wxFRAME_FLOAT_ON_PARENT);
	wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL); 
    XRCCTRL(*this, "txtCode", wxTextCtrl)->SetFont(font);
}

DisassemblyDlg::~DisassemblyDlg()
{
	//dtor
}

void DisassemblyDlg::Clear(const StackFrame& frame)
{
    XRCCTRL(*this, "txtCode", wxTextCtrl)->Clear();
    XRCCTRL(*this, "lblFunction", wxStaticText)->SetLabel(frame.valid ? frame.function : "??");
    wxString addr = "??";
    if (frame.valid)
        addr.Printf("0x%8.8x", frame.address);
    XRCCTRL(*this, "lblAddress", wxStaticText)->SetLabel(addr);
}

void DisassemblyDlg::AddAssemblerLine(const wxString& line)
{
    wxTextCtrl* txt = XRCCTRL(*this, "txtCode", wxTextCtrl);
    txt->AppendText(line + '\n');
    txt->SetSelection(0, 0);
    txt->SetInsertionPoint(0);
}

void DisassemblyDlg::OnSave(wxCommandEvent& event)
{
    wxFileDialog dlg(this,
                        _("Save as text file"),
                        wxEmptyString,
                        wxEmptyString,
                        ALL_FILES_FILTER,
                        wxSAVE | wxOVERWRITE_PROMPT);
    if (dlg.ShowModal() != wxID_OK)
        return;
    if (XRCCTRL(*this, "txtCode", wxTextCtrl)->SaveFile(dlg.GetPath()))
        wxMessageBox("File saved", "Result", wxICON_INFORMATION);
    else
        wxMessageBox("File could not be saved...", "Result", wxICON_ERROR);
}

void DisassemblyDlg::OnRefresh(wxCommandEvent& event)
{
    m_pDbg->CmdDisassemble();
}
