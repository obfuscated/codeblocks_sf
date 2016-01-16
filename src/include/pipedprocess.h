/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef PIPEDPROCESS_H
#define PIPEDPROCESS_H

#include <wx/process.h> // inheriting class' header file
#include <wx/txtstrm.h>
#include <wx/timer.h>

#include "settings.h"

/*
 * No description
 */
class DLLIMPORT PipedProcess : public wxProcess
{
    public:
        // class constructor
        PipedProcess(PipedProcess** pvThis, wxEvtHandler* parent, int id = wxID_ANY, bool pipe = true, const wxString& dir = wxEmptyString);
        // class destructor
        ~PipedProcess();
        virtual int Launch(const wxString& cmd, unsigned int pollingInterval = 100);
        virtual void SendString(const wxString& text);
        virtual bool HasInput();
        virtual int GetPid(){ return m_Pid; }
        void ForfeitStreams();
    protected:
        virtual void OnTerminate(int pid, int status);
        virtual void OnTimer(wxTimerEvent& event);
        virtual void OnIdle(wxIdleEvent& event);
        wxEvtHandler* m_Parent;
        int m_Id;
        int m_Pid;
        wxTimer m_timerPollProcess;
    private:
        PipedProcess** m_pvThis;
        DECLARE_EVENT_TABLE()
};

#endif // PIPEDPROCESS_H
