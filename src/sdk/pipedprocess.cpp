/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date$
*/

#include "pipedprocess.h" // class's header file
#include "sdk_events.h"

int idTimerPollProcess = wxNewId();

BEGIN_EVENT_TABLE(PipedProcess, wxProcess)
    EVT_TIMER(idTimerPollProcess, PipedProcess::OnTimer)
    EVT_IDLE(PipedProcess::OnIdle)
END_EVENT_TABLE()

// class constructor
PipedProcess::PipedProcess(wxEvtHandler* parent, int id, bool pipe, const wxString& dir)
    : wxProcess(parent, id),
	m_Parent(parent),
	m_Id(id),
	m_Pid(0)
{
	wxSetWorkingDirectory(dir);
	if (pipe)
		Redirect();
}

// class destructor
PipedProcess::~PipedProcess()
{
	// insert your code here
}

int PipedProcess::Launch(const wxString& cmd, unsigned int pollingInterval)
{
    m_Pid = wxExecute(cmd, wxEXEC_ASYNC, this);
	if (m_Pid)
	{
//		m_timerPollProcess.SetOwner(this, idTimerPollProcess);
//		m_timerPollProcess.Start(pollingInterval);
	}
	return m_Pid;
}

void PipedProcess::SendString(const wxString& text)
{
	//Manager::Get()->GetMessageManager()->Log(m_PageIndex, cmd);
	wxOutputStream* pOut = GetOutputStream();
	if (pOut)
	{
		wxTextOutputStream sin(*pOut);
		wxString msg = text + '\n';
		sin.WriteString(msg);
	}
}

bool PipedProcess::HasInput()
{
    bool hasInput = false;

    if (IsErrorAvailable())
    {
        wxTextInputStream serr(*GetErrorStream());

        wxString msg;
        msg << serr.ReadLine();

		CodeBlocksEvent event(cbEVT_PIPEDPROCESS_STDERR, m_Id);
        event.SetString(msg);
		m_Parent->ProcessEvent(event);

        hasInput = true;
    }

    if (IsInputAvailable())
    {
        wxTextInputStream sout(*GetInputStream());

        wxString msg;
        msg << sout.ReadLine();

		CodeBlocksEvent event(cbEVT_PIPEDPROCESS_STDOUT, m_Id);
        event.SetString(msg);
		m_Parent->ProcessEvent(event);

        hasInput = true;
    }

    return hasInput;
}

void PipedProcess::OnTerminate(int pid, int status)
{
    // show the rest of the output
    while ( HasInput() )
        ;

	CodeBlocksEvent event(cbEVT_PIPEDPROCESS_TERMINATED, m_Id);
    event.SetInt(status);
//	m_Parent->ProcessEvent(event);
	wxPostEvent(m_Parent, event);

    delete this;
}

void PipedProcess::OnTimer(wxTimerEvent& event)
{
    wxWakeUpIdle();
}

void PipedProcess::OnIdle(wxIdleEvent& event)
{
    while (HasInput())
		;
}
