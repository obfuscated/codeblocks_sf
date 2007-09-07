#ifndef WXSDELETEITEMRES_H
#define WXSDELETEITEMRES_H

//(*Headers(wxsDeleteItemRes)
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

class wxsDeleteItemRes: public wxDialog
{
	public:

		wxsDeleteItemRes();
		virtual ~wxsDeleteItemRes();

		//(*Identifiers(wxsDeleteItemRes)
		static const long ID_CHECKBOX1;
		static const long ID_CHECKBOX2;
		static const long ID_CHECKBOX3;
		static const long ID_STATICTEXT1;
		//*)

		//(*Handlers(wxsDeleteItemRes)
		void Onm_DeleteSourcesClick(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsDeleteItemRes)
		wxBoxSizer* BoxSizer1;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxCheckBox* m_PhisDeleteWXS;
		wxCheckBox* m_DeleteSources;
		wxCheckBox* m_PhisDeleteSources;
		wxStaticText* StaticText1;
		wxStdDialogButtonSizer* StdDialogButtonSizer1;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
