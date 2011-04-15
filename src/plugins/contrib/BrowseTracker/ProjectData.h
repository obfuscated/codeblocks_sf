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
#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include "BrowseTrackerDefs.h"

WX_DEFINE_ARRAY_PTR(cbEditor*, ArrayOfcbEditorPtrs);
WX_DEFINE_ARRAY_PTR(cbStyledTextCtrl*, ArrayOfcbSTCPtrs);
// ----------------------------------------------------------------------------
class ProjectData
// ----------------------------------------------------------------------------
{
    public:
        ProjectData(cbProject* pcbProject);
        ~ProjectData();

        wxString        GetProjectFilename();

        void            AddEditor( wxString filePath );
        BrowseMarks*    GetBrowse_MarksFromHash( wxString filePath);
        BrowseMarks*    GetBook_MarksFromHash( wxString filePath);
        BrowseMarks*    GetPointerToBrowse_MarksArray(FileBrowse_MarksHash& hash ,wxString filePath);
        bool            FindFilename( const wxString filePath);
        void            IncrementActivationCount();
        int             GetActivationCount();

        BrowseMarks*    HashAddBrowse_Marks( const wxString filePath );


        BrowseMarks*    HashAddBook_Marks( wxString filePath);
        BrowseMarks*    HashAddBook_Marks( EditorBase* eb);
        void            SaveLayout();
        void            LoadLayout();
        bool            IsLayoutLoaded(){return m_bLayoutLoaded;}

        void            DumpHash( const wxString hashType);
        void            DumpBrowse_Marks( const wxString hashType );
    protected:
    private:
        ProjectData();

        EditorManager*  m_pEdMgr;
        wxString        m_ProjectFilename;
        cbProject*      m_pCBProject;

        // Hash: filePath, BrowseMarks* array
        FileBrowse_MarksHash m_FileBook_MarksArchive;
        FileBrowse_MarksHash m_FileBrowse_MarksArchive;

        int     m_CurrIndexEntry;
        int     m_LastIndexEntry;

        int     m_ActivationCount;
        bool    m_bLayoutLoaded;

};



#endif // PROJECTDATA_H
