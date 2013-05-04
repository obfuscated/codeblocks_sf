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


BEGIN_EVENT_TABLE(myHandler, wxEvtHandler)
END_EVENT_TABLE()

void myHandler::OnKeyDown(wxKeyEvent& event)
{
    //Manager::Get()->GetLogManager()->Log(mltDevDebug, "OnKeyDown");
    size_t sel = 0;
    switch (event.GetKeyCode())
    {
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            m_pParent->EndModal(wxID_OK);
            break;

        case WXK_ESCAPE:
            m_pParent->EndModal(wxID_CANCEL);
            break;

        case WXK_UP:
        case WXK_NUMPAD_UP:
            sel = m_pList->GetSelection() - 1;
            m_pList->SetSelection(sel == (size_t) -1 ? 0 : sel);
            break;

        case WXK_DOWN:
        case WXK_NUMPAD_DOWN:
            m_pList->SetSelection(m_pList->GetSelection() + 1);
            break;

        case WXK_PAGEUP:
        case WXK_NUMPAD_PAGEUP:
            sel = m_pList->GetSelection() - 10;
            m_pList->SetSelection(sel > m_pList->GetCount() ? 0 : sel);
            break;

        case WXK_PAGEDOWN:
        case WXK_NUMPAD_PAGEDOWN:
            sel = m_pList->GetSelection() + 10;
            m_pList->SetSelection(sel >= m_pList->GetCount() ? m_pList->GetCount() - 1 : sel);
            break;

        case WXK_HOME:
            if (wxGetKeyState(WXK_CONTROL))
                m_pList->SetSelection(0);
            else
                event.Skip();
            break;

        case WXK_END:
            if (wxGetKeyState(WXK_CONTROL))
                m_pList->SetSelection(m_pList->GetCount() - 1);
            else
                event.Skip();
            break;

        default:
            event.Skip();
            break;
    }
}

BEGIN_EVENT_TABLE(IncrementalSelectListDlg, wxScrollingDialog)
    EVT_TEXT(XRCID("txtSearch"), IncrementalSelectListDlg::OnSearch)
    EVT_LISTBOX_DCLICK(XRCID("lstItems"), IncrementalSelectListDlg::OnSelect)
END_EVENT_TABLE()

IncrementalSelectListDlg::IncrementalSelectListDlg(wxWindow* parent, const IncrementalSelectIterator& iterator,
                                                   const wxString& caption, const wxString& message)
    : m_pMyEvtHandler(0L),
    m_List(0L),
    m_Text(0L),
    m_Iterator(iterator)
{
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgIncrementalSelectList"),_T("wxScrollingDialog"));
    if (!caption.IsEmpty())
        SetTitle(caption);
    if (!message.IsEmpty())
        XRCCTRL(*this, "lblMessage", wxStaticText)->SetLabel(message);

    m_Text = XRCCTRL(*this, "txtSearch", wxTextCtrl);
    m_List = XRCCTRL(*this, "lstItems", wxListBox);

    m_pMyEvtHandler = new myHandler(this, m_Text, m_List);
    m_Text->SetNextHandler(m_pMyEvtHandler);
    m_List->SetNextHandler(m_pMyEvtHandler);

    FillList();
}

IncrementalSelectListDlg::~IncrementalSelectListDlg()
{
    m_Text->SetNextHandler(0L);
    m_List->SetNextHandler(0L);

    delete m_pMyEvtHandler;
}

wxString IncrementalSelectListDlg::GetStringSelection()
{
    return m_List->GetStringSelection();
}

long IncrementalSelectListDlg::GetSelection()
{
    int selection = m_List->GetSelection();
    if (selection == wxNOT_FOUND)
        return wxNOT_FOUND;

    #if defined(_WIN64)
    return reinterpret_cast<long long>(m_List->GetClientData(selection));
    #else
    return reinterpret_cast<long>(m_List->GetClientData(selection));
    #endif
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
