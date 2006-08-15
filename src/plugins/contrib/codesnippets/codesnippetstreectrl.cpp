/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson

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

#include "snippetitemdata.h"
#include "codesnippetstreectrl.h"

IMPLEMENT_DYNAMIC_CLASS(CodeSnippetsTreeCtrl, wxTreeCtrl)

CodeSnippetsTreeCtrl::CodeSnippetsTreeCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: wxTreeCtrl(parent, id, pos, size, style)
{
	//ctor
}

CodeSnippetsTreeCtrl::~CodeSnippetsTreeCtrl()
{
	//dtor
}

int CodeSnippetsTreeCtrl::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
	// Get the items' data first
	const SnippetItemData* data1 = (SnippetItemData*)(GetItemData(item1));
	const SnippetItemData* data2 = (SnippetItemData*)(GetItemData(item2));


	if (data1 && data2)
	{
		int compareIndex1 = 0;
		switch (data1->GetType())
		{
			case SnippetItemData::TYPE_ROOT:
				compareIndex1 = 0;
			break;

			case SnippetItemData::TYPE_CATEGORY:
				compareIndex1 = 1;
			break;

			case SnippetItemData::TYPE_SNIPPET:
				compareIndex1 = 2;
			break;
		}
		int compareIndex2 = 0;
		switch (data2->GetType())
		{
			case SnippetItemData::TYPE_ROOT:
				compareIndex2 = 0;
			break;

			case SnippetItemData::TYPE_CATEGORY:
				compareIndex2 = 1;
			break;

			case SnippetItemData::TYPE_SNIPPET:
				compareIndex2 = 2;
			break;
		}

		// Compare the types
		if (compareIndex1 == compareIndex2)
		{
			// If they are both the same type, just use normal compare
			return wxTreeCtrl::OnCompareItems(item1, item2);
		}
		else if (compareIndex1 > compareIndex2)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return 0;
	}
}

