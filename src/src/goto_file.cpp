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
    m_iterator(iterator)
{
    BuildContent(parent);

	//m_ResultList->SetItemCount(30000);
	FilterItems();
}

void GotoFile::BuildContent(wxWindow* parent)
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
    m_ResultList = new GotoFileListCtrl(this, ID_RESULT_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_NO_HEADER|wxLC_VIRTUAL|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_RESULT_LIST"));
    BoxSizer1->Add(m_ResultList, 1, wxALL|wxEXPAND, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GotoFile::OnTextChanged);
    //*)

    // Add first column
    wxListItem column;
    column.SetId(0);
    column.SetText( _("Column") );
    column.SetWidth(300);
    m_ResultList->InsertColumn(0, column);
    m_ResultList->SetIterator(m_iterator);
}

GotoFile::~GotoFile()
{
    //(*Destroy(GotoFile)
    //*)
}

void GotoFile::FilterItems()
{
    m_iterator->Reset();

    const wxString &inputPattern = m_Text->GetValue().Lower();
    if (inputPattern.empty())
    {
        int count = m_iterator->GetTotalCount();
        for (int ii = 0; ii < count; ++ii)
            m_iterator->AddIndex(ii);

        m_ResultList->SetItemCount(m_iterator->GetFilteredCount());
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

    m_ResultList->SetItemCount(m_iterator->GetFilteredCount());
}

void GotoFile::OnTextChanged(wxCommandEvent& event)
{
    FilterItems();
}
