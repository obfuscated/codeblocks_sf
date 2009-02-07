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

#include "HexEditViewBase.h"
#include "HexEditPanel.h"

HexEditViewBase::HexEditViewBase( HexEditPanel* panel )
    : m_IsActive( false )
    , m_ScreenStartOffset( 0 )
    , m_CurrentOffset( 0 )
    , m_BlockStartOffset( 0 )
    , m_BlockEndOffset( 0 )
    , m_Panel( panel )
{
}

HexEditViewBase::~HexEditViewBase()
{
}

bool HexEditViewBase::SetActive(bool makeMeActive)
{
    if ( makeMeActive == m_IsActive ) return m_IsActive;

    OnActivate( makeMeActive );
    m_IsActive = makeMeActive;
    return !m_IsActive;
}

void HexEditViewBase::CalculateBlockSize( OffsetT screenStartOffset, OffsetT currentOffset, OffsetT& blockStart, OffsetT& blockEnd )
{
    m_ScreenStartOffset = screenStartOffset;
    m_CurrentOffset     = currentOffset;
    OnCalculateBlockSize( blockStart, blockEnd );
}

void HexEditViewBase::JumpToOffset( OffsetT screenStartOffset, OffsetT currentOffset, OffsetT blockStart, OffsetT blockEnd, int flags )
{
    m_BlockStartOffset  = blockStart;
    m_BlockEndOffset    = blockEnd;
    m_CurrentOffset     = currentOffset;
    m_ScreenStartOffset = screenStartOffset;

    OnOffsetChange( flags );
}

void HexEditViewBase::PutChar(wxChar ch)
{
    if ( !m_IsActive ) return;
    OnProcessChar( ch );
}

void HexEditViewBase::MoveLeft()
{
    if ( !m_IsActive ) return;
    OnMoveLeft();
}

void HexEditViewBase::MoveRight()
{
    if ( !m_IsActive ) return;
    OnMoveRight();
}

void HexEditViewBase::MoveUp()
{
    if ( !m_IsActive ) return;
    OnMoveUp();
}

void HexEditViewBase::MoveDown()
{
    if ( !m_IsActive ) return;
    OnMoveDown();
}

void HexEditViewBase::PutLine( OffsetT offs, HexEditLineBuffer& buff, char* content, int bytes )
{
    OnPutLine( offs, buff, content, bytes );
}

void HexEditViewBase::GetBlockSizes( int& blockLength, int& blockBytes, int& spacing )
{
    OnGetBlockSizes( blockLength, blockBytes, spacing );
}

void HexEditViewBase::ContentChange()
{
    m_Panel->ViewNotifyContentChange();
}

void HexEditViewBase::OffsetChange(OffsetT currentOffset)
{
    m_CurrentOffset = currentOffset;
    m_Panel->ViewNotifyOffsetChange( currentOffset );
}

FileContentBase * HexEditViewBase::GetContent()
{
    return m_Panel->m_Content;
}

unsigned int HexEditViewBase::GetLineBytes()
{
    return (unsigned int)m_Panel->m_LineBytes;
}

void HexEditViewBase::OnCalculateBlockSize( OffsetT& blockStart, OffsetT& blockEnd )
{
    blockStart = m_CurrentOffset;
    blockEnd   = m_CurrentOffset + 1;
}

void HexEditViewBase::OnOffsetChange( int )
{
}

int HexEditViewBase::OnGetCurrentPositionFlags()
{
    return 0;
}

int HexEditViewBase::GetOffsetFromColumn( int column, int& positionFlags )
{
    return OnGetOffsetFromColumn( column, positionFlags );
}

