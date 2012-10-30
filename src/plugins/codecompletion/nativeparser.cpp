/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#ifndef CB_PRECOMP
    #include <cctype>

    #include <wx/dir.h>
    #include <wx/log.h> // for wxSafeShowMessage()
    #include <wx/regex.h>
    #include <wx/wfstream.h>

    #include <cbauibook.h>
    #include <cbeditor.h>
    #include <cbexception.h>
    #include <cbproject.h>
    #include <compilerfactory.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <logmanager.h>
    #include <macrosmanager.h>
    #include <manager.h>
    #include <pluginmanager.h>
    #include <prep.h> // nullptr
    #include <projectmanager.h>

    #include <tinyxml/tinyxml.h>
#endif

#include <wx/tokenzr.h>

#include <cbstyledtextctrl.h>
#include <compilercommandgenerator.h>
#include <projectloader_hooks.h>

#include "nativeparser.h"
#include "classbrowser.h"
#include "parser/parser.h"
#include "parser/profiletimer.h"

#define CC_NATIVEPARSER_DEBUG_OUTPUT 0

#if CC_GLOBAL_DEBUG_OUTPUT == 1
    #undef CC_NATIVEPARSER_DEBUG_OUTPUT
    #define CC_NATIVEPARSER_DEBUG_OUTPUT 1
#elif CC_GLOBAL_DEBUG_OUTPUT == 2
    #undef CC_NATIVEPARSER_DEBUG_OUTPUT
    #define CC_NATIVEPARSER_DEBUG_OUTPUT 2
#endif

#if CC_NATIVEPARSER_DEBUG_OUTPUT == 1
    #define TRACE(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
    #define TRACE2(format, args...)
