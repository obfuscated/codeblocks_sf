#include <sdk.h>
#include "cpuregistersdlg.h"
#include "debuggergdb.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/wfstream.h>

// static
wxString CPURegistersDlg::Registers[] = {
    _T("eax"),
    _T("ebx"),
    _T("ecx"),
    _T("edx"),
    _T("esp"),
    _T("esi"),
    _T("ebp"),
    _T("edi"),
    _T("eip"),
    _T("eflags"),
    _T("cs"),
    _T("ds"),
    _T("es"),
    _T("fs"),
    _T("gs"),
    _T("ss")
};

int CPURegistersDlg::RegisterIndexFromName(const wxString& name)
{
    for (int i = 0; i < 16; ++i)
    {
        if (name.Matches(CPURegistersDlg::Registers[i]))
            return i;
    }
    return -1;
}

BEGIN_EVENT_TABLE(CPURegistersDlg, wxPanel)
    EVT_BUTTON(XRCID("btnRefresh"), CPURegistersDlg::OnRefresh)
END_EVENT_TABLE()

CPURegistersDlg::CPURegistersDlg(wxWindow* parent, DebuggerGDB* debugger)
    : m_pDbg(debugger)
{
	//ctor
	wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgCPURegisters"));

    Clear();
}

CPURegistersDlg::~CPURegistersDlg()
{
	//dtor
}

void CPURegistersDlg::Clear()
{
    wxListCtrl* lc = XRCCTRL(*this, "lcRegisters", wxListCtrl);
    lc->ClearAll();
	lc->Freeze();
	lc->DeleteAllItems();
    lc->InsertColumn(0, _("Register"), wxLIST_FORMAT_LEFT);
    lc->InsertColumn(1, _("Hex"), wxLIST_FORMAT_RIGHT);
    lc->InsertColumn(2, _("Integer"), wxLIST_FORMAT_RIGHT);
	lc->Thaw();
}

void CPURegistersDlg::AddRegisterValue(int idx, long int value)
{
    if (idx == -1)
        return;
    wxListCtrl* lc = XRCCTRL(*this, "lcRegisters", wxListCtrl);
	lc->Freeze();
	lc->InsertItem(lc->GetItemCount(), Registers[idx]);
	SetRegisterValue(lc->GetItemCount() - 1, value);
	lc->Thaw();
}

void CPURegistersDlg::SetRegisterValue(int idx, long int value)
{
    if (idx == -1)
        return;
    wxListCtrl* lc = XRCCTRL(*this, "lcRegisters", wxListCtrl);
    if (!lc->GetItemCount())
    {
        Clear();
        for (int i = 0; i < 16; ++i)
            AddRegisterValue(i, 0);
    }
	wxString fmt;
	fmt.Printf(_T("0x%x"), (size_t)value);
    lc->SetItem(idx, 1, fmt);
	fmt.Printf(_T("%ld"), value);
    lc->SetItem(idx, 2, fmt);

	for (int i = 0; i < 3; ++i)
	{
        lc->SetColumnWidth(i, wxLIST_AUTOSIZE);
	}
}

void CPURegistersDlg::OnRefresh(wxCommandEvent& event)
{
    m_pDbg->CmdRegisters();
}
