/***************************************************************
 * Name:      ThreadSearchViewManagerLayout
 * Purpose:   Implements the ThreadSearchViewManagerBase
 *            interface to make the ThreadSearchView panel
 *            managed by the wxAui layout.
 * Author:    Jerome ANTOINE
 * Created:   2007-07-19
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef THREAD_SEARCH_VIEW_MANAGER_LAYOUT_H
#define THREAD_SEARCH_VIEW_MANAGER_LAYOUT_H

#include "ThreadSearchViewManagerBase.h"

class ThreadSearchView;

class ThreadSearchViewManagerLayout : public ThreadSearchViewManagerBase
{
public:
    /** Constructor. */
    ThreadSearchViewManagerLayout(ThreadSearchView* pThreadSearchView)
        : ThreadSearchViewManagerBase(pThreadSearchView)
    {}

    /** Destructor. */
    ~ThreadSearchViewManagerLayout() override;

    eManagerTypes GetManagerType() override { return TypeLayout; }

    /** By default, view is not managed by the manager.
      * This method adds view to manager if managed.
      * No parameters because only m_pThreadSearchView is managed
      * and given in constructor.
      */
    void AddViewToManager() override;

    /** By default, view is not managed by the manager.
      * This method removes view from manager if not already managed.
      * No parameters because only m_pThreadSearchView is managed
      * and given in constructor.
      * m_pThreadSearchView is not modified.
      */
    void RemoveViewFromManager() override;

    /** Return true if success. Fails if view is not managed.
      * @param show : true => show, false => hide
      * @return true if success.
      */
    bool ShowView(bool show, bool preserveFocus) override;

    /** Return true if view is visible.
      * @return true if view is visible.
      */
    bool IsViewShown() override;

    void Raise() override;
};

#endif // THREAD_SEARCH_VIEW_MANAGER_LAYOUT_H
