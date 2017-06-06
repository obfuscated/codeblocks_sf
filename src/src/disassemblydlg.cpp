/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"

#include "disassemblydlg.h"
#ifndef CB_PRECOMP
    #include <wx/wxscintilla.h>
    #include <wx/intl.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/textctrl.h>
    #include <wx/button.h>
    #include <wx/listctrl.h>
    #include <wx/wfstream.h>
    #include <wx/fontutil.h>
    #include <wx/stattext.h>
    #include <wx/filedlg.h>

    #include "cbproject.h"
    #include "configmanager.h"
    #include "editorcolourset.h"
    #include "editormanager.h"
    #include "globals.h"
    #include "manager.h"
    #include "projectmanager.h"
#endif

#include "debuggermanager.h"
#include "filefilters.h"

// Keep in sync with cbEditor.cpp:
#define DEBUG_MARKER     6
#define DEBUG_STYLE      wxSCI_MARK_ARROW

BEGIN_EVENT_TABLE(DisassemblyDlg, wxPanel)
    EVT_BUTTON(XRCID("btnSave"), DisassemblyDlg::OnSave)
//    EVT_BUTTON(XRCID("btnRefresh"), DisassemblyDlg::OnRefresh)
    EVT_CHECKBOX(XRCID("chkMode"), DisassemblyDlg::OnMixedModeCB)
    EVT_BUTTON(XRCID("btnAdjustLine"), DisassemblyDlg::OnAdjustLine)
END_EVENT_TABLE()

