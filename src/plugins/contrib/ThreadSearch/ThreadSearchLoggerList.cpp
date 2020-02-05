/***************************************************************
 * Name:      ThreadSearchLoggerList
 * Purpose:   ThreadSearchLoggerList implements the
 *            ThreadSearchLoggerBase interface with a wxListCtrl.
 * Author:    Jerome ANTOINE
 * Created:   2007-07-28
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    // Required extra includes
    #include <configmanager.h>
    #include <infowindow.h>
#endif

#include <wx/listctrl.h>
#include <wx/dynarray.h>
#include <wx/gdicmn.h>

#include "ThreadSearch.h"
#include "ThreadSearchView.h"
#include "ThreadSearchEvent.h"
#include "ThreadSearchLoggerList.h"
#include "ThreadSearchControlIds.h"

enum ListColumns : int
{
    FilePath = 0,
    Line, ///< Line index starting from 1
    Text, ///< File line matching search expression
    ColumnCount
};

ThreadSearchLoggerList::ThreadSearchLoggerList(ThreadSearchView& threadSearchView,
                                               ThreadSearch& threadSearchPlugin,
                                               InsertIndexManager::eFileSorting fileSorting,
                                               wxWindow* pParent,
                                               long id) :
    ThreadSearchLoggerBase(pParent, threadSearchView, threadSearchPlugin, fileSorting),
    m_IndexOffset(0),
    m_SortColumn(-1),
    m_Ascending(true)
{
    m_pListLog = new wxListCtrl(this, id, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER);
    m_pListLog->SetMinSize(wxSize(100,100));

    int size = Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/log_font_size"), platform::macosx ? 10 : 8);
    wxFont default_font(size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    m_pListLog->SetFont(default_font);

    SetupSizer(m_pListLog);

    SetListColumns();

    // Events are managed dynamically to be able to stop/start management when required.
    ConnectEvents(this);
}


ThreadSearchLoggerList::~ThreadSearchLoggerList()
{
    wxWindow* pParent = m_pListLog->GetParent();
    if ( pParent != NULL )
    {
        // Events are managed dynamically to be able to stop/start management when required.
        DisconnectEvents(pParent);
    }
    m_pListLog->Destroy();
}


void ThreadSearchLoggerList::Update()
{
    ThreadSearchLoggerBase::Update();

    // Adds/Removes listview header and adds/removes vertical rules
    m_pListLog->SetSingleStyle(wxLC_NO_HEADER, !m_ThreadSearchPlugin.GetDisplayLogHeaders());
    m_pListLog->SetSingleStyle(wxLC_VRULES,     m_ThreadSearchPlugin.GetDrawLogLines());
    SetListColumns();
}


void ThreadSearchLoggerList::SetListColumns()
{
    m_pListLog->ClearAll();

    m_pListLog->InsertColumn(ListColumns::FilePath, _("File path"), wxLIST_FORMAT_LEFT, 100);
    m_pListLog->InsertColumn(ListColumns::Line, _("Line"), wxLIST_FORMAT_RIGHT, 50);
    m_pListLog->InsertColumn(ListColumns::Text, _("Text"), wxLIST_FORMAT_LEFT, 500);
}


void ThreadSearchLoggerList::OnLoggerListClick(wxListEvent& event)
{
    // Manages list log left (single) click
    if ( IsLineResultLine() )
    {
        // Gets file path and line from list control
        wxString filepath(wxEmptyString);
        long line;
        if ( GetFileLineFromListEvent(event, filepath, line) == false )
        {
            cbMessageBox(_("Failed to retrieve file path and line number"),_("Error"), wxICON_ERROR);
            return;
        }
        m_ThreadSearchView.OnLoggerClick(filepath, line);
    }
    event.Skip();
}


void ThreadSearchLoggerList::OnLoggerListDoubleClick(wxListEvent& event)
{
    // Manages list log left double click
    if ( IsLineResultLine() )
    {
        // Gets file path and line from list control
        wxString filepath;
        long line;
        if ( GetFileLineFromListEvent(event, filepath, line) == false )
        {
            cbMessageBox(_("Failed to retrieve file path and line number"), _("Error"), wxICON_ERROR);
            return;
        }
        m_ThreadSearchView.OnLoggerDoubleClick(filepath, line);
    }
    event.Skip();
}


bool ThreadSearchLoggerList::GetFileLineFromListEvent(wxListEvent& /*event*/, wxString& filepath, long &line)
{
    bool success = false;
    wxListItem listItem;

    do {
        // Finds selected item index
        long index = m_pListLog->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( index == -1 ) break;

        // First, gets file dir
        wxString filedir;
        listItem.m_itemId = index;
        listItem.m_col    = ListColumns::FilePath;
        listItem.m_mask   = wxLIST_MASK_TEXT;

        if ( m_pListLog->GetItem(listItem) == false ) break;

        filepath = listItem.GetText();

        // Then gets line number
        listItem.m_col = ListColumns::Line;
        if ( m_pListLog->GetItem(listItem) == false ) break;

        if ( listItem.GetText().ToLong(&line) == true )
        {
            success = true;
        }
    } while ( 0 );

    return success;
}


