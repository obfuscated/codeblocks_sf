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
#include "cbplugin.h" // the base class we're inheriting
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
        // only parse files when C::B app is already start up
        void OnAppDoneStartup(CodeBlocksEvent& event);

        // menu item click event
        // menu item click (toggle the show status of the todo list control)
        void OnViewList(wxCommandEvent& event);
        // menu item click to add a todo item in the source file
        void OnAddItem(wxCommandEvent& event);

        // event handler of the C::B editor/project event
        // some status has changed, so we need to update the todo list control, currently, project
        // close/activate, project file added/removed will all fire this function
        void OnReparse(CodeBlocksEvent& event);

        // parse the current/active editor, currently, editor open/save/activated/close will fire
        // this function
        void OnReparseCurrent(CodeBlocksEvent& event);

        // update the menu status
        // update the menu items (check status)
        void OnUpdateUI(wxUpdateUIEvent& event);
        // update the menu items (this manu item is valid when an buildin editor is active)
        void OnUpdateAdd(wxUpdateUIEvent& event);
        //write and load from configuration file
        void LoadUsers(); // different user can add todos
        void SaveUsers();
        void LoadTypes(); // types are some thing like fixme, todo, note .....
        void SaveTypes();

        // parse current editor, if force is true, we need to update the items of the current editor
        void ParseCurrent(bool forced = false);
        // parse all the files involved?
        void Parse();

        // ListCtrl of all the Todo items
        ToDoListView* m_pListLog;
        // the slot index in Logs & others panel
        int m_ListPageIndex;
        // when a new todo item is added, do we need to refresh the list (parse the files)
        bool m_AutoRefresh;
        // set true after C::B app is done
        bool m_InitDone;
        // delay parsing the while files if this variable is set to true, this usually happens
        // the project is currently loading or C::B is not fully start up.
        bool m_ParsePending;
        // float window or a table page in Logs & others
        bool m_StandAlone;
        // who wrote the todos
        wxArrayString m_Users;
        // many kinds of todos, like: fixme, todo, note.....
        wxArrayString m_Types;
        wxTimer m_timer;

        DECLARE_EVENT_TABLE()
};

#endif // TODOLIST_H

