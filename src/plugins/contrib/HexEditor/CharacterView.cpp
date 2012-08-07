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

#include "CharacterView.h"

#include <algorithm>

CharacterView::CharacterView( HexEditPanel* panel ): HexEditViewBase( panel )
{
}

void CharacterView::OnActivate( bool )
{
    // We don't have to do anything
}

void CharacterView::OnOffsetChange(
    OffsetT  /*screenStartOffset*/,
    OffsetT  /*currentOffset*/,
    OffsetT /*blockStart*/,
    OffsetT /*blockEnd*/ )
{
    // commented out the fopllowing 2 lines to avoid warning, since value set the argument that can in by value --> useless
    //blockStart = currentOffset;
    //blockEnd   = currentOffset + 1;
}

void CharacterView::OnProcessChar(wxChar ch)
{
    // We skip all non-printable characters
    if ( !wxIsprint( ch ) || ch >= 0x100 ) return;

    // Check if we didnt went out of the file
    if ( GetCurrentOffset() >= GetContent()->GetSize() ) return;

    // Calculate offset after the change
    OffsetT nextPosition = std::min( GetContent()->GetSize(), GetCurrentOffset() + 1 );

    // Change affected byte
    GetContent()->WriteByte(
        FileContentBase::ExtraUndoData( this, GetCurrentOffset(), 0, nextPosition, 0 ),
        GetCurrentOffset(),
        (unsigned char) ch );

    OnMoveRight();
}

void CharacterView::OnMoveLeft()
{
    if ( GetCurrentOffset() == 0 ) return;
    OffsetChange( GetCurrentOffset() - 1 );
}

void CharacterView::OnMoveRight()
{
    if ( GetCurrentOffset() >= GetContent()->GetSize()-1 ) return;
    OffsetChange( GetCurrentOffset() + 1 );
}

void CharacterView::OnMoveUp()
{
    if ( GetCurrentOffset() < GetLineBytes() ) return;
    OffsetChange( GetCurrentOffset() - GetLineBytes() );
}

void CharacterView::OnMoveDown()
{
    if ( GetCurrentOffset() >= GetContent()->GetSize() - GetLineBytes() ) return;
    OffsetChange( GetCurrentOffset() + GetLineBytes() );
}

void CharacterView::OnPutLine( OffsetT startOffset, HexEditLineBuffer& buff, char* content, int bytes )
{
    for ( int i=0; i<bytes; ++i )
    {
        buff.PutChar(
            Strip( *content++ ),
            ( startOffset+i != GetCurrentOffset() ) ? stNormal :
            GetActive() ? stCurCar : stCurNon
        );
    }

    int lineBytes = (int)GetLineBytes();

    for ( int i=bytes; i<lineBytes; ++i )
    {
        buff.PutChar( ' ' );
    }
}

void CharacterView::OnGetBlockSizes( int& blockLength, int& blockBytes, int& spacing )
{
    blockLength = 1;    // One char per byte
    blockBytes  = 1;    // One byte in block
    spacing     = 0;    // No spacing between blocks
}

int CharacterView::OnGetOffsetFromColumn( int column, int& positionFlags )
{
    positionFlags = 0;
    return column;
}

inline char CharacterView::Strip( char ch )
{
    if ( !isprint( ch ) ) return ' ';
    if ( ch >= 0x7F ) return ' ';
    return ch;
}
