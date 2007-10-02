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

#ifndef WXSMITHMIME_H
#define WXSMITHMIME_H

#include <cbplugin.h>

/** \brief Helper plugin dealing with mime types */
class wxSmithMime : public cbMimePlugin
{
    public:

        /** \brief Ctor */
        wxSmithMime();

        /** \brief Returning true if can handle this file
         *
         * This function will handle two file types:
         *  - WXS files (will be opened only when project file is also opened)
         *  - XRC files (when there's project associated opened, it will open
         *             it's editor, when there's no such project, XRC will
         *             be edited externally)
         */
        virtual bool CanHandleFile(const wxString& filename) const;

        /** \brief Opening file
         *
         * If this is wxs file and it's project is opened, proper editor will be
         * opened / selected for xrc files, new editor withour project will be used
         */
        virtual int OpenFile(const wxString& filename);

        /** \brief We do not handle everything */
        virtual bool HandlesEverything() const  { return false; }
};

#endif
