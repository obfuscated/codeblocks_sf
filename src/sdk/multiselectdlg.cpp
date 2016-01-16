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
    #include <wx/checklst.h>
    #include <wx/msgdlg.h>
    #include <wx/stattext.h>
    #include <wx/xrc/xmlres.h>
    #include "globals.h"
#endif

#include <wx/textdlg.h>

#include "multiselectdlg.h"

BEGIN_EVENT_TABLE(MultiSelectDlg, wxScrollingDialog)
    EVT_CHECKLISTBOX(XRCID("lstItems"), MultiSelectDlg::OnItemToggle)
    EVT_BUTTON(XRCID("btnSelectWild"), MultiSelectDlg::OnWildcard)
    EVT_BUTTON(XRCID("btnToggle"), MultiSelectDlg::OnToggle)
    EVT_BUTTON(XRCID("btnSelectAll"), MultiSelectDlg::OnSelectAll)
    EVT_BUTTON(XRCID("btnDeselectAll"), MultiSelectDlg::OnDeselectAll)
END_EVENT_TABLE()

MultiSelectDlg::MultiSelectDlg(wxWindow* parent,
                                const wxArrayString& items,
                                const wxString& wildcard,
                                const wxString& label,
                                const wxString& title)
{
    //ctor
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgGenericMultiSelect"),_T("wxScrollingDialog"));

    SetTitle(title);
    XRCCTRL(*this, "lblLabel", wxStaticText)->SetLabel(label);
    Init(items, wildcard);
}

MultiSelectDlg::MultiSelectDlg(wxWindow* parent,
                                const wxArrayString& items,
                                bool selectall,
                                const wxString& label,
                                const wxString& title)
{
    //ctor
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgGenericMultiSelect"),_T("wxScrollingDialog"));

    SetTitle(title);
    XRCCTRL(*this, "lblLabel", wxStaticText)->SetLabel(label);
    Init(items, selectall ? _T("*") : _T(""));
}

MultiSelectDlg::~MultiSelectDlg()
{
    //dtor
}

void MultiSelectDlg::Init(const wxArrayString& items, const wxString& wildcard)
{
    wxCheckListBox* lst = XRCCTRL(*this, "lstItems", wxCheckListBox);
    for (size_t i = 0; i < items.GetCount(); ++i)
        lst->Append(items[i]);

    SelectWildCard(wildcard);
}

void MultiSelectDlg::UpdateStatus()
{
    int count = 0;
    wxCheckListBox* lst = XRCCTRL(*this, "lstItems", wxCheckListBox);
    for (size_t i = 0; i < lst->GetCount(); ++i)
    {
        if (lst->IsChecked(i))
            ++count;
    }
    wxString msg;
    msg << _("Selected: ") << wxString::Format(_T("%d"), count);
    XRCCTRL(*this, "lblStatus", wxStaticText)->SetLabel(msg);
}

wxArrayString MultiSelectDlg::GetSelectedStrings() const
{
    wxArrayString ret;
    wxCheckListBox* lst = XRCCTRL(*this, "lstItems", wxCheckListBox);
    for (size_t i = 0; i < lst->GetCount(); ++i)
    {
        if (lst->IsChecked(i))
            ret.Add(lst->GetString(i));
    }
    return ret;
}

wxArrayInt MultiSelectDlg::GetSelectedIndices() const
{
    wxArrayInt ret;
    wxCheckListBox* lst = XRCCTRL(*this, "lstItems", wxCheckListBox);
    for (size_t i = 0; i < lst->GetCount(); ++i)
    {
        if (lst->IsChecked(i))
            ret.Add(i);
    }
    return ret;
}

void MultiSelectDlg::SelectWildCard(const wxString& wild, bool select, bool clearOld)
{
    if (wild.IsEmpty())
        return;
    wxArrayString wilds = GetArrayFromString(wild, _T(";"));
    wxCheckListBox* lst = XRCCTRL(*this, "lstItems", wxCheckListBox);
    for (size_t i = 0; i < lst->GetCount(); ++i)
    {
        if (clearOld || !lst->IsChecked(i))
        {
            wxString entry = lst->GetString(i).Lower();
            bool MatchesWildCard = false;
            for (unsigned int x = 0; x < wilds.GetCount(); ++x)
            {
                if (entry.Matches(wilds[x].Lower()))
                {
                    lst->Check(i, select);
                    MatchesWildCard = true;
                    break;
                }
            }
            // did not match the wildcard and was selected ( == in the old list)
            // and we want those to be removed (clearOld) -> uncheck
            if (!MatchesWildCard && clearOld && lst->IsChecked(i))
                lst->Check(i, false);
        }
    }
    UpdateStatus();
}

void MultiSelectDlg::OnWildcard(cb_unused wxCommandEvent& event)
{
    wxString wild = wxGetTextFromUser(_("Enter a selection wildcard\n(e.g. \"dlg*.cpp\" "
                                        "would select all files starting with \"dlg\" and "
                                        "ending in \".cpp\")\nSeparate multiple masks with \";\":"),
                                        _("Wildcard selection"));
    if (wild.IsEmpty())
        return;

    // Do not ask to un-select before if there are no items selected
    bool ask_clear = false;
    wxCheckListBox* lst = XRCCTRL(*this, "lstItems", wxCheckListBox);
    for (size_t i = 0; i < lst->GetCount(); ++i)
    {
        if (lst->IsChecked(i))
        {
            ask_clear = true;
            break;
        }
    }

    bool clear = false;
    if (ask_clear)
        clear = cbMessageBox(_("Do you want to clear the previous selections?"),
                             _("Question"),
                             wxICON_QUESTION | wxYES_NO, this) == wxID_YES;
    SelectWildCard(wild, true, clear);
}

void MultiSelectDlg::OnItemToggle(cb_unused wxCommandEvent& event)
{
    UpdateStatus();
}

void MultiSelectDlg::OnToggle(cb_unused wxCommandEvent& event)
{
    wxCheckListBox* lst = XRCCTRL(*this, "lstItems", wxCheckListBox);
    for (size_t i = 0; i < lst->GetCount(); ++i)
        lst->Check(i, !lst->IsChecked(i));

    UpdateStatus();
}

void MultiSelectDlg::OnSelectAll(cb_unused wxCommandEvent& event)
{
    wxCheckListBox* lst = XRCCTRL(*this, "lstItems", wxCheckListBox);
    for (size_t i = 0; i < lst->GetCount(); ++i)
        lst->Check(i, true);

    UpdateStatus();
}

void MultiSelectDlg::OnDeselectAll(cb_unused wxCommandEvent& event)
{
    wxCheckListBox* lst = XRCCTRL(*this, "lstItems", wxCheckListBox);
    for (size_t i = 0; i < lst->GetCount(); ++i)
        lst->Check(i, false);

    UpdateStatus();
}
