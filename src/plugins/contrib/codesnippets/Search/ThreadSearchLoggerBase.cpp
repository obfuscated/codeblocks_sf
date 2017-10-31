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
#include "ThreadSearch.h"

ThreadSearchLoggerBase* ThreadSearchLoggerBase::BuildThreadSearchLoggerBase(ThreadSearchView& threadSearchView,
																			ThreadSearch&     threadSearchPlugin,
																			eLoggerTypes      loggerType,
															 InsertIndexManager::eFileSorting fileSorting,
																			wxPanel*          pParent,
																			long              id)
{
	ThreadSearchLoggerBase* pLogger = NULL;

	if ( loggerType == TypeList )
	{
		pLogger = new ThreadSearchLoggerList(threadSearchView, threadSearchPlugin, fileSorting , pParent, id);
	}
	else
	{
		pLogger = new ThreadSearchLoggerTree(threadSearchView, threadSearchPlugin, fileSorting , pParent, id);
	}
	return pLogger;
}


void ThreadSearchLoggerBase::Update()
{
	if ( m_ThreadSearchPlugin.GetFileSorting() != m_IndexManager.GetFileSorting() )
	{
		Clear();
		m_IndexManager.SetFileSorting(m_ThreadSearchPlugin.GetFileSorting());
	}
}
