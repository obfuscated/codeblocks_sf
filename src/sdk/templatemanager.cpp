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

#include "sdk_precomp.h"
#include <wx/mdi.h>
#include <wx/intl.h>
#include <wx/dir.h>
#include <wx/menu.h>
#include <wx/log.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>

#include "templatemanager.h"
#include "manager.h"
#include "configmanager.h"
#include "messagemanager.h"
#include "projectmanager.h"
#include "cbproject.h"
#include "globals.h"
#include "managerproxy.h"
#include "compilerfactory.h"

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
    // this is a core manager, so it is removed when the app is shutting down.
    // in this case, the app has already un-hooked us, so no need to do it ourselves...
//	Manager::Get()->GetAppWindow()->RemoveEventHandler(this);

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

    wxString baseDir = ConfigManager::GetDataFolder();
	baseDir << _T("/templates");

    wxDir dir(baseDir);

    if (!dir.IsOpened())
        return;

	WX_CLEAR_ARRAY(m_Templates);
    wxString filename;
    bool ok = dir.GetFirst(&filename, _T("*.template"), wxDIR_FILES);
    while (ok)
    {
        ProjectTemplateLoader* pt = new ProjectTemplateLoader();
		if (pt->Open(baseDir + _T("/") + filename))
			m_Templates.Add(pt);
		else
			delete pt;

        ok = dir.GetNext(&filename);
    }
	Manager::Get()->GetMessageManager()->DebugLog(_("%d templates loaded"), m_Templates.GetCount());
}

void TemplateManager::LoadUserTemplates()
{
    wxLogNull zero; // disable error logging

    m_UserTemplates.Clear();
    wxString baseDir = wxGetHomeDir();
	baseDir << _T("/.CodeBlocks");

    wxDir dir(baseDir);

    if (!dir.IsOpened())
        return;

    wxString filename;
    bool ok = dir.GetFirst(&filename, _T("*"), wxDIR_DIRS);
    while (ok)
    {
        m_UserTemplates.Add(filename);
        ok = dir.GetNext(&filename);
    }

	Manager::Get()->GetMessageManager()->DebugLog(_("%d user templates loaded"), m_UserTemplates.GetCount());
}

cbProject* TemplateManager::NewProject()
{
	cbProject* prj = NULL;
	// one-time warning message
    if (Manager::Get()->GetConfigManager(_T("template_manager"))->ReadBool(_T("/notification"), true))
    {
    	wxMessageBox(_("These templates are only provided for your convenience.\n"
                        "Many of the available templates need extra libraries "
                        "in order to be compiled succesfuly.\n\n"
                        "Extra libraries which Code::Blocks does *NOT* provide..."),
                    _("One-time information"),
                    wxICON_INFORMATION);
    	// don't warn the user again
        Manager::Get()->GetConfigManager(_T("template_manager"))->Write(_T("/notification"), false);
    }

	LoadTemplates();
	LoadUserTemplates();
	NewFromTemplateDlg dlg(m_Templates, m_UserTemplates);
	if (dlg.ShowModal() == wxID_OK)
	{
        if (dlg.SelectedUserTemplate())
            prj = NewProjectFromUserTemplate(dlg);
        else
            prj = NewProjectFromTemplate(dlg);
	}
	return prj;
}

