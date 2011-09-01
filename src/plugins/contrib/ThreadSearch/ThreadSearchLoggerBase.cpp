/***************************************************************
 * Name:      ThreadSearchLoggerBase
 * Purpose:   ThreadSearchLoggerBase is used to search text files
 *            for text pattern.
 * Author:    Jerome ANTOINE
 * Created:   2007-04-07
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include <wx/gdicmn.h>
#include <wx/menu.h>

#include "ThreadSearchLoggerBase.h"
#include "ThreadSearchLoggerList.h"
#include "ThreadSearchLoggerTree.h"
#include "ThreadSearchView.h"
#include "ThreadSearchControlIds.h"
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
    Clear();
    m_IndexManager.SetFileSorting(m_ThreadSearchPlugin.GetFileSorting());
}


void ThreadSearchLoggerBase::ShowMenu(const wxPoint& point)
{
    bool enable = !m_ThreadSearchView.IsSearchRunning();
    wxMenu menu(_(""));
    wxMenuItem* menuItem = menu.Append(idMenuCtxDeleteItem, _("&Delete item"));
    menuItem->Enable(enable);
    menuItem = menu.Append(idMenuCtxDeleteAllItems, _("Delete &all items"));
    menuItem->Enable(enable);
    GetWindow()->PopupMenu(&menu, point);
}