bool ThreadSearchLoggerList::IsLineResultLine(long index /* -1 */)
{
    bool isResultLine = false;
    wxListItem listItem;

    do {
        if ( index == -1 )
            // Finds selected item index
            index = m_pListLog->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

        if ( index == -1 ) break;

        // First, gets file dir
        wxString filedir;
        listItem.m_itemId = index;
        listItem.m_col    = ListColumns::FilePath;
        listItem.m_mask   = wxLIST_MASK_TEXT;

        if ( m_pListLog->GetItem(listItem) == false ) break;

        filedir = listItem.GetText();
        isResultLine = !filedir.StartsWith(_("=>"));
    } while ( 0 );

    return isResultLine;
}


void ThreadSearchLoggerList::OnThreadSearchEvent(const ThreadSearchEvent& event)
{
    // A search event has been sent by the worker thread.
    // List log upddate
    const wxArrayString& words  = event.GetLineTextArray();
    const wxFileName&    filename(event.GetString());
    bool                 setFocus(false);

    m_TotalLinesFound += event.GetNumberOfMatches();

    wxASSERT((words.GetCount() % 2) == 0);

    // Use of Freeze Thaw to enhance speed and limit blink effect
    m_pListLog->Freeze();
    long index = m_IndexManager.GetInsertionIndex(filename.GetFullPath(), words.GetCount()/2);
    index += m_IndexOffset;
    for (size_t i = 0; i + 1 < words.GetCount(); i += 2)
    {
        m_pListLog->InsertItem(index, filename.GetFullPath());
        m_pListLog->SetItem(index, ListColumns::Line, words[i]);
        m_pListLog->SetItem(index, ListColumns::Text, words[i+1]);
        m_pListLog->SetItemData(index, 0);

        // We update preview log for first list item
        if ( m_pListLog->GetItemCount() == 1 )
        {
            // Gets line index
            long line = 0;
            if ( words[i].ToLong(&line) == false )
            {
                cbMessageBox(_("Failed to convert line number from %s") + words[i], _("Error"), wxICON_ERROR);
            }
            else
            {
                m_ThreadSearchView.UpdatePreview(filename.GetFullPath(), line);

                // It is useful to give focus to list to navigate in results
                // just after running a search
                setFocus = true;
            }
        }
        index++;
    }

    size_t count = m_pListLog->GetItemCount();
    size_t countPerPage = std::max<size_t>(m_pListLog->GetCountPerPage() - 1, 0);
    if (count > countPerPage && m_IndexOffset > 0)
    {
        size_t markerLine = m_IndexOffset - 1;
        if (m_TotalLinesFound <= countPerPage)
        {
            m_pListLog->EnsureVisible(markerLine + m_TotalLinesFound);
        }
        else if (m_TotalLinesFound > countPerPage && !m_MadeVisible)
        {
            m_pListLog->EnsureVisible(markerLine + countPerPage);
            if (static_cast<size_t>(m_pListLog->GetTopItem()) != markerLine)
                m_pListLog->EnsureVisible(markerLine);
            m_MadeVisible = true;
        }
    }

    m_pListLog->Thaw();

    if ( setFocus == true )
    {
        // On Linux, executing SetFocus just after UpdatePreview(0)
        // does not work. Probbly because of Thaw...
        m_pListLog->SetFocus();
    }
}


