/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date$
*/

#include "configmanager.h" // class's header file

#include <wx/intl.h>
#include <wx/fileconf.h>
#include "manager.h"
#include "messagemanager.h"
#include "managerproxy.h"

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(Configurations);
Configurations ConfigManager::s_Configurations;

void ConfigManager::Init(wxConfigBase* config)
{
	ConfigBaseProxy::Set( config );
//    if (!g_Config)
//        g_Config = new wxConfig(appName, vendorName);
}

wxConfigBase* ConfigManager::Get()
{
    if (!ConfigBaseProxy::Get())
	{
        ConfigManager::Init(wxConfigBase::Get());
		Manager::Get()->GetMessageManager()->Log(_("ConfigManager initialized"));
	}
    return ConfigBaseProxy::Get();
}

ConfigManager::ConfigManager()
{
}

ConfigManager::~ConfigManager()
{
    if (ConfigBaseProxy::Get())
    {
        delete ConfigBaseProxy::Get();
        ConfigBaseProxy::Set( NULL );
    }
}

void ConfigManager::AddConfiguration(const wxString& desc, const wxString& key)
{
    ConfigurationPath* conf = new ConfigurationPath;
    conf->desc = desc;
    conf->key = key;
    s_Configurations.Add(*conf);
}

static void ExportGroup(wxConfigBase* conf, wxConfigBase* file, const wxString& groupName)
{
    long group_cookie;
    long entry_cookie;
    wxString group;
    wxString entry;

    wxString rStr;
    long int rInt;
    bool rBool;
    double rDbl;

    // loop group entries
    bool cont_entry = conf->GetFirstEntry(entry, entry_cookie);
    while (cont_entry)
    {
        wxConfigBase::EntryType et = conf->GetEntryType(entry);
        switch (et)
        {
            case wxConfigBase::Type_String:
                conf->Read(entry, &rStr);
                file->Write(entry, rStr);
                break;
            case wxConfigBase::Type_Boolean:
                conf->Read(entry, &rBool);
                file->Write(entry, rBool);
                break;
            case wxConfigBase::Type_Integer:
                conf->Read(entry, &rInt);
                file->Write(entry, rInt);
                break;
            case wxConfigBase::Type_Float:
                conf->Read(entry, &rDbl);
                file->Write(entry, rDbl);
                break;
            default: break;
        }
        cont_entry = conf->GetNextEntry(entry, entry_cookie);
    }

    // loop groups and recurse
    bool cont_group = conf->GetFirstGroup(group, group_cookie);
    while (cont_group)
    {
        if (group != "*")
        {
            wxString path = (groupName != "/" ? groupName : "" ) + "/" + group;
    
            conf->SetPath(path);
            file->SetPath(path);
            ExportGroup(conf, file, path);
    
            conf->SetPath(groupName);
            file->SetPath(groupName);
        }
        cont_group = conf->GetNextGroup(group, group_cookie);
    }
}

bool ConfigManager::ExportToFile(const wxString& filename, int index)
{
    if (index < 0 || index >= (int)s_Configurations.Count())
        return false;
    return ExportToFile(filename, s_Configurations[index]);
}

bool ConfigManager::ExportToFile(const wxString& filename, const ConfigurationPath& configuration)
{
    wxConfigBase* conf = ConfigManager::Get();
    if (!conf)
        return false;
    wxString oldpath = conf->GetPath();

    wxFileConfig* file = new wxFileConfig("", "", filename, "", wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
    if (!file)
        return false;

    conf->SetPath(configuration.key);
    file->SetPath(configuration.key);
    ExportGroup(conf, file, configuration.key);

    delete file;
    conf->SetPath(oldpath);

    return true;
}

bool ConfigManager::ImportFromFile(const wxString& filename, int index)
{
    if (index < 0 || index >= (int)s_Configurations.Count())
        return false;
    return ImportFromFile(filename, s_Configurations[index]);
}

bool ConfigManager::ImportFromFile(const wxString& filename, const ConfigurationPath& configuration)
{
    return true;
}
