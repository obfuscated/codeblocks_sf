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

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/dialog.h>
#include "mimetypesarray.h"

class EditMimeTypesDlg : public wxDialog
{
	public:
		EditMimeTypesDlg(wxWindow* parent, MimeTypesArray& array);
		virtual ~EditMimeTypesDlg();
	protected:
        void FillList();
        void UpdateDisplay();
        void Save(int index);
        void OnSelectionChanged(wxCommandEvent& event);
        void OnActionChanged(wxCommandEvent& event);
        void OnBrowseProgram(wxCommandEvent& event);
        void OnNew(wxCommandEvent& event);
        void OnDelete(wxCommandEvent& event);
        void EndModal(int retCode);

        MimeTypesArray& m_Array;
        int m_Selection;
        int m_LastSelection;
	private:
        DECLARE_EVENT_TABLE();
};

#endif // EDITMIMETYPESDLG_H
