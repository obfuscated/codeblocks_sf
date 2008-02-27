/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef EDITMIMETYPESDLG_H
#define EDITMIMETYPESDLG_H

#include <wx/intl.h>
#include "configurationpanel.h"
#include "mimetypesarray.h"

class EditMimeTypesDlg : public cbConfigurationPanel
{
    public:
        EditMimeTypesDlg(wxWindow* parent, MimeTypesArray& array);
        virtual ~EditMimeTypesDlg();

        virtual wxString GetTitle() const { return _("Files extension handling"); }
        virtual wxString GetBitmapBaseName() const { return _T("extensions"); }
        virtual void OnApply();
        virtual void OnCancel(){}
    private:
        void FillList();
        void UpdateDisplay();
        void Save(int index);
        void OnSelectionChanged(wxCommandEvent& event);
        void OnActionChanged(wxCommandEvent& event);
        void OnBrowseProgram(wxCommandEvent& event);
        void OnNew(wxCommandEvent& event);
        void OnDelete(wxCommandEvent& event);

        MimeTypesArray& m_Array;
        int m_Selection;
        int m_LastSelection;

        DECLARE_EVENT_TABLE();
};

#endif // EDITMIMETYPESDLG_H
