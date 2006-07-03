#ifndef PROJECTPATHPANEL_H
#define PROJECTPATHPANEL_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(ProjectPathPanel)
#include <wx/button.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

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
		wxString GetPath(){ return txtPrjPath->GetValue(); }
		wxString GetName(){ return txtPrjName->GetValue(); }
		wxString GetFullFileName(){ return txtFinalDir->GetValue(); }
		wxString GetTitle(){ return txtPrjTitle->GetValue(); }

		//(*Identifiers(ProjectPathPanel)
		enum Identifiers
		{
		    ID_BUTTON1 = 0x1000,
		    ID_STATICTEXT1,
		    ID_STATICTEXT2,
		    ID_STATICTEXT3,
		    ID_STATICTEXT4,
		    ID_STATICTEXT5,
		    ID_TEXTCTRL1,
		    ID_TEXTCTRL2,
		    ID_TEXTCTRL3,
		    ID_TEXTCTRL4
		};
		//*)

	protected:

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

	private:

		DECLARE_EVENT_TABLE()
};

#endif
