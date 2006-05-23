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
#include "cclist.h"
#include <wx/sizer.h>
#include <configmanager.h>
#include <manager.h>
#include <globals.h>

const wxEventType csdEVT_CCLIST_CODECOMPLETE = wxNewEventType();

static CCList* g_CCList = 0L;

int idList = wxNewId();

BEGIN_EVENT_TABLE(CCList, wxFrame)
	EVT_ACTIVATE(CCList::OnActivate)
	EVT_SIZE(CCList::OnSize)
	EVT_KEY_DOWN(CCList::OnKeyDown)
	EVT_GRID_CELL_LEFT_CLICK(CCList::OnLeftClick)
	EVT_GRID_CELL_LEFT_DCLICK(CCList::OnLeftDClick)
	EVT_GRID_SELECT_CELL(CCList::OnCellChanged)
END_EVENT_TABLE()

CCList* CCList::Get(wxEvtHandler* parent, cbStyledTextCtrl* editor, Parser* parser)
{
	if (!g_CCList)
		g_CCList = new CCList(parent, editor, parser);
	return g_CCList;
}

void CCList::Free()
{
	if (g_CCList)
	{
		delete g_CCList;
		g_CCList = 0L;
	}
}

CCList::CCList(wxEvtHandler* parent, cbStyledTextCtrl* editor, Parser* parser)
	: wxFrame(editor, -1, _T("CC"), wxDefaultPosition, wxDefaultSize,
			wxFRAME_NO_TASKBAR | wxRESIZE_BORDER | wxNO_FULL_REPAINT_ON_RESIZE),
	m_pParent(parent),
	m_pEditor(editor),
	m_pParser(parser),
	m_pList(0L),
	m_IsCtrlPressed(false)
{
	m_StartPos = m_pEditor->GetCurrentPos();
	PositionMe();
    PlaceWindow(this, pdlConstrain, true);

	int start = m_pEditor->WordStartPosition(m_StartPos, true);
	wxString prefix = m_pEditor->GetTextRange(start, m_StartPos);

	m_pList = new CCListCtrl(this, idList, m_pParser, prefix);
	SendSizeEvent();
	m_pList->SetFocus();
}

CCList::~CCList()
{
	Manager::Get()->GetConfigManager(_T("code_completion"))->Write(_T("/size/width"), GetSize().GetWidth());
	Manager::Get()->GetConfigManager(_T("code_completion"))->Write(_T("/size/height"), GetSize().GetHeight());
	m_pEditor->SetFocus();
	if (m_pList)
	{
		delete m_pList;
		m_pList = 0L;
	}
	g_CCList = 0L;
}

void CCList::PositionMe()
{
	wxPoint pt = m_pEditor->PointFromPosition(m_StartPos);
	pt = m_pEditor->ClientToScreen(pt);
	int lineHeight = m_pEditor->TextHeight(m_pEditor->GetCurrentLine());
	pt.y += lineHeight;

	int w = Manager::Get()->GetConfigManager(_T("code_completion"))->ReadInt(_T("/size/width"), 320);
	int h = Manager::Get()->GetConfigManager(_T("code_completion"))->ReadInt(_T("/size/height"), 160);
	int screenW = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
	int screenH = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
	// sanity check
	if (w > screenW)
		w = screenW;
	if (h > screenH)
		h = screenH;

	SetSize(pt.x, pt.y, w, h);
}

void CCList::SelectCurrent(wxChar ch)
{
	if (!m_pList)
		return;
	Token* token = m_pList->GetSelectedToken();
	if (token)
	{
		int start = m_pEditor->WordStartPosition(m_pEditor->GetCurrentPos(), true);
		int end = m_pEditor->WordEndPosition(m_pEditor->GetCurrentPos(), true);
		m_pEditor->SetTargetStart(start);
		m_pEditor->SetTargetEnd(end);

		// "smart" support for functions...
		int offset = 0;
		bool funcHasArgs = false;
		bool codeCompleteAgain = false;
		wxString replace = token->m_Name;
		if (token->m_TokenKind == tkFunction)
		{
			// if token is a function, add ()
			replace << _T("()");
			funcHasArgs = !token->m_Args.Matches(_T("()")) && !token->m_Args.Matches(_T("(void)"));
			if (funcHasArgs)
				offset = 1; // adjust cursor position inside parentheses, only if func takes args
		}

		if (ch == '-' || ch == '>')
		{
			replace << _T("->");
			codeCompleteAgain = true;
			if (funcHasArgs)
				offset += 2; // adjust cursor position inside parentheses, only if func takes args
		}
		else if (ch == '.')
		{
			replace << ch;
			codeCompleteAgain = true;
			if (funcHasArgs)
				offset += 1; // adjust cursor position inside parentheses, only if func takes args
		}
		else if (ch == ';')
		{
			replace << ch;
			if (funcHasArgs)
				offset += 1; // adjust cursor position inside parentheses, only if func takes args
		}

		int len = m_pEditor->ReplaceTarget(replace);
		m_pEditor->GotoPos(m_pEditor->GetCurrentPos() + len - offset);
		if (codeCompleteAgain)
		{
			wxNotifyEvent event(csdEVT_CCLIST_CODECOMPLETE);
			wxPostEvent(m_pParent, event);
		}
	}
	Destroy();
}

