#ifndef TODOLISTVIEW_H
#define TODOLISTVIEW_H

#include <wx/string.h>
#include "loggers.h"
#include <vector>
#include <map>
using namespace std;

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
    int line;
    int priority;
};

typedef map<wxString,vector<ToDoItem> > TodoItemsMap;

WX_DECLARE_OBJARRAY(ToDoItem, ToDoItems);

class ToDoListView : public ListCtrlLogger, public wxEvtHandler
{
    public:
        ToDoListView(const wxArrayString& titles, const wxArrayInt& widths, const wxArrayString& types);
        ~ToDoListView();
        void Parse();
        void ParseCurrent(bool forced);
        virtual wxWindow* CreateControl(wxWindow* parent);
        wxWindow* GetWindow(){ return panel; }
    private:
        void LoadUsers();
        void FillList();
        void ParseEditor(cbEditor* pEditor);
        void ParseFile(const wxString& filename);
        void ParseBuffer(const wxString& buffer, const wxString& filename);
        int CalculateLineNumber(const wxString& buffer, int upTo);

        void OnComboChange(wxCommandEvent& event);
        void OnListItemSelected(wxCommandEvent& event);
        void OnButtonRefresh(wxCommandEvent& event);
        void OnDoubleClick( wxCommandEvent& event ); //pecan 1/2/2006 12PM
        void FocusEntry(size_t index);            //pecan 1/2/2006 12PM

        wxWindow* panel;
        TodoItemsMap m_itemsmap;
        ToDoItems m_Items;
        wxComboBox* m_pSource;
        wxComboBox* m_pUser;
        wxButton* m_pRefresh;
        const wxArrayString& m_Types;
        wxString m_LastFile;
        bool m_ignore;

        DECLARE_EVENT_TABLE()
};

#endif // TODOLISTVIEW_H

