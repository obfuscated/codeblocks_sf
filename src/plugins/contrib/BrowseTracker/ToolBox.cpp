/*
	This file is part of BrowseTracker, a plugin for Code::Blocks
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
// RCS-ID: $Id: RubyIdbg.cpp 23 2007-12-21 16:51:03Z Pecan $


#include <sdk.h> // Code::Blocks SDK
//#include <c:\Usr\Proj\cbBeta\trunk\src\src\wxAUI\manager.h>
#include <wxAUI/manager.h>

#include "ToolBox.h"
#include "Version.h"


// ----------------------------------------------------------------------------
ToolBox::ToolBox()
// ----------------------------------------------------------------------------
{
    //ctor
    m_pAuiMgr = (wxAuiManager*)SearchEvtHandlersForClass(wxT("wxAuiManager"));
}

// ----------------------------------------------------------------------------
ToolBox::~ToolBox()
// ----------------------------------------------------------------------------
{
    //dtor
}
// ----------------------------------------------------------------------------
wxWindow* ToolBox::FindWindowRecursively(const wxWindow* pwin, const wxEvtHandler* evtHandler, bool show)
// ----------------------------------------------------------------------------
{
    if ( pwin )
    {
        if (show){
            const wxChar* pClassName = 0;
            pClassName = pwin->GetClassInfo()->GetClassName();
            //-LOGIT( _T("ClassName[%s]"), pClassName );
            LOGIT( _T("%-24s@[%p] name[%s] label[%s]"), pClassName, pwin, pwin->GetName().c_str(),
                    pwin->GetLabel().c_str() );
            ShowWindowEvtHandlers(pwin);
        }
        // see if this is the one we're looking for
        if ( SearchEvtHandlersFor(pwin, evtHandler) )
        //-if ( pwin->GetEventHandler() == evtHandler )
            return (wxWindow *)pwin;

        // It wasn't, so check all its children
        for ( wxWindowList::compatibility_iterator node = pwin->GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            // recursively check each child
            wxWindow *win = (wxWindow *)node->GetData();
            wxWindow *retwin = FindWindowRecursively(win, evtHandler, show);
            if (retwin)
                return retwin;
        }
    }

    // Not found
    return NULL;
}
// ----------------------------------------------------------------------------
wxWindow* ToolBox::FindWindowByEvtHandler(wxEvtHandler* evtHandler, bool show)
// ----------------------------------------------------------------------------
{
    // Find the window that owns this event handler

    if ( !evtHandler )
    {
        return NULL;
    }

    // start at very top of wx's windows
    for ( wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetFirst();
          node;
          node = node->GetNext() )
    {
        // recursively check each window & its children
        wxWindow* win = node->GetData();
        wxWindow* retwin = FindWindowRecursively(win, evtHandler, show);
        if (retwin)
            return retwin;
    }

    return NULL;
}
// ----------------------------------------------------------------------------
wxWindow* ToolBox::ShowWindowsRecursively(const wxWindow* parent, int level)
// ----------------------------------------------------------------------------
{
    // a logger to show all the windows in the app (except wxAuiManager windows)

    if ( parent )
    {
            const wxChar* pClassName = 0;
            pClassName = parent->GetClassInfo()->GetClassName();
            LOGIT( _T("[%d] %-24s@[%p] name[%s] label[%s]"),
                    level,
                    pClassName, parent, parent->GetName().c_str(),
                    parent->GetLabel().Left(24).c_str() );
            ShowWindowEvtHandlers(parent);

        //  check all its children
        ++level;
        for ( wxWindowList::compatibility_iterator node = parent->GetChildren().GetFirst();
                node; node = node->GetNext() )
        {
            // recursively check each child
            wxWindow *win = (wxWindow *)node->GetData();
            ShowWindowsRecursively( win, level );
        }//for
        --level;
    }

    return NULL;
}
// ----------------------------------------------------------------------------
wxWindow* ToolBox::ShowWindowsAndEvtHandlers()
// ----------------------------------------------------------------------------
{
    // a logger to show the relationship of window to their event handlers

    int i = 0; int nLevel = 0;
    // start at very top of wx's windows
    for ( wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetFirst();
          node; node = node->GetNext() )
    {
        ++i;
        LOGIT( _T("ShowWindowsAndEvtHandlers[%d]"),i );
        // recursively check each window & its children
        wxWindow* win = node->GetData();
        ShowWindowsRecursively( win, nLevel );
        nLevel = 0;
    }

    return NULL;
}
// ----------------------------------------------------------------------------
void ToolBox::ShowWindowEvtHandlers(const wxWindow* win)
// ----------------------------------------------------------------------------
{
    // show only the event handlers

    // step down the event handlers
    //-wxEvtHandler* pEvtHandler = pCodeSnippetsPlgn->GetNextHandler();
    wxEvtHandler* pEvtHandler = win->GetEventHandler();
    while (pEvtHandler)
    {
        //Returns the string form of the class name.
        const wxChar* pClassName = 0;
        if (pEvtHandler)
        {    pClassName = pEvtHandler->GetClassInfo()->GetClassName();
            LOGIT( _T("\tEvtHandler[%p] ClassName[%s]"), pEvtHandler, pClassName );
        }
        pEvtHandler = pEvtHandler->GetNextHandler();
    }
}
// ----------------------------------------------------------------------------
bool ToolBox::SearchEvtHandlersFor(const wxWindow* win, const wxEvtHandler* evtHandler)
// ----------------------------------------------------------------------------
{
    // TRUE if this window owns this event handler

    //-wxEvtHandler* pEvtHandler = pCodeSnippetsPlgn->GetNextHandler();
    wxEvtHandler* pEvtHandler = win->GetEventHandler();
    while (pEvtHandler)
    {
        if (pEvtHandler == evtHandler )
            return true;
        pEvtHandler = pEvtHandler->GetNextHandler();
    }
    return false;
}
// ----------------------------------------------------------------------------
wxEvtHandler* ToolBox::SearchEvtHandlersForClass( const wxString className)
// ----------------------------------------------------------------------------
{
    // Step down the event handlers looking for a handler by this class name
    // Most event handlers have a classname == wxEventHandler, but a few
    // are special, eg, wxAuiManager

    wxWindow* pCBwin = Manager::Get()->GetAppWindow();
    wxEvtHandler* pEvtHandler = ::wxGetTopLevelParent(pCBwin)->GetEventHandler();
    while (pEvtHandler)
    {
        #if defined(LOGGING)
        //LOGIT( _T("Class[%p][%s]"), pEvtHandler, pEvtHandler->GetClassInfo()->GetClassName() );
        #endif
        if (pEvtHandler->GetClassInfo()->GetClassName() == className )
            return pEvtHandler;
        pEvtHandler = pEvtHandler->GetNextHandler();
    }
    return pEvtHandler;
}
// ----------------------------------------------------------------------------
wxWindow* ToolBox::GetWindowFromAuiManager( wxString className )
// ----------------------------------------------------------------------------
{
    // works by grabbing the wxEventHandler window of wxAuiManager

    wxPaneInfo* pPaneInfo = 0;
    if ( not m_pAuiMgr )
        m_pAuiMgr = (wxAuiManager*)SearchEvtHandlersForClass(wxT("wxAuiManager"));

    if ( not m_pAuiMgr )
    {
        #if defined(LOGGING)
        LOGIT( _T("ToolBox Error: wxAuiManager *NOT* found.") );
        #endif
        return 0;
    }

    wxPaneInfo& info = m_pAuiMgr->GetPane( className );
    pPaneInfo = &info;
    //#if defined(LOGGING)
    //LOGIT( _T("wxAuiManager:name[%s]caption[%s]window[%p]frame[%p]"),
    //        info.name.c_str(),
    //        info.caption.c_str(),
    //        info.window,
    //        info.frame );
    //#endif

    if (not pPaneInfo)
        return 0;

        // ------------------------------------------------------------------
        // wxAuiManager::wxPanelInfo class data
        // ------------------------------------------------------------------
        //wxPaneInfo public:
        //wxString name;        // name of the pane
        //wxString caption;     // caption displayed on the window
        //
        //wxWindow* window;     // window that is in this pane
        //wxWindow* frame;      // floating frame window that holds the pane
        //unsigned int state;   // a combination of wxPaneState values
        //
        //int dock_direction;   // dock direction (top, bottom, left, right, center)
        //int dock_layer;       // layer number (0 = innermost layer)
        //int dock_row;         // row number on the docking bar (0 = first row)
        //int dock_pos;         // position inside the row (0 = first position)
        //
        //wxSize best_size;     // size that the layout engine will prefer
        //wxSize min_size;      // minimum size the pane window can tolerate
        //wxSize max_size;      // maximum size the pane window can tolerate
        //
        //wxPoint floating_pos; // position while floating
        //wxSize floating_size; // size while floating
        //int dock_proportion;  // proportion while docked
        //
        //wxPaneButtonArray buttons; // buttons on the pane
        //
        //wxRect rect;              // current rectangle (populated by wxAUI)
        // ------------------------------------------------------------------

    #if defined(LOGGING)
    //LOGIT( _T("wxAuiManager:name[%s]caption[%s]window[%p]frame[%p]"),
    //            pPaneInfo->name.c_str(),
    //            pPaneInfo->caption.c_str(),
    //            pPaneInfo->window,
    //            pPaneInfo->frame );
    #endif
    return pPaneInfo->window;
}
// ----------------------------------------------------------------------------
wxWindow* ToolBox::FindWindowRecursively(const wxWindow* parent, const wxString& partialLabel)
// ----------------------------------------------------------------------------
{
    if ( parent )
    {
        #if defined(LOGGING)
        LOGIT( _T("Label[%s]"), parent->GetLabel().c_str() );
        #endif
        // see if this is the one we're looking for
        if ( parent->GetLabel().Contains(partialLabel) )
            return (wxWindow *)parent;

        // It wasn't, so check all its children
        for ( wxWindowList::compatibility_iterator node = parent->GetChildren().GetFirst();
              node; node = node->GetNext() )
        {
            // recursively check each child
            wxWindow *win = (wxWindow *)node->GetData();
            wxWindow *retwin = FindWindowRecursively(win, partialLabel);
            if (retwin)
                return retwin;
        }
    }

    // Not found
    return NULL;
}
// ----------------------------------------------------------------------------
wxWindow* ToolBox::WinExists(wxString partialLabel)
// ----------------------------------------------------------------------------
{

    // start at very top of wx's windows
    for ( wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetFirst();
          node; node = node->GetNext() )
    {
        // recursively check each window & its children
        wxWindow* win = node->GetData();
        wxWindow* retwin = FindWindowRecursively(win, partialLabel);
        if (retwin)
            return retwin;
    }

    return NULL;
}
// ----------------------------------------------------------------------------
//            wxWindow* win = ::wxFindWindowAtPoint(event.GetPosition());
//            LOGIT( _T("win[%p],Name[%s],Label[%s]"),
//                    win ? win->GetName().c_str():wxEmptyString,
//                    win ? win->GetLabel().c_str():wxEmptyString
//                );
//            wxPoint pt;
//            win = ::wxFindWindowAtPointer( pt );
//            LOGIT( _T("win[%p],Name[%s],Label[%s]"),
//                    win ? win->GetName().c_str():wxEmptyString,
//                    win ? win->GetLabel().c_str():wxEmptyString
//                );
//
//            win = ::wxGetActiveWindow();
//            LOGIT( _T("win[%p],Name[%s],Label[%s]"),
//                    win ? win->GetName().c_str():wxEmptyString,
//                    win ? win->GetLabel().c_str():wxEmptyString
//                );
//            win =  ::wxGetTopLevelParent(pControl);
//            LOGIT( _T("win[%p],Name[%s],Label[%s]"),
//                    win ? win->GetName().c_str():wxEmptyString,
//                    win ? win->GetLabel().c_str():wxEmptyString
//                );
