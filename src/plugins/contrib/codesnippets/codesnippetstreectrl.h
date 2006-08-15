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

#ifndef CODESNIPPETSTREECTRL_H
#define CODESNIPPETSTREECTRL_H

#include <wx/treectrl.h>

class CodeSnippetsTreeCtrl : public wxTreeCtrl
{
	public:
		CodeSnippetsTreeCtrl() { }
		CodeSnippetsTreeCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
		~CodeSnippetsTreeCtrl();
		int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
	private:
		// Must use this so overridden OnCompareItems() works on MSW,
		// see wxWidgets Samples -> TreeCtrl sample
		//
		// Yes, again, ugly way to solve wxWidgets' weirdness
		DECLARE_DYNAMIC_CLASS(CodeSnippetsTreeCtrl)
};



#endif // CODESNIPPETSTREECTRL_H
