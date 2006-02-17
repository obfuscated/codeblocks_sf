#include <sdk_precomp.h>
#include <settings.h>
#include <compiletargetbase.h>
#include <cbproject.h>
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
}
