#ifndef PROJECTPATHPANEL_H
#define PROJECTPATHPANEL_H

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(ProjectPathPanel)
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include <wx/string.h>

class ProjectPathPanel: public wxPanel
{
	public:

		ProjectPathPanel(wxWindow* parent,wxWindowID id = -1);
		virtual ~ProjectPathPanel();

		void SetPath(const wxString& path)
		{
		    txtPrjPath->SetValue(path);
//		    if (!path.IsEmpty())
//                txtPrjName->SetFocus();
            Update();
        }
		wxString GetPath() const { return txtPrjPath->GetValue(); }
		//wxString GetName(){ return txtPrjName->GetValue(); }
// NOTE (Biplab#1#): This is a temporary fix. This function
// need to be renamed according to it's visual representation
		wxString GetName() const { return txtPrjTitle->GetValue(); }
		wxString GetFullFileName() const { return txtFinalDir->GetValue(); }
		wxString GetTitle() const { return txtPrjTitle->GetValue(); }

		//(*Identifiers(ProjectPathPanel)
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT4;
		static const long ID_TEXTCTRL3;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT5;
		static const long ID_TEXTCTRL4;
		//*)

	private:

        void Update();
        void UpdateFromResulting();
        bool m_LockUpdates;

		//(*Handlers(ProjectPathPanel)
		void OnFullPathChanged(wxCommandEvent& event);
		void OntxtFinalDirText(wxCommandEvent& event);
		void OntxtPrjTitleText(wxCommandEvent& event);
		//*)

		//(*Declarations(ProjectPathPanel)
		wxBoxSizer* BoxSizer1;
		wxTextCtrl* txtPrjTitle;
		wxBoxSizer* BoxSizer2;
		wxTextCtrl* txtPrjPath;
		wxButton* btnPrjPathBrowse;
		wxTextCtrl* txtPrjName;
		wxTextCtrl* txtFinalDir;
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
