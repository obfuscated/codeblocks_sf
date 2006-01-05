#include "sdk_precomp.h"
#include "searchresultslog.h"
#include "manager.h"
#include "editormanager.h"
#include "cbeditor.h"
#include <wx/filename.h>

BEGIN_EVENT_TABLE(SearchResultsLog, SimpleListLog)
//
END_EVENT_TABLE()

SearchResultsLog::SearchResultsLog(int numCols, int widths[], const wxArrayString& titles)
    : SimpleListLog(numCols, widths, titles)
{
	//ctor
    int id = m_pList->GetId();
    Connect(id, -1, wxEVT_COMMAND_LIST_ITEM_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &SearchResultsLog::OnClick);
    Connect(id, -1, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &SearchResultsLog::OnDoubleClick);
}

SearchResultsLog::~SearchResultsLog()
{
	//dtor
}

void SearchResultsLog::FocusEntry(size_t index)
{
    if (index >= 0 && index < (size_t)m_pList->GetItemCount())
    {
        m_pList->SetItemState(index, wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED, wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED);
        m_pList->EnsureVisible(index);
        SyncEditor(index);
    }
}

void SearchResultsLog::SyncEditor(int selIndex)
{
    wxFileName filename(m_pList->GetItemText(selIndex));
    wxString file;
    if (!filename.IsAbsolute())
        file = m_Base + wxFILE_SEP_PATH + filename.GetFullPath();
    else
        file = filename.GetFullPath();

    wxListItem li;
    li.m_itemId = selIndex;
    li.m_col = 1;
    li.m_mask = wxLIST_MASK_TEXT;
    m_pList->GetItem(li);
    long line = 0;
    li.m_text.ToLong(&line);

    cbEditor* ed = Manager::Get()->GetEditorManager()->Open(file);
    if (!ed)
        return;
    ed->Activate();
    ed->GotoLine(line - 1);
}

void SearchResultsLog::OnClick(wxCommandEvent& event)
{
    // single and double-click, behave the same

    // go to the relevant file/line
    if (m_pList->GetSelectedItemCount() == 0)
        return;

    // find selected item index
    int index = m_pList->GetNextItem(-1,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);

    SyncEditor(index);
}

void SearchResultsLog::OnDoubleClick(wxCommandEvent& event)
{
    // single and double-click, behave the same
    OnClick(event);
}
