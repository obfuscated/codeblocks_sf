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
            pf->editorSplit = cbEditor::stNoSplit;
            pf->editorSplitActive = 1;
            pf->editorZoom = 0;
            pf->editorPos = 0;
            pf->editorTopLine = 0;
            pf->editorZoom_2 = 0;
            pf->editorPos_2 = 0;
            pf->editorTopLine_2 = 0;
            int getInt = 0; // used to fetch int values

            if (elem->QueryIntAttribute("open", &getInt) == TIXML_SUCCESS)
                pf->editorOpen = getInt != 0;
            if (elem->QueryIntAttribute("top", &getInt) == TIXML_SUCCESS)
            {
                if (getInt)
                    m_TopProjectFile = pf;
            }
            if (elem->QueryIntAttribute("tabpos", &getInt) == TIXML_SUCCESS)
                pf->editorTabPos = getInt;
            if (elem->QueryIntAttribute("split", &getInt) == TIXML_SUCCESS)
                pf->editorSplit = getInt;
            if (elem->QueryIntAttribute("active", &getInt) == TIXML_SUCCESS)
                pf->editorSplitActive = getInt;
            if (elem->QueryIntAttribute("splitpos", &getInt) == TIXML_SUCCESS)
                pf->editorSplitPos = getInt;
            if (elem->QueryIntAttribute("zoom_1", &getInt) == TIXML_SUCCESS)
                pf->editorZoom = getInt;
            if (elem->QueryIntAttribute("zoom_2", &getInt) == TIXML_SUCCESS)
                pf->editorZoom_2 = getInt;

            TiXmlElement* cursor = elem->FirstChildElement("Cursor");
            if (cursor)
            {
                cursor = cursor->FirstChildElement();
                if (cursor)
                {
                    if (cursor->QueryIntAttribute("position", &getInt) == TIXML_SUCCESS)
                        pf->editorPos = getInt;
                    if (cursor->QueryIntAttribute("topLine", &getInt) == TIXML_SUCCESS)
                        pf->editorTopLine = getInt;
                    if (pf->editorSplit != cbEditor::stNoSplit)
                    {
                        cursor = cursor->NextSiblingElement();
                        if (cursor)
                        {
                            if (cursor->QueryIntAttribute("position", &getInt) == TIXML_SUCCESS)
                                pf->editorPos_2 = getInt;
                            if (cursor->QueryIntAttribute("topLine", &getInt) == TIXML_SUCCESS)
                                pf->editorTopLine_2 = getInt;
                        }
                    }
                }
            }

            TiXmlElement* folding = elem->FirstChildElement("Folding");
            if (folding)
            {
                folding = folding->FirstChildElement();
                while (folding)
                {
                    if (folding->QueryIntAttribute("line", &getInt) == TIXML_SUCCESS)
                        pf->editorFoldLinesArray.Add(getInt);

                    folding = folding->NextSiblingElement();
                }
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

    for (FilesList::iterator it = m_pProject->GetFilesList().begin(); it != m_pProject->GetFilesList().end(); ++it)
    {
        ProjectFile* f = *it;

        if (f->editorOpen || f->editorPos || f->editorPos_2 || f->editorTopLine || f->editorTopLine_2 || f->editorTabPos)
        {
            TiXmlElement* node = static_cast<TiXmlElement*>(rootnode->InsertEndChild(TiXmlElement("File")));
            node->SetAttribute("name", cbU2C(f->relativeFilename));
            node->SetAttribute("open", f->editorOpen);
            node->SetAttribute("top", (f == active));
            node->SetAttribute("tabpos", f->editorTabPos);
            node->SetAttribute("split", f->editorSplit);
            node->SetAttribute("active", f->editorSplitActive);
            node->SetAttribute("splitpos", f->editorSplitPos);
            node->SetAttribute("zoom_1", f->editorZoom);
            node->SetAttribute("zoom_2", f->editorZoom_2);


            TiXmlElement* cursor = static_cast<TiXmlElement*>(node->InsertEndChild(TiXmlElement("Cursor")));
            TiXmlElement* cursor_1 = static_cast<TiXmlElement*>(cursor->InsertEndChild(TiXmlElement("Cursor1")));
            cursor_1->SetAttribute("position", f->editorPos);
            cursor_1->SetAttribute("topLine", f->editorTopLine);

            if(f->editorSplit != cbEditor::stNoSplit)
            {
                TiXmlElement* cursor_2 = static_cast<TiXmlElement*>(cursor->InsertEndChild(TiXmlElement("Cursor2")));
                cursor_2->SetAttribute("position", f->editorPos_2);
                cursor_2->SetAttribute("topLine", f->editorTopLine_2);
            }

            if (f->editorFoldLinesArray.GetCount() > 0)
            {
                TiXmlElement* folding = static_cast<TiXmlElement*>(node->InsertEndChild(TiXmlElement("Folding")));
                for (unsigned int i = 0; i < f->editorFoldLinesArray.GetCount(); i++)
                {
                    TiXmlElement* line = static_cast<TiXmlElement*>(folding->InsertEndChild(TiXmlElement("Collapse")));
                    line->SetAttribute("line", f->editorFoldLinesArray[i]);
                }
            }
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
