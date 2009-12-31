/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "parserthread.h"
#include "parser.h"
#include <wx/app.h>
#include <wx/msgdlg.h>
#include <manager.h>
#include "logmanager.h"
#include <globals.h>

#include <cctype>
#include <queue>

#define PARSERTHREAD_DEBUG_OUTPUT 0

#if PARSERTHREAD_DEBUG_OUTPUT
    #define TRACE(format, args...)\
    Manager::Get()->GetLogManager()->DebugLog(F( format , ## args))
#else
    #define TRACE(format, args...)
#endif

int THREAD_START       = wxNewId();
int THREAD_END         = wxNewId();
int NEW_TOKEN          = wxNewId();
int FILE_NEEDS_PARSING = wxNewId();

namespace ParserConsts
{
    const wxString space           (_T(" "));
    const wxString spaced_colon    (_T(" : "));
    const wxString empty           (_T(""));
    const wxString equals          (_T("="));
    const wxString hash            (_T("#"));
    const wxString plus            (_T("+"));
    const wxString comma           (_T(","));
    const wxString commaclbrace    (_T(",}"));
    const wxString dash            (_T("-"));
    const wxString dot             (_T("."));
    const wxString colon           (_T(":"));
    const wxString dcolon          (_T("::"));
    const wxString semicolon       (_T(";"));
    const wxString semicolonopbrace(_T(";{"));
    const wxString semicolonclbrace(_T(";}"));
    const wxString lt              (_T("<"));
    const wxString gt              (_T(">"));
    const wxString gtsemicolon     (_T(">;"));
    const wxString unnamed         (_T("Unnamed"));
    const wxString quot            (_T("\""));
    const wxString kw_C            (_T("\"C\""));
    const wxString kw__asm         (_T("__asm"));
    const wxString kw_class        (_T("class"));
    const wxString kw_const        (_T("const"));
    const wxString kw_define       (_T("define"));
    const wxString kw_delete       (_T("delete"));
    const wxString kw_do           (_T("do"));
    const wxString kw_else         (_T("else"));
    const wxString kw_enum         (_T("enum"));
    const wxString kw_extern       (_T("extern"));
    const wxString kw_for          (_T("for"));
    const wxString kw_friend       (_T("friend"));
    const wxString kw_if           (_T("if"));
    const wxString kw_ifdef        (_T("ifdef"));
    const wxString kw_ifndef       (_T("ifndef"));
    const wxString kw_elif         (_T("elif"));
    const wxString kw_endif        (_T("endif"));
    const wxString kw_include      (_T("include"));
    const wxString kw_inline       (_T("inline"));
    const wxString kw_namespace    (_T("namespace"));
    const wxString kw_operator     (_T("operator"));
    const wxString kw_private      (_T("private"));
    const wxString kw_protected    (_T("protected"));
    const wxString kw_public       (_T("public"));
    const wxString kw_return       (_T("return"));
    const wxString kw_static       (_T("static"));
    const wxString kw_struct       (_T("struct"));
    const wxString kw_switch       (_T("switch"));
    const wxString kw_template     (_T("template"));
    const wxString kw_typedef      (_T("typedef"));
    const wxString kw_union        (_T("union"));
    const wxString kw_using        (_T("using"));
    const wxString kw_virtual      (_T("virtual"));
    const wxString kw_volatile     (_T("volatile"));
    const wxString kw_while        (_T("while"));
    const wxString opbrace         (_T("{"));
    const wxString opbracesemicolon(_T("{;"));
    const wxString clbrace         (_T("}"));
    const wxString tilde           (_T("~"));
};

ParserThread::ParserThread(Parser* parent,
                            const wxString& bufferOrFilename,
                            bool isLocal,
                            ParserThreadOptions& parserThreadOptions,
                            TokensTree* tokensTree) :
    m_Tokenizer(),
    m_pParent(parent),
    m_pTokensTree(tokensTree),
    m_pLastParent(0),
    m_LastScope(tsUndefined),
    m_Filename(wxEmptyString),
    m_FileSize(0),
    m_FileIdx(0),
    m_IsLocal(isLocal),
    m_Str(wxEmptyString),
    m_LastToken(wxEmptyString),
    m_Options(parserThreadOptions),
    m_EncounteredNamespaces(),
    m_EncounteredTypeNamespaces(),
    m_LastUnnamedTokenName(wxEmptyString),
    m_ParsingTypedef(false),
    m_PreprocessorIfCount(0),
    m_IsBuffer(parserThreadOptions.useBuffer),
    m_Buffer(bufferOrFilename)
{
    m_Tokenizer.SetTokenizerOption(parserThreadOptions.wantPreprocessor);
}

ParserThread::~ParserThread()
{
    //dtor

    // wait for file loader object to complete (can't abort it)
    if (m_Options.loader)
    {
        m_Options.loader->Sync();
        Delete(m_Options.loader);
    }
}

void ParserThread::Log(const wxString& log)
{
    if (TestDestroy())
        return;
    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, NEW_TOKEN);
    event.SetString(log);
    event.SetInt(m_Tokenizer.GetLineNumber());
    wxPostEvent(m_pParent, event);
//    Manager::ProcessPendingEvents();
}

void ParserThread::SetTokens(TokensTree* tokensTree)
{
    m_pTokensTree = tokensTree;
}

void* ParserThread::DoRun()
{
//    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, THREAD_START);
//    event.SetString(m_Filename);
//    event.SetInt((int)this);
//    wxPostEvent(m_pParent, event);
//
////    Log("ParserThread running for " + m_Filename);
//    Parse();
//
//    wxCommandEvent event1(wxEVT_COMMAND_MENU_SELECTED, THREAD_END);
//    event1.SetString(m_Filename);
//    event1.SetInt((int)this);
//    wxPostEvent(m_pParent, event1);

    return 0L;
}

wxChar ParserThread::SkipToOneOfChars(const wxString& chars, bool supportNesting)
{
    unsigned int level = m_Tokenizer.GetNestingLevel();
    while (1)
    {
        if (TestDestroy())
            return '\0';
        wxString token = m_Tokenizer.GetToken();
        if (token.IsEmpty())
            return '\0'; // eof

        if (!supportNesting || m_Tokenizer.GetNestingLevel() == level)
        {
            wxChar ch = token.GetChar(0);
            if (chars.Find(ch) != wxNOT_FOUND)
                return ch;
        }
    }
}

void ParserThread::SkipBlock()
{
    // need to force the tokenizer _not_ skip anything
    // or else default values for template params would cause us to miss everything (because of the '=' symbol)
    bool oldState = m_Tokenizer.IsSkippingUnwantedTokens();
    m_Tokenizer.SetSkipUnwantedTokens(false);

    // skip tokens until we reach }
    // block nesting is taken into consideration too ;)

    // this is the nesting level we start at
    // we subtract 1 because we 're already inside the block
    // (since we 've read the {)
    unsigned int level = m_Tokenizer.GetNestingLevel() - 1;
    while (1)
    {
        if (TestDestroy())
            return;
        wxString token = m_Tokenizer.GetToken();
        if (token.IsEmpty())
            break; // eof
        else if (token == ParserConsts::hash)
        {
            token = m_Tokenizer.GetToken();
            HandlePreprocessorBlocks(token);
        }

        // if we reach the initial nesting level, we are done
        if (level == m_Tokenizer.GetNestingLevel())
            break;
    }

    // reset tokenizer's functionality
    m_Tokenizer.SetSkipUnwantedTokens(oldState);
}

void ParserThread::SkipAngleBraces()
{
    // need to force the tokenizer _not_ skip anything
    // or else default values for template params would cause us to miss everything (because of the '=' symbol)
    bool oldState = m_Tokenizer.IsSkippingUnwantedTokens();
    m_Tokenizer.SetSkipUnwantedTokens(false);

    int nestLvl = 0;
    // NOTE: only exit this loop with 'break' so the tokenizer's state can
    // be reset afterwards (i.e. don't use 'return')
    while (true)
    {
        if (TestDestroy())
            break;
        wxString tmp = m_Tokenizer.GetToken();
        if (tmp==ParserConsts::lt)
            ++nestLvl;
        else if (tmp==ParserConsts::gt)
            --nestLvl;
        else if (tmp==ParserConsts::semicolon)
        {
            // unget token - leave ; on the stack
            m_Tokenizer.UngetToken();
            break;
        }
        else if (tmp.IsEmpty())
            break;
        if (nestLvl <= 0)
            break;
    }

    // reset tokenizer's functionality
    m_Tokenizer.SetSkipUnwantedTokens(oldState);
}

bool ParserThread::ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result)
{
    if (TestDestroy())
        return false;

    m_Tokenizer.InitFromBuffer(buffer);
    if (!m_Tokenizer.IsOK())
        return false;

    result.Clear();
    m_Str.Clear();
    m_LastUnnamedTokenName.Clear();
    m_ParsingTypedef = false;

    // Notice: clears the queue "m_EncounteredTypeNamespaces"
    while (!m_EncounteredTypeNamespaces.empty())
        m_EncounteredTypeNamespaces.pop();

    // Notice: clears the queue "m_EncounteredNamespaces"
    while (!m_EncounteredNamespaces.empty())
        m_EncounteredNamespaces.pop();

    while (m_Tokenizer.NotEOF())
    {
        if (!m_pTokensTree || TestDestroy())
            return false;

        wxString token = m_Tokenizer.GetToken();
        if (token.IsEmpty())
            continue;

        if (token==ParserConsts::kw_namespace)
        {
            // need this too
            token = m_Tokenizer.GetToken();
            SkipToOneOfChars(ParserConsts::opbrace);

            if (!token.IsEmpty())
                result.Add(token);
        }
        else if (token==ParserConsts::opbrace)
        {
            SkipBlock();
        }
        else if (token==ParserConsts::kw_using)
        {
            wxString peek = m_Tokenizer.PeekToken();
            if (peek == ParserConsts::kw_namespace)
            {
                // ok
                m_Tokenizer.GetToken(); // eat namespace
                while (true) // support full namespaces
                {
                    m_Str << m_Tokenizer.GetToken();
                    if (m_Tokenizer.PeekToken() == ParserConsts::dcolon)
                        m_Str << m_Tokenizer.GetToken();
                    else
                        break;
                }
                if (!m_Str.IsEmpty())
                    result.Add(m_Str);
                m_Str.Clear();
            }
            else
                SkipToOneOfChars(ParserConsts::semicolonclbrace);
        }
    }
    return true;
}

