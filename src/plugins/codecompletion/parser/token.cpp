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

#include "token.h"
#include <wx/intl.h>

Token::Token()
	: m_Line(0),
	m_Bool(false), 
	m_Int(0), 
	m_Data(0L)
{
}

Token::Token(const wxString& name, const wxString& filename, unsigned int line)
	: m_Name(name),
	m_Filename(filename),
	m_Line(line),
	m_IsOperator(false),
	m_IsTemporary(false),
	m_Bool(false),
	m_Int(0),
	m_Data(0L),
	m_pParent(0L)
{
	//ctor
}

Token::~Token()
{
	//dtor
}

wxString Token::GetNamespace()
{
	wxString res;
	Token* parent = m_pParent;
	while (parent)
	{
		res = parent->m_Name + "::" + res;
		parent = parent->m_pParent;
	}
	return res;
}

void Token::AddChild(Token* child)
{
	if (child)
		m_Children.Add(child);
}

bool Token::InheritsFrom(Token* token)
{
	if (!token)
		return false;
	for (unsigned int i = 0; i < m_Ancestors.GetCount(); ++i)
	{
		Token* ancestor = m_Ancestors[i];
		if (ancestor == token || ancestor->InheritsFrom(token))
			return true;
	}
	return false;
}

wxString Token::GetTokenKindString()
{
	switch (m_TokenKind)
	{
		case tkClass: return _("class");
		case tkNamespace: return _("namespace");
		case tkEnum: return _("enum");
		case tkEnumerator: return _("enumerator");
		case tkFunction: return _("function");
		case tkConstructor: return _("constructor");
		case tkDestructor: return _("destructor");
		case tkPreprocessor: return _("preprocessor");
		case tkVariable: return _("variable");
		default: return _(""); // tkUndefined
	}
}

wxString Token::GetTokenScopeString()
{
	switch (m_Scope)
	{
		case tsPrivate: return _("private");
		case tsProtected: return _("protected");
		case tsPublic: return _("public");
		default: return _("");
	}
}
