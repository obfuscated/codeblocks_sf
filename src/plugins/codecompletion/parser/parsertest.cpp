/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/app.h>
#include <wx/filename.h>
#endif

#include "parsertest.h"

#include "token.h"
#include "parserthread.h"
#include "searchtree.h"
#include "tokenizer.h"
#include "parser.h"

extern void ParserTrace(const wxChar* format, ...);

wxCriticalSection g_ParserCritical;

ParserTest::ParserTest()
{
    m_tokensTree = new TokensTree();

    // for GCC
    Tokenizer::SetReplacementString(_T("_GLIBCXX_STD"),                     _T("std"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_STD_D"),                   _T("std"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_BEGIN_NESTED_NAMESPACE"),  _T("+namespace std {"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_END_NESTED_NAMESPACE"),    _T("}"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_BEGIN_NAMESPACE"),         _T("+namespace std {"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_END_NAMESPACE"),           _T("}"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_END_NAMESPACE_TR1"),       _T("}"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_BEGIN_NAMESPACE_TR1"),     _T("namespace tr1 {"));

    // for VC
    Tokenizer::SetReplacementString(_T("_STD_BEGIN"),                       _T("namespace std {"));
    Tokenizer::SetReplacementString(_T("_STD_END"),                         _T("}"));
    Tokenizer::SetReplacementString(_T("_STDEXT_BEGIN"),                    _T("namespace std {"));
    Tokenizer::SetReplacementString(_T("_STDEXT_END"),                      _T("}"));

    // for wxWidgets
    Tokenizer::SetReplacementString(_T("BEGIN_EVENT_TABLE"),                _T("-END_EVENT_TABLE"));
    Tokenizer::SetReplacementString(_T("WXDLLEXPORT"),                      _T(""));
    Tokenizer::SetReplacementString(_T("WXEXPORT"),                         _T(""));
    Tokenizer::SetReplacementString(_T("WXIMPORT"),                         _T(""));
}

ParserTest::~ParserTest()
{
    delete m_tokensTree;
}

bool ParserTest::Start(const wxString& file)
{
    Parser client(nullptr, nullptr);
    FileLoader* loader = new FileLoader(file);
    (*loader)();

    ParserThreadOptions opts;
    opts.wantPreprocessor = true;
    opts.useBuffer = false;
    opts.bufferSkipBlocks = false;
    opts.bufferSkipOuterBlocks = false;
    opts.followLocalIncludes = false;
    opts.followGlobalIncludes = false;
    opts.loader = loader;
    ParserThread* ph = new ParserThread(&client, file, true, opts, m_tokensTree);
    bool b = ph->Parse();
    delete ph;
    return b;
}

void ParserTest::Clear()
{
    m_tokensTree->clear();
}

void ParserTest::PrintTree()
{
    TokenList& tokens = m_tokensTree->m_Tokens;
    for (TokenList::iterator it = tokens.begin(); it != tokens.end(); it++)
    {
        if ((*it)->GetParentToken() == 0)
        {
            PrintTokenTree(*it);
        }
    }
}

void ParserTest::PrintTokenTree(Token* token)
{
    wxString log;
    if (!token->m_Children.empty()) log << _T("+");
    if (token->m_TokenKind == tkFunction)
        log << token->m_Name << token->m_Args << _T("\t");
    else
        log << token->DisplayName() << _T("\t");
    log << _T("[") << token->m_Line << _T(",") << token->m_ImplLine << _T("]");
    ParserTrace(log);

    TokenIdxSet& ids = token->m_Children;
    for (TokenIdxSet::iterator it = ids.begin(); it != ids.end(); it++)
    {
        Token* token = m_tokensTree->at(*it);
        PrintTokenTree(token);
    }
}

void ParserTest::PrintList()
{
    TokenList& tokens = m_tokensTree->m_Tokens;
    for (TokenList::iterator it = tokens.begin(); it != tokens.end(); it++)
    {
        wxString log;
        log << (*it)->GetTokenKindString() << _T(" ") << (*it)->DisplayName() << _T("\t[") << (*it)->m_Line;
        log << _T(",") << (*it)->m_ImplLine << _T("]");
        ParserTrace(log);
    }
}
