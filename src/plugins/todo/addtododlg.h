#ifndef ADDTODODLG_H
#define ADDTODODLG_H

#include <wx/dialog.h>

enum ToDoType
{
    tdtToDo = 0,
    tdtFixMe,
    tdtNote
};

enum ToDoPosition
{
    tdpAbove = 0,
    tdpCurrent,
    tdpBelow
};

enum ToDoCommentType
{
    tdctC = 0, // C style
    tdctCpp, // C++ style,
    tdctWarning, // compiler warning
    tdctError // compiler error
};

class AddTodoDlg : public wxDialog
{
	public:
		AddTodoDlg(wxWindow* parent);
		virtual ~AddTodoDlg();
        
        wxString GetText();
        wxString GetUser();
        int GetPriority();
        ToDoType GetType();
        ToDoPosition GetPosition();
        ToDoCommentType GetCommentType();
		
		virtual void EndModal(int retVal);
	protected:
		void LoadUsers();
		void SaveUsers();
	private:
};

#endif // ADDTODODLG_H

