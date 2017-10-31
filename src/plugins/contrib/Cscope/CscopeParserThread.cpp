#include "CscopeParserThread.h"

#include <wx/stream.h>
#include <wx/txtstrm.h>


int wxEVT_CSCOPE_THREAD_DONE = wxNewId();

CscopeParserThread::CscopeParserThread(wxEvtHandler *notifiedWindow, const wxArrayString &output)
: wxThread( wxTHREAD_DETACHED )
, m_notifiedWindow( notifiedWindow )
, m_CscopeOutput( output )
, m_result( NULL )
{
    //ctor
}

CscopeParserThread::~CscopeParserThread(){}

void* CscopeParserThread::Entry()
{
    m_result = ParseResults();
	return NULL;
}


CscopeResultTable* CscopeParserThread::ParseResults()
{

    CscopeResultTable *results = new CscopeResultTable();
	for (size_t i=0; i< m_CscopeOutput.GetCount(); i++)
	{
		//parse each line
		wxString line = m_CscopeOutput.Item(i);
		CscopeEntryData data;

		//first is the file name
		line = line.Trim().Trim(false);
		//skip errors
		if(line.StartsWith(_T("cscope:"))){continue;}

		wxString file = line.BeforeFirst(_T(' '));
		data.SetFile(file);
		line = line.AfterFirst(_T(' '));

		//next is the scope
		line = line.Trim().Trim(false);
		wxString scope = line.BeforeFirst(_T(' '));
		line = line.AfterFirst(_T(' '));
		data.SetScope( scope );

		//next is the line number
		line = line.Trim().Trim(false);
		long nn;
		wxString line_number = line.BeforeFirst(_T(' '));
		line_number.ToLong( &nn );
		data.SetLine( nn );
		line = line.AfterFirst(_T(' '));

		//the rest is the pattern
		wxString pattern = line;
		data.SetPattern(pattern);

        results->push_back(data);
	}
	return results;
}
void CscopeParserThread::OnExit()
{
    // send the results
	wxCommandEvent e(wxEVT_CSCOPE_THREAD_DONE);
	e.SetClientData(m_result);
	m_notifiedWindow->AddPendingEvent(e);
}