wxWindow* ThreadSearchLoggerList::GetWindow()
{
    return m_pListLog;
}


void ThreadSearchLoggerList::SetFocus()
{
    m_pListLog->SetFocus();
}


void ThreadSearchLoggerList::ConnectEvents(wxEvtHandler* pEvtHandler)
{
    // Dynamic event connections.
    int id = m_pListLog->GetId();

    pEvtHandler->Connect(id, wxEVT_COMMAND_LIST_ITEM_SELECTED,
                        (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
                        &ThreadSearchLoggerList::OnLoggerListClick, NULL, static_cast<wxEvtHandler*>(this));

    pEvtHandler->Connect(id, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
                        (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
                        &ThreadSearchLoggerList::OnLoggerListDoubleClick, NULL, static_cast<wxEvtHandler*>(this));

    pEvtHandler->Connect(id, wxEVT_COMMAND_LIST_COL_CLICK,
                         (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction)
                         &ThreadSearchLoggerList::OnColumnClick, NULL, static_cast<wxEvtHandler*>(this));

#if wxUSE_MENUS
    pEvtHandler->Connect(id, wxEVT_CONTEXT_MENU,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &ThreadSearchLoggerList::OnLoggerListContextualMenu, NULL, this);

    pEvtHandler->Connect(controlIDs.Get(ControlIDs::idMenuCtxDeleteItem), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerList::OnDeleteListItem), NULL, this);

    pEvtHandler->Connect(controlIDs.Get(ControlIDs::idMenuCtxDeleteAllItems), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerList::OnDeleteAllListItems), NULL, this);
#endif // wxUSE_MENUS
}


void ThreadSearchLoggerList::DisconnectEvents(wxEvtHandler* pEvtHandler)
{
    // Dynamic event Disconnections.
    int id = m_pListLog->GetId();
    pEvtHandler->Disconnect(id, wxEVT_COMMAND_LIST_ITEM_SELECTED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &ThreadSearchLoggerList::OnLoggerListClick, NULL, static_cast<wxEvtHandler*>(this));

    pEvtHandler->Disconnect(id, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &ThreadSearchLoggerList::OnLoggerListDoubleClick, NULL, static_cast<wxEvtHandler*>(this));

    pEvtHandler->Disconnect(id, wxEVT_COMMAND_LIST_COL_CLICK,
                            (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction)
                            &ThreadSearchLoggerList::OnColumnClick, NULL, static_cast<wxEvtHandler*>(this));

#if wxUSE_MENUS
    pEvtHandler->Disconnect(id, wxEVT_CONTEXT_MENU,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &ThreadSearchLoggerList::OnLoggerListContextualMenu, NULL, this);

    pEvtHandler->Disconnect(controlIDs.Get(ControlIDs::idMenuCtxDeleteItem), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerList::OnDeleteListItem), NULL, this);

    pEvtHandler->Disconnect(controlIDs.Get(ControlIDs::idMenuCtxDeleteAllItems), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerList::OnDeleteAllListItems), NULL, this);
#endif // wxUSE_MENUS
}


