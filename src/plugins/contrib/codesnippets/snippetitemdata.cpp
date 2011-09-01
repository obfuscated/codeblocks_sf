/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
// RCS-ID: $Id$

#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
#endif

#include "snippetitemdata.h"
#include "snippetsconfig.h"
#include "codesnippetswindow.h"

// ----------------------------------------------------------------------------
// statics
// ----------------------------------------------------------------------------
long     SnippetItemData::m_HighestSnippetID = 0;
unsigned SnippetItemData::m_itemsChangedCount = 0;

// ----------------------------------------------------------------------------
SnippetItemData::SnippetItemData(SnippetItemType type, SnippetItemID oldID)
// ----------------------------------------------------------------------------
    : m_Type(type), m_Snippet(wxEmptyString), m_ID(oldID)
{
    InitializeItem(oldID);
}
// ----------------------------------------------------------------------------
SnippetItemData::SnippetItemData(SnippetItemType type, wxString snippet, SnippetItemID oldID)
// ---------------------------------------------------------------------------
    : m_Type(type), m_Snippet(snippet), m_ID(oldID)
{
    InitializeItem(oldID);
}
// ----------------------------------------------------------------------------
SnippetItemData::~SnippetItemData()
// ----------------------------------------------------------------------------
{
	//dtor
}
// ----------------------------------------------------------------------------
void SnippetItemData::InitializeItem(SnippetItemID oldID)
// ----------------------------------------------------------------------------
{
    //m_ID already set by ctor init m_ID(oldID)
    if ( 0 == oldID )
        m_ID = GetNewID();

    // if ID is less than highest, must be merging another .xml
    if ( m_ID < m_HighestSnippetID )
        if ( GetConfig()->GetSnippetsWindow()->IsAppendingFile() )
            m_ID = GetNewID();

    if ( oldID not_eq m_ID )
        m_itemsChangedCount += 1;

    // if ID is greater than highest, set to highest
    UpdateHighestSnippetID(m_ID);
}

