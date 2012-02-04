/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "tokenstree.h"

#include <wx/tokenzr.h>

#include <set>

#include "cclogger.h"

#define CC_TOKENSTREE_DEBUG_OUTPUT 0

#if CC_GLOBAL_DEBUG_OUTPUT == 1
    #undef CC_TOKENSTREE_DEBUG_OUTPUT
    #define CC_TOKENSTREE_DEBUG_OUTPUT 1
#elif CC_GLOBAL_DEBUG_OUTPUT == 2
    #undef CC_TOKENSTREE_DEBUG_OUTPUT
    #define CC_TOKENSTREE_DEBUG_OUTPUT 2
#endif

#if CC_TOKENSTREE_DEBUG_OUTPUT == 1
    #define TRACE(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
    #define TRACE2(format, args...)
#elif CC_TOKENSTREE_DEBUG_OUTPUT == 2
    #define TRACE(format, args...)                                              \
        do                                                                      \
        {                                                                       \
            if (g_EnableDebugTrace)                                             \
                CCLogger::Get()->DebugLog(F(format, ##args));                   \
        }                                                                       \
        while (false)
    #define TRACE2(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
#else
    #define TRACE(format, args...)
    #define TRACE2(format, args...)
#endif

wxMutex s_TokensTreeMutex;

TokensTree::TokensTree() :
    m_Modified(false),
    m_StructUnionUnnamedCount(0),
    m_EnumUnnamedCount(0),
    m_TokenTicketCount(255) // Reserve some space for the class browser
{
    m_Tokens.clear();
    m_Tree.clear();

    m_FilenamesMap.clear();
    m_FilesMap.clear();
    m_FilesToBeReparsed.clear();
    m_FreeTokens.clear();

    m_TopNameSpaces.clear();
    m_GlobalNameSpace.clear();

    m_FilesStatus.clear();
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
    m_FilesToBeReparsed.clear();
    m_FreeTokens.clear();

    m_TopNameSpaces.clear();
    m_GlobalNameSpace.clear();

    m_FilesStatus.clear();

    size_t i;
    for (i = 0;i < m_Tokens.size(); ++i)
    {
        Token* token = m_Tokens[i];
        if (token)
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
    if (m_Tokens.size() <= m_FreeTokens.size())
        return 0;

    return m_Tokens.size() - m_FreeTokens.size();
}

int TokensTree::insert(Token* newToken)
{
    if (!newToken)
        return -1;

    return AddToken(newToken, -1);
}

int TokensTree::insert(int loc, Token* newToken)
{
    if (!newToken)
        return -1;

    return AddToken(newToken, loc);
}

int TokensTree::erase(int loc)
{
    if (!m_Tokens[loc])
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
    if (!idx)
        return -1;

    TokenIdxSet::iterator it;
    TokenIdxSet& curList = m_Tree.GetItemAtPos(idx);
    int result = -1;
    for (it = curList.begin(); it != curList.end(); ++it)
    {
        result = *it;
        if (result < 0 || (size_t)result >= m_Tokens.size())
            continue;

        Token* curToken = m_Tokens[result];
        if (!curToken)
            continue;

        if ((curToken->m_ParentIndex == parent) && (curToken->m_TokenKind & kindMask))
        {
            return result;
        }
    }

    return -1;
}

int TokensTree::TokenExists(const wxString& name, const wxString& baseArgs, int parent, TokenKind kind)
{
    int idx = m_Tree.GetItemNo(name);
    if (!idx)
        return -1;

    TokenIdxSet::iterator it;
    TokenIdxSet& curList = m_Tree.GetItemAtPos(idx);
    int result = -1;
    for (it = curList.begin(); it != curList.end(); ++it)
    {
        result = *it;
        if (result < 0 || (size_t)result >= m_Tokens.size())
            continue;

        Token* curToken = m_Tokens[result];
        if (!curToken)
            continue;

        if (   (curToken->m_ParentIndex == parent)
            && (curToken->m_TokenKind   == kind)
            && (curToken->m_BaseArgs    == baseArgs) )
        {
            return result;
        }
    }

    return -1;
}

size_t TokensTree::FindMatches(const wxString& s, TokenIdxSet& result, bool caseSensitive, bool is_prefix, short int kindMask)
{
    result.clear();

    std::set<size_t> lists;
    int numitems = m_Tree.FindMatches(s, lists, caseSensitive, is_prefix);
    if (!numitems)
        return 0;

    // now the lists contains indexes to all the matching keywords
    // first loop will find all the keywords
    for (std::set<size_t>::iterator it = lists.begin(); it != lists.end(); ++it)
    {
        TokenIdxSet* curset = &(m_Tree.GetItemAtPos(*it));
        // second loop will get all the items mapped by the same keyword,
        // for example, we have ClassA::foo, ClassB::foo ...
        if (curset)
        {
            for (TokenIdxSet::iterator it2 = curset->begin(); it2 != curset->end(); ++it2)
            {
                Token* token = at(*it2);
                if (   token
                    && (   (kindMask == tkUndefined)
                        || (token->m_TokenKind & kindMask) ) )
                    result.insert(*it2);
            }
        }
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

    // loop all results and add to final result set after filtering on token kind
    TokenIdxSet& tokens = itf->second;
    for (TokenIdxSet::iterator it = tokens.begin(); it != tokens.end(); ++it)
    {
        Token* token = at(*it);
        if (token && (kindMask & token->m_TokenKind))
            result.insert(*it);
    }
    TRACE(_T("TokensTree::FindTokensInFile() : Found %d results for file '%s'."), result.size(), file.wx_str());
    return result.size();
}

int TokensTree::AddToken(Token* newToken, int fileIndex)
{
    if (!newToken)
        return -1;

    const wxString & name = newToken->m_Name;

    static TokenIdxSet tmpTokens = TokenIdxSet();

    // Insert the token's name and the token in the (inserted?) list
    size_t idx = m_Tree.AddItem(name, tmpTokens);
    TokenIdxSet& curList = m_Tree.GetItemAtPos(idx);

    int newItem = AddTokenToList(newToken, fileIndex);
    curList.insert(newItem);

    size_t fileIdx = (fileIndex<0) ? newToken->m_FileIdx : (size_t)fileIndex;
    m_FilesMap[fileIdx].insert(newItem);

    // Add Token (if applicable) to the namespaces indexes
    if (newToken->m_ParentIndex < 0)
    {
        newToken->m_ParentIndex = -1;
        m_GlobalNameSpace.insert(newItem);
        if (newToken->m_TokenKind == tkNamespace)
            m_TopNameSpaces.insert(newItem);
    }

    // All done!
    return newItem;
}

void TokensTree::RemoveToken(int idx)
{
    if (idx<0 || (size_t)idx >= m_Tokens.size())
        return;
    RemoveToken(m_Tokens[idx]);
}

void TokensTree::RemoveToken(Token* oldToken)
{
    if (!oldToken)
        return;

    int idx = oldToken->m_Index;
    if (m_Tokens[idx]!=oldToken)
        return;

    // Step 1: Detach token from its parent

    Token* parentToken = 0;
    if ((size_t)(oldToken->m_ParentIndex) >= m_Tokens.size())
        oldToken->m_ParentIndex = -1;
    if (oldToken->m_ParentIndex >= 0)
        parentToken = m_Tokens[oldToken->m_ParentIndex];
    if (parentToken)
        parentToken->m_Children.erase(idx);

    TokenIdxSet nodes;
    TokenIdxSet::iterator it;

    // Step 2: Detach token from its ancestors

    nodes = (oldToken->m_DirectAncestors);
    for (it = nodes.begin();it!=nodes.end(); ++it)
    {
        int ancestoridx = *it;
        if (ancestoridx < 0 || (size_t)ancestoridx >= m_Tokens.size())
            continue;
        Token* ancestor = m_Tokens[ancestoridx];
        if (ancestor)
            ancestor->m_Descendants.erase(idx);
    }
    oldToken->m_Ancestors.clear();
    oldToken->m_DirectAncestors.clear();

    // Step 3: Remove children

    nodes = (oldToken->m_Children); // Copy the list to avoid interference
    for (it = nodes.begin();it!=nodes.end(); ++it)
        RemoveToken(*it);
    // m_Children SHOULD be empty by now - but clear anyway.
    oldToken->m_Children.clear();

    // Step 4: Remove descendants

    nodes = oldToken->m_Descendants; // Copy the list to avoid interference
    for (it = nodes.begin();it!=nodes.end(); ++it)
    {
        if (*it == idx) // that should not happen, we can not be our own descendant, but in fact that can happen with boost
        {
            CCLogger::Get()->DebugLog(_T("Break out the loop to remove descendants, to avoid a crash. We can not be our own descendant!"));
            break;
        }
        RemoveToken(*it);
    }
    // m_Descendants SHOULD be empty by now - but clear anyway.
    oldToken->m_Descendants.clear();

    // Step 5: Detach token from the SearchTrees

    int idx2 = m_Tree.GetItemNo(oldToken->m_Name);
    if (idx2)
    {
        TokenIdxSet& curList = m_Tree.GetItemAtPos(idx2);
        curList.erase(idx);
    }

    // Now, from the global namespace (if applicable)
    if (oldToken->m_ParentIndex == -1)
    {
        m_GlobalNameSpace.erase(idx);
        m_TopNameSpaces.erase(idx);
    }

    // Step 6: Finally, remove it from the list.

    RemoveTokenFromList(idx);
}

int TokensTree::AddTokenToList(Token* newToken, int forceidx)
{
    if (!newToken)
        return -1;

    int result = -1;

    if (forceidx >= 0) // Reading from cache?
    {
        if ((size_t)forceidx >= m_Tokens.size())
        {
            int max = 250*((forceidx + 250) / 250);
            m_Tokens.resize((max),0); // fill next 250 items with null-values
        }
        m_Tokens[forceidx] = newToken;
        result = forceidx;
    }
    else // For real-time parsing
    {
        if (m_FreeTokens.size())
        {
            result = m_FreeTokens.back();
            m_FreeTokens.pop_back();
            m_Tokens[result] = newToken;
        }
        else
        {
            result = m_Tokens.size();
            m_Tokens.push_back(newToken);
        }
    }

    newToken->m_TokensTree = this;
    newToken->m_Index = result;

    // Clean up extra string memory
    newToken->m_FullType.Shrink();
    newToken->m_BaseType.Shrink();
    newToken->m_Name.Shrink();
    newToken->m_Args.Shrink();
    newToken->m_BaseArgs.Shrink();
    newToken->m_AncestorsString.Shrink();
    newToken->m_TemplateArgument.Shrink();

    return result;
}

void TokensTree::RemoveTokenFromList(int idx)
{
    if (idx < 0 || (size_t)idx >= m_Tokens.size())
        return;
    Token* oldToken = m_Tokens[idx];
    if (oldToken)
    {
        m_Tokens[idx] = 0;
        m_FreeTokens.push_back(idx);
        delete oldToken;
    }
}

void TokensTree::RemoveFile(const wxString& filename)
{
    int fileIndex = GetFileIndex(filename);
    RemoveFile(fileIndex);
}

void TokensTree::RemoveFile(int fileIndex)
{
    if (fileIndex <= 0)
        return;

    TokenIdxSet& the_list = m_FilesMap[fileIndex];
    for (TokenIdxSet::iterator it = the_list.begin(); it != the_list.end();)
    {
        int idx = *it;
        if (idx < 0 || (size_t)idx > m_Tokens.size())
        {
            the_list.erase(it++);
            continue;
        }

        Token* the_token = at(idx);
        if (!the_token)
        {
            the_list.erase(it++);
            continue;
        }

        // do not remove token lightly...
        // only if both its decl filename and impl filename are either empty or match this file
        // if one of those filenames do not match the above criteria
        // just clear the relevant info, leaving the token where it is...
        bool match1 = the_token->m_FileIdx     == 0 || (int)the_token->m_FileIdx     == fileIndex;
        bool match2 = the_token->m_ImplFileIdx == 0 || (int)the_token->m_ImplFileIdx == fileIndex;
        bool match3 = CheckChildRemove(the_token,fileIndex);
        if (match1 && match2 && match3)
        {
            RemoveToken(the_token); // safe to remove the token
            the_list.erase(it++);
            continue;
        }
        else
        {
            // do not remove token, just clear the matching info
            if (match1)
            {
                the_token->m_FileIdx = 0;
                the_token->m_Line = 0;
            }
            else if (match2)
            {
                the_token->m_ImplFileIdx = 0;
                the_token->m_ImplLine = 0;
            }
        }

        ++it;
    }
}

bool TokensTree::CheckChildRemove(Token * token, int fileIndex)
{
    TokenIdxSet& nodes = (token->m_Children); // Copy the list to avoid interference
    TokenIdxSet::iterator it;
    for (it = nodes.begin(); it != nodes.end(); ++it)
    {
        int idx = *it;
        if (idx < 0 || (size_t)idx > m_Tokens.size())
            continue;

        Token* the_token = at(idx);
        if (!the_token)
            continue;

        bool match1 = the_token->m_FileIdx     == 0 || (int)the_token->m_FileIdx     == fileIndex;
        bool match2 = the_token->m_ImplFileIdx == 0 || (int)the_token->m_ImplFileIdx == fileIndex;
        if(match1 && match2)
            continue;
        else
            return false;          // one child is belong to another file
    }
    return true;                   // no children should be reserved, so we can safely remov the token

}

void TokensTree::RecalcFreeList()
{
    m_FreeTokens.clear();
    for (int i = m_Tokens.size() - 1; i >= 0; --i)
    {
        if (!m_Tokens[i])
            m_FreeTokens.push_back(i);
    }
}

void TokensTree::RecalcInheritanceChain(Token* token)
{
    if (!token)
        return;
    if (!(token->m_TokenKind & (tkClass | tkTypedef | tkEnum | tkNamespace)))
        return;
    if (token->m_AncestorsString.IsEmpty())
        return;

    token->m_DirectAncestors.clear();
    token->m_Ancestors.clear();

    wxStringTokenizer tkz(token->m_AncestorsString, _T(","));
    TRACE(_T("RecalcInheritanceChain() : Token %s, Ancestors %s"), token->m_Name.wx_str(),
          token->m_AncestorsString.wx_str());
    TRACE(_T("RecalcInheritanceChain() : Removing ancestor string from %s"), token->m_Name.wx_str());
    token->m_AncestorsString.Clear();

    while (tkz.HasMoreTokens())
    {
        wxString ancestor = tkz.GetNextToken();
        if (ancestor.IsEmpty() || ancestor == token->m_Name)
            continue;

        TRACE(_T("RecalcInheritanceChain() : Ancestor %s"), ancestor.wx_str());

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
                    int ancestorIdx = TokenExists(ns, ancestorToken ? ancestorToken->m_Index : -1,
                                                  tkNamespace | tkClass | tkTypedef);
                    ancestorToken = at(ancestorIdx);
                    if (!ancestorToken) // unresolved
                        break;
                }
            }
            if (   ancestorToken
                && ancestorToken != token
                && (ancestorToken->m_TokenKind == tkClass || ancestorToken->m_TokenKind == tkNamespace) )
            {
                TRACE(_T("RecalcInheritanceChain() : Resolved to %s"), ancestorToken->m_Name.wx_str());
                RecalcInheritanceChain(ancestorToken);
                token->m_Ancestors.insert(ancestorToken->m_Index);
                ancestorToken->m_Descendants.insert(token->m_Index);
                TRACE(_T("RecalcInheritanceChain() :  + '%s'"), ancestorToken->m_Name.wx_str());
            }
            else
                TRACE(_T("RecalcInheritanceChain() :  ! '%s' (unresolved)"), ancestor.wx_str());
        }
        else // no namespaces in ancestor
        {
            // accept multiple matches for inheritance
            TokenIdxSet result;
            FindMatches(ancestor, result, true, false);
            for (TokenIdxSet::iterator it = result.begin(); it != result.end(); ++it)
            {
                Token* ancestorToken = at(*it);
                // only classes take part in inheritance
                if (   ancestorToken
                    && (ancestorToken != token)
                    && (   (ancestorToken->m_TokenKind == tkClass)
                        || (ancestorToken->m_TokenKind == tkEnum)
                        || (ancestorToken->m_TokenKind == tkTypedef)
                        || (ancestorToken->m_TokenKind == tkNamespace) ) )
                {
                    RecalcInheritanceChain(ancestorToken);
                    token->m_Ancestors.insert(*it);
                    ancestorToken->m_Descendants.insert(token->m_Index);
                    TRACE(_T("RecalcInheritanceChain() :  + '%s'"), ancestorToken->m_Name.wx_str());
                }
            }
#if CC_TOKEN_DEBUG_OUTPUT
            if (result.empty())
                TRACE(_T("RecalcInheritanceChain() :  ! '%s' (unresolved)"), ancestor.wx_str());
#endif
        }

        // Now, we have calc all the direct ancestors
        token->m_DirectAncestors = token->m_Ancestors;
    }

#if CC_TOKEN_DEBUG_OUTPUT
    wxStopWatch sw;
    TRACE(_T("RecalcInheritanceChain() : First iteration took : %ld ms"), sw.Time());
    sw.Start();
#endif

    // recalc
    TokenIdxSet result;
    for (TokenIdxSet::iterator it = token->m_Ancestors.begin(); it != token->m_Ancestors.end(); ++it)
        RecalcFullInheritance(*it, result);

    // now, add the resulting set to ancestors set
    for (TokenIdxSet::iterator it = result.begin(); it != result.end(); ++it)
    {
        Token* ancestor = at(*it);
        if (ancestor)
        {
            token->m_Ancestors.insert(*it);
            ancestor->m_Descendants.insert(token->m_Index);
        }
    }

#if CC_TOKEN_DEBUG_OUTPUT
    if (token)
    {
        // debug loop
        TRACE(_T("RecalcInheritanceChain() : Ancestors for %s:"), token->m_Name.wx_str());
        for (TokenIdxSet::iterator it = token->m_Ancestors.begin(); it != token->m_Ancestors.end(); ++it)
        {
            Token* anc_token = at(*it);
            if (anc_token)
                TRACE(_T("RecalcInheritanceChain() :  + %s"), anc_token->m_Name.wx_str());
            else
                TRACE(_T("RecalcInheritanceChain() :  + NULL?!"));
        }
    }
#endif

#if CC_TOKEN_DEBUG_OUTPUT
    TRACE(_T("RecalcInheritanceChain() : Second iteration took : %ld ms"), sw.Time());
#endif

    TRACE(_T("RecalcInheritanceChain() : Full inheritance calculated."));
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
    if (!(ancestor->m_TokenKind & (tkClass | tkTypedef)))
        return;

    TRACE(_T("RecalcFullInheritance() : Anc: '%s'"), ancestor->m_Name.wx_str());

    // for all its ancestors
    for (TokenIdxSet::iterator it = ancestor->m_Ancestors.begin(); it != ancestor->m_Ancestors.end(); ++it)
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
    if (idx < 0 || (size_t)idx >= m_Tokens.size())
        return 0;

    return m_Tokens[idx];
}

const Token* TokensTree::GetTokenAt(int idx) const
{
    if (idx < 0 || (size_t)idx >= m_Tokens.size())
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

    bool parsed = (   m_FilesMap.count(index)
                   && (m_FilesStatus[index]!=fpsNotParsed)
                   && !m_FilesToBeReparsed.count(index) );

    return parsed;
}

void TokensTree::MarkFileTokensAsLocal(const wxString& filename, bool local, void* userData)
{
    MarkFileTokensAsLocal(GetFileIndex(filename), local, userData);
}

void TokensTree::MarkFileTokensAsLocal(size_t file, bool local, void* userData)
{
    if (file == 0)
        return;

    TokenIdxSet& tokens = m_FilesMap[file];
    for (TokenIdxSet::iterator it = tokens.begin(); it != tokens.end(); ++it)
    {
        Token* token = m_Tokens.at(*it);
        if (token)
        {
            token->m_IsLocal = local;
            token->m_UserData = userData;
        }
    }
}

size_t TokensTree::ReserveFileForParsing(const wxString& filename,bool preliminary)
{
    size_t index = GetFileIndex(filename);
    if (   m_FilesToBeReparsed.count(index)
        && (  !m_FilesStatus.count(index)
            || m_FilesStatus[index]==fpsDone) )
    {
        RemoveFile(filename);
        m_FilesToBeReparsed.erase(index);
        m_FilesStatus[index]=fpsNotParsed;
    }
    if (m_FilesStatus.count(index))
    {
        FileParsingStatus status = m_FilesStatus[index];
        if (preliminary)
        {
            if (status >= fpsAssigned)
                return 0; // Already assigned
        }
        else
        {
            if (status > fpsAssigned)
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
