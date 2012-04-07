#ifndef DEBUGGERSETTINGSCOMMONPANEL_H
#define DEBUGGERSETTINGSCOMMONPANEL_H

#ifndef CB_PRECOMP
	//(*HeadersPCH(DebuggerSettingsCommonPanel)
	#include <wx/checkbox.h>
	#include <wx/sizer.h>
	#include <wx/button.h>
	#include <wx/panel.h>
	#include <wx/stattext.h>
	#include <wx/choice.h>
	//*)
#endif
//(*Headers(DebuggerSettingsCommonPanel)
//*)

class DebuggerSettingsCommonPanel: public wxPanel
{
	public:

		DebuggerSettingsCommonPanel(wxWindow* parent);
		virtual ~DebuggerSettingsCommonPanel();

		void SaveChanges();

	private:
	    void UpdateValueTooltipFont();
	private:

		//(*Declarations(DebuggerSettingsCommonPanel)
		wxStaticText* m_valueTooltipLabel;
		wxCheckBox* m_autoBuild;
		wxChoice* m_perspective;
		wxCheckBox* m_requireCtrlForTooltips;
		wxCheckBox* m_jumpOnDoubleClick;
		wxCheckBox* m_autoSwitch;
		wxCheckBox* m_debuggersLog;
		//*)
    private:
        wxString m_valueTooltipFontInfo;
    private:

		//(*Identifiers(DebuggerSettingsCommonPanel)
		static const long ID_AUTOBUILD;
		static const long ID_AUTOSWITCH;
		static const long ID_DEBUGGERS_LOG;
		static const long ID_JUMP_ON_DOUBLE_CLICK;
		static const long ID_REQUIRE_CTRL_FOR_TOOLTIPS;
		static const long ID_VALUE_TOOLTIP_LABEL;
		static const long ID_BUTTON_CHOOSE_FONT;
		static const long ID_CHOICE_PERSPECTIVE;
		//*)

		//(*Handlers(DebuggerSettingsCommonPanel)
		void OnChooseFontClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
