/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/menu.h>
    #include <wx/listctrl.h>
    #include <wx/sizer.h>
    #include <wx/panel.h>

    #include "editormanager.h"
    #include "manager.h"
#endif

#include "ReopenEditorListView.h"

namespace
{
    const int idReopenEditorList = wxNewId();
    const int idReopenItem = wxNewId();
    const int idReopenItems = wxNewId();
    const int idReopenAll = wxNewId();
    const int idRemoveItem = wxNewId();
    const int idRemoveItems = wxNewId();
    const int idRemoveAll = wxNewId();
};

BEGIN_EVENT_TABLE(ReopenEditorListView, wxPanel)
    EVT_CONTEXT_MENU(ReopenEditorListView::OnContextMenu)
    EVT_MENU(idReopenItem,ReopenEditorListView::OnReopenItems)
    EVT_MENU(idReopenItems,ReopenEditorListView::OnReopenItems)
    EVT_MENU(idReopenAll,ReopenEditorListView::OnReopenAll)
    EVT_MENU(idRemoveItem,ReopenEditorListView::OnRemoveItems)
    EVT_MENU(idRemoveItems,ReopenEditorListView::OnRemoveItems)
    EVT_MENU(idRemoveAll,ReopenEditorListView::OnRemoveAll)
END_EVENT_TABLE()

ReopenEditorListView::ReopenEditorListView(const wxArrayString& titles, const wxArrayInt& widths) :
    wxPanel(Manager::Get()->GetAppWindow())
{
    // Constructor
    Init();

    // Fill columns
    const size_t colCount = titles.GetCount();
    for (size_t i = 0; i < colCount; ++i)
        m_pListControl->InsertColumn(i, titles[i], wxLIST_FORMAT_LEFT, widths[i]);
}

ReopenEditorListView::ReopenEditorListView(const ReopenEditorListView& Original) :
    wxPanel(Manager::Get()->GetAppWindow())
{
    // Copy constructor
    Init();

    // Copy the list control
    m_pListControl->Freeze();

    // Copy columns
    const int colCount = Original.m_pListControl->GetColumnCount();
    for (int col = 0; col < colCount; ++col)
    {
        wxListItem item;

        item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_WIDTH | wxLIST_MASK_FORMAT);
        Original.m_pListControl->GetColumn(col, item);
        m_pListControl->InsertColumn(col, item);
    }

    // Copy lines
    wxListItem info;
    info.SetId(0);
    info.SetMask(wxLIST_MASK_TEXT);
    const int rowCount = Original.m_pListControl->GetItemCount();
    for (int row = rowCount-1; row >= 0; --row)
    {
        const wxString  text(Original.m_pListControl->GetItemText(row));
        const wxFont    font(Original.m_pListControl->GetItemFont(row));
        const wxColour  colour(Original.m_pListControl->GetItemTextColour(row));
        const wxUIntPtr data(Original.m_pListControl->GetItemData(row));

        m_pListControl->InsertItem(0, text);
        m_pListControl->SetItemFont(0, font);
        m_pListControl->SetItemTextColour(0, colour);
        m_pListControl->SetItemData(0, data);
        for (int col = 1; col < colCount; ++col)
        {
            info.SetColumn(col);
            Original.m_pListControl->GetItem(info);
            m_pListControl->SetItem(0, col, info.GetText());
        }
    }

    m_pListControl->Thaw();
}

ReopenEditorListView::~ReopenEditorListView()
{
    // Destructor
    wxDELETE(m_pListControl);
}

void ReopenEditorListView::Init()
{
    m_pListControl = new wxListCtrl(this, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT/* | wxLC_SINGLE_SEL*/);
    m_pListControl->SetId(idReopenEditorList);
    Connect(idReopenEditorList, -1, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction)
            &ReopenEditorListView::OnDoubleClick);
    wxSizer* bs = new wxBoxSizer(wxVERTICAL);
    bs->Add(m_pListControl, 1, wxEXPAND);
    SetSizer(bs);
}

size_t ReopenEditorListView::GetItemsCount() const
{
    return m_pListControl ? m_pListControl->GetItemCount() : 0;
}

void ReopenEditorListView::Prepend(const wxString& msg, Logger::level lv)
{
    if (!m_pListControl)
        return;

    m_pListControl->Freeze();
    m_pListControl->InsertItem(0, msg);
    m_pListControl->SetItemFont(0, style[lv].font);
    m_pListControl->SetItemTextColour(0, style[lv].colour);
    m_pListControl->Thaw();
}

void ReopenEditorListView::Prepend(const wxArrayString& colValues, Logger::level lv)
{
    if (!m_pListControl)
        return;

    const int colCount = int(colValues.GetCount());
    if (colCount == 0 || colCount > m_pListControl->GetColumnCount())
        return;

    m_pListControl->Freeze();
    Prepend(colValues[0], lv);
    for (int i = 1; i < colCount; ++i)
        m_pListControl->SetItem(0, i, colValues[i]);

    m_pListControl->Thaw();
}

