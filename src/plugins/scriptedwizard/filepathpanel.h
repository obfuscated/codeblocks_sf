/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#ifndef FILEPATHPANEL_H
#define FILEPATHPANEL_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(FilePathPanel)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checklst.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/button.h>
//*)

#include <wx/string.h>

class FilePathPanel: public wxPanel
{
	public:

		FilePathPanel(wxWindow* parent,wxWindowID id = -1);
		virtual ~FilePathPanel();

        wxString GetFilename() const { return txtFilename->GetValue(); }
        wxString GetHeaderGuard() const { return txtGuard->IsShown() ? txtGuard->GetValue() : _T(""); }
        bool GetAddToProject() const { return chkAddToProject->GetValue(); }
        void SetAddToProject(bool add);
        int GetTargetIndex();
        void SetFilePathSelectionFilter(const wxString& filter){ m_ExtFilter = filter; }
        void ShowHeaderGuard(bool show)
        {
            lblGuard->Show(show);
            txtGuard->Show(show);
        }

		//(*Identifiers(FilePathPanel)
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL2;
		static const long ID_CHECKBOX1;
		static const long ID_STATICTEXT4;
		static const long ID_CHECKLISTBOX2;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		//*)

	private:

		//(*Handlers(FilePathPanel)
		void OntxtFilenameText(wxCommandEvent& event);
		void OnbtnBrowseClick(wxCommandEvent& event);
		void OnchkAddToProjectChange(wxCommandEvent& event);
		void OnbtnAllClick(wxCommandEvent& event);
		void OnbtnNoneClick(wxCommandEvent& event);
		//*)

		//(*Declarations(FilePathPanel)
		wxBoxSizer* BoxSizer6;
		wxButton* btnBrowse;
		wxBoxSizer* BoxSizer2;
		wxStaticText* lblGuard;
		wxButton* btnNone;
		wxCheckListBox* clbTargets;
		wxTextCtrl* txtGuard;
		wxFlexGridSizer* FlexGridSizer1;
		wxBoxSizer* BoxSizer3;
		wxTextCtrl* txtFilename;
		wxButton* btnAll;
		wxCheckBox* chkAddToProject;
		//*)

		wxString m_ExtFilter;
		int m_Selection;

		void ToggleVisibility(bool on);

		DECLARE_EVENT_TABLE()
};

#endif
