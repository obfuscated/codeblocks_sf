/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef INFOPANEL_H
#define INFOPANEL_H


//(*Headers(InfoPanel)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
//*)

class wxString;

class InfoPanel: public wxPanel
{
	public:

		InfoPanel(wxWindow* parent,wxWindowID id = -1);
		virtual ~InfoPanel();

		//(*Identifiers(InfoPanel)
		static const long ID_STATICTEXT1;
		static const long ID_CHECKBOX1;
		//*)

        void SetIntroText(const wxString& intro_msg)
        {
            lblIntro->SetLabel(intro_msg);

            GetSizer()->Fit(this);
            GetSizer()->SetSizeHints(this);
        }
//	protected:

		//(*Handlers(InfoPanel)
		//*)

		//(*Declarations(InfoPanel)
		wxCheckBox* chkSkip;
		wxBoxSizer* BoxSizer1;
		wxStaticText* lblIntro;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif // INFOPANEL_H
