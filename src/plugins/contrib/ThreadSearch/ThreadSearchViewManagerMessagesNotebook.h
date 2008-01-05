/***************************************************************
 * Name:      ThreadSearchViewManagerMessagesNotebook
 * Purpose:   Implements the ThreadSearchViewManagerBase
 *            interface to make the ThreadSearchView panel
 *            managed by the Messages notebook.
 * Author:    Jerome ANTOINE
 * Created:   2007-07-19
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/


#ifndef THREAD_SEARCH_VIEW_MANAGER_MESSAGES_NOTEBOOK_H
#define THREAD_SEARCH_VIEW_MANAGER_MESSAGES_NOTEBOOK_H

#include "logger.h"

#include "ThreadSearchViewManagerBase.h"

class wxWindow;
class ThreadSearchView;
class ThreadSearchLogger;


class ThreadSearchViewManagerMessagesNotebook : public ThreadSearchViewManagerBase
{
public:
	/** Constructor. */
	ThreadSearchViewManagerMessagesNotebook(ThreadSearchView* pThreadSearchView)
		: ThreadSearchViewManagerBase(pThreadSearchView)
	{}

	/** Destructor. */
	virtual ~ThreadSearchViewManagerMessagesNotebook();

	eManagerTypes GetManagerType() {return TypeMessagesNotebook;}

	/** By default, view is not managed by the manager.
	  * This method adds view to manager if not already managed.
	  * No parameters because only m_pThreadSearchView is managed
	  * and given in constructor.
	  */
	virtual void AddViewToManager();

	/** By default, view is not managed by the manager.
	  * This method removes view from manager if managed.
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

#endif // THREAD_SEARCH_VIEW_MANAGER_MESSAGES_NOTEBOOK_H
