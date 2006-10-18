#ifndef PLUGINSCONFIGURATIONDLG_H
#define PLUGINSCONFIGURATIONDLG_H

#include <wx/dialog.h>

/**
 * Dialog for Enabling/Disabling/Installing/Uninstalling a plug-in.
 */
class PluginsConfigurationDlg : public wxDialog
{
	public:
		// class constructor
		PluginsConfigurationDlg(wxWindow* parent);
		// class destructor
		~PluginsConfigurationDlg();

        void EndModal(int retCode);
    private:
        void FillList();
        void OnToggle(wxCommandEvent& event);
        void OnInstall(wxCommandEvent& event);
        void OnUninstall(wxCommandEvent& event);
        void OnExport(wxCommandEvent& event);
        void OnInfo(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);

        DECLARE_EVENT_TABLE();
};

#endif // PLUGINSCONFIGURATIONDLG_H

