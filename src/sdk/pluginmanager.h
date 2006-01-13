#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <wx/dynarray.h>
#include "settings.h"
#include "sdk_events.h"
#include "sanitycheck.h"

//forward decls
struct PluginInfo;
class cbPlugin;
class wxDynamicLibrary;
class wxMenuBar;
class wxMenu;

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
        PluginsArray GetCodeCompletionOffers();
        PluginsArray GetOffersFor(PluginType type);
        void AskPluginsForModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg);
        void NotifyPlugins(CodeBlocksEvent& event);
        cbMimePlugin* GetMIMEHandlerForFile(const wxString& filename);
        int Configure();
		void SetupLocaleDomain(const wxString& DomainName);
    private:
		PluginManager();
		~PluginManager();
        PluginElementsArray m_Plugins;
    DECLARE_SANITY_CHECK
};

#endif // PLUGINMANAGER_H

