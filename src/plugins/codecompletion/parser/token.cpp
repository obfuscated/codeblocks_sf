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
#include "token.h"
#include <wx/intl.h>

Token::Token()
	: m_Line(0),
	m_Bool(false),
	m_Int(-1),
	m_Data(0L),
	m_ParentIndex(-1)
{
}

Token::Token(const wxString& name, const wxString& filename, unsigned int line)
	: m_Name(name),
	m_Filename(filename),
	m_Line(line),
	m_IsOperator(false),
	m_IsTemporary(false),
	m_Bool(false),
	m_Int(-1),
	m_Data(0L),
	m_pParent(0L),
	m_ParentIndex(-1)
{
	//ctor
}

Token::~Token()
{
	//dtor
}

wxString Token::GetNamespace() const
{
	wxString res;
	Token* parent = m_pParent;
	while (parent)
	{
		res = parent->m_Name + _T("::") + res;
		parent = parent->m_pParent;
	}
	return res;
}

void Token::AddChild(Token* child)
{
	if (child)
		m_Children.Add(child);
}

bool Token::InheritsFrom(Token* token) const
{
	if (!token)
		return false;
	for (unsigned int i = 0; i < m_Ancestors.GetCount(); ++i)
	{
		Token* ancestor = m_Ancestors[i];
		if (ancestor == token || ancestor->InheritsFrom(token))  // ##### is this intended?
			return true;
	}
	return false;
}

wxString Token::GetTokenKindString() const
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
		default: return wxEmptyString; // tkUndefined
	}
}

wxString Token::GetTokenScopeString() const
{
	switch (m_Scope)
	{
		case tsPrivate: return _("private");
		case tsProtected: return _("protected");
		case tsPublic: return _("public");
		default: return wxEmptyString;
	}
}

bool Token::SerializeIn(wxFile* f)
{
    if (!LoadIntFromFile(f, (int*)&m_ParentIndex)) return false;
    if (!LoadStringFromFile(f, m_Type)) return false;
    if (!LoadStringFromFile(f, m_ActualType)) return false;
    if (!LoadStringFromFile(f, m_Name)) return false;
    if (!LoadStringFromFile(f, m_DisplayName)) return false;
    if (!LoadStringFromFile(f, m_Args)) return false;
    if (!LoadStringFromFile(f, m_AncestorsString)) return false;
    if (!LoadStringFromFile(f, m_Filename)) return false;
    if (!LoadIntFromFile(f, (int*)&m_Line)) return false;
    if (!LoadStringFromFile(f, m_ImplFilename)) return false;
    if (!LoadIntFromFile(f, (int*)&m_ImplLine)) return false;
    if (!LoadIntFromFile(f, (int*)&m_Scope)) return false;
    if (!LoadIntFromFile(f, (int*)&m_TokenKind)) return false;
    if (!LoadIntFromFile(f, (int*)&m_IsOperator)) return false;
    if (!LoadIntFromFile(f, (int*)&m_IsLocal)) return false;

    m_AncestorsIndices.Clear();
    int count = 0;
    LoadIntFromFile(f, &count);
    for (int i = 0; i < count; ++i)
    {
        int idx = 0;
        LoadIntFromFile(f, &idx);
        m_AncestorsIndices.Add(idx);
    }

    m_ChildrenIndices.Clear();
    count = 0;
    LoadIntFromFile(f, &count);
    for (int i = 0; i < count; ++i)
    {
        int idx = 0;
        LoadIntFromFile(f, &idx);
        m_ChildrenIndices.Add(idx);
    }
    // parent-child relationship is set in a post-processing step
    return true;
}

bool Token::SerializeOut(wxFile* f)
{
    SaveIntToFile(f, m_pParent ? m_pParent->m_Int : -1);
    SaveStringToFile(f, m_Type);
    SaveStringToFile(f, m_ActualType);
    SaveStringToFile(f, m_Name);
    SaveStringToFile(f, m_DisplayName);
    SaveStringToFile(f, m_Args);
    SaveStringToFile(f, m_AncestorsString);
    SaveStringToFile(f, m_Filename);
    SaveIntToFile(f, m_Line);
    SaveStringToFile(f, m_ImplFilename);
    SaveIntToFile(f, m_ImplLine);
    SaveIntToFile(f, m_Scope);
    SaveIntToFile(f, m_TokenKind);
    SaveIntToFile(f, m_IsOperator);
    SaveIntToFile(f, m_IsLocal);

    int tcount = (int)m_Ancestors.GetCount();
    SaveIntToFile(f, tcount);
    for (int i = 0; i < tcount; ++i)
    {
        Token* token = m_Ancestors[i];
        SaveIntToFile(f, token->m_Int);
    }

    tcount = (int)m_Children.GetCount();
    SaveIntToFile(f, tcount);
    for (int i = 0; i < tcount; ++i)
    {
        Token* token = m_Children[i];
        SaveIntToFile(f, token->m_Int);
    }
    // parent-child relationship is set in a post-processing step when serializing in
    return true;
}
