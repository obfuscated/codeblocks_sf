#include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/listctrl.h>
    #include <wx/menu.h>
    #include <wx/string.h>
    #include <wx/utils.h>
    #include "globals.h"
#endif
#include <wx/clipbrd.h>
#include "compilererrors.h"
#include "compilermessages.h"

static int idGotoMessage = wxNewId();
static int idCopyToClipboard = wxNewId();

BEGIN_EVENT_TABLE(CompilerMessages, SimpleListLog)
    EVT_MENU(idGotoMessage, CompilerMessages::OnClick)
    EVT_MENU(idCopyToClipboard, CompilerMessages::OnCopyToClipboard)
END_EVENT_TABLE()

CompilerMessages::CompilerMessages(int numCols, int widths[], const wxArrayString& titles)
    : SimpleListLog(numCols, widths, titles, true)
{
	//ctor
    int id = m_pList->GetId();
    Connect(id, -1, wxEVT_COMMAND_LIST_ITEM_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &CompilerMessages::OnClick);
    Connect(id, -1, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &CompilerMessages::OnDoubleClick);
    Connect(id, -1, wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &CompilerMessages::OnRightClick);
}

CompilerMessages::~CompilerMessages()
{
	//dtor
}

void CompilerMessages::FocusError(int nr)
{
    m_pList->SetItemState(nr, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
    m_pList->EnsureVisible(nr);
}

void CompilerMessages::OnClick(wxCommandEvent& event)
{
    // single and double-click, behave the same

    // a compiler message has been clicked
    // go to the relevant file/line
    if (m_pList->GetSelectedItemCount() == 0 || !m_pErrors)
        return;

    // find selected item index
    int index = m_pList->GetNextItem(-1,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);

    // call the CompilerErrors* ptr; it 'll do all the hard work ;)
    m_pErrors->GotoError(index);
}

void CompilerMessages::OnDoubleClick(wxCommandEvent& event)
{
    // single and double-click, behave the same
    OnClick(event);
    return;
}

void CompilerMessages::OnRightClick(wxCommandEvent& event)
{
    wxMenu m;
    m.Append(idGotoMessage, _("Jump to selected message"));
    m.AppendSeparator();
    m.Append(idCopyToClipboard, _("Copy all messages to clipboard"));
    PopupMenu(&m);
}

void CompilerMessages::OnCopyToClipboard(wxCommandEvent& event)
{
    wxString text;
    for (int i = 0; i < m_pList->GetItemCount(); ++i)
    {
        wxListItem info;
        info.m_itemId = i;
        info.m_mask = wxLIST_MASK_TEXT;

        info.m_col = 1;
        m_pList->GetItem(info);
        wxString line = info.m_text;

        info.m_col = 2;
        m_pList->GetItem(info);
        wxString msg = info.m_text;

        // file:line: msg
        text << m_pList->GetItemText(i) << _T(':') << line << _T(": ");
        text << msg << _T('\n');
    }

    if (!text.IsEmpty() && wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(text));
        wxTheClipboard->Close();
    }
}