bool ParserThread::InitTokenizer()
{
    if (!m_Buffer.IsEmpty())
    {
        if (!m_IsBuffer)
        {
            wxFile file(m_Buffer);
            
            if (file.IsOpened())
            {
                m_Filename = m_Buffer;
                m_FileSize = file.Length();

                TRACE(_T("InitTokenizer() : m_Filename='%s', m_FileSize=%d."), m_Filename.c_str(), m_FileSize);

                bool ret = m_Tokenizer.Init(m_Filename, m_Options.loader);
                Delete(m_Options.loader);
                return ret;
            }
            
            return false;
        }

        return m_Tokenizer.InitFromBuffer(m_Buffer);
    }
    return false;
}

bool ParserThread::Parse()
{
    TRACE(_T("Parse() : parsing %s"),m_Filename.c_str());
    if (!InitTokenizer())
        return false;
    bool result = false;
    m_ParsingTypedef = false;

    do
    {
        if (!m_pTokensTree || !m_Tokenizer.IsOK())
            break;

        if (!m_Options.useBuffer) // Parse a file
        {
            s_MutexProtection.Enter();
            m_FileIdx = m_pTokensTree->ReserveFileForParsing(m_Filename);
            s_MutexProtection.Leave();
            if (!m_FileIdx)
                break;
        }

        DoParse();

        if (!m_Options.useBuffer) // Parsing a file
        {
            s_MutexProtection.Enter();
            m_pTokensTree->FlagFileAsParsed(m_Filename);
            s_MutexProtection.Leave();
        }
        result = true;
    } while(false);

    return result;
}

