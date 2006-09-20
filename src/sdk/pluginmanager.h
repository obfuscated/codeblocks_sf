#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <wx/dynarray.h>
#include "settings.h"
#include "manager.h"
#include "cbplugin.h" // for FreePluginProc

//forward decls
struct PluginInfo;
class cbPlugin;
class cbConfigurationPanel;
class wxDynamicLibrary;
class wxMenuBar;
class wxMenu;
class CodeBlocksEvent;
class TiXmlDocument;

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

/*
 * No description
 */
class DLLIMPORT PluginManager : public Mgr<PluginManager>
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
        int ExecutePlugin(const wxString& pluginName);
        int ConfigurePlugin(const wxString& pluginName);

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
        void NotifyPlugins(CodeBlocksEvent& event);
        cbMimePlugin* GetMIMEHandlerForFile(const wxString& filename);
        void GetConfigurationPanels(int group, wxWindow* parent, ConfigurationPanelsArray& arrayToFill);
        void GetProjectConfigurationPanels(wxWindow* parent, cbProject* project, ConfigurationPanelsArray& arrayToFill);
        int Configure();
        void SetupLocaleDomain(const wxString& DomainName);
    protected:
        /// @return True if the plugin should be loaded, false if not.
        bool ReadManifestFile(const wxString& pluginFilename,
                                const wxString& pluginName = wxEmptyString,
                                PluginInfo* infoOut = 0);
    private:
        PluginManager();
        ~PluginManager();
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
};

#endif // PLUGINMANAGER_H
