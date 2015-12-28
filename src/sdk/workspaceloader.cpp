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
    #include <wx/confbase.h>
    #include <wx/fileconf.h>
    #include <wx/intl.h>
    #include <wx/string.h>

    #include "workspaceloader.h"

    #include "manager.h"
    #include "configmanager.h"
    #include "projectmanager.h"
    #include "logmanager.h"
    #include "cbproject.h"
    #include "globals.h"
    #include "cbworkspace.h"
    #include "editormanager.h"
    #include "cbauibook.h"
#endif



#include "annoyingdialog.h"
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinywxuni.h"

WorkspaceLoader::WorkspaceLoader()
{
    //ctor
}

WorkspaceLoader::~WorkspaceLoader()
{
    //dtor
}

inline ProjectManager* GetpMan() { return Manager::Get()->GetProjectManager(); }
inline LogManager* GetpMsg() { return Manager::Get()->GetLogManager(); }

#include <wx/intl.h>

bool WorkspaceLoader::Open(const wxString& filename, wxString& Title)
{
    TiXmlDocument doc;
    if (!TinyXML::LoadDocument(filename, &doc))
        return false;

//    ProjectManager* pMan = Manager::Get()->GetProjectManager();
//    LogManager* pMsg = Manager::Get()->GetLogManager();

    if (!GetpMan() || !GetpMsg())
        return false;

    // BUG: Race condition. to be fixed by Rick.
    // If I click close AFTER pMan and pMsg are calculated,
    // I get a segfault.
    // I modified classes projectmanager and logmanager,
    // so that when self==NULL, they do nothing
    // (constructors, destructors and static functions excempted from this)
    // This way, we'll use the *manager::Get() functions to check for nulls.

    TiXmlElement* root = doc.FirstChildElement("CodeBlocks_workspace_file");
    if (!root)
    {
        // old tag
        root = doc.FirstChildElement("Code::Blocks_workspace_file");
        if (!root)
        {
            GetpMsg()->DebugLog(_T("Not a valid Code::Blocks workspace file..."));
            return false;
        }
    }
    TiXmlElement* wksp = root->FirstChildElement("Workspace");
    if (!wksp)
    {
        GetpMsg()->DebugLog(_T("No 'Workspace' element in file..."));
        return false;
    }

    Title = cbC2U(wksp->Attribute("title")); // Conversion to unicode is automatic (see wxString::operator= )

    TiXmlElement* proj = wksp->FirstChildElement("Project");
    if (!proj)
    {
        GetpMsg()->DebugLog(_T("Workspace file contains no projects..."));
        return false;
    }

    // first loop to load projects
    while (proj)
    {
        if (Manager::IsAppShuttingDown() || !GetpMan() || !GetpMsg())
            return false;
        wxString projectFilename = UnixFilename(cbC2U(proj->Attribute("filename")));
        if (projectFilename.IsEmpty())
        {
            GetpMsg()->DebugLog(_T("'Project' node exists, but no filename?!?"));
        }
        else
        {
            wxFileName fname(projectFilename);
            wxFileName wfname(filename);
            fname.MakeAbsolute(wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
            cbProject* pProject = GetpMan()->LoadProject(fname.GetFullPath(), false); // don't activate it
            if (!pProject)
            {
                cbMessageBox(_("Unable to open ") + projectFilename,
                 _("Opening WorkSpace") + filename, wxICON_WARNING);
            }
        }
        proj = proj->NextSiblingElement("Project");
    }

    // second loop to setup dependencies
    proj = wksp->FirstChildElement("Project");
    while (proj)
    {
        cbProject* thisprj = nullptr;
        wxString projectFilename = UnixFilename(cbC2U(proj->Attribute("filename")));
        if (projectFilename.IsEmpty())
        {
            GetpMsg()->DebugLog(_T("'Project' node exists, but no filename?!?"));
            thisprj = nullptr;
        }
        else
        {
            wxFileName fname(projectFilename);
            wxFileName wfname(filename);
            fname.MakeAbsolute(wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
            thisprj = Manager::Get()->GetProjectManager()->IsOpen(fname.GetFullPath());
        }

        if (thisprj)
        {
            TiXmlElement* dep = proj->FirstChildElement("Depends");
            while (dep)
            {
                wxFileName fname( UnixFilename(cbC2U(dep->Attribute("filename"))) );
                wxFileName wfname(filename);
                fname.MakeAbsolute(wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
                cbProject* depprj = Manager::Get()->GetProjectManager()->IsOpen(fname.GetFullPath());
                if (depprj)
                    Manager::Get()->GetProjectManager()->AddProjectDependency(thisprj, depprj);
                dep = dep->NextSiblingElement("Depends");
            }
        }
        proj = proj->NextSiblingElement("Project");
    }

    return true;
}

bool WorkspaceLoader::Save(const wxString& title, const wxString& filename)
{
    const char* ROOT_TAG = "CodeBlocks_workspace_file";

    TiXmlDocument doc;
    doc.SetCondenseWhiteSpace(false);
    doc.InsertEndChild(TiXmlDeclaration("1.0", "UTF-8", "yes"));
    TiXmlElement* rootnode = static_cast<TiXmlElement*>(doc.InsertEndChild(TiXmlElement(ROOT_TAG)));
    if (!rootnode)
        return false;

    TiXmlElement* wksp = static_cast<TiXmlElement*>(rootnode->InsertEndChild(TiXmlElement("Workspace")));
    wksp->SetAttribute("title", cbU2C(title));

    ProjectsArray* arr = Manager::Get()->GetProjectManager()->GetProjects();
    for (unsigned int i = 0; i < arr->GetCount(); ++i)
    {
        cbProject* prj = arr->Item(i);

        wxFileName wfname(filename);
        wxFileName fname(prj->GetFilename());
        fname.MakeRelativeTo(wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));

        TiXmlElement* node = static_cast<TiXmlElement*>(wksp->InsertEndChild(TiXmlElement("Project")));
        node->SetAttribute("filename", cbU2C( UnixFilename(fname.GetFullPath(), wxPATH_UNIX) ) );

        const ProjectsArray* deps = Manager::Get()->GetProjectManager()->GetDependenciesForProject(prj);
        if (deps && deps->GetCount())
        {
            for (size_t j = 0; j < deps->GetCount(); ++j)
            {
                prj = deps->Item(j);
                fname.Assign(prj->GetFilename());
                fname.MakeRelativeTo(wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
                TiXmlElement* dnode = static_cast<TiXmlElement*>(node->InsertEndChild(TiXmlElement("Depends")));
                dnode->SetAttribute("filename", cbU2C( UnixFilename(fname.GetFullPath(), wxPATH_UNIX) ) );
            }
        }
    }
    return cbSaveTinyXMLDocument(&doc, filename);
}

bool WorkspaceLoader::SaveLayout(const wxString& filename)
{
    const char* ROOT_TAG = "CodeBlocks_workspace_layout_file";

    TiXmlDocument doc;
    doc.SetCondenseWhiteSpace(false);
    doc.InsertEndChild(TiXmlDeclaration("1.0", "UTF-8", "yes"));
    TiXmlElement* rootnode = static_cast<TiXmlElement*>(doc.InsertEndChild(TiXmlElement(ROOT_TAG)));
    if (!rootnode)
        return false; // Failed creating the root node of the workspace layout XML file?!

    rootnode->InsertEndChild(TiXmlElement("FileVersion"));
    rootnode->FirstChildElement("FileVersion")->SetAttribute("major", WORKSPACE_LAYOUT_FILE_VERSION_MAJOR);
    rootnode->FirstChildElement("FileVersion")->SetAttribute("minor", WORKSPACE_LAYOUT_FILE_VERSION_MINOR);

    // active project
    ProjectManager *pm = Manager::Get()->GetProjectManager();
    if (!pm)
        return false; // Could not access ProjectManager?!

    if (const cbProject *project = pm->GetActiveProject())
    {
        TiXmlElement *el =
            static_cast<TiXmlElement*>(
                rootnode->InsertEndChild( TiXmlElement("ActiveProject") ) );
        wxFileName wfname(filename);
        wxFileName fname( project->GetFilename() );
        fname.MakeRelativeTo(wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
        el->SetAttribute("path", cbU2C( UnixFilename(fname.GetFullPath(), wxPATH_UNIX) ) );
    }
    // else Workspace has no active project?!

    // preferred build target
    if (const cbWorkspace* wsp = pm->GetWorkspace() )
    {
        const wxString preferredTarget = wsp->GetPreferredTarget();
        if ( ! preferredTarget.IsEmpty() )
        {
            TiXmlElement* el =
                static_cast<TiXmlElement*>(
                    rootnode->InsertEndChild( TiXmlElement("PreferredTarget") ) );
            el->SetAttribute("name", cbU2C(preferredTarget) );
        }
        // else Project has not preferred target.
    }
    // else No workspace present to save.

    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/enable_editor_layout"), false))
    {
        TiXmlElement *el =
            static_cast<TiXmlElement*>(
                rootnode->InsertEndChild( TiXmlElement("EditorTabsLayout") ) );
        el->SetAttribute("layout", cbU2C( Manager::Get()->GetEditorManager()->GetNotebook()->SavePerspective() ));
    }
    // else ?!

    return cbSaveTinyXMLDocument(&doc, filename);
}

bool WorkspaceLoader::LoadLayout(const wxString& filename)
{
    TiXmlDocument doc;
    if ( ! TinyXML::LoadDocument(filename, &doc) )
        return false; // Can't load XML file?!

    if ( ! GetpMan() || ! GetpMsg() )
        return false; // GetpMan or GetpMsg returns NULL?!

    TiXmlElement* root = doc.FirstChildElement("CodeBlocks_workspace_layout_file");
    if (!root)
    {
        GetpMsg()->DebugLog(_T("Unable to load Code::Blocks workspace layout file: File is invalid."));
        return false;
    }

    int major = 0;
    int minor = 0;

    TiXmlElement* version = root->FirstChildElement("FileVersion");

    // don't show messages if we 're running a batch build (i.e. no gui)
    if (!Manager::IsBatchBuild() && version)
    {
        version->QueryIntAttribute("major", &major);
        version->QueryIntAttribute("minor", &minor);

        if (major >= WORKSPACE_LAYOUT_FILE_VERSION_MAJOR && minor > WORKSPACE_LAYOUT_FILE_VERSION_MINOR)
        {
            GetpMsg()->DebugLog(F(_T("Workspace layout file version is > %d.%d. Trying to load..."), WORKSPACE_LAYOUT_FILE_VERSION_MAJOR, WORKSPACE_LAYOUT_FILE_VERSION_MINOR));
            AnnoyingDialog dlg(_("Workspace layout file format is newer/unknown"),
                                F(_("This workspace layout file was saved with a newer version of Code::Blocks.\n"
                                "Will try to load, but you might see unexpected results.\n"
                                "In this case close the workspace, delete %s and reopen the workspace."),filename.wx_str()),
                                wxART_WARNING,
                                AnnoyingDialog::OK);
            dlg.ShowModal();
        }
        else
        {
            // use one message for all changes
            wxString msg;
            wxString warn_msg;

            if (major == 0 && minor == 0)
            {
                msg << _("0.0 (unversioned) to 1.0:\n");
                msg << _("  * save editor-pane layout and order.\n");
                msg << _("\n");
            }

            if (!msg.IsEmpty())
            {
                msg.Prepend(wxString::Format(_("Workspace layout file format is older (%d.%d) than the current format (%d.%d).\n"
                                                "The file will automatically be upgraded on close.\n"
                                                "But please read the following list of changes, as some of them\n"
                                                "might not automatically convert existing (old) settings.\n"
                                                "If you don't understand what a change means, you probably don't\n"
                                                "use that feature so you don't have to worry about it.\n\n"
                                                "List of changes:\n"),
                                            major,
                                            minor,
                                            WORKSPACE_LAYOUT_FILE_VERSION_MAJOR,
                                            WORKSPACE_LAYOUT_FILE_VERSION_MINOR));
                AnnoyingDialog dlg(_("Workspace layout file format changed"),
                                    msg,
                                    wxART_INFORMATION,
                                    AnnoyingDialog::OK);
                dlg.ShowModal();
            }

            if (!warn_msg.IsEmpty())
            {
                warn_msg.Prepend(_("!!! WARNING !!!\n\n"));
                AnnoyingDialog dlg(_("Workspace layout file upgrade warning"),
                                    warn_msg,
                                    wxART_WARNING,
                                    AnnoyingDialog::OK);
                dlg.ShowModal();
            }
        }
    }

    // active project
    if (TiXmlElement* el = root->FirstChildElement("ActiveProject"))
    {
        wxFileName fname = cbC2U( el->Attribute("path") );
        wxFileName wfname(filename);
        fname.MakeAbsolute( wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) );
        cbProject *project = GetpMan()->IsOpen( fname.GetFullPath() );
        if (project)
        {
            GetpMan()->SetProject(project);
            Manager::Get()->GetLogManager()->DebugLog(F(_T("Project %s has been activated."), fname.GetFullPath().wx_str()));
        }
        else
            Manager::Get()->GetLogManager()->DebugLog(F(_T("Could not activate project: %s"), fname.GetFullPath().wx_str()));
    }
    // else XML element 'ActiveProject' not found?!

    // preferred build target
    if (TiXmlElement* el = root->FirstChildElement("PreferredTarget"))
    {
        const wxString name = cbC2U(el->Attribute("name"));
        cbWorkspace *wsp = GetpMan()->GetWorkspace();
        if (wsp)
            wsp->SetPreferredTarget(name);
    }
    // else XML element 'PreferredTarget' not found?!

    if (   (major >= 1)
        && (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/enable_editor_layout"), false)) )
    {
        if (TiXmlElement* el = root->FirstChildElement("EditorTabsLayout"))
        {
            if (el->Attribute("layout"))
                Manager::Get()->GetEditorManager()->GetNotebook()->LoadPerspective(cbC2U(el->Attribute("layout")));
        }
    }

    return true;
}
