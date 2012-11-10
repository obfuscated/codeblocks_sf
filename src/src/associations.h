/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ASSOCIATIONS_H
#define ASSOCIATIONS_H

#include "scrollingdialog.h"
#include <wx/intl.h>
#include <wx/string.h>
#ifdef __WXMSW__
    #include <wx/msw/registry.h>
    #include <shlobj.h> // for SHChangeNotify()
    #ifdef __CBDEBUG__
        #include <windows.h>
        #include <wincon.h>
    #endif
    #define DDE_SERVICE    _T("CODEBLOCKS")
#else
    #define DDE_SERVICE    _T("/tmp/CODEBLOCKS%s.socket")
#endif
#define DDE_TOPIC    _T("CodeBlocksDDEServer")
#include <wx/ipc.h>

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
    bool DoCheckAssociation(const wxString& ext, const wxString& descr, const wxString& exe, int icoNum);  // TODO: descriptions are not checked at all, and actually it's not necessary
                                                                                                           //       (extension + executable is actually enough)
                                                                                                           //        ---> decide whether to remove parameter from "Check" function or implement
    void DoClearAssociation(const wxString& ext);

    void UpdateChanges();
};

class ManageAssocsDialog : public wxScrollingDialog
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

#define ASC_ASSOC_DLG_NO_DONT_ASK       0
#define ASC_ASSOC_DLG_NO_ONLY_NOW       1
#define ASC_ASSOC_DLG_YES_C_FILES       2
#define ASC_ASSOC_DLG_YES_ALL_FILES     3

class AskAssocDialog : public wxScrollingDialog
{
    public:
        AskAssocDialog(wxWindow* parent);
    protected:
        void OnOK(wxCommandEvent& event);
        void OnESC(wxCommandEvent& event);
        void OnCharHook(wxKeyEvent& event);
    private:
        DECLARE_EVENT_TABLE()
};



#endif // ASSOCIATIONS_H
