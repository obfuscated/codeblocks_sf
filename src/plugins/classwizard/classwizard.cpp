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

#include "classwizard.h"
#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <wx/mdi.h>
#include <wx/msgdlg.h>
#include <manager.h>
#include <configmanager.h>
#include <projectmanager.h>
#include "classwizarddlg.h"
#include <cbproject.h>

CB_IMPLEMENT_PLUGIN(ClassWizard);


ClassWizard::ClassWizard()
{
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::Get()->Read(_T("data_path"), wxEmptyString);
    wxXmlResource::Get()->Load(resPath + _T("/class_wizard.zip#zip:*.xrc"));

    m_PluginInfo.name = _T("ClassWizard");
    m_PluginInfo.title = _("Class wizard");
    m_PluginInfo.version = _("0.2");
    m_PluginInfo.description = _("This plugin provides an easy way to create a " \
                               "new C++ class file pair. It's by no means " \
                               "complete yet but it's here nevertheless;)");
    m_PluginInfo.author = _("Yiannis An. Mandravellos");
    m_PluginInfo.authorEmail = _("info@codeblocks.org");
    m_PluginInfo.authorWebsite = _("www.codeblocks.org");
    m_PluginInfo.thanksTo = _T("");
	m_PluginInfo.hasConfigure = false;
}

ClassWizard::~ClassWizard()
{
}

void ClassWizard::OnAttach()
{
}

void ClassWizard::OnRelease(bool appShutDown)
{
}

int ClassWizard::Execute()
{
    ProjectManager* prjMan = Manager::Get()->GetProjectManager();
    cbProject* prj = prjMan->GetActiveProject();
    if (prj)
        wxSetWorkingDirectory(prj->GetBasePath());

	ClassWizardDlg dlg(Manager::Get()->GetAppWindow());
	if (dlg.ShowModal() == wxID_OK)
	{
		if (!prj)
		{
			wxMessageDialog msg(Manager::Get()->GetAppWindow(),
							_("The new class has been created."),
							_("Information"),
							wxOK | wxICON_INFORMATION);
			msg.ShowModal();
			return 0;
		}

		wxMessageDialog msg(Manager::Get()->GetAppWindow(),
							_("The new class has been created.\n"
							"Do you want to add it to the current project?"),
							_("Add to project?"),
							wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION);
		if (msg.ShowModal() == wxID_YES)
		{
            wxArrayInt targets;
			prjMan->AddFileToProject(dlg.GetHeaderFilename(), prj, targets);
			if (targets.GetCount() != 0)
				prjMan->AddFileToProject(dlg.GetImplementationFilename(), prj, targets);
			prjMan->RebuildTree();
		}
		return 0;
	}

	return -1;
}
