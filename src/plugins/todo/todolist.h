/***************************************************************
 * Name:      todolist.h
 * Purpose:   Code::Blocks plugin
 * Author:    Yiannis Mandravellos <mandrav@codeblocks.org>
 * Created:   11/21/03 14:01:50
 * Copyright: (c) Yiannis Mandravellos
 * License:   GPL
 **************************************************************/

#ifndef TODOLIST_H
#define TODOLIST_H

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma interface "todolist.h"
#endif
// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/menu.h>
	#include <wx/toolbar.h>
#endif

#include <settings.h> // much of the SDK is here
#include <cbPlugin.h> // the base class we 're inheriting

class PLUGIN_EXPORT ToDoList : public cbPlugin
{
	public:
		ToDoList();
		~ToDoList();
		int Configure(){ return -1; }
		void BuildMenu(wxMenuBar* menuBar);
		void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg);
		void BuildToolBar(wxToolBar* toolBar);
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(); // fires when the plugin is released from the application
	protected:
		void OnAddItem(wxCommandEvent& event);
		void OnViewList(wxCommandEvent& event);
	private:
		wxMenu* m_pMenu;
		DECLARE_EVENT_TABLE()
};

extern "C"
{
	cbPlugin* PLUGIN_EXPORT GetPlugin();
};

#endif // TODOLIST_H

