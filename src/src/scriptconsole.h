#ifndef SCRIPTCONSOLE_H
#define SCRIPTCONSOLE_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(ScriptConsole)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/button.h>
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
		static const long ID_TEXTCTRL2;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		static const long ID_PANEL1;
		//*)

	protected:
		//(*Handlers(ScriptConsole)
		void OnbtnExecuteClick(wxCommandEvent& event);
		void OnbtnLoadClick(wxCommandEvent& event);
		void OnbtnClearClick(wxCommandEvent& event);
		//*)

		//(*Declarations(ScriptConsole)
		wxButton* btnExecute;
		wxTextCtrl* txtCommand;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		wxBoxSizer* BoxSizer2;
		wxButton* btnClear;
		wxButton* btnLoad;
		wxTextCtrl* txtConsole;
		wxBoxSizer* BoxSizer1;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
