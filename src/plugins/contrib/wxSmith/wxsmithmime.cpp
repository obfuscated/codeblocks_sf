/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsmithmime.h"

#include "wxsmith.h"
#include "wxsproject.h"
#include "wxsextresmanager.h"

// TODO: Do not access wxSmith's maps directly

wxSmithMime::wxSmithMime()
{
}

bool wxSmithMime::CanHandleFile(const wxString& FileName) const
{
    if ( !wxsPlugin() ) return false;

    // Scanning for projects using this file
    for ( wxSmith::ProjectMapI i = wxsPlugin()->m_ProjectMap.begin();
          i!=wxsPlugin()->m_ProjectMap.end();
          ++i )
    {
        wxsProject* Proj = (*i).second;
        if ( Proj->CanOpenEditor(FileName) )
        {
            return true;
        }
    }

    if ( wxsExtRes()->CanOpen(FileName) )
    {
        return true;
    }

    return false;
}

int wxSmithMime::OpenFile(const wxString& FileName)
{
    if ( !wxsPlugin() ) return 1;

    // Scanning for projects using this file
    for ( wxSmith::ProjectMapI i = wxsPlugin()->m_ProjectMap.begin();
          i!=wxsPlugin()->m_ProjectMap.end();
          ++i )
    {
        wxsProject* Proj = (*i).second;
        if ( Proj->TryOpenEditor(FileName) )
        {
            return 0;
        }
    }

    if ( wxsExtRes()->Open(FileName) )
    {
        return 0;
    }

    return 1;
}
