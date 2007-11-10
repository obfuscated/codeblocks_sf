/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
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
    #include "manager.h"
    #include "projectmanager.h"
    #include "logmanager.h"
    #include "editormanager.h"
    #include "cbeditor.h"
    #include "cbproject.h"
#endif

#include "projectlayoutloader.h"
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinywxuni.h"

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

// IMPORTANT! We have to be careful of what to unicode and what not to.
// TinyXML must use NON-unicode strings!

bool ProjectLayoutLoader::Open(const wxString& filename)
{
    TiXmlDocument doc;
    if (!TinyXML::LoadDocument(filename, &doc))
        return false;

    ProjectManager* pMan = Manager::Get()->GetProjectManager();
    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMan || !pMsg)
        return false;

    TiXmlElement* root;
    TiXmlElement* elem;
    wxString fname;
    ProjectFile* pf;

    root = doc.FirstChildElement("CodeBlocks_layout_file");
    if (!root)
    {
        // old tag
        root = doc.FirstChildElement("Code::Blocks_layout_file");
        if (!root)
        {
            pMsg->DebugLog(_T("Not a valid Code::Blocks layout file..."));
            return false;
        }
    }

    elem = root->FirstChildElement("ActiveTarget");
    if (elem)
    {
        if (elem->Attribute("name"))
            m_pProject->SetActiveBuildTarget(cbC2U(elem->Attribute("name")));
    }

    elem = root->FirstChildElement("File");
    if (!elem)
    {
        //pMsg->DebugLog(_T("No 'File' element in file..."));
        return false;
    }

    while (elem)
    {
        //pMsg->DebugLog(elem->Value());
        fname = cbC2U(elem->Attribute("name"));
        if (fname.IsEmpty())
        {
            //pMsg->DebugLog(_T("'File' node exists, but no filename?!?"));
            pf = 0L;
        }
        else
            pf = m_pProject->GetFileByFilename(fname);

        if (pf)
        {
            pf->editorOpen = false;
            pf->editorPos = 0;
            pf->editorTopLine = 0;
            int open = 0;
            int top = 0;
            int tabpos = 0;
            if (elem->QueryIntAttribute("open", &open) == TIXML_SUCCESS)
                pf->editorOpen = open != 0;
            if (elem->QueryIntAttribute("top", &top) == TIXML_SUCCESS)
            {
                if(top)
                    m_TopProjectFile = pf;
            }
            if (elem->QueryIntAttribute("tabpos", &tabpos) == TIXML_SUCCESS)
				pf->editorTabPos = tabpos;

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
    const char* ROOT_TAG = "CodeBlocks_layout_file";

    TiXmlDocument doc;
    doc.SetCondenseWhiteSpace(false);
    doc.InsertEndChild(TiXmlDeclaration("1.0", "UTF-8", "yes"));
    TiXmlElement* rootnode = static_cast<TiXmlElement*>(doc.InsertEndChild(TiXmlElement(ROOT_TAG)));
    if (!rootnode)
        return false;

    TiXmlElement* tgtidx = static_cast<TiXmlElement*>(rootnode->InsertEndChild(TiXmlElement("ActiveTarget")));
    tgtidx->SetAttribute("name", cbU2C(m_pProject->GetActiveBuildTarget()));

	ProjectFile* active = 0L;
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	if (ed)
		active = ed->GetProjectFile();

	int count = m_pProject->GetFilesCount();
	for (int i = 0; i < count; ++i)
	{
		ProjectFile* f = m_pProject->GetFile(i);

		if (f->editorOpen || f->editorPos || f->editorTopLine || f->editorTabPos)
		{
            TiXmlElement* node = static_cast<TiXmlElement*>(rootnode->InsertEndChild(TiXmlElement("File")));
            node->SetAttribute("name", cbU2C(f->relativeFilename));
            node->SetAttribute("open", f->editorOpen);
            node->SetAttribute("top", (f == active));
            node->SetAttribute("tabpos", f->editorTabPos);

            TiXmlElement* cursor = static_cast<TiXmlElement*>(node->InsertEndChild(TiXmlElement("Cursor")));
            cursor->SetAttribute("position", f->editorPos);
            cursor->SetAttribute("topLine", f->editorTopLine);
		}
	}
	const wxArrayString& en = m_pProject->ExpandedNodes();
	for (unsigned int i = 0; i < en.GetCount(); ++i)
	{
		if (!en[i].IsEmpty())
		{
            TiXmlElement* node = static_cast<TiXmlElement*>(rootnode->InsertEndChild(TiXmlElement("Expand")));
            node->SetAttribute("folder", cbU2C(en[i]));
		}
	}
    return cbSaveTinyXMLDocument(&doc, filename);
}
