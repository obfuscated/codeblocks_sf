#include "include/sdk.h"
#include "goto_file.h"

#ifndef WX_PRECOMP
    //(*InternalHeadersPCH(GotoFile)
    #include <wx/sizer.h>
    #include <wx/listctrl.h>
    #include <wx/string.h>
    #include <wx/intl.h>
    #include <wx/stattext.h>
    #include <wx/textctrl.h>
    //*)
#endif
//(*InternalHeaders(GotoFile)
//*)

//(*IdInit(GotoFile)
const long GotoFile::ID_TEXTCTRL1 = wxNewId();
const long GotoFile::ID_RESULT_LIST = wxNewId();
//*)

GotoHandler::GotoHandler(wxDialog* parent, GotoFileIterator *iterator) :
    m_parent(parent),
    m_list(nullptr),
    m_text(nullptr),
    m_iterator(iterator)
{
    m_parent->PushEventHandler(this);
    SetEvtHandlerEnabled(true);
}

GotoHandler::~GotoHandler()
{
}

void GotoHandler::Init(wxListCtrl *list, wxTextCtrl *text)
{
    m_list = list;
    m_text = text;

    m_text->Connect(wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&GotoHandler::OnTextChanged, nullptr, this);
    m_text->Connect(wxEVT_KEY_DOWN, (wxObjectEventFunction)&GotoHandler::OnKeyDown, nullptr, this);
    m_list->Connect(wxEVT_KEY_DOWN, (wxObjectEventFunction)&GotoHandler::OnKeyDown, nullptr, this);

    FilterItems();
}

void GotoHandler::DeInit(wxWindow *window)
{
    m_text->Disconnect(wxEVT_KEY_DOWN, (wxObjectEventFunction)&GotoHandler::OnKeyDown, nullptr, this);
    m_list->Disconnect(wxEVT_KEY_DOWN, (wxObjectEventFunction)&GotoHandler::OnKeyDown, nullptr, this);
    m_text->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&GotoHandler::OnTextChanged, nullptr, this);

    SetEvtHandlerEnabled(false);
    window->RemoveEventHandler(this);
}

void GotoHandler::OnTextChanged(wxCommandEvent& event)
{
    FilterItems();
    event.Skip();
}

void GotoHandler::FilterItems()
{
    m_iterator->Reset();

    const wxString &inputPattern = m_text->GetValue().Lower();
    if (inputPattern.empty())
    {
        int count = m_iterator->GetTotalCount();
        for (int ii = 0; ii < count; ++ii)
            m_iterator->AddIndex(ii);

        m_list->SetItemCount(m_iterator->GetFilteredCount());
        if (m_iterator->GetFilteredCount() > 0)
            m_list->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
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

    m_list->SetItemCount(m_iterator->GetFilteredCount());
    if (m_iterator->GetFilteredCount() > 0)
        m_list->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

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
    else
    {
        if (selected > selectedMax)
            selected = selectedMax;
    }
    return wxID_LOWEST;
}

void GotoHandler::OnKeyDown(wxKeyEvent& event)
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


class GotoFileListCtrl : public wxListCtrl
{
    public:
        GotoFileListCtrl(wxWindow *parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize, long style = wxLC_ICON,
                         const wxValidator &validator = wxDefaultValidator, const wxString &name = wxListCtrlNameStr) :
            wxListCtrl(parent, winid, pos, size, style, validator, name)
        {
        }

        wxString OnGetItemText(long item, long column) const override
        {
            //return F(wxT("Item: %ld-%ld"), item, column);
            return m_Iterator->GetDisplayText(item, column);
        }

        void SetIterator(GotoFileIterator *iterator)
        {
            m_Iterator = iterator;
        }
    private:
        GotoFileIterator *m_Iterator;
};

BEGIN_EVENT_TABLE(GotoFile,wxDialog)
    //(*EventTable(GotoFile)
    //*)
END_EVENT_TABLE()

GotoFile::GotoFile(wxWindow* parent, GotoFileIterator *iterator) :
    m_handler(this, iterator)
{
    BuildContent(parent, iterator);

    m_handler.Init(m_ResultList, m_Text);
}

void GotoFile::BuildContent(wxWindow* parent, GotoFileIterator *iterator)
{
    //(*Initialize(GotoFile)
    wxBoxSizer* BoxSizer1;
    wxStaticText* StaticText1;

    Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    StaticText1 = new wxStaticText(this, wxID_ANY, _("Label"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    BoxSizer1->Add(StaticText1, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
    m_Text = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer1->Add(m_Text, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
    m_ResultList = new GotoFileListCtrl(this, ID_RESULT_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_NO_HEADER|wxLC_SINGLE_SEL|wxLC_VIRTUAL|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_RESULT_LIST"));
    BoxSizer1->Add(m_ResultList, 1, wxALL|wxEXPAND, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);
    //*)

    // Add first column
    wxListItem column;
    column.SetId(0);
    column.SetText( _("Column") );
    column.SetWidth(300);
    m_ResultList->InsertColumn(0, column);
    m_ResultList->SetIterator(iterator);
}

GotoFile::~GotoFile()
{
    m_handler.DeInit(this);

    //(*Destroy(GotoFile)
    //*)
}