void ParserThread::DoParse()
{
    // need to reset tokenizer's behavior
    // don't forget to reset that if you add any early exit condition!
    bool oldState = m_Tokenizer.IsSkippingUnwantedTokens();
    m_Tokenizer.SetSkipUnwantedTokens(true);

    m_Str.Clear();
    m_LastToken.Clear();
    m_LastUnnamedTokenName.Clear();

    // Notice: clears the queue "m_EncounteredTypeNamespaces"
    while (!m_EncounteredTypeNamespaces.empty())
        m_EncounteredTypeNamespaces.pop();

    // Notice: clears the queue "m_EncounteredNamespaces"
    while (!m_EncounteredNamespaces.empty())
        m_EncounteredNamespaces.pop();

    while (m_Tokenizer.NotEOF())
    {
        if (!m_pTokensTree || TestDestroy())
            break;

        wxString token = m_Tokenizer.GetToken();
        if (token.IsEmpty())
            continue;

        TRACE(_T("DoParse() : Loop:m_Str='%s', token='%s'"), m_Str.c_str(), token.c_str());

        if (token == _T("template"))
        {
            wxString args = m_Tokenizer.GetToken(false,true);
            token = m_Tokenizer.GetToken();
            if (args == _T("<>"))
            {
                m_Str.Clear();
                wxString name = m_Tokenizer.GetToken();
                args = m_Tokenizer.PeekToken(false,true);
                m_Tokenizer.UngetToken();
            }
            if (token==ParserConsts::kw_class)
            {
                m_Str.Clear();
                if (m_Options.handleClasses)
                    HandleClass(ctClass,args);
                else
                    SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
                continue;
            }
            else if (token==ParserConsts::kw_struct)
            {
                m_Str.Clear();
                if (m_Options.handleClasses)
                    HandleClass(ctStructure,args);
                else
                    SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
                continue;
            }
            else
            {
                m_Tokenizer.UngetToken();
            }
        }

        if (token==ParserConsts::semicolon)
        {
            m_Str.Clear();
            // Notice: clears the queue "m_EncounteredTypeNamespaces"
            while (!m_EncounteredTypeNamespaces.empty())
                m_EncounteredTypeNamespaces.pop();
        }
        else if (token==ParserConsts::kw_delete ||
                (token==ParserConsts::dot) ||
                (token==ParserConsts::gt && m_LastToken==ParserConsts::dash))
        {
            m_Str.Clear();
            SkipToOneOfChars(ParserConsts::semicolonclbrace);
        }
        else if (token==ParserConsts::opbrace)
        {
            if (!m_Options.useBuffer || m_Options.bufferSkipBlocks)
                SkipBlock();
            m_Str.Clear();
        }
        else if (token==ParserConsts::clbrace)
        {
            m_pLastParent = 0L;
            m_LastScope = tsUndefined;
            m_Str.Clear();
            // the only time we get to find a } is when recursively called by e.g. HandleClass
            // we have to return now...
            if (!m_Options.useBuffer || m_Options.bufferSkipBlocks)
                break;
        }
        else if (token==ParserConsts::colon)
        {
            if (m_LastToken==ParserConsts::kw_public)
                m_LastScope = tsPublic;
            else if (m_LastToken==ParserConsts::kw_protected)
                m_LastScope = tsProtected;
            else if (m_LastToken==ParserConsts::kw_private)
                m_LastScope = tsPrivate;
            m_Str.Clear();
        }
        else if (   (token==ParserConsts::kw_while)
                 || (token==ParserConsts::kw_if)
                 || (token==ParserConsts::kw_do)
                 || (token==ParserConsts::kw_else)
                 || (token==ParserConsts::kw_for)
                 || (token==ParserConsts::kw_switch) )
        {
            if (!m_Options.useBuffer || m_Options.bufferSkipBlocks)
                SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
            else
                m_Tokenizer.GetToken(); //skip args
            m_Str.Clear();
        }
        else if (token==ParserConsts::kw_typedef)
        {
            if (m_Options.handleTypedefs)
                HandleTypedef();
            else
                SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
            m_Str.Clear();
        }
        else if (token==ParserConsts::kw_return ||
            token==ParserConsts::colon)
        {
            SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
            m_Str.Clear();
        }
        else if (token==ParserConsts::kw_const)
        {
            // do nothing, just skip keyword "const", otherwise uncomment:
            //m_Str.Clear();
        }
        else if (token==ParserConsts::kw_extern)
        {
            // check for "C"
            m_Str = m_Tokenizer.GetToken();
            if (m_Str==ParserConsts::kw_C)
            {
                m_Tokenizer.GetToken(); // "eat" {
                DoParse(); // time for recursion ;)
            }
            else
            {
                // do nothing, just skip keyword "extern", otherwise uncomment:
                //SkipToOneOfChars(ParserConsts::semicolon); // skip externs
            }
            m_Str.Clear();
        }
        else if (token.StartsWith(ParserConsts::kw__asm))
        {
            SkipToOneOfChars(ParserConsts::semicolon, true);
        }
        else if (   token==ParserConsts::kw_static
                 || token==ParserConsts::kw_virtual
                 || token==ParserConsts::kw_inline)
        {
            // do nothing, just skip keyword "static" / "virtual" / "inline"
        }
        else if (token==ParserConsts::hash)
        {
            token = m_Tokenizer.GetToken();
            if (token==ParserConsts::kw_include)
                HandleIncludes();
            else if (token==ParserConsts::kw_define)
                HandleDefines();
            else
                HandlePreprocessorBlocks(token);
            m_Str.Clear();
        }
        else if (token==ParserConsts::kw_using) // using namespace ?
        {
            SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
            m_Str.Clear();
        }
        else if (token==ParserConsts::kw_namespace)
        {
            m_Str.Clear();
            HandleNamespace();
        }
        else if (token==ParserConsts::kw_template)
        {
            m_Str.Clear();
            if (m_Tokenizer.PeekToken() == ParserConsts::lt) // should be!
                SkipAngleBraces();
        }
        else if (token==ParserConsts::kw_friend)
        {
            // friend methods can be either the decl only or an inline implementation
            SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
            m_Str.Clear();
        }
        else if (token==ParserConsts::kw_class)
        {
            m_Str.Clear();
            if (m_Options.handleClasses)
                HandleClass(ctClass);
            else
                SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
        }
        else if (token==ParserConsts::kw_struct)
        {
            m_Str.Clear();
            if (m_Options.handleClasses)
                HandleClass(ctStructure);
            else
                SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
        }
        else if (token==ParserConsts::kw_enum)
        {
            m_Str.Clear();
            if (m_Options.handleEnums)
                HandleEnum();
            else
                SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
        }
        else if (token==ParserConsts::kw_union)
        {
            m_Str.Clear();
            if (m_Options.handleClasses)
                HandleClass(ctUnion);
            else
                SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
        }
        else if (token==ParserConsts::kw_operator)
        {
            bool oldState = m_Tokenizer.IsSkippingUnwantedTokens();
            m_Tokenizer.SetSkipUnwantedTokens(false);
            m_Tokenizer.SetOperatorState(true);
            wxString func = token;
            while (1)
            {
                token = m_Tokenizer.GetToken();
                if (!token.IsEmpty())
                {
                    if (token.GetChar(0) == '(')
                    {
                        // check for operator()()
                        wxString peek = m_Tokenizer.PeekToken();
                        if (!peek.IsEmpty() && peek.GetChar(0) != '(')
                            m_Tokenizer.UngetToken();
                        else
                            func << token;
                        break;
                    }
                    else
                        func << token;
                }
                else
                    break;
            }
            m_Tokenizer.SetSkipUnwantedTokens(oldState);
            m_Tokenizer.SetOperatorState(false);
            HandleFunction(func, true);
            m_Str.Clear();
        }
        else
        {
            wxString peek = m_Tokenizer.PeekToken();
            if (!peek.IsEmpty())
            {
                if (   (peek.GetChar(0) == '(')
                    && m_Options.handleFunctions
                    && m_Str.IsEmpty()
                    && m_EncounteredNamespaces.empty()
                    && m_EncounteredTypeNamespaces.empty()
                    && (!m_pLastParent || m_pLastParent->m_Name != token) ) // if func has same name as current scope (class)
                {
                    // test Macro
                    if (token.Upper() == token)
                    {
                        HandleMacro(token, peek);
                        m_Tokenizer.GetToken();
                        m_Str.Clear();
                    }
                    else
                    {
                        wxString arg = m_Tokenizer.GetToken(); // eat args ()
                        m_Str = token+arg;
                    }
                }
                else if (peek.GetChar(0) == '(' && m_Options.handleFunctions)
                {
                    if (!m_Options.useBuffer || m_Options.bufferSkipBlocks)
                    {
                        // function
                        HandleFunction(token);
                    }
                    else
                        m_Tokenizer.GetToken(); // eat args when parsing block
                    m_Str.Clear();
                }
                else if (   (peek  == ParserConsts::colon)
                         && (token != ParserConsts::kw_private)
                         && (token != ParserConsts::kw_protected)
                         && (token != ParserConsts::kw_public) )
                {
                    // example decl to encounter a colon is when defining a bitfield: int x:1,y:1,z:1;
                    // token should hold the var (x/y/z)
                    // m_Str should hold the type (int)
                    if (m_Options.handleVars)
                        DoAddToken(tkVariable, token, m_Tokenizer.GetLineNumber());

                    m_Tokenizer.GetToken(); // skip colon
                    m_Tokenizer.GetToken(); // skip bitfield
                    m_Tokenizer.GetToken(); // skip comma
                }
                else if (peek==ParserConsts::comma)
                {
                    // example decl to encounter a comma: int x,y,z;
                    // token should hold the var (x/y/z)
                    // m_Str should hold the type (int)
                    if (!m_Str.IsEmpty() && m_Options.handleVars)
                        DoAddToken(tkVariable, token, m_Tokenizer.GetLineNumber());

                    // else it's a syntax error; let's hope we can recover from this...
                    // skip comma (we had peeked it)
                    m_Tokenizer.GetToken();
                }
                else if (peek==ParserConsts::lt)
                {
                    // a template, e.g. someclass<void>::memberfunc
                    // we have to skip <>, so we 're left with someclass::memberfunc
                    SkipAngleBraces();
                    peek = m_Tokenizer.PeekToken();
                    if (peek==ParserConsts::dcolon)
                    {
                        TRACE(_T("DoParse() : peek='::', token='") + token + _T("', m_LastToken='") + m_LastToken + _T("', m_Str='") + m_Str + _T("'"));
                        if (m_Str.IsEmpty())
                            m_EncounteredTypeNamespaces.push(token); // it's a type's namespace
                        else
                            m_EncounteredNamespaces.push(token);
                        m_Tokenizer.GetToken(); // eat ::
                    }
                    else // case like, std::map<int, int> somevar;
                        m_Str << token << _T(' ');
                }
                else if (peek==ParserConsts::dcolon)
                {
                    if (m_Str.IsEmpty())
                        m_EncounteredTypeNamespaces.push(token); // it's a type's namespace
                    else
                        m_EncounteredNamespaces.push(token);
                    m_Tokenizer.GetToken(); // eat ::
                }
                else if (   m_pTokensTree
                         && (   (peek==ParserConsts::semicolon)
                             || (   (m_Options.useBuffer && (peek.GetChar(0) == _T('(')))
                                 && (!m_Str.Contains(ParserConsts::dcolon)) ) ) )
                {
                    if (!m_Str.IsEmpty() && (wxIsalpha(token.GetChar(0)) || token.GetChar(0) == '_'))
                    {
                        if (m_Options.handleVars)
                            DoAddToken(tkVariable, token, m_Tokenizer.GetLineNumber());
                        else
                            SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
                    }
                }
                else if (!m_EncounteredNamespaces.empty())
                {
                    // Notice: clears the queue "m_EncounteredNamespaces", too
                    while (!m_EncounteredNamespaces.empty())
                    {
                        m_EncounteredTypeNamespaces.push( m_EncounteredNamespaces.front() );
                        m_EncounteredNamespaces.pop();
                    }
                    m_Str = token;
                }
                else
                {
                    m_Str << token << _T(' ');
                }
            }
        }
        m_LastToken = token;
    }
    // reset tokenizer behavior
    m_Tokenizer.SetSkipUnwantedTokens(oldState);
}

Token* ParserThread::TokenExists(const wxString& name, Token* parent, short int kindMask)
{
    Token* result;
    if (!m_pTokensTree)
        return 0;
    int parentidx = !parent ? -1 : parent->GetSelf();
    // no critical section needed here:
    // all functions that call this, already entered a critical section.
    result = m_pTokensTree->at(m_pTokensTree->TokenExists(name, parentidx, kindMask));
    return result;
}

