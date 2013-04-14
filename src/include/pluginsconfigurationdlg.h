/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef PLUGINSCONFIGURATIONDLG_H
#define PLUGINSCONFIGURATIONDLG_H

#include "scrollingdialog.h"

class wxCommandEvent;
class wxListEvent;
class wxUpdateUIEvent;

/**
 * Dialog for Enabling/Disabling/Installing/Uninstalling a plug-in.
 */
class PluginsConfigurationDlg : public wxScrollingDialog
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
        void OnSelect(wxListEvent& event);
        void OnMouseMotion(wxMouseEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);

        DECLARE_EVENT_TABLE();
};

#endif // PLUGINSCONFIGURATIONDLG_H

