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

#include "pluginwizard.h"
#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <wx/mdi.h>
#include <wx/msgdlg.h>
#include <manager.h>
#include <configmanager.h>
#include "pluginwizarddlg.h"

cbPlugin* GetPlugin()
{
    return new PluginWizard;
}


PluginWizard::PluginWizard()
{
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::Get()->Read("data_path", wxEmptyString);
    wxXmlResource::Get()->Load(resPath + "/plugin_wizard.zip#zip:*.xrc");

    m_PluginInfo.name = "PluginWizard";
    m_PluginInfo.title = "Plugin wizard";
    m_PluginInfo.version = "0.1";
    m_PluginInfo.description = "This is the Code::Blocks plugin wizard...";
    m_PluginInfo.author = "Yiannis An. Mandravellos";
    m_PluginInfo.authorEmail = "info@codeblocks.org";
    m_PluginInfo.authorWebsite = "www.codeblocks.org";
    m_PluginInfo.thanksTo = "";
	m_PluginInfo.hasConfigure = false;
}

PluginWizard::~PluginWizard()
{
}

void PluginWizard::OnAttach()
{
}

void PluginWizard::OnRelease()
{
}

int PluginWizard::Execute()
{
	PluginWizardDlg dlg;
	if (dlg.ShowModal() == wxID_OK)
	{
		wxMessageDialog msg(Manager::Get()->GetAppWindow(),
						_("The new plugin has been created."),
						_("Information"),
						wxOK | wxICON_INFORMATION);
		msg.ShowModal();
		return 0;
	}
		
	return -1;
}