wxString ParserThread::GetActualTokenType()
{
    TRACE(_T("GetActualTokenType() : Searching within m_Str='%s'"), m_Str.wx_str());

    // we will compensate for spaces between
    // namespaces (e.g. NAMESPACE :: SomeType) wich is valid C++ construct
    // we 'll remove spaces that follow a semicolon
    int pos = 0;
    while (pos < (int)m_Str.Length())
    {
        if (   (m_Str.GetChar(pos) == ' ')
            && (   (   (pos > 0)
                    && (m_Str.GetChar(pos - 1) == ':') )
                || (   (pos < (int)m_Str.Length() - 1)
                    && (m_Str.GetChar(pos + 1) == ':') ) ) )
        {
            m_Str.Remove(pos, 1);
        }
        else
            ++pos;
    }

    TRACE(_T("GetActualTokenType() : Compensated m_Str='%s'"), m_Str.wx_str());

    // m_Str contains the full text before the token's declaration
    // an example m_Str value would be: const wxString&
    // what we do here is locate the actual return value (wxString in this example)
    // it will be needed by code completion code ;)
    pos = m_Str.Length() - 1;

    // we walk m_Str backwards until we find a non-space character which also is
    // not * or &
    //                        const wxString&
    // in this example, we would stop here ^
    while (   (pos >= 0)
           && (   wxIsspace(m_Str.GetChar(pos))
               || (m_Str.GetChar(pos) == '*')
               || (m_Str.GetChar(pos) == '&')) )
    {
        --pos;
    }

    if (pos >= 0)
    {
        // we have the end of the word we 're interested in
        int end = pos;

        // continue walking backwards until we find the start of the word
        //                               const wxString&
        // in this example, we would stop here ^
        while (   (pos >= 0)
               && (   wxIsalnum(m_Str.GetChar(pos))
                   || (m_Str.GetChar(pos) == '_')
                   || (m_Str.GetChar(pos) == ':')) )
        {
            --pos;
        }

        TRACE(_T("GetActualTokenType() : Found '%s'"), m_Str.Mid(pos + 1, end - pos).wx_str());
        return m_Str.Mid(pos + 1, end - pos);
    }

    TRACE(_T("GetActualTokenType() : Returning '%s'"), m_Str.wx_str());
    return m_Str; // token ends at start of phrase
}

Token* ParserThread::FindTokenFromQueue(std::queue<wxString>& q, Token* parent, bool createIfNotExist, Token* parentIfCreated)
{
    if (q.empty())
        return 0;

    wxString ns = q.front();
    q.pop();

    Token* result = TokenExists(ns, parent, tkNamespace | tkClass);

    if (!result && createIfNotExist)
    {
        result = new Token(ns, m_FileIdx, 0);
        result->m_TokenKind = q.empty() ? tkClass : tkNamespace;
        result->m_IsLocal = m_IsLocal;
        result->m_ParentIndex = parentIfCreated ? parentIfCreated->GetSelf() : -1;
        int newidx = m_pTokensTree->insert(result);
        if (parentIfCreated)
            parentIfCreated->AddChild(newidx);

        TRACE(_T("FindTokenFromQueue() : Created unknown class/namespace %s (%d) under %s (%d)"),
              ns.wx_str(),
              newidx,
              parent ? parent->m_Name.wx_str() : _T("<globals>"),
              parent ? parent->GetSelf() : -1);
    }

    if (q.empty())
        return result;

    if (result)
        result = FindTokenFromQueue(q, result, createIfNotExist, parentIfCreated);

    return result;
}

static wxString GetRealArgs(const wxString & args)
{
    const wxChar * ptr = args;
    wxString str;
    wxString word;
    bool skip = false; // next char skip
    bool sym  = false; // current char symbol
    while (*ptr != _T('\0'))
    {
        switch (*ptr)
        {
        case _T(' '):
            if (   (word == ParserConsts::kw_const)
                || (word == ParserConsts::kw_volatile) )
                skip = false;
            else
                skip = true;
            word = _T("");
            sym  = false;
            break;
        case _T('*'):
        case _T('&'):
            word = _T("");
            skip = true;
            sym  = true;
            // test
            {
                int comma = str.rfind(_T(','));
                int begin = 1;
                if (comma != -1)
                    begin = comma;
                if ( str.Mid(begin).Find(_T('('), begin) == -1)
                {
                    str += *ptr;
                    // find end
                    int n = 0;
                    ptr++; // next char
                    while (*ptr != _T('\0'))
                    {
                        if (*ptr == _T('('))
                            n++;
                        else if (*ptr == _T(')'))
                        {
                            if (n == 0)
                                break;
                            n--;
                        }
                        else if (*ptr == _T(','))
                        {
                            sym = true;
                            skip = false;
                            break;
                        }
                        ptr++; // next char
                    }
                }
            }
            break;
        case _T(':'):
            skip = false;
            sym  = true;
            break;
        case _T('['):
            while (*ptr != _T('\0') && *(ptr) != _T(']'))
            {
                if (*ptr != _T(' '))
                    str += *ptr;
                ptr++; // next char
            }
            skip = true;
            sym  = true;
            break;
        case _T('<'):
            while (*ptr != _T('\0') && *(ptr) != _T('>'))
            {
                if (*ptr != _T(' '))
                    str += *ptr;
                ptr++; // next char
            }
            skip = true;
            sym  = true;
            break;
        case _T(','):
        case _T(')'):
        case _T('('):
            word = _T("");
            sym  = true;
            skip = false;
            break;
        default:
            sym = false;
        }

        if (!skip || sym)
        {
            if (*ptr != _T('\0'))
            {
                str += *ptr;
                if (wxIsalnum(*ptr))
                    word += *ptr;
            }
        }

        if (sym && !skip)
        {
            while (*ptr != _T('\0') && *(ptr+1) == _T(' '))
                ptr++; // next char
        }

        if (*ptr != _T('\0'))
            ptr++; // next char
    }

    return str;
}

