/***************************************************************
 * Name:      ThreadSearchLoggerBase
 * Purpose:   ThreadSearchLoggerBase is used to search text files
 *            for text pattern.
 * Author:    Jerome ANTOINE
 * Created:   2007-04-07
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include "ThreadSearchLoggerBase.h"
#include "ThreadSearchLoggerList.h"
#include "ThreadSearchLoggerTree.h"

ThreadSearchLoggerBase* ThreadSearchLoggerBase::BuildThreadSearchLoggerBase(ThreadSearchView& threadSearchView,
																			ThreadSearch&     threadSearchPlugin,
																			eLoggerTypes      loggerType,
																			wxPanel*          pParent,
																			long              id)
{
	ThreadSearchLoggerBase* pLogger = NULL;

	if ( loggerType == TypeList )
	{
		pLogger = new ThreadSearchLoggerList(threadSearchView, threadSearchPlugin, pParent, id);
	}
	else
	{
		pLogger = new ThreadSearchLoggerTree(threadSearchView, threadSearchPlugin, pParent, id);
	}
	return pLogger;
}
