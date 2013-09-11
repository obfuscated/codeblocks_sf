/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef SYSTEMHEADERSTHREAD_H
#define SYSTEMHEADERSTHREAD_H

#include <wx/arrstr.h>
#include <wx/string.h>
#include <wx/thread.h>

#include <map>

#include "parser/parser.h" // StringSet

class wxEvtHandler;

typedef std::map<wxString, StringSet> SystemHeadersMap;

extern long idSystemHeadersThreadFinish;
extern long idSystemHeadersThreadUpdate;
extern long idSystemHeadersThreadError;

class SystemHeadersThread : public wxThread
{
public:
    SystemHeadersThread(wxEvtHandler*     parent,     wxCriticalSection*   critSect,
                        SystemHeadersMap& headersMap, const wxArrayString& incDirs);
    virtual ~SystemHeadersThread();

    virtual void* Entry();

private:
    wxEvtHandler*      m_Parent;
    wxCriticalSection* m_SystemHeadersThreadCS;
    SystemHeadersMap&  m_SystemHeadersMap;
    wxArrayString      m_IncludeDirs;
};

#endif // SYSTEMHEADERSTHREAD_H
