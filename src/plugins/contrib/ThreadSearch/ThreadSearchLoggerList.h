/***************************************************************
 * Name:      ThreadSearchLoggerList
 * Purpose:   ThreadSearchLoggerList implements the
 *            ThreadSearchLoggerBase with a wxListCtrl.
 * Author:    Jerome ANTOINE
 * Created:   2007-07-28
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef THREAD_SEARCH_LOGGER_LIST_H
#define THREAD_SEARCH_LOGGER_LIST_H

#include <wx/event.h>

#include "ThreadSearchLoggerBase.h"

class wxPanel;
class wxWindow;
class wxListEvent;
class wxListCtrl;
class wxString;

class ThreadSearch;
class ThreadSearchView;
class ThreadSearchEvent;

class ThreadSearchLoggerList : public ThreadSearchLoggerBase
{
public:
    /** Constructor. */
    ThreadSearchLoggerList(ThreadSearchView& threadSearchView, ThreadSearch& threadSearchPlugin,
                            InsertIndexManager::eFileSorting fileSorting, wxWindow* pParent, long id);

    /** Destructor. */
    ~ThreadSearchLoggerList() override;

    /** Getter */
    eLoggerTypes GetLoggerType() override { return TypeList; }

    /** Called by ThreadSearchView when new settings are applied. */
    void Update() override;

    /** Called by ThreadSearchView to process a ThreadSearchEvent
      * sent by worker thread.
      */
    void OnThreadSearchEvent(const ThreadSearchEvent& event) override;

    /** Removes all items from logger. */
    void Clear() override;

    /** Called on search begin to prepare logger. */
    void OnSearchBegin(const ThreadSearchFindData& findData) override;
    /** Called on search end */
    void OnSearchEnd() override;

    /** Returns the logger window. */
    wxWindow* GetWindow() override;

    /** Sets focus on list window. */
    void SetFocus() override;

    /** Single click event handler */
    void OnLoggerListClick(wxListEvent& event);

    /** Double click event handler */
    void OnLoggerListDoubleClick(wxListEvent& event);

    /** Clicked in the header of the list control */
    void OnColumnClick(wxListEvent& event);
private:
    /** SetListColumns
      * The SimpleListLog constructor does not set the provided columns on Linux.
      * It is necessary to set columns after the ThreadSearchLoggerList constructor.
      */
    void SetListColumns();

    /** GetFileLineFromListEvent
      * Return the file path at index from the list control using dir and file columns.
      * @param event    : list control event
      * @param filepath : reference that will receive the path
      * @param line     : reference that will receive the line index
      * @return true if successful.
      */
    bool GetFileLineFromListEvent(wxListEvent& event, wxString& filepath, long &line);

    /** IsLineResultLine
      * Return true if line is a result line. It is not the case for the
      * first line of a new search without deleting previous results.
      * @param index : item index in list control. If -1 is given (default
      * value), first selected line will be processed.
      * @return true if line is usable.
      */
    bool IsLineResultLine(long index = -1);

    /** Dynamic events connection. */
    void ConnectEvents(wxEvtHandler* pEvtHandler);

    /** Dynamic events disconnection. */
    void DisconnectEvents(wxEvtHandler* pEvtHandler);

    /** Contextual menu event handler */
    void OnLoggerListContextualMenu(wxContextMenuEvent& event);

    /** Delete item menu event handler */
    void OnDeleteListItem(wxCommandEvent& event);

    /** Delete item menu event handler */
    void OnDeleteAllListItems(wxCommandEvent& event);

    /** Deletes an item from the List */
    void DeleteListItem(long index);

    /** Deletes all items from the List */
    void DeleteListItems();

private:
    wxListCtrl* m_pListLog;
    long        m_IndexOffset;
    size_t      m_TotalLinesFound; // Count the number of found lines/matches
    int         m_SortColumn;
    bool        m_Ascending;
    bool        m_MadeVisible;
};

#endif // THREAD_SEARCH_LOGGER_LIST_H
