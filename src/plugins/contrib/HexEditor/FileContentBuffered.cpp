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

#include "FileContentBuffered.h"

#include <wx/file.h>
#include <algorithm>

FileContentBuffered::ModificationData FileContentBuffered::m_UndoInvalid;

FileContentBuffered::FileContentBuffered()
    : m_UndoBuffer ( 0 )
    , m_UndoLast   ( 0 )
    , m_UndoCurrent( 0 )
    , m_UndoSaved  ( 0 )
{
}

FileContentBuffered::~FileContentBuffered()
{
    RemoveUndoFrom( m_UndoBuffer );
    m_UndoCurrent = 0;
}

void FileContentBuffered::InsertAndApplyModification( ModificationData* mod )
{
    RemoveUndoFrom( m_UndoCurrent );

    mod->m_Next = 0;
    mod->m_Prev = m_UndoLast;
    if ( m_UndoLast )
    {
        m_UndoLast->m_Next = mod;
    }
    else
    {
        m_UndoBuffer = mod;
    }
    m_UndoLast = mod;

    if ( !m_UndoSaved )
    {
        m_UndoSaved = mod;
    }

    ApplyModification( mod );
    m_UndoCurrent = 0;
}

void FileContentBuffered::ApplyModification( ModificationData* mod )
{
    switch ( mod->m_Type )
    {
        case ModificationData::added:
        {
            assert( m_Buffer.size() >= mod->m_Position );
            m_Buffer.insert( m_Buffer.begin() + mod->m_Position, mod->m_NewData.begin(), mod->m_NewData.end() );
            break;
        }

        case ModificationData::removed:
        {
            assert( m_Buffer.size() > mod->m_Position );
            assert( m_Buffer.size() >= mod->m_Position + mod->m_OldData.size() );
            m_Buffer.erase( m_Buffer.begin() + mod->m_Position, m_Buffer.begin() + mod->m_Position + mod->m_OldData.size() );
            break;
        }

        case ModificationData::change:
        {
            assert( m_Buffer.size() > mod->m_Position );
            assert( m_Buffer.size() >= mod->m_Position + mod->m_NewData.size() );
            assert( mod->m_OldData.size() == mod->m_NewData.size() );

            std::copy( mod->m_NewData.begin(), mod->m_NewData.end(), m_Buffer.begin() + mod->m_Position );
            break;
        }
    }
}

void FileContentBuffered::RevertModification( ModificationData* mod )
{
    switch ( mod->m_Type )
    {
        case ModificationData::removed:
        {
            assert( m_Buffer.size() >= mod->m_Position );
            m_Buffer.insert( m_Buffer.begin() + mod->m_Position, mod->m_OldData.begin(), mod->m_OldData.end() );
            break;
        }

        case ModificationData::added:
        {
            assert( m_Buffer.size() > mod->m_Position );
            assert( m_Buffer.size() >= mod->m_Position + mod->m_NewData.size() );
            m_Buffer.erase( m_Buffer.begin() + mod->m_Position, m_Buffer.begin() + mod->m_Position + mod->m_NewData.size() );
            break;
        }

        case ModificationData::change:
        {
            assert( m_Buffer.size() > mod->m_Position );
            assert( m_Buffer.size() >= mod->m_Position + mod->m_OldData.size() );
            assert( mod->m_OldData.size() == mod->m_NewData.size() );

            std::copy( mod->m_OldData.begin(), mod->m_OldData.end(), m_Buffer.begin() + mod->m_Position );
            break;
        }
    }
}

void FileContentBuffered::RemoveUndoFrom( ModificationData* mod )
{
    if ( !mod ) return;

    m_UndoLast = mod->m_Prev;
    if ( m_UndoLast )
    {
        m_UndoLast->m_Next = 0;
    }
    else
    {
        m_UndoBuffer = 0;
    }

    while ( mod )
    {
        if ( mod == m_UndoSaved )
        {
            m_UndoSaved = &m_UndoInvalid;
        }

        ModificationData* tmp = mod;
        mod = mod->m_Next;
        delete tmp;
    }
}

FileContentBuffered::OffsetT FileContentBuffered::Add( const ExtraUndoData& extraUndoData, OffsetT position, OffsetT length, void* data)
{
    if ( !length ) return 0;

    ModificationData* mod = new ModificationData;
    mod->m_Type      = ModificationData::added;
    mod->m_Position  = position;
    mod->m_ExtraData = extraUndoData;

    mod->m_NewData.resize( length );
    if ( data )
    {
        std::copy( (char*)data, (char*)data + length, mod->m_NewData.begin() );
    }

    InsertAndApplyModification( mod );

    return length;
}