void ThreadSearchLoggerList::OnLoggerListContextualMenu(wxContextMenuEvent& event)
{
    wxPoint point = event.GetPosition();
    bool hasSelection = false;

    // If from keyboard
    if ((point.x == -1) && (point.y == -1))
    {
        wxSize size = m_pListLog->GetSize();
        point.x = size.x / 2;
        point.y = size.y / 2;
        if (m_pListLog->GetSelectedItemCount() > 0)
            hasSelection = true;
    }
    else
    {
        point = m_pListLog->ScreenToClient(point);
        long tmp;
        int flags;
        if (m_pListLog->HitTest(point, flags, &tmp) != wxNOT_FOUND)
            hasSelection = true;
    }
    ShowMenu(point, hasSelection, m_pListLog->GetItemCount() > 0);
    // No event skipping, otherwise, Message notebook contextual menu pops up
}


void ThreadSearchLoggerList::OnDeleteListItem(wxCommandEvent& /*event*/)
{
    // Finds selected item index
    long index = m_pListLog->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if ( index != -1 )
    {
        long start = index;
        long end = index;
        if ( IsLineResultLine(index) )
        {
            if ( (index > 0) && !IsLineResultLine(index - 1) &&
                 ((index == m_pListLog->GetItemCount() - 1) || !IsLineResultLine(index + 1)) )
            {
                start--;
            }
        }
        else
        {
            index++;
            while ( (index < m_pListLog->GetItemCount()) && IsLineResultLine(index) )
            {
                end = index;
                index++;
            }
        }

        for (int i = end; i >= start; i--)
        {
            DeleteListItem(i);
        }
    }
}


void ThreadSearchLoggerList::OnDeleteAllListItems(wxCommandEvent& /*event*/)
{
    DeleteListItems();
}


void ThreadSearchLoggerList::DeleteListItem(long index)
{
    m_pListLog->DeleteItem(index);
}


void ThreadSearchLoggerList::DeleteListItems()
{
    Clear();
}


void ThreadSearchLoggerList::Clear()
{
    m_pListLog->DeleteAllItems();
    m_IndexManager.Reset();
    m_IndexOffset = 0;
}


void ThreadSearchLoggerList::OnSearchBegin(const ThreadSearchFindData& findData)
{
    m_TotalLinesFound = 0;
    m_MadeVisible = false;

    if ( m_ThreadSearchPlugin.GetDeletePreviousResults() )
    {
        Clear();
        m_IndexOffset = 0;
    }
    else
    {
        m_IndexManager.Reset();
        long index = m_pListLog->GetItemCount();
        m_pListLog->InsertItem(index, wxString::Format(_("=> %s"), findData.GetFindText().c_str()));
        m_pListLog->SetItem(index, ListColumns::Line, _("==="));
        m_pListLog->SetItem(index, ListColumns::Text, _("============"));
        m_pListLog->SetItemData(index, 1);

        wxListItem info;
        info.SetStateMask(wxLIST_MASK_STATE);
        info.SetId(index);
        info.SetState(wxLIST_STATE_SELECTED);
        m_pListLog->SetItem(info);

        m_IndexOffset = m_pListLog->GetItemCount();
        m_pListLog->EnsureVisible(index);
    }

    m_SortColumn = -1;
    m_Ascending = true;
}

void ThreadSearchLoggerList::OnSearchEnd()
{
    wxString message = wxString::Format(_("%lu matches found."), static_cast<unsigned long>(m_TotalLinesFound));
    long index = m_pListLog->GetItemCount();
    m_pListLog->InsertItem(index, _("=> Search complete. "));
    m_pListLog->SetItem(index, ListColumns::Text, message);
    m_pListLog->SetItemData(index, 2);

    if (m_TotalLinesFound > static_cast<size_t>(m_pListLog->GetCountPerPage()))
        InfoWindow::Display(_("Search finished"), message);
    else if (m_TotalLinesFound <= static_cast<size_t>(std::max(m_pListLog->GetCountPerPage()-2, 0)))
        m_pListLog->EnsureVisible(index);

    if (m_ThreadSearchPlugin.GetAutosizeLogColumns())
    {
        int columns = m_pListLog->GetColumnCount();
        for (int ii = 0; ii < columns; ++ii)
            m_pListLog->SetColumnWidth(ii, wxLIST_AUTOSIZE);
    }
}

inline int Compare(long a, long b)
{
    return (a < b ? -1 : (a > b ? 1 : 0));
}

