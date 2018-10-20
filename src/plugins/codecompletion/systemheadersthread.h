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

/** dir to files map, for example, you have two dirs c:/a and c:/b
 * so the map looks like:
 * c:/a  ---> {c:/a/a1.h, c:/a/a2.h}
 * c:/b  ---> {c:/b/b1.h, c:/b/b2.h}
 */
typedef std::map<wxString, StringSet> SystemHeadersMap;

/** event ids used to notify parent objects*/
extern long idSystemHeadersThreadFinish;
extern long idSystemHeadersThreadMessage;

/** collect all the header files, so they can be used in auto suggestion after #include<| directive.
 * This is just a file crawler to collect files in the include search paths.
 */
class SystemHeadersThread : public wxThread
{
public:
    SystemHeadersThread(wxEvtHandler*     parent,     wxCriticalSection*   critSect,
                        SystemHeadersMap& headersMap, const wxArrayString& incDirs);
    virtual ~SystemHeadersThread();

    virtual void* Entry();

private:
    wxEvtHandler*      m_Parent; /// this is the target the thread will sent any event to
    wxCriticalSection* m_SystemHeadersThreadCS; /// protect multiply access to its data
    SystemHeadersMap&  m_SystemHeadersMap; /// this takes the result data
    wxArrayString      m_IncludeDirs;  ///added include dirs to system headers
};

#endif // SYSTEMHEADERSTHREAD_H
