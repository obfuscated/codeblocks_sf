/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef ASTYLECONFIGDLG_H
#define ASTYLECONFIGDLG_H

#include <wx/intl.h>
#include "configurationpanel.h"

enum AStylePredefinedStyle
{
  aspsAnsi = 0,
  aspsKr,
  aspsLinux,
  aspsGnu,
  aspsJava,
  aspsCustom
};

class AstyleConfigDlg : public cbConfigurationPanel
{
	public:
		AstyleConfigDlg(wxWindow* parent);
		virtual ~AstyleConfigDlg();

	protected:
        void OnStyleChange(wxCommandEvent& event);
        void OnPreview(wxCommandEvent& event);

        virtual wxString GetTitle() const { return _("Source formatter"); }
        virtual wxString GetBitmapBaseName() const { return _T("astyle-plugin"); }
        virtual void OnApply(){ SaveSettings(); }
        virtual void OnCancel(){}

        void LoadSettings();
        void SaveSettings();
        void SetStyle(AStylePredefinedStyle style);

	private:
        DECLARE_EVENT_TABLE()
};

#endif // ASTYLECONFIGDLG_H
