#include <sdk.h> // Code::Blocks SDK
#include <configurationpanel.h>
#include "[HEADER_FILENAME]"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<[PLUGIN_NAME]> reg(_T("[PLUGIN_NAME]"));
}

[IF NEED_EVENTS]
// events handling
BEGIN_EVENT_TABLE([PLUGIN_NAME], cbPlugin)
    // add any events you want to handle here
END_EVENT_TABLE()[ENDIF NEED_EVENTS]

// constructor
[PLUGIN_NAME]::[PLUGIN_NAME]()
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if(!Manager::LoadResource(_T("[PLUGIN_NAME].zip")))
    {
        NotifyMissingFile(_T("[PLUGIN_NAME].zip"));
    }
}

// destructor
[PLUGIN_NAME]::~[PLUGIN_NAME]()
{
}

void [PLUGIN_NAME]::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
}

void [PLUGIN_NAME]::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
}
[IF HAS_MENU]
void [PLUGIN_NAME]::BuildMenu(wxMenuBar* menuBar)
{
    //The application is offering its menubar for your plugin,
    //to add any menu items you want...
    //Append any items you need in the menu...
    //NOTE: Be careful in here... The application's menubar is at your disposal.
    NotImplemented(_T("[PLUGIN_NAME]::BuildMenu()"));
}[ENDIF HAS_MENU]
[IF HAS_MODULE_MENU]
void [PLUGIN_NAME]::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    //Some library module is ready to display a pop-up menu.
    //Check the parameter \"type\" and see which module it is
    //and append any items you need in the menu...
    //TIP: for consistency, add a separator as the first item...
    NotImplemented(_T("[PLUGIN_NAME]::BuildModuleMenu()"));
}[ENDIF HAS_MODULE_MENU]
[IF HAS_TOOLBAR]
bool [PLUGIN_NAME]::BuildToolBar(wxToolBar* toolBar)
{
    //The application is offering its toolbar for your plugin,
    //to add any toolbar items you want...
    //Append any items you need on the toolbar...
    NotImplemented(_T("[PLUGIN_NAME]::BuildToolBar()"));

    // return true if you add toolbar items
    return false;
}[ENDIF HAS_TOOLBAR]

