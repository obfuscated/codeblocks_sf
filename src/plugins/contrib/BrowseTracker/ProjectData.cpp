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
// RCS-ID: $Id: ProjectData.cpp 24 2008-01-02 15:19:04Z Pecan $

#if defined(CB_PRECOMP)
#include "sdk.h"
#else
    #include "sdk_common.h"
	#include "sdk_events.h"
	#include "manager.h"
	#include "editormanager.h"
	#include "editorbase.h"
	#include "cbeditor.h"
	#include "cbproject.h"
#endif

    #include <wx/dynarray.h> //for wxArray and wxSortedArray
    #include <projectloader_hooks.h>
    #include <editor_hooks.h>
    #include "personalitymanager.h"
	#include <wx/stdpaths.h>
	#include <wx/app.h>
	#include <wx/menu.h>

//-#include "wx/wxFlatNotebook/wxFlatNotebook.h"
#include "Version.h"
#include "BrowseMarks.h"
#include "BrowseTrackerDefs.h"
#include "ProjectData.h"
#include "BrowseTrackerLayout.h"

// ----------------------------------------------------------------------------
ProjectData::ProjectData()
// ----------------------------------------------------------------------------
{
    //private ctor
}
// ----------------------------------------------------------------------------
ProjectData::ProjectData(cbProject* pcbProject)
// ----------------------------------------------------------------------------
{
    //ctor
    #if defined(LOGGING)
    if (not pcbProject) asm("int3"); /*trap*/;
    #endif
    if (not pcbProject) return;
    m_pCBProject = pcbProject;
    m_ProjectFilename = pcbProject->GetFilename();
    m_CurrIndexEntry = 0;
    m_LastIndexEntry = MaxEntries-1;
    m_pEdMgr = Manager::Get()->GetEditorManager();
    m_ActivationCount = 0;
    m_bLayoutLoaded = false;
    LoadLayout();
}
// ----------------------------------------------------------------------------
ProjectData::~ProjectData()
// ----------------------------------------------------------------------------
{
    //dtor

    // *BookMarksArchive* release the editor layout hash table ptrs to BookMarks
    for (FileBrowse_MarksHash::iterator it = m_FileBook_MarksArchive.begin(); it != m_FileBook_MarksArchive.end(); ++it)
    {
        delete it->second;
    }
    m_FileBook_MarksArchive.clear();

    // *BrowseMarksArchive* release the editor layout hash table ptrs to BrowseMarks
    for (FileBrowse_MarksHash::iterator it = m_FileBrowse_MarksArchive.begin(); it != m_FileBrowse_MarksArchive.end(); ++it)
    {
        delete it->second;
    }
    m_FileBrowse_MarksArchive.clear();


}
// ----------------------------------------------------------------------------
void ProjectData::IncrementActivationCount()
// ----------------------------------------------------------------------------
{
    m_ActivationCount += 1;
}
// ----------------------------------------------------------------------------
int ProjectData::GetActivationCount()
// ----------------------------------------------------------------------------
{
     return m_ActivationCount++;
}
// ----------------------------------------------------------------------------
wxString ProjectData::GetProjectFilename()
// ----------------------------------------------------------------------------
{
    return m_ProjectFilename;
}
// ----------------------------------------------------------------------------
void ProjectData::AddEditor( wxString filePath )
// ----------------------------------------------------------------------------
{
////    // Don't stow duplicates
////    if ( m_EditorBaseArray.Index( eb ) != wxNOT_FOUND ) return;
////
////    // not found, stow new data into arrays
////    int index = m_LastIndexEntry;
////    if (++index >= MaxEntries) index = 0;
////    m_LastIndexEntry = index;
////    m_EditorBaseArray[index] = eb;
////    m_cbEditorArray[index] = cbed;
////    m_cbSTCArray[index] = control;

}
// ----------------------------------------------------------------------------
bool ProjectData::FindFilename( const wxString filePath)
// ----------------------------------------------------------------------------
{
    //Return true if we own a BrowseMarks array by this filepath;

    FileBrowse_MarksHash& hash = m_FileBrowse_MarksArchive;
    FileBrowse_MarksHash::iterator it = hash.find(filePath);
    if ( it == hash.end() ) {
        ////DumpHash(wxT("BrowseMarks"));
        return false;
    }
    return true;
}
// ----------------------------------------------------------------------------
BrowseMarks* ProjectData::GetBrowse_MarksFromHash( wxString filePath)
// ----------------------------------------------------------------------------
{
    // Return a pointer to a BrowseMarks array with this filePath

    FileBrowse_MarksHash& hash = m_FileBrowse_MarksArchive;
    return GetPointerToBrowse_MarksArray(hash, filePath);
}
// ----------------------------------------------------------------------------
BrowseMarks* ProjectData::GetBook_MarksFromHash( wxString filePath)
// ----------------------------------------------------------------------------
{
    // Return a pointer to a BrowseMarks array with this filePath
    FileBrowse_MarksHash& hash = m_FileBook_MarksArchive;
    return GetPointerToBrowse_MarksArray( hash, filePath);
}
// ----------------------------------------------------------------------------
BrowseMarks* ProjectData::GetPointerToBrowse_MarksArray(FileBrowse_MarksHash& hash, wxString filePath)
// ----------------------------------------------------------------------------
{
    // Return a pointer to a BrowseMarks array with this filePath

    for (FileBrowse_MarksHash::iterator it = hash.begin(); it != hash.end(); it++)
    {
        BrowseMarks* p = it->second;
        if ( p->GetFilePath() == filePath ) {return p;}
    }

    return 0;
}
// ----------------------------------------------------------------------------
BrowseMarks* ProjectData::HashAddBrowse_Marks( const wxString fullPath )
// ----------------------------------------------------------------------------
{
    // EditorManager calls fail during the OnEditorClose event
    //EditorBase* eb = Manager::Get()->GetEditorManager()->GetEditor(filename);

    EditorBase* eb = m_pEdMgr->GetEditor(fullPath);
    #if defined(LOGGING)
        if(not eb) asm("int3"); /*trap*/
    #endif
    if(not eb) return 0;
    wxString filePath = eb->GetFilename();
    if (filePath.IsEmpty()) return 0;
    // don't add duplicates
    BrowseMarks* pBrowse_Marks = GetBrowse_MarksFromHash( filePath );
    if (pBrowse_Marks) return pBrowse_Marks ;
    pBrowse_Marks = new BrowseMarks( fullPath );
    if (pBrowse_Marks) m_FileBrowse_MarksArchive[filePath] = pBrowse_Marks;

    // allocate the equivalent BookMarks hash
    HashAddBook_Marks( fullPath );

    return pBrowse_Marks;
}
// ----------------------------------------------------------------------------
BrowseMarks* ProjectData::HashAddBook_Marks( wxString filePath)
// ----------------------------------------------------------------------------
{
    // EditorManager calls fail during the OnEditorClose event
    //EditorBase* eb = Manager::Get()->GetEditorManager()->GetEditor(filename);

    if(filePath.IsEmpty()) return 0;
    EditorBase* eb = m_pEdMgr->GetEditor(filePath);
    if (not eb) asm("int3"); //trap;
    if (not eb) return 0;
    // don't add duplicates
    BrowseMarks* pBook_Marks = GetBook_MarksFromHash( filePath);
    if (pBook_Marks) return pBook_Marks ;
    pBook_Marks = new BrowseMarks(filePath );
    if (pBook_Marks) m_FileBook_MarksArchive[filePath] = pBook_Marks;
    return pBook_Marks;
}

