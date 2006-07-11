#include <sdk.h> // Code::Blocks SDK
#include "[HEADER_FILENAME]"

// Implement the plugin's hooks
CB_IMPLEMENT_PLUGIN([PLUGIN_NAME], "[PLUGIN_TITLE]");
[IF NEED_EVENTS]
// events handling
BEGIN_EVENT_TABLE([PLUGIN_NAME], cbPlugin)
	// add any events you want to handle here
END_EVENT_TABLE()[ENDIF NEED_EVENTS]

// constructor
[PLUGIN_NAME]::[PLUGIN_NAME]()
{
	m_PluginInfo.name = _T("[PLUGIN_NAME]");
	m_PluginInfo.title = _("[PLUGIN_TITLE]");
	m_PluginInfo.version = _T("[PLUGIN_VERSION]");
	m_PluginInfo.description = _("[PLUGIN_DESCRIPTION]");
	m_PluginInfo.author = _T("[AUTHOR_NAME]");
	m_PluginInfo.authorEmail = _T("[AUTHOR_EMAIL]");
	m_PluginInfo.authorWebsite = _T("[AUTHOR_WWW]");
	m_PluginInfo.thanksTo = _("[THANKS_TO]");
	m_PluginInfo.license = LICENSE_GPL;
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
	// if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
	// which means you must not use any of the SDK Managers
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be FALSE...
}
[IF HAS_CONFIGURE]
int [PLUGIN_NAME]::Configure()
{
	//create and display the configuration dialog for your plugin
	cbConfigurationDialog dlg(Manager::Get()->GetAppWindow(), wxID_ANY, _("Your dialog title"));
	cbConfigurationPanel* panel = GetConfigurationPanel(&dlg);
	if (panel)
	{
		dlg.AttachConfigurationPanel(panel);
		PlaceWindow(&dlg);
		return dlg.ShowModal() == wxID_OK ? 0 : -1;
	}
	return -1;
}[ENDIF HAS_CONFIGURE]
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

