/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ADDTODODLG_H
#define ADDTODODLG_H

#include <wx/string.h>

#include "scrollingdialog.h"

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
    tdctDoxygenC, // Doxygen C style
    tdctDoxygenCPP, // Doxygen CPP style
    tdctWarning, // compiler warning
    tdctError,   // compiler error
    tdctFortran  // Fortran style
};

class AddTodoDlg : public wxScrollingDialog
{
    public:
        AddTodoDlg(wxWindow* parent, wxArrayString users, wxArrayString types);
        ~AddTodoDlg();

        wxString GetText() const;
        wxString GetUser() const;
        int GetPriority() const;
        ToDoPosition GetPosition() const;
        wxString GetType() const;
        ToDoCommentType GetCommentType() const;

        void EndModal(int retVal);
    private:
        void OnAddUser(wxCommandEvent& event);
        void OnDelUser(wxCommandEvent& event);
        void OnAddType(wxCommandEvent& event);
        void OnDelType(wxCommandEvent& event);

        wxArrayString m_Users;
        wxArrayString m_Types;
        DECLARE_EVENT_TABLE()
};

#endif // ADDTODODLG_H
