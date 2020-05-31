/***************************************************************
 * Name:      ThreadSearchLoggerBase
 * Purpose:   ThreadSearchLoggerBase is used to search text files
 *            for text pattern.
 * Author:    Jerome ANTOINE
 * Created:   2007-04-07
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/menu.h>

#include "ThreadSearchLoggerBase.h"
#include "ThreadSearchLoggerList.h"
#include "ThreadSearchLoggerSTC.h"
#include "ThreadSearchLoggerTree.h"
#include "ThreadSearchView.h"
#include "ThreadSearchControlIds.h"
#include "ThreadSearch.h"

ThreadSearchLoggerBase* ThreadSearchLoggerBase::Build(ThreadSearchView &threadSearchView,
                                                      ThreadSearch &threadSearchPlugin,
                                                      eLoggerTypes loggerType,
                                                      InsertIndexManager::eFileSorting fileSorting,
                                                      wxWindow *pParent, long id)
{

    switch (loggerType)
    {
        case TypeList:
            return new ThreadSearchLoggerList(threadSearchView, threadSearchPlugin, fileSorting , pParent, id);
        case TypeTree:
            return new ThreadSearchLoggerTree(threadSearchView, threadSearchPlugin, fileSorting , pParent, id);
        case TypeSTC:
            return new ThreadSearchLoggerSTC(threadSearchView, threadSearchPlugin, fileSorting , pParent, id);
        default:
            return nullptr;
    }
}

ThreadSearchLoggerBase::ThreadSearchLoggerBase(wxWindow *parent,
                                               ThreadSearchView &threadSearchView,
                                               ThreadSearch &threadSearchPlugin,
                                               InsertIndexManager::eFileSorting fileSorting) :
    wxPanel(parent, -1, wxDefaultPosition, wxSize(1,1)),
    m_ThreadSearchView(threadSearchView),
    m_ThreadSearchPlugin(threadSearchPlugin),
    m_IndexManager(fileSorting)
{
}

void ThreadSearchLoggerBase::SetupSizer(wxWindow *control)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(control, 1, wxEXPAND|wxFIXED_MINSIZE, 0);
    SetAutoLayout(true);
    SetSizer(sizer);
}

void ThreadSearchLoggerBase::Update()
{
    Clear();
    m_IndexManager.SetFileSorting(m_ThreadSearchPlugin.GetFileSorting());
}


void ThreadSearchLoggerBase::ShowMenu(const wxPoint& point, bool hasSelection, bool hasItems)
{
    const bool enable = !m_ThreadSearchView.IsSearchRunning() && hasItems;

    wxMenu menu;
    wxMenuItem* menuItem = menu.Append(controlIDs.Get(ControlIDs::idMenuCtxDeleteItem), _("&Delete item"));
    menuItem->Enable(enable && hasSelection);
    menuItem = menu.Append(controlIDs.Get(ControlIDs::idMenuCtxDeleteAllItems), _("Delete &all items"));
    menuItem->Enable(enable);
    GetWindow()->PopupMenu(&menu, point);
}
