#ifndef ADDTODODLG_H
#define ADDTODODLG_H

#include <wx/dialog.h>

enum ToDoPosition
{
    tdpAbove = 0,
    tdpCurrent,
    tdpBelow
};

enum ToDoCommentType
{
    tdctCpp = 0, // C++ style,
    tdctC, // C style
    tdctWarning, // compiler warning
    tdctError // compiler error
};

class AddTodoDlg : public wxDialog
{
	public:
		AddTodoDlg(wxWindow* parent, wxArrayString& types);
		virtual ~AddTodoDlg();
        
        wxString GetText();
        wxString GetUser();
        int GetPriority();
        ToDoPosition GetPosition();
        wxString GetType();
        ToDoCommentType GetCommentType();
		
		virtual void EndModal(int retVal);
	protected:
		void LoadUsers();
		void SaveUsers();
		wxArrayString& m_Types;
	private:
};

#endif // ADDTODODLG_H