Token* ParserThread::DoAddToken(TokenKind kind,
                                const wxString& name,
                                int line,
                                int implLineStart,
                                int implLineEnd,
                                const wxString& args,
                                bool isOperator,
                                bool isImpl)
{
    if (TestDestroy())
        return 0;

    if (name.IsEmpty())
        return 0; // oops!

    s_MutexProtection.Enter();
    Token* newToken = 0;
    wxString newname(name);
    m_Str.Trim();
    if (kind == tkDestructor)
    {
        // special class destructors case
        newname.Prepend(ParserConsts::tilde);
        m_Str.Clear();
    }

    wxString realArgs = _T("");
    if (kind & tkAnyFunction)
        realArgs = GetRealArgs(args);

    Token* localParent = 0;

    // preserve m_EncounteredTypeNamespaces; needed further down this function
    std::queue<wxString> q = m_EncounteredTypeNamespaces;
    if ((kind == tkDestructor || kind == tkConstructor) && !q.empty())
    {
        // look in m_EncounteredTypeNamespaces
        localParent = FindTokenFromQueue(q, 0, true, m_pLastParent);
        if (localParent)
            newToken = TokenExists(newname, localParent);
        if (newToken) TRACE(_T("DoAddToken() : Found token (ctor/dtor)."));
    }

    // check for implementation member function
    if (!newToken && !m_EncounteredNamespaces.empty())
    {
        localParent = FindTokenFromQueue(m_EncounteredNamespaces, 0, true, m_pLastParent);
        if (localParent)
            newToken = TokenExists(newname, localParent);
        if (newToken) TRACE(_T("DoAddToken() : Found token (member function)."));
    }

    // none of the above; check for token under parent (but not if we 're parsing a temp buffer)
    if (!newToken && !m_Options.isTemp)
    {
        newToken = TokenExists(name, m_pLastParent, kind);
        if (newToken) TRACE(_T("DoAddToken() : Found token (parent)."));
    }


    wxString newTokenArgs = (newToken) ? (newToken->m_Args) : _T("");
//    if (newToken && newToken->m_TokenKind == kind && newTokenArgs == args)
       if (   newToken
        && (newToken->m_TokenKind == kind)
        && (   (newTokenArgs == args)
            || (kind & tkAnyFunction && newToken->m_RealArgs == realArgs) ) )
    {
        m_pTokensTree->m_modified = true;
    }
    else
    {
        Token* finalParent = localParent ? localParent : m_pLastParent;
        newToken = new Token(newname, m_FileIdx, line);

        if (newToken) TRACE(_T("DoAddToken() : Created token='%s', file_idx=%d, line=%d"), newname.wx_str(), m_FileIdx, line);

        newToken->m_ParentIndex = finalParent ? finalParent->GetSelf() : -1;
        newToken->m_TokenKind   = kind;
        newToken->m_Scope       = m_LastScope;
        newToken->m_RealArgs    = realArgs;

        if (newToken->m_TokenKind == tkClass)
            newToken->m_RealArgs = args;    //save template args
        else
            newToken->m_Args = args;

        int newidx = m_pTokensTree->insert(newToken);

        if (finalParent)
            finalParent->AddChild(newidx);
    }

    if (!(kind & (tkConstructor | tkDestructor)))
    {
        wxString tokenType       = m_Str;
        wxString actualTokenType = GetActualTokenType();
        if (actualTokenType.Find(_T(' ')) == wxNOT_FOUND)
        {
            // token type must contain all namespaces
            wxString prepend;

            // Notice: clears the queue "m_EncounteredTypeNamespaces", too
            while (!m_EncounteredTypeNamespaces.empty())
            {
                prepend << m_EncounteredTypeNamespaces.front() << ParserConsts::dcolon;
                m_EncounteredTypeNamespaces.pop();
            }

            TRACE(_T("DoAddToken() : Prepending '%s'"), prepend.wx_str());
            actualTokenType.Prepend(prepend);
        }
        newToken->m_Type       = tokenType;
        newToken->m_ActualType = actualTokenType;
    }

    newToken->m_IsLocal    = m_IsLocal;
    newToken->m_IsTemp     = m_Options.isTemp;
    newToken->m_IsOperator = isOperator;

    if (!isImpl)
    {

        newToken->m_FileIdx = m_FileIdx;
        newToken->m_Line    = line;
        TRACE(_T("DoAddToken() : Added/updated token '%s' (%d), type '%s', actual '%s'. Parent is %s (%d)"),
              name.wx_str(),
              newToken->GetSelf(),
              newToken->m_Type.wx_str(),
              newToken->m_ActualType.wx_str(),
              newToken->GetParentName().wx_str(),
              newToken->m_ParentIndex);

    }
    else
    {
        newToken->m_ImplFileIdx   = m_FileIdx;
        newToken->m_ImplLine      = line;
        newToken->m_ImplLineStart = implLineStart;
        newToken->m_ImplLineEnd   = implLineEnd;
        m_pTokensTree->m_FilesMap[newToken->m_ImplFileIdx].insert(newToken->GetSelf());
    }

    // Notice: clears the queue "m_EncounteredTypeNamespaces"
    while (!m_EncounteredTypeNamespaces.empty())
        m_EncounteredTypeNamespaces.pop();

    // Notice: clears the queue "m_EncounteredNamespaces"
    while (!m_EncounteredNamespaces.empty())
        m_EncounteredNamespaces.pop();

    s_MutexProtection.Leave();
    return newToken;
}

void ParserThread::HandleIncludes()
{
    wxString filename;
    bool isGlobal = !m_IsLocal;
    wxString token = m_Tokenizer.GetToken();
    // now token holds something like:
    // "someheader.h"
    // < and will follow iostream.h, >
    if (TestDestroy())
        return;
    if (!token.IsEmpty())
    {
        if (token.GetChar(0) == '"')
        {
            // "someheader.h"
            // don't use wxString::Replace(); it's too costly
            size_t pos = 0;
            while (pos < token.Length())
            {
                wxChar c = token.GetChar(pos);
                if (c != _T('"'))
                    filename << c;
                ++pos;
            }
        }
        else if (token.GetChar(0) == '<')
        {
            isGlobal = true;
            // next token is filename, next is . (dot), next is extension
            // basically we 'll loop until >
            while (1)
            {
                token = m_Tokenizer.GetToken();
                if (token.IsEmpty())
                    break;
                if (token.GetChar(0) != '>')
                    filename << token;
                else
                    break;
            }
        }
    }

    if (!filename.IsEmpty())
    {
        do
        {
            // setting all #includes as global
            // it's amazing how many projects use #include "..." for global headers (MSVC mainly - booh)
            isGlobal = true;

            if (!(isGlobal ? m_Options.followGlobalIncludes : m_Options.followLocalIncludes))
                break; // Nothing to do!

            wxString real_filename = m_pParent->GetFullFileName(m_Filename,filename,isGlobal);
            // Parser::GetFullFileName is thread-safe :)

            if (real_filename.IsEmpty())
                break; // File not found, do nothing.

            {
                wxCriticalSectionLocker lock(s_MutexProtection);
                if (m_pTokensTree->IsFileParsed(real_filename))
                    break; // Already being parsed elsewhere
            }

            // since we 'll be calling directly the parser's method, let's make it thread-safe
            {
                wxCriticalSectionLocker lock2(s_mutexListProtection);
                m_pParent->OnParseFile(real_filename, isGlobal ? 1 : 0);
            }
        } while(false);
    }
}

void ParserThread::HandleDefines()
{
    wxString filename;
    int lineNr = m_Tokenizer.GetLineNumber();
    wxString token = m_Tokenizer.GetToken(); // read the token after #define
    m_Str.Clear();
    // now token holds something like:
    // BLAH_BLAH
    if (!token.IsEmpty())
    {
        // skip the rest of the #define
        wxString defVal = token + m_Tokenizer.ReadToEOL();
        wxString para(_T(""));
        size_t start = defVal.find('(');
        size_t end = defVal.find(')');

        TRACE(_T("HandleDefines() : Saving nesting level: %d,%d"), start, end);
        // make sure preprocessor definitions are not going under namespaces or classes!
        if (start != wxString::npos && end != wxString::npos)
        {
            para = defVal.Mid(start, end-start+1);
            m_Str = defVal.Mid(end + 1);
            m_Str.Trim(false);
        }
        else
        {
            m_Str = defVal.substr(token.length());
            m_Str.Trim(false);
            //defVal = _T("");
        }
        Token* oldParent = m_pLastParent;
        m_pLastParent = 0L;
        DoAddToken(tkPreprocessor, token, lineNr, lineNr, m_Tokenizer.GetLineNumber(), para, false, true);
        m_pLastParent = oldParent;
        m_Str.Clear();
    }
}

void ParserThread::HandlePreprocessorBlocks(const wxString& preproc)
{
    if (preproc.StartsWith(ParserConsts::kw_if)) // #if, #ifdef, #ifndef
    {
        wxString token = preproc;
        ++m_PreprocessorIfCount;

        token = m_Tokenizer.GetToken();
        if (token.IsSameAs(_T("0")))
        {
            // TODO: handle special case "#if 0"
            TRACE(_T("HandlePreprocessorBlocks() : Special case \"#if 0\" not skipped."));
        }
        m_Tokenizer.SkipToEOL();
    }
    else if (preproc==ParserConsts::kw_else || preproc==ParserConsts::kw_elif) // #else, #elif
    {
        TRACE(_T("HandlePreprocessorBlocks() : Saving nesting level: %d"), m_Tokenizer.GetNestingLevel());
        m_Tokenizer.SaveNestingLevel();
        wxString token = preproc;
        while (!token.IsEmpty() && token != ParserConsts::kw_endif)
            token = m_Tokenizer.GetToken();
        --m_PreprocessorIfCount;
#if PARSERTHREAD_DEBUG_OUTPUT
        int l = m_Tokenizer.GetNestingLevel();
#endif
        m_Tokenizer.RestoreNestingLevel();
        TRACE(_T("HandlePreprocessorBlocks() : Restoring nesting level: %d (was %d)"), m_Tokenizer.GetNestingLevel(), l);
    }
    else if (preproc==ParserConsts::kw_endif) // #endif
        --m_PreprocessorIfCount;
}

