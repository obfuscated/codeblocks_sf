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
        FileContentBase();

        /** \brief Dctor */
        virtual ~FileContentBase();

        /** \brief Reading the data from the file */
        virtual bool ReadFile( const wxString& fileName ) = 0;

        /** \brief Writing the data to the file */
        virtual bool WriteFile( const wxString& fileName ) = 0;

        /** \brief Check if file was modified */
        bool Modified();

        /** \brief Force current state to be modified or not */
        void SetModified( bool modified );

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
        OffsetT Write( const ExtraUndoData& extraUndoData, const void* buff, OffsetT position, OffsetT length );

        /** \brief Deleting some range of data
         *  \param position beginning position of block to remove
         *  \param length length of block to remove in bytes
         *  \return number of bytes removed
         */
        OffsetT Remove( const ExtraUndoData& extraUndoData, OffsetT position, OffsetT length );

        /** \brief Inserting data
         *  \param position location in content where new data will be added
         *  \param length size of block to add in bytes
         *  \param data buffer with data to use for new location, if NULL, new block will be zero-filled
         *  \return number of bytes added
         */
        OffsetT Add( const ExtraUndoData& extraUndoData, OffsetT position, OffsetT length, void* data = 0 );

        /** \brief Check if we can undo */
        bool CanUndo();

        /** \brief Check if we can redo */
        bool CanRedo();

        /** \brief Do undo
         *  \return undo data passed while doing the modification we undo
         */
        const ExtraUndoData* Undo();

        /** \brief Do redo
         *  \return undo data passed while doing the modification we redo
         */
        const ExtraUndoData* Redo();

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

    protected:

        /** \brief Base class for representation of one change inside of the code */
        class ModificationData
        {
            public:

                /** \brief Ctor */
                inline ModificationData() {}

                /** \brief Dctor forcing the class to have vtable */
                virtual ~ModificationData() {}

                /** \brief Apply the modification */
                virtual void Apply() = 0;

                /** \brief Revert the modification */
                virtual void Revert() = 0;

                /** \brief Get the length of modification */
                virtual OffsetT Length() = 0;

            private:

                ModificationData*   m_Next;     ///< \brief Next element in modificatoin list
                ModificationData*   m_Prev;     ///< \brief Previous element in modification list
                ExtraUndoData       m_Data;     ///< \brief Extra data used outside

                friend class FileContentBase;   ///< \brief To allow operations on m_Next and m_Prev
        };

        /** \brief Create modification object for change operation */
        virtual ModificationData* BuildChangeModification( OffsetT position, OffsetT length, const void* data = 0 ) = 0;

        /** \brief Create modification object for data add operation */
        virtual ModificationData* BuildAddModification( OffsetT position, OffsetT length, const void* data = 0 ) = 0;

        /** \brief Create modification object for data remove operation */
        virtual ModificationData* BuildRemoveModification( OffsetT position, OffsetT length ) = 0;

        /** \brief Notify that undo has been saved at current position */
        inline void UndoNotifySaved() { m_UndoSaved = m_UndoCurrent; }

        /** \brief Clear all undo history */
        inline void UndoClear() { RemoveUndoFrom( m_UndoBuffer ); m_UndoCurrent = 0; m_UndoSaved = 0; }

    private:

        ModificationData*   m_UndoBuffer;       ///< \brief Undo buffer
        ModificationData*   m_UndoLast;         ///< \brief Last element in undo buffer
        ModificationData*   m_UndoCurrent;      ///< \brief Current undo position
        ModificationData*   m_UndoSaved;        ///< \brief Position of element where the file was "saved" last time (or opened)

        /** \brief Dummy class to simulate invalid bur also harmless entry in undo buffer */
        class InvalidModificationData: public ModificationData
        {
            void    Apply () { }
            void    Revert() { }
            OffsetT Length() { return 0; }
        };

        static InvalidModificationData m_UndoInvalid;


        void InsertAndApplyModification( ModificationData* mod );
        void RemoveUndoFrom( ModificationData* mod );
        void ApplyModification( ModificationData* mod );
        void RevertModification( ModificationData* mod );
};

#endif
