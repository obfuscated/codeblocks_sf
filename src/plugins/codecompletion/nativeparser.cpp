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

#if defined (CC_GLOBAL_DEBUG_OUTPUT)
    #if CC_GLOBAL_DEBUG_OUTPUT == 1
        #undef CC_NATIVEPARSER_DEBUG_OUTPUT
        #define CC_NATIVEPARSER_DEBUG_OUTPUT 1
    #elif CC_GLOBAL_DEBUG_OUTPUT == 2
        #undef CC_NATIVEPARSER_DEBUG_OUTPUT
        #define CC_NATIVEPARSER_DEBUG_OUTPUT 2
    #endif
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

bool s_DebugSmartSense = false;

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
    m_ImageList->Add(bmp); // PARSER_IMG_MACRO_DEF
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
    m_ImageList->Add(bmp); // PARSER_IMG_MACRO_DEF_FOLDER
    bmp = cbLoadBitmap(prefix + _T("others_folder.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_OTHERS_FOLDER
    bmp = cbLoadBitmap(prefix + _T("typedefs_folder.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_TYPEDEF_FOLDER
    bmp = cbLoadBitmap(prefix + _T("macro.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_MACRO_USE
    bmp = cbLoadBitmap(prefix + _T("macro_private.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_MACRO_USE_PRIVATE
    bmp = cbLoadBitmap(prefix + _T("macro_protected.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_MACRO_USE_PROTECTED
    bmp = cbLoadBitmap(prefix + _T("macro_public.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_MACRO_USE_PUBLIC
    bmp = cbLoadBitmap(prefix + _T("macro_folder.png"), wxBITMAP_TYPE_PNG);
    m_ImageList->Add(bmp); // PARSER_IMG_MACRO_USE_FOLDER

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

    TRACE(_T("NativeParser::GetParserByProject(): Returning nullptr."));
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

    TRACE(_T("NativeParser::GetProjectByParser(): Returning NULL."));
    return NULL;
}

cbProject* NativeParser::GetProjectByFilename(const wxString& filename)
{
    TRACE(_T("NativeParser::GetProjectByFilename(): %s"), filename.wx_str());
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
    bool done = true;
    for (ParserList::const_iterator it = m_ParserList.begin(); it != m_ParserList.end(); ++it)
    {
        if (!it->second->Done())
        {
            done = false;
            break;
        }
    }
    TRACE(_T("NativeParser::Done(): %s"), done ? _T("true"): _T("false"));
    return done;
}

int NativeParser::GetTokenKindImage(const Token* token)
{
    if (!token)
        return PARSER_IMG_NONE;

    switch (token->m_TokenKind)
    {
        case tkMacroDef:          return PARSER_IMG_MACRO_DEF;

        case tkEnum:
            switch (token->m_Scope)
            {
                case tsPublic:    return PARSER_IMG_ENUM_PUBLIC;
                case tsProtected: return PARSER_IMG_ENUM_PROTECTED;
                case tsPrivate:   return PARSER_IMG_ENUM_PRIVATE;
                case tsUndefined:
                default:          return PARSER_IMG_ENUM;
            }

        case tkEnumerator:        return PARSER_IMG_ENUMERATOR;

        case tkClass:
            switch (token->m_Scope)
            {
                case tsPublic:    return PARSER_IMG_CLASS_PUBLIC;
                case tsProtected: return PARSER_IMG_CLASS_PROTECTED;
                case tsPrivate:   return PARSER_IMG_CLASS_PRIVATE;
                case tsUndefined:
                default:          return PARSER_IMG_CLASS;
            }

        case tkNamespace:         return PARSER_IMG_NAMESPACE;

        case tkTypedef:
            switch (token->m_Scope)
            {
                case tsPublic:    return PARSER_IMG_TYPEDEF_PUBLIC;
                case tsProtected: return PARSER_IMG_TYPEDEF_PROTECTED;
                case tsPrivate:   return PARSER_IMG_TYPEDEF_PRIVATE;
                case tsUndefined:
                default:          return PARSER_IMG_TYPEDEF;
            }

        case tkMacroUse:
            switch (token->m_Scope)
            {
                case tsPublic:    return PARSER_IMG_MACRO_USE_PUBLIC;
                case tsProtected: return PARSER_IMG_MACRO_USE_PROTECTED;
                case tsPrivate:   return PARSER_IMG_MACRO_USE_PRIVATE;
                case tsUndefined:
                default:          return PARSER_IMG_MACRO_USE;
            }

        case tkConstructor:
            switch (token->m_Scope)
            {
                case tsProtected: return PARSER_IMG_CTOR_PROTECTED;
                case tsPrivate:   return PARSER_IMG_CTOR_PRIVATE;
                case tsUndefined:
                case tsPublic:
                default:          return PARSER_IMG_CTOR_PUBLIC;
            }

        case tkDestructor:
            switch (token->m_Scope)
            {
                case tsProtected: return PARSER_IMG_DTOR_PROTECTED;
                case tsPrivate:   return PARSER_IMG_DTOR_PRIVATE;
                case tsUndefined:
                case tsPublic:
                default:          return PARSER_IMG_DTOR_PUBLIC;
            }

        case tkFunction:
            switch (token->m_Scope)
            {
                case tsProtected: return PARSER_IMG_FUNC_PROTECTED;
                case tsPrivate:   return PARSER_IMG_FUNC_PRIVATE;
                case tsUndefined:
                case tsPublic:
                default:          return PARSER_IMG_FUNC_PUBLIC;
            }

        case tkVariable:
            switch (token->m_Scope)
            {
                case tsProtected: return PARSER_IMG_VAR_PROTECTED;
                case tsPrivate:   return PARSER_IMG_VAR_PRIVATE;
                case tsUndefined:
                case tsPublic:
                default:          return PARSER_IMG_VAR_PUBLIC;
            }

        case tkAnyContainer:
        case tkAnyFunction:
        case tkUndefined:
        default:                  return PARSER_IMG_NONE;
    }
}

wxArrayString NativeParser::GetAllPathsByFilename(const wxString& filename)
{
    TRACE(_T("NativeParser::GetAllPathsByFilename(): Enter"));

    wxArrayString dirs;
    const wxFileName fn(filename);

    wxDir dir(fn.GetPath());
    if (!dir.IsOpened())
        return wxArrayString();

    wxArrayString files;
    NativeParserHelper::ParserDirTraverser traverser(wxEmptyString, files);
    const wxString filespec = fn.HasExt() ? fn.GetName() + _T(".*") : fn.GetName();
    CCLogger::Get()->DebugLog(_T("NativeParser::GetAllPathsByFilename(): Traversing '") + fn.GetPath() + _T("' for: ") + filespec);

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
                        NativeParserHelper::ParserDirTraverser traverser_2(wxEmptyString, priorityPathSub);
                        CCLogger::Get()->DebugLog(_T("NativeParser::GetAllPathsByFilename(): Traversing '") + priorityPath + _T("' for: ") + filespec);
                        priorityDir.Traverse(traverser_2, filespec, wxDIR_FILES | wxDIR_DIRS);
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
                    NativeParserHelper::ParserDirTraverser traverser_2(priorityPath, prjDirSub);
                    CCLogger::Get()->DebugLog(_T("NativeParser::GetAllPathsByFilename(): Traversing '") + priorityPath + wxT(" - ") + prjPath + _T("' for: ") + filespec);
                    prjDir.Traverse(traverser_2, filespec, wxDIR_FILES | wxDIR_DIRS);
                    if (prjDirSub.GetCount() == 1)
                        AddPaths(dirs, prjDirSub[0], fn.HasExt());
                }
            }
        }
    }

    CCLogger::Get()->DebugLog(F(_T("NativeParser::GetAllPathsByFilename(): Found %lu files:"), static_cast<unsigned long>(files.GetCount())));
    for (size_t i=0; i<files.GetCount(); i++)
        CCLogger::Get()->DebugLog(F(_T("- %s"), files[i].wx_str()));

    if (!files.IsEmpty())
        AddPaths(dirs, files[0], fn.HasExt());

    TRACE(_T("NativeParser::GetAllPathsByFilename(): Leave"));
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
        CCLogger::Get()->DebugLog(_T("NativeParser::CreateParser(): Parser for this project already exists!"));
        return nullptr;
    }

    // Easy case for "one parser per workspace" that has already been created:
    if (m_ParserPerWorkspace && !m_ParsedProjects.empty())
        return m_ParserList.begin()->second;

    TRACE(_T("NativeParser::CreateParser(): Calling DoFullParsing()"));

    ParserBase* parser = new Parser(this, project);
    if ( !DoFullParsing(project, parser) )
    {
        CCLogger::Get()->DebugLog(_T("NativeParser::CreateParser(): Full parsing failed!"));
        delete parser;
        return nullptr;
    }

    if (m_Parser == m_TempParser)
        SetParser(parser); // Also updates class browser

    if (m_ParserPerWorkspace)
        m_ParsedProjects.insert(project);

    m_ParserList.push_back(std::make_pair(project, parser));

    wxString prj = (project ? project->GetTitle() : _T("*NONE*"));
    wxString log(F(_("NativeParser::CreateParser(): Finish creating a new parser for project '%s'"), prj.wx_str()));
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
        CCLogger::Get()->DebugLog(F(_T("NativeParser::DeleteParser(): Parser does not exist for delete '%s'!"), prj.wx_str()));
        return false;
    }

    bool removeProjectFromParser = false;
    if (m_ParserPerWorkspace)
        removeProjectFromParser = RemoveProjectFromParser(project);

    if (m_ParsedProjects.empty())
    {
        if (it->second == m_Parser)
          SetParser(m_TempParser); // Also updates class browser

        wxString log(F(_("NativeParser::DeleteParser(): Deleting parser for project '%s'!"), prj.wx_str()));
        CCLogger::Get()->Log(log);
        CCLogger::Get()->DebugLog(log);

        delete it->second;
        m_ParserList.erase(it);

        return true;
    }

    if (removeProjectFromParser)
        return true;

    CCLogger::Get()->DebugLog(_T("NativeParser::DeleteParser(): Deleting parser failed!"));
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

    TRACE(_T("NativeParser::ReparseFile(): Calling Parser::Reparse()"));

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

    TRACE(_T("NativeParser::AddFileToParser(): Calling Parser::AddFile()"));

    return parser->AddFile(filename, project);
}

bool NativeParser::RemoveFileFromParser(cbProject* project, const wxString& filename)
{
    ParserBase* parser = GetParserByProject(project);
    if (!parser)
        return false;

    TRACE(_T("NativeParser::RemoveFileFromParser(): Calling Parser::RemoveFile()"));

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
        TRACE(_T("NativeParser::ReparseCurrentProject(): Calling DeleteParser() and CreateParser()"));
        DeleteParser(project);
        CreateParser(project);
    }
}

void NativeParser::ReparseSelectedProject()
{
    wxTreeCtrl* tree = Manager::Get()->GetProjectManager()->GetUI().GetTree();
    if (!tree)
        return;

    wxTreeItemId treeItem = Manager::Get()->GetProjectManager()->GetUI().GetTreeSelection();
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
            TRACE(_T("NativeParser::ReparseSelectedProject(): Calling DeleteParser() and CreateParser()"));
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
    ParseLocalBlock(searchData, search_scope, caretPos);

    if (!reallyUseAI)
    {
        tree = m_Parser->GetTokenTree();

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

int NativeParser::GetCallTips(wxArrayString& items, int& typedCommas, cbEditor* ed, int pos)
{
    items.Clear();
    typedCommas = 0;
    int commas = 0;

    if (!ed || !m_Parser->Done())
    {
        items.Add(wxT("Parsing at the moment..."));
        return wxSCI_INVALID_POSITION;
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
            return wxSCI_INVALID_POSITION;
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
    TRACE(_T("Sending \"%s\" for call-tip"), target.wx_str());
    if (target.IsEmpty())
        return wxSCI_INVALID_POSITION;

    TokenIdxSet result;
    MarkItemsByAI(result, true, false, true, end);

    ComputeCallTip(m_Parser->GetTokenTree(), result, items);

    typedCommas = commas;
    TRACE(_T("NativeParser::GetCallTips(): typedCommas=%d"), typedCommas);
    items.Sort();
    return end;
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

    TRACE(_T("NativeParser::CreateClassBrowser(): Enter"));

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
        m_ClassBrowser = new ClassBrowser(Manager::Get()->GetProjectManager()->GetUI().GetNotebook(), this);
        Manager::Get()->GetProjectManager()->GetUI().GetNotebook()->AddPage(m_ClassBrowser, _("Symbols"));
        m_ClassBrowser->UpdateSash();
    }

    // Dreaded DDE-open bug related: do not touch unless for a good reason
    // TODO (Morten): ? what's bug? I test it, it's works well now.
    m_ClassBrowser->SetParser(m_Parser); // Also updates class browser

    TRACE(_T("NativeParser::CreateClassBrowser(): Leave"));
}

void NativeParser::RemoveClassBrowser(cb_unused bool appShutDown)
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
        int idx = Manager::Get()->GetProjectManager()->GetUI().GetNotebook()->GetPageIndex(m_ClassBrowser);
        if (idx != -1)
            Manager::Get()->GetProjectManager()->GetUI().GetNotebook()->RemovePage(idx);
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

    TRACE(_T("NativeParser::DoFullParsing(): Enter"));

    if (!AddCompilerDirs(project, parser))
        CCLogger::Get()->DebugLog(_T("NativeParser::DoFullParsing(): AddCompilerDirs failed!"));

    if (!AddCompilerPredefinedMacros(project, parser))
        CCLogger::Get()->DebugLog(_T("NativeParser::DoFullParsing(): AddCompilerPredefinedMacros failed!"));

    if (!AddProjectDefinedMacros(project, parser))
        CCLogger::Get()->DebugLog(_T("NativeParser::DoFullParsing(): AddProjectDefinedMacros failed!"));

    // add per-project dirs
    if (project)
    {
        AddIncludeDirsToParser(GetProjectSearchDirs(project),
                               project->GetBasePath(), parser);
    }

    StringList localSources;

    if (project)
    {
        for (FilesList::const_iterator fl_it = project->GetFilesList().begin();
             fl_it != project->GetFilesList().end(); ++fl_it)
        {
            ProjectFile* pf = *fl_it;
            if (!pf)
                continue;
            // check the file types in the project files
            ParserCommon::EFileType ft = ParserCommon::FileType(pf->relativeFilename);
            if (ft == ParserCommon::ftSource) // parse source files
            {
                localSources.push_back(pf->file.GetFullPath());
            }
        }
    }

    CCLogger::Get()->DebugLog(_T("NativeParser::DoFullParsing(): Adding cpp/c files to batch-parser"));

    // parse priority files
    wxString prj = (project ? project->GetTitle() : _T("*NONE*"));


    if (!localSources.empty())
    {
        CCLogger::Get()->DebugLog(F(_T("NativeParser::DoFullParsing(): Added %lu source file(s) for project '%s' to batch-parser..."),
                                    static_cast<unsigned long>( localSources.size()), prj.wx_str()));

        // local source files added to Parser
        parser->AddBatchParse(localSources);
    }

    TRACE(_T("NativeParser::DoFullParsing(): Leave"));

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
    TRACE(_T("NativeParser::RemoveObsoleteParsers(): Enter"));

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

            wxString prj = (it->first ? it->first->GetTitle() : _T("*NONE*"));
            if ( DeleteParser(it->first) )
            {
                // Please note that DeleteParser() may erase one element of the m_ParserList, so
                // do NOT use the constant iterator here again, as the element pointed by it may be
                // destroyed in DeleteParser().
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
        wxString log(F(_("NativeParser::RemoveObsoleteParsers():Removed obsolete parser of '%s'"), removedProjectNames[i].wx_str()));
        CCLogger::Get()->Log(log);
        CCLogger::Get()->DebugLog(log);
    }

    TRACE(_T("NativeParser::RemoveObsoleteParsers(): Leave"));
}

std::pair<cbProject*, ParserBase*> NativeParser::GetParserInfoByCurrentEditor()
{
    std::pair<cbProject*, ParserBase*> info(nullptr, nullptr);
    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();

    if ( editor ) //No need to check editor->GetFilename, because a built-in editor always have a filename
    {
        info.first  = GetProjectByEditor(editor);
        info.second = GetParserByProject(info.first);
    }

    return info;
}

#ifdef __WXMSW__
void NativeParser::SetTokenKindImage(int kind, const wxBitmap& bitmap, const wxBitmap& mask)
{
    if (kind < PARSER_IMG_MIN || kind > PARSER_IMG_MAX)
        return;
    m_ImageList->Replace(kind, bitmap, mask);
}
#else
void NativeParser::SetTokenKindImage(int WXUNUSED(kind), const wxBitmap& WXUNUSED(bitmap), const wxBitmap& WXUNUSED(mask))
{
    return;
}
#endif

void NativeParser::SetTokenKindImage(int kind, const wxBitmap& bitmap, cb_unused const wxColour& maskColour)
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
    size_t found_at = FindCurrentFunctionToken(searchData, proc_result, pos);

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
        for (TokenIdxSet::const_iterator tis_it = scope_result.begin(); tis_it != scope_result.end(); ++tis_it)
            search_scope->insert(*tis_it);
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

    for (TokenIdxSet::const_iterator tis_it = scope_result.begin(); tis_it != scope_result.end(); ++tis_it)
    {
        GenerateResultSet(m_Parser->GetTokenTree(), procName, *tis_it, result,
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

    int pos = caretPos == -1 ? searchData->control->GetCurrentPos() : caretPos;
    if (pos < 0 || pos > searchData->control->GetLength())
        return false;

    // Get the buffer from begin of the editor to the current caret position
    wxString buffer = searchData->control->GetTextRange(0, pos);

    return ParseBufferForUsingNamespace(buffer, search_scope);
}

bool NativeParser::ParseBufferForUsingNamespace(const wxString& buffer, TokenIdxSet& search_scope, bool bufferSkipBlocks)
{
    wxArrayString ns;
    m_Parser->ParseBufferForUsingNamespace(buffer, ns, bufferSkipBlocks);

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
    for (TokenIdxSet::const_iterator tis_it = proc_result.begin(); tis_it != proc_result.end(); ++tis_it)
    {
        wxString buffer;
        int initLine = -1;
        int tokenIdx = -1;

        if (locked)
            CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)
        locked = true;

        const Token* token = tree->at(*tis_it);

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

bool NativeParser::ParseLocalBlock(ccSearchData* searchData, TokenIdxSet& search_scope, int caretPos)
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
        cbStyledTextCtrl* stc = searchData->control;
        // if we are in a function body, then blockStart points to the '{', so we just skip the '{'.
        if (stc->GetCharAt(blockStart) == wxT('{'))
            ++blockStart;
        const int pos         = (caretPos == -1 ? stc->GetCurrentPos() : caretPos);
        const int line        = stc->LineFromPosition(pos);
        const int blockEnd    = stc->GetLineEndPosition(line);
        if (blockEnd < 0 || blockEnd > stc->GetLength())
        {
            if (s_DebugSmartSense)
            {
                CCLogger::Get()->DebugLog(F(_T("ParseLocalBlock() ERROR blockEnd=%d and edLength=%d?!"),
                                            blockEnd, stc->GetLength()));
            }
            return false;
        }

        if (blockStart >= blockEnd)
            blockStart = blockEnd;

//        wxString buffer = searchData->control->GetTextRange(blockStart, blockEnd);
        wxString buffer;
        // condense out-of-scope braces {...}
        int scanPos = blockEnd;
        for (int curPos = pos; curPos > blockStart; --curPos)
        {
            if (stc->GetCharAt(curPos) != wxT('}'))
                continue;
            const int style = stc->GetStyleAt(curPos);
            if (   stc->IsString(style)
                || stc->IsCharacter(style)
                || stc->IsComment(style))
            {
                continue;
            }
            const int scopeStart = stc->BraceMatch(curPos);
            if (scopeStart < blockStart)
                break;
            buffer.Prepend(stc->GetTextRange(curPos, scanPos));
            int startLn = stc->LineFromPosition(scopeStart);
            int endLn   = stc->LineFromPosition(curPos);
            if (startLn < endLn) // maintain correct line numbers for parsed tokens
                buffer.Prepend( wxString(wxT('\n'), endLn - startLn) );
            scanPos = scopeStart + 1;
            curPos  = scopeStart;

            // condense out-of-scope for/if/while declarations
            int prevCharIdx = scopeStart - 1;
            for (; prevCharIdx > blockStart; --prevCharIdx)
            {
                if (stc->IsComment(stc->GetStyleAt(prevCharIdx)))
                    continue;
                if (!wxIsspace(stc->GetCharAt(prevCharIdx)))
                    break;
            }
            if (stc->GetCharAt(prevCharIdx) != wxT(')'))
                continue;
            const int paramStart = stc->BraceMatch(prevCharIdx);
            if (paramStart < blockStart)
                continue;
            for (prevCharIdx = paramStart - 1; prevCharIdx > blockStart; --prevCharIdx)
            {
                if (stc->IsComment(stc->GetStyleAt(prevCharIdx)))
                    continue;
                if (!wxIsspace(stc->GetCharAt(prevCharIdx)))
                    break;
            }
            const wxString text = stc->GetTextRange(stc->WordStartPosition(prevCharIdx, true),
                                                    stc->WordEndPosition(  prevCharIdx, true));
            if (text == wxT("for"))
                buffer.Prepend(wxT("(;;){"));
            else if (text == wxT("if") || text == wxT("while"))
                buffer.Prepend(wxT("(0){"));
            else
                continue;
            startLn = stc->LineFromPosition(prevCharIdx);
            endLn   = stc->LineFromPosition(scopeStart);
            if (startLn < endLn)
                buffer.Prepend( wxString(wxT('\n'), endLn - startLn) );
            curPos  = stc->WordStartPosition(prevCharIdx, true);
            scanPos = stc->WordEndPosition(  prevCharIdx, true);
        }
        buffer.Prepend(stc->GetTextRange(blockStart, scanPos));

        buffer.Trim();

        ParseBufferForUsingNamespace(buffer, search_scope, false);

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

    TRACE(_T("NativeParser::AddCompilerDirs(): Enter"));

    // If there is no project, work on default compiler
    if (!project)
    {
        Compiler* compiler = CompilerFactory::GetDefaultCompiler();
        if (compiler)
        {
            // these dirs were the user's compiler include search dirs
            AddIncludeDirsToParser(compiler->GetIncludeDirs(), wxEmptyString, parser);

            if (compiler->GetID().Contains(_T("gcc")))
                AddGCCCompilerDirs(compiler->GetMasterPath(), compiler->GetPrograms().CPP, parser);
        }
        TRACE(_T("NativeParser::AddCompilerDirs(): Leave"));
        return true;
    }

    // Otherwise (if there is a project), work on the project's compiler...
    wxString base = project->GetBasePath();
    parser->AddIncludeDir(base); // add project's base path
    TRACE(_T("NativeParser::AddCompilerDirs(): Adding project base dir to parser: ") + base);

    // ...so we can access post-processed project's search dirs
    Compiler* compiler = CompilerFactory::GetCompiler(project->GetCompilerID());
    cb::shared_ptr<CompilerCommandGenerator> generator(compiler ? compiler->GetCommandGenerator(project) : nullptr);
    if (compiler && generator)
        generator->Init(project);

    // get project include dirs
    AddIncludeDirsToParser(project->GetIncludeDirs(), base, parser);

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
            // post-processed search dirs (from build scripts)
            if (compiler && generator)
                AddIncludeDirsToParser(generator->GetCompilerSearchDirs(target), base, parser);

            // apply target vars
//            target->GetCustomVars().ApplyVarsToEnvironment();
            AddIncludeDirsToParser(target->GetIncludeDirs(), base, parser);

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
        compiler = Compilers[idxCompiler];
        if (!compiler) continue;

        AddIncludeDirsToParser(compiler->GetIncludeDirs(), base, parser);

        // find out which compiler, if gnu, do the special trick
        // to find it's internal include paths
        // but do only once per C::B session, thus cache for later calls
        wxString CompilerID = compiler->GetID();
        if (CompilerID.Contains(_T("gcc")))
            AddGCCCompilerDirs(Compilers[idxCompiler]->GetMasterPath(), compiler->GetPrograms().CPP, parser);
    } // end of while loop over the found compilers

    if (!nCompilers)
        CCLogger::Get()->DebugLog(_T("NativeParser::AddCompilerDirs(): No compilers found!"));

    delete [] Compilers;
    TRACE(_T("NativeParser::AddCompilerDirs(): Leave"));
    return true;
}

bool NativeParser::AddCompilerPredefinedMacros(cbProject* project, ParserBase* parser)
{
    if (!parser)
        return false;

    if (!parser->Options().wantPreprocessor)
        return false;

    TRACE(_T("NativeParser::AddCompilerPredefinedMacros(): Enter"));

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

    TRACE(_T("NativeParser::AddCompilerPredefinedMacros(): Add compiler predefined preprocessor macros:\n%s"), defs.wx_str());
    parser->AddPredefinedMacros(defs);

    TRACE(_T("NativeParser::AddCompilerPredefinedMacros(): Leave"));
    if ( defs.IsEmpty() )
        return false;

    return true;
}

bool NativeParser::AddCompilerPredefinedMacrosGCC(const wxString& compilerId, cbProject* project, wxString& defs)
{
    Compiler* compiler = CompilerFactory::GetCompiler(compilerId);
    if (!compiler)
        return false;

    wxString masterPath = compiler->GetMasterPath();
    Manager::Get()->GetMacrosManager()->ReplaceMacros(masterPath);
    const wxString cpp_compiler = masterPath + _T("\\bin\\") + compiler->GetPrograms().CPP;
    if ( !wxFileName::FileExists(cpp_compiler) )
        return false;

    static std::map<wxString, wxString> gccDefsMap;
    if (gccDefsMap[cpp_compiler].IsEmpty())
    {
        static bool reentry = false;
        if (reentry)
            return false;

        // Check if user set language standard version to use
        wxString standard = GetCompilerStandardGCC(compiler, project);

#ifdef __WXMSW__
        const wxString args(wxString::Format(_T(" -E -dM -x c++ %s nul"), standard.wx_str()) );
#else
        const wxString args(wxString::Format(_T(" -E -dM -x c++ %s /dev/null"), standard.wx_str()) );
#endif

        wxArrayString output;
        reentry = true;
        if ( wxExecute(cpp_compiler + args, output, wxEXEC_SYNC | wxEXEC_NODISABLE) == -1 )
        {
            TRACE(_T("AddCompilerPredefinedMacrosGCC::wxExecute failed!"));
            reentry = false;
            return false;
        }
        reentry = false;

        // wxExecute can be a long action and C::B might have been shutdown in the meantime...
        if ( Manager::IsAppShuttingDown() )
            return false;

        wxString& gccDefs = gccDefsMap[cpp_compiler];
        for (size_t i = 0; i < output.Count(); ++i)
            gccDefs += output[i] + _T("\n");

        CCLogger::Get()->DebugLog(_T("NativeParser::AddCompilerPredefinedMacrosGCC(): Caching predefined macros for compiler '")
                                  + cpp_compiler + _T("':\n") + gccDefs);
    }

    defs = gccDefsMap[cpp_compiler];

    return true;
}

wxString NativeParser::GetCompilerStandardGCC(Compiler* compiler, cbProject* project)
{
    // Check if user set language standard version to use
    // 1.) Global compiler settings are first to search in
    wxString standard = GetCompilerUsingStandardGCC(compiler->GetCompilerOptions());
    if (standard.IsEmpty() && project)
    {
        // 2.) Project compiler setting are second
        standard = GetCompilerUsingStandardGCC(project->GetCompilerOptions());

        // 3.) And targets are third in row to look for standard
        // NOTE: If two targets use different standards, only the one we
        //       encounter first (eg. c++98) will be used, and any other
        //       disregarded (even if it would be c++1y)
        if (standard.IsEmpty())
        {
            for (int i=0; i<project->GetBuildTargetsCount(); ++i)
            {
                ProjectBuildTarget* target = project->GetBuildTarget(i);
                standard = GetCompilerUsingStandardGCC(target->GetCompilerOptions());

                if (!standard.IsEmpty())
                    break;
            }
        }
    }
    return standard;
}

wxString NativeParser::GetCompilerUsingStandardGCC(const wxArrayString& compilerOptions)
{
    wxString standard;
    for (wxArrayString::size_type i=0; i<compilerOptions.Count(); ++i)
    {
        if (compilerOptions[i].StartsWith(_T("-std=")))
        {
            standard = compilerOptions[i];
            CCLogger::Get()->DebugLog(wxString::Format(_T("NativeParser::GetCompilerUsingStandardGCC(): Using language standard: %s"), standard.wx_str()));
            break;
        }
    }
    return standard;
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
        const wxString c_compiler = masterPath + _T("\\bin\\") + compiler->GetPrograms().C;
        if ( !wxFileName::FileExists(c_compiler) )
            return false;

        static bool reentry = false;
        if (reentry)
            return false;

        wxArrayString output, error;
        reentry = true;
        // Just run the compiler which shows e.g.:
        // "Microsoft (R) C/C++ Optimizing Compiler Version 12.00.8804, for x86"
        // ...and extract platform information (32/64 bit) and compiler version (12) out of it
        if ( wxExecute(c_compiler, output, error, wxEXEC_SYNC | wxEXEC_NODISABLE) == -1 )
        {
            TRACE(_T("AddCompilerPredefinedMacrosVC::wxExecute failed!"));
            reentry = false;
            return false;
        }
        reentry = false;

        // wxExecute can be a long action and C::B might have been shutdown in the meantime...
        if ( Manager::IsAppShuttingDown() )
            return false;

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
            wxString ver = str.Mid(pos + tmp.Length(), 4); // is i.e. 12.0
            pos = ver.Find(_T('.'));
            if (pos != wxNOT_FOUND)
            {
                // out of "12.0" make "1200" for the #define
                ver[pos]     = ver[pos + 1]; // move the mintor version first number to the dot position
                ver[pos + 1] = _T('0');      // add another zero at the end
                defs += _T("#define _MSC_VER ") + ver;
                // Known to now:
                // MSVC++ 11.0 _MSC_VER = 1700 (Visual Studio 2012)
                // MSVC++ 10.0 _MSC_VER = 1600 (Visual Studio 2010)
                // MSVC++ 9.0  _MSC_VER = 1500 (Visual Studio 2008)
                // MSVC++ 8.0  _MSC_VER = 1400 (Visual Studio 2005)
                // MSVC++ 7.1  _MSC_VER = 1310 (Visual Studio 2003)
                // MSVC++ 7.0  _MSC_VER = 1300
                // MSVC++ 6.0  _MSC_VER = 1200
                // MSVC++ 5.0  _MSC_VER = 1100
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

    TRACE(_T("NativeParser::AddProjectDefinedMacros(): Enter"));

    wxString compilerId = project->GetCompilerID();
    wxString param;
    if (compilerId.Contains(_T("gcc")))
        param = _T("-D");
    else if (compilerId.StartsWith(_T("msvc")))
        param = _T("/D");

    if (param.IsEmpty())
        return false; // no compiler options, return false

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
    TRACE(_T("NativeParser::AddProjectDefinedMacros(): Leave"));
    if ( defs.IsEmpty() )
        return false;

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

    TRACE(_T("NativeParser::GetGCCCompilerDirs(): Enter"));

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
    if ( wxExecute(Command, Output, Errors, wxEXEC_SYNC | wxEXEC_NODISABLE) == -1 )
    {
        TRACE(_T("NativeParser::GetGCCCompilerDirs(): GetGCCCompilerDirs::wxExecute failed!"));
        flag = false;
        return dirs[cpp_compiler];
    }
    flag = false;

    // wxExecute can be a long action and C::B might have been shutdown in the meantime...
    // This is here, to protect a long run:
    if ( Manager::IsAppShuttingDown() )
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

        dirs[cpp_compiler].Add(fname.GetPath());

        CCLogger::Get()->DebugLog(_T("NativeParser::GetGCCCompilerDirs(): Caching GCC default include dir: ") + fname.GetPath());
    }

    TRACE(_T("NativeParser::GetGCCCompilerDirs(): Leave"));
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
    TRACE(_T("NativeParser::AddGCCCompilerDirs(): Adding %lu cached gcc dirs to parser..."), static_cast<unsigned long>(gccDirs.GetCount()));
    for (size_t i=0; i<gccDirs.GetCount(); ++i)
    {
        parser->AddIncludeDir(gccDirs[i]);
        TRACE(_T("NativeParser::AddGCCCompilerDirs(): Adding cached compiler dir to parser: ") + gccDirs[i]);
    }
}

void NativeParser::AddIncludeDirsToParser(const wxArrayString& dirs, const wxString& base, ParserBase* parser)
{
    for (unsigned int i = 0; i < dirs.GetCount(); ++i)
    {
        wxString dir = dirs[i];
        Manager::Get()->GetMacrosManager()->ReplaceMacros(dir);
        if ( !base.IsEmpty() )
        {
            wxFileName fn(dir);
            if ( NormalizePath(fn, base) )
            {
                parser->AddIncludeDir(fn.GetFullPath());
                TRACE(_T("NativeParser::AddIncludeDirsToParser(): Adding directory to parser: ") + fn.GetFullPath());
            }
            else
                CCLogger::Get()->DebugLog(F(_T("NativeParser::AddIncludeDirsToParser(): Error normalizing path: '%s' from '%s'"), dir.wx_str(), base.wx_str()));
        }
        else
            parser->AddIncludeDir(dir); // no base path, nothing to normalise
    }
}

void NativeParser::OnParserStart(wxCommandEvent& event)
{
    TRACE(_T("NativeParser::OnParserStart(): Enter"));

    cbProject* project = static_cast<cbProject*>(event.GetClientData());
    wxString   prj     = (project ? project->GetTitle() : _T("*NONE*"));
    const ParserCommon::ParserState state = static_cast<ParserCommon::ParserState>(event.GetInt());

    switch (state)
    {
        case ParserCommon::ptCreateParser:
            CCLogger::Get()->DebugLog(F(_("NativeParser::OnParserStart(): Starting batch parsing for project '%s'..."), prj.wx_str()));
            {
                std::pair<cbProject*, ParserBase*> info = GetParserInfoByCurrentEditor();
                if (info.second && m_Parser != info.second)
                {
                    CCLogger::Get()->DebugLog(_T("NativeParser::OnParserStart(): Start switch from OnParserStart::ptCreateParser"));
                    SwitchParser(info.first, info.second); // Calls SetParser() which also calls UpdateClassBrowserView()
                }
            }
            break;

        case ParserCommon::ptAddFileToParser:
            CCLogger::Get()->DebugLog(F(_("NativeParser::OnParserStart(): Starting add file parsing for project '%s'..."), prj.wx_str()));
            break;

        case ParserCommon::ptReparseFile:
            CCLogger::Get()->DebugLog(F(_("NativeParser::OnParserStart(): Starting re-parsing for project '%s'..."), prj.wx_str()));
            break;

        case ParserCommon::ptUndefined:
            if (event.GetString().IsEmpty())
                CCLogger::Get()->DebugLog(F(_("NativeParser::OnParserStart(): Batch parsing error in project '%s'"), prj.wx_str()));
            else
                CCLogger::Get()->DebugLog(F(_("NativeParser::OnParserStart(): %s in project '%s'"), event.GetString().wx_str(), prj.wx_str()));
            return;

        default:
            break;
    }

    event.Skip();

    TRACE(_T("NativeParser::OnParserStart(): Leave"));
}

void NativeParser::OnParserEnd(wxCommandEvent& event)
{
    TRACE(_T("NativeParser::OnParserEnd(): Enter"));

    ParserBase* parser = reinterpret_cast<ParserBase*>(event.GetEventObject());
    cbProject* project = static_cast<cbProject*>(event.GetClientData());
    wxString prj = (project ? project->GetTitle() : _T("*NONE*"));
    const ParserCommon::ParserState state = static_cast<ParserCommon::ParserState>(event.GetInt());

    switch (state)
    {
        case ParserCommon::ptCreateParser:
            {
                wxString log(F(_("NativeParser::OnParserEnd(): Project '%s' parsing stage done!"), prj.wx_str()));
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
                    CCLogger::Get()->DebugLog(_T("NativeParser::OnParserEnd(): Start switch from OnParserEnd::ptReparseFile"));
                    SwitchParser(info.first, info.second); // Calls SetParser() which also calls UpdateClassBrowserView()
                }
            }
            break;

        case ParserCommon::ptUndefined:
            CCLogger::Get()->DebugLog(F(_T("NativeParser::OnParserEnd(): Parser event handling error of project '%s'"), prj.wx_str()));
            return;

        default:
            break;
    }

    if (!event.GetString().IsEmpty())
        CCLogger::Get()->DebugLog(event.GetString());

    UpdateClassBrowser();

    // In this case, the parser will record all the cbprojects' token, so this will start parsing
    // the next cbproject.
    TRACE(_T("NativeParser::OnParserEnd(): Starting m_TimerParsingOneByOne."));
    m_TimerParsingOneByOne.Start(500, wxTIMER_ONE_SHOT);

    // both NativeParser and CodeCompletion class need to handle this event
    event.Skip();
    TRACE(_T("NativeParser::OnParserEnd(): Leave"));
}

void NativeParser::OnParsingOneByOneTimer(cb_unused wxTimerEvent& event)
{
    TRACE(_T("NativeParser::OnParsingOneByOneTimer(): Enter"));

    std::pair<cbProject*, ParserBase*> info = GetParserInfoByCurrentEditor();
    if (m_ParserPerWorkspace)
    {
        // If there is no parser and an active editor file can be obtained, parse the file according the active project
        if (!info.second && Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor())
        {
            // NOTE (Morten#1#): Shouldn't this actually be a temp parser??? I think this screws things with re-opening files on load of a projects...
            AddProjectToParser(info.first);
            CCLogger::Get()->DebugLog(_T("NativeParser::OnParsingOneByOneTimer(): Add foreign active editor to current active project's parser."));
        }
        // Otherwise, there is a parser already present
        else
        {
            // First: try to parse the active project (if any)
            cbProject* activeProject = Manager::Get()->GetProjectManager()->GetActiveProject();
            if (m_ParsedProjects.find(activeProject) == m_ParsedProjects.end())
            {
                AddProjectToParser(activeProject);
                CCLogger::Get()->DebugLog(_T("NativeParser::OnParsingOneByOneTimer(): Add new (un-parsed) active project to parser."));
            }
            // Else: add remaining projects one-by-one (if any)
            else
            {
                ProjectsArray* projs = Manager::Get()->GetProjectManager()->GetProjects();
                // loop on the whole workspace, and only add a new project to the parser
                // here the "new" means a project haven't been parsed. Once it was parsed, it is
                // added to the m_ParsedProjects
                for (size_t i = 0; i < projs->GetCount(); ++i)
                {
                    // Only add, if the project is not already parsed
                    if (m_ParsedProjects.find(projs->Item(i)) == m_ParsedProjects.end())
                    {
                        // AddProjectToParser return true means there are something need to parse, otherwise, it is false
                        if (!AddProjectToParser(projs->Item(i)))
                        {
                            CCLogger::Get()->Log(_T("NativeParser::OnParsingOneByOneTimer(): nothing need to parse in this project, try next project."));
                            continue;
                        }

                        CCLogger::Get()->DebugLog(_T("NativeParser::OnParsingOneByOneTimer(): Add additional (next) project to parser."));
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
            CCLogger::Get()->DebugLog(_T("NativeParser::OnParsingOneByOneTimer(): Start switch from OnParsingOneByOneTimer"));
            SwitchParser(info.first, info.second); // Calls SetParser() which also calls UpdateClassBrowserView()
        }
    }
    TRACE(_T("NativeParser::OnParsingOneByOneTimer(): Leave"));
}

void NativeParser::OnEditorActivated(EditorBase* editor)
{
    cbEditor* curEditor = Manager::Get()->GetEditorManager()->GetBuiltinEditor(editor);
    if (!curEditor)
        return;

    const wxString& activatedFile = editor->GetFilename();
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
    // the caller of the function should guarantee its a built-in editor
    wxString filename = editor->GetFilename();
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

    Reset();
}

bool NativeParser::AddProjectToParser(cbProject* project)
{
    wxString prj = (project ? project->GetTitle() : _T("*NONE*"));
    ParserBase* parser = GetParserByProject(project);
    if (parser)
        return false;

    if (m_ParsedProjects.empty())
        return false;

    m_ParsedProjects.insert(project);
    parser = GetParserByProject(project);
    if (!parser)
        return false;
    else if (!parser->UpdateParsingProject(project))
    {
        m_ParsedProjects.erase(project);
        return false;
    }

    // TODO (ollydbg#1#) did exactly the same thing as the function NativeParser::DoFullParsing()?
    wxString log(F(_("NativeParser::AddProjectToParser(): Add project (%s) to parser"), prj.wx_str()));
    CCLogger::Get()->Log(log);
    CCLogger::Get()->DebugLog(log);

    bool needParseMacros = false;

    if (!AddCompilerDirs(project, parser))
        CCLogger::Get()->DebugLog(_T("NativeParser::AddProjectToParser(): AddCompilerDirs failed!"));

    if (!AddCompilerPredefinedMacros(project, parser))
        CCLogger::Get()->DebugLog(_T("NativeParser::AddProjectToParser(): AddCompilerPredefinedMacros failed!"));
    else
        needParseMacros = true;

    if (!AddProjectDefinedMacros(project, parser))
        CCLogger::Get()->DebugLog(_T("NativeParser::AddProjectToParser(): AddProjectDefinedMacros failed!"));
    else
    {
        if(!needParseMacros)
            needParseMacros = true;
    }

    if (project)
    {
        size_t fileCount = 0;
        for (FilesList::const_iterator fl_it = project->GetFilesList().begin(); fl_it != project->GetFilesList().end(); ++fl_it)
        {
            ProjectFile* pf = *fl_it;
            if (pf && FileTypeOf(pf->relativeFilename) == ftHeader)
            {
                if ( AddFileToParser(project, pf->file.GetFullPath(), parser) )
                    ++fileCount;
            }
        }
        for (FilesList::const_iterator fl_it = project->GetFilesList().begin(); fl_it != project->GetFilesList().end(); ++fl_it)
        {
            ProjectFile* pf = *fl_it;
            if (pf && FileTypeOf(pf->relativeFilename) == ftSource)
            {
                if ( AddFileToParser(project, pf->file.GetFullPath(), parser) )
                    fileCount++;
            }
        }

        CCLogger::Get()->DebugLog(F(_("NativeParser::AddProjectToParser(): Done adding %lu files of project (%s) to parser."), static_cast<unsigned long>(fileCount), prj.wx_str()));

        // in some cases, all the files were already be parsed, so fileCount is still 0
        return ((fileCount>0) || needParseMacros);
    }
    else
    {
        EditorBase* editor = Manager::Get()->GetEditorManager()->GetActiveEditor();
        if (editor && AddFileToParser(project, editor->GetFilename(), parser))
        {
            wxFileName file(editor->GetFilename());
            parser->AddIncludeDir(file.GetPath());
            m_StandaloneFiles.Add(editor->GetFilename());

            CCLogger::Get()->DebugLog(F(_("NativeParser::AddProjectToParser(): Done adding stand-alone file (%s) of editor to parser."), editor->GetFilename().wx_str()));
            return true;
        }
    }
    return false;
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

    for (FilesList::const_iterator fl_it = project->GetFilesList().begin(); fl_it != project->GetFilesList().end(); ++fl_it)
    {
        ProjectFile* pf = *fl_it;
        if (pf && ParserCommon::FileType(pf->relativeFilename) != ParserCommon::ftOther)
            RemoveFileFromParser(project, pf->file.GetFullPath());
    }

    return true;
}