struct ItemLine
{
    long line;
    long searchIndex;
    int type; //(0 - header, 1 - normal item, 2 - footer)
};

inline int wxCALLBACK SortLineAscending(wxIntPtr item1, wxIntPtr item2, wxIntPtr /*data*/)
{
    ItemLine const &i1 = *reinterpret_cast<ItemLine const *>(item1);
    ItemLine const &i2 = *reinterpret_cast<ItemLine const *>(item2);

    int c = Compare(i1.searchIndex, i2.searchIndex);
    if (c)
        return c;
    c = Compare(i1.type, i2.type);
    if (c)
        return c;

    return Compare(i1.line, i2.line);
}

inline int wxCALLBACK SortLineDescending(wxIntPtr item1, wxIntPtr item2, wxIntPtr /*data*/)
{
    ItemLine const &i1 = *reinterpret_cast<ItemLine const *>(item1);
    ItemLine const &i2 = *reinterpret_cast<ItemLine const *>(item2);

    int c = Compare(i1.searchIndex, i2.searchIndex);
    if (c)
        return c;
    c = Compare(i1.type, i2.type);
    if (c)
        return c;

    return Compare(i2.line, i1.line);
}


struct Item
{
    wxString filepath;
    long line;
    long searchIndex;
    int type; //(0 - header, 1 - normal item, 2 - footer)

    int CompareFilepath(Item const &item) const
    {
        int c = filepath.compare(item.filepath);
        if (c)
            return c;
        return Compare(line, item.line);
    }
};

static int wxCALLBACK SortFilepathAscending(wxIntPtr item1, wxIntPtr item2, wxIntPtr /*data*/)
{
    Item const &i1 = *reinterpret_cast<Item const *>(item1);
    Item const &i2 = *reinterpret_cast<Item const *>(item2);

    int c = Compare(i1.searchIndex, i2.searchIndex);
    if (c)
        return c;
    c = Compare(i1.type, i2.type);
    if (c)
        return c;

    return i1.CompareFilepath(i2);
}

static int wxCALLBACK SortFilepathDescending(wxIntPtr item1, wxIntPtr item2, wxIntPtr /*data*/)
{
    Item const &i1 = *reinterpret_cast<Item const *>(item1);
    Item const &i2 = *reinterpret_cast<Item const *>(item2);

    int c = Compare(i1.searchIndex, i2.searchIndex);
    if (c)
        return c;
    c = Compare(i1.type, i2.type);
    if (c)
        return c;

    return i2.CompareFilepath(i1);
}

struct ItemText
{
    wxString text;
    long searchIndex;
    int type; //(0 - header, 1 - normal item, 2 - footer)
};

static int wxCALLBACK SortTextAscending(wxIntPtr item1, wxIntPtr item2, wxIntPtr /*data*/)
{
    ItemText const &i1 = *reinterpret_cast<ItemText const *>(item1);
    ItemText const &i2 = *reinterpret_cast<ItemText const *>(item2);

    int c = Compare(i1.searchIndex, i2.searchIndex);
    if (c)
        return c;
    c = Compare(i1.type, i2.type);
    if (c)
        return c;

    return i1.text.compare(i2.text);
}

static int wxCALLBACK SortTextDescending(wxIntPtr item1, wxIntPtr item2, wxIntPtr /*data*/)
{
    ItemText const &i1 = *reinterpret_cast<ItemText const *>(item1);
    ItemText const &i2 = *reinterpret_cast<ItemText const *>(item2);

    int c = Compare(i1.searchIndex, i2.searchIndex);
    if (c)
        return c;
    c = Compare(i1.type, i2.type);
    if (c)
        return c;

    return i2.text.compare(i1.text);
}

template <typename Item>
void SetItemType(Item &item, wxListCtrl &list, long index, long &searchIndex)
{
    switch (list.GetItemData(index))
    {
        case 0:
            item.type = 1;
            break;
        case 1:
            item.type = 0;
            ++searchIndex;
            break;
        case 2:
        default:
            item.type = 2;
    }
}

