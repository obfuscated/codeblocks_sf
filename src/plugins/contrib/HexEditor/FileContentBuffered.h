/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008-2009 Bartlomiej Swiecki
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
* along with HexEditor. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef FILECONTENTBUFFERED_H
#define FILECONTENTBUFFERED_H

#include "FileContentBase.h"
#include <vector>
#include <list>

/** \brief Class responsible for managing content of the file */
class FileContentBuffered: public FileContentBase
{
    public:

        /** \brief Ctor */
        FileContentBuffered();

        /** \brief Dctor */
        ~FileContentBuffered();

        /** \brief Reading the data */
        bool ReadFile( const wxString& fileName );

        /** \brief Writing the data */
        bool WriteFile( const wxString& fileName );

        /** \brief Getting size of the content */
        OffsetT GetSize();

        /** \brief Reading some part of data */
        OffsetT Read( void* buff, OffsetT position, OffsetT length );

    protected:

        /** \brief Create modification object for change operation */
        virtual ModificationData* BuildChangeModification( OffsetT position, OffsetT length, const void* data = 0 );

        /** \brief Create modification object for data add operation */
        virtual ModificationData* BuildAddModification( OffsetT position, OffsetT length, const void* data = 0 );

        /** \brief Create modification object for data remove operation */
        virtual ModificationData* BuildRemoveModification( OffsetT position, OffsetT length );

    private:

        /** \brief Forward-declaration for internal modification data */
        class IntModificationData;

        std::vector< char > m_Buffer;
};

#endif