void ReopenEditorListView::RemoveAt(size_t i)
{
    m_pListControl->Freeze();
    m_pListControl->DeleteItem(i);
    m_pListControl->Thaw();
}

wxArrayString ReopenEditorListView::GetItemAsArray(long item) const
{
    wxArrayString result;
    if (!m_pListControl)
        return result;

    wxListItem info;
    info.SetId(item);
    info.SetMask(wxLIST_MASK_TEXT);

    const int colCount = m_pListControl->GetColumnCount();
    for (int i = 0; i < colCount; ++i)
    {
        info.SetColumn(i);
        m_pListControl->GetItem(info);
        result.Add(info.GetText());
    }

    return result;
}

wxString ReopenEditorListView::GetFilename(long item) const
{
    if (!m_pListControl)
        return wxString();

    wxListItem info;
    info.SetId(item);
    info.SetMask(wxLIST_MASK_TEXT);
    info.SetColumn(0);
    m_pListControl->GetItem(info);
    return info.GetText();
}

cbProject* ReopenEditorListView::GetProject(long item) const
{
    if (!m_pListControl || m_pListControl->GetItemCount() == 0)
        return nullptr;

    return (cbProject*)m_pListControl->GetItemData(item);
}

void ReopenEditorListView::SetProject(long item, cbProject* prj) const
{
    if (m_pListControl || m_pListControl->GetItemCount() > 0)
    {
        m_pListControl->SetItemPtrData(item, (wxUIntPtr)prj);
    }
}

void ReopenEditorListView::OnDoubleClick(wxListEvent& event)
{
    DoOpen(GetFilename(event.GetIndex()));
}

void ReopenEditorListView::DoOpen(wxString fname)
{
    EditorManager* em = Manager::Get()->GetEditorManager();
    if(!fname.IsEmpty() && !em->IsOpen(fname))
    {
        em->Open(fname);
    }
}

void ReopenEditorListView::DoOpen(wxArrayString fnames)
{
    EditorManager* em = Manager::Get()->GetEditorManager();
    for(size_t i = 0; i < fnames.GetCount(); ++i)
    {
        if(!fnames[i].IsEmpty() && !em->IsOpen(fnames[i]))
        {
            em->Open(fnames[i]);
        }
    }
}

void ReopenEditorListView::OnContextMenu(wxContextMenuEvent& /*event*/)
{
    DoShowContextMenu();
}

void ReopenEditorListView::DoShowContextMenu()
{
    if(m_pListControl && m_pListControl->GetItemCount() > 0)
    {
        wxMenu menu;
        menu.Append(idReopenAll, _("Reopen all files"));
        menu.Append(idRemoveAll, _("Remove all files from list"));

        if (m_pListControl->GetSelectedItemCount() > 0)
        {
            menu.AppendSeparator();
            if(m_pListControl->GetSelectedItemCount() > 1)
            {
                menu.Append(idReopenItems, _("Reopen selected items"));
                menu.Append(idRemoveItems, _("Remove selected items from list"));
            }
            else
            {
                menu.Append(idReopenItem, _("Reopen selected item"));
                menu.Append(idRemoveItem, _("Remove selected item from list"));
            }
        }
        PopupMenu(&menu);

    }
}

void ReopenEditorListView::OnReopenItems(wxCommandEvent& event)
{
    if(m_pListControl && m_pListControl->GetItemCount() > 0)
    {
        long index = -1;
        if(event.GetId() == idReopenItem)
        {
            index = m_pListControl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if(index != -1)
            {
                DoOpen(GetFilename(index));
            }
        }
        if(event.GetId() == idReopenItems)
        {
            wxArrayString list;
            index = m_pListControl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            while(index != -1)
            {
                list.Add(GetFilename(index));
                index = m_pListControl->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            }
            if(list.GetCount() > 0)
            {
                DoOpen(list);
            }
        }
    }
}

void ReopenEditorListView::OnRemoveItems(wxCommandEvent& /*event*/)
{
    if(m_pListControl && m_pListControl->GetItemCount() > 0)
    {
        long index = -1;
        index = m_pListControl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        while(index != -1)
        {
            RemoveAt(index);
            index = m_pListControl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        }
    }
}

void ReopenEditorListView::OnReopenAll(wxCommandEvent& /*event*/)
{
    if(m_pListControl && m_pListControl->GetItemCount() > 0)
    {
        wxArrayString list;
        for(long i = 0; i < m_pListControl->GetItemCount(); ++i)
        {
            list.Add(GetFilename(i));
        }
        DoOpen(list);
    }
}

void ReopenEditorListView::OnRemoveAll(wxCommandEvent& /*event*/)
{
    m_pListControl->DeleteAllItems();
}

