/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef REPLACEDLG_H
#define REPLACEDLG_H

#include "findreplacebase.h"

class wxComboBox;
class wxCommandEvent;
class wxNotebookEvent;
class wxActivateEvent;

class FindReplaceDlg : public FindReplaceBase
{
    public:
        FindReplaceDlg(wxWindow* parent, const wxString& initial = wxEmptyString, bool hasSelection = false,
                   bool findMode = true, bool findReplaceInFilesOnly = false, bool findReplaceInFilesActive = false);
        ~FindReplaceDlg();

        wxString GetFindString() const;
        wxString GetReplaceString() const;
        bool IsFindInFiles() const;
        bool GetDeleteOldSearches() const;
        bool GetSortSearchResult() const;
        bool GetMatchWord() const;
        bool GetStartWord() const;
        bool GetMatchCase() const;
        bool GetRegEx() const;
        bool GetAutoWrapSearch() const;
        bool GetFindUsesSelectedText() const;
        bool GetStartFile() const;
        bool GetMultiLine() const;
        bool GetFixEOLs() const;

        int GetDirection() const;
        int GetOrigin() const;
        int GetScope() const;
        bool GetRecursive() const;      // for find in search path
        bool GetHidden() const;         // for find in search path
        wxString GetSearchPath() const; // for find in search path
        wxString GetSearchMask() const; // for find in search path
        int GetProject() const; // for find in project
        int GetTarget() const; // for find in project

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

    private:
        void FillComboWithLastValues(wxComboBox* combo, const wxString& configKey);
        void SaveComboValues(wxComboBox* combo, const wxString& configKey);
        bool m_findReplaceInFilesActive;
        bool m_findMode;
        wxWindow *m_findPage;

        DECLARE_EVENT_TABLE()
};

#endif // REPLACEDLG_H
