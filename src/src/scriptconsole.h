#ifndef SCRIPTCONSOLE_H
#define SCRIPTCONSOLE_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(ScriptConsole)
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include <messagelog.h>

class ScriptConsole: public wxPanel
{
	public:

		ScriptConsole(wxWindow* parent,wxWindowID id = -1);
		virtual ~ScriptConsole();

        void Log(const wxString& msg);

		//(*Identifiers(ScriptConsole)
		static const long ID_TEXTCTRL1;
		static const long ID_PANEL1;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL2;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		//*)

//	protected:
		//(*Handlers(ScriptConsole)
		void OnbtnExecuteClick(wxCommandEvent& event);
		void OnbtnLoadClick(wxCommandEvent& event);
		void OnbtnClearClick(wxCommandEvent& event);
		//*)

		//(*Declarations(ScriptConsole)
		wxBoxSizer* BoxSizer1;
		wxTextCtrl* txtConsole;
		wxPanel* Panel1;
		wxBoxSizer* BoxSizer2;
		wxStaticText* StaticText1;
		wxTextCtrl* txtCommand;
		wxButton* btnExecute;
		wxButton* btnLoad;
		wxButton* btnClear;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

class ScriptConsoleLog : public MessageLog
{
    public:
        ScriptConsoleLog()
        {
            wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
            m_Console = new ScriptConsole(this, -1);
            bs->Add(m_Console, 1, wxEXPAND | wxALL);
            SetAutoLayout(TRUE);
            SetSizer(bs);

            ResetLogFont();
        }

        ~ScriptConsoleLog(){}

		virtual void AddLog(const wxString& msg, bool addNewLine = true){}
        virtual void Clear(){ m_Console->txtConsole->Clear(); }
		virtual void ResetLogFont()
		{
            m_Console->txtConsole->SetFont(GetDefaultLogFont(true));
		}

        ScriptConsole* m_Console;
};

#endif
