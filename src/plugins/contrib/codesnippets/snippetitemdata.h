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
// RCS-ID: $Id: snippetitemdata.h 19 2007-03-27 02:56:47Z Pecan $

#ifndef SNIPPETITEMDATA_H
#define SNIPPETITEMDATA_H

#include <wx/treectrl.h>
#include <wx/string.h>

// ----------------------------------------------------------------------------
class SnippetItemData : public wxTreeItemData
// ----------------------------------------------------------------------------
{
	public:
		enum SnippetItemType
		{
			TYPE_ROOT,		// The root node
			TYPE_CATEGORY,	// All category nodes
			TYPE_SNIPPET	// The actual snippet node
		};

		SnippetItemData(SnippetItemType type);
		SnippetItemData(SnippetItemType type, wxString snippet);
		~SnippetItemData();

		SnippetItemType GetType() const { return m_Type; }
		void            SetType(SnippetItemType type){ m_Type = type;}
		wxString        GetSnippet() const { return m_Snippet; }
		void            SetSnippet(wxString snippet){ m_Snippet = snippet;}

        bool IsSnippet(){ return (GetType() == SnippetItemData::TYPE_SNIPPET); }
        bool IsSnippetFile()
            {	// verify snippet is file type snippet
                if (not IsSnippet() ) return false ;
                wxString FileName = GetSnippet();
                if (FileName.Length() > 128)
                    return false ;
                if ( (FileName.IsEmpty())
                    || (not ::wxFileExists( FileName)) )
                    return false;
                return true;
            }

	private:
		SnippetItemType m_Type;
		wxString m_Snippet;
};

#endif // SNIPPETITEMDATA_H
