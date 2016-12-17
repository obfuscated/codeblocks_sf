/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/listbox.h>
#include "projectmanager.h"
#include "configmanager.h"

#include <algorithm>

#include "gotofunctiondlg.h"

BEGIN_EVENT_TABLE(GotoFunctionDlg, wxScrollingDialog)
    EVT_TEXT(XRCID("txtSearch"), IncrementalSelectListBase::OnSearch)
    EVT_LISTBOX_DCLICK(XRCID("lstItems"), IncrementalSelectListBase::OnSelect)
    EVT_LIST_ITEM_ACTIVATED(XRCID("lstCtrlItems"), GotoFunctionDlg::OnColumnSelect)
    EVT_CHECKBOX(XRCID("modeSelect"),GotoFunctionDlg::OnModeChange)
END_EVENT_TABLE()

GotoFunctionDlg::GotoFunctionDlg(wxWindow* parent, const IncrementalSelectIterator& iterator):
    IncrementalSelectListBase(parent, iterator, _T("dlgGotoFunction"), _("Select function..."), _("Please select function to go to:")),
    m_ListColumn(nullptr),
    m_Mode(nullptr)
{
    m_Mode = XRCCTRL(*this, "modeSelect", wxCheckBox);
    m_ListColumn = XRCCTRL(*this, "lstCtrlItems", wxListCtrl);

    m_Mode->SetLabel(_("Column mode"));
    if (Manager::Get()->GetConfigManager(_T("code_completion"))->ReadBool(_T("goto_function_window/column_mode")))
        m_Mode->Set3StateValue(wxCHK_CHECKED);

    m_ListColumn->Connect( wxEVT_KEY_DOWN,
                    (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)
                    &IncrementalSelectListBase::OnKeyDown,
                    nullptr, this );
    // Add colum to column mode
    m_ListColumn->InsertColumn(0, _(""), wxLIST_FORMAT_LEFT);
    m_ListColumn->InsertColumn(1, _("Function name"), wxLIST_FORMAT_LEFT);
    m_ListColumn->InsertColumn(2, _("Parameters"), wxLIST_FORMAT_LEFT);
    // Setup for first use
    SetupMode();
}

GotoFunctionDlg::~GotoFunctionDlg()
{
    m_ListColumn->Disconnect( wxEVT_KEY_DOWN,
                       (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)
                       &GotoFunctionDlg::OnKeyDown,
                       nullptr, this );
}

void GotoFunctionDlg::SetupMode()
{
    // What mode should we use ?
    if (m_Mode->GetValue())
    {
        // Column mode, so hide m_List and show m_ListColumn
        m_List->Hide();
        m_ListColumn->Show();
    }
    else
    {
        // List mode
        m_ListColumn->Hide();
        m_List->Show();
    }
    // Force new layout
    Layout();
    // Fill with data
    FillData();
    // Force focus to text entry
    m_Text->SetFocus();
}

wxListCtrl* GotoFunctionDlg::GetListCtrl()
{
    return m_ListColumn;
}

wxIntPtr GotoFunctionDlg::GetSelection()
{
    int selection;

    if (m_Mode->GetValue())
    {
        // Colum Mode
        selection = m_ListColumn->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

        if (selection == wxNOT_FOUND)
            return wxNOT_FOUND;
        return (wxIntPtr)(m_ListColumn->GetItemData(selection));
    }
    else
    {
        //List Mode
        selection = m_List->GetSelection();

        if (selection == wxNOT_FOUND)
            return wxNOT_FOUND;
        return reinterpret_cast<wxIntPtr>(m_List->GetClientData(selection));
    }
}

inline int wxCALLBACK SortAscending(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
    // Just compare function names
    GotoFunctionDlg *dialog = (GotoFunctionDlg*) sortData;
    wxListCtrl *listCtrl = dialog->GetListCtrl();
    // Identify 2 items
    long itemId1 = listCtrl->FindItem(-1, item1);
    long itemId2 = listCtrl->FindItem(-1, item2);

    wxListItem listItem1, listItem2;

    // Grab first one
    listItem1.SetId(itemId1);
    listItem1.SetColumn(1);
    listItem1.SetMask(wxLIST_MASK_TEXT);
    listCtrl->GetItem(listItem1);

    // Grab second one
    listItem2.SetId(itemId2);
    listItem2.SetColumn(1);
    listItem2.SetMask(wxLIST_MASK_TEXT);
    listCtrl->GetItem(listItem2);

    // Compare
    return wxStricmp(listItem1.GetText(), listItem2.GetText());
}

