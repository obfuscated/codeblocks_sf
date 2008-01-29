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

#ifndef WXSEXTRESMANAGER_H
#define WXSEXTRESMANAGER_H

#include <wx/hashmap.h>
#include <wx/treectrl.h>

#include "wxsresource.h"

/** \brief Manager for external resources
 *
 * This class is responsible for editing resource files which are
 * not binded inside wxSmith project. Currently it's used to edit
 * XRC files externally.
 */
class wxsExtResManager
{
    public:

        /** \brief Checking if can handle given file type */
        bool CanOpen(const wxString& FileName);

        /** \brief Opening external file */
        bool Open(const wxString& FileName);

        /** \brief Returning singleton object */
        static wxsExtResManager* Get() { return &m_Singleton; }

        /** \brief Deleting all external resources */
        void DeleteAll();

    private:

        WX_DECLARE_STRING_HASH_MAP(wxsResource*,FilesMapT);
        typedef FilesMapT::iterator FilesMapI;

        FilesMapT m_Files;                        ///< \brief Map of opened files
        bool      m_ClosingAll;                   ///< \brief Set to true when closing all resources
        static wxsExtResManager m_Singleton;      ///< \brief Singleton object

        /** \brief Function notifying that editor has been closed */
        void EditorClosed(wxsResource* Res);

        /** \brief Ctor, private to forbid creating own instances */
        wxsExtResManager();

        /** \brief Dctor */
        ~wxsExtResManager();

        // Allow calling EditorClosed from wxsResource
        friend class wxsResource;
};

/** \brief Helper function to access external resource manager easily */
inline wxsExtResManager* wxsExtRes() { return wxsExtResManager::Get(); }

#endif
