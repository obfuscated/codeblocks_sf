#include <sdk.h>
#include "disassemblydlg.h"
#include "debuggergdb.h"
#include <wx/wxscintilla.h>
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/wfstream.h>
#include <wx/fontutil.h>
#include <globals.h>
#include <editorcolourset.h>
#include <manager.h>
#include <editormanager.h>
#include <configmanager.h>
#include <filefilters.h>

#define DEBUG_MARKER        4
#define DEBUG_STYLE         wxSCI_MARK_ARROW

BEGIN_EVENT_TABLE(DisassemblyDlg, wxPanel)
    EVT_BUTTON(XRCID("btnSave"), DisassemblyDlg::OnSave)
//    EVT_BUTTON(XRCID("btnRefresh"), DisassemblyDlg::OnRefresh)
END_EVENT_TABLE()

DisassemblyDlg::DisassemblyDlg(wxWindow* parent, DebuggerGDB* debugger)
    : m_pDbg(debugger),
    m_LastActiveAddr(0),
    m_ClearFlag(false)
{
    //ctor
    wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgDisassembly"));
//    SetWindowStyle(GetWindowStyle() | wxFRAME_FLOAT_ON_PARENT);

    m_pCode = new wxScintilla(this, wxID_ANY);
    m_pCode->SetReadOnly(true);
    m_pCode->SetCaretWidth(0);
    m_pCode->SetMarginWidth(0, 0);
    m_pCode->SetMarginType(1, wxSCI_MARGIN_SYMBOL);
    m_pCode->SetMarginSensitive(1, 0);
    m_pCode->SetMarginMask(1, (1 << DEBUG_MARKER));
    m_pCode->MarkerDefine(DEBUG_MARKER, DEBUG_STYLE);
    m_pCode->MarkerSetBackground(DEBUG_MARKER, wxColour(0xFF, 0xFF, 0x00));
    wxXmlResource::Get()->AttachUnknownControl(_T("lcCode"), m_pCode);

    // use the same font as editor's
    wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
    wxString fontstring = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/font"), wxEmptyString);
    if (!fontstring.IsEmpty())
    {
        wxNativeFontInfo nfi;
        nfi.FromString(fontstring);
        font.SetNativeFontInfo(nfi);
    }
    m_pCode->StyleSetFont(wxSCI_STYLE_DEFAULT, font);

    EditorColourSet* set = Manager::Get()->GetEditorManager()->GetColourSet();
    if (set)
    {
        HighlightLanguage lang = set->GetHighlightLanguage(wxSCI_LEX_ASM);
        set->Apply(lang, (cbStyledTextCtrl*)m_pCode);
    }

    StackFrame sf;
    Clear(sf);
}

DisassemblyDlg::~DisassemblyDlg()
{
    //dtor
}

void DisassemblyDlg::Clear(const StackFrame& frame)
{
    XRCCTRL(*this, "lblFunction", wxStaticText)->SetLabel(frame.valid ? frame.function : _T("??"));
    wxString addr = _T("??");
    if (frame.valid)
        addr.Printf(_T("%p"), (void*)frame.address);
    XRCCTRL(*this, "lblAddress", wxStaticText)->SetLabel(addr);

    m_HasActiveAddr = false;

    m_pCode->SetReadOnly(false);
    if (m_pDbg->IsRunning())
    {
        // if debugger is running, show a message
        m_pCode->SetText(_("\"Please wait while disassemblying...\""));
        m_ClearFlag = true; // clear the above message when adding the first line
    }
    else
    {
        // if debugger isn't running, just clear the window
        m_pCode->ClearAll();
        m_ClearFlag = false;
    }
    m_pCode->SetReadOnly(true);
    m_pCode->MarkerDeleteAll(DEBUG_MARKER);
}

void DisassemblyDlg::AddAssemblerLine(unsigned long int addr, const wxString& line)
{
    m_pCode->SetReadOnly(false);
    if (m_ClearFlag)
    {
        m_ClearFlag = false;
        m_pCode->ClearAll();
    }
    wxString fmt;
    fmt.Printf(_T("0x%x\t%s\n"), (size_t)addr, line.c_str());
    m_pCode->AppendText(fmt);
    SetActiveAddress(m_LastActiveAddr);
    m_pCode->SetReadOnly(true);
}

void DisassemblyDlg::SetActiveAddress(unsigned long int addr)
{
    if (m_HasActiveAddr && addr == m_LastActiveAddr)
        return;
    m_HasActiveAddr = false;
    m_LastActiveAddr = addr;
    for (int i = 0; i < m_pCode->GetLineCount(); ++i)
    {
        wxString str = m_pCode->GetLine(i).AfterFirst(_T('x')).BeforeFirst(_T('\t'));
        unsigned long lineaddr;
        if (str.ToULong(&lineaddr, 16) && lineaddr >= addr)
        {
            m_pCode->MarkerDeleteAll(DEBUG_MARKER);
            m_pCode->MarkerAdd(i, DEBUG_MARKER);
            m_pCode->GotoLine(i);
            m_pCode->EnsureVisible(i);
            m_HasActiveAddr = true;
            break;
        }
    }
}

void DisassemblyDlg::OnSave(wxCommandEvent& event)
{
    wxFileDialog dlg(this,
                     _("Save as text file"),
                     wxEmptyString,
                     wxEmptyString,
                     FileFilters::GetFilterAll(),
                     wxSAVE | wxOVERWRITE_PROMPT);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxFFileOutputStream output(dlg.GetPath());
    if (!output.Ok())
    {
        cbMessageBox(_("Could not open file for saving..."), _("Result"), wxICON_ERROR);
        return;
    }
    wxTextOutputStream text(output);

//    wxListCtrl* lc = XRCCTRL(*this, "lcCode", wxListCtrl);
//    for (int i = 0; i < lc->GetItemCount(); ++i)
//    {
//        wxListItem info;
//        info.m_itemId = i;
//        info.m_col = 1;
//        info.m_mask = wxLIST_MASK_TEXT;
//        wxString instr = lc->GetItem(info) && !info.m_text.IsEmpty() ? info.m_text : _T("??");
//
//        text << lc->GetItemText(i) << _T(": ") << instr << _T('\n');
//    }
    cbMessageBox(_("File saved"), _("Result"), wxICON_INFORMATION);
}

void DisassemblyDlg::OnRefresh(wxCommandEvent& event)
{
    m_pDbg->Disassemble();
}
