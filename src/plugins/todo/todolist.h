/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

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
        virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
        void BuildMenu(wxMenuBar* menuBar);
        void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
        void OnAttach(); // fires when the plugin is attached to the application
        void OnRelease(bool appShutDown); // fires when the plugin is released from the application
    private:
        void OnAppDoneStartup(CodeBlocksEvent& event);
        void OnViewList(wxCommandEvent& event);
        void OnAddItem(wxCommandEvent& event);
        void OnReparse(CodeBlocksEvent& event);
        void OnReparseCurrent(CodeBlocksEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnUpdateAdd(wxUpdateUIEvent& event);
        void OnStartParsingProjects(wxTimerEvent& event);
        void LoadUsers();
        void SaveUsers();
        void LoadTypes();
        void SaveTypes();
        void ParseCurrent(bool forced = false);
        void Parse();

        ToDoListView* m_pListLog;
        int m_ListPageIndex;
        bool m_AutoRefresh;
        bool m_InitDone;
        bool m_ParsePending;
        bool m_StandAlone;
        wxArrayString m_Users;
        wxArrayString m_Types;
        wxTimer m_timer;

        DECLARE_EVENT_TABLE()
};

#endif // TODOLIST_H

