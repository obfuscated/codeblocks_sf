/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"

#ifndef CB_PRECOMP
    #include <wx/filename.h>
    #include <wx/intl.h>
    #include <wx/utils.h>
    #include <wx/filename.h>
    #include <wx/fs_zip.h>
    #include <wx/menu.h>
    #include <wx/xrc/xmlres.h>

    #include "globals.h"
    #include "manager.h"
    #include "logmanager.h"
    #include "projectmanager.h"
    #include "logmanager.h"
    #include "compilerfactory.h"
    #include "cbproject.h"
    #include "cbworkspace.h"
#endif

#include "projectsimporter.h"
#include "devcpploader.h"
#include "msvc7loader.h"
#include "msvc10loader.h"
#include "msvc7workspaceloader.h"
#include "msvcloader.h"
#include "msvcworkspaceloader.h"
#include "importers_globals.h"
#include "filefilters.h"

// this auto-registers the plugin
namespace
{
    PluginRegistrant<ProjectsImporter> reg(_T("ProjectsImporter"));
}

ProjectsImporter::ProjectsImporter()
{
    //ctor
    if (!Manager::LoadResource(_T("projectsimporter.zip")))
        NotifyMissingFile(_T("projectsimporter.zip"));
}

ProjectsImporter::~ProjectsImporter()
{
    //dtor
}

void ProjectsImporter::OnAttach()
{
}


void ProjectsImporter::OnRelease(bool appShutDown)
{
}

int ProjectsImporter::Configure()
{
    return 0;
}

cbConfigurationPanel* ProjectsImporter::GetConfigurationPanel(wxWindow* parent)
{
    return 0;
}

void ProjectsImporter::BuildMenu(wxMenuBar* menuBar)
{
    if (!IsAttached() || !menuBar)
        return;

    m_Menu = Manager::Get()->LoadMenu(_T("project_import_menu"), false);
    if (!m_Menu)
        return;

    wxMenu* fileMenu = menuBar->GetMenu(0);
    if (fileMenu)
    {
        int menuId = 0, id = 0;
        wxMenuItemList menuItems = fileMenu->GetMenuItems();
        menuId = fileMenu->FindItem(_T("R&ecent files"));
        wxMenuItem* recentFileItem = fileMenu->FindItem(menuId);
        id = menuItems.IndexOf(recentFileItem);
        if (id == wxNOT_FOUND)
            id = 7;
        else
            ++id;

        // The position is hard-coded to "Recent Files" menu. Please adjust it if necessary
        fileMenu->Insert(++id, wxNewId(), _("&Import project"), m_Menu);
        fileMenu->InsertSeparator(++id);
    }
}

bool ProjectsImporter::CanHandleFile(const wxString& filename) const
{
    const FileType ft = FileTypeOf(filename);
    return (   ft == ftDevCppProject
            || ft == ftMSVC6Project
            || ft == ftMSVC6Workspace
            || ft == ftMSVC7Project
            || ft == ftMSVC7Workspace
            || ft == ftMSVC10Project );
}

int ProjectsImporter::OpenFile(const wxString& filename)
{
    const FileType ft = FileTypeOf(filename);
    switch (ft)
    {
        case ftDevCppProject: // fallthrough
        case ftMSVC6Project:  // fallthrough
        case ftMSVC7Project:  // fallthrough
        case ftMSVC10Project: // fallthrough
        case ftXcode1Project: // fallthrough
        case ftXcode2Project: // fallthrough
            return LoadProject(filename);
            break;

        case ftMSVC6Workspace: // fallthrough
        case ftMSVC7Workspace: // fallthrough
            return LoadWorkspace(filename);
            break;

        default:
            cbMessageBox(_("Failed to import file: unsupported"), _("Error"), wxICON_ERROR);
            return -1;
    }
    return -1;
}

