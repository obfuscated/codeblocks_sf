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

bool Token::SerializeIn(wxInputStream* f)
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

bool Token::SerializeOut(wxOutputStream* f)
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

// *** TokensTree ***

TokensTree::TokensTree()
{
#if 0
    // Initialization: Pseudo-random numbers for distributing the string space

    // Sequences obtained from www.random.org
    unsigned int hash1[16] = { 0x98, 0xb0, 0xf0, 0xaa, 0xc2, 0xa2, 0xe6, 0x96, 0x0c, 0x18, 0xd4, 0xae, 0x1b, 0xd4, 0xab, 0xe8 };
    unsigned int hash2[16] = { 0xfd, 0x3b, 0x18, 0x2b, 0xb4, 0x1c, 0xb6, 0x40, 0xdf, 0x1f, 0x99, 0xcd, 0x56, 0x22, 0xd7, 0xfe };
    unsigned int hash3[16] = { 0xf1, 0xc2, 0x4f, 0x11, 0xac, 0x6f, 0x24, 0x55, 0xcf, 0x33, 0x35, 0x1f, 0xfc, 0x14, 0x71, 0xbc };
    unsigned int hash4[16] = { 0xd6, 0x36, 0x93, 0xa7, 0x82, 0xf6, 0xa8, 0x83, 0xb7, 0xde, 0x9b, 0xfb, 0x46, 0x85, 0x75, 0x26 };

    unsigned int i;
    for(i = 0; i < 256; i++)
    {
        m_hash1[i]=hash1[i & 15] ^ hash2[(i >> 4) & 15];
        m_hash2[i]=hash3[i & 15] ^ hash4[(i >> 4) & 15];
    }
#endif
}

#if 0
inline size_t TokensTree::GetHash(const wxString& name)
{
    return 0;
    if(!name.Length())
        return 0;
    return 255 & (m_hash1[255 & name.Length()] ^ m_hash2[255 & (unsigned char)(name[0])]);
}
#endif

Token* TokensTree::TokenExists(const wxString& name, Token* parent, short int kindMask)
{
//    TokenSearchTree* curtree = &m_Trees[GetHash(name)];
    TokenSearchTree* curtree = &m_Tree;
    int idx = curtree->GetItemNo(name);
    if(idx==0)
        return 0;
    unsigned int i;
    TokensArray* curlist = &(curtree->GetItemAtPos(idx));
    for(i = 0; i < curlist->GetCount(); i++)
    {
        Token* curtoken = curlist->Item(i);
        if((!parent || curtoken->m_pParent == parent) && curtoken->m_TokenKind & kindMask)
            return curtoken;
    }
    return 0;
}

void TokensTree::AddToken(const wxString& name,Token* newToken)
{
    static TokensArray tmp_tokens;
    tmp_tokens.Clear();
//    TokenSearchTree* curtree = &m_Trees[GetHash(name)];
    TokenSearchTree* curtree = &m_Tree;
    size_t idx = curtree->AddItem(name,tmp_tokens,false);
    TokensArray* curlist = &(curtree->GetItemAtPos(idx));
    int subidx = curlist->Index(newToken);
    if(subidx == wxNOT_FOUND)
        curlist->Add(newToken);
}

void TokensTree::Clear()
{
    m_Tree.Clear();
//    size_t i;
//    for(i=0;i<255;i++)
//        m_Trees[i].Clear();
}
