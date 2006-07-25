#ifndef FILEPATHPANEL_H
#define FILEPATHPANEL_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(FilePathPanel)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/checklst.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class FilePathPanel: public wxPanel
{
	public:

		FilePathPanel(wxWindow* parent,wxWindowID id = -1);
		virtual ~FilePathPanel();

        wxString GetFilename(){ return txtFilename->GetValue(); }
        wxString GetHeaderGuard(){ return txtGuard->IsShown() ? txtGuard->GetValue() : _T(""); }
        bool GetAddToProject(){ return chkAddToProject->GetValue(); }
        void SetAddToProject(bool add);
        int GetTargetIndex();
        void SetFilePathSelectionFilter(const wxString& filter){ m_ExtFilter = filter; }
        void ShowHeaderGuard(bool show)
        {
            lblGuard->Show(show);
            txtGuard->Show(show);
        }

		//(*Identifiers(FilePathPanel)
		enum Identifiers
		{
		  ID_STATICTEXT1 = 0x1000,
		  ID_STATICTEXT2,
		  ID_TEXTCTRL1,
		  ID_BUTTON1,
		  ID_STATICTEXT3,
		  ID_TEXTCTRL2,
		  ID_CHECKBOX1,
		  ID_STATICTEXT4,
		  ID_CHECKLISTBOX2,
		  ID_BUTTON2,
		  ID_BUTTON3
		};
		//*)

	protected:

		//(*Handlers(FilePathPanel)
		void OntxtFilenameText(wxCommandEvent& event);
		void OnbtnBrowseClick(wxCommandEvent& event);
		void OnchkAddToProjectChange(wxCommandEvent& event);
		void OnbtnAllClick(wxCommandEvent& event);
		void OnbtnNoneClick(wxCommandEvent& event);
		//*)

		//(*Declarations(FilePathPanel)
		wxBoxSizer* BoxSizer2;
		wxTextCtrl* txtFilename;
		wxButton* btnBrowse;
		wxStaticText* lblGuard;
		wxTextCtrl* txtGuard;
		wxCheckBox* chkAddToProject;
		wxBoxSizer* BoxSizer3;
		wxBoxSizer* BoxSizer4;
		wxBoxSizer* BoxSizer5;
		wxCheckListBox* clbTargets;
		wxBoxSizer* BoxSizer6;
		wxButton* btnAll;
		wxButton* btnNone;
		//*)

		wxString m_ExtFilter;
		int m_Selection;

	private:
		void ToggleVisibility(bool on);

		DECLARE_EVENT_TABLE()
};

#endif
