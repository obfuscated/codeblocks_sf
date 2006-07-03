#ifndef LANGUAGEPANEL_H
#define LANGUAGEPANEL_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(LanguagePanel)
#include <wx/intl.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

#include <wx/dynarray.h>

class LanguagePanel: public wxPanel
{
	public:

		LanguagePanel(wxWindow* parent,wxWindowID id = -1);
		virtual ~LanguagePanel();

        void SetChoices(const wxArrayString& langs, int defLang);
        int GetLanguage(){ return lstLanguages->GetSelection(); }
        void SetLanguage(int lang){ lstLanguages->SetSelection(lang); }

		//(*Identifiers(LanguagePanel)
		enum Identifiers
		{
		    ID_LISTBOX1 = 0x1000,
		    ID_STATICTEXT1
		};
		//*)

	protected:

		//(*Handlers(LanguagePanel)
		//*)

		//(*Declarations(LanguagePanel)
		wxListBox* lstLanguages;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