#elif CC_NATIVEPARSER_DEBUG_OUTPUT == 2
    #define TRACE(format, args...)                                              \
        do                                                                      \
        {                                                                       \
            if (g_EnableDebugTrace)                                             \
                CCLogger::Get()->DebugLog(F(format, ##args));   \
        }                                                                       \
        while (false)
    #define TRACE2(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
#else
    #define TRACE(format, args...)
    #define TRACE2(format, args...)
#endif

/*
 * (Recursive) functions that are surrounded by a critical section:
 * GenerateResultSet() -> AddChildrenOfUnnamed
 * GetCallTips() -> PrettyPrintToken (recursive function)
 * FindCurrentFunctionToken() -> ParseFunctionArguments, FindAIMatches (recursive function)
 * GenerateResultSet (recursive function):
 *     FindAIMatches(), ResolveActualType(), ResolveExpression(),
 *     FindCurrentFunctionToken(), ResolveOperator()
 * FindCurrentFunctionStart() -> GetTokenFromCurrentLine
 */

namespace NativeParserHelper
{
    class ParserDirTraverser : public wxDirTraverser
    {
    public:
        ParserDirTraverser(const wxString& excludePath, wxArrayString& files) :
            m_ExcludeDir(excludePath),
            m_Files(files)
        {}

        virtual wxDirTraverseResult OnFile(const wxString& filename)
        {
            if (ParserCommon::FileType(filename) != ParserCommon::ftOther)
                m_Files.Add(filename);
            return wxDIR_CONTINUE;
        }

        virtual wxDirTraverseResult OnDir(const wxString& dirname)
        {
            if (dirname == m_ExcludeDir)
                return wxDIR_IGNORE;
            if (m_Files.GetCount() == 1)
                return wxDIR_STOP;
            m_Files.Clear();
            return wxDIR_CONTINUE;
        }

    private:
        const wxString& m_ExcludeDir;
        wxArrayString&  m_Files;
    };
}// namespace NativeParserHelper

int idTimerParsingOneByOne = wxNewId();

bool s_DebugSmartSense           = false;
const wxString g_StartHereTitle  = _("Start here");

NativeParser::NativeParser() :
    m_TimerParsingOneByOne(this, idTimerParsingOneByOne),
    m_ClassBrowser(nullptr),
    m_ClassBrowserIsFloating(false),
    m_ImageList(nullptr),
    m_ParserPerWorkspace(false),
    m_EditorStartWord(-1),
    m_EditorEndWord(-1),
    m_LastAISearchWasGlobal(false),
    m_LastControl(nullptr),
    m_LastFunctionIndex(-1),
    m_LastFuncTokenIdx(-1),
    m_LastLine(-1),
    m_LastResult(-1)
{
    m_TempParser = new ParserBase;
    m_Parser     = m_TempParser;

    m_ImageList = new wxImageList(16, 16);
    wxBitmap bmp;
    wxString prefix;
    prefix = ConfigManager::GetDataFolder() + _T("/images/codecompletion/");
    // bitmaps must be added by order of PARSER_IMG_* consts
    bmp = cbLoadBitmap(prefix + _T("class_folder.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_CLASS_FOLDER
    bmp = cbLoadBitmap(prefix + _T("class.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_CLASS
    bmp = cbLoadBitmap(prefix + _T("class_private.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_CLASS_PRIVATE
    bmp = cbLoadBitmap(prefix + _T("class_protected.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_CLASS_PROTECTED
    bmp = cbLoadBitmap(prefix + _T("class_public.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_CLASS_PUBLIC
    bmp = cbLoadBitmap(prefix + _T("ctor_private.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_CTOR_PRIVATE
    bmp = cbLoadBitmap(prefix + _T("ctor_protected.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_CTOR_PROTECTED
    bmp = cbLoadBitmap(prefix + _T("ctor_public.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_CTOR_PUBLIC
    bmp = cbLoadBitmap(prefix + _T("dtor_private.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_DTOR_PRIVATE
    bmp = cbLoadBitmap(prefix + _T("dtor_protected.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_DTOR_PROTECTED
    bmp = cbLoadBitmap(prefix + _T("dtor_public.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_DTOR_PUBLIC
    bmp = cbLoadBitmap(prefix + _T("method_private.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_FUNC_PRIVATE
    bmp = cbLoadBitmap(prefix + _T("method_protected.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_FUNC_PRIVATE
    bmp = cbLoadBitmap(prefix + _T("method_public.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_FUNC_PUBLIC
    bmp = cbLoadBitmap(prefix + _T("var_private.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_VAR_PRIVATE
    bmp = cbLoadBitmap(prefix + _T("var_protected.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_VAR_PROTECTED
    bmp = cbLoadBitmap(prefix + _T("var_public.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_VAR_PUBLIC
    bmp = cbLoadBitmap(prefix + _T("preproc.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_PREPROCESSOR
    bmp = cbLoadBitmap(prefix + _T("enum.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_ENUM
    bmp = cbLoadBitmap(prefix + _T("enum_private.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_ENUM_PRIVATE
    bmp = cbLoadBitmap(prefix + _T("enum_protected.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_ENUM_PROTECTED
    bmp = cbLoadBitmap(prefix + _T("enum_public.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_ENUM_PUBLIC
    bmp = cbLoadBitmap(prefix + _T("enumerator.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_ENUMERATOR
    bmp = cbLoadBitmap(prefix + _T("namespace.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_NAMESPACE
    bmp = cbLoadBitmap(prefix + _T("typedef.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_TYPEDEF
    bmp = cbLoadBitmap(prefix + _T("typedef_private.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_TYPEDEF_PRIVATE
    bmp = cbLoadBitmap(prefix + _T("typedef_protected.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_TYPEDEF_PROTECTED
    bmp = cbLoadBitmap(prefix + _T("typedef_public.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_TYPEDEF_PUBLIC
    bmp = cbLoadBitmap(prefix + _T("symbols_folder.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_SYMBOLS_FOLDER
    bmp = cbLoadBitmap(prefix + _T("vars_folder.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_VARS_FOLDER
    bmp = cbLoadBitmap(prefix + _T("funcs_folder.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_FUNCS_FOLDER
    bmp = cbLoadBitmap(prefix + _T("enums_folder.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_ENUMS_FOLDER
    bmp = cbLoadBitmap(prefix + _T("preproc_folder.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_PREPROC_FOLDER
    bmp = cbLoadBitmap(prefix + _T("others_folder.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_OTHERS_FOLDER
    bmp = cbLoadBitmap(prefix + _T("typedefs_folder.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_TYPEDEF_FOLDER
    bmp = cbLoadBitmap(prefix + _T("macro.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_MACRO
    bmp = cbLoadBitmap(prefix + _T("macro_private.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_MACRO_PRIVATE
    bmp = cbLoadBitmap(prefix + _T("macro_protected.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_MACRO_PROTECTED
    bmp = cbLoadBitmap(prefix + _T("macro_public.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_MACRO_PUBLIC
    bmp = cbLoadBitmap(prefix + _T("macro_folder.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_MACRO_FOLDER

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
    m_ParserPerWorkspace = cfg->ReadBool(_T("/parser_per_workspace"), false);

    // hook to project loading procedure
    ProjectLoaderHooks::HookFunctorBase* myhook = new ProjectLoaderHooks::HookFunctor<NativeParser>(this, &NativeParser::OnProjectLoadingHook);
    m_HookId = ProjectLoaderHooks::RegisterHook(myhook);

    Connect(ParserCommon::idParserStart, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(NativeParser::OnParserStart));
    Connect(ParserCommon::idParserEnd,   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(NativeParser::OnParserEnd));
    Connect(idTimerParsingOneByOne,      wxEVT_TIMER,                 wxTimerEventHandler(NativeParser::OnParsingOneByOneTimer));
}

NativeParser::~NativeParser()
{
    Disconnect(ParserCommon::idParserStart, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(NativeParser::OnParserStart));
    Disconnect(ParserCommon::idParserEnd,   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(NativeParser::OnParserEnd));
    Disconnect(idTimerParsingOneByOne,      wxEVT_TIMER,                 wxTimerEventHandler(NativeParser::OnParsingOneByOneTimer));
    ProjectLoaderHooks::UnregisterHook(m_HookId, true);
    RemoveClassBrowser();
    ClearParsers();
    Delete(m_ImageList);
    Delete(m_TempParser);
}

ParserBase* NativeParser::GetParserByProject(cbProject* project)
{
    if (m_ParserPerWorkspace)
    {
        std::set<cbProject*>::iterator it = m_ParsedProjects.find(project);
        if (it != m_ParsedProjects.end())
            return m_ParserList.begin()->second;
    }
    else
    {
        for (ParserList::const_iterator it = m_ParserList.begin(); it != m_ParserList.end(); ++it)
        {
            if (it->first == project)
                return it->second;
        }
    }

    TRACE(_T("GetProjectByParser() : Returning nullptr."));
    return nullptr;
}

ParserBase* NativeParser::GetParserByFilename(const wxString& filename)
{
    cbProject* project = GetProjectByFilename(filename);
    return GetParserByProject(project);
}

cbProject* NativeParser::GetProjectByParser(ParserBase* parser)
{
    for (ParserList::const_iterator it = m_ParserList.begin(); it != m_ParserList.end(); ++it)
    {
        if (it->second == parser)
            return it->first;
    }

    TRACE(_T("GetProjectByParser() : Returning NULL."));
    return NULL;
}

cbProject* NativeParser::GetProjectByFilename(const wxString& filename)
{
    TRACE(_T("GetProjectByFilename() : %s"), filename.wx_str());
    cbProject* activeProject = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (activeProject)
    {
        ParserBase* parser = GetParserByProject(activeProject);
        if (   (   parser
                && parser->IsFileParsed(filename) )
            || activeProject->GetFileByFilename(filename, false, true) )
        {
            return activeProject;
        }
        else
        {
            ProjectsArray* projs = Manager::Get()->GetProjectManager()->GetProjects();
            for (size_t i = 0; i < projs->GetCount(); ++i)
            {
                cbProject* project = projs->Item(i);
                if (!project || project == activeProject)
                    continue;

                parser = GetParserByProject(project);
                if (   (   parser
                        && parser->IsFileParsed(filename) )
                    || project->GetFileByFilename(filename, false, true) )
                {
                    return project;
                }
            }
        }
    }

    return nullptr;
}

cbProject* NativeParser::GetProjectByEditor(cbEditor* editor)
{
    if (!editor)
        return nullptr;
    ProjectFile* pf = editor->GetProjectFile();
    if (pf && pf->GetParentProject())
        return pf->GetParentProject();
    return GetProjectByFilename(editor->GetFilename());
}

cbProject* NativeParser::GetCurrentProject()
{
    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    cbProject* project = GetProjectByEditor(editor);
    if (!project)
        project = Manager::Get()->GetProjectManager()->GetActiveProject();
    return project;
}

bool NativeParser::Done()
{
    TRACE(_T("NativeParser::Done()"));

    bool done = true;
    for (ParserList::const_iterator it = m_ParserList.begin(); it != m_ParserList.end(); ++it)
    {
        if (!it->second->Done())
        {
            done = false;
            break;
        }
    }

    return done;
}

int NativeParser::GetTokenKindImage(const Token* token)
{
    if (!token)
        return PARSER_IMG_NONE;

    switch (token->m_TokenKind)
    {
        case tkPreprocessor:      return PARSER_IMG_PREPROCESSOR;

        case tkEnum:
            switch (token->m_Scope)
            {
                case tsPublic:    return PARSER_IMG_ENUM_PUBLIC;
                case tsProtected: return PARSER_IMG_ENUM_PROTECTED;
                case tsPrivate:   return PARSER_IMG_ENUM_PRIVATE;
                default:          return PARSER_IMG_ENUM;
            }

        case tkEnumerator:        return PARSER_IMG_ENUMERATOR;

        case tkClass:
            switch (token->m_Scope)
            {
                case tsPublic:    return PARSER_IMG_CLASS_PUBLIC;
                case tsProtected: return PARSER_IMG_CLASS_PROTECTED;
                case tsPrivate:   return PARSER_IMG_CLASS_PRIVATE;
                default:          return PARSER_IMG_CLASS_PUBLIC;
            }

        case tkNamespace:         return PARSER_IMG_NAMESPACE;

        case tkTypedef:
            switch (token->m_Scope)
            {
                case tsPublic:    return PARSER_IMG_TYPEDEF_PUBLIC;
                case tsProtected: return PARSER_IMG_TYPEDEF_PROTECTED;
                case tsPrivate:   return PARSER_IMG_TYPEDEF_PRIVATE;
                default:          return PARSER_IMG_TYPEDEF;
            }

        case tkMacro:
            switch (token->m_Scope)
            {
                case tsPublic:    return PARSER_IMG_MACRO_PUBLIC;
                case tsProtected: return PARSER_IMG_MACRO_PROTECTED;
                case tsPrivate:   return PARSER_IMG_MACRO_PRIVATE;
                default:          return PARSER_IMG_MACRO;
            }

        case tkConstructor:
            switch (token->m_Scope)
            {
                case tsProtected: return PARSER_IMG_CTOR_PROTECTED;
                case tsPrivate:   return PARSER_IMG_CTOR_PRIVATE;
                default:          return PARSER_IMG_CTOR_PUBLIC;
            }

        case tkDestructor:
            switch (token->m_Scope)
            {
                case tsProtected: return PARSER_IMG_DTOR_PROTECTED;
                case tsPrivate:   return PARSER_IMG_DTOR_PRIVATE;
                default:          return PARSER_IMG_DTOR_PUBLIC;
            }

        case tkFunction:
            switch (token->m_Scope)
            {
                case tsProtected: return PARSER_IMG_FUNC_PROTECTED;
                case tsPrivate:   return PARSER_IMG_FUNC_PRIVATE;
                default:          return PARSER_IMG_FUNC_PUBLIC;
            }

        case tkVariable:
            switch (token->m_Scope)
            {
                case tsProtected: return PARSER_IMG_VAR_PROTECTED;
                case tsPrivate:   return PARSER_IMG_VAR_PRIVATE;
                default:          return PARSER_IMG_VAR_PUBLIC;
            }

        default:                  return PARSER_IMG_NONE;
    }
}

wxArrayString NativeParser::GetAllPathsByFilename(const wxString& filename)
{
    TRACE(_T("NativeParser::GetAllPathsByFilename()"));

    wxArrayString dirs;
    const wxFileName fn(filename);

    wxDir dir(fn.GetPath());
    if (!dir.IsOpened())
        return wxArrayString();

    wxArrayString files;
    NativeParserHelper::ParserDirTraverser traverser(wxEmptyString, files);
    const wxString filespec = fn.HasExt() ? fn.GetName() + _T(".*") : fn.GetName();
    CCLogger::Get()->DebugLog(_T("Traversing '") + fn.GetPath() + _T("' for: ") + filespec);
    dir.Traverse(traverser, filespec, wxDIR_FILES);

    // only find one file in the dir, go other place
    if (files.GetCount() == 1)
    {
        cbProject* project = IsParserPerWorkspace() ? GetCurrentProject()
                                                    : GetProjectByParser(m_Parser);
        // search in the project
        if (project)
        {
            const wxString prjPath = project->GetCommonTopLevelPath();
            wxString priorityPath;
            if (fn.HasExt() && (fn.GetExt().StartsWith(_T("h")) || fn.GetExt().StartsWith(_T("c"))))
            {
                wxFileName priFn(prjPath);
                // hard-coded candidate path, the ./sdk or ./include under the project top level folder
                priFn.AppendDir(fn.GetExt().StartsWith(_T("h")) ? _T("sdk") : _T("include"));
                if (priFn.DirExists())
                {
                    priorityPath = priFn.GetFullPath();
                    wxDir priorityDir(priorityPath);
                    if ( priorityDir.IsOpened() )
                    {
                        wxArrayString priorityPathSub;
                        NativeParserHelper::ParserDirTraverser traverser(wxEmptyString, priorityPathSub);
                        CCLogger::Get()->DebugLog(_T("Traversing '") + priorityPath + _T("' for: ") + filespec);
                        priorityDir.Traverse(traverser, filespec, wxDIR_FILES | wxDIR_DIRS);
                        if (priorityPathSub.GetCount() == 1)
                            AddPaths(dirs, priorityPathSub[0], fn.HasExt());
                    }
                }
            }

            if (dirs.IsEmpty())
            {
                wxDir prjDir(prjPath);
                if (prjDir.IsOpened())
                {
                    // try to search the project top level folder
                    wxArrayString prjDirSub;
                    NativeParserHelper::ParserDirTraverser traverser(priorityPath, prjDirSub);
                    CCLogger::Get()->DebugLog(_T("Traversing '") + priorityPath + wxT(" - ") + prjPath + _T("' for: ") + filespec);
                    prjDir.Traverse(traverser, filespec, wxDIR_FILES | wxDIR_DIRS);
                    if (prjDirSub.GetCount() == 1)
                        AddPaths(dirs, prjDirSub[0], fn.HasExt());
                }
            }
        }
    }

    CCLogger::Get()->DebugLog(F(_T("Found %lu files:"), static_cast<unsigned long>(files.GetCount())));
    for (size_t i=0; i<files.GetCount(); i++)
        CCLogger::Get()->DebugLog(F(_T("- %s"), files[i].wx_str()));

    if (!files.IsEmpty())
        AddPaths(dirs, files[0], fn.HasExt());

    return dirs;
}

void NativeParser::AddPaths(wxArrayString& dirs, const wxString& path, bool hasExt)
{
    wxString s;
    if (hasExt)
        s = UnixFilename(path.BeforeLast(_T('.'))) + _T(".");
    else
        s = UnixFilename(path);

    if (dirs.Index(s, false) == wxNOT_FOUND)
        dirs.Add(s);
}

ParserBase* NativeParser::CreateParser(cbProject* project)
{
    if ( GetParserByProject(project) )
    {
        CCLogger::Get()->DebugLog(_T("Parser for this project already exists!"));
        return nullptr;
    }

    // Easy case for "one parser per workspace" that has already been created:
    if (m_ParserPerWorkspace && !m_ParsedProjects.empty())
        return m_ParserList.begin()->second;

    TRACE(_T("NativeParser::CreateParser()"));

    ParserBase* parser = new Parser(this, project);
    if ( !DoFullParsing(project, parser) )
    {
        CCLogger::Get()->DebugLog(_T("Full parsing failed!"));
        delete parser;
        return nullptr;
    }

    if (m_Parser == m_TempParser)
        SetParser(parser); // Also updates class browser

    if (m_ParserPerWorkspace)
        m_ParsedProjects.insert(project);

    m_ParserList.push_back(std::make_pair(project, parser));

    wxString prj = (project ? project->GetTitle() : _T("*NONE*"));
    wxString log(F(_("Create new parser for project '%s'"), prj.wx_str()));
    CCLogger::Get()->Log(log);
    CCLogger::Get()->DebugLog(log);

    RemoveObsoleteParsers();

    return parser;
}

bool NativeParser::DeleteParser(cbProject* project)
{
    wxString prj = (project ? project->GetTitle() : _T("*NONE*"));

    ParserList::iterator it = m_ParserList.begin();
    if (!m_ParserPerWorkspace)
    {
        for (; it != m_ParserList.end(); ++it)
        {
            if (it->first == project)
                break;
        }
    }

    if (it == m_ParserList.end())
    {
        CCLogger::Get()->DebugLog(F(_T("Parser does not exist for delete '%s'!"), prj.wx_str()));
        return false;
    }

    TRACE(_T("NativeParser::DeleteParser()"));

    bool removeProjectFromParser = false;
    if (m_ParserPerWorkspace)
        removeProjectFromParser = RemoveProjectFromParser(project);

    if (m_ParsedProjects.empty())
    {
        if (it->second == m_Parser)
          SetParser(m_TempParser); // Also updates class browser

        wxString log(F(_("Delete parser for project '%s'!"), prj.wx_str()));
        CCLogger::Get()->Log(log);
        CCLogger::Get()->DebugLog(log);

        delete it->second;
        m_ParserList.erase(it);

        return true;
    }

    if (removeProjectFromParser)
        return true;

    CCLogger::Get()->DebugLog(_T("Delete parser failed!"));
    return false;
}

bool NativeParser::ReparseFile(cbProject* project, const wxString& filename)
{
    if (ParserCommon::FileType(filename) == ParserCommon::ftOther)
        return false;

    ParserBase* parser = GetParserByProject(project);
    if (!parser)
        return false;

    if (!parser->UpdateParsingProject(project))
        return false;

    TRACE(_T("NativeParser::ReparseFile()"));

    return parser->Reparse(filename);
}

bool NativeParser::AddFileToParser(cbProject* project, const wxString& filename, ParserBase* parser)
{
    if (ParserCommon::FileType(filename) == ParserCommon::ftOther)
        return false;

    if (!parser)
    {
        parser = GetParserByProject(project);
        if (!parser)
            return false;
    }

    if (!parser->UpdateParsingProject(project))
        return false;

    TRACE(_T("NativeParser::AddFileToParser()"));

    return parser->AddFile(filename, project);
}

bool NativeParser::RemoveFileFromParser(cbProject* project, const wxString& filename)
{
    ParserBase* parser = GetParserByProject(project);
    if (!parser)
        return false;

    TRACE(_T("NativeParser::RemoveFileFromParser()"));

    return parser->RemoveFile(filename);
}

void NativeParser::RereadParserOptions()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
    if (cfg->ReadBool(_T("/use_symbols_browser"), true))
    {
        if (!m_ClassBrowser)
        {
            CreateClassBrowser();
            UpdateClassBrowser();
        }
        // change class-browser docking settings
        else if (m_ClassBrowserIsFloating != cfg->ReadBool(_T("/as_floating_window"), false))
        {
            RemoveClassBrowser();
            CreateClassBrowser();
            // force re-update
            UpdateClassBrowser();
        }
    }
    else if (!cfg->ReadBool(_T("/use_symbols_browser"), true) && m_ClassBrowser)
        RemoveClassBrowser();

    const bool parserPerWorkspace = cfg->ReadBool(_T("/parser_per_workspace"), false);
    if (m_Parser == m_TempParser)
    {
        m_ParserPerWorkspace = parserPerWorkspace;
        return;
    }

    RemoveObsoleteParsers();

    // re-parse if settings changed
    ParserOptions opts = m_Parser->Options();
    m_Parser->ReadOptions();
    bool reparse = false;
    cbProject* project = GetCurrentProject();
    if (   opts.followLocalIncludes  != m_Parser->Options().followLocalIncludes
        || opts.followGlobalIncludes != m_Parser->Options().followGlobalIncludes
        || opts.wantPreprocessor     != m_Parser->Options().wantPreprocessor
        || opts.parseComplexMacros   != m_Parser->Options().parseComplexMacros
        || m_ParserPerWorkspace      != parserPerWorkspace )
    {
        // important options changed... flag for reparsing
        if (cbMessageBox(_("You changed some class parser options. Do you want to "
                           "reparse your projects now, using the new options?"),
                         _("Reparse?"), wxYES_NO | wxICON_QUESTION) == wxID_YES)
        {
            reparse = true;
        }
    }

    if (reparse)
        ClearParsers();

    m_ParserPerWorkspace = parserPerWorkspace;

    if (reparse)
        CreateParser(project);
}

void NativeParser::ReparseCurrentProject()
{
    cbProject* project = GetCurrentProject();
    if (project)
    {
        TRACE(_T("NativeParser::ReparseCurrentProject()"));
        DeleteParser(project);
        CreateParser(project);
    }
}

void NativeParser::ReparseSelectedProject()
{
    wxTreeCtrl* tree = Manager::Get()->GetProjectManager()->GetTree();
    if (!tree)
        return;

    wxTreeItemId treeItem = Manager::Get()->GetProjectManager()->GetTreeSelection();
    if (!treeItem.IsOk())
        return;

    const FileTreeData* data = static_cast<FileTreeData*>(tree->GetItemData(treeItem));
    if (!data)
        return;

    if (data->GetKind() == FileTreeData::ftdkProject)
    {
        cbProject* project = data->GetProject();
        if (project)
        {
            TRACE(_T("NativeParser::ReparseSelectedProject()"));
            DeleteParser(project);
            CreateParser(project);
        }
    }
}

// Here, we collect the "using namespace XXXX" directives
// Also, we locate the current caret in which function, then, add the function parameters to Token trie
// Also, the variables in the function body( local block ) was add to the Token trie
size_t NativeParser::MarkItemsByAI(ccSearchData* searchData,
                                   TokenIdxSet&  result,
                                   bool          reallyUseAI,
                                   bool          isPrefix,
                                   bool          caseSensitive,
                                   int           caretPos)
{
    result.clear();

    if (!m_Parser->Done())
    {
        wxString msg(_("The Parser is still parsing files."));
        msg += m_Parser->NotDoneReason();
        CCLogger::Get()->DebugLog(msg);
        return 0;
    }

    TRACE(_T("NativeParser::MarkItemsByAI_2()"));

    TokenTree* tree = m_Parser->GetTempTokenTree();

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    // remove old temporaries
    tree->Clear();

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    RemoveLastFunctionChildren(m_Parser->GetTokenTree(), m_LastFuncTokenIdx);

    // find "using namespace" directives in the file
    TokenIdxSet search_scope;
    ParseUsingNamespace(searchData, search_scope, caretPos);

    // parse function's arguments
    ParseFunctionArguments(searchData, caretPos);

    // parse current code block (from the start of function up to the cursor)
    ParseLocalBlock(searchData, caretPos);

    if (!reallyUseAI)
    {
        TokenTree* tree = m_Parser->GetTokenTree();

        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        // all tokens, no AI whatsoever
        for (size_t i = 0; i < tree->size(); ++i)
            result.insert(i);

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

        return result.size();
    }

    // we have correctly collected all the tokens, so we will do the artificial intelligence search
    return AI(result, searchData, wxEmptyString, isPrefix, caseSensitive, &search_scope, caretPos);
}

size_t NativeParser::MarkItemsByAI(TokenIdxSet& result,
                                   bool         reallyUseAI,
                                   bool         isPrefix,
                                   bool         caseSensitive,
                                   int          caretPos)
{
    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(F(_T("MarkItemsByAI_1()")));

    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!editor)
        return 0;

    ccSearchData searchData = { editor->GetControl(), editor->GetFilename() };
    if (!searchData.control)
        return 0;

    TRACE(_T("NativeParser::MarkItemsByAI_1()"));

    return MarkItemsByAI(&searchData, result, reallyUseAI, isPrefix, caseSensitive, caretPos);
}

void NativeParser::GetCallTips(int chars_per_line, wxArrayString &items, int& typedCommas, int pos)
{
    items.Clear();
    typedCommas = 0;
    int commas = 0;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !m_Parser->Done())
    {
        items.Add(wxT("Parsing at the moment..."));
        return;
    }

    TRACE(_T("NativeParser::GetCallTips()"));

    ccSearchData searchData = { ed->GetControl(), ed->GetFilename() };
    if (pos == wxNOT_FOUND)
        pos = searchData.control->GetCurrentPos();
    int nest = 0;
    while (--pos > 0)
    {
        const int style = searchData.control->GetStyleAt(pos);
        if (   searchData.control->IsString(style)
            || searchData.control->IsCharacter(style)
            || searchData.control->IsComment(style) )
        {
            continue;
        }

        const wxChar ch = searchData.control->GetCharAt(pos);
        if (ch == _T(';'))
            return;
        else if (ch == _T(','))
        {
            if (nest == 0)
                ++commas;
        }
        else if (ch == _T(')'))
            --nest;
        else if (ch == _T('('))
        {
            ++nest;
            if (nest > 0)
                break;
        }
    }// while

    // strip un-wanted
    while (--pos > 0)
    {
        if (   searchData.control->GetCharAt(pos) <= _T(' ')
            || searchData.control->IsComment(searchData.control->GetStyleAt(pos)) )
        {
            continue;
        }
        break;
    }

    const int start = searchData.control->WordStartPosition(pos, true);
    const int end = searchData.control->WordEndPosition(pos, true);
    const wxString target = searchData.control->GetTextRange(start, end);
    TRACE(_T("Sending \"%s\" for call-tip"), target.c_str());
    if (target.IsEmpty())
        return;

    TokenIdxSet result;
    MarkItemsByAI(result, true, false, true, end);

    ComputeCallTip(m_Parser->GetTokenTree(), result, chars_per_line, items);

    typedCommas = commas;
    TRACE(_T("GetCallTips() : typedCommas=%d"), typedCommas);
    items.Sort();
}

wxArrayString& NativeParser::GetProjectSearchDirs(cbProject* project)
{
    ProjectSearchDirsMap::iterator it;
    it = m_ProjectSearchDirsMap.find(project);
    if (it == m_ProjectSearchDirsMap.end())
        it = m_ProjectSearchDirsMap.insert(m_ProjectSearchDirsMap.end(), std::make_pair(project, wxArrayString()));

    return it->second;
}

void NativeParser::CreateClassBrowser()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
    if (m_ClassBrowser || !cfg->ReadBool(_T("/use_symbols_browser"), true))
        return;

    TRACE(_T("NativeParser::CreateClassBrowser()"));

    m_ClassBrowserIsFloating = cfg->ReadBool(_T("/as_floating_window"), false);

    if (m_ClassBrowserIsFloating)
    {
        m_ClassBrowser = new ClassBrowser(Manager::Get()->GetAppWindow(), this);

        // make this a free floating/docking window
        CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);

        evt.name = _T("SymbolsBrowser");
        evt.title = _("Symbols browser");
        evt.pWindow = m_ClassBrowser;
        evt.dockSide = CodeBlocksDockEvent::dsRight;
        evt.desiredSize.Set(200, 250);
        evt.floatingSize.Set(200, 250);
        evt.minimumSize.Set(150, 150);
        evt.shown = true;
        evt.hideable = true;
        Manager::Get()->ProcessEvent(evt);
        m_ClassBrowser->UpdateSash();
    }
    else
    {
        // make this a tab in projectmanager notebook
        m_ClassBrowser = new ClassBrowser(Manager::Get()->GetProjectManager()->GetNotebook(), this);
        Manager::Get()->GetProjectManager()->GetNotebook()->AddPage(m_ClassBrowser, _("Symbols"));
        m_ClassBrowser->UpdateSash();
    }

    // Dreaded DDE-open bug related: do not touch unless for a good reason
    // TODO (Loaden) ? what's bug? I test it, it's works well now.
    m_ClassBrowser->SetParser(m_Parser); // Also updates class browser
}

void NativeParser::RemoveClassBrowser(bool appShutDown)
{
    if (!m_ClassBrowser)
        return;

    TRACE(_T("NativeParser::RemoveClassBrowser()"));

    if (m_ClassBrowserIsFloating)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_ClassBrowser;
        Manager::Get()->ProcessEvent(evt);
    }
    else
    {
        int idx = Manager::Get()->GetProjectManager()->GetNotebook()->GetPageIndex(m_ClassBrowser);
        if (idx != -1)
            Manager::Get()->GetProjectManager()->GetNotebook()->RemovePage(idx);
    }
    m_ClassBrowser->Destroy();
    m_ClassBrowser = NULL;
}

void NativeParser::UpdateClassBrowser()
{
    if (!m_ClassBrowser)
          return;

    TRACE(_T("NativeParser::UpdateClassBrowser()"));

    if (   m_Parser != m_TempParser
        && m_Parser->Done()
        && !Manager::IsAppShuttingDown())
    {
        m_ClassBrowser->UpdateClassBrowserView();
    }
}

bool NativeParser::DoFullParsing(cbProject* project, ParserBase* parser)
{
    if (!parser)
        return false;

    TRACE(_T("NativeParser::DoFullParsing()"));

    if (!AddCompilerDirs(project, parser))
        CCLogger::Get()->DebugLog(_T("AddCompilerDirs failed!"));

    if (!AddCompilerPredefinedMacros(project, parser))
        CCLogger::Get()->DebugLog(_T("AddCompilerPredefinedMacros failed!"));

    if (!AddProjectDefinedMacros(project, parser))
        CCLogger::Get()->DebugLog(_T("AddProjectDefinedMacros failed!"));

    // add per-project dirs
    if (project)
    {
        wxArrayString& pdirs = GetProjectSearchDirs(project);
        wxString base = project->GetBasePath();
        for (size_t i = 0; i < pdirs.GetCount(); ++i)
        {
            wxString path = pdirs[i];
            Manager::Get()->GetMacrosManager()->ReplaceMacros(path);
            wxFileName dir(path);

            if (NormalizePath(dir, base))
                parser->AddIncludeDir(dir.GetFullPath());
            else
                CCLogger::Get()->DebugLog(F(_T("Error normalizing path: '%s' from '%s'"), path.wx_str(), base.wx_str()));
        }
    }

    StringList priority_files;
    StringList headers;
    StringList sources;

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
    // NOTE (Morten#1#): Keep this in sync with files in the XRC file (settings.xrc) and ccoptionsdlg.cpp
    const wxString default_priority_headers =
        _T("<cstddef>, <w32api.h>, ")
        _T("<wx/defs.h>, <wx/dlimpexp.h>, <wx/toplevel.h>, ")
        _T("<boost/config.hpp>, <boost/filesystem/config.hpp>, ")
        _T("\"pch.h\", \"sdk.h\", \"stdafx.h\"");
    wxString priority_headers = cfg->Read(_T("/priority_headers"), default_priority_headers);
    if (!priority_headers.StartsWith(default_priority_headers))
    {
        wxStringTokenizer default_ph(default_priority_headers, _T(","));
        wxArrayString default_phs;
        while (default_ph.HasMoreTokens())
            default_phs.Add(default_ph.GetNextToken().Trim(false).Trim(true));

        wxStringTokenizer user(priority_headers, _T(","));
        wxArrayString users;
        while (user.HasMoreTokens())
            users.Add(user.GetNextToken().Trim(false).Trim(true));

        priority_headers = default_priority_headers;
        for (size_t i = 0; i < users.GetCount(); ++i)
        {
            if (!users[i].IsEmpty() && default_phs.Index(users[i], false) == wxNOT_FOUND)
                priority_headers.Append(_T(", ") + users[i]);
        }

        cfg->Write(_T("/priority_headers"), priority_headers);
    }

    typedef std::map<int, wxString> PriorityMap;
    PriorityMap priorityMap;
    PriorityMap priorityTempMap;
    int priorityCnt = 0;
    wxStringTokenizer tkz(priority_headers, _T(","));
    while (tkz.HasMoreTokens())
    {
        wxString token = tkz.GetNextToken().Trim(false).Trim(true);
        if (token.Len() <= 2) // error, at least "" or <> is required
            continue;

        if (   parser->Options().followLocalIncludes
            && token[0] == _T('"')
            && token[token.Len() - 1] == _T('"') )
        {
            priorityTempMap[++priorityCnt] = token.SubString(1, token.Len() - 2).Trim(false).Trim(true);
        }
        else if (   parser->Options().followLocalIncludes
                 && token[0] == _T('<')
                 && token[token.Len() - 1] == _T('>') )
        {
            token = token.SubString(1, token.Len() - 2).Trim(false).Trim(true);
            wxArrayString inc_file = parser->FindFileInIncludeDirs(token);
            for (size_t i = 0; i < inc_file.GetCount(); ++i)
                priorityMap[++priorityCnt] = inc_file[i] + _T(", 1");
        }
    }

    if (project)
    {
        for (FilesList::const_iterator it = project->GetFilesList().begin(); it != project->GetFilesList().end(); ++it)
        {
            ProjectFile* pf = *it;
            if (!pf)
                continue;

            ParserCommon::EFileType ft = ParserCommon::FileType(pf->relativeFilename);
            if (ft == ParserCommon::ftHeader) // parse header files
            {
                bool isPriorityFile = false;
                for (PriorityMap::iterator it = priorityTempMap.begin(); it != priorityTempMap.end(); ++it)
                {
                    if (it->second.IsSameAs(pf->file.GetFullName(), false))
                    {
                        isPriorityFile = true;
                        priorityMap[it->first] = pf->file.GetFullPath() + _T(", 0");
                        priorityTempMap.erase(it);
                        break;
                    }
                }

                if (!isPriorityFile)
                    headers.push_back(pf->file.GetFullPath());
            }
            else if (ft == ParserCommon::ftSource) // parse source files
            {
                sources.push_back(pf->file.GetFullPath());
            }
        }
    }

    for (PriorityMap::iterator it = priorityMap.begin(); it != priorityMap.end(); ++it)
        priority_files.push_back(it->second);

    CCLogger::Get()->DebugLog(_T("Passing list of files to batch-parser."));

    // parse priority files
    wxString prj = (project ? project->GetTitle() : _T("*NONE*"));
    if (!priority_files.empty())
    {
        for (StringList::iterator it = priority_files.begin(); it != priority_files.end(); ++it)
        {
            wxString& file = *it;
            const bool systemHeaderFile = (file.Last() == _T('1'));
            const int pos = file.Find(_T(','), true);
            file = file.Left(pos);
            CCLogger::Get()->DebugLog(F(_T("Header to parse with priority: '%s'"), file.wx_str()));
            parser->AddPriorityHeaders(file, systemHeaderFile);
        }

        CCLogger::Get()->DebugLog(F(_T("Add %lu priority parsing file(s) for project '%s'..."),
                                    static_cast<unsigned long>(priority_files.size()), prj.wx_str()));
    }

    if (!headers.empty() || !sources.empty())
    {
        CCLogger::Get()->DebugLog(F(_T("Added %lu file(s) for project '%s' to batch-parser..."),
                                    static_cast<unsigned long>(headers.size() + sources.size()), prj.wx_str()));
        parser->AddBatchParse(headers);
        parser->AddBatchParse(sources);
    }

    return true;
}

bool NativeParser::SwitchParser(cbProject* project, ParserBase* parser)
{
    if (!parser || parser == m_Parser || GetParserByProject(project) != parser)
    {
        TRACE(_T("NativeParser::SwitchParser(): No need to / cannot switch."));
        return false;
    }

    TRACE(_T("NativeParser::SwitchParser()"));

    SetParser(parser); // Also updates class browser

    wxString prj = (project ? project->GetTitle() : _T("*NONE*"));
    wxString log(F(_("Switch parser to project '%s'"), prj.wx_str()));
    CCLogger::Get()->Log(log);
    CCLogger::Get()->DebugLog(log);

    return true;
}

void NativeParser::SetParser(ParserBase* parser)
{
    if (m_Parser == parser)
        return;

    RemoveLastFunctionChildren(m_Parser->GetTokenTree(), m_LastFuncTokenIdx);
    InitCCSearchVariables();
    m_Parser = parser;

    if (m_ClassBrowser)
        m_ClassBrowser->SetParser(parser); // Also updates class browser
}

void NativeParser::ClearParsers()
{
    TRACE(_T("NativeParser::ClearParsers()"));

    if (m_ParserPerWorkspace)
    {
        while (!m_ParsedProjects.empty() && DeleteParser(*m_ParsedProjects.begin()))
            ;
    }
    else
    {
        while (!m_ParserList.empty() && DeleteParser(m_ParserList.begin()->first))
            ;
    }
}

void NativeParser::RemoveObsoleteParsers()
{
    TRACE(_T("NativeParser::RemoveObsoleteParsers()"));

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
    const size_t maxParsers = cfg->ReadInt(_T("/max_parsers"), 5);
    wxArrayString removedProjectNames;
    std::pair<cbProject*, ParserBase*> info = GetParserInfoByCurrentEditor();

    while (m_ParserList.size() > maxParsers)
    {
        bool deleted = false;
        for (ParserList::const_iterator it = m_ParserList.begin(); it != m_ParserList.end(); ++it)
        {
            if (it->second == info.second)
                continue;

            if (DeleteParser(it->first))
            {
                wxString prj = (it->first ? it->first->GetTitle() : _T("*NONE*"));
                removedProjectNames.Add(prj);
                deleted = true;
                break;
            }
        }

        if (!deleted)
            break;
    }

    for (size_t i = 0; i < removedProjectNames.GetCount(); ++i)
    {
        wxString log(F(_("Removed obsolete parser of '%s'"), removedProjectNames[i].wx_str()));
        CCLogger::Get()->Log(log);
        CCLogger::Get()->DebugLog(log);
    }
}

std::pair<cbProject*, ParserBase*> NativeParser::GetParserInfoByCurrentEditor()
{
    std::pair<cbProject*, ParserBase*> info(nullptr, nullptr);
    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (editor && editor->GetFilename() != g_StartHereTitle)
    {
        info.first  = GetProjectByEditor(editor);
        info.second = GetParserByProject(info.first);
    }

    return info;
}

void NativeParser::SetTokenKindImage(int kind, const wxBitmap& bitmap, const wxBitmap& mask)
{
    if (kind < PARSER_IMG_MIN || kind > PARSER_IMG_MAX)
        return;
#ifdef __WXMSW__
    m_ImageList->Replace(kind, bitmap, mask);
#endif
}

void NativeParser::SetTokenKindImage(int kind, const wxBitmap& bitmap, const wxColour& maskColour)
{
    if (kind < PARSER_IMG_MIN || kind > PARSER_IMG_MAX)
        return;
    m_ImageList->Replace(kind, bitmap);//, maskColour);
}

void NativeParser::SetTokenKindImage(int kind, const wxIcon& icon)
{
    if (kind < PARSER_IMG_MIN || kind > PARSER_IMG_MAX)
        return;
    m_ImageList->Replace(kind, icon);
}

void NativeParser::SetCBViewMode(const BrowserViewMode& mode)
{
    m_Parser->ClassBrowserOptions().showInheritance = (mode == bvmInheritance) ? true : false;
    UpdateClassBrowser();
}

void NativeParser::OnProjectLoadingHook(cbProject* project, TiXmlElement* elem, bool loading)
{
    if (loading)
    {
        // Hook called when loading project file.
        wxArrayString& pdirs = GetProjectSearchDirs(project);

        TiXmlElement* CCConf = elem->FirstChildElement("code_completion");
        if (CCConf)
        {
            TiXmlElement* pathsElem = CCConf->FirstChildElement("search_path");
            while (pathsElem)
            {
                if (pathsElem->Attribute("add"))
                {
                    wxString dir = cbC2U(pathsElem->Attribute("add"));
                    if (pdirs.Index(dir) == wxNOT_FOUND)
                        pdirs.Add(dir);
                }

                pathsElem = pathsElem->NextSiblingElement("search_path");
            }
        }
    }
    else
    {
        // Hook called when saving project file.
        wxArrayString& pdirs = GetProjectSearchDirs(project);

        // since rev4332, the project keeps a copy of the <Extensions> element
        // and re-uses it when saving the project (so to avoid losing entries in it
        // if plugins that use that element are not loaded atm).
        // so, instead of blindly inserting the element, we must first check it's
        // not already there (and if it is, clear its contents)
        TiXmlElement* node = elem->FirstChildElement("code_completion");
        if (!node)
            node = elem->InsertEndChild(TiXmlElement("code_completion"))->ToElement();
        if (node)
        {
            node->Clear();
            for (size_t i = 0; i < pdirs.GetCount(); ++i)
            {
                TiXmlElement* path = node->InsertEndChild(TiXmlElement("search_path"))->ToElement();
                if (path) path->SetAttribute("add", cbU2C(pdirs[i]));
            }
        }
    }
}

// helper funcs

// Start an Artificial Intelligence (!) sequence to gather all the matching tokens..
// The actual AI is in FindAIMatches() below...
size_t NativeParser::AI(TokenIdxSet&    result,
                        ccSearchData*   searchData,
                        const wxString& lineText,
                        bool            isPrefix,
                        bool            caseSensitive,
                        TokenIdxSet*    search_scope,
                        int             caretPos)
{
    m_LastAISearchWasGlobal = false;
    m_LastAIGlobalSearch.Clear();

    int pos = caretPos == -1 ? searchData->control->GetCurrentPos() : caretPos;
    if (pos < 0 || pos > searchData->control->GetLength())
        return 0;

    m_EditorStartWord = searchData->control->WordStartPosition(pos, true);
    m_EditorEndWord   = pos; //editor->GetControl()->WordEndPosition(pos, true);
    int line = searchData->control->LineFromPosition(pos);

    // Get the actual search text, such as "objA.m_aaa.m_bbb"
    wxString actual_search(lineText);
    if (actual_search.IsEmpty())
    {
        // Get the position at the start of current line
        const int startPos = searchData->control->PositionFromLine(line);
        actual_search = searchData->control->GetTextRange(startPos, pos).Trim();
    }

    // Do the whole job here
    if (s_DebugSmartSense)
    {
        CCLogger::Get()->DebugLog(_T("AI() ========================================================="));
        CCLogger::Get()->DebugLog(F(_T("AI() Doing AI for '%s':"), actual_search.wx_str()));
    }
    TRACE(_T("NativeParser::AI()"));

    TokenTree* tree = m_Parser->GetTokenTree();

    // find current function's namespace so we can include local scope's tokens
    // we ' ll get the function's token (all matches) and add its parent namespace
    TokenIdxSet proc_result;
    size_t found_at = FindCurrentFunctionToken(searchData, proc_result);

    TokenIdxSet scope_result;
    if (found_at)
        FindCurrentFunctionScope(tree, proc_result, scope_result);

    // add additional search scopes???
    // for example, we are here:
    /*  void ClassA::FunctionB(int paraC){
            m_aaa
    */
    // then, ClassA should be added as a search_scope, the global scope should be added too.

    // if search_scope is already defined, then, add scope_result to search_scope
    // otherwise we just set search_scope as scope_result
    if (!search_scope)
        search_scope = &scope_result;
    else
    {
        // add scopes
        for (TokenIdxSet::const_iterator it = scope_result.begin(); it != scope_result.end(); ++it)
            search_scope->insert(*it);
    }

    // remove non-namespace/class tokens
    CleanupSearchScope(tree, search_scope);

    // find all other matches
    std::queue<ParserComponent> components;
    BreakUpComponents(actual_search, components);

    m_LastAISearchWasGlobal = components.size() <= 1;
    if (!components.empty())
        m_LastAIGlobalSearch = components.front().component;

    ResolveExpression(tree, components, *search_scope, result, caseSensitive, isPrefix);

    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(F(_T("AI() AI leave, returned %lu results"),static_cast<unsigned long>(result.size())));

    return result.size();
}

// find a function where current caret located.
// We need to find extra class scope, otherwise, we will failed do the cc in a class declaration
size_t NativeParser::FindCurrentFunctionToken(ccSearchData* searchData, TokenIdxSet& result, int caretPos)
{
    TokenIdxSet scope_result;
    wxString procName;
    wxString scopeName;
    FindCurrentFunctionStart(searchData, &scopeName, &procName, nullptr, caretPos);

    if (procName.IsEmpty())
        return 0;

    // add current scope
    if (!scopeName.IsEmpty())
    {
        // _namespace ends with double-colon (::). remove it
        scopeName.RemoveLast();
        scopeName.RemoveLast();

        // search for namespace
        std::queue<ParserComponent> ns;
        BreakUpComponents(scopeName, ns);

        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        // No critical section needed in this recursive function!
        // All functions that call this recursive FindAIMatches function, should already entered a critical section.
        FindAIMatches(m_Parser->GetTokenTree(), ns, scope_result, -1,
                      true, true, false, tkNamespace | tkClass | tkTypedef);

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
    }

    // if no scope, use global scope
    if (scope_result.empty())
        scope_result.insert(-1);

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    for (TokenIdxSet::const_iterator it = scope_result.begin(); it != scope_result.end(); ++it)
    {
        GenerateResultSet(m_Parser->GetTokenTree(), procName, *it, result,
                          true, false, tkAnyFunction | tkClass);
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return result.size();
}

// returns current function's position (not line) in the editor
int NativeParser::FindCurrentFunctionStart(ccSearchData* searchData,
                                           wxString*     nameSpace,
                                           wxString*     procName,
                                           int*          functionIndex,
                                           int           caretPos)
{
    // cache last result for optimization
    int pos = caretPos == -1 ? searchData->control->GetCurrentPos() : caretPos;
    if ((pos < 0) || (pos > searchData->control->GetLength()))
    {
        if (s_DebugSmartSense)
            CCLogger::Get()->DebugLog(F(_T("FindCurrentFunctionStart() Cannot determine position. caretPos=%d, control=%d"),
                                        caretPos, searchData->control->GetCurrentPos()));
        return -1;
    }

    TRACE(_T("NativeParser::FindCurrentFunctionStart()"));

    const int curLine = searchData->control->LineFromPosition(pos) + 1;
    if (   (curLine == m_LastLine)
        && ( (searchData->control == m_LastControl) && (!searchData->control->GetModify()) )
        && (searchData->file == m_LastFile) )
    {
        if (nameSpace)     *nameSpace     = m_LastNamespace;
        if (procName)      *procName      = m_LastPROC;
        if (functionIndex) *functionIndex = m_LastFunctionIndex;

        if (s_DebugSmartSense)
            CCLogger::Get()->DebugLog(F(_T("FindCurrentFunctionStart() Cached namespace='%s', cached proc='%s' (returning %d)"),
                                        m_LastNamespace.wx_str(), m_LastPROC.wx_str(), m_LastResult));

        return m_LastResult;
    }

    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(F(_T("FindCurrentFunctionStart() Looking for tokens in '%s'"),
                                    searchData->file.wx_str()));
    m_LastFile    = searchData->file;
    m_LastControl = searchData->control;
    m_LastLine    = curLine;

    // we have all the tokens in the current file, then just do a loop on all
    // the tokens, see if the line is in the token's imp.
    TokenIdxSet result;
    size_t num_results = m_Parser->FindTokensInFile(searchData->file, result, tkAnyFunction | tkClass);
    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(F(_T("FindCurrentFunctionStart() Found %lu results"), static_cast<unsigned long>(num_results)));

    TokenTree* tree = m_Parser->GetTokenTree();

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    const int idx = GetTokenFromCurrentLine(tree, result, curLine, searchData->file);
    const Token* token = tree->at(idx);
    if (token)
    {
        // got it :)
        if (s_DebugSmartSense)
            CCLogger::Get()->DebugLog(F(_T("FindCurrentFunctionStart() Current function: '%s' (at line %u)"),
                                        token->DisplayName().wx_str(),
                                        token->m_ImplLine));

        m_LastNamespace      = token->GetNamespace();
        m_LastPROC           = token->m_Name;
        m_LastFunctionIndex  = token->m_Index;
        m_LastResult         = searchData->control->PositionFromLine(token->m_ImplLine - 1);

        // locate function's opening brace
        if (token->m_TokenKind & tkAnyFunction)
        {
            while (m_LastResult < searchData->control->GetTextLength())
            {
                wxChar ch = searchData->control->GetCharAt(m_LastResult);
                if (ch == _T('{'))
                    break;
                else if (ch == 0)
                {
                    if (s_DebugSmartSense)
                        CCLogger::Get()->DebugLog(_T("FindCurrentFunctionStart() Can't determine functions opening brace..."));

                    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
                    return -1;
                }

                ++m_LastResult;
            }
        }

        if (nameSpace)     *nameSpace     = m_LastNamespace;
        if (procName)      *procName      = m_LastPROC;
        if (functionIndex) *functionIndex = token->m_Index;

        if (s_DebugSmartSense)
            CCLogger::Get()->DebugLog(F(_T("FindCurrentFunctionStart() Namespace='%s', proc='%s' (returning %d)"),
                                        m_LastNamespace.wx_str(), m_LastPROC.wx_str(), m_LastResult));

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
        return m_LastResult;
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(_T("FindCurrentFunctionStart() Can't determine current function..."));

    m_LastResult = -1;
    return -1;
}

bool NativeParser::SkipWhitespaceForward(cbEditor* editor, int& pos)
{
    if (!editor)
        return false;
    wxChar ch = editor->GetControl()->GetCharAt(pos);
    int len = editor->GetControl()->GetLength() - 1;
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
    {
        while (pos < len && (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n'))
        {
            ++pos;
            ch = editor->GetControl()->GetCharAt(pos);
        }
        return true;
    }
    return false;
}

bool NativeParser::SkipWhitespaceBackward(cbEditor* editor, int& pos)
{
    if (!editor)
        return false;
    wxChar ch = editor->GetControl()->GetCharAt(pos);
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
    {
        while (pos > 0 && (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n'))
        {
            --pos;
            ch = editor->GetControl()->GetCharAt(pos);
        }
        return true;
    }
    return false;
}

bool NativeParser::ParseUsingNamespace(ccSearchData* searchData, TokenIdxSet& search_scope, int caretPos)
{
    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(_T("ParseUsingNamespace() Parse file scope for \"using namespace\""));
    TRACE(_T("NativeParser::ParseUsingNamespace()"));

    wxArrayString ns;
    int pos = caretPos == -1 ? searchData->control->GetCurrentPos() : caretPos;
    if (pos < 0 || pos > searchData->control->GetLength())
        return false;

    // Get the buffer from begin of the editor to the current caret position
    wxString buffer = searchData->control->GetTextRange(0, pos);
    m_Parser->ParseBufferForUsingNamespace(buffer, ns);

    TokenTree* tree = m_Parser->GetTokenTree();

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    for (size_t i = 0; i < ns.GetCount(); ++i)
    {
        std::queue<ParserComponent> components;
        BreakUpComponents(ns[i], components);

        int parentIdx = -1;
        while (!components.empty())
        {
            ParserComponent pc = components.front();
            components.pop();

            int id = tree->TokenExists(pc.component, parentIdx, tkNamespace);
            if (id == -1)
            {
                parentIdx = -1;
                break;
            }
            parentIdx = id;
        }

        if (s_DebugSmartSense && parentIdx != -1)
        {
            const Token* token = tree->at(parentIdx);
            if (token)
                CCLogger::Get()->DebugLog(F(_T("ParseUsingNamespace() Found %s%s"),
                                            token->GetNamespace().wx_str(), token->m_Name.wx_str()));
        }
        search_scope.insert(parentIdx);
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return true;
}

bool NativeParser::ParseFunctionArguments(ccSearchData* searchData, int caretPos)
{
    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(_T("ParseFunctionArguments() Parse function arguments"));
    TRACE(_T("NativeParser::ParseFunctionArguments()"));

    TokenIdxSet proc_result;

    TokenTree* tree = m_Parser->GetTokenTree(); // the one used inside FindCurrentFunctionToken, FindAIMatches and GenerateResultSet

    size_t found_at = FindCurrentFunctionToken(searchData, proc_result, caretPos);
    if (!found_at)
    {
        if (s_DebugSmartSense)
            CCLogger::Get()->DebugLog(_T("ParseFunctionArguments() Could not determine current function's namespace..."));
        TRACE(_T("ParseFunctionArguments() Could not determine current function's namespace..."));
        return false;
    }

    const int pos = caretPos == -1 ? searchData->control->GetCurrentPos() : caretPos;
    const unsigned int curLine = searchData->control->LineFromPosition(pos) + 1;

    bool locked = false;
    for (TokenIdxSet::const_iterator it = proc_result.begin(); it != proc_result.end(); ++it)
    {
        wxString buffer;
        int initLine = -1;
        int tokenIdx = -1;

        if (locked)
            CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)
        locked = true;

        const Token* token = tree->at(*it);

        if (!token)
            continue;
        if (curLine < token->m_ImplLineStart || curLine > token->m_ImplLineEnd)
            continue;

        if (s_DebugSmartSense)
            CCLogger::Get()->DebugLog(_T("ParseFunctionArguments() + Function match: ") + token->m_Name);
        TRACE(_T("ParseFunctionArguments() + Function match: ") + token->m_Name);

        if (!token->m_Args.IsEmpty() && !token->m_Args.Matches(_T("()")))
        {
            buffer = token->m_Args;
            // Now we have something like "(int my_int, const TheClass* my_class, float f)"
            buffer.Remove(0, 1);              // remove (
            buffer.RemoveLast();              // remove )
            // Now we have                "int my_int, const TheClass* my_class, float f"
            buffer.Replace(_T(","), _T(";")); // replace commas with semi-colons
            // Now we have                "int my_int; const TheClass* my_class; float f"
            buffer << _T(';');                // aid parser ;)
            // Finally we have            "int my_int; const TheClass* my_class; float f;"
            buffer.Trim();

            if (s_DebugSmartSense)
                CCLogger::Get()->DebugLog(F(_T("ParseFunctionArguments() Parsing arguments: \"%s\""), buffer.wx_str()));

            if (!buffer.IsEmpty())
            {
                const int textLength= searchData->control->GetLength();
                if (textLength == -1)
                    continue;
                int paraPos = searchData->control->PositionFromLine(token->m_ImplLine - 1);
                if (paraPos == -1)
                    continue;
                while (paraPos < textLength && searchData->control->GetCharAt(paraPos++) != _T('('))
                    ;
                while (paraPos < textLength && searchData->control->GetCharAt(paraPos++) < _T(' '))
                    ;
                initLine = searchData->control->LineFromPosition(paraPos) + 1;
                if (initLine == -1)
                    continue;
                tokenIdx = token->m_Index;
            }
        }

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
        locked = false;

        if (   !buffer.IsEmpty()
            && !m_Parser->ParseBuffer(buffer, false, false, true, searchData->file, tokenIdx, initLine)
            && s_DebugSmartSense)
        {
            CCLogger::Get()->DebugLog(_T("ParseFunctionArguments() Error parsing arguments."));
        }
    }

    if (locked)
        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return true;
}

bool NativeParser::ParseLocalBlock(ccSearchData* searchData, int caretPos)
{
    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(_T("ParseLocalBlock() Parse local block"));
    TRACE(_T("NativeParser::ParseLocalBlock()"));

    int parentIdx = -1;
    int blockStart = FindCurrentFunctionStart(searchData, nullptr, nullptr, &parentIdx, caretPos);
    int initLine = 0;
    if (parentIdx != -1)
    {
        TokenTree* tree = m_Parser->GetTokenTree();

        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        const Token* parent = tree->at(parentIdx);
        if (parent && (parent->m_TokenKind & tkAnyFunction))
        {
            m_LastFuncTokenIdx = parent->m_Index;
            initLine = parent->m_ImplLineStart;
        }

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

        if (!parent)
            return false;
    }

    if (blockStart != -1)
    {
        ++blockStart; // skip {
        const int pos = caretPos == -1 ? searchData->control->GetCurrentPos() : caretPos;
        const int line = searchData->control->LineFromPosition(pos);
        const int blockEnd = searchData->control->GetLineEndPosition(line);
        if (blockEnd < 0 || blockEnd > searchData->control->GetLength())
        {
            if (s_DebugSmartSense)
            {
                CCLogger::Get()->DebugLog(F(_T("ParseLocalBlock() ERROR blockEnd=%d and edLength=%d?!"),
                                            blockEnd, searchData->control->GetLength()));
            }
            return false;
        }

        if (blockStart >= blockEnd)
            blockStart = blockEnd;

        wxString buffer = searchData->control->GetTextRange(blockStart, blockEnd);
        buffer.Trim();
        if (   !buffer.IsEmpty()
            && !m_Parser->ParseBuffer(buffer, false, false, true, searchData->file, m_LastFuncTokenIdx, initLine) )
        {
            if (s_DebugSmartSense)
                CCLogger::Get()->DebugLog(_T("ParseLocalBlock() ERROR parsing block:\n") + buffer);
        }
        else
        {
            if (s_DebugSmartSense)
            {
                CCLogger::Get()->DebugLog(F(_T("ParseLocalBlock() Block:\n%s"), buffer.wx_str()));
                CCLogger::Get()->DebugLog(_T("ParseLocalBlock() Local tokens:"));

                TokenTree* tree = m_Parser->GetTokenTree();

                CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

                for (size_t i = 0; i < tree->size(); ++i)
                {
                    const Token* token = tree->at(i);
                    if (token && token->m_IsTemp)
                    {
                        wxString log(wxString::Format(_T(" + %s (%d)"), token->DisplayName().wx_str(), token->m_Index));
                        const Token* parent = tree->at(token->m_ParentIndex);
                        if (parent)
                            log += wxString::Format(_T("; Parent = %s (%d)"), parent->m_Name.wx_str(), token->m_ParentIndex);
                        CCLogger::Get()->DebugLog(log);
                    }
                }

                CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
            }
            return true;
        }
    }
    else
    {
        if (s_DebugSmartSense)
            CCLogger::Get()->DebugLog(_T("ParseLocalBlock() Could not determine current block start..."));
    }
    return false;
}

bool NativeParser::AddCompilerDirs(cbProject* project, ParserBase* parser)
{
    if (!parser)
        return false;

    TRACE(_T("NativeParser::AddCompilerDirs()"));

    // If there is no project, work on default compiler
    if (!project)
    {
        Compiler* compiler = CompilerFactory::GetDefaultCompiler();
        if (compiler)
        {
            // these dirs were the user's compiler include search dirs
            const wxArrayString& dirs = compiler->GetIncludeDirs();
            for (size_t i = 0; i < dirs.GetCount(); ++i)
            {
                wxString path = dirs[i];
                Manager::Get()->GetMacrosManager()->ReplaceMacros(path);
                parser->AddIncludeDir(path);
            }

            if (compiler->GetID().Contains(_T("gcc")))
                AddGCCCompilerDirs(compiler->GetMasterPath(), compiler->GetPrograms().CPP, parser);
        }

        return true;
    }

    // Otherwise (if there is a project), work on the project's compiler...
    wxString base = project->GetBasePath();
    parser->AddIncludeDir(base); // add project's base path
    TRACE(_T("AddCompilerDirs() : Adding project base dir to parser: ") + base);

    // ...so we can access post-processed project's search dirs
    Compiler* compiler = CompilerFactory::GetCompiler(project->GetCompilerID());
    cb::shared_ptr<CompilerCommandGenerator> generator(compiler ? compiler->GetCommandGenerator(project) : nullptr);
    if (compiler && generator)
        generator->Init(project);

    // get project include dirs
    for (unsigned int i = 0; i < project->GetIncludeDirs().GetCount(); ++i)
    {
        wxString out = project->GetIncludeDirs()[i];
        Manager::Get()->GetMacrosManager()->ReplaceMacros(out);
        wxFileName dir(out);
        if ( NormalizePath(dir, base) )
        {
            parser->AddIncludeDir(dir.GetFullPath());
            TRACE(_T("AddCompilerDirs() : Adding project dir to parser: ") + dir.GetFullPath());
        }
        else
            CCLogger::Get()->DebugLog(F(_T("Error normalizing path: '%s' from '%s'"), out.wx_str(), base.wx_str()));
    }

    // alloc array for project compiler AND "no. of targets" times target compilers
    int nCompilers = 1 + project->GetBuildTargetsCount();
    Compiler** Compilers = new Compiler* [nCompilers];
    memset(Compilers, 0, sizeof(Compiler*) * nCompilers);
    nCompilers = 0; // reset , use as insert index in the next for loop

    // get targets include dirs
    for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
    {
        ProjectBuildTarget* target = project->GetBuildTarget(i);
        if (target && target->SupportsCurrentPlatform())
        {
            if (compiler && generator)
            {
                // post-processed search dirs (from build scripts)
                for (unsigned int ti = 0; ti < generator->GetCompilerSearchDirs(target).GetCount(); ++ti)
                {
                    wxString out = generator->GetCompilerSearchDirs(target)[ti];
                    wxFileName dir(out);
                    if ( NormalizePath(dir, base) )
                    {
                        parser->AddIncludeDir(dir.GetFullPath());
                        TRACE(_T("AddCompilerDirs() : Adding compiler target dir to parser: ") + dir.GetFullPath());
                    }
                    else
                        CCLogger::Get()->DebugLog(F(_T("Error normalizing path: '%s' from '%s'"), out.wx_str(), base.wx_str()));
                }
            }

            // apply target vars
//            target->GetCustomVars().ApplyVarsToEnvironment();
            for (unsigned int ti = 0; ti < target->GetIncludeDirs().GetCount(); ++ti)
            {
                wxString out = target->GetIncludeDirs()[ti];
                Manager::Get()->GetMacrosManager()->ReplaceMacros(out);
                wxFileName dir(out);
                if ( NormalizePath(dir, base) )
                {
                    parser->AddIncludeDir(dir.GetFullPath());
                    TRACE(_T("AddCompilerDirs() : Adding target dir to parser: ") + dir.GetFullPath());
                }
                else
                    CCLogger::Get()->DebugLog(F(_T("Error normalizing path: '%s' from '%s'"), out.wx_str(), base.wx_str()));
            }
            // get the compiler
            wxString CompilerIndex = target->GetCompilerID();
            Compiler* tgtCompiler = CompilerFactory::GetCompiler(CompilerIndex);
            if (tgtCompiler)
            {
                Compilers[nCompilers] = tgtCompiler;
                ++nCompilers;
            }
        } // if (target)
    } // end loop over the targets

    // add the project compiler to the array of compilers
    if (compiler)
    {   // note it might be possible that this compiler is already in the list
        // no need to worry since the compiler list of the parser will filter out duplicate
        // entries in the include dir list
        Compilers[nCompilers++] = compiler;
    }

    // add compiler include dirs
    for (int idxCompiler = 0; idxCompiler < nCompilers; ++idxCompiler)
    {
        const wxArrayString& dirs = (Compilers[idxCompiler])->GetIncludeDirs();
        for (unsigned int i = 0; i < dirs.GetCount(); ++i)
        {
//            CCLogger::Get()->Log(mltDevDebug, "Adding %s", dirs[i].c_str());
            wxString out = dirs[i];
            Manager::Get()->GetMacrosManager()->ReplaceMacros(out);
            wxFileName dir(out);
            if ( NormalizePath(dir,base) )
            {
                parser->AddIncludeDir(dir.GetFullPath());
                TRACE(_T("AddCompilerDirs() : Adding compiler dir to parser: ") + dir.GetFullPath());
            }
            else
                CCLogger::Get()->DebugLog(F(_T("Error normalizing path: '%s' from '%s'"), out.wx_str(), base.wx_str()));
        }

        // find out which compiler, if gnu, do the special trick
        // to find it's internal include paths
        // but do only once per C::B session, thus cache for later calls
        wxString CompilerID = (Compilers[idxCompiler])->GetID();
        if (CompilerID.Contains(_T("gcc")))
            AddGCCCompilerDirs(Compilers[idxCompiler]->GetMasterPath(), Compilers[idxCompiler]->GetPrograms().CPP, parser);
    } // end of while loop over the found compilers

    if (!nCompilers)
        CCLogger::Get()->DebugLog(_T("No compilers found!"));

    delete [] Compilers;
    return true;
}

bool NativeParser::AddCompilerPredefinedMacros(cbProject* project, ParserBase* parser)
{
    if (!parser)
        return false;

    if (!parser->Options().wantPreprocessor)
        return true;

    TRACE(_T("NativeParser::AddCompilerPredefinedMacros()"));

    // Default compiler is used for for single file parser (non project)
    wxString compilerId = project ? project->GetCompilerID() : CompilerFactory::GetDefaultCompilerID();

    wxString defs;
    // gcc
    if (compilerId.Contains(_T("gcc")))
    {
        if ( !AddCompilerPredefinedMacrosGCC(compilerId, project, defs) )
            return false;
    }
    // vc
    else if (compilerId.StartsWith(_T("msvc")))
    {
        if ( !AddCompilerPredefinedMacrosVC(compilerId, defs) )
          return false;
    }

    TRACE(_T("Add compiler predefined preprocessor macros:\n%s"), defs.wx_str());
    parser->AddPredefinedMacros(defs);
    return true;
}

bool NativeParser::AddCompilerPredefinedMacrosGCC(const wxString& compilerId, cbProject* project, wxString& defs)
{
    Compiler* compiler = CompilerFactory::GetCompiler(compilerId);
    if (!compiler)
        return false;

    wxFileName fn(wxEmptyString, compiler->GetPrograms().CPP);
    wxString masterPath = compiler->GetMasterPath();
    Manager::Get()->GetMacrosManager()->ReplaceMacros(masterPath);
    fn.SetPath(masterPath);
    fn.AppendDir(_T("bin"));

    const wxString cpp_compiler(fn.GetFullPath());
    static std::map<wxString, wxString> gccDefsMap;
    if (gccDefsMap[cpp_compiler].IsEmpty())
    {
#ifdef __WXMSW__
        const wxString args(_T(" -E -dM -x c++ nul"));
#else
        const wxString args(_T(" -E -dM -x c++ /dev/null"));
#endif

        // wxExecute can be a long action and C::B might have been shutdown in the meantime...
        if (Manager::IsAppShuttingDown())
            return false;

        static bool reentry = false;
        if (reentry)
            return false;

        wxArrayString output;
        reentry = true;
        if ( wxExecute(cpp_compiler + args, output, wxEXEC_SYNC | wxEXEC_NODISABLE) == -1 )
        {
            TRACE(_T("AddCompilerPredefinedMacrosGCC::wxExecute failed!"));
            reentry = false;
            return false;
        }
        reentry = false;

        wxString& gccDefs = gccDefsMap[cpp_compiler];
        for (size_t i = 0; i < output.Count(); ++i)
            gccDefs += output[i] + _T("\n");
    }

    static const wxString cxx0xOption(_T("-std=c++0x"));
    static const wxString gnu0xOption(_T("-std=gnu++0x"));
    bool useCxx0x = false;
    if (project)
    {
        const wxArrayString& options = project->GetCompilerOptions();
        if (   options.Index(cxx0xOption) != wxNOT_FOUND
            || options.Index(gnu0xOption) != wxNOT_FOUND )
        {
            useCxx0x = true;
        }
        else
        {
            for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
            {
                ProjectBuildTarget* target = project->GetBuildTarget(i);
                const wxArrayString& targetOptions = target->GetCompilerOptions();
                if (   targetOptions.Index(cxx0xOption) != wxNOT_FOUND
                    || targetOptions.Index(gnu0xOption) != wxNOT_FOUND )
                {
                    useCxx0x = true;
                    break;
                }
            }
        }
    }

    if (useCxx0x)
        defs = gccDefsMap[cpp_compiler] + _T("#define __GXX_EXPERIMENTAL_CXX0X__ 1 \n");
    else
        defs = gccDefsMap[cpp_compiler];

    return true;
}

bool NativeParser::AddCompilerPredefinedMacrosVC(const wxString& compilerId, wxString& defs)
{
    static wxString vcDefs;
    static bool     firstExecute = true;
    if (firstExecute)
    {
        firstExecute = false;
        Compiler* compiler = CompilerFactory::GetCompiler(compilerId);
        if (!compiler)
            return false;

        wxString masterPath = compiler->GetMasterPath();
        Manager::Get()->GetMacrosManager()->ReplaceMacros(masterPath);
        wxString cmd = masterPath + _T("\\bin\\") + compiler->GetPrograms().C;

        // wxExecute can be a long action and C::B might have been shutdown in the meantime...
        if (Manager::IsAppShuttingDown())
            return false;

        static bool reentry = false;
        if (reentry)
            return false;

        wxArrayString output, error;
        reentry = true;
        if (wxExecute(cmd, output, error, wxEXEC_SYNC | wxEXEC_NODISABLE) == -1)
        {
            TRACE(_T("AddCompilerPredefinedMacrosVC::wxExecute failed!"));
            reentry = false;
            return false;
        }
        reentry = false;

        if (error.IsEmpty())
        {
            TRACE(_T("AddCompilerPredefinedMacrosVC:: Can't get pre-defined macros for MSVC."));
            return false;
        }

        wxString str = error[0];
        wxString tmp(_T("Microsoft (R) "));
        int pos = str.Find(tmp);
        if (pos != wxNOT_FOUND)
        {
            wxString bit = str.Mid(pos + tmp.Length(), 2);
            if (bit == _T("32"))
                defs += _T("#define _WIN32") _T("\n");
            else if (bit == _T("64"))
                defs += _T("#define _WIN64") _T("\n");
        }

        tmp = _T("Compiler Version ");
        pos = str.Find(tmp);
        if (pos != wxNOT_FOUND)
        {
            wxString ver = str.Mid(pos + tmp.Length(), 4);
            pos = ver.Find(_T('.'));
            if (pos != wxNOT_FOUND)
            {
                ver[pos] = ver[pos + 1];
                ver[pos + 1] = _T('0');
                defs += _T("#define _MSC_VER ") + ver;
            }
        }
    }

    defs = vcDefs;

    return true;
}

bool NativeParser::AddProjectDefinedMacros(cbProject* project, ParserBase* parser)
{
    if (!parser)
        return false;

    if (!project)
        return true;

    TRACE(_T("NativeParser::AddProjectDefinedMacros()"));

    wxString compilerId = project->GetCompilerID();
    wxString param;
    if (compilerId.Contains(_T("gcc")))
        param = _T("-D");
    else if (compilerId.StartsWith(_T("msvc")))
        param = _T("/D");

    if (param.IsEmpty())
        return true;

    wxString defs;
    wxArrayString opts = project->GetCompilerOptions();
    ProjectBuildTarget* target = project->GetBuildTarget(project->GetActiveBuildTarget());
    if (target != NULL)
    {
        wxArrayString targetOpts = target->GetCompilerOptions();
        for (size_t i = 0; i < targetOpts.GetCount(); ++i)
            opts.Add(targetOpts[i]);
    }

    for (size_t i = 0; i < opts.GetCount(); ++i)
    {
        wxString def = opts[i];
        Manager::Get()->GetMacrosManager()->ReplaceMacros(def);
        if (!def.StartsWith(param))
            continue;

        def = def.Right(def.Length() - param.Length());
        int pos = def.Find(_T('='));
        if (pos != wxNOT_FOUND)
            def[pos] = _T(' ');

        defs += _T("#define ") + def + _T("\n");
    }

    TRACE(_T("Add project and current build target defined preprocessor macros:\n%s"), defs.wx_str());
    parser->AddPredefinedMacros(defs);
    return true;
}

// These dirs are the built-in search dirs of the compiler itself (GCC).
// Such as when you install your MinGW GCC in E:/code/MinGW/bin
// The buildin search dir may contains: E:/code/MinGW/include
const wxArrayString& NativeParser::GetGCCCompilerDirs(const wxString &cpp_compiler)
{
    // keep the gcc compiler path's once if found across C::B session
    // makes opening workspaces a *lot* faster by avoiding endless calls to the compiler
    static std::map<wxString, wxArrayString> dirs;
    if (!dirs[cpp_compiler].IsEmpty())
        return dirs[cpp_compiler];

    // wxExecute can be a long action and C::B might have been shutdown in the meantime...
    // This is here, to protect at re-entry:
    if (Manager::IsAppShuttingDown())
        return dirs[cpp_compiler];

    TRACE(_T("NativeParser::GetGCCCompilerDirs()"));

    // for starters , only do this for gnu compiler
    //CCLogger::Get()->DebugLog(_T("CompilerID ") + CompilerID);
    //
    //   Windows: mingw32-g++ -v -E -x c++ nul
    //   Linux  : g++ -v -E -x c++ /dev/null
    // do the trick only for c++, not needed then for C (since this is a subset of C++)


    // let's construct the command
    // use a null file handler
    // both works fine in Windows and Linux

    wxString Command(cpp_compiler + _T(" -v -E -x c++ /dev/null"));
    if (platform::windows)
      Command = cpp_compiler + _T(" -v -E -x c++ nul"); // on Windows, its different

    static bool flag = false;
    if (flag)
        return dirs[cpp_compiler];

    // action time  (everything shows up on the error stream
    wxArrayString Output, Errors;
    flag = true;
    if (wxExecute(Command, Output, Errors, wxEXEC_SYNC | wxEXEC_NODISABLE) == -1)
    {
        TRACE(_T("GetGCCCompilerDirs::wxExecute failed!"));
        flag = false;
        return dirs[cpp_compiler];
    }
    flag = false;

    // wxExecute can be a long action and C::B might have been shutdown in the meantime...
    // This is here, to protect a long run:
    if (Manager::IsAppShuttingDown())
        return dirs[cpp_compiler];

    // start from "#include <...>", and the path followed
    // let's hope this does not change too quickly, otherwise we need
    // to adjust our search code (for several versions ...)
    bool start = false;
    for (size_t idxCount = 0; idxCount < Errors.GetCount(); ++idxCount)
    {
        wxString path = Errors[idxCount].Trim(true).Trim(false);
        if (!start)
        {
            if (!path.StartsWith(_T("#include <...>")))
                continue;
            path = Errors[++idxCount].Trim(true).Trim(false);
            start = true;
        }

        wxFileName fname(path, wxEmptyString);
        fname.Normalize();
        fname.SetVolume(fname.GetVolume().MakeUpper());
        if (!fname.DirExists())
            break;

        CCLogger::Get()->DebugLog(_T("Caching GCC default include dir: ") + fname.GetPath());
        dirs[cpp_compiler].Add(fname.GetPath());
    }

    return dirs[cpp_compiler];
}

void NativeParser::AddGCCCompilerDirs(const wxString& masterPath, const wxString& compilerCpp, ParserBase* parser)
{
    wxFileName fn(wxEmptyString, compilerCpp);
    wxString masterPathNoMacros(masterPath);
    Manager::Get()->GetMacrosManager()->ReplaceMacros(masterPathNoMacros);
    fn.SetPath(masterPathNoMacros);
    fn.AppendDir(_T("bin"));

    const wxArrayString& gccDirs = GetGCCCompilerDirs(fn.GetFullPath());
    TRACE(_T("Adding %lu cached gcc dirs to parser..."), static_cast<unsigned long>(gccDirs.GetCount()));
    for (size_t i=0; i<gccDirs.GetCount(); ++i)
    {
        parser->AddIncludeDir(gccDirs[i]);
        TRACE(_T("AddCompilerDirs() : Adding cached compiler dir to parser: ") + gccDirs[i]);
    }
}

void NativeParser::OnParserStart(wxCommandEvent& event)
{
    TRACE(_T("NativeParser::OnParserStart()"));

    cbProject* project = static_cast<cbProject*>(event.GetClientData());
    wxString prj = (project ? project->GetTitle() : _T("*NONE*"));
    const ParserCommon::ParserState state = static_cast<ParserCommon::ParserState>(event.GetInt());

    switch (state)
    {
    case ParserCommon::ptCreateParser:
        CCLogger::Get()->DebugLog(F(_("Starting batch parsing for project '%s'..."), prj.wx_str()));
        {
            std::pair<cbProject*, ParserBase*> info = GetParserInfoByCurrentEditor();
            if (info.second && m_Parser != info.second)
            {
                CCLogger::Get()->DebugLog(_T("Start switch from OnParserStart::ptCreateParser"));
                SwitchParser(info.first, info.second); // Calls SetParser() which also calls UpdateClassBrowserView()
            }
        }
        break;

    case ParserCommon::ptAddFileToParser:
        CCLogger::Get()->DebugLog(F(_("Starting add file parsing for project '%s'..."), prj.wx_str()));
        break;

    case ParserCommon::ptReparseFile:
        CCLogger::Get()->DebugLog(F(_("Starting re-parsing for project '%s'..."), prj.wx_str()));
        break;

    case ParserCommon::ptUndefined:
        if (event.GetString().IsEmpty())
            CCLogger::Get()->DebugLog(F(_("Batch parsing error in project '%s'"), prj.wx_str()));
        else
            CCLogger::Get()->DebugLog(F(_("%s in project '%s'"), event.GetString().wx_str(), prj.wx_str()));
        return;
    }

    event.Skip();
}

void NativeParser::OnParserEnd(wxCommandEvent& event)
{
    TRACE(_T("NativeParser::OnParserEnd()"));

    ParserBase* parser = reinterpret_cast<ParserBase*>(event.GetEventObject());
    cbProject* project = static_cast<cbProject*>(event.GetClientData());
    wxString prj = (project ? project->GetTitle() : _T("*NONE*"));
    const ParserCommon::ParserState state = static_cast<ParserCommon::ParserState>(event.GetInt());

    switch (state)
    {
    case ParserCommon::ptCreateParser:
        {
            wxString log(F(_("Project '%s' parsing stage done!"), prj.wx_str()));
            CCLogger::Get()->Log(log);
            CCLogger::Get()->DebugLog(log);
        }
        break;

    case ParserCommon::ptAddFileToParser:
        break;

    case ParserCommon::ptReparseFile:
        if (parser != m_Parser)
        {
            std::pair<cbProject*, ParserBase*> info = GetParserInfoByCurrentEditor();
            if (info.second && info.second != m_Parser)
            {
                CCLogger::Get()->DebugLog(_T("Start switch from OnParserEnd::ptReparseFile"));
                SwitchParser(info.first, info.second); // Calls SetParser() which also calls UpdateClassBrowserView()
            }
        }
        break;

    case ParserCommon::ptUndefined:
        CCLogger::Get()->DebugLog(F(_T("Parser event handling error of project '%s'"), prj.wx_str()));
        return;
    }

    if (!event.GetString().IsEmpty())
        CCLogger::Get()->DebugLog(event.GetString());

    UpdateClassBrowser();
    m_TimerParsingOneByOne.Start(500, wxTIMER_ONE_SHOT);

    event.Skip();
}

void NativeParser::OnParsingOneByOneTimer(wxTimerEvent& event)
{
    TRACE(_T("NativeParser::OnParsingOneByOneTimer()"));

    std::pair<cbProject*, ParserBase*> info = GetParserInfoByCurrentEditor();
    if (m_ParserPerWorkspace)
    {
        // If there is no parser and an active editor file can be obtained, parse the file according the active project
        if (!info.second && Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor())
        {
            // NOTE (Morten#1#): Shouldn't this actually be a temp parser??? I think this screws things with re-opening files on load of a projects...
            AddProjectToParser(info.first);
            CCLogger::Get()->DebugLog(_T("Add foreign active editor to current active project's parser."));
        }
        // Otherwise, there is a parser already present
        else
        {
            // First: try to parse the active project (if any)
            cbProject* activeProject = Manager::Get()->GetProjectManager()->GetActiveProject();
            if (m_ParsedProjects.find(activeProject) == m_ParsedProjects.end())
            {
                AddProjectToParser(activeProject);
                CCLogger::Get()->DebugLog(_T("Add new (un-parsed) active project to parser."));
            }
            // Else: add remaining projects one-by-one (if any)
            else
            {
                ProjectsArray* projs = Manager::Get()->GetProjectManager()->GetProjects();
                for (size_t i = 0; i < projs->GetCount(); ++i)
                {
                    // Only add, if the project is not already parsed
                    if (m_ParsedProjects.find(projs->Item(i)) == m_ParsedProjects.end())
                    {
                        AddProjectToParser(projs->Item(i));
                        CCLogger::Get()->DebugLog(_T("Add additional (next) project to parser."));
                        break;
                    }
                }
            }
        }
    }
    else if (info.first && !info.second)
    {
        info.second = CreateParser(info.first);
        if (info.second && info.second != m_Parser)
        {
            CCLogger::Get()->DebugLog(_T("Start switch from OnParsingOneByOneTimer"));
            SwitchParser(info.first, info.second); // Calls SetParser() which also calls UpdateClassBrowserView()
        }
    }
    TRACE(_T("NativeParser::~OnParsingOneByOneTimer()"));
}

void NativeParser::OnEditorActivated(EditorBase* editor)
{
    const wxString& activatedFile = editor->GetFilename();
    if (activatedFile == g_StartHereTitle)
    {
        SetParser(m_TempParser); // Also updates class browser
        return;
    }

    cbEditor* curEditor = Manager::Get()->GetEditorManager()->GetBuiltinEditor(editor);
    if (!curEditor)
        return;

    if ( !wxFile::Exists(activatedFile) )
        return;

    cbProject* project = GetProjectByEditor(curEditor);
    const int pos = m_StandaloneFiles.Index(activatedFile);
    if (project && pos != wxNOT_FOUND)
    {
        m_StandaloneFiles.RemoveAt(pos);
        if (m_StandaloneFiles.IsEmpty())
            DeleteParser(NULL);
        else
            RemoveFileFromParser(NULL, activatedFile);
    }

    ParserBase* parser = GetParserByProject(project);
    if (!parser)
    {
        ParserCommon::EFileType ft = ParserCommon::FileType(activatedFile);
        if (ft != ParserCommon::ftOther && (parser = CreateParser(project)))
        {
            if (!project && AddFileToParser(project, activatedFile, parser) )
            {
                wxFileName file(activatedFile);
                parser->AddIncludeDir(file.GetPath());
                m_StandaloneFiles.Add(activatedFile);
            }
        }
        else
            parser = m_TempParser; // do *not* instead by SetParser(m_TempParser)
    }
    else if (!project)
    {
        if (   !parser->IsFileParsed(activatedFile)
            && m_StandaloneFiles.Index(activatedFile) == wxNOT_FOUND
            && AddFileToParser(project, activatedFile, parser) )
        {
            wxFileName file(activatedFile);
            parser->AddIncludeDir(file.GetPath());
            m_StandaloneFiles.Add(activatedFile);
        }
    }

    if (parser != m_Parser)
    {
        CCLogger::Get()->DebugLog(_T("Start switch from OnEditorActivatedTimer"));
        SwitchParser(project, parser); // Calls SetParser() which also calls UpdateClassBrowserView()
    }

    if (m_ClassBrowser)
    {
        if      (m_Parser->ClassBrowserOptions().displayFilter == bdfFile)
            m_ClassBrowser->UpdateClassBrowserView(true); // check header and implementation file swap
        else if (   m_ParserPerWorkspace // project view only available in case of one parser per WS
                 && m_Parser->ClassBrowserOptions().displayFilter == bdfProject)
            m_ClassBrowser->UpdateClassBrowserView();
    }
}

void NativeParser::OnEditorClosed(EditorBase* editor)
{
    wxString filename = editor->GetFilename();
    if (filename == g_StartHereTitle)
        return;

    const int pos = m_StandaloneFiles.Index(filename);
    if (pos != wxNOT_FOUND)
    {
        m_StandaloneFiles.RemoveAt(pos);
        if (m_StandaloneFiles.IsEmpty())
            DeleteParser(NULL);
        else
            RemoveFileFromParser(NULL, filename);
    }
}

void NativeParser::InitCCSearchVariables()
{
    m_LastControl       = nullptr;
    m_LastFunctionIndex = -1;
    m_EditorStartWord   = -1;
    m_EditorEndWord     = -1;
    m_LastLine          = -1;
    m_LastResult        = -1;
    m_LastFile.Clear();
    m_LastNamespace.Clear();
    m_LastPROC.Clear();
    m_CCItems.Clear();

    Reset();
}

void NativeParser::AddProjectToParser(cbProject* project)
{
    wxString prj = (project ? project->GetTitle() : _T("*NONE*"));
    ParserBase* parser = GetParserByProject(project);
    if (parser)
        return;

    if (m_ParsedProjects.empty())
        return;

    m_ParsedProjects.insert(project);
    parser = GetParserByProject(project);
    if (!parser)
        return;
    else if (!parser->UpdateParsingProject(project))
    {
        m_ParsedProjects.erase(project);
        return;
    }

    wxString log(F(_("Add project (%s) to parser"), prj.wx_str()));
    CCLogger::Get()->Log(log);
    CCLogger::Get()->DebugLog(log);

    if (!AddCompilerDirs(project, parser))
        CCLogger::Get()->DebugLog(_T("AddCompilerDirs failed!"));

    if (!AddCompilerPredefinedMacros(project, parser))
        CCLogger::Get()->DebugLog(_T("AddCompilerPredefinedMacros failed!"));

    if (!AddProjectDefinedMacros(project, parser))
        CCLogger::Get()->DebugLog(_T("AddProjectDefinedMacros failed!"));

    if (project)
    {
        size_t fileCount = 0;
        for (FilesList::const_iterator it = project->GetFilesList().begin(); it != project->GetFilesList().end(); ++it)
        {
            ProjectFile* pf = *it;
            if (pf && FileTypeOf(pf->relativeFilename) == ftHeader)
            {
                if (AddFileToParser(project, pf->file.GetFullPath(), parser))
                    ++fileCount;
            }
        }
        for (FilesList::const_iterator it = project->GetFilesList().begin(); it != project->GetFilesList().end(); ++it)
        {
            ProjectFile* pf = *it;
            if (pf && FileTypeOf(pf->relativeFilename) == ftSource)
            {
                if (AddFileToParser(project, pf->file.GetFullPath(), parser))
                    fileCount++;
            }
        }

        wxString log(F(_("Done adding %lu files of project (%s) to parser."), static_cast<unsigned long>(fileCount), prj.wx_str()));
        CCLogger::Get()->DebugLog(log);
    }
    else
    {
        EditorBase* editor = Manager::Get()->GetEditorManager()->GetActiveEditor();
        if (editor && AddFileToParser(project, editor->GetFilename(), parser))
        {
            wxFileName file(editor->GetFilename());
            parser->AddIncludeDir(file.GetPath());
            m_StandaloneFiles.Add(editor->GetFilename());

            wxString log(F(_("Done adding stand-alone file (%s) of editor to parser."),
                           editor->GetFilename().wx_str()));
            CCLogger::Get()->DebugLog(log);
        }
    }
}

bool NativeParser::RemoveProjectFromParser(cbProject* project)
{
    ParserBase* parser = GetParserByProject(project);
    if (!parser)
        return false;

    // Remove from the cbProject set
    m_ParsedProjects.erase(project);

    if (!project || m_ParsedProjects.empty())
        return true;

    wxString prj = (project ? project->GetTitle() : _T("*NONE*"));
    wxString log(F(_("Remove project (%s) from parser"), prj.wx_str()));
    CCLogger::Get()->Log(log);
    CCLogger::Get()->DebugLog(log);

    for (FilesList::const_iterator it = project->GetFilesList().begin(); it != project->GetFilesList().end(); ++it)
    {
        ProjectFile* pf = *it;
        if (pf && ParserCommon::FileType(pf->relativeFilename) != ParserCommon::ftOther)
            RemoveFileFromParser(project, pf->file.GetFullPath());
    }

    return true;
}
