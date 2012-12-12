/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef AUTODETECTCOMPILERS_H
#define AUTODETECTCOMPILERS_H

#include "scrollingdialog.h"

class wxCommandEvent;
class wxUpdateUIEvent;

class AutoDetectCompilers : public wxScrollingDialog
{
	public:
		AutoDetectCompilers(wxWindow* parent);
		virtual ~AutoDetectCompilers();

		void OnDefaultClick(wxCommandEvent& event);
		void OnMouseMotion(wxMouseEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
	private:
        DECLARE_EVENT_TABLE()
};

#endif // AUTODETECTCOMPILERS_H
