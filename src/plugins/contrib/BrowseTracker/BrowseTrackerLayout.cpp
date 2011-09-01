#include "BrowseTrackerLayout.h"
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
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinywxuni.h"

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
bool BrowseTrackerLayout::Open(const wxString& filename, FileBrowse_MarksHash& m_FileBrowse_MarksArchive , FileBrowse_MarksHash& m_EdBook_MarksArchive )
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
            ///if (not browsemarks)
            ///    LOGIT( _T("OPEN LAYOUT failed for BrowseMarks") );
            if (browsemarks)
            {
                wxString marksString = cbC2U(browsemarks->Attribute("positions"));
                #if defined(LOGGING)
                ////LOGIT( _T("OPEN_LAYOUT BROWSEMarksStrng[%s][%s]"), fname.c_str(), marksString.c_str() );
                #endif
                ParseBrowse_MarksString( fname, marksString, m_FileBrowse_MarksArchive );
            }

            TiXmlElement* bookmarks = cursor->NextSiblingElement("Book_Marks");
            ///if (not bookmarks)
            ///    LOGIT( _T("OPEN LAYOUT failed for Book_Marks") );
            if (bookmarks)
            {
                wxString marksString = cbC2U(bookmarks->Attribute("positions"));
                #if defined(LOGGING)
                ////LOGIT( _T("OPEN_LAYOUT BOOKMarksStrng[%s][%s]"), fname.c_str(), marksString.c_str() );
                #endif
                ParseBrowse_MarksString( fname, marksString, m_EdBook_MarksArchive );
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
bool BrowseTrackerLayout::Save(const wxString& filename, FileBrowse_MarksHash& m_FileBrowse_MarksArchive, FileBrowse_MarksHash& m_EdBook_MarksArchive)
// ----------------------------------------------------------------------------
{
    ////DumpBrowse_Marks(wxT("BookMarks"), m_FileBrowse_MarksArchive, m_EdBook_MarksArchive);

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

            EditorBase* eb = 0;
            // write out a string of browse mark positions
            #if defined(LOGGING)
            ////LOGIT( _T("ProjectFilename[%s]"),f->file.GetFullPath().c_str() );
            #endif

            eb = Manager::Get()->GetEditorManager()->GetEditor(f->file.GetFullPath());
            #if defined(LOGGING)
            ////if (eb) LOGIT( _T("EditorBase Filename[%d][%s]"), i, eb->GetFilename().c_str() );
            #endif
            ////if(eb) if (f->file.GetFullPath() != eb->GetFilename())
            ////{
            ////    #if defined(LOGGING)
            ////    LOGIT( _T("NAME MISSMATCH ProjectFile[%s]EditorBase[%s]"), f->file.GetFullPath().c_str(), eb->GetFilename().c_str() );
            ////    #endif
            ////}

////            #if defined(LOGGING)
////            if (m_FileBrowse_MarksArchive.find(eb) != m_FileBrowse_MarksArchive.end() )
////                LOGIT( _T("Found eb[%p][%s]"), eb, eb->GetShortName().c_str() );
////            else{
////                int i = 0;
////                for (EbBrowse_MarksHash::iterator it = m_FileBrowse_MarksArchive.begin();
////                        it != m_FileBrowse_MarksArchive.end(); ++it)
////                {
////                	#if defined(LOGGING)
////                	LOGIT( _T("m_FileBrowse_MarksArchive[i][%d][%p]"), i, it->first );
////                	#endif
////                	++i;
////                }
////            }
////            #endif
            #if defined(LOGGING)
            ////LOGIT( _T("Layout processing for[%s]"),/*f->relativeFilename.c_str(),*/ f->file.GetFullPath().c_str() );
            #endif
            // Save the BrowseMarks
            FileBrowse_MarksHash::iterator it = m_FileBrowse_MarksArchive.find(f->file.GetFullPath());
            if (it != m_FileBrowse_MarksArchive.end() ) do
            {
                BrowseMarks* pBrowse_Marks = it->second;
                if (not pBrowse_Marks) break;
                wxString browseMarks = pBrowse_Marks->GetStringOfBrowse_Marks();
                #if defined(LOGGING)
                ////LOGIT( _T("Layout writing BROWSEMarkString [%p]is[%s]"), pBrowse_Marks, browseMarks.c_str());
                #endif
                TiXmlElement* btMarks = static_cast<TiXmlElement*>(node->InsertEndChild(TiXmlElement("BrowseMarks")));
                btMarks->SetAttribute("positions", cbU2C(browseMarks));
            }while(0);
            ////else{
            ////    #if defined(LOGGING)
            ////    LOGIT( _T("Browse_Marks failed find for[%s]"), f->file.GetFullPath().c_str() );
            ////    #endif
            ////}
            // Save the Book_Marks
            it = m_EdBook_MarksArchive.find(f->file.GetFullPath());
            if (it != m_EdBook_MarksArchive.end() ) do
            {
                BrowseMarks* pBook_Marks = it->second;
                if (not pBook_Marks) break;
                wxString bookMarks = pBook_Marks->GetStringOfBrowse_Marks();
                #if defined(LOGGING)
                ////LOGIT( _T("Layout writing BOOKMarkString [%p]is[%s]"), pBook_Marks, bookMarks.c_str());
                #endif
                TiXmlElement* btMarks = static_cast<TiXmlElement*>(node->InsertEndChild(TiXmlElement("Book_Marks")));
                btMarks->SetAttribute("positions", cbU2C(bookMarks));
            }while(0);
            ////else{
            ////    #if defined(LOGGING)
            ////    LOGIT( _T("Book_Marks failed find for[%s]"), f->file.GetFullPath().c_str() );
            ////    #endif
            ////}

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
void BrowseTrackerLayout::DumpBrowse_Marks( const wxString hashType, FileBrowse_MarksHash& m_FileBrowse_MarksArchive, FileBrowse_MarksHash& m_FileBook_MarksArchive )
#else
void BrowseTrackerLayout::DumpBrowse_Marks( const wxString /*hashType*/, FileBrowse_MarksHash& /*m_FileBrowse_MarksArchive*/, FileBrowse_MarksHash& /*m_FileBook_MarksArchive*/ )
#endif
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    LOGIT( _T("--- DumpBrowseData ---[%s]"), hashType.c_str()  );

    FileBrowse_MarksHash* phash = &m_FileBrowse_MarksArchive;
    if ( hashType == wxT("BookMarks") )
        phash = &m_FileBook_MarksArchive;
    FileBrowse_MarksHash& hash = *phash;
    #if defined(LOGGING)
    LOGIT( _T("Dump_%s Size[%d]"), hashType.c_str(), hash.size() );
    #endif
    for (FileBrowse_MarksHash::iterator it = hash.begin(); it != hash.end(); ++it)
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