void CCList::OnActivate(wxActivateEvent& event)
{
	if (!event.GetActive())
		Destroy();
}

void CCList::OnSize(wxSizeEvent& event)
{
	if (m_pList)
	{
		m_pList->SetSize(GetClientSize());
		m_pList->SetColSize(0, GetClientSize().GetWidth() - 20);
	}
}

void CCList::OnLeftClick(wxGridEvent& event)
{
	event.Skip(); // let the event proceed, anyway
	if (!m_IsCtrlPressed)
		return;
	if (!m_pList)
		return;
	Token* token = m_pList->GetTokenAt(event.GetRow());
	if (token)
	{
		wxString msg;
		msg << _T("\"") << token->m_Name << _T("\" breaks down to:\n\n");
		msg << _T("Kind: ") << token->GetTokenKindString() << _T('\n');
		msg << _T("Namespace: ") << token->GetNamespace() << _T('\n');
		msg << _T("Name: ") << token->m_Name << _T('\n');
		msg << _T("Arguments: ") << token->m_Args << _T('\n');
		msg << _T("Return value: ") << token->m_Type << _T('\n');
		msg << _T("Actual return value: ") << token->m_ActualType << _T('\n');
		msg << _T("Scope: ") << token->GetTokenScopeString() << _T("\n\n");
		msg << _T("and is met in ") << token->GetFilename() << _T(", at line ") << token->m_Line;
		cbMessageBox(msg);
	}
}

void CCList::OnLeftDClick(wxGridEvent& event)
{
	SelectCurrent();
	Destroy();
}

void CCList::OnCellChanged(wxGridEvent& event)
{
	//Manager::Get()->GetMessageManager()->DebugLog("OnCellChanged");
	event.Skip(); // let the event proceed, anyway
#if 0
	if (!m_pList)
		return;
	Token* token = m_pList->GetTokenAt(event.GetRow());
	if (token)
	{
		wxString msg;
		msg << token->m_Filename << " : " << token->m_Line;
		SetStatusText(msg);
	}
#endif
}

void CCList::OnKeyDown(wxKeyEvent& event)
{
	// unfortunately, for some odd reason, we never get wxGrid's (m_pList)
	// OnChar event.
	// So we have to process all key handling here (with raw keycodes...)
	char c = (char)event.GetKeyCode();
	m_IsCtrlPressed = event.ControlDown();
	//Manager::Get()->GetMessageManager()->DebugLog("OnKeyDown: %c (%d)", c, c);
	switch (event.GetKeyCode())
	{
		case WXK_ESCAPE:
		case WXK_LEFT:
		case WXK_RIGHT:
		{
			event.Skip();
			Destroy();
			break;
		}

		case WXK_SPACE:
		{
			if (m_pList && !m_pList->AddChar(c))
				Destroy();
			event.Skip();
			break;
		}

		case ';':
		{
			if (!event.ShiftDown())
				SelectCurrent(c);
			break;
		}

		case '.':
		{
			if (event.ShiftDown())
				SelectCurrent('>');
			else
				SelectCurrent('.');
			break;
		}

		case WXK_RETURN:
		{
			SelectCurrent();
			break;
		}

		case WXK_BACK:
		{
			if (m_pEditor->GetCurrentPos() <= m_StartPos)
				Destroy();
			else
			{
				if (m_pList && !m_pList->RemoveLastChar())
					Destroy();
			}
			break;
		}

		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':	case 'G':
		case 'H': case 'I':	case 'J': case 'K':	case 'L': case 'M':	case 'N':
		case 'O': case 'P':	case 'Q': case 'R':	case 'S': case 'T':	case 'U':
		case 'V': case 'W':	case 'X': case 'Y':	case 'Z':
		{
			if (!event.ShiftDown())
				c += 32;
			if (m_pList && !m_pList->AddChar(c))
				Destroy();
			break;
		}

		case '~':
		{
			if (event.ShiftDown())
			{
				if (m_pList && !m_pList->AddChar(c))
					Destroy();
			}
			break;
		}

		case '-':
		{
			if (event.ShiftDown())
			{
				if (m_pList && !m_pList->AddChar('_'))
					Destroy();
			}
			else
				SelectCurrent(c);
			break;
		}

		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
		case '8':
		{
			if (!event.ShiftDown())
			{
				if (m_pList && !m_pList->AddChar(c))
					Destroy();
			}
			break;
		}

		case '9':
		{
			if (!event.ShiftDown())
			{
				if (m_pList && !m_pList->AddChar(c))
					Destroy();
			}
			else
				SelectCurrent('(');
			break;
		}

		default:
			event.Skip();
			break;
	}
}
