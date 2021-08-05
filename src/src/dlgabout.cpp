/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"

#ifndef CB_PRECOMP
    #ifdef __WXMAC__
        #include <wx/font.h>
    #endif //__WXMAC__
    #include <wx/button.h>    // wxImage
    #include <wx/image.h>    // wxImage
    #include <wx/intl.h>
    #include <wx/stattext.h>
    #include <wx/string.h>
    #include <wx/textctrl.h>
    #include <wx/xrc/xmlres.h>
    #if wxCHECK_VERSION(3, 0, 0)
        #include <wx/versioninfo.h>
    #endif // wxCHECK_VERSION

    #include "licenses.h"
    #include "configmanager.h"
    #include "wx/wxscintilla.h"
#endif

#include <wx/bitmap.h>
#include <wx/dcmemory.h>    // wxMemoryDC
#include <wx/display.h>
#include <wx/statbmp.h>
#include <wx/stdpaths.h>
#include <vector>

#include "appglobals.h"
#include "dlgabout.h" // class's header file
#include "configmanager.h"
#include "splashscreen.h"
#include "macrosmanager.h"
#include "cbproject.h"
#include "compilerfactory.h"
#include "uservarmanager.h"
#include "debuggermanager.h"
#include "cbplugin.h"

// class constructor

