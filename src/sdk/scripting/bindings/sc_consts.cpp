/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk_precomp.h>
#ifndef CB_PRECOMP

    #include <wx/string.h>
    #include <wx/filedlg.h>

    #include <compileoptionsbase.h>
    #include <cbproject.h>
    #include <cbplugin.h>
    #include <compileoptionsbase.h>
    #include <configmanager.h>
    #include <globals.h>
    #include <printing_types.h>
    #include <settings.h>
    #include <scriptingmanager.h>
#endif

#include <filefilters.h>
#include <scripting/sqrat.h>
#include "scripting/bindings/sc_binding_util.h"
#include "scripting/bindings/sc_plugin.h"



namespace ScriptBindings
{
    //wxString s_PathSep = wxFILE_SEP_PATH;


    void Register_Constants(HSQUIRRELVM vm)
    {
        // platform constants
        BIND_INT_CONSTANT_NAMED(0,  "PLATFORM_MSW");
        BIND_INT_CONSTANT_NAMED(1,  "PLATFORM_GTK");
        BIND_INT_CONSTANT_NAMED(2,  "PLATFORM_MAC");
        BIND_INT_CONSTANT_NAMED(3,  "PLATFORM_OS2");
        BIND_INT_CONSTANT_NAMED(4,  "PLATFORM_X11");
        BIND_INT_CONSTANT_NAMED(99, "PLATFORM_UNKNOWN");

        #if   defined(__WXMSW__)
            BIND_INT_CONSTANT_NAMED(0, "PLATFORM");
        #elif defined(__WXGTK__)
            BIND_INT_CONSTANT_NAMED(1, "PLATFORM");
        #elif defined(__WXMAC__)
            BIND_INT_CONSTANT_NAMED(2, "PLATFORM");
        #elif defined(__WXOS2__)
            BIND_INT_CONSTANT_NAMED(3, "PLATFORM");
        #elif defined(__WXX11__)
            BIND_INT_CONSTANT_NAMED(4, "PLATFORM");
        #else
            BIND_INT_CONSTANT_NAMED(99, "PLATFORM");
        #endif

        BIND_INT_CONSTANT_NAMED(PLUGIN_SDK_VERSION_MAJOR, "PLUGIN_SDK_VERSION_MAJOR");
        BIND_INT_CONSTANT_NAMED(PLUGIN_SDK_VERSION_MINOR, "PLUGIN_SDK_VERSION_MINOR");
        BIND_INT_CONSTANT_NAMED(PLUGIN_SDK_VERSION_RELEASE, "PLUGIN_SDK_VERSION_RELEASE");

        BIND_INT_CONSTANT_NAMED(SCRIPT_BINDING_VERSION_MAJOR,"SCRIPT_BINDING_VERSION_MAJOR");
        BIND_INT_CONSTANT_NAMED(SCRIPT_BINDING_VERSION_MINOR,"SCRIPT_BINDING_VERSION_MINOR");
        BIND_INT_CONSTANT_NAMED(SCRIPT_BINDING_VERSION_RELEASE,"SCRIPT_BINDING_VERSION_RELEASE");


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
        BIND_INT_CONSTANT(mcAskRebuildNeeded);
        BIND_INT_CONSTANT(mcSilentBuild);

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
        BIND_INT_CONSTANT(mtLogManager);
        BIND_INT_CONSTANT(mtOpenFilesList);
        BIND_INT_CONSTANT(mtEditorTab);
        BIND_INT_CONSTANT(mtUnknown);

        // File Type
        BIND_INT_CONSTANT(ftCodeBlocksProject);
        BIND_INT_CONSTANT(ftCodeBlocksWorkspace);
        BIND_INT_CONSTANT(ftDevCppProject);
        BIND_INT_CONSTANT(ftMSVC6Project);
        BIND_INT_CONSTANT(ftMSVC7Project);
        BIND_INT_CONSTANT(ftMSVC10Project);
        BIND_INT_CONSTANT(ftMSVC6Workspace);
        BIND_INT_CONSTANT( ftMSVC7Workspace);
        BIND_INT_CONSTANT(ftXcode1Project);
        BIND_INT_CONSTANT(ftXcode2Project);
        BIND_INT_CONSTANT(ftSource);
        BIND_INT_CONSTANT(ftHeader);
        BIND_INT_CONSTANT(ftObject);
        BIND_INT_CONSTANT(ftXRCResource);
        BIND_INT_CONSTANT(ftResource);
        BIND_INT_CONSTANT(ftResourceBin);
        BIND_INT_CONSTANT(ftStaticLib);
        BIND_INT_CONSTANT(ftDynamicLib);
        BIND_INT_CONSTANT(ftExecutable);
        BIND_INT_CONSTANT(ftNative);
        BIND_INT_CONSTANT(ftXMLDocument);
        BIND_INT_CONSTANT(ftScript);
        BIND_INT_CONSTANT(ftOther);

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
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::MSVC10_EXT, "EXT_MSVC10");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::ASM_EXT, "EXT_ASM");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::D_EXT, "EXT_D");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F_EXT, "EXT_F");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F77_EXT, "EXT_F77");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F90_EXT, "EXT_F90");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F95_EXT, "EXT_F95");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::FOR_EXT, "EXT_FOR");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::FPP_EXT, "EXT_FPP");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F03_EXT, "EXT_F03");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F08_EXT, "EXT_F08");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::JAVA_EXT, "EXT_JAVA");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::C_EXT, "EXT_C");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CC_EXT, "EXT_CC");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CPP_EXT, "EXT_CPP");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CXX_EXT, "EXT_CXX");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CPLPL_EXT, "EXT_CPLPL");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::INL_EXT, "EXT_INL");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::H_EXT, "EXT_H");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::HH_EXT, "EXT_HH");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::HPP_EXT, "EXT_HPP");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::HXX_EXT, "EXT_HXX");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::HPLPL_EXT, "EXT_HPLPL");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::S_EXT, "EXT_S");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::SS_EXT, "EXT_SS");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::S62_EXT, "EXT_S62");
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
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::MSVC10_DOT_EXT, "DOT_EXT_MSVC10");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::ASM_DOT_EXT, "DOT_EXT_ASM");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::D_DOT_EXT, "DOT_EXT_D");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F_DOT_EXT, "DOT_EXT_F");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F77_DOT_EXT, "DOT_EXT_F77");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F90_DOT_EXT, "DOT_EXT_F90");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F95_DOT_EXT, "DOT_EXT_F95");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::FOR_DOT_EXT, "DOT_EXT_FOR");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::FPP_DOT_EXT, "DOT_EXT_FPP");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F03_DOT_EXT, "DOT_EXT_F03");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::F08_DOT_EXT, "DOT_EXT_F08");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::JAVA_DOT_EXT, "DOT_EXT_JAVA");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::C_DOT_EXT, "DOT_EXT_C");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CC_DOT_EXT, "DOT_EXT_CC");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CPP_DOT_EXT, "DOT_EXT_CPP");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CXX_DOT_EXT, "DOT_EXT_CXX");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::CPLPL_DOT_EXT, "DOT_EXT_CPLPL");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::H_DOT_EXT, "DOT_EXT_H");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::HH_DOT_EXT, "DOT_EXT_HH");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::HPP_DOT_EXT, "DOT_EXT_HPP");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::HXX_DOT_EXT, "DOT_EXT_HXX");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::HPLPL_DOT_EXT, "DOT_EXT_HPLPL");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::S_DOT_EXT, "DOT_EXT_S");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::SS_DOT_EXT, "DOT_EXT_SS");
        BIND_WXSTR_CONSTANT_NAMED(FileFilters::S62_DOT_EXT, "DOT_EXT_S62");
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

        // cbEvents
        BIND_INT_CONSTANT(cbEVT_APP_STARTUP_DONE);
        BIND_INT_CONSTANT(cbEVT_APP_START_SHUTDOWN);
        BIND_INT_CONSTANT(cbEVT_APP_ACTIVATED);
        BIND_INT_CONSTANT(cbEVT_APP_DEACTIVATED);
        // plugin events
        BIND_INT_CONSTANT(cbEVT_PLUGIN_ATTACHED);
        BIND_INT_CONSTANT(cbEVT_PLUGIN_RELEASED);
        BIND_INT_CONSTANT(cbEVT_PLUGIN_INSTALLED);
        BIND_INT_CONSTANT(cbEVT_PLUGIN_UNINSTALLED);
        BIND_INT_CONSTANT(cbEVT_PLUGIN_LOADING_COMPLETE);
        // editor events
        BIND_INT_CONSTANT(cbEVT_EDITOR_CLOSE);
        BIND_INT_CONSTANT(cbEVT_EDITOR_OPEN);
        BIND_INT_CONSTANT(cbEVT_EDITOR_SWITCHED);
        BIND_INT_CONSTANT(cbEVT_EDITOR_ACTIVATED);
        BIND_INT_CONSTANT(cbEVT_EDITOR_DEACTIVATED);
        BIND_INT_CONSTANT(cbEVT_EDITOR_BEFORE_SAVE);
        BIND_INT_CONSTANT(cbEVT_EDITOR_SAVE);
        BIND_INT_CONSTANT(cbEVT_EDITOR_MODIFIED);
        BIND_INT_CONSTANT(cbEVT_EDITOR_TOOLTIP);
        BIND_INT_CONSTANT(cbEVT_EDITOR_TOOLTIP_CANCEL);
        BIND_INT_CONSTANT(cbEVT_EDITOR_UPDATE_UI);
        // project events
        BIND_INT_CONSTANT(cbEVT_PROJECT_NEW);
        BIND_INT_CONSTANT(cbEVT_PROJECT_CLOSE);
        BIND_INT_CONSTANT(cbEVT_PROJECT_OPEN);
        BIND_INT_CONSTANT(cbEVT_PROJECT_SAVE);
        BIND_INT_CONSTANT(cbEVT_PROJECT_ACTIVATE);
        BIND_INT_CONSTANT(cbEVT_PROJECT_BEGIN_ADD_FILES);
        BIND_INT_CONSTANT(cbEVT_PROJECT_END_ADD_FILES);
        BIND_INT_CONSTANT(cbEVT_PROJECT_BEGIN_REMOVE_FILES);
        BIND_INT_CONSTANT(cbEVT_PROJECT_END_REMOVE_FILES);
        BIND_INT_CONSTANT(cbEVT_PROJECT_FILE_ADDED);
        BIND_INT_CONSTANT(cbEVT_PROJECT_FILE_REMOVED);
        BIND_INT_CONSTANT(cbEVT_PROJECT_FILE_CHANGED);
        BIND_INT_CONSTANT(cbEVT_PROJECT_POPUP_MENU);
        BIND_INT_CONSTANT(cbEVT_PROJECT_TARGETS_MODIFIED);
        BIND_INT_CONSTANT(cbEVT_PROJECT_RENAMED);
        BIND_INT_CONSTANT(cbEVT_PROJECT_OPTIONS_CHANGED);
        BIND_INT_CONSTANT(cbEVT_WORKSPACE_CHANGED);
        BIND_INT_CONSTANT(cbEVT_WORKSPACE_LOADING_COMPLETE);
        // build targets events
        BIND_INT_CONSTANT(cbEVT_BUILDTARGET_ADDED);
        BIND_INT_CONSTANT(cbEVT_BUILDTARGET_REMOVED);
        BIND_INT_CONSTANT(cbEVT_BUILDTARGET_RENAMED);
        BIND_INT_CONSTANT(cbEVT_BUILDTARGET_SELECTED);
        // pipedprocess events
        BIND_INT_CONSTANT(cbEVT_PIPEDPROCESS_STDOUT);
        BIND_INT_CONSTANT(cbEVT_PIPEDPROCESS_STDERR);
        BIND_INT_CONSTANT(cbEVT_PIPEDPROCESS_TERMINATED);
        // thread-pool events
        BIND_INT_CONSTANT(cbEVT_THREADTASK_STARTED);
        BIND_INT_CONSTANT(cbEVT_THREADTASK_ENDED);
        BIND_INT_CONSTANT(cbEVT_THREADTASK_ALLDONE);
        // request app to dock/undock a window
        BIND_INT_CONSTANT(cbEVT_ADD_DOCK_WINDOW);
        BIND_INT_CONSTANT(cbEVT_REMOVE_DOCK_WINDOW);
        BIND_INT_CONSTANT(cbEVT_SHOW_DOCK_WINDOW);
        BIND_INT_CONSTANT(cbEVT_HIDE_DOCK_WINDOW);
        // force update current view layout
        BIND_INT_CONSTANT(cbEVT_UPDATE_VIEW_LAYOUT);
        // ask which is the current view layout
        BIND_INT_CONSTANT(cbEVT_QUERY_VIEW_LAYOUT);
        // request app to switch view layout
        BIND_INT_CONSTANT(cbEVT_SWITCH_VIEW_LAYOUT);
        // app notifies that a new layout has been applied
        BIND_INT_CONSTANT(cbEVT_SWITCHED_VIEW_LAYOUT);
        // app notifies that a docked window has been hidden/shown
        BIND_INT_CONSTANT(cbEVT_DOCK_WINDOW_VISIBILITY);
        // app notifies that the menubar is started being (re)created
        BIND_INT_CONSTANT(cbEVT_MENUBAR_CREATE_BEGIN);
        // app notifies that the menubar (re)creation ended
        BIND_INT_CONSTANT(cbEVT_MENUBAR_CREATE_END);
        // compiler-related events
        BIND_INT_CONSTANT(cbEVT_COMPILER_STARTED);
        BIND_INT_CONSTANT(cbEVT_COMPILER_FINISHED);
        BIND_INT_CONSTANT(cbEVT_COMPILER_SET_BUILD_OPTIONS);
        BIND_INT_CONSTANT(cbEVT_CLEAN_PROJECT_STARTED);
        BIND_INT_CONSTANT(cbEVT_CLEAN_WORKSPACE_STARTED);
        BIND_INT_CONSTANT(cbEVT_COMPILER_SETTINGS_CHANGED);
        // request app to compile a single file
        BIND_INT_CONSTANT(cbEVT_COMPILE_FILE_REQUEST);
        // debugger-related events
        BIND_INT_CONSTANT(cbEVT_DEBUGGER_STARTED);
        BIND_INT_CONSTANT(cbEVT_DEBUGGER_PAUSED);
        BIND_INT_CONSTANT(cbEVT_DEBUGGER_FINISHED);

        // logger-related events
        BIND_INT_CONSTANT(cbEVT_ADD_LOG_WINDOW);
        BIND_INT_CONSTANT(cbEVT_REMOVE_LOG_WINDOW);
        BIND_INT_CONSTANT(cbEVT_HIDE_LOG_WINDOW);
        BIND_INT_CONSTANT(cbEVT_SWITCH_TO_LOG_WINDOW);
        BIND_INT_CONSTANT(cbEVT_GET_ACTIVE_LOG_WINDOW);
        BIND_INT_CONSTANT(cbEVT_SHOW_LOG_MANAGER);
        BIND_INT_CONSTANT(cbEVT_HIDE_LOG_MANAGER);
        BIND_INT_CONSTANT(cbEVT_LOCK_LOG_MANAGER);
        BIND_INT_CONSTANT(cbEVT_UNLOCK_LOG_MANAGER);

        //cbAUiNotebook related events
        BIND_INT_CONSTANT(cbEVT_CBAUIBOOK_LEFT_DCLICK);

        // code-completion related events
        BIND_INT_CONSTANT(cbEVT_COMPLETE_CODE);
        BIND_INT_CONSTANT(cbEVT_SHOW_CALL_TIP);

        BIND_INT_CONSTANT(cbEVT_SETTINGS_CHANGED);
    }
};
