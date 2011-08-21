/***************************************************************
 * Name:      ThreadSearchLoggerBase
 * Purpose:   ThreadSearchLoggerBase is an interface to the
 *            different graphical controls that are able to
 *            manage ThreadSearchEvents received by the view.
 * Author:    Jerome ANTOINE
 * Created:   2007-07-28
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef THREAD_SEARCH_LOGGER_BASE_H
#define THREAD_SEARCH_LOGGER_BASE_H

#include "InsertIndexManager.h"

class wxWindow;
class wxPanel;
class wxPoint;
class wxEvtHandler;

class ThreadSearch;
class ThreadSearchView;
class ThreadSearchEvent;
class ThreadSearchFindData;


class ThreadSearchLoggerBase
{
public:
    enum eLoggerTypes
    {
        TypeList = 0,
        TypeTree
    };

    /** BuildThreadSearchLoggerBase
      * Builds a ThreadSearchLoggerList or a ThreadSearchLoggerTree pointer depending on loggerType.
      * @return ThreadSearchLoggerBase*
      */
    static ThreadSearchLoggerBase* BuildThreadSearchLoggerBase(ThreadSearchView& threadSearchView,
                                                               ThreadSearch&     threadSearchPlugin,
                                                               eLoggerTypes      loggerType,
                                                               InsertIndexManager::eFileSorting fileSorting,
                                                               wxPanel* pParent,
                                                               long id);

    /** Destructor. */
    virtual ~ThreadSearchLoggerBase() {}

    eLoggerTypes virtual GetLoggerType() = 0;

    /** Called by ThreadSearchView when new settings are applied. */
    virtual void Update();

    /** Called by ThreadSearchView to process a ThreadSearchEvent
      * sent by worker thread.
      */
    virtual void OnThreadSearchEvent(const ThreadSearchEvent& event) = 0;

    /** Removes all items from logger. */
    virtual void Clear() = 0;

    /** Called on search begin to prepare logger. */
    virtual void OnSearchBegin(const ThreadSearchFindData& findData) = 0;

    /** Called on search end */
    virtual void OnSearchEnd() {};

    /** Returns logger window. */
    virtual wxWindow* GetWindow() = 0;

    /** Sets focus on logger window. */
    virtual void      SetFocus()  = 0;

protected:
    /** Constructor. */
    ThreadSearchLoggerBase(ThreadSearchView& threadSearchView,
                           ThreadSearch&                    threadSearchPlugin,
                           InsertIndexManager::eFileSorting fileSorting)
                           : m_ThreadSearchView  (threadSearchView)
                           , m_ThreadSearchPlugin(threadSearchPlugin)
                           , m_IndexManager(fileSorting)
    {}

    /** Dynamic events connection. */
    virtual void ConnectEvents(wxEvtHandler* pEvtHandler) = 0;

    /** Dynamic events disconnection. */
    virtual void DisconnectEvents(wxEvtHandler* pEvtHandler) = 0;

    /** Displays contextual menu. */
    void ShowMenu(const wxPoint& point);

    ThreadSearchView& m_ThreadSearchView;
    ThreadSearch&     m_ThreadSearchPlugin;
    InsertIndexManager m_IndexManager;
};

#endif // THREAD_SEARCH_LOGGER_BASE_H
