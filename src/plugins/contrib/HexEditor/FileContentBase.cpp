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

#include "FileContentBase.h"
#include "FileContentBuffered.h"
#include "FileContentDisk.h"

#include <wx/file.h>

namespace
{
    const FileContentBase::OffsetT maxBufferedSize = 1024L * 1024L * 4;
    const FileContentBase::OffsetT maxAnySize      = 0x8000000000000000ULL;//1024L * 1024L * 1024L * 2;  // 2 GB limit
}

FileContentBase::InvalidModificationData FileContentBase::m_UndoInvalid;

FileContentBase* FileContentBase::BuildInstance( const wxString& fileName )
{
    wxFile fl( fileName );
    if ( !fl.IsOpened() )
    {
        return NULL;
    }

    if ( (OffsetT)fl.Length() <= maxBufferedSize )
    {
        return new FileContentBuffered();
    }

    if ( (OffsetT)fl.Length() <= maxAnySize )
    {
        return new FileContentDisk();
    }


    return NULL;
}


FileContentBase::FileContentBase()
    : m_UndoBuffer ( 0 )
    , m_UndoLast   ( 0 )
    , m_UndoCurrent( 0 )
    , m_UndoSaved  ( 0 )
{
}

FileContentBase::~FileContentBase()
{
    UndoClear();
}

void FileContentBase::InsertAndApplyModification( ModificationData* mod )
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

    if ( !m_UndoSaved )
    {
        m_UndoSaved = mod;
    }

    ApplyModification( mod );

    m_UndoLast = mod;
    m_UndoCurrent = 0;
}

void FileContentBase::ApplyModification( ModificationData* mod )
{
    mod->Apply();
}

void FileContentBase::RevertModification( ModificationData* mod )
{
    mod->Revert();
}

void FileContentBase::RemoveUndoFrom( ModificationData* mod )
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

FileContentBase::OffsetT FileContentBase::Remove( const ExtraUndoData& extraUndoData, OffsetT position, OffsetT length )
{
    if ( !length ) return 0;

    ModificationData* mod = BuildRemoveModification( position, length );
    if ( !mod ) return 0;

    mod->m_Data = extraUndoData;

    InsertAndApplyModification( mod );

    return mod->Length();
}

FileContentBase::OffsetT FileContentBase::Add( const ExtraUndoData& extraUndoData, OffsetT position, OffsetT length, void* data)
{
    if ( !length ) return 0;

    ModificationData* mod = BuildAddModification( position, length, data );
    if ( !mod ) return 0;

    mod->m_Data = extraUndoData;

    InsertAndApplyModification( mod );

    return mod->Length();
}

FileContentBase::OffsetT FileContentBase::Write( const ExtraUndoData& extraUndoData, const void* buff, OffsetT position, OffsetT length )
{
    if ( !buff ) return 0;
    if ( !length ) return 0;

    ModificationData* mod = BuildChangeModification( position, length, buff );
    if ( !mod ) return 0;

    mod->m_Data = extraUndoData;

    InsertAndApplyModification( mod );

    return mod->Length();
}

bool FileContentBase::Modified()
{
    return m_UndoSaved != m_UndoCurrent;
}

void FileContentBase::SetModified( bool modified )
{
    m_UndoSaved = modified ? &m_UndoInvalid : m_UndoCurrent;
}

const FileContentBase::ExtraUndoData* FileContentBase::Redo()
{
    if ( !m_UndoCurrent ) return 0;

    ApplyModification( m_UndoCurrent );
    const ExtraUndoData* ret = &m_UndoCurrent->m_Data;

    m_UndoCurrent = m_UndoCurrent->m_Next;

    return ret;
}

const FileContentBase::ExtraUndoData* FileContentBase::Undo()
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

    return &m_UndoCurrent->m_Data;
}

bool FileContentBase::CanRedo()
{
    return m_UndoCurrent != 0;
}

bool FileContentBase::CanUndo()
{
    return m_UndoCurrent != m_UndoBuffer;
}
