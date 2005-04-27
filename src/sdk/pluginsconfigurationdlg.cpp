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

#include "pluginsconfigurationdlg.h" // class's header file
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/checklst.h>
#include <wx/button.h>
#include "manager.h"
#include "configmanager.h"
#include "pluginmanager.h"
#include "personalitymanager.h"

BEGIN_EVENT_TABLE(PluginsConfigurationDlg, wxDialog)
	EVT_BUTTON(XRCID("btnOK"), PluginsConfigurationDlg::OnOK)
END_EVENT_TABLE()

// class constructor
PluginsConfigurationDlg::PluginsConfigurationDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgConfigurePlugins"));

    wxCheckListBox* list = XRCCTRL(*this, "lstPlugins", wxCheckListBox);
    PluginManager* man = Manager::Get()->GetPluginManager();
    PluginElementsArray& plugins = man->GetPlugins();

    const wxString& personalityKey = Manager::Get()->GetPersonalityManager()->GetPersonalityKey();

    // populate Plugins and Help/Plugins menu
    for (unsigned int i = 0; i < plugins.GetCount(); ++i)
    {
        PluginElement* elem = plugins[i];
        list->Append(elem->plugin->GetInfo()->title + ", v" + elem->plugin->GetInfo()->version);

        wxString baseKey;
        baseKey << personalityKey << "/plugins/" << elem->name;
        list->Check(list->GetCount()-1, ConfigManager::Get()->Read(baseKey, true));
    }
}

// class destructor
PluginsConfigurationDlg::~PluginsConfigurationDlg()
{
	// insert your code here
}

void PluginsConfigurationDlg::OnOK(wxCommandEvent& event)
{
    wxCheckListBox* list = XRCCTRL(*this, "lstPlugins", wxCheckListBox);
    PluginManager* man = Manager::Get()->GetPluginManager();
    PluginElementsArray& plugins = man->GetPlugins();

    const wxString& personalityKey = Manager::Get()->GetPersonalityManager()->GetPersonalityKey();

    for (int i = 0; i < list->GetCount(); ++i)
    {
        PluginElement* elem = plugins[i];
        wxString baseKey;
        baseKey << personalityKey << "/plugins/" << elem->name;
        bool checked = list->IsChecked(i);
        ConfigManager::Get()->Write(baseKey, checked);
    }
    
    EndModal(wxID_OK);
}
