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

#include <sdk.h>
#ifndef CB_PRECOMP
#include <wx/fs_zip.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/utils.h>
#include <wx/xrc/xmlres.h>
#include "cbproject.h"
#include "configmanager.h"
#include "globals.h"
#include "manager.h"
#include "projectmanager.h"
#endif
#include <wx/filesys.h>
#include "classwizard.h"
#include "classwizarddlg.h"

// this auto-registers the plugin
namespace
{
    PluginRegistrant<ClassWizard> reg(_T("ClassWizard"));
}

ClassWizard::ClassWizard()
{
    if(!Manager::LoadResource(_T("classwizard.zip")))
    {
        NotifyMissingFile(_T("classwizard.zip"));
    }
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
    PlaceWindow(&dlg);
	if (dlg.ShowModal() == wxID_OK)
	{
		if (!prj)
		{
			cbMessageBox(	_("The new class has been created."),
							_("Information"),
							wxOK | wxICON_INFORMATION,
							Manager::Get()->GetAppWindow());
		}
		else if( cbMessageBox( _("The new class has been created.\n"
							"Do you want to add it to the current project?"),
							_("Add to project?"),
							wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION,
							Manager::Get()->GetAppWindow()) == wxID_YES)
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
