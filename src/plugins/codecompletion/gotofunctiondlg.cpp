/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include "sdk.h"
#include "gotofunctiondlg.h"

#ifndef CB_PRECOMP
    //(*InternalHeadersPCH(GotoFunctionDlg)
    #include <wx/checkbox.h>
    #include <wx/sizer.h>
    #include <wx/listctrl.h>
    #include <wx/string.h>
    #include <wx/intl.h>
    #include <wx/stattext.h>
    #include <wx/textctrl.h>
    //*)

    #include "configmanager.h"
    #include <algorithm>
#endif
//(*InternalHeaders(GotoFunctionDlg)
//*)

//(*IdInit(GotoFunctionDlg)
const long GotoFunctionDlg::ID_CHECKBOX1 = wxNewId();
const long GotoFunctionDlg::ID_TEXTCTRL1 = wxNewId();
const long GotoFunctionDlg::ID_LISTCTRL1 = wxNewId();
//*)

GotoFunctionDlg::Iterator::Iterator() : m_columnLength{ 300, 100, 300 }, m_columnMode(false)
{
}

void GotoFunctionDlg::Iterator::AddToken(const FunctionToken &token)
{
    m_tokens.push_back(token);
}

auto GotoFunctionDlg::Iterator::GetToken(int index) const -> const FunctionToken*
{
    if (index >= 0 && index < int(m_tokens.size()))
        return &m_tokens[index];
    else
        return nullptr;
}

int GotoFunctionDlg::Iterator::GetTotalCount() const
{
    return m_tokens.size();
}

const wxString& GotoFunctionDlg::Iterator::GetItemFilterString(int index) const
{
    return m_tokens[index].displayName;
}

wxString GotoFunctionDlg::Iterator::GetDisplayText(int index, int column) const
{
    if (m_columnMode)
    {
        const FunctionToken &t = m_tokens[m_indices[index]];
        switch (column)
        {
        case 0:
            return t.funcName;
        case 1:
            return t.paramsAndreturnType;

        default:
            return wxT("<invalid>");
        }
    }
    else
        return m_tokens[m_indices[index]].displayName;
}

void GotoFunctionDlg::Iterator::SetColumnMode(bool flag)
{
    m_columnMode = flag;
}

void GotoFunctionDlg::Iterator::CalcColumnWidth(wxListCtrl &list)
{
    m_columnLength[0] = m_columnLength[1] = m_columnLength[2] = 0;

    for (const auto &t : m_tokens)
    {
        m_columnLength[0] = std::max<int>(m_columnLength[0], t.displayName.length());
        m_columnLength[1] = std::max<int>(m_columnLength[1], t.funcName.length());
        m_columnLength[2] = std::max<int>(m_columnLength[2], t.paramsAndreturnType.length());
    }

    for (int ii = 0; ii < 3; ++ii)
    {
        int x, y;
        list.GetTextExtent(wxString(wxT('A'), m_columnLength[ii]), &x, &y);
        m_columnLength[ii] = x;
    }
}

int GotoFunctionDlg::Iterator::GetColumnWidth(int column) const
{
    if (m_columnMode)
        return m_columnLength[column + 1];
    else
        return m_columnLength[0];
}

void GotoFunctionDlg::Iterator::Sort()
{
    std::sort(m_tokens.begin(), m_tokens.end(), [] (const FunctionToken &a, const FunctionToken &b) {
        return a.funcName.CmpNoCase(b.funcName)<0;
    });
}

class GotoFunctionListCtrl : public wxListCtrl
{
    public:
        GotoFunctionListCtrl(wxWindow *parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                             const wxSize& size = wxDefaultSize, long style = wxLC_ICON,
                             const wxValidator &validator = wxDefaultValidator,
                             const wxString &name = wxListCtrlNameStr) :
            wxListCtrl(parent, winid, pos, size, style, validator, name)
        {
        }

        wxString OnGetItemText(long item, long column) const override
        {
            //return F(wxT("Item: %ld-%ld"), item, column);
            return m_Iterator->GetDisplayText(item, column);
        }

        void SetIterator(IncrementalSelectIterator *iterator)
        {
            m_Iterator = iterator;
        }
    private:
        IncrementalSelectIterator *m_Iterator;
};


BEGIN_EVENT_TABLE(GotoFunctionDlg, wxDialog)
    //(*EventTable(GotoFunctionDlg)
    //*)
END_EVENT_TABLE()

GotoFunctionDlg::GotoFunctionDlg(wxWindow* parent, Iterator* iterator) :
    m_handler(this, iterator),
    m_iterator(iterator)
{
    BuildContent(parent, iterator);
}

void GotoFunctionDlg::BuildContent(wxWindow* parent, Iterator* iterator)
{
    //(*Initialize(GotoFunctionDlg)
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer2;
    wxStaticText* StaticText1;

    Create(parent, wxID_ANY, _("Select function..."), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxCLOSE_BOX|wxMAXIMIZE_BOX, _T("wxID_ANY"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    StaticText1 = new wxStaticText(this, wxID_ANY, _("Please select function to go to:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    BoxSizer2->Add(StaticText1, 0, wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(-1,-1,1, wxALL|wxEXPAND, 5);
    m_mode = new wxCheckBox(this, ID_CHECKBOX1, _("Column Mode"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    m_mode->SetValue(false);
    BoxSizer2->Add(m_mode, 0, wxEXPAND, 5);
    BoxSizer1->Add(BoxSizer2, 0, wxLEFT|wxRIGHT|wxEXPAND, 8);
    m_text = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    m_text->SetFocus();
    BoxSizer1->Add(m_text, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
    m_list = new GotoFunctionListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VIRTUAL|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_LISTCTRL1"));
    m_list->SetMinSize(wxSize(500,300));
    BoxSizer1->Add(m_list, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&GotoFunctionDlg::OnModeClick);
    //*)

    if (Manager::Get()->GetConfigManager(_T("code_completion"))->ReadBool(_T("goto_function_window/column_mode")))
        m_mode->SetValue(true);

    m_list->SetIterator(iterator);
    SwitchMode();
    m_handler.Init(m_list, m_text);
}

GotoFunctionDlg::~GotoFunctionDlg()
{
    m_handler.DeInit(this);

    //(*Destroy(GotoFunctionDlg)
    //*)
}

void GotoFunctionDlg::SwitchMode()
{
    bool columnMode = m_mode->IsChecked();
    m_iterator->SetColumnMode(columnMode);

    while (m_list->GetColumnCount() > 0)
        m_list->DeleteColumn(0);

    if (columnMode)
    {
        m_list->SetWindowStyleFlag(m_list->GetWindowStyleFlag() & ~wxLC_NO_HEADER);

        m_list->InsertColumn(0, _("Function name"), wxLIST_FORMAT_LEFT, m_iterator->GetColumnWidth(0));
        m_list->InsertColumn(1, _("Parameters and return type"), wxLIST_FORMAT_LEFT, m_iterator->GetColumnWidth(1));
    }
    else
    {
        m_list->SetWindowStyleFlag(m_list->GetWindowStyleFlag() | wxLC_NO_HEADER);

        m_list->InsertColumn(0, _("Column"), wxLIST_FORMAT_LEFT, m_iterator->GetColumnWidth(0));
    }
}

void GotoFunctionDlg::OnModeClick(wxCommandEvent& event)
{
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
    cfg->Write(_T("goto_function_window/column_mode"), m_mode->IsChecked());

    SwitchMode();
    m_handler.FilterItems();

    event.Skip();
}

int GotoFunctionDlg::GetSelection()
{
    return m_handler.GetSelection();
}
