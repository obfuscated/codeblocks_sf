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

#include <wx/mdi.h>
#include <wx/intl.h>
#include <wx/dir.h>
#include <wx/menu.h>
#include <wx/log.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>

#include "templatemanager.h"
#include "manager.h"
#include "configmanager.h"
#include "messagemanager.h"
#include "projectmanager.h"
#include "cbproject.h"
#include "globals.h"
#include "managerproxy.h"

int idMenuNewFromTemplate = wxNewId();

TemplateManager* TemplateManager::Get()
{
	if (!TemplateManagerProxy::Get())
	 {
		TemplateManagerProxy::Set( new TemplateManager );
		Manager::Get()->GetMessageManager()->Log(_("TemplateManager initialized"));
	 }
	 return TemplateManagerProxy::Get();
}

void TemplateManager::Free()
{
	if (TemplateManagerProxy::Get())
	{
		delete TemplateManagerProxy::Get();
		TemplateManagerProxy::Set( 0L );
	}
}

BEGIN_EVENT_TABLE(TemplateManager, wxEvtHandler)
	EVT_MENU(idMenuNewFromTemplate, TemplateManager::OnNew)
END_EVENT_TABLE()

TemplateManager::TemplateManager()
{
	//ctor
	Manager::Get()->GetAppWindow()->PushEventHandler(this);
}

TemplateManager::~TemplateManager()
{
	//dtor
	Manager::Get()->GetAppWindow()->RemoveEventHandler(this);

	WX_CLEAR_ARRAY(m_Templates);
}

void TemplateManager::CreateMenu(wxMenuBar* menuBar)
{
}

void TemplateManager::ReleaseMenu(wxMenuBar* menuBar)
{
}

void TemplateManager::BuildToolsMenu(wxMenu* menu)
{
	if (menu)
		menu->Append(idMenuNewFromTemplate, _("&From template..."));
}

void TemplateManager::LoadTemplates()
{
    wxLogNull zero; // disable error logging

    wxString baseDir = ConfigManager::Get()->Read("/data_path");
	baseDir << "/templates";

    wxDir dir(baseDir);

    if (!dir.IsOpened())
        return;

	WX_CLEAR_ARRAY(m_Templates);
    wxString filename;
    bool ok = dir.GetFirst(&filename, "*.template", wxDIR_FILES);
    while (ok)
    {
        ProjectTemplateLoader* pt = new ProjectTemplateLoader();
		if (pt->Open(baseDir + "/" + filename))
			m_Templates.Add(pt);
		else
			delete pt;

        ok = dir.GetNext(&filename);
    }
	Manager::Get()->GetMessageManager()->DebugLog(_("%d templates loaded"), m_Templates.GetCount());
}

void TemplateManager::NewProject()
{
	LoadTemplates();
	NewFromTemplateDlg dlg(m_Templates);
	if (dlg.ShowModal() == wxID_OK)
	{
		ProjectTemplateLoader* pt = dlg.GetTemplate();
		if (!pt)
		{
			Manager::Get()->GetMessageManager()->DebugLog(_("Templates dialog returned OK but no template was selected ?!?"));
			return;
		}
		int optidx = dlg.GetOptionIndex();
		int filesetidx = dlg.GetFileSetIndex();
		TemplateOption& option = pt->m_TemplateOptions[optidx];
		FileSet& fileset = pt->m_FileSets[filesetidx];

		wxString baseDir = ConfigManager::Get()->Read("/data_path");
		wxFileDialog* fdlg = new wxFileDialog(0L,
								_("Save project"),
								wxEmptyString,
								pt->m_Name,
								CODEBLOCKS_FILES_FILTER,
								wxSAVE | wxHIDE_READONLY | wxOVERWRITE_PROMPT);
	
		if (fdlg->ShowModal() != wxID_OK)
			return;
		wxFileName fname;
		fname.Assign(fdlg->GetPath());
		wxString path = fname.GetPath(wxPATH_GET_VOLUME);
		wxString filename = fname.GetFullPath();
		
		baseDir << "/templates";
		wxCopyFile(baseDir + "/" + option.file, filename);

		cbProject* prj = Manager::Get()->GetProjectManager()->LoadProject(filename);
		if (prj)
		{
			for (unsigned int i = 0; i < fileset.files.GetCount(); ++i)
			{
				FileSetFile& fsf = fileset.files[i];
				wxString dst = path + "/" + fsf.destination;
				if (wxFileExists(dst))
				{
                    wxString msg;
                    msg.Printf(_("File %s already exists. Do you really want to overwrite this file?"), dst.c_str());
                    if (wxMessageBox(msg, _("Overwrite existing file?"), wxYES_NO | wxICON_QUESTION) != wxYES)
                        continue;
				}
				wxCopyFile(baseDir + "/" + fsf.source, dst);
				prj->AddFile(0, dst);
			}
		
			for (unsigned int i = 0; i < option.extraCFlags.GetCount(); ++i)
				prj->AddCompilerOption(option.extraCFlags[i]);
			for (unsigned int i = 0; i < option.extraLDFlags.GetCount(); ++i)
				prj->AddLinkerOption(option.extraLDFlags[i]);
				
			Manager::Get()->GetProjectManager()->RebuildTree();
		}
	}
}

// events

void TemplateManager::OnNew(wxCommandEvent& event)
{
	NewProject();
}
