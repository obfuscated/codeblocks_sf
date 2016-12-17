
/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision: 9929 $
 * $Id: incrementalselectlistdlg.cpp 9929 2014-09-28 20:28:44Z alpha0010 $
 * $HeadURL: svn://svn.code.sf.net/p/codeblocks/code/trunk/src/sdk/incrementalselectlistdlg.cpp $
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/xrc/xmlres.h>
    #include <wx/intl.h>
    #include <wx/textctrl.h>
    #include <wx/stattext.h>
    #include "manager.h"
    #include <wx/listbox.h>

    #include <algorithm>
#endif

#include "incrementalselectlistbase.h"

BEGIN_EVENT_TABLE(IncrementalSelectListBase, wxScrollingDialog)
END_EVENT_TABLE()

IncrementalSelectListBase::IncrementalSelectListBase(wxWindow* parent, const IncrementalSelectIterator& iterator,
                                                   const wxString& dialog, const wxString& caption, const wxString& message)
    : m_List(nullptr),
    m_Iterator(iterator)
{
    wxXmlResource::Get()->LoadObject(this, parent, dialog,_T("wxScrollingDialog"));
    if (!caption.IsEmpty())
        SetTitle(caption);
    if (!message.IsEmpty())
        XRCCTRL(*this, "lblMessage", wxStaticText)->SetLabel(message);

    m_Text = XRCCTRL(*this, "txtSearch", wxTextCtrl);
    m_List = XRCCTRL(*this, "lstItems", wxListBox);

    SetSize(GetPosition().x - 90, GetPosition().y - 70, 500, 300);

    m_Text->Connect( wxEVT_KEY_DOWN,
                    (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)
                    &IncrementalSelectListBase::OnKeyDown,
                    nullptr, this );
    m_List->Connect( wxEVT_KEY_DOWN,
                    (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)
                    &IncrementalSelectListBase::OnKeyDown,
                    nullptr, this );
}

IncrementalSelectListBase::~IncrementalSelectListBase()
{
    m_Text->Disconnect( wxEVT_KEY_DOWN,
                       (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)
                       &IncrementalSelectListBase::OnKeyDown,
                       nullptr, this );
    m_List->Disconnect( wxEVT_KEY_DOWN,
                       (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)
                       &IncrementalSelectListBase::OnKeyDown,
                       nullptr, this );
}

void IncrementalSelectListBase::FilterItems()
{
    m_Result.Empty();
    m_Indexes.Empty();

    // We put a star before and after pattern to find search expression everywhere in path
    // that is: if user enter "a", it will match "123a", "12a" or "a12".
    wxString search(wxT("*") + m_Text->GetValue().Lower() + wxT("*"));

    for (int i = 0; i < m_Iterator.GetCount(); ++i)
    {
        wxString const &item = m_Iterator.GetItem(i);
        // 2 for before and after stars =~ empty string
        if ((search.Length()==2) || item.Lower().Matches(search.c_str()))
        {
            m_Result.Add(m_Iterator.GetDisplayItem(i));
            m_Indexes.Add(i);
        }
    }

    // if only alphabetical, pull word boundaries to the top
    if (search.Length() > 2)
    {
        wxString prefix;
        for (size_t i = 0; i < search.Length(); ++i)
        {
            if (wxIsalpha(search[i]))
                prefix += search[i];
        }
        if (prefix.Length() == search.Length() - 2)
        {
            std::vector<size_t> promoteIdxs;
            wxArrayString newRes;
            wxArrayLong newIndx;
            for (size_t i = 0; i < m_Result.Count(); ++i)
            {
                wxString cur = m_Result[i].Lower();
                bool promote = false;
                if (cur.StartsWith(prefix))
                    promote = true;
                else
                {
                    int maxLn = cur.Length() - prefix.Length();
                    for (int j = 0; j < maxLn; ++j)
                    {
                        if (!wxIsalpha(cur[j]) && cur.Mid(j + 1).StartsWith(prefix))
                        {
                            promote = true;
                            break;
                        }
                    }
                }
                if (promote)
                {
                    promoteIdxs.push_back(i);
                    newRes.Add(m_Result[i]);
                    newIndx.Add(m_Indexes[i]);
                }
            }
            if (!promoteIdxs.empty())
            {
                for (size_t i = 0; i < m_Result.Count(); ++i)
                {
                    if (!std::binary_search(promoteIdxs.begin(), promoteIdxs.end(), i))
                    {
                        newRes.Add(m_Result[i]);
                        newIndx.Add(m_Indexes[i]);
                    }
                }
                m_Result = newRes;
                m_Indexes = newIndx;
            }
        }
    }
    m_Promoted = search.Length()>2;

}

void IncrementalSelectListBase::FillList(void)
{
    m_List->Clear();

    if(m_Indexes.GetCount() > 0)
        m_List->Set(m_Result, reinterpret_cast<void**>(&m_Indexes[0]));

    if (m_List->GetCount() > 0)
        m_List->SetSelection(0);
}

// events

void IncrementalSelectListBase::OnSearch(cb_unused wxCommandEvent& event)
{
    FillData();
}

void IncrementalSelectListBase::OnSelect(cb_unused wxCommandEvent& event)
{
    EndModal(wxID_OK);
}

void IncrementalSelectListBase::KeyDownAction(wxKeyEvent& event, int &selected, int selectedMax)
{
    // now, adjust position from key input
    switch (event.GetKeyCode())
    {
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            EndModal(wxID_OK);
            return;
            break;

        case WXK_ESCAPE:
            EndModal(wxID_CANCEL);
            return;
            break;

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
}

void IncrementalSelectListBase::OnKeyDown(wxKeyEvent& event)
{
    int selected = 0, selectedMax = 0;

    GetCurrentSelection(selected, selectedMax);
    if (selected == wxNOT_FOUND)
        selected = 0;

    // remember previous selection
    int selectedPrevious = selected;
    KeyDownAction(event, selected, selectedMax);
    if (selectedMax < 0)
        return;
    UpdateCurrentSelection(selected, selectedPrevious);
}
