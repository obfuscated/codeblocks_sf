/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ASSOCIATIONS_H
#define ASSOCIATIONS_H

#include <wx/intl.h>
#include <wx/dialog.h>
#include <wx/string.h>
#ifdef __WXMSW__
    #include <wx/msw/registry.h>
    #include <shlobj.h> // for SHChangeNotify()
    #define DDE_SERVICE    _T("CODEBLOCKS")
    #define DDE_TOPIC    _T("CodeBlocksDDEServer")
    #ifdef __CBDEBUG__
        #include <windows.h>
        #include <wincon.h>
    #endif
#endif
class wxCheckListBox;
class wxWindow;
class wxCommandEvent;

namespace Associations
{
    struct Assoc
    {
    wxString ext;
    wxString descr;
    int index;
    };

    unsigned int CountAssocs();

    void SetCore();
    void SetAll();
    void ClearAll();
    void SetBatchBuildOnly();
    bool Check();

    void DoSetAssociation(const wxString& ext, const wxString& descr, const wxString& exe, int icoNum);
    bool DoCheckAssociation(const wxString& ext, const wxString& descr, const wxString& exe, int icoNum);
    void DoClearAssociation(const wxString& ext);

    void UpdateChanges();
};

class ManageAssocsDialog : public wxDialog
{
    wxCheckListBox* list;

    public:
        ManageAssocsDialog(wxWindow* parent);
    protected:
        void OnApply(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);
        void OnClearAll(wxCommandEvent& event);
    private:
        DECLARE_EVENT_TABLE()
};

class AskAssocDialog : public wxDialog
{
    public:
        AskAssocDialog(wxWindow* parent);
    protected:
        void OnOK(wxCommandEvent& event);
    private:
        DECLARE_EVENT_TABLE()
};



#endif // ASSOCIATIONS_H
