#ifndef FILEPATHPANEL_H
#define FILEPATHPANEL_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(FilePathPanel)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/intl.h>
#include <wx/panel.h>
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
        int GetTargetIndex(){ return cmbTargets->GetSelection(); }
        void SetFilePathSelectionFilter(const wxString& filter){ m_ExtFilter = filter; }
        void ShowHeaderGuard(bool show)
        {
            lblGuard->Show(show);
            txtGuard->Show(show);
        }

		//(*Identifiers(FilePathPanel)
		enum Identifiers
		{
		    ID_BUTTON1 = 0x1000,
		    ID_CHECKBOX1,
		    ID_CHOICE1,
		    ID_STATICTEXT1,
		    ID_STATICTEXT2,
		    ID_STATICTEXT3,
		    ID_STATICTEXT4,
		    ID_TEXTCTRL1,
		    ID_TEXTCTRL2
		};
		//*)

	protected:

		//(*Handlers(FilePathPanel)
		void OntxtFilenameText(wxCommandEvent& event);
		void OnbtnBrowseClick(wxCommandEvent& event);
		void OnchkAddToProjectChange(wxCommandEvent& event);
		//*)

		//(*Declarations(FilePathPanel)
		wxBoxSizer* BoxSizer2;
		wxTextCtrl* txtFilename;
		wxButton* btnBrowse;
		wxStaticText* lblGuard;
		wxTextCtrl* txtGuard;
		wxCheckBox* chkAddToProject;
		wxBoxSizer* BoxSizer3;
		wxChoice* cmbTargets;
		//*)

		wxString m_ExtFilter;

	private:

		DECLARE_EVENT_TABLE()
};

#endif