dlgAbout::dlgAbout(wxWindow* parent)
{
    if (!wxXmlResource::Get()->LoadObject(this, parent, _T("dlgAbout"), _T("wxScrollingDialog")))
    {
        cbMessageBox(_("There was an error loading the \"About\" dialog from XRC file."),
                     _("Information"), wxICON_EXCLAMATION);
        return;
    }

    wxButton *cancelButton = XRCCTRL(*this, "wxID_CANCEL", wxButton);
    cancelButton->SetDefault();
    cancelButton->SetFocus();

    const wxString description = _("Welcome to ") + appglobals::AppName + _T(" ") +
                                 appglobals::AppVersion + _T("!\n") + appglobals::AppName +
                                 _(" is a full-featured IDE (Integrated Development Environment) "
                                   "aiming to make the individual developer (and the development team) "
                                   "work in a nice programming environment offering everything he/they "
                                   "would ever need from a program of that kind.\n"
                                   "Its pluggable architecture allows you, the developer, to add "
                                   "any kind of functionality to the core program, through the use of "
                                   "plugins...\n");

    wxString file = ConfigManager::ReadDataPath() + _T("/images/splash_1312.png");

    wxImage im;
    im.LoadFile(file, wxBITMAP_TYPE_PNG);
    im.ConvertAlphaToMask();

    wxBitmap bmp(im);
    wxMemoryDC dc;
    dc.SelectObject(bmp);
    cbSplashScreen::DrawReleaseInfo(dc);

    wxStaticBitmap *bmpControl = XRCCTRL(*this, "lblTitle", wxStaticBitmap);
    bmpControl->SetSize(im.GetWidth(),im.GetHeight());
    bmpControl->SetBitmap(bmp);

    XRCCTRL(*this, "lblBuildTimestamp", wxStaticText)->SetLabel(wxString(_("Build: ")) + appglobals::AppBuildTimestamp);

    wxTextCtrl *txtDescription = XRCCTRL(*this, "txtDescription", wxTextCtrl);
    txtDescription->SetValue(description);

    wxTextCtrl *txtThanksTo = XRCCTRL(*this, "txtThanksTo", wxTextCtrl);
    // Note: Keep this is sync with the AUTHORS file in SVN.
    txtThanksTo->SetValue(_(
        "Developers:\n"
        "--------------\n"
        "Yiannis Mandravellos: Developer - Project leader\n"
        "Thomas Denk         : Developer\n"
        "Lieven de Cock      : Developer\n"
        "\"tiwag\"             : Developer\n"
        "Martin Halle        : Developer\n"
        "Biplab Modak        : Developer\n"
        "Jens Lody           : Developer\n"
        "Yuchen Deng         : Developer\n"
        "Teodor Petrov       : Developer\n"
        "Daniel Anselmi      : Developer\n"
        "Yuanhui Zhang       : Developer\n"
        "Damien Moore        : Developer\n"
        "Micah Ng            : Developer\n"
        "BlueHazzard         : Developer\n"
        "Ricardo Garcia      : All-hands person\n"
        "Paul A. Jimenez     : Help and AStyle plugins\n"
        "Thomas Lorblanches  : CodeStat and Profiler plugins\n"
        "Bartlomiej Swiecki  : wxSmith RAD plugin\n"
        "Jerome Antoine      : ThreadSearch plugin\n"
        "Pecan Heber         : Keybinder, BrowseTracker, DragScroll\n"
        "                      CodeSnippets plugins\n"
        "Arto Jonsson        : CodeSnippets plugin (passed on to Pecan)\n"
        "Darius Markauskas   : Fortran support\n"
        "Mario Cupelli       : Compiler support for embedded systems\n"
        "                      User's manual\n"
        "Jonas Zinn          : Misc. wxSmith AddOns and plugins\n"
        "Mirai Computing     : cbp2make tool\n"
        "Anders F Bjoerklund : wxMac compatibility\n"
        "\n"
        "Contributors (in no special order):\n"
        "-----------------------------------\n"
        "Daniel Orb          : RPM spec file and packages\n"
        "byo,elvstone, me22  : Conversion to Unicode\n"
        "pasgui              : Providing Ubuntu nightly packages\n"
        "Hakki Dogusan       : DigitalMars compiler support\n"
        "ybx                 : OpenWatcom compiler support\n"
        "Tim Baker           : Patches for the direct-compile-mode\n"
        "                      dependencies generation system\n"
        "David Perfors       : Unicode tester and future documentation writer\n"
        "Sylvain Prat        : Initial MSVC workspace and project importers\n"
        "Chris Raschko       : Design of the 3D logo for Code::Blocks\n"
        "J.A. Ortega         : 3D Icon based on the above\n"
        "Alexandr Efremo     : Providing OpenSuSe packages\n"
        "Huki                : Misc. Code-Completion improvements\n"
        "stahta01            : Misc. patches for several enhancements\n"
        "Miguel Gimenez      : Misc. patches for several enhancements\n"
        "\n"
        "All contributors that provided patches.\n"
        "The wxWidgets project (http://www.wxwidgets.org).\n"
        "wxScintilla (http://sourceforge.net/projects/wxscintilla).\n"
        "TinyXML parser (http://www.grinninglizard.com/tinyxml).\n"
        "Squirrel scripting language (http://www.squirrel-lang.org).\n"
        "The GNU Software Foundation (http://www.gnu.org).\n"
        "Last, but not least, the open-source community."));
    wxTextCtrl *txtLicense = XRCCTRL(*this, "txtLicense", wxTextCtrl);
    txtLicense->SetValue(LICENSE_GPL);

#if wxCHECK_VERSION(3, 0, 0)
    const wxVersionInfo scintillaVersion = wxScintilla::GetLibraryVersionInfo();
    const wxString scintillaStr = wxString::Format(wxT("%d.%d.%d"),
                                                   scintillaVersion.GetMajor(),
                                                   scintillaVersion.GetMinor(),
                                                   scintillaVersion.GetMicro());
#else
    const wxString scintillaStr = wxSCINTILLA_VERSION;
#endif // wxCHECK_VERSION

    struct Item
    {
        wxString name, value;
    };
    std::vector<Item> items;
    // ===========================================================================
    // Code::Blocks version and build info details
    items.push_back({_("Name"), appglobals::AppName});
    items.push_back({_("Version"), appglobals::AppActualVersion});
    items.push_back({_("App Version"), appglobals::AppVersion});
    items.push_back({_("Platform"), appglobals::AppPlatform});
    items.push_back({_("AppWXAnsiUnicode"), appglobals::AppWXAnsiUnicode});
    items.push_back({_("AppActualVersionVerb"), appglobals::AppActualVersionVerb});
    items.push_back({_("Build"), appglobals::AppBuildTimestamp});
    items.push_back({_("Code::Blocks Executable"), wxStandardPaths::Get().GetExecutablePath()});;
    items.push_back({wxEmptyString, wxEmptyString});

    items.push_back({_("SDK Version"), appglobals::AppSDKVersion});
    items.push_back({_("Scintilla Version"), scintillaStr});
    // ===========================================================================
    // wxwidget info
    wxVersionInfo wxVer = wxGetLibraryVersionInfo();
    items.push_back({_("wxWidget version"), wxVer.GetVersionString()});
    if (wxVer.HasDescription())
    {
        items.push_back({"wxWidget description", _("- - - - - - -")});
        wxArrayString aswxDescription = wxSplit(wxVer.GetDescription(),'\n');
        for (wxString &sInfo : aswxDescription)
        {
            sInfo.Replace("\n", "");
            if (!sInfo.IsEmpty())
                items.push_back({wxEmptyString, sInfo});
        }
        items.push_back({wxEmptyString, _("- - - - - - -")});
    }
    items.push_back({wxEmptyString, wxEmptyString});

    // ===========================================================================
    items.push_back({_("Author"), _("The Code::Blocks Team")});
    items.push_back({_("E-mail"), appglobals::AppContactEmail});
    items.push_back({_("Website"), appglobals::AppUrl});
    items.push_back({wxEmptyString, wxEmptyString});

    // ===========================================================================
    // Operating system info
    const wxPlatformInfo &platform = wxPlatformInfo::Get();
    const wxString desktopEnv = platform.GetDesktopEnvironment();

    items.push_back({_("Operating System"), platform.GetOperatingSystemDescription()});
    if (!desktopEnv.empty())
        items.push_back({_("Desktop environment"), desktopEnv });
    items.push_back({_("wxLocale Language"), wxLocale::GetLanguageName(wxLocale::GetSystemLanguage())});
    items.push_back({_("Code::Blocks config Folder"), ConfigManager::GetConfigFolder()});
    items.push_back({wxEmptyString, wxEmptyString});

    // ===========================================================================
    // Display info
    items.push_back({_("Display Info"), wxEmptyString});
    items.push_back({_("     Scaling factor"), wxString::Format("%f", GetContentScaleFactor())});
    items.push_back({_("     Detected scaling factor"), wxString::Format("%f", cbGetActualContentScaleFactor(*this))});
    const wxSize displayPPI = wxGetDisplayPPI();
    items.push_back({_("     Display PPI"), wxString::Format("%d x %d", displayPPI.x, displayPPI.y)});

    unsigned displays = wxDisplay::GetCount();
    items.push_back({_("     Display count"), wxString::Format("%u", displays)});

    for (unsigned ii = 0; ii < displays; ++ii)
    {
        wxDisplay display(ii);

        items.push_back({_("     Display"),wxString::Format(_("%u"), ii)});
        items.push_back({_("        Name"),display.GetName()});
        const wxRect geometry = display.GetGeometry();
        items.push_back({_("        XY"),wxString::Format(_("[%d , %d]"), geometry.GetLeft(), geometry.GetTop())});
        items.push_back({_("        Size"),wxString::Format(_("[%d , %d]"), geometry.GetWidth(), geometry.GetHeight())});
        items.push_back({_("        IsPrimary"),display.IsPrimary() ? _("True") : _("False")});
    }
    items.push_back({wxEmptyString, wxEmptyString});

    // ===========================================================================
    // Compiler Info
    items.push_back({_("Compiler Information"), wxEmptyString});

    ProjectManager *prjMan = Manager::Get()->GetProjectManager();
    cbProject *pProject = prjMan->GetActiveProject();

    Compiler *selectedCompiler = nullptr;
    if (pProject && prjMan->IsProjectStillOpen(pProject))
    {
        selectedCompiler = CompilerFactory::GetCompiler(pProject->GetCompilerID());
        items.push_back({_("       Project"), _("loaded")});
    }
    else
    {
        selectedCompiler = CompilerFactory::GetDefaultCompiler();
        items.push_back({_("       Project"), _("not loaded")});
    }
    items.push_back({_("       Default compiler"), CompilerFactory::GetDefaultCompilerID()});

    if (selectedCompiler)
    {
        items.push_back({_("       Name"), selectedCompiler->GetName()});
        items.push_back({_("       Version"), selectedCompiler->GetVersionString()});
        items.push_back({_("       Master Path"), selectedCompiler->GetMasterPath()});
        items.push_back({_("       IsValid"), selectedCompiler->IsValid()?_("True"):_("False")});
        const CompilerPrograms &cp = selectedCompiler->GetPrograms();
        items.push_back({_("       C Compiler"), cp.C});
        items.push_back({_("       C++ Compiler"), cp.CPP});
        items.push_back({_("       Dynamic Linker (LD)"), cp.LD});
        items.push_back({_("       Static Linker (LIB)"), cp.LIB});
        items.push_back({_("       Windows Resource Compiler"), cp.WINDRES});
        items.push_back({_("       Make"), cp.MAKE});
        items.push_back({_("       DBGconfig"), cp.DBGconfig});
        if (selectedCompiler->GetExtraPaths().GetCount() == 0)
            items.push_back({_("       Extra Paths"), _("None specified")});
        else
            items.push_back({_("       Extra Paths"), wxJoin(selectedCompiler->GetExtraPaths(), ',')});

        // ===========================================================================
        // GCC Compiler Info
        if (cp.C.IsSameAs("gcc") || cp.C.IsSameAs("gcc.exe"))
        {
            const wxString gcc(selectedCompiler->GetMasterPath()+wxFileName::GetPathSeparator()+wxT("bin")+wxFileName::GetPathSeparator()+cp.C);
            if (wxFileExists(gcc))
            {
                items.push_back({_("       GNU GCC"), _("details below")});

                wxString Command(gcc+" -v");
                if (platform::windows && platform::WindowsVersion() >= platform::winver_WindowsNT2000)
                {
                    Command = wxT("cmd /c ")+Command;
                }

                wxArrayString asOutput, asErrors;
                wxExecute(Command, asOutput, asErrors, wxEXEC_NODISABLE);
                for (wxString &sInfo : asOutput)
                {
                    sInfo.Replace("\n", "");
                    items.push_back({wxEmptyString, sInfo});
                }
                for (wxString &sInfo : asErrors)
                {
                    sInfo.Replace("\n", "");
                    items.push_back({wxEmptyString, sInfo});
                }
                items.push_back({wxEmptyString, _("- - - - - - -")});
            }
            else
            {
                items.push_back({_("       GNU GCC"), _("could not find gcc")});
            }

            // ===========================================================================
            // GDB Debugger Info
            wxString sGDBExecutableFileName = wxEmptyString;
            wxArrayString asDBGDetails = wxSplit(cp.DBGconfig,':');
            items.push_back({_("       GNU GDB"), _("details below")});
            cbDebuggerPlugin * pDBGPlugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
            if (!pDBGPlugin)
                items.push_back({_("         Default Plugin"), _("not found")});
            if (pDBGPlugin)
            {
                items.push_back({_("         Default Plugin"),wxEmptyString});
                items.push_back({_("             GUI Name"), pDBGPlugin->GetGUIName()});
                items.push_back({_("             Settings Name"), pDBGPlugin->GetSettingsName()});
            }

            const DebuggerManager::RegisteredPlugins & prDbgPlugins = Manager::Get()->GetDebuggerManager()->GetAllDebuggers();
            bool bFoundGDB = false;
            for (   DebuggerManager::RegisteredPlugins::const_iterator it = prDbgPlugins.begin();
                    it != prDbgPlugins.end();
                    ++it
                )
            {
                const cbDebuggerPlugin *pdebugger = it->first;
                const DebuggerManager::PluginData & pdata = it->second;
                for (   DebuggerManager::ConfigurationVector::const_iterator itConf = pdata.GetConfigurations().begin();
                        itConf != pdata.GetConfigurations().end();
                        ++itConf
                    )
                {
                    if (    (asDBGDetails[0] == pdebugger->GetSettingsName()) &&
                            (asDBGDetails[1] == (*itConf)->GetName())
                        )
                    {
                        items.push_back({_("         Configured Plugin"),wxEmptyString});

                        items.push_back({_("             GetGUIName()"), pdebugger->GetGUIName()});
                        items.push_back({_("             GetSettingsName()"), pdebugger->GetSettingsName()});
                        items.push_back({_("             GetName()"), (*itConf)->GetName()});


                        ConfigManager *config = Manager::Get()->GetConfigManager(wxT("debugger_common"));
                        wxString setPath = wxString::Format(wxT("/sets/%s"),pdebugger->GetSettingsName());
                        wxArrayString configs = config->EnumerateSubPaths(setPath);
                        configs.Sort();

                        for (wxString &sConfig : configs)
                        {
                            wxString sName = config->Read(wxString::Format(wxT("%s/%s/name"), setPath, sConfig));;
                            if (sName.IsSameAs((*itConf)->GetName()))
                            {
                                sGDBExecutableFileName = config->Read(wxString::Format(wxT("%s/%s/values/executable_path"), setPath, sConfig));;
                                items.push_back({_("             GDB executable"),sGDBExecutableFileName});
                                bFoundGDB = true;
                                break;
                            }
                        }
                    }
                }
                if (bFoundGDB)
                    break;
            }

            if (!sGDBExecutableFileName.IsEmpty() && wxFileExists(sGDBExecutableFileName))
            {
                items.push_back({_("             GNU GDB"), _("gdb -v info below")});

                wxString Command = wxString::Format(wxT("%s -v"), sGDBExecutableFileName);
                if (platform::windows && platform::WindowsVersion() >= platform::winver_WindowsNT2000)
                {
                    Command = wxString::Format(wxT("cmd /c %s"), Command);;
                }

                wxArrayString asOutput, asErrors;
                wxExecute(Command, asOutput, asErrors, wxEXEC_NODISABLE);
                for (wxString &sInfo : asOutput)
                {
                    sInfo.Replace(_("\n"), _(""));
                    items.push_back({wxEmptyString, sInfo});
                }
                for (wxString &sInfo : asErrors)
                {
                    sInfo.Replace(_("\n"), _(""));
                    items.push_back({wxEmptyString, sInfo});
                }
                items.push_back({wxEmptyString, _("- - - - - - -")});
            }
            else
            {
                items.push_back({_("             GNU GDB"), _("Could not find gdb!!!")});
            }
        }
    }
    items.push_back({wxEmptyString, wxEmptyString});

    // ===========================================================================
    // Global variables
    ConfigManager *pCfgMan = Manager::Get()->GetConfigManager(_T("gcv"));
    if (pCfgMan)
    {
        const wxString cSets     (_T("/sets/"));
        wxArrayString sets = pCfgMan->EnumerateSubPaths(cSets);
        sets.Sort();

        items.push_back({_("GlobalVariables"), wxEmptyString});

        for (const wxString &sCurrentSet : sets)
        {
            items.push_back({_("       Set"), sCurrentSet});

            wxArrayString vars = pCfgMan->EnumerateSubPaths(cSets + sCurrentSet + _T("/"));
            vars.Sort();

            for (const wxString &sCurrentVar : vars)
            {
                wxString path(cSets + sCurrentSet + _T('/') + sCurrentVar + _T('/'));

                wxArrayString knownMembers = pCfgMan->EnumerateKeys(path);

                for (unsigned int i = 0; i < knownMembers.GetCount(); ++i)
                {
                    Item item;
                    item.name = wxString::Format(_("         %s.%s"), sCurrentSet, knownMembers[i].Lower());
                    item.value = pCfgMan->Read(path + knownMembers[i]);
                    items.push_back(item);
                }

                items.push_back({_("         - - - - - -"), _("- - - - - - - - - - - - - -")});
            }
        }
        items.push_back({wxEmptyString, wxEmptyString});
    }
    else
    {
        items.push_back({_("GlobalVariables"), _("no sets found")});
    }

    // ===========================================================================
    // Macros
    MacrosManager *macroMan = Manager::Get()->GetMacrosManager();
    if (macroMan)
    {
        const MacrosMap &Macros = macroMan->GetMacros();
        if (Macros.empty())
        {
            items.push_back({_("Macros"), _("None")});
        }
        else
        {
            items.push_back({_("Macros:"), wxEmptyString});
            // MacrosMap uses a hash as key, to get sorted macros we need to copy them to a non-hashed map
            std::map <wxString, wxString> NewMap;
            for (MacrosMap::const_iterator it = Macros.begin(); it != Macros.end(); ++it)
            {
                NewMap[it->first] = it->second;
            }

            for (decltype(NewMap)::value_type &Item : NewMap)
            {
                items.push_back({wxString::Format(_("       %s"), Item.first), Item.second});
            }
        }
        items.push_back({wxEmptyString, wxEmptyString});
    }

    // ===========================================================================
    // OS Environment variables
    wxEnvVariableHashMap env;
    if ( wxGetEnvMap(&env) )
    {
        items.push_back({_("OS environment variables"), wxEmptyString});
        for ( wxEnvVariableHashMap::iterator it = env.begin();
              it != env.end();
              ++it )
        {
            items.push_back({wxString::Format(_("       %s"), it->first), it->second});
        }
        items.push_back({wxEmptyString, wxEmptyString});
    }

    // ===========================================================================
    // OS paths
    wxStandardPathsBase& stdp = wxStandardPaths::Get();

    items.push_back({_("OS standard paths"), wxEmptyString});
    items.push_back({wxString::Format(_("       GetAppDocumentsDir")),  stdp.GetAppDocumentsDir()});    // Return the directory for the document files used by this application. More...
    items.push_back({wxString::Format(_("       GetConfigDir")),        stdp.GetConfigDir()});          // Return the directory containing the system config files. More...
    items.push_back({wxString::Format(_("       GetDataDir")),          stdp.GetDataDir()});            // Return the location of the applications global, i.e. not user-specific, data files. More...
    items.push_back({wxString::Format(_("       GetDocumentsDir")),     stdp.GetDocumentsDir()});       // Return the directory containing the current user's documents. More...
    items.push_back({wxString::Format(_("       GetExecutablePath")),   stdp.GetExecutablePath()});     // Return the location for application data files which are host-specific and can't, or shouldn't, be shared with the other machines. More...
    // Only available in wxOS2, wxGTK ports : items.push_back({wxString::Format(_("       GetInstallPrefix")),    stdp.GetInstallPrefix()});      // Return the program installation prefix, e.g. /usr, /opt or /home/zeitlin. More...
    items.push_back({wxString::Format(_("       GetLocalDataDir")),     stdp.GetLocalDataDir()});       // Return the location for application data files which are host-specific and can't, or shouldn't, be shared with the other machines. More...
    // future work with languages : items.push_back({wxString::Format(_("       GetLocalizedResourcesDir")), stdp.GetLocalizedResourcesDir(....)}); //Return the localized resources directory containing the resource files of the specified category for the given language. More...
    items.push_back({wxString::Format(_("       GetPluginsDir")),       stdp.GetPluginsDir()});         // Return the directory where the loadable modules (plugins) live. More...
    items.push_back({wxString::Format(_("       GetResourcesDir")),     stdp.GetResourcesDir()});       // Return the directory where the application resource files are located. More...
    items.push_back({wxString::Format(_("       GetTempDir")),          stdp.GetTempDir()});            // Return the directory for storing temporary files. More...
    items.push_back({wxString::Format(_("       GetUserConfigDir")),    stdp.GetUserConfigDir()});      // Return the directory for the user config files: More...
    items.push_back({wxString::Format(_("       GetUserDataDir")),      stdp.GetUserDataDir()});        // Return the directory for the user-dependent application data files: More...
    items.push_back({wxString::Format(_("       GetUserLocalDataDir")), stdp.GetUserLocalDataDir()});   // Return the directory for user data files which shouldn't be shared with the other machines. More...
    items.push_back({wxEmptyString, wxEmptyString});

    // ===========================================================================
    // Plugins
    PluginManager *plugMan = Manager::Get()->GetPluginManager();
    if (plugMan)
    {

        bool bAnyActive = false;
        const PluginElementsArray& plugins = plugMan->GetPlugins();
        const size_t numPlugins = plugins.GetCount();

        for (size_t i = 0; i < numPlugins; ++i)
        {
            const wxString pluginName(plugins[i]->info.name);
            const bool bActive = Manager::Get()->GetConfigManager(wxT("plugins"))->ReadBool(wxT("/")+pluginName, true);
            if (bActive)
            {
                if (!bAnyActive)
                    items.push_back({_("Active plugins:"), wxEmptyString});
                Item item;
                item.name = wxString::Format(_("       %s"), plugins[i]->info.title);
                item.value = plugins[i]->info.version;
                items.push_back(item);

                bAnyActive = true;
            }
        }

        if (!bAnyActive)
        {
            items.push_back({_("Active plugins"), _("None found")});
        }
        items.push_back({wxEmptyString, wxEmptyString});

        bAnyActive = false;

        for (size_t i = 0; i < numPlugins; ++i)
        {
            const wxString pluginName(plugins[i]->info.name);
            const bool bActive = Manager::Get()->GetConfigManager(wxT("plugins"))->ReadBool(wxT("/")+pluginName, true);
            if (!bActive)
            {
                if (!bAnyActive)
                    items.push_back({_("InActive plugins:"), wxEmptyString});

                Item item;
                item.name = wxString::Format(_("       %s"), plugins[i]->info.title);
                item.value = plugins[i]->info.version;
                items.push_back(item);

                bAnyActive = true;
            }
        }

        if (!bAnyActive)
        {
            items.push_back({_("InActive plugins"), _("None found")});
        }
        items.push_back({wxEmptyString, wxEmptyString});
    }

    // ===========================================================================
    // Backticks

    const cbBackticksMap &Backticks = cbGetBackticksCache();
    if (Backticks.empty())
    {
        items.push_back({_("Backticks"), _("None")});
    }
    else
    {
        items.push_back({_("Backticks:"), wxEmptyString});
        for (cbBackticksMap::const_iterator it = Backticks.begin(); it != Backticks.end(); ++it)
        {
            items.push_back({wxString::Format(_("       %s"), it->first), it->second});
        }
    }
    items.push_back({wxEmptyString, wxEmptyString});


    // ===========================================================================
    // Linux configuration directory permissions
    if (platform::Unix)
    {
        wxArrayString sOutput;
        const wxString sConfigFolder(ConfigManager::GetConfigFolder());

        items.push_back({_("Linux config folder permissions:"), wxEmptyString});
        items.push_back({_("    config folder"), _(sConfigFolder)});
        wxExecute(wxT("ls -l -d ")+sConfigFolder+wxT(" && ls -l ")+sConfigFolder, sOutput, wxEXEC_NODISABLE);
        if (!sOutput.IsEmpty())
        {
            items.push_back({wxJoin(sOutput, '\n'), wxEmptyString});
        }
    }
    items.push_back({wxEmptyString, wxEmptyString});
    items.push_back({wxEmptyString, wxEmptyString});

    // ===========================================================================
    // Process items now!
    int maxNameLength = 0;
    for (const Item &item : items)
    {
        maxNameLength = std::max(maxNameLength, int(item.name.length()));
    }
    maxNameLength = std::min(maxNameLength, 50);

    wxString information;
    for (const Item &item : items)
    {
        if ((item.name == wxEmptyString) || (item.value  == wxEmptyString))
        {
            if (item.name != wxEmptyString)
            {
                information += item.name;
            }
            if (item.value != wxEmptyString)
            {
                if (maxNameLength > int(item.name.length()))
                    information += wxString(wxT(' '), maxNameLength - int(item.name.length()));
                information += wxT(": ") + item.value;
            }

            information += wxT("\n");
        }
        else
        {
            information += item.name;
            if (maxNameLength > int(item.name.length()))
                information += wxString(wxT(' '), maxNameLength - int(item.name.length()));
            information += wxT(": ") + item.value + wxT("\n");
        }
    }

    // ===========================================================================
    // display information now.
    wxTextCtrl *txtInformation = XRCCTRL(*this, "txtInformation", wxTextCtrl);
    txtInformation->SetValue(information);

#ifdef __WXMAC__
    // Courier 8 point is not readable on Mac OS X, increase font size:
    wxFont font1 = txtThanksTo->GetFont();
    font1.SetPointSize(10);
    txtThanksTo->SetFont(font1);

    wxFont font2 = txtLicense->GetFont();
    font2.SetPointSize(10);
    txtLicense->SetFont(font2);

    wxFont font3 = txtInformation->GetFont();
    font3.SetPointSize(10);
    txtInformation->SetFont(font3);
#endif

    Fit();
    CentreOnParent();
}

// class destructor
dlgAbout::~dlgAbout()
{
    // insert your code here
}
