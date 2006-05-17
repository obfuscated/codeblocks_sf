/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include <sdk.h>
#include "ccrenderer.h"

void CCRenderer::Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected)
{
	wxGridCellStringRenderer::SetTextColoursAndFont(grid, attr, dc, isSelected);
    wxGridCellRenderer::Draw(grid, attr, dc, rect, row, col, isSelected);

	/*if (row == grid.GetGridCursorRow() && col == grid.GetGridCursorCol())
	{
		// selected cell
		wxColour selColour(grid.GetGridLineColour());
		dc.SetTextBackground(selColour);
		dc.SetBrush(wxBrush(selColour, wxSOLID));
		dc.SetPen(wxPen(selColour, 1, wxTRANSPARENT));
		dc.DrawRectangle(rect);
	}*/

	Token* token = m_Tokens->at(row);
	if (!token)
		return;

	wxString str;
	m_Font = attr.GetFont();
	wxRect myrect = rect;
	int w = 0;
	int h = 0;
	dc.GetTextExtent(_T("preprocessor  "), &w, &h);

	// shrink rect by one pixel from all sides, so that grid's highlight cursor
	// doesn't interfere with cell contents...
	myrect.x += 1;
	myrect.width -= 2;
	myrect.y += 1;
	myrect.height -= 2;

	// token kind string, e.g. "constructor"
	str = token->GetTokenKindString() + _T(" ");
	DoDrawText(grid, dc, myrect, str, *wxBLUE, wxNORMAL, w);

	// token namespace, e.g. "SomeClass::"
	if (!token->GetNamespace().IsEmpty())
	{
		str = token->GetNamespace() + _T(" ");
		DoDrawText(grid, dc, myrect, str, wxColour(0x80, 0x80, 0x80), wxNORMAL);
	}

	// token name, e.g. "SetThisValue"
	str = token->m_Name + _T(" ");
	DoDrawText(grid, dc, myrect, str, *wxBLACK, wxBOLD);

	// token args, e.g. "(int x, int y)"
	if (!token->m_Args.IsEmpty())
	{
		str = token->m_Args;
		DoDrawText(grid, dc, myrect, str, *wxBLACK, wxNORMAL);
	}

	// token actual return type, e.g. "void"
	if (!token->m_ActualType.IsEmpty())
	{
		str = _T(" : ") + token->m_ActualType;
		DoDrawText(grid, dc, myrect, str, *wxRED, wxNORMAL);
	}
}

void CCRenderer::DoDrawText(wxGrid& grid, wxDC& dc, wxRect& rect, const wxString& text, const wxColour& fore, int weight, int useThisOffset)
{
	m_Font.SetWeight(weight);
	dc.SetFont(m_Font);
	dc.SetTextForeground(fore);
	grid.DrawTextRectangle(dc, text, rect, wxALIGN_LEFT, wxALIGN_TOP);

	int w = 0;
	if (useThisOffset == -1)
	{
		int h = 0;
		dc.GetTextExtent(text, &w, &h);
	}
	else
		w = useThisOffset;
	rect.x += w;
	rect.width -= w;
}
