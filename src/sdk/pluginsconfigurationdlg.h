#ifndef PLUGINSCONFIGURATIONDLG_H
#define PLUGINSCONFIGURATIONDLG_H

#include  "settings.h"
#include <wx/dialog.h>

/*
 * No description
 */
class PluginsConfigurationDlg : public wxDialog
{
	public:
		// class constructor
		PluginsConfigurationDlg(wxWindow* parent);
		// class destructor
		~PluginsConfigurationDlg();

		void OnOK(wxCommandEvent& event);
    private:
    	DECLARE_EVENT_TABLE()
};

#endif // PLUGINSCONFIGURATIONDLG_H

