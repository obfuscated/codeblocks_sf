#include <sdk.h>
#include "cpuregistersdlg.h"
#include "debuggergdb.h"
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>

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
//    EVT_BUTTON(XRCID("btnRefresh"), CPURegistersDlg::OnRefresh)
END_EVENT_TABLE()

CPURegistersDlg::CPURegistersDlg(wxWindow* parent, DebuggerGDB* debugger)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
    m_pDbg(debugger)
{
	//ctor
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	m_pList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
	sizer->Add(m_pList, 1, wxGROW);
	SetSizer(sizer);
	Layout();

    Clear();
}

CPURegistersDlg::~CPURegistersDlg()
{
	//dtor
}

void CPURegistersDlg::Clear()
{
    m_pList->ClearAll();
	m_pList->Freeze();
	m_pList->DeleteAllItems();
    m_pList->InsertColumn(0, _("Register"), wxLIST_FORMAT_LEFT);
    m_pList->InsertColumn(1, _("Hex"), wxLIST_FORMAT_RIGHT);
    m_pList->InsertColumn(2, _("Integer"), wxLIST_FORMAT_RIGHT);
	m_pList->Thaw();
}

void CPURegistersDlg::AddRegisterValue(int idx, long int value)
{
    if (idx == -1)
        return;
	m_pList->Freeze();
	m_pList->InsertItem(m_pList->GetItemCount(), Registers[idx]);
	SetRegisterValue(m_pList->GetItemCount() - 1, value);
	m_pList->Thaw();
}

void CPURegistersDlg::SetRegisterValue(int idx, long int value)
{
    if (idx == -1)
        return;
    if (!m_pList->GetItemCount())
    {
        Clear();
        for (int i = 0; i < 16; ++i)
            AddRegisterValue(i, 0);
    }
	wxString fmt;
	fmt.Printf(_T("0x%x"), (size_t)value);
    m_pList->SetItem(idx, 1, fmt);
	fmt.Printf(_T("%ld"), value);
    m_pList->SetItem(idx, 2, fmt);

	for (int i = 0; i < 3; ++i)
	{
        m_pList->SetColumnWidth(i, wxLIST_AUTOSIZE);
	}
}
