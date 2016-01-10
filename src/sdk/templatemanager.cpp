/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/menu.h>
    #include <wx/filename.h>
    #include <wx/msgdlg.h>
    #include <wx/dir.h>
    #include <wx/textdlg.h>

    #include "templatemanager.h"
    #include "manager.h"
    #include "configmanager.h"
    #include "logmanager.h"
    #include "projectmanager.h"
    #include "cbproject.h"
    #include "globals.h"
    #include "compilerfactory.h"
    #include "cbplugin.h"
    #include "cbproject.h"
#endif

#include <wx/filedlg.h>
#include "filefilters.h"
#include "newfromtemplatedlg.h"

template<> TemplateManager* Mgr<TemplateManager>::instance = nullptr;
template<> bool  Mgr<TemplateManager>::isShutdown = false;

TemplateManager::TemplateManager()
{
    //ctor
    Manager::Get()->GetAppWindow()->PushEventHandler(this);
}

TemplateManager::~TemplateManager()
{
    //dtor
}

void TemplateManager::CreateMenu(cb_unused wxMenuBar* menuBar)
{
}

void TemplateManager::ReleaseMenu(cb_unused wxMenuBar* menuBar)
{
}

void TemplateManager::BuildToolsMenu(cb_unused wxMenu* menu)
{
}

void TemplateManager::LoadUserTemplates()
{
    m_UserTemplates.Clear();
    wxString baseDir = ConfigManager::GetConfigFolder() + wxFILE_SEP_PATH + _T("UserTemplates");

    if (!wxDirExists(baseDir)) // avoid warnings in debug builds
        return;

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

    Manager::Get()->GetLogManager()->DebugLog(F(_T("%d user templates loaded"), m_UserTemplates.GetCount()));
}

cbProject* TemplateManager::New(TemplateOutputType initial, wxString* pFilename)
{
    cbProject* prj = NULL;

    LoadUserTemplates();
    NewFromTemplateDlg dlg(initial, m_UserTemplates);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        if (dlg.SelectedUserTemplate())
            prj = NewProjectFromUserTemplate(dlg, pFilename);
        else
            prj = NewFromTemplate(dlg, pFilename);
    }
    return prj;
}

cbProject* TemplateManager::NewFromTemplate(NewFromTemplateDlg& dlg, wxString* pFilename)
{
    cbProject* prj = NULL;
    cbWizardPlugin* wiz = dlg.GetWizard();
    if (wiz)
    {
        // wizard, too easy ;)
        CompileTargetBase* ret = wiz->Launch(dlg.GetWizardIndex(), pFilename);
        switch (wiz->GetOutputType(dlg.GetWizardIndex()))
        {
            case totProject: prj = dynamic_cast<cbProject*>(ret); break;
            case totTarget: // fall-though
            case totFiles:  // fall-though
            case totCustom: // fall-though
            case totUser:   // fall-though
            default: break;
        }
    }
    return prj;
}

