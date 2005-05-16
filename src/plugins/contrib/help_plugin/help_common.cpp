#include "help_common.h"
#include <configmanager.h>
#include <wx/intl.h>

int g_DefaultHelpIndex = -1;

void LoadHelpFilesMap(HelpFilesMap& map)
{
	map.clear();
	long cookie;
	wxString entry;
	wxConfigBase* conf = ConfigManager::Get();
	wxString oldPath = conf->GetPath();
	conf->SetPath(_("/help_plugin"));
	bool cont = conf->GetFirstEntry(entry, cookie);
	while (cont)
	{
        if (entry == _("default"))
            g_DefaultHelpIndex = conf->Read(entry, (int)-1);
        else
        {
            wxString file = conf->Read(entry, _(""));
            if (!file.IsEmpty())
                map[entry] = file;
        }
		cont = conf->GetNextEntry(entry, cookie);
	}
	conf->SetPath(oldPath);
}

void SaveHelpFilesMap(HelpFilesMap& map)
{
	wxConfigBase* conf = ConfigManager::Get();
	conf->DeleteGroup(_("/help_plugin"));
	wxString oldPath = conf->GetPath();
	conf->SetPath(_("/help_plugin"));
	HelpFilesMap::iterator it;
	for (it = map.begin(); it != map.end(); ++it)
	{
        wxString file = it->second;
        if (!file.IsEmpty())
            conf->Write(it->first, file);
	}
	conf->Write(_("default"), g_DefaultHelpIndex);
	conf->SetPath(oldPath);
}
