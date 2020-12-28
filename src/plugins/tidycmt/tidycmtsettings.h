/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef TIDYCMTSETTINGS_H
#define TIDYCMTSETTINGS_H

//(*Headers(TidyCmtSettings)
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include "tidycmtconfig.h"

class TidyCmtSettings : public wxPanel
{
	public:

		TidyCmtSettings(wxWindow* parent, const TidyCmtConfig& tcc);
		virtual ~TidyCmtSettings();

		TidyCmtConfig GetTidyCmtConfig();

		//(*Identifiers(TidyCmtSettings)
		static const long ID_CHK_ENABLE;
		static const long ID_SPN_LENGTH;
		static const long ID_TXT_SINGLE_LINE_CMT;
		static const long ID_TXT_MULTI_LINE_CMT;
		//*)

	protected:

		//(*Handlers(TidyCmtSettings)
		void OnEnableClick(wxCommandEvent& event);
		//*)

		//(*Declarations(TidyCmtSettings)
		wxCheckBox* chkEnable;
		wxSpinCtrl* spnLength;
		wxTextCtrl* txtMultiLineCmt;
		wxTextCtrl* txtSingleLineCmt;
		//*)

	private:

    void DoEnable(bool en);

		DECLARE_EVENT_TABLE()
};

#endif // TIDYCMTSETTINGS_H
