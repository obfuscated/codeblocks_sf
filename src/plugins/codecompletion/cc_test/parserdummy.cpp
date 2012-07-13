/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "parser.h"
#include "cc_test.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/app.h>
#endif

#include <wx/arrstr.h>
#include <wx/busyinfo.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/string.h>

namespace CCTestAppGlobal
{
    extern wxArrayString s_includeDirs;
    extern wxArrayString s_fileQueue;
    extern wxArrayString s_filesParsed;
}// CCTestAppGlobal

ParserCommon::EFileType ParserCommon::FileType(const wxString& filename, bool force_refresh)
{
    static bool          empty_ext = true;
    static wxArrayString header_ext;
    header_ext.Add(_T("h")); header_ext.Add(_T("hpp")); header_ext.Add(_T("tcc")); header_ext.Add(_T("xpm"));
    static wxArrayString source_ext;
    source_ext.Add(_T("c")); source_ext.Add(_T("cpp")); source_ext.Add(_T("cxx")); source_ext.Add(_T("cc")); source_ext.Add(_T("c++"));

    if (filename.IsEmpty())
    {
        wxString log;
        log.Printf(wxT("ParserDummy::ParserCommon::FileType() : File '%s' is of type 'ftOther' (empty)."), filename.wx_str());
        CCLogger::Get()->Log(log);
        return ParserCommon::ftOther;
    }

    const wxString file = filename.AfterLast(wxFILE_SEP_PATH).Lower();
    const int      pos  = file.Find(_T('.'), true);
    wxString       ext;
    if (pos != wxNOT_FOUND)
        ext = file.SubString(pos + 1, file.Len());

    if (empty_ext && ext.IsEmpty())
    {
        wxString log;
        log.Printf(wxT("ParserDummy::ParserCommon::FileType() : File '%s' is of type 'ftHeader' (w/o ext.)."), filename.wx_str());
        CCLogger::Get()->Log(log);
        return ParserCommon::ftHeader;
    }

    for (size_t i=0; i<header_ext.GetCount(); ++i)
    {
        if (ext==header_ext[i])
        {
            wxString log;
            log.Printf(wxT("ParserDummy::ParserCommon::FileType() : File '%s' is of type 'ftHeader' (w/ ext.)."), filename.wx_str());
            CCLogger::Get()->Log(log);
            return ParserCommon::ftHeader;
        }
    }

    for (size_t i=0; i<source_ext.GetCount(); ++i)
    {
        if (ext==source_ext[i])
        {
            wxString log;
            log.Printf(wxT("ParserDummy::ParserCommon::FileType() : File '%s' is of type 'ftSource' (w/ ext.)."), filename.wx_str());
            CCLogger::Get()->Log(log);
            return ParserCommon::ftSource;
        }
    }

    wxString log;
    log.Printf(wxT("ParserDummy::ParserCommon::FileType() : File '%s' is of type 'ftOther' (unknown ext)."), filename.wx_str());
    CCLogger::Get()->Log(log);

    return ParserCommon::ftOther;
}

ParserBase::ParserBase() : m_TokensTree(NULL), m_TempTokensTree(NULL)
{
    // override default constructor of ParserBase (parser.h)
}

ParserBase::~ParserBase()
{
    // override default destructor of ParserBase (parser.h)
}

wxString ParserBase::GetFullFileName(const wxString& src, const wxString& tgt, bool isGlobal)
{
    wxString log;
    log.Printf(wxT("ParserDummy::ParserBase::GetFullFileName() : Querying full file name for source '%s', target '%s' (isGlobal=%s)."),
               src.wx_str(), tgt.wx_str(), (isGlobal ? wxT("true") : wxT("false")));
    CCLogger::Get()->Log(log);

    // first, try local include file
    wxFileName fn(src);
    wxString full_file_name = fn.GetPath(wxPATH_GET_SEPARATOR) + tgt;
    if ( ::wxFileExists(full_file_name) )
        return full_file_name;

    // second, try taking include directories into account
    for (size_t i=0; i<CCTestAppGlobal::s_includeDirs.GetCount(); i++)
    {
        wxString include_dir = CCTestAppGlobal::s_includeDirs.Item(i);
        CCLogger::Get()->Log(wxT("ParserDummy::ParserBase::GetFullFileName() : Checking existence of ")+include_dir);
        if ( ::wxDirExists(include_dir) )
        {
            full_file_name = include_dir + fn.GetPathSeparator() + tgt;
            CCLogger::Get()->Log(wxT("ParserDummy::ParserBase::GetFullFileName() : Checking existence of ")+full_file_name);
            if ( ::wxFileExists(full_file_name) )
                return full_file_name;
        }
    }

    CCLogger::Get()->Log(wxT("ParserDummy::ParserBase::GetFullFileName() : File not found"));
    return wxEmptyString;
}

bool ParserBase::ParseFile(const wxString& filename, bool isGlobal, bool locked)
{
    wxString log;
    log.Printf(wxT("ParserDummy::ParserBase::ParseFile() : Parse file request for file name '%s' (isGlobal=%s)"),
               filename.wx_str(), (isGlobal ? wxT("true") : wxT("false")));
    CCLogger::Get()->Log(log);

    if (filename.IsEmpty())
        return false;

    // avoid parsing the same file(s) over and over again
    for (size_t i=0; i<CCTestAppGlobal::s_filesParsed.GetCount(); i++)
    {
        if (filename.IsSameAs(CCTestAppGlobal::s_filesParsed.Item(i), false))
        {
            log.Printf(wxT("ParserDummy::ParserBase::ParseFile() : File '%s' has already been parsed"),
                       filename.wx_str());
            CCLogger::Get()->Log(log);
            return true;
        }
    }

    // check, if the file is already queued
    if (CCTestAppGlobal::s_fileQueue.Index(filename)==wxNOT_FOUND)
    {
        log.Printf(wxT("ParserDummy::ParserBase::ParseFile() : Appending new file to parse to queue: '%s'"),
           filename.wx_str());
        CCLogger::Get()->Log(log);
        CCTestAppGlobal::s_fileQueue.Add(filename);
    }
    else
    {
        log.Printf(wxT("ParserDummy::ParserBase::ParseFile() : File '%s' is already queued"),
           filename.wx_str());
        CCLogger::Get()->Log(log);
    }

    return true;
}
