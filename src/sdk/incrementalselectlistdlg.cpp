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

    #include <algorithm>
#endif

#include "incrementalselectlistdlg.h"

BEGIN_EVENT_TABLE(IncrementalSelectListDlg, wxScrollingDialog)
    EVT_TEXT(XRCID("txtSearch"), IncrementalSelectListBase::OnSearch)
    EVT_LISTBOX_DCLICK(XRCID("lstItems"), IncrementalSelectListBase::OnSelect)
END_EVENT_TABLE()

IncrementalSelectListDlg::IncrementalSelectListDlg(wxWindow* parent, const IncrementalSelectIterator& iterator,
                                                   const wxString& caption, const wxString& message):
    IncrementalSelectListBase(parent, iterator, _T("dlgIncrementalSelectList"), caption, message)
{
    FillData();
}

IncrementalSelectListDlg::~IncrementalSelectListDlg()
{
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

void IncrementalSelectListDlg::FillData()
{
    // Stop refresh
    Freeze();

    // Get items list taking in account typed text (promote result system)
    FilterItems();

    // Show results
    FillList();

    Thaw();
}

void IncrementalSelectListDlg::GetCurrentSelection(int &selected, int &selectedMax)
{
    selected = m_List->GetSelection();
    selectedMax = m_List->GetCount() - 1;
}

void IncrementalSelectListDlg::UpdateCurrentSelection(int selected, cb_unused int selectedPrevious)
{
    m_List->SetSelection(selected);
}
