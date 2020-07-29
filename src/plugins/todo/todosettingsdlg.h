/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef TODOSETTINGSDLG_H
#define TODOSETTINGSDLG_H

#include <wx/intl.h>
#include "configurationpanel.h"
#include <settings.h>

class ToDoSettingsDlg : public cbConfigurationPanel
{
	public:
		ToDoSettingsDlg(wxWindow* parent);
		~ToDoSettingsDlg() override;

        wxString GetTitle() const override { return _("Todo list"); }
        wxString GetBitmapBaseName() const override { return _T("todo"); }
        void OnApply() override;
        void OnCancel() override {}
};

#endif // TODOSETTINGSDLG_H

