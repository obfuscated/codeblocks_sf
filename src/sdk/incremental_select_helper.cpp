
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
