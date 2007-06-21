#ifndef TODOLISTVIEW_H
#define TODOLISTVIEW_H

#include <wx/string.h>
#include "simplelistlog.h"
#include <vector>
#include <map>
using namespace std;

class cbEditor;
class wxArrayString;
class wxCommandEvent;
class wxListEvent;
class wxComboBox;
class wxButton;

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

class ToDoListView : public SimpleListLog
{
	public:
		ToDoListView(int numCols, int widths[], const wxArrayString& titles, const wxArrayString& types);
		~ToDoListView();
        void Parse();
        void ParseCurrent(bool forced);
	private:
        void LoadUsers();
        void FillList();
		void ParseEditor(cbEditor* pEditor);
		void ParseFile(const wxString& filename);
		void ParseBuffer(const wxString& buffer, const wxString& filename);
		int CalculateLineNumber(const wxString& buffer, int upTo);

        void OnComboChange(wxCommandEvent& event);
        void OnListItemSelected(wxListEvent& event);
        void OnButtonRefresh(wxCommandEvent& event);
        void OnDoubleClick( wxListEvent& event ); //pecan 1/2/2006 12PM
        void FocusEntry(size_t index);            //pecan 1/2/2006 12PM

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

