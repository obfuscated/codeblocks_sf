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

#include "wxspredefinedids.h"

namespace
{
    struct IdEntry
    {
        const wxChar* Name;
        wxWindowID Value;
    };

    #define ID(name) { _T(#name), name },

    IdEntry IDs[] =
    {
        ID(wxID_ANY)
        ID(wxID_SEPARATOR)
        ID(wxID_OPEN)
        ID(wxID_CLOSE)
        ID(wxID_NEW)
        ID(wxID_SAVE)
        ID(wxID_SAVEAS)
        ID(wxID_REVERT)
        ID(wxID_EXIT)
        ID(wxID_UNDO)
        ID(wxID_REDO)
        ID(wxID_HELP)
        ID(wxID_PRINT)
        ID(wxID_PRINT_SETUP)
        ID(wxID_PREVIEW)
        ID(wxID_ABOUT)
        ID(wxID_HELP_CONTENTS)
        ID(wxID_HELP_COMMANDS)
        ID(wxID_HELP_PROCEDURES)
        ID(wxID_HELP_CONTEXT)
        ID(wxID_CLOSE_ALL)
        ID(wxID_PREFERENCES)
        ID(wxID_CUT)
        ID(wxID_COPY)
        ID(wxID_PASTE)
        ID(wxID_CLEAR)
        ID(wxID_FIND)
        ID(wxID_DUPLICATE)
        ID(wxID_SELECTALL)
        ID(wxID_DELETE)
        ID(wxID_REPLACE)
        ID(wxID_REPLACE_ALL)
        ID(wxID_PROPERTIES)
        ID(wxID_VIEW_DETAILS)
        ID(wxID_VIEW_LARGEICONS)
        ID(wxID_VIEW_SMALLICONS)
        ID(wxID_VIEW_LIST)
        ID(wxID_VIEW_SORTDATE)
        ID(wxID_VIEW_SORTNAME)
        ID(wxID_VIEW_SORTSIZE)
        ID(wxID_VIEW_SORTTYPE)
        ID(wxID_FILE1)
        ID(wxID_FILE2)
        ID(wxID_FILE3)
        ID(wxID_FILE4)
        ID(wxID_FILE5)
        ID(wxID_FILE6)
        ID(wxID_FILE7)
        ID(wxID_FILE8)
        ID(wxID_FILE9)
        ID(wxID_OK)
        ID(wxID_CANCEL)
        ID(wxID_APPLY)
        ID(wxID_YES)
        ID(wxID_NO)
        ID(wxID_STATIC)
        ID(wxID_FORWARD)
        ID(wxID_BACKWARD)
        ID(wxID_DEFAULT)
        ID(wxID_MORE)
        ID(wxID_SETUP)
        ID(wxID_RESET)
        ID(wxID_CONTEXT_HELP)
        ID(wxID_YESTOALL)
        ID(wxID_NOTOALL)
        ID(wxID_ABORT)
        ID(wxID_RETRY)
        ID(wxID_IGNORE)
        ID(wxID_ADD)
        ID(wxID_REMOVE)
        ID(wxID_UP)
        ID(wxID_DOWN)
        ID(wxID_HOME)
        ID(wxID_REFRESH)
        ID(wxID_STOP)
        ID(wxID_INDEX)
        ID(wxID_BOLD)
        ID(wxID_ITALIC)
        ID(wxID_JUSTIFY_CENTER)
        ID(wxID_JUSTIFY_FILL)
        ID(wxID_JUSTIFY_RIGHT)
        ID(wxID_JUSTIFY_LEFT)
        ID(wxID_UNDERLINE)
        ID(wxID_INDENT)
        ID(wxID_UNINDENT)
        ID(wxID_ZOOM_100)
        ID(wxID_ZOOM_FIT)
        ID(wxID_ZOOM_IN)
        ID(wxID_ZOOM_OUT)
        ID(wxID_UNDELETE)
        ID(wxID_REVERT_TO_SAVED)
        ID(wxID_SYSTEM_MENU)
        ID(wxID_CLOSE_FRAME)
        ID(wxID_MOVE_FRAME)
        ID(wxID_RESIZE_FRAME)
        ID(wxID_MAXIMIZE_FRAME)
        ID(wxID_ICONIZE_FRAME)
        ID(wxID_RESTORE_FRAME)
    };

    static const int IDsCount = sizeof(IDs) / sizeof(IDs[0]);
}

bool wxsPredefinedIDs::Check(const wxString& Name)
{
    // First checking if this is value
    long Tmp;
    if ( Name.ToLong(&Tmp) ) return true;

    // Second - checking array of ids
    for ( int i=0; i<IDsCount; i++ )
    {
        if ( Name == IDs[i].Name ) return true;
    }

    return false;
}

wxWindowID wxsPredefinedIDs::Value(const wxString& Name)
{
    // First trying to convert to number
    long Tmp;
    if ( Name.ToLong(&Tmp) ) return Tmp;

    // Second searching in array of ids
    for ( int i=0; i<IDsCount; i++ )
    {
        if ( Name == IDs[i].Name ) return IDs[i].Value;
    }

    return wxID_ANY;
}