DisassemblyDlg::DisassemblyDlg(wxWindow* parent) :
    m_LastActiveAddr(0),
    m_ClearFlag(false)
{
    if (!wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgDisassembly")))
        return;

    m_pCode = new wxScintilla(this, wxID_ANY, wxDefaultPosition, wxSize(1,1));
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
    wxFont font(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxString fontstring = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/font"), wxEmptyString);
    if (!fontstring.IsEmpty())
    {
        wxNativeFontInfo nfi;
        nfi.FromString(fontstring);
        font.SetNativeFontInfo(nfi);
    }
    m_pCode->StyleSetFont(wxSCI_STYLE_DEFAULT, font);

    EditorColourSet* colour_set = Manager::Get()->GetEditorManager()->GetColourSet();
    if (colour_set)
    {
        HighlightLanguage lang = colour_set->GetHighlightLanguage(wxSCI_LEX_ASM);
        colour_set->Apply(lang, (cbStyledTextCtrl*)m_pCode);
    }

    m_MixedModeCB = (wxCheckBox*)FindWindow(XRCID("chkMode"));
    m_MixedModeCB->SetValue(Manager::Get()->GetDebuggerManager()->IsDisassemblyMixedMode());

    cbStackFrame sf;
    Clear(sf);
}

void DisassemblyDlg::Clear(const cbStackFrame& frame)
{
    m_FrameFunction = frame.IsValid() ? frame.GetSymbol() : _T("??");
    m_FrameAddress = _T("??");
    if (frame.IsValid())
        m_FrameAddress = frame.GetAddressAsString();

    m_LineTypes.clear();

    XRCCTRL(*this, "lblFunction", wxStaticText)->SetLabel(m_FrameFunction);
    XRCCTRL(*this, "lblAddress", wxStaticText)->SetLabel(m_FrameAddress);

    m_HasActiveAddr = false;

    m_pCode->SetReadOnly(false);

    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();

    if (frame.IsValid() && plugin->IsRunning())
    {
        // if debugger is running, show a message
        m_pCode->SetText(_("\"Please wait while disassembling...\""));
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

void DisassemblyDlg::AddAssemblerLine(uint64_t addr, const wxString& line)
{
    m_pCode->SetReadOnly(false);
    if (m_ClearFlag)
    {
        m_ClearFlag = false;
        m_pCode->ClearAll();
    }

    m_pCode->AppendText(cbDebuggerAddressToString(addr) + wxT("\t") + line + wxT("\n"));
    SetActiveAddress(m_LastActiveAddr);
    m_pCode->SetReadOnly(true);
    m_LineTypes.push_back('D') ;
}

void DisassemblyDlg::AddSourceLine(int lineno, const wxString& line)
{
    m_pCode->SetReadOnly(false);
    if (m_ClearFlag)
    {
        m_ClearFlag = false;
        m_pCode->ClearAll();
    }
    wxString fmt;
    fmt.Printf(_T(";%-3d:\t%s\n"), lineno, line.c_str());

    m_pCode->AppendText(fmt);

    m_pCode->SetReadOnly(true);
    m_LineTypes.push_back('S') ;
}

void DisassemblyDlg::CenterLine(int lineno)
{
    //make line middle of display window if reasonable
    int firstdispline ;
    int los = m_pCode->LinesOnScreen() ;
    if (lineno > los / 2)
        firstdispline = lineno - (los/2) ;
    else
        firstdispline = 0 ; //or is it zero?
    m_pCode->SetFirstVisibleLine(firstdispline) ;
}

void DisassemblyDlg::CenterCurrentLine()
{
    int displine;
    displine = m_pCode->GetCurrentLine() ;
    CenterLine(displine);
}

bool DisassemblyDlg::SetActiveAddress(uint64_t addr)
{
    if (m_HasActiveAddr && addr == m_LastActiveAddr)
        return m_HasActiveAddr ;
    m_HasActiveAddr = false;
    m_LastActiveAddr = addr;
    bool MixedAsmMode = Manager::Get()->GetDebuggerManager()->IsDisassemblyMixedMode();
    for (int i = 0; i < m_pCode->GetLineCount() && i < int(m_LineTypes.size()); ++i)
    {
        if(MixedAsmMode && m_LineTypes[i] == 'S')
            continue;

        const wxString &str = m_pCode->GetLine(i).AfterFirst(_T('x')).BeforeFirst(_T('\t'));
        uint64_t lineaddr = cbDebuggerStringToAddress(str);
        if (lineaddr > 0 && (lineaddr == addr))
        {
            m_pCode->MarkerDeleteAll(DEBUG_MARKER);
            m_pCode->MarkerAdd(i, DEBUG_MARKER);
            m_pCode->GotoLine(i);

            //check and shift window lines if needed
            if (!m_pCode->GetLineVisible(i))
            {
                this->CenterLine(i);
            }
            //are we close to bottom line? if so shift display if possible
            else if (i == (m_pCode->LinesOnScreen() + m_pCode->GetFirstVisibleLine() - 1))
            {
                this->CenterLine(i);
            }

            m_HasActiveAddr = true;
            break;
        }
    }
    return m_HasActiveAddr ;
}

void DisassemblyDlg::OnAdjustLine(cb_unused wxCommandEvent& event)
{
    int los = m_pCode->LinesOnScreen();

    int displine;
    if (m_pCode->GetCurrentLine() == m_pCode->GetFirstVisibleLine())
        displine = m_pCode->GetCurrentLine();
    else if (m_pCode->GetCurrentLine() == m_pCode->GetFirstVisibleLine() + los/2)
        displine = m_pCode->GetCurrentLine() - (los/2) + 1;
    else
        displine = m_pCode->GetCurrentLine() + (los/2);

    if (displine < 0)
        displine = 0;
    CenterLine(displine);
}

void DisassemblyDlg::OnSave(cb_unused wxCommandEvent& event)
{
    wxFileDialog dlg(this,
                     _("Save as text file"),
                     _T("assembly_dump.txt"),
                     wxEmptyString,
                     FileFilters::GetFilterAll(),
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxString output;
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (prj)
    {
        output << _("Project title : ") << prj->GetTitle() << _T('\n');
        output << _("Project path  : ") << prj->GetBasePath() << _T('\n') << _T('\n');
    }

    output << _("Frame function: ") << m_FrameFunction << _T('\n');
    output << _("Frame address : ") << m_FrameAddress << _T('\n');
    output << wxString(_T('-'), 80) << _T('\n');
    output << m_pCode->GetText();

    if (!cbSaveToFile(dlg.GetPath(), output))
        cbMessageBox(_("Could not save file..."), _("Error"), wxICON_ERROR);
}

void DisassemblyDlg::OnRefresh(cb_unused wxCommandEvent& event)
{
    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    cbAssert(plugin);
    plugin->RequestUpdate(cbDebuggerPlugin::Disassembly);
}

void DisassemblyDlg::OnMixedModeCB(cb_unused wxCommandEvent &event)
{
    DebuggerManager &manager = *Manager::Get()->GetDebuggerManager();
    bool newMode = !manager.IsDisassemblyMixedMode();
    manager.SetDisassemblyMixedMode(newMode);
    m_MixedModeCB->SetValue(newMode);

    cbDebuggerPlugin *plugin = manager.GetActiveDebugger();
    cbAssert(plugin);
    plugin->RequestUpdate(cbDebuggerPlugin::Disassembly);
}

void DisassemblyDlg::EnableWindow(bool enable)
{
    Enable(enable);
}
