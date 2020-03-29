/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef REPLACEDLG_H
#define REPLACEDLG_H

#include <wx/notebook.h>

#include "findreplacebase.h"

class wxComboBox;
class wxCommandEvent;

class DLLIMPORT FindReplaceDlg : public FindReplaceBase
{
    public:
        FindReplaceDlg(wxWindow* parent, const wxString& initial = wxEmptyString, bool hasSelection = false,
                   bool findMode = true, bool findReplaceInFilesOnly = false, bool findReplaceInFilesActive = false);
        ~FindReplaceDlg() override;

        wxString GetFindString() const override;
        wxString GetReplaceString() const override;
        bool IsFindInFiles() const override;
        bool GetDeleteOldSearches() const override;
        bool GetSortSearchResult() const override;
        bool GetMatchWord() const override;
        bool GetStartWord() const override;
        bool GetMatchCase() const override;
        bool GetRegEx() const override;
        bool GetAutoWrapSearch() const override;
        bool GetFindUsesSelectedText() const override;
        bool GetStartFile() const override;
        bool GetMultiLine() const override;
        bool GetFixEOLs() const override;

        int GetDirection() const override;
        int GetOrigin() const override;
        int GetScope() const override;
        bool GetRecursive() const override;      // for find in search path
        bool GetHidden() const override;         // for find in search path
        wxString GetSearchPath() const override; // for find in search path
        wxString GetSearchMask() const override; // for find in search path
        int GetProject() const override; // for find in project
        int GetTarget() const override; // for find in project

        bool IsMultiLine() const;

    protected:
        void OnReplaceChange(wxNotebookEvent& event);
        void OnRegEx(wxCommandEvent& event);
        void OnActivate(wxActivateEvent& event);
        void OnMultiChange(wxCommandEvent& event);
        void OnLimitToChange(wxCommandEvent& event);
        void OnScopeChange(wxCommandEvent& event);
        void OnBrowsePath(wxCommandEvent& event);
        void OnSearchProject(wxCommandEvent& event);
        void OnDeferredFocus(wxCommandEvent& event);
        void OnSelectTarget(wxCommandEvent& event);

    private:
        void FillComboWithLastValues(wxComboBox* combo, const wxString& configKey);
        void SaveComboValues(wxComboBox* combo, const wxString& configKey);
        bool m_findReplaceInFilesActive;
        bool m_findMode;
        wxWindow *m_findPage;

        DECLARE_EVENT_TABLE()
};

#endif // REPLACEDLG_H
