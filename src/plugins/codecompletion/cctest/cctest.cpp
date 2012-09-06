/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/app.h>
    #include <wx/filename.h>
#endif
#include <wx/thread.h>

#include "cctest.h"

#include "token.h"
#include "parserthread.h"
#include "searchtree.h"
#include "tokenizer.h"
#include "parser.h"

bool                  s_DebugSmartSense = true;
wxCriticalSection     g_ParserCritical;
std::auto_ptr<CCTest> CCTest::s_Inst;

void CCTest::Init()
{
    // Keep this in sync with CodeCompletion::LoadTokenReplacements()

    // for GCC
    Tokenizer::SetReplacementString(_T("_GLIBCXX_STD"),                     _T("std"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_STD_D"),                   _T("std"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_BEGIN_NESTED_NAMESPACE"),  _T("+namespace std {"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_END_NESTED_NAMESPACE"),    _T("}"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_BEGIN_NAMESPACE"),         _T("+namespace std {"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_END_NAMESPACE"),           _T("}"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_END_NAMESPACE_TR1"),       _T("}"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_BEGIN_NAMESPACE_TR1"),     _T("namespace tr1 {"));

    // for GCC 4.6.x
    Tokenizer::SetReplacementString(_T("_GLIBCXX_VISIBILITY"),              _T("+"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_BEGIN_NAMESPACE_VERSION"), _T(""));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_END_NAMESPACE_VERSION"),   _T(""));

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

TokenTree* ParserBase::GetTokenTree()
{
    return CCTest::Get()->GetTokenTree();
}

bool CCTest::Start(const wxString& file)
{
    if (!m_pClient)     m_pClient     = new ParserBase();
    if (!m_pTokenTree) m_pTokenTree = new TokenTree();

    FileLoader* loader = new FileLoader(file);
    (*loader)();

    ParserThreadOptions opts;

    opts.useBuffer             = false; // default
    opts.parentIdxOfBuffer     = -1;    // default
    opts.initLineOfBuffer      = -1;    // default
    opts.bufferSkipBlocks      = false; // default
    opts.bufferSkipOuterBlocks = false; // default
    opts.isTemp                = false; // default

    opts.followLocalIncludes   = true;  // default
    opts.followGlobalIncludes  = true;  // default
    opts.wantPreprocessor      = true;  // default
    opts.parseComplexMacros    = true;  // default

    opts.handleFunctions       = true;  // default
    opts.handleVars            = true;  // default
    opts.handleClasses         = true;  // default
    opts.handleEnums           = true;  // default
    opts.handleTypedefs        = true;  // default

    opts.loader                = loader;

    ParserThread* pt = new ParserThread(m_pClient, file, true, opts, m_pTokenTree);
    bool success = pt->Parse();
    delete pt;

    return success;
}

void CCTest::Clear()
{
    if (!m_pTokenTree) m_pTokenTree = new TokenTree();
    m_pTokenTree->clear();
}

void CCTest::PrintTree()
{
    if (!m_pTokenTree) return;

    TokenList& tokens = m_pTokenTree->m_Tokens;
    for (TokenList::iterator it=tokens.begin(); it!=tokens.end(); ++it)
    {
        Token* token = (*it);
        if (token)
        {
          Token* parent = m_pTokenTree->at(token->m_ParentIndex);
          if (!parent)
              PrintTokenTree(token);
        }
    }
}

void CCTest::PrintTokenTree(Token* token)
{
    if (!token || !m_pTokenTree) return;

    wxString log;
    if (!token->m_Children.empty()) log << _T("+");
    if (token->m_TokenKind == tkFunction)
        log << token->m_Name << token->m_Args << _T("\t");
    else
        log << token->DisplayName() << _T("\t");
    log << _T("[") << token->m_Line << _T(",") << token->m_ImplLine << _T("]");
    CCLogger::Get()->Log(log);

    TokenIdxSet& ids = token->m_Children;
    for (TokenIdxSet::iterator it=ids.begin(); it!=ids.end(); ++it)
    {
        Token* token = m_pTokenTree->at(*it);
        PrintTokenTree(token); // recursion
    }
}

void CCTest::PrintList()
{
    if (!m_pTokenTree) return;

    TokenList& tokens = m_pTokenTree->m_Tokens;
    for (TokenList::iterator it=tokens.begin(); it!=tokens.end(); ++it)
    {
        Token* token = (*it);
        if (token)
        {
            wxString log;
            log << token->GetTokenKindString() << _T(" ")
                << token->DisplayName()        << _T("\t[")
                << token->m_Line               << _T(",")
                << token->m_ImplLine           << _T("]");
            CCLogger::Get()->Log(log);
        }
    }
}

wxString CCTest::SerializeTree()
{
  if (!m_pTokenTree) return wxEmptyString;

  return m_pTokenTree->m_Tree.Serialize();
}
