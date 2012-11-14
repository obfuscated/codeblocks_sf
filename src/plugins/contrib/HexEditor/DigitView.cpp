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

#include "DigitView.h"

#include <logmanager.h>

#define DEFAULT_DIGIT_BITS      4
#define DEFAULT_BLOCK_BYTES     1
#define DEFAULT_LITTLE_ENDIAN   false
#define BYTE_BITS               8

DigitView::DigitView( HexEditPanel* panel )
    : HexEditViewBase( panel )
    , m_DigitBits( DEFAULT_DIGIT_BITS )
    , m_BlockBytes( DEFAULT_BLOCK_BYTES )
    , m_LittleEndian( DEFAULT_LITTLE_ENDIAN )
    , m_BitPosition( GetHighestOffsetInByte() )
{
}

void DigitView::OnPutLine(OffsetT startOffset, HexEditLineBuffer& buff, char* content, int bytes)
{
    int i;
    for ( i=0; i<bytes; i += GetBlockBytes() )
    {
        for ( int j = 0; j < GetBlockBytes(); ++j )
        {
            int offset = i + ( GetLittleEndian() ? ( GetBlockBytes() - j - 1 ) : j );

            char sn = stNormal;
            char ss = stNormal;

            bool inBlock = ( startOffset + offset ) >= GetBlockStartOffset() &&
                           ( startOffset + offset ) <  GetBlockEndOffset();

            bool inActiveByte = ( startOffset + offset ) == GetCurrentOffset();

            if ( inBlock )
            {
                sn = stCurNon;
                ss = ( GetActive() && inActiveByte ) ? stCurCar : stCurNon;
            }

            if ( offset >= bytes )
            {
                for ( int j2 = BYTE_BITS / GetDigitBits(); j2-- > 0; )
                {
                    buff.PutChar( '.', sn );
                }
            }
            else
            {
                char currentByte = content[ offset ];

                // TODO: Support for offset outside the content
                for ( int j2 = BYTE_BITS / GetDigitBits(); j2-- > 0; )
                {
                    buff.PutChar(
                        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" [ ( currentByte >> ( j2 * GetDigitBits() ) ) & GetMaxDigitValue() ],
                        ( j2 == m_BitPosition / GetDigitBits() ) ? ss : sn );
                }
            }
        }

        buff.PutChar( ' ' );
    }

    for ( ; i<(int)GetLineBytes(); i += GetBlockBytes() )
    {
        for ( int j = 0; j < GetBlockBytes(); ++j )
        {
            for ( int j2 = BYTE_BITS / GetDigitBits(); j2-- > 0; )
            {
                buff.PutChar( ' ' );
            }
        }
        buff.PutChar( ' ' );
    }
}

void DigitView::OnMoveDown()
{
    if ( GetCurrentOffset() >= GetContent()->GetSize() - GetLineBytes() ) return;
    OffsetChange( GetCurrentOffset() + GetLineBytes() );
}

void DigitView::OnMoveUp()
{
    if ( GetCurrentOffset() < GetLineBytes() ) return;
    OffsetChange( GetCurrentOffset() - GetLineBytes() );
}

void DigitView::OnMoveLeft()
{
    if ( m_BitPosition + GetDigitBits() < BYTE_BITS )
    {
        m_BitPosition += GetDigitBits();
        OffsetChange( GetCurrentOffset() );
        return;
    }

    if ( !m_LittleEndian )
    {
        if ( GetCurrentOffset() > 0 )
        {
            m_BitPosition = 0;
            OffsetChange( GetCurrentOffset() - 1 );
        }
    }
    else
    {
        int blockOffset    = (int)( GetCurrentOffset() - GetScreenStartOffset() ) % m_BlockBytes;
        OffsetT blockStart = GetCurrentOffset() - blockOffset;

        if ( ++blockOffset == m_BlockBytes )
        {
            if ( blockStart > 0 )
            {
                m_BitPosition = 0;
                OffsetChange( blockStart - m_BlockBytes );
            }
        }
        else if ( blockStart + blockOffset < GetContent()->GetSize() )
        {
            m_BitPosition = 0;
            OffsetChange( blockStart + blockOffset );
        }
        else
        {
            if ( blockStart > 0 )
            {
                m_BitPosition = 0;
                OffsetChange( blockStart - m_BlockBytes );
            }
        }
    }

}

