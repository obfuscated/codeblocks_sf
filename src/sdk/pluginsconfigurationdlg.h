#ifndef PLUGINSCONFIGURATIONDLG_H
#define PLUGINSCONFIGURATIONDLG_H

#include "settings.h"
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

		void EndModal(int retCode);
};

#endif // PLUGINSCONFIGURATIONDLG_H

