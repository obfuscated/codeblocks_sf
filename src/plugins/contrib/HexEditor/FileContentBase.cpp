/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008 Bartlomiej Swiecki
*
* HexEditor plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* HexEditor pluging is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision:$
* $Id:$
* $HeadURL:$
*/

#include "FileContentBase.h"
#include "FileContentBuffered.h"

#include <wx/file.h>

// Limit buffered files to 4 MBytes
static const FileContentBase::OffsetT maxBufferedSize = 1024 * 1024 * 4;

FileContentBase* FileContentBase::BuildInstance( const wxString& fileName )
{
    wxFile fl( fileName );
    if ( !fl.IsOpened() )
    {
        return NULL;
    }

    if ( fl.Length() <= maxBufferedSize )
    {
        return new FileContentBuffered();
    }

    return NULL;
}
