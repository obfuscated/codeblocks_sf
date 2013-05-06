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
typedef std::map<wxString,std::vector<ToDoItem> > TodoItemsMap;
WX_DECLARE_OBJARRAY(ToDoItem, ToDoItems);

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

        bool          IsChecked(wxString item);
        wxArrayString GetChecked();
        void          SetChecked(wxArrayString items);

    protected:
        wxCheckListBox* m_checkList;
        wxButton*       m_okBtn;

    private:
};

class ToDoListView : public wxEvtHandler, public ListCtrlLogger
{
    public:
        ToDoListView(const wxArrayString& titles, const wxArrayInt& widths, const wxArrayString& types);
        ~ToDoListView();
        virtual wxWindow* CreateControl(wxWindow* parent);
        void DestroyControls(bool control);

        void Parse();
        void ParseCurrent(bool forced);
        wxWindow* GetWindow() { return m_pPanel; }

        CheckListDialog * m_pAllowedTypesDlg;
    private:
        void LoadUsers();
        void FillList();
        void SortList();
        void FillListControl();
        void ParseEditor(cbEditor* pEditor);
        void ParseFile(const wxString& filename);
        void ParseBuffer(const wxString& buffer, const wxString& filename);
        int CalculateLineNumber(const wxString& buffer, int upTo, int &oldline, int &oldlinepos );
        void FocusEntry(size_t index);
        void SkipSpaces(const wxString& buffer, size_t &pos);

        void OnComboChange(wxCommandEvent& event);
        void OnListItemSelected(wxCommandEvent& event);
        void OnButtonRefresh(wxCommandEvent& event);
        void OnButtonTypes(wxCommandEvent& event);
        void OnDoubleClick( wxCommandEvent& event );
        void OnColClick( wxListEvent& event );

        wxWindow*            m_pPanel;
        TodoItemsMap         m_ItemsMap;
        ToDoItems            m_Items;
        wxComboBox*          m_pSource;
        wxComboBox*          m_pUser;
        const wxArrayString& m_Types;
        wxString             m_LastFile;
        bool                 m_Ignore;
        bool                 m_SortAscending;
        int                  m_SortColumn;

        DECLARE_EVENT_TABLE()
};

#endif // TODOLISTVIEW_H

