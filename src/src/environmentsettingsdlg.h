/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ENVIRONMENTSETTINGSDLG_H
#define ENVIRONMENTSETTINGSDLG_H

#include <wx/dialog.h>
#include <pluginmanager.h>

#include "aui_compat.h"

class wxListbookEvent;
class wxCheckListBox;

class EnvironmentSettingsDlg : public wxDialog
{
	public:
		EnvironmentSettingsDlg(wxWindow* parent, wxAuiDockArt* art);
		virtual ~EnvironmentSettingsDlg();
		virtual void EndModal(int retCode);
	protected:
        void OnPageChanging(wxListbookEvent& event);
        void OnPageChanged(wxListbookEvent& event);
        void OnSetAssocs(wxCommandEvent& event);
        void OnManageAssocs(wxCommandEvent& event);
        void OnNbDefaults(wxCommandEvent& event);
        void OnChooseColour(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnPlaceCheck(wxCommandEvent& event);
        void OnHeadCheck(wxCommandEvent& event);
        void OnAutoHide(wxCommandEvent& event);
        void OnI18NCheck(wxCommandEvent& event);
        void OnSettingsIconsSize(wxCommandEvent& event);
	private:
        void AddPluginPanels();
        void LoadListbookImages();
        void UpdateListbookImages();

        wxAuiDockArt* m_pArt;
        ConfigurationPanelsArray m_PluginPanels;
        DECLARE_EVENT_TABLE()
};



#endif // ENVIRONMENTSETTINGSDLG_H