void DigitView::OnMoveRight()
{
    if ( m_BitPosition >= GetDigitBits() )
    {
        m_BitPosition -= GetDigitBits();
        OffsetChange( GetCurrentOffset() );
        return;
    }

    if ( !m_LittleEndian )
    {
        if ( GetCurrentOffset() < GetContent()->GetSize()-1 )
        {
            m_BitPosition = GetHighestOffsetInByte();
            OffsetChange( GetCurrentOffset() + 1 );
        }
    }
    else
    {
        int blockOffset    = (int)( GetCurrentOffset() - GetScreenStartOffset() ) % m_BlockBytes;
        OffsetT blockStart = GetCurrentOffset() - blockOffset;

        if ( !blockOffset-- )
        {
            if ( blockStart + m_BlockBytes < GetContent()->GetSize() )
            {
                m_BitPosition = GetHighestOffsetInByte();
                OffsetChange( wxMin( blockStart + 2*m_BlockBytes - 1, GetContent()->GetSize() - 1 ) );
            }
        }
        else
        {
            m_BitPosition = GetHighestOffsetInByte();
            OffsetChange( blockStart + blockOffset );
        }
    }
}

void DigitView::OnProcessChar(wxChar ch)
{
    // Check if we didnt went out of the file
    if ( GetCurrentOffset() >= GetContent()->GetSize() ) return;

    // Get the value of pressed character
    int value = ( ch>='0' && ch<='9' ) ? ( ch - '0' ) :
                ( ch>='A' && ch<='Z' ) ? ( ch - 'A' + 10 ) :
                ( ch>='a' && ch<='z' ) ? ( ch - 'a' + 10 ) : -1;

    if ( value<0 || value>GetMaxDigitValue() ) return;

    bool toNextByte = m_BitPosition < GetDigitBits();

    // Calculate offset after the change
    OffsetT nextPosition = std::min( GetContent()->GetSize(), GetCurrentOffset() + ( toNextByte ? 1 : 0 ) );
    int nextBitPosition = toNextByte ? GetHighestOffsetInByte() : m_BitPosition - GetDigitBits();

    unsigned char byte = GetContent()->ReadByte( GetCurrentOffset() );

    byte &= ~ ( GetMaxDigitValue() << m_BitPosition );
    byte |= value << m_BitPosition;


    // Change affected byte
    GetContent()->WriteByte(
        FileContentBase::ExtraUndoData( this, GetCurrentOffset(), m_BitPosition, nextPosition, nextBitPosition ),
        GetCurrentOffset(),
        byte );

    OnMoveRight();

}

void DigitView::OnActivate( bool )
{

}

void DigitView::OnCalculateBlockSize( OffsetT& blockStart, OffsetT& blockEnd )
{
    OffsetT onScreenBlock = ( GetCurrentOffset() - GetScreenStartOffset() ) / m_BlockBytes;

    blockStart = GetScreenStartOffset() + onScreenBlock * m_BlockBytes;
    blockEnd   = std::min( blockStart + m_BlockBytes, GetContent()->GetSize() );
}

void DigitView::OnOffsetChange( int positionFlags )
{
    if ( positionFlags >= 0 )
    {
        m_BitPosition = ( (int)( positionFlags / GetDigitBits() ) ) * GetDigitBits();
    }
}

int DigitView::OnGetCurrentPositionFlags()
{
    return m_BitPosition;
}

void DigitView::OnGetBlockSizes( int& blockLength, int& blockBytes, int& spacing )
{
    blockLength = m_BlockBytes * ( ( BYTE_BITS + m_DigitBits - 1 ) / m_DigitBits );
    blockBytes  = m_BlockBytes;
    spacing     = 1;
}

inline unsigned char DigitView::GetMaxDigitValue()
{
    return ( 1U << GetDigitBits() ) - 1;
}

inline int DigitView::GetHighestOffsetInByte()
{
    return  ( (int)((BYTE_BITS-1) / GetDigitBits()) ) * GetDigitBits();
}

int DigitView::OnGetOffsetFromColumn( int column, int& positionFlags )
{
    int blockLength;
    int blockBytes;
    int spacing;

    // Get block settings
    OnGetBlockSizes( blockLength, blockBytes, spacing );

    // Calculate number of block in this line
    int blockNum = column / ( blockLength + spacing );

    // Calculate shift in characters inside this block
    int blockShift = column % ( blockLength + spacing );

    // Clamp to skip spacing
    blockShift = wxMin( blockShift, blockLength - 1 );

    // Calculate number of characters per one byte
    int byteChars = ( ( BYTE_BITS + m_DigitBits - 1 ) / m_DigitBits );

    // Calculate shift in bytes inside this block
    int bytesShift = blockShift / byteChars;

    // Calculate new position flags
    positionFlags = ( byteChars - 1 - blockShift % byteChars ) * m_DigitBits;

    if ( m_LittleEndian )
    {
        // Big endian require reverting byte order when displaying data
        bytesShift = blockBytes - 1 - bytesShift;
    }



    return blockNum * blockBytes + bytesShift;
}
