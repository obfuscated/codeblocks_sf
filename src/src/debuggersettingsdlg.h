#ifndef DEBUGGERSETTINGSDLG_H
#define DEBUGGERSETTINGSDLG_H

#ifndef CB_PRECOMP
	//(*HeadersPCH(DebuggerSettingsDlg)
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/panel.h>
	#include "scrollingdialog.h"
	//*)

	#include <map>
#endif
//(*Headers(DebuggerSettingsDlg)
#include <wx/statline.h>
#include <wx/treebook.h>
//*)

class cbDebuggerConfiguration;
class cbDebuggerPlugin;
class DebuggerSettingsCommonPanel;

class DebuggerSettingsDlg: public wxScrollingDialog
{
	public:

		DebuggerSettingsDlg(wxWindow* parent);
		virtual ~DebuggerSettingsDlg();

		bool CreateConfig(wxWindow *panel, cbDebuggerPlugin *plugin, const wxString &name);
		void DeleteConfig(wxWindow *panel, cbDebuggerPlugin *plugin);
		void ResetConfig(wxWindow *panel, cbDebuggerPlugin *plugin);

	private:
		//(*Handlers(DebuggerSettingsDlg)
		void OnPageChanged(wxNotebookEvent& event);
		//*)

        void OnOK(wxCommandEvent &event);

	private:
		//(*Declarations(DebuggerSettingsDlg)
		wxTreebook* m_treebook;
		wxStaticText* m_activeInfo;
		//*)

		//(*Identifiers(DebuggerSettingsDlg)
		static const long ID_LABEL_ACTIVE_INFO;
		static const long ID_TREEBOOK;
		//*)

    private:
        struct Config
        {
            cbDebuggerPlugin *plugin;
            cbDebuggerConfiguration *config;
            wxString pluginGUIName;
        };

        typedef std::map<wxWindow*, Config> MapPanelToConfiguration;

        MapPanelToConfiguration m_mapPanelToConfig;
        DebuggerSettingsCommonPanel *m_commonPanel;
    private:
		DECLARE_EVENT_TABLE()
};

#endif