void ParserThread::HandleNamespace()
{
    wxString ns = m_Tokenizer.GetToken();
    int line = m_Tokenizer.GetLineNumber();

    if (ns == ParserConsts::opbrace)
    {
        // parse inside anonymous namespace
        Token*     lastParent = m_pLastParent;
        TokenScope lastScope  = m_LastScope;

        DoParse();

        m_pLastParent = lastParent;
        m_LastScope   = lastScope;
    }
    else
    {
        // for namespace aliases to be parsed, we need to tell the tokenizer
        // not to skip the usually unwanted tokens. One of those tokens is the
        // "assignment" (=).
        // we just have to remember to revert this setting below, or else problems will follow
        m_Tokenizer.SetSkipUnwantedTokens(false);

        wxString next = m_Tokenizer.PeekToken(); // named namespace
        if (next==ParserConsts::opbrace)
        {
            m_Tokenizer.SetSkipUnwantedTokens(true);

            // use the existing copy (if any)
            Token* newToken = TokenExists(ns, m_pLastParent, tkNamespace);
            if (!newToken)
                newToken = DoAddToken(tkNamespace, ns, line);
            if (!newToken)
                return;

            m_Tokenizer.GetToken(); // eat {
            int lineStart = m_Tokenizer.GetLineNumber();

            Token*     lastParent = m_pLastParent;
            TokenScope lastScope  = m_LastScope;

            m_pLastParent = newToken;
            // default scope is: public for namespaces (actually no, but emulate it)
            m_LastScope   = tsPublic;

            DoParse();

            m_pLastParent = lastParent;
            m_LastScope   = lastScope;

            // update implementation file and lines of namespace.
            // this doesn't make much sense because namespaces are all over the place,
            // but do it anyway so that buffer-based parsing returns the correct values.
            newToken->m_ImplFileIdx   = m_FileIdx;
            newToken->m_ImplLine      = line;
            newToken->m_ImplLineStart = lineStart;
            newToken->m_ImplLineEnd   = m_Tokenizer.GetLineNumber();
        }
        else if (next==ParserConsts::equals)
        {
            // namespace alias; example from cxxabi.h:
            //
            // namespace __cxxabiv1
            // {
            // ...
            // }
            // namespace abi = __cxxabiv1; <-- we 're in this case now

            m_Tokenizer.GetToken(); // eat '='
            wxString aliasns = m_Tokenizer.GetToken();

            m_Tokenizer.SetSkipUnwantedTokens(true);

            // use the existing copy (if any)
            Token* aliasToken = TokenExists(aliasns, m_pLastParent, tkNamespace);
            if (!aliasToken)
                aliasToken = DoAddToken(tkNamespace, aliasns, line);
            if (!aliasToken)
                return;

            aliasToken->m_Aliases.Add(ns);
        }
        else
        {
            m_Tokenizer.SetSkipUnwantedTokens(true);
            SkipToOneOfChars(ParserConsts::semicolonopbrace); // some kind of error in code ?
        }
    }
}

void ParserThread::HandleClass(EClassType ct, const wxString& templateArgs)
{
    // need to force the tokenizer _not_ skip anything
    // as we 're manually parsing class decls
    // don't forget to reset that if you add any early exit condition!
    bool oldState = m_Tokenizer.IsSkippingUnwantedTokens();
    m_Tokenizer.SetSkipUnwantedTokens(false);

    int lineNr = m_Tokenizer.GetLineNumber();
    wxString ancestors;
    while (1)
    {
        wxString current = m_Tokenizer.GetToken(); // class name
        wxString next = m_Tokenizer.PeekToken();

        TRACE(_T("HandleClass() : Found class '%s'"), current.wx_str());

        // handle preprocessor directives in class definition, e.g.
        //
        // class MyClass
        //         #ifdef FOO
        //             : public MyClass1
        //             , public MyClass2
        //         #endif
        // {}
        while (next==ParserConsts::hash)
        {
            m_Tokenizer.GetToken(); // make # current
            next = m_Tokenizer.GetToken();
            HandlePreprocessorBlocks(next);
            next = m_Tokenizer.PeekToken();
        }

        if (!current.IsEmpty() && !next.IsEmpty())
        {
            if (next==ParserConsts::lt) // template specialization
            {
                SkipAngleBraces();
                next = m_Tokenizer.PeekToken();
            }

            if (next==ParserConsts::colon) // has ancestor(s)
            {
                TRACE(_T("HandleClass() : Class '%s' has ancestors"), current.wx_str());
                m_Tokenizer.GetToken(); // eat ":"
                while (1)
                {
                    wxString tmp = m_Tokenizer.GetToken();
                    next = m_Tokenizer.PeekToken();
                    if (tmp==ParserConsts::kw_public ||
                        tmp==ParserConsts::kw_protected ||
                        tmp==ParserConsts::kw_private)
                    {
                        continue;
                    }

                    if (!(tmp==ParserConsts::comma || tmp==ParserConsts::gt))
                    {
                        // fix for namespace usage in ancestors
                        if (tmp==ParserConsts::dcolon || next==ParserConsts::dcolon)
                            ancestors << tmp;
                        else
                            ancestors << tmp << _T(',');
                        TRACE(_T("HandleClass() : Adding ancestor ") + tmp);
                    }
                    if (next.IsEmpty() ||
                        next==ParserConsts::opbrace ||
                        next==ParserConsts::semicolon)
                    {
                        break;
                    }
                    else if (next==ParserConsts::lt)
                    {
                        // template class
                        //m_Tokenizer.GetToken(); // reach "<"
                        // must not "eat" the token,
                        // SkipAngleBraces() will do it to see what it must match
                        SkipAngleBraces();
                        // also need to 'unget' the last token (>)
                        // so next iteration will see the { or ; in 'next'
                        m_Tokenizer.UngetToken();
                    }
                }
                TRACE(_T("HandleClass() : Ancestors: ") + ancestors);
            }

            if (current==ParserConsts::opbrace) // unnamed class/struct/union
            {
                static size_t num = 0;
                wxString unnamedTmp;
                unnamedTmp.Printf(_T("%s%s%d"),
                                  ParserConsts::unnamed.wx_str(),
                                  ct == ctClass ? _T("Class") :
                                  ct == ctUnion ? _T("Union") :
                                                  _T("Struct"), num++);

                Token* newToken = DoAddToken(tkClass, unnamedTmp, lineNr, 0, 0,templateArgs);

                Token* lastParent = m_pLastParent;
                TokenScope lastScope = m_LastScope;
                bool parsingTypedef = m_ParsingTypedef;

                m_pLastParent = newToken;
                // default scope is: private for classes, public for structs, public for unions
                m_LastScope = ct == ctClass ? tsPrivate : tsPublic;
                m_ParsingTypedef = false;

                DoParse();

                m_ParsingTypedef = parsingTypedef;
                m_pLastParent = lastParent;
                m_LastScope = lastScope;

                m_LastUnnamedTokenName = unnamedTmp; // used for typedef'ing anonymous class/struct/union

                // we should now be right after the closing brace
                // no vars are defined on a typedef, only types
                // In the former example, aa is not part of the typedef.
                if (m_ParsingTypedef)
                {
                    m_Str.Clear();
                    ReadClsNames(newToken->m_Name);
//                    m_ParsingTypedef = false;
                    break;
                }
                else
                {
                    m_Str = newToken->m_Name;
                    ReadVarNames();
                    m_Str.Clear();
                    break;
                }
            }
            else if (next==ParserConsts::opbrace)
            {
                Token* newToken = DoAddToken(tkClass, current, lineNr, 0, 0, templateArgs);
                if (!newToken)
                {
                    // restore tokenizer's functionality
                    m_Tokenizer.SetSkipUnwantedTokens(oldState);
                    return;
                }
                newToken->m_AncestorsString = ancestors;

                m_Tokenizer.GetToken(); // eat {

                Token* lastParent = m_pLastParent;
                TokenScope lastScope = m_LastScope;
                bool parsingTypedef = m_ParsingTypedef;

                m_pLastParent = newToken;
                // default scope is: private for classes, public for structs, public for unions
                m_LastScope = ct == ctClass ? tsPrivate : tsPublic;
                m_ParsingTypedef = false;

                DoParse();

                m_ParsingTypedef = parsingTypedef;
                m_pLastParent = lastParent;
                m_LastScope = lastScope;

                // we should now be right after the closing brace
                // no vars are defined on a typedef, only types
                // In the former example, aa is not part of the typedef.
                if (m_ParsingTypedef)
                {
                    m_Str.Clear();
                    ReadClsNames(newToken->m_Name);
//                    m_ParsingTypedef = false;
                    break;
                }
                else
                {
                    m_Str = newToken->m_Name;
                    ReadVarNames();
                    m_Str.Clear();
                    break;
                }
            }
            else if (next==ParserConsts::semicolon) // forward decl; we don't care
                break;
            else if (next.GetChar(0) == '(') // function: struct xyz& DoSomething()...
            {
                HandleFunction(current);
                break;
            }
            else if (next.GetChar(0) == '*' || next.GetChar(0) == '&' )
            {
                m_Str << current;
                break;
            }
            else
            {
                // might be instantiation, see the following
                /*
                struct HiddenStruct {
                    int val;
                };

                struct HiddenStruct yy;
                */
                if (m_ParsingTypedef)
                {
                     m_Tokenizer.UngetToken();
                     break;
                }
                if (TokenExists(current, m_pLastParent, tkClass))
                {
                    if (!TokenExists(next, m_pLastParent, tkVariable) )
                    {
                        wxString farnext;

                        m_Tokenizer.GetToken(); // go ahead of identifier
                        farnext = m_Tokenizer.PeekToken();

                        if (farnext==ParserConsts::semicolon)
                        {
                            if (m_Options.handleVars)
                            {
                                m_Str = current;
                                DoAddToken(tkVariable, next, m_Tokenizer.GetLineNumber(), 0, 0, templateArgs);
                                m_Str.Clear();
                            }

                            m_Tokenizer.GetToken(); // eat semi-colon
                            break;
                        }
                        else
                            m_Tokenizer.UngetToken(); // restore the identifier
                    }
                }
            }

        }
        else
            break;
    }

    // restore tokenizer's functionality
    m_Tokenizer.SetSkipUnwantedTokens(oldState);
}

