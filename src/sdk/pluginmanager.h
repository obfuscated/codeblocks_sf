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

struct PluginElement
{
    wxString name;
    wxString fileName;
    wxDynamicLibrary* library;
    FreePluginProc freeProc;
    cbPlugin* plugin;
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
        int ScanForPlugins(const wxString& path);
        bool LoadPlugin(const wxString& pluginName);
        void LoadAllPlugins();
        void UnloadAllPlugins();
        int ExecutePlugin(const wxString& pluginName);
        int ConfigurePlugin(const wxString& pluginName);
        const PluginInfo* GetPluginInfo(const wxString& pluginName);
        PluginElementsArray& GetPlugins(){ return m_Plugins; }
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
    private:
        PluginManager();
        ~PluginManager();
        PluginElementsArray m_Plugins;
};

#endif // PLUGINMANAGER_H
