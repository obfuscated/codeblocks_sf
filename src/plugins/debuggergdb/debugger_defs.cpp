#include <sdk.h>
#include "debugger_defs.h"
#include "debuggerdriver.h"
#include "debuggertree.h"
#include "manager.h"

#include <wx/dialog.h>
#include <wx/font.h>
#include <wx/sizer.h>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(WatchesArray);

const int DEBUGGER_CURSOR_CHANGED = wxNewId();

DebuggerCmd::DebuggerCmd(DebuggerDriver* driver, const wxString& cmd, bool logToNormalLog)
    : m_Cmd(cmd),
    m_pDriver(driver),
    m_LogToNormalLog(logToNormalLog)
{
}

void DebuggerCmd::ParseOutput(const wxString& output)
{
    if (!output.IsEmpty() && m_LogToNormalLog)
        m_pDriver->Log(output);
}

DbgCmd_UpdateWatchesTree::DbgCmd_UpdateWatchesTree(DebuggerDriver* driver, DebuggerTree* tree)
    : DebuggerCmd(driver),
    m_pTree(tree)
{
}

void DbgCmd_UpdateWatchesTree::Action()
{
    m_pTree->EndUpdateTree();
}

// Custom window to display output of DebuggerInfoCmd
class DebuggerInfoWindow : public wxDialog
{
    public:
        DebuggerInfoWindow(wxWindow *parent, const wxChar *title, const wxString& content)
            : wxDialog(parent, -1, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX)
        {
            wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
            m_pText = new wxTextCtrl(this, -1, content, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxTE_RICH2 | wxHSCROLL);
            m_pText->SetFont(font);

            sizer->Add(m_pText, 1, wxGROW);

            SetSizer(sizer);
            sizer->Layout();
        }
        wxTextCtrl* m_pText;
};

void DebuggerInfoCmd::ParseOutput(const wxString& output)
{
    DebuggerInfoWindow win(Manager::Get()->GetAppWindow(), m_Title, output);
    win.ShowModal();
}
