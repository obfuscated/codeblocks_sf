/***************************************************************
 * Name:      editmimetypesdlg.h
 * Purpose:   Window to edit the supported mime types handling
 * Author:    Yiannis An. Mandravellos<mandrav@codeblocks.org>
 * Created:   03/13/05 14:08:51
 * Copyright: (c) Yiannis An. Mandravellos
 * License:   GPL
 **************************************************************/

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
