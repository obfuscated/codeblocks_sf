/*
* inline long cbSyncExecute(const wxString& command, wxArrayString& output, wxArrayString& error)
* inline long cbSyncExecute(const wxString& command, wxArrayString& output)
*
* Implements behaviour identical to synchronous wxExecute, but uses *safe* yields
* and is aware of application shutdowns (will stop polling and send SIGTERM to other process).
*
* Can be used in place of wxExecute to avoid possible wxYield() reentrancy problems (code completion and tool manager?)
*/

#ifndef CBEXECUTE
#define CBEXECUTE

#include "manager.h"

class cbExecuteProcess : public wxProcess
{
    wxInputStream *stream_stdout;
    wxInputStream *stream_stderr;

    wxArrayString *std_out;
    wxArrayString *std_err;
    bool running;
    int exitCode;

public:
    cbExecuteProcess(wxArrayString* out, wxArrayString* err) : std_out(out), std_err(err), running(true)
    {
        Redirect();
    };

    void FlushPipe()
    {
        wxString line;
        stream_stdout = GetInputStream();
        stream_stderr = GetErrorStream();

        if(stream_stdout && stream_stderr)
        {
            wxTextInputStream t_stream_stdout(*stream_stdout);
            wxTextInputStream t_stream_stderr(*stream_stderr);

            while(! stream_stdout->Eof() )
            {
                line = t_stream_stdout.ReadLine();
                std_out->Add(line);
            }

            if(std_err)
            {
            while(! stream_stderr->Eof() )
            {
                line = t_stream_stderr.ReadLine();
                std_err->Add(line);
            }
            }
        }
    };

    virtual void OnTerminate(int pid, int status)
    {
        FlushPipe();
        exitCode = status;
        running = false;
    }

    bool Running()
    {
        return running;
    };
    bool ExitCode()
    {
        return exitCode;
    };

};




inline long cbSyncExecute(const wxString& command, wxArrayString& output, wxArrayString& error)
{
    cbExecuteProcess process(&output, &error);

    if(wxExecute(command, wxEXEC_ASYNC, &process) == 0)
        return -1;

    while(process.Running())
        {
            if(Manager::isappShuttingDown())
            {
                process.Kill(wxSIGTERM); // will not work under Windows
                return -1;
            }
            Manager::Yield();
        }
    return process.ExitCode();
}

inline long cbSyncExecute(const wxString& command, wxArrayString& output)
{
    cbExecuteProcess process(&output, 0);

    if(wxExecute(command, wxEXEC_ASYNC, &process) == 0)
        return -1;

    while(process.Running())
        {
            if(Manager::isappShuttingDown())
            {
                process.Kill(wxSIGTERM);
                return -1;
            }
            Manager::Yield();
        }
    return process.ExitCode();
}


#endif
