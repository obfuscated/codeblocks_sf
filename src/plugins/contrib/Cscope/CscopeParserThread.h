#ifndef CSCOPE_PARSER_THREAD
#define CSCOPE_PARSER_THREAD


#include <wx/thread.h>
#include <wx/event.h>

#include "CscopeEntryData.h"

extern int wxEVT_CSCOPE_THREAD_DONE;

class wxInputStream;
class CscopeParserThread : public wxThread
{
    public:
        CscopeParserThread(wxEvtHandler *evtHandler, const wxArrayString &output);
        CscopeParserThread(wxEvtHandler *evtHandler, wxInputStream *input);
        ~CscopeParserThread();

        virtual void *Entry();
        virtual void OnExit();

    protected:
        CscopeResultTable* ParseResults();

    protected:
        wxEvtHandler *m_notifiedWindow;
        const wxArrayString &m_CscopeOutput;
        CscopeResultTable *m_result;
};



#endif //CSCOPE_PARSER_THREAD
