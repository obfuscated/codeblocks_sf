
/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#include "sdk_precomp.h"

#include "incremental_select_helper.h"

#ifndef CB_PRECOMP
    #include <wx/dialog.h>
    #include <wx/textctrl.h>
    #include <wx/listctrl.h>
#endif

IncrementalSelectIterator::~IncrementalSelectIterator()
{
}

int IncrementalSelectIterator::GetColumnWidth(int column) const
{
    return wxLIST_AUTOSIZE;
}

void IncrementalSelectIterator::CalcColumnWidth(cb_unused wxListCtrl &list)
{
}

int IncrementalSelectIteratorIndexed::GetFilteredCount() const
{
    return m_indices.size();
}

void IncrementalSelectIteratorIndexed::Reset()
{
    m_indices.clear();
}

void IncrementalSelectIteratorIndexed::AddIndex(int index)
{
    m_indices.push_back(index);
}

int IncrementalSelectIteratorIndexed::GetUnfilteredIndex(int index) const
{
    if (index >= 0 && index < int(m_indices.size()))
        return m_indices[index];
    else
        return wxNOT_FOUND;
}

IncrementalSelectHandler::IncrementalSelectHandler(wxDialog* parent, IncrementalSelectIterator *iterator) :
    m_parent(parent),
    m_list(nullptr),
    m_text(nullptr),
    m_iterator(iterator)
{
    m_parent->PushEventHandler(this);
    SetEvtHandlerEnabled(true);
}

IncrementalSelectHandler::~IncrementalSelectHandler()
{
}

void IncrementalSelectHandler::Init(wxListCtrl *list, wxTextCtrl *text)
{
    m_list = list;
    m_text = text;

    m_text->Connect(wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&IncrementalSelectHandler::OnTextChanged,
                    nullptr, this);
    m_text->Connect(wxEVT_KEY_DOWN, (wxObjectEventFunction)&IncrementalSelectHandler::OnKeyDown, nullptr, this);
    m_list->Connect(wxEVT_KEY_DOWN, (wxObjectEventFunction)&IncrementalSelectHandler::OnKeyDown, nullptr, this);
    m_list->Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(IncrementalSelectHandler::OnItemActivated),
                    nullptr, this);

    m_iterator->CalcColumnWidth(*m_list);
    FilterItems();
}

void IncrementalSelectHandler::DeInit(wxWindow *window)
{
    m_list->Disconnect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(IncrementalSelectHandler::OnItemActivated),
                       nullptr, this);
    m_text->Disconnect(wxEVT_KEY_DOWN, (wxObjectEventFunction)&IncrementalSelectHandler::OnKeyDown, nullptr, this);
    m_list->Disconnect(wxEVT_KEY_DOWN, (wxObjectEventFunction)&IncrementalSelectHandler::OnKeyDown, nullptr, this);
    m_text->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&IncrementalSelectHandler::OnTextChanged,
                       nullptr, this);

    SetEvtHandlerEnabled(false);
    window->RemoveEventHandler(this);
}

void IncrementalSelectHandler::OnTextChanged(wxCommandEvent& event)
{
    FilterItems();
    event.Skip();
}

void FilterItemsFinalize(wxListCtrl &list, IncrementalSelectIterator &iterator)
{
    list.SetItemCount(iterator.GetFilteredCount());

    for (int ii = 0; ii < list.GetColumnCount(); ++ii)
    {
        int width = iterator.GetColumnWidth(ii);
        if (width != -1)
            list.SetColumnWidth(ii, width);
    }

    if (iterator.GetFilteredCount() > 0)
        list.SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    list.Refresh();
}

void IncrementalSelectHandler::FilterItems()
{
    m_iterator->Reset();

    const wxString &inputPattern = m_text->GetValue().Lower();
    if (inputPattern.empty())
    {
        int count = m_iterator->GetTotalCount();
        for (int ii = 0; ii < count; ++ii)
            m_iterator->AddIndex(ii);

        FilterItemsFinalize(*m_list, *m_iterator);
        return;
    }

    // We put a star before and after pattern to find search expression everywhere in path
    // that is: if user enter "a", it will match "123a", "12a" or "a12".
    wxString search(wxT("*") + inputPattern + wxT("*"));
    bool isWord = !inputPattern.empty();
    for (auto ch : inputPattern)
    {
        if (!wxIsalpha(ch))
        {
            isWord = false;
            break;
        }
    }

    std::vector<int> indices, promoted;
    indices.reserve(100);
    promoted.reserve(100);

    for (int i = 0; i < m_iterator->GetTotalCount(); ++i)
    {
        wxString const &item = m_iterator->GetItemFilterString(i).Lower();
        if (item.Matches(search.c_str()))
        {
            // If the search pattern doesn't contain non alpha characters and it matches at the start of the word in
            // the item string then promote these items to the top of the list. The order is preserved.
            if (isWord)
            {
                size_t pos = 0, newPos;
                bool isPromoted = false;

                while ((newPos = item.find(inputPattern, pos)) != wxString::npos)
                {
                    if (newPos == 0)
                    {
                        isPromoted = true;
                        break;
                    }

                    if (!wxIsalpha(item[newPos - 1]))
                    {
                        isPromoted = true;
                        break;
                    }

                    // Move one character forward to prevent the same string to be found again.
                    pos = newPos + 1;
                }
                if (isPromoted)
                    promoted.push_back(i);
                else
                    indices.push_back(i);
            }
            else
                indices.push_back(i);
        }
    }

    for (auto i : promoted)
        m_iterator->AddIndex(i);
    for (auto i : indices)
        m_iterator->AddIndex(i);

    FilterItemsFinalize(*m_list, *m_iterator);
}

