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

extern wxArrayString s_includeDirs;
extern wxArrayString s_filesParsed;
extern wxBusyInfo*   s_busyInfo;

ParserCommon::EFileType ParserCommon::FileType(const wxString& filename, bool force_refresh)
{
    static bool          empty_ext = true;
    static wxArrayString header_ext;
    header_ext.Add(_T("h")); header_ext.Add(_T("hpp")); header_ext.Add(_T("tcc")); header_ext.Add(_T("xpm"));
    static wxArrayString source_ext;
    source_ext.Add(_T("c")); source_ext.Add(_T("cpp")); source_ext.Add(_T("cxx")); source_ext.Add(_T("cc")); source_ext.Add(_T("c++"));

    if (filename.IsEmpty())
        return ParserCommon::ftOther;

    const wxString file = filename.AfterLast(wxFILE_SEP_PATH).Lower();
    const int      pos  = file.Find(_T('.'), true);
    wxString       ext;
    if (pos != wxNOT_FOUND)
        ext = file.SubString(pos + 1, file.Len());

    if (empty_ext && ext.IsEmpty())
        return ParserCommon::ftHeader;

    for (size_t i=0; i<header_ext.GetCount(); ++i)
    {
        if (ext==header_ext[i])
            return ParserCommon::ftHeader;
    }

    for (size_t i=0; i<source_ext.GetCount(); ++i)
    {
        if (ext==source_ext[i])
            return ParserCommon::ftSource;
    }

    return ParserCommon::ftOther;
}

bool Parse(const wxString& filename, bool isLocal)
{
    wxString log;
    log.Printf(wxT("ParserDummy() : Parse() : Parsing file '%s' (isLocal=%s)."),
               filename.wx_str(), (isLocal ? wxT("true") : wxT("false")));
    CCLogger::Get()->Log(log);

    // avoid parsing the same file(s) over and over again
    for (size_t i=0; i<s_filesParsed.GetCount(); i++)
    {
        if (filename.IsSameAs(s_filesParsed.Item(i), false))
        {
            log.Printf(wxT("ParserDummy() : Parse() : File '%s' has already been parsed"),
                       filename.wx_str());
            CCLogger::Get()->Log(log);
            return true;
        }
    }
    s_filesParsed.Add(filename);

    if (s_busyInfo) { delete s_busyInfo; s_busyInfo = 0; }
    s_busyInfo = new wxBusyInfo(wxT("Please wait, operating '")+filename+wxT("'..."));
    wxTheApp->Yield();

    log.Printf(wxT("ParserDummy() : Parse() : Creating new parser thread for '%s'"),
       filename.wx_str());
    CCLogger::Get()->Log(log);

    ParserTest pt; pt.Clear();
    CCLogger::Get()->Log(_T("-----------I-n-t-e-r-i-m--L-o-g-----------"));
    pt.Start(filename);
    // TODO: The following lines cause a crash in
//    CCLogger::Get()->Log(_T("--------------T-r-e-e--L-o-g--------------"));
//    pt.PrintTree();
//    CCLogger::Get()->Log(_T("--------------L-i-s-t--L-o-g--------------"));
//    pt.PrintList();

    return true;
}

wxString ParserBase::GetFullFileName(const wxString& src, const wxString& tgt, bool isGlobal)
{
    wxString log;
    log.Printf(wxT("ParserDummy() : GetFullFileName() : Querying full file name for source '%s', target '%s' (isGlobal=%s)."),
               src.wx_str(), tgt.wx_str(), (isGlobal ? wxT("true") : wxT("false")));
    CCLogger::Get()->Log(log);

    // first, try local include file
    wxFileName fn(src);
    wxString full_file_name = fn.GetPath(wxPATH_GET_SEPARATOR) + tgt;
    if ( ::wxFileExists(full_file_name) )
        return full_file_name;

    // second, try taking include directories into account
    for (size_t i=0; i<s_includeDirs.GetCount(); i++)
    {
        wxString include_dir = s_includeDirs.Item(i);
        CCLogger::Get()->Log(wxT("ParserDummy() : GetFullFileName() : Checking existence of ")+include_dir);
        if ( ::wxDirExists(include_dir) )
        {
            full_file_name = include_dir + fn.GetPathSeparator() + tgt;
            CCLogger::Get()->Log(wxT("ParserDummy() : GetFullFileName() : Checking existence of ")+full_file_name);
            if ( ::wxFileExists(full_file_name) )
                return full_file_name;
        }
    }

    CCLogger::Get()->Log(wxT("ParserDummy() : GetFullFileName() : File not found"));
    return wxEmptyString;
}

bool ParserBase::ParseFile(const wxString& filename, bool isGlobal, bool locked)
{
    wxString log;
    log.Printf(wxT("ParserDummy() : DoParseFile() : Parse file request for file name '%s' (isGlobal=%s)"),
               filename.wx_str(), (isGlobal ? wxT("true") : wxT("false")));
    CCLogger::Get()->Log(log);

    if (filename.IsEmpty())
        return false;

    return Parse(filename, !isGlobal);
}

ParserBase::ParserBase()
{
    CCLogger::Get()->Log(wxT("ParserDummy() : ParserBase() : Instantiation of Parser object."));
}

ParserBase::~ParserBase()
{
    CCLogger::Get()->Log(wxT("ParserDummy() : ~ParserBase() : Destruction of Parser object."));
}
