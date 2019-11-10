/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef SCRIPTCONSOLE_H
#define SCRIPTCONSOLE_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(ScriptConsole)
#include <wx/bmpbuttn.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class ScriptConsole: public wxPanel
{
	public:

		ScriptConsole(wxWindow* parent,wxWindowID id = -1);
		virtual ~ScriptConsole();

        void Log(const wxString& msg);

		//(*Identifiers(ScriptConsole)
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT1;
		static const long ID_COMBOBOX1;
		static const long ID_BITMAPBUTTON1;
		static const long ID_BITMAPBUTTON2;
		static const long ID_BITMAPBUTTON3;
		static const long ID_PANEL1;
		//*)

	protected:
		//(*Handlers(ScriptConsole)
		void OnbtnExecuteClick(wxCommandEvent& event);
		void OnbtnLoadClick(wxCommandEvent& event);
		void OnbtnClearClick(wxCommandEvent& event);
		//*)

		//(*Declarations(ScriptConsole)
		wxPanel* Panel1;
		wxBitmapButton* btnLoad;
		wxComboBox* txtCommand;
		wxBitmapButton* btnExecute;
		wxBitmapButton* btnClear;
		wxTextCtrl* txtConsole;
		wxStaticText* lblCommand;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
