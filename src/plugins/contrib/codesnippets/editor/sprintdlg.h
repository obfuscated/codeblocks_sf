/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef SPRINTDLG_H
#define SPRINTDLG_H

#include "scrollingdialog.h"
#include "printing_types.h"

class SEditorManager;

class SPrintDialog : public wxScrollingDialog
{
	public:
		SPrintDialog(wxWindow* parent, SEditorManager* edMgr);
		virtual ~SPrintDialog();

        virtual void EndModal(int retCode);

		PrintScope GetPrintScope() const;
		PrintColourMode GetPrintColourMode() const;
		bool GetPrintLineNumbers() const;
};

#endif // SPRINTDLG_H
