#include <sdk_precomp.h>
#include <settings.h>
#include <compiletargetbase.h>
#include <cbproject.h>
#include <filefilters.h>
#include "const_bindings.h"

// platform constants
static const int PLATFORM_MSW = 0;
static const int PLATFORM_GTK = 1;
static const int PLATFORM_UNKNOWN = 99;
#ifdef __WXMSW__
    static const int PLATFORM = PLATFORM_MSW;
#elif __WXGTK__
    static const int PLATFORM = PLATFORM_GTK;
#else
    static const int PLATFORM = PLATFORM_UNKNOWN;
#endif

// path separator for filenames
static const wxString SEP_PATH = wxFILE_SEP_PATH;

// dialog buttons
static const int scOK                   = wxOK;
static const int scYES_NO               = wxYES_NO;
static const int scCANCEL               = wxCANCEL;
static const int scID_OK                = wxID_OK;
static const int scID_YES               = wxID_YES;
static const int scID_NO                = wxID_NO;
static const int scID_CANCEL            = wxID_CANCEL;
static const int scICON_QUESTION        = wxICON_QUESTION;
static const int scICON_INFORMATION     = wxICON_INFORMATION;
static const int scICON_WARNING         = wxICON_WARNING;
static const int scICON_ERROR           = wxICON_ERROR;

// OptionsRelationType
static const int s_ortCompilerOptions = ortCompilerOptions;
static const int s_ortLinkerOptions = ortLinkerOptions;
static const int s_ortIncludeDirs = ortIncludeDirs;
static const int s_ortLibDirs = ortLibDirs;
static const int s_ortResDirs = ortResDirs;

// OptionsRelation
static const int s_orUseParentOptionsOnly = orUseParentOptionsOnly;
static const int s_orUseTargetOptionsOnly = orUseTargetOptionsOnly;
static const int s_orPrependToParentOptions = orPrependToParentOptions;
static const int s_orAppendToParentOptions = orAppendToParentOptions;

// TargetType
static const int s_ttExecutable = ttExecutable;
static const int s_ttConsoleOnly = ttConsoleOnly;
static const int s_ttStaticLib = ttStaticLib;
static const int s_ttDynamicLib = ttDynamicLib;
static const int s_ttCommandsOnly = ttCommandsOnly;

// MakeCommand
static const int s_mcClean = mcClean;
static const int s_mcDistClean = mcDistClean;
static const int s_mcBuild = mcBuild;
static const int s_mcCompileFile = mcCompileFile;

// PCHMode
static const int s_pchSourceDir = pchSourceDir;
static const int s_pchObjectDir = pchObjectDir;
static const int s_pchSourceFile = pchSourceFile;

