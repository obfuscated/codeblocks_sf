#ifndef VIEWTODO_H
#define VIEWTODO_H

#include <wx/dynarray.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/combobox.h>
#include <cbeditor.h>

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

class ViewTodoDlg : public wxDialog
{
	public:
		ViewTodoDlg(wxWindow* parent);
		virtual ~ViewTodoDlg();

		wxString GetFilename();
		int GetLine();
	protected:
		void LoadUsers();
		void Parse();
		void InitList();
		void FillList();
		void ParseEditor(cbEditor* pEditor);
		void ParseFile(const wxString& filename);
		void ParseBuffer(const wxString& buffer, const wxString& filename);
		int CalculateLineNumber(const wxString& buffer, int upTo);
		ToDoItems m_Items;
	private:
		void OnComboChange(wxCommandEvent& event);
		void OnListSelected(wxListEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
		wxListCtrl* m_pItems;
		wxComboBox* m_pSource;
		wxComboBox* m_pUser;
		
		DECLARE_EVENT_TABLE();
};

#endif // VIEWTODO_H

