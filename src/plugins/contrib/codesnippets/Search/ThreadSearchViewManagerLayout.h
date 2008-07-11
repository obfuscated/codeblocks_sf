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
	virtual ~ThreadSearchViewManagerLayout();

	eManagerTypes GetManagerType() {return TypeLayout;}

	/** By default, view is not managed by the manager.
	  * This method adds view to manager if managed.
	  * No parameters because only m_pThreadSearchView is managed
	  * and given in constructor.
	  */
	virtual void AddViewToManager();

	/** By default, view is not managed by the manager.
	  * This method removes view from manager if not already managed.
	  * No parameters because only m_pThreadSearchView is managed
	  * and given in constructor.
	  * m_pThreadSearchView is not modified.
	  */
	virtual void RemoveViewFromManager();

	/** Return true if success. Fails if view is not managed.
	  * @param show : true => show, false => hide
	  * @return true if success.
	  */
	virtual bool ShowView(bool show = true);

	/** Return true if view is visible.
	  * @param show : true => show, false => hide
	  * @return true if view is visible.
	  */
	bool IsViewShown();
};

#endif // THREAD_SEARCH_VIEW_MANAGER_LAYOUT_H
