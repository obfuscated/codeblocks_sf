/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PRINTDLG_H
#define PRINTDLG_H

#include <wx/dialog.h>
#include "printing_types.h"

class PrintDialog : public wxDialog
{
	public:
		PrintDialog(wxWindow* parent);
		virtual ~PrintDialog();

        virtual void EndModal(int retCode);

		PrintScope GetPrintScope() const;
		PrintColourMode GetPrintColourMode() const;
		bool GetPrintLineNumbers() const;
};

#endif // PRINTDLG_H
