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
    cbProject* loadedProject;
    wxString projectFilename;

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

    while (proj)
    {
        if(Manager::isappShuttingDown() || !GetpMan() || !GetpMsg())
            return false;
        projectFilename = _U(proj->Attribute("filename"));
        if (projectFilename.IsEmpty())
        {
            GetpMsg()->DebugLog(_("'Project' node exists, but no filename?!?"));
            loadedProject = 0L;
        }
        else
        {
            wxFileName wfname(filename);
            wxFileName fname(projectFilename);
            fname.MakeAbsolute(wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
            loadedProject = GetpMan()->LoadProject(fname.GetFullPath());
        }
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
                    GetpMsg()->DebugLog(_("Error %s: %s"), doc.Value(), doc.ErrorDesc());
                    GetpMsg()->DebugLog(_("Wrong attribute type (expected 'int')"));
                    break;
                default:
                    break;
            }
        }
        proj = proj->NextSiblingElement();
    }

    if (m_pActiveProj)
        GetpMan()->SetProject(m_pActiveProj);

    return true;
}

bool WorkspaceLoader::Save(const wxString& title, const wxString& filename)
{
    wxString buffer;
    wxArrayString array;

    ProjectsArray* arr = Manager::Get()->GetProjectManager()->GetProjects();

    buffer << _T("<?xml version=\"1.0\"?>") << _T("\n");
    buffer << _T("<!DOCTYPE CodeBlocks_workspace_file>") << _T("\n");
    buffer << _T("<CodeBlocks_workspace_file>") << _T("\n");
    buffer << _T("\t") << _T("<Workspace title=\"") << title << _T("\">") << _T("\n");

    for (unsigned int i = 0; i < arr->GetCount(); ++i)
    {
        cbProject* prj = arr->Item(i);

        wxFileName wfname(filename);
        wxFileName fname(prj->GetFilename());
        fname.MakeRelativeTo(wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));

        buffer << _T("\t\t") << _T("<Project filename=\"") << fname.GetFullPath() << _T("\"");
        if (prj == Manager::Get()->GetProjectManager()->GetActiveProject())
            buffer << _T(" active=\"1\"");
        buffer << _T("/>") << _T("\n");
    }

    buffer << _T("\t") << _T("</Workspace>") << _T("\n");
    buffer << _T("</CodeBlocks_workspace_file>") << _T("\n");

    wxFile file(filename, wxFile::write);
    return cbWrite(file,buffer);
}