template <typename Item>
void RestoreItemData(wxListCtrl &list, const Item *items, long count)
{
    for (int ii = 0; ii < count; ++ii)
    {
        switch (items[ii].type)
        {
            case 0:
                list.SetItemData(ii, 1);
                break;
            case 1:
                list.SetItemData(ii, 0);
                break;
            case 2:
            default:
                list.SetItemData(ii, 2);
        }
    }
}

void ThreadSearchLoggerList::OnColumnClick(wxListEvent& event)
{
    int column = event.GetColumn();
    int count = m_pListLog->GetItemCount();
    if (column < 0 || column >= ListColumns::ColumnCount || count == 0)
        return;

    if (column != m_SortColumn)
    {
        m_SortColumn = column;
        m_Ascending = true;
    }
    else
        m_Ascending = !m_Ascending;

    switch (column)
    {
        case ListColumns::Line:
            {
                long searchIndex = -1;
                std::unique_ptr<ItemLine[]> items(new ItemLine[count]);

                for (int ii = 0; ii < count; ++ii)
                {
                    wxListItem item;
                    item.SetId(ii);
                    item.SetMask(wxLIST_MASK_TEXT);
                    item.SetColumn(ListColumns::Line);
                    m_pListLog->GetItem(item);

                    wxString const &str_line = item.GetText();

                    long line;
                    if (str_line.ToLong(&line))
                        items[ii].line = line;
                    else
                        items[ii].line = -1;

                    SetItemType(items[ii], *m_pListLog, ii, searchIndex);
                    items[ii].searchIndex = searchIndex;

                    m_pListLog->SetItemPtrData(ii, reinterpret_cast<wxUIntPtr>(items.get() + ii));
                }
                m_pListLog->SortItems(m_Ascending ? SortLineAscending : SortLineDescending, 0);

                RestoreItemData(*m_pListLog, items.get(), count);
            }

            break;
        case ListColumns::FilePath:
            {
                long searchIndex = -1;
                std::unique_ptr<Item[]> items(new Item[count]);

                for (int ii = 0; ii < count; ++ii)
                {
                    wxListItem item;
                    item.SetId(ii);
                    item.SetMask(wxLIST_MASK_TEXT);
                    item.SetColumn(ListColumns::FilePath);
                    m_pListLog->GetItem(item);

                    items[ii].filepath = item.GetText();

                    item.SetColumn(ListColumns::Line);
                    m_pListLog->GetItem(item);

                    items[ii].line = -1;
                    item.GetText().ToLong(&items[ii].line);

                    SetItemType(items[ii], *m_pListLog, ii, searchIndex);
                    items[ii].searchIndex = searchIndex;

                    m_pListLog->SetItemPtrData(ii, reinterpret_cast<wxUIntPtr>(items.get() + ii));
                }

                m_pListLog->SortItems(m_Ascending ? SortFilepathAscending : SortFilepathDescending, 0);

                RestoreItemData(*m_pListLog, items.get(), count);
            }
            break;
        case ListColumns::Text:
            {
                long searchIndex = -1;
                std::unique_ptr<ItemText[]> items(new ItemText[count]);

                for (int ii = 0; ii < count; ++ii)
                {
                    wxListItem item;
                    item.SetId(ii);
                    item.SetMask(wxLIST_MASK_TEXT);
                    item.SetColumn(ListColumns::Text);
                    m_pListLog->GetItem(item);

                    items[ii].text = item.GetText();
                    SetItemType(items[ii], *m_pListLog, ii, searchIndex);
                    items[ii].searchIndex = searchIndex;

                    m_pListLog->SetItemPtrData(ii, reinterpret_cast<wxUIntPtr>(items.get() + ii));
                }

                m_pListLog->SortItems(m_Ascending ? SortTextAscending : SortTextDescending, 0);

                RestoreItemData(*m_pListLog, items.get(), count);
            }
            break;
        default:
            break;
    }
}
