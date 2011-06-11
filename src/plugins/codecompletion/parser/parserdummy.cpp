/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "parser.h"
#include "parsertest.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/app.h>
#endif

#include <wx/arrstr.h>
#include <wx/busyinfo.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/string.h>

BEGIN_EVENT_TABLE(Parser, wxEvtHandler)
END_EVENT_TABLE()

extern wxArrayString     s_includeDirs;
extern wxArrayString     s_filesParsed;
extern wxBusyInfo*       s_busyInfo;

extern void ParserTrace(const wxChar* format, ...);

wxString Parser::GetFullFileName(const wxString& src, const wxString& tgt, bool isGlobal)
{
    wxString log;
    log.Printf(wxT("ParserDummy() : GetFullFileName() : Querying full file name for source '%s', target '%s' (isGlobal=%s)."),
               src.wx_str(), tgt.wx_str(), (isGlobal ? wxT("true") : wxT("false")));
    ParserTrace(log);

    // first, try local include file
    wxFileName fn(src);
    wxString full_file_name = fn.GetPath(wxPATH_GET_SEPARATOR) + tgt;
    if ( ::wxFileExists(full_file_name) )
        return full_file_name;

    // second, try taking include directories into account
    for (size_t i=0; i<s_includeDirs.GetCount(); i++)
    {
        wxString include_dir = s_includeDirs.Item(i);
        ParserTrace(wxT("ParserDummy() : GetFullFileName() : Checking existence of ")+include_dir);
        if ( ::wxDirExists(include_dir) )
        {
            full_file_name = include_dir + fn.GetPathSeparator() + tgt;
            ParserTrace(wxT("ParserDummy() : GetFullFileName() : Checking existence of ")+full_file_name);
            if ( ::wxFileExists(full_file_name) )
                return full_file_name;
        }
    }

    ParserTrace(wxT("ParserDummy() : GetFullFileName() : File not found"));
    return wxEmptyString;
}

void Parser::DoParseFile(const wxString& filename, bool isGlobal)
{
    wxString log;
    log.Printf(wxT("ParserDummy() : DoParseFile() : Parse file request for file name '%s' (isGlobal=%s)"),
               filename.wx_str(), (isGlobal ? wxT("true") : wxT("false")));
    ParserTrace(log);

    if (filename.IsEmpty())
        return;

    Parse(filename, !isGlobal);
}

bool Parser::Parse(const wxString& filename, bool isLocal, LoaderBase* loader)
{
    wxString log;
    log.Printf(wxT("ParserDummy() : Parse() : Parsing file '%s' (isLocal=%s)."),
               filename.wx_str(), (isLocal ? wxT("true") : wxT("false")));
    ParserTrace(log);

    // avoid parsing the same file(s) over and over again
    for (size_t i=0; i<s_filesParsed.GetCount(); i++)
    {
        if (filename.IsSameAs(s_filesParsed.Item(i), false))
        {
            log.Printf(wxT("ParserDummy() : Parse() : File '%s' has already been parsed"),
                       filename.wx_str());
            ParserTrace(log);
            return true;
        }
    }
    s_filesParsed.Add(filename);

    if (s_busyInfo) { delete s_busyInfo; s_busyInfo = 0; }
    s_busyInfo = new wxBusyInfo(wxT("Please wait, operating '")+filename+wxT("'..."));
    wxTheApp->Yield();

    log.Printf(wxT("ParserDummy() : Parse() : Creating new parser thread for '%s'"),
       filename.wx_str());
    ParserTrace(log);

    ParserTest pt; pt.Clear();
    ParserTrace(_T("-----------I-n-t-e-r-i-m--L-o-g-----------"));
    pt.Start(filename);
    // TODO: The following lines cause a crash in
//    ParserTrace(_T("--------------T-r-e-e--L-o-g--------------"));
//    pt.PrintTree();
//    ParserTrace(_T("--------------L-i-s-t--L-o-g--------------"));
//    pt.PrintList();

    return true;
}

Parser::Parser(wxEvtHandler* pEvt, cbProject* project) : m_Pool(NULL, 0, 0)
{
    ParserTrace(wxT("ParserDummy() : Parser() : Instantiation of Parser object."));
}

Parser::~Parser()
{
    ParserTrace(wxT("ParserDummy() : ~Parser() : Destruction of Parser object."));
}
