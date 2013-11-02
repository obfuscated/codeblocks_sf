/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/xrc/xmlres.h>
    #include <wx/intl.h>
    #include <wx/textctrl.h>
    #include <wx/stattext.h>
    #include "manager.h"
    #include <wx/listbox.h>
#endif

#include "incrementalselectlistdlg.h"



BEGIN_EVENT_TABLE(IncrementalSelectListDlg, wxScrollingDialog)
    EVT_TEXT(XRCID("txtSearch"), IncrementalSelectListDlg::OnSearch)
    EVT_LISTBOX_DCLICK(XRCID("lstItems"), IncrementalSelectListDlg::OnSelect)
END_EVENT_TABLE()

IncrementalSelectListDlg::IncrementalSelectListDlg(wxWindow* parent, const IncrementalSelectIterator& iterator,
                                                   const wxString& caption, const wxString& message)
    : m_List(nullptr),
    m_Text(nullptr),
    m_Iterator(iterator)
{
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgIncrementalSelectList"),_T("wxScrollingDialog"));
    if (!caption.IsEmpty())
        SetTitle(caption);
    if (!message.IsEmpty())
        XRCCTRL(*this, "lblMessage", wxStaticText)->SetLabel(message);

    m_Text = XRCCTRL(*this, "txtSearch", wxTextCtrl);
    m_List = XRCCTRL(*this, "lstItems", wxListBox);

    SetSize(GetPosition().x - 90, GetPosition().y - 70, 500, 300);

    m_Text->Connect( wxEVT_KEY_DOWN,
                    (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)
                    &IncrementalSelectListDlg::OnKeyDown,
                    nullptr, this );
    m_List->Connect( wxEVT_KEY_DOWN,
                    (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)
                    &IncrementalSelectListDlg::OnKeyDown,
                    nullptr, this );

    FillList();
}

IncrementalSelectListDlg::~IncrementalSelectListDlg()
{
    m_Text->Disconnect( wxEVT_KEY_DOWN,
                       (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)
                       &IncrementalSelectListDlg::OnKeyDown,
                       nullptr, this );
    m_List->Disconnect( wxEVT_KEY_DOWN,
                       (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)
                       &IncrementalSelectListDlg::OnKeyDown,
                       nullptr, this );
}

wxString IncrementalSelectListDlg::GetStringSelection()
{
    return m_List->GetStringSelection();
}

wxIntPtr IncrementalSelectListDlg::GetSelection()
{
    int selection = m_List->GetSelection();
    if (selection == wxNOT_FOUND)
        return wxNOT_FOUND;

    return reinterpret_cast<wxIntPtr>(m_List->GetClientData(selection));
}

void IncrementalSelectListDlg::FillList()
{
    Freeze();

    // We put a star before and after pattern to find search expression everywhere in path
    wxString search(wxT("*") + m_Text->GetValue().Lower() + wxT("*"));

    wxArrayString result;
    wxArrayLong indexes;

    m_List->Clear();
    for (int i = 0; i < m_Iterator.GetCount(); ++i)
    {
        wxString const &item = m_Iterator.GetItem(i);
        // 2 for before and after stars =~ empty string
        if ((search.Length()==2) || item.Lower().Matches(search.c_str()))
        {
            result.Add(m_Iterator.GetDisplayItem(i));
            indexes.Add(i);
        }
    }
    m_List->Set(result, reinterpret_cast<void**>(&indexes[0]));
    if (m_List->GetCount())
        m_List->SetSelection(0);

    Thaw();
}

// events

void IncrementalSelectListDlg::OnSearch(cb_unused wxCommandEvent& event)
{
    FillList();
}

void IncrementalSelectListDlg::OnSelect(cb_unused wxCommandEvent& event)
{
    EndModal(wxID_OK);
}

void IncrementalSelectListDlg::OnKeyDown(wxKeyEvent& event)
{
    //Manager::Get()->GetLogManager()->Log(mltDevDebug, "OnKeyDown");
    size_t sel = 0;
    switch (event.GetKeyCode())
    {
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            EndModal(wxID_OK);
            break;

        case WXK_ESCAPE:
            EndModal(wxID_CANCEL);
            break;

        case WXK_UP:
        case WXK_NUMPAD_UP:
            sel = m_List->GetSelection() - 1;
            m_List->SetSelection(sel == (size_t) -1 ? 0 : sel);
            break;

        case WXK_DOWN:
        case WXK_NUMPAD_DOWN:
            m_List->SetSelection(m_List->GetSelection() + 1);
            break;

        case WXK_PAGEUP:
        case WXK_NUMPAD_PAGEUP:
            sel = m_List->GetSelection() - 10;
            m_List->SetSelection(sel > m_List->GetCount() ? 0 : sel);
            break;

        case WXK_PAGEDOWN:
        case WXK_NUMPAD_PAGEDOWN:
            sel = m_List->GetSelection() + 10;
            m_List->SetSelection(sel >= m_List->GetCount() ? m_List->GetCount() - 1 : sel);
            break;

        case WXK_HOME:
            if (wxGetKeyState(WXK_CONTROL))
                m_List->SetSelection(0);
            else
                event.Skip();
            break;

        case WXK_END:
            if (wxGetKeyState(WXK_CONTROL))
                m_List->SetSelection(m_List->GetCount() - 1);
            else
                event.Skip();
            break;

        default:
            event.Skip();
            break;
    }
}
