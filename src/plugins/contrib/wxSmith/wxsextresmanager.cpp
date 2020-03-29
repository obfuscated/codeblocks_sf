/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsextresmanager.h"
#include "wxsmith.h"
#include "wxsresourcefactory.h"

wxsExtResManager::wxsExtResManager():
    m_ClosingAll(false)
{
}

wxsExtResManager::~wxsExtResManager()
{
    int TestCnt = m_Files.size();
    while ( !m_Files.empty() )
    {
        // Each delete should remove one entry
        delete m_Files.begin()->second;
        // Just in case of invalid resources to avoid infinite loops
        if ( --TestCnt < 0 ) break;
    }
}

bool wxsExtResManager::CanOpen(const wxString& FileName)
{
    if ( m_Files.find(FileName) != m_Files.end() ) return true;
    return wxsResourceFactory::CanHandleExternal(FileName);
}

bool wxsExtResManager::Open(const wxString& FileName)
{
    if ( m_Files.find(FileName) == m_Files.end() )
    {
        wxsResource* NewResource = wxsResourceFactory::BuildExternal(FileName);
        if ( !NewResource ) return false;
        NewResource->BuildTreeEntry(wxsTree()->ExternalResourcesId());
        m_Files[FileName] = NewResource;
        NewResource->EditOpen();
        return true;
    }

    m_Files[FileName]->EditOpen();
    return true;
}

void wxsExtResManager::EditorClosed(wxsResource* Res)
{
    if ( m_ClosingAll ) return;

    for ( FilesMapI i = m_Files.begin(); i!=m_Files.end(); ++i )
    {
        if ( i->second == Res )
        {
            m_Files.erase(i);
            // wxsResource dtor calls "wxsTree()->Delete(m_TreeItemId);" itself
            // no need to call it here, outside the class
            // (which will crash if following line is uncommented)
            //wxsTree()->Delete(Res->GetTreeItemId());
            delete Res;
            if ( m_Files.empty() )
            {
                wxsTree()->DeleteExternalResourcesId();
            }
            return;
        }
    }
}

void wxsExtResManager::DeleteAll()
{
    m_ClosingAll = true;

    for ( FilesMapI i = m_Files.begin(); i!=m_Files.end(); ++i )
    {
        delete i->second;
    }
    m_Files.clear();
    wxsTree()->DeleteExternalResourcesId();

    m_ClosingAll = false;
}

wxsExtResManager wxsExtResManager::m_Singleton;
