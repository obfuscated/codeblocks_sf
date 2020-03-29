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

#include "TestCasesDlg.h"

//(*InternalHeaders(TestCasesDlg)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(TestCasesDlg)
const long TestCasesDlg::ID_LISTBOX1 = wxNewId();
const long TestCasesDlg::ID_BUTTON1 = wxNewId();
const long TestCasesDlg::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(TestCasesDlg,wxScrollingDialog)
	//(*EventTable(TestCasesDlg)
	//*)
END_EVENT_TABLE()

TestCasesDlg::TestCasesDlg(wxWindow* parent, TestCasesBase& tests): m_Tests( tests ), m_Thread( 0 )
{
    m_Tests.InitOutput( *this );
	BuildContent(parent);
}

void TestCasesDlg::BuildContent(wxWindow* parent)
{
	//(*Initialize(TestCasesDlg)
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Test log:"));
	ListBox1 = new wxListBox(this, ID_LISTBOX1, wxDefaultPosition, wxSize(410,268), 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
	StaticBoxSizer1->Add(ListBox1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
	Button1 = new wxButton(this, ID_BUTTON1, _("Stop"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	StaticBoxSizer1->Add(Button1, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_RIGHT, 5);
	BoxSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND, 5);
	SetSizer(BoxSizer1);
	Timer1.SetOwner(this, ID_TIMER1);
	Timer1.Start(50, false);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TestCasesDlg::OnButton1Click);
	Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&TestCasesDlg::OnTimer1Trigger);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&TestCasesDlg::OnClose);
	//*)

    m_Running     = true;
    m_StopRequest = false;
    m_BtnChanged  = false;

    // Start the thread
    m_Thread = new MyThread( this );
    m_Thread->Create();
    m_Thread->Run();
}

TestCasesDlg::~TestCasesDlg()
{
	//(*Destroy(TestCasesDlg)
	//*)
	m_Thread->Wait();
	delete m_Thread;
}

int TestCasesDlg::Entry()
{
    m_Result  = m_Tests.PerformTests();
    m_Running = false;
    return 0;
}

bool TestCasesDlg::StopTest()
{
    // Don't have to lock anything since
    // we only send one bit of information
    return m_StopRequest;
}

void TestCasesDlg::AddLog( const wxString& logLine )
{
    wxCriticalSectionLocker lock( m_Section );
    m_NewLogs.Add( logLine );
}

void TestCasesDlg::OnButton1Click(wxCommandEvent& /*event*/)
{
    if ( m_Running )
    {
        m_StopRequest = true;
        Button1->Disable();
        AddLog( _T("Cancelled by the user") );
    }
    else if ( m_BtnChanged )
    {
        EndDialog( wxID_OK );
    }
}

void TestCasesDlg::OnTimer1Trigger(wxTimerEvent& /*event*/)
{
    if ( !m_Running && !m_BtnChanged )
    {
        m_BtnChanged = true;
        Button1->Enable();
        Button1->SetLabel( _("Close") );
    }

    wxCriticalSectionLocker lock( m_Section );
    if ( !m_NewLogs.IsEmpty() )
    {
        ListBox1->Append( m_NewLogs );
        m_NewLogs.clear();

        ListBox1->SetFirstItem( ListBox1->GetCount() - 1 );
    }
}

void TestCasesDlg::OnClose(wxCloseEvent& event)
{
    if ( m_Running && event.CanVeto() )
    {
        event.Veto();
    }
    else
    {
        m_StopRequest = true;
        event.Skip();
    }
}
