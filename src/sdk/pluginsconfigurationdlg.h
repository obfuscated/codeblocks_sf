#ifndef PLUGINSCONFIGURATIONDLG_H
#define PLUGINSCONFIGURATIONDLG_H

#include <wx/dialog.h>

/*
 * Dialog for Enabling/Disabling the plug-in.
 */
class PluginsConfigurationDlg : public wxDialog
{
	public:
		// class constructor
		PluginsConfigurationDlg(wxWindow* parent);
		// class destructor
		~PluginsConfigurationDlg();

		void EndModal(int retCode);
};

#endif // PLUGINSCONFIGURATIONDLG_H