cbProject* TemplateManager::NewProjectFromUserTemplate(NewFromTemplateDlg& dlg, wxString* pFilename)
{
    cbProject* prj = NULL;
    if (!dlg.SelectedUserTemplate())
    {
        Manager::Get()->GetLogManager()->DebugLog(_T("TemplateManager::NewProjectFromUserTemplate() called when no user template was selected ?!?"));
        return NULL;
    }

    wxString path = Manager::Get()->GetConfigManager(_T("template_manager"))->Read(_T("/projects_path"));
    wxString sep = wxFileName::GetPathSeparator();
    // select directory to copy user template files
    path = ChooseDirectory(nullptr, _("Choose a directory to create the new project"),
                        path, _T(""), false, true);
    if (path.IsEmpty())
        return NULL;
    else if (path.Mid(path.Length() - 1) == wxFILE_SEP_PATH)
        path.RemoveLast();

    // check for existing files; if found, notify about overwriting them
    if (wxDirExists(path))
    {
        wxDir dir(path);
        if (dir.HasFiles() || dir.HasSubDirs())
        {
            if (cbMessageBox(path + _(" already contains other files.\n"
                                      "If you continue, files with the same names WILL BE OVERWRITTEN.\n"
                                      "Are you sure you want to continue?"),
                                    _("Files exist in directory"), wxICON_EXCLAMATION | wxYES_NO | wxNO_DEFAULT) != wxID_YES)
            {
                return nullptr;
            }
        }
    }

    wxBusyCursor busy;

    wxString templ = ConfigManager::GetConfigFolder() + wxFILE_SEP_PATH + _T("UserTemplates");
    templ << sep << dlg.GetSelectedUserTemplate();
    if (!wxDirExists(templ))
    {
        Manager::Get()->GetLogManager()->DebugLog(F(_T("Cannot open user-template source path '%s'!"), templ.wx_str()));
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
//        Manager::Get()->GetLogManager()->DebugLog("dst=%s, dstname=%s", dst.c_str(), dstname.GetFullPath().c_str());
        if (!CreateDirRecursively(dst))
            Manager::Get()->GetLogManager()->DebugLog(_T("Failed creating directory for ") + dst);
        if (wxCopyFile(src, dst, true))
        {
            if (FileTypeOf(dst) == ftCodeBlocksProject)
                project_filename = dst;
            ++count;
        }
        else
            #if wxCHECK_VERSION(2, 9, 0)
            Manager::Get()->GetLogManager()->DebugLog(F(_T("Failed copying %s to %s"), src.wx_str(), dst.wx_str()));
            #else
            Manager::Get()->GetLogManager()->DebugLog(F(_T("Failed copying %s to %s"), src.c_str(), dst.c_str()));
            #endif
    }
    if (count != total_count)
        cbMessageBox(_("Some files could not be loaded with the template..."), _("Error"), wxICON_ERROR);
    else
    {
        // open new project
        if (project_filename.IsEmpty())
            cbMessageBox(_("User-template saved successfully but no project file exists in it!"));
        else
        {
            // ask to rename the project file, if need be
            wxFileName fname(project_filename);
            wxString newname = cbGetTextFromUser(_("If you want, you can change the project's filename here (without extension):"), _("Change project's filename"), fname.GetName());
            if (!newname.IsEmpty() && newname != fname.GetName())
            {
                fname.SetName(newname);
                wxRenameFile(project_filename, fname.GetFullPath());
                project_filename = fname.GetFullPath();
            }
            prj = Manager::Get()->GetProjectManager()->LoadProject(project_filename);
            if (prj && !newname.IsEmpty())
            {
                prj->SetTitle(newname);
                for (int i = 0; i < prj->GetBuildTargetsCount(); ++i)
                {
                    ProjectBuildTarget* bt = prj->GetBuildTarget(i);
                    TargetFilenameGenerationPolicy tgfpPrefix, tgfpExtension;
                    bt->GetTargetFilenameGenerationPolicy(tgfpPrefix, tgfpExtension);
                    wxString outputFileName = bt->GetOutputFilename();
                    wxFileName outFname(outputFileName);
                    if (tgfpPrefix == tgfpPlatformDefault && bt->GetTargetType() == ttStaticLib)
                    {
                        Compiler* projectCompiler = CompilerFactory::GetCompiler(bt->GetCompilerID());
                        if (projectCompiler)
                            newname.Prepend(projectCompiler->GetSwitches().libPrefix);
                    }
                    outFname.SetName(newname);
                    bt->SetOutputFilename(outFname.GetFullPath());
                }
                Manager::Get()->GetProjectManager()->GetUI().RebuildTree(); // so the tree shows the new name
                CodeBlocksEvent evt(cbEVT_PROJECT_OPEN, 0, prj);
                Manager::Get()->ProcessEvent(evt);
            }
        }
    }
    if (prj && pFilename)
        *pFilename = prj->GetFilename();
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
        cbMessageBox(_("Could not save project and/or all its files. Aborting..."), _("Error"), wxICON_ERROR);
        return;
    }

    // create destination dir
    wxString templ = ConfigManager::GetConfigFolder() + wxFILE_SEP_PATH + _T("UserTemplates");
    if (!CreateDirRecursively(templ, 0755))
    {
        cbMessageBox(_("Couldn't create directory for user templates:\n") + templ, _("Error"), wxICON_ERROR);
        return;
    }

    // check if it exists and ask a different title
    wxString title = prj->GetTitle();
    while (true)
    {
        // ask for template title (unique)
        wxTextEntryDialog dlg(nullptr, _("Enter a title for this template"), _("Enter title"), title);
        PlaceWindow(&dlg);
        if (dlg.ShowModal() != wxID_OK)
            return;

        title = dlg.GetValue();
        if (!wxDirExists(templ + wxFILE_SEP_PATH + title))
        {
            templ << wxFILE_SEP_PATH << title;
            wxMkdir(templ, 0755);
            break;
        }
        else
            cbMessageBox(_("You have another template with the same title.\nPlease choose another title..."));
    }

    wxBusyCursor busy;

    // copy project and all files to destination dir
    int count = 0;
    int total_count = prj->GetFilesCount();
    templ << wxFILE_SEP_PATH;
    wxFileName fname;

    for (FilesList::iterator it = prj->GetFilesList().begin(); it != prj->GetFilesList().end(); ++it)
    {
        wxString src = (*it)->file.GetFullPath();
        wxString dst = templ + (*it)->relativeToCommonTopLevelPath;
        #if wxCHECK_VERSION(2, 9, 0)
        Manager::Get()->GetLogManager()->DebugLog(F(_T("Copying %s to %s"), src.wx_str(), dst.wx_str()));
        #else
        Manager::Get()->GetLogManager()->DebugLog(F(_T("Copying %s to %s"), src.c_str(), dst.c_str()));
        #endif
        if (!CreateDirRecursively(dst))
            Manager::Get()->GetLogManager()->DebugLog(_T("Failed creating directory for ") + dst);
        if (wxCopyFile(src, dst, true))
            ++count;
        else
            #if wxCHECK_VERSION(2, 9, 0)
            Manager::Get()->GetLogManager()->DebugLog(F(_T("Failed copying %s to %s"), src.wx_str(), dst.wx_str()));
            #else
            Manager::Get()->GetLogManager()->DebugLog(F(_T("Failed copying %s to %s"), src.c_str(), dst.c_str()));
            #endif
    }

    // cbProject doesn't have a GetRelativeToCommonTopLevelPath() function, so we simulate it here
    // to find out the real destination file to create...
    wxString topLevelPath = prj->GetCommonTopLevelPath();
    fname.Assign(prj->GetFilename());
    fname.MakeRelativeTo(topLevelPath);
    fname.Assign(templ + fname.GetFullPath());
    if (!CreateDirRecursively(fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR), 0755))
    {
        cbMessageBox(_("Failed to create the directory for the project file!"), _("Error"), wxICON_ERROR);
        ++count;
    }
    else
    {
        if (!wxCopyFile(prj->GetFilename(), fname.GetFullPath()))
        {
            Manager::Get()->GetLogManager()->DebugLog(_T("Failed to copy the project file: ") + fname.GetFullPath());
            cbMessageBox(_("Failed to copy the project file!"), _("Error"), wxICON_ERROR);
            ++count;
        }
    }

    if (count == total_count)
        cbMessageBox(_("User-template saved successfully"), _("Information"), wxICON_INFORMATION | wxOK);
    else
        cbMessageBox(_("Some files could not be saved with the template..."), _("Error"), wxICON_ERROR);
}
