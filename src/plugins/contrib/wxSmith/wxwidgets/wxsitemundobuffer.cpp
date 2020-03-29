/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsitemundobuffer.h"

namespace
{
    wxString Empty = wxEmptyString;
}

wxsItemUndoBuffer::wxsItemUndoBuffer(int MaxEnteries):
    m_CurrentPos(0),
    m_SavedPos(0),
    m_MaxEnteries(MaxEnteries)
{
}

wxsItemUndoBuffer::~wxsItemUndoBuffer()
{
}

void wxsItemUndoBuffer::StoreChange(const wxString& XmlData)
{
    // Removing all undo points after current one
    if ( m_CurrentPos < GetCount()-1 )
    {
        m_Enteries.RemoveAt(m_CurrentPos+1,GetCount()-m_CurrentPos-1);
    }

    // Removing all outdated undos
    if ( m_MaxEnteries > 0 )
    {
        int ToRemove = GetCount()-m_MaxEnteries;
        if ( ToRemove > 0 )
        {
            m_Enteries.RemoveAt(0,ToRemove);
            m_CurrentPos -= ToRemove;
            m_SavedPos -= ToRemove;
        }
    }

    m_Enteries.Add(XmlData);
    m_CurrentPos = GetCount()-1;
}

const wxString& wxsItemUndoBuffer::Undo()
{
    if ( m_CurrentPos == 0 ) return Empty;
    return m_Enteries[--m_CurrentPos];
}

const wxString& wxsItemUndoBuffer::Redo()
{
    if ( m_CurrentPos >= GetCount() - 1 ) return Empty;
    return m_Enteries[++m_CurrentPos];
}
