/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#ifndef CB_PRECOMP
#endif

#include <cbstyledtextctrl.h>
#include "nativeparser_base.h"

#include "parser/cclogger.h"

#define CC_NATIVEPARSERBASE_DEBUG_OUTPUT 0

#if CC_GLOBAL_DEBUG_OUTPUT == 1
    #undef CC_NATIVEPARSERBASE_DEBUG_OUTPUT
    #define CC_NATIVEPARSERBASE_DEBUG_OUTPUT 1
#elif CC_GLOBAL_DEBUG_OUTPUT == 2
    #undef CC_NATIVEPARSERBASE_DEBUG_OUTPUT
    #define CC_NATIVEPARSERBASE_DEBUG_OUTPUT 2
#endif

#if CC_NATIVEPARSERBASE_DEBUG_OUTPUT == 1
    #define TRACE(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
    #define TRACE2(format, args...)
#elif CC_NATIVEPARSERBASE_DEBUG_OUTPUT == 2
    #define TRACE(format, args...)                                              \
        do                                                                      \
        {                                                                       \
            if (g_EnableDebugTrace)                                             \
                CCLogger::Get()->DebugLog(F(format, ##args));   \
        }                                                                       \
        while (false)
    #define TRACE2(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
#else
    #define TRACE(format, args...)
    #define TRACE2(format, args...)
#endif

NativeParserBase::NativeParserBase()
{
}

NativeParserBase::~NativeParserBase()
{
}

void NativeParserBase::Reset()
{
    m_LastComponent.Clear();
}

// Here's the meat of code-completion :)
// This function decides most of what gets included in the auto-completion
// list presented to the user.
// It's called recursively for each component of the std::queue argument.
// for example: objA.objB.function()
// The queue is like: 'objA' 'objB' 'function'. We deal with objA first.
//
// No critical section needed in this recursive function!
// All functions that call this recursive function, should already entered a critical section.
size_t NativeParserBase::FindAIMatches(TokensTree*                 tree,
                                       std::queue<ParserComponent> components,
                                       TokenIdxSet&                result,
                                       int                         parentTokenIdx,
                                       bool                        isPrefix,
                                       bool                        caseSensitive,
                                       bool                        use_inheritance,
                                       short int                   kindMask,
                                       TokenIdxSet*                search_scope)
{
    if (components.empty())
        return 0;

    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(_T("FindAIMatches() ----- FindAIMatches - enter -----"));

    TRACE(_T("NativeParser::FindAIMatches()"));

    // pop top component
    ParserComponent parser_component = components.front();
    components.pop();

    // handle the special keyword "this".
    if ((parentTokenIdx != -1) && (parser_component.component == _T("this")))
    {
        // this will make the AI behave like it's the previous scope (or the current if no previous scope)

        // move on please, nothing to see here...
        // All functions that call the recursive FindAIMatches should already entered a critical section.
        return FindAIMatches(tree, components, result, parentTokenIdx,
                             isPrefix, caseSensitive, use_inheritance,
                             kindMask, search_scope);
    }

    // we 'll only add tokens in the result set if we get matches for the last token
    bool isLastComponent = components.empty();
    wxString searchtext = parser_component.component;

    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(F(_T("FindAIMatches() Search for %s, isLast = %d"),
                                    searchtext.wx_str(), isLastComponent?1:0));

    // get a set of matches for the current token
    TokenIdxSet local_result;
    // All functions that call the recursive GenerateResultSet should already entered a critical section.
    GenerateResultSet(tree, searchtext, parentTokenIdx, local_result,
                      (caseSensitive || !isLastComponent),
                      (isLastComponent && !isPrefix), kindMask);

    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(F(_T("FindAIMatches() Looping %d results"), local_result.size()));

    // loop all matches, and recurse
    for (TokenIdxSet::iterator it = local_result.begin(); it != local_result.end(); it++)
    {
        int id = *it;
        Token* token = tree->at(id);

        // sanity check
        if (!token)
        {
            if (s_DebugSmartSense)
                CCLogger::Get()->DebugLog(_T("FindAIMatches() Token is NULL?!"));
            continue;
        }

        // ignore operators
        if (token->m_IsOperator)
            continue;

        // enums children (enumerators), are added by default
        if (token->m_TokenKind == tkEnum)
        {
            // insert enum type
            result.insert(id);

            // insert enumerators
            for (TokenIdxSet::iterator it2 = token->m_Children.begin(); it2 != token->m_Children.end(); it2++)
                result.insert(*it2);

            continue; // done with this token
        }

        if (s_DebugSmartSense)
            CCLogger::Get()->DebugLog(F(_T("FindAIMatches() Match: '%s' (ID='%d') : type='%s'"), token->m_Name.wx_str(), id, token->m_BaseType.wx_str()));


        // is the token a function or variable (i.e. is not a type)
        if (    !searchtext.IsEmpty()
             && (parser_component.tokenType != pttSearchText)
             && !token->m_BaseType.IsEmpty() )
        {
            // the token is not a type
            // find its type's ID and use this as parent instead of (*it)
            TokenIdxSet type_result;
            std::queue<ParserComponent> type_components;
            wxString actual = token->m_BaseType;

            // TODO: ignore builtin types (void, int, etc)
            BreakUpComponents(actual, type_components);
            // the parent to search under is a bit troubling, because of namespaces
            // what we 'll do is search under current parent and traverse up the parentship
            // until we find a result, or reach -1...

            if (s_DebugSmartSense)
                CCLogger::Get()->DebugLog(F(_T("FindAIMatches() Looking for type: '%s' (%d components)"), actual.wx_str(), type_components.size()));

            // search under all search-scope namespaces too
            TokenIdxSet temp_search_scope;
            if (search_scope)
                temp_search_scope = *search_scope;

            // add grand-parent as search scope (if none defined)
            // this helps with namespaces when the token's type doesn't contain
            // namespace info. In that case (with the code here) we 're searching in
            // the parent's namespace too
            if (parentTokenIdx != -1)
            {
                Token* token = tree->at(parentTokenIdx);
                if (token)
                {
                    Token* parent = tree->at(token->m_ParentIndex);
                    if (parent)
                    {
                        temp_search_scope.insert(parent->m_Index);
                        if (s_DebugSmartSense)
                            CCLogger::Get()->DebugLog(_T("FindAIMatches() Implicit search scope added:") + parent->m_Name);
                    }
                }
            }

            TokenIdxSet::iterator itsearch;
            itsearch = temp_search_scope.begin();
            while (!search_scope || itsearch != temp_search_scope.end())
            {
                Token* parent = tree->at(*itsearch);

                if (s_DebugSmartSense)
#if wxCHECK_VERSION(2, 9, 0)
                    CCLogger::Get()->DebugLog(F(_T("FindAIMatches() Now looking under '%s'"),
                                                parent ? parent->m_Name.wx_str() : _("Global namespace").wx_str()));
#else
                    CCLogger::Get()->DebugLog(F(_T("FindAIMatches() Now looking under '%s'"),
                                                parent ? parent->m_Name.wx_str() : _("Global namespace")));
#endif
                do
                {
                    // types are searched as whole words, case sensitive and only classes/namespaces
                    // All functions that call the recursive FindAIMatches should already entered a critical section.
                    if (FindAIMatches(tree,
                                      type_components,
                                      type_result,
                                      parent ? parent->m_Index : -1,
                                      true,
                                      false,
                                      false,
                                      tkClass | tkNamespace | tkTypedef | tkEnum,
                                      &temp_search_scope) != 0)
                        break;
                    if (!parent)
                        break;
                    parent = tree->at(parent->m_ParentIndex);
                } while (true);
                ++itsearch;
            }

            // we got all possible types (hopefully should be just one)
            if (!type_result.empty())
            {
                // this is the first result
                id = *(type_result.begin());
                if (type_result.size() > 1)
                {
                    // if we have more than one result, recurse for all of them
                    TokenIdxSet::iterator it = type_result.begin();
                    ++it;
                    while (it != type_result.end())
                    {
                        std::queue<ParserComponent> lcomp = components;
                        // All functions that call the recursive FindAIMatches should already entered a critical section.
                        FindAIMatches(tree, lcomp, result, *it, isPrefix,
                                      caseSensitive, use_inheritance,
                                      kindMask, search_scope);
                        ++it;
                    }
                }

                if (s_DebugSmartSense)
                {
                    CCLogger::Get()->DebugLog(F(_T("FindAIMatches() Type: '%s' (%d)"), tree->at(id)->m_Name.wx_str(), id));
                    if (type_result.size() > 1)
                        CCLogger::Get()->DebugLog(F(_T("FindAIMatches() Multiple types matched for '%s': %d results"),
                                                    token->m_BaseType.wx_str(),
                                                    type_result.size()));
                }
            }
            else if (s_DebugSmartSense)
                CCLogger::Get()->DebugLog(F(_T("FindAIMatches() No types matched '%s'."), token->m_BaseType.wx_str()));
        }

        // if no more components, add to result set
        if (isLastComponent)
            result.insert(id);
        // else recurse this function using id as a parent
        else
            // All functions that call the recursive FindAIMatches should already entered a critical section.
            FindAIMatches(tree, components, result, id, isPrefix,
                          caseSensitive, use_inheritance, kindMask,
                          search_scope);
    }

    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(_T("FindAIMatches() ----- FindAIMatches - leave -----"));

    return result.size();
}

void NativeParserBase::FindCurrentFunctionScope(TokensTree*        tree,
                                                const TokenIdxSet& procResult,
                                                TokenIdxSet&       scopeResult)
{
    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    for (TokenIdxSet::iterator it = procResult.begin(); it != procResult.end(); ++it)
    {
        Token* token = tree->at(*it);
        if (!token)
            continue;

        if (token->m_TokenKind == tkClass)
            scopeResult.insert(*it);
        else
        {
            if (token->m_TokenKind & tkAnyFunction && token->HasChildren()) // for local variable
                scopeResult.insert(*it);
            scopeResult.insert(token->m_ParentIndex);
        }

        if (s_DebugSmartSense)
        {
            Token* parent = tree->at(token->m_ParentIndex);
            CCLogger::Get()->DebugLog(_T("AI() Adding search namespace: ") +
                                      (parent ? parent->m_Name : _T("Global namespace")));
        }
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
}

void NativeParserBase::CleanupSearchScope(TokensTree*  tree,
                                          TokenIdxSet* searchScope)
{
    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    for (TokenIdxSet::iterator it = searchScope->begin(); it != searchScope->end();)
    {
        Token* token = tree->at(*it);
        if (!token || !(token->m_TokenKind & (tkNamespace | tkClass | tkTypedef | tkAnyFunction)))
            searchScope->erase(it++);
        else
            ++it;
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

    // ...but alwayse search the global scope.
    searchScope->insert(-1);
}

// Set start and end for the calltip highlight region.
void NativeParserBase::GetCallTipHighlight(const wxString& calltip,
                                           int*            start,
                                           int*            end,
                                           int             typedCommas)
{
    TRACE(_T("NativeParserBase::GetCallTipHighlight()"));

    int pos = 0;
    int paramsCloseBracket = calltip.length() - 1;
    int nest = 0;
    int commas = 0;
    *start = FindFunctionOpenParenthesis(calltip) + 1;
    *end = 0;
    while (true)
    {
        wxChar c = calltip.GetChar(pos++);
        if (c == '\0')
            break;
        else if (c == '(')
            ++nest;
        else if (c == ')')
        {
            --nest;
            if (nest == 0)
                paramsCloseBracket = pos - 1;
        }
        else if (c == ',' && nest == 1)
        {
            ++commas;
            if (commas == typedCommas + 1)
            {
                *end = pos - 1;
                return;
            }
            *start = pos;
        }
    }
    if (*end == 0)
        *end = paramsCloseBracket;
}

int NativeParserBase::FindFunctionOpenParenthesis(const wxString& calltip)
{
    int nest = 0;
    for (size_t i = calltip.length(); i > 0; --i)
    {
        wxChar c = calltip[i - 1];
        if (c == wxT('('))
        {
            --nest;
            if (nest == 0)
            return i - 1;
        }
        else if (c == wxT(')'))
            ++nest;
    }
    return -1;
}

// Decides if the token belongs to its parent or one of its ancestors
bool NativeParserBase::BelongsToParentOrItsAncestors(TokensTree* tree,
                                                     Token*      token,
                                                     int         parentIdx,
                                                     bool        use_inheritance)
{
    // sanity check
    if (!tree || !token)
        return false;

    if (token->m_ParentIndex == parentIdx)
        return true; // direct child of parent (matches globals too)

    if (token->m_ParentIndex == -1)
        return false; // global

    if (!use_inheritance)
        return false;

    bool belongsTo = false;

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    // no parent token? no ancestors...
    Token* parentToken = tree->at(parentIdx);
    if (parentToken)
    {
        tree->RecalcInheritanceChain(parentToken);
        belongsTo = parentToken->m_Ancestors.find(token->m_ParentIndex) != parentToken->m_Ancestors.end();
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

    return belongsTo;
}

wxString NativeParserBase::GetCCToken(wxString&        line,
                                      ParserTokenType& tokenType,
                                      OperatorType&    tokenOperatroType)
{
    // line contains a string on the following form:
    // "    char* mychar = SomeNamespace::m_SomeVar.SomeMeth"
    // first we locate the first non-space char starting from the *end*:
    //
    // "    char* mychar = SomeNamespace::m_SomeVar.SomeMeth"
    //                     ^
    // then we remove everything before it.
    // after it, what we do here, is (by this example) return "SomeNamespace"
    // *and* modify line to become:
    // m_SomeVar.SomeMeth
    // so that if we 're called again with the (modified) line,
    // we 'll return "m_SomeVar" and modify line (again) to become:
    // SomeMeth
    // and so on and so forth until we return an empty string...
    // NOTE: if we find () args or [] arrays in our way, we skip them (done in GetNextCCToken)...

    tokenType         = pttSearchText;
    tokenOperatroType = otOperatorUndefine;
    if (line.IsEmpty())
        return wxEmptyString;

    tokenOperatroType = otOperatorUndefine;
    unsigned int startAt = FindCCTokenStart(line);
    wxString res = GetNextCCToken(line, startAt, tokenOperatroType);

    TRACE(_T("GetCCToken() : FindCCTokenStart returned %d \"%s\""), startAt, line.c_str());
    TRACE(_T("GetCCToken() : GetNextCCToken returned %d \"%s\""), startAt, res.c_str());


    if (startAt == line.Len())
        line.Clear();
    else
    {
        // skip whitespace
        startAt = AfterWhitespace(startAt, line);

        // Check for [Class]. ('.' pressed)
        if (IsOperatorDot(startAt, line))
        {
            tokenType = pttClass;
            line.Remove(0, startAt + 1);
        }
        // Check for [Class]-> ('>' pressed)
        // Check for [Class]:: (':' pressed)
        else if (IsOperatorEnd(startAt, line))
        {
            if (IsOperatorPointer(startAt, line) && !res.IsEmpty())
                tokenOperatroType = otOperatorPointer;
            if (line.GetChar(startAt) == ':')
                tokenType = pttNamespace;
            else
                tokenType = pttClass;
            line.Remove(0, startAt + 1);
        }
        else
            line.Clear();
    }

    TRACE(_T("GetCCToken() : Left \"%s\""), line.c_str());

    if (tokenOperatroType == otOperatorParentheses)
        tokenType = pttFunction;

    return res;
}

unsigned int NativeParserBase::FindCCTokenStart(const wxString& line)
{
    // Careful: startAt can become negative, so it's defined as integer here!
    int startAt = line.Len() - 1;
    int nest    = 0;

    bool repeat = true;
    while (repeat)
    {
        repeat = false;
        // Go back to the beginning of the function/variable (token)
        startAt = BeginOfToken(startAt, line);

        // Check for [Class]. ('.' pressed)
        if (IsOperatorDot(startAt, line))
        {
            --startAt;
            repeat = true; // yes -> repeat.
        }
        // Check for [Class]-> ('>' pressed)
        // Check for [Class]:: (':' pressed)
        else if (IsOperatorEnd(startAt, line))
        {
            startAt -= 2;
            repeat = true; // yes -> repeat.
        }

        if (repeat)
        {
            // now we're just before the "." or "->" or "::"
            // skip any whitespace
            startAt = BeforeWhitespace(startAt, line);

            // check for function/array/cast ()
            if (IsClosingBracket(startAt, line))
            {
                ++nest;
                while (   (--startAt >= 0)
                       && (nest != 0) )
                {
                    #if wxCHECK_VERSION(2, 9, 0)
                    switch (line.GetChar(startAt).GetValue())
                    #else
                    switch (line.GetChar(startAt))
                    #endif
                    {
                        case ']':
                        case ')': ++nest; --startAt; break;

                        case '[':
                        case '(': --nest; --startAt; break;

                    }

                    startAt = BeforeWhitespace(startAt, line);

                    if (IsClosingBracket(startAt, line))
                        ++nest;
                }

                startAt = BeforeToken(startAt, line);
            }
        }
    }
    ++startAt;

    startAt = AfterWhitespace(startAt, line);

    TRACE(_T("FindCCTokenStart() : Starting at %d \"%s\""), startAt, line.Mid(startAt).c_str());

    return startAt;
}

wxString NativeParserBase::GetNextCCToken(const wxString& line,
                                          unsigned int&   startAt,
                                          OperatorType&   tokenOperatroType)
{
    wxString res;
    int nest = 0;

    if (   (startAt < line.Len())
        && (line.GetChar(startAt) == '(') )
    {
        while (   (startAt < line.Len())
               && (   (line.GetChar(startAt) == '*')
                   || (line.GetChar(startAt) == '&')
                   || (line.GetChar(startAt) == '(') ) )
        {
            if (line.GetChar(startAt) == '(')
                ++nest;
            if (line.GetChar(startAt) == _T('*'))
                tokenOperatroType = otOperatorStar;
            ++startAt;
        }
    }

    TRACE(_T("GetNextCCToken() : at %d (%c): res=%s"), startAt, line.GetChar(startAt), res.c_str());

    while (InsideToken(startAt, line))
    {
        res << line.GetChar(startAt);
        ++startAt;
    }
    while (   (nest > 0)
           && (startAt < line.Len()) )
    {
        if (line.GetChar(startAt) == ')')
            --nest;
        ++startAt;
    }

    TRACE(_T("GetNextCCToken() : Done nest: at %d (%c): res=%s"), startAt, line.GetChar(startAt), res.c_str());

    startAt = AfterWhitespace(startAt, line);
    if (IsOpeningBracket(startAt, line))
    {
        if (line.GetChar(startAt) == _T('('))
            tokenOperatroType = otOperatorParentheses;
        else if (line.GetChar(startAt) == _T('['))
            tokenOperatroType = otOperatorSquare;
        ++nest;
        while (   (startAt < line.Len()-1)
               && (nest != 0) )
        {
            ++startAt;
            #if wxCHECK_VERSION(2, 9, 0)
            switch (line.GetChar(startAt).GetValue())
            #else
            switch (line.GetChar(startAt))
            #endif
            {
                case ']':
                case ')': --nest; ++startAt; break;

                case '[':tokenOperatroType = otOperatorSquare;
                case '(': ++nest; ++startAt; break;
            }

            startAt = AfterWhitespace(startAt, line);

            if (IsOpeningBracket(startAt, line))
                ++nest;
        }
    }
    if (IsOperatorBegin(startAt, line))
        ++startAt;

    TRACE(_T("GetNextCCToken() : Return at %d (%c): res=%s"), startAt, line.GetChar(startAt), res.c_str());

    return res;
}

void NativeParserBase::RemoveLastFunctionChildren(TokensTree* tree,
                                                  int&        lastFuncTokenIdx)
{
    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    Token* token = tree->at(lastFuncTokenIdx);
    if (token)
    {
        lastFuncTokenIdx = -1;
        if (token->m_TokenKind & tkAnyFunction)
            token->DeleteAllChildren();
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
}

// Breaks up the phrase for code-completion.
// Suppose the user has invoked code-completion in this piece of code:
//
//   Ogre::Root::getSingleton().|
//
// This function will break this up into an std::queue (FIFO) containing
// the following items (top is first-out):
//
// Ogre             [pttNamespace]
// Root             [pttClass]
// getSingleton     [pttFunction]
// (empty space)    [pttSearchText]
//
// It also classifies each component as a pttClass, pttNamespace, pttFunction, pttSearchText
size_t NativeParserBase::BreakUpComponents(const wxString&              actual,
                                           std::queue<ParserComponent>& components)
{
    ParserTokenType tokenType;
    wxString tmp = actual;
    OperatorType tokenOperatorType;
    // break up components of phrase
    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(F(_T("BreakUpComponents() Breaking up '%s'"), tmp.wx_str()));
    TRACE(_T("NativeParserBase::BreakUpComponents()"));

    while (true)
    {
        wxString tok = GetCCToken(tmp, tokenType, tokenOperatorType);

        ParserComponent pc;
        pc.component         = tok;
        pc.tokenType         = tokenType;
        pc.tokenOperatorType = tokenOperatorType;
        // Debug smart sense: output the component's name and type.
        if (s_DebugSmartSense)
        {
            wxString tokenTypeString;
            switch (tokenType)
            {
                case (pttFunction):
                {   tokenTypeString = _T("Function");   break; }
                case (pttClass):
                {   tokenTypeString = _T("Class");      break; }
                case (pttNamespace):
                {   tokenTypeString = _T("Namespace");  break; }
                case (pttSearchText):
                {   tokenTypeString = _T("SearchText"); break; }
                default:
                {   tokenTypeString = _T("Undefined");         }
            }
            CCLogger::Get()->DebugLog(F(_T("BreakUpComponents() Found component: '%s' (%s)"), tok.wx_str(), tokenTypeString.wx_str()));
        }

        // Support global namespace like ::MessageBoxA
        // Break up into "", type is pttNameSpace and "MessageBoxA", type is pttSearchText.
        // for pttNameSpace  type, if its text (tok) is empty -> ignore this component.
        // for pttSearchText type, don't do this because for ss:: we need this, too.
        if (!tok.IsEmpty() || (tokenType == pttSearchText && !components.empty()))
        {
            if (s_DebugSmartSense)
                CCLogger::Get()->DebugLog(F(_T("BreakUpComponents() Adding component: '%s'."), tok.wx_str()));
            components.push(pc);
        }

        if (tokenType == pttSearchText)
            break;
    }

    return 0;
}

size_t NativeParserBase::ResolveExpression(TokensTree*                 tree,
                                           std::queue<ParserComponent> components,
                                           const TokenIdxSet&          searchScope,
                                           TokenIdxSet&                result,
                                           bool                        caseSense,
                                           bool                        isPrefix)
{
    m_TemplateMap.clear();
    if (components.empty())
        return 0;

    TokenIdxSet initialScope;
    if (!searchScope.empty())
        initialScope = searchScope;
    else
        initialScope.insert(-1);

    while (!components.empty())
    {
        TokenIdxSet initialResult;
        ParserComponent subComponent = components.front();
        components.pop();
        wxString searchText = subComponent.component;
        if (searchText == _T("this"))
        {
            initialScope.erase(-1);
            TokenIdxSet tempInitialScope = initialScope;

            CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

            for (TokenIdxSet::iterator it = tempInitialScope.begin(); it != tempInitialScope.end(); ++it)
            {
                Token* token = tree->at(*it);
                if (token && (token->m_TokenKind !=tkClass))
                    initialScope.erase(*it);
            }

            CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

            if (!initialScope.empty())
                continue;
            else
                break; // error happened.
        }

        if (s_DebugSmartSense)
        {
            CCLogger::Get()->DebugLog(F(_T("ResolveExpression() search scope is %d result."), initialScope.size()));
            for (TokenIdxSet::iterator tt = initialScope.begin(); tt != initialScope.end(); ++tt)
                CCLogger::Get()->DebugLog(F(_T("search scope: %d"), (*tt)));
        }

        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

        // All functions that call the recursive GenerateResultSet should already entered a critical section.

        // e.g. A.BB.CCC.DDDD|
        if (components.empty()) // is the last component (DDDD)
            GenerateResultSet(tree, searchText, initialScope, initialResult, caseSense, isPrefix);
        else // case sensitive and full-match always (A / BB / CCC)
            GenerateResultSet(tree, searchText, initialScope, initialResult, true, false);

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

        // now we should clear the initialScope.
        initialScope.clear();

        //-------------------------------------

        if (s_DebugSmartSense)
            CCLogger::Get()->DebugLog(F(_T("ResolveExpression() Looping %d result."), initialResult.size()));

        //------------------------------------
        if (!initialResult.empty())
        {
            bool locked = false;

            // loop all matches.
            for (TokenIdxSet::iterator it = initialResult.begin(); it != initialResult.end(); ++it)
            {
                const size_t id = (*it);
                wxString actualTypeStr;
                int parentIndex = -1;
                bool isFuncOrVar = false;

                if (locked)
                    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
                CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)
                locked = true;

                Token* token = tree->at(id);
                if (!token)
                {
                    if (s_DebugSmartSense)
                        CCLogger::Get()->DebugLog(F(_T("ResolveExpression() token is NULL?!")));
                    continue;
                }

                // TODO: we should deal with operators carefully.
                // it should work for class::/namespace::
                if (token->m_IsOperator && (m_LastComponent.tokenType != pttNamespace))
                    continue;

                if (s_DebugSmartSense)
                    CCLogger::Get()->DebugLog(F(_T("ResolvExpression() Match:'%s(ID=%d) : type='%s'"),
                                                token->m_Name.wx_str(), id, token->m_BaseType.wx_str()));

                // recond the template map message here. hope it will work.
                // wxString tkname = token->m_Name;
                // wxArrayString tks = token->m_TemplateType;
                if (!token->m_TemplateMap.empty())
                    m_TemplateMap = token->m_TemplateMap;

                // if the token is a function/variable(i.e. is not a type)
                isFuncOrVar =   !searchText.IsEmpty()
                             && (subComponent.tokenType != pttSearchText)
                             && !token->m_BaseType.IsEmpty();
                if (isFuncOrVar)
                {
                    actualTypeStr = token->m_BaseType;
                    parentIndex = token->m_Index;
                }

                CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
                locked = false;

                // handle it if the token is a function/variable(i.e. is not a type)
                if (isFuncOrVar)
                {
                    TokenIdxSet autualTypeResult;
                    TokenIdxSet actualTypeScope;
                    if (searchScope.empty())
                        actualTypeScope.insert(-1);
                    else
                    {
                        // now collect the search scope for actual type of function/variable.
                        CollectSearchScopes(searchScope, actualTypeScope, tree);

                        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

                        // now add the current token's parent scope;
                        Token* currentTokenParent = tree->at(parentIndex);
                        while (true)
                        {
                            if (!currentTokenParent)
                                break;
                            actualTypeScope.insert(currentTokenParent->m_Index);
                            currentTokenParent = tree->at(currentTokenParent->m_ParentIndex);
                        }

                        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
                    }

                    // now get the tokens of variable/function.
                    TokenIdxSet actualTypeResult;
                    ResolveActualType(tree, actualTypeStr, actualTypeScope, actualTypeResult);
                    if (!actualTypeResult.empty())
                    {
                        for (TokenIdxSet::iterator it2 = actualTypeResult.begin(); it2 != actualTypeResult.end(); ++it2)
                        {
                            initialScope.insert(*it2);

                            CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

                            Token* typeToken = tree->at(*it2);
                            if (typeToken && !typeToken->m_TemplateMap.empty())
                                m_TemplateMap = typeToken->m_TemplateMap;

                            CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

                            // and we need to add the template argument alias too.
                            AddTemplateAlias(tree, *it2, actualTypeScope, initialScope);
                        }
                    }
                    else // ok ,we search template container to check if type is template formal.
                        ResolveTemplateMap(tree, actualTypeStr, actualTypeScope, initialScope);

                    continue;
                }

                initialScope.insert(id);
            }// for

            if (locked)
                CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
        }
        else
        {
            initialScope.clear();
            break;
        }

        if (subComponent.tokenOperatorType != otOperatorUndefine)
        {
            TokenIdxSet operatorResult;
            ResolveOperator(tree, subComponent.tokenOperatorType, initialScope, searchScope, operatorResult);
            if (!operatorResult.empty())
                initialScope = operatorResult;
        }
        if (subComponent.tokenType != pttSearchText)
            m_LastComponent = subComponent;
    }// while

    if (!initialScope.empty())
        result = initialScope;

    return result.size();
}

void NativeParserBase::ResolveOperator(TokensTree*         tree,
                                       const OperatorType& tokenOperatorType,
                                       const TokenIdxSet&  tokens,
                                       const TokenIdxSet&  searchScope,
                                       TokenIdxSet&        result)
{
    if (!tree || searchScope.empty())
        return;

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    // first,we need to eliminate the tokens which are not tokens.
    TokenIdxSet opInitialScope;
    for (TokenIdxSet::iterator it=tokens.begin(); it!=tokens.end(); ++it)
    {
        int id = (*it);
        Token* token = tree->at(id);
        if (token && (token->m_TokenKind == tkClass || token->m_TokenKind == tkTypedef))
            opInitialScope.insert(id);
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

    // if we get nothing, just return.
    if (opInitialScope.empty())
        return;

    wxString operatorStr;
    switch(tokenOperatorType)
    {
    case otOperatorParentheses:
        operatorStr = _T("operator()"); break;
    case otOperatorSquare:
        operatorStr = _T("operator[]"); break;
    case otOperatorPointer:
        operatorStr = _T("operator->"); break;
    case otOperatorStar:
        operatorStr = _T("operator*"); break;
    default:
        break;
    }
    if (operatorStr.IsEmpty())
        return;

    //s tart to parse the operator overload actual type.
    TokenIdxSet opInitialResult;

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    // All functions that call the recursive GenerateResultSet should already entered a critical section.
    GenerateResultSet(tree, operatorStr, opInitialScope, opInitialResult);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

    CollectSearchScopes(searchScope, opInitialScope, tree);

    if (opInitialResult.empty())
        return;

    for (TokenIdxSet::iterator it=opInitialResult.begin(); it!=opInitialResult.end(); ++it)
    {
        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

        wxString type;
        Token* token = tree->at((*it));
        if (token)
            type = token->m_BaseType;

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

        if (type.IsEmpty())
            continue;

        TokenIdxSet typeResult;
        ResolveActualType(tree, type, opInitialScope, typeResult);
        if (!typeResult.empty())
        {
            for (TokenIdxSet::iterator pTypeResult = typeResult.begin();
                 pTypeResult!=typeResult.end();
                 ++pTypeResult)
            {
                result.insert(*pTypeResult);
                AddTemplateAlias(tree, *pTypeResult, opInitialScope, result);
            }
        }
        else
            ResolveTemplateMap(tree, type, opInitialScope, result);
    }
}

size_t NativeParserBase::ResolveActualType(TokensTree*        tree,
                                           wxString           searchText,
                                           const TokenIdxSet& searchScope,
                                           TokenIdxSet&       result)
{
    // break up the search text for next analysis.
    std::queue<ParserComponent> typeComponents;
    BreakUpComponents(searchText, typeComponents);
    if (!typeComponents.empty())
    {
        TokenIdxSet initialScope;
        if (!searchScope.empty())
            initialScope = searchScope;
        else
            initialScope.insert(-1);

        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

        while (!typeComponents.empty())
        {
            TokenIdxSet initialResult;
            ParserComponent component = typeComponents.front();
            typeComponents.pop();
            wxString actualTypeStr = component.component;

            // All functions that call the recursive GenerateResultSet should already entered a critical section.
            GenerateResultSet(tree, actualTypeStr, initialScope, initialResult, true, false, 0xFFFF);

            if (!initialResult.empty())
            {
                initialScope.clear();
                for (TokenIdxSet::iterator it = initialResult.begin(); it != initialResult.end(); ++it)
                    // TODO (blueshake#1#): eclimate the variable/function
                    initialScope.insert(*it);
            }
            else
            {
                initialScope.clear();
                break;
            }
        }

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

        if (!initialScope.empty())
            result = initialScope;
    }

    return result.size();
}

void NativeParserBase::ResolveTemplateMap(TokensTree*        tree,
                                          const wxString&    searchStr,
                                          const TokenIdxSet& actualTypeScope,
                                          TokenIdxSet&       initialScope)
{
    if (actualTypeScope.empty())
        return;

    wxString actualTypeStr = searchStr;
    std::map<wxString, wxString>::iterator it = m_TemplateMap.find(actualTypeStr);
    if (it != m_TemplateMap.end())
    {
        actualTypeStr = it->second;
        TokenIdxSet actualTypeResult;
        ResolveActualType(tree, actualTypeStr, actualTypeScope, actualTypeResult);
        if (!actualTypeResult.empty())
        {
            for (TokenIdxSet::iterator it2=actualTypeResult.begin(); it2!=actualTypeResult.end(); ++it2)
                initialScope.insert(*it2);
        }
    }
}

void NativeParserBase::AddTemplateAlias(TokensTree*        tree,
                                        const int&         id,
                                        const TokenIdxSet& actualTypeScope,
                                        TokenIdxSet&       initialScope)
{
    if (!tree || actualTypeScope.empty())
        return;

    // and we need to add the template argument alias too.
    wxString actualTypeStr;

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    Token* typeToken = tree->at(id);
    if (typeToken &&  typeToken->m_TokenKind == tkTypedef
                  && !typeToken->m_TemplateAlias.IsEmpty() )
        actualTypeStr = typeToken->m_TemplateAlias;

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

    std::map<wxString, wxString>::iterator it = m_TemplateMap.find(actualTypeStr);
    if (it != m_TemplateMap.end())
    {
        actualTypeStr = it->second;
        TokenIdxSet actualTypeResult;
        ResolveActualType(tree, actualTypeStr, actualTypeScope, actualTypeResult);
        if (!actualTypeResult.empty())
        {
            for (TokenIdxSet::iterator it2 = actualTypeResult.begin(); it2 != actualTypeResult.end(); ++it2)
                initialScope.insert(*it2);
        }
    }
}

// No critical section needed in this recursive function!
// All functions that call this recursive function, should already entered a critical section.
size_t NativeParserBase::GenerateResultSet(TokensTree*     tree,
                                           const wxString& target,
                                           int             parentIdx,
                                           TokenIdxSet&    result,
                                           bool            caseSens,
                                           bool            isPrefix,
                                           short int       kindMask)
{
    TRACE(_T("NativeParserBase::GenerateResultSet_1()"));

    Token* parent = tree->at(parentIdx);
    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(F(_("GenerateResultSet() search '%s', parent='%s (id:%d, type:%s), isPrefix=%d'"),
                                    target.wx_str(),
#if wxCHECK_VERSION(2, 9, 0)
                                    parent ? parent->m_Name.wx_str() : _("Global namespace").wx_str(),
#else
                                    parent ? parent->m_Name.wx_str() : _("Global namespace"),
#endif
                                    parent ? parent->m_Index : 0,
                                    parent ? parent->GetTokenKindString().wx_str():0,
                                    isPrefix ? 1 : 0));

    // parent == null means we are searching in the global scope
    if (parent)
    {
        // we got a parent; add its children
        for (TokenIdxSet::iterator it = parent->m_Children.begin(); it != parent->m_Children.end(); ++it)
        {
            Token* token = tree->at(*it);
            if (token && MatchType(token->m_TokenKind, kindMask))
            {
                if (MatchText(token->m_Name, target, caseSens, isPrefix))
                    result.insert(*it);
                else if (token && token->m_TokenKind == tkNamespace && token->m_Aliases.size()) // handle namespace aliases
                {
                    for (size_t i = 0; i < token->m_Aliases.size(); ++i)
                    {
                        if (MatchText(token->m_Aliases[i], target, caseSens, isPrefix))
                        {
                            result.insert(*it);
                            // break; ?
                        }
                    }
                }
                else if (token && token->m_TokenKind == tkEnum) // check enumerators for match too
                    // All functions that call the recursive GenerateResultSet should already entered a critical section.
                    GenerateResultSet(tree, target, *it, result, caseSens, isPrefix, kindMask);
            }
        }
        // now go up the inheritance chain and add all ancestors' children too
        tree->RecalcInheritanceChain(parent);
        for (TokenIdxSet::iterator it = parent->m_Ancestors.begin(); it != parent->m_Ancestors.end(); ++it)
        {
            Token* ancestor = tree->at(*it);
            if (!ancestor)
                continue;
            for (TokenIdxSet::iterator it2 = ancestor->m_Children.begin(); it2 != ancestor->m_Children.end(); ++it2)
            {
                Token* token = tree->at(*it2);
                if (token && MatchType(token->m_TokenKind, kindMask))
                {
                    if (MatchText(token->m_Name, target, caseSens, isPrefix))
                        result.insert(*it2);
                    else if (token && token->m_TokenKind == tkNamespace && token->m_Aliases.size()) // handle namespace aliases
                    {
                        for (size_t i = 0; i < token->m_Aliases.size(); ++i)
                        {
                            if (MatchText(token->m_Aliases[i], target, caseSens, isPrefix))
                            {
                                result.insert(*it2);
                                // break; ?
                            }
                        }
                    }
                    else if (token && token->m_TokenKind == tkEnum) // check enumerators for match too
                        // All functions that call the recursive GenerateResultSet should already entered a critical section.
                        GenerateResultSet(tree, target, *it2, result, caseSens, isPrefix, kindMask);
                }
            }
        }
    }
    else
    {
        // all global tokens
        for (TokenList::iterator it = tree->m_Tokens.begin(); it != tree->m_Tokens.end(); ++it)
        {
            Token* token = *it;
            if (token && token->m_ParentIndex == -1)
            {
                if (token && MatchType(token->m_TokenKind, kindMask))
                {
                    if (MatchText(token->m_Name, target, caseSens, isPrefix))
                        result.insert(token->m_Index);
                    else if (token && token->m_TokenKind == tkNamespace && token->m_Aliases.size()) // handle namespace aliases
                    {
                        for (size_t i = 0; i < token->m_Aliases.size(); ++i)
                        {
                            if (MatchText(token->m_Aliases[i], target, caseSens, isPrefix))
                            {
                                result.insert(token->m_Index);
                                // break; ?
                            }
                        }
                    }
                    else if (token && token->m_TokenKind == tkEnum) // check enumerators for match too
                        // All functions that call the recursive GenerateResultSet should already entered a critical section.
                        GenerateResultSet(tree, target, token->m_Index, result, caseSens, isPrefix, kindMask);
                }
            }
        }
    }

    // done
    return result.size();
}

// No critical section needed in this recursive function!
// All functions that call this recursive function, should already entered a critical section.
size_t NativeParserBase::GenerateResultSet(TokensTree*        tree,
                                           const wxString&    target,
                                           const TokenIdxSet& ptrParentID,
                                           TokenIdxSet&       result,
                                           bool               caseSens,
                                           bool               isPrefix,
                                           short int          kindMask)
{
    if (!tree) return 0;

    TRACE(_T("NativeParserBase::GenerateResultSet_2()"));

    if (target.IsEmpty())
    {
        for (TokenIdxSet::iterator ptr = ptrParentID.begin(); ptr != ptrParentID.end(); ++ptr)
        {
            size_t parentIdx = (*ptr);
            Token* parent = tree->at(parentIdx);
            if (!parent)
                continue;

            for (TokenIdxSet::iterator it = parent->m_Children.begin(); it != parent->m_Children.end(); ++it)
            {
                Token* token = tree->at(*it);
                if (!token)
                    continue;
                if ( !AddChildrenOfUnnamed(tree, token, result) )
                    result.insert(*it);
            }

            tree->RecalcInheritanceChain(parent);

            for (TokenIdxSet::iterator it = parent->m_Ancestors.begin(); it != parent->m_Ancestors.end(); ++it)
            {
                Token* ancestor = tree->at(*it);
                if (!ancestor)
                    continue;
                for (TokenIdxSet::iterator it2 = ancestor->m_Children.begin(); it2 != ancestor->m_Children.end(); ++it2)
                {
                    Token* token = tree->at(*it2);
                    if (!token)
                        continue;
                    if ( !AddChildrenOfUnnamed(tree, token, result) )
                        result.insert(*it2);
                }
            }
        }
    }
    else
    {
        // we use FindMatches to get the items from tree directly and eclimate the
        // items which are not under the search scope.
        TokenIdxSet finalResult, tmpResult;
        if (tree->FindMatches(target, tmpResult, caseSens, isPrefix))
        {
            TokenIdxSet::iterator it;
            for (it = tmpResult.begin(); it != tmpResult.end(); ++it)
            {
                Token* token = tree->at(*it);
                if (token)
                    finalResult.insert(*it);
            }
        }

        if (!finalResult.empty())
        {
            TRACE(_T("Find %d result from the tree."), finalResult.size());
            //get the tokens under the search scope.
            for (TokenIdxSet::iterator ptr = ptrParentID.begin(); ptr != ptrParentID.end(); ++ptr)
            {
                //to make it clear, parentIdx stand for search scope.
                // (*it) stand for matched item id.
                int parentIdx = (*ptr);
                for (TokenIdxSet::iterator it = finalResult.begin(); it != finalResult.end(); ++it)
                {
                    Token* token = tree->at(*it);
                    if (token && (token->m_ParentIndex == parentIdx))
                        result.insert(*it);

                    //if the matched item id is under the search scope's ancestor scope.
                    //we need to add them too.
                    if (parentIdx != -1)
                    {
                        Token* tokenParent = tree->at(parentIdx);
                        if (tokenParent)
                        {
                            tree->RecalcInheritanceChain(tokenParent);

                            //match the ancestor scope,add them
                            //(*it2) should be the search scope ancestor's id(search scope)
                            for (TokenIdxSet::iterator it2=tokenParent->m_Ancestors.begin();
                                 it2!=tokenParent->m_Ancestors.end();
                                 ++it2)
                            {
                                if (token->m_ParentIndex == (*it2)) //matched
                                    result.insert(*it);
                            }
                        }
                    }
                    else if (-1 == parentIdx)
                    {
                        //if the search scope is global,and the token's parent token kind is tkEnum ,we add them too.
                        Token* parentToken = tree->at(token->m_ParentIndex);
                        if (parentToken && parentToken->m_TokenKind == tkEnum)
                            result.insert(*it);
                    }
                }
            }
        }
        else
        {
            //we need to handle namespace aliases too.I hope we can find a good to do this.
            //TODO: handle template class here.
            if (ptrParentID.count(-1))
            {
                for (TokenList::iterator it = tree->m_Tokens.begin(); it != tree->m_Tokens.end(); ++it)
                {
                    Token* token = (*it);
                    if (token && token->m_TokenKind == tkNamespace && token->m_Aliases.size())
                    {
                        for (size_t i = 0; i < token->m_Aliases.size(); ++i)
                        {
                            if (token->m_Aliases[i] == target)
                            {
                                result.insert(token->m_Index);
                                // break; ?
                            }
                        }
                    }
                }
            }
        }
    }

    return result.size();
}

void NativeParserBase::CollectSearchScopes(const TokenIdxSet& searchScope,
                                           TokenIdxSet&       actualTypeScope,
                                           TokensTree*        tree)
{
    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    for (TokenIdxSet::iterator pScope=searchScope.begin(); pScope!=searchScope.end(); ++pScope)
    {
        actualTypeScope.insert(*pScope);
        // we need to pScope's parent scope too.
        if ((*pScope) != -1)
        {
            Token* token = tree->at(*pScope);
            if (!token)
                continue;
            Token* parent = tree->at(token->m_ParentIndex);
            while (true)
            {
                if (!parent)
                    break;
                actualTypeScope.insert(parent->m_Index);
                parent = tree->at(parent->m_ParentIndex);
            }
        }
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
}

// No critical section needed in this recursive function!
// All functions that call this function, should already entered a critical section.
int NativeParserBase::GetTokenFromCurrentLine(TokensTree*        tree,
                                              const TokenIdxSet& tokens,
                                              size_t             curLine,
                                              const wxString&    file)
{
    TRACE(_T("NativeParserBase::GetTokenFromCurrentLine()"));

    int result = -1; bool found = false;
    if (!tree) return result;

    size_t fileIdx = tree->GetFileIndex(file);
    const Token* classToken = nullptr;
    for (TokenIdxSet::iterator it = tokens.begin(); it != tokens.end(); ++it)
    {
        const Token* token = tree->at(*it);
        if (!token)
            continue;

        TRACE(_T("GetTokenFromCurrentLine() Iterating: tN='%s', tF='%s', tStart=%d, tEnd=%d"),
              token->DisplayName().wx_str(), token->GetFilename().wx_str(),
              token->m_ImplLineStart, token->m_ImplLineEnd);

        if (   token->m_TokenKind & tkAnyFunction
            && token->m_ImplFileIdx == fileIdx
            && token->m_ImplLine    <= curLine
            && token->m_ImplLineEnd >= curLine)
        {
            TRACE(_T("GetTokenFromCurrentLine() tkAnyFunction : tN='%s', tF='%s', tStart=%d, tEnd=%d"),
                   token->DisplayName().wx_str(), token->GetFilename().wx_str(),
                   token->m_ImplLineStart, token->m_ImplLineEnd);
            result = token->m_Index;
            found = true;
        }
        else if (   token->m_TokenKind == tkConstructor
                 && token->m_ImplFileIdx == fileIdx
                 && token->m_ImplLine <= curLine
                 && token->m_ImplLineStart >= curLine)
        {
            TRACE(_T("GetTokenFromCurrentLine() tkConstructor : tN='%s', tF='%s', tStart=%d, tEnd=%d"),
                  token->DisplayName().wx_str(), token->GetFilename().wx_str(),
                  token->m_ImplLineStart, token->m_ImplLineEnd);
            result = token->m_Index;
            found = true;
        }
        else if (   token->m_TokenKind == tkClass
                 && token->m_ImplLineStart <= curLine
                 && token->m_ImplLineEnd >= curLine)
        {
            TRACE(_T("GetTokenFromCurrentLine() tkClass : tN='%s', tF='%s', tStart=%d, tEnd=%d"),
                  token->DisplayName().wx_str(), token->GetFilename().wx_str(),
                  token->m_ImplLineStart, token->m_ImplLineEnd);
            classToken = token;
            continue;
        }

        if (found) break; // exit for-loop

        TRACE(_T("GetTokenFromCurrentLine() Function out of bounds: tN='%s', tF='%s', tStart=%d, ")
              _T("tEnd=%d, line=%d (size_t)line=%d"), token->DisplayName().wx_str(),
              token->GetFilename().wx_str(), token->m_ImplLineStart, token->m_ImplLineEnd,
              curLine, curLine);
    }

    if (classToken)
        result = classToken->m_Index;

    return result;
}

void NativeParserBase::ComputeCallTip(TokensTree*        tree,
                                      const TokenIdxSet& tokens,
                                      int                chars_per_line,
                                      wxArrayString&     items)
{
    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    for (TokenIdxSet::iterator it = tokens.begin(); it != tokens.end(); ++it)
    {
        Token* token = tree->at(*it);
        if (!token)
            continue;

        // support constructor call tips
        if (token->m_TokenKind == tkClass)
        {
            Token* tk = tree->at(tree->TokenExists(token->m_Name, token->m_Index, tkConstructor));
            if (tk)
                token = tk;
        }

        // support macro call tips
        while (token->m_TokenKind == tkPreprocessor)
        {
            Token* tk = tree->at(tree->TokenExists(token->m_BaseType, -1, tkPreprocessor | tkFunction));
            if (tk && tk->m_BaseType != token->m_Name)
                token = tk;
            else
                break;
        }

        if (token->m_TokenKind == tkTypedef && token->m_BaseType.Contains(_T("(")))
            items.Add(token->m_BaseType); // typedef'd function pointer
        else
        {
            wxString s;
            wxString full;
            if ( !PrettyPrintToken(tree, token, full) )
                full = wxT("Error while pretty printing token!");

            BreakUpInLines(s, full, chars_per_line);

            items.Add(s);
        }
    }// for

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
}

bool NativeParserBase::PrettyPrintToken(const TokensTree* tree,
                                        const Token*      token,
                                        wxString&         result,
                                        bool              isRoot)
{
    // if the token has parents and the token is a container or a function,
    // then pretty print the parent of the token->
    if (   (token->m_ParentIndex != -1)
        && (token->m_TokenKind & (tkAnyContainer | tkAnyFunction)) )
    {
        const Token* parentToken = tree->at(token->m_ParentIndex);
        if (!parentToken || !PrettyPrintToken(tree, parentToken, result, false))
            return false;
    }

    switch (token->m_TokenKind)
    {
        case tkConstructor:
            result = result + token->m_Name + token->m_Args;
            return true;

        case tkFunction:
            result = token->m_BaseType + wxT(" ") + result + token->m_Name + token->m_Args;
            if (token->m_IsConst)
                result += wxT(" const");
            return true;

        case tkClass:
        case tkNamespace:
            if (isRoot)
                result += token->m_Name;
            else
                result += token->m_Name + wxT("::");
            return true;
        default:
            ;
    }
    return true;
}

void NativeParserBase::BreakUpInLines(wxString&       str,
                                      const wxString& original_str,
                                      int             chars_per_line)
{
    if (chars_per_line == -1 || original_str.Length() <= (size_t)chars_per_line)
    {
        str = original_str;
        return;
    }

    // break it up in lines
    size_t pos = 0;
    size_t copy_start = 0;
    int last_comma = -1;
    while (pos < original_str.Length())
    {
        wxChar c = original_str.GetChar(pos);

        if (c == _T(','))
            last_comma = pos;

        if (pos % chars_per_line == 0 && last_comma != -1)
        {
            str << original_str.Mid(copy_start, last_comma - copy_start + 1);
            str << _T('\n');
            copy_start = last_comma + 1;
        }
        else if (pos == original_str.Length() - 1)
            str << original_str.Mid(copy_start); // rest of the string
        ++pos;
    }
}