void RegisterConstBindings(asIScriptEngine* engine)
{
    // platform related
    engine->RegisterGlobalProperty("const int PLATFORM", (void*)&PLATFORM);
    engine->RegisterGlobalProperty("const int PLATFORM_MSW", (void*)&PLATFORM_MSW);
    engine->RegisterGlobalProperty("const int PLATFORM_GTK", (void*)&PLATFORM_GTK);
    engine->RegisterGlobalProperty("const int PLATFORM_UNKNOWN", (void*)&PLATFORM_UNKNOWN);

    // dialog buttons
    engine->RegisterGlobalProperty("const int wxOK", (void*)&scOK);
    engine->RegisterGlobalProperty("const int wxYES_NO", (void*)&scYES_NO);
    engine->RegisterGlobalProperty("const int wxCANCEL", (void*)&scCANCEL);
    engine->RegisterGlobalProperty("const int wxID_OK", (void*)&scID_OK);
    engine->RegisterGlobalProperty("const int wxID_YES", (void*)&scID_YES);
    engine->RegisterGlobalProperty("const int wxID_NO", (void*)&scID_NO);
    engine->RegisterGlobalProperty("const int wxID_CANCEL", (void*)&scID_CANCEL);
    engine->RegisterGlobalProperty("const int wxICON_QUESTION", (void*)&scICON_QUESTION);
    engine->RegisterGlobalProperty("const int wxICON_INFORMATION", (void*)&scICON_INFORMATION);
    engine->RegisterGlobalProperty("const int wxICON_WARNING", (void*)&scICON_WARNING);
    engine->RegisterGlobalProperty("const int wxICON_ERROR", (void*)&scICON_ERROR);

    // OptionsRelationType
    engine->RegisterGlobalProperty("const int ortCompilerOptions", (void*)&s_ortCompilerOptions);
    engine->RegisterGlobalProperty("const int ortLinkerOptions", (void*)&s_ortLinkerOptions);
    engine->RegisterGlobalProperty("const int ortIncludeDirs", (void*)&s_ortIncludeDirs);
    engine->RegisterGlobalProperty("const int ortLibDirs", (void*)&s_ortLibDirs);
    engine->RegisterGlobalProperty("const int ortResDirs", (void*)&s_ortResDirs);

    // OptionsRelation
    engine->RegisterGlobalProperty("const int orUseParentOptionsOnly", (void*)&s_orUseParentOptionsOnly);
    engine->RegisterGlobalProperty("const int orUseTargetOptionsOnly", (void*)&s_orUseTargetOptionsOnly);
    engine->RegisterGlobalProperty("const int orPrependToParentOptions", (void*)&s_orPrependToParentOptions);
    engine->RegisterGlobalProperty("const int orAppendToParentOptions", (void*)&s_orAppendToParentOptions);

    // TargetType
    engine->RegisterGlobalProperty("const int ttExecutable", (void*)&s_ttExecutable);
    engine->RegisterGlobalProperty("const int ttConsoleOnly", (void*)&s_ttConsoleOnly);
    engine->RegisterGlobalProperty("const int ttStaticLib", (void*)&s_ttStaticLib);
    engine->RegisterGlobalProperty("const int ttDynamicLib", (void*)&s_ttDynamicLib);
    engine->RegisterGlobalProperty("const int ttCommandsOnly", (void*)&s_ttCommandsOnly);

    // MakeCommand
    engine->RegisterGlobalProperty("const int mcClean", (void*)&s_mcClean);
    engine->RegisterGlobalProperty("const int mcDistClean", (void*)&s_mcDistClean);
    engine->RegisterGlobalProperty("const int mcBuild", (void*)&s_mcBuild);
    engine->RegisterGlobalProperty("const int mcCompileFile", (void*)&s_mcCompileFile);

    // PCHMode
    engine->RegisterGlobalProperty("const int pchSourceDir", (void*)&s_pchSourceDir);
    engine->RegisterGlobalProperty("const int pchObjectDir", (void*)&s_pchObjectDir);
    engine->RegisterGlobalProperty("const int pchSourceFile", (void*)&s_pchSourceFile);
    
    // path separator for filenames
    engine->RegisterGlobalProperty("const wxString SEP_PATH", (void*)&SEP_PATH);
    
    // file extensions
    engine->RegisterGlobalProperty("const wxString EXT_WORKSPACE", (void*)&FileFilters::WORKSPACE_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_CODEBLOCKS", (void*)&FileFilters::CODEBLOCKS_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_DEVCPP", (void*)&FileFilters::DEVCPP_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_MSVC6", (void*)&FileFilters::MSVC6_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_MSVC6_WORKSPACE", (void*)&FileFilters::MSVC6_WORKSPACE_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_MSVC7", (void*)&FileFilters::MSVC7_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_MSVC7_WORKSPACE", (void*)&FileFilters::MSVC7_WORKSPACE_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_D", (void*)&FileFilters::D_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_CPP", (void*)&FileFilters::CPP_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_C", (void*)&FileFilters::C_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_CC", (void*)&FileFilters::CC_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_CXX", (void*)&FileFilters::CXX_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_HPP", (void*)&FileFilters::HPP_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_H", (void*)&FileFilters::H_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_HH", (void*)&FileFilters::HH_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_HXX", (void*)&FileFilters::HXX_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_OBJECT", (void*)&FileFilters::OBJECT_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_XRCRESOURCE", (void*)&FileFilters::XRCRESOURCE_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_STATICLIB", (void*)&FileFilters::STATICLIB_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_DYNAMICLIB", (void*)&FileFilters::DYNAMICLIB_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_EXECUTABLE", (void*)&FileFilters::EXECUTABLE_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_RESOURCE", (void*)&FileFilters::RESOURCE_EXT);
    engine->RegisterGlobalProperty("const wxString EXT_RESOURCEBIN", (void*)&FileFilters::RESOURCEBIN_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_WORKSPACE", (void*)&FileFilters::WORKSPACE_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_CODEBLOCKS", (void*)&FileFilters::CODEBLOCKS_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_DEVCPP", (void*)&FileFilters::DEVCPP_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_MSVC6", (void*)&FileFilters::MSVC6_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_MSVC6_WORKSPACE", (void*)&FileFilters::MSVC6_WORKSPACE_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_MSVC7", (void*)&FileFilters::MSVC7_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_MSVC7_WORKSPACE", (void*)&FileFilters::MSVC7_WORKSPACE_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_D", (void*)&FileFilters::D_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_CPP", (void*)&FileFilters::CPP_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_C", (void*)&FileFilters::C_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_CC", (void*)&FileFilters::CC_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_CXX", (void*)&FileFilters::CXX_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_HPP", (void*)&FileFilters::HPP_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_H", (void*)&FileFilters::H_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_HH", (void*)&FileFilters::HH_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_HXX", (void*)&FileFilters::HXX_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_OBJECT", (void*)&FileFilters::OBJECT_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_XRCRESOURCE", (void*)&FileFilters::XRCRESOURCE_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_STATICLIB", (void*)&FileFilters::STATICLIB_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_DYNAMICLIB", (void*)&FileFilters::DYNAMICLIB_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_EXECUTABLE", (void*)&FileFilters::EXECUTABLE_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_RESOURCE", (void*)&FileFilters::RESOURCE_DOT_EXT);
    engine->RegisterGlobalProperty("const wxString DOT_EXT_RESOURCEBIN", (void*)&FileFilters::RESOURCEBIN_DOT_EXT);
}
