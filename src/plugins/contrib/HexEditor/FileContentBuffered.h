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

        /** \brief Check if file was modified */
        bool Modified();

        /** \brief Getting size of the content */
        OffsetT GetSize();

        /** \brief Reading some part of data */
        OffsetT Read( void* buff, OffsetT position, OffsetT length );

        /** \brief Writing some part of data */
        OffsetT Write( const ExtraUndoData& extraUndoData, const void* buff, OffsetT position, OffsetT length );

        /** \brief Deleting some range of data */
        OffsetT Remove( const ExtraUndoData& extraUndoData, OffsetT position, OffsetT length );

        /** \brief Inserting data */
        OffsetT Add( const ExtraUndoData& extraUndoData, OffsetT position, OffsetT length, void* data = 0 );

        /** \brief Check if we can undo */
        bool CanUndo();

        /** \brief Check if we can redo */
        bool CanRedo();

        /** \brief Do undo */
        const ExtraUndoData* Undo();

        /** \brief Do redo */
        const ExtraUndoData* Redo();

    private:

        struct ModificationData
        {
            enum typeEnum
            {
                change,         ///< \brief Some content was changed
                added,          ///< \brief Some data was inserted
                removed,        ///< \brief Some data was removed
            };

            typeEnum            m_Type;
            OffsetT             m_Position;
            std::vector< char > m_OldData;
            std::vector< char > m_NewData;

            ModificationData*   m_Next;
            ModificationData*   m_Prev;

            ExtraUndoData       m_ExtraData;
        };


        std::vector< char > m_Buffer;
        ModificationData*   m_UndoBuffer;
        ModificationData*   m_UndoLast;
        ModificationData*   m_UndoCurrent;
        ModificationData*   m_UndoSaved;

        static ModificationData m_UndoInvalid;

        void InsertAndApplyModification( ModificationData* mod );
        void RemoveUndoFrom( ModificationData* mod );
        void ApplyModification( ModificationData* mod );
        void RevertModification( ModificationData* mod );
};

#endif
