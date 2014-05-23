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

#ifndef SNIPPETITEMDATA_H
#define SNIPPETITEMDATA_H

#include <wx/treectrl.h>
#include <wx/string.h>

//-#if defined(BUILDING_PLUGIN)
    #include "macrosmanager.h"
//-#endif

// ----------------------------------------------------------------------------
class SnippetTreeItemData : public wxTreeItemData
// ----------------------------------------------------------------------------
{
	public:
		enum SnippetItemType
		{
			TYPE_ROOT,		// The root node
			TYPE_CATEGORY,	// All category nodes
			TYPE_SNIPPET	// The actual snippet node
		};
        #define SnippetItemID long

		SnippetTreeItemData(SnippetItemType type, long ID = 0);
		SnippetTreeItemData(SnippetItemType type, wxString snippet, long ID = 0);
		~SnippetTreeItemData();

		SnippetItemType GetType() const { return m_Type; }
		void            SetType(SnippetItemType type){ m_Type = type;}
		SnippetItemID   GetID()   const { return m_ID; }
		wxString        GetSnippetIDStr() const { return wxString::Format(_T("%ld"),m_ID); }
		void            SetID(SnippetItemID ID){ m_ID = ID;}
		wxString        GetSnippetString() const { return m_Snippet; }
		void            SetSnippetString(wxString snippet){ m_Snippet = snippet;}

        bool IsCategory(){ return (GetType() == SnippetTreeItemData::TYPE_CATEGORY); }
        bool IsSnippet(){ return (GetType() == SnippetTreeItemData::TYPE_SNIPPET); }
        bool IsSnippetFile()
            {	// verify snippet is file type snippet
                if (not IsSnippet() ) return false ;
                if ( GetSnippetFileLink() not_eq wxEmptyString )
                    return true;
                return false;
            }
        wxString GetSnippetFileLink()
            {	// if FileLink, return the filename
                if (not IsSnippet() ) return wxEmptyString ;
                wxString FileName = GetSnippetString().BeforeFirst('\r');
                         FileName = FileName.BeforeFirst('\n');
                //-#if defined(BUILDING_PLUGIN)
                static const wxString delim(_T("$%["));
                if( FileName.find_first_of(delim) != wxString::npos )
                    Manager::Get()->GetMacrosManager()->ReplaceMacros(FileName);
                //-#endif
                if (FileName.Length() > 128)
                    return wxEmptyString ;
                if ( (FileName.IsEmpty())
                    || (not ::wxFileExists( FileName)) )
                    return wxEmptyString;
                return FileName;
            }

        long        GetNewID(){return ++m_HighestSnippetID;}
        static void SetHighestSnippetID( long value){ m_HighestSnippetID = value;}
        long        UpdateHighestSnippetID(long ID)
                    { m_HighestSnippetID = (m_HighestSnippetID<ID) ? ID : m_HighestSnippetID;
                      return m_HighestSnippetID;
                    }
        static unsigned GetSnippetsItemsChangedCount(){return m_itemsChangedCount;}
        static void     SetSnippetsItemsChangedCount(unsigned count){m_itemsChangedCount = count;}

	private:
        void     InitializeItem(SnippetItemID oldID);

		SnippetItemType m_Type;
		wxString        m_Snippet;
		SnippetItemID   m_ID;
		static unsigned m_itemsChangedCount;
		static long     m_HighestSnippetID;
};

#endif // SNIPPETITEMDATA_H
