/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <vector>
#include <map>

#include <wx/dynarray.h>
#include "globals.h" // PluginType
#include "settings.h"
#include "manager.h"

//forward decls
struct PluginInfo;
class cbPlugin;
class cbMimePlugin;
class cbConfigurationPanel;
class cbProject;
class wxDynamicLibrary;
class wxMenuBar;
class wxMenu;
class CodeBlocksEvent;
class TiXmlDocument;
class FileTreeData;

// typedefs for plugins' function pointers
typedef void(*PluginSDKVersionProc)(int*,int*,int*);
typedef cbPlugin*(*CreatePluginProc)();
typedef void(*FreePluginProc)(cbPlugin*);

/** Information about the plugin */
struct PluginInfo
{
    wxString name;
    wxString title;
    wxString version;
    wxString description;
    wxString author;
    wxString authorEmail;
    wxString authorWebsite;
    wxString thanksTo;
    wxString license;
};

// struct with info about each pluing
struct PluginElement
{
    PluginInfo info; // plugin's info struct
    wxString fileName; // plugin's filename
    wxDynamicLibrary* library; // plugin's library
    FreePluginProc freeProc; // plugin's release function pointer
    cbPlugin* plugin; // the plugin itself
};

WX_DEFINE_ARRAY(PluginElement*, PluginElementsArray);
WX_DEFINE_ARRAY(cbPlugin*, PluginsArray);
WX_DEFINE_ARRAY(cbConfigurationPanel*, ConfigurationPanelsArray);

/**
 * PluginManager manages plugins.
 *
 * There are two plugin types: binary and scripted.
 *
 * Binary plugins are dynamically loaded shared libraries (dll/so) which
 * can do pretty much anything with the SDK.
 *
 * Script plugins are more lightweight and are very convenient for
 * smaller scale/functionality plugins.
 */
class DLLIMPORT PluginManager : public Mgr<PluginManager>, public wxEvtHandler
{
    public:
        friend class Mgr<PluginManager>;
        friend class Manager; // give Manager access to our private members
        void CreateMenu(wxMenuBar* menuBar);
        void ReleaseMenu(wxMenuBar* menuBar);

        void RegisterPlugin(const wxString& name,
                            CreatePluginProc createProc,
                            FreePluginProc freeProc,
                            PluginSDKVersionProc versionProc);

        int ScanForPlugins(const wxString& path);
        bool LoadPlugin(const wxString& pluginName);
        void LoadAllPlugins();
        void UnloadAllPlugins();
        void UnloadPlugin(cbPlugin* plugin);
        int ExecutePlugin(const wxString& pluginName);
        int ConfigurePlugin(const wxString& pluginName);

        bool AttachPlugin(cbPlugin* plugin, bool ignoreSafeMode = false);
        bool DetachPlugin(cbPlugin* plugin);

        bool InstallPlugin(const wxString& pluginName, bool forAllUsers = true, bool askForConfirmation = true);
        bool UninstallPlugin(cbPlugin* plugin, bool removeFiles = true);
        bool ExportPlugin(cbPlugin* plugin, const wxString& filename);

        const PluginInfo* GetPluginInfo(const wxString& pluginName);
        const PluginInfo* GetPluginInfo(cbPlugin* plugin);

        PluginElementsArray& GetPlugins(){ return m_Plugins; }

        PluginElement* FindElementByName(const wxString& pluginName);
        cbPlugin* FindPluginByName(const wxString& pluginName);
        cbPlugin* FindPluginByFileName(const wxString& pluginFileName);

        PluginsArray GetToolOffers();
        PluginsArray GetMimeOffers();
        PluginsArray GetCompilerOffers();
        PluginsArray GetDebuggerOffers();
        PluginsArray GetCodeCompletionOffers();
        PluginsArray GetOffersFor(PluginType type);
        void AskPluginsForModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
        cbMimePlugin* GetMIMEHandlerForFile(const wxString& filename);
        void GetConfigurationPanels(int group, wxWindow* parent, ConfigurationPanelsArray& arrayToFill);
        void GetProjectConfigurationPanels(wxWindow* parent, cbProject* project, ConfigurationPanelsArray& arrayToFill);
        int Configure();
        void SetupLocaleDomain(const wxString& DomainName);

        void NotifyPlugins(CodeBlocksEvent& event);
        void NotifyPlugins(CodeBlocksDockEvent& event);
        void NotifyPlugins(CodeBlocksLayoutEvent& event);
        
        static void SetSafeMode(bool on){ s_SafeMode = on; }
        static bool GetSafeMode(){ return s_SafeMode; }
    private:
        PluginManager();
        ~PluginManager();

		void OnScriptMenu(wxCommandEvent& event);
		void OnScriptModuleMenu(wxCommandEvent& event);

        /// @return True if the plugin should be loaded, false if not.
        bool ReadManifestFile(const wxString& pluginFilename,
                                const wxString& pluginName = wxEmptyString,
                                PluginInfo* infoOut = 0);
		void ReadExtraFilesFromManifestFile(const wxString& pluginFilename,
											wxArrayString& extraFiles);
        bool ExtractFile(const wxString& bundlename,
                        const wxString& src_filename,
                        const wxString& dst_filename,
                        bool isMandatory = true);

        PluginElementsArray m_Plugins;
        wxString m_CurrentlyLoadingFilename;
        wxDynamicLibrary* m_pCurrentlyLoadingLib;
        TiXmlDocument* m_pCurrentlyLoadingManifestDoc;

        // this struct fills the following vector each time
        // RegisterPlugin() is called.
        // this vector is then used in LoadPlugin() (which triggered
        // the call to RegisterPlugin()) to actually
        // load the plugins and then it is cleared.
        //
        // This is done to avoid global variables initialization order issues
        // inside the plugins (yes, it happened to me ;)).
        struct PluginRegistration
        {
            PluginRegistration() : createProc(0), freeProc(0), versionProc(0) {}
            PluginRegistration(const PluginRegistration& rhs)
                : name(rhs.name),
                createProc(rhs.createProc),
                freeProc(rhs.freeProc),
                versionProc(rhs.versionProc),
                info(rhs.info)
            {}
            wxString name;
            CreatePluginProc createProc;
            FreePluginProc freeProc;
            PluginSDKVersionProc versionProc;
            PluginInfo info;
        };
        std::vector<PluginRegistration> m_RegisteredPlugins;
        
        static bool s_SafeMode;
        
		DECLARE_EVENT_TABLE()
};

#endif // PLUGINMANAGER_H
