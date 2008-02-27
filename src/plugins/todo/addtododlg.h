/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ADDTODODLG_H
#define ADDTODODLG_H

#include <wx/dialog.h>
#include <wx/string.h>

class wxArrayString;
class wxWindow;
class wxCommandEvent;

enum ToDoPosition
{
    tdpAbove = 0,
    tdpCurrent,
    tdpBelow
};

enum ToDoCommentType
{
    tdctCpp = 0, // C++ style,
    tdctC,       // C style
    tdctDoxygen, // Doxygen style
    tdctWarning, // compiler warning
    tdctError    // compiler error
};

class AddTodoDlg : public wxDialog
{
    public:
        AddTodoDlg(wxWindow* parent, wxArrayString& types); // make sure the types live as long (or longer) then this dialog
        ~AddTodoDlg();

        wxString GetText() const;
        wxString GetUser() const;
        int GetPriority() const;
        ToDoPosition GetPosition() const;
        wxString GetType() const;
        ToDoCommentType GetCommentType() const;

        void EndModal(int retVal);
    private:
        void LoadUsers() const;
        void SaveUsers() const;
        void OnAddUser(wxCommandEvent& event);
        void OnDelUser(wxCommandEvent& event);
        wxArrayString& m_Types;
        DECLARE_EVENT_TABLE()
};

#endif // ADDTODODLG_H

