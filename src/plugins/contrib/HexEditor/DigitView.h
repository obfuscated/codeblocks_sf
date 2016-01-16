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

#ifndef DIGITVIEW_H
#define DIGITVIEW_H

#include "HexEditViewBase.h"

/** \brief class representing digits view in hex editor */
class DigitView: public HexEditViewBase
{
    public:

        /** \brief Ctor */
        DigitView( HexEditPanel* panel );

        /** \brief Get current number of bits in one digit */
        inline int GetDigitBits() { return m_DigitBits; }

        /** \brief Set current number of bits in one digit */
        inline void SetDigitBits( int bits ) { m_DigitBits = bits; }

        /** \brief Get current endianess settings */
        inline bool GetLittleEndian() { return m_LittleEndian; }

        /** \brief Set current endianess settings */
        inline void SetLittleEndian( bool le ) { m_LittleEndian = le; }

        /** \brief Get number of bytes inside one block */
        inline int GetBlockBytes() { return m_BlockBytes; }

        /** \brief Set number of bytes inside one block */
        inline void SetBlockBytes( int blockBytes ) { m_BlockBytes = blockBytes; }

    private:

        /** \brief Called when switching the active state */
        virtual void OnActivate( bool becomesActive );

        /** \brief Detect block start and end at given offset */
        virtual void OnCalculateBlockSize( OffsetT& blockStart, OffsetT& blockEnd );

        /** \brief Called when character is sent to this view */
        virtual void OnProcessChar( wxChar ch );

        /** \brief Called when moving left */
        virtual void OnMoveLeft();

        /** \brief Called when moving right */
        virtual void OnMoveRight();

        /** \brief Called when moving up */
        virtual void OnMoveUp();

        /** \brief Called when moving down */
        virtual void OnMoveDown();

        /** \brief Called when there's need to fill line buffer for this content */
        virtual void OnPutLine( OffsetT startOffset, HexEditLineBuffer& buff, char* content, int bytes );

        /** \brief Called when changing current cursor offset */
        virtual void OnOffsetChange( int positionFlags );

        /** \brief Called when fetching current position flags */
        virtual int OnGetCurrentPositionFlags();

        /** \brief Get block sizes */
        virtual void OnGetBlockSizes( int& blockLength, int& blockBytes, int& spacing );

        /** \brief Calculate offset from the beginning of the line at given position */
        virtual int OnGetOffsetFromColumn( int column, int& positionFlags );

        /** \brief Get maximum value of one digit */
        inline unsigned char GetMaxDigitValue();

        /** \brief Calculate highest offset inside of one byte  */
        inline int GetHighestOffsetInByte();


        int  m_DigitBits;    ///< \brief Bits per one digit, should be 1 or 4, 2 also accepted but not practical
        int  m_BlockBytes;   ///< \brief Bytes per one block
        bool m_LittleEndian; ///< \brief True if we show data in little endian

        int m_BitPosition;  ///< \brief Position of first bit starting selected digit
};

#endif
