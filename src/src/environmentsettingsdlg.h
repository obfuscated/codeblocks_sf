#ifndef ENVIRONMENTSETTINGSDLG_H
#define ENVIRONMENTSETTINGSDLG_H

#include <wx/dialog.h>

class EnvironmentSettingsDlg : public wxDialog
{
	public:
		EnvironmentSettingsDlg(wxWindow* parent);
		virtual ~EnvironmentSettingsDlg();
		virtual void EndModal(int retCode);
	protected:
	private:
};

#endif // ENVIRONMENTSETTINGSDLG_H

