#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/log.h>
#include <wx/intl.h>
#include "manager.h"
#include "projectmanager.h"
#include "messagemanager.h"
#include "editormanager.h"
#include "cbeditor.h"
#include "cbproject.h"
#include "projectlayoutloader.h"
#include "tinyxml/tinyxml.h"

ProjectLayoutLoader::ProjectLayoutLoader(cbProject* project)
    : m_pProject(project),
    m_TopProjectFile(0L)
{
	//ctor
}

ProjectLayoutLoader::~ProjectLayoutLoader()
{
	//dtor
}

bool ProjectLayoutLoader::Open(const wxString& filename)
{
    TiXmlDocument doc(filename.c_str());
    if (!doc.LoadFile())
        return false;
    
    ProjectManager* pMan = Manager::Get()->GetProjectManager();
    MessageManager* pMsg = Manager::Get()->GetMessageManager();
    if (!pMan || !pMsg)
        return false;

    cbProject* pProject = pMan->GetActiveProject();
    if (!pProject)
        return false;

    TiXmlElement* root;
    TiXmlElement* elem;
    wxString fname;
    ProjectFile* pf;
    
    root = doc.FirstChildElement("Code::Blocks_layout_file");
    if (!root)
    {
        pMsg->DebugLog("Not a valid Code::Blocks layout file...");
        return false;
    }
    elem = root->FirstChildElement("File");
    if (!elem)
    {
        //pMsg->DebugLog("No 'File' element in file...");
        return false;
    }
    
    while (elem)
    {
        //pMsg->DebugLog(elem->Value());
        fname = elem->Attribute("name");
        if (fname.IsEmpty())
        {
            //pMsg->DebugLog("'File' node exists, but no filename?!?");
            pf = 0L;
        }
        else
            pf = pProject->GetFileByFilename(fname);
        
        if (pf)
        {
            pf->editorOpen = false;
            pf->editorPos = 0;
            pf->editorTopLine = 0;
            int open = 0;
            int top = 0;
            if (elem->QueryIntAttribute("open", &open) == TIXML_SUCCESS)
                pf->editorOpen = open != 0;
            if (elem->QueryIntAttribute("top", &top) == TIXML_SUCCESS)
            {   
                if(top)
                    m_TopProjectFile = pf;
            }

            TiXmlElement* cursor = elem->FirstChildElement();
            if (cursor)
            {
                int pos = 0;
                int topline = 0;
                if (cursor->QueryIntAttribute("position", &pos) == TIXML_SUCCESS)
                    pf->editorPos = pos;
                if (cursor->QueryIntAttribute("topLine", &topline) == TIXML_SUCCESS)
                    pf->editorTopLine = topline;
            }
        }

        elem = elem->NextSiblingElement();
    }
    
    return true;
}

bool ProjectLayoutLoader::Save(const wxString& filename)
{
    wxString buffer;
    wxArrayString array;
	

    buffer << "<?xml version=\"1.0\"?>" << '\n';
    buffer << "<!DOCTYPE Code::Blocks_layout_file>" << '\n';
    buffer << "<Code::Blocks_layout_file>" << '\n';

	ProjectFile* active = 0L;
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	if (ed)
		active = ed->GetProjectFile();

	int count = m_pProject->GetFilesCount();
	for (int i = 0; i < count; ++i)
	{
		ProjectFile* f = m_pProject->GetFile(i);

		if (f->editorOpen || f->editorPos || f->editorTopLine)
		{
			buffer << '\t' << "<File name=\"" << f->relativeFilename << "\" ";
			buffer << "open=\"" << f->editorOpen << "\" ";
			buffer << "top=\"" << (f == active) << "\">" << '\n';
			buffer << '\t' << '\t' << "<Cursor position=\"" << f->editorPos << "\" topLine=\"" << f->editorTopLine << "\"/>" << '\n';
			buffer << '\t' << "</File>" << '\n';
		}
	}
	const wxArrayString& en = m_pProject->ExpandedNodes();
	for (unsigned int i = 0; i < en.GetCount(); ++i)
	{
		if (!en[i].IsEmpty())
			buffer << '\t' << "<Expand folder=\"" << en[i] << "\"/>" << '\n';
	}

    buffer << "</Code::Blocks_layout_file>" << '\n';

    wxFile file(filename, wxFile::write);
    return file.Write(buffer, buffer.Length()) == buffer.Length();
}
