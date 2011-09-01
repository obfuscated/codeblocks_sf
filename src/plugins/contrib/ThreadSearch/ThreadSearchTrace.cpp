/***************************************************************
 * Name:      ThreadSearchTrace
 *
 * Purpose:   This class implements the events sent by the
 *            worker search thread (ThreadSearchThread) to the
 *            ThreadSearchView to update the file/line/line
 *            list control.
 *            wxCommandEvent m_commandString contains file path.
 *            m_LineTextArray contains Line/FoundText series of items
 *
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/
 #include <wx/datetime.h>
 #include <wx/intl.h>
 #include "ThreadSearchTrace.h"


ThreadSearchTrace* ThreadSearchTrace::ms_Tracer = NULL;


bool ThreadSearchTrace::Trace(const wxString& str)
{
    wxASSERT(ms_Tracer != NULL);
    wxMutexLocker mutexLocker(*ms_Tracer);
    if ( mutexLocker.IsOk() )
    {
        if ( (ms_Tracer != NULL) && (ms_Tracer->IsOpened() == true) )
        {
            wxDateTime now = wxDateTime::Now();
            //ms_Tracer->Write(_T(" ") + now.FormatISOTime() + _T(" "));
            ms_Tracer->Write(_T(" ") + wxString::Format(wxT("%d:%d:%d:%d %s\n"), now.GetHour(), now.GetMinute(), now.GetSecond(), now.GetMillisecond(), str.c_str()));
//            ms_Tracer->Write(str);
//            ms_Tracer->Write(_T("\n"));
        }
    }

    return mutexLocker.IsOk();
}


bool ThreadSearchTrace::Init(const wxString& path)
{
    wxASSERT(ms_Tracer == NULL);
    ms_Tracer = new ThreadSearchTrace();
    if ( wxFile::Exists(path) )
    {
        wxRemoveFile(path);
    }
    return ms_Tracer->Open(path.c_str(), wxFile::write_excl);
}


void ThreadSearchTrace::Uninit()
{
    wxASSERT(ms_Tracer != NULL);
    wxMutexLocker mutexLocker(*ms_Tracer);
    if ( mutexLocker.IsOk() )
    {
        if ( ms_Tracer != NULL )
        {
            if ( ms_Tracer->IsOpened() == true )
            {
                ms_Tracer->Close();
            }
            delete ms_Tracer;
            ms_Tracer = NULL;
        }
    }
}


TraceBeginEndOfMethod::TraceBeginEndOfMethod(const wxString& method)
                      :m_Method(method)
{
    wxString begin(_("Begin of "));
    begin += m_Method;
    ThreadSearchTrace::Trace(begin);
}

TraceBeginEndOfMethod::~TraceBeginEndOfMethod()
{
    wxString end(_("End of "));
    end += m_Method;
    ThreadSearchTrace::Trace(end);
}
