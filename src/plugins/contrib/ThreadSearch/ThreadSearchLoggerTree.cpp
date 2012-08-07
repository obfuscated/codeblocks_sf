/***************************************************************
 * Name:      ThreadSearchLoggerTree
 * Purpose:   ThreadSearchLoggerTree implements the
 *            ThreadSearchLoggerBase interface with a wxTreeCtrl.
 * Author:    Jerome ANTOINE
 * Created:   2007-07-28
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    // Required extra includes
#endif

#include "ThreadSearch.h"
#include "ThreadSearchLoggerTree.h"
#include "ThreadSearchEvent.h"
#include "ThreadSearchView.h"
#include "ThreadSearchControlIds.h"

ThreadSearchLoggerTree::ThreadSearchLoggerTree(ThreadSearchView& threadSearchView,
                                               ThreadSearch& threadSearchPlugin,
                                               InsertIndexManager::eFileSorting fileSorting,
                                               wxPanel* pParent,
                                               long id)
                       : ThreadSearchLoggerBase(threadSearchView, threadSearchPlugin, fileSorting)
                       , m_pTreeLog(NULL)
                       , m_FirstItemProcessed(false)
{
    m_pTreeLog = new wxTreeCtrl(pParent, id, wxDefaultPosition, wxSize(1,1), wxTR_HAS_BUTTONS|wxTR_LINES_AT_ROOT|wxTR_FULL_ROW_HIGHLIGHT|wxTR_HIDE_ROOT|wxTR_DEFAULT_STYLE|wxSUNKEN_BORDER);
    m_pTreeLog->SetMinSize(wxSize(100, 100));
    m_FilesParentId = m_pTreeLog->AddRoot(wxEmptyString);

    // Events are managed dynamically to be able to stop/start management when required.
    ConnectEvents(pParent);
}


ThreadSearchLoggerTree::~ThreadSearchLoggerTree()
{
    wxWindow* pParent = m_pTreeLog->GetParent();
    if ( pParent != NULL )
    {
        // Events are managed dynamically to be able to stop/start management when required.
        DisconnectEvents(pParent);
    }
    m_pTreeLog->Destroy();
    m_pTreeLog = NULL;
}


wxWindow* ThreadSearchLoggerTree::GetWindow()
{
    return m_pTreeLog;
}


void ThreadSearchLoggerTree::SetFocus()
{
    m_pTreeLog->SetFocus();
}


void ThreadSearchLoggerTree::ConnectEvents(wxEvtHandler* pEvtHandler)
{
    // Dynamic event connections.
    int id = m_pTreeLog->GetId();
    pEvtHandler->Connect(id, wxEVT_COMMAND_TREE_SEL_CHANGED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &ThreadSearchLoggerTree::OnLoggerTreeClick, NULL, this);

    pEvtHandler->Connect(id, wxEVT_COMMAND_TREE_ITEM_ACTIVATED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &ThreadSearchLoggerTree::OnLoggerTreeDoubleClick, NULL, this);

#if wxUSE_MENUS
    pEvtHandler->Connect(id, wxEVT_COMMAND_TREE_ITEM_MENU,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &ThreadSearchLoggerTree::OnLoggerTreeContextualMenu, NULL, this);

    pEvtHandler->Connect(idMenuCtxDeleteItem, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerTree::OnDeleteTreeItem), NULL, this);

    pEvtHandler->Connect(idMenuCtxDeleteAllItems, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerTree::OnDeleteAllTreeItems), NULL, this);
#endif // wxUSE_MENUS
}


void ThreadSearchLoggerTree::DisconnectEvents(wxEvtHandler* pEvtHandler)
{
    // Dynamic event disconnections.
    int id = m_pTreeLog->GetId();
    pEvtHandler->Disconnect(id, wxEVT_COMMAND_TREE_SEL_CHANGED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &ThreadSearchLoggerTree::OnLoggerTreeClick, NULL, this);

    pEvtHandler->Disconnect(id, wxEVT_COMMAND_TREE_ITEM_ACTIVATED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &ThreadSearchLoggerTree::OnLoggerTreeDoubleClick, NULL, this);

#if wxUSE_MENUS
    pEvtHandler->Disconnect(id, wxEVT_COMMAND_TREE_ITEM_MENU,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &ThreadSearchLoggerTree::OnLoggerTreeContextualMenu, NULL, this);

    pEvtHandler->Disconnect(idMenuCtxDeleteItem, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerTree::OnDeleteTreeItem), NULL, this);

    pEvtHandler->Disconnect(idMenuCtxDeleteAllItems, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerTree::OnDeleteAllTreeItems), NULL, this);
#endif // wxUSE_MENUS
}


void ThreadSearchLoggerTree::OnLoggerTreeClick(wxTreeEvent& event)
{
    // Manages list log left single click
    wxTreeItemId itemId = event.GetItem();
    if ( itemId.IsOk() && hasResultLineForTreeItem(itemId) )
    {
        // Gets file path and line from list control
        wxString filepath(wxEmptyString);
        long line;
        if ( GetFileLineFromTreeEvent(event, filepath, line) == false )
        {
            cbMessageBox(_("Failed to retrieve file path and line number"), _("Error"), wxICON_ERROR);
            return;
        }
        m_ThreadSearchView.OnLoggerClick(filepath, line);
    }
    event.Skip();
}


void ThreadSearchLoggerTree::OnLoggerTreeDoubleClick(wxTreeEvent& event)
{
    // Manages list log left double click
    wxTreeItemId itemId = event.GetItem();
    if ( itemId.IsOk() && hasResultLineForTreeItem(itemId) )
    {
        // Gets file path and line from list control
        wxString filepath(wxEmptyString);
        long line;
        if ( GetFileLineFromTreeEvent(event, filepath, line) == false )
        {
            cbMessageBox(_("Failed to retrieve file path and line number"), _("Error"), wxICON_ERROR);
            return;
        }
        m_ThreadSearchView.OnLoggerDoubleClick(filepath, line);
    }

    event.Skip();
}


void ThreadSearchLoggerTree::OnLoggerTreeContextualMenu(wxTreeEvent& event)
{
    wxPoint clientPoint = event.GetPoint();
    m_ToDeleteItemId = event.GetItem();
    ShowMenu(clientPoint);
    // No event skipping, otherwise, Message notebook contextual menu pops up
}


bool ThreadSearchLoggerTree::hasResultLineForTreeItem(wxTreeItemId treeItemId)
{
    return (m_pTreeLog->GetItemText(treeItemId).StartsWith(_("=>")) == false) || (m_pTreeLog->ItemHasChildren(treeItemId));
}


bool ThreadSearchLoggerTree::GetFileLineFromTreeEvent(wxTreeEvent& event, wxString& filepath, long &line)
{
    wxTreeItemId lineItemId;
    wxTreeItemId fileItemId;
    wxTreeItemId tmpItemId;
    wxTreeItemId eventItemId = event.GetItem();
    bool success(false);

    filepath = wxEmptyString;
    line = 0;

    do
    {
        // Looks for deepest child, which is the first interesting item (line item)
        wxTreeItemIdValue cookie;
        tmpItemId = eventItemId;
        do {
            lineItemId = tmpItemId;
            tmpItemId = m_pTreeLog->GetFirstChild(lineItemId, cookie);
        } while ( tmpItemId.IsOk() == true );
        fileItemId = m_pTreeLog->GetItemParent(lineItemId);

        // We extract line and file path values
        // Look at ThreadSearchLoggerTree::OnThreadSearchEvent to have format

        // Line:
        const wxString& lineText = m_pTreeLog->GetItemText(lineItemId);
        int columnPos = lineText.Find(':');
        if ( columnPos == wxNOT_FOUND )
            break;

        if ( lineText.Left(columnPos).ToLong(&line) == false )
            break;

        // File:
        const wxString& fileText = m_pTreeLog->GetItemText(fileItemId);
        int fileLength = fileText.Find(wxT(" ("));
        if ( fileLength == wxNOT_FOUND )
            break;

        int dirBegin   = fileLength + 2; // 2 for " ("
        int dirLength  = fileText.Length() - dirBegin - 1;
        if ( dirLength <= 0 )
            break;

        wxFileName filename(fileText.Mid(dirBegin, dirLength), fileText.Left(fileLength));
        filepath = filename.GetFullPath();

        success = true;
    } while ( false );

    return success;
}


void ThreadSearchLoggerTree::OnThreadSearchEvent(const ThreadSearchEvent& event)
{
    // A search event has been sent by the worker thread.
    // Tree log upddate
    const wxArrayString& words  = event.GetLineTextArray();
    const wxFileName&    filename(event.GetString());
    bool                 setFocus(false);
    wxTreeItemId         fileItemId;
    long                 index    = m_IndexManager.GetInsertionIndex(filename.GetFullPath());
    long                 nb_items = m_pTreeLog->GetChildrenCount(m_FilesParentId, false);

    wxASSERT(index != wxNOT_FOUND);
    wxASSERT((words.GetCount() % 2) == 0);

    // Use of Freeze Thaw to enhance speed and limit blink effect
    m_pTreeLog->Freeze();
    wxTreeItemId lineItemId;

    if ( index == nb_items )
    {
        fileItemId = m_pTreeLog->AppendItem(m_FilesParentId,
                                            wxString::Format(wxT("%s (%s)"),
                                                            filename.GetFullName().c_str(),
                                                            filename.GetPath().c_str()));
    }
    else
    {
        fileItemId = m_pTreeLog->InsertItem(m_FilesParentId, index,
                                            wxString::Format(wxT("%s (%s)"),
                                                             filename.GetFullName().c_str(),
                                                             filename.GetPath().c_str()));
    }

    for (size_t i = 0; i < words.GetCount(); i += 2)
    {
        lineItemId = m_pTreeLog->AppendItem(fileItemId, wxString::Format(wxT("%s: %s"),
                                                                         words[i].c_str(),     // Line index starting from 1
                                                                         words[i+1].c_str())); // File line matching search expression

        // We update preview log for first list item
        if ( (m_FirstItemProcessed == false)                        &&
             (m_pTreeLog->GetChildrenCount(fileItemId, false) == 1) &&
             (m_pTreeLog->GetChildrenCount(m_FilesParentId, false) == 1) )
        {
            // Expand first file item
            m_pTreeLog->Expand(fileItemId);

            // Select first item (generates selection event to update ThreadSearchView code preview)
            m_pTreeLog->SelectItem(lineItemId);

            // to avoid lots of preview that freezes app
            m_FirstItemProcessed = true;

            // It is useful to give focus to tree to navigate in results
            // just after running a search
            setFocus = true;
        }
    }
    m_pTreeLog->Thaw();

    if ( setFocus == true )
    {
        // On Linux, executing SetFocus just after SelectItem
        // does not work. Probbly because of Thaw...
        m_pTreeLog->SetFocus();
    }
}


void ThreadSearchLoggerTree::Clear()
{
    // As wxTreeCtrl::DeleteChildren sends lots of selected events
    // and because we don't want any wxTreeCtrl event to be processed
    // during delete, we disable events management during tree cleaning
    // (DisconnectEvents then ConnectEvents).
    wxWindow* pParent = m_pTreeLog->GetParent();
    if ( pParent == NULL ) return;

    DisconnectEvents(pParent);

    m_pTreeLog->DeleteChildren(m_pTreeLog->GetRootItem());
    m_FirstItemProcessed = false;

    m_IndexManager.Reset();

    ConnectEvents(pParent);
}


void ThreadSearchLoggerTree::OnSearchBegin(const ThreadSearchFindData& findData)
{
    if ( m_ThreadSearchPlugin.GetDeletePreviousResults() )
    {
        Clear();
        m_FilesParentId = m_pTreeLog->GetRootItem();
    }
    else
    {
        m_IndexManager.Reset();
        m_FirstItemProcessed = false;
        m_FilesParentId = m_pTreeLog->AppendItem(m_pTreeLog->GetRootItem(),
                wxString::Format(_("=> %s"), findData.GetFindText().c_str()));
    }
}


void ThreadSearchLoggerTree::OnDeleteTreeItem(wxCommandEvent& event)
{
    if ( m_ToDeleteItemId.IsOk() )
    {
        wxTreeItemId rootId = m_pTreeLog->GetRootItem();
        wxTreeItemId parentId = m_pTreeLog->GetItemParent(m_ToDeleteItemId);
        while ( (parentId != rootId) && (m_pTreeLog->GetChildrenCount(parentId, false) == 1) )
        {
            m_ToDeleteItemId = parentId;
            parentId = m_pTreeLog->GetItemParent(m_ToDeleteItemId);
        }
        DeleteTreeItem(m_ToDeleteItemId);
    }
    event.Skip();
}


void ThreadSearchLoggerTree::DeleteTreeItem(wxTreeItemId id)
{
    if ( id.IsOk() )
    {
        wxWindow* pParent = m_pTreeLog->GetParent();
        if ( pParent == NULL ) return;

        // Disconnect events to avoid lots of seleced events
        DisconnectEvents(pParent);
        m_pTreeLog->Delete(id);
        ConnectEvents(pParent);

        // Select current selected item to refresh code preview
        wxTreeItemId selectedItemId = m_pTreeLog->GetSelection();
        if ( selectedItemId.IsOk() )
        {
            m_pTreeLog->SelectItem(selectedItemId, true);
        }
    }
}


void ThreadSearchLoggerTree::OnDeleteAllTreeItems(wxCommandEvent& /*event*/)
{
    if ( m_ToDeleteItemId.IsOk() )
    {
        DeleteTreeItems();
    }
}


void ThreadSearchLoggerTree::DeleteTreeItems()
{
    Clear();
}
