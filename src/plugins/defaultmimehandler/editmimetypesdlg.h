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

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma interface "editmimetypesdlg.h"
#endif
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
        void OnBrowseProgram(wxCommandEvent& event);
        void EndModal(int retCode);

        MimeTypesArray& m_Array;
        int m_Selection;
        int m_LastSelection;
	private:
        DECLARE_EVENT_TABLE();
};

#endif // EDITMIMETYPESDLG_H
