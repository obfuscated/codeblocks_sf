#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/confbase.h>
    #include <wx/fileconf.h>
    #include <wx/intl.h>
    #include <wx/log.h>

    #include "manager.h"
    #include "projectmanager.h"
    #include "messagemanager.h"
    #include "cbproject.h"
    #include "workspaceloader.h"
#endif



#include "tinyxml/tinyxml.h"

WorkspaceLoader::WorkspaceLoader()
{
	//ctor
}

WorkspaceLoader::~WorkspaceLoader()
{
	//dtor
}

inline ProjectManager* GetpMan() { return Manager::Get()->GetProjectManager(); }
inline MessageManager* GetpMsg() { return Manager::Get()->GetMessageManager(); }

bool WorkspaceLoader::Open(const wxString& filename)
{
    TiXmlDocument doc(filename.mb_str());
    if (!doc.LoadFile())
        return false;

//    ProjectManager* pMan = Manager::Get()->GetProjectManager();
//    MessageManager* pMsg = Manager::Get()->GetMessageManager();

    if (!GetpMan() || !GetpMsg())
        return false;

    // BUG: Race condition. to be fixed by Rick.
    // If I click close AFTER pMan and pMsg are calculated,
    // I get a segfault.
    // I modified classes projectmanager and messagemanager,
    // so that when self==NULL, they do nothing
    // (constructors, destructors and static functions excempted from this)
    // This way, we'll use the *manager::Get() functions to check for nulls.

    TiXmlElement* root;
    TiXmlElement* wksp;
    TiXmlElement* proj;
    wxString projectFilename;
    wxFileName wfname(filename);

    root = doc.FirstChildElement("CodeBlocks_workspace_file");
    if (!root)
    {
        // old tag
        root = doc.FirstChildElement("Code::Blocks_workspace_file");
        if (!root)
        {
            GetpMsg()->DebugLog(_("Not a valid Code::Blocks workspace file..."));
            return false;
        }
    }
    wksp = root->FirstChildElement("Workspace");
    if (!wksp)
    {
        GetpMsg()->DebugLog(_("No 'Workspace' element in file..."));
        return false;
    }

    m_Title = _U(wksp->Attribute("title")); // Conversion to unicode is automatic (see wxString::operator= )
    if (m_Title.IsEmpty())
        m_Title = _("Default workspace");

    proj = wksp->FirstChildElement("Project");
    if (!proj)
    {
        GetpMsg()->DebugLog(_("Workspace file contains no projects..."));
        return false;
    }

    // first loop to load projects
    while (proj)
    {
        if(Manager::isappShuttingDown() || !GetpMan() || !GetpMsg())
            return false;
        projectFilename = _U(proj->Attribute("filename"));
        if (projectFilename.IsEmpty())
        {
            GetpMsg()->DebugLog(_("'Project' node exists, but no filename?!?"));
        }
        else
        {
            wxFileName fname(projectFilename);
            fname.MakeAbsolute(wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
            int active = 0;
            int ret = proj->QueryIntAttribute("active", &active);
            switch (ret)
            {
                case TIXML_SUCCESS:
                    if (active == 1)
                        GetpMan()->LoadProject(fname.GetFullPath(), true); // activate it
                    break;
                case TIXML_WRONG_TYPE:
                    GetpMsg()->DebugLog(_("Error %s: %s"), doc.Value(), doc.ErrorDesc());
                    GetpMsg()->DebugLog(_("Wrong attribute type (expected 'int')"));
                    break;
                default:
                    GetpMan()->LoadProject(fname.GetFullPath(), false); // don;t activate it
                    break;
            }
        }
        proj = proj->NextSiblingElement("Project");
    }

    // second loop to setup dependencies
    proj = wksp->FirstChildElement("Project");
    while (proj)
    {
        cbProject* thisprj = 0;
        projectFilename = _U(proj->Attribute("filename"));
        if (projectFilename.IsEmpty())
        {
            GetpMsg()->DebugLog(_("'Project' node exists, but no filename?!?"));
            thisprj = 0;
        }
        else
        {
            wxFileName fname(projectFilename);
            fname.MakeAbsolute(wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
            thisprj = Manager::Get()->GetProjectManager()->IsOpen(fname.GetFullPath());
        }

        if (thisprj)
        {
            TiXmlElement* dep = proj->FirstChildElement("Depends");
            while (dep)
            {
                wxFileName fname(_U(dep->Attribute("filename")));
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
    wksp->SetAttribute("title", _C(title));

    ProjectsArray* arr = Manager::Get()->GetProjectManager()->GetProjects();
    for (unsigned int i = 0; i < arr->GetCount(); ++i)
    {
        cbProject* prj = arr->Item(i);

        wxFileName wfname(filename);
        wxFileName fname(prj->GetFilename());
        fname.MakeRelativeTo(wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));

        TiXmlElement* node = static_cast<TiXmlElement*>(wksp->InsertEndChild(TiXmlElement("Project")));
        node->SetAttribute("filename", _C(fname.GetFullPath()));
        if (prj == Manager::Get()->GetProjectManager()->GetActiveProject())
            node->SetAttribute("active", 1);

        const ProjectsArray* deps = Manager::Get()->GetProjectManager()->GetDependenciesForProject(prj);
        if (deps && deps->GetCount())
        {
            for (size_t i = 0; i < deps->GetCount(); ++i)
            {
                prj = deps->Item(i);
                fname.Assign(prj->GetFilename());
                fname.MakeRelativeTo(wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
                TiXmlElement* dnode = static_cast<TiXmlElement*>(node->InsertEndChild(TiXmlElement("Depends")));
                dnode->SetAttribute("filename", _C(fname.GetFullPath()));
            }
        }
    }
    return cbSaveTinyXMLDocument(&doc, filename);
}
