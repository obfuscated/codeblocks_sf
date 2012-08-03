/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008-2009 Bartlomiej Swiecki
*
* HexEditor plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* HexEditor pluging is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with HexEditor. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef TESTCASESDLG_H
#define TESTCASESDLG_H

//(*Headers(TestCasesDlg)
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/timer.h>
#include <wx/listbox.h>
#include "scrollingdialog.h"
//*)
#include <wx/thread.h>

#include "TestCasesBase.h"

class TestCasesDlg: public wxScrollingDialog, public TestCasesBase::Output
{
	public:

		TestCasesDlg(wxWindow* parent, TestCasesBase& tests);
		virtual ~TestCasesDlg();

	protected:

		virtual void AddLog( const wxString& logLine );
		virtual bool StopTest();

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

		virtual int Entry();

		void BuildContent(wxWindow* parent);

		class MyThread: public wxThread
		{
		    public:

                MyThread( TestCasesDlg* dlg ): wxThread(wxTHREAD_JOINABLE), m_Dlg( dlg ) {}

            private:

                TestCasesDlg* m_Dlg;

                virtual ExitCode Entry() { return reinterpret_cast<ExitCode>(m_Dlg->Entry()); }
		};

        TestCasesBase&    m_Tests;
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
