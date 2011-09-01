/***************************************************************
 * Name:      ThreadSearchViewManagerBase
 * Purpose:   ThreadSearchViewManagerBase is an interface to the
 *            different graphical classes that can manage the view.
 * Author:    Jerome ANTOINE
 * Created:   2007-07-19
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef THREAD_SEARCH_VIEW_MANAGER_BASE_H
#define THREAD_SEARCH_VIEW_MANAGER_BASE_H

class ThreadSearchView;

class ThreadSearchViewManagerBase
{
public:
	enum eManagerTypes
	{
		TypeMessagesNotebook = 0,
		TypeLayout
	};

	/** BuildThreadSearchViewManagerBase
	  * Builds a ThreadSearchViewManagerMessagesNotebook or a ThreadSearchViewManagerAui pointer depending
	  * on managerType.
	  * @return ThreadSearchViewManagerBase*
	  */
	static ThreadSearchViewManagerBase* BuildThreadSearchViewManagerBase(ThreadSearchView* pView,
																		 bool              addViewToManager,
																		 eManagerTypes     managerType);

	/** Destructor. */
	virtual ~ThreadSearchViewManagerBase() {}

	eManagerTypes virtual GetManagerType() = 0;

	/** By default, view is not managed by the manager.
	  * This method adds view to manager if not already managed
	  * and given in constructor.
	  * No parameters because only m_pThreadSearchView is managed.
	  */
	virtual void AddViewToManager() = 0;

	/** By default, view is not managed by the manager.
	  * This method removes view from manager if managed.
	  * No parameters because only m_pThreadSearchView is managed
	  * and given in constructor.
	  * m_pThreadSearchView is not modified.
	  */
	virtual void RemoveViewFromManager() = 0;

	/** Return true if success. Fails if view is not managed.
	  * @param show : true => show, false => hide
	  * @return true if success.
	  */
	virtual bool ShowView(bool show = true) = 0;

	/** Return true if view is visible.
	  * @param show : true => show, false => hide
	  * @return true if view is visible.
	  */
	virtual bool IsViewShown() = 0;

protected:
	/** Constructor. */
	ThreadSearchViewManagerBase(ThreadSearchView* pThreadSearchView)
		: m_pThreadSearchView(pThreadSearchView)
		, m_IsManaged(false)
		, m_IsShown(false)
	{
	}

	ThreadSearchView* m_pThreadSearchView;
	bool              m_IsManaged;
	bool              m_IsShown;
};

#endif // THREAD_SEARCH_VIEW_MANAGER_BASE_H
