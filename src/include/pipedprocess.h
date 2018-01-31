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
        PipedProcess(PipedProcess** pvThis, wxEvtHandler* parent, int id = wxID_ANY,
                     bool pipe = true, const wxString& dir = wxEmptyString, int index = -1);
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
    protected:
        wxEvtHandler* m_Parent;
        wxTimer m_timerPollProcess;
        int m_Id;
        int m_Pid;

        /// When there are multiple processes started you could use this to distinguish between
        /// different processes. You could also use the id, but then you must preallocate too many
        /// ids and with the growing number of threads available in contemporary machines, this
        /// becomes unfeasible.
        /// It is sent back in the X variable of the CodeBlocksEvent.
        int m_Index;

        bool m_Stopped;
    private:
        PipedProcess** m_pvThis;
        DECLARE_EVENT_TABLE()
};

#endif // PIPEDPROCESS_H
