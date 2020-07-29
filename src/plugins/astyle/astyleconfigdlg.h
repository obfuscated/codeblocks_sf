/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ASTYLECONFIGDLG_H
#define ASTYLECONFIGDLG_H

#include <wx/intl.h>
#include "astylepredefinedstyles.h"
#include "configurationpanel.h"

class AstyleConfigDlg : public cbConfigurationPanel
{
	public:
		AstyleConfigDlg(wxWindow* parent);
		virtual ~AstyleConfigDlg();

	protected:
        void OnStyleChange(wxCommandEvent& event);
        void OnPreview(wxCommandEvent& event);
        void OnBreakLineChange(wxCommandEvent& event);
        void OnBreakBlocksChange(wxCommandEvent& event);

        wxString GetTitle() const  override { return _("Source formatter"); }
        wxString GetBitmapBaseName() const  override { return _T("astyle-plugin"); }
        void OnApply() override { SaveSettings(); }
        void OnCancel() override {}

        void LoadSettings();
        void SaveSettings();
        void SetStyle(AStylePredefinedStyle style);

	private:
        DECLARE_EVENT_TABLE()
};

#endif // ASTYLECONFIGDLG_H
