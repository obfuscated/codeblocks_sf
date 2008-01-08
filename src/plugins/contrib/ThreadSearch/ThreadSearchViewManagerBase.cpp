/***************************************************************
 * Name:      ThreadSearchViewManagerBase
 * Purpose:   ThreadSearchViewManagerBase is an interface to the
 *            different graphical classes that can manage the view.
 * Author:    Jerome ANTOINE
 * Created:   2007-07-19
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include "ThreadSearchViewManagerBase.h"
#include "ThreadSearchViewManagerMessagesNotebook.h"
#include "ThreadSearchViewManagerLayout.h"

ThreadSearchViewManagerBase* ThreadSearchViewManagerBase::BuildThreadSearchViewManagerBase(
																			ThreadSearchView* pView,
																			bool              addViewToManager,
																			eManagerTypes     managerType)
{
	ThreadSearchViewManagerBase* pMgr = 0;

	if ( managerType == TypeMessagesNotebook )
	{
		pMgr = new ThreadSearchViewManagerMessagesNotebook(pView);
	}
	else
	{
		// ThreadSearchViewManagerLayout is the default view manager
		// No error management on managerType
		pMgr = new ThreadSearchViewManagerLayout(pView);
	}
	if ( addViewToManager == true )
	{
		pMgr->AddViewToManager();
	}
	return pMgr;
}
