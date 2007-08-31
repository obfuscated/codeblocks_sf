#include <sdk_precomp.h>
#ifndef CB_PRECOMP
    #include <wx/string.h>
    #include <globals.h>
    #include <settings.h>
#endif

#include <filefilters.h>
#include "sc_base_types.h"

// helper macros to bind constants
#define BIND_INT_CONSTANT(a) SqPlus::BindConstant<SQInteger>(a, #a);
#define BIND_INT_CONSTANT_NAMED(a,n) SqPlus::BindConstant<SQInteger>(a, n);
#define BIND_WXSTR_CONSTANT_NAMED(a,n) BindVariable(const_cast<wxString*>(&a), n, SqPlus::VAR_ACCESS_CONSTANT);

namespace ScriptBindings
{
    wxString s_PathSep = wxFILE_SEP_PATH;

    void Register_Constants()
    {
        // platform constants
        BIND_INT_CONSTANT_NAMED(0, "PLATFORM_MSW");
        BIND_INT_CONSTANT_NAMED(1, "PLATFORM_GTK");
        BIND_INT_CONSTANT_NAMED(2, "PLATFORM_MAC");
        BIND_INT_CONSTANT_NAMED(3, "PLATFORM_OS2");
        BIND_INT_CONSTANT_NAMED(4, "PLATFORM_X11");
        BIND_INT_CONSTANT_NAMED(99, "PLATFORM_UNKNOWN");

        #ifdef __WXMSW__
            BIND_INT_CONSTANT_NAMED(0, "PLATFORM");
        #elif __WXGTK__
            BIND_INT_CONSTANT_NAMED(1, "PLATFORM");
        #elif __WXMAC__
            BIND_INT_CONSTANT_NAMED(2, "PLATFORM");
        #elif __WXOS2__
            BIND_INT_CONSTANT_NAMED(3, "PLATFORM");
        #elif __WXX11__
            BIND_INT_CONSTANT_NAMED(4, "PLATFORM");
        #else
            BIND_INT_CONSTANT_NAMED(99, "PLATFORM");
        #endif

        BIND_INT_CONSTANT_NAMED(PLUGIN_SDK_VERSION_MAJOR, "PLUGIN_SDK_VERSION_MAJOR");
        BIND_INT_CONSTANT_NAMED(PLUGIN_SDK_VERSION_MINOR, "PLUGIN_SDK_VERSION_MINOR");
        BIND_INT_CONSTANT_NAMED(PLUGIN_SDK_VERSION_RELEASE, "PLUGIN_SDK_VERSION_RELEASE");

        // path separator for filenames
        BIND_WXSTR_CONSTANT_NAMED(s_PathSep, "wxFILE_SEP_PATH");

        // dialog buttons
        BIND_INT_CONSTANT(wxOK);
        BIND_INT_CONSTANT(wxYES_NO);
        BIND_INT_CONSTANT(wxCANCEL);
        BIND_INT_CONSTANT(wxID_OK);
        BIND_INT_CONSTANT(wxID_YES);
        BIND_INT_CONSTANT(wxID_NO);
        BIND_INT_CONSTANT(wxID_CANCEL);
        BIND_INT_CONSTANT(wxICON_QUESTION);
        BIND_INT_CONSTANT(wxICON_INFORMATION);
        BIND_INT_CONSTANT(wxICON_WARNING);
        BIND_INT_CONSTANT(wxICON_ERROR);

        // wxPathFormat
        BIND_INT_CONSTANT(wxPATH_NATIVE);
        BIND_INT_CONSTANT(wxPATH_UNIX);
        BIND_INT_CONSTANT(wxPATH_BEOS);
        BIND_INT_CONSTANT(wxPATH_MAC);
        BIND_INT_CONSTANT(wxPATH_DOS);
        BIND_INT_CONSTANT(wxPATH_WIN);
        BIND_INT_CONSTANT(wxPATH_OS2);
        BIND_INT_CONSTANT(wxPATH_VMS);

        // for wxFileName::GetPath()
        BIND_INT_CONSTANT(wxPATH_GET_VOLUME);
        BIND_INT_CONSTANT(wxPATH_GET_SEPARATOR);

        // wxPathNormalize
        BIND_INT_CONSTANT(wxPATH_NORM_ENV_VARS);
        BIND_INT_CONSTANT(wxPATH_NORM_DOTS);
        BIND_INT_CONSTANT(wxPATH_NORM_TILDE);
        BIND_INT_CONSTANT(wxPATH_NORM_CASE);
        BIND_INT_CONSTANT(wxPATH_NORM_ABSOLUTE);
        BIND_INT_CONSTANT(wxPATH_NORM_LONG);
        BIND_INT_CONSTANT(wxPATH_NORM_SHORTCUT);
        BIND_INT_CONSTANT(wxPATH_NORM_ALL);

        // OptionsRelationType
        BIND_INT_CONSTANT(ortCompilerOptions);
        BIND_INT_CONSTANT(ortLinkerOptions);
        BIND_INT_CONSTANT(ortIncludeDirs);
        BIND_INT_CONSTANT(ortLibDirs);
        BIND_INT_CONSTANT(ortResDirs);

        // OptionsRelation
        BIND_INT_CONSTANT(orUseParentOptionsOnly);
        BIND_INT_CONSTANT(orUseTargetOptionsOnly);
        BIND_INT_CONSTANT(orPrependToParentOptions);
        BIND_INT_CONSTANT(orAppendToParentOptions);

        // TargetType
        BIND_INT_CONSTANT(ttExecutable);
        BIND_INT_CONSTANT(ttConsoleOnly);
        BIND_INT_CONSTANT(ttStaticLib);
        BIND_INT_CONSTANT(ttDynamicLib);
        BIND_INT_CONSTANT(ttCommandsOnly);
	BIND_INT_CONSTANT(ttNative);

        // MakeCommand
        BIND_INT_CONSTANT(mcClean);
        BIND_INT_CONSTANT(mcDistClean);
        BIND_INT_CONSTANT(mcBuild);
        BIND_INT_CONSTANT(mcCompileFile);

        // PCHMode
        BIND_INT_CONSTANT(pchSourceDir);
        BIND_INT_CONSTANT(pchObjectDir);
        BIND_INT_CONSTANT(pchSourceFile);

        // printing scope for print dialog
        BIND_INT_CONSTANT(psSelection);
        BIND_INT_CONSTANT(psActiveEditor);
        BIND_INT_CONSTANT(psAllOpenEditors);

        // printing colour mode
        BIND_INT_CONSTANT(pcmBlackAndWhite);
        BIND_INT_CONSTANT(pcmColourOnWhite);
        BIND_INT_CONSTANT(pcmInvertColours);
        BIND_INT_CONSTANT(pcmAsIs);

        // TemplateOutputType
        BIND_INT_CONSTANT_NAMED(totProject, "wizProject");
        BIND_INT_CONSTANT_NAMED(totTarget, "wizTarget");
        BIND_INT_CONSTANT_NAMED(totFiles, "wizFiles");
        BIND_INT_CONSTANT_NAMED(totCustom, "wizCustom");

        // SearchDirs
        BIND_INT_CONSTANT(sdHome);
        BIND_INT_CONSTANT(sdBase);
        BIND_INT_CONSTANT(sdTemp);
        BIND_INT_CONSTANT(sdPath);
        BIND_INT_CONSTANT(sdConfig);
        BIND_INT_CONSTANT(sdCurrent);
        BIND_INT_CONSTANT(sdPluginsUser);
        BIND_INT_CONSTANT(sdScriptsUser);
        BIND_INT_CONSTANT(sdDataUser);
        BIND_INT_CONSTANT(sdAllUser);
        BIND_INT_CONSTANT(sdPluginsGlobal);
        BIND_INT_CONSTANT(sdScriptsGlobal);
        BIND_INT_CONSTANT(sdDataGlobal);
        BIND_INT_CONSTANT(sdAllGlobal);
        BIND_INT_CONSTANT(sdAllKnown);

        // SupportedPlatforms
        BIND_INT_CONSTANT(spWindows);
        BIND_INT_CONSTANT(spUnix);
        BIND_INT_CONSTANT(spMac);
        BIND_INT_CONSTANT(spAll);

		// TargetFilenameGenerationPolicy
        BIND_INT_CONSTANT(tgfpPlatformDefault);
        BIND_INT_CONSTANT(tgfpNone);

        // ModuleType
        BIND_INT_CONSTANT(mtProjectManager);
        BIND_INT_CONSTANT(mtEditorManager);
        BIND_INT_CONSTANT(mtMessageManager);
        BIND_INT_CONSTANT(mtOpenFilesList);
        BIND_INT_CONSTANT(mtUnknown);

        // FileTreeDataKind
        BIND_INT_CONSTANT_NAMED(FileTreeData::ftdkUndefined, "ftdkUndefined");
        BIND_INT_CONSTANT_NAMED(FileTreeData::ftdkProject, "ftdkProject");
        BIND_INT_CONSTANT_NAMED(FileTreeData::ftdkFolder, "ftdkFolder");
        BIND_INT_CONSTANT_NAMED(FileTreeData::ftdkFile, "ftdkFile");
        BIND_INT_CONSTANT_NAMED(FileTreeData::ftdkVirtualGroup, "ftdkVirtualGroup");
        BIND_INT_CONSTANT_NAMED(FileTreeData::ftdkVirtualFolder, "ftdkVirtualFolder");

        // file extensions
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::WORKSPACE_EXT, "EXT_WORKSPACE");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CODEBLOCKS_EXT, "EXT_CODEBLOCKS");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::DEVCPP_EXT, "EXT_DEVCPP");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::MSVC6_EXT, "EXT_MSVC6");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::MSVC6_WORKSPACE_EXT, "EXT_MSVC6_WORKSPACE");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::MSVC7_EXT, "EXT_MSVC7");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::MSVC7_WORKSPACE_EXT, "EXT_MSVC7_WORKSPACE");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::D_EXT, "EXT_D");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F_EXT, "EXT_F");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F77_EXT, "EXT_F77");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F90_EXT, "EXT_F90");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F95_EXT, "EXT_F95");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::JAVA_EXT, "EXT_JAVA");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::C_EXT, "EXT_C");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CC_EXT, "EXT_CC");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CPP_EXT, "EXT_CPP");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CXX_EXT, "EXT_CXX");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::H_EXT, "EXT_H");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::HH_EXT, "EXT_HH");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::HPP_EXT, "EXT_HPP");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::HXX_EXT, "EXT_HXX");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::S_EXT, "EXT_S");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::SS_EXT, "EXT_SS");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::OBJECT_EXT, "EXT_OBJECT");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::XRCRESOURCE_EXT, "EXT_XRCRESOURCE");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::STATICLIB_EXT, "EXT_STATICLIB");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::DYNAMICLIB_EXT, "EXT_DYNAMICLIB");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::EXECUTABLE_EXT, "EXT_EXECUTABLE");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::EXECUTABLE_EXT, "EXT_NATIVE");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::RESOURCE_EXT, "EXT_RESOURCE");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::RESOURCEBIN_EXT, "EXT_RESOURCEBIN");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::XML_EXT, "EXT_XML");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::SCRIPT_EXT, "EXT_SCRIPT");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::WORKSPACE_DOT_EXT, "DOT_EXT_WORKSPACE");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CODEBLOCKS_DOT_EXT, "DOT_EXT_CODEBLOCKS");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::DEVCPP_DOT_EXT, "DOT_EXT_DEVCPP");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::MSVC6_DOT_EXT, "DOT_EXT_MSVC6");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::MSVC6_WORKSPACE_DOT_EXT, "DOT_EXT_MSVC6_WORKSPACE");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::MSVC7_DOT_EXT, "DOT_EXT_MSVC7");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::MSVC7_WORKSPACE_DOT_EXT, "DOT_EXT_MSVC7_WORKSPACE");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::D_DOT_EXT, "DOT_EXT_D");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F_DOT_EXT, "DOT_EXT_F");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F77_DOT_EXT, "DOT_EXT_F77");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F90_DOT_EXT, "DOT_EXT_F90");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F95_DOT_EXT, "DOT_EXT_F95");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::JAVA_DOT_EXT, "DOT_EXT_JAVA");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::C_DOT_EXT, "DOT_EXT_C");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CC_DOT_EXT, "DOT_EXT_CC");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CPP_DOT_EXT, "DOT_EXT_CPP");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CXX_DOT_EXT, "DOT_EXT_CXX");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::H_DOT_EXT, "DOT_EXT_H");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::HH_DOT_EXT, "DOT_EXT_HH");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::HPP_DOT_EXT, "DOT_EXT_HPP");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::HXX_DOT_EXT, "DOT_EXT_HXX");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::S_DOT_EXT, "DOT_EXT_S");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::SS_DOT_EXT, "DOT_EXT_SS");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::OBJECT_DOT_EXT, "DOT_EXT_OBJECT");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::XRCRESOURCE_DOT_EXT, "DOT_EXT_XRCRESOURCE");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::STATICLIB_DOT_EXT, "DOT_EXT_STATICLIB");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::DYNAMICLIB_DOT_EXT, "DOT_EXT_DYNAMICLIB");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::EXECUTABLE_DOT_EXT, "DOT_EXT_EXECUTABLE");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::NATIVE_DOT_EXT, "DOT_EXT_NATIVE");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::RESOURCE_DOT_EXT, "DOT_EXT_RESOURCE");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::RESOURCEBIN_DOT_EXT, "DOT_EXT_RESOURCEBIN");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::XML_DOT_EXT, "DOT_EXT_XML");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::SCRIPT_DOT_EXT, "DOT_EXT_SCRIPT");
    }
};
