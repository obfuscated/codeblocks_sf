#ifndef WXSWIZARD_H
#define WXSWIZARD_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(wxsWizard)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

struct wxsFilePart;

#include <cbproject.h>

class wxsWizard: public wxDialog
{
	public:

		wxsWizard(wxWindow* parent,wxWindowID id = -1);
		virtual ~wxsWizard();

		//(*Identifiers(wxsWizard)
		enum Identifiers
		{
		    ID_BUTTON1 = 0x1000,
		    ID_BUTTON2,
		    ID_BUTTON3,
		    ID_BUTTON4,
		    ID_CHECKBOX1,
		    ID_CHECKBOX2,
		    ID_CHECKBOX3,
		    ID_CHECKBOX4,
		    ID_CHECKBOX5,
		    ID_COMBOBOX1,
		    ID_COMBOBOX2,
		    ID_STATICTEXT1,
		    ID_STATICTEXT2,
		    ID_STATICTEXT3,
		    ID_STATICTEXT4,
		    ID_STATICTEXT5,
		    ID_STATICTEXT6,
		    ID_STATICTEXT7,
		    ID_TEXTCTRL1,
		    ID_TEXTCTRL2,
		    ID_TEXTCTRL3,
		    ID_TEXTCTRL4,
		    ID_TEXTCTRL5
		};
		//*)

	protected:

		//(*Handlers(wxsWizard)
		void OnButton1Click(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnConfModeSelect(wxCommandEvent& event);
		void OnDirChooseClick(wxCommandEvent& event);
		void OnwxDirChooseClick(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsWizard)
		wxFlexGridSizer* MainSizer;
		wxTextCtrl* PrjName;
		wxTextCtrl* FrmTitle;
		wxTextCtrl* PrjDir;
		wxCheckBox* AddMenu;
		wxCheckBox* AddStatus;
		wxCheckBox* AddAbout;
		wxCheckBox* UseXrc;
		wxCheckBox* AddDesc;
		wxStaticBoxSizer* wxWidgetsConfig;
		wxComboBox* ConfMode;
		wxComboBox* LibType;
		wxTextCtrl* WxDir;
		wxButton* WxDirChoose;
		wxTextCtrl* WxConf;
		//*)

	private:

        bool BuildFile(
            cbProject* project,
            const wxString& RootPath,
            const wxString& FileName,
            const wxsFilePart* FP,
            int Flags,
            bool AddToProject,
            bool Compile,
            bool Link,
            unsigned short Weight);

 		DECLARE_EVENT_TABLE()
};

#endif
