#ifndef TESTCASESDLG_H
#define TESTCASESDLG_H

//(*Headers(TestCasesDlg)
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/timer.h>
#include <wx/listbox.h>
//*)
#include <wx/thread.h>

#include "ExpressionTestCases.h"

class TestCasesDlg: public wxDialog, public Expression::TestCases
{
	public:

		TestCasesDlg(wxWindow* parent);
		virtual ~TestCasesDlg();

	private:

		//(*Declarations(TestCasesDlg)
		wxListBox* ListBox1;
		wxButton* Button1;
		wxTimer Timer1;
		//*)

		//(*Identifiers(TestCasesDlg)
		static const long ID_LISTBOX1;
		static const long ID_BUTTON1;
		static const long ID_TIMER1;
		//*)

		//(*Handlers(TestCasesDlg)
		void OnButton1Click(wxCommandEvent& event);
		void OnTimer1Trigger(wxTimerEvent& event);
		void OnClose(wxCloseEvent& event);
		//*)

		virtual void AddLog( const wxString& logLine );
		virtual bool StopTest();
		virtual int Entry();

		void BuildContent(wxWindow* parent);

		class MyThread: public wxThread
		{
		    public:

                MyThread( TestCasesDlg* dlg ): wxThread(wxTHREAD_JOINABLE), m_Dlg( dlg ) {}

            private:

                TestCasesDlg* m_Dlg;

                virtual ExitCode Entry() { return (ExitCode)m_Dlg->Entry(); }
		};

        MyThread*         m_Thread;
		wxCriticalSection m_Section;
        wxArrayString     m_NewLogs;
		bool              m_Running;
		bool              m_StopRequest;
		bool              m_Result;
		bool              m_BtnChanged;

		DECLARE_EVENT_TABLE()
};

#endif