#if !wxCHECK_VERSION(3, 0, 0)
    typedef int wxStandardID;
#endif

static wxStandardID KeyDownAction(wxKeyEvent& event, int &selected, int selectedMax)
{
    // now, adjust position from key input
    switch (event.GetKeyCode())
    {
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            return wxID_OK;

        case WXK_ESCAPE:
            return wxID_CANCEL;

        case WXK_UP:
        case WXK_NUMPAD_UP:
            if (selected)
                selected--;
            break;

        case WXK_DOWN:
        case WXK_NUMPAD_DOWN:
            selected++;
            break;

        case WXK_PAGEUP:
        case WXK_NUMPAD_PAGEUP:
            selected -= 10;
            break;

        case WXK_PAGEDOWN:
        case WXK_NUMPAD_PAGEDOWN:
            selected += 10;
            break;

        case WXK_HOME:
            if (wxGetKeyState(WXK_CONTROL))
                selected = 0;
            else
                event.Skip();
            break;

        case WXK_END:
            if (wxGetKeyState(WXK_CONTROL))
                selected = selectedMax;
            else
                event.Skip();
            break;

        default:
            event.Skip();
            break;
    }

    // Clamp value below 0 and above Max
    if (selected < 0)
        selected = 0;
    else if (selected > selectedMax)
        selected = selectedMax;

    return wxID_LOWEST;
}

void IncrementalSelectHandler::OnKeyDown(wxKeyEvent& event)
{
    int selected = m_list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    int selectedMax = m_list->GetItemCount() - 1;

    wxStandardID result = KeyDownAction(event, selected, selectedMax);
    if (result != wxID_LOWEST)
        m_parent->EndModal(result);
    else if (selectedMax >= 0)
    {
        m_list->SetItemState(selected, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        m_list->EnsureVisible(selected);
    }
}

void IncrementalSelectHandler::OnItemActivated(wxListEvent &event)
{
    m_parent->EndModal(wxID_OK);
}

int IncrementalSelectHandler::GetSelection()
{
    int index = m_list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (index == -1)
        return wxNOT_FOUND;
    else
        return m_iterator->GetUnfilteredIndex(index);
}

IncrementalListCtrl::IncrementalListCtrl(wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size,
                                         long style, const wxValidator &validator, const wxString &name) :
    wxListCtrl(parent, winid, pos, size, style, validator, name)
{
}

wxString IncrementalListCtrl::OnGetItemText(long item, long column) const
{
    return m_Iterator->GetDisplayText(item, column);
}

void IncrementalListCtrl::SetIterator(IncrementalSelectIterator *iterator)
{
    m_Iterator = iterator;
}


IncrementalSelectArrayIterator::IncrementalSelectArrayIterator(const wxArrayString &items) :
    m_items(items), m_columnWidth(300)
{
}

int IncrementalSelectArrayIterator::GetTotalCount() const
{
    return m_items.size();
}
const wxString& IncrementalSelectArrayIterator::GetItemFilterString(int index) const
{
    return m_items[index];
}
wxString IncrementalSelectArrayIterator::GetDisplayText(int index, int column) const
{
    return m_items[m_indices[index]];
}
int IncrementalSelectArrayIterator::GetColumnWidth(int column) const
{
    return m_columnWidth;
}
void IncrementalSelectArrayIterator::CalcColumnWidth(wxListCtrl &list)
{
    m_columnWidth = 300;
}

const long ID_TEXTCTRL1 = wxNewId();
const long ID_RESULT_LIST = wxNewId();

BEGIN_EVENT_TABLE(IncrementalSelectDialog, wxDialog)
END_EVENT_TABLE()

IncrementalSelectDialog::IncrementalSelectDialog(wxWindow* parent, IncrementalSelectIterator *iterator,
                                                 const wxString &title, const wxString &message) :
    m_handler(this, iterator)
{
    BuildContent(parent, iterator, title, message);

    m_handler.Init(m_resultList, m_text);
}

IncrementalSelectDialog::~IncrementalSelectDialog()
{
    m_handler.DeInit(this);
}

void IncrementalSelectDialog::BuildContent(wxWindow* parent, IncrementalSelectIterator *iterator, const wxString &title,
                                           const wxString &message)
{
    Create(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
           wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxCLOSE_BOX|wxMAXIMIZE_BOX, _T("wxID_ANY"));
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText *labelCtrl = new wxStaticText(this, wxID_ANY, message, wxDefaultPosition, wxDefaultSize, 0,
                                               _T("wxID_ANY"));
    sizer->Add(labelCtrl, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
    m_text = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator,
                            _T("ID_TEXTCTRL1"));
    m_text->SetFocus();
    sizer->Add(m_text, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
    m_resultList = new IncrementalListCtrl(this, ID_RESULT_LIST, wxDefaultPosition, wxDefaultSize,
                                           wxLC_REPORT|wxLC_NO_HEADER|wxLC_SINGLE_SEL|wxLC_VIRTUAL|wxVSCROLL|wxHSCROLL,
                                           wxDefaultValidator, _T("ID_RESULT_LIST"));
    m_resultList->SetMinSize(wxSize(500,300));
    sizer->Add(m_resultList, 1, wxALL|wxEXPAND, 5);
    SetSizer(sizer);
    sizer->Fit(this);
    sizer->SetSizeHints(this);

    // Add first column
    wxListItem column;
    column.SetId(0);
    column.SetText( _("Column") );
    column.SetWidth(300);
    m_resultList->InsertColumn(0, column);
    m_resultList->SetIterator(iterator);
}

int IncrementalSelectDialog::GetSelection()
{
    return m_handler.GetSelection();
}
