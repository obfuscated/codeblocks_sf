#ifndef ENVIRONMENTSETTINGSDLG_H
#define ENVIRONMENTSETTINGSDLG_H

#include <wx/dialog.h>

class wxDockArt;

class EnvironmentSettingsDlg : public wxDialog
{
	public:
		EnvironmentSettingsDlg(wxWindow* parent, wxDockArt* art);
		virtual ~EnvironmentSettingsDlg();
		virtual void EndModal(int retCode);
	protected:
        void OnSetAssocs(wxCommandEvent& event);
        void OnChooseColor(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
	private:
        wxDockArt* m_pArt;
        DECLARE_EVENT_TABLE()
};

#endif // ENVIRONMENTSETTINGSDLG_H

