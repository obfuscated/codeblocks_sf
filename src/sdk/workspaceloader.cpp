#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/log.h>
#include <wx/intl.h>
#include "manager.h"
#include "projectmanager.h"
#include "messagemanager.h"
#include "cbproject.h"
#include "workspaceloader.h"
#include "tinyxml/tinyxml.h"

WorkspaceLoader::WorkspaceLoader()
    : m_pActiveProj(0L)
{
	//ctor
}

WorkspaceLoader::~WorkspaceLoader()
{
	//dtor
}

bool WorkspaceLoader::Open(const wxString& filename)
{
    TiXmlDocument doc(filename.c_str());
    if (!doc.LoadFile())
        return false;
    
    ProjectManager* pMan = Manager::Get()->GetProjectManager();
    MessageManager* pMsg = Manager::Get()->GetMessageManager();
    if (!pMan || !pMsg)
        return false;

    TiXmlElement* root;
    TiXmlElement* wksp;
    TiXmlElement* proj;
    cbProject* loadedProject;
    wxString projectFilename;
    
    root = doc.FirstChildElement("Code::Blocks_workspace_file");
    if (!root)
    {
        pMsg->DebugLog("Not a valid Code::Blocks workspace file...");
        return false;
    }
    wksp = root->FirstChildElement("Workspace");
    if (!wksp)
    {
        pMsg->DebugLog("No 'Workspace' element in file...");
        return false;
    }
    proj = wksp->FirstChildElement("Project");
    if (!proj)
    {
        pMsg->DebugLog("Workspace file contains no projects...");
        return false;
    }
    
    while (proj)
    {
        projectFilename = proj->Attribute("filename");
        if (projectFilename.IsEmpty())
        {
            pMsg->DebugLog("'Project' node exists, but no filename?!?");
            loadedProject = 0L;
        }
        else
            loadedProject = pMan->LoadProject(projectFilename);
        if (loadedProject)
        {
            int active = 0;
            int ret = proj->QueryIntAttribute("active", &active);
            switch (ret)
            {
                case TIXML_SUCCESS:
                    if (active == 1)
                        m_pActiveProj = loadedProject;
                    break;
                case TIXML_WRONG_TYPE:
                    pMsg->DebugLog("Error %s: %s", doc.Value(), doc.ErrorDesc());
                    pMsg->DebugLog("Wrong attribute type (expected 'int')");
                    break;
                default:
                    break;
            }
        }
        proj = proj->NextSiblingElement();
    }
    
    if (m_pActiveProj)
        pMan->SetProject(m_pActiveProj);
    
    return true;
}

bool WorkspaceLoader::Save(const wxString& filename)
{
    wxString buffer;
    wxArrayString array;

    ProjectsArray* arr = Manager::Get()->GetProjectManager()->GetProjects();

    buffer << "<?xml version=\"1.0\"?>" << '\n';
    buffer << "<!DOCTYPE Code::Blocks_workspace_file>" << '\n';
    buffer << "<Code::Blocks_workspace_file>" << '\n';
    buffer << '\t' << "<Workspace>" << '\n';
    
    for (unsigned int i = 0; i < arr->GetCount(); ++i)
    {
        cbProject* prj = arr->Item(i);
        buffer << '\t' << '\t' << "<Project filename=\"" << prj->GetFilename() << "\"";
        if (prj == Manager::Get()->GetProjectManager()->GetActiveProject())
            buffer << " active=\"1\"";
        buffer << "/>" << '\n';
    }

    buffer << '\t' << "</Workspace>" << '\n';
    buffer << "</Code::Blocks_workspace_file>" << '\n';

    wxFile file(filename, wxFile::write);
    return file.Write(buffer, buffer.Length()) == buffer.Length();
}
