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
};

IMPLEMENT_APP(TestApp)
DECLARE_APP(TestApp)

bool TestApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    //*)

    Frame* frame = new Frame;
    frame->Center();
    frame->Show();
    frame->Start(_T("test.h"));

    return wxsOK;
}

