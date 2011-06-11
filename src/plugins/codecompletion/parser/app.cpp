/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/app.h>
#endif

//(*AppHeaders
#include <wx/image.h>
//*)

#include <wx/arrstr.h>
#include <wx/busyinfo.h>
#include <wx/thread.h>

#include "frame.h"

// global variable to ease handling of include directories
wxArrayString     s_includeDirs;
wxArrayString     s_filesParsed;
wxBusyInfo*       s_busyInfo;

class TestApp : public wxApp
{
public:
    virtual bool OnInit();
    Frame* GetFrame() const { return m_frame; }

private:
    Frame* m_frame;
};

IMPLEMENT_APP(TestApp)
DECLARE_APP(TestApp)

bool TestApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    //*)

    m_frame = new Frame;
    m_frame->Center();
    m_frame->Show();
    m_frame->Start(_T("test.h"));

    return wxsOK;
}

void ParserTrace(const wxChar* format, ...)
{
    va_list ap;
    va_start(ap, format);
    wxString log = wxString::FormatV(format, ap);
    va_end(ap);

    // Convert '\r' to "\r", '\n' to "\n"
    for (size_t i = 0; i < log.Len(); ++i)
    {
        if (log.GetChar(i) == _T('\r'))
        {
            log.SetChar(i, _T('\\'));
            log.insert(++i, 1, _T('r'));
        }
        else if (log.GetChar(i) == _T('\n'))
        {
            log.SetChar(i, _T('\\'));
            log.insert(++i, 1, _T('n'));
        }
    }

    wxGetApp().GetFrame()->Log(log);
}