cbProject* TemplateManager::NewProjectFromTemplate(NewFromTemplateDlg& dlg)
{
	cbProject* prj = NULL;
	// is it a wizard or a template?
    cbProjectWizardPlugin* wiz = dlg.GetWizard();
	if (wiz)
	{
		// wizard, too easy ;)
		wiz->Launch(dlg.GetWizardIndex());
        // TODO (rickg22#1#): Mandrav: Please add some way to return the project from the wizard
        //                             so the project can be added to the history
		return NULL;
    }

	// else it's a template
    ProjectTemplateLoader* pt = dlg.GetTemplate();
    if (!pt)
    {
        Manager::Get()->GetMessageManager()->DebugLog(_("Templates dialog returned OK but no template was selected ?!?"));
        return NULL;
    }
    int optidx = dlg.GetOptionIndex();
    int filesetidx = dlg.GetFileSetIndex();
    TemplateOption& option = pt->m_TemplateOptions[optidx];
    FileSet& fileset = pt->m_FileSets[filesetidx];

    if (!wxDirExists(dlg.GetProjectPath() + wxFILE_SEP_PATH))
    {
        if (wxMessageBox(wxString::Format(_("The directory %s does not exist. Are you sure you want to create it?"), dlg.GetProjectPath().c_str()), _("Confirmation"), wxICON_QUESTION | wxYES_NO) != wxYES)
            return NULL;
    }
    if (wxDirExists(dlg.GetProjectPath() + wxFILE_SEP_PATH + dlg.GetProjectName() + wxFILE_SEP_PATH))
    {
        if (wxMessageBox(wxString::Format(_("The directory %s already exists. Are you sure you want to create the new project there?"), wxString(dlg.GetProjectPath() + wxFILE_SEP_PATH + dlg.GetProjectName()).c_str()), _("Confirmation"), wxICON_QUESTION | wxYES_NO) != wxYES)
            return NULL;
    }

    wxFileName fname;
    fname.Assign(dlg.GetProjectPath() + wxFILE_SEP_PATH +
                dlg.GetProjectName() + wxFILE_SEP_PATH +
                dlg.GetProjectName() + _T(".") + CODEBLOCKS_EXT);
    LOGSTREAM << _T("Creating ") << fname.GetPath() << _T('\n');
    if (!CreateDirRecursively(fname.GetPath() + wxFILE_SEP_PATH))
    {
        wxMessageBox(_("Failed to create directory ") + fname.GetPath(), _("Error"), wxICON_ERROR);
        return NULL;
    }

    if (dlg.GetProjectPath() != Manager::Get()->GetConfigManager(_T("template_manager"))->Read(_T("/projects_path")))
    {
        if (wxMessageBox(wxString::Format(_("Do you want to set %s as the default directory for new projects?"), dlg.GetProjectPath().c_str()), _("Question"), wxICON_QUESTION | wxYES_NO) == wxYES)
            Manager::Get()->GetConfigManager(_T("template_manager"))->Write(_T("/projects_path"), dlg.GetProjectPath());
    }

    wxString path = fname.GetPath(wxPATH_GET_VOLUME);
    wxString filename = fname.GetFullPath();
    wxString sep = wxFILE_SEP_PATH;

    wxString baseDir = ConfigManager::GetDataFolder();
    baseDir << sep << _T("templates");
    wxCopyFile(baseDir + sep + option.file, filename);

    prj = Manager::Get()->GetProjectManager()->LoadProject(filename);
    if (prj)
    {
        prj->SetTitle(dlg.GetProjectName());
        if (option.useDefaultCompiler)
        {
            // we must update the project (and the targets) to use the default compiler
            int compilerIdx = CompilerFactory::GetDefaultCompilerIndex();
            prj->SetCompilerIndex(compilerIdx);
            for (int i = 0; i < prj->GetBuildTargetsCount(); ++i)
            {
                ProjectBuildTarget* bt = prj->GetBuildTarget(i);
                bt->SetCompilerIndex(compilerIdx);
            }
        }

        if (!dlg.DoNotCreateFiles())
        {
            for (unsigned int i = 0; i < fileset.files.GetCount(); ++i)
            {
                FileSetFile& fsf = fileset.files[i];
                wxString dst = path + sep + fsf.destination;
                bool skipped = false;
                while (wxFileExists(dst))
                {
                    wxString msg;
                    msg.Printf(_("File %s already exists.\nDo you really want to overwrite this file?"), dst.c_str());
                    if (wxMessageBox(msg, _("Overwrite existing file?"), wxYES_NO | wxICON_WARNING) == wxYES)
                        break;
                    wxFileDialog fdlg(0L,
                                        _("Save file as..."),
                                        wxEmptyString,
                                        dst,
                                        SOURCE_FILES_FILTER,
                                        wxSAVE);
                    if (fdlg.ShowModal() == wxID_CANCEL)
                    {
                        msg.Printf(_("File %s is skipped..."), dst.c_str());
                        wxMessageBox(msg, _("File skipped"), wxICON_ERROR);
                        skipped = true;
                        break;
                    }
                    dst = fdlg.GetPath();
                }
                if (skipped)
                    continue;
                wxCopyFile(baseDir + sep + fsf.source, dst);
                for (int i = 0; i < prj->GetBuildTargetsCount(); ++i)
                    prj->AddFile(i, fsf.destination);
            }
        }

        for (unsigned int i = 0; i < option.extraCFlags.GetCount(); ++i)
            prj->AddCompilerOption(option.extraCFlags[i]);
        for (unsigned int i = 0; i < option.extraLDFlags.GetCount(); ++i)
            prj->AddLinkerOption(option.extraLDFlags[i]);

        Manager::Get()->GetProjectManager()->RebuildTree();

        if (!pt->m_Notice.IsEmpty())
            wxMessageBox(pt->m_Notice, _("Notice"), pt->m_NoticeMsgType);
        if (!option.notice.IsEmpty())
            wxMessageBox(option.notice, _("Notice"), option.noticeMsgType);
    }
    return prj;
}