void ParserThread::HandleFunction(const wxString& name, bool isOperator)
{
    TRACE(_T("HandleFunction() : Adding function '")+name+_T("': m_Str='")+m_Str+_T("'"));
    wxString args = m_Tokenizer.GetToken();
    wxString peek = m_Tokenizer.PeekToken();
    TRACE(_T("HandleFunction() : name='")+name+_T("', args='")+args+_T("', peek='")+peek+_T("'"));

    if (!m_Str.StartsWith(ParserConsts::kw_friend))
    {
        int lineNr = m_Tokenizer.GetLineNumber();
        int lineStart = 0;
        int lineEnd = 0;
        bool isCtor = m_Str.IsEmpty();
        bool isDtor = m_Str.StartsWith(ParserConsts::tilde);
        Token* localParent = 0;

        if ((isCtor || isDtor) && !m_EncounteredTypeNamespaces.empty())
        {
            // probably a ctor/dtor
            std::queue<wxString> q = m_EncounteredTypeNamespaces; // preserve m_EncounteredTypeNamespaces; needed in DoAddToken()
            localParent = FindTokenFromQueue(q);

            TRACE(_T("HandleFunction() : Ctor/Dtor '%s', m_Str='%s', localParent='%s'"),
                name.wx_str(),
                m_Str.wx_str(),
                localParent ? localParent->m_Name.wx_str() : _T("<none>"));
        }
        else
        {
            std::queue<wxString> q = m_EncounteredNamespaces; // preserve m_EncounteredNamespaces; needed in DoAddToken()
            localParent = FindTokenFromQueue(q);

            TRACE(_T("HandleFunction() : !(Ctor/Dtor) '%s', m_Str='%s', localParent='%s'"),
                name.wx_str(),
                m_Str.wx_str(),
                localParent ? localParent->m_Name.wx_str() : _T("<none>"));
        }

        bool isCtorOrDtor = m_pLastParent && name == m_pLastParent->m_Name;

        if (!isCtorOrDtor)
            isCtorOrDtor = localParent && name == localParent->m_Name;

        if (!isCtorOrDtor && m_Options.useBuffer)
            isCtorOrDtor = isCtor || isDtor;

        TRACE(_T("HandleFunction() : Adding function '%s', ': m_Str='%s', enc_ns='%s'."),
              name.wx_str(),
              m_Str.wx_str(),
              m_EncounteredNamespaces.size() ? m_EncounteredNamespaces.front().wx_str() : wxT("nil"));

        bool isImpl = false;
        bool isConst = false;
        while (!peek.IsEmpty()) // !eof
        {
            if (peek == ParserConsts::colon) // probably a ctor with member initializers
            {
                SkipToOneOfChars(ParserConsts::opbrace, false);
                m_Tokenizer.UngetToken(); // leave brace there
                peek = m_Tokenizer.PeekToken();
                continue;
            }
            else if (peek == ParserConsts::opbrace)// function implementation
            {
                isImpl = true;
                m_Tokenizer.GetToken(); // eat {
                lineStart = m_Tokenizer.GetLineNumber();
                SkipBlock(); // skip  to matching }
                lineEnd = m_Tokenizer.GetLineNumber();

                // Show message, if skipped buffer is more than 10% of whole buffer (might be a bug in the parser)
                if (!m_IsBuffer && ((lineEnd-lineStart) > (int)(m_FileSize/10)))
                    TRACE(_T("HandleFunction() : Skipped function '%s' impl. from %d to %d (file name='%s', file size=%d)."),
                          name.wx_str(),
                          lineStart,
                          lineEnd,
                          m_Filename.wx_str(),
                          m_FileSize);

                break;
            }
            else if (peek == ParserConsts::clbrace || peek == ParserConsts::semicolon)
            {
                break; // function decl
            }
            else if (peek == ParserConsts::kw_const)
            {
                isConst= true;
            }
            else
            {
                break; // darned macros that do not end with a semicolon :/
            }

            // if we reached here, eat the token so peek gets a new value
            m_Tokenizer.GetToken();
            peek = m_Tokenizer.PeekToken();
        }

        TRACE(_T("HandleFunction() : Add token name='")+name+_T("', args='")+args+_T("', return type='") + m_Str+ _T("'"));
        TokenKind tokenKind = !isCtorOrDtor ? tkFunction : (isDtor ? tkDestructor : tkConstructor);
        Token* newToken =  DoAddToken(tokenKind, name, lineNr, lineStart, lineEnd, args, isOperator, isImpl);
        if (newToken)
        {
            newToken->m_IsConst = isConst;
        }
    }
}

void ParserThread::HandleEnum()
{
    // enums have the following rough definition:
    // enum [xxx] { type1 name1 [= 1][, [type2 name2 [= 2]]] };
    bool isUnnamed = false;
    int lineNr = m_Tokenizer.GetLineNumber();
    wxString token = m_Tokenizer.GetToken();
    if (token.IsEmpty())
        return;
    else if (token==ParserConsts::opbrace)
    {
        // we have an un-named enum
        if (m_ParsingTypedef)
        {
            static size_t num = 0;
            token.Printf(_T("%sEnum%d"), ParserConsts::unnamed.wx_str(), num++);
            m_LastUnnamedTokenName = token;
        }
        else
            token = ParserConsts::unnamed;
        m_Tokenizer.UngetToken(); // return '{' back
        isUnnamed = true;
    }

    Token* newEnum = 0L;
    unsigned int level = 0;
    if (wxIsalpha(token.GetChar(0)) || token.GetChar(0) == '_')
    {
        if (m_Tokenizer.PeekToken().GetChar(0) != '{')
        {
            if (TokenExists(token, m_pLastParent, tkEnum))
            {
                if (!TokenExists(m_Tokenizer.PeekToken(), m_pLastParent, tkVariable) )
                {
                    wxString ident = m_Tokenizer.GetToken(); // go ahead of identifier

                    if (m_Tokenizer.PeekToken()==ParserConsts::semicolon)
                    {
                        if (m_Options.handleEnums)
                        {
                            m_Str = token;
                            DoAddToken(tkVariable, ident, m_Tokenizer.GetLineNumber());
                            m_Str.Clear();
                        }

                        m_Tokenizer.GetToken(); // eat semi-colon
                    }
                    else
                        m_Tokenizer.UngetToken(); // restore the identifier
                }
            }

            return;
        }

        if (isUnnamed && !m_ParsingTypedef)
        {
            // for unnamed enums, look if we already have "Unnamed", so we don't
            // add a new one for every unnamed enum we encounter, in this scope...
            newEnum = TokenExists(token, m_pLastParent, tkEnum);
        }

        if (!newEnum) // either named or first unnamed enum
            newEnum = DoAddToken(tkEnum, token, lineNr);
        level = m_Tokenizer.GetNestingLevel();
        m_Tokenizer.GetToken(); // skip {
    }
    else
    {
        if (token.GetChar(0) != '{')
            return;
        level = m_Tokenizer.GetNestingLevel() - 1; // we 've already entered the { block
    }

    while (1)
    {
        // process enumerators
        token = m_Tokenizer.GetToken();
        wxString peek = m_Tokenizer.PeekToken();
        if (token.IsEmpty() || peek.IsEmpty())
            return; //eof
        if (token==ParserConsts::clbrace && level == m_Tokenizer.GetNestingLevel())
            break;
        // assignments (=xxx) are ignored by the tokenizer,
        // so we don't have to worry about them here ;)
        if (peek==ParserConsts::comma || peek==ParserConsts::clbrace || peek==ParserConsts::colon)
        {
            // this "if", avoids non-valid enumerators
            // like a comma (if no enumerators follow)
            if (wxIsalpha(token.GetChar(0)) || token.GetChar(0) == '_')
            {
                Token* lastParent = m_pLastParent;
                m_pLastParent = newEnum;
                DoAddToken(tkEnumerator, token, m_Tokenizer.GetLineNumber());
                m_pLastParent = lastParent;
            }
            if (peek==ParserConsts::colon)
            {
                // bit specifier (eg, xxx:1)
                //walk to , or }
                SkipToOneOfChars(ParserConsts::commaclbrace);
            }
        }
    }
//    // skip to ;
//    SkipToOneOfChars(ParserConsts::semicolon);
}

