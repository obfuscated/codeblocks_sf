#ifndef CONFIGPANEL_H
#define CONFIGPANEL_H

//(*Headers(ConfigPanel)
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>
//*)

class ConfigPanel: public wxPanel
{
	public:

		ConfigPanel(wxWindow* parent, wxWindowID &id);
		virtual ~ConfigPanel();

		//(*Declarations(ConfigPanel)
		wxCheckBox* Cfg_BrowseMarksEnabled;
		wxCheckBox* Cfg_WrapJumpEntries;
		wxRadioBox* Cfg_ClearAllKey;
		wxRadioBox* Cfg_MarkStyle;
		wxRadioBox* Cfg_ToggleKey;
		wxSlider* Cfg_LeftMouseDelay;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		//*)

	protected:

		//(*Identifiers(ConfigPanel)
		static const long ID_STATICTEXT1;
		static const long ID_CHECKBOX1;
		static const long ID_CHECKBOX2;
		static const long ID_RADIOBOX1;
		static const long ID_RADIOBOX3;
		static const long ID_SLIDER1;
		static const long ID_RADIOBOX2;
		static const long ID_STATICTEXT2;
		//*)

	private:

		//(*Handlers(ConfigPanel)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