cbProject* TemplateManager::NewProjectFromUserTemplate(NewFromTemplateDlg& dlg)
{
    cbProject* prj = NULL;
    if (!dlg.SelectedUserTemplate())
    {
        Manager::Get()->GetMessageManager()->DebugLog(_("TemplateManager::NewProjectFromUserTemplate() called when no user template was selected ?!?"));
        return NULL;
    }

    // select directory to copy user template files
    wxString sep = wxFileName::GetPathSeparator();
    wxString path = ChooseDirectory(0, _("Choose a directory to create the new project"));
    if (path.IsEmpty())
        return NULL;

    wxBusyCursor busy;

    wxString templ = wxGetHomeDir();
    templ << sep << _T(".CodeBlocks") << sep << dlg.GetSelectedUserTemplate();
    if (!wxDirExists(templ))
    {
        Manager::Get()->GetMessageManager()->DebugLog(_("Cannot open user-template source path '%s'!"), templ.c_str());
        return NULL;
    }

    // copy files
    wxString project_filename;
    wxArrayString files;
    wxDir::GetAllFiles(templ, &files);
    int count = 0;
    int total_count = files.GetCount();
    for (size_t i = 0; i < files.GetCount(); ++i)
    {
        wxFileName dstname(files[i]);
        dstname.MakeRelativeTo(templ + sep);
        wxString src = files[i];
        wxString dst = path + sep + dstname.GetFullPath();
//        Manager::Get()->GetMessageManager()->DebugLog("dst=%s, dstname=%s", dst.c_str(), dstname.GetFullPath().c_str());
        if (!CreateDirRecursively(dst))
            Manager::Get()->GetMessageManager()->DebugLog(_("Failed creating directory for %s"), dst.c_str());
        if (wxCopyFile(src, dst, true))
        {
            if (FileTypeOf(dst) == ftCodeBlocksProject)
                project_filename = dst;
            ++count;
        }
        else
            Manager::Get()->GetMessageManager()->DebugLog(_("Failed copying %s to %s"), src.c_str(), dst.c_str());
    }
    if (count != total_count)
        wxMessageBox(_("Some files could not be loaded with the template..."), _("Error"), wxICON_ERROR);
    else
    {
        // open new project
        if (project_filename.IsEmpty())
            wxMessageBox(_("User-template saved succesfuly but no project file exists in it!"));
        else
        {
        	// ask to rename the project file, if need be
        	wxFileName fname(project_filename);
        	wxString newname = wxGetTextFromUser(_("If you want, you can change the project's filename here (without extension):"), _("Change project's filename"), fname.GetName());
        	if (!newname.IsEmpty() && newname != fname.GetName())
        	{
        		fname.SetName(newname);
        		wxRenameFile(project_filename, fname.GetFullPath());
        		project_filename = fname.GetFullPath();
        	}
            prj = Manager::Get()->GetProjectManager()->LoadProject(project_filename);
        }
    }
    return prj;
}

void TemplateManager::SaveUserTemplate(cbProject* prj)
{
    if (!prj)
        return;

    // save project & all files
    if (!prj->SaveAllFiles() ||
        !prj->Save())
    {
        wxMessageBox(_("Could not save project and/or all its files. Aborting..."), _("Error"), wxICON_ERROR);
        return;
    }

    // create destination dir
    wxString templ = wxGetHomeDir();
    templ << _T("/.CodeBlocks");
    if (!wxDirExists(templ))
        wxMkdir(templ, 0755);

    // check if it exists and ask a different title
    wxString title = prj->GetTitle();
    while (true)
    {
        // ask for template title (unique)
        wxTextEntryDialog dlg(0, _("Enter a title for this template"), _("Enter title"), title);
        if (dlg.ShowModal() != wxID_OK)
            return;

        title = dlg.GetValue();
        if (!wxDirExists(templ + _T("/") + title))
        {
            templ << _T("/") << title;
            wxMkdir(templ, 0755);
            break;
        }
        else
            wxMessageBox(_("You have another template with the same title.\nPlease choose another title..."));
    }

    wxBusyCursor busy;

    // copy project and all files to destination dir
    int count = 0;
    int total_count = prj->GetFilesCount();
    templ << _T("/");
    wxFileName fname;
    for (int i = 0; i < total_count; ++i)
    {
        wxString src = prj->GetFile(i)->file.GetFullPath();
        wxString dst = templ + prj->GetFile(i)->relativeFilename;
//        Manager::Get()->GetMessageManager()->DebugLog(_("Copying %s to %s"), src.c_str(), dst.c_str());
        if (!CreateDirRecursively(dst))
            Manager::Get()->GetMessageManager()->DebugLog(_("Failed creating directory for %s"), dst.c_str());
        if (wxCopyFile(src, dst, true))
            ++count;
        else
            Manager::Get()->GetMessageManager()->DebugLog(_("Failed copying %s to %s"), src.c_str(), dst.c_str());
    }
    fname.Assign(prj->GetFilename());
    if (!wxCopyFile(prj->GetFilename(), templ + fname.GetFullName()))
        wxMessageBox(_("Failed to copy the project file!"), _("Error"), wxICON_ERROR);

    if (count == total_count)
        wxMessageBox(_("User-template saved succesfuly"));
    else
        wxMessageBox(_("Some files could not be saved with the template..."), _("Error"), wxICON_ERROR);
}

// events

void TemplateManager::OnNew(wxCommandEvent& event)
{
	NewProject();
}
