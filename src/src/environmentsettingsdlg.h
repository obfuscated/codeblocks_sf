#ifndef ENVIRONMENTSETTINGSDLG_H
#define ENVIRONMENTSETTINGSDLG_H

#include <wx/dialog.h>
#include <pluginmanager.h>

class wxDockArt;
class wxListbookEvent;

class EnvironmentSettingsDlg : public wxDialog
{
	public:
		EnvironmentSettingsDlg(wxWindow* parent, wxDockArt* art);
		virtual ~EnvironmentSettingsDlg();
		virtual void EndModal(int retCode);
	protected:
        void OnPageChanging(wxListbookEvent& event);
        void OnPageChanged(wxListbookEvent& event);
        void OnSetAssocs(wxCommandEvent& event);
        void OnChooseColor(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnPlaceCheck(wxCommandEvent& event);
        void OnHeadCheck(wxCommandEvent& event);
        void OnI18NCheck(wxCommandEvent& event);
	private:
        void AddPluginPanels();
        void LoadListbookImages();
        void UpdateListbookImages();
        wxDockArt* m_pArt;
        ConfigurationPanelsArray m_PluginPanels;
        DECLARE_EVENT_TABLE()
};

#endif // ENVIRONMENTSETTINGSDLG_H

