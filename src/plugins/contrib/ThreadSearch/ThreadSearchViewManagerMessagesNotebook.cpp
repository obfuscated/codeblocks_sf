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


#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
	#include "configmanager.h"
	#include "manager.h"
#endif

#include "ThreadSearchView.h"
#include "ThreadSearchViewManagerMessagesNotebook.h"


ThreadSearchViewManagerMessagesNotebook::~ThreadSearchViewManagerMessagesNotebook()
{
}


void ThreadSearchViewManagerMessagesNotebook::AddViewToManager()
{
	if ( m_IsManaged == false )
	{
		// Creates log image
		wxBitmap bmp;
		wxString prefix = ConfigManager::GetDataFolder() + _T("/images/16x16/");
		bmp = cbLoadBitmap(prefix + _T("filefind.png"), wxBITMAP_TYPE_PNG);

		// Adds log to C::B Messages notebook
		CodeBlocksLogEvent evtShow(cbEVT_ADD_LOG_WINDOW, m_pThreadSearchView, wxString(_T("Thread search")), &bmp);
		Manager::Get()->ProcessEvent(evtShow);

		CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_pThreadSearchView);
		Manager::Get()->ProcessEvent(evtSwitch);

		// Status update
		m_IsManaged = true;
		m_IsShown   = true;
	}
}

void ThreadSearchViewManagerMessagesNotebook::RemoveViewFromManager()
{
	if ( m_IsManaged == true )
	{
		// Status update
		m_IsManaged = false;
		m_IsShown   = false;

		// Removes ThreadSearch panel from C::B Messages notebook
		// Reparent call to avoid m_pThreadSearchView deletion
		CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_pThreadSearchView);
		Manager::Get()->ProcessEvent(evt);
		m_pThreadSearchView->Reparent(Manager::Get()->GetAppWindow());
		m_pThreadSearchView->Show(false);
	}
}


bool ThreadSearchViewManagerMessagesNotebook::ShowView(bool show)
{
	if ( show == IsViewShown() )
		return false;

	// m_IsManaged is updated in called methods
	if ( show == true )
	{
		if ( m_IsManaged == true )
		{
			CodeBlocksLogEvent evtShow(cbEVT_SHOW_LOG_MANAGER);
			Manager::Get()->ProcessEvent(evtShow);
			CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_pThreadSearchView);
			Manager::Get()->ProcessEvent(evtSwitch);
			m_IsShown = true;
		}
		else
		{
			AddViewToManager();
		}
	}
	else
	{
		RemoveViewFromManager();
	}

	return true;
}


bool ThreadSearchViewManagerMessagesNotebook::IsViewShown()
{
	return m_IsShown && IsWindowReallyShown((wxWindow*)m_pThreadSearchView);
}
