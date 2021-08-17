/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
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
    #include <wx/versioninfo.h>
    #include "configmanager.h"
    #include "wx/wxscintilla.h"
#endif

#include <wx/clipbrd.h>
#include <wx/display.h>
#include <wx/stdpaths.h>
#include <vector>

#include "appglobals.h"
#include "configmanager.h"
#include "splashscreen.h"
#include "macrosmanager.h"
#include "cbproject.h"
#include "compilerfactory.h"
#include "uservarmanager.h"
#include "debuggermanager.h"
#include "cbplugin.h"

#include "dlghelpsysteminformation.h" // class's header file

static int idbtnCopyToClipboard     = XRCID("ID_btnCopyToClipboard");

BEGIN_EVENT_TABLE(dlgHelpSystemInformation, wxDialog)
    EVT_BUTTON(idbtnCopyToClipboard, dlgHelpSystemInformation::CopyInfoToClipbaord)
END_EVENT_TABLE()

dlgHelpSystemInformation::dlgHelpSystemInformation(wxWindow* parent)
{
    if (!wxXmlResource::Get()->LoadObject(this, parent, "dlgAboutSystemInformation", "wxDialog"))
    {
        cbMessageBox("There was an error loading the \"dlgAboutSystemInformation\" from the \"dlg_about_systerm_information.xrc\" file.",
                     "Information", wxICON_EXCLAMATION);
        return;
    }

    wxButton *cancelButton = XRCCTRL(*this, "wxID_CANCEL", wxButton);
    cancelButton->SetDefault();
    cancelButton->SetFocus();

    struct Item
    {
        wxString name, value;
    };
    std::vector<Item> items;

    // ===========================================================================
    // Code::Blocks version and build info details
    items.push_back({"Name", appglobals::AppName});
    items.push_back({"Version", appglobals::AppActualVersion});
    items.push_back({"App Version", appglobals::AppVersion});
    items.push_back({"Platform", appglobals::AppPlatform});
    items.push_back({"AppWXAnsiUnicode", appglobals::AppWXAnsiUnicode});
    items.push_back({"AppActualVersionVerb", appglobals::AppActualVersionVerb});
    items.push_back({"Build", appglobals::AppBuildTimestamp});
    items.push_back({"Code::Blocks Executable", wxStandardPaths::Get().GetExecutablePath()});;
    items.push_back({wxString(), wxString()});

    items.push_back({"SDK Version", appglobals::AppSDKVersion});

    const wxVersionInfo scintillaVersion = wxScintilla::GetLibraryVersionInfo();
    const wxString scintillaStr = wxString::Format(_("%d.%d.%d"),
                                                   scintillaVersion.GetMajor(),
                                                   scintillaVersion.GetMinor(),
                                                   scintillaVersion.GetMicro());
    items.push_back({"Scintilla Version", scintillaStr});
    // ===========================================================================
    // wxwidget info
    wxVersionInfo wxVer = wxGetLibraryVersionInfo();
    items.push_back({"wxWidget version", wxVer.GetVersionString()});
    if (wxVer.HasDescription())
    {
        items.push_back({"wxWidget description", "- - - - - - -"});
        wxArrayString aswxDescription = wxSplit(wxVer.GetDescription(),'\n');
        for (wxString &sInfo : aswxDescription)
        {
            sInfo.Replace("\n", "");
            if (!sInfo.IsEmpty())
                items.push_back({wxString(), sInfo});
        }
        items.push_back({wxString(), "- - - - - - -"});
    }
    items.push_back({wxString(), wxString()});

    // ===========================================================================
    items.push_back({"Author", "The Code::Blocks Team"});
    items.push_back({"E-mail", appglobals::AppContactEmail});
    items.push_back({"Website", appglobals::AppUrl});
    items.push_back({wxString(), wxString()});

    // ===========================================================================
    // Operating system info
    const wxPlatformInfo &platform = wxPlatformInfo::Get();
    const wxString desktopEnv = platform.GetDesktopEnvironment();

    items.push_back({"Operating System", platform.GetOperatingSystemDescription()});
    if (!desktopEnv.empty())
        items.push_back({"Desktop environment", desktopEnv });
    items.push_back({"wxLocale Language", wxLocale::GetLanguageName(wxLocale::GetSystemLanguage())});
    items.push_back({"Code::Blocks config Folder", ConfigManager::GetConfigFolder()});
    items.push_back({wxString(), wxString()});

    // ===========================================================================
    // Display info
    items.push_back({"Display Info", wxString()});
    items.push_back({"     Scaling factor", wxString::Format(_("%f"), GetContentScaleFactor())});
    items.push_back({"     Detected scaling factor", wxString::Format(_("%f"), cbGetActualContentScaleFactor(*this))});
    const wxSize displayPPI = wxGetDisplayPPI();
    items.push_back({"     Display PPI", wxString::Format(_("%d x %d"), displayPPI.x, displayPPI.y)});

    unsigned displays = wxDisplay::GetCount();
    items.push_back({"     Display count", wxString::Format(_("%u"), displays)});

    for (unsigned ii = 0; ii < displays; ++ii)
    {
        wxDisplay display(ii);

        items.push_back({"     Display", wxString::Format(_("%u"), ii)});
        items.push_back({"        Name", display.GetName()});
        const wxRect geometry = display.GetGeometry();
        items.push_back({"        XY", wxString::Format(_("[%d , %d]"), geometry.GetLeft(), geometry.GetTop())});
        items.push_back({"        Size", wxString::Format(_("[%d , %d]"), geometry.GetWidth(), geometry.GetHeight())});
        items.push_back({"        IsPrimary", display.IsPrimary() ? "True":"False"});
    }
    items.push_back({wxString(), wxString()});

    // ===========================================================================
    // Compiler Info
    items.push_back({"Compiler Information", wxString()});

    ProjectManager *prjMan = Manager::Get()->GetProjectManager();
    cbProject *pProject = prjMan->GetActiveProject();

    Compiler *selectedCompiler = nullptr;
    if (pProject && prjMan->IsProjectStillOpen(pProject))
    {
        selectedCompiler = CompilerFactory::GetCompiler(pProject->GetCompilerID());
        items.push_back({"       Project", "loaded"});
    }
    else
    {
        selectedCompiler = CompilerFactory::GetDefaultCompiler();
        items.push_back({"       Project", "not loaded"});
    }
    items.push_back({"       Default compiler", CompilerFactory::GetDefaultCompilerID()});

    if (selectedCompiler)
    {
        items.push_back({"       Name", selectedCompiler->GetName()});
        items.push_back({"       Version", selectedCompiler->GetVersionString()});
        items.push_back({"       Master Path", selectedCompiler->GetMasterPath()});
        items.push_back({"       IsValid", selectedCompiler->IsValid() ? "True":"False"});
        const CompilerPrograms &cp = selectedCompiler->GetPrograms();
        items.push_back({"       C Compiler", cp.C});
        items.push_back({"       C++ Compiler", cp.CPP});
        items.push_back({"       Dynamic Linker (LD)", cp.LD});
        items.push_back({"       Static Linker (LIB)", cp.LIB});
        items.push_back({"       Windows Resource Compiler", cp.WINDRES});
        items.push_back({"       Make", cp.MAKE});
        items.push_back({"       DBGconfig", cp.DBGconfig});
        if (selectedCompiler->GetExtraPaths().GetCount() == 0)
            items.push_back({"       Extra Paths", "None specified"});
        else
            items.push_back({"       Extra Paths", wxJoin(selectedCompiler->GetExtraPaths(), ',')});

        // ===========================================================================
        // GCC Compiler Info
        if (cp.C.IsSameAs("gcc") || cp.C.IsSameAs("gcc.exe"))
        {
            const wxString gcc(selectedCompiler->GetMasterPath() + wxFileName::GetPathSeparator() + "bin" + wxFileName::GetPathSeparator() + cp.C);
            if (wxFileExists(gcc))
            {
                items.push_back({"       GNU GCC", "details below"});

                wxString Command(gcc+" -v");
                if (platform::windows && platform::WindowsVersion() >= platform::winver_WindowsNT2000)
                {
                    Command = "cmd /c " + Command;
                }

                wxArrayString asOutput, asErrors;
                wxExecute(Command, asOutput, asErrors, wxEXEC_NODISABLE);
                for (wxString &sInfo : asOutput)
                {
                    sInfo.Replace("\n", "");
                    items.push_back({wxString(), sInfo});
                }
                for (wxString &sInfo : asErrors)
                {
                    sInfo.Replace("\n", "");
                    items.push_back({wxString(), sInfo});
                }
                items.push_back({wxString(), "- - - - - - -"});
            }
            else
            {
                items.push_back({"       GNU GCC", "could not find gcc"});
            }

            // ===========================================================================
            // GDB Debugger Info
            wxString sGDBExecutableFileName = wxString();
            wxArrayString asDBGDetails = wxSplit(cp.DBGconfig,':');
            items.push_back({"       GNU GDB", "details below"});
            cbDebuggerPlugin * pDBGPlugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
            if (!pDBGPlugin)
                items.push_back({"         Default Plugin", "not found"});
            if (pDBGPlugin)
            {
                items.push_back({"         Default Plugin",wxString()});
                items.push_back({"             GUI Name", pDBGPlugin->GetGUIName()});
                items.push_back({"             Settings Name", pDBGPlugin->GetSettingsName()});
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
                        items.push_back({"         Configured Plugin",wxString()});

                        items.push_back({"             GetGUIName()", pdebugger->GetGUIName()});
                        items.push_back({"             GetSettingsName()", pdebugger->GetSettingsName()});
                        items.push_back({"             GetName()", (*itConf)->GetName()});


                        ConfigManager *config = Manager::Get()->GetConfigManager("debugger_common");
                        wxString setPath = wxString::Format(_("/sets/%s"),pdebugger->GetSettingsName());
                        wxArrayString configs = config->EnumerateSubPaths(setPath);
                        configs.Sort();

                        for (wxString &sConfig : configs)
                        {
                            wxString sName = config->Read(wxString::Format(_("%s/%s/name"), setPath, sConfig));;
                            if (sName.IsSameAs((*itConf)->GetName()))
                            {
                                sGDBExecutableFileName = config->Read(wxString::Format(_("%s/%s/values/executable_path"), setPath, sConfig));;
                                items.push_back({"             GDB executable",sGDBExecutableFileName});
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
                items.push_back({"             GNU GDB", "gdb -v info below"});

                wxString Command = wxString::Format(_("%s -v"), sGDBExecutableFileName);
                if (platform::windows && platform::WindowsVersion() >= platform::winver_WindowsNT2000)
                {
                    Command = wxString::Format(_("cmd /c %s"), Command);;
                }

                wxArrayString asOutput, asErrors;
                wxExecute(Command, asOutput, asErrors, wxEXEC_NODISABLE);
                for (wxString &sInfo : asOutput)
                {
                    sInfo.Replace("\n", "");
                    items.push_back({wxString(), sInfo});
                }
                for (wxString &sInfo : asErrors)
                {
                    sInfo.Replace("\n", "");
                    items.push_back({wxString(), sInfo});
                }
                items.push_back({wxString(), "- - - - - - -"});
            }
            else
            {
                items.push_back({"             GNU GDB", "Could not find gdb!!!"});
            }
        }
    }
    items.push_back({wxString(), wxString()});

    // ===========================================================================
    // Global variables
    ConfigManager *pCfgMan = Manager::Get()->GetConfigManager(_T("gcv"));
    if (pCfgMan)
    {
        const wxString cSets     (_T("/sets/"));
        wxArrayString sets = pCfgMan->EnumerateSubPaths(cSets);
        sets.Sort();

        items.push_back({"GlobalVariables", wxString()});

        for (const wxString &sCurrentSet : sets)
        {
            items.push_back({"       Set", sCurrentSet});

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

                items.push_back({"         - - - - - -", "- - - - - - - - - - - - - -"});
            }
        }
        items.push_back({wxString(), wxString()});
    }
    else
    {
        items.push_back({"GlobalVariables", "no sets found"});
    }

    // ===========================================================================
    // Macros
    MacrosManager *macroMan = Manager::Get()->GetMacrosManager();
    if (macroMan)
    {
        const MacrosMap &Macros = macroMan->GetMacros();
        if (Macros.empty())
        {
            items.push_back({"Macros", "None"});
        }
        else
        {
            items.push_back({"Macros:", wxString()});
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
        items.push_back({wxString(), wxString()});
    }

    // ===========================================================================
    // OS Environment variables
    wxEnvVariableHashMap env;
    if ( wxGetEnvMap(&env) )
    {
        items.push_back({"OS environment variables", wxString()});
        for ( wxEnvVariableHashMap::iterator it = env.begin(); it != env.end(); ++it )
        {
            items.push_back({wxString::Format(_("       %s"), it->first), it->second});
        }
        items.push_back({wxString(), wxString()});
    }

    // ===========================================================================
    // OS paths
    wxStandardPathsBase& stdp = wxStandardPaths::Get();

    items.push_back({"OS standard paths", wxString()});
    items.push_back({"       GetAppDocumentsDir",  stdp.GetAppDocumentsDir()});    // Return the directory for the document files used by this application. More...
    items.push_back({"       GetConfigDir",        stdp.GetConfigDir()});          // Return the directory containing the system config files. More...
    items.push_back({"       GetDataDir",          stdp.GetDataDir()});            // Return the location of the applications global, i.e. not user-specific, data files. More...
    items.push_back({"       GetDocumentsDir",     stdp.GetDocumentsDir()});       // Return the directory containing the current user's documents. More...
    items.push_back({"       GetExecutablePath",   stdp.GetExecutablePath()});     // Return the location for application data files which are host-specific and can't, or shouldn't, be shared with the other machines. More...
    // Only available in wxOS2, wxGTK ports : items.push_back({"       GetInstallPrefix",    stdp.GetInstallPrefix()});      // Return the program installation prefix, e.g. /usr, /opt or /home/zeitlin. More...
    items.push_back({"       GetLocalDataDir",     stdp.GetLocalDataDir()});       // Return the location for application data files which are host-specific and can't, or shouldn't, be shared with the other machines. More...
    // future work with languages : items.push_back({"       GetLocalizedResourcesDir", stdp.GetLocalizedResourcesDir(....)}); //Return the localized resources directory containing the resource files of the specified category for the given language. More...
    items.push_back({"       GetPluginsDir",       stdp.GetPluginsDir()});         // Return the directory where the loadable modules (plugins) live. More...
    items.push_back({"       GetResourcesDir",     stdp.GetResourcesDir()});       // Return the directory where the application resource files are located. More...
    items.push_back({"       GetTempDir",          stdp.GetTempDir()});            // Return the directory for storing temporary files. More...
    items.push_back({"       GetUserConfigDir",    stdp.GetUserConfigDir()});      // Return the directory for the user config files: More...
    items.push_back({"       GetUserDataDir",      stdp.GetUserDataDir()});        // Return the directory for the user-dependent application data files: More...
    items.push_back({"       GetUserLocalDataDir", stdp.GetUserLocalDataDir()});   // Return the directory for user data files which shouldn't be shared with the other machines. More...
    items.push_back({wxString(), wxString()});

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
            const bool bActive = Manager::Get()->GetConfigManager("plugins")->ReadBool("/" + pluginName, true);
            if (bActive)
            {
                if (!bAnyActive)
                    items.push_back({"Active plugins:", wxString()});
                Item item;
                item.name = wxString::Format(_("       %s"), plugins[i]->info.title);
                item.value = plugins[i]->info.version;
                items.push_back(item);

                bAnyActive = true;
            }
        }

        if (!bAnyActive)
        {
            items.push_back({"Active plugins", "None found"});
        }
        items.push_back({wxString(), wxString()});

        bAnyActive = false;

        for (size_t i = 0; i < numPlugins; ++i)
        {
            const wxString pluginName(plugins[i]->info.name);
            const bool bActive = Manager::Get()->GetConfigManager("plugins")->ReadBool("/" + pluginName, true);
            if (!bActive)
            {
                if (!bAnyActive)
                    items.push_back({"InActive plugins:", wxString()});

                Item item;
                item.name = wxString::Format(_("       %s"), plugins[i]->info.title);
                item.value = plugins[i]->info.version;
                items.push_back(item);

                bAnyActive = true;
            }
        }

        if (!bAnyActive)
        {
            items.push_back({"InActive plugins", "None found"});
        }
        items.push_back({wxString(), wxString()});
    }

    // ===========================================================================
    // Backticks

    const cbBackticksMap &Backticks = cbGetBackticksCache();
    if (Backticks.empty())
    {
        items.push_back({"Backticks", "None"});
    }
    else
    {
        items.push_back({"Backticks:", wxString()});
        for (cbBackticksMap::const_iterator it = Backticks.begin(); it != Backticks.end(); ++it)
        {
            items.push_back({wxString::Format(_("       %s"), it->first), it->second});
        }
    }
    items.push_back({wxString(), wxString()});


    // ===========================================================================
    // Linux configuration directory permissions
    if (platform::Unix)
    {
        wxArrayString sOutput;
        const wxString sConfigFolder(ConfigManager::GetConfigFolder());

        items.push_back({"Linux config folder permissions:", wxString()});
        items.push_back({"    config folder", sConfigFolder});
        wxExecute("ls -l -d " + sConfigFolder + " && ls -l " + sConfigFolder, sOutput, wxEXEC_NODISABLE);
        if (!sOutput.IsEmpty())
        {
            items.push_back({wxJoin(sOutput, '\n'), wxString()});
        }
    }
    items.push_back({wxString(), wxString()});
    items.push_back({wxString(), wxString()});

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
        if ((item.name == wxString()) || (item.value  == wxString()))
        {
            if (item.name != wxString())
            {
                information += item.name;
            }
            if (item.value != wxString())
            {
                if (maxNameLength > int(item.name.length()))
                    information += wxString(' ', maxNameLength - int(item.name.length()));
                information += ": " + item.value;
            }

            information += "\n";
        }
        else
        {
            information += item.name;
            if (maxNameLength > int(item.name.length()))
                information += wxString(' ', maxNameLength - int(item.name.length()));
            information += ": " + item.value + "\n";
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
dlgHelpSystemInformation::~dlgHelpSystemInformation()
{
    // insert your code here
}

void dlgHelpSystemInformation::CopyInfoToClipbaord(cb_unused wxCommandEvent& event)
{
   if (wxTheClipboard->Open())
   {
        if (wxTheClipboard->IsSupported( wxDF_TEXT ))
        {
            wxBusyCursor wait;
            wxTextCtrl *txtInformation = XRCCTRL(*this, "txtInformation", wxTextCtrl);
            wxString sTxtInformation = txtInformation->GetValue();

            // Sanatise the data
            sTxtInformation.Replace(wxGetHostName(), "<ComputerName>");
            sTxtInformation.Replace(wxGetFullHostName(),"<ComputerName>");
            sTxtInformation.Replace(wxGetUserName(), "<UserName>");

            wxTheClipboard->SetData( new wxTextDataObject(sTxtInformation) );
        }
        wxTheClipboard->Close();
   }
}
