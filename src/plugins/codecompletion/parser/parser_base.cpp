#include <sdk.h>

#ifndef CB_PRECOMP
    #include "editorbase.h"
    #include <wx/dir.h>
#endif

#include <wx/tokenzr.h>
#include <cbstyledtextctrl.h>

#include "parser_base.h"


#define CC_PARSER_BASE_DEBUG_OUTPUT 0

#if defined(CC_GLOBAL_DEBUG_OUTPUT)
    #if CC_GLOBAL_DEBUG_OUTPUT == 1
        #undef CC_PARSER_BASE_DEBUG_OUTPUT
        #define CC_PARSER_BASE_DEBUG_OUTPUT 1
    #elif CC_PARSER_BASE_DEBUG_OUTPUT == 2
        #undef CC_PARSER_BASE_DEBUG_OUTPUT
        #define CC_PARSER_BASE_DEBUG_OUTPUT 2
    #endif
#endif

#ifdef CC_PARSER_TEST
    #define TRACE(format, args...) \
            CCLogger::Get()->DebugLog(F(format, ##args))
    #define TRACE2(format, args...) \
            CCLogger::Get()->DebugLog(F(format, ##args))
    #define TRACE2_SET_FLAG(traceFile)

    // don't use locker macros in the cctest project
    #undef CC_LOCKER_TRACK_TT_MTX_LOCK
    #define CC_LOCKER_TRACK_TT_MTX_LOCK(a)
    #undef CC_LOCKER_TRACK_TT_MTX_UNLOCK
    #define CC_LOCKER_TRACK_TT_MTX_UNLOCK(a)
#else
    #if CC_PARSER_BASE_DEBUG_OUTPUT == 1
        #define TRACE(format, args...) \
            CCLogger::Get()->DebugLog(F(format, ##args))
        #define TRACE2(format, args...)
        #define TRACE2_SET_FLAG(traceFile)
    #elif CC_PARSER_BASE_DEBUG_OUTPUT == 2
        #define TRACE(format, args...)                                              \
            do                                                                      \
            {                                                                       \
                if (g_EnableDebugTrace)                                             \
                    CCLogger::Get()->DebugLog(F(format, ##args));                   \
            }                                                                       \
            while (false)
        #define TRACE2(format, args...) \
            CCLogger::Get()->DebugLog(F(format, ##args))
        #define TRACE2_SET_FLAG(traceFile) \
            g_EnableDebugTrace = !g_DebugTraceFile.IsEmpty() && traceFile.EndsWith(g_DebugTraceFile)
    #else
        #define TRACE(format, args...)
        #define TRACE2(format, args...)
        #define TRACE2_SET_FLAG(traceFile)
    #endif
#endif // CC_PARSER_TEST

// both cctest and codecompletion plugin need the FileType() function, but the former is much
// simpler, so we use a preprocess directive here
#ifdef CC_PARSER_TEST
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
#else
ParserCommon::EFileType ParserCommon::FileType(const wxString& filename, bool force_refresh)
{
    static bool          cfg_read  = false;
    static bool          empty_ext = true;
    static wxArrayString header_ext;
    static wxArrayString source_ext;

    if (!cfg_read || force_refresh)
    {
        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
        empty_ext               = cfg->ReadBool(_T("/empty_ext"), true);
        wxString header_ext_str = cfg->Read(_T("/header_ext"), _T("h,hpp,hxx,hh,h++,tcc,tpp,xpm"));
        wxString source_ext_str = cfg->Read(_T("/source_ext"), _T("c,cpp,cxx,cc,c++"));

        header_ext.Clear();
        wxStringTokenizer header_ext_tknzr(header_ext_str, _T(","));
        while (header_ext_tknzr.HasMoreTokens())
            header_ext.Add(header_ext_tknzr.GetNextToken().Trim(false).Trim(true).Lower());

        source_ext.Clear();
        wxStringTokenizer source_ext_tknzr(source_ext_str, _T(","));
        while (source_ext_tknzr.HasMoreTokens())
            source_ext.Add(source_ext_tknzr.GetNextToken().Trim(false).Trim(true).Lower());

        cfg_read = true; // caching done
    }

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
#endif //CC_PARSER_TEST

ParserBase::ParserBase()
{
    m_TokenTree     = new TokenTree;
    m_TempTokenTree = new TokenTree;
}

ParserBase::~ParserBase()
{
    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    Delete(m_TokenTree);
    Delete(m_TempTokenTree);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
}

TokenTree* ParserBase::GetTokenTree() const
{
    return m_TokenTree;
}

bool ParserBase::ParseFile(const wxString& filename, bool isGlobal, bool /*locked*/)
{
    return Reparse(filename, !isGlobal);
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
    opts.platformCheck         = true;  // default

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


bool ParserBase::ParseBuffer(const wxString& buffer,
                             bool isLocal,
                             bool bufferSkipBlocks,
                             bool isTemp,
                             const wxString& filename,
                             int parentIdx,
                             int initLine)
{
    ParserThreadOptions opts;

    opts.useBuffer            = true;
    opts.fileOfBuffer         = filename;
    opts.parentIdxOfBuffer    = parentIdx;
    opts.initLineOfBuffer     = initLine;
    opts.bufferSkipBlocks     = bufferSkipBlocks;
    opts.isTemp               = isTemp;

    opts.followLocalIncludes  = true;
    opts.followGlobalIncludes = true;
    opts.wantPreprocessor     = m_Options.wantPreprocessor;
    opts.parseComplexMacros   = true;
    opts.platformCheck        = true;

    opts.handleFunctions      = true;   // enabled to support function ptr in local block

    opts.storeDocumentation   = false;

    ParserThread thread(this, buffer, isLocal, opts, m_TokenTree);

    bool success = thread.Parse();

    return success;

}

void ParserBase::AddIncludeDir(const wxString& dir)
{
    if (dir.IsEmpty())
        return;

    wxString base = dir;
    if (base.Last() == wxFILE_SEP_PATH)
        base.RemoveLast();
    if (!wxDir::Exists(base))
    {
        TRACE(_T("ParserBase::AddIncludeDir(): Directory %s does not exist?!"), base.wx_str());
        return;
    }

    if (m_IncludeDirs.Index(base) == wxNOT_FOUND)
    {
        TRACE(_T("ParserBase::AddIncludeDir(): Adding %s"), base.wx_str());
        m_IncludeDirs.Add(base);
    }
}

wxString ParserBase::FindFirstFileInIncludeDirs(const wxString& file)
{
    wxString FirstFound = m_GlobalIncludes.GetItem(file);
    if (FirstFound.IsEmpty())
    {
        wxArrayString FoundSet = FindFileInIncludeDirs(file,true);
        if (FoundSet.GetCount())
        {
            FirstFound = UnixFilename(FoundSet[0]);
            m_GlobalIncludes.AddItem(file, FirstFound);
        }
    }
    return FirstFound;
}

wxArrayString ParserBase::FindFileInIncludeDirs(const wxString& file, bool firstonly)
{
    wxArrayString FoundSet;
    for (size_t idxSearch = 0; idxSearch < m_IncludeDirs.GetCount(); ++idxSearch)
    {
        wxString base = m_IncludeDirs[idxSearch];
        wxFileName tmp = file;
        NormalizePath(tmp,base);
        wxString fullname = tmp.GetFullPath();
        if (wxFileExists(fullname))
        {
            FoundSet.Add(fullname);
            if (firstonly)
                break;
        }
    }

    TRACE(_T("ParserBase::FindFileInIncludeDirs(): Searching %s"), file.wx_str());
    TRACE(_T("ParserBase::FindFileInIncludeDirs(): Found %lu"), static_cast<unsigned long>(FoundSet.GetCount()));

    return FoundSet;
}

wxString ParserBase::GetFullFileName(const wxString& src, const wxString& tgt, bool isGlobal)
{
    wxString fullname;
    if (isGlobal)
    {
        fullname = FindFirstFileInIncludeDirs(tgt);
        if (fullname.IsEmpty())
        {
            // not found; check this case:
            //
            // we had entered the previous file like this: #include <gl/gl.h>
            // and it now does this: #include "glext.h"
            // glext.h was correctly not found above but we can now search
            // for gl/glext.h.
            // if we still not find it, it's not there. A compilation error
            // is imminent (well, almost - I guess the compiler knows a little better ;).
            wxString base = wxFileName(src).GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
            fullname = FindFirstFileInIncludeDirs(base + tgt);
        }
    }

    // NOTE: isGlobal is always true. The following code never executes...

    else // local files are more tricky, since they depend on two filenames
    {
        wxFileName fname(tgt);
        wxFileName source(src);
        if (NormalizePath(fname,source.GetPath(wxPATH_GET_VOLUME)))
        {
            fullname = fname.GetFullPath();
            if (!wxFileExists(fullname))
                fullname.Clear();
        }
    }

    return fullname;
}

size_t ParserBase::FindTokensInFile(const wxString& filename, TokenIdxSet& result, short int kindMask)
{
    result.clear();
    size_t tokens_found = 0;

    TRACE(_T("Parser::FindTokensInFile() : Searching for file '%s' in tokens tree..."), filename.wx_str());

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    TokenIdxSet tmpresult;
    if ( m_TokenTree->FindTokensInFile(filename, tmpresult, kindMask) )
    {
        for (TokenIdxSet::const_iterator it = tmpresult.begin(); it != tmpresult.end(); ++it)
        {
            const Token* token = m_TokenTree->at(*it);
            if (token)
                result.insert(*it);
        }
        tokens_found = result.size();
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return tokens_found;
}
