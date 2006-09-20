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
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/checklst.h>
    #include <wx/intl.h>
    #include <wx/string.h>
    #include <wx/xrc/xmlres.h>
    #include "manager.h"
    #include "configmanager.h"
    #include "pluginmanager.h"
#endif

#include "pluginsconfigurationdlg.h" // class's header file

// class constructor
PluginsConfigurationDlg::PluginsConfigurationDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgConfigurePlugins"));

    wxCheckListBox* list = XRCCTRL(*this, "lstPlugins", wxCheckListBox);
    PluginManager* man = Manager::Get()->GetPluginManager();
    const PluginElementsArray& plugins = man->GetPlugins();

    // populate Plugins checklist
    for (unsigned int i = 0; i < plugins.GetCount(); ++i)
    {
        const PluginElement* elem = plugins[i];
        if (!elem->plugin)
        {
            // this plugin is not loaded
            // display its name
            list->Append(elem->info.name);
            list->Check(list->GetCount()-1, false);
            continue;
        }
        list->Append(elem->info.title + _(", v") + elem->info.version);

        wxString baseKey;
        baseKey << _T("/") << elem->info.name;
        list->Check(list->GetCount()-1, Manager::Get()->GetConfigManager(_T("plugins"))->ReadBool(baseKey, true));
    }
}

// class destructor
PluginsConfigurationDlg::~PluginsConfigurationDlg()
{
	// insert your code here
}

void PluginsConfigurationDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        const wxCheckListBox* list = XRCCTRL(*this, "lstPlugins", wxCheckListBox);
        PluginManager* man = Manager::Get()->GetPluginManager();
        const PluginElementsArray& plugins = man->GetPlugins();

        for (int i = 0; i < list->GetCount(); ++i)
        {
            const PluginElement* elem = plugins[i];
            wxString baseKey;
            baseKey << _T("/") << elem->info.name;
            bool checked = list->IsChecked(i);
            Manager::Get()->GetConfigManager(_T("plugins"))->Write(baseKey, checked);
        }
    }

    wxDialog::EndModal(retCode);
}
