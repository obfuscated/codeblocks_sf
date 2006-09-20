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

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include <wx/arrstr.h>
#include "cbplugin.h" // the base class we 're inheriting
#include "globals.h"

class wxMenuBar;
class wxMenu;
class wxToolBar;
class FileTreeData;
class wxCommandEvent;
class wxUpdateUIEvent;
class CodeBlocksEvent;
class ToDoListView;
class FileTreeData;


class ToDoList : public cbPlugin
{
	public:
		ToDoList();
		~ToDoList();
		int Configure();
		void BuildMenu(wxMenuBar* menuBar);
		void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
		bool BuildToolBar(wxToolBar* toolBar);
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
	private:
		void OnViewList(wxCommandEvent& event);
		void OnAddItem(wxCommandEvent& event);
		void OnReparse(CodeBlocksEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
        void LoadTypes();
        void SaveTypes();
		ToDoListView* m_pListLog;
		int m_ListPageIndex;
		bool m_AutoRefresh;
		wxArrayString m_Types;
		DECLARE_EVENT_TABLE()
};

#endif // TODOLIST_H

