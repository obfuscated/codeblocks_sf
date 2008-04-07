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
// RCS-ID: $Id: snippetitemdata.cpp 28 2007-04-01 14:32:57Z Pecan $

#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
#endif

#include "snippetitemdata.h"

// ----------------------------------------------------------------------------
// statics
// ----------------------------------------------------------------------------
long SnippetItemData::HighestSnippetID = 0;
// ----------------------------------------------------------------------------
SnippetItemData::SnippetItemData(SnippetItemType type, SnippetItemID ID)
// ----------------------------------------------------------------------------
    : m_Type(type), m_Snippet(wxEmptyString), m_ID(ID)
{
    if ( 0 == m_ID )
        m_ID = GetNewID();
    else
        SetHighestSnippetID(ID);
}

// ----------------------------------------------------------------------------
SnippetItemData::SnippetItemData(SnippetItemType type, wxString snippet, SnippetItemID ID)
// ---------------------------------------------------------------------------
    : m_Type(type), m_Snippet(snippet), m_ID(ID)
{
    if ( 0 == m_ID )
        m_ID = GetNewID();
    else
        SetHighestSnippetID(ID);
}

// ----------------------------------------------------------------------------
SnippetItemData::~SnippetItemData()
// ----------------------------------------------------------------------------
{
	//dtor
}

