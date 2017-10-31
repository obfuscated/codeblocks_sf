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

#ifndef HEXEDITVIEWBASE_H
#define HEXEDITVIEWBASE_H

#include "FileContentBase.h"
#include "HexEditLineBuffer.h"

class HexEditPanel;


/** \brief Base class for the view in editor area
 *
 * This is base interface used to represent views inside of the editor
 * (like the hex view or character preview, I don't know anything about
 * other usages at the momment of writing this peace of code ;) )
 *
 * Offtopic: There's strange story about how this class was created
 *  I tried to make some nice solution for the problem with lack
 *  of editor's flexibility. I tied at least four solutions and
 *  I was dropping the m one after another since more problems
 *  were found while coding. Finally I was so angry about that
 *  that I said "God help me" ... and the solution was just
 *  few hours later, jumped into my mind just with all those
 *  details needed to make it work. Yeah, for me it's a proof
 *  that God really exists and that He cares about the
 *  Open-Source community :)
 */
class HexEditViewBase
{
    public:

        typedef FileContentBase::OffsetT OffsetT;

        /** \brief Ctor */
        HexEditViewBase( HexEditPanel* panel );

        /** \brief Dctor */
        virtual ~HexEditViewBase();

        /** \brief Activate this view
         *  \return previous active state
         */
        bool SetActive( bool makeMeActive = true );

        /** \brief Get the active state */
        bool GetActive() { return m_IsActive; }

        /** \brief Calculate size of block at current offset's position */
        void CalculateBlockSize(
            OffsetT  screenStartOffset,
            OffsetT  currentOffset,
            OffsetT& blockStart,
            OffsetT& blockEnd );

        /** \brief Jump to given offset and calculate range for current data block
         *  \param screenStartOffset offset of first data byte shown on the screen
         *  \param currentOffset offset of current position
         *  \param blockStart here current block start is put and new value will be stored if wider range is required
         *  \param blockEnd here current block end is put and new value will be storeed if wider range is required
         *  \param positionFlags flags of current position (like which bit of the byte is currently edited), this
         *         argument will be -1 if called can not deduce it's value
         */
        void JumpToOffset(
            OffsetT screenStartOffset,
            OffsetT currentOffset,
            OffsetT blockStart,
            OffsetT blockEnd,
            int     positionFlags );

        /** \brief Get scrren start offset */
        OffsetT GetScreenStartOffset() { return m_ScreenStartOffset; }

        /** \brief Get current position offset */
        OffsetT GetCurrentOffset() { return m_CurrentOffset; }

        /** \brief Get flags for current offset */
        int GetCurrentPositionFlags() { return OnGetCurrentPositionFlags(); }

        /** \brief Get block start offset */
        OffsetT GetBlockStartOffset() { return m_BlockStartOffset; }

        /** \brief Get block end offset */
        OffsetT GetBlockEndOffset() { return m_BlockEndOffset; }

        /** \brief Drop a character into this view
         *  \note If the view is not active, character will not be processed
         */
        void PutChar( wxChar ch );

        /** \brief Move cursor left */
        void MoveLeft();

        /** \brief Move cursor right */
        void MoveRight();

        /** \brief Move cursor up */
        void MoveUp();

        /** \brief Move cursor down */
        void MoveDown();

        /** \brief Put given line into line buffer */
        void PutLine( OffsetT lineStartOffset, HexEditLineBuffer& buffer, char* content, int bytes );

        /** \brief Get block sizez */
        void GetBlockSizes( int& blockLength, int& blockBytes, int& spacing );

        /** \brief Detect offset from position in line */
        int GetOffsetFromColumn( int column, int& positionFlags );

    protected:

        /** \brief Called when switching the active state */
        virtual void OnActivate( bool becomesActive ) = 0;

        /** \brief Detect block start and end at given offset */
        virtual void OnCalculateBlockSize( OffsetT& blockStart, OffsetT& blockEnd );

        /** \brief Called when changing current cursor offset */
        virtual void OnOffsetChange( int positionFlags );

        /** \brief Called when fetching current position flags */
        virtual int OnGetCurrentPositionFlags();

        /** \brief Called when character is sent to this view */
        virtual void OnProcessChar( wxChar ) = 0;

        /** \brief Called when moving left */
        virtual void OnMoveLeft() = 0;

        /** \brief Called when moving right */
        virtual void OnMoveRight() = 0;

        /** \brief Called when moving up */
        virtual void OnMoveUp() = 0;

        /** \brief Called when moving down */
        virtual void OnMoveDown() = 0;

        /** \brief Called when there's need to fill line buffer for this content */
        virtual void OnPutLine( OffsetT startOffset, HexEditLineBuffer& buff, char* content, int bytes ) = 0;

        /** \brief Fetching informations about current data block sizes
         *  \param blockLength length of block in characters
         *  \param blockBytes number of bytes in one block
         *  \param spacing number of characters splitting two blocks next to each other
         */
        virtual void OnGetBlockSizes( int& blockLength, int& blockBytes, int& spacing ) = 0;

        /** \brief Calculate offset from the beginning of the line at given position */
        virtual int OnGetOffsetFromColumn( int column, int& positionFlags ) = 0;


        /** \brief Fetch assigned file content */
        FileContentBase* GetContent();

        /** \brief Get length of one line */
        unsigned int GetLineBytes();

        /** \brief Notify about offset change
         *  \param currentOffset new current offset
         */
        void OffsetChange( OffsetT currentOffset );

        /** \brief Notify about content change */
        void ContentChange();


    private:

        bool    m_IsActive;             ///< \brief Set to true if this view is active (has keyboard focus)
        OffsetT m_ScreenStartOffset;    ///< \brief Offset of first data byte shown on the screen
        OffsetT m_CurrentOffset;        ///< \brief Offset of current byte
        OffsetT m_BlockStartOffset;     ///< \brief Start offset of current data block
        OffsetT m_BlockEndOffset;       ///< \brief End offset of current data block
        HexEditPanel* m_Panel;          ///< \brief Owning panel

};

#endif
