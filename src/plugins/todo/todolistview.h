/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef TODOLISTVIEW_H
#define TODOLISTVIEW_H

#include <wx/dynarray.h> // WX_DECLARE_OBJARRAY
#include <wx/string.h>

#include <vector>
#include <map>

#include "loggers.h"

class cbEditor;
class wxArrayString;
class wxCommandEvent;
class wxListEvent;
class wxComboBox;
class wxButton;
class wxPanel;
class wxStaticText;

// an item is one record in the file, such as a fixme, it can have several properties, such as
// the type (todo, note, fixme..), the user (who wrote the item) and the date, all its properties
// are wrappered in the ToDoItem struct
struct ToDoItem
{
    wxString type;
    wxString text;
    wxString user;
    wxString filename;
    wxString lineStr;
    wxString priorityStr;
    wxString date;
    int line;
    int priority;
};
// each source file can have several ToDoItems, so we use a Map structure to record all the items
typedef std::map<wxString,std::vector<ToDoItem> > TodoItemsMap;
WX_DECLARE_OBJARRAY(ToDoItem, ToDoItems);

// when user click the "Types" button on the Todo list control panel, it will show a dialog, the
// dialog can let user to filter which types will be shown in the Todo list.
class CheckListDialog : public wxDialog
{
    public:
        CheckListDialog( wxWindow*       parent,
                         wxWindowID      id    = wxID_ANY,
                         const wxString& title = wxEmptyString,
                         const wxPoint&  pos   = wxDefaultPosition,
                         const wxSize&   size  = wxSize(150,180),
                         long            style = 0 );
        ~CheckListDialog();

        virtual void OkOnButtonClick( wxCommandEvent& event );

        void AddItem(const wxArrayString& items) { m_checkList->InsertItems(items, 0); }
        void Clear()                             { m_checkList->Clear();               }

        bool          IsChecked(const wxString& item) const;
        wxArrayString GetChecked() const;
        void          SetChecked(const wxArrayString& items);

    protected:
        wxCheckListBox* m_checkList;
        wxButton*       m_okBtn;

    private:
};

// the list control to show all the todo items
class ToDoListView : public wxEvtHandler, public ListCtrlLogger
{
    public:
        ToDoListView(const wxArrayString& titles, const wxArrayInt& widths, const wxArrayString& types);
        ~ToDoListView();
        virtual wxWindow* CreateControl(wxWindow* parent);
        void DestroyControls(bool control);

        // parse all the sources
        void Parse();
        // if forced == true, we need to reparse the editor, otherwise, if the same editor, we do
        // not rebuild the list
        void ParseCurrent(bool forced);
        wxWindow* GetWindow() { return m_pPanel; }

        CheckListDialog * m_pAllowedTypesDlg;
    private:

        // reset the user selection list
        void LoadUsers();
        // Fill the list control by using the data in m_ItemsMap
        void FillList();
        // sort the items (m_Items)
        void SortList();
        // will only be called by FillList()
        void FillListControl();


        // parse the specified editor
        void ParseEditor(cbEditor* pEditor);
        // parse the file on hard disk, if the file is opened in editor, then we should call ParseEditor
        void ParseFile(const wxString& filename);
        // this actually parse the buffer, and fill the items map
        void ParseBuffer(const wxString& buffer, const wxString& filename);
        // ensure the ith element of the list control is shown
        void FocusEntry(size_t index);


        // GUI event handler
        // either source or user selection changed
        void OnComboChange(wxCommandEvent& event);
        // when a list item is selected (single click)
        void OnListItemSelected(wxCommandEvent& event);
        // refresh the list
        void OnButtonRefresh(wxCommandEvent& event);
        // select which types need to show
        void OnButtonTypes(wxCommandEvent& event);
        // user double click on the entry, or hit the Enter key on the keyboard
        void OnDoubleClick( wxCommandEvent& event );
        // sort the column
        void OnColClick( wxListEvent& event );

        wxWindow*            m_pPanel;
        // a map file->vector<Items>
        TodoItemsMap         m_ItemsMap;
        // this is all the todo items need to show on the list control
        ToDoItems            m_Items;

        // GUI
        // show item's source, whether it show the current file's items, or current target's items
        // or current project's items.
        wxComboBox*          m_pSource;
        // user filter, we can show only the specified todo items belongs to a single user
        wxComboBox*          m_pUser;
        wxStaticText*        m_pTotal;

        // type string array: such as  todo, readme, note, fixme, and so on
        const wxArrayString& m_Types;

        wxString             m_LastFile;
        // if this variable is true, we don't actually do the parse (this avoid recursive parsing
        // files)
        bool                 m_Ignore;
        bool                 m_SortAscending;
        int                  m_SortColumn;

        DECLARE_EVENT_TABLE()
};

#endif // TODOLISTVIEW_H

