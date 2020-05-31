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
    ThreadSearchViewManagerLayout(ThreadSearchView* pThreadSearchView)
        : ThreadSearchViewManagerBase(pThreadSearchView)
    {}
    ~ThreadSearchViewManagerLayout() override;

    eManagerTypes GetManagerType() override { return TypeLayout; }
    void AddViewToManager() override;
    void RemoveViewFromManager() override;
    bool ShowView(uint32_t flags) override;
    bool IsViewShown() override;
    void Raise() override;
};

#endif // THREAD_SEARCH_VIEW_MANAGER_LAYOUT_H
