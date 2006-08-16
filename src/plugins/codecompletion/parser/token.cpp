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
#include <globals.h>
#include "token.h"
#include <wx/intl.h>
#include <wx/tokenzr.h>

inline void SaveTokenIdxSetToFile(wxOutputStream* f,const TokenIdxSet& data)
{
    SaveIntToFile(f, (int)(data.size()));
    for (TokenIdxSet::iterator it = data.begin(); it != data.end(); it++)
    {
        int num = *it;
        SaveIntToFile(f, num);
    }
}

bool LoadTokenIdxSetFromFile(wxInputStream* f,TokenIdxSet* data)
{
    if(!data)
        return false;
    bool result = true;
    data->clear();
    int size = 0;
    do
    {
        if (!LoadIntFromFile(f, &size))
        {
            result = false;
            break;
        }
        int i,num = 0;
        for (i = 0; i < size; i++)
        {
            if(!LoadIntFromFile(f,&num))
            {
                result = false;
                break;
            }
            data->insert(num);
        }
    }while(false);
    return result;
}

Token::Token()
	:
	m_File(0),
	m_Line(0),
	m_IsOperator(false),
	m_ParentIndex(-1),
	m_Bool(false),
	m_pTree(0),
	m_Self(-1)
{
}

Token::Token(const wxString& name, unsigned int file, unsigned int line)
	: m_Name(name),
	m_File(file),
	m_Line(line),
	m_IsOperator(false),
	m_ParentIndex(-1),
	m_Bool(false),
	m_pTree(0),
	m_Self(-1)
{
	//ctor
}

Token::~Token()
{
	//dtor
}

const wxString Token::GetParentName()
{
    wxString parentname = _T("");
    wxCriticalSectionLocker* lock = 0;
    if(m_pTree)
        lock = new wxCriticalSectionLocker(s_MutexProtection);
    Token* parent = GetParentToken();
    if(parent)
        parentname = parent->m_Name;
    if(lock)
        delete lock;
    return parentname;
}

const wxString Token::DisplayName()
{
//    wxString result(_T(""));
//    wxString parentname = GetParentName();
//    if (!parentname.IsEmpty())
//        result << parentname << _T("::");
    wxString result = GetNamespace();
    result << m_Name << m_Args;
    if (!m_Type.IsEmpty())
        result << _T(" : ") << m_Type;
    return result;
}

Token* Token::GetParentToken()
{
    Token* the_token = 0;
    if(!m_pTree)
        return 0;
    the_token = m_pTree->at(m_ParentIndex);
    return the_token;
}

wxString Token::GetFilename() const
{
    if(!m_pTree)
        return wxString(_T(""));
    return m_pTree->GetFilename(m_File);
}

wxString Token::GetImplFilename() const
{
    if(!m_pTree)
        return wxString(_T(""));
    return m_pTree->GetFilename(m_ImplFile);
}

bool Token::MatchesFiles(const TokenFilesSet& files)
{
    if(!files.size())
        return true;
    if(!m_File && !m_ImplFile)
        return true;
    if((m_File && files.count(m_File)) || (m_ImplFile && files.count(m_ImplFile)))
        return true;
    return false;
}

wxString Token::GetNamespace() const
{
	const wxString dcolon(_T("::"));
	wxString res;
	Token* parentToken = m_pTree->at(m_ParentIndex);
	while (parentToken)
	{
		res.Prepend(dcolon);
		res.Prepend(parentToken->m_Name);
		parentToken = parentToken->GetParentToken();
	}
	return res;
}

void Token::AddChild(int child)
{
	if (child >= 0)
        m_Children.insert(child);
}

