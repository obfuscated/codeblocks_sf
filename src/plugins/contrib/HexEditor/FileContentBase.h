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

#ifndef FILECONTENTBASE_H
#define FILECONTENTBASE_H

#include <wx/string.h>

class HexEditViewBase;

/** \brief Abstract class for managing content of some file */
class FileContentBase
{
    public:

        /** \brief Offset within the file */
        typedef unsigned long long OffsetT;

        /** \brief Structure used as a base of undo data */
        struct ExtraUndoData
        {
            HexEditViewBase* m_View;        ///< \brief View which was active while performing the change
            OffsetT          m_PosBefore;   ///< \brief Position before the change
            int              m_PosBeforeF;  ///< \brief View-defined flags before the change (for example bit no before the change)
            OffsetT          m_PosAfter;    ///< \brief Position after the change
            int              m_PosAfterF;   ///< \brief View-defined flags after the change

            inline ExtraUndoData( HexEditViewBase* view, OffsetT posBefore, int posBeforeF, OffsetT posAfter, int posAfterF )
                : m_View      ( view       )
                , m_PosBefore ( posBefore  )
                , m_PosBeforeF( posBeforeF )
                , m_PosAfter  ( posAfter   )
                , m_PosAfterF ( posAfterF  )
            {
            }

            inline ExtraUndoData() {}
        };

        /** \brief Ctor */
        FileContentBase() {}

        /** \brief Dctor */
        virtual ~FileContentBase() {}

        /** \brief Reading the data from the file */
        virtual bool ReadFile( const wxString& fileName ) = 0;

        /** \brief Writing the data to the file */
        virtual bool WriteFile( const wxString& fileName ) = 0;

        /** \brief Check if file was modified */
        virtual bool Modified( ) = 0;

        /** \brief Force current state to be modified or not */
        virtual void SetModified( bool modified ) = 0;

        /** \brief Getting size of the content */
        virtual OffsetT GetSize() = 0;

        /** \brief Reading some part of data
         *  \param buff memory location where to read data into
         *  \param position location of data in the content
         *  \param length number of bytes to read
         *  \return number of bytes actually read
         */
        virtual OffsetT Read( void* buff, OffsetT position, OffsetT length ) = 0;

        /** \brief Writing some part of data
         *  \param buff memory location with new content
         *  \param position location of data in the content
         *  \param length number of bytes to read
         *  \return number of bytes actually written
         */
        virtual OffsetT Write( const ExtraUndoData& extraUndoData, const void* buff, OffsetT position, OffsetT length ) = 0;

        /** \brief Deleting some range of data
         *  \param position beginning position of block to remove
         *  \param length length of block to remove in bytes
         *  \return number of bytes removed
         */
        virtual OffsetT Remove( const ExtraUndoData& extraUndoData, OffsetT position, OffsetT length ) = 0;

        /** \brief Inserting data
         *  \param position location in content where new data will be added
         *  \param length size of block to add in bytes
         *  \param data buffer with data to use for new location, if NULL, new block will be zero-filled
         *  \return number of bytes added
         */
        virtual OffsetT Add( const ExtraUndoData& extraUndoData, OffsetT position, OffsetT length, void* data = 0 ) = 0;

        /** \brief Check if we can undo */
        virtual bool CanUndo() = 0;

        /** \brief Check if we can redo */
        virtual bool CanRedo() = 0;

        /** \brief Do undo
         *  \return undo data passed while doing the modification we undo
         */
        virtual const ExtraUndoData* Undo() = 0;

        /** \brief Do redo
         *  \return undo data passed while doing the modification we redo
         */
        virtual const ExtraUndoData* Redo() = 0;

        /** \brief Helper function to read one byte */
        inline unsigned char ReadByte( OffsetT position )
        {
            unsigned char val = 0;
            return ( Read( &val, position, 1 ) == 1 ) ? val : 0;
        }

        /** \brief Helper function to write one byte */
        inline bool WriteByte( const ExtraUndoData& extraUndoData, OffsetT position, unsigned char val )
        {
            return Write( extraUndoData, &val, position, 1 ) == 1;
        }

        /** \brief Build implementation of this class */
        static FileContentBase* BuildInstance( const wxString& fileName );
};

#endif
