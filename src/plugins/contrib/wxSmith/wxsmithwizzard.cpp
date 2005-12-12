#include "wxsheaders.h"
#include "wxsmithwizzard.h"
#include "wxswizzard.h"

wxSmithWizzard::wxSmithWizzard()
{
	m_PluginInfo.name = _("wxSmithWizzard");
	m_PluginInfo.title = _("wxSmith - Project Wizzard plugin");
	m_PluginInfo.version = _("1.0");
	m_PluginInfo.description = _("Project Wizzard for wxSmith");
	m_PluginInfo.author = _("BYO");
	m_PluginInfo.authorEmail = _("byo.spoon@gmail.com");
	m_PluginInfo.authorWebsite = _T("");
	m_PluginInfo.thanksTo = _T("");
	m_PluginInfo.license = LICENSE_GPL;
	m_PluginInfo.hasConfigure = false;
    Bitmap.LoadFile(
        ConfigManager::GetDataFolder() + 
        _T("/images/wxsmith/wxsmith32x32.png"),
        wxBITMAP_TYPE_PNG);
}

wxSmithWizzard::~wxSmithWizzard()
{
}

int wxSmithWizzard::Launch(int Index)
{
    wxsWizzard Dlg(0L);
    return Dlg.ShowModal();
}