void GotoFunctionDlg::FillColumn()
{
    // Remove all previous
    m_ListColumn->DeleteAllItems();
    for (size_t i = 0; i < m_Result.Count(); ++i)
    {
        int parametersStart;
        int functionStart;

        // Example :
        // __declspec(dllexport) int xxxx(int a,void (*b)(int c,unsigned long d),unsigned long e)
        //
        // Search for parameters list start : first parenthesis from end counting braces
        // It won't work for default string parameters/character with brace in
        int braceCount = 0;
        parametersStart = 0;
        for (int j = m_Result[i].Len()-1; j >=0 ; j--)
        {
            // New nested
            if (m_Result[i][j] == _T(')'))
                braceCount++;
            else
            {
                if (m_Result[i][j] == _T('('))
                {
                    if (!--braceCount)
                    {
                        // End of nested
                        parametersStart = j;
                        break;
                    }
                }
            }
        }
        // Search function name start : void dummy(int param);
        functionStart = m_Result[i].rfind(_T(' '),parametersStart);
        if (parametersStart == wxNOT_FOUND)
        {
            if (functionStart == wxNOT_FOUND)
            {
                // Nothing found, just put in function name
                m_ListColumn->InsertItem(i, _T(""));
                m_ListColumn->SetItem(i, 1, m_Result[i]);
            }
            else
            {
                // Fonction and return found
                m_ListColumn->InsertItem(i, m_Result[i].Left(functionStart));
                m_ListColumn->SetItem(i, 1, m_Result[i].Mid(functionStart + 1));
            }
        }
        else
        {
            if (functionStart == wxNOT_FOUND)
            {
                // Function and parameters found
                m_ListColumn->InsertItem(i,  _T(""));
                m_ListColumn->SetItem(i, 2, m_Result[i].Mid(parametersStart));
                m_ListColumn->SetItem(i, 1, m_Result[i].Left(parametersStart));
            }
            else
            {
                // Fonction, return and parameters found
                m_ListColumn->InsertItem(i, m_Result[i].Left(functionStart));
                m_ListColumn->SetItem(i, 2, m_Result[i].Mid(parametersStart));
                m_ListColumn->SetItem(i, 1, m_Result[i].Mid(functionStart + 1, parametersStart - functionStart - 1));
            }
        }
        m_ListColumn->SetItemData(i, m_Indexes[i]);
    }
    // Sort item only if no one has been promoted (text typed on search boc)
    if (!m_Promoted)
        m_ListColumn->SortItems(SortAscending,(wxIntPtr)this);

    // AutoFit columns
    int count = m_ListColumn->GetColumnCount();
    for (int j = 0; j < count; ++j)
        m_ListColumn->SetColumnWidth(j, wxLIST_AUTOSIZE);

}

void GotoFunctionDlg::FillData()
{
    // Stop refresh
    Freeze();

    // Get items list taking in account typed text (promote result system)
    FilterItems();

    // Now fill control
    if (m_Mode->GetValue())
        FillColumn();
    else
        FillList();

    // Restart refresh
    Thaw();
}

void GotoFunctionDlg::GetCurrentSelection(int &selected, int &selectedMax)
{
    // Get current selection and last possible one
    if (m_Mode->GetValue())
    {
        // We are on column mode
        selected = m_ListColumn->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        selectedMax = m_ListColumn->GetItemCount() - 1;
    }
    else
    {
        // We are on list mode
        selected = m_List->GetSelection();
        selectedMax = static_cast<int>(m_List->GetCount()) - 1;
    }
}

void GotoFunctionDlg::UpdateCurrentSelection(int selected, int selectedPrevious)
{
    // Apply new value
    if (m_Mode->GetValue())
    {
        // We are on column mode
        m_ListColumn->SetItemState(selectedPrevious, 0, wxLIST_STATE_SELECTED);
        m_ListColumn->SetItemState(selected, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        m_ListColumn->EnsureVisible(selected);
    }
    else
    {
        m_List->SetSelection(selected);
    }
}

// events

void GotoFunctionDlg::OnColumnSelect(cb_unused wxListEvent& event)
{
    EndModal(wxID_OK);
}

void GotoFunctionDlg::OnModeChange(cb_unused wxCommandEvent& event)
{
    Manager::Get()->GetConfigManager(_T("code_completion"))->Write(_T("goto_function_window/column_mode"), m_Mode->Get3StateValue() == wxCHK_CHECKED);
    SetupMode();
}

