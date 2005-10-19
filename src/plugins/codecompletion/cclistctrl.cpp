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
* $Id$
* $Date$
*/

#include <sdk.h>
#include "cclistctrl.h"
#include <wx/app.h>
#include <wx/msgdlg.h>
#include "cclist.h"

#define CUSTOM_DATA_TYPE _T("cc_items")

BEGIN_EVENT_TABLE(CCListCtrl, wxGrid)
	//EVT_CHAR(CCListCtrl::OnChar)
END_EVENT_TABLE()

CCListCtrl::CCListCtrl(wxWindow* parent, int id, Parser* parser, const wxString& initial)
	: wxGrid(parent, id, wxDefaultPosition, wxDefaultSize, wxNO_FULL_REPAINT_ON_RESIZE),
	m_pParent(parent),
	m_pParser(parser),
	m_Id(id),
	m_Initial(initial),
	m_pGridTable(0L),
	m_pRenderer(0L)
{
 	// set some default options
 	EnableEditing(false);
 	EnableGridLines(false);
 	EnableCellEditControl(false);
 	SetColLabelSize(0);
 	SetRowLabelSize(0);
 	SetDefaultRowSize(18);
 	SetGridLineColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));

 	// register our custom renderer
 	m_pRenderer = new CCRenderer(&m_CCTokens);
 	RegisterDataType(CUSTOM_DATA_TYPE, m_pRenderer, 0L);
 	SetDefaultRenderer(m_pRenderer);

 	// create list of tokens
 	m_pEditor = (cbStyledTextCtrl*)m_pParent->GetParent();
 	PrepareTokens();
}

CCListCtrl::~CCListCtrl()
{
//	delete m_pRenderer;
//	delete m_pGridTable;
}

Token* CCListCtrl::GetTokenAt(unsigned int pos)
{
	if (pos >= 0 && pos < m_CCTokens.GetCount())
		return m_CCTokens[pos];
	return 0L;
}

Token* CCListCtrl::GetSelectedToken()
{
	return GetTokenAt(GetGridCursorRow());
}

void CCListCtrl::PrepareTokens()
{
	// don't do any GUI updates while we 're building the tokens list
	Freeze();

	// build valid tokens list
	m_CCTokens.Clear();
	if (!m_pParser->Options().caseSensitive)
		m_Initial.MakeLower();
	for (unsigned int i = 0; i < m_pParser->GetTokens().GetCount(); ++i)
	{
		Token* token = m_pParser->GetTokens()[i];

		wxString name = token->m_Name;
		if (!m_pParser->Options().caseSensitive)
			name.MakeLower();

		if (token && // valid
			token->m_Bool && // marked by AI
			name.StartsWith(m_Initial)) // matches text start
		{
			m_CCTokens.Add(token);
		}
	}

	// this sets the grid to "virtual" mode
	if (!m_pGridTable)
		m_pGridTable = new CCTable(&m_CCTokens);
	m_created = false; // this is a hack; look in wxGrid::SetTable() to understand why...
	SetTable(m_pGridTable, true);

	// set the format of cells to that of our custom renderer
	SetColFormatCustom(0, CUSTOM_DATA_TYPE);

	// proceed to GUI updates
	Thaw();

	// select first cell
	if (GetGridCursorRow() != 0)
	{
		SetGridCursor(0, 0);
		MakeCellVisible(0, 0);
	}

	// refresh list
	ForceRefresh();
}

void CCListCtrl::AddChar(const wxChar& ch)
{
	wxString s;
	s << ch;
	m_pEditor->InsertText(m_pEditor->GetCurrentPos(), s);
	m_pEditor->GotoPos(m_pEditor->GetCurrentPos() + 1);
	m_Initial << s;
	PrepareTokens();
}

void CCListCtrl::RemoveLastChar()
{
	m_Initial.RemoveLast();
	m_pEditor->CmdKeyExecute(wxSCI_CMD_DELETEBACKNOTLINE);
	PrepareTokens();
}
