/***************************************************************
 * Name:      ThreadSearchViewManagerLayout
 * Purpose:   Implements the ThreadSearchViewManagerBase
 *            interface to make the ThreadSearchView panel
 *            managed by the layout.
 * Author:    Jerome ANTOINE
 * Created:   2007-07-19
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/


#if defined(CB_PRECOMP)
#include "sdk.h"
#endif
//#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
	#include "manager.h"
	#include "sdk_events.h"
#endif
#include "globals.h"

#include "ThreadSearchViewManagerLayout.h"

ThreadSearchViewManagerLayout::~ThreadSearchViewManagerLayout()
{
}

void ThreadSearchViewManagerLayout::AddViewToManager()
{
	if ( m_IsManaged == false )
	{
		// Docking event creation
		CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
		evt.name = _T("SnippetsSearch");
		evt.title = _("Snippets search");
		evt.pWindow = (wxWindow*)m_pThreadSearchView;
		evt.desiredSize.Set(800, 200);
		evt.floatingSize.Set(600, 200);
		evt.minimumSize.Set( 30, 40 );
		evt.stretch = true;
		evt.dockSide = CodeBlocksDockEvent::dsBottom;
		evt.shown = true;

		// Adds view to layout manager
		//- Manager::Get()->ProcessEvent(evt);

		m_IsManaged = true;
		m_IsShown   = true;
	}
}

void ThreadSearchViewManagerLayout::RemoveViewFromManager()
{
	if ( m_IsManaged == true )
	{
		// Undocking event creation
		CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
		evt.pWindow = (wxWindow*)m_pThreadSearchView;
		evt.shown = false;

		// Removes view from layout manager
		//-Manager::Get()->ProcessEvent(evt);

		m_IsManaged = false;
		m_IsShown   = false;
	}
}


bool ThreadSearchViewManagerLayout::ShowView(bool show)
{
	if ( (m_IsManaged == false) || (show == IsViewShown()) )
		return false;

	CodeBlocksDockEvent evt(show ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
	evt.pWindow = (wxWindow*)m_pThreadSearchView;
	evt.shown = show;
	//-Manager::Get()->ProcessEvent(evt);

	m_IsShown = show;

	return true;
}


bool ThreadSearchViewManagerLayout::IsViewShown()
{
	// m_IsShown is not sufficient because user can close the view with the close cross
	// so we use IsWindowReallyShown to give correct result.
	return m_IsShown && IsWindowReallyShown((wxWindow*)m_pThreadSearchView);
}