bool Token::InheritsFrom(int idx) const
{
	if (idx < 0 || !m_pTree)
		return false;
    Token* token = m_pTree->at(idx);
    if(!token)
        return false;

	for (TokenIdxSet::iterator it = m_Ancestors.begin(); it != m_Ancestors.end(); it++)
	{
		int idx2 = *it;
		Token* ancestor = m_pTree->at(idx2);
		if(!ancestor)
            continue;
		if (ancestor == token || ancestor->InheritsFrom(idx))  // ##### is this intended?
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
    bool result = true;
    do
    {
        if (!LoadIntFromFile(f, (int*)&m_Self))
        {
            result = false;
            break;
        }
        if (!LoadIntFromFile(f, (int*)&m_ParentIndex))
        {
            result = false;
            break;
        }
        if(m_ParentIndex < 0)
            m_ParentIndex = -1;

        if (!LoadStringFromFile(f, m_Type))
        {
            result = false;
            break;
        }
        if (!LoadStringFromFile(f, m_ActualType))
        {
            result = false;
            break;
        }
        if (!LoadStringFromFile(f, m_Name))
        {
            result = false;
            break;
        }
        if (!LoadStringFromFile(f, m_Args))
        {
            result = false;
            break;
        }
        if (!LoadStringFromFile(f, m_AncestorsString))
        {
            result = false;
            break;
        }
        if (!LoadIntFromFile(f, (int*)&m_File))
        {
            result = false;
            break;
        }
        if (!LoadIntFromFile(f, (int*)&m_Line))
        {
            result = false;
            break;
        }
        if (!LoadIntFromFile(f, (int*)&m_ImplFile))
        {
            result = false;
            break;
        }
        if (!LoadIntFromFile(f, (int*)&m_ImplLine))
        {
            result = false;
            break;
        }
        if (!LoadIntFromFile(f, (int*)&m_Scope))
        {
            result = false;
            break;
        }
        if (!LoadIntFromFile(f, (int*)&m_TokenKind))
        {
            result = false;
            break;
        }
        if (!LoadIntFromFile(f, (int*)&m_IsOperator))
        {
            result = false;
            break;
        }
        if (!LoadIntFromFile(f, (int*)&m_IsLocal))
        {
            result = false;
            break;
        }
        if(!LoadTokenIdxSetFromFile(f, &m_Ancestors))
        {
            result = false;
            break;
        }
        if(!LoadTokenIdxSetFromFile(f, &m_Children))
        {
            result = false;
            break;
        }
        if(!LoadTokenIdxSetFromFile(f, &m_Descendants))
        {
            result = false;
            break;
        }
    }while(false);
    return result;
}

bool Token::SerializeOut(wxOutputStream* f)
{
    SaveIntToFile(f, m_Self);
    SaveIntToFile(f, m_ParentIndex);
    SaveStringToFile(f, m_Type);
    SaveStringToFile(f, m_ActualType);
    SaveStringToFile(f, m_Name);
    SaveStringToFile(f, m_Args);
    SaveStringToFile(f, m_AncestorsString);
    SaveIntToFile(f, m_File);
    SaveIntToFile(f, m_Line);
    SaveIntToFile(f, m_ImplFile);
    SaveIntToFile(f, m_ImplLine);
    SaveIntToFile(f, m_Scope);
    SaveIntToFile(f, m_TokenKind);
    SaveIntToFile(f, m_IsOperator ? 1 : 0);
    SaveIntToFile(f, m_IsLocal ? 1 : 0);

    SaveTokenIdxSetToFile(f, m_Ancestors);
    SaveTokenIdxSetToFile(f, m_Children);
    SaveTokenIdxSetToFile(f, m_Descendants);
    return true;
}

// *** TokensTree ***

TokensTree::TokensTree():
m_modified(false)
{
    m_Tokens.clear();
    m_FilenamesMap.clear();
    m_FilesMap.clear();
    m_FilesStatus.clear();
    m_FreeTokens.clear();
    m_FilesToBeReparsed.clear();
    m_TopNameSpaces.clear();
    m_GlobalNameSpace.clear();

}


TokensTree::~TokensTree()
{
    clear();
}

void TokensTree::clear()
{
    m_Tree.clear();
    m_FilenamesMap.clear();
    m_FilesMap.clear();
    m_FilesStatus.clear();
    m_FilesToBeReparsed.clear();
    m_FreeTokens.clear();
    m_TopNameSpaces.clear();
    m_GlobalNameSpace.clear();

    size_t i;
    for(i = 0;i < m_Tokens.size(); i++)
    {
        Token* token = m_Tokens[i];
        if(token)
            delete token;
    }
    m_Tokens.clear();
}

size_t TokensTree::size()
{
    return m_Tokens.size();
}

size_t TokensTree::realsize()
{
    if(m_Tokens.size() <= m_FreeTokens.size())
        return 0;
    return m_Tokens.size() - m_FreeTokens.size();
}

int TokensTree::insert(Token* newToken)
{
    if(!newToken)
        return -1;
    return AddToken(newToken, -1);
}

int TokensTree::insert(int loc, Token* newToken)
{
    if(!newToken)
        return -1;

    return AddToken(newToken, loc);
}

int TokensTree::erase(int loc)
{
    if(!m_Tokens[loc])
        return 0;
    RemoveToken(loc);
    return 1;
}

void TokensTree::erase(Token* oldToken)
{
    RemoveToken(oldToken);
}

int TokensTree::TokenExists(const wxString& name, int parent, short int kindMask)
{
    int idx = m_Tree.GetItemNo(name);
    if(!idx)
        return -1;
    TokenIdxSet::iterator it;
    TokenIdxSet& curlist = m_Tree.GetItemAtPos(idx);
    int result = -1;
    for(it = curlist.begin(); it != curlist.end(); it++)
    {
        result = *it;
        if(result < 0 || (size_t)result >= m_Tokens.size())
            continue;
        Token* curtoken = m_Tokens[result];
        if(!curtoken)
            continue;
        if((parent<0 || curtoken->m_ParentIndex == parent) && curtoken->m_TokenKind & kindMask)
            return result;
    }
    return -1;
}

size_t TokensTree::FindMatches(const wxString& s,TokenIdxSet& result,bool caseSensitive,bool is_prefix)
{
    set<size_t> lists;
    result.clear();
    int numitems = m_Tree.FindMatches(s,lists,caseSensitive,is_prefix);
    if(!numitems)
        return 0;
    TokenIdxSet* curset;
    set<size_t>::iterator it;
    TokenIdxSet::iterator it2;
    for(it = lists.begin(); it != lists.end(); it++)
    {
        curset = &(m_Tree.GetItemAtPos(*it));
        for(it2 = curset->begin();it2 != curset->end(); it2++)
            result.insert(*it2);
    }
    return result.size();
}

size_t TokensTree::FindTokensInFile(const wxString& file, TokenIdxSet& result, short int kindMask)
{
    result.clear();

    // get file idx
    if (!m_FilenamesMap.HasItem(file))
        return 0;
    int idx = m_FilenamesMap.GetItemNo(file);

    // now get the tokens set matching this file idx
    TokenFilesMap::iterator itf = m_FilesMap.find(idx);
    if (itf == m_FilesMap.end())
        return 0;
    TokenIdxSet& tokens = itf->second;

    // loop all results and add to final result set after filtering on token kind
    for (TokenIdxSet::iterator it = tokens.begin(); it != tokens.end(); ++it)
    {
        Token* token = at(*it);
        if (kindMask & token->m_TokenKind)
            result.insert(*it);
    }
    return result.size();
}

int TokensTree::AddToken(Token* newToken,int forceidx)
{
    if(!newToken)
        return -1;

    const wxString& name = newToken->m_Name;
    static TokenIdxSet tmp_tokens = TokenIdxSet();
    // tmp_tokens.clear();

    // Insert the token's name and the token in the (inserted?) list
    size_t idx2 = m_Tree.AddItem(name,tmp_tokens,false);
    TokenIdxSet& curlist = m_Tree.GetItemAtPos(idx2);

    int newitem = AddTokenToList(newToken,forceidx);
    curlist.insert(newitem);
    m_FilesMap[newToken->m_File].insert(newitem);

    // Add Token (if applicable) to the namespaces indexes
    if(newToken->m_ParentIndex < 0)
    {
        newToken->m_ParentIndex = -1;
        m_GlobalNameSpace.insert(newitem);
        if(newToken->m_TokenKind == tkNamespace)
            m_TopNameSpaces.insert(newitem);
    }

    // All done!
    return newitem;
}

void TokensTree::RemoveToken(int idx)
{
    if(idx<0 || (size_t)idx >= m_Tokens.size())
        return;
    RemoveToken(m_Tokens[idx]);
}

void TokensTree::RemoveToken(Token* oldToken)
{
    if(!oldToken)
        return;
    int idx = oldToken->m_Self;
    if(m_Tokens[idx]!=oldToken)
        return;

    // Step 1: Detach token from its parent

    Token* parentToken = 0;
    if((size_t)(oldToken->m_ParentIndex) >= m_Tokens.size())
        oldToken->m_ParentIndex = -1;
    if(oldToken->m_ParentIndex >= 0)
        parentToken = m_Tokens[oldToken->m_ParentIndex];
    if(parentToken)
        parentToken->m_Children.erase(idx);

    TokenIdxSet nodes;
    TokenIdxSet::iterator it;

    // Step 2: Detach token from its ancestors
    nodes = (oldToken->m_Ancestors);
    for(it = nodes.begin();it!=nodes.end(); it++)
    {
        int ancestoridx = *it;
        if(ancestoridx < 0 || (size_t)ancestoridx >= m_Tokens.size())
            continue;
        Token* ancestor = m_Tokens[ancestoridx];
        if(ancestor)
            ancestor->m_Descendants.erase(idx);
    }
    oldToken->m_Ancestors.clear();

    // Step 3: Remove children
    nodes = (oldToken->m_Children); // Copy the list to avoid interference
    for(it = nodes.begin();it!=nodes.end(); it++)
        RemoveToken(*it);
    // m_Children SHOULD be empty by now - but clear anyway.
    oldToken->m_Children.clear();

    // Step 4: Remove descendants
    nodes = oldToken->m_Descendants; // Copy the list to avoid interference
    for(it = nodes.begin();it!=nodes.end(); it++)
        RemoveToken(*it);
    // m_Descendants SHOULD be empty by now - but clear anyway.
    oldToken->m_Descendants.clear();

    // Step 5: Detach token from the SearchTrees
    int idx2 = m_Tree.GetItemNo(oldToken->m_Name);
    if(idx2)
    {
        TokenIdxSet& curlist = m_Tree.GetItemAtPos(idx2);
        curlist.erase(idx);
    }

    // Now, from the global namespace (if applicable)
    if(oldToken->m_ParentIndex == -1)
    {
        m_GlobalNameSpace.erase(idx);
        m_TopNameSpaces.erase(idx);
    }

    // Step 6: Finally, remove it from the list.
    RemoveTokenFromList(idx);
}

int TokensTree::AddTokenToList(Token* newToken,int forceidx)
{
    int result = -1;
    if(!newToken)
        return -1;
    if(forceidx >= 0) // Reading from Cache?
    {
        if((size_t)forceidx >= m_Tokens.size())
        {
            int max = 250*((forceidx + 250) / 250);
            m_Tokens.resize((max),0); // fill next 250 items with null-values
        }
        m_Tokens[forceidx] = newToken;
        result = forceidx;
    }
    else // For Realtime Parsing
    {
        if(m_FreeTokens.size())
        {
            result = m_FreeTokens[m_FreeTokens.size() - 1];
            m_FreeTokens.pop_back();
            m_Tokens[result] = newToken;
        }
        else
        {
            result = m_Tokens.size();
            m_Tokens.push_back(newToken);
        }
    }

    newToken->m_pTree = this;
    newToken->m_Self = result;
    // Clean up extra string memory

    newToken->m_Type.Shrink();
    newToken->m_Name.Shrink();
    newToken->m_Args.Shrink();
    newToken->m_AncestorsString.Shrink();

    return result;
}

void TokensTree::RemoveTokenFromList(int idx)
{
    if(idx < 0 || (size_t)idx >= m_Tokens.size())
        return;
    Token* oldToken = m_Tokens[idx];
    if(oldToken)
    {
        m_Tokens[idx] = 0;
        m_FreeTokens.push_back(idx);
        m_FilesToBeReparsed.insert(oldToken->m_File);
        delete oldToken;
    }
}

void TokensTree::RemoveFile(const wxString& filename)
{
    int index = GetFileIndex(filename);
    RemoveFile(index);
}

void TokensTree::RemoveFile(int index)
{
    if(index <=0)
        return;
    TokenIdxSet& the_list = m_FilesMap[index];
    TokenIdxSet::iterator it;
    for(it = the_list.begin(); it != the_list.end();it++)
    {
        int idx = *it;
        if(idx < 0 || (size_t)idx > m_Tokens.size())
            continue;
        Token* the_token = at(idx);
        if(!the_token)
            continue;

        // do not remove token lightly...
        // only if both its decl filename and impl filename are either empty or match this file
        // if one of those filenames do not match the above criteria
        // just clear the relevant info, leaving the token where it is...

        bool match1 = the_token->m_File == 0 || (int)the_token->m_File == index;
        bool match2 = the_token->m_ImplFile == 0 || (int)the_token->m_ImplFile == index;
        if (match1 && match2)
            RemoveToken(the_token); // safe to remove the token
        else
        {
            // do not remove token, just clear the matching info
            if (match1)
            {
                the_token->m_File = 0;
                the_token->m_Line = 0;
            }
            else if (match2)
            {
                the_token->m_ImplFile = 0;
                the_token->m_ImplLine = 0;
            }
        }
    }
    the_list.clear();
}

void TokensTree::RecalcFreeList()
{
    m_FreeTokens.clear();
    int i;
    for(i = m_Tokens.size() -1;i >= 0;i--)
    {
        if(!m_Tokens[i])
            m_FreeTokens.push_back(i);
    }
}

void TokensTree::RecalcData()
{
//    Manager::Get()->GetMessageManager()->DebugLog(_T("Calculating full inheritance tree"));

    // first loop to convert ancestors string to token indices for each token
    for (size_t i = 0; i < size(); ++i)
    {
        Token* token = at(i);
        if (!token)
            continue;

        if (token->m_TokenKind != tkClass)
            continue;
        if (token->m_AncestorsString.IsEmpty())
            continue;
        // only local symbols might change inheritance
//        if (!token->m_IsLocal)
//            continue;

        token->m_Ancestors.clear();
//        Manager::Get()->GetMessageManager()->DebugLog(_T(" : '%s'"), token->m_Name.c_str());

        //Manager::Get()->GetMessageManager()->DebugLog("Token %s, Ancestors %s", token->m_Name.c_str(), token->m_AncestorsString.c_str());
        wxStringTokenizer tkz(token->m_AncestorsString, _T(","));
        while (tkz.HasMoreTokens())
        {
            wxString ancestor = tkz.GetNextToken();
            if (ancestor.IsEmpty() || ancestor == token->m_Name)
                continue;
//            Manager::Get()->GetMessageManager()->DebugLog(_T("Ancestor %s"), ancestor.c_str());
            // ancestors might contain namespaces, e.g. NS::Ancestor
            if (ancestor.Find(_T("::")) != wxNOT_FOUND)
            {
                Token* ancestorToken = 0;
                wxStringTokenizer anctkz(ancestor, _T("::"));
                while (anctkz.HasMoreTokens())
                {
                    wxString ns = anctkz.GetNextToken();
                    if (!ns.IsEmpty())
                    {
                        int ancestorIdx = TokenExists(ns, ancestorToken ? ancestorToken->GetSelf() : -1, tkNamespace | tkClass);
                        ancestorToken = at(ancestorIdx);
//                        ancestorToken = token->HasChildToken(ns, tkNamespace | tkClass);
                        if (!ancestorToken) // unresolved
                            break;
                    }
                }
                if (ancestorToken)
                {
//                    Manager::Get()->GetMessageManager()->DebugLog(_T("Resolved to %s"), ancestorToken->m_Name.c_str());
                    token->m_Ancestors.insert(ancestorToken->GetSelf());
                    ancestorToken->m_Descendants.insert(i);
//                    Manager::Get()->GetMessageManager()->DebugLog(_T("   + '%s'"), ancestorToken->m_Name.c_str());
                }
//                else
//                    Manager::Get()->GetMessageManager()->DebugLog(_T("   ! '%s' (unresolved)"), ancestor.c_str());
            }
            else // no namespaces in ancestor
            {
                // accept multiple matches for inheritance
                TokenIdxSet result;
                FindMatches(ancestor, result, true, false);
                for (TokenIdxSet::iterator it = result.begin(); it != result.end(); it++)
                {
                    Token* ancestorToken = at(*it);
                    if (ancestorToken && ancestorToken->m_TokenKind == tkClass) // only classes take part in inheritance
                    {
                        token->m_Ancestors.insert(*it);
                        ancestorToken->m_Descendants.insert(i);
//                        Manager::Get()->GetMessageManager()->DebugLog(_T("   + '%s'"), ancestorToken->m_Name.c_str());
                    }
                }
//                if (result.empty())
//                    Manager::Get()->GetMessageManager()->DebugLog(_T("   ! '%s' (unresolved)"), ancestor.c_str());
            }
        }

        if (!token->m_IsLocal) // global symbols are linked once
        {
            //Manager::Get()->GetMessageManager()->DebugLog("Removing ancestor string from %s", token->m_Name.c_str(), token->m_Name.c_str());
            token->m_AncestorsString.Clear();
        }
    }

    // second loop to calculate full inheritance for each token
    for (size_t i = 0; i < size(); ++i)
    {
        Token* token = at(i);
        if (!token)
            continue;

        if (token->m_TokenKind != tkClass)
            continue;

        // recalc
        TokenIdxSet result;
        for (TokenIdxSet::iterator it = token->m_Ancestors.begin(); it != token->m_Ancestors.end(); it++)
            RecalcFullInheritance(*it, result);

        // now, add the resulting set to ancestors set
        for (TokenIdxSet::iterator it = result.begin(); it != result.end(); it++)
        {
            Token* ancestor = at(*it);
            if (ancestor)
            {
                token->m_Ancestors.insert(*it);
                ancestor->m_Descendants.insert(i);
            }
        }

//        // debug loop
//        Manager::Get()->GetMessageManager()->DebugLog(_T("Ancestors for %s:"),token->m_Name.c_str());
//        for (TokenIdxSet::iterator it = token->m_Ancestors.begin(); it != token->m_Ancestors.end(); it++)
//            Manager::Get()->GetMessageManager()->DebugLog(_T(" + %s"), at(*it)->m_Name.c_str());
    }
//    Manager::Get()->GetMessageManager()->DebugLog(_T("Full inheritance calculated."));
}

// caches the inheritance info for each token (recursive function)
void TokensTree::RecalcFullInheritance(int parentIdx, TokenIdxSet& result)
{
    // no parent token? no ancestors...
    if (parentIdx == -1)
        return;

    // no parent token? no ancestors...
    Token* ancestor = at(parentIdx);
    if (!ancestor)
        return;

    // only classes take part in inheritance
    if (ancestor->m_TokenKind != tkClass)
        return;
//    Manager::Get()->GetMessageManager()->DebugLog(_T("Anc: '%s'"), ancestor->m_Name.c_str());

    // for all its ancestors
    for (TokenIdxSet::iterator it = ancestor->m_Ancestors.begin(); it != ancestor->m_Ancestors.end(); it++)
    {
        if (*it != -1 && // not global scope
            *it != parentIdx && // not the same token (avoid infinite loop)
            result.find(*it) == result.end()) // not already in the set (avoid infinite loop)
        {
            // add it to the set
            result.insert(*it);
            // and recurse for its ancestors
            RecalcFullInheritance(*it, result);
        }
    }
}

Token* TokensTree::GetTokenAt(int idx)
{
    if(idx < 0 || (size_t)idx >= m_Tokens.size())
        return 0;
    return m_Tokens[idx];
}

size_t TokensTree::GetFileIndex(const wxString& filename)
{
    size_t result = m_FilenamesMap.insert(filename);
    // Insert does not alter the tree if the filename is already found.
    return result;
}

const wxString TokensTree::GetFilename(size_t idx) const
{
    wxString result = m_FilenamesMap.GetString(idx);
    return result;
}

bool TokensTree::IsFileParsed(const wxString& filename)
{
    size_t index = GetFileIndex(filename);
    bool parsed = (m_FilesMap.count(index) &&
                   m_FilesStatus[index]!=fpsNotParsed &&
                  !m_FilesToBeReparsed.count(index)
                  );
    return parsed;
}

size_t TokensTree::ReserveFileForParsing(const wxString& filename,bool preliminary)
{
    size_t index = GetFileIndex(filename);
    if(m_FilesToBeReparsed.count(index) &&
       (!m_FilesStatus.count(index) || m_FilesStatus[index]==fpsDone))
    {
        RemoveFile(filename);
        m_FilesToBeReparsed.erase(index);
        m_FilesStatus[index]=fpsNotParsed;
    }
    if(m_FilesStatus.count(index))
    {
        FileParsingStatus status = m_FilesStatus[index];
        if(preliminary)
        {
            if(status >= fpsAssigned)
                return 0; // Already assigned
        }
        else
        {
            if(status > fpsAssigned)
                return 0; // No parsing needed
        }
    }
    m_FilesToBeReparsed.erase(index);
    m_FilesStatus[index]=preliminary ? fpsAssigned : fpsBeingParsed; // Reserve file
    return index;
}

void TokensTree::FlagFileForReparsing(const wxString& filename)
{
    m_FilesToBeReparsed.insert(GetFileIndex(filename));
}

void TokensTree::FlagFileAsParsed(const wxString& filename)
{
    m_FilesStatus[GetFileIndex(filename)]=fpsDone;
}