FileContentBuffered::OffsetT FileContentBuffered::Remove( const ExtraUndoData& extraUndoData,OffsetT position, OffsetT length)
{
    if ( position > m_Buffer.size() ) return 0;
    if ( position + length > m_Buffer.size() )
    {
        length = m_Buffer.size() - position;
        if ( !length ) return 0;
    }

    ModificationData* mod = new ModificationData;
    mod->m_Type      = ModificationData::removed;
    mod->m_Position  = position;
    mod->m_ExtraData = extraUndoData;

    mod->m_OldData.resize( length );
    std::copy( m_Buffer.begin() + position, m_Buffer.begin() + position + length, mod->m_OldData.begin() );

    InsertAndApplyModification( mod );

    return length;
}

FileContentBuffered::OffsetT FileContentBuffered::Write( const ExtraUndoData& extraUndoData, const void* buff, OffsetT position, OffsetT length )
{
    if ( !buff ) return 0;
    if ( position > m_Buffer.size() ) return 0;
    if ( position + length > m_Buffer.size() )
    {
        length = m_Buffer.size() - position;
        if ( !length ) return 0;
    }

    ModificationData* mod = new ModificationData;
    mod->m_Type     = ModificationData::change;
    mod->m_Position = position;
    mod->m_OldData.resize( length );
    mod->m_NewData.resize( length );
    mod->m_ExtraData = extraUndoData;

    std::copy( m_Buffer.begin() + position, m_Buffer.begin() + position + length, mod->m_OldData.begin() );
    std::copy( (char*)buff, (char*)buff + length, mod->m_NewData.begin() );

    InsertAndApplyModification( mod );

    return length;
}

FileContentBuffered::OffsetT FileContentBuffered::Read( void* buff, OffsetT position, OffsetT length)
{
    if ( position > m_Buffer.size() ) return 0;
    if ( position + length > m_Buffer.size() )
    {
        length = m_Buffer.size() - position;
        if ( !length ) return 0;
    }

    memcpy( buff, &m_Buffer[ position ], length );
    return length;
}

FileContentBuffered::OffsetT FileContentBuffered::GetSize()
{
    return m_Buffer.size();
}

bool FileContentBuffered::Modified()
{
    return m_UndoSaved != m_UndoCurrent;
}

void FileContentBuffered::SetModified( bool modified )
{
    m_UndoSaved = modified ? &m_UndoInvalid : m_UndoCurrent;
}

bool FileContentBuffered::WriteFile(const wxString& fileName)
{
    wxFile fl( fileName, wxFile::write );
    if ( !fl.IsOpened() ) return false;
    if ( fl.Write( &m_Buffer[0], m_Buffer.size() ) == m_Buffer.size() )
    {
        m_UndoSaved = m_UndoCurrent;
        return true;
    }
    return false;
}

bool FileContentBuffered::ReadFile(const wxString& fileName)
{
    wxFile fl( fileName, wxFile::read );
    if ( !fl.IsOpened() ) return false;

    m_Buffer.resize( fl.Length() );

    RemoveUndoFrom( m_UndoBuffer );
    m_UndoCurrent = 0;
    m_UndoSaved = 0;

    return (size_t)fl.Read( &m_Buffer[0], m_Buffer.size() ) == m_Buffer.size();
}

const FileContentBuffered::ExtraUndoData* FileContentBuffered::Redo()
{
    if ( !m_UndoCurrent ) return 0;

    ApplyModification( m_UndoCurrent );
    const ExtraUndoData* ret = &m_UndoCurrent->m_ExtraData;

    m_UndoCurrent = m_UndoCurrent->m_Next;

    return ret;
}

const FileContentBuffered::ExtraUndoData* FileContentBuffered::Undo()
{
    if ( m_UndoCurrent == m_UndoBuffer ) return 0;

    if ( !m_UndoCurrent )
    {
        m_UndoCurrent = m_UndoLast;
        assert ( m_UndoCurrent->m_Next == 0 );
    }
    else
    {
        assert( m_UndoCurrent->m_Prev != 0 );
        m_UndoCurrent = m_UndoCurrent->m_Prev;
    }
    RevertModification( m_UndoCurrent );

    return &m_UndoCurrent->m_ExtraData;
}

bool FileContentBuffered::CanRedo()
{
    return m_UndoCurrent != 0;
}

bool FileContentBuffered::CanUndo()
{
    return m_UndoCurrent != m_UndoBuffer;
}

