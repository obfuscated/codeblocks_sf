/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

/* This one is used for the Preview button, when the dialog is active,
 * and no settings have been saved.
 */

#ifndef DLGFORMATTERSETTINGS_H
#define DLGFORMATTERSETTINGS_H

#include "./astyle/astyle.h"
#include <wx/dialog.h>

class dlgFormatterSettings
{
  private:
    wxWindow *m_dlg;

	public:
		dlgFormatterSettings(wxWindow *dlg);
		virtual ~dlgFormatterSettings();

		void ApplyTo(astyle::ASFormatter& formatter);
};

#endif // DLGFORMATTERSETTINGS_H
