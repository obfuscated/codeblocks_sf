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

#include <wx/intl.h>
#include "configmanager.h" // class's header file
#include "manager.h"
#include "messagemanager.h"

wxConfigBase* g_Config = NULL;

void ConfigManager::Init(wxConfigBase* config)
{
    g_Config = config;
//    if (!g_Config)
//        g_Config = new wxConfig(appName, vendorName);
}

wxConfigBase* ConfigManager::Get()
{
    if (!g_Config)
	{
        ConfigManager::Init(wxConfigBase::Get());
		Manager::Get()->GetMessageManager()->Log(_("ConfigManager initialized"));
	}
    return g_Config;
}

ConfigManager::ConfigManager()
{
}

ConfigManager::~ConfigManager()
{
    if (g_Config)
    {
        delete g_Config;
        g_Config = NULL;
    }
}

