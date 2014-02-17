/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "parser.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/app.h>
#endif

#include <wx/arrstr.h>
#include <wx/busyinfo.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/string.h>

#ifdef CC_PARSER_TEST
    #define ADDTOKEN(format, args...) \
            wxLogMessage(F(format, ##args))
    #define TRACE(format, args...) \
            wxLogMessage(F(format, ##args))
    #define TRACE2(format, args...) \
            wxLogMessage(F(format, ##args))
#else
    #define ADDTOKEN(format, args...)
    #define TRACE(format, args...)
    #define TRACE2(format, args...)
#endif


// Supply the implementation of ParserBase class

namespace CCTestAppGlobal
{
    extern wxArrayString s_includeDirs;
    extern wxArrayString s_fileQueue;
    extern wxArrayString s_filesParsed;
}// CCTestAppGlobal

ParserCommon::EFileType ParserCommon::FileType(const wxString& filename, bool /*force_refresh*/)
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
        //CCLogger::Get()->Log(log);
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
        //CCLogger::Get()->Log(log);
        return ParserCommon::ftHeader;
    }

    for (size_t i=0; i<header_ext.GetCount(); ++i)
    {
        if (ext==header_ext[i])
        {
            wxString log;
            log.Printf(wxT("ParserDummy::ParserCommon::FileType() : File '%s' is of type 'ftHeader' (w/ ext.)."), filename.wx_str());
            TRACE(log);
            return ParserCommon::ftHeader;
        }
    }

    for (size_t i=0; i<source_ext.GetCount(); ++i)
    {
        if (ext==source_ext[i])
        {
            wxString log;
            log.Printf(wxT("ParserDummy::ParserCommon::FileType() : File '%s' is of type 'ftSource' (w/ ext.)."), filename.wx_str());
            TRACE(log);
            return ParserCommon::ftSource;
        }
    }

    wxString log;
    log.Printf(wxT("ParserDummy::ParserCommon::FileType() : File '%s' is of type 'ftOther' (unknown ext)."), filename.wx_str());
    TRACE(log);

    return ParserCommon::ftOther;
}

ParserBase::ParserBase() : m_TokenTree(NULL), m_TempTokenTree(NULL)
{
    // override default constructor of ParserBase (parser.h)
    m_TokenTree = new TokenTree;
    // No need to allocate the TempTokenTree, as we don't need it here in the cc_test project
}

ParserBase::~ParserBase()
{
    // override default destructor of ParserBase (parser.h)
    delete m_TokenTree;
    m_TokenTree = 0;
}

TokenTree* ParserBase::GetTokenTree()
{
    return m_TokenTree;
}

wxString ParserBase::GetFullFileName(const wxString& src, const wxString& tgt, bool isGlobal)
{
    wxString log;
    log.Printf(wxT("ParserDummy::ParserBase::GetFullFileName() : Querying full file name for source '%s', target '%s' (isGlobal=%s)."),
               src.wx_str(), tgt.wx_str(), (isGlobal ? wxT("true") : wxT("false")));
    TRACE(log);

    // first, try local include file
    wxFileName fn(src);
    wxString full_file_name = fn.GetPath(wxPATH_GET_SEPARATOR) + tgt;
    if ( ::wxFileExists(full_file_name) )
        return full_file_name;

    // second, try taking include directories into account
    for (size_t i=0; i<CCTestAppGlobal::s_includeDirs.GetCount(); i++)
    {
        wxString include_dir = CCTestAppGlobal::s_includeDirs.Item(i);
        TRACE(wxT("ParserDummy::ParserBase::GetFullFileName() : Checking existence of ")+include_dir);
        if ( ::wxDirExists(include_dir) )
        {
            full_file_name = include_dir + fn.GetPathSeparator() + tgt;
            TRACE(wxT("ParserDummy::ParserBase::GetFullFileName() : Checking existence of ")+full_file_name);
            if ( ::wxFileExists(full_file_name) )
                return full_file_name;
        }
    }

    TRACE(wxT("ParserDummy::ParserBase::GetFullFileName() : File not found"));
    return wxEmptyString;
}

bool ParserBase::Reparse(const wxString& file, cb_unused bool isLocal)
{
    FileLoader* loader = new FileLoader(file);
    (*loader)();

    ParserThreadOptions opts;

    opts.useBuffer             = false; // default
    opts.parentIdxOfBuffer     = -1;    // default
    opts.initLineOfBuffer      = -1;    // default
    opts.bufferSkipBlocks      = false; // default
    opts.bufferSkipOuterBlocks = false; // default
    opts.isTemp                = false; // default

    opts.followLocalIncludes   = true;  // default
    opts.followGlobalIncludes  = true;  // default
    opts.wantPreprocessor      = true;  // default
    opts.parseComplexMacros    = true;  // default

    opts.handleFunctions       = true;  // default
    opts.handleVars            = true;  // default
    opts.handleClasses         = true;  // default
    opts.handleEnums           = true;  // default
    opts.handleTypedefs        = true;  // default

    opts.loader                = loader;

    ParserThread* pt = new ParserThread(this, file, true, opts, m_TokenTree);
    bool success = pt->Parse();
    delete pt;

    return success;
}

bool ParserBase::ParseFile(const wxString& filename, bool isGlobal, bool /*locked*/)
{
    return Reparse(filename, !isGlobal);
}
