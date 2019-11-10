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

#include "FileContentBuffered.h"

#include <wx/file.h>
#include <algorithm>

/** \brief Internal modification class */
class FileContentBuffered::IntModificationData: public FileContentBuffered::ModificationData
{
    public:

        IntModificationData( std::vector< char >& buffer ): m_Buffer( buffer ) {}

        enum typeEnum
        {
            change,         ///< \brief Some content was changed
            added,          ///< \brief Some data was inserted
            removed,        ///< \brief Some data was removed
        };

        std::vector< char >& m_Buffer;

        typeEnum             m_Type;
        OffsetT              m_Position;
        std::vector< char >  m_OldData;
        std::vector< char >  m_NewData;

        void Apply()
        {
            switch ( m_Type )
            {
                case added:
                {
                    assert( m_Buffer.size() >= m_Position );
                    m_Buffer.insert( m_Buffer.begin() + m_Position, m_NewData.begin(), m_NewData.end() );
                    break;
                }

                case removed:
                {
                    assert( m_Buffer.size() > m_Position );
                    assert( m_Buffer.size() >= m_Position + m_OldData.size() );
                    m_Buffer.erase( m_Buffer.begin() + m_Position, m_Buffer.begin() + m_Position + m_OldData.size() );
                    break;
                }

                case change:
                {
                    assert( m_Buffer.size() > m_Position );
                    assert( m_Buffer.size() >= m_Position + m_NewData.size() );
                    assert( m_OldData.size() == m_NewData.size() );

                    std::copy( m_NewData.begin(), m_NewData.end(), m_Buffer.begin() + m_Position );
                    break;
                }
                default:
                    break;
            }
        }

        void Revert()
        {
            switch ( m_Type )
            {
                case removed:
                {
                    assert( m_Buffer.size() >= m_Position );
                    m_Buffer.insert( m_Buffer.begin() + m_Position, m_OldData.begin(), m_OldData.end() );
                    break;
                }

                case added:
                {
                    assert( m_Buffer.size() > m_Position );
                    assert( m_Buffer.size() >= m_Position + m_NewData.size() );
                    m_Buffer.erase( m_Buffer.begin() + m_Position, m_Buffer.begin() + m_Position + m_NewData.size() );
                    break;
                }

                case change:
                {
                    assert( m_Buffer.size() > m_Position );
                    assert( m_Buffer.size() >= m_Position + m_OldData.size() );
                    assert( m_OldData.size() == m_NewData.size() );

                    std::copy( m_OldData.begin(), m_OldData.end(), m_Buffer.begin() + m_Position );
                    break;
                }
                default:
                    break;
            }
        }

        OffsetT Length()
        {
            return m_OldData.empty() ? m_NewData.size() : m_OldData.size();
        }
};

FileContentBuffered::FileContentBuffered()
{
}

FileContentBuffered::~FileContentBuffered()
{
}

FileContentBuffered::ModificationData* FileContentBuffered::BuildAddModification( OffsetT position, OffsetT length, const void* data )
{
    IntModificationData* mod = new IntModificationData( m_Buffer );
    mod->m_Type     = IntModificationData::added;
    mod->m_Position = position;

    mod->m_NewData.resize( length );
    if ( data )
    {
        std::copy( (char*)data, (char*)data + length, mod->m_NewData.begin() );
    }

    return mod;
}

FileContentBuffered::ModificationData* FileContentBuffered::BuildRemoveModification( OffsetT position, OffsetT length )
{
    if ( position > m_Buffer.size() ) return 0;

    if ( position + length > m_Buffer.size() )
    {
        length = m_Buffer.size() - position;
        if ( !length ) return 0;
    }

    IntModificationData* mod = new IntModificationData( m_Buffer );
    mod->m_Type      = IntModificationData::removed;
    mod->m_Position  = position;
    mod->m_OldData.resize( length );

    std::copy( m_Buffer.begin() + position, m_Buffer.begin() + position + length, mod->m_OldData.begin() );

    return mod;
}

FileContentBuffered::ModificationData* FileContentBuffered::BuildChangeModification( OffsetT position, OffsetT length, const void* data )
{
    if ( position > m_Buffer.size() ) return 0;
    if ( position + length > m_Buffer.size() )
    {
        length = m_Buffer.size() - position;
        if ( !length ) return 0;
    }

    IntModificationData* mod = new IntModificationData( m_Buffer );
    mod->m_Type     = IntModificationData::change;
    mod->m_Position = position;
    mod->m_OldData.resize( length );
    mod->m_NewData.resize( length );

    std::copy( m_Buffer.begin() + position, m_Buffer.begin() + position + length, mod->m_OldData.begin() );
    if ( data )
    {
        std::copy( (char*)data, (char*)data + length, mod->m_NewData.begin() );
    }

    return mod;
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

bool FileContentBuffered::WriteFile(const wxString& fileName)
{
    wxFile fl( fileName, wxFile::write );
    if ( !fl.IsOpened() ) return false;
    if ( fl.Write( &m_Buffer[0], m_Buffer.size() ) == m_Buffer.size() )
    {
        UndoNotifySaved();
        return true;
    }
    return false;
}

bool FileContentBuffered::ReadFile(const wxString& fileName)
{
    wxFile fl( fileName, wxFile::read );
    if ( !fl.IsOpened() ) return false;

    m_Buffer.resize( fl.Length() );

    UndoClear();

    return (size_t)fl.Read( &m_Buffer[0], m_Buffer.size() ) == m_Buffer.size();
}