// ----------------------------------------------------------------------------
void ProjectData::LoadLayout()
// ----------------------------------------------------------------------------
{
    // Load a layout file for this project
    #if defined(LOGGING)
    LOGIT( _T("ProjectData::LoadLayout()for[%s]"),m_ProjectFilename.c_str() );
    #endif

    if (m_ProjectFilename.IsEmpty())
        return ;

    wxFileName fname(m_ProjectFilename);
    fname.SetExt(_T("bmarks"));
    BrowseTrackerLayout layout( m_pCBProject );
    layout.Open(fname.GetFullPath(), m_FileBrowse_MarksArchive, m_FileBook_MarksArchive);
    m_bLayoutLoaded = true;
}
// ----------------------------------------------------------------------------
void ProjectData::SaveLayout()
// ----------------------------------------------------------------------------
{
    // Write a layout file for this project
    #if defined(LOGGING)
    LOGIT( _T("ProjectData::SAVELayout()") );
    #endif

    if (m_ProjectFilename.IsEmpty())
        return ;

    wxFileName fname(m_ProjectFilename);
    fname.SetExt(_T("bmarks"));
    BrowseTrackerLayout layout( m_pCBProject );
    ////DumpBrowse_Marks(wxT("BookMarks"));
    ////DumpBrowse_Marks(wxT("BrowseMarks"));
    layout.Save(fname.GetFullPath(), m_FileBrowse_MarksArchive, m_FileBook_MarksArchive);


    //// *Testing* See if cbEditor is actually there
    //EditorBase* eb = m_EditorBaseArray[1];
    //cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);
    //cbStyledTextCtrl* control = cbed->GetControl();
    //#if defined(LOGGING)
    //LOGIT( _T("eb[%p]cbed[%p]control[%p]"), eb, cbed, control );
    //#endif

    //// *Testing* Check against our array
    //eb = m_EditorBaseArray[1];
    //cbed = m_cbEditorArray[1];
    //control = m_cbSTCArray[1];
    //#if defined(LOGGING)
    //LOGIT( _T("eb[%p]cbed[%p]control[%p]"), eb, cbed, control );
    //#endif


}
// ----------------------------------------------------------------------------
void ProjectData::DumpHash( const wxString hashType)
// ----------------------------------------------------------------------------
{

    #if defined(LOGGING)

    FileBrowse_MarksHash* phash = &m_FileBrowse_MarksArchive;
    if ( hashType == wxT("BookMarks") )
        phash = &m_FileBook_MarksArchive;
    FileBrowse_MarksHash& hash = *phash;

    LOGIT( _T("--- DumpProjectHash ---[%s]Count[%d]"), hashType.c_str(), hash.size(), m_ProjectFilename.c_str() );
    for (FileBrowse_MarksHash::iterator it = hash.begin(); it != hash.end(); it++)
    {
        wxString filename = it->first; //an Editor filename withing this project
        BrowseMarks* p = it->second;    // ptr to array of Editor Browse/Book mark cursor positions
        LOGIT( _T("filename[%s]BrowseMark*[%p]name[%s]"), filename.c_str(), p, p->GetFilePath().c_str() );
    }

    #endif
}

// ----------------------------------------------------------------------------
void ProjectData::DumpBrowse_Marks( const wxString hashType )
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    LOGIT( _T("--- DumpBrowseData ---[%s]"), hashType.c_str()  );

    FileBrowse_MarksHash* phash = &m_FileBrowse_MarksArchive;
    if ( hashType == wxT("BookMarks") )
        phash = &m_FileBook_MarksArchive;
    FileBrowse_MarksHash& hash = *phash;

    LOGIT( _T("Dump_%s Size[%d]"), hashType.c_str(), hash.size() );

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
// 2008/01/23 MortenMacFly
// ----------------------------------------------------------------------------
// I have serious issues with this plugin (recently). Whenever I try to import a
// Visual Studio solution C::B crashes. The reason is the BT plugin, file
// ProjectData.cpp, line 63 (m_ProjectFilename = pcbProject->GetFilename();).
// It seems pcbProject *is* NULL sometimes in the case of an import.
// So you better don't rely on it. I had to disable the BT plugin to be able to
// import VS solutions again. Sad
