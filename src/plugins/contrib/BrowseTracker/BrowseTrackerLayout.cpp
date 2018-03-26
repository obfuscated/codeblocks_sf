/*
    This file is part of Browse Tracker, a plugin for Code::Blocks
    Copyright (C) 2007 Pecan Heber

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
// RCS-ID: $Id$

/*
* This file is part of Code::Bocks, an open-source cross-platform IDE
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
#include "BrowseTrackerLayout.h"

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

#include <wx/tokenzr.h>
#include <tinyxml.h>
#include <tinywxuni.h>

#include "BrowseTrackerLayout.h"
#include "BrowseMarks.h"
#include "Version.h"

// ----------------------------------------------------------------------------
BrowseTrackerLayout::BrowseTrackerLayout(cbProject* project)
// ----------------------------------------------------------------------------
    : m_pProject(project)
{
    //ctor
}

// ----------------------------------------------------------------------------
BrowseTrackerLayout::~BrowseTrackerLayout()
// ----------------------------------------------------------------------------
{
    //dtor
}

// IMPORTANT! We have to be careful of what to unicode and what not to.
// TinyXML must use NON-unicode strings!
// ----------------------------------------------------------------------------
bool BrowseTrackerLayout::Open(const wxString& filename, FileBrowse_MarksHash& m_FileBrowse_MarksArchive )
// ----------------------------------------------------------------------------
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


    root = doc.FirstChildElement("BrowseTracker_layout_file");
    if (!root)
    {
        // old tag
        root = doc.FirstChildElement("BrowseTracker_layout_file");
        if (!root)
        {
            pMsg->DebugLog(_T("Not a valid BrowseTracker layout file..."));
            return false;
        }
    }

    elem = root->FirstChildElement("ActiveTarget");
    if (elem)
    {
        if (elem->Attribute("name"))
        {
            ;//m_pProject->SetActiveBuildTarget(cbC2U(elem->Attribute("name")));
        }
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
            //pf->editorOpen = false;
            //pf->editorPos = 0;
            //pf->editorTopLine = 0;
            int open = 0;
            int top = 0;
            int tabpos = 0;
            if (elem->QueryIntAttribute("open", &open) == TIXML_SUCCESS)
            {
                ;//pf->editorOpen = open != 0;
            }
            if (elem->QueryIntAttribute("top", &top) == TIXML_SUCCESS)
            {
                if(top)
                    m_TopProjectFile = pf;
            }
            if (elem->QueryIntAttribute("tabpos", &tabpos) == TIXML_SUCCESS)
            {
                ;//pf->editorTabPos = tabpos;
            }

            TiXmlElement* cursor = elem->FirstChildElement();
            if (cursor)
            {
                int pos = 0;
                int topline = 0;
                if (cursor->QueryIntAttribute("position", &pos) == TIXML_SUCCESS)
                {
                    ;//pf->editorPos = pos;
                }
                if (cursor->QueryIntAttribute("topLine", &topline) == TIXML_SUCCESS)
                {
                    ;//pf->editorTopLine = topline;
                }
            }

            #if defined(LOGGING)
            ///LOGIT( _T("Open Layout processing for[%s]"),fname.c_str() );
            #endif

            TiXmlElement* browsemarks = cursor->NextSiblingElement("BrowseMarks");
            //if (not browsemarks)
            //    LOGIT( _T("OPEN LAYOUT failed for BrowseMarks") );
            if (browsemarks)
            {
                wxString marksString = cbC2U(browsemarks->Attribute("positions"));
                #if defined(LOGGING)
                //LOGIT( _T("OPEN_LAYOUT BROWSEMarksStrng[%s][%s]"), fname.c_str(), marksString.c_str() );
                #endif
                ParseBrowse_MarksString( fname, marksString, m_FileBrowse_MarksArchive );
            }
        }

        elem = elem->NextSiblingElement();
    }

    return true;
}//Open
// ----------------------------------------------------------------------------
bool BrowseTrackerLayout::ParseBrowse_MarksString(const wxString& filename, wxString BrowseMarksString, FileBrowse_MarksHash& m_EdMarksArchive)
// ----------------------------------------------------------------------------
{
    if (filename.IsEmpty()) return false;
    if ( BrowseMarksString.IsEmpty() ) return false;
    ProjectFile* pf = m_pProject->GetFileByFilename( filename, true, true);
    if ( not pf ) return false;
    wxString filenamePath = pf->file.GetFullPath();

   // parse the comma delimited string
    BrowseMarks*  pEdPosnArchive = new BrowseMarks(filenamePath );
    wxStringTokenizer tkz(BrowseMarksString, wxT(","));
    while ( tkz.HasMoreTokens() )
    {   long longnum;
        tkz.GetNextToken().ToLong(&longnum);
        pEdPosnArchive->RecordMark(longnum);
    }//while
    m_EdMarksArchive[filenamePath] = pEdPosnArchive;
    return true;
}//ParseBrowse_MarksString
// ----------------------------------------------------------------------------
bool BrowseTrackerLayout::Save(const wxString& filename, FileBrowse_MarksHash& m_FileBrowse_MarksArchive)
// ----------------------------------------------------------------------------
{
    //DumpBrowse_Marks(wxT("BookMarks"), m_FileBrowse_MarksArchive, m_EdBook_MarksArchive);

    const char* ROOT_TAG = "BrowseTracker_layout_file";

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

            // Save the BrowseMarks
            FileBrowse_MarksHash::iterator it2 = m_FileBrowse_MarksArchive.find(f->file.GetFullPath());
            if (it2 != m_FileBrowse_MarksArchive.end() ) do
            {
                const BrowseMarks* pBrowse_Marks = it2->second;
                if (not pBrowse_Marks) break;
                wxString browseMarks = pBrowse_Marks->GetStringOfBrowse_Marks();
                #if defined(LOGGING)
                //LOGIT( _T("Layout writing BROWSEMarkString [%p]is[%s]"), pBrowse_Marks, browseMarks.c_str());
                #endif
                TiXmlElement* btMarks = static_cast<TiXmlElement*>(node->InsertEndChild(TiXmlElement("BrowseMarks")));
                btMarks->SetAttribute("positions", cbU2C(browseMarks));
            }while(0);
        }
    }//for

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
// ----------------------------------------------------------------------------
#if defined(LOGGING)
void BrowseTrackerLayout::DumpBrowse_Marks( const wxString hashType, FileBrowse_MarksHash& m_FileBrowse_MarksArchive )
#else
void BrowseTrackerLayout::DumpBrowse_Marks( const wxString /*hashType*/, FileBrowse_MarksHash& /*m_FileBrowse_MarksArchive*/ )
#endif
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    LOGIT( _T("--- DumpBrowseData ---[%s]"), hashType.c_str()  );

    FileBrowse_MarksHash* phash = &m_FileBrowse_MarksArchive;
    #if defined(LOGGING)
    LOGIT( _T("Dump_%s Size[%lu]"), hashType.wx_str(), static_cast<unsigned long>(phash->size()) );
    #endif
    for (FileBrowse_MarksHash::iterator it = phash->begin(); it != phash->end(); ++it)
    {
        wxString filename = it->first;
        BrowseMarks* p = it->second;
        LOGIT( _T("Filename[%s]%s*[%p]name[%s]"), filename.c_str(), hashType.c_str(), p, p->GetFilePath().c_str() );
        if (p)
        {   //dump the browse marks
            p->Dump();
        }
    }

    #endif
}
// ----------------------------------------------------------------------------