void ParserThread::HandleTypedef()
{
    // typedefs are handled as tkClass and we put the typedef'd type as the class's
    // ancestor. this way, it will work through inheritance.
    // function pointers are a different beast and are handled differently.

    // this is going to be tough :(
    // let's see some examples:
    //
    // relatively easy:
    // typedef unsigned int uint32;
    // typedef std::map<String, StringVector> AnimableDictionaryMap;
    // typedef class|struct|enum {...} type;
    //
    // harder:
    // typedef void dMessageFunction (int errnum, const char *msg, va_list ap);
    //
    // even harder:
    // typedef void (*dMessageFunction)(int errnum, const char *msg, va_list ap);

    size_t lineNr = m_Tokenizer.GetLineNumber();
    bool is_function_pointer = false;
    wxString typ;
    std::queue<wxString> components;
    // get everything on the same line

    TRACE(_T("HandleTypedef() : Typedef start"));
    wxString args;
    wxString token;
    wxString peek;
    m_ParsingTypedef = true;

    while (true)
    {
        token = m_Tokenizer.GetToken();
        peek = m_Tokenizer.PeekToken();
        TRACE(_T("HandleTypedef() : token=%s, peek=%s"), token.wx_str(), peek.wx_str());
        if (token.IsEmpty() || token == ParserConsts::semicolon)
            break;

        if (   token == ParserConsts::kw_class
            || token == ParserConsts::kw_struct
            || token == ParserConsts::kw_union)
        {
            // "typedef struct|class|union"
            TRACE(_("HandleTypedef() : Before HandleClass m_LastUnnamedTokenName='%s'"), m_LastUnnamedTokenName.wx_str());
            HandleClass(token == ParserConsts::kw_class ? ctClass :
                        token == ParserConsts::kw_union ? ctUnion :
                                                          ctStructure);
            token = m_LastUnnamedTokenName;
            TRACE(_("HandleTypedef() : After HandleClass m_LastUnnamedTokenName='%s'"), m_LastUnnamedTokenName.wx_str());
        }
        else if (token == ParserConsts::kw_enum)
        {
            // "typedef enum"
            HandleEnum();
            token = m_LastUnnamedTokenName;
        }

        // keep namespaces together
        while (peek == ParserConsts::dcolon)
        {
            token << peek;
            m_Tokenizer.GetToken(); // eat it
            token << m_Tokenizer.GetToken(); // get what's next
            peek = m_Tokenizer.PeekToken();
        }

        if (token.GetChar(0) == '(')
        {
            // function pointer (probably)
            is_function_pointer = true;
            if (peek.GetChar(0) == '(')
            {
                // typedef void (*dMessageFunction)(int errnum, const char *msg, va_list ap);
                // typedef void (MyClass::*Function)(int);

                // remove parentheses and keep everything after the dereferencing symbol
                token.RemoveLast();
                int pos = token.Find('*', true);
                if (pos != wxNOT_FOUND)
                {
                    typ << _T('(') << token.Mid(1, pos) << _T(')');
                    token.Remove(0, pos + 1);
                }
                else
                {
                    typ = _T("(*)");
                    token.Remove(0, 1); // remove opening parenthesis
                }
                args = peek;
                components.push(token);
            }
            else
            {
                // typedef void dMessageFunction (int errnum, const char *msg, va_list ap);

                // last component is already the name and this is the args
                args = token;
            }
            break;
        }

        components.push(token);

        // skip templates <>
        if (peek == ParserConsts::lt)
        {
            SkipAngleBraces();
            continue;
        }

        TRACE(_T(" + '%s'"), token.wx_str());
    }
    TRACE(_T("HandleTypedef() : Typedef done"));
    m_ParsingTypedef = false;

    if (components.empty())
        return; // invalid typedef

    if (!is_function_pointer && components.size() <= 1)
            return; // invalid typedef

    // now get the type
    wxString ancestor;
    while (components.size() > 1)
    {
        wxString token = components.front();
        components.pop();

        if (!ancestor.IsEmpty())
            ancestor << _T(' ');
        ancestor << token;
    }

    // no return type
    m_Str.Clear();
    TRACE(_("HandleTypedef() : Adding typedef: name='%s', ancestor='%s'"), components.front().wx_str(), ancestor.wx_str());
//    Token* tdef = DoAddToken(tkClass, components.front(), lineNr, 0, 0, args);
    Token* tdef = DoAddToken(tkTypedef, components.front(), lineNr, 0, 0, args);
    if (tdef)
    {
        if (!is_function_pointer)
        {
            tdef->m_AncestorsString = ancestor;
            tdef->m_ActualType      = ancestor;
            tdef->m_Type            = ancestor;
        }
        else
        {
            tdef->m_ActualType      = ancestor;
            tdef->m_Type            = ancestor + typ; // + args;
            tdef->m_AncestorsString = ancestor;
        }

    }
}

void ParserThread::HandleMacro(const wxString &token, const wxString &peek)
{
    TRACE(_T("HandleMacro() : Adding token '%s' (peek='%s')"), token.wx_str(), peek.wx_str());
    DoAddToken(tkMacro, token, m_Tokenizer.GetLineNumber(), 0, 0, peek);
}

void ParserThread::ReadVarNames()
{
    while (1)
    {
        wxString current = m_Tokenizer.GetToken();

        if (current.IsEmpty())
            break;
        if (current==ParserConsts::comma)
            continue;
        else if (current==ParserConsts::semicolon)
            break;
        else if (wxIsalpha(current.GetChar(0)))
        {

            TRACE(_T("ReadVarNames() : Adding variable '%s' as '%s' to '%s'"),
                  current.wx_str(),
                  m_Str.wx_str(),
                  (m_pLastParent ? m_pLastParent->m_Name.wx_str():_T("<no-parent>")));
            DoAddToken(tkVariable, current, m_Tokenizer.GetLineNumber());
        }
        else // unexpected
            break;
    }
}

void ParserThread::ReadClsNames(wxString& ancestor)
{
    while (1)
    {
        wxString current = m_Tokenizer.GetToken();

        if (current.IsEmpty())
            break;
        if (current==ParserConsts::comma)
            continue;
        else if (current==ParserConsts::semicolon)
        {
            m_Tokenizer.UngetToken();
            break;
        }
        else if (   wxIsalpha(current.GetChar(0))
                 && (   (m_Tokenizer.PeekToken() == ParserConsts::semicolon)
                     || (m_Tokenizer.PeekToken() == ParserConsts::comma)) )
        {
            TRACE(_T("ReadClsNames() : Adding variable '%s' as '%s' to '%s'"),
                  current.wx_str(),
                  m_Str.wx_str(),
                  (m_pLastParent ? m_pLastParent->m_Name.wx_str():_T("<no-parent>")));

            Token* newToken = DoAddToken(tkTypedef, current, m_Tokenizer.GetLineNumber());
            if (!newToken)
                break;
            else
            {
                wxString tempAncestor       = ancestor;
                newToken->m_AncestorsString = tempAncestor;
                newToken->m_ActualType      = tempAncestor;
                newToken->m_Type            = tempAncestor;
            }
        }
        else // unexpected
        {
            m_Tokenizer.UngetToken();
            break;
        }
    }
}
