#ifndef TODOLISTVIEW_H
#define TODOLISTVIEW_H

#include <simplelistlog.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/dynarray.h>

class cbEditor;

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
WX_DECLARE_OBJARRAY(ToDoItem, ToDoItems);

class ToDoListView : public SimpleListLog
{
	public:
		ToDoListView(int numCols, int widths[], const wxArrayString& titles, const wxArrayString& types);
		virtual ~ToDoListView();
        void Parse();
	protected:
        void LoadUsers();
        void FillList();
		void ParseEditor(cbEditor* pEditor);
		void ParseFile(const wxString& filename);
		void ParseBuffer(const wxString& buffer, const wxString& filename);
		int CalculateLineNumber(const wxString& buffer, int upTo);

        void OnComboChange(wxCommandEvent& event);
        void OnListItemSelected(wxListEvent& event);
        void OnRefresh(wxCommandEvent& event);
        void OnDoubleClick( wxListEvent& event ); //pecan 1/2/2006 12PM
        void FocusEntry(size_t index);            //pecan 1/2/2006 12PM

		ToDoItems m_Items;
        wxComboBox* m_pSource;
        wxComboBox* m_pUser;
        wxButton* m_pRefresh;
        const wxArrayString& m_Types;
	private:
        DECLARE_EVENT_TABLE()
};

#endif // TODOLISTVIEW_H