int ProjectsImporter::LoadProject(const wxString& filename)
{
    wxFileName the_file(filename);
    if (!the_file.FileExists())
        return -1;
    the_file.SetExt(FileFilters::CODEBLOCKS_EXT);

    cbProject* prj = Manager::Get()->GetProjectManager()->NewProject(the_file.GetFullPath());
    if (!prj)
        return -1;

    // Make a check whether the project exists in current workspace
    if (Manager::Get()->GetProjectManager()->BeginLoadingProject())
    {
        wxBusyCursor wait;

        #if wxCHECK_VERSION(2, 9, 0)
        Manager::Get()->GetLogManager()->Log(F(_("Importing %s: "), filename.wx_str()));
        #else
        Manager::Get()->GetLogManager()->Log(F(_("Importing %s: "), filename.c_str()));
        #endif
        IBaseLoader* loader = 0L;
        FileType ft = FileTypeOf(filename);
        switch (ft)
        {
            case ftDevCppProject:
                loader = new DevCppLoader(prj); break;
            case ftMSVC6Project:
                loader = new MSVCLoader(prj); break;
            case ftMSVC7Project:
                loader = new MSVC7Loader(prj); break;
            case ftMSVC10Project:
                loader = new MSVC10Loader(prj); break;
            case ftXcode1Project: /* placeholder, fallthrough (for now) */
            case ftXcode2Project: /* placeholder, fallthrough (for now) */
            default:
                Manager::Get()->GetProjectManager()->EndLoadingProject(0);
                cbMessageBox(_("Failed to import file: File type not supported."), _("Error"), wxICON_ERROR);
                return -1;
        }

        wxString compilerID;
        if (ImportersGlobals::UseDefaultCompiler)
            compilerID = CompilerFactory::GetDefaultCompilerID();
        else
        {
            // select compiler for the imported project
            // need to do it before actual import, because the importer might need
            // project's compiler information (like the object files extension etc).
            Compiler* compiler = CompilerFactory::SelectCompilerUI(_("Select compiler for ") + wxFileName(filename).GetFullName());
            if (!compiler) // User hit Cancel.
            {
                if (loader) delete loader;
                Manager::Get()->GetProjectManager()->EndLoadingProject(0);
                Manager::Get()->GetProjectManager()->CloseProject(prj, true, false);
                Manager::Get()->GetProjectManager()->RebuildTree();
                cbMessageBox(_("Import canceled."), _("Information"), wxICON_INFORMATION);
                return -1;
            }

            compilerID = compiler->GetID();
            if (compilerID.IsEmpty())
                compilerID = CompilerFactory::GetDefaultCompilerID();
        }

        prj->SetCompilerID(compilerID);

        // actually import project file
        if (loader->Open(filename))
        {
            prj->CalculateCommonTopLevelPath();
            prj->Save(); // Save it now to avoid project file opening error

            Manager::Get()->GetProjectManager()->EndLoadingProject(prj);
            if (!Manager::Get()->GetProjectManager()->IsLoadingWorkspace())
                Manager::Get()->GetProjectManager()->SetProject(prj);
        }
        else
        {
            if (loader) delete loader;
            Manager::Get()->GetProjectManager()->EndLoadingProject(0);
            Manager::Get()->GetProjectManager()->CloseProject(prj, true, false);
            Manager::Get()->GetProjectManager()->RebuildTree();
            cbMessageBox(_("Failed to import file: Wrong file format."), _("Error"), wxICON_ERROR);
            return -1;
        }

        if (loader) delete loader;
        Manager::Get()->GetProjectManager()->EndLoadingProject(prj);
        return prj ? 0 : -1;
    }

    cbMessageBox(_("Failed to import file (internal error)."), _("Error"), wxICON_ERROR);
    Manager::Get()->GetProjectManager()->EndLoadingProject(0);
    Manager::Get()->GetProjectManager()->CloseProject(prj, true, false);
    Manager::Get()->GetProjectManager()->RebuildTree();
    return -1;
}

int ProjectsImporter::LoadWorkspace(const wxString& filename)
{
    wxFileName the_file(filename);
    if (!the_file.FileExists())
        return -1;

    wxBusyCursor wait;

    if (!Manager::Get()->GetProjectManager()->BeginLoadingWorkspace())
        return -1;

    cbWorkspace* wksp = Manager::Get()->GetProjectManager()->GetWorkspace();
    if (!wksp)
    {
        Manager::Get()->GetProjectManager()->EndLoadingWorkspace();
        return -1;
    }

    #if wxCHECK_VERSION(2, 9, 0)
    Manager::Get()->GetLogManager()->Log(F(_("Importing %s: "), filename.wx_str()));
    #else
    Manager::Get()->GetLogManager()->Log(F(_("Importing %s: "), filename.c_str()));
    #endif
    FileType ft = FileTypeOf(filename);
    IBaseWorkspaceLoader* pWsp = 0;
    switch (ft)
    {
        case ftMSVC6Workspace:
            pWsp = new MSVCWorkspaceLoader; break;
        case ftMSVC7Workspace:
            pWsp = new MSVC7WorkspaceLoader; break;
        default:
            break;
    }

    if (!pWsp)
    {
        cbMessageBox(_("Failed to import file: unsupported"), _("Error"), wxICON_ERROR);
        Manager::Get()->GetProjectManager()->EndLoadingWorkspace();
        return -1;
    }

    wxString Title;
    if (pWsp->Open(filename, Title))
    {
        if (!Title.IsEmpty())
            wksp->SetTitle(Title);

        wksp->SetModified(true);
    }
    else
        cbMessageBox(_("Failed to import *any* projects from workspace file."), _("Error"), wxICON_ERROR);

    delete pWsp;
    Manager::Get()->GetProjectManager()->EndLoadingWorkspace();

    return 0;
}
